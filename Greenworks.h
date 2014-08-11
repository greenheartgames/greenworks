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

#ifndef GREENWORKS_H
#define GREENWORKS_H

#include "Includes.h"
#include <fstream>
#include <iostream>
#include <string>
#include "CSteamCallable.h"
#include "CSteamWorkshop.h"
#include "CSteamWorkshopItem.h"
#include "CUtils.h"
#include "CUnzip.h"
#include "CZip.h"
#include "Greenutils.h"
#include "v8.h"

using namespace v8;
using namespace std;

class Greenworks : node::ObjectWrap {
	private: 
		/* Greenutils Object Instance */
		static Persistent<Value> _Utils;

		/* Creates a V8 JS Object from the specified SteamUGCDetails_t structure */
		static Handle<Object> GetObjectFromWsItem(CSteamWorkshopItem item);

		/* Creates a V8 JS Object from the specified Steam Account Type enum value containing a name and a value property */
		static Handle<Value> GetObjectFromSteamAccountType(int eAccountType);
		
		/* Creates a V8 JS Object from the specified */
		static Handle<Object> GetProgressCallbackObject(string statusMsg, string reasonMsg, float value);
		
		/* Returns the percentage value used for progress callbacks */
		static float GetProgressPerc(int numOfWsItems, int wsItemIndex);

		/* Worker Action for Text File Writing */
		static void steamWriteToFile(uv_work_t *req);

		/* Worker Action for Text File Reading */
		static void steamReadFromFile(uv_work_t *req);

		/* Worker Action for Quota Detection */
		static void steamGetCloudQuota(uv_work_t *req);

		/* Worker Action for Achievement Storage */
		static void steamSetAchievement(uv_work_t *req);

		/* Worker Complete Action for Text File Writing */
		static void fileWrote(uv_work_t *req);

		/* Worker Complete Action for Text File Reading */
		static void fileLoaded(uv_work_t *req);

		/* Worker Complete Action for Quota Detection */
		static void gotQuota(uv_work_t *req);

		/* Worker Complete Action for Achievement Storage */
		static void achievementStored(uv_work_t *req);

		/* Worker Action for UGC Items Acquisition */
		static void ugcGetItemsWorker(uv_work_t *req);

		/* Worker Complete Action for UGC Items Acquisition */
		static void ugcGetItemsWorkerComplete(uv_work_t *req);

		/* Worker Action for UGC User Items Acquisition */
		static void ugcGetUserItemsWorker(uv_work_t *req);

		/* Worker Complete Action for UGC User Items Acquisition */
		static void ugcGetUserItemsWorkerComplete(uv_work_t *req);

		/* Worker Action for UGC Item Download */
		static void ugcDownloadItemWorker(uv_work_t *req);

		/* Worker Complete Action for UGC Item Download */
		static void ugcDownloadItemWorkerComplete(uv_work_t *req);

		/* Worker Action for for UGC Item Synchronization */
		static void ugcSynchronizeItemsWorker(uv_work_t *req);

		/* Worker Complete Action for for UGC Item Synchronization */
		static void ugcSynchronizeItemsWorkerComplete(uv_work_t *req);

		/* Worker Complete Action for UGC Item Download during Synchronization  */
		static void ugcSynchronizeItemsDownloadWorkerComplete(uv_work_t *req);

		/* Worker Action for for UGC Publish */
		static void ugcPublishWorker(uv_work_t *req);

		/* Worker Complete Action for UGC Publish  */
		static void ugcPublishWorkerComplete(uv_work_t *req);

		/* Worker Action for for UGC Update Publishing  */
		static void ugcPublishUpdateWorker(uv_work_t *req);

		/* Worker Complete Action for UGC Update Publishing  */
		static void ugcPublishUpdateWorkerComplete(uv_work_t *req);

		/* Worker Action for UGC Unsubscribe */
		static void ugcUnsubscribeWorker(uv_work_t *req);

		/* Worker Complete Action for UGC User Items Acquisition */
		static void ugcUnsubscribeWorkerComplete(uv_work_t *req);


	public:
		Greenworks(); 
		~Greenworks();

		struct FileIOAsync {
			Persistent<Function> errorCallback;
			Persistent<Function> successCallback;
			string sFilename;
			string sContent;
			string sError;
			bool bSuccess;
		};

		struct CloudQuota {
			Persistent<Function> errorCallback;
			Persistent<Function> successCallback;
			int nTotalBytes;
			int nAvailableBytes;
			bool bSuccess;
		};

		struct Achievement {
			Persistent<Function> errorCallback;
			Persistent<Function> successCallback;
			string sAchievementId;
			bool bSuccess;
		};

		struct Workshop {
			bool bRet;					// Generic return value
			bool bSuccess;				// Generic success flag for ops
			CSteamWorkshop*	steamWs;	// Pointer to the CSteamWorkshop class instance
			Persistent<Array> wsItems;	// UGC Requests / Synchronize: Items resulting from requests ops
			int fileIndex;				// UGC Requests: File index
			string targetFolder;		// UGC Synchronize: Target folder where to download UGC files
			string targetFile;			// Publish / Update: File to publish
			string title;				// Publish / Update: Title of file to publish
			string description;			// Publish / Update: Description of file to publish
			string targetImageFile;		// Publish / Update: Image file location (optional)
			BYTE *fileBuf;				// Publish / Update: Pointer to buffered data
			long fileSize;				// Publish / Update: Size of the file (buffer)
			BYTE *imageFileBuf;			// Publish / Update: Pointer to buffered image data
			long imageFileSize;			// Publish / Update: Size of the image file (buffer)
			bool bNeedsPublish;			// Publish / Update: Used to handle publish logic in worker thread
			bool bIsPublishing;			// Publish / Update: Used to handle publish logic in worker thread
			uint64 publishedFileId;		// Update: Used to handle publish logic in worker thread
			float progress;				// Progress callback: Percentage value
			string reason;				// Progress callback: Reason message
		};

		struct Unsubscribe {
			bool bRet;					// Generic return value
			bool bSuccess;				// Generic success flag for ops
			CSteamWorkshop*	steamWs;	// Pointer to the CSteamWorkshop class instance
			uint64 publishedFileId;		// Used to handle unpublish logic in worker thread
		};

		/*
			Call from javascript:
			Greenworks.initAPI()

			Returns true if Steam API was successfully initialized or false if not.
			You need to be connected and logged into Steam or using the offline mode for cache operations (where available).
		*/	
		static Handle<Value> initAPI(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.getCloudQuotas()

			Returns 2 integers (Steam Cloud Quota):
			quotaData->nTotalBytes
			quotaData->nAvailableBytes

			success		(int nTotalBytes, int nAvailableBytes)
			Callback if the method call has been successful

			error		Callback if the method call encountered an error

		*/
		static Handle<Value> getCloudQuota(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.saveTextToFile()

			fileName	string
			Name of the text file to write to the cloud.

			content		string
			Content to write into the text file.

			success		Callback if the method call has been successful

			error		Callback if the method call encountered an error

		*/
		static Handle<Value> saveTextToFile(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.readTextFromFile()

			fileName	string
			Name of the text file to read from the cloud.


			success		Callback if the method call has been successful

			error		Callback if the method call encountered an error

		*/
		static Handle<Value> readTextFromFile(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.activateAchievement()

			achievementId	string
			Id of the achievement to activate


			success			Callback if the method call has been successful

			error			Callback if the method call encountered an error

		*/
		static Handle<Value> activateAchievement(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.enableCloud()

			Enables / Disabled the Cloud feature for the current app.
		*/
		static Handle<Value> enableCloud(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.isCloudEnabled()

			Checks if Cloud is Enabled for the current app.
		*/
		static Handle<Value> isCloudEnabled(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.isCloudEnabledForUser()

			Checks if Cloud is Enabled for the current user account.
		*/
		static Handle<Value> isCloudEnabledForUser(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.getNumberOfPlayers()

			Returns the current number of players on Steam.
		*/
		static Handle<Value> getNumberOfPlayers(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.ugcPublish(string fileName, string title, string description, string imageFile, func successCallback, func errorCallback, func progressCallback)

			fileName	string
			Name of the file to publish (preview image)

			title	string
			Title of the workshop item

			description	string
			Description of the workshop item

			imageFile	string
			Name of the image file to publish (preview image)

			success		Callback if the method call has been successful

			error		Callback if the method call encountered an error

			progress	obj{status: string, reason: string, value: int}
			Callback on a download progress

		*/
		static Handle<Value> ugcPublish(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.ugcPublishUpdate(int publishedFileId, string fileName, string title, string description, string imageFile, func successCallback, func errorCallback, func progressCallback)

			publishedFileId	int
			Id of the published workshop item you wish to update (as obtained through ugcGetItems or ugcGetUserItems)

			fileName		string
			Name of the file to publish (preview image)

			title			string
			Title of the workshop item

			description		string
			Description of the workshop item

			imageFile		string
			Name of the image file to publish (preview image)

			success			Callback if the method call has been successful

			error			Callback if the method call encountered an error

			progress		obj{status: string, reason: string, value: int}
			Callback on a download progress


		*/
		static Handle<Value> ugcPublishUpdate(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.ugcGetItems(int type, int sort, func successCallback, func errorCallback, func progressCallback);

			type		int
			Type corresponds to k_EUGCMatchingUGCType
			EUGCMatchingUGCType_Items					= 0,		// both mtx items and ready-to-use items
			EUGCMatchingUGCType_ItemsMtx				= 1,
			EUGCMatchingUGCType_ItemsReadyToUse			= 2,
			EUGCMatchingUGCType_Collections				= 3,
			EUGCMatchingUGCType_Artwork					= 4,
			EUGCMatchingUGCType_Videos					= 5,
			EUGCMatchingUGCType_Screenshots				= 6,
			EUGCMatchingUGCType_AllGuides				= 7,		// both web guides and integrated guides
			EUGCMatchingUGCType_WebGuides				= 8,
			EUGCMatchingUGCType_IntegratedGuides		= 9,
			EUGCMatchingUGCType_UsableInGame			= 10,		// ready-to-use items and integrated guides
			EUGCMatchingUGCType_ControllerBindings		= 11,

			sort		int
			Sort corresponds to k_EUGCQuery
			EUGCQuery_RankedByVote									= 0,
			EUGCQuery_RankedByPublicationDate						= 1,
			EUGCQuery_AcceptedForGameRankedByAcceptanceDate			= 2,
			EUGCQuery_RankedByTrend									= 3,
			EUGCQuery_FavoritedByFriendsRankedByPublicationDate		= 4,
			EUGCQuery_CreatedByFriendsRankedByPublicationDate		= 5,
			EUGCQuery_RankedByNumTimesReported						= 6,
			EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate = 7,
			EUGCQuery_NotYetRated									= 8,
			EUGCQuery_RankedByTotalVotesAsc							= 9,
			EUGCQuery_RankedByVotesUp								= 10,
			EUGCQuery_RankedByTextSearch							= 11,

			success		Callback if the method call has been successful

			error		Callback if the method call encountered an error

			progress	obj{status: string, reason: string, value: int}
			Callback on a download progress


		*/
		static Handle<Value> ugcGetItems(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.ugcGetUserItems(int type, int sort, int filter, func successCallback, func errorCallback, func progressCallback);

			type		int
			Type corresponds to EUGCMatchingUGCType
			EUGCMatchingUGCType::Items					= 0,		// both mtx items and ready-to-use items
			EUGCMatchingUGCType::ItemsMtx				= 1,
			EUGCMatchingUGCType::ItemsReadyToUse		= 2,
			EUGCMatchingUGCType::Collections			= 3,
			EUGCMatchingUGCType::Artwork				= 4,
			EUGCMatchingUGCType::Videos					= 5,
			EUGCMatchingUGCType::Screenshots			= 6,
			EUGCMatchingUGCType::AllGuides				= 7,		// both web guides and integrated guides
			EUGCMatchingUGCType::WebGuides				= 8,
			EUGCMatchingUGCType::IntegratedGuides		= 9,
			EUGCMatchingUGCType::UsableInGame			= 10,		// ready-to-use items and integrated guides
			EUGCMatchingUGCType::ControllerBindings		= 11,

			sort		int
			Sort corresponds to EUserUGCListSortOrder
			EUserUGCListSortOrder::CreationOrderDesc	= 0,
			EUserUGCListSortOrder::CreationOrderAsc		= 1,
			EUserUGCListSortOrder::TitleAsc				= 2,
			EUserUGCListSortOrder::LastUpdatedDesc		= 3,
			EUserUGCListSortOrder::SubscriptionDateDesc	= 4,
			EUserUGCListSortOrder::VoteScoreDesc		= 5,
			EUserUGCListSortOrder::ForModeration		= 6,

			filter		int
			Filter corresponds to EUserUGCList
			EUserUGCList::Published						= 0,
			EUserUGCList::VotedOn						= 1,
			EUserUGCList::VotedUp						= 2,
			EUserUGCList::VotedDown						= 3,
			EUserUGCList::WillVoteLater					= 4,
			EUserUGCList::Favorited						= 5,
			EUserUGCList::Subscribed					= 6,
			EUserUGCList::UsedOrPlayed					= 7,
			EUserUGCList::Followed						= 8,

			success		Callback if the method call has been successful

			error		Callback if the method call encountered an error

			progress	obj{status: string, reason: string, value: int}
			Callback on a download progress

		*/
		static Handle<Value> ugcGetUserItems(const Arguments& args);
		
		/*
			Call from javascript:
			Greenworks.ugcDownloadItem(string fileName, int hFile, string targetFolder, func successCallback, func errorCallback, func progressCallback)

			fileName	string
			Name of the file to download (as obtained through ugcGetItems or ugcGetUserItems)

			hFile		int
			Handle to the file you wish to download (as obtained through ugcGetItems or ugcGetUserItems)

			success		Callback if the method call has been successful

			error		Callback if the method call encountered an error

			progress	obj{status: string, reason: string, value: int}
			Callback on a download progress
		*/
		static Handle<Value> ugcDownloadItem(const Arguments& args);

		/*
			Call from javascript:

			Greenworks.ugcSynchronizeItems(string targetFolder, func success, func error, func progress)

			targetFolder	string
			Destination folder to download and sync WS items (usually a custom cache for better control)

			success			Result{items: array_UGC_DETAILS, count: array length}
			Callback if the method call has been successful

			error			Callback if the method call encountered an error

			progress		obj{status: string, reason: string, value: int}
			Callback on a download progress
		*/
		static Handle<Value> ugcSynchronizeItems(const Arguments& args);
		
		/*
			Call from javascript:
			Greenworks.getCurrentGameLanguage()

			Returns the current language from Steam specifically set for the Game
		*/
		static Handle<Value> getCurrentGameLanguage(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.getCurrentUILanguage()

			Returns the current language from Steam set in the UI
		*/
		static Handle<Value> getCurrentUILanguage(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.ugcShowOverlay()
			Greenworks.ugcShowOverlay(itemId)

			Shows the Steam overlay pointed to the app's workshop page
			Shows the Steam overlay pointed to the workshop page of item with the specified id
		*/
		static Handle<Value> ugcShowOverlay(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.ugcUnsubscribe(int publishedFileId, func success, func error, func progress)
		*/
		static Handle<Value> ugcUnsubscribe(const Arguments& args);

		/*
			Call from javascript:
			Greenworks.getSteamId()

			Returns extensive information from the Steam ID object of the current user.
			object: flags (object)	boolean flags describing type of user information
			------------------------------------------------------------------------------------------------------
			anonymous					Is this an anonymous account?
			anonymousGameServer			Is this an anonymous game server account?
			anonymousGameServerLogin	Is this an anonymous game server account login request?
			anonymousUser				Is this an anonymouse user account?
			chat						Is this a chat account?
			clan						Is this a clan account?
			consoleUser					Is this a console user (PSN) account?
			contentServer				Is this a content server account?
			gameServer					Is this a game server account?
			individual					Is this an individual account?
			gameServerPersistent		Is this a persistent game server account?
			lobby						Is this a lobby (chat) account?

			type (object)				object describing type of user account
			------------------------------------------------------------------------------------------------------
			name						Name of the resulting enum value (i.e. k_EAccountTypeAnonGameServer)
			value						Value of the resulting enum value (i.e. 0)

			accountId (int)				Account ID (Steam ID)
			------------------------------------------------------------------------------------------------------

			staticAccountId (int)		Static int64 representation of a Steam ID
			------------------------------------------------------------------------------------------------------

			screenName (string)			Screen Name associated with this Steam user ID
			------------------------------------------------------------------------------------------------------

			level (int)					Steam Level associated with this Steam user ID
			------------------------------------------------------------------------------------------------------

			isValid (boolean)			Is it is a valid account
			------------------------------------------------------------------------------------------------------

		*/
		static Handle<Value> getSteamId(const Arguments& args);

		/* Work in progress * do not use */
		static Handle<Value> getCurrentGameInstallDir(const Arguments& args);

		/* Exposed internals: Forces Steam to run its callbacks */
		static Handle<Value> runCallbacks(const Arguments& args);

		// this.title
		static Handle<Value> GetUtils(Local<String> property, const AccessorInfo& info);
};

void init(Handle<Object> exports) {
	/* Common (Global) Exports */
	exports->Set(String::NewSymbol("initAPI"), FunctionTemplate::New(Greenworks::initAPI)->GetFunction());
	exports->Set(String::NewSymbol("getSteamId"), FunctionTemplate::New(Greenworks::getSteamId)->GetFunction());

	/* Achievement Related Exports */
	exports->Set(String::NewSymbol("activateAchievement"), FunctionTemplate::New(Greenworks::activateAchievement)->GetFunction());

	/* Text File Related Exports */
	exports->Set(String::NewSymbol("saveTextToFile"), FunctionTemplate::New(Greenworks::saveTextToFile)->GetFunction());
	exports->Set(String::NewSymbol("readTextFromFile"), FunctionTemplate::New(Greenworks::readTextFromFile)->GetFunction());

	/* Cloud Related Exports */
	exports->Set(String::NewSymbol("isCloudEnabled"), FunctionTemplate::New(Greenworks::isCloudEnabled)->GetFunction());
	exports->Set(String::NewSymbol("isCloudEnabledForUser"), FunctionTemplate::New(Greenworks::isCloudEnabledForUser)->GetFunction());
	exports->Set(String::NewSymbol("enableCloud"), FunctionTemplate::New(Greenworks::enableCloud)->GetFunction());
	exports->Set(String::NewSymbol("getCloudQuota"), FunctionTemplate::New(Greenworks::getCloudQuota)->GetFunction());

	/* Player Related Exports */
	exports->Set(String::NewSymbol("getNumberOfPlayers"), FunctionTemplate::New(Greenworks::getNumberOfPlayers)->GetFunction());

	/* Language Related Exports */
	exports->Set(String::NewSymbol("getCurrentGameLanguage"), FunctionTemplate::New(Greenworks::getCurrentGameLanguage)->GetFunction());
	exports->Set(String::NewSymbol("getCurrentUILanguage"), FunctionTemplate::New(Greenworks::getCurrentUILanguage)->GetFunction());
	exports->Set(String::NewSymbol("getCurrentGameInstallDir"), FunctionTemplate::New(Greenworks::getCurrentGameInstallDir)->GetFunction());

	/* UGC Related Exports */
	exports->Set(String::NewSymbol("ugcPublish"), FunctionTemplate::New(Greenworks::ugcPublish)->GetFunction());
	exports->Set(String::NewSymbol("ugcPublishUpdate"), FunctionTemplate::New(Greenworks::ugcPublishUpdate)->GetFunction());
	exports->Set(String::NewSymbol("ugcUnsubscribe"), FunctionTemplate::New(Greenworks::ugcUnsubscribe)->GetFunction());
	exports->Set(String::NewSymbol("ugcGetItems"), FunctionTemplate::New(Greenworks::ugcGetItems)->GetFunction());
	exports->Set(String::NewSymbol("ugcGetUserItems"), FunctionTemplate::New(Greenworks::ugcGetUserItems)->GetFunction());
	exports->Set(String::NewSymbol("ugcSynchronizeItems"), FunctionTemplate::New(Greenworks::ugcSynchronizeItems)->GetFunction());
	exports->Set(String::NewSymbol("ugcDownloadItem"), FunctionTemplate::New(Greenworks::ugcDownloadItem)->GetFunction());
	exports->Set(String::NewSymbol("ugcShowOverlay"), FunctionTemplate::New(Greenworks::ugcShowOverlay)->GetFunction());

	/* Exposed internals Exports */
	exports->Set(String::NewSymbol("runCallbacks"), FunctionTemplate::New(Greenworks::runCallbacks)->GetFunction());
	exports->SetAccessor(String::New("Utils"), Greenworks::GetUtils);

	/* Greenutils */
	Greenutils::Init(exports);
	
}


/* Conditional setup for node export naming */
#ifdef _WIN32
	// Win32 machines
	NODE_MODULE(greenworks_win, init)
#elif __APPLE__
	// Apple MAC
	NODE_MODULE(greenworks_osx, init)
#elif __linux__
	// Linux based systems
	#if __x86_64__ || __ppc64__
		// Linux 64 bit based systems
		NODE_MODULE(greenworks_linux64, init)
	#else
		// Linux 32 bit based systems
		NODE_MODULE(greenworks_linux32, init)
	#endif
#endif
#endif