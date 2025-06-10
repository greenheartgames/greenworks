//========= Copyright � 1996-2009, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking leaderboards
//
//=============================================================================

#include "stdafx.h"
#include "musicplayer.h"
#include "BaseMenu.h"
#include <math.h>

//-----------------------------------------------------------------------------
// Purpose: Menu that shows a music player
//-----------------------------------------------------------------------------
class CMusicPlayerMenu : public CBaseMenu< MusicPlayerMenuItem_t >
{
public:

	//-----------------------------------------------------------------------------
	// Purpose: Constructor
	//-----------------------------------------------------------------------------
	CMusicPlayerMenu( IGameEngine *pGameEngine ) 
		: CBaseMenu< MusicPlayerMenuItem_t >( pGameEngine )
		, m_menuItemPause( "Pause" )
		, m_menuItemPlay( "Play" )
		, m_menuItemPlayPrevious( "Play Previous" )
		, m_menuItemPlayNext( "Play Next" )
		, m_menuItemIncreaseVolume( "Increase Volume" )
		, m_menuItemDecreaseVolume( "Decrease Volume" )
		, m_menuItemBack( "Back" )
	{
	}

	//-----------------------------------------------------------------------------
	// Purpose: Creates menu
	//-----------------------------------------------------------------------------
	void Rebuild()
	{
		PushSelectedItem();
		ClearMenuItems();

		AddMenuItem( CMusicPlayerMenu::MenuItem_t( "Pause", m_menuItemPause ) );
		AddMenuItem( CMusicPlayerMenu::MenuItem_t( "Play", m_menuItemPlay ) );
		AddMenuItem( CMusicPlayerMenu::MenuItem_t( "Play Previous", m_menuItemPlayPrevious ) );
		AddMenuItem( CMusicPlayerMenu::MenuItem_t( "Play Next", m_menuItemPlayNext ) );
		AddMenuItem( CMusicPlayerMenu::MenuItem_t( "Increase Volume", m_menuItemIncreaseVolume ) );
		AddMenuItem( CMusicPlayerMenu::MenuItem_t( "Decrease Volume", m_menuItemDecreaseVolume ) );
		AddMenuItem( CMusicPlayerMenu::MenuItem_t( "Return to main menu", m_menuItemBack ) );

		UpdateHeading();

		PopSelectedItem();
	}


	//-----------------------------------------------------------------------------
	// Purpose:
	//-----------------------------------------------------------------------------
	void UpdateHeading()
	{
		const char *pchEnabled = "Disabled";

		if ( SteamMusic()->BIsEnabled() )
		{
			pchEnabled = "Enabled";
		}

		const char *pchPlaybackStatus = "";
 		AudioPlayback_Status nStatus = SteamMusic()->GetPlaybackStatus();
 		switch( nStatus )
 		{
 		case AudioPlayback_Undefined:
			pchPlaybackStatus = "Undefined"; 
			break;
 		case AudioPlayback_Playing:
			pchPlaybackStatus = "Playing";
			break;
 		case AudioPlayback_Paused:
			pchPlaybackStatus = "Paused";
			break;
 		case AudioPlayback_Idle:
			pchPlaybackStatus = "Done";
			break;
 		};


		// Music Volume is between 0.0 and 1.0: multiply by ten, so its equivalent to Big Picture display.
		float flVolume = SteamMusic()->GetVolume();
		int nVolume = int( flVolume * 10 );

		sprintf_safe( m_szHeadingString, "Music: %s Status: %s Volume: %d", pchEnabled, pchPlaybackStatus, nVolume );
		SetHeading( m_szHeadingString );
	}


	//-----------------------------------------------------------------------------
	// Purpose: variables
	//-----------------------------------------------------------------------------
	char m_szHeadingString[255];		// String to show in server browser

	MusicPlayerMenuItem_t m_menuItemPause;
	MusicPlayerMenuItem_t m_menuItemPlay;
	MusicPlayerMenuItem_t m_menuItemPlayPrevious;
	MusicPlayerMenuItem_t m_menuItemPlayNext;

	MusicPlayerMenuItem_t m_menuItemIncreaseVolume;
	MusicPlayerMenuItem_t m_menuItemDecreaseVolume;

	MusicPlayerMenuItem_t m_menuItemBack;
};




//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMusicPlayer::CMusicPlayer( IGameEngine *pGameEngine ) 
	: m_pGameEngine( pGameEngine )
	, m_CallbackVolumeChanged( this, &CMusicPlayer::OnVolumeChanged )
	, m_CallbackPlaybackStatusHasChanged( this, &CMusicPlayer::OnPlaybackStatusHasChanged )
{
	m_pMusicPlayerMenu = new CMusicPlayerMenu( pGameEngine );
}


//-----------------------------------------------------------------------------
// Purpose: Run a frame
//-----------------------------------------------------------------------------
void CMusicPlayer::RunFrame()
{
	m_pMusicPlayerMenu->RunFrame();	
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions
//-----------------------------------------------------------------------------
void CMusicPlayer::OnMenuSelection( MusicPlayerMenuItem_t selection )
{
	if (selection == m_pMusicPlayerMenu->m_menuItemPlay)
	{
		SteamMusic()->Play();
	}
	else if (selection == m_pMusicPlayerMenu->m_menuItemPause)
	{
		SteamMusic()->Pause();
	}
	else if (selection == m_pMusicPlayerMenu->m_menuItemPlayPrevious)
	{
		SteamMusic()->PlayPrevious();
	}
	else if (selection == m_pMusicPlayerMenu->m_menuItemPlayNext)
	{
		SteamMusic()->PlayNext();
	}
	else if (selection == m_pMusicPlayerMenu->m_menuItemIncreaseVolume)
	{
		// conversion necessary, because the UI in big picture shows 10 bars,
		// but volume is a value between 0.0 and 1.0
		float flVolume = SteamMusic()->GetVolume();
		int nVolume = int( flVolume * 10 );
		nVolume = MIN( nVolume + 1, 10 );
		SteamMusic()->SetVolume( (float)nVolume * 0.1f );
	}
	else if (selection == m_pMusicPlayerMenu->m_menuItemDecreaseVolume)
	{
		// conversion necessary, because the UI in big picture shows 10 bars,
		// but volume is a value between 0.0 and 1.0
		float flVolume = SteamMusic()->GetVolume();
		int nVolume = int( flVolume * 10 );
		nVolume = MAX( nVolume - 1, 0 );
		SteamMusic()->SetVolume( (float)nVolume * 0.1f );
	}
	else if (selection == m_pMusicPlayerMenu->m_menuItemBack)
	{
		SpaceWarClient()->SetGameState(k_EClientGameMenu);
	}
}


//-----------------------------------------------------------------------------
// Purpose: Shows / Refreshes
//-----------------------------------------------------------------------------
void CMusicPlayer::Show()
{
	m_pMusicPlayerMenu->Rebuild();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CMusicPlayer::OnPlaybackStatusHasChanged(  PlaybackStatusHasChanged_t *pPlaybackStatusHasChanged )
{
	m_pMusicPlayerMenu->UpdateHeading();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CMusicPlayer::OnVolumeChanged(  VolumeHasChanged_t *pVolumeChanged )
{
	m_pMusicPlayerMenu->UpdateHeading();
}