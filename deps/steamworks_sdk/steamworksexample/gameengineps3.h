//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the game engine -- ps3 implementation
//
// $NoKeywords: $
//=============================================================================

#ifndef GAMEENGINEPS3_H
#define GAMEENGINEPS3_H

#include "GameEngine.h"
#include <set>
#include <map>


// Font info for PS3 dbg font output
struct PS3DbgFont_t
{
	float m_nScale;
};

class CVoiceContext;

class CGameEnginePS3 : public IGameEngine, public ISteamPS3OverlayRenderHost
{
public:

	// Static methods for tracing mapping of game engine class instances to hwnds
	static CGameEnginePS3 * FindEngineInstanceForPtr( void *ptr );
	static void AddInstanceToPtrMap( CGameEnginePS3* pInstance );
	static void RemoveInstanceFromPtrMap( void *ptr );

	// Constructor
	CGameEnginePS3();

	// Destructor
	~CGameEnginePS3() { Shutdown(); }

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
	HGAMETEXTURE HCreateTexture( byte *pRGBAData, uint32 uWidth, uint32 uHeight );

	// Draw a line, the engine itself will manage batching these (although you can explicitly flush if you need to)
	bool BDrawLine( float xPos0, float yPos0, DWORD dwColor0, float xPos1, float yPos1, DWORD dwColor1 );

	// Flush the line buffer
	bool BFlushLineBuffer();

	// Draw a point, the engine itself will manage batching these (although you can explicitly flush if you need to)
	bool BDrawPoint( float xPos, float yPos, DWORD dwColor );

	// Flush the point buffer
	bool BFlushPointBuffer();

	// Draw a filled quad
	bool BDrawFilledQuad( float xPos0, float yPos0, float xPos1, float yPos1, DWORD dwColor );

	// Draw a textured rectangle 
	bool BDrawTexturedQuad( float xPos0, float yPos0, float xPos1, float yPos1, 
		float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture );

	// Flush any still cached quad buffers
	bool BFlushQuadBuffer();

	// Get the current state of a key
	bool BIsKeyDown( DWORD dwVK );

	// Get the first (in some arbitrary order) key down, if any
	bool BGetFirstKeyDown( DWORD *pdwVK );

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

	// ISteamPS3OverlayRenderHost implementation
	virtual void DrawTexturedRect( int x0, int y0, int x1, int y1, float u0, float v0, float u1, float v1, int32 iTextureID, DWORD colorStart, DWORD colorEnd, EOverlayGradientDirection eDirection );
	virtual void LoadOrUpdateTexture( int32 iTextureID, bool bIsFullTexture, int x0, int y0, uint32 uWidth, uint32 uHeight, int32 iBytes, char *pData );
	virtual void DeleteTexture( int32 iTextureID );
	virtual void DeleteAllTextures();

private:

	// Draw a textured rectangle 
	bool BDrawTexturedGradientQuad( float xPos0, float yPos0, float xPos1, float yPos1, 
		float u0, float v0, float u1, float v1,
		DWORD dwColorTopLeft, DWORD dwColorTopRight, DWORD dwColorBottomLeft, DWORD dwColorBottomRight, HGAMETEXTURE hTexture );


	// Initialize the PSGL rendering interfaces and default state
	bool BInitializePSGL();

	// Initialize the debug font library
	bool BInitializeCellDbgFont();

	// Initialize libpad for controller input
	bool BInitializeLibPad();

	bool BInitializeAudio();

	void RunAudio();


private:
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

	// White texture used when drawing filled quads
	HGAMETEXTURE m_hTextureWhite;

	// PSGL and CellDbgFont objects
	PSGLcontext * m_pPSGLContext;
	PSGLdevice * m_pPSGLDevice;
	CellDbgFontConsoleId m_DbgFontConsoleID;

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

	// Currently active PS3 pad index to use for input
	int m_iCurrentPadIndex;

	// Map of engine instances by ptr, used in ps3 sys callbacks to find engine instance to handle callback
	static std::map<void *, CGameEnginePS3 *> m_MapEngineInstances;

	// Map of font handles we have given out
	HGAMEFONT m_nNextFontHandle;
	std::map< HGAMEFONT, PS3DbgFont_t > m_MapGameFonts;

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

	// Map of voice handles
	std::map<HGAMEVOICECHANNEL, CVoiceContext* > m_MapVoiceChannel;
	uint32 m_unVoiceChannelCount;

	// Map of Steam texture ids to our engine texture handles
	std::map< int, HGAMETEXTURE> m_MapSteamTextures;

};

#endif // GAMEENGINEPS3_H
