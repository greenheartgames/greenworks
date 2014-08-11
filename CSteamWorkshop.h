/*
	Greenworks Node-Webkit Library for Windows, Linux and Macintosh
	------------------------------------------------------------------------------------------------
	CSteamWorkshop
	Class providing implementation of Steam operations such as synchronize (download), publish 
	(upload, share), update (upload, share, update) and other Steam specific operations.

	This class is at the heart of interaction with Steam and takes care of the related communication.

	Written and developed by Francesco Abbattista
	Additional code and adaptions by Daniel Klug

	Copyright(C) 2014 Greenheart Games(http://greenheartgames.com )

	Greenworks is published under the MIT license.
	See LICENSE file for details.

	Also consult the licenses folder for additional libraries.
*/

#ifndef STEAM_WORKSHOP_H
#define STEAM_WORKSHOP_H

#include "Includes.h"
#include <v8.h>
#include <node.h>
#include <iostream>
#include "steamworks-sdk/public/steam/steam_api.h"
#include "steamworks-sdk/public/steam/steam_gameserver.h"
#include "steamworks-sdk/public/steam/isteamremotestorage.h"
#include "CUtils.h"
#include "CSteamCallable.h"
#include "CSteamWorkshopItem.h"

// #ifdef _WIN32 
// #else
// 	#include <utime.h>
// #endif

// crt_utime.c
#ifdef _WIN32 
	#include <sys/utime.h>
#else
	#include <utime.h>
#endif

#include <sys/types.h>

#include <time.h>

using namespace std;


class CSteamWorkshop : public CSteamCallable 
{
private:
	int m_iAppID; // Our current AppID
	UGCHandle_t m_Handle;
	bool m_bInitialized; // Have we called Request stats and received the callback?
	PublishedFileId_t m_publishedFileId;
	string lastName;

	string m_fileName;
	string m_title;
	string m_description;
	string m_targetDirForDownload;
	string m_imageFileName;
	int m_iNumWorkshopItemsProcessed;

	CCallResult<CSteamWorkshop, SteamUGCQueryCompleted_t> m_CallResultOnUGCQueryCompleted;
	CCallResult<CSteamWorkshop, SteamUGCQueryCompleted_t> m_CallResultOnUGCUserQueryCompleted;

	CCallResult<CSteamWorkshop, SteamUGCQueryCompleted_t> m_CallResultOnUGCQueryAndSyncCompleted;

	CCallResult<CSteamWorkshop, RemoteStorageFileShareResult_t> m_CallResultOnFileShared;
	CCallResult<CSteamWorkshop, RemoteStoragePublishFileResult_t> m_CallResultOnFilePublished;

	CCallResult<CSteamWorkshop, RemoteStorageDownloadUGCResult_t> m_CallResultOnFileDownloaded;
	CCallResult<CSteamWorkshop, RemoteStorageDownloadUGCResult_t> m_CallResultOnFileFromUGCSyncDownloaded;

	CCallResult<CSteamWorkshop, RemoteStorageFileShareResult_t> m_CallResultOnFileShareUpdateCompleted;

	CCallResult<CSteamWorkshop, RemoteStorageGetPublishedFileDetailsResult_t> m_CallResultOnGetPublishedFileDetailsCompleted;
	CCallResult<CSteamWorkshop, RemoteStorageUpdatePublishedFileResult_t> m_CallResultOnCommitPublishedFileUpdateCompleted;
	
	CCallResult<CSteamWorkshop, RemoteStoragePublishedFileUnsubscribed_t> m_CallResultOnUnsubscribeCompleted;
	

	CSteamWorkshopItem GetItemByFileName(string fileName);
	int GetItemIndexByFileName(string fileName);

	void DownloadFromUGCSynchronize(UGCHandle_t hFile);

public:
	CSteamWorkshop();
	~CSteamWorkshop();

	vector<CSteamWorkshopItem> m_wsItems;
	int m_iNumWorkshopItems; // The number of items
	bool HasChanged;		 // If an item has changed compared to a previous operation (i.e. on download)
	bool Response;			// A general response (True / False) for async queries

	Persistent<Function> ProgressDelegate;
	Persistent<Function> SuccessDelegate;
	Persistent<Function> ErrorDelegate;

	PublishedFileId_t GetLastPublishedFileId();

	bool RequestUGC(int type, int sort);
	bool RequestUserUGC(int type, int sort, int filter);
	bool RequestAndSynchronizeUserUGC(string targetDir);
	void Download(UGCHandle_t hFile, string targetDir);

	void Publish(string fileName, string title, string description, void *content, int contentLength, string imageFileName, void *contentImage, int contentImageLength);
	void PublishUpdate(string fileName, string title, string description, void *content, int contentLength, string imageFileName, void *contentImage, int contentImageLength, uint64 publishedFileId);
	void Unsubscribe(uint64 publishedFileId);

	void OnUGCQueryCompleted(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure);
	void OnUGCUserQueryCompleted(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure);
	void OnUGCQueryAndSyncCompleted(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure);

	void OnFileShareCompleted(RemoteStorageFileShareResult_t *pCallback, bool bIOFailure);
	void OnFileShareUpdateCompleted(RemoteStorageFileShareResult_t *pCallback, bool bIOFailure);
	void OnFilePublishCompleted(RemoteStoragePublishFileResult_t *pCallback, bool bIOFailure);
	void OnGetPublishedFileDetailsCompleted(RemoteStorageGetPublishedFileDetailsResult_t *pCallback, bool bIOFailure);
	void OnCommitPublishedFileUpdateCompleted(RemoteStorageUpdatePublishedFileResult_t *pCallback, bool bIOFailure);

	void OnDownloadCompleted(RemoteStorageDownloadUGCResult_t *pCallback, bool bIOFailure);
	void OnDownloadFromUGCSynchronizeCompleted(RemoteStorageDownloadUGCResult_t *pCallback, bool bIOFailure);

	void OnUnsubscribeCompleted(RemoteStoragePublishedFileUnsubscribed_t *pCallback, bool bIOFailure);
};
#endif 
