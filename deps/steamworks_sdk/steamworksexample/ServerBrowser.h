//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for handling finding servers, getting their details, and displaying
// them inside the game
//
// $NoKeywords: $
//=============================================================================

#ifndef SERVERBROWSER_H
#define SERVERBROWSER_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "BaseMenu.h"
#include <list>

class CSpaceWarClient;
class CServerBrowserMenu;


// Class to encapsulate game server data
class CGameServer
{
public:
	CGameServer( gameserveritem_t *pGameServerItem );

	const char* GetName() { return m_szServerName; }

	const char* GetDisplayString() { return m_szServerString; }

	uint32 GetIP() { return m_unIPAddress; }

	int32 GetPort() { return m_nConnectionPort; }
	CSteamID GetSteamID()	{ return m_steamID; }

private:
	uint32 m_unIPAddress;			// IP address for the server
	int32 m_nConnectionPort;		// Port for game clients to connect to for this server
	int m_nPing;					// current ping time in milliseconds
	char m_szMap[32];				// current map
	char m_szGameDescription[64];	// game description
	int m_nPlayers;					// current number of players on the server
	int m_nMaxPlayers;				// Maximum players that can join this server
	int m_nBotPlayers;				// Number of bots (i.e simulated players) on this server
	bool m_bPassword;				// true if this server needs a password to join
	bool m_bSecure;					// Is this server protected by VAC
	int	m_nServerVersion;			// server version as reported to Steam
	char m_szServerName[64];		// Game server name
	char m_szServerString[128];		// String to show in server browser
	CSteamID m_steamID;
};

class CServerBrowser : public ISteamMatchmakingServerListResponse
{
public:
	CServerBrowser( IGameEngine *pGameEngine );
	~CServerBrowser();

	// Initiate a refresh of internet servers
	void RefreshInternetServers();

	// Initiate a refresh of LAN servers
	void RefreshLANServers();

	// Run a frame (to handle kb input and such as well as render)
	void RunFrame();

	// ISteamMatchmakingServerListResponse
	void ServerResponded( HServerListRequest hReq, int iServer );
	void ServerFailedToRespond( HServerListRequest hReq, int iServer );
	void RefreshComplete( HServerListRequest hReq, EMatchMakingServerResponse response );

private:

	// Pointer to engine instance (so we can draw stuff)
	IGameEngine *m_pGameEngine;

	// Track the number of servers we know about
	int m_nServers;

	// Track whether we are in the middle of a refresh or not
	bool m_bRequestingServers;

	// Track what server list request is currently running
	HServerListRequest m_hServerListRequest;

	// Menu object
	CServerBrowserMenu *m_pMenu;

	// List of game servers
	std::list< CGameServer > m_ListGameServers; 
};

#endif //SERVERBROWSER_H