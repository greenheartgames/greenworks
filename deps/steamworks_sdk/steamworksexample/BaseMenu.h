//========= Copyright (c) 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Base class for various game menu screens
//
// $NoKeywords: $
//=============================================================================

#ifndef BASEMENU_H
#define BASEMENU_H

#include <string>
#include <vector>
#include "GameEngine.h"
#include "SpaceWar.h"
#include "SpaceWarClient.h"
#include "steam/isteamcontroller.h"

#define MENU_FONT_HEIGHT 24
#define MENU_ITEM_PADDING 12

extern HGAMEFONT g_hMenuFont;
extern uint64 g_ulLastReturnKeyTick;
extern uint64 g_ulLastKeyDownTick;
extern uint64 g_ulLastKeyUpTick;

template <class T> class CBaseMenu
{
public:
	// Typedef for menu items
	typedef std::pair<std::string, T> MenuItem_t;

	// Constructor
	CBaseMenu( IGameEngine *pGameEngine )
	{
		m_pGameEngine = pGameEngine;

		m_uSelectedItem = 0;
		m_bSelectionPushed = false;

		if ( !g_hMenuFont )
		{
			g_hMenuFont = pGameEngine->HCreateFont( MENU_FONT_HEIGHT, FW_BOLD, false, "Arial" );
			if ( !g_hMenuFont )
				OutputDebugString( "Menu font was not created properly, text won't draw\n" );
		}

	}

	// Destructor
	virtual ~CBaseMenu() { }

	// Sets a heading for the menu
	void SetHeading( const char *pchHeading )
	{
		m_sHeading = pchHeading;
	}

	// Clear all menu entries
	void ClearMenuItems() 
	{
		m_VecMenuItems.clear();
		m_uSelectedItem = 0;
	}

	// Add a menu item to the menu
	void AddMenuItem( MenuItem_t item )
	{
		m_VecMenuItems.push_back( item );
	}

	void PushSelectedItem()
	{
		if ( m_VecMenuItems.size() )
		{
			m_bSelectionPushed = true;
			m_selection = m_VecMenuItems[m_uSelectedItem].second;
		}
	}

	void PopSelectedItem()
	{
		if ( m_bSelectionPushed )
		{
			m_bSelectionPushed = false;

			// find the item and set it as selected if it exists
			for ( unsigned int i = 0; i < m_VecMenuItems.size(); i++ )
			{
				if ( !memcmp( &m_VecMenuItems[i].second, &m_selection, sizeof( m_selection ) ) )
				{
					m_uSelectedItem = i;
					break;
				}
			}
		}
	}

	// Run a frame + render
	void RunFrame()
	{
		// Note: The below code uses globals that are shared across all menus to avoid double
		// key press registration, this is so that when you do something like hit return in the pause 
		// menu to "go back to main menu" you don't end up immediately registering a return in the
		// main menu afterwards.

		// check if the enter key is down, if it is take action
		if ( m_pGameEngine->BIsKeyDown( VK_RETURN ) || 
			m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_MenuSelect ) )
		{
			uint64 ulCurrentTickCount = m_pGameEngine->GetGameTickCount();
			if ( ulCurrentTickCount - 220 > g_ulLastReturnKeyTick )
			{
				g_ulLastReturnKeyTick = ulCurrentTickCount;
				if ( m_uSelectedItem < m_VecMenuItems.size() )
				{
					SpaceWarClient()->OnMenuSelection( m_VecMenuItems[m_uSelectedItem].second );
					return;
				}
			}
		}
		// Check if we need to change the selected menu item
		else if ( m_pGameEngine->BIsKeyDown( VK_DOWN ) || 
			m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_MenuDown ) )
		{
			uint64 ulCurrentTickCount = m_pGameEngine->GetGameTickCount();
			if ( ulCurrentTickCount - 140 > g_ulLastKeyDownTick )
			{
				g_ulLastKeyDownTick = ulCurrentTickCount;
				if ( m_uSelectedItem < m_VecMenuItems.size() - 1 )
					m_uSelectedItem++;
				else
					m_uSelectedItem = 0;
			}
		}
		else if ( m_pGameEngine->BIsKeyDown( VK_UP ) || 
			m_pGameEngine->BIsControllerActionActive( eControllerDigitalAction_MenuUp ) )
		{
			uint64 ulCurrentTickCount = m_pGameEngine->GetGameTickCount();
			if ( ulCurrentTickCount - 140 > g_ulLastKeyUpTick )
			{
				g_ulLastKeyUpTick = ulCurrentTickCount;
				if ( m_uSelectedItem > 0 )
					m_uSelectedItem--;
				else
					m_uSelectedItem = (uint32)m_VecMenuItems.size() - 1;
			}
		}

		Render();
	}

	// Render the menu
	virtual void Render()
	{
		const int32 iMaxMenuItems = 14;
		int32 iNumItems = (int32)m_VecMenuItems.size();
		uint32 uBoxHeight = MIN( iNumItems, iMaxMenuItems ) * ( MENU_FONT_HEIGHT + MENU_ITEM_PADDING );
		uint32 yPos = m_pGameEngine->GetViewportHeight()/2 - uBoxHeight/2;

		RECT rect;
		rect.top = yPos;
		rect.bottom = yPos + MENU_FONT_HEIGHT + MENU_ITEM_PADDING;
		rect.left = 0;
		rect.right = m_pGameEngine->GetViewportWidth();
		char rgchBuffer[256];

		if ( m_sHeading.length() )
		{
			DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 128, 128 );
			RECT rectHeader;
			rectHeader.top = 10;
			rectHeader.bottom = rectHeader.top + MENU_FONT_HEIGHT + ( MENU_ITEM_PADDING * 2 );
			rectHeader.left = 0;
			rectHeader.right = m_pGameEngine->GetViewportWidth();
			m_pGameEngine->BDrawString( g_hMenuFont, rectHeader, dwColor, TEXTPOS_CENTER|TEXTPOS_VCENTER, m_sHeading.c_str() );
		}

		int32 iStartItem = 0;
		int32 iEndItem = iNumItems;
		if ( iNumItems > iMaxMenuItems )
		{
			iStartItem = MAX( (int32)m_uSelectedItem - iMaxMenuItems/2, 0 );
			iEndItem = MIN( iStartItem + iMaxMenuItems, iNumItems );
		}

		if ( iStartItem > 0 )
		{
			// Draw ... Scroll Up ... 
			DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			m_pGameEngine->BDrawString( g_hMenuFont, rect, dwColor, TEXTPOS_CENTER|TEXTPOS_VCENTER, "... Scroll Up ..." );

			rect.top = rect.bottom;
			rect.bottom += MENU_FONT_HEIGHT + MENU_ITEM_PADDING;
		}

		for( int32 i=iStartItem; i<iEndItem; ++i )
		{
			// Empty strings can be used to space menus, they don't get drawn or selected
			if ( strlen( m_VecMenuItems[i].first.c_str() ) > 0 )
			{
				DWORD dwColor;
				if ( i == m_uSelectedItem )
				{
					dwColor = D3DCOLOR_ARGB( 255, 25, 200, 25 );
					sprintf_safe( rgchBuffer, "{ %s }", m_VecMenuItems[i].first.c_str() );
				}
				else
				{
					dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
					sprintf_safe( rgchBuffer, "%s", m_VecMenuItems[i].first.c_str() );
				}
				m_pGameEngine->BDrawString( g_hMenuFont, rect, dwColor, TEXTPOS_CENTER|TEXTPOS_VCENTER, rgchBuffer );
			}

			rect.top = rect.bottom;
			rect.bottom += MENU_FONT_HEIGHT + MENU_ITEM_PADDING;
		}

		if ( iNumItems > iEndItem )
		{
			// Draw ... Scroll Down ... 
			DWORD dwColor = D3DCOLOR_ARGB( 255, 255, 255, 255 );
			m_pGameEngine->BDrawString( g_hMenuFont, rect, dwColor, TEXTPOS_CENTER|TEXTPOS_VCENTER, "... Scroll Down ..." );

			rect.top = rect.bottom;
			rect.bottom += MENU_FONT_HEIGHT + MENU_ITEM_PADDING;
		}
	}

private:
	// Game engine instance
	IGameEngine *m_pGameEngine;

	// Heading
	std::string m_sHeading;

	// Vector of menu options
	std::vector< MenuItem_t > m_VecMenuItems;

	// Currently selected item index
	uint32 m_uSelectedItem;

	// pushed selection
	bool m_bSelectionPushed;
	T m_selection;
};

#endif // MAINMENU_H
