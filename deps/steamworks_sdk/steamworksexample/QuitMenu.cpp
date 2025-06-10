//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class to define the pause menu
//
// $NoKeywords: $
//=============================================================================


#include "stdafx.h"
#include "QuitMenu.h"
#include "SpaceWar.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CQuitMenu::CQuitMenu( IGameEngine *pGameEngine ) : CBaseMenu<EClientGameState>( pGameEngine )
{
	AddMenuItem( MenuItem_t( "Resume Game", k_EClientGameActive ) );
	AddMenuItem( MenuItem_t( "Exit To Menu", k_EClientGameMenu ) );
	AddMenuItem( MenuItem_t( "Exit To Desktop", k_EClientGameExiting ) );
}
