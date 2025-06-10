//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Base class for various game menu screens
//
// $NoKeywords: $
//=============================================================================


#include "stdafx.h"
#include "BaseMenu.h"

// Static data
HGAMEFONT g_hMenuFont = 0;
uint64 g_ulLastReturnKeyTick = 0;
uint64 g_ulLastKeyDownTick = 0;
uint64 g_ulLastKeyUpTick = 0;
