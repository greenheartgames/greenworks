//========= Copyright © 1996-2004, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================


#include "stdafx.h"
#include "SpaceWarClient.h"
#include "p2pauth.h"

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CP2PAuthPlayer::CP2PAuthPlayer( IGameEngine *pGameEngine, CSteamID steamID, HSteamNetConnection hServerConn )
: m_CallbackBeginAuthResponse( this, &CP2PAuthPlayer::OnBeginAuthResponse )
, m_steamID( steamID )
, m_hServerConnection( hServerConn )
{
	m_pGameEngine = pGameEngine;
	m_bSentTicket = false;
	m_bSubmittedHisTicket = false;
	m_bHaveAnswer = false;
	m_ulConnectTime = GetGameTimeInSeconds();
	m_cubTicketIGaveThisUser = 0;
	m_cubTicketHeGaveMe = 0;
}


//-----------------------------------------------------------------------------
// Purpose: destructor
//-----------------------------------------------------------------------------
CP2PAuthPlayer::~CP2PAuthPlayer()
{
	EndGame();
}


//-----------------------------------------------------------------------------
// Purpose: the steam backend has responded
//-----------------------------------------------------------------------------
void CP2PAuthPlayer::OnBeginAuthResponse( ValidateAuthTicketResponse_t *pCallback )
{
	if ( m_steamID == pCallback->m_SteamID )
	{
		char rgch[128];
		sprintf( rgch, "P2P:: Received steam response for account=%d\n", m_steamID.GetAccountID() );
		OutputDebugString( rgch );
		m_ulAnswerTime = GetGameTimeInSeconds();
		m_bHaveAnswer = true;
		m_eAuthSessionResponse = pCallback->m_eAuthSessionResponse;
	}
}


void CP2PAuthPlayer::StartAuthPlayer()
{
	// Create a ticket if we haven't yet
	if ( m_cubTicketIGaveThisUser == 0 )
	{
		SteamNetworkingIdentity snid;
		snid.SetSteamID( m_steamID );
		m_hAuthTicketIGaveThisUser = SteamUser()->GetAuthSessionTicket( m_rgubTicketIGaveThisUser, sizeof( m_rgubTicketIGaveThisUser ), &m_cubTicketIGaveThisUser, &snid );
	}

	// Send the ticket to the server.  It will relay to the client
	MsgP2PSendingTicket_t msg;
	msg.SetToken( m_rgubTicketIGaveThisUser, m_cubTicketIGaveThisUser );
	msg.SetSteamID( m_steamID.ConvertToUint64() );

	int64 nIgnoreMessageID;
	if ( SteamNetworkingSockets()->SendMessageToConnection( m_hServerConnection, &msg, sizeof(msg), k_nSteamNetworkingSend_Reliable, &nIgnoreMessageID ) == k_EResultOK )
	{
		m_bSentTicket = true;
	}

	// start a timer on this, if we dont get a ticket back within reasonable time, mark him timed out
	m_ulTicketTime = GetGameTimeInSeconds();
}


//-----------------------------------------------------------------------------
// Purpose: is this auth ok?
//-----------------------------------------------------------------------------
bool CP2PAuthPlayer::BIsAuthOk()
{
	if ( m_steamID.IsValid() )
	{
		// Timeout if we fail to establish communication with this player
		if ( !m_bSentTicket && !m_bSubmittedHisTicket )
		{
			if ( GetGameTimeInSeconds() - m_ulConnectTime > 30  )
			{
				char rgch[128];
				sprintf( rgch, "P2P:: Nothing received for account=%d\n", m_steamID.GetAccountID() );
				OutputDebugString( rgch );
				return false;
			}
		}
		// first ticket check: if i submitted his ticket - was it good?
		if ( m_bSubmittedHisTicket && m_eBeginAuthSessionResult != k_EBeginAuthSessionResultOK )
		{
			char rgch[128];
			sprintf( rgch, "P2P:: Ticket from account=%d was bad\n", m_steamID.GetAccountID() );
			OutputDebugString( rgch );
			return false;
		}
		// second ticket check: if the steam backend replied, was that good?
		if ( m_bHaveAnswer && m_eAuthSessionResponse != k_EAuthSessionResponseOK )
		{
			char rgch[128];
			sprintf( rgch, "P2P:: Steam response for account=%d was bad\n", m_steamID.GetAccountID() );
			OutputDebugString( rgch );
			return false;
		}
		// last: if i sent him a ticket and he has not reciprocated, time out after 30 sec
		if ( m_bSentTicket && !m_bSubmittedHisTicket )
		{
			if ( GetGameTimeInSeconds() - m_ulTicketTime > 30  )
			{
				char rgch[128];
				sprintf( rgch, "P2P:: No ticket received for account=%d\n", m_steamID.GetAccountID() );
				OutputDebugString( rgch );
				return false;
			}
		}
	}
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: the game engine is telling us about someone who left the game
//-----------------------------------------------------------------------------
void CP2PAuthPlayer::EndGame()
{
	if ( m_bSentTicket )
	{
		SteamUser()->CancelAuthTicket( m_hAuthTicketIGaveThisUser );
		m_bSentTicket = false;
	}
	if ( m_bSubmittedHisTicket )
	{
		SteamUser()->EndAuthSession( m_steamID );
		m_bSubmittedHisTicket = false;
	}
}


//-----------------------------------------------------------------------------
// Purpose: message from another player providing his ticket
//-----------------------------------------------------------------------------
void CP2PAuthPlayer::HandleP2PSendingTicket( const MsgP2PSendingTicket_t *pMsg )
{
	m_cubTicketHeGaveMe = pMsg->GetTokenLen();
	memcpy( m_rgubTicketHeGaveMe, pMsg->GetTokenPtr(), m_cubTicketHeGaveMe );
	m_eBeginAuthSessionResult = SteamUser()->BeginAuthSession( m_rgubTicketHeGaveMe, m_cubTicketHeGaveMe, m_steamID );
	m_bSubmittedHisTicket = true;
	char rgch[128];
	sprintf( rgch, "P2P:: ReceivedTicket from account=%d \n", m_steamID.GetAccountID() );
	OutputDebugString( rgch );
	if ( !m_bSentTicket )
		StartAuthPlayer();
}


//-----------------------------------------------------------------------------
// Purpose: utility wrapper
//-----------------------------------------------------------------------------
CSteamID CP2PAuthPlayer::GetSteamID()
{
	return SteamUser()->GetSteamID();
}


//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CP2PAuthedGame::CP2PAuthedGame( IGameEngine *pGameEngine )
{
	m_pGameEngine = pGameEngine;
	m_hConnServer = k_HSteamNetConnection_Invalid;

	// no players yet
	for ( int i = 0; i < MAX_PLAYERS_PER_SERVER; i++ )
	{
		m_rgpP2PAuthPlayer[i] = NULL;
	}
}


//-----------------------------------------------------------------------------
// Purpose: game with this player is over
//-----------------------------------------------------------------------------
void CP2PAuthedGame::PlayerDisconnect( int iSlot )
{
	if ( m_rgpP2PAuthPlayer[iSlot] )
	{
		m_rgpP2PAuthPlayer[iSlot]->EndGame();
		delete m_rgpP2PAuthPlayer[iSlot];
		m_rgpP2PAuthPlayer[iSlot] = NULL;
	}
}


//-----------------------------------------------------------------------------
// Purpose: game is over, disconnect everyone
//-----------------------------------------------------------------------------
void CP2PAuthedGame::EndGame()
{
	for ( int i = 0; i < MAX_PLAYERS_PER_SERVER; i++ )
	{
		if ( m_rgpP2PAuthPlayer[i] )
		{
			m_rgpP2PAuthPlayer[i]->EndGame();
			delete m_rgpP2PAuthPlayer[i];
			m_rgpP2PAuthPlayer[i] = NULL;
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: initialize player
//-----------------------------------------------------------------------------
void CP2PAuthedGame::InternalInitPlayer( int iSlot, CSteamID steamID, bool bStartAuthProcess )
{
	char rgch[128];
	sprintf( rgch, "P2P:: StartAuthPlayer slot=%d account=%d \n", iSlot, steamID.GetAccountID() );
	OutputDebugString( rgch );
	m_rgpP2PAuthPlayer[iSlot] = new CP2PAuthPlayer( m_pGameEngine, steamID, m_hConnServer );
	if ( bStartAuthProcess )
		m_rgpP2PAuthPlayer[iSlot]->StartAuthPlayer();
}


//-----------------------------------------------------------------------------
// Purpose: game host register this player, we wait for this player
//			to start the auth process by sending us his ticket, then we will
//			reciprocate
//-----------------------------------------------------------------------------
void CP2PAuthedGame::RegisterPlayer( int iSlot, CSteamID steamID )
{
	if (iSlot < MAX_PLAYERS_PER_SERVER)
		InternalInitPlayer( iSlot, steamID, false );
}
//-----------------------------------------------------------------------------
// Purpose: start the auth process by sending ticket to this player
//			he will reciprocate
//-----------------------------------------------------------------------------
void CP2PAuthedGame::StartAuthPlayer( int iSlot, CSteamID steamID )
{
	if (iSlot < MAX_PLAYERS_PER_SERVER)
		InternalInitPlayer( iSlot, steamID, true );
}


//-----------------------------------------------------------------------------
// Purpose: message handler
//-----------------------------------------------------------------------------
void CP2PAuthedGame::HandleP2PSendingTicket( const void *pMessage )
{
	const MsgP2PSendingTicket_t *pMsg = (const MsgP2PSendingTicket_t*)pMessage;
	for ( int i = 0; i < MAX_PLAYERS_PER_SERVER; i++ )
	{
		if ( m_rgpP2PAuthPlayer[i] && m_rgpP2PAuthPlayer[i]->GetSteamID() == pMsg->GetSteamID() )
		{
			m_rgpP2PAuthPlayer[i]->HandleP2PSendingTicket( pMsg );
			break;
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: utility wrapper
//-----------------------------------------------------------------------------
CSteamID CP2PAuthedGame::GetSteamID()
{
	return SteamUser()->GetSteamID();
}
