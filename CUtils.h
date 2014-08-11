/*
	Greenworks Node-Webkit Library for Windows, Linux and Macintosh
	------------------------------------------------------------------------------------------------
	CUtils
	Provides internal utilities

	Written and developed by Francesco Abbattista
	Additional code and adaptions by Daniel Klug

	Copyright(C) 2014 Greenheart Games(http://greenheartgames.com )

	Greenworks is published under the MIT license.
	See LICENSE file for details.

	Also consult the licenses folder for additional libraries.
*/

#ifndef UTILS_H
#define UTILS_H

#include "Includes.h"
#include <node.h>
#include <v8.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sstream>
#include "steamworks-sdk/public/steam/steam_api.h"
#include "steamworks-sdk/public/steam/steam_gameserver.h"
#include "steamworks-sdk/public/steam/isteamremotestorage.h"

/* check_access.c */
#ifdef WIN32
	#include "misc/dirent.h"
	#include <io.h>
	#include <chrono>
	#include <thread>
	#define EXIST 00
	#define EXEC  01
	#define WRITE 02
	#define READ  04
#else
	#include <dirent.h>
	#include <unistd.h>
	#define EXIST F_OK
	#define EXEC  X_OK
	#define WRITE W_OK
	#define READ  R_OK
#endif

using namespace std;
using namespace v8;

class CUtils 
{
private:
	static void EscapeSingleChar(unsigned char u, char *buffer, size_t buflen);

public:
	static void WriteToJsConsole(string msg);
	static void WriteToJsConsoleAndLogFile(string msg);
	static void WriteToLogFile(string msg);
	static bool WriteToJsEnabled;
	static bool WriteToLogEnabled;
	static string WriteToLogFileLocation;
	static FILE FileOpen(const char* fileName);
	static long FileSize(FILE *file);
	static BYTE* FileBuffer(FILE *file);
	static void FileClose(FILE *file);
	static bool IsFileNewer(const char* file1, const char* file2);
	static bool FileExists(const char* file);
	static int GetFileTime(const char* file);
	static bool DirectoryExists(const char* dir);
	static vector<string> GetDirectoryList(const string& path);

	static string Replace(string &s, string toReplace, string replaceWith);
	static string Escape(const string& s);
	static int ToInt(const char *str);
	static long ToLong(const char *str);

	static uint64 ToUInt64(const char *str);
	static string ToString(const char *str);
	static string GetFilename(string path);
	static string PathCombine(string path1, string path2);
	static void Sleep(int ms);
};

#endif