//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for rendering photon beams
//
// $NoKeywords: $
//=============================================================================

#ifndef PHOTONBEAM_H
#define PHOTONBEAM_H

#include "GameEngine.h"
#include "SpaceWarEntity.h"
#include "SpaceWar.h"

class CPhotonBeam : public CSpaceWarEntity
{
public:
	// Constructor
	CPhotonBeam( IGameEngine *pGameEngine, float xPos, float yPos, DWORD dwBeamColor, float flInitialRotation, float flInitialXVelocity, float flInitialYVelocity );

	// Check if the photon beam needs to die
	bool BIsBeamExpired() { return m_pGameEngine->GetGameTickCount() > m_ulTickCountToDieAt; }

	// Update with new data from server
	void OnReceiveServerUpdate( ServerPhotonBeamUpdateData_t *pUpdateData );

private:
	uint64 m_ulTickCountToDieAt;
};

#endif // PHOTONBEAM_H