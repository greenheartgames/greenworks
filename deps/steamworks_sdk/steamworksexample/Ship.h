//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for rendering the player ships
//
// $NoKeywords: $
//=============================================================================

#ifndef SHIP_H
#define SHIP_H

#include <list>
#include "GameEngine.h"
#include "SpaceWarEntity.h"
#include "PhotonBeam.h"
#include "SpaceWar.h"

#define MAXIMUM_SHIP_THRUST 150

#define SHIP_DEBRIS_PIECES 6

// Forward declaration
class CShip;
class CSpaceWarServer;
class CStatsAndAchievements;

// Simple class for the ship thrusters
class CForwardThrusters : public CVectorEntity
{
public:
	CForwardThrusters( IGameEngine *pGameEngine, CShip *pShip );

	// Run Frame
	void RunFrame();

private:
	CShip *m_pShip;
};

// Again, but in reverse
class CReverseThrusters : public CVectorEntity
{
public:
	CReverseThrusters( IGameEngine *pGameEngine, CShip *pShip );

	// Run Frame
	void RunFrame();

private:
	CShip *m_pShip;
};

// Class to represent debris after explosion
class CShipDebris : public CSpaceWarEntity
{
public:
	CShipDebris( IGameEngine *pGameEngine, float xPos, float yPos, DWORD dwDebrisColor );

	// Run Frame
	void RunFrame();
private:

	// We keep the debris spinning
	float m_flRotationPerInterval;
};

class CShip : public CSpaceWarEntity
{
public:
	// Constructor
	CShip( IGameEngine *pGameEngine, bool bIsServerInstance, float xPos, float yPos, DWORD dwShipColor );

	// Destructor
	~CShip();

	// Run a frame
	void RunFrame();

	// Render a frame
	void Render();

	// Update ship with data from server 
	void OnReceiveServerUpdate( ServerShipUpdateData_t *pUpdateData );

	// Update the ship with data from a client
	void OnReceiveClientUpdate( ClientSpaceWarUpdateData_t *pUpdateData );

	// Get the update data for this ship client side (copying into memory passed in)
	bool BGetClientUpdateData( ClientSpaceWarUpdateData_t *pUpdatedata );

	// Build update data for the ship to send to clients
	void BuildServerUpdate( ServerShipUpdateData_t *pUpdateData );

	// Build update data for photon beams to send to clients
	void BuildServerPhotonBeamUpdate( ServerShipUpdateData_t *pUpdateData );

	// Reset vertex data for our object
	void ResetVertexData();

	// Set whether the ship is exploding
	void SetExploding( bool bExploding );

	// Rebuild the geometry when we change decoration
	void BuildGeometry();

	// Set whether the ship is disabled
	void SetDisabled( bool bDisabled ) { m_bDisabled = bDisabled; }

	// Set the initial rotation for the ship
	void SetInitialRotation( float flRotation ) { SetAccumulatedRotation( flRotation ); }

	// Setters for key bindings
	void SetVKBindingLeft( DWORD dwVKLeft ) { m_dwVKLeft = dwVKLeft; }
	void SetVKBindingRight( DWORD dwVKRight ) { m_dwVKRight = dwVKRight; }
	void SetVKBindingForwardThrusters( DWORD dwVKForward ) { m_dwVKForwardThrusters = dwVKForward; }
	void SetVKBindingReverseThrusters( DWORD dwVKReverse ) { m_dwVKReverseThrusters = dwVKReverse; }
	void SetVKBindingFire( DWORD dwVKFire ) { m_dwVKFire = dwVKFire; }

	// Check for photons which have hit the entity and destroy the photons
	void DestroyPhotonsColldingWith( CVectorEntity *pTarget );

	// Check whether any of the photons this ship has fired are colliding with the target
	bool BCheckForPhotonsCollidingWith( CVectorEntity *pTarget );

	// Check if the ship is currently exploding
	bool BIsExploding() { return m_bExploding; }

	// Check if the ship is currently disabled
	bool BIsDisabled() { return m_bDisabled; }

	// Set whether this ship instance is for the local player 
	// (meaning it should pay attention to key input and such)
	void SetIsLocalPlayer( bool bValue ) { m_bIsLocalPlayer = bValue; }
	bool BIsLocalPlayer() { return m_bIsLocalPlayer; }

	// Accumulate stats for this ship
	void AccumulateStats( CStatsAndAchievements *pStats );

	// Get the name for this ship (only really works server side)
	const char* GetPlayerName();

	int GetShieldStrength() { return m_nShipShieldStrength;  }
	void SetShieldStrength( int strength ) { m_nShipShieldStrength = strength; }

	// Update the vibration effects for the ship
	void UpdateVibrationEffects();

private:

	// Last time we sent an update on our local data to the server
	uint64 m_ulLastClientUpdateTick;

	// Last time we detected the thrust key go down
	uint64 m_ulLastThrustStartedTickCount;

	// Last time we fired a photon
	uint64 m_ulLastPhotonTickCount;

	// When we exploded
	uint64 m_ulExplosionTickCount;

	// Current trigger effect state
	bool m_bTriggerEffectEnabled;

	// is this ship our local ship, or a remote player?
	bool m_bIsLocalPlayer;

	// Is this ship instance running inside the server (otherwise its a client...)
	bool m_bIsServerInstance;

	// is the ship exploding?
	bool m_bExploding;

	// is the ship disabled for now?
	bool m_bDisabled;

	// cloak fade out
	int m_nFade;

	// vector of beams we have fired (in order of firing time)
	CPhotonBeam * m_rgPhotonBeams[MAX_PHOTON_BEAMS_PER_SHIP];

	// vector of debris to draw after an explosion
	std::list< CShipDebris *> m_ListDebris;

	// Color for this ship
	DWORD m_dwShipColor;

	// Decoration for this ship
	int m_nShipDecoration;

	// Weapon for this ship
	int m_nShipWeapon;

	// Power for this ship
	int m_nShipPower;

	// Power for this ship
	int m_nShipShieldStrength;

	HGAMETEXTURE m_hTextureWhite;

	// Thrusters for this ship
	CForwardThrusters m_ForwardThrusters;

	// Track whether to draw the thrusters next render call
	bool m_bForwardThrustersActive;

	// Thrusters for this ship
	CReverseThrusters m_ReverseThrusters;

	// Thrust and rotation speed can be anlog when using a Steam Controller
	float m_fThrusterLevel;
	float m_fTurnSpeed;

	// Track whether to draw the thrusters next render call
	bool m_bReverseThrustersActive;

	// This will get populated only if we are the local instance, and then
	// sent to the server in response to each server update
	ClientSpaceWarUpdateData_t m_SpaceWarClientUpdateData;

	// key bindings
	DWORD m_dwVKLeft;
	DWORD m_dwVKRight;
	DWORD m_dwVKForwardThrusters;
	DWORD m_dwVKReverseThrusters;
	DWORD m_dwVKFire;
};

#endif // SHIP_H