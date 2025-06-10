//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the game engine
//
// $NoKeywords: $
//=============================================================================

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <set>
#include <map>

// Typedef for font handles
typedef int HGAMEFONT;

// Typedef for vertex buffer handles
typedef int HGAMEVERTBUF;

// Typedef for texture handles
typedef int HGAMETEXTURE;


// Typedef for voice channels
typedef int HGAMEVOICECHANNEL;

// BDrawText position flags
#define TEXTPOS_TOP                      0x00000000
#define TEXTPOS_LEFT                     0x00000000
#define TEXTPOS_CENTER                   0x00000001
#define TEXTPOS_RIGHT                    0x00000002
#define TEXTPOS_VCENTER                  0x00000004
#define TEXTPOS_BOTTOM                   0x00000008


#define VOICE_OUTPUT_SAMPLE_RATE			11000	// real sample rate is 11025 but for XAudio2 it must be a multiple of XAUDIO2_QUANTUM_DENOMINATOR
#define VOICE_OUTPUT_SAMPLE_RATE_IDEAL		11025
#define BYTES_PER_SAMPLE					2

// Texture formats we support for upload to textures
enum ETEXTUREFORMAT
{
	eTextureFormat_RGBA, // 8 bits per channel
	eTextureFormat_BGRA, // 8 bits per channel
	eTextureFormat_BGRA16, // 16 bits per channel
};

#define MAX_CONTROLLERS 4

enum ECONTROLLERDIGITALACTION
{
	eControllerDigitalAction_TurnLeft,
	eControllerDigitalAction_TurnRight,
	eControllerDigitalAction_ForwardThrust,
	eControllerDigitalAction_ReverseThrust,
	eControllerDigitalAction_FireLasers,
	eControllerDigitalAction_PauseMenu,

	eControllerDigitalAction_MenuUp,
	eControllerDigitalAction_MenuDown,
	eControllerDigitalAction_MenuLeft,
	eControllerDigitalAction_MenuRight,
	eControllerDigitalAction_MenuSelect,
	eControllerDigitalAction_MenuCancel,

	eControllerDigitalAction_NumActions
};

enum ECONTROLLERANALOGACTION
{
	eControllerAnalogAction_AnalogControls,

	eControllerAnalogAction_NumActions
};

enum ECONTROLLERACTIONSET
{
	eControllerActionSet_ShipControls,
	eControllerActionSet_MenuControls,
	eControllerActionSet_Layer_Thrust,

	eControllerActionSet_NumSets
};

// used for VR support
namespace vr { class IVRSystem; }

//
// Interface that needs to be implemented for game engines on all platforms
//
class IGameEngine
{
public:

	// Just here to stop warnings on non-virtual destructor in gcc builds
	virtual ~IGameEngine() {};

	// Check if the game engine is initialized ok and ready for use
	virtual bool BReadyForUse() = 0;

	// Check if the engine is shutting down
	virtual bool BShuttingDown() = 0;

	// Set the background color
	virtual void SetBackgroundColor( short a, short r, short g, short b ) = 0;

	// Start a frame, clear(), beginscene(), etc
	virtual bool StartFrame() = 0;

	// Finish a frame, endscene(), present(), etc.
	virtual void EndFrame() = 0;

	// Shutdown the game engine
	virtual void Shutdown() = 0;

	// Pump messages from the OS
	virtual void MessagePump() = 0;

	// Accessors for game screen size
	virtual int32 GetViewportWidth() = 0;
	virtual int32 GetViewportHeight() = 0;

	// Function for drawing text to the screen, dwFormat is a combination of flags like DT_LEFT, TEXTPOS_VCENTER etc...
	virtual bool BDrawString( HGAMEFONT hFont, RECT rect, DWORD dwColor, DWORD dwFormat, const char *pchText ) = 0;

	// Create a new font returning our internal handle value for it (0 means failure)
	virtual HGAMEFONT HCreateFont( int nHeight, int nFontWeight, bool bItalic, const char * pchFont ) = 0;

	// Create a new texture returning our internal handle value for it (0 means failure), texture type specifies the type of data contained in pData
	virtual HGAMETEXTURE HCreateTexture( byte *pData, uint32 uWidth, uint32 uHeight, ETEXTUREFORMAT eTextureFormat = eTextureFormat_RGBA ) = 0;

	// update an existing texture, texture type specifies the type of data contained in pData
	virtual bool UpdateTexture( HGAMETEXTURE texture, byte *pData, uint32 uWidth, uint32 uHeight, ETEXTUREFORMAT eTextureFormat = eTextureFormat_RGBA ) = 0;

	// Draw a line, the engine itself will manage batching these (although you can explicitly flush if you need to)
	virtual bool BDrawLine( float xPos0, float yPos0, DWORD dwColor0, float xPos1, float yPos1, DWORD dwColor1 ) = 0;

	// Flush the line buffer
	virtual bool BFlushLineBuffer() = 0;

	// Draw a point, the engine itself will manage batching these (although you can explicitly flush if you need to)
	virtual bool BDrawPoint( float xPos, float yPos, DWORD dwColor ) = 0;

	// Flush the point buffer
	virtual bool BFlushPointBuffer() = 0;

	// Draw a filled quad
	virtual bool BDrawFilledRect( float xPos0, float yPos0, float xPos1, float yPos1, DWORD dwColor ) = 0;

	// Draw a textured rectangle 
	virtual bool BDrawTexturedRect( float xPos0, float yPos0, float xPos1, float yPos1, 
		float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture ) = 0;

	// Draw a textured arbitrary quad
	virtual bool BDrawTexturedQuad( float xPos0, float yPos0, float xPos1, float yPos1, float xPos2, float yPos2, float xPos3, float yPos3,
		float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture ) = 0;

	// Flush any still cached quad buffers
	virtual bool BFlushQuadBuffer() = 0;

	// Get the current state of a key
	virtual bool BIsKeyDown( DWORD dwVK ) = 0;

	// Get the first (in some arbitrary order) key down, if any
	virtual bool BGetFirstKeyDown( DWORD *pdwVK ) = 0;

	// Return true if there is an active Steam Controller
	virtual bool BIsSteamInputDeviceActive() = 0;
		
	// Get the current state of a controller action
	virtual bool BIsControllerActionActive( ECONTROLLERDIGITALACTION dwAction ) = 0;

	// Find an active Steam controller
	virtual void FindActiveSteamInputDevice() = 0;

	// Get the current state of a controller analog action
	virtual void GetControllerAnalogAction( ECONTROLLERANALOGACTION dwAction, float *x, float *y ) = 0;

	// Set the current Steam Controller Action set
	virtual void SetSteamControllerActionSet( ECONTROLLERACTIONSET dwActionSet ) = 0;

	// Set an Action Set Layer for Steam Input
	virtual void ActivateSteamControllerActionSetLayer( ECONTROLLERACTIONSET dwActionSet ) = 0;
	virtual void DeactivateSteamControllerActionSetLayer( ECONTROLLERACTIONSET dwActionSet ) = 0;

	// Returns whether a given action set layer is active
	virtual bool BIsActionSetLayerActive( ECONTROLLERACTIONSET dwActionSetLayer ) = 0;

	// These calls return a string describing which controller button the action is currently bound to
	virtual const char *GetTextStringForControllerOriginDigital( ECONTROLLERACTIONSET dwActionSet, ECONTROLLERDIGITALACTION dwDigitalAction ) = 0;
	virtual const char *GetTextStringForControllerOriginAnalog( ECONTROLLERACTIONSET dwActionSet, ECONTROLLERANALOGACTION dwDigitalAction ) = 0;

	virtual void SetControllerColor( uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags ) = 0;
	virtual void SetTriggerEffect( bool bEnabled ) = 0;
	virtual void TriggerControllerVibration( unsigned short nLeftSpeed, unsigned short nRightSpeed ) = 0;
	virtual void TriggerControllerHaptics( ESteamControllerPad ePad, unsigned short usOnMicroSec, unsigned short usOffMicroSec, unsigned short usRepeat ) = 0;

	// Get current tick count for the game engine
	virtual uint64 GetGameTickCount() = 0;

	// Tell the game engine to update current tick count
	virtual void UpdateGameTickCount() = 0;

	// Tell the game engine to sleep for a bit if needed to limit frame rate.  Returns
	// true if you need to keep calling it to sleep more to reach your limit, returns
	// false when you should proceed to the next frame.
	virtual bool BSleepForFrameRateLimit( uint32 ulMaxFrameRate ) = 0;

	// Get the tick count elapsed since the previous frame
	// bugbug - We use this time to compute things like thrust and acceleration in the game,
	//			so it's important in doesn't jump ahead by large increments... Need a better
	//			way to handle that.  
	virtual uint64 GetGameTicksFrameDelta() = 0;

	// Check if the game engine hwnd currently has focus (and a working d3d device)
	virtual bool BGameEngineHasFocus() = 0;

	// Voice chat functions
	virtual HGAMEVOICECHANNEL HCreateVoiceChannel() = 0;
	virtual void DestroyVoiceChannel( HGAMEVOICECHANNEL hChannel ) = 0;
	virtual bool AddVoiceData( HGAMEVOICECHANNEL hChannel, const uint8 *pVoiceData, uint32 uLength ) = 0;
};

#endif // GAMEENGINE_H
