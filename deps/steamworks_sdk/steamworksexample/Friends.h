//========= Copyright © 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking friends list
//
//=============================================================================

#ifndef FRIENDS_H
#define FRIENDS_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "SpaceWarClient.h"


class CSpaceWarClient;
class CFriendsListMenu;

class CFriendsList
{
public:
	// Constructor
	CFriendsList( IGameEngine *pGameEngine );

	// Run a frame
	void RunFrame();

	// shows / refreshes friends list
	void Show();

	// handles input from friends list menu 
	void OnMenuSelection( FriendsListMenuItem_t selection );

private:
	// Engine
	IGameEngine *m_pGameEngine;

	CFriendsListMenu *m_pFriendsListMenu;
};

#endif // FRIENDS_H