//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: A SpaceWarEntity is just like a VectorEntity, except it knows how
//			to apply gravity from the SpaceWar Sun
//
// $NoKeywords: $
//=============================================================================

#ifndef SPACEWARENTITY_H
#define SPACEWARENTITY_H

#include "GameEngine.h"
#include "VectorEntity.h"

class CSpaceWarEntity : public CVectorEntity
{
public:
	// Constructor
	CSpaceWarEntity( IGameEngine *pGameEngine, uint32 uCollisionRadius, bool bAffectedByGravity );

	// Destructor
	virtual ~CSpaceWarEntity() { return; }

	// Run Frame
	void RunFrame();

private:
	bool m_bAffectedByGravity;
};

#endif // SPACEWARENTITY_H