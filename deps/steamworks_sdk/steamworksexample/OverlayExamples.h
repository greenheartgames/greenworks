//========= Copyright © Valve LLC, All rights reserved. ============
//
// Purpose: Class that shows some examples for bringing up the Steam Overlay
//
//=============================================================================

#ifndef OVERLAYEXAMPLES_H
#define OVERLAYEXAMPLES_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "SpaceWarClient.h"


class CSpaceWarClient;
class COverlayExamplesMenu;

class COverlayExamples
{
public:
	// Constructor
	COverlayExamples( IGameEngine *pGameEngine );

	// Run a frame
	void RunFrame();

	// shows / refreshes item store
	void Show();

	// handles input from menu
	void OnMenuSelection( OverlayExample_t selection );

	bool BHasLastGamePhase() const { return !m_strLastGamePhaseIDToShow.empty();  }
	bool BHasLastTimelineEvent() const { return m_ulLastCrashIntoSunEventIDToShow != 0; }

private:	
	
	// Engine
	IGameEngine *m_pGameEngine;
	
	COverlayExamplesMenu *m_pMenu;
	OverlayExample_t m_delayedCommand;
	std::string m_strLastGamePhaseIDToShow;
	uint64 m_ulLastCrashIntoSunEventIDToShow = 0;

	STEAM_CALLBACK( COverlayExamples, OnScreenshotRequested, ScreenshotRequested_t );
	STEAM_CALLBACK( COverlayExamples, OnSteamScreenshotReady, ScreenshotReady_t );

	// callback for when we ask about an event having recordings
	void OnDoesEventRecordingExist( SteamTimelineEventRecordingExists_t *pCallback, bool bIOFailure );
	CCallResult<COverlayExamples, SteamTimelineEventRecordingExists_t> m_SteamCallResultDoesEventRecordingExist;

	// callback for when we ask about a phase having recordings
	void OnDoesGamePhaseRecordingExist( SteamTimelineGamePhaseRecordingExists_t *pCallback, bool bIOFailure );
	CCallResult<COverlayExamples, SteamTimelineGamePhaseRecordingExists_t> m_SteamCallResultDoesGamePhaseRecordingExist;
};

#endif // OVERLAYEXAMPLES_H
