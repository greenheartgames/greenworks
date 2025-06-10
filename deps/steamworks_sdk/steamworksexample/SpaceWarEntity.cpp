//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: A SpaceWarEntity is just like a VectorEntity, except it knows how
//			to apply gravity from the SpaceWar Sun
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "SpaceWarEntity.h"
#include "stdlib.h"
#include <math.h>

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CSpaceWarEntity::CSpaceWarEntity( IGameEngine *pGameEngine, uint32 uCollisionRadius, bool bAffectedByGravity ) 
	: CVectorEntity( pGameEngine, uCollisionRadius )
{
	m_bAffectedByGravity = bAffectedByGravity;
}

//-----------------------------------------------------------------------------
// Purpose: RunFrame
//-----------------------------------------------------------------------------
void CSpaceWarEntity::RunFrame()
{

	if ( m_bAffectedByGravity )
	{
		float xAccel = GetXAcceleration();
		float yAccel = GetYAcceleration();

		// Ships are also affected by the suns gravity, compute that here, sun is always at the center of the screen
		float xPosSun = (float)m_pGameEngine->GetViewportWidth()/2;
		float yPosSun = (float)m_pGameEngine->GetViewportHeight()/2;

		float distanceToSun = (float)sqrt( pow( xPosSun - GetXPos(), 2 ) + pow( yPosSun - GetYPos(), 2 ) );
		float distancePower = (float)pow( distanceToSun, 2.0f ); // gravity power falls off exponentially
		float factor = MIN( 5200000.0f / distancePower, 150.0f ); // arbitrary value for power of gravity

		float xDirection = (GetXPos() - xPosSun)/distanceToSun;
		float yDirection = (GetYPos() - yPosSun)/distanceToSun;

		xAccel -= factor * xDirection;
		yAccel -= factor * yDirection;

		// Set updated acceleration
		SetAcceleration( xAccel, yAccel );
	}

	CVectorEntity::RunFrame();
}