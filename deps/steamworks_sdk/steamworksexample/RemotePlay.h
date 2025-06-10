//========= Copyright © 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for Remote Play session list
//
//=============================================================================

#ifndef REMOTEPLAY_H
#define REMOTEPLAY_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "SpaceWarClient.h"


class CSpaceWarClient;
class CRemotePlayListMenu;

class CRemotePlayList
{
public:
	// Constructor
	CRemotePlayList( IGameEngine *pGameEngine );

	// Run a frame
	void RunFrame();

	// shows / refreshes Remote Play session list
	void Show();

	// handles input from Remote Play session list menu 
	void OnMenuSelection( RemotePlayListMenuItem_t selection );

private:
	STEAM_CALLBACK( CRemotePlayList, OnRemotePlaySessionConnected, SteamRemotePlaySessionConnected_t );
	STEAM_CALLBACK( CRemotePlayList, OnRemotePlaySessionDisconnected, SteamRemotePlaySessionDisconnected_t );

private:
	// Engine
	IGameEngine *m_pGameEngine;

	CRemotePlayListMenu *m_pRemotePlayListMenu;
	int m_nNumControllers;
};

#endif // REMOTEPLAY_H