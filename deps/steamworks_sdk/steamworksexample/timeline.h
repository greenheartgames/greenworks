//====== Copyright © 1996-2023 Valve Corporation, All rights reserved. =======
//
// Purpose: Class for adding to the Game Recording Timeline for different game states
//
//=============================================================================

#ifndef TIMELINE_H
#define TIMELINE_H

#include "SpaceWar.h"
#include "GameEngine.h"


class CTimeline
{
public:
	CTimeline( IGameEngine *pGameEngine );

	void RunFrame();
	void OnGameStateChange( EClientGameState eNewState );

private:
	CGameID m_GameID;
	IGameEngine *m_pGameEngine;

	bool m_bInGame;
	uint64 m_ulInGameStartTime;
	uint32 m_unLastTimestampIndexDisplayed;
	uint64 m_ulSessionCounter;	
	uint32 m_unSessionStart;
};

#endif // TIMELINE_H
