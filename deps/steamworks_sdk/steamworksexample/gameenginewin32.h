//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the game engine -- win32 implementation
//
// $NoKeywords: $
//=============================================================================

#ifndef GAMEENGINEWIN32_H
#define GAMEENGINEWIN32_H

#include "GameEngine.h"
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



// Vertex struct for line batches
struct LineVertex_t
{
	float x, y, z, rhw;
	DWORD color;
};


// Vertex struct for point batches
struct PointVertex_t
{
	float x, y, z, rhw;
	DWORD color;
};

// Vertex struct for textured quads in pixel space
struct TexturedQuadVertex_t
{
	float x, y, z, rhw;
	DWORD color;
	float u, v; // texture coordinates
};

// Vertex struct for textured quads in 3D space
struct Textured3DQuadVertex_t
{
	float x, y, z;
	DWORD color;
	float u, v; // texture coordinates
};

class CVoiceContext;

// WndProc declaration
LRESULT CALLBACK GameWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

class CGameEngineWin32 : public IGameEngine
{
public:

	// Static methods for tracing mapping of game engine class instances to hwnds
	static CGameEngineWin32 * FindEngineInstanceForHWND( HWND hWnd );
	static void AddInstanceToHWNDMap( CGameEngineWin32* pInstance, HWND hWnd );
	static void RemoveInstanceFromHWNDMap( HWND hWnd );

	// Constructor
	CGameEngineWin32( HINSTANCE hInstance, int nShowCommand, int32 nWindowWidth, int32 nWindowHeight );

	// Destructor
	~CGameEngineWin32() { Shutdown(); }

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
	int32 GetViewportWidth() { return m_nViewportWidth; }
	int32 GetViewportHeight() { return m_nViewportHeight; }

	// Function for drawing text to the screen, dwFormat is a combination of flags like DT_LEFT, DT_VCENTER etc...
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

	// Draw a textured rectangle with full 3D points
	bool BDraw3DTexturedQuad( Textured3DQuadVertex_t vert[4], HGAMETEXTURE hTexture );

	// Flush any still cached quad buffers
	bool BFlush3DQuadBuffer();

	// sets the texture as the 0th one to draw with
	bool BSetTexture( HGAMETEXTURE hTexture  );

	// sets the texture as a render target. 
	bool BSetRenderTarget( HGAMETEXTURE hTexture );

	// sets the render target back to the frame buffer
	bool BUnsetRenderTarget();
	
	// make sure the texture is created on the device and ready to use
	bool BReadyTexture( HGAMETEXTURE hTexture );

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

	// Called each frame to update the Steam Controller interface
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
	bool BGameEngineHasFocus() { return ::GetForegroundWindow() == m_hWnd && !m_bDeviceLost; }

	// voice chat sound engine
	virtual HGAMEVOICECHANNEL HCreateVoiceChannel();
	virtual void DestroyVoiceChannel( HGAMEVOICECHANNEL hChannel );
	virtual bool AddVoiceData( HGAMEVOICECHANNEL hChannel, const uint8 *pVoiceData, uint32 uLength );

	// Track the state of keyboard input (these are public, but not part of the public interface)
	void RecordKeyDown( DWORD dwVK );
	void RecordKeyUp( DWORD dwVK );

private:

	// Creates the hwnd for the game
	bool BCreateGameWindow( int nShowCommand );

	// Initializes D3D for the game
	bool BInitializeD3D9();

	// Resets all the render, texture, and sampler states to our defaults
	void ResetRenderStates();

	// Create a new vertex buffer returning our internal handle for it (0 means failure)
	HGAMEVERTBUF HCreateVertexBuffer( uint32 nSizeInBytes, DWORD dwUsage, DWORD dwFVF );

	// Lock an entire vertex buffer with the specified flags
	bool BLockEntireVertexBuffer( HGAMEVERTBUF hVertBuf, void **ppVoid, DWORD dwFlags );

	// Unlock a vertex buffer
	bool BUnlockVertexBuffer( HGAMEVERTBUF hVertBuf );

	// Release a vertex buffer and free its resources
	bool BReleaseVertexBuffer( HGAMEVERTBUF hVertBuf );

	// Set steam source
	bool BSetStreamSource( HGAMEVERTBUF hVertBuf, uint32 uOffset, uint32 uStride );

	// Render primitives out of the current stream source
	bool BRenderPrimitive( D3DPRIMITIVETYPE primType, uint32 uStartVertex, uint32 uCount );

	// Set vertex format
	bool BSetFVF( DWORD dwFormat );

	// Handle losing the d3d device (ie, release resources)
	bool BHandleLostDevice();

	// Handle reseting the d3d device (ie, acquire resources again)
	bool BHandleResetDevice();

private:
	// Tracks whether the engine is ready for use
	bool m_bEngineReadyForUse;

	// Tracks if we are shutting down
	bool m_bShuttingDown;

	// Color we clear the background of the window to each frame
	DWORD m_dwBackgroundColor;

	// HInstance for the application running the engine
	HINSTANCE m_hInstance;

	// HWND for the engine instance
	HWND m_hWnd;

	// IDirect3D9 interface
	IDirect3D9 *m_pD3D9Interface;

	// IDirect3DDevice9 interface
	IDirect3DDevice9 *m_pD3D9Device;

	// Depth/stencil surface associated with the back buffer
	IDirect3DSurface9 *m_pBackbufferDepth;

	// Size of the window to display the game in
	int32 m_nWindowWidth;
	int32 m_nWindowHeight;

	// Size of actual d3d viewport (window size minus borders, title, etc)
	int32 m_nViewportWidth;
	int32 m_nViewportHeight;

	// Next font handle value to give out
	HGAMEFONT m_nNextFontHandle;

	// Map of font handles to font objects
	std::map<HGAMEFONT, ID3DXFont *> m_MapFontInstances;

	// Next vertex buffer handle value to give out
	HGAMEVERTBUF m_nNextVertBufferHandle;

	// Map of handles to vertex buffer objects
	struct VertBufData_t
	{
		bool m_bIsLocked;
		IDirect3DVertexBuffer9 * m_pBuffer;
	};
	std::map<HGAMEVERTBUF, VertBufData_t> m_MapVertexBuffers;

	HGAMETEXTURE m_nNextTextureHandle;

	// Map of handles to texture objects
	struct TextureData_t
	{
		byte *m_pRGBAData; // We keep a copy of the raw data so we can rebuild textures after a device is lost
		uint32 m_uWidth;
		uint32 m_uHeight;
		LPDIRECT3DTEXTURE9 m_pTexture;
		LPDIRECT3DSURFACE9 m_pDepthSurface; // render targets only
		D3DFORMAT m_eFormat; // format for the texture on the card itself
		ETEXTUREFORMAT m_eTextureFormat; // format of the data you provide
	};
	std::map<HGAMETEXTURE, TextureData_t> m_MapTextures;

	// Vertex buffer for textured quads
	HGAMEVERTBUF m_hQuadBuffer;

	// Last texture used in drawing a batched quad
	HGAMETEXTURE m_hLastTexture;

	// Pointer to quad vertex data
	TexturedQuadVertex_t *m_pQuadVertexes;

	// How many quads are awaiting flushing
	DWORD m_dwQuadsToFlush;

	// Where does the current batch begin
	DWORD m_dwQuadBufferBatchPos;

	// Vertex buffer for textured quads
	HGAMEVERTBUF m_h3DQuadBuffer;

	// Last texture used in drawing a batched quad
	HGAMETEXTURE m_h3DLastTexture;

	// Pointer to quad 3D vertex data
	Textured3DQuadVertex_t *m_p3DQuadVertexes;

	// How many 3D quads are awaiting flushing
	DWORD m_dw3DQuadsToFlush;

	// Where does the current 3D batch begin
	DWORD m_dw3DQuadBufferBatchPos;

	// White texture used when drawing filled quads
	HGAMETEXTURE m_hTextureWhite;

	// Currently set FVF format
	DWORD m_dwCurrentFVF;

	// Map of key state
	std::set<DWORD> m_SetKeysDown;

	// Current game time in milliseconds
	uint64 m_ulGameTickCount;

	// Game time at the start of the previous frame
	uint64 m_ulPreviousGameTickCount;

	// Divisor for turning QPC values to milliseconds
	uint64 m_ulPerfCounterToMillisecondsDivisor;

	// First value for QPC when we started the process
	uint64 m_ulFirstQueryPerformanceCounterValue;

	// Map of engine instances by HWND, used in wndproc to find engine instance for messages
	static std::map<HWND, CGameEngineWin32 *> m_MapEngineInstances;

	// Internal vertex buffer for batching line drawing
	HGAMEVERTBUF m_hLineBuffer;

	// Pointer to actual line buffer memory (valid only while locked)
	LineVertex_t *m_pLineVertexes;

	// Track how many lines are awaiting flushing in our line buffer
	DWORD m_dwLinesToFlush;

	// Track where the current batch starts in the vert buffer
	DWORD m_dwLineBufferBatchPos;

	// Internal vertex buffer for batching point drawing
	HGAMEVERTBUF m_hPointBuffer;

	// Pointer to actual point buffer memory (valid only while locked)
	PointVertex_t *m_pPointVertexes;

	// Track how many points are awaiting flushing in our line buffer
	DWORD m_dwPointsToFlush;

	// Track where the current batch starts in the vert buffer
	DWORD m_dwPointBufferBatchPos;

	// Track if we have lost the d3d device
	bool m_bDeviceLost;

	// Presentation parameters, saved in case of lost device needing reset
	D3DPRESENT_PARAMETERS m_d3dpp;

	IXAudio2* m_pXAudio2;
	IXAudio2MasteringVoice* m_pMasteringVoice;

	// Map of font handles to font objects
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
};

#endif // GAMEENGINEWIN32_H
