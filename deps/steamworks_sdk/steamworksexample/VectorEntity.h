//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Base class for representation objects in the game which are drawn as 
//			vector art (ie, a series of lines)
//
// $NoKeywords: $
//=============================================================================

#ifndef VECTORENTITY_H
#define VECTORENTITY_H

#include "GameEngine.h"
#include <vector>

struct VectorEntityVertex_t
{
	float x, y;
	DWORD color;
};

#define DEFAULT_MAXIMUM_VELOCITY 450.0f

#define PI_VALUE 3.14159265f

class CVectorEntity
{
public:
	// Constructor
	CVectorEntity( IGameEngine *pGameEngine, uint32 uCollisionRadius );
	
	// Destructor
	virtual ~CVectorEntity();

	// Run a frame
	virtual void RunFrame();

	// Render the sun field
	virtual void Render();

	// Render with an explicit color
	virtual void Render(DWORD overrideColor);

	// Check if the entity is colliding with another given entity
	bool BCollidesWith( CVectorEntity * pTarget );

	// Get the rotation value that is to be applied next frame
	float GetRotationDeltaNextFrame() { return m_flRotationDeltaNextFrame; }

	// Get the rotation value that was applied last frame
	float GetRotationDeltaLastFrame() { return m_flRotationDeltaLastFrame; }

	// Get the cumulative rotation for this entity
	float GetAccumulatedRotation() { return m_flAccumulatedRotation; }

	// Get the acceleration to be applied next frame
	float GetXAcceleration() { return m_flXAccel; }
	float GetYAcceleration() { return m_flYAccel; }

	// Get the acceleration applied last frame
	float GetXAccelerationLastFrame() { return m_flXAccelLastFrame; }
	float GetYAccelerationLastFrame() { return m_flYAccelLastFrame; }

	// Get the current velocity
	float GetXVelocity() { return m_flXVelocity; }
	float GetYVelocity() { return m_flYVelocity; }

	// Get the current position of the object
	float GetXPos() { return m_flXPos; }
	float GetYPos() { return m_flYPos; }

	// Get the distance traveled each frame
	float GetDistanceTraveledLastFrame();
	
	// Add a line to the entity
	void AddLine(float xPos0, float yPos0, float xPos1, float yPos1, DWORD dwColor);

	// Clear all lines in the entity
	void ClearVertexes();

	// Set the objects current position
	void SetPosition(float xPos, float yPos);

	// Set the velocity of the entity (normally you should just set acceleration and this will be computed)
	void SetVelocity(float xVelocity, float yVelocity) { m_flXVelocity = xVelocity; m_flYVelocity = yVelocity; }

protected:

	// Set the rotation to be applied next frame
	void SetRotationDeltaNextFrame( float flRotationInRadians );

	// Set the acceleration to be applied next frame
	void SetAcceleration( float xAccel, float yAccel );
	// Set the cumulative rotation for this entity (overriding any existing value)
	void SetAccumulatedRotation( float flRotation ) { m_flAccumulatedRotation = flRotation; }
	
	// Reset velocity of the entity
	void ResetVelocity() { m_flXVelocity = 0; m_flYVelocity = 0; }

	// Get the collision radius for the entity
	uint32 GetCollisionRadius() { return m_uCollisionRadius; }

	// Enable/Disable collision detection for this entity
	void SetCollisionDetectionDisabled( bool bDisabled ) { m_bDisableCollisions = bDisabled; }

	// Check whether collision detection has been disabled for the entity
	bool BCollisionDetectionDisabled() { return m_bDisableCollisions; }

	// Set a maximum velocity other than the default
	void SetMaximumVelocity( float flMaximumVelocity ) { m_flMaximumVelocity = flMaximumVelocity; }

protected:
	// Game engine instance we are running under
	IGameEngine *m_pGameEngine;

private:
	// Vector of points (always built 2 at a time so it's actually lines)
	std::vector< VectorEntityVertex_t > m_VecVertexes;

	// Previous position
	float m_flXPosLastFrame;
	float m_flYPosLastFrame;

	// current position (position is at the center of the object)
	float m_flXPos;
	float m_flYPos;

	// maximum velocity the object can have in either x/y 
	float m_flMaximumVelocity;

	// acceleration to be applied next frame
	float m_flXAccel;
	float m_flYAccel;

	// acceleration applied last frame
	float m_flXAccelLastFrame;
	float m_flYAccelLastFrame;

	// current velocity (affected by acceleration changes)
	float m_flXVelocity;
	float m_flYVelocity;

	// rotation to apply this frame (in radians)
	float m_flRotationDeltaNextFrame;

	// rotation which was applied last frame
	float m_flRotationDeltaLastFrame;

	// total cumulative rotation that has been applied to this entity
	float m_flAccumulatedRotation;

	// radius to use for collisions, this is applied from the center of the object out
	uint32 m_uCollisionRadius;

	// bool to disable collision detection for this object
	bool m_bDisableCollisions;
};

#endif // VECTORENTITY_H