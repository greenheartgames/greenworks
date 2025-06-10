//========= Copyright © 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for joining and showing clan chat rooms
//
//=============================================================================

#ifndef CLANCHATROOM_H
#define CLANCHATROOM_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "StatsAndAchievements.h"
#include "SpaceWarClient.h"


class ISteamUser;

class CClanChatRoom
{
public:
	// Constructor
	CClanChatRoom( IGameEngine *pGameEngine );

	// Run a frame
	void RunFrame();

	// shows / refreshes chat
	void Show();

private:
	// Engine
	IGameEngine *m_pGameEngine;
	
	// Called when SteamFriends()->JoinClanChatRoom() returns asynchronously
	void OnJoinChatRoom( JoinClanChatRoomCompletionResult_t *pResult, bool bIOFailure );
	CCallResult<CClanChatRoom, JoinClanChatRoomCompletionResult_t> m_SteamCallResultJoinChatRoom;

	CSteamID m_steamIDChat;

};

#endif // CLANCHATROOM_H