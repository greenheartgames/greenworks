//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for handling finding & creating lobbies, getting their details, 
//			and seeing other users in the current lobby
//
//=============================================================================

#include "stdafx.h"
#include "Lobby.h"
#include "SpaceWarClient.h"
#include "p2pauth.h"


//-----------------------------------------------------------------------------
// Purpose: Menu that shows a list of other users in a lobby
//-----------------------------------------------------------------------------
class CLobbyMenu : public CBaseMenu<LobbyMenuItem_t>
{
public:
	// Constructor
	CLobbyMenu( IGameEngine *pGameEngine ) : CBaseMenu<LobbyMenuItem_t>( pGameEngine ) {}

	void Rebuild( const CSteamID &steamIDLobby )
	{
		PushSelectedItem();
		ClearMenuItems();

		if ( !steamIDLobby.IsValid() )
		{
			LobbyMenuItem_t menuItem = { CSteamID(), LobbyMenuItem_t::k_ELobbyMenuItemLeaveLobby };
			AddMenuItem( CLobbyMenu::MenuItem_t( "Lobby Disconnected - Return to main menu", menuItem ) );
			return;
		}

		// list of users in lobby
		// iterate all the users in the lobby and show their details
		int cLobbyMembers = SteamMatchmaking()->GetNumLobbyMembers( steamIDLobby );
		for ( int i = 0; i < cLobbyMembers; i++ )
		{
			CSteamID steamIDLobbyMember = SteamMatchmaking()->GetLobbyMemberByIndex( steamIDLobby, i ) ;

			// we get the details of a user from the ISteamFriends interface
			const char *pchName = SteamFriends()->GetFriendPersonaName( steamIDLobbyMember );
			// we may not know the name of the other users in the lobby immediately; but we'll receive
			// a PersonaStateUpdate_t callback when they do, and we'll rebuild the list then
			if ( pchName && *pchName )
			{
				const char *pchReady = SteamMatchmaking()->GetLobbyMemberData( steamIDLobby, steamIDLobbyMember, "ready" );
				bool bReady = ( pchReady && atoi( pchReady ) == 1);
				LobbyMenuItem_t menuItem = { steamIDLobbyMember, LobbyMenuItem_t::k_ELobbyMenuItemUser };

				char rgchMenuText[256];
				sprintf_safe( rgchMenuText, "%s %s", pchName, bReady ? "(READY)" : "" );
				AddMenuItem( MenuItem_t( std::string( rgchMenuText ), menuItem ) );
			}
		}


		// ready/not ready toggle
		{
			const char *pchReady = SteamMatchmaking()->GetLobbyMemberData( steamIDLobby, SteamUser()->GetSteamID(), "ready" );
			bool bReady = ( pchReady && atoi( pchReady ) == 1 );
			LobbyMenuItem_t menuItem = { CSteamID(), LobbyMenuItem_t::k_ELobbyMenuItemToggleReadState };
			if ( bReady )
				AddMenuItem( CLobbyMenu::MenuItem_t( "Set myself as Not Ready", menuItem ) );
			else
				AddMenuItem( CLobbyMenu::MenuItem_t( "Set myself as Ready", menuItem ) );
		}

		// see if the local user is the owner of this lobby
		bool bLobbyOwner = false;
		if ( SteamUser()->GetSteamID() == SteamMatchmaking()->GetLobbyOwner( steamIDLobby ) )
		{
			bLobbyOwner = true;
		}

		// start game
		if ( bLobbyOwner )
		{
			LobbyMenuItem_t menuItem = { CSteamID(), LobbyMenuItem_t::k_ELobbyMenuItemStartGame };
			AddMenuItem( CLobbyMenu::MenuItem_t( "Start game", menuItem ) );
		}

		// invite friend
		{
			LobbyMenuItem_t menuItem = { CSteamID(), LobbyMenuItem_t::k_ELobbyMenuItemInviteToLobby, steamIDLobby };
			AddMenuItem( CLobbyMenu::MenuItem_t( "Invite Friend", menuItem ) );
		}
		

		// exit lobby
		{
			LobbyMenuItem_t menuItem = { CSteamID(), LobbyMenuItem_t::k_ELobbyMenuItemLeaveLobby };
			AddMenuItem( CLobbyMenu::MenuItem_t( "Return to main menu", menuItem ) );
		}

		// reset selection
		PopSelectedItem();
	}
};


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CLobby::CLobby( IGameEngine *pGameEngine ) : 
		m_pGameEngine( pGameEngine ),
		m_CallbackPersonaStateChange( this, &CLobby::OnPersonaStateChange ),
		m_CallbackLobbyDataUpdate( this, &CLobby::OnLobbyDataUpdate ),
		m_CallbackChatDataUpdate( this, &CLobby::OnLobbyChatUpdate )
{
	m_pMenu = new CLobbyMenu( pGameEngine );
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CLobby::~CLobby()
{

}


//-----------------------------------------------------------------------------
// Purpose: Sets the ID of the lobby to display
//-----------------------------------------------------------------------------
void CLobby::SetLobbySteamID( const CSteamID &steamIDLobby )
{
	m_steamIDLobby = steamIDLobby;
	m_pMenu->Rebuild( m_steamIDLobby );
}


//-----------------------------------------------------------------------------
// Purpose: Draws the lobby
//-----------------------------------------------------------------------------
void CLobby::RunFrame()
{
	m_pMenu->RunFrame();
}


//-----------------------------------------------------------------------------
// Purpose: Handles a user in the lobby changing their name or details
//			( note: joining and leaving is handled below by CLobby::OnLobbyChatUpdate() )
//-----------------------------------------------------------------------------
void CLobby::OnPersonaStateChange( PersonaStateChange_t *pCallback )
{
	// callbacks are broadcast to all listeners, so we'll get this for every friend who changes state
	// so make sure the user is in the lobby before acting
	if ( !SteamFriends()->IsUserInSource( pCallback->m_ulSteamID, m_steamIDLobby ) )
		return;

	// rebuild the menu
	m_pMenu->Rebuild( m_steamIDLobby );
}


//-----------------------------------------------------------------------------
// Purpose: Handles lobby data changing
//-----------------------------------------------------------------------------
void CLobby::OnLobbyDataUpdate( LobbyDataUpdate_t *pCallback )
{
	// callbacks are broadcast to all listeners, so we'll get this for every lobby we're requesting
	if ( m_steamIDLobby != pCallback->m_ulSteamIDLobby )
		return;

	// set the heading
	m_pMenu->SetHeading( SteamMatchmaking()->GetLobbyData( m_steamIDLobby, "name" ) );

	// rebuild the menu
	m_pMenu->Rebuild( m_steamIDLobby );
}


//-----------------------------------------------------------------------------
// Purpose: Handles users in the lobby joining or leaving
//-----------------------------------------------------------------------------
void CLobby::OnLobbyChatUpdate( LobbyChatUpdate_t *pCallback )
{
	// callbacks are broadcast to all listeners, so we'll get this for every lobby we're requesting
	if ( m_steamIDLobby != pCallback->m_ulSteamIDLobby )
		return;

	if ( pCallback->m_ulSteamIDUserChanged == SteamUser()->GetSteamID().ConvertToUint64() && 
		( pCallback->m_rgfChatMemberStateChange &
			( k_EChatMemberStateChangeLeft|
				k_EChatMemberStateChangeDisconnected|
				k_EChatMemberStateChangeKicked|
				k_EChatMemberStateChangeBanned ) ) )
	{
		// we've left the lobby, so it is now invalid
		m_steamIDLobby = CSteamID();
	}

	// rebuild the menu
	m_pMenu->Rebuild( m_steamIDLobby );


	int cLobbyMembers = SteamMatchmaking()->GetNumLobbyMembers( m_steamIDLobby );
	for ( int i = 0; i < cLobbyMembers; i++ )
	{
		CSteamID steamIDLobbyMember = SteamMatchmaking()->GetLobbyMemberByIndex( m_steamIDLobby, i ) ;

		// ignore yourself.
		if ( SteamUser()->GetSteamID() == steamIDLobbyMember )
			continue;

	}
}


//-----------------------------------------------------------------------------
// Purpose: Menu that shows a list of lobbies to choose from
//-----------------------------------------------------------------------------
class CLobbyBrowserMenu : public CBaseMenu<LobbyBrowserMenuItem_t>
{
public:
	// Constructor
	CLobbyBrowserMenu( IGameEngine *pGameEngine ) : CBaseMenu<LobbyBrowserMenuItem_t>( pGameEngine ) {}

	void ShowSearching()
	{
		PushSelectedItem();
		ClearMenuItems();

		LobbyBrowserMenuItem_t data;
		data.m_eStateToTransitionTo = k_EClientGameMenu;
		AddMenuItem( CLobbyBrowserMenu::MenuItem_t( "Searching...", data ) );

		data.m_eStateToTransitionTo = k_EClientGameMenu;
		AddMenuItem( CLobbyBrowserMenu::MenuItem_t( "Return to main menu", data ) );

		PopSelectedItem();
	}

	void Rebuild( std::list<Lobby_t> &listLobbies )
	{
		PushSelectedItem();
		ClearMenuItems();

		LobbyBrowserMenuItem_t data;
		std::list<Lobby_t>::iterator iter;

		for( iter = listLobbies.begin(); iter != listLobbies.end(); ++iter )
		{
			data.m_eStateToTransitionTo = k_EClientJoiningLobby;
			data.m_steamIDLobby = iter->m_steamIDLobby;
			if ( iter->m_rgchName[0] )
			{
				AddMenuItem( MenuItem_t( std::string( iter->m_rgchName ), data ) );
			}
		}

		data.m_eStateToTransitionTo = k_EClientGameMenu;
		AddMenuItem( CLobbyBrowserMenu::MenuItem_t( "Return to main menu", data ) );

		// reset selection
		PopSelectedItem();
	}
};


//-----------------------------------------------------------------------------
// Purpose: Constructor
//			just initializes base data
//-----------------------------------------------------------------------------
CLobbyBrowser::CLobbyBrowser( IGameEngine *pGameEngine ) 
	: m_CallbackLobbyDataUpdated( this, &CLobbyBrowser::OnLobbyDataUpdatedCallback )
{
	m_pGameEngine = pGameEngine;
	m_pMenu = new CLobbyBrowserMenu( pGameEngine );
	m_pMenu->Rebuild( m_ListLobbies );
	m_pMenu->SetHeading( "Lobby browser" );
	m_bRequestingLobbies = false;
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CLobbyBrowser::~CLobbyBrowser()
{
	delete m_pMenu;
}


//-----------------------------------------------------------------------------
// Purpose: Run a frame (to handle KB input and such as well as render)
//-----------------------------------------------------------------------------
void CLobbyBrowser::RunFrame()
{
	m_pMenu->RunFrame();
}


//-----------------------------------------------------------------------------
// Purpose: Starts rebuilding the lobby list
//-----------------------------------------------------------------------------
void CLobbyBrowser::Refresh()
{
	if ( !m_bRequestingLobbies )
	{
		m_bRequestingLobbies = true;
		// request all lobbies for this game
		SteamAPICall_t hSteamAPICall = SteamMatchmaking()->RequestLobbyList();
		// set the function to call when this API call has completed
		m_SteamCallResultLobbyMatchList.Set( hSteamAPICall, this, &CLobbyBrowser::OnLobbyMatchListCallback );
		m_pMenu->ShowSearching();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Callback, on a list of lobbies being received from the Steam back-end
//-----------------------------------------------------------------------------
void CLobbyBrowser::OnLobbyMatchListCallback( LobbyMatchList_t *pCallback, bool bIOFailure )
{
	m_ListLobbies.clear();
	m_bRequestingLobbies = false;

	if ( bIOFailure )
	{
		// we had a Steam I/O failure - we probably timed out talking to the Steam back-end servers
		// doesn't matter in this case, we can just act if no lobbies were received
	}

	// lobbies are returned in order of closeness to the user, so add them to the list in that order
	for ( uint32 iLobby = 0; iLobby < pCallback->m_nLobbiesMatching; iLobby++ )
	{
		CSteamID steamIDLobby = SteamMatchmaking()->GetLobbyByIndex( iLobby );

		// add the lobby to the list
		Lobby_t lobby;
		lobby.m_steamIDLobby = steamIDLobby;
		// pull the name from the lobby metadata
		const char *pchLobbyName = SteamMatchmaking()->GetLobbyData( steamIDLobby, "name" );
		if ( pchLobbyName && pchLobbyName[0] )
		{
			// set the lobby name
			sprintf_safe( lobby.m_rgchName, "%s", pchLobbyName );
		}
		else
		{
			// we don't have info about the lobby yet, request it
			SteamMatchmaking()->RequestLobbyData( steamIDLobby );
			// results will be returned via LobbyDataUpdate_t callback
			sprintf_safe( lobby.m_rgchName, "Lobby %d", steamIDLobby.GetAccountID() );
		}

		m_ListLobbies.push_back( lobby );
	}

	m_pMenu->Rebuild( m_ListLobbies );
}


//-----------------------------------------------------------------------------
// Purpose: Callback, on a list of lobbies being received from the Steam back-end
//-----------------------------------------------------------------------------
void CLobbyBrowser::OnLobbyDataUpdatedCallback( LobbyDataUpdate_t *pCallback )
{
	// find the lobby in our local list 
	std::list<Lobby_t>::iterator iter;
	for( iter = m_ListLobbies.begin(); iter != m_ListLobbies.end(); ++iter )
	{
		// update the name of the lobby
		if ( iter->m_steamIDLobby == pCallback->m_ulSteamIDLobby )
		{
			// extract the display name from the lobby metadata
			const char *pchLobbyName = SteamMatchmaking()->GetLobbyData( iter->m_steamIDLobby, "name" );
			if ( pchLobbyName[0] )
			{
				sprintf_safe( iter->m_rgchName, "%s", pchLobbyName );
				// update the menu
				m_pMenu->Rebuild( m_ListLobbies );
			}
			return;
		}
	}
}
