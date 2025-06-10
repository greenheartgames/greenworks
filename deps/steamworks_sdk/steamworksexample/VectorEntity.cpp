//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Base class for representation objects in the game which are drawn as 
//			vector art (ie, a series of lines)
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "VectorEntity.h"
#include "stdlib.h"
#include <math.h>


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CVectorEntity::CVectorEntity( IGameEngine *pGameEngine, uint32 uCollisionRadius ) 
{
	m_uCollisionRadius = uCollisionRadius;
	m_pGameEngine = pGameEngine;
	m_flRotationDeltaNextFrame = 0.0;
	m_flAccumulatedRotation = 0.0;
	m_flXAccel = 0.0;
	m_flYAccel = 0.0;
	m_flXAccelLastFrame = 0.0;
	m_flYAccelLastFrame = 0.0;
	m_flXPos = 0.0;
	m_flYPos = 0.0;
	m_flXVelocity = 0.0;
	m_flYVelocity = 0.0;
	m_bDisableCollisions = false;
	m_flRotationDeltaLastFrame = 0.0;

	// we should have at least one frame Run before
	// anyone asks for a delta, so this shouldn't cause
	// a large initial delta to our starting position, in theory
	m_flXPosLastFrame = 0;
	m_flYPosLastFrame = 0;

	m_flMaximumVelocity = DEFAULT_MAXIMUM_VELOCITY;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CVectorEntity::~CVectorEntity()
{
	
}


//-----------------------------------------------------------------------------
// Purpose: Add a line to our geometry
//-----------------------------------------------------------------------------
void CVectorEntity::AddLine( float xPos0, float yPos0, float xPos1, float yPos1, DWORD dwColor )
{
	VectorEntityVertex_t vert;
	vert.x = xPos0;
	vert.y = yPos0;
	vert.color = dwColor;

	m_VecVertexes.push_back( vert );

	vert.x = xPos1;
	vert.y = yPos1;
	vert.color = dwColor;

	m_VecVertexes.push_back( vert );
}

void CVectorEntity::ClearVertexes()
{
	m_VecVertexes.clear();
}

//-----------------------------------------------------------------------------
// Purpose: Set the current position for the object
//-----------------------------------------------------------------------------
void CVectorEntity::SetPosition( float xPos, float yPos )
{
	m_flXPos = xPos;
	m_flYPos = yPos;
}


//-----------------------------------------------------------------------------
// Purpose: Set the rotation to be applied next frame (in radians)
//-----------------------------------------------------------------------------
void CVectorEntity::SetRotationDeltaNextFrame( float flRotationInRadians )
{
	m_flRotationDeltaNextFrame = flRotationInRadians;
}


//-----------------------------------------------------------------------------
// Purpose: Set the acceleration to be applied next frame
//-----------------------------------------------------------------------------
void CVectorEntity::SetAcceleration( float flXAccel, float flYAccel )
{
	m_flXAccel = flXAccel;
	m_flYAccel = flYAccel;
}


//-----------------------------------------------------------------------------
// Purpose: Run a frame for the vector entity (ie, compute rotation, position, etc...)
//-----------------------------------------------------------------------------
void CVectorEntity::RunFrame()
{
	// Accumulate the rotation so we know our current rotation total at all times
	m_flAccumulatedRotation += m_flRotationDeltaNextFrame;
	m_flRotationDeltaLastFrame = m_flRotationDeltaNextFrame;
	m_flRotationDeltaNextFrame = 0.0f;

	m_flXPosLastFrame = m_flXPos;
	m_flYPosLastFrame = m_flYPos;

	// If the accumulated rotation is > 2pi (360) then wrap it (same for negative direction)
	// This prevents the value getting really large and losing precision
	int nInfiniteLoopProtector = 0;
	while ( m_flAccumulatedRotation >= 2.0f*PI_VALUE && ++nInfiniteLoopProtector < 100 )
		m_flAccumulatedRotation -= 2.0f*PI_VALUE;
	nInfiniteLoopProtector = 0;
	while ( m_flAccumulatedRotation <= -2.0f*PI_VALUE && ++nInfiniteLoopProtector < 100 )
		m_flAccumulatedRotation += 2.0f*PI_VALUE;


	// Update our acceleration, velocity, and finally position
	// Note: The min here is so we don't get massive acceleration if frames for some reason don't run for a bit
	float ulElapsedSeconds = MIN( (float)m_pGameEngine->GetGameTicksFrameDelta() / 1000.0f, 0.1f );
	m_flXVelocity += m_flXAccel * ulElapsedSeconds;
	m_flYVelocity += m_flYAccel * ulElapsedSeconds;

	// Make sure velocity does not exceed maximum allowed

	float flVelocity = (float)sqrt( m_flXVelocity*m_flXVelocity + m_flYVelocity*m_flYVelocity );

	if ( flVelocity > m_flMaximumVelocity )
	{
		float flRatio = m_flMaximumVelocity / flVelocity;

		m_flXVelocity = m_flXVelocity * flRatio;
		m_flYVelocity = m_flYVelocity * flRatio;
	}

	m_flXPos += m_flXVelocity * ulElapsedSeconds;
	m_flYPos += m_flYVelocity * ulElapsedSeconds;

	// Clear acceleration values, child classes should keep reseting it as appropriate each frame
	m_flXAccelLastFrame = m_flXAccel;
	m_flYAccelLastFrame = m_flYAccel;
	m_flXAccel = 0;
	m_flYAccel = 0;

	// Check for wrapping around the screen
	float width = (float)m_pGameEngine->GetViewportWidth();
	float height = (float)m_pGameEngine->GetViewportHeight();

	if ( m_flXPos > width )
		m_flXPos -= width;
	if ( m_flXPos < 0 )
		m_flXPos += width;

	if ( m_flYPos > height )
		m_flYPos -= height;
	if ( m_flYPos < 0 )
		m_flYPos += height;
}


//-----------------------------------------------------------------------------
// Purpose: Render the entity
//-----------------------------------------------------------------------------
void CVectorEntity::Render()
{
	// Compute values which will be used for rotation below
	float flSinRotation = (float)sin(m_flAccumulatedRotation);
	float flCosRotation = (float)cos(m_flAccumulatedRotation);

	if ( m_VecVertexes.size() < 2 )
		return;

	// Iterate our vector of vertexes 2 at a time drawing lines
	for( size_t i=0; i < m_VecVertexes.size() - 1; ++i )
	{
		DWORD dwColor0, dwColor1;
		float xPos0, yPos0, xPos1, yPos1;
		float xPrime0, yPrime0, xPrime1, yPrime1;

		// Grab the first point and apply rotation and translation
		xPos0 = m_VecVertexes[i].x;
		yPos0 = m_VecVertexes[i].y;
		dwColor0 = m_VecVertexes[i].color;

		// Apply any needed rotation
		xPrime0 = flCosRotation*xPos0 - flSinRotation*yPos0;
		yPrime0 = flSinRotation*xPos0 + flCosRotation*yPos0;

		// Apply translation to current position
		xPrime0 += m_flXPos;
		yPrime0 += m_flYPos;

		// Next vertex, we use 2 per iteration
		++i;

		// Grab the second point and apply rotation and translation
		xPos1 = m_VecVertexes[i].x;
		yPos1 = m_VecVertexes[i].y;
		dwColor1 = m_VecVertexes[i].color;

		// Apply any needed rotation
		xPrime1 = flCosRotation*xPos1 - flSinRotation*yPos1;
		yPrime1 = flSinRotation*xPos1 + flCosRotation*yPos1;

		// Apply translation to current position
		xPrime1 += m_flXPos;
		yPrime1 += m_flYPos;

		// Have the game engine draw the actual line (it batches these operations)
		m_pGameEngine->BDrawLine( xPrime0, yPrime0, dwColor0, xPrime1, yPrime1, dwColor1 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Render the entity with an override color instead of the vertex color
//-----------------------------------------------------------------------------
void CVectorEntity::Render(DWORD overrideColor)
{
	// Compute values which will be used for rotation below
	float flSinRotation = (float)sin(m_flAccumulatedRotation);
	float flCosRotation = (float)cos(m_flAccumulatedRotation);

	// Iterate our vector of vertexes 2 at a time drawing lines
	for( size_t i=0; i < m_VecVertexes.size() - 1; ++i )
	{
		DWORD dwColor0, dwColor1;
		float xPos0, yPos0, xPos1, yPos1;
		float xPrime0, yPrime0, xPrime1, yPrime1;

		// Grab the first point and apply rotation and translation
		xPos0 = m_VecVertexes[i].x;
		yPos0 = m_VecVertexes[i].y;
		dwColor0 = overrideColor;

		// Apply any needed rotation
		xPrime0 = flCosRotation*xPos0 - flSinRotation*yPos0;
		yPrime0 = flSinRotation*xPos0 + flCosRotation*yPos0;

		// Apply translation to current position
		xPrime0 += m_flXPos;
		yPrime0 += m_flYPos;

		// Next vertex, we use 2 per iteration
		++i;

		// Grab the second point and apply rotation and translation
		xPos1 = m_VecVertexes[i].x;
		yPos1 = m_VecVertexes[i].y;
		dwColor1 = overrideColor;

		// Apply any needed rotation
		xPrime1 = flCosRotation*xPos1 - flSinRotation*yPos1;
		yPrime1 = flSinRotation*xPos1 + flCosRotation*yPos1;

		// Apply translation to current position
		xPrime1 += m_flXPos;
		yPrime1 += m_flYPos;

		// Have the game engine draw the actual line (it batches these operations)
		m_pGameEngine->BDrawLine( xPrime0, yPrime0, dwColor0, xPrime1, yPrime1, dwColor1 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Check if the entity is colliding with the other given entity
//-----------------------------------------------------------------------------
bool CVectorEntity::BCollidesWith ( CVectorEntity * pTarget )
{

	// Note: Yes, this is a lame way to do collision detection just using a set radius.
	//       I don't care for the moment, just want it running!

	if ( m_bDisableCollisions )
		return false;
	else if ( pTarget->BCollisionDetectionDisabled() )
		return false;

	// Compute distance between the center of the two objects
	float distance = (float)sqrt( pow( m_flXPos - pTarget->GetXPos(), 2 ) + pow( m_flYPos - pTarget->GetYPos(), 2 ) );

	if ( distance < m_uCollisionRadius + pTarget->GetCollisionRadius() )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Check if the entity is colliding with the other given entity
//-----------------------------------------------------------------------------
float CVectorEntity::GetDistanceTraveledLastFrame()
{
	return (float)sqrt( pow( m_flXPos - m_flXPosLastFrame, 2 ) + pow( m_flYPos - m_flYPosLastFrame, 2 ) );
}
