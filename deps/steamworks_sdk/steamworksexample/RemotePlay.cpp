//========= Copyright � 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for Remote Play session list
//
//=============================================================================

#include "stdafx.h"
#include "RemotePlay.h"
#include "BaseMenu.h"


//-----------------------------------------------------------------------------
// Purpose: Menu that shows your Remote Play session
//-----------------------------------------------------------------------------
class CRemotePlayListMenu : public CBaseMenu<RemotePlayListMenuItem_t>
{
	static const RemotePlayListMenuItem_t k_menuItemEmpty;

public:

	//-----------------------------------------------------------------------------
	// Purpose: Constructor
	//-----------------------------------------------------------------------------
	CRemotePlayListMenu( IGameEngine *pGameEngine ) : CBaseMenu<RemotePlayListMenuItem_t>( pGameEngine )
	{
		
	}

	//-----------------------------------------------------------------------------
	// Purpose: Creates Remote Play session list menu
	//-----------------------------------------------------------------------------
	void Rebuild()
	{
		PushSelectedItem();
		ClearMenuItems();

		AddMenuItem( CRemotePlayListMenu::MenuItem_t( "Remote Play Session List", k_menuItemEmpty ) );

		InputHandle_t arrInputHandles[ STEAM_INPUT_MAX_COUNT ];
		int nNumControllers = SteamInput()->GetConnectedControllers( arrInputHandles );

		uint32 unSessionCount = SteamRemotePlay()->GetSessionCount();
		for ( uint32 iIndex = 0; iIndex < unSessionCount; iIndex++ )
		{
			RemotePlaySessionID_t unSessionID = SteamRemotePlay()->GetSessionID( iIndex );
			if ( !unSessionID )
			{
				continue;
			}

			RemotePlayListMenuItem_t item;
			item.m_unSessionID = unSessionID;

			const char *pszSessionPersonaName = SteamFriends()->GetFriendPersonaName( SteamRemotePlay()->GetSessionSteamID( unSessionID ) );
			const char *pszSessionClientName = SteamRemotePlay()->GetSessionClientName( unSessionID );
			const char *pszSessionClientFormFactor = GetFormFactor( SteamRemotePlay()->GetSessionClientFormFactor( unSessionID ) );

			int nResolutionX, nResolutionY;
			SteamRemotePlay()->BGetSessionClientResolution( unSessionID, &nResolutionX, &nResolutionY );

			char szLabel[ 1024 ];
			snprintf( szLabel, sizeof( szLabel ), "%s streaming to %s: %s %dx%d", pszSessionPersonaName, pszSessionClientName, pszSessionClientFormFactor, nResolutionX, nResolutionY );

			for ( int iController = 0; iController < nNumControllers; ++iController )
			{
				if ( SteamInput()->GetRemotePlaySessionID( arrInputHandles[ iController ] ) == unSessionID )
				{
					strncat( szLabel, ", has ", sizeof( szLabel ) - strlen( szLabel ) - 1 );
					strncat( szLabel, GetControllerType( SteamInput()->GetInputTypeForHandle( arrInputHandles[ iController ] ) ), sizeof( szLabel ) - strlen( szLabel ) - 1 );
				}
			}
			AddMenuItem( CRemotePlayListMenu::MenuItem_t( szLabel, item ) );
		}

		PopSelectedItem();
	}

private:
	const char *GetFormFactor( ESteamDeviceFormFactor eFormFactor )
	{
		switch ( eFormFactor )
		{
		case k_ESteamDeviceFormFactorPhone:
			return "[PHONE]";
		case k_ESteamDeviceFormFactorTablet:
			return "[TABLET]";
		case k_ESteamDeviceFormFactorComputer:
			return "[COMPUTER]";
		case k_ESteamDeviceFormFactorTV:
			return "[TV]";
		default:
			return "[UNKNOWN]";
		}
	}

	const char *GetControllerType( ESteamInputType eInputType )
	{
		switch ( eInputType )
		{
		case k_ESteamInputType_SteamController:
			return "Steam Controller";
		case k_ESteamInputType_XBox360Controller:
			return "XBox 360 Controller";
		case k_ESteamInputType_XBoxOneController:
			return "XBox One Controller";
		case k_ESteamInputType_PS4Controller:
			return "PS4 Controller";
		case k_ESteamInputType_MobileTouch:
			return "Touch Controller";
		default:
			return "Game Controller";
		}
	}
};

const RemotePlayListMenuItem_t CRemotePlayListMenu::k_menuItemEmpty = { 0 };


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CRemotePlayList::CRemotePlayList( IGameEngine *pGameEngine ) : m_pGameEngine( pGameEngine )
{
	m_pRemotePlayListMenu = new CRemotePlayListMenu( pGameEngine );
	m_nNumControllers = 0;
}


//-----------------------------------------------------------------------------
// Purpose: Run a frame for the CRemotePlayList
//-----------------------------------------------------------------------------
void CRemotePlayList::RunFrame()
{
	InputHandle_t arrInputHandles[ STEAM_INPUT_MAX_COUNT ];
	int nNumControllers = SteamInput()->GetConnectedControllers( arrInputHandles );
	if ( nNumControllers != m_nNumControllers )
	{
		m_nNumControllers = nNumControllers;

		m_pRemotePlayListMenu->Rebuild();
	}

	m_pRemotePlayListMenu->RunFrame();	
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing a Remote Play session list
//-----------------------------------------------------------------------------
void CRemotePlayList::OnMenuSelection( RemotePlayListMenuItem_t selection )
{
	// Do nothing (yet)
}


//-----------------------------------------------------------------------------
// Purpose: Shows / Refreshes the Remote Play session list
//-----------------------------------------------------------------------------
void CRemotePlayList::Show()
{
	m_pRemotePlayListMenu->Rebuild();
}


//-----------------------------------------------------------------------------
// Purpose: Handle Remote Play session connected
//-----------------------------------------------------------------------------
void CRemotePlayList::OnRemotePlaySessionConnected( SteamRemotePlaySessionConnected_t *pParam )
{
	m_pRemotePlayListMenu->Rebuild();
}


//-----------------------------------------------------------------------------
// Purpose: Handle Remote Play session disconnected
//-----------------------------------------------------------------------------
void CRemotePlayList::OnRemotePlaySessionDisconnected( SteamRemotePlaySessionDisconnected_t *pParam )
{
	m_pRemotePlayListMenu->Rebuild();
}
