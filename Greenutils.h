/*
	Greenworks Node-Webkit Library for Windows, Linux and Macintosh
	------------------------------------------------------------------------------------------------
	A node.js plugin to integrate with Steamworks.

	The plugin was developed to enable the Steam release of Greenheart Games' Game Dev Tycoon, a game powered by node-webkit.
	It has since been used by other projects.

	Written and developed by Francesco Abbattista
	Additional code and adaptions by Daniel Klug

	Project initially started and developed by Evgheni C.

	Copyright(C) 2014 Greenheart Games(http://greenheartgames.com )

	Greenworks is published under the MIT license.
	See LICENSE file for details.

	Also consult the licenses folder for additional libraries.
*/

#ifndef GREENUTILS_H
#define GREENUTILS_H

#include "Includes.h"
#include <fstream>
#include <iostream>
#include "CUtils.h"
#include "CUnzip.h"
#include "CZip.h"

using namespace v8;
using namespace std;


class Greenutils : node::ObjectWrap {
private:

	Greenutils();
	~Greenutils();

	/* Node specific Constructor for allowing creating NEW instance of this class */
	static Persistent<Function> constructor;

	/* Used to mimic the New operator of this class for creating new instances */
	static Handle<Value> New(const Arguments& args);

	/* Worker Action for Archive Extraction */
	static void extractArchiveWorker(uv_work_t *req);

	/* Worker Complete Action for Archive Extraction */
	static void extractArchiveWorkerComplete(uv_work_t *req);

	/* Worker Action for Archive Creation */
	static void createArchiveWorker(uv_work_t *req);

	/* Worker Complete Action for Archive Creation */
	static void createArchiveWorkerComplete(uv_work_t *req);

public:

	/* Archive creation and extraction data struct */
	struct CreateExtractArchive {
		bool bSuccess;
		Persistent<Function> errorCallback;
		Persistent<Function> successCallback;
		string targetFolder;
		string targetFile;
		string password;
		string error;
		int compressionLevel;
	};

	/* Class specific init for exporting local members to JS context (usually called by main exporting class) */
	static void Init(Handle<Object> exports);

	/* Used to create New instances of this class */
	static Handle<Value> NewInstance();

	/* Exposed internals: Sleep - Wait for X-ms */
	static Handle<Value> sleep(const Arguments& args);

	/*
		Call from javascript:
		Greenutils.createArchive(string zipFile, string sourceDir, string password,  int compressionLevel, func success, func error)
	*/
	static Handle<Value> createArchive(const Arguments& args);

	/*
		Call from javascript:
		Greenutils.extractArchive(string zipFile, string targetDir, string password, func success, func error)
	*/
	static Handle<Value> extractArchive(const Arguments& args);

	/*
		Call from javascript:
		Greenutils.enableConsole()

		Enables output to JS Console
	*/
	static Handle<Value> enableConsole(const Arguments& args);

	/*
		Call from javascript:
		Greenutils.disableConsole()

		Disables output to JS Console
	*/
	static Handle<Value> disableConsole(const Arguments& args);

	/*
		Call from javascript:
		Greenutils.enableConsole(string targetFile) or Greenworks.Utils.enableWriteToLog(string targetFile)

		Enables output to the specified log file
	*/
	static Handle<Value> enableWriteToLog(const Arguments& args);

	/*
		Call from javascript:
		Greenutils.disableWriteToLog() or Greenworks.Utils.disableWriteToLog()

		Disables output to a log file
	*/
	static Handle<Value> disableWriteToLog(const Arguments& args);

	/*
		Call from javascript:
		Greenutils.getOS() or Greenworks.Utils.getOS()

		Returns the current OS (linux, apple, win)
	*/
	static Handle<Value> getOS(const Arguments& args);

	/*
		Call from javascript:
		Greenutils.move(sourceFolder, targetFolder)

		Moves the source folder to the target folder within the same device
	*/
	static Handle<Value> move(const Arguments& args);

};

#endif