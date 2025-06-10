//========= Copyright � Valve LLC, All rights reserved. ============
//
// Purpose: Examples for interacting with Overlay
//
//=============================================================================

#include "stdafx.h"
#include "OverlayExamples.h"
#include "BaseMenu.h"
#include <math.h>
#include <vector>
#include <algorithm>


//-----------------------------------------------------------------------------
// Purpose: Menu that shows your friends
//-----------------------------------------------------------------------------
class COverlayExamplesMenu : public CBaseMenu<OverlayExample_t>
{
public:

	//-----------------------------------------------------------------------------
	// Purpose: Constructor
	//-----------------------------------------------------------------------------
	COverlayExamplesMenu( IGameEngine *pGameEngine, COverlayExamples *pOverlayExamples ) : CBaseMenu<OverlayExample_t>( pGameEngine ), m_pOverlayExamples( pOverlayExamples )
	{
		
	}

	//-----------------------------------------------------------------------------
	// Purpose: Creates menu
	//-----------------------------------------------------------------------------
	void Rebuild()
	{
		PushSelectedItem();
		ClearMenuItems();

		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlay - Friends", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlay, "Friends" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlay - Community", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlay, "Community" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlay - Settings", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlay, "Settings" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlay - LobbyInvite", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlay, "LobbyInvite" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlay - OfficialGameGroup", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlay, "OfficialGameGroup" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlay - Leaderboards", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlay, "Leaderboards" } ) );

		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - steamid", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "steamid" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - chat", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "chat" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - jointrade", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "jointrade" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - stats", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "stats" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - achievements", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "achievements" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - friendadd", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "friendadd" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - friendremove", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "friendremove" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - friendrequestaccept", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "friendrequestaccept" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToUser - friendrequestignore", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser, "friendrequestignore" } ) );

		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToWebPage", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToWebPage, "https://steamcommunity.com/" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToWebPageModal", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToWebPageModal, "https://steamcommunity.com/" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToStore", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToStore, "" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToStore - Add to Cart", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToStore, "addtocart" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayToStore - Add to Cart & Show", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToStore, "addtocartandshow" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "ActivateGameOverlayInviteDialogConnectString", { OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayInviteDialogConnectString, NULL } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( SteamScreenshots()->IsScreenshotsHooked() ? "Screenshots Hooked!" : "Hook Screenshots", { OverlayExample_t::k_EOverlayExampleItem_HookScreenshots, NULL } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Request Keyboard", { OverlayExample_t::k_EOverlayExampleItem_RequestKeyboard, NULL } ) );

		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Set Notification Inset", { OverlayExample_t::k_EOverlayExampleItem_Notification_SetInset, "100" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Reset Notification Inset", { OverlayExample_t::k_EOverlayExampleItem_Notification_SetInset, "0" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Set Notification Position: Top Left", { OverlayExample_t::k_EOverlayExampleItem_Notification_SetPosition, "0" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Set Notification Position: Top Right", { OverlayExample_t::k_EOverlayExampleItem_Notification_SetPosition, "1" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Set Notification Position: Bottom Left", { OverlayExample_t::k_EOverlayExampleItem_Notification_SetPosition, "2" } ) );
		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Set Notification Position: Bottom Right", { OverlayExample_t::k_EOverlayExampleItem_Notification_SetPosition, "3" } ) );

		if ( m_pOverlayExamples->BHasLastGamePhase() )
		{
			AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Show last match", { OverlayExample_t::k_EOverlayExampleItem_Timeline_OpenOverlayToGamePhase } ) );
		}
		if ( m_pOverlayExamples->BHasLastTimelineEvent() )
		{
			AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Show last crash into sun", { OverlayExample_t::k_EOverlayExampleItem_Timeline_OpenOverlayToTimelineEvent } ) );
		}

		AddMenuItem( COverlayExamplesMenu::MenuItem_t( "Return to main menu", { OverlayExample_t::k_EOverlayExampleItem_BackToMenu, NULL } ) );
		
		PopSelectedItem();
	}

private:

	COverlayExamples *m_pOverlayExamples;
};


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
COverlayExamples::COverlayExamples( IGameEngine *pGameEngine )
	: m_pGameEngine( pGameEngine )
{
	m_pMenu = new COverlayExamplesMenu( pGameEngine, this );
	m_delayedCommand = { OverlayExample_t::k_EOverlayExampleItem_Invalid, NULL };

	Show();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void COverlayExamples::RunFrame()
{
	m_pMenu->RunFrame();

	if ( !m_pGameEngine->BIsKeyDown( VK_RETURN ) && !m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_MenuSelect ) )
	{
		switch ( m_delayedCommand.m_eItem )
		{
			case OverlayExample_t::k_EOverlayExampleItem_BackToMenu:
			{
				SpaceWarClient()->SetGameState( k_EClientGameMenu );
			}
			break;

			case OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlay:
			{
				SteamFriends()->ActivateGameOverlay( m_delayedCommand.m_pchExtraCommandData );
			}
			break;

			case OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToUser:
			{
				// pick first friend
				if ( SteamFriends()->GetFriendCount( k_EFriendFlagImmediate ) != 0 )
				{
					CSteamID steamID = SteamFriends()->GetFriendByIndex( 0, k_EFriendFlagImmediate );
					SteamFriends()->ActivateGameOverlayToUser( m_delayedCommand.m_pchExtraCommandData, steamID );
				}
			}
			break;

			case OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToWebPage:
			{
				SteamFriends()->ActivateGameOverlayToWebPage( m_delayedCommand.m_pchExtraCommandData );
			}
			break;

			case OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToWebPageModal:
			{
				SteamFriends()->ActivateGameOverlayToWebPage( m_delayedCommand.m_pchExtraCommandData, k_EActivateGameOverlayToWebPageMode_Modal );
			}
			break;

			case OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayToStore:
			{
				if ( !strcmp( m_delayedCommand.m_pchExtraCommandData, "addtocart" ) )
				{
					SteamFriends()->ActivateGameOverlayToStore( 440, k_EOverlayToStoreFlag_AddToCart );
				}
				else if ( !strcmp( m_delayedCommand.m_pchExtraCommandData, "addtocartandshow" ) )
				{
					SteamFriends()->ActivateGameOverlayToStore( 440, k_EOverlayToStoreFlag_AddToCartAndShow );
				}
				else
				{
					SteamFriends()->ActivateGameOverlayToStore( 440, k_EOverlayToStoreFlag_None );
				}
			}
			break;

			/*
			case OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayRemotePlayTogetherInviteDialog:
			{
				SteamFriends()->ActivateGameOverlayRemotePlayTogetherInviteDialog( steamIDLobby );
			}
			break;
			*/

			case OverlayExample_t::k_EOverlayExampleItem_ActivateGameOverlayInviteDialogConnectString:
			{
				const char *pConnectString = SteamFriends()->GetFriendRichPresence( SteamUser()->GetSteamID(), "connect" );
				SteamFriends()->ActivateGameOverlayInviteDialogConnectString( pConnectString );
			}
			break;

			case OverlayExample_t::k_EOverlayExampleItem_HookScreenshots:
			{
				SteamScreenshots()->HookScreenshots( !SteamScreenshots()->IsScreenshotsHooked() );
				m_pMenu->Rebuild();
			}
			break;

			case OverlayExample_t::k_EOverlayExampleItem_RequestKeyboard:
			{
				EGamepadTextInputMode eInputMode = k_EGamepadTextInputModeNormal;
				EGamepadTextInputLineMode eLineInputMode = k_EGamepadTextInputLineModeSingleLine;
				const char *pchDescription = "Enter Text Here";
				uint32 unCharMax = 20;
				const char *pchExistingText = "Placeholder";
				SteamUtils()->ShowGamepadTextInput( eInputMode , eLineInputMode, pchDescription, unCharMax, pchExistingText );
			}
			break;

			case OverlayExample_t::k_EOverlayExampleItem_Notification_SetInset:
				SteamUtils()->SetOverlayNotificationInset( atoi( m_delayedCommand.m_pchExtraCommandData ), atoi( m_delayedCommand.m_pchExtraCommandData ) );
				break;
			case OverlayExample_t::k_EOverlayExampleItem_Notification_SetPosition:
				SteamUtils()->SetOverlayNotificationPosition( (ENotificationPosition)atoi( m_delayedCommand.m_pchExtraCommandData ) );
				break;

			case OverlayExample_t::k_EOverlayExampleItem_Timeline_OpenOverlayToGamePhase:
				SteamTimeline()->OpenOverlayToGamePhase( m_strLastGamePhaseIDToShow.c_str() );
				break;
			case OverlayExample_t::k_EOverlayExampleItem_Timeline_OpenOverlayToTimelineEvent:
				SteamTimeline()->OpenOverlayToTimelineEvent( m_ulLastCrashIntoSunEventIDToShow );
				break;

			default:
				break;
		}

		m_delayedCommand.m_eItem = OverlayExample_t::k_EOverlayExampleItem_Invalid;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing a friends list
//-----------------------------------------------------------------------------
void COverlayExamples::OnMenuSelection( OverlayExample_t selection )
{
	m_delayedCommand = selection;
}


//-----------------------------------------------------------------------------
// Purpose: Shows / Refreshes the friends list
//-----------------------------------------------------------------------------
void COverlayExamples::Show()
{

	if ( SpaceWarClient()->GetLastGamePhaseID() )
	{
		SteamAPICall_t hSteamAPICall = SteamTimeline()->DoesGamePhaseRecordingExist( std::to_string( SpaceWarClient()->GetLastGamePhaseID() ).c_str() );
		m_SteamCallResultDoesGamePhaseRecordingExist.Set( hSteamAPICall, this, &COverlayExamples::OnDoesGamePhaseRecordingExist );
	}
	m_strLastGamePhaseIDToShow.erase();

	if ( SpaceWarClient()->GetLastCrashIntoSunEvent() )
	{
		SteamAPICall_t hSteamAPICall = SteamTimeline()->DoesEventRecordingExist( SpaceWarClient()->GetLastCrashIntoSunEvent() );
		m_SteamCallResultDoesEventRecordingExist.Set( hSteamAPICall, this, &COverlayExamples::OnDoesEventRecordingExist );
	}

	m_pMenu->Rebuild();
}

void COverlayExamples::OnScreenshotRequested( ScreenshotRequested_t *pCallback )
{
	SteamFriends()->ActivateGameOverlayToWebPage( "google.com" );
}

void COverlayExamples::OnSteamScreenshotReady( ScreenshotReady_t *pCallback )
{

}

void COverlayExamples::OnDoesEventRecordingExist( SteamTimelineEventRecordingExists_t *pCallback, bool bIOFailure )
{
	if ( bIOFailure || !pCallback->m_bRecordingExists )
	{
		// nothing to do here. We didn't show these items when showing the menu
		return;
	}

	m_ulLastCrashIntoSunEventIDToShow = pCallback->m_ulEventID;
	m_pMenu->Rebuild();
}


void COverlayExamples::OnDoesGamePhaseRecordingExist( SteamTimelineGamePhaseRecordingExists_t *pCallback, bool bIOFailure )
{
	if ( bIOFailure || ( pCallback->m_ulRecordingMS == 0 && pCallback->m_unClipCount == 0 ) )
	{
		// nothing to do here. We didn't show these items when showing the menu
		return;
	}

	m_strLastGamePhaseIDToShow = pCallback->m_rgchPhaseID;
	m_pMenu->Rebuild();
}


