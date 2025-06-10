//========= Copyright � 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking friends list
//
//=============================================================================

#include "stdafx.h"
#include "Friends.h"
#include "BaseMenu.h"
#include <math.h>
#include <vector>
#include <algorithm>


//-----------------------------------------------------------------------------
// Purpose: Menu that shows your friends
//-----------------------------------------------------------------------------
class CFriendsListMenu : public CBaseMenu<FriendsListMenuItem_t>
{
	static const FriendsListMenuItem_t k_menuItemEmpty;

public:

	//-----------------------------------------------------------------------------
	// Purpose: Constructor
	//-----------------------------------------------------------------------------
	CFriendsListMenu( IGameEngine *pGameEngine ) : CBaseMenu<FriendsListMenuItem_t>( pGameEngine )
	{
		
	}

	//-----------------------------------------------------------------------------
	// Purpose: Creates friends list menu
	//-----------------------------------------------------------------------------
	void Rebuild()
	{
		PushSelectedItem();
		ClearMenuItems();

		AddMenuItem( CFriendsListMenu::MenuItem_t( "Friends List", k_menuItemEmpty ) );

		// First add pending incoming requests
		AddFriendsByFlag( k_EFriendFlagFriendshipRequested, "Incoming Friend Requests" );

		// Add each Tag group and record the users with tags
		std::vector<CSteamID> vecTaggedSteamIDs;
		int nFriendsGroups = SteamFriends()->GetFriendsGroupCount();
		for ( int iFG = 0; iFG < nFriendsGroups; iFG++ )
		{
			FriendsGroupID_t friendsGroupID = SteamFriends()->GetFriendsGroupIDByIndex( iFG );
			if ( friendsGroupID == k_FriendsGroupID_Invalid )
				continue;

			int nFriendsGroupMemberCount = SteamFriends()->GetFriendsGroupMembersCount( friendsGroupID );
			if ( !nFriendsGroupMemberCount )
				continue;

			const char *pszFriendsGroupName = SteamFriends()->GetFriendsGroupName( friendsGroupID );
			if ( pszFriendsGroupName == NULL )
				pszFriendsGroupName = "";
			AddMenuItem( CFriendsListMenu::MenuItem_t( "", k_menuItemEmpty ) );
			AddMenuItem( CFriendsListMenu::MenuItem_t( pszFriendsGroupName, k_menuItemEmpty ) );

			std::vector<CSteamID> vecSteamIDMembers( nFriendsGroupMemberCount );
			SteamFriends()->GetFriendsGroupMembersList( friendsGroupID, &vecSteamIDMembers[0], nFriendsGroupMemberCount );
			for ( int iMember = 0; iMember < nFriendsGroupMemberCount; iMember++ )
			{
				const CSteamID &steamIDMember = vecSteamIDMembers[iMember];
				AddFriendToMenu( steamIDMember );
				vecTaggedSteamIDs.push_back( steamIDMember );
			}
		}

		// Add the "normal" Friends category, filtering out the ones with tags
		AddFriendsByFlag( k_EFriendFlagImmediate, "Friends", &vecTaggedSteamIDs );

		// Finally add the pending outgoing requests
		AddFriendsByFlag( k_EFriendFlagRequestingFriendship, "Outgoing Friend Requests" );

		PopSelectedItem();
	}

private:

	void AddFriendsByFlag( int iFriendFlag, const char *pszName, std::vector<CSteamID> *pVecIgnoredSteamIDs = NULL )
	{
		int iFriendCount = SteamFriends()->GetFriendCount( iFriendFlag );
		if ( !iFriendCount )
			return;

		AddMenuItem( CFriendsListMenu::MenuItem_t( "", k_menuItemEmpty ) );
		AddMenuItem( CFriendsListMenu::MenuItem_t( pszName, k_menuItemEmpty ) );

		for ( int iFriend = 0; iFriend < iFriendCount; iFriend++ )
		{
			CSteamID steamIDFriend = SteamFriends()->GetFriendByIndex( iFriend, iFriendFlag );

			// This mimicks the Steam client's feature where it only shows
			// untagged friends in the canonical Friends section by default
			if ( pVecIgnoredSteamIDs && ( std::find( pVecIgnoredSteamIDs->begin(), pVecIgnoredSteamIDs->end(), steamIDFriend ) != pVecIgnoredSteamIDs->end() ) )
				continue;

			AddFriendToMenu( steamIDFriend );
		}
	}

	void AddFriendToMenu( CSteamID steamIDFriend )
	{
		if ( !steamIDFriend.IsValid() )
			return;

		FriendsListMenuItem_t menuItemFriend = { steamIDFriend };

		char szFriendNameBuffer[512] = { '\0' };

		const char *pszFriendName = SteamFriends()->GetFriendPersonaName( steamIDFriend );
		sprintf_safe( szFriendNameBuffer, "%s", pszFriendName );

		const char *pszFriendNickname = SteamFriends()->GetPlayerNickname( steamIDFriend );
		if ( pszFriendNickname )
		{
			sprintf_safe( szFriendNameBuffer, "%s (%s)", szFriendNameBuffer, pszFriendNickname );
		}

		AddMenuItem( CFriendsListMenu::MenuItem_t( szFriendNameBuffer, menuItemFriend ) );
	}
};

const FriendsListMenuItem_t CFriendsListMenu::k_menuItemEmpty = { k_steamIDNil };


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CFriendsList::CFriendsList( IGameEngine *pGameEngine ) : m_pGameEngine( pGameEngine )
{
	m_pFriendsListMenu = new CFriendsListMenu( pGameEngine );

	Show();
}


//-----------------------------------------------------------------------------
// Purpose: Run a frame for the CFriendsList
//-----------------------------------------------------------------------------
void CFriendsList::RunFrame()
{
	m_pFriendsListMenu->RunFrame();	
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing a friends list
//-----------------------------------------------------------------------------
void CFriendsList::OnMenuSelection( FriendsListMenuItem_t selection )
{
	// Do nothing (yet)
}


//-----------------------------------------------------------------------------
// Purpose: Shows / Refreshes the friends list
//-----------------------------------------------------------------------------
void CFriendsList::Show()
{
	m_pFriendsListMenu->Rebuild();
}

