//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for rendering the sun
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "Sun.h"
#include <math.h>


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CSun::CSun( IGameEngine *pGameEngine ) : CSpaceWarEntity( pGameEngine, 2*SUN_VECTOR_SCALE_FACTOR, false )
{
	float xcenter = (float)pGameEngine->GetViewportWidth()/2;
	float ycenter = (float)pGameEngine->GetViewportHeight()/2;
	float sqrtof2 = (float)sqrt( 2.0 );

	DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 102 );

	// Initialize our geometry
	AddLine( (2.0f*SUN_VECTOR_SCALE_FACTOR), 0.0f, (-2.0f*SUN_VECTOR_SCALE_FACTOR), 0.0f, dwColor );
	AddLine( 0.0f, (2.0f*SUN_VECTOR_SCALE_FACTOR), 0.0f, (-2.0f*SUN_VECTOR_SCALE_FACTOR), dwColor );
	AddLine( -1.0f*sqrtof2*SUN_VECTOR_SCALE_FACTOR, sqrtof2*SUN_VECTOR_SCALE_FACTOR, sqrtof2*SUN_VECTOR_SCALE_FACTOR, -1.0f*sqrtof2*SUN_VECTOR_SCALE_FACTOR, dwColor );
	AddLine( sqrtof2*SUN_VECTOR_SCALE_FACTOR, sqrtof2*SUN_VECTOR_SCALE_FACTOR, -1.0f*sqrtof2*SUN_VECTOR_SCALE_FACTOR, -1.0f*sqrtof2*SUN_VECTOR_SCALE_FACTOR, dwColor );

	// Has to be after unlock since the base class will lock in this call
	SetPosition( xcenter, ycenter );
}

//-----------------------------------------------------------------------------
// Purpose: Run a frame for the sun
//-----------------------------------------------------------------------------
void CSun::RunFrame()
{
	// We want to rotate 90 degrees every 800ms (1.57 is 1/2pi, or 90 degrees in radians)
	SetRotationDeltaNextFrame( (PI_VALUE/2.0f) * (float)m_pGameEngine->GetGameTicksFrameDelta()/800.0f );
	CVectorEntity::RunFrame();
}
