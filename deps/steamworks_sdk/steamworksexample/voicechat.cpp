//========= Copyright © 1996-2010, Valve LLC, All rights reserved. ============
//
// Purpose:Class for P2P voice chat
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "voicechat.h"


CVoiceChat::CVoiceChat( IGameEngine *pGameEngine )
{
	m_pGameEngine = pGameEngine;
	m_bIsActive = false;
	m_ulLastTimeTalked = 0;
	m_hVoiceLoopback = 0;
}


CVoiceChat::~CVoiceChat()
{
	m_pGameEngine = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::RunFrame()
{
	if ( m_bIsActive )
	{
		// read local microphone input
		uint32 nBytesAvailable = 0;
		EVoiceResult res = SteamUser()->GetAvailableVoice( &nBytesAvailable, NULL, 0 );

		if ( res == k_EVoiceResultOK && nBytesAvailable > 0 )
		{	
			uint32 nBytesWritten = 0;
			MsgVoiceChatData_t msg;

			// don't send more then 1 KB at a time
			uint8 buffer[ 1024+sizeof(msg) ];

			res = SteamUser()->GetVoice( true, buffer+sizeof(msg), 1024, &nBytesWritten, false, NULL, 0, NULL, 0 );

			if ( res == k_EVoiceResultOK && nBytesWritten > 0 )
			{
				// assemble message.  note that we don't fill in the SteamID
				// here.  The server will know who sent
				msg.SetDataLength( nBytesWritten );
				memcpy( buffer, &msg, sizeof(msg) );

				// Send a message to the server with the data, server will broadcast this data on to all other clients.
				SteamNetworkingSockets()->SendMessageToConnection( m_hConnServer, buffer, sizeof(msg)+nBytesWritten, k_nSteamNetworkingSend_UnreliableNoDelay, nullptr );

				m_ulLastTimeTalked = m_pGameEngine->GetGameTickCount();

				// if local voice loopback is enabled, play it back now
				if ( m_hVoiceLoopback !=  0 )
				{
					// Uncompress the voice data, buffer holds up to 1 second of data
					uint32 numUncompressedBytes = 0; 
					const uint8* pVoiceData = (const uint8*) buffer;
					pVoiceData += sizeof(MsgVoiceChatData_t);

					res = SteamUser()->DecompressVoice( pVoiceData , nBytesWritten,
						m_ubUncompressedVoice, sizeof( m_ubUncompressedVoice ), &numUncompressedBytes, VOICE_OUTPUT_SAMPLE_RATE );

					if ( res == k_EVoiceResultOK && numUncompressedBytes > 0 )
					{
						m_pGameEngine->AddVoiceData( m_hVoiceLoopback, m_ubUncompressedVoice, numUncompressedBytes );
					}
				}
			}
		}		
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::HandleVoiceChatData( const void *pMessage )
{
	const MsgVoiceChatData_t *pMsgVoiceData = (const MsgVoiceChatData_t *) pMessage; 
	CSteamID fromSteamID = pMsgVoiceData->GetSteamID();

	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	iter = m_MapConnections.find( fromSteamID.ConvertToUint64() );
	if (iter == m_MapConnections.end())
		return;

	VoiceChatConnection_t &chatClient = iter->second;
	chatClient.ulLastReceiveVoiceTime = m_pGameEngine->GetGameTickCount();

	// Uncompress the voice data, buffer holds up to 1 second of data
	uint8 pbUncompressedVoice[ VOICE_OUTPUT_SAMPLE_RATE * BYTES_PER_SAMPLE ]; 
	uint32 numUncompressedBytes = 0; 
	const uint8* pVoiceData = (const uint8*) pMessage;
	pVoiceData += sizeof(MsgVoiceChatData_t);

	EVoiceResult res = SteamUser()->DecompressVoice( pVoiceData , pMsgVoiceData->GetDataLength(),
		pbUncompressedVoice, sizeof( pbUncompressedVoice ), &numUncompressedBytes, VOICE_OUTPUT_SAMPLE_RATE );

	if ( res == k_EVoiceResultOK && numUncompressedBytes > 0 )
	{
		// play it again Sam
		if ( chatClient.hVoiceChannel == 0 )
		{
			chatClient.hVoiceChannel = m_pGameEngine->HCreateVoiceChannel();
		}

		m_pGameEngine->AddVoiceData( chatClient.hVoiceChannel, pbUncompressedVoice, numUncompressedBytes );
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::MarkAllPlayersInactive()
{
	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	for( iter = m_MapConnections.begin(); iter != m_MapConnections.end(); ++iter )
	{
		iter->second.bActive = false;
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::MarkPlayerAsActive( CSteamID steamID )
{
	if ( !m_bIsActive )
		return;

	if ( m_SteamIDLocalUser == steamID )
		return;

	std::map< uint64, VoiceChatConnection_t >::iterator iter;
	iter = m_MapConnections.find( steamID.ConvertToUint64() );
	if ( iter != m_MapConnections.end() )
	{
		// player already has a session object, no new object created
		iter->second.bActive = true;
		return;
	}

	/*char szText[100];
	sprintf_safe(szText, "CVoiceChat::AddPlayerToSession: %s.\n", SteamFriends()->GetFriendPersonaName( steamID ) );
	OutputDebugString( szText ); */

	VoiceChatConnection_t session;
	session.ulLastReceiveVoiceTime = 0;
	session.hVoiceChannel = 0;
	session.bActive = true;

	m_MapConnections[ steamID.ConvertToUint64() ] = session;

	return;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CVoiceChat::IsPlayerTalking( CSteamID steamID )
{

	if ( steamID == m_SteamIDLocalUser )
	{
		// thats ourself
		if ( m_ulLastTimeTalked + 250 >  m_pGameEngine->GetGameTickCount() )
			return true;
	}
	else
	{
		std::map< uint64, VoiceChatConnection_t >::iterator iter;
		iter = m_MapConnections.find( steamID.ConvertToUint64() );
		if ( iter != m_MapConnections.end() )
		{
			if ( (iter->second.ulLastReceiveVoiceTime + 250) >  m_pGameEngine->GetGameTickCount() )
			{
				// user talked less then 250msec ago, assume still active
				return true;
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CVoiceChat::StartVoiceChat()
{
	if ( !m_bIsActive )
	{
		m_SteamIDLocalUser = SteamUser()->GetSteamID();

		SteamUser()->StartVoiceRecording();

		m_bIsActive = true;

		// here you can enable optional local voice loopback:		
		// m_hVoiceLoopback = m_pGameEngine->HCreateVoiceChannel();
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CVoiceChat::StopVoiceChat()
{
	if ( m_bIsActive )
	{
		std::map< uint64, VoiceChatConnection_t >::iterator iter;
		for( iter = m_MapConnections.begin(); iter != m_MapConnections.end(); ++iter )
		{
			CSteamID steamID( iter->first );
			m_pGameEngine->DestroyVoiceChannel( iter->second.hVoiceChannel );
		}

		m_MapConnections.clear();

		if ( m_hVoiceLoopback )
		{
			m_pGameEngine->DestroyVoiceChannel( m_hVoiceLoopback );
			m_hVoiceLoopback = 0;
		}

		SteamUser()->StopVoiceRecording();

		m_bIsActive = false;
	}
}
