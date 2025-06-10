//========= Copyright Valve LLC, All rights reserved. ============
//
// Purpose: Class for joining and showing clan chats
//
//================================================================

#include "stdafx.h"
#include "clanchatroom.h"
#include "BaseMenu.h"
#include <math.h>

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CClanChatRoom::CClanChatRoom( IGameEngine *pGameEngine ) : m_pGameEngine( pGameEngine )
{
}


//-----------------------------------------------------------------------------
// Purpose: Run a frame for the CClanChatRoom
//-----------------------------------------------------------------------------
void CClanChatRoom::RunFrame()
{
}


//-----------------------------------------------------------------------------
// Purpose: Shows / Refreshes the chat room
//-----------------------------------------------------------------------------
void CClanChatRoom::Show()
{
	// start joining a chat, if we aren't in one already
	if ( !m_steamIDChat.IsValid() || !m_SteamCallResultJoinChatRoom.IsActive() )
	{
		// pick a clan to join from the users current data
		CSteamID steamIDBestClan;
		for ( int i = 0; i < SteamFriends()->GetClanCount(); i++ )
		{
			CSteamID steamIDClan = SteamFriends()->GetClanByIndex( i );
			int online, ingame, chatting;
			if ( SteamFriends()->GetClanActivityCounts( steamIDClan, &online, &ingame, &chatting ) )
			{
				if ( chatting > 0 )
				{
					steamIDBestClan = steamIDClan;
					break;
				}
				else if ( online )
				{
					steamIDBestClan = steamIDClan;
				}
			}
		}

		if ( steamIDBestClan.IsValid() )
		{
			SteamAPICall_t hCall = SteamFriends()->JoinClanChatRoom( steamIDBestClan );
			m_SteamCallResultJoinChatRoom.Set( hCall, this, &CClanChatRoom::OnJoinChatRoom );
			OutputDebugString( "joining clan chat...\n" );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Called when SteamFriends()->JoinClanChatRoom() returns asynchronously
//-----------------------------------------------------------------------------
void CClanChatRoom::OnJoinChatRoom( JoinClanChatRoomCompletionResult_t *pResult, bool bIOFailure )
{
	if ( pResult->m_eChatRoomEnterResponse == k_EChatRoomEnterResponseSuccess )
	{
		// we've entered
		OutputDebugString( "succesfully joined clan chat\n" );
	}

}
