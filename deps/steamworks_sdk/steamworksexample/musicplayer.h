//========= Copyright © 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for controlling the Music Player
//
//=============================================================================

#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include "GameEngine.h"

class CMusicPlayerMenu;


struct MusicPlayerMenuItem_t
{
	const char *m_pchCommand;

	MusicPlayerMenuItem_t() : m_pchCommand( "" ) {}
	MusicPlayerMenuItem_t( const char *pchCommand ) : m_pchCommand( pchCommand ) {}

	bool operator==( const MusicPlayerMenuItem_t& rhs ) const
	{
		return strncmp( m_pchCommand, rhs.m_pchCommand, strlen( m_pchCommand ) ) == 0;
	}
};



class CMusicPlayer
{
public:
	// Constructor
	CMusicPlayer( IGameEngine *pGameEngine );

	// Run a frame
	void RunFrame();

	// shows / refreshes music player
	void Show();

	// handles input from menu 
	void OnMenuSelection( MusicPlayerMenuItem_t selection );	

private:

	IGameEngine *m_pGameEngine;
	CMusicPlayerMenu *m_pMusicPlayerMenu;

	STEAM_CALLBACK( CMusicPlayer, OnPlaybackStatusHasChanged, PlaybackStatusHasChanged_t, m_CallbackPlaybackStatusHasChanged );
	STEAM_CALLBACK( CMusicPlayer, OnVolumeChanged, VolumeHasChanged_t, m_CallbackVolumeChanged );
};

#endif // MUSICPLAYER_H