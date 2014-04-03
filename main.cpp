#include <node.h>
#include <v8.h>
#include <string>
#include <iostream>
#include "steamworks-sdk/public/steam/steam_api.h"
#include "steamworks-sdk/public/steam/steam_gameserver.h"
#include "steamworks-sdk/public/steam/isteamremotestorage.h"
	
using namespace v8;
using namespace std;

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

class Greenworks : node::ObjectWrap {
	private:

		static void steamWriteToFile(uv_work_t *req) {
			FileIOAsync *writeData = (FileIOAsync*)req->data;
			ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

			// Checking quota (in the future we may need it)
			int nTotal = -1, nAvailable = -1;

			if (!pSteamRemoteStorage->GetQuota(&nTotal, &nAvailable)) {
				writeData->sError = "Error getting Cloud quota";
				writeData->bSuccess = false;
				return;
			}

			writeData->bSuccess = pSteamRemoteStorage->FileWrite(writeData->sFilename.c_str(), writeData->sContent.c_str(), (int)strlen(writeData->sContent.c_str()));
			if (!writeData->bSuccess)
				writeData->sError = "Error writing to file. ";

			return;
		}

		static void steamReadFromFile(uv_work_t *req) {
			FileIOAsync *readData = (FileIOAsync*)req->data;
			ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

			// Check if file exists
			if (!pSteamRemoteStorage->FileExists(readData->sFilename.c_str())) {
				readData->bSuccess = false;
				readData->sError = "File doesn't exist";
				return;
			}

			int32 nFileSize = pSteamRemoteStorage->GetFileSize(readData->sFilename.c_str());

			char *sFileContents = new char[nFileSize+1];
			int32 cubRead = pSteamRemoteStorage->FileRead( readData->sFilename.c_str(), sFileContents, nFileSize );
			sFileContents[cubRead] = 0; // null-terminate

			if (cubRead == 0 && nFileSize > 0) {
				readData->bSuccess = false;
				readData->sError = "Error loading file";
			}
			else {
				readData->bSuccess = true;
				readData->sContent = sFileContents;
			}

			delete sFileContents;
			return;
		}

		static void steamGetCloudQuota(uv_work_t *req) {
			CloudQuota *quotaData = (CloudQuota*)req->data;
			ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

			int nTotal = -1, nAvailable = -1;

			if (!pSteamRemoteStorage->GetQuota(&nTotal, &nAvailable)) {
				quotaData->bSuccess = false;
				return;
			}

			quotaData->bSuccess = true;
			quotaData->nTotalBytes = nTotal;
			quotaData->nAvailableBytes = nAvailable;

			return;
		}

		static void steamSetAchievement(uv_work_t *req) {
			Achievement *achievementData = (Achievement*)req->data;
			ISteamUserStats *pSteamUserStats = SteamUserStats();

			pSteamUserStats->SetAchievement(achievementData->sAchievementId.c_str());
			achievementData->bSuccess = pSteamUserStats->StoreStats();

			return;
		}

		static void fileWrote(uv_work_t *req) {
			FileIOAsync *writeData = (FileIOAsync*)req->data;

			// Calling callback here
			Handle<Value> callbackResultArgs[1];

			if (writeData->bSuccess) {
				callbackResultArgs[0] = Undefined();
				writeData->successCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
			}
			else {
				callbackResultArgs[0] = String::New(writeData->sError.c_str());
				writeData->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
			}

			return;
		}

		static void fileLoaded(uv_work_t *req) {
			FileIOAsync *readData = (FileIOAsync*)req->data;

			// Calling callback here
			Handle<Value> callbackResultArgs[1];

			if (readData->bSuccess) {
				callbackResultArgs[0] = String::New(readData->sContent.c_str());
				readData->successCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
			}
			else {
				callbackResultArgs[0] = String::New(readData->sError.c_str());
				readData->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
			}
		}

		static void gotQuota(uv_work_t *req) {
			CloudQuota *quotaData = (CloudQuota*)req->data;

			Handle<Value> callbackResultArgs[2];

			if (quotaData->bSuccess) {
				callbackResultArgs[0] = Integer::New(quotaData->nTotalBytes);
				callbackResultArgs[1] = Integer::New(quotaData->nAvailableBytes);
				quotaData->successCallback->Call(Context::GetCurrent()->Global(), 2, callbackResultArgs);
			}
			else {
				callbackResultArgs[0] = Undefined();
				quotaData->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
			}
		}

		static void achievementStored(uv_work_t *req) {
			Achievement *achievementData = (Achievement*)req->data;

			Handle<Value> callbackResultArgs[1];
			callbackResultArgs[0] = Undefined();

			if (achievementData->bSuccess) {
				achievementData->successCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
			}
			else {
				achievementData->errorCallback->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
			}
		}

	public:
		Greenworks()  {}
		~Greenworks() {}

		static Handle<Value> initAPI(const Arguments& args) {
			HandleScope scope;

			// Initializing Steam API
			bool bSuccess = SteamAPI_Init();

			if (bSuccess) {
				ISteamUserStats *pSteamUserStats = SteamUserStats();
				pSteamUserStats->RequestCurrentStats();
			}

			return scope.Close(Boolean::New(bSuccess));
		}

		static Handle<Value> getCloudQuota(const Arguments& args) {
			HandleScope scope;
			ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

			Local<Function>successCallback = Local<Function>::Cast(args[0]);
			Local<Function>errorCallback   = Local<Function>::Cast(args[1]);

			CloudQuota *quotaData = new CloudQuota;
			quotaData->successCallback  = Persistent<Function>::New(successCallback);
			quotaData->errorCallback    = Persistent<Function>::New(errorCallback);

			uv_work_t *req = new uv_work_t;
			req->data = quotaData;

			// Call separate thread work
			uv_queue_work(uv_default_loop(), req, steamGetCloudQuota, (uv_after_work_cb)gotQuota);

			return scope.Close(Undefined());
		}

		static Handle<Value> saveTextToFile(const Arguments& args) {
			HandleScope scope;
			ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

			// Convert passed string arguments directly to std::string
			string sFilename(*String::Utf8Value(args[0]));
			string sContent(*String::Utf8Value(args[1]));
			Local<Function>successCallback = Local<Function>::Cast(args[2]);
			Local<Function>errorCallback   = Local<Function>::Cast(args[3]);

			FileIOAsync *writeData = new FileIOAsync;
			writeData->successCallback  = Persistent<Function>::New(successCallback);
			writeData->errorCallback    = Persistent<Function>::New(errorCallback);
			writeData->sFilename = sFilename;
			writeData->sContent  = sContent;

			// Preparing separate thread work call
			uv_work_t *req = new uv_work_t;
			req->data = writeData;

			// Call separate thread work
			uv_queue_work(uv_default_loop(), req, steamWriteToFile, (uv_after_work_cb)fileWrote);

			return scope.Close(Undefined());
		}

		static Handle<Value> readTextFromFile(const Arguments& args) {
			HandleScope scope;

			// Convert passed string arguments directly to std::string
			string sFilename(*String::Utf8Value(args[0]));
			Local<Function>successCallback = Local<Function>::Cast(args[1]);
			Local<Function>errorCallback   = Local<Function>::Cast(args[2]);

			ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();
			FileIOAsync *readData = new FileIOAsync;
			readData->successCallback  = Persistent<Function>::New(successCallback);
			readData->errorCallback    = Persistent<Function>::New(errorCallback);
			readData->sFilename = sFilename;

			// Preparing separate thread work call
			uv_work_t *req = new uv_work_t;
			req->data = readData;

			// Call separate thread work
			uv_queue_work(uv_default_loop(), req, steamReadFromFile, (uv_after_work_cb)fileLoaded);

			return scope.Close(Undefined());
		}

		static Handle<Value> activateAchievement(const Arguments& args) {
			HandleScope scope;

			string sAchievementId(*String::Utf8Value(args[0]));
			Local<Function>successCallback = Local<Function>::Cast(args[1]);
			Local<Function>errorCallback   = Local<Function>::Cast(args[2]);

			Achievement *achievementData = new Achievement;
			achievementData->successCallback = Persistent<Function>::New(successCallback);
			achievementData->errorCallback   = Persistent<Function>::New(errorCallback);
			achievementData->sAchievementId  = sAchievementId;

			// Preparing separate thread work call
			uv_work_t *req = new uv_work_t;
			req->data = achievementData;

			// Call separate thread work
			uv_queue_work(uv_default_loop(), req, steamSetAchievement, (uv_after_work_cb)achievementStored);

			return scope.Close(Undefined());
		}

		static Handle<Value> enableCloud(const Arguments& args){
			HandleScope scope;

			bool bEnableCloud = args[0]->ToBoolean()->Value();
			ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

			pSteamRemoteStorage->SetCloudEnabledForApp(bEnableCloud);

			return scope.Close(Undefined());
		}

		static Handle<Value> isCloudEnabled(const Arguments& args){
			HandleScope scope;
			ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();
			return scope.Close(Boolean::New(pSteamRemoteStorage->IsCloudEnabledForApp()));
		}
};

void init(Handle<Object> exports) {
	exports->Set(String::NewSymbol("initAPI"), FunctionTemplate::New(Greenworks::initAPI)->GetFunction());
	exports->Set(String::NewSymbol("getCloudQuota"), FunctionTemplate::New(Greenworks::getCloudQuota)->GetFunction());
	exports->Set(String::NewSymbol("saveTextToFile"), FunctionTemplate::New(Greenworks::saveTextToFile)->GetFunction());
	exports->Set(String::NewSymbol("readTextFromFile"), FunctionTemplate::New(Greenworks::readTextFromFile)->GetFunction());
	exports->Set(String::NewSymbol("activateAchievement"), FunctionTemplate::New(Greenworks::activateAchievement)->GetFunction());
	exports->Set(String::NewSymbol("isCloudEnabled"), FunctionTemplate::New(Greenworks::isCloudEnabled)->GetFunction());
	exports->Set(String::NewSymbol("enableCloud"), FunctionTemplate::New(Greenworks::enableCloud)->GetFunction());
}

#ifdef _WIN32
	NODE_MODULE(greenworks_win, init)
#elif __APPLE__
	NODE_MODULE(greenworks_osx, init)
#elif __linux__
	NODE_MODULE(greenworks_linux, init)
#endif
