/*
	Greenworks Node-Webkit Library for Windows, Linux and Macintosh
	------------------------------------------------------------------------------------------------
	CSteamWorkshopItem
	Represents a Workshop item and contains all necessary information for operating on Workshop items

	Written and developed by Francesco Abbattista
	Additional code and adaptions by Daniel Klug

	Copyright(C) 2014 Greenheart Games(http://greenheartgames.com )

	Greenworks is published under the MIT license.
	See LICENSE file for details.

	Also consult the licenses folder for additional libraries.
*/

#ifndef STEAM_WORKSHOP_ITEM_H
#define STEAM_WORKSHOP_ITEM_H

#include "Includes.h"
#include "steamworks-sdk/public/steam/steam_api.h"
#include "steamworks-sdk/public/steam/steam_gameserver.h"
#include "steamworks-sdk/public/steam/isteamremotestorage.h"
#include <string>

using namespace std;

class CSteamWorkshopItem
{
private:

public:
	CSteamWorkshopItem();
	~CSteamWorkshopItem();

	SteamUGCDetails_t *Details;
	bool HasChanged;
	bool IsNew;
	string TargetFile;
	int Size;
	int Modified;
	EResult Result;
};

#endif