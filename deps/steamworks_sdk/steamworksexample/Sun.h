//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for rendering the sun
//
// $NoKeywords: $
//=============================================================================

#ifndef SUN_H
#define SUN_H

#include "GameEngine.h"
#include "SpaceWarEntity.h"

#define SUN_VECTOR_SCALE_FACTOR 14

class CSun : public CSpaceWarEntity
{
public:
	// Constructor
	CSun( IGameEngine *pGameEngine );

	// Run a frame
	void RunFrame();

};

#endif // SUN_H