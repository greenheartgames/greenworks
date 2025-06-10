//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking stats and achievements
//
// $NoKeywords: $
//=============================================================================

#ifndef STATS_AND_ACHIEVEMENTS_H
#define STATS_AND_ACHIEVEMENTS_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "Inventory.h"

enum EAchievements
{
	ACH_WIN_ONE_GAME = 0,
	ACH_WIN_100_GAMES = 1,
	ACH_HEAVY_FIRE = 2,
	ACH_TRAVEL_FAR_ACCUM = 3,
	ACH_TRAVEL_FAR_SINGLE = 4,
};

struct Achievement_t
{
	EAchievements m_eAchievementID;
	const char *m_pchAchievementID;
	char m_rgchName[128];
	char m_rgchDescription[256];
	bool m_bAchieved;
	int m_iIconImage;
};

class ISteamUser;
class CSpaceWarClient;

class CStatsAndAchievements
{
public:
	// Constructor
	CStatsAndAchievements( IGameEngine *pGameEngine );

	// Run a frame. Does not need to run at full frame rate.
	void RunFrame();

	// Display the stats and achievements
	void Render();

	// Game state changed
	void OnGameStateChange( EClientGameState eNewState );

	// Accumulators
	void AddDistanceTraveled( float flDistance );

	// accessors
	float GetGameFeetTraveled() { return m_flGameFeetTraveled; }
	double GetGameDurationSeconds() { return m_flGameDurationSeconds; }

	STEAM_CALLBACK( CStatsAndAchievements, OnUserStatsStored, UserStatsStored_t, m_CallbackUserStatsStored );
	STEAM_CALLBACK( CStatsAndAchievements, OnAchievementStored, UserAchievementStored_t, m_CallbackAchievementStored );
	
private:

	void LoadUserStats();

	// Determine if we get this achievement now
	void EvaluateAchievement( Achievement_t &achievement );
	void UnlockAchievement( Achievement_t &achievement );

	// Store stats
	void StoreStatsIfNecessary();

	// Render helpers
	void DrawAchievementInfo( RECT &rect, Achievement_t &ach );
	void DrawStatInfo( RECT &rect, const char *pchName, float flValue );
	void DrawInventory( RECT &rect, SteamItemInstanceID_t itemid );

	// our GameID
	CGameID m_GameID;

	// Engine
	IGameEngine *m_pGameEngine;

	// Steam User interface
	ISteamUser *m_pSteamUser;

	// Steam UserStats interface
	ISteamUserStats *m_pSteamUserStats;

	// Display font
	HGAMEFONT m_hDisplayFont;

	// Did we get the stats from Steam?
	bool m_bStatsValid;

	// Should we store stats this frame?
	bool m_bStoreStats;

	// Current Stat details
	float m_flGameFeetTraveled;
	uint64 m_ulTickCountGameStart;
	double m_flGameDurationSeconds;

	// Persisted Stat details
	int m_nTotalGamesPlayed;
	int m_nTotalNumWins;
	int m_nTotalNumLosses;
	float m_flTotalFeetTraveled;
	float m_flMaxFeetTraveled;
	float m_flAverageSpeed;
};

#endif // STATS_AND_ACHIEVEMENTS_H
