//========= Copyright © 1996-2010, Valve LLC, All rights reserved. ============
//
// Purpose: Class for P2P voice chat
//
// $NoKeywords: $
//=============================================================================

#ifndef VOICE_CHAT_H
#define VOICE_CHAT_H

#include "GameEngine.h"
#include "SpaceWar.h"
#include "Messages.h"
#include "steam/isteamnetworkingsockets.h"

typedef struct VoiceChatConnection_s
{
	uint64 ulLastReceiveVoiceTime;
	HGAMEVOICECHANNEL hVoiceChannel;	// engine voice channel for this player
	bool   bActive;	
}  VoiceChatConnection_t;

class CVoiceChat
{
public:

	CVoiceChat( IGameEngine *pGameEngine );
	~CVoiceChat();

	bool StartVoiceChat();
	void StopVoiceChat();

	// chat control
	void MarkAllPlayersInactive();
	void MarkPlayerAsActive( CSteamID steamID );

	bool IsPlayerTalking( CSteamID steamID );

	// chat engine
	void RunFrame();
	void HandleVoiceChatData( const void *pMessage );
	
	HSteamNetConnection m_hConnServer;

private:

	// Pointer to engine instance (so we can play sound)
	IGameEngine *m_pGameEngine;
	
	// map of voice chat sessions with other players
	std::map< uint64, VoiceChatConnection_t > m_MapConnections;

	CSteamID m_SteamIDLocalUser; // ourself
	bool m_bIsActive;	// is voice chat system active
	uint64 m_ulLastTimeTalked; // last time we've talked ourself
	HGAMEVOICECHANNEL m_hVoiceLoopback;
	uint8 m_ubUncompressedVoice[ VOICE_OUTPUT_SAMPLE_RATE * BYTES_PER_SAMPLE ]; // too big for the stack
};

#endif