//====== Copyright © 1996-2023 Valve Corporation, All rights reserved. =======
//
// Purpose: Class for adding to the Game Recording Timeline for different game states
//
//=============================================================================

#include "stdafx.h"
#include "timeline.h"
#include "SpaceWarClient.h"

extern uint32 Plat_GetTicks();

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTimeline::CTimeline( IGameEngine *pGameEngine ) : 
	m_pGameEngine( pGameEngine ),
	m_GameID( SteamUtils()->GetAppID() ),
	m_bInGame( false )
{
	SteamTimeline()->SetTimelineGameMode( k_ETimelineGameMode_Menus );
	m_ulSessionCounter = 0;
	m_ulInGameStartTime = 0;
	m_unLastTimestampIndexDisplayed = 0;
}


//-----------------------------------------------------------------------------
// Purpose: Game state has changed
//-----------------------------------------------------------------------------
void CTimeline::OnGameStateChange( EClientGameState eNewState )
{
	bool bInGameNow = false;	
	switch ( eNewState )
	{
	case k_EClientGameWaitingForPlayers:
	case k_EClientGameActive:
	case k_EClientGameQuitMenu:
	case k_EClientGameDraw:
	case k_EClientGameWinner:
		bInGameNow = true;
		break;

	case k_EClientStatsAchievements:
	case k_EClientGameStartServer:
	case k_EClientGameMenu:
	case k_EClientGameExiting:
	case k_EClientGameInstructions:
	case k_EClientGameConnecting:
	case k_EClientGameConnectionFailure:	
	case k_EClientFindInternetServers:
	default:
		break;
	}

	// change the timeline bar from gray to blue and add a timeline range covering the game session
	if ( m_bInGame != bInGameNow )
	{
		if ( bInGameNow )
		{			
			SteamTimeline()->SetTimelineGameMode( k_ETimelineGameMode_Playing );

			m_unSessionStart = Plat_GetTicks();

			// start timers for adding timeline timestamps
			m_ulInGameStartTime = m_pGameEngine->GetGameTickCount();
			m_unLastTimestampIndexDisplayed = 0;
		}
		else
		{
			SteamTimeline()->SetTimelineGameMode( k_ETimelineGameMode_Menus );

			uint32 unSessionEnd = Plat_GetTicks();
			uint32 unSessionDuration = unSessionEnd - m_unSessionStart;
			
			float flDurationSeconds = (float)unSessionDuration / 1000.f;
			float flStartOffsetSeconds = -flDurationSeconds;

			SteamTimeline()->AddRangeTimelineEvent( "In Match", nullptr, "steam_starburst", 100, flStartOffsetSeconds, flDurationSeconds, k_ETimelineEventClipPriority_None );
		}

		m_bInGame = bInGameNow;
	}

	// add a highlight marker every time the player wins
	if ( eNewState == k_EClientGameWinner && SpaceWarClient()->BLocalPlayerWonLastGame() )
	{
		SteamTimeline()->AddInstantaneousTimelineEvent( "Winner!", "You won a round!", "steam_attack", 10, 0, k_ETimelineEventClipPriority_Standard );
	}
	else if ( eNewState == k_EClientGameDraw )
	{
		SteamTimeline()->AddInstantaneousTimelineEvent( "Draw", "This round was a draw.", "steam_defend", 5, 0, k_ETimelineEventClipPriority_None );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Run a frame. Does not need to run at full frame rate.
//-----------------------------------------------------------------------------
void CTimeline::RunFrame()
{
	if ( m_bInGame )
	{
		// every 5 minutes, add a new timeline timestamp in the form of "05:00", "10:00", etc.
		// Note: we use 5 minutes here for demo purposes, but if appropriate for your game, you
		// might want to choose a larger interval to keep the Timeline less cluttered for users
		const uint32 k_unMinutesBetweenTimestamps = 5;
		const uint64 k_unMaxTimeToDisplayIndex = 95 / k_unMinutesBetweenTimestamps;
		
		uint64 ulSinceStartMS = m_pGameEngine->GetGameTickCount() - m_ulInGameStartTime;
		uint32 unTimestampIndex = (int)( ulSinceStartMS / (k_unMinutesBetweenTimestamps * 60 * 1000) );
		if ( unTimestampIndex > 0 && unTimestampIndex > m_unLastTimestampIndexDisplayed && unTimestampIndex <= k_unMaxTimeToDisplayIndex )
		{
			// max string length is "95:00"
			char rgchBuffer[ 6 ];
			sprintf_safe( rgchBuffer, "%02d:00", unTimestampIndex * k_unMinutesBetweenTimestamps );
			SteamTimeline()->SetTimelineTooltip( rgchBuffer, 0 );

			m_unLastTimestampIndexDisplayed = unTimestampIndex;
		}
	}
}
