//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the game engine -- win32 implementation
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"
#include "steam/isteamps3overlayrenderer.h"
#include "GameEnginePS3.h"
#include "steam/steamps3params_internal.h"
#include <sysutil/sysutil_gamecontent.h>
#include <map>
#include <cell/sysmodule.h>
#include <cell/voice.h>
#include <sysutil/sysutil_userinfo.h>


#define DebuggerBreak() {  __asm volatile ("tw 31,1,1"); }

// Allocate static member
std::map<void *, CGameEnginePS3* > CGameEnginePS3::m_MapEngineInstances;

// How big is the vertex buffer for batching lines in total?
#define LINE_BUFFER_TOTAL_SIZE 1000

// How big is the vertex buffer for batching points in total?
#define POINT_BUFFER_TOTAL_SIZE 1800

// How big is the vertex buffer for batching quads in total?
#define QUAD_BUFFER_TOTAL_SIZE 1000

// Only a single global console can be setup for output, track that here
CellDbgFontConsoleId g_DbgFontConsoleID = -1;

// Global for PS3 params
SteamPS3Params_t g_SteamPS3Params;

void RunGameLoop( IGameEngine *pGameEngine, const char *pchServerAddress, const char *pchLobbyID  );
extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText );

void OutputDebugString( const char *pchMsg )
{
#ifndef _CERT
	fprintf( stderr, "%s", pchMsg );
	cellDbgFontConsolePrintf( g_DbgFontConsoleID, "%s", pchMsg );
#endif
}

// taken from sample
static const char s_npCommunicationSignature[STEAM_PS3_COMMUNICATION_SIG_MAX] = {
		0xb9,0xdd,0xe1,0x3b,0x01,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x1d,0x3c,0x55,0x0f,
		0x35,0xb5,0x54,0xfe,0x4e,0x97,0x1a,0x01,
		0x23,0x38,0xaa,0xd6,0x3d,0xda,0x6a,0xac,
		0x3e,0x95,0xff,0x09,0x49,0xd7,0xb3,0xda,
		0x11,0xae,0xf0,0xde,0xd6,0x2b,0x70,0x96,
		0x40,0x09,0x0e,0xed,0x8c,0x38,0x1d,0xa4,
		0xc3,0x0e,0xc9,0x30,0xc1,0xcc,0x66,0x92,
		0xd1,0xb0,0x6e,0x01,0xc0,0x44,0xb2,0xa2,
		0xd0,0x62,0x88,0xa8,0x26,0x7f,0x91,0xb5,
		0x7b,0x40,0x0c,0x6a,0xc9,0x3b,0x5c,0x89,
		0x43,0x22,0x16,0x4e,0x27,0x56,0x46,0x4a,
		0x63,0xc4,0x55,0xce,0xb3,0xce,0xf7,0x92,
		0x07,0x71,0x13,0x60,0x6e,0xcb,0xad,0xd5,
		0xf0,0x60,0xd6,0x71,0x3a,0x45,0xaa,0x25,
		0x38,0x60,0x11,0x1a,0xa5,0x0e,0xcf,0xa4,
		0x21,0xc8,0x94,0x6d,0xf2,0x0d,0xac,0xcf,
		0x67,0x8d,0x4a,0x14,0x14,0x4e,0xed,0x45,
		0x67,0x40,0x60,0x93,0x2b,0x00,0xeb,0xb7,
		0xf3,0x2f,0x09,0x36,0xb6,0x59,0x84,0x0e
	};

//-----------------------------------------------------------------------------
// Purpose: Loads the Steam PS3 module
//-----------------------------------------------------------------------------
sys_prx_id_t g_sys_prx_id_steam = -1;
static bool LoadSteamPS3Module()
{
	g_sys_prx_id_steam = sys_prx_load_module( SYS_APP_HOME "/steam_api_ps3.sprx", 0, NULL );
	if ( g_sys_prx_id_steam < CELL_OK )
	{
		OutputDebugString( "LoadSteamModule() - failed to load steam_api_ps3\n" );
		return false;
	}

	int modres;
	int res = sys_prx_start_module( g_sys_prx_id_steam, 0, NULL, &modres, 0, NULL);
	if ( res < CELL_OK )
	{
		OutputDebugString( "LoadSteamModule() - failed to start steam_api_ps3\n" );
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Unloads the Steam PS3 module
//-----------------------------------------------------------------------------
static bool UnloadSteamPS3Module()
{
	// check if loaded
	if ( g_sys_prx_id_steam < CELL_OK )
		return false;

	int modres;
	int res = sys_prx_stop_module( g_sys_prx_id_steam, 0, NULL, &modres, 0, NULL);
	if ( res < CELL_OK )
	{
		OutputDebugString( "LoadSteamModule() - failed to stop steam_api_ps3\n" );
		return false;
	}

	res = sys_prx_unload_module( g_sys_prx_id_steam, 0, NULL );
	if ( res < CELL_OK )
	{
		OutputDebugString( "LoadSteamModule() - failed to unload steam_api_ps3\n" );
		return false;
	}

	g_sys_prx_id_steam = -1;
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Initializes Steam PS3 params for our application
//			This is very similar to CPs3ContentPathInfo::Init
//-----------------------------------------------------------------------------
SteamPS3ParamsInternal_t g_steamPS3ParamsInternal = { STEAM_PS3_PARAMS_INTERNAL_VERSION, k_EUniverseBeta, "", true };
bool SetSteamPS3Params( SteamPS3Params_t *pParams )
{
	char bootdir[CELL_GAME_DIRNAME_SIZE] = { 0 };
	char gameHDDataPath[CELL_GAME_DIRNAME_SIZE];
	char gameTitle[CELL_GAME_SYSP_TITLE_SIZE];
	char gameTitleID[CELL_GAME_SYSP_TITLEID_SIZE]; // CELL_GAME_PARAMID_TITLE_ID
	char gameAppVer[CELL_GAME_SYSP_VERSION_SIZE]; // CELL_GAME_PARAMID_APP_VER
	char gameContentPath[CELL_GAME_PATH_MAX]; // as returned by contentPermit but usually meaningless (?)
	char gameBasePath[CELL_GAME_PATH_MAX];

	unsigned int nBootType = 0; /// either CELL_GAME_GAMETYPE_DISC or CELL_GAME_GAMETYPE_HDD
	unsigned int nBootAttribs = 0; /// some combination of attribute masks -- see .cpp for details

	CellSysCacheParam sysCacheParams;
	memset( &sysCacheParams, 0, sizeof( CellSysCacheParam ) );

	CellGameContentSize size;
	memset(&size, 0, sizeof(CellGameContentSize));


	/////////////////////////////////////////////////////////////////////////
	//
	// load sysutil GAME
	//
	//////////////////////////////////////////////////////////////////////////

	// we'll need to haul libsysutil into memory  ( CELL_SYSMODULE_SYSUTIL_GAME )
	bool bSysModuleIsLoaded = cellSysmoduleIsLoaded( CELL_SYSMODULE_SYSUTIL_GAME ) == CELL_SYSMODULE_LOADED ;
	// if this assert trips, then:
	// 1) look at where the sysutil_game module is loaded to make sure it still needs to be loaded at this point (maybe you can dump it to save memory)
	// 2) if it's being taken care of somewhere else, we don't need to load the module here. 
	if ( !bSysModuleIsLoaded )
	{
		//  SYSUTIL_GAME module not loaded yet
		if ( CELL_OK != cellSysmoduleLoadModule( CELL_SYSMODULE_SYSUTIL_GAME ) )
			return false;
	}

	// get the base to the content directory.
	bool bSuccess = CELL_GAME_RET_OK == cellGameBootCheck( &nBootType, &nBootAttribs, &size, bootdir );

	if ( bSuccess )
	{

		bSuccess &= CELL_GAME_RET_OK == cellGameGetParamString( CELL_GAME_PARAMID_TITLE, gameTitle, sizeof( gameTitle ) ); 
		bSuccess &= CELL_GAME_RET_OK == cellGameGetParamString( CELL_GAME_PARAMID_TITLE_ID, gameTitleID, sizeof( gameTitleID ) );
		bSuccess &= CELL_GAME_RET_OK == cellGameGetParamString( CELL_GAME_PARAMID_APP_VER, gameAppVer, sizeof( gameAppVer ) ); 
	}

	if ( bSuccess )
	{
		bSuccess = CELL_GAME_RET_OK == cellGameContentPermit( gameContentPath, gameBasePath ) ; 
	}

	if ( bSuccess )
	{
		// Get the game data directory on the hard disk. 
		memset(&size, 0, sizeof(CellGameContentSize));
		const int ret = cellGameDataCheck( CELL_GAME_GAMETYPE_GAMEDATA, gameTitleID, &size );
		if ( ret == CELL_GAME_RET_NONE )
		{
			// create game directory for the first time
			CellGameSetInitParams init; memset( &init, 0, sizeof( init ) );
			memcpy( init.title, gameTitle, sizeof( gameTitle ) );
			memcpy( init.titleId, gameTitleID, sizeof( gameTitleID ) );
			memcpy( init.version, gameAppVer, sizeof( gameAppVer ) );

			char tmp_contentInfoPath[CELL_GAME_PATH_MAX] = {0};
			char tmp_usrdirPath[CELL_GAME_PATH_MAX] = {0};

			bSuccess = CELL_GAME_RET_OK == cellGameCreateGameData( &init, tmp_contentInfoPath, tmp_usrdirPath );
		}
		else if ( ret != CELL_GAME_RET_OK )
		{
			// failure
			bSuccess = false;
		}
	}

	if ( bSuccess )
	{
		char contentInfoPath[256];
		bSuccess = CELL_GAME_RET_OK == cellGameContentPermit( contentInfoPath, gameHDDataPath );
	}

	if ( bSuccess )
	{
		// Steam needs the system cache path. Passing an empty string so it is always cleared for testing
		// memcpy( sysCacheParams.cacheId, gameTitleID, sizeof( gameTitleID ) );
		sysCacheParams.cacheId[0] = '\0';

		const int ret = cellSysCacheMount( &sysCacheParams );
		bSuccess =  ( ret == CELL_SYSCACHE_RET_OK_CLEARED ) || ( ret == CELL_SYSCACHE_RET_OK_RELAYED );
	}

	if ( !bSysModuleIsLoaded )
	{
		// actually this means it wasn't loaded when we got into the function. unload again
		cellSysmoduleUnloadModule( CELL_SYSMODULE_SYSUTIL_GAME );
	}

	if ( bSuccess )
	{
		// Internal params, not used by public games.
		pParams->pReserved = &g_steamPS3ParamsInternal;

		// configure the Steamworks PS3 parameters. All params need to be set.
		pParams->m_nAppId = 480;

		pParams->m_cSteamInputTTY = SYS_TTYP3;

		strncpy( pParams->m_rgchNpServiceID, "UD0031-NPXX00848_00", STEAM_PS3_SERVICE_ID_MAX );
		strncpy( pParams->m_rgchNpCommunicationID, "NPXS00022", STEAM_PS3_COMMUNICATION_ID_MAX );
		memcpy( pParams->m_rgchNpCommunicationSig, s_npCommunicationSignature, STEAM_PS3_COMMUNICATION_SIG_MAX );
		strncpy( pParams->m_rgchInstallationPath, SYS_APP_HOME, STEAM_PS3_PATH_MAX );
		strncpy( pParams->m_rgchSystemCache, sysCacheParams.getCachePath, STEAM_PS3_PATH_MAX );
		strncpy( pParams->m_rgchGameData, gameHDDataPath, STEAM_PS3_PATH_MAX );
		strncpy( pParams->m_rgchSteamLanguage, "english", STEAM_PS3_LANGUAGE_MAX );
		strncpy( pParams->m_rgchRegionCode, "SCEA", STEAM_PS3_REGION_CODE_MAX );

		pParams->m_sysNetInitInfo.m_bNeedInit = true;	// default network initialization
		pParams->m_sysJpgInitInfo.m_bNeedInit = true;
		pParams->m_sysSysUtilUserInfo.m_bNeedInit = true;
		pParams->m_sysPngInitInfo.m_bNeedInit = true;
	}

	return bSuccess;
}


//-----------------------------------------------------------------------------
// Purpose: Path to save user data
//-----------------------------------------------------------------------------
static char g_rgchUserDataPath[CELL_GAME_PATH_MAX] = {0};
bool SetUserSaveDataPath()
{	
	// On PS3, we need to save the user's stats & achievement information into the save container. In this example, we are simply
	// saving the data to a known location on disk.	

	// To get a unique path per user, include the local user id in the file name

	// need the user info module
	if ( cellSysmoduleLoadModule( CELL_SYSMODULE_SYSUTIL_USERINFO ) != CELL_OK )
		return false;

	// get local id
	CellSysutilUserId unLocalUserID;
	if ( cellUserInfoGetList( NULL, NULL, &unLocalUserID ) != CELL_USERINFO_RET_OK )
		return false;

	// can now unload the module
	cellSysmoduleUnloadModule( CELL_SYSMODULE_SYSUTIL_USERINFO );

	// save to the game directory
	if ( snprintf( g_rgchUserDataPath, sizeof( g_rgchUserDataPath ), "%s/%u_stats.bin", g_SteamPS3Params.m_rgchGameData, unLocalUserID ) > sizeof( g_rgchUserDataPath ) - 1 )
		return false;

	return true;
}

const char *GetUserSaveDataPath()
{
	return g_rgchUserDataPath;
}


//-----------------------------------------------------------------------------
// Purpose: Main entry point for the program -- ps3
//-----------------------------------------------------------------------------
int main( int argc, char *argv[] )
{
#ifdef PS3_MTT_DEBUG
	mttLogInit( "/app_home/libmtt_log.txt" );
#endif
	OutputDebugString( "PS3 main\n" );

	// Initialize 6 SPUs but reserve 1 SPU as a raw SPU for PSGL
	sys_spu_initialize(6, 1);	

	// Load Steam
	if ( !LoadSteamPS3Module() )
		return EXIT_FAILURE;

	// Construct a new instance of the game engine 
	// bugbug jmccaskey - make screen resolution dynamic, maybe take it on command line?
	CGameEnginePS3 *pGameEngine = new CGameEnginePS3();

	// No restart app if necessary, or CEG initialization on PS3

	// Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
	// You don't necessarily have to though if you write your code to check whether all the Steam
	// interfaces are NULL before using them and provide alternate paths when they are unavailable.

	if ( !SetSteamPS3Params( &g_SteamPS3Params ) )
	{
		OutputDebugString( "SetSteamPS3Params() failed\n" );
		return EXIT_FAILURE;
	}

	// do before SteamAPI_Init(), so we can load and unload the userinfo module (we will tell Steam it isn't loaded)
	if ( !SetUserSaveDataPath() )
	{
		OutputDebugString( "SetUserSaveDataPath() failed\n" );
		return EXIT_FAILURE;
	}

	if ( !SteamAPI_Init( &g_SteamPS3Params ) )
	{
		OutputDebugString( "SteamAPI_Init() failed\n" );
		return EXIT_FAILURE;
	}

	// set our debug handler
	SteamClient()->SetWarningMessageHook( &SteamAPIDebugTextHook );

	// set text for Steam to use for PSN game invites
	SteamUtils()->SetPSNGameBootInviteStrings( "Spacewar Invite", "You've been invited to join a Spacewar lobby!" );

	// Setup overlay render interface for PS3 Steam overlay
	SteamPS3OverlayRender()->BHostInitialize( pGameEngine->GetViewportWidth(), pGameEngine->GetViewportHeight(), 60, pGameEngine, NULL );

	// No +connect support on PS3 since Steam isn't launching us, but we check for PSN boot invites, and this may postback a lobby join 
	// requested callback to us.

	// bugbug jmccaskey - MUST call cellGameBootCheck() to get attributes param to pass here!
	SteamMatchmaking()->CheckForPSNGameBootInvite( 0 );

	// This call will block and run until the game exits
	RunGameLoop( pGameEngine, NULL, NULL );

#ifdef PS3_MTT_DEBUG
	mttLogShutdown();
#endif

	// Shutdown the SteamAPI
	SteamAPI_Shutdown();

	// Unload Steam
	UnloadSteamPS3Module();

	// exit
	return 0;	
}

//-----------------------------------------------------------------------------
// Purpose: PS3 callback handler
//-----------------------------------------------------------------------------
static void PS3SysutilCallback( uint64_t status, uint64_t param, void* userdata )
{
	(void) param;

	CGameEnginePS3 *pGameEngine = CGameEnginePS3::FindEngineInstanceForPtr( userdata );

	switch( status ) 
	{
		case CELL_SYSUTIL_REQUEST_EXITGAME:
			pGameEngine->Shutdown();
			break;
		case CELL_SYSUTIL_DRAWING_BEGIN:
		case CELL_SYSUTIL_DRAWING_END:
			break;
		case CELL_SYSUTIL_SYSTEM_MENU_OPEN:
			OutputDebugString( "System menu opened!\n" );
			break;
		case CELL_SYSUTIL_SYSTEM_MENU_CLOSE:
			OutputDebugString( "System menu closed!\n" );
			break;
		default:
			// Ok that we don't know them all, Steam handles some that we don't know.
			//OutputDebugString( "PS3SysutilCallback: Unknown status received\n" );
			break;
	}

	// Must call this to pass along to Steam which may need async status provided by these
	// callbacks as well.
	SteamUtils()->PostPS3SysutilCallback( status, param, userdata );
}

struct PacketQueue_t
{
	uint32 unSize;
	void *pData;
	uint32 unWritten;
	PacketQueue_t *pNext;
};

class CVoiceContext 
{
public:
	CVoiceContext() 
	{
		m_PortIdInput = 0;
		m_PortIdOutput = 0;
		m_pQueue = NULL;
	}
	virtual ~CVoiceContext()
	{
		// 
	}

	uint32_t m_PortIdInput;
	uint32_t m_PortIdOutput;
	PacketQueue_t *m_pQueue;

};


//-----------------------------------------------------------------------------
// Purpose: Constructor for game engine instance
//-----------------------------------------------------------------------------
CGameEnginePS3::CGameEnginePS3()
{
	m_bEngineReadyForUse = false;
	m_bShuttingDown = false;
	m_nWindowWidth = 0;
	m_nWindowHeight = 0;
	m_ulPreviousGameTickCount = 0;
	m_ulGameTickCount = 0;
	m_hTextureWhite = 0;
	m_pPSGLContext = NULL;
	m_pPSGLDevice = NULL;
	m_DbgFontConsoleID = -1;
	m_nNextFontHandle = 1;
	m_nNextTextureHandle = 1;
	m_hLastTexture = 0;

	m_rgflPointsData = new GLfloat[ 3*POINT_BUFFER_TOTAL_SIZE ];
	m_rgflPointsColorData = new GLubyte[ 4*POINT_BUFFER_TOTAL_SIZE ];
	m_dwPointsToFlush = 0;

	m_rgflLinesData = new GLfloat[ 6*LINE_BUFFER_TOTAL_SIZE ];
	m_rgflLinesColorData = new GLubyte[ 8*LINE_BUFFER_TOTAL_SIZE ];
	m_dwLinesToFlush = 0;

	m_rgflQuadsData = new GLfloat [ 12*QUAD_BUFFER_TOTAL_SIZE ];
	m_rgflQuadsColorData = new GLubyte[ 16*QUAD_BUFFER_TOTAL_SIZE ];
	m_rgflQuadsTextureData = new GLfloat[ 8*QUAD_BUFFER_TOTAL_SIZE ];
	m_dwQuadsToFlush = 0;
	m_unVoiceChannelCount = 0;


	CGameEnginePS3::AddInstanceToPtrMap( this );

	// Setup timing data
	m_ulGameTickCount = cell::fios::FIOSAbstimeToMilliseconds( cell::fios::FIOSGetCurrentTime() );

	// Register sysutil exit callback
	int ret = cellSysutilRegisterCallback( 0, PS3SysutilCallback, this );
	if( ret != CELL_OK ) 
	{
		OutputDebugString( "!! Registering sysutil callback failed...\n" );
		return;
	}

	if( !BInitializePSGL() )
	{
		OutputDebugString( "!! Initializing PSGL failed\n" );
		return;
	}

	if( !BInitializeCellDbgFont() )
	{
		OutputDebugString( "!! Initializing CellDbgFont failed\n" );
		return;
	}

	if ( !BInitializeLibPad() )
	{
		OutputDebugString( "!! Initializing libpad failed\n" );
		return;
	}
	
	if ( !BInitializeAudio() )
	{
		OutputDebugString( "!! Initializing audio failed\n" );
		return;
	}

	m_bEngineReadyForUse = true;
}


//-----------------------------------------------------------------------------
// Purpose: Shutdown the game engine
//-----------------------------------------------------------------------------
void CGameEnginePS3::Shutdown()
{
	// Flag that we are shutting down so the frame loop will stop running
	m_bShuttingDown = true;

	// Shutdown dbg font library
	if ( m_DbgFontConsoleID >= 0 )
	{
		cellDbgFontConsoleClose( m_DbgFontConsoleID );
		cellDbgFontExit();
	}

	// Should be safe to call even if we didn't actually init.
	cellPadEnd();

	// PS3 docs say it's best not to call this and allow the os/vshell to handle it instead to avoid brief noise
	// in the video display

	if ( m_rgflPointsData )
	{
		delete[] m_rgflPointsData;
		m_rgflPointsData = NULL;
	}

	if ( m_rgflPointsColorData )
	{
		delete[] m_rgflPointsColorData;
		m_rgflPointsColorData = NULL;
	}

	if ( m_rgflLinesData )
	{
		delete[] m_rgflLinesData;
		m_rgflLinesData = NULL;
	}

	if ( m_rgflLinesColorData )
	{
		delete[] m_rgflLinesColorData;
		m_rgflLinesColorData = NULL;
	}

	if ( m_rgflQuadsData )
	{
		delete[] m_rgflQuadsData;
		m_rgflQuadsData = NULL;
	}

	if ( m_rgflQuadsColorData )
	{
		delete[] m_rgflQuadsColorData;
		m_rgflQuadsColorData = NULL;
	}

	if ( m_rgflQuadsTextureData )
	{
		delete[] m_rgflQuadsTextureData;
		m_rgflQuadsTextureData = NULL;
	}

	m_dwLinesToFlush = 0;
	m_dwPointsToFlush = 0;
	m_dwQuadsToFlush = 0;

	/*

	// PS3 docs say it's best not to call this and allow the os/vshell to handle it instead to avoid brief noise
	// in the video display.  Should we not do this then?
	//
	// bugbug jmccaskey - don't do this?
	if ( m_pPSGLDevice )
	{
		psglMakeCurrent( NULL, m_pPSGLDevice );
		if ( m_pPSGLContext )
		{
			psglDestroyContext( m_pPSGLContext );
			m_pPSGLContext = NULL;
		}

		psglDestroyDevice( m_pPSGLDevice );
		m_pPSGLDevice = NULL;
	}

	psglExit();

	*/
}


//-----------------------------------------------------------------------------
// Purpose: Initialize voice/audio interfaces
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BInitializeAudio()
{
	int ret = cellSysmoduleLoadModule(CELL_SYSMODULE_VOICE);
	if ( ret < 0 )
		return false;

	CellVoiceInitParam Params;
	memset(&Params, 0, sizeof(CellVoiceInitParam)); 
	Params.appType = CELLVOICE_APPTYPE_GAME_1MB;
	Params.version = CELLVOICE_VERSION_100;

	ret = cellVoiceInitEx( &Params );

	if (ret != CELL_OK )
		return false;

	sys_ipc_key_t voiceEventKey;
	sys_event_queue_t voiceQueue;
	int err = cellVoiceCreateNotifyEventQueue(&voiceQueue, &voiceEventKey);
	if (err != CELL_OK)
		return false;

	uint64_t source = 12345;

	err = cellVoiceSetNotifyEventQueue(voiceEventKey, source);

	if (err != CELL_OK)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize libpad for controller input
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BInitializeLibPad()
{
	int ret = cellPadInit( CELL_PAD_MAX_PORT_NUM );
	if ( ret != CELL_OK )
		return false;

	// We don't use pressure sensitivity or sixaxis
	for ( int i=0; i<CELL_PAD_MAX_PORT_NUM; ++i )
	{
		// May be we don't have all ports connected, but this setting will be set and persist anyway
		cellPadSetPortSetting( i, CELL_PAD_SETTING_PRESS_OFF | CELL_PAD_SETTING_SENSOR_OFF );
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Initialize the PSGL rendering interfaces and default state
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BInitializePSGL()
{
	// Clear any errors
	glGetError();

	// First, initialize PSGL
	// Note that since we initialized the SPUs ourselves earlier we should
	// make sure that PSGL doesn't try to do so as well.
	PSGLinitOptions initOpts = {
		enable: PSGL_INIT_MAX_SPUS | PSGL_INIT_INITIALIZE_SPUS | PSGL_INIT_HOST_MEMORY_SIZE,
		maxSPUs: 1,
		initializeSPUs: false,
						// We're not specifying values for these options, the code is only here
						// to alleviate compiler warnings.
		persistentMemorySize: 0,
		transientMemorySize: 0,
		errorConsole: 0,
		fifoSize: 0,	
		hostMemorySize: 8*1024*1024,  // 8mb for host memory 
	};

	psglInit( &initOpts );

	m_pPSGLDevice = psglCreateDeviceAuto( GL_ARGB_SCE, GL_DEPTH_COMPONENT24, GL_MULTISAMPLING_4X_SQUARE_ROTATED_SCE );
	if ( !m_pPSGLDevice )
	{
		OutputDebugString( "!! Failed to init the device \n" ); 
		return false;
	}

	GLuint width, height;
	psglGetDeviceDimensions( m_pPSGLDevice, &width, &height );
	m_nWindowHeight = height;
	m_nWindowWidth = width;

	// Now create a PSGL context
	m_pPSGLContext = psglCreateContext();
	if ( !m_pPSGLContext ) 
	{
		OutputDebugString( "Error creating PSGL context\n" );
		return false;
	}

	// Make this context current for the device we initialized
	psglMakeCurrent( m_pPSGLContext, m_pPSGLDevice );

	// Our sub texture updates trigger this warning, we don't care since this is a trivial example app.
	psglDisableReport( PSGL_REPORT_TEXTURE_COPY_BACK );

	// Since we're using fixed function stuff (i.e. not using our own shader
	// yet), we need to load shaders.bin that contains the fixed function 
	// shaders.
	psglLoadShaderLibrary( SYS_APP_HOME"/shaders.bin" );

	// Reset the context
	psglResetCurrentContext();

	glViewport( 0, 0, width, height );
	glScissor( 0, 0, width, height );
	glClearDepthf(1.0f);
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable( GL_VSYNC_SCE );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glDisable( GL_CULL_FACE );
	glDisable( GL_ALPHA_TEST );
	glDisable( GL_STENCIL_TEST );
	glDisable( GL_SCISSOR_TEST );
	glDisable( GL_LIGHTING );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_FOG );

	glDepthMask( GL_FALSE );

	// We always need these two
	glEnableClientState( GL_COLOR_ARRAY );
	glEnableClientState( GL_VERTEX_ARRAY );

	// This we'll enable as needed
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrthof( 0, width, height, 0, -1.0f, 1.0f );
	glTranslatef( 0, 0, 0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef( 0, 0, 0 );

	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	glTranslatef( 0, 0, 0 );

	glDepthRangef( 0.0f, 1.0f );

	// PSGL doesn't clear the screen on startup, so let's do that here.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	psglSwap();

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Initialize the debug font library
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BInitializeCellDbgFont()
{
	// initialize debug font library, then open 2 consoles
	CellDbgFontConfig cfg;
	cfg.bufSize      = 4096;
	cfg.screenWidth  = m_nWindowWidth;
	cfg.screenHeight = m_nWindowHeight;
	if ( cellDbgFontInit( &cfg) != CELL_OK )
	{
		OutputDebugString( "Failed initializing CellDbgFont\n" );
	}

	CellDbgFontConsoleConfig ccfg0;
	ccfg0.posLeft     = 0.18f;
	ccfg0.posTop      = 0.82f;
	ccfg0.cnsWidth    = 128;
	ccfg0.cnsHeight   = 8;
	ccfg0.scale       = 0.65f;
	ccfg0.color       = 0xff0080ff;  // ABGR -> orange
	g_DbgFontConsoleID = m_DbgFontConsoleID = cellDbgFontConsoleOpen( &ccfg0 );
	if ( g_DbgFontConsoleID < 0 )
	{
		OutputDebugString( "Failed creating CellDbgFontConsole\n" );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Updates current tick count for the game engine
//-----------------------------------------------------------------------------
void CGameEnginePS3::UpdateGameTickCount()
{
	m_ulPreviousGameTickCount = m_ulGameTickCount;
	m_ulGameTickCount = cell::fios::FIOSAbstimeToMilliseconds( cell::fios::FIOSGetCurrentTime() );
}


//-----------------------------------------------------------------------------
// Purpose: Tell the game engine to sleep for a bit if needed to limit frame rate.  You must keep
// calling this repeatedly until it returns false.  If it returns true it's slept a little, but more
// time may be needed.
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BSleepForFrameRateLimit( uint32 ulMaxFrameRate )
{
	// Frame rate limiting
	float flDesiredFrameMilliseconds = 1000.0f/ulMaxFrameRate;

	uint64 ulGameTickCount = cell::fios::FIOSAbstimeToMilliseconds( cell::fios::FIOSGetCurrentTime() );

	float flMillisecondsElapsed = (float)(ulGameTickCount - m_ulGameTickCount);
	if ( flMillisecondsElapsed < flDesiredFrameMilliseconds )
	{
		// If enough time is left sleep, otherwise just keep spinning so we don't go over the limit...
		if ( flDesiredFrameMilliseconds - flMillisecondsElapsed > 3.0f )
		{
			sys_timer_usleep( 2000 );
		}
		else
		{
			// Just return right away so we busy loop, don't want to sleep too long and go over
		}

		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Set the background color to clear to
//-----------------------------------------------------------------------------
void CGameEnginePS3::SetBackgroundColor( short a, short r, short g, short b )
{
	glClearColor( (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f );
}

//-----------------------------------------------------------------------------
// Purpose: Start a new frame
//-----------------------------------------------------------------------------
bool CGameEnginePS3::StartFrame()
{
#ifdef PS3_MTT_DEBUG
	mttLogNewFrame();
#endif
	// Pump PS3 system callbacks
	MessagePump();

	// We may now be shutting down, check and don't start a frame then
	if ( BShuttingDown() )
		return false;

	// Clear the screen for the new frame
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: End the current frame
//-----------------------------------------------------------------------------
void CGameEnginePS3::EndFrame()
{
	if ( BShuttingDown() )
		return;

	if ( !m_pPSGLDevice )
		return;

	if ( !m_pPSGLContext )
		return;

	// Flush point buffer
	BFlushPointBuffer();

	// Flush line buffer
	BFlushLineBuffer();

	// Flush quad buffer
	BFlushQuadBuffer();

	// Flush dbg font data
	cellDbgFontDraw();

	// Tell the Steam overlay to draw now
	SteamPS3OverlayRender()->Render();

	// Draw a few lines, for 10% and 15% safe boundaries
	DWORD dwColor = D3DCOLOR_ARGB( 50, 255, 0, 0 );

	float flXSafe = GetViewportWidth()*0.05f;
	float flYSafe = GetViewportHeight()*0.05f;
	BDrawLine( flXSafe, flYSafe, dwColor, flXSafe, GetViewportHeight()-flYSafe, dwColor );
	BDrawLine( flXSafe, flYSafe, dwColor, GetViewportWidth()-flXSafe, flYSafe, dwColor );
	BDrawLine( GetViewportWidth()-flXSafe, flYSafe, dwColor, GetViewportWidth()-flXSafe, GetViewportHeight()-flYSafe, dwColor );
	BDrawLine( flXSafe, GetViewportHeight()-flYSafe, dwColor, GetViewportWidth()-flXSafe, GetViewportHeight()-flYSafe, dwColor );

	dwColor = D3DCOLOR_ARGB( 50, 255, 255, 0 );
	flXSafe = GetViewportWidth()*0.075f;
	flYSafe = GetViewportHeight()*0.075f;
	BDrawLine( flXSafe, flYSafe, dwColor, flXSafe, GetViewportHeight()-flYSafe, dwColor );
	BDrawLine( flXSafe, flYSafe, dwColor, GetViewportWidth()-flXSafe, flYSafe, dwColor );
	BDrawLine( GetViewportWidth()-flXSafe, flYSafe, dwColor, GetViewportWidth()-flXSafe, GetViewportHeight()-flYSafe, dwColor );
	BDrawLine( flXSafe, GetViewportHeight()-flYSafe, dwColor, GetViewportWidth()-flXSafe, GetViewportHeight()-flYSafe, dwColor );

	// Flush quads a second time, as Steam may have queued more batches.
	BFlushQuadBuffer();

	// Flush lines again
	BFlushLineBuffer();
	
	// Swap buffers now that everything is flushed
	psglSwap();

	RunAudio();
}


//-----------------------------------------------------------------------------
// Purpose: Draw a line, the engine internally manages a vertex buffer for batching these
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BDrawLine( float xPos0, float yPos0, DWORD dwColor0, float xPos1, float yPos1, DWORD dwColor1 )
{
	if ( !m_pPSGLContext || !m_pPSGLDevice || m_bShuttingDown )
		return false;


	// Check if we are out of room and need to flush the buffer
	if ( m_dwLinesToFlush == LINE_BUFFER_TOTAL_SIZE )	
	{
		BFlushLineBuffer();
	}

	DWORD dwOffset = m_dwLinesToFlush*6;
	m_rgflLinesData[dwOffset] = xPos0;
	m_rgflLinesData[dwOffset+1] = yPos0;
	m_rgflLinesData[dwOffset+2] = 1.0;
	m_rgflLinesData[dwOffset+3] = xPos1;
	m_rgflLinesData[dwOffset+4] = yPos1;
	m_rgflLinesData[dwOffset+5] = 1.0;

	dwOffset = m_dwLinesToFlush*8;
	m_rgflLinesColorData[dwOffset] = COLOR_RED( dwColor0 );
	m_rgflLinesColorData[dwOffset+1] = COLOR_GREEN( dwColor0 );
	m_rgflLinesColorData[dwOffset+2] = COLOR_BLUE( dwColor0 );
	m_rgflLinesColorData[dwOffset+3] = COLOR_ALPHA( dwColor0 );
	m_rgflLinesColorData[dwOffset+4] = COLOR_RED( dwColor1 );
	m_rgflLinesColorData[dwOffset+5] = COLOR_GREEN( dwColor1 );
	m_rgflLinesColorData[dwOffset+6] = COLOR_BLUE( dwColor1 );
	m_rgflLinesColorData[dwOffset+7] = COLOR_ALPHA( dwColor1 );

	++m_dwLinesToFlush;

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush batched lines to the screen
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BFlushLineBuffer()
{
	if ( !m_pPSGLContext || !m_pPSGLDevice || !m_rgflLinesColorData || !m_rgflLinesData || m_bShuttingDown )
		return false;

	if ( m_dwLinesToFlush )
	{
		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, m_rgflLinesColorData );
		glVertexPointer( 3, GL_FLOAT, 0, m_rgflLinesData );
		glDrawArrays( GL_LINES, 0, m_dwLinesToFlush*2 );

		m_dwLinesToFlush = 0;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Draw a point, the engine internally manages a vertex buffer for batching these
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BDrawPoint( float xPos, float yPos, DWORD dwColor )
{
	
	if ( !m_pPSGLContext || !m_pPSGLDevice || m_bShuttingDown )
		return false;


	// Check if we are out of room and need to flush the buffer
	if ( m_dwPointsToFlush == POINT_BUFFER_TOTAL_SIZE )	
	{
		BFlushPointBuffer();
	}

	DWORD dwOffset = m_dwPointsToFlush*3;
	m_rgflPointsData[dwOffset] = xPos;
	m_rgflPointsData[dwOffset+1] = yPos;
	m_rgflPointsData[dwOffset+2] = 1.0;

	dwOffset = m_dwPointsToFlush*4;
	m_rgflPointsColorData[dwOffset] = COLOR_RED( dwColor );
	m_rgflPointsColorData[dwOffset+1] = COLOR_GREEN( dwColor );
	m_rgflPointsColorData[dwOffset+2] = COLOR_BLUE( dwColor );
	m_rgflPointsColorData[dwOffset+3] = COLOR_ALPHA( dwColor );

	++m_dwPointsToFlush;

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush batched points to the screen
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BFlushPointBuffer()
{
	if ( !m_pPSGLContext || !m_pPSGLDevice || !m_rgflPointsColorData || !m_rgflPointsData || m_bShuttingDown )
		return false;

	if ( m_dwPointsToFlush )
	{
		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, m_rgflPointsColorData );
		glVertexPointer( 3, GL_FLOAT, 0, m_rgflPointsData );
		glDrawArrays( GL_POINTS, 0, m_dwPointsToFlush );

		m_dwPointsToFlush = 0;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Draw a filled quad
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BDrawFilledQuad( float xPos0, float yPos0, float xPos1, float yPos1, DWORD dwColor )
{
	if ( !m_hTextureWhite )
	{
		byte *pRGBAData = new byte[ 1 * 1 * 4 ];
		memset( pRGBAData, 255, 1*1*4 );
		m_hTextureWhite = HCreateTexture( pRGBAData, 1, 1 );
		delete[] pRGBAData;
	}

	return BDrawTexturedQuad( xPos0, yPos0, xPos1, yPos1, 0.0f, 0.0f, 1.0f, 1.0f, dwColor, m_hTextureWhite );
}


//-----------------------------------------------------------------------------
// Purpose: Draw a textured quad
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BDrawTexturedQuad( float xPos0, float yPos0, float xPos1, float yPos1, float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture )
{
	return BDrawTexturedGradientQuad( xPos0, yPos0, xPos1, yPos1, u0, v0, u1, v1, dwColor, dwColor, dwColor, dwColor, hTexture );
}

//-----------------------------------------------------------------------------
// Purpose: Draw a textured quad, with different colors at each vertex
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BDrawTexturedGradientQuad( float xPos0, float yPos0, float xPos1, float yPos1, 
		float u0, float v0, float u1, float v1, 
		DWORD dwColorTopLeft, DWORD dwColorTopRight, DWORD dwColorBottomLeft, DWORD dwColorBottomRight, HGAMETEXTURE hTexture )
{
	if ( m_bShuttingDown || !m_pPSGLDevice || !m_pPSGLContext )
		return false;

	// Find the texture
	std::map<HGAMETEXTURE, TextureData_t>::iterator iter;
	iter = m_MapTextures.find( hTexture );
	if ( iter == m_MapTextures.end() )
	{
		OutputDebugString( "BDrawTexturedQuad called with invalid hTexture value\n" );
		return false;
	}

	// Check if we are out of room and need to flush the buffer, or if our texture is changing
	// then we also need to flush the buffer.
	if ( m_dwQuadsToFlush == QUAD_BUFFER_TOTAL_SIZE || m_hLastTexture != hTexture )	
	{
		BFlushQuadBuffer();
	}

	// Bind the new texture
	glBindTexture( GL_TEXTURE_2D, iter->second.m_uTextureID );

	DWORD dwOffset = m_dwQuadsToFlush*12;
	m_rgflQuadsData[dwOffset] = xPos0;
	m_rgflQuadsData[dwOffset+1] = yPos0;
	m_rgflQuadsData[dwOffset+2] = 1.0f;
	m_rgflQuadsData[dwOffset+3] = xPos1;
	m_rgflQuadsData[dwOffset+4] = yPos0;
	m_rgflQuadsData[dwOffset+5] = 1.0f;
	m_rgflQuadsData[dwOffset+6] = xPos1;
	m_rgflQuadsData[dwOffset+7] = yPos1;
	m_rgflQuadsData[dwOffset+8] = 1.0f;
	m_rgflQuadsData[dwOffset+9] = xPos0;
	m_rgflQuadsData[dwOffset+10] = yPos1;
	m_rgflQuadsData[dwOffset+11] = 1.0f;

	dwOffset = m_dwQuadsToFlush*16;
	m_rgflQuadsColorData[dwOffset] = COLOR_RED( dwColorTopLeft );
	m_rgflQuadsColorData[dwOffset+1] = COLOR_GREEN( dwColorTopLeft );
	m_rgflQuadsColorData[dwOffset+2] = COLOR_BLUE( dwColorTopLeft );
	m_rgflQuadsColorData[dwOffset+3] = COLOR_ALPHA( dwColorTopLeft );
	m_rgflQuadsColorData[dwOffset+4] = COLOR_RED( dwColorTopRight );
	m_rgflQuadsColorData[dwOffset+5] = COLOR_GREEN( dwColorTopRight );
	m_rgflQuadsColorData[dwOffset+6] = COLOR_BLUE( dwColorTopRight );
	m_rgflQuadsColorData[dwOffset+7] = COLOR_ALPHA( dwColorTopRight );
	m_rgflQuadsColorData[dwOffset+8] = COLOR_RED( dwColorBottomLeft );
	m_rgflQuadsColorData[dwOffset+9] = COLOR_GREEN( dwColorBottomLeft );
	m_rgflQuadsColorData[dwOffset+10] = COLOR_BLUE( dwColorBottomLeft );
	m_rgflQuadsColorData[dwOffset+11] = COLOR_ALPHA( dwColorBottomLeft );
	m_rgflQuadsColorData[dwOffset+12] = COLOR_RED( dwColorBottomRight );
	m_rgflQuadsColorData[dwOffset+13] = COLOR_GREEN( dwColorBottomRight );
	m_rgflQuadsColorData[dwOffset+14] = COLOR_BLUE( dwColorBottomRight );
	m_rgflQuadsColorData[dwOffset+15] = COLOR_ALPHA( dwColorBottomRight );

	dwOffset = m_dwQuadsToFlush*8;
	m_rgflQuadsTextureData[dwOffset] = u0;
	m_rgflQuadsTextureData[dwOffset+1] = v0;
	m_rgflQuadsTextureData[dwOffset+2] = u1;
	m_rgflQuadsTextureData[dwOffset+3] = v0;
	m_rgflQuadsTextureData[dwOffset+4] = u1;
	m_rgflQuadsTextureData[dwOffset+5] = v1;
	m_rgflQuadsTextureData[dwOffset+6] = u0;
	m_rgflQuadsTextureData[dwOffset+7] = v1;


	++m_dwQuadsToFlush;

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush buffered quads
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BFlushQuadBuffer()
{
	if ( !m_pPSGLContext || !m_pPSGLDevice || !m_rgflPointsColorData || !m_rgflPointsData || m_bShuttingDown )
		return false;

	if ( m_dwQuadsToFlush )
	{
		glEnable( GL_TEXTURE_2D );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

		glColorPointer( 4, GL_UNSIGNED_BYTE, 0, m_rgflQuadsColorData );
		glVertexPointer( 3, GL_FLOAT, 0, m_rgflQuadsData );
		glTexCoordPointer( 2, GL_FLOAT, 0, m_rgflQuadsTextureData );
		glDrawArrays( GL_QUADS, 0, m_dwQuadsToFlush*4 );

		glDisable( GL_TEXTURE_2D );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );

		m_dwQuadsToFlush = 0;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Creates a new texture 
//-----------------------------------------------------------------------------
HGAMETEXTURE CGameEnginePS3::HCreateTexture( byte *pRGBAData, uint32 uWidth, uint32 uHeight )
{
	if ( m_bShuttingDown || !m_pPSGLDevice || !m_pPSGLContext )
		return 0;

	BFlushQuadBuffer();

	TextureData_t TexData;
	TexData.m_uWidth = uWidth;
	TexData.m_uHeight = uHeight;
	TexData.m_uTextureID = 0;

	glEnable( GL_TEXTURE_2D );
	glGenTextures( 1, &TexData.m_uTextureID );
	glBindTexture( GL_TEXTURE_2D, TexData.m_uTextureID );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0 );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

	// build our texture mipmaps
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, uWidth, uHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, (void *)pRGBAData );
	glDisable( GL_TEXTURE_2D );

	int nHandle = m_nNextTextureHandle;
	++m_nNextTextureHandle;
	m_MapTextures[nHandle] = TexData;

	return nHandle;
}


//-----------------------------------------------------------------------------
// Purpose: Creates a new font
//-----------------------------------------------------------------------------
HGAMEFONT CGameEnginePS3::HCreateFont( int nHeight, int nFontWeight, bool bItalic, const char * pchFont )
{
	HGAMEFONT hFont = m_nNextFontHandle;
	++m_nNextFontHandle;

	// weight + italic are not supported in our dbg font output on ps3.  Neither is specifying font.
	// We also have to compute a "scale" relative to screen size, so it may not match pc exactly. 

	// 1.0f for scale means 80 characters fit the screen width, 32 lines fit the height. 
	// We'll call that 1.0 scale font roughly equivalent to 28pt font height on pc/d3d.

	PS3DbgFont_t font;
	font.m_nScale = (float)nHeight/28.0f;
	m_MapGameFonts[ hFont ] = font;

	return hFont;
}


//-----------------------------------------------------------------------------
// Purpose: Draws text to the screen inside the given rectangular region, using the given font
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BDrawString( HGAMEFONT hFont, RECT rect, DWORD dwColor, DWORD dwFormat, const char *pchText )
{
	if ( !hFont )
	{
		OutputDebugString( "Someone is calling BDrawString with a null font handle\n" );
		return false;
	}

	float fCharWidth = m_nWindowWidth/80.0f;
	float fCharHeight = m_nWindowHeight/32.0f;

	// Find the font object for the passed handle
	std::map<HGAMEFONT, PS3DbgFont_t>::iterator iter;
	iter = m_MapGameFonts.find( hFont );
	if ( iter == m_MapGameFonts.end() )
	{
		OutputDebugString( "Invalid font handle passed to BDrawString()\n" );
		return false;
	}

	fCharWidth *= iter->second.m_nScale;
	fCharHeight *= iter->second.m_nScale;

	// Compute width/height in chars/lines
	int nLinesInText = 1;
	int nCharsWideMax = 0;
	int nCharsLine = 0;
	for( int i=0; i < strlen(pchText); ++i )
	{
		if ( pchText[i] == '\n' )
		{
			++nLinesInText;
			nCharsWideMax = MAX( nCharsLine, nCharsWideMax );
			nCharsLine = 0;
		}
		else 
		{
			// We assume all non linebreak chars are printable, don't pass others!
			++nCharsLine;
		}
	}
	nCharsWideMax = MAX( nCharsLine, nCharsWideMax );

	// Assume top left positioning
	float x = (float)rect.left;
	float y = (float)rect.top;

	if ( TEXTPOS_CENTER & dwFormat )
	{
		float fTextWidth = nCharsWideMax * fCharWidth;
		x = (float)rect.left + ((float)( rect.right-rect.left) - fTextWidth)/2.0f;
	}
	else if ( TEXTPOS_RIGHT &dwFormat )
	{
		float fTextWidth = nCharsWideMax * fCharWidth;
		x = (float)rect.right - fTextWidth;
	}

	if ( TEXTPOS_VCENTER & dwFormat )
	{
		float fTextHeight = nLinesInText * fCharHeight;
		y = (float)rect.top + ((float)( rect.bottom-rect.top) - fTextHeight)/2.0f;
	}
	else if ( TEXTPOS_RIGHT &dwFormat )
	{
		float fTextHeight = nLinesInText * fCharHeight;
		y = (float)rect.bottom - fTextHeight;
	}

	// Convert x/y to 0.0->1.0 range vs screen size
	x = x/(float)m_nWindowWidth;
	y = y/(float)m_nWindowHeight;

	// we have the font, try to draw with it
	if( cellDbgFontPuts( x, y, iter->second.m_nScale, DWARGB_TO_DWABGR(dwColor), pchText ) < 0 )
	{
		OutputDebugString( "cellDbgFontPuts call failed\n" );
		return false;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Message pump for OS messages
//-----------------------------------------------------------------------------
void CGameEnginePS3::MessagePump()
{
	cellSysutilCheckCallback();

	// Running callbacks may have triggered shutdown, if not run input
	if ( !m_bShuttingDown )
	{
		CellPadInfo2 padInfo;
		int ret = cellPadGetInfo2( &padInfo );
		if ( ret == CELL_OK )
		{
			if ( padInfo.system_info & CELL_PAD_INFO_INTERCEPTED )
			{
				// System has taken control of controller info, we can't currently access it.
			}
			
			bool bControllerFound = false;
			m_iCurrentPadIndex = -1;
			for( int i=0; i < CELL_PAD_MAX_PORT_NUM; ++i )
			{
				if ( padInfo.port_status[i] & CELL_PAD_STATUS_ASSIGN_CHANGES )
				{
					if ( (padInfo.port_status[i] & CELL_PAD_STATUS_CONNECTED) == 0 )
					{
						char rgchBuffer[512];
						sprintf_safe( rgchBuffer, "Gamepad %d removed\n", i );
						OutputDebugString( rgchBuffer );
					}
					else if ( (padInfo.port_status[i] & CELL_PAD_STATUS_CONNECTED) > 0 )
					{
						char rgchBuffer[512];
						sprintf_safe( rgchBuffer, "Gamepad %d connected\n", i );
						OutputDebugString( rgchBuffer );
					}
				}

				if ( (padInfo.port_status[i] & CELL_PAD_STATUS_CONNECTED ) > 0 && padInfo.device_type[i] == CELL_PAD_DEV_TYPE_STANDARD )
				{
					bControllerFound = true;
					m_iCurrentPadIndex = i;
					break;
				}
			}

			if ( padInfo.system_info & CELL_PAD_INFO_INTERCEPTED )
			{
				// Pass zeroed pad data to overlay to clear it's button state too
				SteamPS3OverlayRender()->BResetInputState();

				// Clear all keys 
				m_SetKeysDown.clear();
			}

			if ( !bControllerFound )
			{
				// Definitely no appropriate controller plugged in, can't do input
				static DWORD dwLastSpewTime = 0;
				if ( GetGameTickCount() - dwLastSpewTime > 3000 || dwLastSpewTime == 0 || dwLastSpewTime > GetGameTickCount() )
				{
					dwLastSpewTime = GetGameTickCount();
					OutputDebugString( "No supported controllers are active, activate one.\n" );
				}

				// Pass zeroed pad data to overlay to clear it's button state too
				SteamPS3OverlayRender()->BResetInputState();

				// Clear all keys 
				m_SetKeysDown.clear();
			}
			else
			{
				// Get status of the first found controller now
				CellPadData padData;
				int ret = cellPadGetData( m_iCurrentPadIndex, &padData );

				// If we got data ok, and if the data is new (len != 0) then process it
				if ( ret == CELL_OK && padData.len )
				{
					if ( !SteamPS3OverlayRender()->BHandleCellPadData( padData ) )
					{
						if ( padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_R2 )
						{
							m_SetKeysDown.insert( 0x57 ); // W key, thrusters, mapped to R2 on PS3
						}
						else
						{
							m_SetKeysDown.erase( 0x57 );
						}

						if ( padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_L2 )
						{
							m_SetKeysDown.insert( 0x53 ); // S key, reverse thrusters, mapped to L2 on PS3
						}
						else
						{
							m_SetKeysDown.erase( 0x53 );
						}

						if ( padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CROSS )
						{
							// Mapped to both enter in menus, and fire in game
							m_SetKeysDown.insert( VK_RETURN );
							m_SetKeysDown.insert( VK_SPACE );
						}
						else
						{
							m_SetKeysDown.erase( VK_RETURN );
							m_SetKeysDown.erase( VK_SPACE );
						}

						if ( padData.button[CELL_PAD_BTN_OFFSET_DIGITAL2] & CELL_PAD_CTRL_CIRCLE )
						{
							m_SetKeysDown.insert( VK_ESCAPE );
						}
						else
						{
							m_SetKeysDown.erase( VK_ESCAPE );
						}

						if ( padData.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_UP || padData.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y] == 0x00 )
						{
							m_SetKeysDown.insert( VK_UP );
						}
						else
						{
							m_SetKeysDown.erase( VK_UP );
						}

						if ( padData.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_DOWN || padData.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_Y] == 0xFF )
						{
							m_SetKeysDown.insert( VK_DOWN );
						}
						else
						{
							m_SetKeysDown.erase( VK_DOWN );
						}

						if ( padData.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_LEFT || padData.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X] == 0x00 )
						{
							m_SetKeysDown.insert( 0x41 ); // A Key, mapped to left on PS3
						}
						else
						{
							m_SetKeysDown.erase( 0x41 );
						}

						if ( padData.button[CELL_PAD_BTN_OFFSET_DIGITAL1] & CELL_PAD_CTRL_RIGHT || padData.button[CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X] == 0xFF )
						{
							m_SetKeysDown.insert( 0x44 ); // D key, mapped to right on PS3
						}
						else
						{
							m_SetKeysDown.erase( 0x44 );
						}
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Find out if a key is currently down
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BIsKeyDown( DWORD dwVK )
{
	std::set<DWORD>::iterator iter;
	iter = m_SetKeysDown.find( dwVK );
	if ( iter != m_SetKeysDown.end() )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Get a down key value
//-----------------------------------------------------------------------------
bool CGameEnginePS3::BGetFirstKeyDown( DWORD *pdwVK )
{
	std::set<DWORD>::iterator iter;
	iter = m_SetKeysDown.begin();
	if ( iter != m_SetKeysDown.end() )
	{
		*pdwVK = *iter;
		m_SetKeysDown.erase( iter );
		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Find the engine instance tied to a given ptr
//-----------------------------------------------------------------------------
CGameEnginePS3 * CGameEnginePS3::FindEngineInstanceForPtr( void *ptr )
{
	std::map<void *, CGameEnginePS3 *>::iterator iter;
	iter = m_MapEngineInstances.find( ptr );
	if ( iter == m_MapEngineInstances.end() )
		return NULL;
	else
		return iter->second;
}


//-----------------------------------------------------------------------------
// Purpose: Add the engine instance tied to a given ptr to our static map
//-----------------------------------------------------------------------------
void CGameEnginePS3::AddInstanceToPtrMap( CGameEnginePS3 *pInstance )
{
	m_MapEngineInstances[(void*)pInstance] = pInstance;
}


//-----------------------------------------------------------------------------
// Purpose: Removes the instance associated with a given ptr from the map
//-----------------------------------------------------------------------------
void CGameEnginePS3::RemoveInstanceFromPtrMap( void *ptr )
{
	std::map<void *, CGameEnginePS3 *>::iterator iter;
	iter = m_MapEngineInstances.find( ptr );
	if ( iter != m_MapEngineInstances.end() )
		m_MapEngineInstances.erase( iter );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
HGAMEVOICECHANNEL CGameEnginePS3::HCreateVoiceChannel()
{
	m_unVoiceChannelCount++;
	CVoiceContext* pVoiceContext = new CVoiceContext;

	CellVoicePortParam PortArgs;
	memset( &PortArgs, 0, sizeof(PortArgs) );

	PortArgs.portType                  = CELLVOICE_PORTTYPE_IN_PCMAUDIO;
	PortArgs.bMute                     = false;
	PortArgs.threshold                 = 100;
	PortArgs.volume                    = 1.0f;
	PortArgs.pcmaudio.format.sampleRate= CELLVOICE_SAMPLINGRATE_16000;
	PortArgs.pcmaudio.format.dataType  = CELLVOICE_PCM_SHORT;
	PortArgs.pcmaudio.bufSize          = 11000;
	
	int ret= cellVoiceCreatePort( &pVoiceContext->m_PortIdInput, &PortArgs );

	PortArgs.portType                  = CELLVOICE_PORTTYPE_OUT_SECONDARY;
	PortArgs.bMute                     = false;
	PortArgs.threshold                 = 100;                   
	PortArgs.volume                    = 1.0f;
	PortArgs.device.playerId           = 0;
	
	ret = cellVoiceCreatePort( &pVoiceContext->m_PortIdOutput, &PortArgs );

	ret = cellVoiceConnectIPortToOPort( pVoiceContext->m_PortIdInput, pVoiceContext->m_PortIdOutput );

	if( ret != CELL_OK )
	{
		delete pVoiceContext;
		return 0; // failed
	}

	if ( m_unVoiceChannelCount == 1 )
	{
		CellVoiceStartParam startParams;
		startParams.container = SYS_MEMORY_CONTAINER_ID_INVALID;
		ret = sys_memory_container_create(&startParams.container,1024*1024);
		ret = cellVoiceStartEx(&startParams);
	}

	m_MapVoiceChannel[m_unVoiceChannelCount] = pVoiceContext;

	return m_unVoiceChannelCount;
}

void CGameEnginePS3::RunAudio()
{
	std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;

	for( iter = m_MapVoiceChannel.begin(); iter!=m_MapVoiceChannel.end(); ++iter)
	{
		CVoiceContext* pVoiceContext = iter->second;

		PacketQueue_t *pVoicePacket = pVoiceContext->m_pQueue;

		if ( pVoicePacket )
		{
			CellVoiceBasePortInfo PortInfo;
			memset(&PortInfo, 0, sizeof(PortInfo));
			int Result = cellVoiceGetPortInfo( pVoiceContext->m_PortIdInput, &PortInfo );
			if (Result != CELL_OK && Result != CELL_VOICE_ERROR_SERVICE_DETACHED )
			{
				printf("cellVoiceGetPortInfo PCMInputPort failed %x\n",Result);
			}

			if ( PortInfo.numByte > pVoicePacket->unSize )
			{
				uint32_t bytes = pVoicePacket->unSize;
				Result = cellVoiceWriteToIPort( pVoiceContext->m_PortIdInput, pVoicePacket->pData, &bytes );
				pVoicePacket->unWritten += bytes;
			
				if ( pVoicePacket->unWritten >= pVoicePacket->unSize )
				{
					pVoiceContext->m_pQueue = pVoicePacket->pNext;
					free( pVoicePacket->pData );
					delete pVoicePacket;
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGameEnginePS3::DestroyVoiceChannel( HGAMEVOICECHANNEL hChannel )
{
	std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;
	iter = m_MapVoiceChannel.find( hChannel );
	if ( iter != m_MapVoiceChannel.end() )
	{
		CVoiceContext* pVoiceContext = iter->second;
		
		// free outstanding voice packets

		PacketQueue_t *pVoicePacket = pVoiceContext->m_pQueue;

		while( pVoicePacket )
		{
			PacketQueue_t *pNextPacket = pVoicePacket->pNext;

			free( pVoicePacket->pData );
			delete pVoicePacket;

			pVoicePacket = pNextPacket;
		}
		
		// stop voice

		cellVoiceDisconnectIPortFromOPort( pVoiceContext->m_PortIdInput,  pVoiceContext->m_PortIdOutput );
		cellVoiceDeletePort( pVoiceContext->m_PortIdInput );
		cellVoiceDeletePort( pVoiceContext->m_PortIdOutput );

		delete pVoiceContext;

		m_MapVoiceChannel.erase( iter );
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CGameEnginePS3::AddVoiceData( HGAMEVOICECHANNEL hChannel, const uint8 *pVoiceData, uint32 uLength )
{
	std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;
	iter = m_MapVoiceChannel.find( hChannel );
	if ( iter == m_MapVoiceChannel.end() )
		return false; // channel not found

	CVoiceContext* pVoiceContext = iter->second;

	PacketQueue_t *pVoicePacket = new PacketQueue_t;

	pVoicePacket->pData = malloc ( uLength );
	memcpy( pVoicePacket->pData, pVoiceData, uLength );
	pVoicePacket->unSize = uLength;
	pVoicePacket->pNext = NULL;
	pVoicePacket->unWritten = 0;

	if ( pVoiceContext->m_pQueue == NULL )
	{
		// start queue 
		pVoiceContext->m_pQueue = pVoicePacket;
	}
	else
	{
		PacketQueue_t *pLastPacket = pVoiceContext->m_pQueue;

		// find tail
		while ( pLastPacket->pNext )
			pLastPacket = pLastPacket->pNext;

		// append to tail
		pLastPacket->pNext = pVoicePacket;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Part of the render host interface for Steam overlay to draw through
//-----------------------------------------------------------------------------
void CGameEnginePS3::DrawTexturedRect( int x0, int y0, int x1, int y1, float u0, float v0, float u1, float v1, int32 iTextureID, DWORD colorStart, DWORD colorEnd, EOverlayGradientDirection eDirection )
{
	std::map<int, HGAMETEXTURE>::iterator iter;
	iter = m_MapSteamTextures.find( iTextureID );
	if ( iter != m_MapSteamTextures.end() )
	{
		if ( eDirection == k_EOverlayGradientHorizontal )
			BDrawTexturedGradientQuad( x0, y0, x1, y1, u0, v0, u1, v1, colorStart, colorEnd, colorEnd, colorStart, iter->second );
		else if ( eDirection == k_EOverlayGradientVertical )
			BDrawTexturedGradientQuad( x0, y0, x1, y1, u0, v0, u1, v1, colorStart, colorStart, colorEnd, colorEnd, iter->second );
		else
			BDrawTexturedGradientQuad( x0, y0, x1, y1, u0, v0, u1, v1, colorStart, colorStart, colorStart, colorStart, iter->second );

	}
	else
	{
		char rgchBuf[512];
		sprintf_safe( rgchBuf, "Steam trying to draw for invalid textureid: %d\n", iTextureID );
		OutputDebugString( rgchBuf );

	}
}


//-----------------------------------------------------------------------------
// Purpose: Part of the render host interface for Steam overlay to draw through
//-----------------------------------------------------------------------------
void CGameEnginePS3::LoadOrUpdateTexture( int32 iTextureID, bool bIsFullTexture, int x0, int y0, uint32 uWidth, uint32 uHeight, int32 iBytes, char *pData )
{
	BFlushQuadBuffer();
	m_hLastTexture = 0;

	if ( !bIsFullTexture )
	{
		bool bUpdated = false;
		std::map<int, HGAMETEXTURE>::iterator iter_steam;
		iter_steam = m_MapSteamTextures.find( iTextureID );
		if ( iter_steam != m_MapSteamTextures.end() )
		{
			std::map<HGAMETEXTURE, TextureData_t>::iterator iter_game; 
			iter_game = m_MapTextures.find( iter_steam->second );
			if ( iter_game != m_MapTextures.end() )
			{
				TextureData_t &TexData = iter_game->second;

				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, TexData.m_uTextureID );
				glTexSubImage2D( GL_TEXTURE_2D, 0, x0, y0, uWidth, uHeight, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, pData );
				glDisable( GL_TEXTURE_2D );
				
				bUpdated = true;
			}
			else
			{
				char rgchBuf[512];
				sprintf_safe( rgchBuf, "Couldn't find texture: %d\n", iTextureID );
				OutputDebugString( rgchBuf );
			}
		}
		else
		{
			char rgchBuf[512];
			sprintf_safe( rgchBuf, "Couldn't find Steam mapping for texture: %d\n", iTextureID );
			OutputDebugString( rgchBuf );
		}

		if ( !bUpdated )
		{
			char rgchBuf[512];
			sprintf_safe( rgchBuf, "Failed updating texture: %d\n", iTextureID );
			OutputDebugString( rgchBuf );
		}
	}
	else
	{
		HGAMETEXTURE hGameTexture = HCreateTexture( (byte*)pData, uWidth, uHeight );
		m_MapSteamTextures[iTextureID] = hGameTexture;
	}
}


//-----------------------------------------------------------------------------
// Purpose: Part of the render host interface for Steam overlay to draw through
//-----------------------------------------------------------------------------
void CGameEnginePS3::DeleteTexture( int32 iTextureID )
{
	std::map<int, HGAMETEXTURE>::iterator iter;
	iter = m_MapSteamTextures.find( iTextureID );
	if ( iter != m_MapSteamTextures.end() )
	{
		// Our game engine doesn't know how to free textures, lol.
		m_MapSteamTextures.erase( iter );
	}
	else
	{
		char rgchBuf[512];
		sprintf_safe( rgchBuf, "Got DeleteTexture from Steam for texture we don't have mapped: %d\n", iTextureID );
		OutputDebugString( rgchBuf );
	}
}


//-----------------------------------------------------------------------------
// Purpose: Part of the render host interface for Steam overlay to draw through
//-----------------------------------------------------------------------------
void CGameEnginePS3::DeleteAllTextures()
{
	m_MapSteamTextures.clear();

	// Don't really know how to delete textures in the engine, lol.
}
