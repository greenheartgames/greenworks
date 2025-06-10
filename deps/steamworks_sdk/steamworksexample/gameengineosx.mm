//========= Copyright 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Main class for the game engine -- osx implementation
//
// $NoKeywords: $
//=============================================================================

#include "stdafx.h"

#include <Cocoa/Cocoa.h>
#include <map>
#include <queue>
#include <sys/time.h>

#include "glstringosx.h"
#include "gameengineosx.h"

#include "steam/isteamdualsense.h"

#if MAC_OS_X_VERSION_MIN_REQUIRED < 101400
#define NSOpenGLContextParameterSwapInterval NSOpenGLCPSwapInterval

#if MAC_OS_X_VERSION_MIN_REQUIRED < 101200
#define NSWindowStyleMaskTitled NSTitledWindowMask
#define NSWindowStyleMaskClosable  NSClosableWindowMask
#define NSWindowStyleMaskResizable NSResizableWindowMask

#define NSEventTypeKeyDown NSKeyDown
#define NSEventTypeKeyUp NSKeyUp
#define NSEventTypeFlagsChanged NSFlagsChanged

#define NSEventModifierFlagShift NSShiftKeyMask
#define NSEventModifierFlagControl NSControlKeyMask
#define NSEventModifierFlagOption NSAlternateKeyMask

#define NSEventMaskAny NSAnyEventMask
#endif

#endif

CGameEngineGL *g_engine;		// dxabstract will use this.. it is set by the engine constructor

IGameEngine *CreateGameEngineOSX()
{
	static CGameEngineGL* s_pGameEngine = NULL;
	
	if (!s_pGameEngine)
	{
		s_pGameEngine = new CGameEngineGL();
	}
	
	return s_pGameEngine;
}

uint64_t GetTickCount()
{
	timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000ULL) + (time.tv_usec / 1000ULL);
}

void OutputDebugString( const char *pchMsg )
{
	fprintf( stderr, "%s", pchMsg );
}

struct Packet_t
{
	uint32 unSize;
	void *pData;
};

class CVoiceContext 
{
public:
	CVoiceContext() 
	{
		alGenBuffers( ARRAYSIZE(m_buffers), m_buffers );
		alGenSources( 1, &m_nSource );
		
		alSourcei( m_nSource, AL_LOOPING, AL_FALSE );
		
		for (int i = 0; i < ARRAYSIZE(m_buffers); i++ )
			alSourcei( m_nSource, AL_BUFFER, m_buffers[i] ); 
		
		m_nNextFreeBuffer = 0;
	}
	virtual ~CVoiceContext()
	{
		// 
	}

	ALuint m_buffers[4];
	ALuint m_nSource;
	size_t m_nNextFreeBuffer;
	std::queue<Packet_t> m_pending;
};

class AutoReleasePool
{
public:
	AutoReleasePool()
	{
		pool = [[NSAutoreleasePool alloc] init];
	}
	~AutoReleasePool()
	{
		[pool release];
	}
	
	NSAutoreleasePool *pool;
};


@interface GLApplication : NSApplication
{
	
}
- (void)sendEvent:(NSEvent *)anEvent;
- (void)windowWillClose:(NSNotification *)notification;
@end

@implementation GLApplication
- (void)sendEvent:(NSEvent *)anEvent
{
	//fprintf( stderr, "sendEvent: %s\n", [[anEvent description] UTF8String] );
	
	[super sendEvent:anEvent];
}

- (void)windowWillClose:(NSNotification *)notification
{
	CreateGameEngineOSX()->Shutdown();
}

@end


//-----------------------------------------------------------------------------
// Purpose: Constructor for game engine instance
//-----------------------------------------------------------------------------
CGameEngineGL::CGameEngineGL()
{
	g_engine = this;
	
	m_bEngineReadyForUse = false;
	m_bShuttingDown = false;
	m_nWindowWidth = 0;
	m_nWindowHeight = 0;
	m_ulPreviousGameTickCount = 0;
	m_ulGameTickCount = 0;
	m_unVoiceChannelCount = 0;

	
	#if DX9MODE

		m_hwnd				= NULL;
		m_pD3D9Interface	= NULL;
		m_pD3D9Device		= NULL;

		m_displayDB			= NULL;

		m_dwBackgroundColor = D3DCOLOR_ARGB(0, 255, 255, 255 );

		m_nNextVertBufferHandle = 1;
		m_nNextTextureHandle = 1;

		m_hLineBuffer = 0;
		m_pLineVertexes = NULL;
		m_dwLinesToFlush = 0;
		m_dwLineBufferBatchPos = 0;

		m_hPointBuffer = NULL;
		m_pPointVertexes = NULL;
		m_dwPointsToFlush = 0;
		m_dwPointBufferBatchPos = 0;
		
		m_hQuadBuffer = NULL;
		m_pQuadVertexes = NULL;
		m_dwQuadsToFlush = 0;
		m_dwQuadBufferBatchPos = 0;

		m_hTextureWhite = NULL;

		m_nNextFontHandle = 1;
	#else

		m_hTextureWhite = 0;
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

	#endif
	
	if( !BInitializeGraphics() )
	{
		OutputDebugString( "!! Initializing graphics failed\n" );
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
void CGameEngineGL::Shutdown()
{
	// Flag that we are shutting down so the frame loop will stop running
	m_bShuttingDown = true;

	#if DX9MODE
	#else
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
	
		std::map<std::string, GLString *>::const_iterator i;
		for (i = m_MapStrings.begin(); i != m_MapStrings.end(); ++i)
		{
			[i->second release];
		}
		
		m_MapStrings.clear();
		
		m_dwLinesToFlush = 0;
		m_dwPointsToFlush = 0;
		m_dwQuadsToFlush = 0;
	#endif
}


//-----------------------------------------------------------------------------
// Purpose: Initialize voice/audio interfaces
//-----------------------------------------------------------------------------
bool CGameEngineGL::BInitializeAudio()
{
	m_palDevice = alcOpenDevice(NULL);
	if ( m_palDevice ) 
	{
		m_palContext = alcCreateContext( m_palDevice, NULL );
		alcMakeContextCurrent( m_palContext );
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize the GL rendering interfaces and default state
//-----------------------------------------------------------------------------
#define D3DADAPTER_DEFAULT 0

bool CGameEngineGL::BInitializeGraphics()
{
	m_nWindowWidth = 1024;
	m_nWindowHeight = 768;

	AutoReleasePool pool;

	ProcessSerialNumber psn = { 0, kCurrentProcess };
	TransformProcessType( &psn, kProcessTransformToForegroundApplication );
	[[GLApplication sharedApplication] activateIgnoringOtherApps: YES];

	uint32_t mask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;

	GLuint attribs[] = 
	{
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFANoRecovery,
		NSOpenGLPFABackingStore,
		NSOpenGLPFAColorSize, 32,
		NSOpenGLPFADepthSize, 16,
		0
	};
	
	NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes: (NSOpenGLPixelFormatAttribute*) attribs];

	NSApplicationLoad();

	m_view = [[NSOpenGLView alloc] initWithFrame:NSMakeRect( 0, 0, m_nWindowWidth, m_nWindowHeight )
									 pixelFormat:pixelFormat];

	m_view.wantsBestResolutionOpenGLSurface = NO;

	
	int wherex = 50;
	int wherey = 50;
	m_window = [[NSWindow alloc] initWithContentRect:NSMakeRect( wherex, wherey, m_nWindowWidth, m_nWindowHeight )
										   styleMask:mask
											 backing:NSBackingStoreBuffered
											   defer:NO];

	[m_window setAcceptsMouseMovedEvents:YES];
	
	GLint swapInt = 1;
	[[m_view openGLContext] setValues:&swapInt forParameter:NSOpenGLContextParameterSwapInterval];
	[[m_view openGLContext] makeCurrentContext];

	[m_window setContentView:m_view];
	
	[m_window makeKeyAndOrderFront:nil];
	// [m_view setPostsFrameChangedNotifications:YES];

#if DX9MODE
	GLMgr::NewGLMgr();				// init GL manager

	m_hwnd = (void*) [m_window windowRef];

	// code transplanted from Windows sample.
	if ( !m_pD3D9Interface )
	{
		// Initialize the d3d interface
		m_pD3D9Interface = Direct3DCreate9( D3D_SDK_VERSION );
		if ( m_pD3D9Interface == NULL )
		{
			OutputDebugString( "Direct3DCreate9 failed\n" );
			return false;
		}
	}

	if ( !m_pD3D9Device )
	{
		D3DDISPLAYMODE d3ddisplaymode;

		// Get the current desktop display mode, only needed if running in a window.
		HRESULT hRes = m_pD3D9Interface->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddisplaymode );
		if (FAILED(hRes))
		{
			OutputDebugString( "GetAdapterDisplayMode failed\n");
			return false;
		}

		// Setup presentation parameters
		ZeroMemory( &m_d3dpp, sizeof( m_d3dpp ) );
		m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; 
		m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; // no v-sync
		m_d3dpp.hDeviceWindow = m_hwnd;
		m_d3dpp.BackBufferCount = 1; 
		m_d3dpp.EnableAutoDepthStencil = TRUE;
		m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		m_d3dpp.Windowed = TRUE; // bugbug jmccaskey - make a parameter?

		m_d3dpp.BackBufferWidth = m_nWindowWidth;
		m_d3dpp.BackBufferHeight = m_nWindowHeight;	// get it officially later on
		m_d3dpp.BackBufferFormat  = d3ddisplaymode.Format; 
		m_d3dpp.BackBufferCount = 1;

		m_d3dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)0;
		m_d3dpp.MultiSampleQuality = 0;

		// Create Direct3D9 device 
		// (if it fails to create hardware vertex processing, then go with the software alternative).
		hRes = m_pD3D9Interface->CreateDevice(	
			D3DADAPTER_DEFAULT, 
			D3DDEVTYPE_HAL,	
			m_hwnd, 
			D3DCREATE_HARDWARE_VERTEXPROCESSING, 
			&m_d3dpp, 
			&m_pD3D9Device );

		// Could not create a hardware device, create a software one instead (slow....)
		if ( FAILED( hRes ) )
		{
			hRes = m_pD3D9Interface->CreateDevice(
				D3DADAPTER_DEFAULT, 
				D3DDEVTYPE_HAL, 
				m_hwnd, 
				D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
				&m_d3dpp, 
				&m_pD3D9Device );
		}

		// If we couldn't create a device even with software vertex processing then 
		// it's a fatal error
		if ( FAILED( hRes ) )
		{
			// Make sure the pointer is NULL after failures (seems it sometimes gets modified even when failing)
			m_pD3D9Device = NULL;

			OutputDebugString( "Failed to create D3D9 device\n" );
			return false;
		}

		//Initialize our render, texture, and sampler stage states
		ResetRenderStates();
	}

	[[m_view openGLContext] update];

#else
	// Clear any errors
	glGetError();

	glClearDepth( 1.0f );
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

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

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef( 0, 0, 0 );
	
	glMatrixMode( GL_TEXTURE );
	glLoadIdentity();
	glTranslatef( 0, 0, 0 );

	glDepthRange( 0.0f, 1.0f );
	
	AdjustViewport();
#endif

	return true;
}

#if DX9MODE

	void CGameEngineGL::ResetRenderStates()
	{
		// Since we are just a really basic rendering engine we'll setup our initial 
		// render states here and we can just assume that they don't change later
		m_pD3D9Device->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pD3D9Device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pD3D9Device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_pD3D9Device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		m_pD3D9Device->SetRenderState( D3DRS_ZENABLE, FALSE );
		m_pD3D9Device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		m_pD3D9Device->SetRenderState( D3DRS_COLORWRITEENABLE, (D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );
		m_pD3D9Device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		
		m_pD3D9Device->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		m_pD3D9Device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
		m_pD3D9Device->SetRenderState( D3DRS_FOGENABLE, FALSE );
		m_pD3D9Device->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
		m_pD3D9Device->SetRenderState( D3DRS_CLIPPING, FALSE );


		m_pD3D9Device->SetRenderState( D3DRS_LIGHTING, 0 );

		// texture stage state - not meaningful on all-shader impl
		#if 0
			m_pD3D9Device->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			m_pD3D9Device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pD3D9Device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
			m_pD3D9Device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			m_pD3D9Device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pD3D9Device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
		#endif
		
		// sampler state
		m_pD3D9Device->SetSamplerState(	0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pD3D9Device->SetSamplerState(	0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pD3D9Device->SetSamplerState(	0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		
		// vertex decls
		static D3DVERTEXELEMENT9 s_elems_P4C1[] = 
		{
			//	stream		offset		type					method		usage					usageindex
			{	0,			0,			D3DDECLTYPE_FLOAT4,		0,			D3DDECLUSAGE_POSITION,	0			},
			{	0,			16,			D3DDECLTYPE_D3DCOLOR,	0,			D3DDECLUSAGE_COLOR,		0			},
			{	0xFF,		0,			0,						0,			0,						0			}	// end marker
		};

		static D3DVERTEXELEMENT9 s_elems_P4C1T2[] = 
		{
			//	stream		offset		type					method		usage					usageindex
			{	0,			0,			D3DDECLTYPE_FLOAT4,		0,			D3DDECLUSAGE_POSITION,	0			},
			{	0,			16,			D3DDECLTYPE_D3DCOLOR,	0,			D3DDECLUSAGE_COLOR,		0			},
			{	0,			20,			D3DDECLTYPE_FLOAT2,		0,			D3DDECLUSAGE_TEXCOORD,	0			},
			{	0xFF,		0,			0,						0,			0,						0			}	// end marker
		};
		
		m_pD3D9Device->CreateVertexDeclaration( s_elems_P4C1,	&m_decl_P4C1 );
		m_pD3D9Device->CreateVertexDeclaration( s_elems_P4C1T2,	&m_decl_P4C1T2 );
		
		// vertex shaders
		
		static char s_vsh_P4C1[] =
			"//GLSLvp\n"
			"#version 120  \n"
			"//ATTRIBMAP-00-A0-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx  \n"
			"attribute vec4 v0;  \n"
			"attribute vec4 v1;  \n"
			"//SAMPLERMASK-0  \n"
			"//HIGHWATER-1  \n"
			"uniform vec4 vc[1];  \n"
			"varying vec4 oColor;  \n"
			"void main()  \n"
			"{  \n"
			"vec4 consts = vec4( 0.0, 0.5, 1.0, 2.0 );  \n"
			"vec4 bias = vec4( -1.0, -1.0, 0.0, 0.0 );  \n"
			"vec4 scaling = vec4( 2.0/800.0, 2.0/600.0, 1.0, 1.0 );  \n"
			"vec4 r0;  \n"
			"vec4 r1;  \n"
			"vec4 vTempPos;  \n"
			"oColor = v1; //.wzyx;  \n"	// flip the channels
			"vTempPos = (v0 * scaling) + bias;  \n"
			"vTempPos.z = vTempPos.z * consts.w - vTempPos.w; // z' = (2*z)-w  \n"
			"//vTempPos.y = -vTempPos.y; // y' = -y   \n"
			"gl_Position = vTempPos;  \n"
			"}  \n";
			
		static char s_vsh_P4C1T2[] =
			"//GLSLvp\n"
			"#version 120  \n"
			"//ATTRIBMAP-00-A0-50-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx-xx  \n"
			"attribute vec4 v0;  \n"
			"attribute vec4 v1;  \n"
			"attribute vec2 v2;  \n"
			"//SAMPLERMASK-1  \n"
			"//HIGHWATER-1  \n"
			"uniform vec4 vc[1];  \n"
			"varying vec4 oColor;  \n"
			"varying vec2 oTexC0;  \n"
			"void main()  \n"
			"{  \n"
			"vec4 consts = vec4( 0.0, 0.5, 1.0, 2.0 );  \n"
			"vec4 bias = vec4( -1.0, -1.0, 0.0, 0.0 );  \n"
			"vec4 scaling = vec4( 2.0/800.0, 2.0/600.0, 1.0, 1.0 );  \n"
			"vec4 r0;  \n"
			"vec4 r1;  \n"
			"vec4 vTempPos;  \n"
			"oColor = v1; //.wzyx;  \n"	// flip the channels
			"oTexC0 = v2;  \n"
			"vTempPos = (v0 * scaling)+bias;  \n"
			"vTempPos.z = vTempPos.z * consts.w - vTempPos.w; // z' = (2*z)-w  \n"
			"//vTempPos.y = -vTempPos.y; // y' = -y   \n"
			"gl_Position = vTempPos;  \n"
			"}  \n";
			
		
		m_pD3D9Device->CreateVertexShader( (const DWORD*) s_vsh_P4C1,	&m_vsh_P4C1, "vsh-P4C1", NULL);
		m_pD3D9Device->CreateVertexShader( (const DWORD*) s_vsh_P4C1T2,	&m_vsh_P4C1T2, "vsh-P4C1T2", NULL);

		static char s_psh_P4C1[] = 
			"//GLSLfp\n"
			"#version 120  \n"
			"//SAMPLERMASK-0  \n"
			"//HIGHWATER-1  \n"
			"varying vec4 oColor;  \n"
			"uniform vec4 pc[1];  \n"
			"void main()  \n"
			"{  \n"
			"gl_FragColor = oColor;\n"
			"}  \n";
			
		static char s_psh_P4C1T2[] =
			"//GLSLfp\n"
			"#version 120  \n"
			"//SAMPLERMASK-1  \n"
			"//HIGHWATER-1  \n"
			"varying vec4 oColor;  \n"
			"varying vec2 oTexC0;  \n"
			"uniform vec4 pc[1];  \n"
			"uniform sampler2D sampler0;  \n"
			"void main()  \n"
			"{  \n"
			"vec4 r0;  \n"
			"r0 = texture2D( sampler0, oTexC0.xy );  \n"
			"r0 = r0 * oColor;  \n"
			"gl_FragColor = r0;  \n"
			"}  \n";

		m_pD3D9Device->CreatePixelShader( (const DWORD*) s_psh_P4C1,	&m_psh_P4C1, "psh-P4C1", NULL);
		m_pD3D9Device->CreatePixelShader( (const DWORD*) s_psh_P4C1T2,	&m_psh_P4C1T2, "psh-P4C1T2", NULL);
	}

#else	
	void CGameEngineGL::AdjustViewport()
	{
		NSRect viewBounds = [m_view convertRectToBacking:m_view.bounds];
		m_nWindowWidth = viewBounds.size.width;
		m_nWindowHeight = viewBounds.size.height;

		// Perspective
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho( 0, m_nWindowWidth, m_nWindowHeight, 0, -1.0f, 1.0f );
		glTranslatef( 0, 0, 0 );
		
		// View port has changed as well
		glMatrixMode(GL_MODELVIEW);

		glViewport( 0, 0, m_nWindowWidth, m_nWindowHeight );
		glScissor( 0, 0, m_nWindowWidth, m_nWindowHeight );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
		glFlush();
	}
#endif

//-----------------------------------------------------------------------------
// Purpose: Updates current tick count for the game engine
//-----------------------------------------------------------------------------
void CGameEngineGL::UpdateGameTickCount()
{
	m_ulPreviousGameTickCount = m_ulGameTickCount;
	m_ulGameTickCount = GetTickCount();
}


//-----------------------------------------------------------------------------
// Purpose: Tell the game engine to sleep for a bit if needed to limit frame rate.  You must keep
// calling this repeatedly until it returns false.  If it returns true it's slept a little, but more
// time may be needed.
//-----------------------------------------------------------------------------
bool CGameEngineGL::BSleepForFrameRateLimit( uint32 ulMaxFrameRate )
{
	// Frame rate limiting
	float flDesiredFrameMilliseconds = 1000.0f/ulMaxFrameRate;

	uint64 ulGameTickCount = GetTickCount();

	float flMillisecondsElapsed = (float)(ulGameTickCount - m_ulGameTickCount);
	if ( flMillisecondsElapsed < flDesiredFrameMilliseconds )
	{
		// If enough time is left sleep, otherwise just keep spinning so we don't go over the limit...
		if ( flDesiredFrameMilliseconds - flMillisecondsElapsed > 3.0f )
		{
			usleep( 5000 );
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
void CGameEngineGL::SetBackgroundColor( short a, short r, short g, short b )
{
	#if DX9MODE
		m_dwBackgroundColor = D3DCOLOR_ARGB( a, r, g, b );
	#else
		glClearColor( (float)r/255.0f, (float)g/255.0f, (float)b/255.0f, (float)a/255.0f );
	#endif
}

//-----------------------------------------------------------------------------
// Purpose: Start a new frame
//-----------------------------------------------------------------------------
bool CGameEngineGL::StartFrame()
{
	#if DX9MODE
		m_pD3D9Device->BeginScene();
	#else
	    AdjustViewport();
	#endif
    
	// Pump system callbacks
	MessagePump();

	// We may now be shutting down, check and don't start a frame then
	if ( BShuttingDown() )
		return false;

	// Poll Steam Input devices
	PollSteamInput();

	#if DX9MODE
		uint bkcolor = m_dwBackgroundColor;

		#if 0 // for debug
			static unsigned char counter;
			counter++;
			bkcolor = ((uint)counter * 0x01010101) | 0xFF000000;

			bkcolor = 0x20202020;
		#endif
		
		m_pD3D9Device->Clear( 0, NULL, D3DCLEAR_TARGET, bkcolor, 0, 0 );		
	#else
		#if 0 // for debug
			static unsigned char counter;
			counter++;
			glClearColor( (float)counter/255.0f, (float)counter/255.0f, (float)counter/255.0f, (float)1.0f );
		#endif

		// Clear the screen for the new frame
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	#endif

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: End the current frame
//-----------------------------------------------------------------------------
void CGameEngineGL::EndFrame()
{
	if ( BShuttingDown() )
		return;

	// Flush point buffer
	BFlushPointBuffer();

	// Flush line buffer
	BFlushLineBuffer();

	// Flush quad buffer
	BFlushQuadBuffer();

	#if DX9MODE
		m_pD3D9Device->EndScene();
		m_pD3D9Device->Present( NULL, NULL, NULL, NULL );
	#else
		// Swap buffers now that everything is flushed
	    [[m_view openGLContext] flushBuffer];
	#endif
    
	RunAudio();
}

#if DX9MODE

//-----------------------------------------------------------------------------
// Purpose: Creates a new vertex buffer
//-----------------------------------------------------------------------------
HGAMEVERTBUF CGameEngineGL::HCreateVertexBuffer( uint32 nSizeInBytes, DWORD dwUsage, DWORD dwFVF )
{
	if ( !m_pD3D9Device )
		return false;

	// Create a vertex buffer object
	IDirect3DVertexBuffer9 *pVertBuffer;
	HRESULT hRes = m_pD3D9Device->CreateVertexBuffer( nSizeInBytes, dwUsage,
		dwFVF, D3DPOOL_DEFAULT, &pVertBuffer, NULL );
	if ( FAILED( hRes ) )
	{
		OutputDebugString( "Failed creating vertex buffer\n" );
		return 0;
	}

	HGAMEVERTBUF hVertBuf = m_nNextVertBufferHandle;
	++m_nNextVertBufferHandle;

	VertBufData_t data;
	data.m_bIsLocked = false;
	data.m_pBuffer = pVertBuffer;

	m_MapVertexBuffers[ hVertBuf ] = data;
	return hVertBuf;
}


//-----------------------------------------------------------------------------
// Purpose: Locks an entire vertex buffer with the specified flags into memory
//-----------------------------------------------------------------------------
bool CGameEngineGL::BLockEntireVertexBuffer( HGAMEVERTBUF hVertBuf, void **ppVoid, DWORD dwFlags )
{
	if ( !m_pD3D9Device )
		return false;

	if ( !hVertBuf )
	{
		OutputDebugString( "Someone is calling BLockEntireVertexBuffer() with a null handle\n" );
		return false;
	}

	// Find the vertex buffer object for the passed handle
	std::map<HGAMEVERTBUF, VertBufData_t>::iterator iter;
	iter = m_MapVertexBuffers.find( hVertBuf );
	if ( iter == m_MapVertexBuffers.end() )
	{
		OutputDebugString( "Invalid vertex buffer handle passed to BLockEntireVertexBuffer()\n" );
		return false;
	}

	// Make sure the pointer is valid
	if ( !iter->second.m_pBuffer )
	{
		OutputDebugString( "Pointer to vertex buffer is invalid (lost device and not recreated?)!\n" );
		return false;
	}


	// Make sure its not already locked
	if ( iter->second.m_bIsLocked )
	{
		OutputDebugString( "Trying to lock an already locked vertex buffer!\n" );
		return false;
	}

	// we have the buffer, try to lock it
	if( FAILED( iter->second.m_pBuffer->Lock( 0, 0, ppVoid, dwFlags ) ) )
	{
		OutputDebugString( "BLockEntireVertexBuffer call failed\n" );
		return false;
	}

	// Track that we are now locked
	iter->second.m_bIsLocked = true;

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Unlocks a vertex buffer
//-----------------------------------------------------------------------------
bool CGameEngineGL::BUnlockVertexBuffer( HGAMEVERTBUF hVertBuf )
{
	if ( !m_pD3D9Device )
		return false;

	if ( !hVertBuf )
	{
		OutputDebugString( "Someone is calling BUnlockVertexBuffer() with a null handle\n" );
		return false;
	}

	// Find the vertex buffer for the passed handle
	std::map<HGAMEVERTBUF, VertBufData_t>::iterator iter;
	iter = m_MapVertexBuffers.find( hVertBuf );
	if ( iter == m_MapVertexBuffers.end() )
	{
		OutputDebugString( "Invalid vertex buffer handle passed to BUnlockVertexBuffer()\n" );
		return false;
	}

	// Make sure the pointer is valid
	if ( !iter->second.m_pBuffer )
	{
		OutputDebugString( "Pointer to vertex buffer is invalid (lost device and not recreated?)!\n" );
		return false;
	}

	// Make sure we are locked if someone is trying to unlock
	if ( !iter->second.m_bIsLocked )
	{
		OutputDebugString( "Trying to unlock a vertex buffer that is not locked!\n" );
		return false;
	}

	// we have the buffer, try to lock it
	if( FAILED( iter->second.m_pBuffer->Unlock() ) )
	{
		OutputDebugString( "BUnlockVertexBuffer call failed\n" );
		return false;
	}

	// Track that we are now unlocked
	iter->second.m_bIsLocked = false;

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Release a vertex buffer and free its resources
//-----------------------------------------------------------------------------
bool CGameEngineGL::BReleaseVertexBuffer( HGAMEVERTBUF hVertBuf )
{
	if ( !m_pD3D9Device )
		return false;

	if ( !hVertBuf )
	{
		OutputDebugString( "Someone is calling BReleaseVertexBuffer() with a null handle\n" );
		return false;
	}

	// Find the vertex buffer object for the passed handle
	std::map<HGAMEVERTBUF, VertBufData_t>::iterator iter;
	iter = m_MapVertexBuffers.find( hVertBuf );
	if ( iter == m_MapVertexBuffers.end() )
	{
		OutputDebugString( "Invalid vertex buffer handle passed to BReleaseVertexBuffer()\n" );
		return false;
	}

	// Make sure the pointer is valid
	if ( !iter->second.m_pBuffer )
	{
		OutputDebugString( "Pointer to vertex buffer is invalid (lost device and not recreated?)!\n" );
		return false;
	}

	// Make sure its unlocked, if it isn't locked this will just fail quietly
	if ( iter->second.m_bIsLocked )
		iter->second.m_pBuffer->Unlock();

	// Release the resources
	iter->second.m_pBuffer->Release();

	// Remove from the map
	m_MapVertexBuffers.erase( iter );

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: set vertex decl
//-----------------------------------------------------------------------------
bool CGameEngineGL::BSetVertexDeclaration( IDirect3DVertexDeclaration9 *decl )
{
	if ( !m_pD3D9Device )
		return false;
		
	m_pD3D9Device->SetVertexDeclaration( decl );
	
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: set stream source
//-----------------------------------------------------------------------------
bool CGameEngineGL::BSetStreamSource( uint streamNumber, HGAMEVERTBUF hVertBuf, uint32 uOffset, uint32 uStride )
{
	if ( !m_pD3D9Device )
		return false;

	std::map<HGAMEVERTBUF, VertBufData_t>::iterator iter;
	iter = m_MapVertexBuffers.find( hVertBuf );
	if ( iter == m_MapVertexBuffers.end() )
	{
		OutputDebugString( "Invalid vertex buffer handle passed to BSetStreamSource()\n" );
		return false;
	}

	// Make sure the pointer is valid
	if ( !iter->second.m_pBuffer )
	{
		OutputDebugString( "Pointer to vertex buffer is invalid (lost device and not recreated?)!\n" );
		return false;
	}

	// buffer located, pass it to set stream source call
	m_pD3D9Device->SetStreamSource( streamNumber, iter->second.m_pBuffer, uOffset, uStride );
		
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Set shader pair for rendering
//-----------------------------------------------------------------------------
bool CGameEngineGL::BSetShaders( IDirect3DVertexShader9 *vsh, IDirect3DPixelShader9 *psh )
{
	if ( !m_pD3D9Device )
		return false;

	m_pD3D9Device->SetVertexShader( vsh );
	m_pD3D9Device->SetPixelShader( psh );
	
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Render primitives out of the current stream source
//-----------------------------------------------------------------------------
bool CGameEngineGL::BRenderPrimitive( D3DPRIMITIVETYPE primType, uint32 uStartVertex, uint32 uCount )
{
	if ( !m_pD3D9Device )
		return false;

	if ( FAILED( m_pD3D9Device->DrawPrimitive( primType, uStartVertex, uCount ) ) )
	{
		OutputDebugString( "BRenderPrimitive() call failed\n" );
		return false;
	}
		
	return true;
}

bool CGameEngineGL::BUberRenderPrimitive(	IDirect3DVertexShader9 *vsh,
									IDirect3DPixelShader9 *psh,
									IDirect3DVertexDeclaration9 *decl,
									uint streamNumber,
									HGAMEVERTBUF hVertBuf,
									uint32 uOffset,
									uint32 uStride,
									D3DPRIMITIVETYPE primType,
									uint32 uStartVertex,
									uint32 uCount )
{
	if ( !m_pD3D9Device )
		return false;
	
	bool bRes = false;
	bRes |= BSetShaders				( vsh, psh );
	bRes |= BSetVertexDeclaration	( decl );
	bRes |= BSetStreamSource		( streamNumber, hVertBuf, 0 /*uOffset*/, uStride );		// streams are based at start of buffer
	bRes |= BRenderPrimitive		( primType, uStartVertex, uCount );						// uStartVertex advances the fetch offset from the base

	return bRes;
}



#endif

//-----------------------------------------------------------------------------
// Purpose: Draw a line, the engine internally manages a vertex buffer for batching these
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawLine( float xPos0, float yPos0, DWORD dwColor0, float xPos1, float yPos1, DWORD dwColor1 )
{
	if ( m_bShuttingDown )
		return false;

	#if DX9MODE
		if ( !m_pD3D9Device )
			return false;

		if ( !m_hLineBuffer )
		{
			// Create the line buffer
			m_hLineBuffer = HCreateVertexBuffer( sizeof( LineVertex_t ) * LINE_BUFFER_TOTAL_SIZE * 2, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

			if ( !m_hLineBuffer )
			{
				OutputDebugString( "Can't BDrawLine() because vertex buffer creation failed\n" );
				return false;
			}
		}

		// Check if we are out of room and need to flush the buffer
		if ( m_dwLinesToFlush == LINE_BUFFER_BATCH_SIZE )
		{
			BFlushLineBuffer();
		}

		// Lock the vertex buffer into memory
		if ( !m_pLineVertexes )
		{
			if ( !BLockEntireVertexBuffer( m_hLineBuffer, (void**)&m_pLineVertexes, m_dwLineBufferBatchPos ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) )
			{
				m_pLineVertexes = NULL;
				OutputDebugString( "BDrawLine failed because locking vertex buffer failed\n" );
				return false;
			}
		}

		LineVertex_t *pVertData = &m_pLineVertexes[ m_dwLineBufferBatchPos*2+m_dwLinesToFlush*2 ];
		pVertData[0].rhw = 1.0;
		pVertData[0].z = 1.0;
		pVertData[0].x = xPos0;
		pVertData[0].y = yPos0;
		pVertData[0].color = dwColor0;

		pVertData[1].rhw = 1.0;
		pVertData[1].z = 1.0;
		pVertData[1].x = xPos1;
		pVertData[1].y = yPos1;
		pVertData[1].color = dwColor1;

		++m_dwLinesToFlush;
	#else
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
	#endif
	
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush batched lines to the screen
//-----------------------------------------------------------------------------
bool CGameEngineGL::BFlushLineBuffer()
{
	#if DX9MODE
		// If the vert buffer isn't already locked into memory, then there is nothing to flush
		if ( m_pLineVertexes == NULL )
			return true; // consider this successful since there was no error

		// OK, it is locked, so unlock now
		if ( !BUnlockVertexBuffer( m_hLineBuffer ) )
		{
			OutputDebugString( "Failed flushing line buffer because BUnlockVertexBuffer failed\n" );
			return false;
		}

		// Clear the memory pointer as its invalid now that we unlocked
		m_pLineVertexes = NULL;

		// If there is nothing to actual flush, we are done
		if ( m_dwLinesToFlush == 0 )
			return true;

		BUberRenderPrimitive( m_vsh_P4C1, m_psh_P4C1, m_decl_P4C1, 0, m_hLineBuffer, 0, sizeof( LineVertex_t ), D3DPT_LINELIST, m_dwLineBufferBatchPos*2, m_dwLinesToFlush );

		m_dwLinesToFlush = 0;
		m_dwLineBufferBatchPos += LINE_BUFFER_BATCH_SIZE;
		if ( m_dwLineBufferBatchPos >= LINE_BUFFER_TOTAL_SIZE )
		{
			m_dwLineBufferBatchPos = 0;
		}

		return true;
	#else
		if ( !m_rgflLinesColorData || !m_rgflLinesData || m_bShuttingDown )
			return false;
	
		if ( m_dwLinesToFlush )
		{
			glColorPointer( 4, GL_UNSIGNED_BYTE, 0, m_rgflLinesColorData );
			glVertexPointer( 3, GL_FLOAT, 0, m_rgflLinesData );
			glDrawArrays( GL_LINES, 0, m_dwLinesToFlush*2 );
	
			m_dwLinesToFlush = 0;
		}
	#endif

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Draw a point, the engine internally manages a vertex buffer for batching these
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawPoint( float xPos, float yPos, DWORD dwColor )
{
	if ( m_bShuttingDown )
		return false;


	#if DX9MODE
		if ( !m_pD3D9Device )
			return false;

		if ( !m_hPointBuffer )
		{
			// Create the point buffer
			m_hPointBuffer = HCreateVertexBuffer( sizeof( PointVertex_t ) * POINT_BUFFER_TOTAL_SIZE * 2, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE );

			if ( !m_hPointBuffer )
			{
				OutputDebugString( "Can't BDrawPoint() because vertex buffer creation failed\n" );
				return false;
			}
		}

		// Check if we are out of room and need to flush the buffer
		if ( m_dwPointsToFlush == POINT_BUFFER_BATCH_SIZE )	
		{
			BFlushPointBuffer();
		}

		// Set FVF
		//if ( !BSetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ) )
		//	return false;

		// Lock the vertex buffer into memory
		if ( !m_pPointVertexes )
		{
			if ( !BLockEntireVertexBuffer( m_hPointBuffer, (void**)&m_pPointVertexes, m_dwPointBufferBatchPos+m_dwPointsToFlush ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) )
			{
				m_pPointVertexes = NULL;
				OutputDebugString( "BDrawPoint failed because locking vertex buffer failed\n" );
				return false;
			}
		}

		PointVertex_t *pVertData = &m_pPointVertexes[ m_dwPointBufferBatchPos+m_dwPointsToFlush ];
		pVertData[0].rhw = 1.0;
		pVertData[0].z = 1.0;
		pVertData[0].x = xPos;
		pVertData[0].y = yPos;
		pVertData[0].color = dwColor;

		++m_dwPointsToFlush;

	#else
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
	#endif
	
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush batched points to the screen
//-----------------------------------------------------------------------------
bool CGameEngineGL::BFlushPointBuffer()
{
	#if DX9MODE
		#if 1
			// If the vert buffer isn't already locked into memory, then there is nothing to flush
			if ( m_pPointVertexes == NULL )
				return true; // consider this successful since there was no error

			// OK, it is locked, so unlock now
			if ( !BUnlockVertexBuffer( m_hPointBuffer ) )
			{
				OutputDebugString( "Failed flushing point buffer because BUnlockVertexBuffer failed\n" );
				return false;
			}

			// Clear the memory pointer as its invalid now that we unlocked
			m_pPointVertexes = NULL;

			// If there is nothing to actual flush, we are done
			if ( m_dwPointsToFlush == 0 )
				return true;

			BUberRenderPrimitive( m_vsh_P4C1, m_psh_P4C1, m_decl_P4C1, 0, m_hPointBuffer, 0, sizeof( PointVertex_t ), D3DPT_POINTLIST, m_dwPointBufferBatchPos, m_dwPointsToFlush );

			m_dwPointsToFlush = 0;
			m_dwPointBufferBatchPos += POINT_BUFFER_BATCH_SIZE;
			if ( m_dwPointBufferBatchPos >= POINT_BUFFER_TOTAL_SIZE )
			{
				m_dwPointBufferBatchPos = 0;
			}
		#endif
		m_dwPointsToFlush = 0;
	#else
		if ( !m_rgflPointsColorData || !m_rgflPointsData || m_bShuttingDown )
			return false;
	
		if ( m_dwPointsToFlush )
		{
			glColorPointer( 4, GL_UNSIGNED_BYTE, 0, m_rgflPointsColorData );
			glVertexPointer( 3, GL_FLOAT, 0, m_rgflPointsData );
			glDrawArrays( GL_POINTS, 0, m_dwPointsToFlush );
	
			m_dwPointsToFlush = 0;
		}
    #endif
    
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Draw a filled quad
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawFilledRect( float xPos0, float yPos0, float xPos1, float yPos1, DWORD dwColor )
{
	#if DX9MODE
		if ( !m_hTextureWhite )
		{
			#if 0	// debug gradient
				byte *pRGBAData = new byte[ 16 * 16 * 4 ];
				for( uint y = 0; y< 16; y++)
				{
					for( uint x = 0; x< 16; x++)
					{
						byte *dest = pRGBAData + (y*64) + (x*4);
						dest[0] = 0xFF;
						dest[1] = y * 0x11;
						dest[2] = x * 0x11;
						dest[3] = 0xFF;
					}
				}
				m_hTextureWhite = HCreateTexture( pRGBAData, 16, 16 );
			#else
				byte *pRGBAData = new byte[ 1 * 1 * 4 ];
				memset( pRGBAData, 255, 1*1*4 );
				m_hTextureWhite = HCreateTexture( pRGBAData, 1, 1 );
			#endif
			
			delete[] pRGBAData;
		}

		return BDrawTexturedRect( xPos0, yPos0, xPos1, yPos1, 0.0f, 0.0f, 1.0f, 1.0f, dwColor, m_hTextureWhite );
	#else
		if ( !m_hTextureWhite )
		{
			byte *pRGBAData = new byte[ 1 * 1 * 4 ];
			memset( pRGBAData, 255, 1*1*4 );
			m_hTextureWhite = HCreateTexture( pRGBAData, 1, 1 );
			delete[] pRGBAData;
		}
	
		return BDrawTexturedRect( xPos0, yPos0, xPos1, yPos1, 0.0f, 0.0f, 1.0f, 1.0f, dwColor, m_hTextureWhite );
	#endif
}


//-----------------------------------------------------------------------------
// Purpose: Draw a textured rect
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawTexturedRect( float xPos0, float yPos0, float xPos1, float yPos1, float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture )
{
	if ( m_bShuttingDown )
		return false;

	#if DX9MODE
		if ( !m_pD3D9Device )
			return false;

		// Find the texture
		std::map<HGAMETEXTURE, TextureData_t>::iterator iter;
		iter = m_MapTextures.find( hTexture );
		if ( iter == m_MapTextures.end() )
		{
			OutputDebugString( "BDrawTexturedRect called with invalid hTexture value\n" );
			return false;
		}

		if ( !m_hQuadBuffer )
		{
			// Create the line buffer
			m_hQuadBuffer = HCreateVertexBuffer( sizeof( TexturedQuadVertex_t ) * QUAD_BUFFER_TOTAL_SIZE * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

			if ( !m_hQuadBuffer )
			{
				OutputDebugString( "Can't BDrawTexturedRect() because vertex buffer creation failed\n" );
				return false;
			}
		}

		// Check if we are out of room and need to flush the buffer
		if ( m_dwQuadsToFlush == QUAD_BUFFER_BATCH_SIZE )
		{
			BFlushQuadBuffer();
		}

		// Check if the texture changed so we need to flush the buffer
		if ( m_hLastTexture != hTexture )
		{
			BFlushQuadBuffer();
		}	

		// Save the texture to use for next flush
		m_hLastTexture = hTexture;

		// Lock the vertex buffer into memory
		if ( !m_pQuadVertexes )
		{
			if ( !BLockEntireVertexBuffer( m_hQuadBuffer, (void**)&m_pQuadVertexes, m_dwQuadBufferBatchPos ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) )
			{
				m_pQuadVertexes = NULL;
				OutputDebugString( "BDrawTexturedRect failed because locking vertex buffer failed\n" );
				return false;
			}
		}

		TexturedQuadVertex_t *pVertData = &m_pQuadVertexes[ m_dwQuadBufferBatchPos*4+m_dwQuadsToFlush*4 ];

		#if GLMDEBUG && 0
			GLMPRINTF(("-D- m_dwQuadBufferBatchPos = %d, m_dwQuadsToFlush = %d, net vertex index = %d(offset $%08x), m_pQuadVertexes is %08x, pVertData is %08x " ,
				m_dwQuadBufferBatchPos,
				m_dwQuadsToFlush,
				m_dwQuadBufferBatchPos*4+m_dwQuadsToFlush*4,
				(m_dwQuadBufferBatchPos*4+m_dwQuadsToFlush*4) * sizeof( m_pQuadVertexes[0] ),
				m_pQuadVertexes,
				pVertData
				));
		#endif
		pVertData[0].color = dwColor;
		pVertData[0].rhw = 1.0f;
		pVertData[0].z = 1.0f;
		pVertData[0].x = xPos0;
		pVertData[0].y = yPos0;
		pVertData[0].u = u0;
		pVertData[0].v = v0;

		pVertData[1].color = dwColor;
		pVertData[1].rhw = 1.0f;
		pVertData[1].z = 1.0f;
		pVertData[1].x = xPos1;
		pVertData[1].y = yPos0;
		pVertData[1].u = u1;
		pVertData[1].v = v0;

		pVertData[2].color = dwColor;
		pVertData[2].rhw = 1.0f;
		pVertData[2].z = 1.0f;
		pVertData[2].x = xPos0;
		pVertData[2].y = yPos1;
		pVertData[2].u = u0;
		pVertData[2].v = v1;

		pVertData[3].color = dwColor;
		pVertData[3].rhw = 1.0f;
		pVertData[3].z = 1.0f;
		pVertData[3].x = xPos1;
		pVertData[3].y = yPos1;
		pVertData[3].u = u1;
		pVertData[3].v = v1;

		++m_dwQuadsToFlush;
	#else
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
		m_rgflQuadsData[dwOffset+2] = 1.0;
		m_rgflQuadsData[dwOffset+3] = xPos1;
		m_rgflQuadsData[dwOffset+4] = yPos0;
		m_rgflQuadsData[dwOffset+5] = 1.0;
		m_rgflQuadsData[dwOffset+6] = xPos1;
		m_rgflQuadsData[dwOffset+7] = yPos1;
		m_rgflQuadsData[dwOffset+8] = 1.0;
		m_rgflQuadsData[dwOffset+9] = xPos0;
		m_rgflQuadsData[dwOffset+10] = yPos1;
		m_rgflQuadsData[dwOffset+11] = 1.0;
	
		dwOffset = m_dwQuadsToFlush*16;
		m_rgflQuadsColorData[dwOffset] = COLOR_RED( dwColor );
		m_rgflQuadsColorData[dwOffset+1] = COLOR_GREEN( dwColor );
		m_rgflQuadsColorData[dwOffset+2] = COLOR_BLUE( dwColor );
		m_rgflQuadsColorData[dwOffset+3] = COLOR_ALPHA( dwColor );
		m_rgflQuadsColorData[dwOffset+4] = COLOR_RED( dwColor );
		m_rgflQuadsColorData[dwOffset+5] = COLOR_GREEN( dwColor );
		m_rgflQuadsColorData[dwOffset+6] = COLOR_BLUE( dwColor );
		m_rgflQuadsColorData[dwOffset+7] = COLOR_ALPHA( dwColor );
		m_rgflQuadsColorData[dwOffset+8] = COLOR_RED( dwColor );
		m_rgflQuadsColorData[dwOffset+9] = COLOR_GREEN( dwColor );
		m_rgflQuadsColorData[dwOffset+10] = COLOR_BLUE( dwColor );
		m_rgflQuadsColorData[dwOffset+11] = COLOR_ALPHA( dwColor );
		m_rgflQuadsColorData[dwOffset+12] = COLOR_RED( dwColor );
		m_rgflQuadsColorData[dwOffset+13] = COLOR_GREEN( dwColor );
		m_rgflQuadsColorData[dwOffset+14] = COLOR_BLUE( dwColor );
		m_rgflQuadsColorData[dwOffset+15] = COLOR_ALPHA( dwColor );
	
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
	#endif
	
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Draw a textured rect
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawTexturedQuad( float xPos0, float yPos0, float xPos1, float yPos1, float xPos2, float yPos2, float xPos3, float yPos3,
	float u0, float v0, float u1, float v1, DWORD dwColor, HGAMETEXTURE hTexture )
{
	if ( m_bShuttingDown )
		return false;

	#if DX9MODE
		if ( !m_pD3D9Device )
			return false;

		// Find the texture
		std::map<HGAMETEXTURE, TextureData_t>::iterator iter;
		iter = m_MapTextures.find( hTexture );
		if ( iter == m_MapTextures.end() )
		{
			OutputDebugString( "BDrawTexturedRect called with invalid hTexture value\n" );
			return false;
		}

		if ( !m_hQuadBuffer )
		{
			// Create the line buffer
			m_hQuadBuffer = HCreateVertexBuffer( sizeof( TexturedQuadVertex_t ) * QUAD_BUFFER_TOTAL_SIZE * 4, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

			if ( !m_hQuadBuffer )
			{
				OutputDebugString( "Can't BDrawTexturedRect() because vertex buffer creation failed\n" );
				return false;
			}
		}

		// Check if we are out of room and need to flush the buffer
		if ( m_dwQuadsToFlush == QUAD_BUFFER_BATCH_SIZE )
		{
			BFlushQuadBuffer();
		}

		// Check if the texture changed so we need to flush the buffer
		if ( m_hLastTexture != hTexture )
		{
			BFlushQuadBuffer();
		}	

		// Save the texture to use for next flush
		m_hLastTexture = hTexture;

		// Lock the vertex buffer into memory
		if ( !m_pQuadVertexes )
		{
			if ( !BLockEntireVertexBuffer( m_hQuadBuffer, (void**)&m_pQuadVertexes, m_dwQuadBufferBatchPos ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD ) )
			{
				m_pQuadVertexes = NULL;
				OutputDebugString( "BDrawTexturedRect failed because locking vertex buffer failed\n" );
				return false;
			}
		}

		TexturedQuadVertex_t *pVertData = &m_pQuadVertexes[ m_dwQuadBufferBatchPos*4+m_dwQuadsToFlush*4 ];

		#if GLMDEBUG && 0
			GLMPRINTF(("-D- m_dwQuadBufferBatchPos = %d, m_dwQuadsToFlush = %d, net vertex index = %d(offset $%08x), m_pQuadVertexes is %08x, pVertData is %08x " ,
				m_dwQuadBufferBatchPos,
				m_dwQuadsToFlush,
				m_dwQuadBufferBatchPos*4+m_dwQuadsToFlush*4,
				(m_dwQuadBufferBatchPos*4+m_dwQuadsToFlush*4) * sizeof( m_pQuadVertexes[0] ),
				m_pQuadVertexes,
				pVertData
				));
		#endif
		pVertData[0].color = dwColor;
		pVertData[0].rhw = 1.0f;
		pVertData[0].z = 1.0f;
		pVertData[0].x = xPos0;
		pVertData[0].y = yPos0;
		pVertData[0].u = u0;
		pVertData[0].v = v0;

		pVertData[1].color = dwColor;
		pVertData[1].rhw = 1.0f;
		pVertData[1].z = 1.0f;
		pVertData[1].x = xPos1;
		pVertData[1].y = yPos1;
		pVertData[1].u = u1;
		pVertData[1].v = v0;

		pVertData[2].color = dwColor;
		pVertData[2].rhw = 1.0f;
		pVertData[2].z = 1.0f;
		pVertData[2].x = xPos2;
		pVertData[2].y = yPos2;
		pVertData[2].u = u0;
		pVertData[2].v = v1;

		pVertData[3].color = dwColor;
		pVertData[3].rhw = 1.0f;
		pVertData[3].z = 1.0f;
		pVertData[3].x = xPos3;
		pVertData[3].y = yPos3;
		pVertData[3].u = u1;
		pVertData[3].v = v1;

		++m_dwQuadsToFlush;
	#else
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
		m_rgflQuadsData[dwOffset+2] = 1.0;
		m_rgflQuadsData[dwOffset+3] = xPos1;
		m_rgflQuadsData[dwOffset+4] = yPos1;
		m_rgflQuadsData[dwOffset+5] = 1.0;
		m_rgflQuadsData[dwOffset+6] = xPos2;
		m_rgflQuadsData[dwOffset+7] = yPos2;
		m_rgflQuadsData[dwOffset+8] = 1.0;
		m_rgflQuadsData[dwOffset+9] = xPos3;
		m_rgflQuadsData[dwOffset+10] = yPos3;
		m_rgflQuadsData[dwOffset+11] = 1.0;
	
		dwOffset = m_dwQuadsToFlush*16;
		m_rgflQuadsColorData[dwOffset] = COLOR_RED( dwColor );
		m_rgflQuadsColorData[dwOffset+1] = COLOR_GREEN( dwColor );
		m_rgflQuadsColorData[dwOffset+2] = COLOR_BLUE( dwColor );
		m_rgflQuadsColorData[dwOffset+3] = COLOR_ALPHA( dwColor );
		m_rgflQuadsColorData[dwOffset+4] = COLOR_RED( dwColor );
		m_rgflQuadsColorData[dwOffset+5] = COLOR_GREEN( dwColor );
		m_rgflQuadsColorData[dwOffset+6] = COLOR_BLUE( dwColor );
		m_rgflQuadsColorData[dwOffset+7] = COLOR_ALPHA( dwColor );
		m_rgflQuadsColorData[dwOffset+8] = COLOR_RED( dwColor );
		m_rgflQuadsColorData[dwOffset+9] = COLOR_GREEN( dwColor );
		m_rgflQuadsColorData[dwOffset+10] = COLOR_BLUE( dwColor );
		m_rgflQuadsColorData[dwOffset+11] = COLOR_ALPHA( dwColor );
		m_rgflQuadsColorData[dwOffset+12] = COLOR_RED( dwColor );
		m_rgflQuadsColorData[dwOffset+13] = COLOR_GREEN( dwColor );
		m_rgflQuadsColorData[dwOffset+14] = COLOR_BLUE( dwColor );
		m_rgflQuadsColorData[dwOffset+15] = COLOR_ALPHA( dwColor );
	
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
	#endif
	
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Flush buffered quads
//-----------------------------------------------------------------------------
bool CGameEngineGL::BFlushQuadBuffer()
{
	#if DX9MODE
		// If the vert buffer isn't already locked into memory, then there is nothing to flush
		if ( m_pQuadVertexes == NULL )
			return true; // consider this successful since there was no error

		// OK, it is locked, so unlock now
		if ( !BUnlockVertexBuffer( m_hQuadBuffer ) )
		{
			OutputDebugString( "Failed flushing quad buffer because BUnlockVertexBuffer failed\n" );
			return false;
		}

		// Clear the memory pointer as its invalid now that we unlocked
		m_pQuadVertexes = NULL;

		// If there is nothing to actual flush, we are done
		if ( m_dwQuadsToFlush == 0 )
			return true;

		// Find the texture
		std::map<HGAMETEXTURE, TextureData_t>::iterator iter;
		iter = m_MapTextures.find( m_hLastTexture );
		if ( iter == m_MapTextures.end() )
		{
			OutputDebugString( "BFlushQuadBuffer failed with invalid m_hLastTexture value\n" );
			return false;
		}

		// See if we need to actually create the d3d texture
		if ( !iter->second.m_pTexture )
		{
			//HRESULT CreateTexture(UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,VD3DHANDLE* pSharedHandle, char *debugLabel=NULL);
			HRESULT hRes = m_pD3D9Device->CreateTexture( 
				(UINT)iter->second.m_uWidth, 
				(UINT)iter->second.m_uHeight, 
				(UINT)1, // mip map levels (0 = generate all levels down to 1x1 if supported)
				(DWORD)0, // have to set the right flag here if you want to autogen mipmaps... we don't
				(D3DFORMAT)D3DFMT_A8R8G8B8,
				(D3DPOOL)D3DPOOL_MANAGED,
				(IDirect3DTexture9** )&iter->second.m_pTexture,
				(VD3DHANDLE*)NULL,
				"debuglabel" );
				
			if( FAILED( hRes ) )
			{
				OutputDebugString( "BFlushQuadBuffer failed because CreateTexture() call failed\n" );
				iter->second.m_pTexture = NULL;
				return false;
			}

			// Put the data into the texture
			D3DLOCKED_RECT rect;
			hRes = iter->second.m_pTexture->LockRect( 0, &rect, NULL, 0 );
			if( FAILED( hRes ) )
			{
				OutputDebugString( "LockRect call failed\n" );
				iter->second.m_pTexture->Release();
				iter->second.m_pTexture = NULL;
				return false;
			}

			DWORD *pARGB = (DWORD *) rect.pBits;
			byte *pRGBA = (byte *) iter->second.m_pRGBAData;

			byte r,g,b,a;
			for( uint32 y = 0; y < iter->second.m_uHeight; ++y )
			{
				for( uint32 x = 0; x < iter->second.m_uWidth; ++x )
				{
					// swap position of alpha value from back to front to be in correct format for d3d...
					r = *pRGBA++;
					g = *pRGBA++;
					b = *pRGBA++;
					a = *pRGBA++;

					*pARGB++ =  D3DCOLOR_ARGB( a, r, g, b );
				}
			}

			hRes = iter->second.m_pTexture->UnlockRect( 0 );
			if( FAILED( hRes ) )
			{
				OutputDebugString( "UnlockRect call failed\n" );
				iter->second.m_pTexture->Release();
				iter->second.m_pTexture = NULL;
				return false;
			}
		}

		// Ok, texture should be created ok, do the drawing work
		if ( FAILED( m_pD3D9Device->SetTexture( 0, iter->second.m_pTexture ) ) )
		{
			OutputDebugString( "BFlushQuadBuffer failed setting texture\n" );
			return false;
		}

		for ( DWORD i=0; i < m_dwQuadsToFlush*4; i += 4 )
		{
			BUberRenderPrimitive( m_vsh_P4C1T2, m_psh_P4C1T2, m_decl_P4C1T2, 0, m_hQuadBuffer, 0, sizeof( TexturedQuadVertex_t ), D3DPT_TRIANGLESTRIP, (m_dwQuadBufferBatchPos*4)+i, 2 );
		}

		m_pD3D9Device->SetTexture( 0, NULL ); // need to clear the texture before other drawing ops


		m_dwQuadsToFlush = 0;
		m_dwQuadBufferBatchPos += QUAD_BUFFER_BATCH_SIZE;
		if ( m_dwQuadBufferBatchPos >= QUAD_BUFFER_TOTAL_SIZE )
		{
			m_dwQuadBufferBatchPos = 0;
		}
	#else
		if ( !m_rgflPointsColorData || !m_rgflPointsData || m_bShuttingDown )
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
	#endif
	
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Creates a new texture 
//-----------------------------------------------------------------------------
HGAMETEXTURE CGameEngineGL::HCreateTexture( byte *pRGBAData, uint32 uWidth, uint32 uHeight, ETEXTUREFORMAT eTextureFormat )
{
	#if DX9MODE
		if ( !m_pD3D9Device )
			return 0;

		TextureData_t TexData;
		TexData.m_uWidth = uWidth;
		TexData.m_uHeight = uHeight;
		TexData.m_pRGBAData = new byte[uWidth*uHeight*4];
		memcpy( TexData.m_pRGBAData, pRGBAData, uWidth*uHeight*4 );
		TexData.m_pTexture = NULL;

		int nHandle = m_nNextTextureHandle;
		++m_nNextTextureHandle;
		m_MapTextures[nHandle] = TexData;

		return nHandle;
	#else
		if ( m_bShuttingDown )
			return 0;
	
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
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, uWidth, uHeight, 0, eTextureFormat == eTextureFormat_RGBA ? GL_RGBA : GL_BGRA, GL_UNSIGNED_BYTE, (void *)pRGBAData );
		glDisable( GL_TEXTURE_2D );
	
		int nHandle = m_nNextTextureHandle;
		++m_nNextTextureHandle;
		m_MapTextures[nHandle] = TexData;
	
		return nHandle;
	#endif
}


//-----------------------------------------------------------------------------
// Purpose: update an exiting textue
//-----------------------------------------------------------------------------
bool CGameEngineGL::UpdateTexture( HGAMETEXTURE texture, byte *pRGBAData, uint32 uWidth, uint32 uHeight, ETEXTUREFORMAT eTextureFormat )
{
#if DX9MODE
		
	return false;
#else
	if ( m_bShuttingDown )
		return false;
	
	std::map<HGAMETEXTURE, TextureData_t>::iterator iter;
	iter = m_MapTextures.find( texture );
	if ( iter == m_MapTextures.end() )
	{
		OutputDebugString( "BDrawTexturedQuad called with invalid hTexture value\n" );
		return false;
	}

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, iter->second.m_uTextureID );
	
	// build our texture mipmaps
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, uWidth, uHeight, 0, eTextureFormat == eTextureFormat_RGBA ? GL_RGBA : GL_BGRA, GL_UNSIGNED_BYTE, (void *)pRGBAData );
	glDisable( GL_TEXTURE_2D );
	
	return true;
#endif
	
}


//-----------------------------------------------------------------------------
// Purpose: Creates a new font
//-----------------------------------------------------------------------------
HGAMEFONT CGameEngineGL::HCreateFont( int nHeight, int nFontWeight, bool bItalic, const char * pchFont )
{
	#if DX9MODE
		extern unsigned char g_glmDebugFontMap[ 128 * 128 ];		// raster order
		
		#if GLMDEBUG
			GLMPRINTF(("-D- CGameEngineGL::HCreateFont nHeight=%d, nFontWeight=%d, bItalic=%s, fontname=%s", nHeight, nFontWeight, bItalic?"T":"F", pchFont ));
		#endif
		
		byte *pRGBAData = new byte[ 128 * 128 * 4];

		byte *src = (byte*)g_glmDebugFontMap;
		byte *dst = pRGBAData;
		
		for( uint y = 0; y< 128; y++)
		{
			for( uint x = 0; x< 128; x++)
			{
				if (*src == ' ')
				{
					dst[0] = 0;
					dst[1] = 0;
					dst[2] = 0;
					dst[3] = 0;
				}
				else
				{
					dst[0] = 0xFF;
					dst[1] = 0xFF;
					dst[2] = 0xFF;
					dst[3] = 0xFF;
				}
				src++;
				dst += 4;
			}
		}
		HGAMETEXTURE font = HCreateTexture( pRGBAData, 128, 128 );
		
		delete[] pRGBAData;

		HGAMEFONT hFont = m_nNextFontHandle;
		++m_nNextFontHandle;
		
		m_MapGameFonts[ hFont ] = font;
	
		return hFont;		
	#else
		AutoReleasePool pool;
		
		HGAMEFONT hFont = m_nNextFontHandle;
		++m_nNextFontHandle;
		
		NSString *fontName = [NSString stringWithUTF8String:pchFont];
	
		NSFont *font = [NSFont fontWithName: fontName size: nHeight];
		[font retain];
		
		m_MapGameFonts[ hFont ] = font;
	
		return hFont;
	#endif
}


//-----------------------------------------------------------------------------
// Purpose: Draws text to the screen inside the given rectangular region, using the given font
//-----------------------------------------------------------------------------
bool CGameEngineGL::BDrawString( HGAMEFONT hFont, RECT rect, DWORD dwColor, DWORD dwFormat, const char *pchText )
{
	#if DX9MODE
		HGAMETEXTURE font = (HGAMETEXTURE)m_MapGameFonts[ hFont ];

		int stringlen = strlen( pchText );

		float stringwidth = stringlen * 7.0f;
		float stringheight = 11.0f;
		
		float stringleft = rect.left + floor( ((rect.right - rect.left) - stringwidth) / 2.0f );
		float stringtop = rect.top + floor( ((rect.bottom - rect.top) - stringheight) / 2.0f );

		for( int charindex = 0; charindex < stringlen; charindex++ )
		{
			float	leftU,rightU,topV,bottomV;
			
			int character = (int)pchText[charindex];
			character -= 0x20;
			if ( (character<0) || (character > 0x7F) )
			{
				character = '*' - 0x20;
			}
			
			leftU	= ((character & 0x0F) * 6.0f ) / 128.0f;
			rightU	= leftU + (6.0f / 128.0f);

			topV	= ((character >> 4) * 11.0f ) / 128.0f;
			bottomV	= topV + (11.0f / 128.0f);
			
			float posx,posy;
			
			posx = stringleft + (7.0f * (float)charindex);
			posy = stringtop;
			
			BDrawTexturedRect( posx, posy, posx + 6.0f, posy + 11.0f, leftU, topV, rightU, bottomV, dwColor, font );		
		}
	
	#else
		if ( !hFont )
		{
			OutputDebugString( "Someone is calling BDrawString with a null font handle\n" );
			return false;
		}

		AutoReleasePool pool;
		
		NSFont *pFont = (NSFont*) m_MapGameFonts[ hFont ];
		NSRect box = { { static_cast<CGFloat>(rect.left), static_cast<CGFloat>(rect.top) }, { static_cast<CGFloat>(rect.right-rect.left), static_cast<CGFloat>(rect.bottom-rect.top) } };
		NSColor *pColor = [NSColor colorWithCalibratedRed:COLOR_RED(dwColor)/255.0
													green:COLOR_GREEN(dwColor)/255.0
													 blue:COLOR_BLUE(dwColor)/255.0
													alpha:COLOR_ALPHA(dwColor)/255.0 ];
		
		GLString *&string = m_MapStrings[ std::string(pchText) ];
		
		NSString *nsString = [NSString stringWithUTF8String:pchText];
		
		if ( string == NULL)
		{
			string = [[GLString alloc] initWithString:nsString
											 withFont:pFont
										withTextColor:pColor
												inBox:&box
											withFlags:dwFormat];
		}
		else
		{
			[string setFont: pFont];
			[string setColor: pColor];
			[string setBox: &box];
		}
		
		[string drawWithBounds: box];	
	
	#endif
	return true;
}

void CGameEngineGL::UpdateKey( uint32_t vkKey, int nDown )
{
    if ( nDown )
        m_SetKeysDown.insert( vkKey );
    else
        m_SetKeysDown.erase( vkKey );
}

//-----------------------------------------------------------------------------
// Purpose: Message pump for OS messages
//-----------------------------------------------------------------------------
void CGameEngineGL::MessagePump()
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSApplication *pApp = [GLApplication sharedApplication];
    do
    {
        NSEvent *event = [pApp nextEventMatchingMask:NSEventMaskAny untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];
		if ( event == nil )
			break;
		
		// fprintf( stderr, ": %s\n", [[event description] UTF8String] );
		
        uint32_t c = 0;
		switch ( [event type] )
		{
			case NSEventTypeKeyDown:
			case NSEventTypeKeyUp:
                c = [[event charactersIgnoringModifiers] characterAtIndex:0];
            
                switch ( c )
                {
                    case NSUpArrowFunctionKey:
                        c = VK_UP;
                        break;
                    case NSDownArrowFunctionKey:
                        c = VK_DOWN;
                        break;
                    case NSLeftArrowFunctionKey:
                        c = VK_LEFT;
                        break;
                    case NSRightArrowFunctionKey:
                        c = VK_RIGHT;
                        break;
                    case 127: // on mac 'del' is backspace
                        c = VK_BACK;
                        break;
                }
                
                c = toupper(c);
                
                if ( [event type] == NSEventTypeKeyDown )
                    m_SetKeysDown.insert( c );
                else
                    m_SetKeysDown.erase( c );
				continue;
            
            case NSEventTypeFlagsChanged:
                c = [event modifierFlags];
                UpdateKey( VK_SHIFT, c & NSEventModifierFlagShift );
                UpdateKey( VK_CONTROL, c & NSEventModifierFlagControl );
                UpdateKey( VK_SELECT, c & NSEventModifierFlagOption );
                continue;

			default:
				break;
		}

        [pApp sendEvent:event];
        [pApp updateWindows];
    } while ( !BShuttingDown() );

    [pool release];
}

//-----------------------------------------------------------------------------
// Purpose: Find out if a key is currently down
//-----------------------------------------------------------------------------
bool CGameEngineGL::BIsKeyDown( DWORD dwVK )
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
bool CGameEngineGL::BGetFirstKeyDown( DWORD *pdwVK )
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
// Purpose: 
//-----------------------------------------------------------------------------
HGAMEVOICECHANNEL CGameEngineGL::HCreateVoiceChannel()
{
	m_unVoiceChannelCount++;
	CVoiceContext* pVoiceContext = new CVoiceContext;
    
	m_MapVoiceChannel[m_unVoiceChannelCount] = pVoiceContext;
	
	return m_unVoiceChannelCount;
}

void CGameEngineGL::RunAudio()
{
	std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;

	for( iter = m_MapVoiceChannel.begin(); iter!=m_MapVoiceChannel.end(); ++iter)
	{
		CVoiceContext* pVoice = iter->second;

		const int nBufferCount =  ARRAYSIZE( pVoice->m_buffers );
		ALint nQueued, nProcessed;
		alGetSourcei( pVoice->m_nSource, AL_BUFFERS_QUEUED, &nQueued );
		alGetSourcei( pVoice->m_nSource, AL_BUFFERS_PROCESSED, &nProcessed );
		
		if ( ( nQueued == nBufferCount ) && ( nProcessed == 0 ) )
		{	// No room at the inn
			continue;
		}
		
		ALuint nBufferID;
		for ( int i = 0; i < nProcessed; i++ )
			alSourceUnqueueBuffers( pVoice->m_nSource, 1, &nBufferID );
		
		int nMaxToQueue = nBufferCount - nQueued + nProcessed;
		bool bQueued = false;
		
		while ( nMaxToQueue && !pVoice->m_pending.empty() )
		{
			Packet_t &packet = pVoice->m_pending.front();
			
			nBufferID = pVoice->m_buffers[ pVoice->m_nNextFreeBuffer ];
			alBufferData( nBufferID, AL_FORMAT_MONO16, packet.pData, packet.unSize, VOICE_OUTPUT_SAMPLE_RATE_IDEAL );
			pVoice->m_nNextFreeBuffer = (pVoice->m_nNextFreeBuffer + 1 ) % nBufferCount;
			
            alSourceQueueBuffers( pVoice->m_nSource, 1, &nBufferID);

			nMaxToQueue--;
			free( packet.pData );
			pVoice->m_pending.pop();
			bQueued = true;
        }
	
		if ( bQueued && ( (nQueued-nProcessed) == 0 ) )
		{
			alSourcePlay( pVoice->m_nSource );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGameEngineGL::DestroyVoiceChannel( HGAMEVOICECHANNEL hChannel )
{
	std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;
	iter = m_MapVoiceChannel.find( hChannel );
	if ( iter != m_MapVoiceChannel.end() )
	{
		CVoiceContext* pVoiceContext = iter->second;
		
		// free outstanding voice packets

		while( !pVoiceContext->m_pending.empty() )
		{
			free( pVoiceContext->m_pending.front().pData );
			pVoiceContext->m_pending.pop();
		}

		delete pVoiceContext;
		m_MapVoiceChannel.erase( iter );
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CGameEngineGL::AddVoiceData( HGAMEVOICECHANNEL hChannel, const uint8 *pVoiceData, uint32 uLength )
{
	std::map<HGAMEVOICECHANNEL, CVoiceContext* >::iterator iter;
	iter = m_MapVoiceChannel.find( hChannel );
	if ( iter == m_MapVoiceChannel.end() )
		return false; // channel not found

	CVoiceContext* pVoiceContext = iter->second;

	Packet_t packet;

	packet.pData = malloc ( uLength );
	memcpy( packet.pData, pVoiceData, uLength );
	packet.unSize = uLength;

	pVoiceContext->m_pending.push( packet );

	return true;
}


#if DX9MODE

//-----------------------------------------------------------------------------
// Purpose: return a pointer to a display db object, created on demand
//-----------------------------------------------------------------------------

GLMDisplayDB	*CGameEngineGL::GetDisplayDB( void )
{
	if (!m_displayDB)
	{
		m_displayDB = new GLMDisplayDB;		
		m_displayDB->Populate();
	}
	
	return m_displayDB;
}

//-----------------------------------------------------------------------------
// Purpose: allow dxabstract to read back the capabilities of the selected renderer
//-----------------------------------------------------------------------------

void	CGameEngineGL::GetRendererInfo( GLMRendererInfoFields *rendInfoOut )
{
	// hardwired to renderer 0 which is fine on any single monitor system
	if (rendInfoOut)
	{
		GLMDisplayDB *db = GetDisplayDB();
		*rendInfoOut = ((*db->m_renderers)[ 0 ])->m_info;
	}
}

PseudoNSGLContextPtr CGameEngineGL::GetNSGLContextForWindow ( void* windowref )
{
	WindowRef win = (WindowRef)windowref;
	
	if (win==[m_window windowRef])
	{
		PseudoNSGLContextPtr nsctx = [ m_view openGLContext ];
		
		Assert( nsctx != NULL );
		
		return nsctx;
	}
	else
	{
		return NULL;	// sorry, no idea
	}
}


//-----------------------------------------------------------------------------
// Purpose: either set or get the rendered size 
//-----------------------------------------------------------------------------

void	CGameEngineGL::RenderedSize( uint &width, uint &height, bool set )
{
	if (set)
	{
		Assert( 0 );	// not impl'd yet
	}
	else
	{
		width = m_nWindowWidth;
		height = m_nWindowHeight;
	}
}


//-----------------------------------------------------------------------------
// Purpose: get the displayed size which need not match the rendered size
//-----------------------------------------------------------------------------

void	CGameEngineGL::DisplayedSize( uint &width, uint &height )
{
	width = m_nWindowWidth;
	height = m_nWindowHeight;
}

//-----------------------------------------------------------------------------
// Purpose: display completed frame - assumes GLM/DXA already blitted final bits into back buffer of m_view's context
//-----------------------------------------------------------------------------

void	CGameEngineGL::ShowPixels		( CShowPixelsParams *params )
{
	Assert( m_window != NULL );
	Assert( m_view != NULL );

	[ [ m_view openGLContext ] makeCurrentContext ];

	//if (!params->m_onlySyncView)
	{
		// save old context
		NSOpenGLContext *oldctx = [ NSOpenGLContext currentContext ];
		
		// get target context
		NSOpenGLContext *newctx = [ m_view openGLContext ];
		
		// make it current
		[newctx makeCurrentContext];

		[newctx flushBuffer];	

		[oldctx makeCurrentContext];
	}
}
	
#endif

//-----------------------------------------------------------------------------
// Purpose: Return true if there is an active Steam Controller
//-----------------------------------------------------------------------------
bool CGameEngineGL::BIsSteamInputDeviceActive( )
{
	if ( m_ActiveControllerHandle )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize the steam controller actions
//-----------------------------------------------------------------------------
void CGameEngineGL::InitSteamInput( )
{
	// Digital game actions
	m_ControllerDigitalActionHandles[eControllerDigitalAction_TurnLeft] = SteamInput()->GetDigitalActionHandle( "turn_left" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_TurnRight] = SteamInput()->GetDigitalActionHandle( "turn_right" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_ForwardThrust] = SteamInput()->GetDigitalActionHandle( "forward_thrust" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_ReverseThrust] = SteamInput()->GetDigitalActionHandle( "backward_thrust" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_FireLasers] = SteamInput()->GetDigitalActionHandle( "fire_lasers" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_PauseMenu] = SteamInput()->GetDigitalActionHandle( "pause_menu" );

	m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuUp] = SteamInput()->GetDigitalActionHandle( "menu_up" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuDown] = SteamInput()->GetDigitalActionHandle( "menu_down" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuLeft] = SteamInput()->GetDigitalActionHandle( "menu_left" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuRight] = SteamInput()->GetDigitalActionHandle( "menu_right" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuSelect] = SteamInput()->GetDigitalActionHandle( "menu_select" );
	m_ControllerDigitalActionHandles[eControllerDigitalAction_MenuCancel] = SteamInput()->GetDigitalActionHandle( "menu_cancel" );

	// Analog game actions
	m_ControllerAnalogActionHandles[eControllerAnalogAction_AnalogControls] = SteamInput()->GetAnalogActionHandle( "analog_controls" );

	// Action set handles
	m_ControllerActionSetHandles[eControllerActionSet_ShipControls] = SteamInput()->GetActionSetHandle( "ship_controls" );
	m_ControllerActionSetHandles[eControllerActionSet_MenuControls] = SteamInput()->GetActionSetHandle( "menu_controls" );

}

//-----------------------------------------------------------------------------
// Purpose: Find an active Steam controller
//-----------------------------------------------------------------------------
void CGameEngineGL::FindActiveSteamInputDevice( )
{
	// Use the first available steam controller for all interaction. We can call this each frame to handle
	// a controller disconnecting and a different one reconnecting. Handles are guaranteed to be unique for
	// a given controller, even across power cycles.

	// See how many Steam Controllers are active. 
	ControllerHandle_t pHandles[STEAM_CONTROLLER_MAX_COUNT];
	int nNumActive = SteamInput()->GetConnectedControllers( pHandles );

	// If there's an active controller, and if we're not already using it, select the first one.
	if ( nNumActive && (m_ActiveControllerHandle != pHandles[0]) )
	{
		m_ActiveControllerHandle = pHandles[0];
	}
}


//--------------------------------------------------------------------------------------------------------------
// Purpose: For a given in-game action in a given action set, return a human-reaadable string to use as a prompt.
//--------------------------------------------------------------------------------------------------------------
const char *CGameEngineGL::GetTextStringForControllerOriginDigital( ECONTROLLERACTIONSET dwActionSet, ECONTROLLERDIGITALACTION dwDigitalAction )
{
	EInputActionOrigin origins[STEAM_CONTROLLER_MAX_ORIGINS];
	int nNumOrigins = SteamInput()->GetDigitalActionOrigins( m_ActiveControllerHandle, m_ControllerActionSetHandles[dwActionSet], m_ControllerDigitalActionHandles[dwDigitalAction], origins );

	if ( nNumOrigins )
	{
		// We should handle the case where this action is bound to multiple buttons, but
		// here we just grab the first.
		return SteamInput()->GetStringForActionOrigin( origins[0] );
	}

	return SteamInput()->GetStringForActionOrigin( k_EInputActionOrigin_None ); // Return "None"
}

//--------------------------------------------------------------------------------------------------------------
// Purpose: For a given in-game action in a given action set, return a human-reaadable string to use as a prompt.
//--------------------------------------------------------------------------------------------------------------
const char *CGameEngineGL::GetTextStringForControllerOriginAnalog( ECONTROLLERACTIONSET dwActionSet, ECONTROLLERANALOGACTION dwDigitalAction )
{
	EInputActionOrigin origins[STEAM_CONTROLLER_MAX_ORIGINS];
	int nNumOrigins = SteamInput()->GetAnalogActionOrigins( m_ActiveControllerHandle, m_ControllerActionSetHandles[dwActionSet], m_ControllerDigitalActionHandles[dwDigitalAction], origins );

	if ( nNumOrigins )
	{
		// We should handle the case where this action is bound to multiple buttons, but
		// here we just grab the first.
		return SteamInput()->GetStringForActionOrigin( origins[0] );
	}

	return SteamInput()->GetStringForActionOrigin( k_EInputActionOrigin_None ); // Return "None"
}

//-----------------------------------------------------------------------------
// Purpose: Called each frame
//-----------------------------------------------------------------------------
void CGameEngineGL::PollSteamInput( )
{
	// There's a bug where the action handles aren't non-zero until a config is done loading. Soon config
	// information will be available immediately. Until then try to init as long as the handles are invalid.
	if ( m_ControllerDigitalActionHandles[eControllerDigitalAction_TurnLeft] == 0 )
	{
		InitSteamInput( );
		return;
	}

	// Each frame check our active controller handle
	FindActiveSteamInputDevice( );

}

//-----------------------------------------------------------------------------
// Purpose: Set the LED color on the controller, if supported by controller
//-----------------------------------------------------------------------------
void CGameEngineGL::SetControllerColor( uint8 nColorR, uint8 nColorG, uint8 nColorB, unsigned int nFlags )
{
	SteamInput()->SetLEDColor( m_ActiveControllerHandle, nColorR, nColorG, nColorB, nFlags );
}

//-----------------------------------------------------------------------------
// Purpose: Set the trigger effect on DualSense controllers
//-----------------------------------------------------------------------------
void CGameEngineGL::SetTriggerEffect( bool bEnabled )
{
	ScePadTriggerEffectParam param;

	memset( &param, 0, sizeof( param ) );
	param.triggerMask = SCE_PAD_TRIGGER_EFFECT_TRIGGER_MASK_R2;

	// Clear any existing effect
	param.command[ SCE_PAD_TRIGGER_EFFECT_PARAM_INDEX_FOR_R2 ].mode = SCE_PAD_TRIGGER_EFFECT_MODE_OFF;
	SteamInput()->SetDualSenseTriggerEffect( m_ActiveControllerHandle, &param );

	if ( bEnabled )
	{
		param.command[ SCE_PAD_TRIGGER_EFFECT_PARAM_INDEX_FOR_R2 ].mode = SCE_PAD_TRIGGER_EFFECT_MODE_VIBRATION;
		param.command[ SCE_PAD_TRIGGER_EFFECT_PARAM_INDEX_FOR_R2 ].commandData.vibrationParam.position = 5;
		param.command[ SCE_PAD_TRIGGER_EFFECT_PARAM_INDEX_FOR_R2 ].commandData.vibrationParam.amplitude = 5;
		param.command[ SCE_PAD_TRIGGER_EFFECT_PARAM_INDEX_FOR_R2 ].commandData.vibrationParam.frequency = 8;
		SteamInput()->SetDualSenseTriggerEffect( m_ActiveControllerHandle, &param );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Trigger vibration on the controller, if supported by controller
//-----------------------------------------------------------------------------
void CGameEngineGL::TriggerControllerVibration( unsigned short nLeftSpeed, unsigned short nRightSpeed )
{
	SteamInput()->TriggerVibration( m_ActiveControllerHandle, nLeftSpeed, nRightSpeed );
}

//-----------------------------------------------------------------------------
// Purpose: Trigger haptics on the controller, if supported by controller
//-----------------------------------------------------------------------------
void CGameEngineGL::TriggerControllerHaptics( ESteamControllerPad ePad, unsigned short usOnMicroSec, unsigned short usOffMicroSec, unsigned short usRepeat )
{
	SteamInput()->Legacy_TriggerRepeatedHapticPulse( m_ActiveControllerHandle, ePad, usOnMicroSec, usOffMicroSec, usRepeat, 0 );
}

//-----------------------------------------------------------------------------
// Purpose: Find out if a controller event is currently active
//-----------------------------------------------------------------------------
bool CGameEngineGL::BIsControllerActionActive( ECONTROLLERDIGITALACTION dwAction )
{
	ControllerDigitalActionData_t digitalData = SteamInput()->GetDigitalActionData( m_ActiveControllerHandle, m_ControllerDigitalActionHandles[dwAction] );

	// Actions are only 'active' when they're assigned to a control in an action set, and that action set is active.
	if ( digitalData.bActive )
		return digitalData.bState;
	
	return false;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
// Purpose: Get the current x,y state of the analog action. Examples of an analog action are a virtual joystick on the trackpad or the real joystick.
//---------------------------------------------------------------------------------------------------------------------------------------------------
void CGameEngineGL::GetControllerAnalogAction( ECONTROLLERANALOGACTION dwAction, float *x, float *y )
{
	ControllerAnalogActionData_t analogData = SteamInput()->GetAnalogActionData( m_ActiveControllerHandle, m_ControllerAnalogActionHandles[dwAction] );

	// Actions are only 'active' when they're assigned to a control in an action set, and that action set is active.
	if ( analogData.bActive )
	{
		*x = analogData.x;
		*y = analogData.y;
	}
	else
	{
		*x = 0.0f;
		*y = 0.0f;
	}
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Purpose: Put the controller into a specific action set. Action sets are collections of game-context actions ie "walking", "flying" or "menu"
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CGameEngineGL::SetSteamControllerActionSet( ECONTROLLERACTIONSET dwActionSet )
{
	if ( m_ActiveControllerHandle == 0 )
		return;

	// This call is low-overhead and can be called repeatedly from game code that is active in a specific mode.
	SteamInput()->ActivateActionSet( m_ActiveControllerHandle, m_ControllerActionSetHandles[dwActionSet] );
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Purpose: Put the controller into a specific action set layer. Action sets layers apply modifications to an existing action set.
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CGameEngineGL::ActivateSteamControllerActionSetLayer( ECONTROLLERACTIONSET dwActionSetLayer )
{
	if ( m_ActiveControllerHandle == 0 )
		return;

	// This call is low-overhead and can be called repeatedly from game code that is active in a specific mode.
	SteamInput()->ActivateActionSetLayer( m_ActiveControllerHandle, m_ControllerActionSetHandles[ dwActionSetLayer ] );
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Purpose: Deactivate an existing action set layer
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void CGameEngineGL::DeactivateSteamControllerActionSetLayer( ECONTROLLERACTIONSET dwActionSetLayer )
{
	if ( m_ActiveControllerHandle == 0 )
		return;

	// This call is low-overhead and can be called repeatedly from game code that is active in a specific mode.
	SteamInput()->DeactivateActionSetLayer( m_ActiveControllerHandle, m_ControllerActionSetHandles[ dwActionSetLayer ] );
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------
// Purpose: Determine whether an action set layer is currently active
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool CGameEngineGL::BIsActionSetLayerActive( ECONTROLLERACTIONSET dwActionSetLayer )
{
	if ( m_ActiveControllerHandle == 0 )
		return false;

	ControllerActionSetHandle_t pActionSetLayerHandles[ 32 ];
	int nActiveLayerCount = SteamInput()->GetActiveActionSetLayers( m_ActiveControllerHandle, pActionSetLayerHandles );

	for ( int i = 0; i < nActiveLayerCount; i++ )
	{
		if ( pActionSetLayerHandles[ i ] == m_ControllerActionSetHandles[ dwActionSetLayer ] )
			return true;
	}

	return false;
}
