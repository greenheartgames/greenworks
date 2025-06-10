//========= Copyright © Valve LLC, All rights reserved. ============
//
// Purpose: Class for interacting with the Item Store
//
//=============================================================================

#include "stdafx.h"
#include "ItemStore.h"
#include "BaseMenu.h"
#include <math.h>
#include <vector>
#include <algorithm>


//-----------------------------------------------------------------------------
// Purpose: Menu that shows purchaseable items
//-----------------------------------------------------------------------------
class CItemStoreMenu : public CBaseMenu<PurchaseableItem_t>
{
public:

	//-----------------------------------------------------------------------------
	// Purpose: Constructor
	//-----------------------------------------------------------------------------
	CItemStoreMenu( IGameEngine *pGameEngine, CItemStore *pItemStore ) : CBaseMenu<PurchaseableItem_t>( pGameEngine ), m_pItemStore( pItemStore )
	{
		
	}

	//-----------------------------------------------------------------------------
	// Purpose: Creates menu
	//-----------------------------------------------------------------------------
	void Rebuild()
	{
		PushSelectedItem();
		ClearMenuItems();

		const std::vector<PurchaseableItem_t> &vecPurchaseableItems = m_pItemStore->GetPurchaseableItems();
		for ( uint32 i = 0; i < vecPurchaseableItems.size(); ++i )
		{
			const PurchaseableItem_t &t = vecPurchaseableItems[i];
			AddItemToMenu( t );
		}

		PurchaseableItem_t menuItemBack = { 0, 0 };
		AddMenuItem( CItemStoreMenu::MenuItem_t( "Return to main menu", menuItemBack ) );
		
		PopSelectedItem();
	}

private:

	void AddItemToMenu( const PurchaseableItem_t &item )
	{
		char bufName[512];
		uint32 bufNameSize = sizeof( bufName );
		if ( !SteamInventory()->GetItemDefinitionProperty( item.m_nItemDefID, "name", bufName, &bufNameSize ) && bufNameSize <= sizeof( bufName ) )
		{
			return;
		}
		uint32 unQuantity = SpaceWarLocalInventory()->GetNumOf( item.m_nItemDefID );

		char rgchBuffer[1024];
		sprintf_safe( rgchBuffer, "%u. Purchase %-25s    %s %0.2f    (own %u)", item.m_nItemDefID, bufName, m_pItemStore->GetCurrency(), float( item.m_ulPrice ) / 100, unQuantity );

		AddMenuItem( CItemStoreMenu::MenuItem_t( rgchBuffer, item ) );
	}

	CItemStore *m_pItemStore;
};


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CItemStore::CItemStore( IGameEngine *pGameEngine ) : m_pGameEngine( pGameEngine )
{
	m_pItemStoreMenu = new CItemStoreMenu( pGameEngine, this );

	Show();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CItemStore::RunFrame()
{
	m_pItemStoreMenu->RunFrame();	
}


//-----------------------------------------------------------------------------
// Purpose: Handles menu actions when viewing purchaseable items
//-----------------------------------------------------------------------------
void CItemStore::OnMenuSelection( PurchaseableItem_t selection )
{
	if ( selection.m_nItemDefID == 0 )
	{
		SpaceWarClient()->SetGameState( k_EClientGameMenu );
		return;
	}

	uint32 rgQuantity[1] = {1};
	SteamInventory()->StartPurchase( &selection.m_nItemDefID, rgQuantity, 1 );
}


//-----------------------------------------------------------------------------
// Purpose: load all all purchaseable items
//-----------------------------------------------------------------------------
void CItemStore::LoadItemsWithPrices()
{
	m_vecPurchaseableItems.clear();

	SteamAPICall_t hSteamAPICall = SteamInventory()->RequestPrices();
	m_SteamCallResultRequestPrices.Set( hSteamAPICall, this, &CItemStore::OnRequestPricesResult );
}


//-----------------------------------------------------------------------------
// Purpose: Request prices from the Steam Inventory Service
//-----------------------------------------------------------------------------
void CItemStore::OnRequestPricesResult( SteamInventoryRequestPricesResult_t *pParam, bool bIOFailure )
{
	if ( pParam->m_result == k_EResultOK )
	{
		strncpy( m_rgchCurrency, pParam->m_rgchCurrency, sizeof( m_rgchCurrency ) );

		uint32 unItems = SteamInventory()->GetNumItemsWithPrices();
		std::vector<SteamItemDef_t> vecItemDefs;
		vecItemDefs.resize( unItems );
		std::vector<uint64> vecPrices;
		vecPrices.resize( unItems );

		if ( SteamInventory()->GetItemsWithPrices( vecItemDefs.data(), vecPrices.data(), NULL, unItems ) )
		{
			m_vecPurchaseableItems.reserve( unItems );
			for ( uint32 i = 0; i < unItems; ++i )
			{
				PurchaseableItem_t t;
				t.m_nItemDefID = vecItemDefs[i];
				t.m_ulPrice = vecPrices[i];
				m_vecPurchaseableItems.push_back( t );
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Shows / Refreshes the friends list
//-----------------------------------------------------------------------------
void CItemStore::Show()
{
	m_pItemStoreMenu->Rebuild();
}

