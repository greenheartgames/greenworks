//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for rendering the player ships
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "GameEngine.h"
#include "Ship.h"
#include "stdlib.h"
#include "SpaceWarServer.h"
#include "StatsAndAchievements.h"
#include "Inventory.h"
#include <math.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Purpose: Constructor for thrusters
//-----------------------------------------------------------------------------
CForwardThrusters::CForwardThrusters( IGameEngine *pGameEngine, CShip *pShip ) : CVectorEntity( pGameEngine, 0 )
{
	DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 102 );

	// Initialize our geometry
	AddLine( 0.0, 12.0f, 0.0f, 19.0f, dwColor );
	AddLine( 1.0, 12.0f, 6.0f, 19.0f, dwColor );
	AddLine( 4.0, 12.0f, 11.0f, 19.0f, dwColor );
	AddLine( -1.0, 12.0f, -6.0f, 19.0f, dwColor );
	AddLine( -4.0, 12.0f, -11.0f, 19.0f, dwColor );

	m_pShip = pShip;
}


//-----------------------------------------------------------------------------
// Purpose: Run Frame, updates us to be in the same position/rotation as the ship we belong to
//-----------------------------------------------------------------------------
void CForwardThrusters::RunFrame() 
{
	SetAccumulatedRotation( m_pShip->GetAccumulatedRotation() );
	SetPosition( m_pShip->GetXPos(), m_pShip->GetYPos() );

	CVectorEntity::RunFrame();
}


//-----------------------------------------------------------------------------
// Purpose: Constructor for reverse thrusters
//-----------------------------------------------------------------------------
CReverseThrusters::CReverseThrusters( IGameEngine *pGameEngine, CShip *pShip ) : CVectorEntity( pGameEngine, 0 )
{
	DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 102 );

	// Initialize our geometry
	AddLine( -8.875, 10.5f, -14.85f, 10.5f, dwColor );
	AddLine( -8.875, 10.5f, -13.765f, 5.61f, dwColor );
	AddLine( -8.875, 10.5f, -7.85f, 3.5f, dwColor );

	AddLine( 8.875, 10.5f, 14.85f, 10.5f, dwColor );
	AddLine( 8.875, 10.5f, 13.765f, 5.61f, dwColor );
	AddLine( 8.875, 10.5f, 7.85f, 3.5f, dwColor );

	m_pShip = pShip;
}


//-----------------------------------------------------------------------------
// Purpose: Run Frame, updates us to be in the same position/rotation as the ship we belong to
//-----------------------------------------------------------------------------
void CReverseThrusters::RunFrame() 
{
	SetAccumulatedRotation( m_pShip->GetAccumulatedRotation() );
	SetPosition( m_pShip->GetXPos(), m_pShip->GetYPos() );

	CVectorEntity::RunFrame();
}


//-----------------------------------------------------------------------------
// Purpose: Constructor for ship debris after explosion
//-----------------------------------------------------------------------------
CShipDebris::CShipDebris( IGameEngine *pGameEngine, float xPos, float yPos, DWORD dwDebrisColor ) : CSpaceWarEntity( pGameEngine, 0, true )
{
	AddLine( 0.0f, 0.0f, 16.0f, 0.0f, dwDebrisColor );

	// Random rotation between 0 and 360 degrees (6.28 radians)
	float flRotation = (float)(rand()%628)/100.0f;
	SetRotationDeltaNextFrame( flRotation );

	// Rotation to apply per second
	int nRandRotation = rand()%(157*2) - (157);
	m_flRotationPerInterval = nRandRotation/100.0f;

	float sinvalue = (float)sin( flRotation );
	float cosvalue = (float)cos( flRotation );

	float xVelocity = GetXVelocity() + ( sinvalue * 80 );
	float yVelocity = GetYVelocity() - ( cosvalue * 80 );

	// Offset out a bit from the center of the ship compensating for rotation
	float offset = (rand()%12)-6.0f;
	float xOffset = xPos + (cosvalue*-offset - sinvalue*-offset);
	float yOffset = yPos + (cosvalue*-offset + sinvalue*-offset);
	
	// Set velocity
	SetVelocity( xVelocity, yVelocity );

	// Set position
	SetPosition( xOffset, yOffset );
}


//-----------------------------------------------------------------------------
// Purpose: Run frame for debris (keep it spinning)
//-----------------------------------------------------------------------------
void CShipDebris::RunFrame()
{
	SetRotationDeltaNextFrame( m_flRotationPerInterval * MIN( m_pGameEngine->GetGameTicksFrameDelta(), 400.0f )/400.0f );
	CSpaceWarEntity::RunFrame();
}


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
#pragma warning( push )
//  warning C4355: 'this' : used in base member initializer list
//  This is OK because the thruster classes won't use the ship object in their constructors (where it may still be only partly constructed)
#pragma warning( disable : 4355 ) 
CShip::CShip( IGameEngine *pGameEngine, bool bIsServerInstance, float xPos, float yPos, DWORD dwShipColor ) : 
	CSpaceWarEntity( pGameEngine, 11, true ), m_ForwardThrusters( pGameEngine, this ), m_ReverseThrusters( pGameEngine, this )
{
	m_bDisabled = false;
	m_bExploding = false;
	m_ulLastThrustStartedTickCount = 0;
	m_dwVKLeft = 0;
	m_dwVKRight = 0;
	m_nFade = 255;
	m_dwVKForwardThrusters = 0;
	m_dwVKReverseThrusters = 0;
	m_dwVKFire = 0;
	m_ulLastPhotonTickCount = 0;
	m_dwShipColor = dwShipColor;
	m_bForwardThrustersActive = false;
	m_bReverseThrustersActive = false;
	m_bIsLocalPlayer = false;
	m_ulLastClientUpdateTick = 0;
	m_bIsServerInstance = bIsServerInstance;
	m_nShipDecoration = 0;
	m_nShipPower = 0;
	m_nShipWeapon = 0;
	m_hTextureWhite = 0;
	m_nShipShieldStrength = 0;
	m_ulExplosionTickCount = 0;
	m_bTriggerEffectEnabled = false;

	memset( &m_SpaceWarClientUpdateData, 0, sizeof( m_SpaceWarClientUpdateData ) );

	for( int i=0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
	{
		m_rgPhotonBeams[i] = NULL;
	}

	BuildGeometry();

	SetPosition( xPos, yPos );

	// Set Controller color to ship color
	m_pGameEngine->SetControllerColor( m_dwShipColor >> 16 & 255, m_dwShipColor >> 8 & 255, m_dwShipColor & 255, k_ESteamControllerLEDFlag_SetColor );
}
#pragma warning( pop )

void CShip::BuildGeometry()
{
	ClearVertexes();

	// Initialize our geometry
	AddLine( -9.0f, 12.0f, 0.0f, -12.0f, m_dwShipColor );
	AddLine( 0.0f, -12.0f, 9.0f, 12.0f, m_dwShipColor );
	AddLine( 9.0f, 12.0f, -9.0f, 12.0f, m_dwShipColor );

	switch ( m_nShipDecoration )
	{
	case 1:
		AddLine( 0.0f, -12.0f, -0.0f, 12.0f, m_dwShipColor );
		AddLine( 4.5f, 0.0f, -4.5f, 0.0f, m_dwShipColor );
		break;
	case 2:
		AddLine( 0.0f, -12.0f, -0.0f, 12.0f, m_dwShipColor );
		AddLine( 4.5f, 0.0f, -4.5f, 0.0f, m_dwShipColor );
		AddLine( 2.5f, -6.0f, -9.0f, 12.0f, m_dwShipColor );
		AddLine( 9.0f, 12.0f, -2.5f, -6.0f, m_dwShipColor );
		break;
	case 3:
		AddLine( 0.0f, -12.0f, 0.0f, 12.0f, m_dwShipColor );
		AddLine( 2.0f, -8.0f, 2.0f, 12.0f, m_dwShipColor );
		AddLine( -2.0f, -8.0f, -2.0f, 12.0f, m_dwShipColor );
		break;
	case 4:
		AddLine( -12.0,  12.0f, -3.0f,-12.0f, m_dwShipColor );
		AddLine( -17.0f,  4.0f,-11.0f,-10.0f, m_dwShipColor );
		AddLine( -17.0f,  4.0f,-10.0f, 7.0f, m_dwShipColor );
		AddLine( -11.0f,-10.0f, -3.0f,-7.0f, m_dwShipColor );
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CShip::~CShip() 
{
	// Cleanup beams
	{
		for( int i=0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
		{
			if ( m_rgPhotonBeams[i] )
				delete m_rgPhotonBeams[i];
		}
	}

	// Cleanup debris
	{
		std::list<CShipDebris *>::iterator iter;
		for ( iter = m_ListDebris.begin(); iter != m_ListDebris.end(); ++iter )
		{
			delete (*iter);
		}
		m_ListDebris.clear();
	}

	// Restore Controller Color
	m_pGameEngine->SetControllerColor( 0, 0, 0, k_ESteamControllerLEDFlag_RestoreUserDefault );

	// Turn off trigger effect
	if ( m_bTriggerEffectEnabled )
	{
		m_pGameEngine->SetTriggerEffect( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Update entity with updated data from the server
//-----------------------------------------------------------------------------
void CShip::OnReceiveServerUpdate( ServerShipUpdateData_t *pUpdateData )
{
	if ( m_bIsServerInstance )
	{
		OutputDebugString( "Should not be receiving server updates on the server itself\n" );
		return;
	}

	SetDisabled( pUpdateData->GetDisabled() );

	SetExploding( pUpdateData->GetExploding() );

	SetPosition( pUpdateData->GetXPosition()*m_pGameEngine->GetViewportWidth(), pUpdateData->GetYPosition()*m_pGameEngine->GetViewportHeight() );
	SetVelocity( pUpdateData->GetXVelocity(), pUpdateData->GetYVelocity() );
	SetAccumulatedRotation( pUpdateData->GetRotation() );

	m_nShipPower = pUpdateData->GetPower();
	m_nShipWeapon = pUpdateData->GetWeapon();
	if ( m_nShipDecoration != pUpdateData->GetDecoration() )
	{
		m_nShipDecoration = pUpdateData->GetDecoration();
		BuildGeometry();
	}
	if ( !m_bIsLocalPlayer || pUpdateData->GetShieldStrength() == 0 )
	{
		m_nShipShieldStrength = pUpdateData->GetShieldStrength();
	}

	m_bForwardThrustersActive = pUpdateData->GetForwardThrustersActive();
	m_bReverseThrustersActive = pUpdateData->GetReverseThrustersActive();

	// Update the photon beams
	for ( int i=0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
	{
		ServerPhotonBeamUpdateData_t *pPhotonUpdate = pUpdateData->AccessPhotonBeamData( i );
		if ( pPhotonUpdate->GetActive() )
		{
			if ( !m_rgPhotonBeams[i] )
			{
				m_rgPhotonBeams[i] = new CPhotonBeam( m_pGameEngine, 
					pPhotonUpdate->GetXPosition(), pPhotonUpdate->GetYPosition(), 
					m_dwShipColor, pPhotonUpdate->GetRotation(), 
					pPhotonUpdate->GetXVelocity(), pPhotonUpdate->GetYVelocity() );
			}
			else
			{
				m_rgPhotonBeams[i]->OnReceiveServerUpdate( pPhotonUpdate );
			}
		}
		else
		{
			if ( m_rgPhotonBeams[i] )
			{
				delete m_rgPhotonBeams[i];
				m_rgPhotonBeams[i] = NULL;
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Update entity with updated data from the client
//-----------------------------------------------------------------------------
void CShip::OnReceiveClientUpdate( ClientSpaceWarUpdateData_t *pUpdateData )
{
	if ( !m_bIsServerInstance )
	{
		OutputDebugString( "Should not be receiving client updates on non-server instances\n" );
		return;
	}

	m_nShipDecoration = pUpdateData->GetDecoration();
	m_nShipPower = pUpdateData->GetPower();
	m_nShipWeapon = pUpdateData->GetWeapon();
	m_nShipShieldStrength = pUpdateData->GetShieldStrength();

	memcpy( &m_SpaceWarClientUpdateData, pUpdateData, sizeof( ClientSpaceWarUpdateData_t ) );
}


//-----------------------------------------------------------------------------
// Purpose: Tell the server about any updates we have had client-side
//-----------------------------------------------------------------------------
bool CShip::BGetClientUpdateData( ClientSpaceWarUpdateData_t *pUpdateData  )
{
	// Limit the rate at which we send updates, even if our internal frame rate is higher
	if ( m_pGameEngine->GetGameTickCount() - m_ulLastClientUpdateTick < 1000.0f/CLIENT_UPDATE_SEND_RATE )
		return false;

	m_ulLastClientUpdateTick = m_pGameEngine->GetGameTickCount();

	// Update playername before sending
	if ( m_bIsLocalPlayer )
	{
		m_SpaceWarClientUpdateData.SetPlayerName( SteamFriends()->GetFriendPersonaName( SteamUser()->GetSteamID() ) );
		m_SpaceWarClientUpdateData.SetDecoration( m_nShipDecoration );
		m_SpaceWarClientUpdateData.SetWeapon( m_nShipWeapon );
		m_SpaceWarClientUpdateData.SetPower( m_nShipPower );
		m_SpaceWarClientUpdateData.SetShieldStrength( m_nShipShieldStrength );
	}

	memcpy( pUpdateData, &m_SpaceWarClientUpdateData, sizeof( ClientSpaceWarUpdateData_t ) );
	memset( &m_SpaceWarClientUpdateData, 0, sizeof( m_SpaceWarClientUpdateData ) );

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Get the name for this ship (only really works server side)
//-----------------------------------------------------------------------------
const char* CShip::GetPlayerName()
{
	return m_SpaceWarClientUpdateData.GetPlayerName();
}


//-----------------------------------------------------------------------------
// Purpose: Run a frame for the ship
//-----------------------------------------------------------------------------
void CShip::RunFrame()
{
	if ( m_bDisabled )
		return;

	const uint64 ulCurrentTickCount = m_pGameEngine->GetGameTickCount();

	// Look for expired photon beams
	int nNextAvailablePhotonBeamSlot = -1;  // Track next available slot for use spawning new beams below
	for( int i=0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
	{
		if ( m_rgPhotonBeams[i] )
		{
			if ( m_rgPhotonBeams[i]->BIsBeamExpired() )
			{
				delete m_rgPhotonBeams[i];
				m_rgPhotonBeams[i] = NULL;
			}
		}

		if ( !m_rgPhotonBeams[i] && nNextAvailablePhotonBeamSlot == -1 )
			nNextAvailablePhotonBeamSlot = i;
	}

	// run all the photon beams we have outstanding
	for( int i=0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
	{
		if ( m_rgPhotonBeams[i] )
			m_rgPhotonBeams[i]->RunFrame();
	}

	// run all the space debris
	{
		std::list<CShipDebris *>::iterator iter;
		for( iter = m_ListDebris.begin(); iter != m_ListDebris.end(); ++iter )
			(*iter)->RunFrame();
	}

	if ( m_bIsLocalPlayer )
	{
		m_SpaceWarClientUpdateData.SetTurnLeftPressed( false );
		m_SpaceWarClientUpdateData.SetTurnRightPressed( false );

		if ( m_pGameEngine->BIsKeyDown( m_dwVKLeft )
			|| m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_TurnLeft ) )
		{
			m_SpaceWarClientUpdateData.SetTurnLeftPressed( true );
		}
		
		if ( m_pGameEngine->BIsKeyDown( m_dwVKRight ) 
			|| m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_TurnRight ) )
		{
			m_SpaceWarClientUpdateData.SetTurnRightPressed( true );
		}

		// The Steam Controller can also map an anlog axis to thrust and steer
		float fTurnSpeed, fUnused;
		m_pGameEngine->GetControllerAnalogAction( eControllerAnalogAction_AnalogControls, &fTurnSpeed, &fUnused );

		if ( fTurnSpeed > 0.0f )
		{
			m_SpaceWarClientUpdateData.SetTurnRightPressed( true );
			m_SpaceWarClientUpdateData.SetTurnSpeed( fTurnSpeed );
		}
		else if ( fTurnSpeed < 0.0f )
		{
			m_SpaceWarClientUpdateData.SetTurnLeftPressed( true );
			m_SpaceWarClientUpdateData.SetTurnSpeed( fTurnSpeed );
		}
	}
	else if ( m_bIsServerInstance )
	{
		// Server side
		const float fMaxTurnSpeed = (PI_VALUE / 2.0f) * (float)m_pGameEngine->GetGameTicksFrameDelta( ) / 400.0f;

		float flRotationDelta = 0.0f;
		float fTurnSpeed = m_SpaceWarClientUpdateData.GetTurnSpeed();
		if ( fTurnSpeed != 0.0f )
		{
			flRotationDelta += fMaxTurnSpeed * fTurnSpeed;
		}
		else
		{
			if ( m_SpaceWarClientUpdateData.GetTurnLeftPressed( ) )
			{
				flRotationDelta += -1.0f * fMaxTurnSpeed;
			}

			if ( m_SpaceWarClientUpdateData.GetTurnRightPressed( ) )
			{
				flRotationDelta += fMaxTurnSpeed;
			}
		}

		SetRotationDeltaNextFrame( flRotationDelta );
	}
	
	// Compute acceleration
	if ( m_bIsLocalPlayer )
	{
		// client side
		m_SpaceWarClientUpdateData.SetReverseThrustersPressed( false );
		m_SpaceWarClientUpdateData.SetForwardThrustersPressed( false );
		
		bool bForwardThrustActive = false;
		if ( m_pGameEngine->BIsKeyDown( m_dwVKForwardThrusters ) ||
			m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_ForwardThrust ) )
		{
			m_SpaceWarClientUpdateData.SetForwardThrustersPressed( true );
			bForwardThrustActive = true;
			//m_pGameEngine->SetControllerColor( 100, 255, 0, k_ESteamControllerLEDFlag_SetColor );
		}

		if ( m_pGameEngine->BIsKeyDown( m_dwVKReverseThrusters ) ||
			m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_ReverseThrust ) )
		{
			m_SpaceWarClientUpdateData.SetReverseThrustersPressed( true );
		}


		// The Steam Controller can also map an analog axis to thrust and steer
		float fThrusterLevel, fUnused;
		m_pGameEngine->GetControllerAnalogAction( eControllerAnalogAction_AnalogControls, &fUnused, &fThrusterLevel );

		if ( fThrusterLevel > 0.0f )
		{
			m_SpaceWarClientUpdateData.SetForwardThrustersPressed( true );
			m_SpaceWarClientUpdateData.SetThrustersLevel( fThrusterLevel );
			bForwardThrustActive = true;
		}
		else if ( fThrusterLevel < 0.0f )
		{
			m_SpaceWarClientUpdateData.SetReverseThrustersPressed( true );
			m_SpaceWarClientUpdateData.SetThrustersLevel( fThrusterLevel );
		}

		// We can activate action set layers based upon our state.  
		// This allows action bindings or settings to be changed on an existing action set for contextual usage
		if ( bForwardThrustActive )
		{
			m_pGameEngine->ActivateSteamControllerActionSetLayer( eControllerActionSet_Layer_Thrust );
		}
		else if ( m_pGameEngine->BIsActionSetLayerActive( eControllerActionSet_Layer_Thrust ) )
		{
			m_pGameEngine->DeactivateSteamControllerActionSetLayer( eControllerActionSet_Layer_Thrust );
		}

		// Hardcoded keys to choose various outfits and weapon powerups which require inventory. Note that this is not
		// a "secure" multiplayer model - clients can lie about what they own. A more robust solution, if your items
		// matter enough to bother, would be to use SerializeResult / DeserializeResult to encode the fact that your
		// steamid owns certain items, and then send that encoded result to the server which decodes and verifies it.
		if ( m_pGameEngine->BIsKeyDown( 0x30 ) )
		{
			m_nShipDecoration = 0;
			BuildGeometry();
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x31 ) && SpaceWarLocalInventory()->HasInstanceOf( k_SpaceWarItem_ShipDecoration1 ) )
		{
			m_nShipDecoration = 1;
			BuildGeometry();
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x32 ) && SpaceWarLocalInventory()->HasInstanceOf( k_SpaceWarItem_ShipDecoration2 ) )
		{
			m_nShipDecoration = 2;
			BuildGeometry();
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x33 ) && SpaceWarLocalInventory()->HasInstanceOf( k_SpaceWarItem_ShipDecoration3 ) )
		{
			m_nShipDecoration = 3;
			BuildGeometry();
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x34 ) && SpaceWarLocalInventory()->HasInstanceOf( k_SpaceWarItem_ShipDecoration4 ) )
		{
			m_nShipDecoration = 4;
			BuildGeometry();
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x35 ) && SpaceWarLocalInventory()->HasInstanceOf( k_SpaceWarItem_ShipWeapon1 ) )
		{
			m_nShipWeapon = 1;
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x36 ) && SpaceWarLocalInventory()->HasInstanceOf( k_SpaceWarItem_ShipWeapon2 ) )
		{
			m_nShipWeapon = 2;
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x37 ) && SpaceWarLocalInventory()->HasInstanceOf( k_SpaceWarItem_ShipSpecial1 ) )
		{
			m_nShipPower = 1;
		}
		else if ( m_pGameEngine->BIsKeyDown( 0x38 ) && SpaceWarLocalInventory()->HasInstanceOf( k_SpaceWarItem_ShipSpecial2 ) )
		{
			m_nShipPower = 2;
		}
	}
	else if ( m_bIsServerInstance )
	{
		// Server side
		float xThrust = 0;
		float yThrust = 0;
		m_bReverseThrustersActive = false;
		m_bForwardThrustersActive = false;
		if ( m_SpaceWarClientUpdateData.GetReverseThrustersPressed() || m_SpaceWarClientUpdateData.GetForwardThrustersPressed() )
		{
			float flSign = 1.0f;
			if ( m_SpaceWarClientUpdateData.GetReverseThrustersPressed() )
			{
				m_bReverseThrustersActive = true;
				flSign = -1.0f;
			}
			else
			{
				m_bForwardThrustersActive = true;
			}

			float fThrusterLevel = m_SpaceWarClientUpdateData.GetThrustersLevel();
			if ( fThrusterLevel != 0.0f )
			{
				flSign = fThrusterLevel;
			}

			if ( m_ulLastThrustStartedTickCount == 0 )
			{
				m_ulLastThrustStartedTickCount = ulCurrentTickCount;
				m_pGameEngine->TriggerControllerHaptics( k_ESteamControllerPad_Left, 2900, 1200, 4 );
			}

			// You have to hold the key for a second to reach maximum thrust
			float factor = MIN( ((float)(ulCurrentTickCount - m_ulLastThrustStartedTickCount) / 500.0f) + 0.2f, 1.0f );

			xThrust = flSign * (float)(MAXIMUM_SHIP_THRUST * factor * sin( GetAccumulatedRotation() ) );
			yThrust = flSign * -1.0f * (float)(MAXIMUM_SHIP_THRUST * factor * cos( GetAccumulatedRotation() ) );
		}
		else
		{
			m_ulLastThrustStartedTickCount = 0;
		}

		SetAcceleration( xThrust, yThrust );
	}


	// We'll use these values in a few places below to compute positions of child objects
	// appropriately given our rotation
	float sinvalue = (float)sin( GetAccumulatedRotation() );
	float cosvalue = (float)cos( GetAccumulatedRotation() );

	if ( m_bIsLocalPlayer )
	{
		// client side
		if ( m_pGameEngine->BIsKeyDown( m_dwVKFire ) ||
			m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_FireLasers ) )
		{
			m_SpaceWarClientUpdateData.SetFirePressed( true );
		}
	}
	else if ( m_bIsServerInstance )
	{
		// server side
		if ( nNextAvailablePhotonBeamSlot != -1 && !m_bExploding && m_SpaceWarClientUpdateData.GetFirePressed() && ulCurrentTickCount - PHOTON_BEAM_FIRE_INTERVAL_TICKS > m_ulLastPhotonTickCount )
		{
			m_ulLastPhotonTickCount = ulCurrentTickCount;

			if ( m_nShipWeapon == 1 ) // Item#101
			{
				float sinvalue1 = (float)sin( GetAccumulatedRotation() - .1f );
				float cosvalue1 = (float)cos( GetAccumulatedRotation() - .1f );
				float sinvalue2 = (float)sin( GetAccumulatedRotation() + .1f );
				float cosvalue2 = (float)cos( GetAccumulatedRotation() + .1f );

				float xVelocity = GetXVelocity() + ( sinvalue1 * 275 );
				float yVelocity = GetYVelocity() - ( cosvalue1 * 275 );

				// Offset 12 points up from the center of the ship, compensating for rotation
				float xPos = GetXPos() - sinvalue1*-12.0f;
				float yPos = GetYPos() + cosvalue1*-12.0f;

				m_rgPhotonBeams[nNextAvailablePhotonBeamSlot] = new CPhotonBeam( m_pGameEngine, xPos, yPos, m_dwShipColor, GetAccumulatedRotation(), xVelocity, yVelocity );

				nNextAvailablePhotonBeamSlot = -1;  // Track next available slot for use spawning new beams below
				for( int i=0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
				{
					if ( !m_rgPhotonBeams[i] && nNextAvailablePhotonBeamSlot == -1 )
						nNextAvailablePhotonBeamSlot = i;
				}

				if ( nNextAvailablePhotonBeamSlot != -1 )
				{
					xVelocity = GetXVelocity() + ( sinvalue2 * 275 );
					yVelocity = GetYVelocity() - ( cosvalue2 * 275 );

					// Offset 12 points up from the center of the ship, compensating for rotation
					xPos = GetXPos() - sinvalue2*-12.0f;
					yPos = GetYPos() + cosvalue2*-12.0f;

					m_rgPhotonBeams[nNextAvailablePhotonBeamSlot] = new CPhotonBeam( m_pGameEngine, xPos, yPos, m_dwShipColor, GetAccumulatedRotation(), xVelocity, yVelocity );
					m_pGameEngine->TriggerControllerHaptics( k_ESteamControllerPad_Right, 1000, 1500, 2 );
				}
			}
			else
			{
				float speed = 275;
				if ( m_nShipWeapon == 2 ) // Item#102
				{
					speed = 500;
				}
				float xVelocity = GetXVelocity() + ( sinvalue * speed );
				float yVelocity = GetYVelocity() - ( cosvalue * speed );

				// Offset 12 points up from the center of the ship, compensating for rotation
				float xPos = GetXPos() - sinvalue*-12.0f;
				float yPos = GetYPos() + cosvalue*-12.0f;

				m_rgPhotonBeams[nNextAvailablePhotonBeamSlot] = new CPhotonBeam( m_pGameEngine, xPos, yPos, m_dwShipColor, GetAccumulatedRotation(), xVelocity, yVelocity );
				m_pGameEngine->TriggerControllerHaptics( k_ESteamControllerPad_Right, 1200, 2500, 3 );
			}
		}
	}

	CSpaceWarEntity::RunFrame();

	// Finally, update the thrusters ( we do this after the base class call as they rely on our data being fully up-to-date)
	m_ForwardThrusters.RunFrame();
	m_ReverseThrusters.RunFrame();
}


//-----------------------------------------------------------------------------
// Purpose: Render the ship
//-----------------------------------------------------------------------------
void CShip::Render()
{
	int beamCount = 0;

	if ( m_bDisabled )
		return;

	// render all the photon beams we have outstanding
	for ( int i = 0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
	{
		if ( m_rgPhotonBeams[i] )
		{
			m_rgPhotonBeams[i]->Render();
			beamCount++;
		}
	}


	if ( m_bExploding )
	{
		// Don't draw actual ship, instead draw the pieces created in the explosion
		std::list<CShipDebris *>::iterator iter;
		for ( iter = m_ListDebris.begin(); iter != m_ListDebris.end(); ++iter )
			( *iter )->Render();
		return;
	}

	// Check if we should be drawing thrusters
	if ( m_bForwardThrustersActive )
	{
		if ( rand() % 3 == 0 )
			m_ForwardThrusters.Render();
	}

	if ( m_bReverseThrustersActive )
	{
		if ( rand() % 3 == 0 )
			m_ReverseThrusters.Render();
	}

	DWORD actualColor = m_dwShipColor;

	if ( m_nShipPower == 1 ) // Item#103 but need to check if the other guy has it sometimes?
	{
		if ( beamCount > 0 )
		{
			m_nFade = 255;
		}
		else if ( m_nFade > 0 )
		{
			m_nFade -= 5;
			if ( m_nFade < 0 )
				m_nFade = 0;
			if ( m_bIsLocalPlayer && m_nFade < 50 )
			{
				m_nFade = 128;
			}
		}
		actualColor = (actualColor & 0xffffff) | (m_nFade<<24);
	}

	DWORD shieldColor = 0x00af8f00;
	
	if ( m_nShipPower == 2 )
	{
		shieldColor = shieldColor | ((m_nShipShieldStrength / 4)<<24);
		if ( m_nShipShieldStrength < 256 )
			m_nShipShieldStrength++;

		if ( !m_hTextureWhite )
		{
			byte *pRGBAData = new byte[1 * 1 * 4];
			memset( pRGBAData, 255, 1 * 1 * 4 );
			m_hTextureWhite = m_pGameEngine->HCreateTexture( pRGBAData, 1, 1 );
			delete[] pRGBAData;
		}

		float rotationClockwise = (m_pGameEngine->GetGameTickCount() / 500.0f);
		float rotationCounter = -(m_pGameEngine->GetGameTickCount() / 500.0f);

		float x1 = 28.0f * (float)cos( rotationClockwise );
		float y1 = 28.0f * (float)sin( rotationClockwise );
		float x2 = 28.0f * (float)cos( rotationCounter );
		float y2 = 28.0f * (float)sin( rotationCounter );

		m_pGameEngine->BDrawTexturedQuad(
			this->GetXPos() - x1, this->GetYPos() - y1, this->GetXPos() + y1, this->GetYPos() - x1,
			this->GetXPos() - y1, this->GetYPos() + x1, this->GetXPos() + x1, this->GetYPos() + y1, 
			0, 0, 1, 1, shieldColor, m_hTextureWhite );

		m_pGameEngine->BDrawTexturedQuad(
			this->GetXPos() - x2, this->GetYPos() - y2, this->GetXPos() + y2, this->GetYPos() - x2,
			this->GetXPos() - y2, this->GetYPos() + x2, this->GetXPos() + x2, this->GetYPos() + y2, 
			0, 0, 1, 1, shieldColor, m_hTextureWhite );
	}
	else
	{
		m_nShipShieldStrength = 0;
	}

	CSpaceWarEntity::Render(actualColor);
}

void CShip::UpdateVibrationEffects()
{
	if ( m_ulExplosionTickCount > 0 )
	{
		float flVibration = MIN( ((float)(m_pGameEngine->GetGameTickCount() - m_ulExplosionTickCount) / 1000.0f), 1.0f );
		if ( flVibration == 1.0f )
		{
			m_pGameEngine->TriggerControllerVibration( 0, 0 );
			m_ulExplosionTickCount = 0;
		}
		else
		{
			m_pGameEngine->TriggerControllerVibration( (unsigned short)( ( 1.0f - flVibration ) * 48000.0f), (unsigned short)( ( 1.0f - flVibration ) * 24000.0f) );
		}
	}

	bool bTriggerEffectEnabled = !BIsDisabled() && !BIsExploding();
	if ( bTriggerEffectEnabled != m_bTriggerEffectEnabled )
	{
		m_pGameEngine->SetTriggerEffect( bTriggerEffectEnabled );
		m_bTriggerEffectEnabled = bTriggerEffectEnabled;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set whether the ship is exploding
//-----------------------------------------------------------------------------
void CShip::SetExploding( bool bExploding )
{
	// If we are already in the specified state, no need to do the below work
	if ( m_bExploding == bExploding )
	{
		UpdateVibrationEffects();
		return;
	}
	
	Steamworks_TestSecret();

	// Track that we are exploding, and disable collision detection
	m_bExploding = bExploding;
	SetCollisionDetectionDisabled( m_bExploding );

	if ( bExploding )
	{
		m_ulExplosionTickCount = m_pGameEngine->GetGameTickCount();

		for( int i = 0; i < SHIP_DEBRIS_PIECES; ++i )
		{
			CShipDebris * pDebris = new CShipDebris( m_pGameEngine, GetXPos(), GetYPos(), m_dwShipColor );
			m_ListDebris.push_back( pDebris );
		}
	}
	else
	{
		m_ulExplosionTickCount = 0;

		std::list<CShipDebris *>::iterator iter;
		for( iter = m_ListDebris.begin(); iter != m_ListDebris.end(); ++iter )
			delete *iter;
		m_ListDebris.clear();
	}

	UpdateVibrationEffects();
}


//-----------------------------------------------------------------------------
// Purpose: Check for photons which have hit the target and remove them
//-----------------------------------------------------------------------------
void CShip::DestroyPhotonsColldingWith( CVectorEntity *pTarget )
{
	for( int i=0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
	{
		if ( !m_rgPhotonBeams[i] )
			continue;

		if ( m_rgPhotonBeams[i]->BCollidesWith( pTarget ) )
		{
			// Photon beam hit the entity, destroy beam
			delete m_rgPhotonBeams[i];
			m_rgPhotonBeams[i] = NULL;
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Check whether any of the photons this ship has fired are colliding with the target 
//-----------------------------------------------------------------------------
bool CShip::BCheckForPhotonsCollidingWith( CVectorEntity *pTarget )
{
	for( int i=0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
	{
		if ( !m_rgPhotonBeams[i] )
			continue;

		if ( m_rgPhotonBeams[i]->BCollidesWith( pTarget ) )
		{
			return true;
		}
	}

	return false;
}



//-----------------------------------------------------------------------------
// Purpose: Build the update data to send from server to clients
//-----------------------------------------------------------------------------
void CShip::BuildServerUpdate( ServerShipUpdateData_t *pUpdateData )
{
	pUpdateData->SetDisabled( BIsDisabled() );
	pUpdateData->SetExploding( BIsExploding() );
	pUpdateData->SetXAcceleration( GetXAccelerationLastFrame() );
	pUpdateData->SetYAcceleration( GetYAccelerationLastFrame() );
	pUpdateData->SetXPosition( GetXPos()/(float)m_pGameEngine->GetViewportWidth() );
	pUpdateData->SetYPosition( GetYPos()/(float)m_pGameEngine->GetViewportHeight() );
	pUpdateData->SetXVelocity( GetXVelocity() );
	pUpdateData->SetYVelocity( GetYVelocity() );
	pUpdateData->SetRotation( GetAccumulatedRotation() );
	pUpdateData->SetRotationDeltaLastFrame( GetRotationDeltaLastFrame() );
	pUpdateData->SetForwardThrustersActive( m_bForwardThrustersActive );
	pUpdateData->SetReverseThrustersActive( m_bReverseThrustersActive );
	pUpdateData->SetDecoration( m_nShipDecoration );
	pUpdateData->SetWeapon( m_nShipWeapon );
	pUpdateData->SetPower( m_nShipPower );
	pUpdateData->SetShieldStrength( m_nShipShieldStrength );

	BuildServerPhotonBeamUpdate( pUpdateData );
}

//-----------------------------------------------------------------------------
// Purpose: Build the photon beam update data to send from the server to clients
//-----------------------------------------------------------------------------
void CShip::BuildServerPhotonBeamUpdate( ServerShipUpdateData_t *pUpdateData )
{
	for( int i = 0; i < MAX_PHOTON_BEAMS_PER_SHIP; ++i )
	{
		ServerPhotonBeamUpdateData_t *pPhotonUpdate = pUpdateData->AccessPhotonBeamData( i );
		if ( m_rgPhotonBeams[i] )
		{
			pPhotonUpdate->SetActive( true );
			pPhotonUpdate->SetXPosition( m_rgPhotonBeams[i]->GetXPos()/(float)m_pGameEngine->GetViewportWidth() );
			pPhotonUpdate->SetYPosition( m_rgPhotonBeams[i]->GetYPos()/(float)m_pGameEngine->GetViewportHeight() );
			pPhotonUpdate->SetXVelocity( m_rgPhotonBeams[i]->GetXVelocity() );
			pPhotonUpdate->SetYVelocity( m_rgPhotonBeams[i]->GetYVelocity() );
			pPhotonUpdate->SetRotation( m_rgPhotonBeams[i]->GetAccumulatedRotation() ); 
		}
		else
		{
			pPhotonUpdate->SetActive( false );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Accumulate stats for this ship
//-----------------------------------------------------------------------------
void CShip::AccumulateStats( CStatsAndAchievements *pStats )
{
	if ( m_bIsLocalPlayer )
	{
		pStats->AddDistanceTraveled( GetDistanceTraveledLastFrame() );
	}
}
