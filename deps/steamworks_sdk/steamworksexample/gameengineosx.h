//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the game engine -- osx implementation
//
// $NoKeywords: $
//=============================================================================

#ifndef GAMEENGINEOSX_H
#define GAMEENGINEOSX_H

#ifdef __OBJC__
#define OBJC_ENABLED 1
#else
#define OBJC_ENABLED 0
#endif

typedef unsigned char byte;

#include "steam/steam_api.h"
#include "GameEngine.h"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenGL/OpenGL.h>
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




#ifndef DX9MODE
#define DX9MODE 0	// change to 1 to turn on the DX9 mode
#endif

#if DX9MODE
#include "../glmgr/dxabstract.h"

class CShowPixelsParams
{
public:
	GLuint					m_srcTexName;
	int						m_width,m_height;
};

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

// Vertex struct for textured quads
struct TexturedQuadVertex_t
{
	float x, y, z, rhw;
	DWORD color;
	float u, v; // texture coordinates
};

#endif


class CVoiceContext;

class CGameEngineGL : public IGameEngine
{
public:

	// Constructor
	CGameEngineGL();

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
	// on OSX client with DX9MODE=1, the HGAMEFONT is a texture with the glyphs in a 16x16 grid (see "g_glmDebugFontMap" in glmgrbasics.cpp)
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

	// Return true if there is an active Steam Input device
	bool BIsSteamInputDeviceActive( );

	// Find the active device
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

	// Initialize the Steam Input interface
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

	#if DX9MODE
	#else
		void AdjustViewport();
	
	#endif

	// Initialize graphics in either GL or DX9 form
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

	// Size of the window to display the game in
	int32 m_nWindowWidth;
	int32 m_nWindowHeight;

	// Current game time in milliseconds
	uint64 m_ulGameTickCount;

	// Game time at the start of the previous frame
	uint64 m_ulPreviousGameTickCount;

	#if DX9MODE
		// Windows code transplants--------------------------------------------------------

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

		// set vertex decl
		bool BSetVertexDeclaration( IDirect3DVertexDeclaration9 *decl );
		
		// Set stream source
		bool BSetStreamSource( uint streamNumber, HGAMEVERTBUF hVertBuf, uint32 uOffset, uint32 uStride );

		// Set shaders
		bool BSetShaders( IDirect3DVertexShader9 *vsh, IDirect3DPixelShader9 *psh );

		// Render primitives out of the current stream source
		bool BRenderPrimitive( D3DPRIMITIVETYPE primType, uint32 uStartVertex, uint32 uCount );

		bool BUberRenderPrimitive(	IDirect3DVertexShader9 *vsh,
									IDirect3DPixelShader9 *psh,
									IDirect3DVertexDeclaration9 *decl,
									uint streamNumber,
									HGAMEVERTBUF hVertBuf,
									uint32 uOffset,
									uint32 uStride,
									D3DPRIMITIVETYPE primType,
									uint32 uStartVertex,
									uint32 uCount );

		// fake hwnd is a WindowRef ( = [m_window windowRef] )
		void				*m_hwnd;
		
		// IDirect3D9 interface
		IDirect3D9			*m_pD3D9Interface;

		// IDirect3DDevice9 interface
		IDirect3DDevice9	*m_pD3D9Device;

		// Presentation parameters - device resets don't happen on OS X, but just for commonality with windows code
		D3DPRESENT_PARAMETERS m_d3dpp;

		// vertex declarations - one per vertex layout used
		IDirect3DVertexDeclaration9	*m_decl_P4C1;				// aka D3DFVF_XYZRHW | D3DFVF_DIFFUSE
																// (4 floats pos, one ubyte4 color) - 20 bytes / vert
																
		IDirect3DVertexDeclaration9	*m_decl_P4C1T2;				// aka D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1
																// (4 floats pos, one ubyte4 color, 2 floats texc) - 28 bytes / vert

		// vertex and pixel shaders - one per vertex layout
		IDirect3DVertexShader9	*m_vsh_P4C1;
		IDirect3DVertexShader9	*m_vsh_P4C1T2;
		IDirect3DPixelShader9	*m_psh_P4C1;
		IDirect3DPixelShader9	*m_psh_P4C1T2;

		
		// Color we clear the background of the window to each frame
		DWORD m_dwBackgroundColor;

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
		};
		std::map<HGAMETEXTURE, TextureData_t> m_MapTextures;

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

		// White texture used when drawing filled quads
		HGAMETEXTURE m_hTextureWhite;
		
		// font stuff
		HGAMEFONT m_nNextFontHandle;
		std::map< HGAMEFONT, HGAMETEXTURE > m_MapGameFonts;
		
		//CocoaMgr transplants-------------------------------------------------------------		
		GLMDisplayDB			*GetDisplayDB	( void );
		void					GetRendererInfo	( GLMRendererInfoFields *rendInfoOut );
		PseudoNSGLContextPtr	GetNSGLContextForWindow( void* windowref );
		void					RenderedSize	( uint &width, uint &height, bool set );	// either set or retrieve rendered size value (from dxabstract)
		void					DisplayedSize	( uint &width, uint &height );				// query backbuffer size (window size whether FS or windowed)
		void					ShowPixels		( CShowPixelsParams *params );				// present


		GLMDisplayDB			*m_displayDB;
	#else
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
		std::map< HGAMEFONT, void* > m_MapGameFonts;
	
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
	#endif

	// Map of button state, translated to VK for win32.
	std::set< DWORD > m_SetKeysDown;
	
	ALCcontext* m_palContext;
	ALCdevice* m_palDevice;

	// Map of voice handles
	std::map<HGAMEVOICECHANNEL, CVoiceContext* > m_MapVoiceChannel;
	uint32 m_unVoiceChannelCount;

#if OBJC_ENABLED
	// any objective-c members go at the end of the class in a block
	// they are invisible to callers in pure C++ files
	// that also means callers in pure C++ files must not try to instantiate or destroy this type of object

	#if DX9MODE
	#else
		std::map< std::string, GLString * > m_MapStrings;
	#endif

	NSOpenGLView	*m_view;
	NSWindow		*m_window;	
#endif

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

#endif // GAMEENGINEOSX_H
