//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Shared definitions for the communication between the server/client
//
// $NoKeywords: $
//=============================================================================

#ifndef SPACEWAR_H
#define SPACEWAR_H


// The Steamworks API's are modular, you can use some subsystems without using others
// When USE_GS_AUTH_API is defined you get the following Steam features:
// - Strong user authentication and authorization
// - Game server matchmaking
// - VAC cheat protection
// - Access to achievement/community API's
// - P2P networking capability

// Remove this define to disable using the native Steam authentication and matchmaking system
// You can use this as a sample of how to integrate your game without replacing an existing matchmaking system
// When you un-define USE_GS_AUTH_API you get:
// - Access to achievement/community API's
// - P2P networking capability
// You CANNOT use:
// - VAC cheat protection
// - Game server matchmaking
// as these function depend on using Steam authentication
#define USE_GS_AUTH_API 


// Current game server version
#define SPACEWAR_SERVER_VERSION "1.0.0.0"

// UDP port for the spacewar server to listen on
#define SPACEWAR_SERVER_PORT 27015

// UDP port for the master server updater to listen on
#define SPACEWAR_MASTER_SERVER_UPDATER_PORT 27016

// How long to wait for a response from the server before resending our connection attempt
#define SERVER_CONNECTION_RETRY_MILLISECONDS 350

// How long to wait for a client to send an update before we drop its connection server side
#define SERVER_TIMEOUT_MILLISECONDS 5000

// Maximum packet size in bytes
#define MAX_SPACEWAR_PACKET_SIZE 1024*512

// Maximum number of players who can join a server and play simultaneously
#define MAX_PLAYERS_PER_SERVER 4

// Time to pause wait after a round ends before starting a new one
#define MILLISECONDS_BETWEEN_ROUNDS 4000

// How long photon beams live before expiring
#define PHOTON_BEAM_LIFETIME_IN_TICKS 1750

// How fast can photon beams be fired?
#define PHOTON_BEAM_FIRE_INTERVAL_TICKS 250

// Amount of space needed for beams per ship
#define MAX_PHOTON_BEAMS_PER_SHIP (PHOTON_BEAM_LIFETIME_IN_TICKS/PHOTON_BEAM_FIRE_INTERVAL_TICKS)

// Time to timeout a connection attempt in
#define MILLISECONDS_CONNECTION_TIMEOUT 30000

// How many times a second does the server send world updates to clients
#define SERVER_UPDATE_SEND_RATE 60

// How many times a second do we send our updated client state to the server
#define CLIENT_UPDATE_SEND_RATE 30

// How fast does the server internally run at?
#define MAX_CLIENT_AND_SERVER_FPS 86


template <typename T>
inline T WordSwap( T w )
{
	uint16 temp;

	temp  = ((*((uint16 *)&w) & 0xff00) >> 8);
	temp |= ((*((uint16 *)&w) & 0x00ff) << 8);

	return *((T*)&temp);
}

template <typename T>
inline T DWordSwap( T dw )
{
	uint32 temp;

	temp  =   *((uint32 *)&dw)               >> 24;
	temp |= ((*((uint32 *)&dw) & 0x00FF0000) >> 8);
	temp |= ((*((uint32 *)&dw) & 0x0000FF00) << 8);
	temp |= ((*((uint32 *)&dw) & 0x000000FF) << 24);

	return *((T*)&temp);
}

template <typename T>
inline T QWordSwap( T dw )
{
	uint64 temp;

	temp  =   *((uint64 *)&dw)                          >> 56;
	temp |= ((*((uint64 *)&dw) & 0x00FF000000000000ull) >> 40);
	temp |= ((*((uint64 *)&dw) & 0x0000FF0000000000ull) >> 24);
	temp |= ((*((uint64 *)&dw) & 0x000000FF00000000ull) >> 8);
	temp |= ((*((uint64 *)&dw) & 0x00000000FF000000ull) << 8);
	temp |= ((*((uint64 *)&dw) & 0x0000000000FF0000ull) << 24);
	temp |= ((*((uint64 *)&dw) & 0x000000000000FF00ull) << 40);
	temp |= ((*((uint64 *)&dw) & 0x00000000000000FFull) << 56);

	return *((T*)&temp);
}

#define LittleInt16( val )	( val )
#define LittleWord( val )	( val )
#define LittleInt32( val )	( val )
#define LittleDWord( val )	( val )
#define LittleQWord( val )	( val )
#define LittleFloat( val )	( val )

// Leaderboard names
#define LEADERBOARD_QUICKEST_WIN "Quickest Win"
#define LEADERBOARD_FEET_TRAVELED "Feet Traveled"


// Player colors
DWORD const g_rgPlayerColors[ MAX_PLAYERS_PER_SERVER ] = 
{ 
	D3DCOLOR_ARGB( 255, 255, 150, 150 ), // red 
	D3DCOLOR_ARGB( 255, 200, 200, 255 ), // blue
	D3DCOLOR_ARGB( 255, 255, 204, 102 ), // orange
	D3DCOLOR_ARGB( 255, 153, 255, 153 ), // green
};


// Enum for possible game states on the client
enum EClientGameState
{
	k_EClientGameStartServer,
	k_EClientGameActive,
	k_EClientGameWaitingForPlayers,
	k_EClientGameMenu,
	k_EClientGameQuitMenu,
	k_EClientGameExiting,
	k_EClientGameInstructions,
	k_EClientGameDraw,
	k_EClientGameWinner,
	k_EClientGameConnecting,
	k_EClientGameConnectionFailure,
	k_EClientFindInternetServers,
	k_EClientStatsAchievements,
	k_EClientCreatingLobby,
	k_EClientInLobby,
	k_EClientFindLobby,
	k_EClientJoiningLobby,
	k_EClientFindLANServers,
	k_EClientRemoteStorage,
	k_EClientLeaderboards,
	k_EClientFriendsList,
	k_EClientMinidump,
	k_EClientClanChatRoom,
	k_EClientWebCallback,
	k_EClientMusic,
	k_EClientWorkshop,
	k_EClientHTMLSurface,
	k_EClientInGameStore,
	k_EClientRemotePlayInvite,
	k_EClientRemotePlaySessions,
	k_EClientOverlayAPI,
};


// Enum for possible game states on the server
enum EServerGameState
{
	k_EServerWaitingForPlayers,
	k_EServerActive,
	k_EServerDraw,
	k_EServerWinner,
	k_EServerExiting,
};

#pragma pack( push, 1 )

// Data sent per photon beam from the server to update clients photon beam positions
struct ServerPhotonBeamUpdateData_t
{
	void SetActive( bool bIsActive ) { m_bIsActive = bIsActive; }
	bool GetActive() { return m_bIsActive; }

	void SetRotation( float flRotation ) { m_flCurrentRotation = LittleFloat( flRotation ); }
	float GetRotation() { return LittleFloat( m_flCurrentRotation ); }

	void SetXVelocity( float flVelocity ) { m_flXVelocity = LittleFloat( flVelocity ); }
	float GetXVelocity() { return LittleFloat( m_flXVelocity ); }

	void SetYVelocity( float flVelocity ) { m_flYVelocity = LittleFloat( flVelocity ); }
	float GetYVelocity() { return LittleFloat( m_flYVelocity ); }

	void SetXPosition( float flPosition ) { m_flXPosition = LittleFloat( flPosition ); }
	float GetXPosition() { return LittleFloat( m_flXPosition ); }

	void SetYPosition( float flPosition ) { m_flYPosition = LittleFloat( flPosition ); }
	float GetYPosition() { return LittleFloat( m_flYPosition ); }


private:
	// Does the photon beam exist right now?
	bool m_bIsActive; 

	// The current rotation 
	float m_flCurrentRotation;

	// The current velocity
	float m_flXVelocity;
	float m_flYVelocity;

	// The current position
	float m_flXPosition;
	float m_flYPosition;
};


// This is the data that gets sent per ship in each update, see below for the full update data
struct ServerShipUpdateData_t
{
	void SetRotation( float flRotation ) { m_flCurrentRotation = LittleFloat( flRotation ); }
	float GetRotation() { return LittleFloat( m_flCurrentRotation ); }

	void SetRotationDeltaLastFrame( float flDelta ) { m_flRotationDeltaLastFrame = LittleFloat( flDelta ); }
	float GetRotationDeltaLastFrame() { return LittleFloat( m_flRotationDeltaLastFrame ); }

	void SetXAcceleration( float flAcceleration ) { m_flXAcceleration = LittleFloat( flAcceleration ); }
	float GetXAcceleration() { return LittleFloat( m_flXAcceleration ); }

	void SetYAcceleration( float flAcceleration ) { m_flYAcceleration = LittleFloat( flAcceleration ); }
	float GetYAcceleration() { return LittleFloat( m_flYAcceleration ); }

	void SetXVelocity( float flVelocity ) { m_flXVelocity = LittleFloat( flVelocity ); }
	float GetXVelocity() { return LittleFloat( m_flXVelocity ); }

	void SetYVelocity( float flVelocity ) { m_flYVelocity = LittleFloat( flVelocity ); }
	float GetYVelocity() { return LittleFloat( m_flYVelocity ); }

	void SetXPosition( float flPosition ) { m_flXPosition = LittleFloat( flPosition ); }
	float GetXPosition() { return LittleFloat( m_flXPosition ); }

	void SetYPosition( float flPosition ) { m_flYPosition = LittleFloat( flPosition ); }
	float GetYPosition() { return LittleFloat( m_flYPosition ); }

	void SetExploding( bool bIsExploding ) { m_bExploding = bIsExploding; }
	bool GetExploding() { return m_bExploding; }

	void SetDisabled( bool bIsDisabled ) { m_bDisabled = bIsDisabled; }
	bool GetDisabled() { return m_bDisabled; }

	void SetForwardThrustersActive( bool bActive ) { m_bForwardThrustersActive = bActive; }
	bool GetForwardThrustersActive() { return m_bForwardThrustersActive; }

	void SetReverseThrustersActive( bool bActive ) { m_bReverseThrustersActive = bActive; }
	bool GetReverseThrustersActive() { return m_bReverseThrustersActive; }

	void SetDecoration( int nDecoration ) { m_nShipDecoration = nDecoration;  }
	int GetDecoration() { return m_nShipDecoration; }

	void SetWeapon( int nWeapon ) { m_nShipWeapon = nWeapon;  }
	int GetWeapon() { return m_nShipWeapon; }

	void SetPower( int nPower ) { m_nShipPower = nPower;  }
	int GetPower() { return m_nShipPower; }

	void SetShieldStrength( int nShieldStrength ) { m_nShieldStrength = nShieldStrength;  }
	int GetShieldStrength() { return m_nShieldStrength; }

	void SetThrustersLevel( float fLevel ) { m_fThrusterLevel = fLevel; }
	float GetThrustersLevel( ) { return m_fThrusterLevel; }

	void SetTurnSpeed( float fSpeed ) { m_fTurnSpeed = fSpeed; }
	float GetTurnSpeed( ) { return m_fTurnSpeed; }

	ServerPhotonBeamUpdateData_t *AccessPhotonBeamData( int iIndex ) { return &m_PhotonBeamData[iIndex]; }

private:
	// The current rotation of the ship
	float m_flCurrentRotation;

	// The delta in rotation for the last frame (client side interpolation will use this)
	float m_flRotationDeltaLastFrame;

	// The current thrust for the ship
	float m_flXAcceleration;
	float m_flYAcceleration;

	// The current velocity for the ship
	float m_flXVelocity;
	float m_flYVelocity;

	// The current position for the ship
	float m_flXPosition;
	float m_flYPosition;

	// Is the ship exploding?
	bool m_bExploding;

	// Is the ship disabled?
	bool m_bDisabled;

	// Are the thrusters to be drawn?
	bool m_bForwardThrustersActive;
	bool m_bReverseThrustersActive;

	// Decoration for this ship
	int m_nShipDecoration;

	// Weapon for this ship
	int m_nShipWeapon;

	// Power for this ship
	int m_nShipPower;
	int m_nShieldStrength;

	// Photon beam positions and data
	ServerPhotonBeamUpdateData_t m_PhotonBeamData[MAX_PHOTON_BEAMS_PER_SHIP];

	// Thrust and rotation speed can be anlog when using a Steam Controller
	float m_fThrusterLevel;
	float m_fTurnSpeed;
};


// This is the data that gets sent from the server to each client for each update
struct ServerSpaceWarUpdateData_t
{
	void SetServerGameState( EServerGameState eState ) { m_eCurrentGameState = LittleDWord( (uint32)eState ); }
	EServerGameState GetServerGameState() { return (EServerGameState)LittleDWord( m_eCurrentGameState ); }

	void SetPlayerWhoWon( uint32 iIndex ) { m_uPlayerWhoWonGame = LittleDWord( iIndex ); }
	uint32 GetPlayerWhoWon() { return LittleDWord( m_uPlayerWhoWonGame ); }

	void SetPlayerActive( uint32 iIndex, bool bIsActive ) { m_rgPlayersActive[iIndex] = bIsActive; }
	bool GetPlayerActive( uint32 iIndex ) { return m_rgPlayersActive[iIndex]; }

	void SetPlayerScore( uint32 iIndex, uint32 unScore ) { m_rgPlayerScores[iIndex] = LittleDWord(unScore); }
	uint32 GetPlayerScore( uint32 iIndex ) { return LittleDWord(m_rgPlayerScores[iIndex]); }

	void SetPlayerSteamID( uint32 iIndex, uint64 ulSteamID ) { m_rgPlayerSteamIDs[iIndex] = LittleQWord(ulSteamID); }
	uint64 GetPlayerSteamID( uint32 iIndex ) { return LittleQWord(m_rgPlayerSteamIDs[iIndex]); }

	ServerShipUpdateData_t *AccessShipUpdateData( uint32 iIndex ) { return &m_rgShipData[iIndex];}

private:
	// What state the game is in
	uint32 m_eCurrentGameState;

	// Who just won the game? -- only valid when m_eCurrentGameState == k_EGameWinner
	uint32 m_uPlayerWhoWonGame;

	// which player slots are in use
	bool m_rgPlayersActive[MAX_PLAYERS_PER_SERVER];

	// what are the scores for each player?
	uint32 m_rgPlayerScores[MAX_PLAYERS_PER_SERVER];

	// array of ship data
	ServerShipUpdateData_t m_rgShipData[MAX_PLAYERS_PER_SERVER];

	// array of players steamids for each slot, serialized to uint64
	uint64 m_rgPlayerSteamIDs[MAX_PLAYERS_PER_SERVER];
};


// This is the data that gets sent from each client to the server for each update
struct ClientSpaceWarUpdateData_t
{
	void SetPlayerName( const char *pchName ) { strncpy_safe( m_rgchPlayerName, pchName, sizeof( m_rgchPlayerName ) ); }
	const char *GetPlayerName() { return m_rgchPlayerName; }

	void SetFirePressed( bool bIsPressed ) { m_bFirePressed = bIsPressed; }
	bool GetFirePressed() { return m_bFirePressed; }

	void SetTurnLeftPressed( bool bIsPressed ) { m_bTurnLeftPressed = bIsPressed; }
	bool GetTurnLeftPressed() { return m_bTurnLeftPressed; }

	void SetTurnRightPressed( bool bIsPressed ) { m_bTurnRightPressed = bIsPressed; }
	bool GetTurnRightPressed() { return m_bTurnRightPressed; }

	void SetForwardThrustersPressed( bool bIsPressed ) { m_bForwardThrustersPressed = bIsPressed; }
	bool GetForwardThrustersPressed() { return m_bForwardThrustersPressed; }

	void SetReverseThrustersPressed( bool bIsPressed ) { m_bReverseThrustersPressed = bIsPressed; }
	bool GetReverseThrustersPressed() { return m_bReverseThrustersPressed; }

	void SetDecoration( int nDecoration ) { m_nShipDecoration = nDecoration;  }
	int GetDecoration() { return m_nShipDecoration; }

	void SetWeapon( int nWeapon ) { m_nShipWeapon = nWeapon;  }
	int GetWeapon() { return m_nShipWeapon; }

	void SetPower( int nPower ) { m_nShipPower = nPower;  }
	int GetPower() { return m_nShipPower; }

	void SetShieldStrength( int nShieldPower ) { m_nShieldStrength = nShieldPower;  }
	int GetShieldStrength() { return m_nShieldStrength; }

	void SetThrustersLevel( float fLevel ) { m_fThrusterLevel = fLevel; }
	float GetThrustersLevel( ) { return m_fThrusterLevel; }

	void SetTurnSpeed( float fSpeed ) { m_fTurnSpeed = fSpeed; }
	float GetTurnSpeed( ) { return m_fTurnSpeed; }

private:
	// Key's which are done
	bool m_bFirePressed;
	bool m_bTurnLeftPressed;
	bool m_bTurnRightPressed;
	bool m_bForwardThrustersPressed;
	bool m_bReverseThrustersPressed;

	// Decoration for this ship
	int m_nShipDecoration;

	// Weapon for this ship
	int m_nShipWeapon;

	// Power for this ship
	int m_nShipPower;

	int m_nShieldStrength;

	// Name of the player (needed server side to tell master server about)
	// bugbug jmccaskey - Really lame to send this every update instead of event driven...
	char m_rgchPlayerName[64];

	// Thrust and rotation speed can be anlog when using a Steam Controller
	float m_fThrusterLevel;
	float m_fTurnSpeed;
};

#pragma pack( pop )

#endif // SPACEWAR_H
