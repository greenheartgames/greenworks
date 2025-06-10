//========= Copyright  1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: provide some call-out glue to ObjC from the C++ GLMgr code
//
// $Revision: $
// $NoKeywords: $
//=============================================================================//


#include <Cocoa/Cocoa.h>
#undef MIN
#undef MAX
#define DONT_DEFINE_BOOL	// Don't define BOOL!
//#include "tier0/threadtools.h"
//#include "tier1/interface.h"
//#include "tier1/strtools.h"
//#include "tier1/utllinkedlist.h"
#include "glmgr.h"

// Debugger - 10.8
// CGDisplayModeCopyPixelEncoding - 10.11
// CGDisplayIOServicePort - 10.9
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

//===============================================================================

void __checkgl__( void )
{
#if GLMDEBUG
	GLenum errorcode = (GLenum)glGetError();
	if (errorcode != GL_NO_ERROR)
	{
		Debugger();
		printf("\nGL Error %d",errorcode);
	}
#endif
}


void	swap(GLMRendererInfo*& a, GLMRendererInfo*& b)
{
	GLMRendererInfo* temp = a;
	a = b;
	b = temp;
}

void	swap(GLMDisplayInfo*& a, GLMDisplayInfo*& b)
{
	GLMDisplayInfo* temp = a;
	a = b;
	b = temp;
}

void	swap(GLMDisplayMode*& a, GLMDisplayMode*& b)
{
	GLMDisplayMode* temp = a;
	a = b;
	b = temp;
}

// ------------------------------------------------------------------------------------ //
// some helper functions

// this one makes a new context
bool	GLMDetectSLGU( void );
bool	GLMDetectSLGU( void )
{
	CGLError	cgl_error = (CGLError)0;
	bool		result = false;
	
	CGLContextObj oldctx = CGLGetCurrentContext();

	static CGLPixelFormatAttribute attribs[] = 
	{
		kCGLPFADoubleBuffer,
		kCGLPFANoRecovery,
		kCGLPFAAccelerated,
		kCGLPFADepthSize,
			(CGLPixelFormatAttribute)0,
		kCGLPFAColorSize,
			(CGLPixelFormatAttribute)32,

		(CGLPixelFormatAttribute)0	// list term
	};

	CGLPixelFormatObj	pixfmtobj = NULL;
	GLint				npix;
	
	CGLContextObj		ctxobj = NULL;
	
	cgl_error = CGLChoosePixelFormat( attribs, &pixfmtobj, &npix );
	if (!cgl_error)
	{
		// got pixel format, make a context
		
		cgl_error = CGLCreateContext( pixfmtobj, NULL, &ctxobj );
		if (!cgl_error)
		{
			CGLSetCurrentContext( ctxobj );

			// now do the test

			_CGLContextParameter	kCGLCPGCDMPEngine = ((_CGLContextParameter)1314);

			GLint dummyval = 0;
			cgl_error = CGLGetParameter( CGLGetCurrentContext(), kCGLCPGCDMPEngine, &dummyval );

			result = (!cgl_error);
			
			// all done, go back to old context, and destroy the temp one
			CGLSetCurrentContext( oldctx );
			CGLDestroyContext( ctxobj );
		}
		
		// destroy the pixel format obj
		CGLDestroyPixelFormat( pixfmtobj );
	}

	return result;
}


bool	GLMDetectScaledResolveMode( uint osComboVersion, bool hasSLGU );
bool	GLMDetectScaledResolveMode( uint osComboVersion, bool hasSLGU )
{
	bool result = false;
	
	// note this function assumes a current context on the renderer in question
	// and that FB blit and SLGU are present..
	
	if (!hasSLGU)
		return false;
		
	if (osComboVersion <= 0x000A0604)	// we know no one has it before 10.6.5
		return false;

	bool forceTrickyTest = true;		// for debug
	if ( (osComboVersion < 0x000A0606) || forceTrickyTest)
	{
		// make two FBO's
		GLuint	fbos[2];
		GLuint	rbos[2];
		int extent = 64;
		
		// make two render buffers

		for( int fbi = 0; fbi < 2; fbi++ )
		{
			glGenFramebuffersEXT( 1, &fbos[fbi] );  __checkgl__();
			glBindFramebufferEXT( fbi ? GL_DRAW_FRAMEBUFFER_EXT : GL_READ_FRAMEBUFFER_EXT , fbos[fbi] );  __checkgl__();

			glGenRenderbuffersEXT( 1, &rbos[fbi] );  __checkgl__();
			glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, rbos[fbi] );  __checkgl__();

			// make it multisampled if 0
			if (!fbi)
			{
				glRenderbufferStorageMultisampleEXT( GL_RENDERBUFFER_EXT, 2, GL_RGBA8, extent,extent );	  __checkgl__();
			}
			else
			{
				glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_RGBA8, extent,extent );	  __checkgl__();			
			}

			// attach it 
			// #0 gets to be read and multisampled
			// #1 gets to be draw and multisampled
			glFramebufferRenderbufferEXT( fbi ? GL_DRAW_FRAMEBUFFER_EXT : GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, rbos[fbi] );  __checkgl__();			
		}
		// now test
		while( glGetError() )	// clear error queue
		{
			;
		}

		// now do the dummy blit
		glBlitFramebufferEXT(	0,0,extent,extent, 0,0,extent,extent, GL_COLOR_BUFFER_BIT, XGL_SCALED_RESOLVE_FASTEST_EXT );

		// type of error we get back lets us know what the outcome is.
		// invalid enum error								-> unsupported
		// no error or invalid op							-> supported

		GLenum errorcode = (GLenum)glGetError();
		switch(errorcode)
		{
			// expected outcomes.
			
			// positive
			case GL_NO_ERROR:
			case GL_INVALID_OPERATION:
				result = true;			// new scaled resolve detected
			break;
			
			default:
				result = false;			// no scaled resolve
			break;
		}
		
		// unbind and wipe stuff
		
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );  __checkgl__();
		
		for( int xfbi = 0; xfbi < 2; xfbi++ )
		{
			// unbind FBO
			glBindFramebufferEXT( xfbi ? GL_DRAW_FRAMEBUFFER_EXT : GL_READ_FRAMEBUFFER_EXT , 0 );  __checkgl__();

			// del FBO and RBO
			glDeleteFramebuffersEXT( 1, &fbos[xfbi] );  __checkgl__();
			glDeleteRenderbuffersEXT( 1, &rbos[xfbi] );  __checkgl__();
		}
	}
	else
	{
		// in 10.6.6 and later, just check for the ext string.
		char *gl_ext_string = (char*)glGetString(GL_EXTENSIONS);

		result = strstr(gl_ext_string, "GL_EXT_framebuffer_multisample_blit_scaled") != NULL;
	}

	return result; // no SLGU, no scaled resolve blit even possible
}


// ------------------------------------------------------------------------------------ //
// some glue to let GLMgr call into NS/ObjC classes.
// ------------------------------------------------------------------------------------ //

bool NewNSGLContext( unsigned long *attribs, PseudoNSGLContextPtr nsglShareCtx, PseudoNSGLContextPtr *nsglCtxOut, CGLContextObj *cglCtxOut )
{
	NSAutoreleasePool	*tempPool = [[NSAutoreleasePool alloc] init ];
	NSOpenGLPixelFormat	*pixFmt = NULL; 
	NSOpenGLContext		*nsglCtx = NULL;

	bool result = true;		// optimism
	
	if (result)
	{
		pixFmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute*)attribs];
		if (!pixFmt)
		{
			Debugger();	// bad news
			result = false;
		}
	}

	if (result)
	{
		nsglCtx = [[NSOpenGLContext alloc] initWithFormat: pixFmt shareContext: (NSOpenGLContext*) nsglShareCtx ];
		if (!nsglCtx)
		{
			Debugger();
			result = false;
		}
	}

	if (result)
	{
		[nsglCtx makeCurrentContext];
		
		*nsglCtxOut = nsglCtx;
		*cglCtxOut = (CGLContextObj)[ (NSOpenGLContext*)nsglCtx CGLContextObj ];
	}
	else
	{
		*nsglCtxOut = NULL;
		*cglCtxOut = NULL;
	}

	[tempPool release];
	
	return result;
}

CGLContextObj GetCGLContextFromNSGL( PseudoNSGLContextPtr nsglCtx )
{
	return (CGLContextObj)[ (NSOpenGLContext*)nsglCtx CGLContextObj];
}

void DelNSGLContext( PseudoNSGLContextPtr nsglCtx )
{
	[ (NSOpenGLContext*)nsglCtx release ];
}




// ------------------------------------------------------------------------------------ //

// GLMDisplayDB stuff hoisted from appframework (cocoamgr)

//===============================================================================
//  GLMDisplayMode, GLMDisplayInfo, GLMRendererInfo, GLMDisplayDB methods

GLMDisplayMode::GLMDisplayMode( uint width, uint height, uint refreshHz )
{
	m_info.m_modePixelWidth = width;
	m_info.m_modePixelHeight = height;
	m_info.m_modeRefreshHz = refreshHz;
}

GLMDisplayMode::~GLMDisplayMode()
{
	// empty
}

void	GLMDisplayMode::Dump( int which )
{
	GLMPRINTF(("\n             # %-2d  width=%-4d  height=%-4d  refreshHz=%-2d", which, m_info.m_modePixelWidth, m_info.m_modePixelHeight, m_info.m_modeRefreshHz ));
}


//===============================================================================

GLMDisplayInfo::GLMDisplayInfo( CGDirectDisplayID displayID, CGOpenGLDisplayMask displayMask )
{	
	m_info.m_cgDisplayID			= displayID;
	m_info.m_glDisplayMask			= displayMask;
	
	// extract info about this display such as pixel width and height
	m_info.m_displayPixelWidth		= (uint)CGDisplayPixelsWide( m_info.m_cgDisplayID );
	m_info.m_displayPixelHeight		= (uint)CGDisplayPixelsHigh( m_info.m_cgDisplayID );

	m_modes = NULL;
}

GLMDisplayInfo::~GLMDisplayInfo( void )
{
	if (m_modes)
	{
		// delete all the new'd display modes
		for( std::vector< GLMDisplayMode * >::iterator p = m_modes->begin(); p != m_modes->end(); p++ )
		{
			delete *p;
		}
		delete m_modes;
		m_modes = NULL;
	}
}

struct DisplayModeLessThan
{
	bool operator ()(GLMDisplayMode *A, GLMDisplayMode *B)
	{
		bool bigger		= false;
		bool smaller	= true;
		
		// check refreshrate - higher should win
		if ( A->m_info.m_modeRefreshHz > B->m_info.m_modeRefreshHz )
		{	
			return bigger;
		}
		else if ( A->m_info.m_modeRefreshHz < B->m_info.m_modeRefreshHz )
		{
			return smaller;
		}
		
		// check area - larger mode should win
		int areaa = A->m_info.m_modePixelWidth * A->m_info.m_modePixelHeight;
		int areab = B->m_info.m_modePixelWidth * B->m_info.m_modePixelHeight;
		
		if ( areaa > areab )
		{	
			return bigger;
		}
		else if ( areaa < areab )
		{
			return smaller;
		}
	
		return false;	// equal rank
	}
};

void	GLMDisplayInfo::PopulateModes( void )
{
	Assert( !m_modes );
	m_modes = new std::vector< GLMDisplayMode* >;
	
	if ( CFArrayRef	modeList = CGDisplayCopyAllDisplayModes( m_info.m_cgDisplayID, nullptr ) )
	{
		//  examine each mode
		CFIndex count = CFArrayGetCount( modeList );
		
		for (CFIndex i = 0; i < count; i++) 
		{
			long depth = 0;
			Boolean usable, stretched = false;

			CGDisplayModeRef mode= (CGDisplayModeRef)CFArrayGetValueAtIndex (modeList, i);
			
			CFStringRef pixEnc = CGDisplayModeCopyPixelEncoding(mode);
			if(CFStringCompare(pixEnc, CFSTR(IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
				depth = 32;
			else if(CFStringCompare(pixEnc, CFSTR(IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
				depth = 16;
			else if(CFStringCompare(pixEnc, CFSTR(IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
				depth = 8;
			
			usable = CGDisplayModeIsUsableForDesktopGUI( mode );
			uint32_t ioFlags = CGDisplayModeGetIOFlags(mode);
			stretched = ioFlags & kDisplayModeStretchedFlag ? true : false;
			
			if ( usable && (!stretched) && (depth==32) )
			{
				long modeHeight = CGDisplayModeGetHeight( mode );
				long modeWidth = CGDisplayModeGetWidth( mode );
				long refreshrate = CGDisplayModeGetRefreshRate( mode );

				if ( (modeHeight >= 384) && (modeWidth >= 512) )
				{
					GLMDisplayMode *newmode = new GLMDisplayMode( modeWidth, modeHeight, refreshrate );
					m_modes->push_back( newmode );
				}
			}
			CGDisplayModeRelease( mode );
		}
	}
	
	// now sort the modes
	// primary key is refresh rate
	// secondary key is area

	sort( m_modes->begin(), m_modes->end(), DisplayModeLessThan() );
}


void	GLMDisplayInfo::Dump( int which )
{
	GLMPRINTF(("\n         #%d: GLMDisplayInfo @ %08x, cg-id=%p  display-mask=%08x  pixwidth=%d  pixheight=%d", which, this, m_info.m_cgDisplayID, m_info.m_glDisplayMask, m_info.m_displayPixelWidth,  m_info.m_displayPixelHeight ));

	int i=0;
	for( std::vector< GLMDisplayMode * >::iterator p = m_modes->begin(); p != m_modes->end(); p++ )
	{
		(*p)->Dump(i);
		i++;
	}
}


//===============================================================================

GLMRendererInfo::GLMRendererInfo( GLMRendererInfoFields *info )
{
	NSAutoreleasePool	*tempPool = [[NSAutoreleasePool alloc] init ];

	// absorb info obtained so far by caller
	m_info = *info;
	m_displays = NULL;

	// gather more info using a dummy context
	unsigned int attribs[] = 
	{
		kCGLPFADoubleBuffer, kCGLPFANoRecovery, kCGLPFAAccelerated,
		kCGLPFADepthSize, 0,
		kCGLPFAColorSize, 32,
		kCGLPFARendererID, static_cast<unsigned int>(info->m_rendererID),
		0
	};

	NSOpenGLPixelFormat	*pixFmt		=	[[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute*)attribs]; 
	NSOpenGLContext		*nsglCtx	=	[[NSOpenGLContext alloc] initWithFormat: pixFmt shareContext: NULL ];

	[nsglCtx makeCurrentContext];
		
	// run queries.
	char *gl_ext_string = (char*)glGetString(GL_EXTENSIONS);

	uint vers = m_info.m_osComboVersion;
	
	//-------------------------------------------------------------------
	// booleans
	//-------------------------------------------------------------------
	// gamma writes.
	m_info.m_hasGammaWrites = true;
	if ( vers < 0x000A0600 )				// pre 10.6.0, no SRGB write - see http://developer.apple.com/graphicsimaging/opengl/capabilities/GLInfo_1058.html
	{
		m_info.m_hasGammaWrites = false;
	}
	
	if (m_info.m_atiR5xx)
	{
		m_info.m_hasGammaWrites = false;	// it just don't, even post 10.6.3
	}
	
	// if CLI option for fake SRGB mode is enabled, turn off this cap, act like we do not have EXT FB SRGB
	if (0 /* CommandLine()->FindParm("-glmenablefakesrgb") */)
	{
		m_info.m_hasGammaWrites = false;
	}
	
	// extension string *could* be checked, but on 10.6.3 the ext string is not there, but the func *is*

	//-------------------------------------------------------------------
	// mixed attach sizes for FBO
	m_info.m_hasMixedAttachmentSizes = true;
	if ( vers < 0x000A0603 )	// pre 10.6.3, no mixed attach sizes
	{
		m_info.m_hasMixedAttachmentSizes = false;
	}
	else
	{
		if (!strstr(gl_ext_string, "GL_ARB_framebuffer_object"))
		{
			// ARB_framebuffer_object not available
			m_info.m_hasMixedAttachmentSizes = false;
		}
	}
	// also check ext string

	//-------------------------------------------------------------------
	// BGRA vert attribs
	m_info.m_hasBGRA = true;
	if ( vers < 0x000A0603 )	// pre 10.6.3, no BGRA attribs
	{
		m_info.m_hasBGRA = false;
	}
	else
	{
		if (!strstr(gl_ext_string, "EXT_vertex_array_bgra"))
		{
			// EXT_vertex_array_bgra not available
			m_info.m_hasBGRA = false;
		}
	}

	//-------------------------------------------------------------------
	m_info.m_hasNewFullscreenMode = true;
	if ( vers < 0x000A0600 )	// pre 10.6.0, no clever window server full screen mode
	{
		m_info.m_hasNewFullscreenMode = false;
	}
	
	//-------------------------------------------------------------------
	m_info.m_hasNativeClipVertexMode = true;
	// this one uses a heuristic, and allows overrides in case the heuristic is wrong
	// or someone wants to try a beta driver or something.

	// known bad combinations get turned off here..
	
	// any ATI hardware...
	// TURNED OFF OS CHECK if (m_info.m_osComboVersion <= 0x000A0603)
	// still believe to be broken in 10.6.4
	{
		if (m_info.m_ati)
		{
			m_info.m_hasNativeClipVertexMode = false;
		}
	}
	
	// R500, forever..
	if (m_info.m_atiR5xx)
	{
		m_info.m_hasNativeClipVertexMode = false;
	}

	// if user disabled them
	if (0 /* CommandLine()->FindParm("-glmdisableclipplanes") */)
	{
		m_info.m_hasNativeClipVertexMode = false;
	}
	
	// or maybe enabled them..
	if (0 /* CommandLine()->FindParm("-glmenableclipplanes") */)
	{
		m_info.m_hasNativeClipVertexMode = true;
	}
	
	//-------------------------------------------------------------------
	m_info.m_hasOcclusionQuery = true;
	if (!strstr(gl_ext_string, "ARB_occlusion_query"))
	{
		m_info.m_hasOcclusionQuery = false;		// you don't got it!
	}
	
	//-------------------------------------------------------------------
	m_info.m_hasFramebufferBlit = true;
	if (!strstr(gl_ext_string, "EXT_framebuffer_blit"))
	{
		m_info.m_hasFramebufferBlit = false;	// you know you don't got it!
	}
	
	//-------------------------------------------------------------------
	m_info.m_maxAniso = 4;			//FIXME needs real query
	
	//-------------------------------------------------------------------
	m_info.m_hasBindableUniforms = true;
	if (!strstr(gl_ext_string, "EXT_bindable_uniform"))
	{
		m_info.m_hasBindableUniforms = false;
	}
	m_info.m_hasBindableUniforms = false;		// hardwiring this path to false until we see how to accelerate it properly
	
	//-------------------------------------------------------------------
	m_info.m_hasUniformBuffers = true;
	if (!strstr(gl_ext_string, "ARB_uniform_buffer"))
	{
		m_info.m_hasUniformBuffers = false;
	}

	//-------------------------------------------------------------------
	// test for performance pack (10.6.4+)

	bool perfPackageDetected = GLMDetectSLGU();
	
	if (perfPackageDetected)
	{
		m_info.m_hasPerfPackage1 = true;
	}	

	if (0 /* CommandLine()->FindParm("-glmenableperfpackage") */)	// force it on
	{
		m_info.m_hasPerfPackage1 = true;
	}
	
	if (0 /* CommandLine()->FindParm("-glmdisableperfpackage") */)	// force it off
	{
		m_info.m_hasPerfPackage1 = false;
	}


	//-------------------------------------------------------------------
	// runtime options that aren't negotiable once set

	m_info.m_hasDualShaders = 0; /*CommandLine()->FindParm("-glmdualshaders"); */

	//-------------------------------------------------------------------
	// "can'ts "
	
	m_info.m_cantBlitReliably = m_info.m_intel;		//FIXME X3100&10.6.3 has problems blitting.. adjust this if bug fixed in 10.6.4
	if (0 /*CommandLine()->FindParm("-glmenabletrustblit") */)
	{
		m_info.m_cantBlitReliably = false;			// we trust the blit, so set the cant-blit cap to false
	}
	if (0 /* CommandLine()->FindParm("-glmdisabletrustblit") */)
	{
		m_info.m_cantBlitReliably = true;			// we do not trust the blit, so set the cant-blit cap to true
	}

	//m_info.m_cantAttachSRGB = (m_info.m_nv && m_info.m_osComboVersion < 0x000A0600);	//NV drivers won't accept SRGB tex on an FBO color target in 10.5.8
	//m_info.m_cantAttachSRGB = (m_info.m_ati && m_info.m_osComboVersion < 0x000A0600);	//... does ATI have the same problem?
	m_info.m_cantAttachSRGB = (m_info.m_osComboVersion < 0x000A0600);	// across the board on 10.5.x actually..

	// MSAA resolve issues
	m_info.m_cantResolveFlipped	= false;	// initial stance
	
	if (m_info.m_nv)
	{
		// we're going to mark it 'broken' unless perf package 1 (10.6.4+) is present
		if (!m_info.m_hasPerfPackage1)
		{
			m_info.m_cantResolveFlipped = true;
		}
	}
	
	// this is just the private assessment of whather scaled resolve is available.
	// the activation of it will stay tied to the gl_minify_resolve_mode / gl_magnify_resolve_mode convars in glmgr
	if 	( 1 /* CommandLine()->FindParm("-gl_enable_scaled_resolve") */ )
	{
		bool scaledResolveDetected = GLMDetectScaledResolveMode( m_info.m_osComboVersion, m_info.m_hasPerfPackage1 );
		m_info.m_cantResolveScaled = !scaledResolveDetected;
	}
	else
	{
		m_info.m_cantResolveScaled = true;
	}

	// and you can force it to be "available" if you really want to..
	if ( 0 /* CommandLine()->FindParm("-gl_force_enable_scaled_resolve") */ )
	{
		m_info.m_cantResolveScaled = false;
	}
	
	// gamma decode impacting shader codegen
	m_info.m_costlyGammaFlips = false;
	if (m_info.m_osComboVersion < 0x000A0600)		// if Leopard
		m_info.m_costlyGammaFlips = true;
		
	if (m_info.m_atiR5xx)							// or r5xx - always
		m_info.m_costlyGammaFlips = true;

	if ( (m_info.m_atiR6xx) && (m_info.m_osComboVersion < 0x000A0605) )	// or r6xx prior to 10.6.5
		m_info.m_costlyGammaFlips = true;

	[nsglCtx release];
	[pixFmt release];
	
	[tempPool release];
}

GLMRendererInfo::~GLMRendererInfo( void )
{
	if (m_displays)
	{
		// delete all the new'd renderer infos that the table tracks
		for( std::vector< GLMDisplayInfo * >::iterator p = m_displays->begin(); p != m_displays->end(); p++ )
		{
			delete *p;
		}
		delete m_displays;
		m_displays = NULL;
	}
}

struct DisplayInfoLessThan
{
	bool operator ()(GLMDisplayInfo *A, GLMDisplayInfo *B)
	{
		bool bigger		= false;
		bool smaller	= true;

		// check main-ness - main should win
	
		uint maskOfMainDisplay = CGDisplayIDToOpenGLDisplayMask( CGMainDisplayID() );
		//Assert( maskOfMainDisplay==1 );	// just curious
		
		int mainscreena = (*A).m_info.m_glDisplayMask & maskOfMainDisplay;
		int mainscreenb = (*B).m_info.m_glDisplayMask & maskOfMainDisplay;
		
		if ( mainscreena > mainscreenb )
		{
			return bigger;
		}
		else if ( mainscreena < mainscreenb )
		{
			return smaller;
		}
		
		// check area - larger screen should win
		int areaa = (*A).m_info.m_displayPixelWidth * (*A).m_info.m_displayPixelHeight;
		int areab = (*B).m_info.m_displayPixelWidth * (*B).m_info.m_displayPixelHeight;
	
		if ( areaa > areab )
		{	
			return bigger;
		}
		else if ( areaa < areab )
		{
			return smaller;
		}
		
		return false;	// equal rank
	}
};

void	GLMRendererInfo::PopulateDisplays( void )
{
	Assert( !m_displays );
	m_displays = new std::vector< GLMDisplayInfo* >;
	
	for( int i=0; i<32; i++)
	{
		// check mask to see if the selected display intersects this renderer
		CGOpenGLDisplayMask dspMask = (CGOpenGLDisplayMask)(1<<i);
		
		if ( m_info.m_displayMask & dspMask )
		{
			// exclude teeny displays (they may represent offline displays)
			// exclude inactive displays
			
			CGDirectDisplayID cgid = CGOpenGLDisplayMaskToDisplayID ( dspMask );

			if ( (cgid != kCGNullDirectDisplay) && CGDisplayIsActive( cgid ) && (CGDisplayPixelsWide( cgid ) >= 512) && (CGDisplayPixelsHigh( cgid ) >= 384) )
			{
				GLMDisplayInfo *newdisp = new GLMDisplayInfo( cgid, dspMask );
				m_displays->push_back( newdisp );			
			}
		}
	}
	
	// now sort the table of displays.
	sort( m_displays->begin(), m_displays->end(), DisplayInfoLessThan() );

	// then go back and ask each display to populate its display mode table.
	for( std::vector< GLMDisplayInfo * >::iterator p = m_displays->begin(); p != m_displays->end(); p++ )
	{
		(*p)->PopulateModes();
	}
}

const char *CheesyRendererDecode( uint value )
{
	switch(value)
	{
		case 0x00020200 :  return "Generic";
		case 0x00020400 :  return "GenericFloat";
		case 0x00020600 :  return "AppleSW";
		case 0x00021000 :  return "ATIRage128";
		case 0x00021200 :  return "ATIRadeon";
		case 0x00021400 :  return "ATIRagePro";
		case 0x00021600 :  return "ATIRadeon8500";
		case 0x00021800 :  return "ATIRadeon9700";
		case 0x00021900 :  return "ATIRadeonX1000";
		case 0x00021A00 :  return "ATIRadeonX2000";
		case 0x00022000 :  return "NVGeForce2MX";
		case 0x00022200 :  return "NVGeForce3";
		case 0x00022400 :  return "NVGeForceFX";
		case 0x00022600 :  return "NVGeForce8xxx";
		case 0x00023000 :  return "VTBladeXP2";
		case 0x00024000 :  return "Intel900";
		case 0x00024200 :  return "IntelX3100";
		case 0x00040000 :  return "Mesa3DFX";

		default: return "UNKNOWN";
	}
}

extern const char *GLMDecode( GLMThing_t thingtype, unsigned long value );

void	GLMRendererInfo::Dump( int which )
{
	GLMPRINTF(("\n     #%d: GLMRendererInfo @ %08x, renderer-id=%s(%08x)  display-mask=%08x  vram=%dMB",
		which, this,
		CheesyRendererDecode( m_info.m_rendererID & 0x00FFFF00 ), m_info.m_rendererID,
		m_info.m_displayMask,
		m_info.m_vidMemory >> 20
	));
	GLMPRINTF(("\n       VendorID=%04x  DeviceID=%04x  Model=%s",
		m_info.m_pciVendorID,
		m_info.m_pciDeviceID,
		m_info.m_pciModelString
	));

	int i=0;
	for( std::vector< GLMDisplayInfo * >::iterator p = m_displays->begin(); p != m_displays->end(); p++ )
	{
		(*p)->Dump(i);
		i++;
	}
}


//===============================================================================


GLMDisplayDB::GLMDisplayDB	( void )
{
	m_renderers = NULL;	
}

GLMDisplayDB::~GLMDisplayDB	( void )
{
	if (m_renderers)
	{
		// delete all the new'd renderer infos that the table tracks
		for( std::vector< GLMRendererInfo * >::iterator p = m_renderers->begin(); p != m_renderers->end(); p++ )
		{
			delete (*p);
		}
		delete m_renderers;
		m_renderers = NULL;
	}
}

struct RendererInfoLessThan
{
	bool operator ()(GLMRendererInfo *A, GLMRendererInfo *B)
	{
		bool bigger		= false;
		bool smaller	= true;

		// check VRAM
		if ( A->m_info.m_vidMemory > B->m_info.m_vidMemory )
		{	
			return bigger;
		}
		else if ( A->m_info.m_vidMemory < B->m_info.m_vidMemory )
		{
			return smaller;
		}
		
		// check MSAA limit
		if ( A->m_info.m_maxSamples > B->m_info.m_maxSamples )
		{	
			return bigger;
		}
		else if ( A->m_info.m_maxSamples < B->m_info.m_maxSamples )
		{
			return smaller;
		}
		
		return false;	// equal rank
	}
};

void	GLMDisplayDB::PopulateRenderers( void )
{
	Assert( !m_renderers );
	m_renderers = new std::vector< GLMRendererInfo* >;
	
	// now walk the renderer list
	// find the eligible ones and insert them into vector
	// if more than one, sort the vector by desirability with favorite at 0
	// then ask each renderer object to populate its displays

	// turns out how you have to do this is to walk the display mask 1<<n..
	// and query at each one, what renderers can hit that one.
	
	// when you find one, see if it's already in the vector above. if not, add it.
	// later, we sort them.
	
	for( int i=0; i<32; i++ )
	{
		CGLError			cgl_err		= (CGLError)0;
		CGLRendererInfoObj	cgl_rend	= NULL;
		GLint				nrend;
	
		CGOpenGLDisplayMask	dspMask		= (CGOpenGLDisplayMask)(1<<i);	
		CGDirectDisplayID	cgid		= CGOpenGLDisplayMaskToDisplayID( dspMask );

		bool selected = true;	// assume the best		

		if (selected)
		{
			if ( (cgid == kCGNullDirectDisplay)  || (!CGDisplayIsActive( cgid )) )
			{
				selected = false;
			}
		}

		if (selected)
		{
			cgl_err = CGLQueryRendererInfo( dspMask, &cgl_rend, &nrend );	// FIXME this call spams the console if you ask about an out of bounds display mask
																			// "<Error>: unknown error code: invalid display"
																			// we can fix that by getting the active display mask first.
			if (!cgl_err)
			{
				// walk the renderers that can hit this display
				// add to table if not already in table, and minimums met

				for( int j=0; j<nrend; j++)
				{
					int problems = 0;
					
					GLMRendererInfoFields	fields;
					memset( &fields, 0, sizeof(fields) );

					// early out if renderer ID already in the table
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPRendererID, &fields.m_rendererID );			problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPDisplayMask, &fields.m_displayMask );		problems += (cgl_err != 0);

					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPFullScreen, &fields.m_fullscreen );			problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPAccelerated, &fields.m_accelerated );		problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPWindow, &fields.m_windowed );				problems += (cgl_err != 0);

					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPBufferModes, &fields.m_bufferModes );		problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPColorModes, &fields.m_colorModes );			problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPDepthModes, &fields.m_depthModes );			problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPStencilModes, &fields.m_stencilModes );		problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPMaxAuxBuffers, &fields.m_maxAuxBuffers );	problems += (cgl_err != 0);				
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPMaxSampleBuffers, &fields.m_maxSampleBuffers );	problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPMaxSamples, &fields.m_maxSamples );			problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPSampleModes, &fields.m_sampleModes );		problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPSampleAlpha, &fields.m_sampleAlpha );		problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPVideoMemoryMegabytes, &fields.m_vidMemory );			problems += (cgl_err != 0);
					cgl_err = CGLDescribeRenderer( cgl_rend, j,   kCGLRPTextureMemoryMegabytes, &fields.m_texMemory );		problems += (cgl_err != 0);

					// decide if this renderer goes in the table.
					// only insert renderers with at least one active display.
					
					selected = !problems;
					
					if (selected)
					{
						// grab the OS version

						SInt32 vMajor = 0;	SInt32 vMinor = 0;	SInt32 vMinorMinor = 0;
						
#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_9
						NSOperatingSystemVersion osVersion = [[NSProcessInfo processInfo] operatingSystemVersion];
						vMajor = osVersion.majorVersion;
						vMinor = osVersion.minorVersion;
						vMinorMinor = osVersion.patchVersion;
#else
						OSStatus gestalt_err = 0;
						gestalt_err = Gestalt(gestaltSystemVersionMajor, &vMajor);
						Assert(!gestalt_err);
						
						gestalt_err = Gestalt(gestaltSystemVersionMinor, &vMinor);
						Assert(!gestalt_err);

						gestalt_err = Gestalt(gestaltSystemVersionBugFix, &vMinorMinor);
						Assert(!gestalt_err);
#endif
						//encode into one quantity - 10.6.3 becomes 0x000A0603
						fields.m_osComboVersion = (vMajor << 16) | (vMinor << 8) | (vMinorMinor);

						if (0 /* CommandLine()->FindParm("-fakeleopard") */)
						{
							// lie
							fields.m_osComboVersion = 0x000A0508;
						}
						
						if (fields.m_osComboVersion < 0x000A0508)
						{
							// no support below 10.5.8
							// we'll wind up with no valid renderers and give up
							selected = false;
						}
					}
					
					if (selected)
					{
						// gather more info from IOKit
						// cribbed from http://developer.apple.com/mac/library/samplecode/VideoHardwareInfo/listing3.html
						
						CFDataRef vendorID, deviceID, model;
						io_registry_entry_t dspPort;
							
						// Get the I/O Kit service port for the display
						dspPort = CGDisplayIOServicePort( cgid );

						// Get the information for the device
						// The vendor ID, device ID, and model are all available as properties of the hardware's I/O Kit service port
						
						vendorID	= (CFDataRef)IORegistryEntrySearchCFProperty(dspPort,kIOServicePlane,CFSTR("vendor-id"),	kCFAllocatorDefault,kIORegistryIterateRecursively | kIORegistryIterateParents);
						deviceID	= (CFDataRef)IORegistryEntrySearchCFProperty(dspPort,kIOServicePlane,CFSTR("device-id"),	kCFAllocatorDefault,kIORegistryIterateRecursively | kIORegistryIterateParents);
						model		= (CFDataRef)IORegistryEntrySearchCFProperty(dspPort,kIOServicePlane,CFSTR("model"),		kCFAllocatorDefault,kIORegistryIterateRecursively | kIORegistryIterateParents);
						
						// Send the appropriate data to the outputs checking to validate the data
						if(vendorID)
						{
							fields.m_pciVendorID = *((UInt32*)CFDataGetBytePtr(vendorID));
							CFRelease( vendorID );
							vendorID = NULL;
						}
						else
						{
							fields.m_pciVendorID = 0;
						}
						
						if(deviceID)
						{
							fields.m_pciDeviceID = *((UInt32*)CFDataGetBytePtr(deviceID));
							CFRelease( deviceID );
							deviceID = NULL;
						}
						else
						{
							fields.m_pciDeviceID = 0;
						}
						
						if(model)
						{
							int length = CFDataGetLength(model);
							(void)length;

							char *data = (char*)CFDataGetBytePtr(model);
							strncpy( fields.m_pciModelString, data, sizeof(fields.m_pciModelString) );
							CFRelease( model );
							model = NULL;
						}
						else
						{
							strncpy( fields.m_pciModelString, "UnknownModel", sizeof(fields.m_pciModelString) );
						}
						

						// iterate through IOAccelerators til we find one that matches the vendorid and deviceid of this renderer (ugh!)
						// this provides the driver version string which can in turn be used to uniquely identify bad drivers and special case for them
						// first example to date - forcing vsync on 10.6.4 + NV
						
						{
							io_iterator_t	ioIterator		= (io_iterator_t)0;
							kern_return_t	ioResult		= 0;
							bool			ioDone			= false;
														
							ioResult = IOServiceGetMatchingServices( kIOMasterPortDefault, IOServiceMatching("IOAccelerator"), &ioIterator );
							if( ioResult == KERN_SUCCESS )
							{
								io_service_t	ioAccelerator = (io_service_t)0;

								while( ( !ioDone ) && ( ioAccelerator = IOIteratorNext( ioIterator ) )  )
								{
									io_service_t ioDevice;
									
									ioDevice = 0;
									ioResult = IORegistryEntryGetParentEntry( ioAccelerator, kIOServicePlane, &ioDevice);
									
									CFDataRef this_vendorID, this_deviceID;

									if(ioResult == KERN_SUCCESS)
									{
										this_vendorID	=	(CFDataRef)IORegistryEntryCreateCFProperty(ioDevice, CFSTR("vendor-id"), kCFAllocatorDefault, kNilOptions );
										this_deviceID	=	(CFDataRef)IORegistryEntryCreateCFProperty(ioDevice, CFSTR("device-id"), kCFAllocatorDefault, kNilOptions );
										
										if (this_vendorID && this_deviceID)	// null check..
										{
											// see if it matches. if so, do our business (get the extended version string), set ioDone, call it a day
											unsigned short this_vendorIDValue = *(unsigned short*)CFDataGetBytePtr(this_vendorID);
											unsigned short this_deviceIDValue = *(unsigned short*)CFDataGetBytePtr(this_deviceID);
											
											if ( (fields.m_pciVendorID == this_vendorIDValue) && (fields.m_pciDeviceID == this_deviceIDValue) )
											{
												// see if it matches. if so, do our business (get the extended version string), set ioDone, call it a day
												unsigned short* this_vendorIDBytes = (unsigned short*)CFDataGetBytePtr( this_vendorID );
												unsigned short* this_deviceIDBytes = (unsigned short*)CFDataGetBytePtr( this_deviceID );
												
												if (this_vendorIDBytes && this_deviceIDBytes)	// null check...
												{
													this_vendorIDValue = *this_vendorIDBytes;
													this_deviceIDValue = *this_deviceIDBytes;
													
													if ( (fields.m_pciVendorID == this_vendorIDValue) && (fields.m_pciDeviceID == this_deviceIDValue) )
													{
														// match, stop looking
														ioDone = true;
														
														// get extended info
														CFStringRef this_ioglName = (CFStringRef)IORegistryEntryCreateCFProperty( ioAccelerator, CFSTR("IOGLBundleName"), kCFAllocatorDefault, kNilOptions );
	
														NSString *bundlePath = [ NSString stringWithFormat:@"/System/Library/Extensions/%@.bundle", this_ioglName ];
														
														NSDictionary* this_driverDict = [ [NSBundle bundleWithPath: bundlePath] infoDictionary ];
														if (this_driverDict)
														{
															NSString* this_driverInfo = [ this_driverDict objectForKey:@"CFBundleGetInfoString" ];
															if ( this_driverInfo )
															{
																const char* theString = [ this_driverInfo UTF8String ];
																
																strncpy(fields.m_driverInfoString, theString, sizeof( fields.m_driverInfoString )  );
															}												
														}
														
														// [bundlePath release];
														
														CFRelease(this_ioglName);
													}
												}
	
												CFRelease(this_vendorID);
												CFRelease(this_deviceID);
											}
										}
									}
								}

								IOObjectRelease(ioAccelerator);
							}

							IOObjectRelease(ioIterator);
						}

						// generate shorthand bools
						switch( fields.m_pciVendorID )
						{
							case	0x1002:	//ATI
							{
								fields.m_ati = true;

								// http://www.pcidatabase.com/search.php?device_search_str=radeon&device_search.x=0&device_search.y=0&device_search=search+devices
								
								// Mac-relevant ATI R5xx PCI device ID's lie in this range: 0x7100 - 0x72FF
								// X1600, X1900, X1950
								if ( (fields.m_pciDeviceID >= 0x7100) && (fields.m_pciDeviceID <= 0x72ff) )
								{
									fields.m_atiR5xx = true;
								}

								// R6xx PCI device ID's lie in these ranges:
									// 0x94C1 - 0x9515 ... also 0x9581 - 0x9713
									// 2400HD, 2600HD, 3870, et al
								if	( 
										( (fields.m_pciDeviceID >= 0x94C1) && (fields.m_pciDeviceID <= 0x9515) )
									||	( (fields.m_pciDeviceID >= 0x9581) && (fields.m_pciDeviceID <= 0x9713) )
									)
								{
									fields.m_atiR6xx = true;
								}

								// R7xx PCI device ID's lie in: 0x9440 - 0x9460, also 9480-94b5.
								// why there is an HD5000 at 9462, I dunno.  Don't think that's an R8xx part.
								if	( 
										( (fields.m_pciDeviceID >= 0x9440) && (fields.m_pciDeviceID <= 0x9460) )
									||	( (fields.m_pciDeviceID >= 0x9480) && (fields.m_pciDeviceID <= 0x94B5) )
									)
								{
									fields.m_atiR7xx = true;
								}
								
								// R8xx: 0x6898-0x68BE
								if ( (fields.m_pciDeviceID >= 0x6898) && (fields.m_pciDeviceID <= 0x68Be) )
								{
									fields.m_atiR8xx = true;
								}

								#if 0
										// turned off, but we could use this for cross check.
										// we could also use the bit encoding of the renderer ID to ferret out a geberation clue.
										
										// string-scan for each generation
										// this could be a lot better if we got the precise PCI ID's used and/or cross-ref'd that against the driver name
										if (strstr("X1600", fields.m_pciModelString) || strstr("X1900", fields.m_pciModelString) || strstr("X1950", fields.m_pciModelString) )
										{
											fields.m_atiR5xx = true;
										}

										if (strstr("2600", fields.m_pciModelString) || strstr("3870", fields.m_pciModelString) || strstr("X2000", fields.m_pciModelString) )
										{
											fields.m_atiR6xx = true;
										}

										if (strstr("4670", fields.m_pciModelString) || strstr("4650", fields.m_pciModelString) || strstr("4850", fields.m_pciModelString)|| strstr("4870", fields.m_pciModelString) )
										{
											fields.m_atiR7xx = true;
										}
								#endif
							}
							break;
							
							case	0x8086:	//INTC
							{
								fields.m_intel = true;
								
								switch( fields.m_pciDeviceID )
								{
									case	0x27A6:	fields.m_intel95x = true;	break;	// GMA 950
									case	0x2A02:	fields.m_intel3100 = true;	break;	// X3100
									
									default:
									{
										if (fields.m_pciDeviceID > 0x2A02)		// assume ascending ID's for newer devices
										{
											fields.m_intelNewer = true;
										}
									}
								}
							}
							break;
							
							case	0x10DE:	//NV
							{
								fields.m_nv = true;

								// G7x: 0x0391 0x393 0x0395 (7300/7600 GT)  0x009D (Quadro FX)
								if	( (fields.m_pciDeviceID == 0x0391) || (fields.m_pciDeviceID == 0x0393) || (fields.m_pciDeviceID == 0x0395) || (fields.m_pciDeviceID == 0x009D) )
								{
									fields.m_nvG7x = true;
								}
								
								// G8x: 0400-04ff, also 0x5E1 (GTX280) through 0x08FF
								if	(
										( (fields.m_pciDeviceID >= 0x0400) && (fields.m_pciDeviceID <= 0x04ff) )
									||	( (fields.m_pciDeviceID >= 0x05E1) && (fields.m_pciDeviceID <= 0x08ff) )
									)
								{
									fields.m_nvG8x = true;
								}

								if ( fields.m_pciDeviceID > 0x0900 )
								{
									fields.m_nvNewer = true;
								}
								
								// detect the specific revision of NV driver in 10.6.4 that caused all the grief
								if (strstr(fields.m_driverInfoString, "1.6.16.11 (19.5.8f01)"))
								{
									fields.m_badDriver1064NV = true;
								}
							}
							break;
						}						
					}
					
					if (selected)
					{
						// dupe check
						for( std::vector< GLMRendererInfo * >::iterator p = m_renderers->begin(); p != m_renderers->end(); p++ )
						{
							uint rendid = (*p)->m_info.m_rendererID;
							
							if ( rendid == fields.m_rendererID )
							{
								// don't add to table, it's a dupe
								selected = false;
							}
						}
					}
					
					if (selected)
					{
						// criteria check
						if (fields.m_fullscreen==0)
							selected = false;
						if (fields.m_accelerated==0)
							selected = false;
						if (fields.m_windowed==0)
							selected = false;
					}
					
					// we need something here that will exclude the renderer if it does not have any good displays attached.
					
					Assert( fields.m_displayMask != 0 );
					
					if (selected)
					{
						// add to table
						// note this constructor makes a dummy context just long enough to query remaining fields in the m_info.
						GLMRendererInfo *newinfo = new GLMRendererInfo( &fields );
						m_renderers->push_back( newinfo );
					}
				}
				if (cgl_rend)
				{
					CGLDestroyRendererInfo( cgl_rend );
				}
			}
		}
	}
	
	// now sort the table.
	sort( m_renderers->begin(), m_renderers->end(), RendererInfoLessThan() );

	// then go back and ask each renderer to populate its display info table.
	for( std::vector< GLMRendererInfo * >::iterator p = m_renderers->begin(); p != m_renderers->end(); p++ )
	{
		(*p)->PopulateDisplays();
	}
}

void	GLMDisplayDB::PopulateFakeAdapters( uint realRendererIndex )		// fake adapters = one real adapter times however many displays are on it
{
	// presumption is that renderers have been populated.
	Assert( GetRendererCount() > 0 );
	Assert( realRendererIndex < GetRendererCount() );
	
	m_fakeAdapters.clear();
	
	// for( int r = 0; r < GetRendererCount(); r++ )
	int r = realRendererIndex;
	{
		for( int d = 0; d < GetDisplayCount( r ); d++ )
		{
			GLMFakeAdapter temp;
			
			temp.m_rendererIndex = r;
			temp.m_displayIndex = d;
			
			m_fakeAdapters.push_back( temp );
		}
	}
}

void	GLMDisplayDB::Populate(void)
{
	this->PopulateRenderers();
	
	// passing in zero here, constrains the set of fake adapters (GL renderer + a display) to the ones using the highest ranked renderer.
	//FIXME introduce some kind of convar allowing selection of other GPU's in the system.
	
	int realRendererIndex = 0;

	/*
	if (CommandLine()->FindParm("-glmrenderer0"))
		realRendererIndex = 0;
	if (CommandLine()->FindParm("-glmrenderer1"))
		realRendererIndex = 1;
	if (CommandLine()->FindParm("-glmrenderer2"))
		realRendererIndex = 2;
	if (CommandLine()->FindParm("-glmrenderer3"))
		realRendererIndex = 3;
	*/
	
	if (realRendererIndex >= GetRendererCount())
	{
		// fall back to 0
		realRendererIndex = 0;
	}
	
	this->PopulateFakeAdapters( 0 );

	#if GLMDEBUG
		this->Dump();
	#endif
}
	


int		GLMDisplayDB::GetFakeAdapterCount( void )
{
	return m_fakeAdapters.size();
}

bool	GLMDisplayDB::GetFakeAdapterInfo( int fakeAdapterIndex, int *rendererOut, int *displayOut, GLMRendererInfoFields *rendererInfoOut, GLMDisplayInfoFields *displayInfoOut )
{
	if (fakeAdapterIndex >= GetFakeAdapterCount() )
	{
		*rendererOut = 0;
		*displayOut = 0;
		return true;		// fail
	}

	*rendererOut = m_fakeAdapters[fakeAdapterIndex].m_rendererIndex;
	*displayOut = m_fakeAdapters[fakeAdapterIndex].m_displayIndex;

	bool rendResult = GetRendererInfo( *rendererOut, rendererInfoOut );
	bool dispResult = GetDisplayInfo( *rendererOut, *displayOut, displayInfoOut );
	
	return rendResult || dispResult;
}
	

int		GLMDisplayDB::GetRendererCount( void )
{
	return	m_renderers->size();
}

bool	GLMDisplayDB::GetRendererInfo( int rendererIndex, GLMRendererInfoFields *infoOut )
{
	memset( infoOut, 0, sizeof( GLMRendererInfoFields ) );

	if (rendererIndex >= GetRendererCount())
		return true; // fail
	
	GLMRendererInfo *rendInfo = (*m_renderers)[rendererIndex];		
	*infoOut = rendInfo->m_info;

	return false;
}

int		GLMDisplayDB::GetDisplayCount( int rendererIndex )
{
	if (rendererIndex >= GetRendererCount())
		return 0; // fail
	
	GLMRendererInfo *rendInfo = (*m_renderers)[rendererIndex];
		
	return	rendInfo->m_displays->size();
}

bool	GLMDisplayDB::GetDisplayInfo( int rendererIndex, int displayIndex, GLMDisplayInfoFields *infoOut )
{
	memset( infoOut, 0, sizeof( GLMDisplayInfoFields ) );
	
	if (rendererIndex >= GetRendererCount())
		return true; // fail
	
	if (displayIndex >= GetDisplayCount(rendererIndex))
		return true; // fail
	
	GLMDisplayInfo *displayInfo = (*(*m_renderers)[rendererIndex]->m_displays)[displayIndex];
	*infoOut = displayInfo->m_info;

	return false;
}

int		GLMDisplayDB::GetModeCount( int rendererIndex, int displayIndex )
{
	if (rendererIndex >= GetRendererCount())
		return 0; // fail
	
	if (displayIndex >= GetDisplayCount(rendererIndex))
		return 0; // fail
		
	GLMDisplayInfo *displayInfo = (*(*m_renderers)[rendererIndex]->m_displays)[displayIndex];

	return displayInfo->m_modes->size();
}

bool	GLMDisplayDB::GetModeInfo( int rendererIndex, int displayIndex, int modeIndex, GLMDisplayModeInfoFields *infoOut )
{
	memset( infoOut, 0, sizeof( GLMDisplayModeInfoFields ) );
	
	if (rendererIndex >= GetRendererCount())
		return true; // fail
	
	if (displayIndex >= GetDisplayCount(rendererIndex))
		return true; // fail
	
	if (modeIndex >= GetModeCount(rendererIndex,displayIndex))
		return true; // fail
	
	if (modeIndex>=0)
	{
		GLMDisplayMode *displayModeInfo = (*(*(*m_renderers)[rendererIndex]->m_displays)[displayIndex]->m_modes)[ modeIndex ];
		*infoOut = displayModeInfo->m_info;
	}
	else
	{
		// passing modeIndex = -1 means "tell me about current mode"..

		GLMRendererInfo		*rendInfo = (*m_renderers)[ rendererIndex ];
		GLMDisplayInfo		*dispinfo = (*rendInfo ->m_displays)[displayIndex];	
		CGDirectDisplayID	cgid = dispinfo->m_info.m_cgDisplayID;
		
		CGDisplayModeRef		cgMode = CGDisplayCopyDisplayMode( cgid );

		// get the mode number from the mode dict (using system mode numbering, not our sorted numbering)
		if (cgMode)
		{
			// grab the width and height, I am unclear on whether this is the displayed FB width or the display device width.
			int screenWidth=CGDisplayModeGetWidth( cgMode );
			int screenHeight=CGDisplayModeGetHeight( cgMode );
			int refreshHz=CGDisplayModeGetRefreshRate( cgMode );
			
			GLMPRINTF(( "-D- GLMDisplayDB::GetModeInfo sees mode-index=%d, width=%d, height=%d on CGID %08x (display index %d on rendererindex %d)", 
				modeIndex,
				screenWidth,
				screenHeight,
				cgid,
				displayIndex,
				rendererIndex ));

			// now match
			int foundIndex = -1;
			(void)foundIndex;

			int i=0;
			for( std::vector< GLMDisplayMode * >::iterator p = (*dispinfo).m_modes->begin(); p != (*dispinfo).m_modes->end(); p++ )
			{
				GLMDisplayMode *mode = (*p);
				
				if (mode->m_info.m_modePixelWidth == screenWidth)
				{
					if (mode->m_info.m_modePixelHeight == screenHeight)
					{
						if (mode->m_info.m_modeRefreshHz == refreshHz)
						{
							foundIndex = i;
							*infoOut = mode->m_info;
							return false;
						}
					}
				}
				i++;
			}
		}

		// if we get here, we could not find the mode
		memset( infoOut, 0, sizeof( *infoOut ) );
		return true; // fail
	}
	return false;
}


void	GLMDisplayDB::Dump( void )
{
	GLMPRINTF(("\n GLMDisplayDB @ %08x ",this ));

	int i=0;
	for( std::vector< GLMRendererInfo * >::iterator p = m_renderers->begin(); p != m_renderers->end(); p++ )
	{
		(*p)->Dump(i);
		i++;
	}
}


