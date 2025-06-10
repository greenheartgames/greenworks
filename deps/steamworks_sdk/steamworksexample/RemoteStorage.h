//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for manipulating Steam Cloud
//
// $NoKeywords: $
//=============================================================================

#ifndef REMOTE_STORAGE_H
#define REMOTE_STORAGE_H

#include "SpaceWar.h"
#include "GameEngine.h"

class ISteamUser;
class CSpaceWarClient;
class IRemoteStorageSync;
class CRemoteStorageScreen;

enum ERemoteStorageSyncMenuCommand
{
	k_EMenuCommandNone = 0,
	k_EMenuCommandProgress = 1,
	k_EMenuCommandSyncComplete = 2,
};

//-----------------------------------------------------------------------------
// Purpose: Example of Steam Cloud
//-----------------------------------------------------------------------------
class CRemoteStorage
{
public:

	// Constructor
	CRemoteStorage( IGameEngine *pGameEngine );
	~CRemoteStorage();

	// call when user changes to this menu
	void Show();

	// Display the remote storage screen
	void Render();

	// A sync menu item has been selected
	void OnMenuSelection( ERemoteStorageSyncMenuCommand selection );

private:
	IGameEngine *m_pGameEngine;
	CRemoteStorageScreen *m_pRemoteStorageScreen;
};


//-----------------------------------------------------------------------------
// Purpose: Screen where user can enter their custom message
//-----------------------------------------------------------------------------
class CRemoteStorageScreen
{
public:
	CRemoteStorageScreen( IGameEngine *pGameEngine );

	// call when user changes to this menu
	void Show();

	// Display the remote storage screen
	void Render();

	// Done showing this page?
	bool BFinished() { return m_bFinished; }

private:
	void GetFileStats();
	void LoadMessage();
	bool BHandleSelect();
	bool BHandleCancel();

	// Game engine
	IGameEngine *m_pGameEngine;

	// Display font
	HGAMEFONT m_hDisplayFont;

	// Steam User interface
	ISteamUser *m_pSteamUser;

	// Steam RemoteStorage interface
	ISteamRemoteStorage *m_pSteamRemoteStorage;

	// Greeting message
	char m_rgchGreeting[40];
	char m_rgchGreetingNext[40];

	bool m_bFinished;

	int32 m_nNumFilesInCloud;
	uint64 m_ulBytesQuota;
	uint64 m_ulAvailableBytes;
};



#endif
