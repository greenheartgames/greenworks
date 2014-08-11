#include "CSteamWorkshopItem.h"


CSteamWorkshopItem::CSteamWorkshopItem() : 
HasChanged(false),
IsNew(false),
TargetFile(""),
Size(0),
Modified(0),
Result(k_EResultBusy)
{
	Details = new SteamUGCDetails_t();
}

CSteamWorkshopItem::~CSteamWorkshopItem() {};
