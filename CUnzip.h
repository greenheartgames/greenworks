/*
	ported from:

	miniunz.c
	Version 1.1, February 14h, 2010
	sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

	Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

	Modifications of Unzip for Zip64
	Copyright (C) 2007-2008 Even Rouault

	Modifications for Zip64 support on both zip and unzip
	Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

	Porting for Greenworks integration of both zip and unzip by Francesco Abbattista
	Copyright (C) 2014 Greenheart Games ( http://greenheartgames.com )
*/

#ifndef UNZIP_H
#define UNZIP_H

#ifndef _WIN32
#ifndef __USE_FILE_OFFSET64
#define __USE_FILE_OFFSET64
#endif
#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _FILE_OFFSET_BIT
#define _FILE_OFFSET_BIT 64
#endif

#define off64_t off_t
#define fopen64 fopen
#define fseeko64 fseeko
#define ftello64 ftello

#endif

#include "Includes.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#ifndef USE_FILE32API
#define USE_FILE32API
#endif


#ifdef _WIN32
	#include <direct.h>
	#include <io.h>
#else
	#include <unistd.h>
	#include <utime.h>
	#include <sys/stat.h> 
#endif

#include "minizip/unzip.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "minizip/iowin32.h"
#endif

using namespace std;

class CUnzip 
{
	private:
		void change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date);
		int mymkdir(const char* dirname);
		int makedir(const char *newdir);
		void Display64BitsSize(ZPOS64_T n, int size_char);
		int do_list(unzFile uf);
		int do_extract_currentfile(unzFile uf, const int* popt_extract_without_path, int* popt_overwrite, const char* password);
		int do_extract(unzFile uf, int opt_extract_without_path, int opt_overwrite, const char* password);
		int do_extract_onefile(unzFile uf, const char* filename, int opt_extract_without_path, int opt_overwrite, const char* password);

	public:
		int unzip(const char *zipfilename, const char *dirname, const char *password);
};
#endif 