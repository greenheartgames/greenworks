//========= Copyright © 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking leaderboards
//
//=============================================================================

#ifndef LEADERBOARDS_H
#define LEADERBOARDS_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "StatsAndAchievements.h"
#include "SpaceWarClient.h"


class ISteamUser;
class CSpaceWarClient;
class CLeaderboardMenu;

class CLeaderboards
{
public:
	// Constructor
	CLeaderboards( IGameEngine *pGameEngine );

	// Run a frame
	void RunFrame();

	// shows / refreshes leaderboard
	void Show();

	// Updates leaderboards with stats from our just finished game
	void UpdateLeaderboards( CStatsAndAchievements *pStats );

	// handles input from leaderboard menu 
	void OnMenuSelection( LeaderboardMenuItem_t selection );	

private:
	void FindLeaderboards();

	// Engine
	IGameEngine *m_pGameEngine;

	// Called when SteamUserStats()->FindOrCreateLeaderboard() returns asynchronously
	void OnFindLeaderboard( LeaderboardFindResult_t *pFindLearderboardResult, bool bIOFailure );
	CCallResult<CLeaderboards, LeaderboardFindResult_t> m_SteamCallResultCreateLeaderboard;

	// Called when SteamUserStats()->UploadLeaderboardScore() returns asynchronously
	void OnUploadScore( LeaderboardScoreUploaded_t *pFindLearderboardResult, bool bIOFailure );
	CCallResult<CLeaderboards, LeaderboardScoreUploaded_t> m_SteamCallResultUploadScore;

	// handles to our leaderboards
	SteamLeaderboard_t m_hQuickestWinLeaderboard;
	SteamLeaderboard_t m_hFeetTraveledLeaderboard;

	int m_bLoading;										// true if we looking up a leaderboard handle


	CLeaderboardMenu *m_pLeaderboardMenu;				// Displays the current leaderboard
	int m_nCurrentLeaderboard;							// Index for leaderboard the user is currently viewing
	
};

#endif // LEADERBOARDS_H