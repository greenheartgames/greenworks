//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the space war game server
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "SpaceWarServer.h"
#include "SpaceWarClient.h"
#include "stdlib.h"
#include "time.h"
#include <math.h>


//-----------------------------------------------------------------------------
// Purpose: Constructor -- note the syntax for setting up Steam API callback handlers
//-----------------------------------------------------------------------------
CSpaceWarServer::CSpaceWarServer( IGameEngine *pGameEngine ) 
{
	m_bConnectedToSteam = false;


	const char *pchGameDir = "spacewar";
	uint32 unIP = INADDR_ANY;
	uint16 usMasterServerUpdaterPort = SPACEWAR_MASTER_SERVER_UPDATER_PORT;

#ifdef USE_GS_AUTH_API
	EServerMode eMode = eServerModeAuthenticationAndSecure;
#else
	// Don't let Steam do authentication
	EServerMode eMode = eServerModeNoAuthentication;
#endif
	// Initialize the SteamGameServer interface, we tell it some info about us, and we request support
	// for both Authentication (making sure users own games) and secure mode, VAC running in our game
	// and kicking users who are VAC banned

	// !FIXME! We need a way to pass the dedicated server flag here!

	SteamErrMsg errMsg = { 0 };
	if ( SteamGameServer_InitEx( unIP, SPACEWAR_SERVER_PORT, usMasterServerUpdaterPort, eMode, SPACEWAR_SERVER_VERSION, &errMsg ) != k_ESteamAPIInitResult_OK )
	{
		OutputDebugString( "SteamGameServer_Init call failed: " );
		OutputDebugString( errMsg );
		OutputDebugString( "\n" );
	}

	if ( SteamGameServer() )
	{

		// Set the "game dir".
		// This is currently required for all games.  However, soon we will be
		// using the AppID for most purposes, and this string will only be needed
		// for mods.  it may not be changed after the server has logged on
		SteamGameServer()->SetModDir( pchGameDir );

		// These fields are currently required, but will go away soon.
		// See their documentation for more info
		SteamGameServer()->SetProduct( "SteamworksExample" );
		SteamGameServer()->SetGameDescription( "Steamworks Example" );

		// We don't support specators in our game.
		// .... but if we did:
		//SteamGameServer()->SetSpectatorPort( ... );
		//SteamGameServer()->SetSpectatorServerName( ... );

		// Initiate Anonymous logon.
		// Coming soon: Logging into authenticated, persistent game server account
		SteamGameServer()->LogOnAnonymous();

		// Initialize the peer to peer connection process.  This is not required, but we do it
		// because we cannot accept connections until this initialization completes, and so we
		// want to start it as soon as possible.
		SteamNetworkingUtils()->InitRelayNetworkAccess();

		// We want to actively update the master server with our presence so players can
		// find us via the steam matchmaking/server browser interfaces
		#ifdef USE_GS_AUTH_API
			SteamGameServer()->SetAdvertiseServerActive( true );
		#endif
	}
	else
	{
		OutputDebugString( "SteamGameServer() interface is invalid\n" );
	}

	m_uPlayerCount = 0;
	m_pGameEngine = pGameEngine;
	m_eGameState = k_EServerWaitingForPlayers;

	for( uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		m_rguPlayerScores[i] = 0;
		m_rgpShips[i] = NULL;
	}

	// No one has won
	m_uPlayerWhoWonGame = 0;
	m_ulStateTransitionTime = m_pGameEngine->GetGameTickCount();
	m_ulLastServerUpdateTick = 0;

	// zero the client connection data
	memset( &m_rgClientData, 0, sizeof( m_rgClientData ) );
	memset( &m_rgPendingClientData, 0, sizeof( m_rgPendingClientData ) );

	// Seed random num generator
	srand( (uint32)time( NULL ) );

	// Initialize sun
	m_pSun = new CSun( pGameEngine );

	// Initialize ships
	ResetPlayerShips();

	// create the listen socket for listening for players connecting
	m_hListenSocket = SteamGameServerNetworkingSockets()->CreateListenSocketP2P(0, 0, nullptr);

	// create the poll group
	m_hNetPollGroup = SteamGameServerNetworkingSockets()->CreatePollGroup();
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CSpaceWarServer::~CSpaceWarServer()
{
	delete m_pSun;

	for( uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( m_rgpShips[i] )
		{
			// Tell this client we are exiting
			MsgServerExiting_t msg;
			BSendDataToClient( i, (char*)&msg, sizeof(msg) );

			delete m_rgpShips[i];
			m_rgpShips[i] = NULL;
		}
	}

	SteamGameServerNetworkingSockets()->CloseListenSocket(m_hListenSocket);
	SteamGameServerNetworkingSockets()->DestroyPollGroup(m_hNetPollGroup);

	// Disconnect from the steam servers
	SteamGameServer()->LogOff();

	// release our reference to the steam client library
	SteamGameServer_Shutdown();
}

//-----------------------------------------------------------------------------
// Purpose: Handle any connection status change
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pCallback)
{
	/// Connection handle
	HSteamNetConnection hConn = pCallback->m_hConn;

	/// Full connection info
	SteamNetConnectionInfo_t info = pCallback->m_info;

	/// Previous state.  (Current state is in m_info.m_eState)
	ESteamNetworkingConnectionState eOldState = pCallback->m_eOldState;

	// Parse information to know what was changed

	// Check if a client has connected
	if (info.m_hListenSocket && 
		eOldState == k_ESteamNetworkingConnectionState_None && 
		info.m_eState == k_ESteamNetworkingConnectionState_Connecting)
	{
		// Connection from a new client
		// Search for an available slot
		for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
		{
			if (!m_rgClientData[i].m_bActive && !m_rgPendingClientData[i].m_hConn)
			{

				// Found one.  "Accept" the connection.
				EResult res = SteamGameServerNetworkingSockets()->AcceptConnection( hConn );
				if ( res != k_EResultOK )
				{
					char msg[ 256 ];
					sprintf( msg, "AcceptConnection returned %d", res );
					OutputDebugString( msg );
					SteamGameServerNetworkingSockets()->CloseConnection( hConn, k_ESteamNetConnectionEnd_AppException_Generic, "Failed to accept connection", false );
					return;
				}

				m_rgPendingClientData[i].m_hConn = hConn;

				// add the user to the poll group
				SteamGameServerNetworkingSockets()->SetConnectionPollGroup(hConn, m_hNetPollGroup);

				// Send them the server info as a reliable message
				MsgServerSendInfo_t msg;
				msg.SetSteamIDServer(SteamGameServer()->GetSteamID().ConvertToUint64());
				#ifdef USE_GS_AUTH_API
					// You can only make use of VAC when using the Steam authentication system
					msg.SetSecure(SteamGameServer()->BSecure());
				#endif
				msg.SetServerName(m_sServerName.c_str());
				SteamGameServerNetworkingSockets()->SendMessageToConnection( hConn, &msg, sizeof(MsgServerSendInfo_t), k_nSteamNetworkingSend_Reliable, nullptr );

				return;
			}
		}

		// No empty slots.  Server full!
		OutputDebugString("Rejecting connection; server full");
		SteamGameServerNetworkingSockets()->CloseConnection( hConn, k_ESteamNetConnectionEnd_AppException_Generic, "Server full!", false );
	}
	// Check if a client has disconnected
	else if ((eOldState == k_ESteamNetworkingConnectionState_Connecting || eOldState == k_ESteamNetworkingConnectionState_Connected) &&
			 info.m_eState == k_ESteamNetworkingConnectionState_ClosedByPeer)
	{
		// Handle disconnecting a client
		for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
		{
			// If there is no ship, skip
			if (!m_rgClientData[i].m_bActive)
				continue;

			if (m_rgClientData[i].m_SteamIDUser == info.m_identityRemote.GetSteamID())//pCallback->m_steamIDRemote)
			{
				OutputDebugString("Disconnected dropped user\n");
				RemovePlayerFromServer(i, k_EDRClientDisconnect);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handle sending data to a client at a given index
//-----------------------------------------------------------------------------
bool CSpaceWarServer::BSendDataToClient( uint32 uShipIndex, char *pData, uint32 nSizeOfData )
{
	// Validate index
	if ( uShipIndex >= MAX_PLAYERS_PER_SERVER )
		return false;

	int64 messageOut;
	if (!SteamGameServerNetworkingSockets()->SendMessageToConnection(m_rgClientData[uShipIndex].m_hConn, pData, nSizeOfData, k_nSteamNetworkingSend_Unreliable, &messageOut))
	{
		OutputDebugString("Failed sending data to a client\n");
			return false;
	}
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Handle sending data to a pending client at a given index
//-----------------------------------------------------------------------------
bool CSpaceWarServer::BSendDataToPendingClient( uint32 uShipIndex, char *pData, uint32 nSizeOfData )
{
	// Validate index
	if ( uShipIndex >= MAX_PLAYERS_PER_SERVER )
		return false;

	int64 messageOut;
	if (!SteamGameServerNetworkingSockets()->SendMessageToConnection(m_rgPendingClientData[uShipIndex].m_hConn, pData, nSizeOfData, k_nSteamNetworkingSend_Unreliable, &messageOut))
	{
		OutputDebugString("Failed sending data to a client\n");
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Handle a new client connecting
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnClientBeginAuthentication(CSteamID steamIDClient, HSteamNetConnection connectionID, void* pToken, uint32 uTokenLen)
{
	// First, check this isn't a duplicate and we already have a user logged on from the same steamid
	for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		if (m_rgClientData[i].m_hConn == connectionID)
		{
			// We already logged them on... (should maybe tell them again incase they don't know?)
			return;
		}
	}

	// Second, do we have room?
	uint32 nPendingOrActivePlayerCount = 0;
	for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		if (m_rgPendingClientData[i].m_bActive)
			++nPendingOrActivePlayerCount;

		if (m_rgClientData[i].m_bActive)
			++nPendingOrActivePlayerCount;
	}

	// We are full (or will be if the pending players auth), deny new login
	if ( nPendingOrActivePlayerCount >= MAX_PLAYERS_PER_SERVER )
	{
		SteamGameServerNetworkingSockets()->CloseConnection(connectionID, k_EDRServerFull, "Server full", false);
	}

	// If we get here there is room, add the player as pending
	for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		if (!m_rgPendingClientData[i].m_bActive)
		{
			m_rgPendingClientData[i].m_ulTickCountLastData = m_pGameEngine->GetGameTickCount();
#ifdef USE_GS_AUTH_API
			// authenticate the user with the Steam back-end servers
			EBeginAuthSessionResult res = SteamGameServer()->BeginAuthSession(pToken, uTokenLen, steamIDClient);
			if (res != k_EBeginAuthSessionResultOK)
			{
				SteamGameServerNetworkingSockets()->CloseConnection(connectionID, k_EDRServerReject, "BeginAuthSession failed", false);
				break;
			}

			m_rgPendingClientData[i].m_SteamIDUser = steamIDClient;
			m_rgPendingClientData[i].m_bActive = true;
			m_rgPendingClientData[i].m_hConn = connectionID;
			break;
#else
			m_rgPendingClientData[i].m_bActive = true;
			// we need to tell the server our Steam id in the non-auth case, so we stashed it in the login message, pull it back out
			m_rgPendingClientData[i].m_SteamIDUser = *(CSteamID*)pToken;
			m_rgPendingClientData[i].m_connection = connectionID;
			// You would typically do your own authentication method here and later call OnAuthCompleted
			// In this sample we just automatically auth anyone who connects
			OnAuthCompleted(true, i);
			break;
#endif
		}
	}
}
//-----------------------------------------------------------------------------
// Purpose: A new client that connected has had their authentication processed
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnAuthCompleted( bool bAuthSuccessful, uint32 iPendingAuthIndex )
{
	if ( !m_rgPendingClientData[iPendingAuthIndex].m_bActive )
	{
		OutputDebugString( "Got auth completed callback for client who is not pending\n" );
		return;
	}

	if ( !bAuthSuccessful )
	{
#ifdef USE_GS_AUTH_API
		// Tell the GS the user is leaving the server
		SteamGameServer()->EndAuthSession( m_rgPendingClientData[iPendingAuthIndex].m_SteamIDUser );
#endif
		// Send a deny for the client, and zero out the pending data
		MsgServerFailAuthentication_t msg;
		int64 outMessage;
		SteamGameServerNetworkingSockets()->SendMessageToConnection(m_rgPendingClientData[iPendingAuthIndex].m_hConn, &msg, sizeof(msg), k_nSteamNetworkingSend_Reliable, &outMessage);
		m_rgPendingClientData[iPendingAuthIndex] = ClientConnectionData_t();
		return;
	}

	bool bAddedOk = false;
	for( uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i ) 
	{
		if ( !m_rgClientData[i].m_bActive )
		{
			// copy over the data from the pending array
			memcpy( &m_rgClientData[i], &m_rgPendingClientData[iPendingAuthIndex], sizeof( ClientConnectionData_t ) );
			m_rgPendingClientData[iPendingAuthIndex] = ClientConnectionData_t();
			m_rgClientData[i].m_ulTickCountLastData = m_pGameEngine->GetGameTickCount();

			// Add a new ship, make it dead immediately
			AddPlayerShip( i );
			m_rgpShips[i]->SetDisabled( true );

			MsgServerPassAuthentication_t msg;
			msg.SetPlayerPosition( i );
			BSendDataToClient( i, (char*)&msg, sizeof( msg ) );

			bAddedOk = true;

			break;
		}
	}

	// If we just successfully added the player, check if they are #2 so we can restart the round
	if ( bAddedOk )
	{
		uint32 uPlayers = 0;
		for( uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i ) 
		{
			if ( m_rgClientData[i].m_bActive )
				++uPlayers;
		}

		// If we just got the second player, immediately reset round as a draw.  This will prevent
		// the existing player getting a win, and it will cause a new round to start right off
		// so that the one player can't just float around not letting the new one get into the game.
		if ( uPlayers == 2 )
		{
			if ( m_eGameState != k_EServerWaitingForPlayers )
				SetGameState( k_EServerDraw );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Used to reset scores (at start of a new game usually)
//-----------------------------------------------------------------------------
void CSpaceWarServer::ResetScores()
{
	for( uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		m_rguPlayerScores[i] = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Add a new player ship at given position
//-----------------------------------------------------------------------------
void CSpaceWarServer::AddPlayerShip( uint32 uShipPosition )
{
	if ( uShipPosition >= MAX_PLAYERS_PER_SERVER )
	{
		OutputDebugString( "Trying to add ship at invalid positon\n" );
		return;
	}

	if ( m_rgpShips[uShipPosition] )
	{
		OutputDebugString( "Trying to add a ship where one already exists\n" );
		return;
	}

	float flHeight = (float)m_pGameEngine->GetViewportHeight();
	float flWidth = (float)m_pGameEngine->GetViewportWidth();
	float flXOffset = flWidth*0.12f;
	float flYOffset = flHeight*0.12f;

	float flAngle = (float)atan( flHeight/flWidth ) + PI_VALUE/2.0f;
	switch( uShipPosition )
	{
	case 0:
		m_rgpShips[uShipPosition] = new CShip( m_pGameEngine, true, flXOffset, flYOffset, g_rgPlayerColors[uShipPosition] );
		m_rgpShips[uShipPosition]->SetInitialRotation( flAngle );
		break;
	case 1:
		m_rgpShips[uShipPosition] = new CShip( m_pGameEngine, true, flWidth-flXOffset, flYOffset, g_rgPlayerColors[uShipPosition] );
		m_rgpShips[uShipPosition]->SetInitialRotation( -1.0f*flAngle );
		break;
	case 2:
		m_rgpShips[uShipPosition] = new CShip( m_pGameEngine, true, flXOffset, flHeight-flYOffset, g_rgPlayerColors[uShipPosition] );
		m_rgpShips[uShipPosition]->SetInitialRotation( PI_VALUE-flAngle );
		break;
	case 3:
		m_rgpShips[uShipPosition] = new CShip( m_pGameEngine, true, flWidth-flXOffset, flHeight-flYOffset, g_rgPlayerColors[uShipPosition] );
		m_rgpShips[uShipPosition]->SetInitialRotation( -1.0f*(PI_VALUE-flAngle) );
		break;
	default:
		OutputDebugString( "AddPlayerShip() code needs updating for more than 4 players\n" );
	}

	if ( m_rgpShips[uShipPosition] )
	{
		// Setup key bindings... don't even really need these on server?
		m_rgpShips[uShipPosition]->SetVKBindingLeft( 0x41 ); // A key
		m_rgpShips[uShipPosition]->SetVKBindingRight( 0x44 ); // D key
		m_rgpShips[uShipPosition]->SetVKBindingForwardThrusters( 0x57 ); // W key
		m_rgpShips[uShipPosition]->SetVKBindingReverseThrusters( 0x53 ); // S key
		m_rgpShips[uShipPosition]->SetVKBindingFire( VK_SPACE ); 
	}
}


//-----------------------------------------------------------------------------
// Purpose: Removes a player at the given position
//-----------------------------------------------------------------------------
void CSpaceWarServer::RemovePlayerFromServer( uint32 uShipPosition, EDisconnectReason reason)
{
	if ( uShipPosition >= MAX_PLAYERS_PER_SERVER )
	{
		OutputDebugString( "Trying to remove ship at invalid position\n" );
		return;
	}

	if ( !m_rgpShips[uShipPosition] )
	{
		OutputDebugString( "Trying to remove a ship that does not exist\n" );
		return;
	}

	OutputDebugString( "Removing a ship\n" );
	delete m_rgpShips[uShipPosition];
	m_rgpShips[uShipPosition] = NULL;
	m_rguPlayerScores[uShipPosition] = 0;

	// close the hNet connection
	SteamGameServerNetworkingSockets()->CloseConnection( m_rgClientData[uShipPosition].m_hConn, reason, nullptr, false);

#ifdef USE_GS_AUTH_API
	// Tell the GS the user is leaving the server
	SteamGameServer()->EndAuthSession( m_rgClientData[uShipPosition].m_SteamIDUser );
#endif
	m_rgClientData[uShipPosition] = ClientConnectionData_t();
}


//-----------------------------------------------------------------------------
// Purpose: Used to reset player ship positions for a new round
//-----------------------------------------------------------------------------
void CSpaceWarServer::ResetPlayerShips()
{
	// Delete any currently active ships, but immediately recreate 
	// (which causes all ship state/position to reset)
	for( uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( m_rgpShips[i] )
		{		
			delete m_rgpShips[i];
			m_rgpShips[i] = NULL;
			AddPlayerShip( i );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Used to transition game state
//-----------------------------------------------------------------------------
void CSpaceWarServer::SetGameState( EServerGameState eState )
{
	if ( m_eGameState == eState )
		return;

	// If we were in waiting for players and are now going active clear old scores
	if ( m_eGameState == k_EServerWaitingForPlayers && eState == k_EServerActive )
	{
		ResetScores();
		ResetPlayerShips();
	}

	m_ulStateTransitionTime = m_pGameEngine->GetGameTickCount();
	m_eGameState = eState;
}


//-----------------------------------------------------------------------------
// Purpose: Receives incoming network data
//-----------------------------------------------------------------------------
void CSpaceWarServer::ReceiveNetworkData()
{
	SteamNetworkingMessage_t* msgs[128];
	int numMessages = SteamGameServerNetworkingSockets()->ReceiveMessagesOnPollGroup(m_hNetPollGroup, msgs, 128);
	for (int idxMsg = 0; idxMsg < numMessages; idxMsg++)
	{
		SteamNetworkingMessage_t* message = msgs[idxMsg];
		CSteamID steamIDRemote = message->m_identityPeer.GetSteamID();
		HSteamNetConnection connection = message->m_conn;

		if (message->GetSize() < sizeof(DWORD))
		{
			OutputDebugString("Got garbage on server socket, too short\n");
			message->Release();
			message = nullptr;
			continue;
		}

		EMessage eMsg = (EMessage)LittleDWord(*(DWORD*)message->GetData());

		switch (eMsg)
		{
		case k_EMsgClientBeginAuthentication:
		{
			if (message->GetSize() != sizeof(MsgClientBeginAuthentication_t))
			{
				OutputDebugString("Bad connection attempt msg\n");
				message->Release();
				message = nullptr;
				continue;
			}
			MsgClientBeginAuthentication_t* pMsg = (MsgClientBeginAuthentication_t*)message->GetData();
#ifdef USE_GS_AUTH_API
			OnClientBeginAuthentication(steamIDRemote, connection, (void*)pMsg->GetTokenPtr(), pMsg->GetTokenLen());
#else
			OnClientBeginAuthentication(connection, 0);
#endif
		}
		break;
		case k_EMsgClientSendLocalUpdate:
		{
			if (message->GetSize() != sizeof(MsgClientSendLocalUpdate_t))
			{
				OutputDebugString("Bad client update msg\n");
				message->Release();
				message = nullptr;
				continue;
			}

			// Find the connection that should exist for this users address
			bool bFound = false;
			for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
			{
				if (m_rgClientData[i].m_hConn == connection)
				{
					bFound = true;
					MsgClientSendLocalUpdate_t* pMsg = (MsgClientSendLocalUpdate_t*)message->GetData();
					OnReceiveClientUpdateData(i, pMsg->AccessUpdateData());
					break;
				}
			}
			if (!bFound)
				OutputDebugString("Got a client data update, but couldn't find a matching client\n");
		}
		break;

		case k_EMsgVoiceChatData:
		{
			// Received voice chat messages, broadcast to all other players
			MsgVoiceChatData_t *pMsg = (MsgVoiceChatData_t *)message->GetData();
			pMsg->SetSteamID( message->m_identityPeer.GetSteamID() ); // Make sure sender steam ID is set.
			SendMessageToAll( connection, pMsg, message->GetSize() );
			break;
		}
		case k_EMsgP2PSendingTicket:
		{
			// Received a P2P auth ticket, forward it to the intended recipient
			MsgP2PSendingTicket_t msgP2PSendingTicket;
			memcpy(&msgP2PSendingTicket, message->GetData(), sizeof(MsgP2PSendingTicket_t));
			CSteamID toSteamID = msgP2PSendingTicket.GetSteamID();

			HSteamNetConnection toHConn = 0;
			for (int j = 0; j < MAX_PLAYERS_PER_SERVER; j++)
			{
				if ( toSteamID == m_rgClientData[j].m_SteamIDUser )
				{

					// Mutate the message, replacing the destination SteamID with the sender's SteamID
					msgP2PSendingTicket.SetSteamID( message->m_identityPeer.GetSteamID64() );

					SteamNetworkingSockets()->SendMessageToConnection( m_rgClientData[j].m_hConn, &msgP2PSendingTicket, sizeof(msgP2PSendingTicket), k_nSteamNetworkingSend_Reliable, nullptr );
					break;
				}
			}

			if (toHConn == 0)
			{
				OutputDebugString("msgP2PSendingTicket received with no valid target to send to.");
			}
		}
		break;

		default:
			char rgch[128];
			sprintf_safe(rgch, "Invalid message %x\n", eMsg);
			rgch[sizeof(rgch) - 1] = 0;
			OutputDebugString(rgch);
		}

		message->Release();
		message = nullptr;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Main frame function, updates the state of the world and performs rendering
//-----------------------------------------------------------------------------
void CSpaceWarServer::RunFrame()
{
	// Run any Steam Game Server API callbacks
	SteamGameServer_RunCallbacks();

	// Update our server details
	SendUpdatedServerDetailsToSteam();

	// Timeout stale player connections, also update player count data
	uint32 uPlayerCount = 0;
	for( uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		// If there is no ship, skip
		if ( !m_rgClientData[i].m_bActive )
			continue;

		if ( m_pGameEngine->GetGameTickCount() - m_rgClientData[i].m_ulTickCountLastData > SERVER_TIMEOUT_MILLISECONDS )
		{
			OutputDebugString( "Timing out player connection\n" );
			RemovePlayerFromServer( i, k_EDRClientKicked );
		}
		else
		{
			++uPlayerCount;
		}
	}
	m_uPlayerCount = uPlayerCount;

	switch ( m_eGameState )
	{
	case k_EServerWaitingForPlayers:
		// Wait a few seconds (so everyone can join if a lobby just started this server)
		if ( m_pGameEngine->GetGameTickCount() - m_ulStateTransitionTime >= MILLISECONDS_BETWEEN_ROUNDS )
		{
			// Just keep waiting until at least one ship is active
			for( uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i )
			{
				if ( m_rgClientData[i].m_bActive )
				{
					// Transition to active
					OutputDebugString( "Server going active after waiting for players\n" );
					SetGameState( k_EServerActive );
				}
			}
		}
		break;
	case k_EServerDraw:
	case k_EServerWinner:
		// Update all the entities...
		m_pSun->RunFrame();
		for( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
		{
			if ( m_rgpShips[i] )
				m_rgpShips[i]->RunFrame();
		}

		// NOTE: no collision detection, because the round is really over, objects are now invulnerable

		// After 5 seconds start the next round
		if ( m_pGameEngine->GetGameTickCount() - m_ulStateTransitionTime >= MILLISECONDS_BETWEEN_ROUNDS )
		{
			ResetPlayerShips();
			SetGameState( k_EServerActive );
		}

		break;

	case k_EServerActive:
		// Update all the entities...
		m_pSun->RunFrame();
		for( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
		{
			if ( m_rgpShips[i] )
				m_rgpShips[i]->RunFrame();
		}

		// Check for collisions which could lead to a winner this round
		CheckForCollisions();

		break;
	case k_EServerExiting:
		break;
	default:
		OutputDebugString( "Unhandled game state in CSpaceWarServer::RunFrame\n" );
	}

	// Send client updates (will internal limit itself to the tick rate desired)
	SendUpdateDataToAllClients();
}


//-----------------------------------------------------------------------------
// Purpose: Sends updates to all connected clients
//-----------------------------------------------------------------------------
void CSpaceWarServer::SendUpdateDataToAllClients()
{
	// Limit the rate at which we update, even if our internal frame rate is higher
	if ( m_pGameEngine->GetGameTickCount() - m_ulLastServerUpdateTick < 1000.0f/SERVER_UPDATE_SEND_RATE )
		return;

	m_ulLastServerUpdateTick = m_pGameEngine->GetGameTickCount();

	MsgServerUpdateWorld_t msg;

	msg.AccessUpdateData()->SetServerGameState( m_eGameState );
	for( int i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
	{
		msg.AccessUpdateData()->SetPlayerActive( i, m_rgClientData[i].m_bActive );
		msg.AccessUpdateData()->SetPlayerScore( i, m_rguPlayerScores[i]  );
		msg.AccessUpdateData()->SetPlayerSteamID( i, m_rgClientData[i].m_SteamIDUser.ConvertToUint64() );

		if ( m_rgpShips[i] )
		{
			m_rgpShips[i]->BuildServerUpdate( msg.AccessUpdateData()->AccessShipUpdateData( i ) );
		}
	}

	msg.AccessUpdateData()->SetPlayerWhoWon( m_uPlayerWhoWonGame );
	
	for( int i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( !m_rgClientData[i].m_bActive ) 
			continue;

		BSendDataToClient( i, (char*)&msg, sizeof( msg ) );
	}
}



void CSpaceWarServer::SendMessageToAll( HSteamNetConnection hConnIgnore, const void* pubData, uint32 cubData)
{
	for (int i = 0; i < MAX_PLAYERS_PER_SERVER; i++)
	{
		if ( m_rgClientData[i].m_hConn != k_HSteamNetConnection_Invalid && m_rgClientData[i].m_hConn != hConnIgnore )
		{
			SteamNetworkingSockets()->SendMessageToConnection(m_rgClientData[i].m_hConn, pubData, cubData, k_nSteamNetworkingSend_UnreliableNoDelay, nullptr );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Receives update data from clients
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnReceiveClientUpdateData( uint32 uShipIndex, ClientSpaceWarUpdateData_t *pUpdateData )
{
	if ( m_rgClientData[uShipIndex].m_bActive && m_rgpShips[uShipIndex] )
	{
		m_rgClientData[uShipIndex].m_ulTickCountLastData = m_pGameEngine->GetGameTickCount();
		m_rgpShips[uShipIndex]->OnReceiveClientUpdate( pUpdateData );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Checks various game objects for collisions and updates state 
//			appropriately if they have occurred
//-----------------------------------------------------------------------------
void CSpaceWarServer::CheckForCollisions()
{
	// Make the ships check their photons for ones that have hit the sun and remove
	// them before we go and check for them hitting the opponent
	for ( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( m_rgpShips[i] )
			m_rgpShips[i]->DestroyPhotonsColldingWith( m_pSun );
	}

	// Array to track who exploded, can't set the ship exploding within the loop below,
	// or it will prevent that ship from colliding with later ships in the sequence
	bool rgbExplodingShips[MAX_PLAYERS_PER_SERVER];
	memset( rgbExplodingShips, 0, sizeof( rgbExplodingShips ) );

	// Check each ship for colliding with the sun or another ships photons
	for ( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
	{
		// If the pointer is invalid skip the ship
		if ( !m_rgpShips[i] )
			continue;

		if ( m_rgpShips[i]->BCollidesWith( m_pSun ) )
		{
			{
				MsgServerPlayerHitSun_t msg;
				msg.SetSteamID( m_rgClientData[ i ].m_SteamIDUser );
				BSendDataToClient( i, ( char * )&msg, sizeof( msg ) );
			}

			rgbExplodingShips[i] |= 1;
		}

		for( uint32 j=0; j<MAX_PLAYERS_PER_SERVER; ++j )
		{
			// Don't check against your own photons, or NULL pointers!
			if ( j == i || !m_rgpShips[j] )
				continue;
			
			rgbExplodingShips[i] |= m_rgpShips[i]->BCollidesWith( m_rgpShips[j] );
			if ( m_rgpShips[j]->BCheckForPhotonsCollidingWith( m_rgpShips[i] ) )
			{
				if ( m_rgpShips[i]->GetShieldStrength() > 200 )
				{
					// Shield protects from the hit
					m_rgpShips[i]->SetShieldStrength( 0 );
					m_rgpShips[j]->DestroyPhotonsColldingWith( m_rgpShips[i] );
				}
				else
				{
					rgbExplodingShips[i] |= 1;
				}
			}
		}
	}

	for ( uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( rgbExplodingShips[i] && m_rgpShips[i] )
			m_rgpShips[i]->SetExploding( true );
	}

	// Count how many ships are active, and how many are exploding
	uint32 uActiveShips = 0;
	uint32 uShipsExploding = 0;
	uint32 uLastShipFoundAlive = 0;
	for ( uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( m_rgpShips[i] )
		{
			// Disabled ships don't count at all
			if ( m_rgpShips[i]->BIsDisabled() )
				continue;

			++uActiveShips;
		
			if ( m_rgpShips[i]->BIsExploding() )
				++uShipsExploding;
			else
				uLastShipFoundAlive = i;
		}
	}

	// If exploding == active, then its a draw, everyone is dead
	if ( uActiveShips == uShipsExploding )
	{
		SetGameState( k_EServerDraw );
	}
	else if ( uActiveShips > 1 && uActiveShips - uShipsExploding == 1 )
	{
		// If only one ship is alive they win
		m_uPlayerWhoWonGame = uLastShipFoundAlive;
		m_rguPlayerScores[uLastShipFoundAlive]++;
		SetGameState( k_EServerWinner );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Take any action we need to on Steam notifying us we are now logged in
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnSteamServersConnected( SteamServersConnected_t *pLogonSuccess )
{
	OutputDebugString( "SpaceWarServer connected to Steam successfully\n" );
	m_bConnectedToSteam = true;

	// log on is not finished until OnPolicyResponse() is called

	// Tell Steam about our server details
	SendUpdatedServerDetailsToSteam();
}


//-----------------------------------------------------------------------------
// Purpose: Callback from Steam when logon is fully completed and VAC secure policy is set
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnPolicyResponse( GSPolicyResponse_t *pPolicyResponse )
{
#ifdef USE_GS_AUTH_API
	// Check if we were able to go VAC secure or not
	if ( SteamGameServer()->BSecure() )
	{
		OutputDebugString( "SpaceWarServer is VAC Secure!\n" );
	}
	else
	{
		OutputDebugString( "SpaceWarServer is not VAC Secure!\n" );
	}
	char rgch[128];
	sprintf_safe( rgch, "Game server SteamID: %llu\n", SteamGameServer()->GetSteamID().ConvertToUint64() );
	rgch[ sizeof(rgch) - 1 ] = 0;
	OutputDebugString( rgch );
#endif
}


//-----------------------------------------------------------------------------
// Purpose: Called when we were previously logged into steam but get logged out
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnSteamServersDisconnected( SteamServersDisconnected_t *pLoggedOff )
{
	m_bConnectedToSteam = false;
	OutputDebugString( "SpaceWarServer got logged out of Steam\n" );
}


//-----------------------------------------------------------------------------
// Purpose: Called when an attempt to login to Steam fails
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnSteamServersConnectFailure( SteamServerConnectFailure_t *pConnectFailure )
{
	m_bConnectedToSteam = false;
	OutputDebugString( "SpaceWarServer failed to connect to Steam\n" );
}


//-----------------------------------------------------------------------------
// Purpose: Called once we are connected to Steam to tell it about our details
//-----------------------------------------------------------------------------
void CSpaceWarServer::SendUpdatedServerDetailsToSteam()
{

	// Tell the Steam authentication servers about our game
	char rgchServerName[128];
	if ( SpaceWarClient() )
	{
		// If a client is running (should always be since we don't support a dedicated server)
		// then we'll form the name based off of it
		sprintf_safe( rgchServerName, "%s's game", SpaceWarClient()->GetLocalPlayerName() );
	}
	else
	{
		sprintf_safe( rgchServerName, "%s", "Spacewar!" );
	}
	m_sServerName = rgchServerName;

	//
	// Set state variables, relevant to any master server updates or client pings
	//

	// These server state variables may be changed at any time.  Note that there is no lnoger a mechanism
	// to send the player count.  The player count is maintained by steam and you should use the player
	// creation/authentication functions to maintain your player count.
	SteamGameServer()->SetMaxPlayerCount( 4 );
	SteamGameServer()->SetPasswordProtected( false );
	SteamGameServer()->SetServerName( m_sServerName.c_str() );
	SteamGameServer()->SetBotPlayerCount( 0 ); // optional, defaults to zero
	SteamGameServer()->SetMapName( "MilkyWay" );

#ifdef USE_GS_AUTH_API

	// Update all the players names/scores
	for( uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		if ( m_rgClientData[i].m_bActive && m_rgpShips[i] )
		{
			SteamGameServer()->BUpdateUserData( m_rgClientData[i].m_SteamIDUser, m_rgpShips[i]->GetPlayerName(), m_rguPlayerScores[i] );
		}
	}
#endif

	// game type is a special string you can use for your game to differentiate different game play types occurring on the same maps
	// When users search for this parameter they do a sub-string search of this string 
	// (i.e if you report "abc" and a client requests "ab" they return your server)
	//SteamGameServer()->SetGameType( "dm" );

	// update any rule values we publish
	//SteamMasterServerUpdater()->SetKeyValue( "rule1_setting", "value" );
	//SteamMasterServerUpdater()->SetKeyValue( "rule2_setting", "value2" );
}


//-----------------------------------------------------------------------------
// Purpose: Tells us Steam3 (VAC and newer license checking) has accepted the user connection
//-----------------------------------------------------------------------------
void CSpaceWarServer::OnValidateAuthTicketResponse( ValidateAuthTicketResponse_t *pResponse )
{
	if ( pResponse->m_eAuthSessionResponse == k_EAuthSessionResponseOK )
	{
		// This is the final approval, and means we should let the client play (find the pending auth by steamid)
		for ( uint32 i = 0; i<MAX_PLAYERS_PER_SERVER; ++i )
		{
			if ( !m_rgPendingClientData[i].m_bActive )
				continue;
			else if ( m_rgPendingClientData[i].m_SteamIDUser == pResponse->m_SteamID )
			{
				OutputDebugString( "Auth completed for a client\n" );
				OnAuthCompleted( true, i );
				return;
			}
		}
	}
	else
	{
		// Looks like we shouldn't let this user play, kick them
		for ( uint32 i = 0; i<MAX_PLAYERS_PER_SERVER; ++i )
		{
			if ( !m_rgPendingClientData[i].m_bActive )
				continue;
			else if ( m_rgPendingClientData[i].m_SteamIDUser == pResponse->m_SteamID )
			{
				OutputDebugString( "Auth failed for a client\n" );
				OnAuthCompleted( false, i );
				return;
			}
		}
	}
}



//-----------------------------------------------------------------------------
// Purpose: Returns the SteamID of the game server
//-----------------------------------------------------------------------------
CSteamID CSpaceWarServer::GetSteamID()
{
#ifdef USE_GS_AUTH_API
	return SteamGameServer()->GetSteamID();
#else
	// this is a placeholder steam id to use when not making use of Steam auth or matchmaking
	return k_steamIDNonSteamGS;
#endif
}


//-----------------------------------------------------------------------------
// Purpose: Kicks a player off the server
//-----------------------------------------------------------------------------
void CSpaceWarServer::KickPlayerOffServer( CSteamID steamID )
{
	uint32 uPlayerCount = 0;
	for( uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i )
	{
		// If there is no ship, skip
		if ( !m_rgClientData[i].m_bActive )
			continue;

		if ( m_rgClientData[i].m_SteamIDUser == steamID )
		{
			OutputDebugString( "Kicking player\n" );
			RemovePlayerFromServer( i, k_EDRClientKicked);
			// send him a kick message
			MsgServerFailAuthentication_t msg;
			int64 outMessage;
			SteamGameServerNetworkingSockets()->SendMessageToConnection(m_rgClientData[i].m_hConn, &msg, sizeof(msg), k_nSteamNetworkingSend_Reliable, &outMessage);
		}
		else
		{
			++uPlayerCount;
		}
	}
	m_uPlayerCount = uPlayerCount;
}
