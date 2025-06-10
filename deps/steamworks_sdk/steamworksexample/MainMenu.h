//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class to define the main game menu
//
// $NoKeywords: $
//=============================================================================

#ifndef MAINMENU_H
#define MAINMENU_H

#include <string>
#include <vector>
#include "GameEngine.h"
#include "SpaceWar.h"
#include "BaseMenu.h"
#include "SpaceWarClient.h"

class CMainMenu : public CBaseMenu<EClientGameState>
{
public:
	// Constructor
	CMainMenu( IGameEngine *pGameEngine );

	void SetupMenu();

private:
	STEAM_CALLBACK( CMainMenu, OnParentalSettingsChanged, SteamParentalSettingsChanged_t );
};

#endif // MAINMENU_H