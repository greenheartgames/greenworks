//========= Copyright © 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Class for tracking stats and achievements
//
// $NoKeywords: $
//=============================================================================

#ifndef INVENTORY_H
#define INVENTORY_H

#include "SpaceWar.h"
#include "GameEngine.h"
#include <list>
#include <string>

class CSpaceWarItem;

// These are hardcoded in the game and match the item definition IDs which were uploaded to Steam.
enum ESpaceWarItemDefIDs
{
	k_SpaceWarItem_TimedDropList = 10,
	k_SpaceWarItem_ShipDecoration1 = 100,
	k_SpaceWarItem_ShipDecoration2 = 101,
	k_SpaceWarItem_ShipDecoration3 = 102,
	k_SpaceWarItem_ShipDecoration4 = 103,
	k_SpaceWarItem_ShipWeapon1 = 110,
	k_SpaceWarItem_ShipWeapon2 = 111,
	k_SpaceWarItem_ShipSpecial1 = 120,
	k_SpaceWarItem_ShipSpecial2 = 121
};


class CSpaceWarLocalInventory
{
public:
	void RefreshFromServer();

	void GrantTestItems();
	void CheckForItemDrops();
	void DoExchange();
	void ModifyItemProperties();

	const std::list<CSpaceWarItem *>& GetItemList() const { return m_listPlayerItems; }
	const CSpaceWarItem * GetItem( SteamItemInstanceID_t nItemId ) const;
	const CSpaceWarItem *  GetInstanceOf( SteamItemDef_t nDefinition ) const;
	bool HasInstanceOf( SteamItemDef_t nDefinition ) const;
	uint32 GetNumOf( SteamItemDef_t nDefinition ) const;

	bool IsWaitingForDropResults() const { return m_hPlaytimeRequestResult != k_SteamInventoryResultInvalid; }
	const CSpaceWarItem * GetLastDroppedItem() const { return GetItem( m_LastDropInstanceID ); }

private:
	friend CSpaceWarLocalInventory *SpaceWarLocalInventory();
	CSpaceWarLocalInventory();
	STEAM_CALLBACK( CSpaceWarLocalInventory, OnSteamInventoryResult, SteamInventoryResultReady_t, m_SteamInventoryResult );
	STEAM_CALLBACK( CSpaceWarLocalInventory, OnSteamInventoryFullUpdate, SteamInventoryFullUpdate_t, m_SteamInventoryFullUpdate );

private:
	SteamInventoryResult_t m_hPlaytimeRequestResult;
	SteamInventoryResult_t m_hPromoRequestResult;
	SteamInventoryResult_t m_hLastFullUpdate;
	SteamInventoryResult_t m_hExchangeRequestResult;

	std::list<CSpaceWarItem *> m_listPlayerItems;
	SteamItemInstanceID_t m_LastDropInstanceID;
};

CSpaceWarLocalInventory *SpaceWarLocalInventory();


class CSpaceWarItem
{
public:
	SteamItemInstanceID_t GetItemId() const { return m_Details.m_itemId; }
	SteamItemDef_t GetDefinition() const { return m_Details.m_iDefinition; }
	uint16 GetQuantity() const { return m_Details.m_unQuantity; }
	std::string GetLocalizedName() const;
	std::string GetLocalizedDescription() const;
	std::string GetIconURL() const;
private:
	friend class CSpaceWarLocalInventory;
	SteamItemDetails_t m_Details;
};



#endif // INVENTORY_H