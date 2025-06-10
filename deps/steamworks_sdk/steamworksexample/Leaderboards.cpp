//========= Copyright � 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking leaderboards
//
//=============================================================================

#include "stdafx.h"
#include "Leaderboards.h"
#include "BaseMenu.h"
#include <math.h>

//-----------------------------------------------------------------------------
// Purpose: Menu that shows a leaderboard
//-----------------------------------------------------------------------------
class CLeaderboardMenu : public CBaseMenu<LeaderboardMenuItem_t>
{
	static const int k_nMaxLeaderboardEntries = 10;						// maximum number of leaderboard entries we can display
	LeaderboardEntry_t m_leaderboardEntries[k_nMaxLeaderboardEntries];	// leaderboard entries we received from DownloadLeaderboardEntries
	int m_nLeaderboardEntries;											// number of leaderboard entries we received

	SteamLeaderboard_t m_hSteamLeaderboard;			// handle to the leaderboard we are displaying
	ELeaderboardDataRequest m_eLeaderboardData;		// type of data we are displaying
	bool m_bLoading;								// waiting to receive leaderboard results
	bool m_bIOFailure;								// last attempt to retrieve the leaderboard failed

	CCallResult<CLeaderboardMenu, LeaderboardScoresDownloaded_t> m_callResultDownloadEntries;

public:

	//-----------------------------------------------------------------------------
	// Purpose: Constructor
	//-----------------------------------------------------------------------------
	CLeaderboardMenu( IGameEngine *pGameEngine ) : CBaseMenu<LeaderboardMenuItem_t>( pGameEngine )
	{
		m_hSteamLeaderboard = 0;
		m_nLeaderboardEntries = 0;
		m_bLoading = false;
		m_bIOFailure = false;
	}

	//-----------------------------------------------------------------------------
	// Purpose: Menu that shows a leaderboard
	//-----------------------------------------------------------------------------
	void ShowLeaderboard( SteamLeaderboard_t hLeaderboard, ELeaderboardDataRequest eLeaderboardData, int offset )
	{
		m_hSteamLeaderboard = hLeaderboard;
		m_eLeaderboardData = eLeaderboardData;
		m_bLoading = true;
		m_bIOFailure = false;

		if ( hLeaderboard )
		{
			// load the specified leaderboard data. We only display k_nMaxLeaderboardEntries entries at a time
			SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries( hLeaderboard, eLeaderboardData, 
				offset, offset + k_nMaxLeaderboardEntries );

			// Register for the async callback
			m_callResultDownloadEntries.Set( hSteamAPICall, this, &CLeaderboardMenu::OnLeaderboardDownloadedEntries );
		}

		Rebuild();
	}

	//-----------------------------------------------------------------------------
	// Purpose: Creates leaderboard menu
	//-----------------------------------------------------------------------------
	void Rebuild()
	{
		PushSelectedItem();
		ClearMenuItems();

		LeaderboardMenuItem_t menuItemBack = { true, false };
		LeaderboardMenuItem_t menuItemNextLeaderboard = { false, true };
		LeaderboardMenuItem_t menuItemEmpty = { 0 };

		if ( m_hSteamLeaderboard )
		{
			// create a header for the leaderboard

			std::string strName = "Leaderboard: ";
			strName += SteamUserStats()->GetLeaderboardName( m_hSteamLeaderboard );

			if ( m_eLeaderboardData == k_ELeaderboardDataRequestGlobal )
				strName += ", Top 10";
			else if ( m_eLeaderboardData == k_ELeaderboardDataRequestGlobalAroundUser )
				strName += ", Around User";
			else if ( m_eLeaderboardData == k_ELeaderboardDataRequestFriends )
				strName += ", Friends of User";

			AddMenuItem( CLeaderboardMenu::MenuItem_t( strName, menuItemEmpty ) );
		}

		// create leaderboard
		if ( !m_hSteamLeaderboard || m_bLoading )
		{
			AddMenuItem( CLeaderboardMenu::MenuItem_t( "Loading...", menuItemEmpty ) );
		}
		else if ( m_bIOFailure )
		{
			AddMenuItem( CLeaderboardMenu::MenuItem_t( "Network failure!", menuItemEmpty ) );
		}
		else
		{
			if ( m_nLeaderboardEntries == 0 )
			{
				// Requesting for global scores around the user will return successfully with 0 results if the
				// user does not have an entry on the leaderboard

				std::string strText;
				if ( m_eLeaderboardData != k_ELeaderboardDataRequestGlobalAroundUser )
				{
					strText = "No scores for this leaderboard";
				}
				else
				{
					strText = SteamFriends()->GetPersonaName();
					strText += " does not have a score for this leaderboard";
				}

				AddMenuItem( CLeaderboardMenu::MenuItem_t( strText, menuItemEmpty ) );
			}

			for ( int index = 0; index < m_nLeaderboardEntries; index++ )
			{
				char rgchMenuText[256];
				const char *pchName = SteamFriends()->GetFriendPersonaName( m_leaderboardEntries[index].m_steamIDUser );
				sprintf_safe( rgchMenuText, "(%d) %s - %d", m_leaderboardEntries[index].m_nGlobalRank, 
					pchName, m_leaderboardEntries[index].m_nScore );

				AddMenuItem( MenuItem_t( std::string( rgchMenuText ), menuItemEmpty ) );
			}
		}

		// navigation buttons
		AddMenuItem( CLeaderboardMenu::MenuItem_t( "Next leaderboard", menuItemNextLeaderboard ) );
		AddMenuItem( CLeaderboardMenu::MenuItem_t( "Return to main menu", menuItemBack ) );

		PopSelectedItem();
	}
	
	//-----------------------------------------------------------------------------
	// Purpose: Called when SteamUserStats()->DownloadLeaderboardEntries() returns asynchronously
	//-----------------------------------------------------------------------------
	void OnLeaderboardDownloadedEntries( LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded, bool bIOFailure )
	{
		m_bLoading = false;
		m_bIOFailure = bIOFailure;
		
		// leaderboard entries handle will be invalid once we return from this function. Copy all data now.
		m_nLeaderboardEntries = MIN( pLeaderboardScoresDownloaded->m_cEntryCount, k_nMaxLeaderboardEntries );
		for ( int index = 0; index < m_nLeaderboardEntries; index++ )
		{
			SteamUserStats()->GetDownloadedLeaderboardEntry( pLeaderboardScoresDownloaded->m_hSteamLeaderboardEntries, 
				index, &m_leaderboardEntries[ index ], NULL, 0 );
		}

		// show our new data
		Rebuild();		
	}
};




//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CLeaderboards::CLeaderboards( IGameEngine *pGameEngine ) : m_pGameEngine( pGameEngine )
{
	m_hQuickestWinLeaderboard = 0;
	m_hFeetTraveledLeaderboard = 0;
	m_nCurrentLeaderboard = 0;

	m_bLoading = false;
	m_pLeaderboardMenu = new CLeaderboardMenu( pGameEngine );

	FindLeaderboards();
}


//-----------------------------------------------------------------------------
// Purpose: Run a frame for the CLeaderboards
//-----------------------------------------------------------------------------
void CLeaderboards::RunFrame()
{
	m_pLeaderboardMenu->RunFrame();	
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing a leaderboard
//-----------------------------------------------------------------------------
void CLeaderboards::OnMenuSelection( LeaderboardMenuItem_t selection )
{
	if ( selection.m_bBack )
	{
		SpaceWarClient()->SetGameState( k_EClientGameMenu );
	}
	else if ( selection.m_bNextLeaderboard )
	{
		m_nCurrentLeaderboard = (m_nCurrentLeaderboard+1) % 2;
		Show();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Shows / Refreshes the leaderboard
//-----------------------------------------------------------------------------
void CLeaderboards::Show()
{
	if ( m_nCurrentLeaderboard == 0 )
	{
		// we want to show the top 10. To do so, we request global score data beginning at 0
		m_pLeaderboardMenu->ShowLeaderboard( m_hQuickestWinLeaderboard, k_ELeaderboardDataRequestGlobal, 0 );
	}
	else if ( m_nCurrentLeaderboard == 1 )
	{
		// we want to show the 10 users around us
		m_pLeaderboardMenu->ShowLeaderboard( m_hFeetTraveledLeaderboard, k_ELeaderboardDataRequestGlobalAroundUser, -5 );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Gets handles for our leaderboards. If the leaderboards don't exist, creates them.
//			Each time this is called, we look up another leaderboard.
//-----------------------------------------------------------------------------
void CLeaderboards::FindLeaderboards()
{
	if ( m_bLoading )
		return;

	SteamAPICall_t hSteamAPICall = 0;

	if ( !m_hQuickestWinLeaderboard )
	{
		// find/create a leaderboard for the quickest win
		hSteamAPICall = SteamUserStats()->FindOrCreateLeaderboard( LEADERBOARD_QUICKEST_WIN,
			k_ELeaderboardSortMethodAscending, k_ELeaderboardDisplayTypeTimeSeconds );
	}
	else if ( !m_hFeetTraveledLeaderboard )
	{
		// find/create a leaderboard for the most feet traveled in 1 round
		hSteamAPICall = SteamUserStats()->FindOrCreateLeaderboard( LEADERBOARD_FEET_TRAVELED,
			k_ELeaderboardSortMethodDescending, k_ELeaderboardDisplayTypeNumeric );
	}

	if ( hSteamAPICall != 0 )
	{
		// set the function to call when this API call has completed
		m_SteamCallResultCreateLeaderboard.Set( hSteamAPICall, this, &CLeaderboards::OnFindLeaderboard );
		m_bLoading = true;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Called when SteamUserStats()->FindOrCreateLeaderboard() returns asynchronously
//-----------------------------------------------------------------------------
void CLeaderboards::OnFindLeaderboard( LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure )
{
	m_bLoading = false;

	// see if we encountered an error during the call
	if ( !pFindLeaderboardResult->m_bLeaderboardFound || bIOFailure )
		return;

	// check to see which leaderboard handle we just retrieved
	const char *pchName = SteamUserStats()->GetLeaderboardName( pFindLeaderboardResult->m_hSteamLeaderboard );
	if ( strcmp( pchName, LEADERBOARD_QUICKEST_WIN ) == 0 )
		m_hQuickestWinLeaderboard = pFindLeaderboardResult->m_hSteamLeaderboard;
	else if ( strcmp( pchName, LEADERBOARD_FEET_TRAVELED ) == 0 )
		m_hFeetTraveledLeaderboard = pFindLeaderboardResult->m_hSteamLeaderboard;

	// look up any other leaderboards
	FindLeaderboards();

	// if the user is currently looking at a leaderboard, it might be one we didn't have a handle for yet. Update the leaderboard.
	if ( SpaceWarClient()->GetGameState() == k_EClientLeaderboards )
		Show();	
}



//-----------------------------------------------------------------------------
// Purpose: Updates leaderboards with stats from our just finished game
//-----------------------------------------------------------------------------
void CLeaderboards::UpdateLeaderboards( CStatsAndAchievements *pStats )
{
	// if the user won, update the leaderboard with the time it took. If the user's previous time was faster, this time will be thrown out.
	if ( m_hQuickestWinLeaderboard && SpaceWarClient()->BLocalPlayerWonLastGame() )
	{
		SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore( m_hQuickestWinLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, (int)pStats->GetGameDurationSeconds(), NULL, 0 );
		m_SteamCallResultUploadScore.Set( hSteamAPICall, this, &CLeaderboards::OnUploadScore );
	}

	// update the leaderboard for the most feet traveled in 1 round. If the user previously traveled farther in a round than this one,
	// this value will be thrown out
	if ( m_hFeetTraveledLeaderboard )
	{
		SteamUserStats()->UploadLeaderboardScore( m_hFeetTraveledLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, (int)pStats->GetGameFeetTraveled(), NULL, 0 );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Called when SteamUserStats()->UploadLeaderboardScore() returns asynchronously
//-----------------------------------------------------------------------------
void CLeaderboards::OnUploadScore( LeaderboardScoreUploaded_t *pScoreUploadedResult, bool bIOFailure )
{
	if ( !pScoreUploadedResult->m_bSuccess )
	{
		// error
	}

	if ( pScoreUploadedResult->m_bScoreChanged )
	{
		// could display new rank
	}
}
