//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for handling finding servers, getting their details, and displaying
// them inside the game
//
// $NoKeywords: $
//=============================================================================

#ifndef HTMLSURFACE_H
#define HTMLSURFACE_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "steam/steam_api.h"
#include "steam/isteamhtmlsurface.h"


class CHTMLSurface 
{
public:
	CHTMLSurface( IGameEngine *pGameEngine );
	~CHTMLSurface();

	// Run a frame (to handle kb input and such as well as render)
	void RunFrame();

	void Render();

	void Show();

private:
	STEAM_CALLBACK( CHTMLSurface, OnStartRequest, HTML_StartRequest_t ); // REQUIRED
	STEAM_CALLBACK( CHTMLSurface, OnJSAlert, HTML_JSAlert_t ); // REQUIRED
	STEAM_CALLBACK( CHTMLSurface, OnJSConfirm, HTML_JSConfirm_t ); // REQUIRED
	STEAM_CALLBACK( CHTMLSurface, OnUploadLocalFile, HTML_FileOpenDialog_t ); // REQUIRED

	STEAM_CALLBACK( CHTMLSurface, OnNeedsPaint, HTML_NeedsPaint_t );
	STEAM_CALLBACK( CHTMLSurface, OnCloseBrowser, HTML_CloseBrowser_t );
	STEAM_CALLBACK( CHTMLSurface, OnFinishedRequest, HTML_FinishedRequest_t );
	STEAM_CALLBACK( CHTMLSurface, OnBrowserRestarted, HTML_BrowserRestarted_t );


	void OnBrowserReady( HTML_BrowserReady_t *pBrowserReady, bool bIOFailure );
	CCallResult< CHTMLSurface, HTML_BrowserReady_t > m_SteamCallResultBrowserReady;


	// Pointer to engine instance (so we can draw stuff)
	IGameEngine *m_pGameEngine;

	HGAMEFONT m_hDisplayFont;
	
	HHTMLBrowser m_unBrowserHandle; // handle to the html surface object
	HGAMETEXTURE m_hHTMLTexture; // the texture data for the page

	uint32 m_unHTMLWide; // the size of the html page we want to show
	uint32 m_unHTMLTall;

};

#endif //HTMLSURFACE_H