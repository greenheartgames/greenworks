//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the game engine -- SDL implementation
//
// $NoKeywords: $
//=============================================================================

#ifndef GAMEENGINESDL_H
#define GAMEENGINESDL_H

typedef unsigned char byte;

#include "GameEngine.h"

#include <AL/al.h>
#include <AL/alc.h>

#if defined(USE_SDL2)
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#else
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_ttf/SDL_ttf.h>
#endif

#include <string>
#include <set>
#include <map>



// How big is the vertex buffer for batching lines in total?
// NOTE: This must be a multiple of the batch size below!!! (crashes will occur if it isn't)
#define LINE_BUFFER_TOTAL_SIZE 1000

// How many lines do we put in the buffer in between flushes?
//
// This should be enough smaller than the total size so that draw calls
// can finish using the data before we wrap around and discard it.
#define LINE_BUFFER_BATCH_SIZE 250

// How big is the vertex buffer for batching points in total?
// NOTE: This must be a multiple of the batch size below!!! (crashes will occur if it isn't)
#define POINT_BUFFER_TOTAL_SIZE 1800

// How many points do we put in the buffer in between flushes?
//
// This should be enough smaller than the total size so that draw calls
// can finish using the data before we wrap around and discard it.
#define POINT_BUFFER_BATCH_SIZE 600

// How big is the vertex buffer for batching quads in total?
// NOTE: This must be a multiple of the batch size below!!! (crashes will occur if it isn't)
#define QUAD_BUFFER_TOTAL_SIZE 1000

// How many quads do we put in the buffer in between flushes?
//
// This should be enough smaller than the total size so that draw calls
// can finish using the data before we wrap around and discard it.
#define QUAD_BUFFER_BATCH_SIZE 250



class CVoiceContext;
class GLString;

class CGameEngineGL : public IGameEngine
{
public:

	// Constructor
	CGameEngineGL( );

	// Destructor
	~CGameEngineGL() { Shutdown(); }

	// Check if the game engine is initialized ok and ready for use
	bool BReadyForUse() { return m_bEngineReadyForUse; }

	// Check if the engine is shutting down
	bool BShuttingDown() { return m_bShuttingDown; }

	// Set the background color
	void SetBackgroundColor( short a, short r, short g, short b );

	// Start a frame, clear(), beginscene(), etc
	bool StartFrame();

	// Finish a frame, endscene(), present(), etc.
	void EndFrame();

	// Shutdown the game engine
	void Shutdown();

	// Pump messages from the OS
	void MessagePump();

	// Accessors for game screen size
	int32 GetViewportWidth() { return m_nWindowWidth; }
	int32 GetViewportHeight() { return m_nWindowHeight; }

	// Function for drawing text to the screen, dwFormat is a combination of flags like DT_LEFT, TEXTPOS_VCENTER etc...
	bool BDrawString( HGAMEFONT hFont, RECT rect, DWORD dwColor, DWORD dwFormat, const char *pchText );

	// Create a new font returning our internal handle value for it (0 means failure)
	HGAMEFONT HCreateFont( int nHeight, int nFontWeight, bool bItalic, const char * pchFont );
	
	// Create a new texture returning our internal handle value for it (0 means failure)
	HGAMETEXTURE HCreateTexture( byte *pRGBAData, uint32 uWidth, uint32 uHeight, ETEXTUREFORMAT eTextureFormat = eTextureFormat_RGBA );

	// update an existing texture
	bool UpdateTexture( HGAMETEXTURE texture, byte *pRGBAData, uint32 uWidth, uint32 uHeight, ETEXTUREFORMAT eTextureFormat );

	// Draw a line, the engine itself will manage batching these (although you can explicitly flush if you need to)
	bool BDrawLine( float xPos0, float yPos0, DWORD dwColor0, float xPos1, float yPos1, DWORD dwColor1 );

	// Flush the line buffer
	bool BFlushLineBuffer();

	// Draw a point, the engine itself will manage batching these (although you can explicitly flush if you need to)
	bool BDrawPoint( float xPos, float yPos, DWORD dwColor );

	// Flush the point buffer
	bool BFlushPointBuffer();

	// Draw a filled quad
	bool BDrawFilledRect( float xPos0, float yPos0, float xPos1, float yPos1, DWORD dwColor );

	// Draw a textured rectangle 
	bool BDrawTexturedRect( float xPos0, float yPos0, float xPos1, float yPos1, 
		float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture );

	// Draw a textured arbitrary quad
	bool BDrawTexturedQuad( float xPos0, float yPos0, float xPos1, float yPos1, float xPos2, float yPos2, float xPos3, float yPos3,
		float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture );

	// Flush any still cached quad buffers
	bool BFlushQuadBuffer();

	// Get the current state of a key
	bool BIsKeyDown( DWORD dwVK );

	// Get the first (in some arbitrary order) key down, if any
	bool BGetFirstKeyDown( DWORD *pdwVK );

	// Return true if there is an active Steam Controller
	bool BIsSteamInputDeviceActive( );

	// Find an active Steam controller
	void FindActiveSteamInputDevice( );

	// Get the current state of a controller action
	bool BIsControllerActionActive( ECONTROLLERDIGITALACTION dwAction );

	// Get the current state of a controller action
	void GetControllerAnalogAction( ECONTROLLERANALOGACTION dwAction, float *x, float *y );

	// Set the current Steam Controller Action set
	void SetSteamControllerActionSet( ECONTROLLERACTIONSET dwActionSet );

	// Set an Action Set Layer for Steam Input
	virtual void ActivateSteamControllerActionSetLayer( ECONTROLLERACTIONSET dwActionSet );
	virtual void DeactivateSteamControllerActionSetLayer( ECONTROLLERACTIONSET dwActionSet );

	// Returns whether a given action set layer is active
	virtual bool BIsActionSetLayerActive( ECONTROLLERACTIONSET dwActionSetLayer );

	// These calls return a string describing which controller button the action is currently bound to
	const char *GetTextStringForControllerOriginDigital( ECONTROLLERACTIONSET dwActionSet, ECONTROLLERDIGITALACTION dwDigitalAction );
	const char *GetTextStringForControllerOriginAnalog( ECONTROLLERACTIONSET dwActionSet, ECONTROLLERANALOGACTION dwDigitalAction );

	// Set the controller LED Color, if available
	void SetControllerColor( uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags );

	// Set the trigger effect on DualSense controllers
	void SetTriggerEffect( bool bEnabled );

	// Trigger a vibration on the controller, if available
	void TriggerControllerVibration( unsigned short nLeftSpeed, unsigned short nRightSpeed );

	// Trigger haptics on the specified pad of the controller, if available
	void TriggerControllerHaptics( ESteamControllerPad ePad, unsigned short usOnMicroSec, unsigned short usOffMicroSec, unsigned short usRepeat );

	// Initialize the Steam Controller interfaces
	void InitSteamInput( );

	// Called each frame to update the Steam Input interface
	void PollSteamInput();

	// Get current tick count for the game engine
	uint64 GetGameTickCount() { return m_ulGameTickCount; }

	// Get the tick count elapsed since the previous frame
	// bugbug - We use this time to compute things like thrust and acceleration in the game,
	//			so it's important in doesn't jump ahead by large increments... Need a better
	//			way to handle that.  
	uint64 GetGameTicksFrameDelta() { return m_ulGameTickCount - m_ulPreviousGameTickCount; }

	// Tell the game engine to update current tick count
	void UpdateGameTickCount();

	// Tell the game engine to sleep for a bit if needed to limit frame rate
	bool BSleepForFrameRateLimit( uint32 ulMaxFrameRate );

	// Check if the game engine hwnd currently has focus (and a working d3d device)
	bool BGameEngineHasFocus() { return true; }

	// Voice chat functions
	virtual HGAMEVOICECHANNEL HCreateVoiceChannel();
	virtual void DestroyVoiceChannel( HGAMEVOICECHANNEL hChannel );
	virtual bool AddVoiceData( HGAMEVOICECHANNEL hChannel, const uint8 *pVoiceData, uint32 uLength );

	void AdjustViewport();

	// Initialize graphics
	bool BInitializeGraphics();

	// Initialize the debug font library
	bool BInitializeCellDbgFont();

	bool BInitializeAudio();

	void RunAudio();

	void UpdateKey( uint32_t vkKey, int nDown );

	// Tracks whether the engine is ready for use
	bool m_bEngineReadyForUse;

	// Tracks if we are shutting down
	bool m_bShuttingDown;

	// The SDL window
	SDL_Window *m_window;
	SDL_GLContext m_context;

	// Size of the window to display the game in
	int32 m_nWindowWidth;
	int32 m_nWindowHeight;

	// Current game time in milliseconds
	uint64 m_ulGameTickCount;

	// Game time at the start of the previous frame
	uint64 m_ulPreviousGameTickCount;

	// White texture used when drawing filled quads
	HGAMETEXTURE m_hTextureWhite;

	// Pointer to actual data for points
	GLfloat *m_rgflPointsData;
	GLubyte *m_rgflPointsColorData;

	// How many points are outstanding needing flush
	DWORD m_dwPointsToFlush;

	// Pointer to actual data for lines
	GLfloat *m_rgflLinesData;
	GLubyte *m_rgflLinesColorData;


	// How many lines are outstanding needing flush
	DWORD m_dwLinesToFlush;

	// Pointer to actual data for quads
	GLfloat *m_rgflQuadsData;
	GLubyte *m_rgflQuadsColorData;
	GLfloat *m_rgflQuadsTextureData;

	// How many lines are outstanding needing flush
	DWORD m_dwQuadsToFlush;

	// Map of font handles we have given out
	HGAMEFONT m_nNextFontHandle;
	std::map< HGAMEFONT, TTF_Font * > m_MapGameFonts;
	std::map< std::string, HGAMETEXTURE > m_MapStrings;

	// Map of handles to texture objects
	struct TextureData_t
	{
		uint32 m_uWidth;
		uint32 m_uHeight;
		GLuint m_uTextureID;
	};
	std::map<HGAMETEXTURE, TextureData_t> m_MapTextures;
	HGAMETEXTURE m_nNextTextureHandle;

	// Last bound texture, used to know when we must flush
	HGAMETEXTURE m_hLastTexture;

	// Map of button state, translated to VK for win32.
	std::set< DWORD > m_SetKeysDown;
	
	ALCcontext* m_palContext;
	ALCdevice* m_palDevice;

	// Map of voice handles
	std::map<HGAMEVOICECHANNEL, CVoiceContext* > m_MapVoiceChannel;
	uint32 m_unVoiceChannelCount;

	// An array of handles to Steam Controller events that player can bind to controls
	InputDigitalActionHandle_t m_ControllerDigitalActionHandles[eControllerDigitalAction_NumActions];

	// An array of handles to Steam Controller events that player can bind to controls
	InputAnalogActionHandle_t m_ControllerAnalogActionHandles[eControllerAnalogAction_NumActions];

	// An array of handles to different Steam Controller action set configurations
	InputActionSetHandle_t m_ControllerActionSetHandles[eControllerActionSet_NumSets];

	// A handle to the currently active Steam Controller. 
	InputHandle_t m_ActiveControllerHandle;

	// Origins for all the Steam Input actions. The 'origin' is where the action is currently bound to,
	// ie 'jump' is currently bound to the Steam Controller 'A' button.
	EInputActionOrigin m_ControllerDigitalActionOrigins[eControllerDigitalAction_NumActions];
	EInputActionOrigin m_ControllerAnalogActionOrigins[eControllerDigitalAction_NumActions];

	static const char *pOriginStrings[k_EControllerActionOrigin_Count];

};

#endif // GAMEENGINESDL_H
