//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class to find servers menu
//
//=============================================================================

#ifndef SERVERBROWSERMENU_H
#define SERVERBROWSERMENU_H

#include "BaseMenu.h"
#include "ServerBrowser.h"

class CServerBrowserMenu : public CBaseMenu<ServerBrowserMenuData_t>
{
public:
	// Constructor
	CServerBrowserMenu( IGameEngine *pGameEngine ) : CBaseMenu<ServerBrowserMenuData_t>( pGameEngine ) {}

	void Rebuild( std::list<CGameServer> &List, bool bIsRefreshing )
	{
		ClearMenuItems();

		ServerBrowserMenuData_t data;
		std::list<CGameServer>::iterator iter;

		for( iter = List.begin(); iter != List.end(); ++iter )
		{
			data.m_eStateToTransitionTo = k_EClientGameConnecting;
			data.m_steamIDGameServer = iter->GetSteamID();
			AddMenuItem( MenuItem_t( iter->GetDisplayString(), data ) );
		}

		data.m_eStateToTransitionTo = k_EClientGameMenu;
		AddMenuItem( CServerBrowserMenu::MenuItem_t( "Return to main menu", data ) );
	}
};

#endif // SERVERBROWSERMENU_H