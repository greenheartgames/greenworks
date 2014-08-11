/*
	Greenworks Node-Webkit Library for Windows, Linux and Macintosh
	------------------------------------------------------------------------------------------------
	CSteamCallable
	Base class that represents a callable Steam class.

	Written and developed by Francesco Abbattista
	Additional code and adaptions by Daniel Klug

	Copyright(C) 2014 Greenheart Games(http://greenheartgames.com )

	Greenworks is published under the MIT license.
	See LICENSE file for details.

	Also consult the licenses folder for additional libraries.
*/

#ifndef STEAM_CALLABLE_H
#define STEAM_CALLABLE_H

#include "Includes.h"

class CSteamCallable
{
	protected:
		CSteamCallable();
		// ~CSteamCallable();

		bool Requesting;

	public:

		bool IsRequesting();
};
#endif 