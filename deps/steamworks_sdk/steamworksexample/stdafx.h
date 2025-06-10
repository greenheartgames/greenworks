//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================


// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>
#include <stdarg.h>

#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))

#ifdef WIN32

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
// Allow use of features specific to Windows 8.1 or later.
// Change this to the appropriate value to target other versions of Windows.
#ifndef WINVER
#define WINVER 0x0602
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0602
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0602
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )
#pragma comment( lib, "dxguid.lib" )

// Windows Header Files:
#include <windows.h>
#include <tchar.h>

// Winsock
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib" )

// d3d header files
#include "d3d9.h"
#include "d3dx9.h"

// XAudio2 header files
#include <xaudio2.h>

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

#include "steam/isteamuserstats.h"
#include "steam/isteamremotestorage.h"
#include "steam/isteammatchmaking.h"
#include "steam/steam_gameserver.h"

#elif defined(POSIX)

#include <limits.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

#if defined(OSX)	
	#include <OpenGL/OpenGL.h>
#endif

#define ARRAYSIZE(A) ( sizeof(A)/sizeof(A[0]) )
// Need to define some types on POSIX
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef uint32 DWORD;
typedef DWORD HWND;
typedef DWORD HINSTANCE;
typedef short SHORT;
typedef long LONG;
typedef unsigned char byte;
typedef unsigned char uint8;

/* Font Weights */
#define FW_DONTCARE         0
#define FW_THIN             100
#define FW_EXTRALIGHT       200
#define FW_LIGHT            300
#define FW_NORMAL           400
#define FW_MEDIUM           500
#define FW_SEMIBOLD         600
#define FW_BOLD             700
#define FW_EXTRABOLD        800
#define FW_HEAVY            900

/* Some VK_ defines from windows, we'll map these for posix */
#define VK_BACK           0x08
#define VK_TAB            0x09
#define VK_RETURN         0x0D
#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_ESCAPE         0x1B
#define VK_SPACE          0x20
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_F5			  0x74

#ifndef VALVE_RECT_DEFINED
#define VALVE_RECT_DEFINED

	typedef struct tagRECT
	{
		LONG    left;
		LONG    top;
		LONG    right;
		LONG    bottom;
	} RECT;

	#define _RECT tagRECT
#endif

#define D3DCOLOR_ARGB(a,r,g,b) \
	((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

// Macros for converting ARGB DWORD color representation into opengl formats...
#define COLOR_RED( color ) \
	(GLubyte)(((color)>>16)&0xff)

#define COLOR_GREEN( color ) \
	(GLubyte)(((color)>>8)&0xff)

#define COLOR_BLUE( color ) \
	(GLubyte)((color)&0xff)

#define COLOR_ALPHA( color ) \
	(GLubyte)(((color)>>24)&0xff)

#define DWARGB_TO_DWRGBA(color) \
	((DWORD)(( (((((color)>>16)&0xff))<<24)|(((((color)>>8)&0xff))<<16)|((color&0xff)<<8)|((color)>>24)&0xff)))

#define DWARGB_TO_DWABGR(color) \
	((DWORD)(( (((((color)>>24)&0xff))<<24)|(((((color))&0xff))<<16)|(((color>>8)&0xff)<<8)|((color)>>16)&0xff)))

#define DWRGBA_TO_DWARGB(color) \
	((DWORD)(( (((((color))&0xff))<<24)|(((((color>>24))&0xff))<<16)|(((color>>16)&0xff)<<8)|((color)>>8)&0xff)))

// steam api header file
#include "steam/steam_api.h"
#include "steam/isteamuserstats.h"
#include "steam/isteamremotestorage.h"
#include "steam/isteammatchmaking.h"
#include "steam/steam_gameserver.h"

extern void OutputDebugString( const char *pchMsg );
extern int Alert( const char *lpCaption, const char *lpText );
extern const char *GetUserSaveDataPath();

#ifdef OSX
extern uint64_t GetTickCount();
#endif // OSX

#define V_ARRAYSIZE(a) sizeof(a)/sizeof(a[0]) 

#endif	// POSIX

// OUT_Z_ARRAY indicates an output array that will be null-terminated.
#if _MSC_VER >= 1600
       // Include the annotation header file.
       #include <sal.h>
       #if _MSC_VER >= 1700
              // VS 2012+
              #define OUT_Z_ARRAY _Post_z_
       #else
              // VS 2010
              #define OUT_Z_ARRAY _Deref_post_z_
       #endif
#else
       // gcc, clang, old versions of VS
       #define OUT_Z_ARRAY
#endif
 
template <size_t maxLenInChars> void sprintf_safe(OUT_Z_ARRAY char (&pDest)[maxLenInChars], const char *pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
#ifdef POSIX
	vsnprintf( pDest, maxLenInChars, pFormat, params );
#else
	_vsnprintf( pDest, maxLenInChars, pFormat, params );
#endif
	pDest[maxLenInChars - 1] = '\0';
	va_end( params );
}

inline void strncpy_safe( char *pDest, char const *pSrc, size_t maxLen )
{
	size_t nCount = maxLen;
	char *pstrDest = pDest;
	const char *pstrSource = pSrc;

	while ( 0 < nCount && 0 != ( *pstrDest++ = *pstrSource++ ) )
		nCount--;

	if ( maxLen > 0 )
		pstrDest[-1] = 0;
}

#ifdef STEAM_CEG
// Steam DRM header file
#include "cegclient.h"
#else
#define Steamworks_InitCEGLibrary() (true)
#define Steamworks_TermCEGLibrary()
#define Steamworks_TestSecret()
#define Steamworks_SelfCheck()
#endif

