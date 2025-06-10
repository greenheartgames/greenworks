//========= Copyright © Valve LLC, All rights reserved. ============
//
// Purpose: Class for interacting with in-game store
//
//=============================================================================

#ifndef ITEMSTORE_H
#define ITEMSTORE_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include "SpaceWarClient.h"


class CSpaceWarClient;
class CItemStoreMenu;

class CItemStore
{
public:
	// Constructor
	CItemStore( IGameEngine *pGameEngine );

	// Run a frame
	void RunFrame();

	// shows / refreshes item store
	void Show();

	// handles input from item store menu
	void OnMenuSelection( PurchaseableItem_t selection );

	// ask the inventory service for things to purchase
	void LoadItemsWithPrices();

	const std::vector<PurchaseableItem_t> &GetPurchaseableItems() const { return m_vecPurchaseableItems; }

	const char *GetCurrency() const { return m_rgchCurrency; }
	
private:	
	// callback when we ask the Inventory Service for prices
	void OnRequestPricesResult( SteamInventoryRequestPricesResult_t *pParam, bool bIOFailure );
	CCallResult<CItemStore, SteamInventoryRequestPricesResult_t> m_SteamCallResultRequestPrices;
	char m_rgchCurrency[4];
	std::vector<PurchaseableItem_t> m_vecPurchaseableItems;
	
	// Engine
	IGameEngine *m_pGameEngine;
	
	CItemStoreMenu *m_pItemStoreMenu;
};

#endif // ITEMSTORE_H
