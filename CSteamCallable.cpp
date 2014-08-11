#include "CSteamCallable.h"

CSteamCallable::CSteamCallable(){
	Requesting = false;
}

bool CSteamCallable::IsRequesting(){
	return Requesting;
}