/*
	Greenworks Node-Webkit Library for Windows, Linux and Macintosh
	------------------------------------------------------------------------------------------------
	Includes
	Provides basic / common includes (especially for implementing platform specific requirements)

	Written and developed by Francesco Abbattista
	Additional code and adaptions by Daniel Klug

	Copyright(C) 2014 Greenheart Games(http://greenheartgames.com )

	Greenworks is published under the MIT license.
	See LICENSE file for details.

	Also consult the licenses folder for additional libraries.
*/

#ifndef INCLUDES_H
#define INCLUDES_H

#ifdef _WIN32
#else
	#include <sstream>

	typedef unsigned char       BYTE;
	typedef unsigned char		byte;

	// Provide an alternative to to_string on *nix / apple systems
	template <typename T>
	std::string to_string(T value)
	{
		std::ostringstream os;
		os << value;
		return os.str();
	}
#endif
#endif
