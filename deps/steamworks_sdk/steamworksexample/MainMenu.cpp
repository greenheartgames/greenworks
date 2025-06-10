//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class to define the main game menu
//
// $NoKeywords: $
//=============================================================================


#include "stdafx.h"
#include "MainMenu.h"
#include "SpaceWar.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMainMenu::CMainMenu( IGameEngine *pGameEngine ) : CBaseMenu<EClientGameState>( pGameEngine )
{
	SetupMenu();
}


//-----------------------------------------------------------------------------
// Purpose: Add relevant menu entries, honoring parental settings
//-----------------------------------------------------------------------------
void CMainMenu::SetupMenu()
{
	ISteamParentalSettings *pSettings = SteamParentalSettings();

	AddMenuItem( MenuItem_t( "Start New Server", k_EClientGameStartServer ) );
	AddMenuItem( MenuItem_t( "Find LAN Servers", k_EClientFindLANServers ) );
	AddMenuItem( MenuItem_t( "Find Internet Servers", k_EClientFindInternetServers ) );
	AddMenuItem( MenuItem_t( "Create Lobby", k_EClientCreatingLobby ) );
	AddMenuItem( MenuItem_t( "Find Lobby", k_EClientFindLobby ) );
	AddMenuItem( MenuItem_t( "Instructions", k_EClientGameInstructions ) );
	if ( !pSettings->BIsFeatureBlocked( k_EFeatureProfile ) )
	{
		AddMenuItem( MenuItem_t( "Stats and Achievements", k_EClientStatsAchievements ) );
	}
	AddMenuItem( MenuItem_t( "Leaderboards", k_EClientLeaderboards ) );	

	if ( !pSettings->BIsFeatureBlocked( k_EFeatureFriends ) )
	{
		AddMenuItem( MenuItem_t( "Friends List", k_EClientFriendsList ) );
		AddMenuItem( MenuItem_t( "Group chat room", k_EClientClanChatRoom ) );
	}
	AddMenuItem( MenuItem_t( "Remote Play Invite", k_EClientRemotePlayInvite ) );
	AddMenuItem( MenuItem_t( "Remote Play Sessions", k_EClientRemotePlaySessions ) );
	AddMenuItem( MenuItem_t( "Remote Storage", k_EClientRemoteStorage ) );
	AddMenuItem( MenuItem_t( "Write Minidump", k_EClientMinidump ) );

	if ( !pSettings->BIsFeatureBlocked( k_EFeatureBrowser ) )
	{
		AddMenuItem( MenuItem_t( "Web Callback", k_EClientWebCallback ) );
	}

	AddMenuItem( MenuItem_t( "Music Player", k_EClientMusic ) );
	if ( !pSettings->BIsFeatureBlocked( k_EFeatureCommunity ) )
	{
		AddMenuItem( MenuItem_t( "Workshop Items", k_EClientWorkshop ) );
	}

	if ( !pSettings->BIsFeatureBlocked( k_EFeatureBrowser ) )
	{
		AddMenuItem( MenuItem_t( "HTML Page", k_EClientHTMLSurface ) );
	}

	AddMenuItem( MenuItem_t( "In-game Store", k_EClientInGameStore ) );

	AddMenuItem( MenuItem_t( "OverlayAPI", k_EClientOverlayAPI ) );

	AddMenuItem( MenuItem_t( "Exit Game", k_EClientGameExiting ) );
}


//-----------------------------------------------------------------------------
// Purpose: Callback for a change in parental settings. Rebuild menu.
//-----------------------------------------------------------------------------
void CMainMenu::OnParentalSettingsChanged( SteamParentalSettingsChanged_t *pParam )
{
	ClearMenuItems();
	SetupMenu();
}
