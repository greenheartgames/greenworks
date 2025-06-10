//========= Copyright © 1996-2004, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================



const int k_cMaxSockets = 16;
class CP2PAuthPlayer;

bool SendAuthTicketToConnection( CSteamID steamIDFrom, HSteamNetConnection hConnectionTo, uint32 cubTicket, uint8 *pubTicket );

//-----------------------------------------------------------------------------
// Purpose: one player p2p auth process state machine
//-----------------------------------------------------------------------------
class CP2PAuthPlayer
{
public:
	CP2PAuthPlayer( IGameEngine *pGameEngine, CSteamID steamID, HSteamNetConnection hServerConn );
	~CP2PAuthPlayer();
	void EndGame();
	void StartAuthPlayer();
	bool BIsAuthOk();
	void HandleP2PSendingTicket( const MsgP2PSendingTicket_t *pMsg );

	CSteamID GetSteamID();

	STEAM_CALLBACK( CP2PAuthPlayer, OnBeginAuthResponse, ValidateAuthTicketResponse_t, m_CallbackBeginAuthResponse );

	const CSteamID m_steamID;
	const HSteamNetConnection m_hServerConnection;
private:
	uint64 GetGameTimeInSeconds() 
	{ 
		return m_pGameEngine->GetGameTickCount()/1000;
	}
	bool m_bSentTicket;
	bool m_bSubmittedHisTicket;
	bool m_bHaveAnswer;
	uint64 m_ulConnectTime;
	uint64 m_ulTicketTime;
	uint64 m_ulAnswerTime;
	uint32 m_cubTicketIGaveThisUser;
	uint8 m_rgubTicketIGaveThisUser[1024];
	uint32 m_cubTicketHeGaveMe;
	uint8 m_rgubTicketHeGaveMe[1024];
	HAuthTicket m_hAuthTicketIGaveThisUser;
	EBeginAuthSessionResult m_eBeginAuthSessionResult;
	EAuthSessionResponse m_eAuthSessionResponse;

	IGameEngine *m_pGameEngine;
};

//-----------------------------------------------------------------------------
// Purpose: simple wrapper for multiple players
//-----------------------------------------------------------------------------
class CP2PAuthedGame
{
public:
	CP2PAuthedGame( IGameEngine *pGameEngine );
	void PlayerDisconnect( int iSlot );
	void EndGame();
	void StartAuthPlayer( int iSlot, CSteamID steamID );
	void RegisterPlayer( int iSlot, CSteamID steamID );
	void HandleP2PSendingTicket( const void *pMessage );
	CSteamID GetSteamID();
	void InternalInitPlayer( int iSlot, CSteamID steamID, bool bStartAuthProcess );

	CP2PAuthPlayer *m_rgpP2PAuthPlayer[MAX_PLAYERS_PER_SERVER];
	IGameEngine *m_pGameEngine;
	HSteamNetConnection m_hConnServer;
};
