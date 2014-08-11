/* INCLUDE HEADER */
#include "Greenworks.h"

/* ********************************************************************************************************************************** */
/* PRIVATE  MEMBERS */
/* ********************************************************************************************************************************** */
Persistent<Value> Greenworks::_Utils;

Handle<Object> Greenworks::GetObjectFromWsItem(CSteamWorkshopItem item){

	Handle<Object> result = Object::New();

	result->Set(String::New("acceptedForUse"), Boolean::New(item.Details->m_bAcceptedForUse));
	result->Set(String::New("banned"), Boolean::New(item.Details->m_bBanned));
	result->Set(String::New("tagsTruncated"), Boolean::New(item.Details->m_bTagsTruncated));
	result->Set(String::New("fileType"), Integer::New(item.Details->m_eFileType));
	result->Set(String::New("result"), Integer::New(item.Details->m_eResult));
	result->Set(String::New("visibility"), Integer::New(item.Details->m_eVisibility));
	result->Set(String::New("score"), Number::New(item.Details->m_flScore));

	result->Set(String::New("file"), String::New(to_string(item.Details->m_hFile).c_str()));
	result->Set(String::New("fileName"), String::New(item.Details->m_pchFileName));
	result->Set(String::New("fileSize"), Integer::New(item.Details->m_nFileSize));

	result->Set(String::New("previewFile"), String::New(to_string(item.Details->m_hPreviewFile).c_str()));
	result->Set(String::New("previewFileSize"), Integer::New(item.Details->m_nPreviewFileSize));

	result->Set(String::New("steamIDOwner"), Integer::New(item.Details->m_ulSteamIDOwner));
	result->Set(String::New("consumerAppID"), Integer::New(item.Details->m_nConsumerAppID));
	result->Set(String::New("creatorAppID"), Integer::New(item.Details->m_nCreatorAppID));
	result->Set(String::New("publishedFileId"), Integer::New(item.Details->m_nPublishedFileId));

	result->Set(String::New("title"), String::New(item.Details->m_rgchTitle));
	result->Set(String::New("description"), String::New(item.Details->m_rgchDescription));
	result->Set(String::New("URL"), String::New(item.Details->m_rgchURL));
	result->Set(String::New("tags"), String::New(item.Details->m_rgchTags));

	result->Set(String::New("timeAddedToUserList"), Integer::New(item.Details->m_rtimeAddedToUserList));
	result->Set(String::New("timeCreated"), Integer::New(item.Details->m_rtimeCreated));
	result->Set(String::New("timeUpdated"), Integer::New(item.Details->m_rtimeUpdated));
	result->Set(String::New("votesDown"), Integer::New(item.Details->m_unVotesDown));
	result->Set(String::New("votesUp"), Integer::New(item.Details->m_unVotesUp));

	// Extended info
	result->Set(String::New("hasChanged"), Boolean::New(item.HasChanged));
	result->Set(String::New("isNew"), Boolean::New(item.IsNew));
	result->Set(String::New("timeModified"), Integer::New(item.Modified));
	result->Set(String::New("fileSize"), Integer::New(item.Size));
	result->Set(String::New("targetFile"), String::New(item.TargetFile.c_str()));

	return result;
}

Handle<Value> Greenworks::GetObjectFromSteamAccountType(int eAccountType){
	Handle<Object> obj = Object::New();
	string name = "";

	obj->Set(String::New("value"), Integer::New(eAccountType));


	switch (eAccountType){
	case k_EAccountTypeAnonGameServer:
		name = "k_EAccountTypeAnonGameServer";
		break;

	case k_EAccountTypeAnonUser:
		name = "k_EAccountTypeAnonUser";
		break;

	case k_EAccountTypeChat:
		name = "k_EAccountTypeChat";
		break;

	case k_EAccountTypeClan:
		name = "k_EAccountTypeClan";
		break;

	case k_EAccountTypeConsoleUser:
		name = "k_EAccountTypeConsoleUser";
		break;

	case k_EAccountTypeContentServer:
		name = "k_EAccountTypeContentServer";
		break;

	case k_EAccountTypeGameServer:
		name = "k_EAccountTypeGameServer";
		break;

	case k_EAccountTypeIndividual:
		name = "k_EAccountTypeIndividual";
		break;

	case k_EAccountTypeInvalid:
		name = "k_EAccountTypeInvalid";
		break;

	case k_EAccountTypeMax:
		name = "k_EAccountTypeMax";
		break;

	case k_EAccountTypeMultiseat:
		name = "k_EAccountTypeMultiseat";
		break;

	case k_EAccountTypePending:
		name = "k_EAccountTypePending";
		break;

	}

	obj->Set(String::New("name"), String::New(name.c_str()));

	return obj;
}

Handle<Object> Greenworks::GetProgressCallbackObject(string statusMsg, string reasonMsg, float value){
	Handle<Object> result = Object::New();

	result->Set(String::New("status"), String::New(statusMsg.c_str()));
	result->Set(String::New("reason"), String::New(reasonMsg.c_str()));
	result->Set(String::New("value"), Number::New(value));

	return result;
}

float Greenworks::GetProgressPerc(int numOfWsItems, int wsItemIndex){
	float fact = 100 / numOfWsItems;
	return fact * (wsItemIndex + 1);
}
	
void Greenworks::steamWriteToFile(uv_work_t *req) {
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
	
void Greenworks::steamReadFromFile(uv_work_t *req) {
	FileIOAsync *readData = (FileIOAsync*)req->data;
	ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

	// Check if file exists
	if (!pSteamRemoteStorage->FileExists(readData->sFilename.c_str())) {
		readData->bSuccess = false;
		readData->sError = "File doesn't exist";
		return;
	}

	int32 nFileSize = pSteamRemoteStorage->GetFileSize(readData->sFilename.c_str());

	char *sFileContents = new char[nFileSize + 1];
	int32 cubRead = pSteamRemoteStorage->FileRead(readData->sFilename.c_str(), sFileContents, nFileSize);
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
	
void Greenworks::steamGetCloudQuota(uv_work_t *req) {
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
	
void Greenworks::steamSetAchievement(uv_work_t *req) {
	Achievement *achievementData = (Achievement*)req->data;
	ISteamUserStats *pSteamUserStats = SteamUserStats();

	pSteamUserStats->SetAchievement(achievementData->sAchievementId.c_str());
	achievementData->bSuccess = pSteamUserStats->StoreStats();

	return;
}

void Greenworks::fileWrote(uv_work_t *req) {
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

void Greenworks::fileLoaded(uv_work_t *req) {
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

void Greenworks::gotQuota(uv_work_t *req) {
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

void Greenworks::achievementStored(uv_work_t *req) {
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

void Greenworks::ugcGetItemsWorker(uv_work_t *req) {
	// Workshop *ws = (Workshop*)req->data;
	CUtils::Sleep(100);
}

void Greenworks::ugcGetItemsWorkerComplete(uv_work_t *req) {
	// Get Workshop object from worker
	Workshop *ws = (Workshop*)req->data;

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Value> errorCallbackVals[1];
	Handle<Value> successCallbackVals[1];

	Handle<Object> progressCallbackArg = Object::New();
	Handle<Object> successCallbackArg = Object::New();
	Handle<String> errorCallbackArg = String::New("");

	progressCallbackVals[0] = progressCallbackArg;
	errorCallbackVals[0] = errorCallbackArg;
	successCallbackVals[0] = successCallbackArg;

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	try {
		if (ws->steamWs->IsRequesting()){
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Waiting for Steam Request to complete....");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

			// Call Progress Callback on JS
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcGetItemsWorker, (uv_after_work_cb)ugcGetItemsWorkerComplete);
		}
		else {
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Steam Request completed.");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_complete", ws->reason, ws->progress);

			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Preparing data to return...");

			// Prepare an array of items matching the requested items prior to call the Success callback
			Handle<Array> data = Array::New(0);

			if (ws->steamWs->m_iNumWorkshopItems > 0){
				data = Array::New(ws->steamWs->m_iNumWorkshopItems);

				for (int i = 0; i < ws->steamWs->m_wsItems.size(); i++){
					CSteamWorkshopItem item = ws->steamWs->m_wsItems[i];
					Handle<Object> result = GetObjectFromWsItem(item);
					data->Set(i, result);
				}
			}

			if (ws->steamWs->SuccessDelegate->IsCallable()){
				successCallbackArg->Set(String::New("count"), Integer::New(ws->steamWs->m_iNumWorkshopItems));
				successCallbackArg->Set(String::New("items"), data);

				ws->steamWs->SuccessDelegate->Call(Context::GetCurrent()->Global(), 1, successCallbackVals);
			}
		}
	}
	catch (Exception ex) {
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			errorCallbackArg = String::New("An error occured while trying to handle a Steam UGC Request!\n\nSource: Greenworks::ugcGetItemsWorkerComplete");
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}
	}


}

void Greenworks::ugcGetUserItemsWorker(uv_work_t *req) {
	// Workshop *ws = (Workshop*)req->data;
	CUtils::Sleep(100);
}

void Greenworks::ugcGetUserItemsWorkerComplete(uv_work_t *req) {
	// Get Workshop object from worker
	Workshop *ws = (Workshop*)req->data;

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Value> errorCallbackVals[1];
	Handle<Value> successCallbackVals[1];

	Handle<Object> progressCallbackArg = Object::New();
	Handle<Object> successCallbackArg = Object::New();
	Handle<String> errorCallbackArg = String::New("");

	progressCallbackVals[0] = progressCallbackArg;
	errorCallbackVals[0] = errorCallbackArg;
	successCallbackVals[0] = successCallbackArg;

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	try {
		if (ws->steamWs->IsRequesting()){
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Waiting for Steam Request to complete....");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

			// Call Progress Callback on JS
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcGetUserItemsWorker, (uv_after_work_cb)ugcGetUserItemsWorkerComplete);
		}
		else {
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Steam Request completed.");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_complete", ws->reason, ws->progress);

			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Preparing data to return...");

			// Prepare an array of items matching the requested items prior to call the Success callback
			Handle<Array> data = Array::New(0);

			if (ws->steamWs->m_iNumWorkshopItems > 0){
				data = Array::New(ws->steamWs->m_iNumWorkshopItems);

				for (int i = 0; i < ws->steamWs->m_wsItems.size(); i++){
					CSteamWorkshopItem item = ws->steamWs->m_wsItems[i];
					Handle<Object> result = GetObjectFromWsItem(item);
					data->Set(i, result);
				}
			}

			if (ws->steamWs->SuccessDelegate->IsCallable()){
				successCallbackArg->Set(String::New("count"), Integer::New(ws->steamWs->m_iNumWorkshopItems));
				successCallbackArg->Set(String::New("items"), data);

				ws->steamWs->SuccessDelegate->Call(Context::GetCurrent()->Global(), 1, successCallbackVals);
			}
		}
	}
	catch (Exception ex) {
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			errorCallbackArg = String::New("An error occured while trying to handle a Steam UGC User Request!\n\nSource: Greenworks::ugcGetUserItemsWorkerComplete");
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}
	}


}

void Greenworks::ugcDownloadItemWorker(uv_work_t *req) {
	// Workshop *ws = (Workshop*)req->data;
	CUtils::Sleep(100);
}

void Greenworks::ugcDownloadItemWorkerComplete(uv_work_t *req) {
	// Get Workshop object from worker
	Workshop *ws = (Workshop*)req->data;

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Value> errorCallbackVals[1];
	Handle<Value> successCallbackVals[1];

	Handle<Object> progressCallbackArg = Object::New();
	Handle<Object> successCallbackArg = Object::New();
	Handle<String> errorCallbackArg = String::New("");

	progressCallbackVals[0] = progressCallbackArg;
	errorCallbackVals[0] = errorCallbackArg;
	successCallbackVals[0] = successCallbackArg;

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	try {
		if (ws->steamWs->IsRequesting()){
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Waiting for Steam Download Request to complete....");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

			// Call Progress Callback on JS
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcDownloadItemWorker, (uv_after_work_cb)ugcDownloadItemWorkerComplete);
		}
		else {
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Steam Download Request completed.");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_complete", ws->reason, ws->progress);

			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			if (ws->steamWs->SuccessDelegate->IsCallable()){
				// successCallbackArg->Set(String::New("count"), Integer::New(ws->steamWs->m_iNumWorkshopItems));
				// successCallbackArg->Set(String::New("items"), data);

				ws->steamWs->SuccessDelegate->Call(Context::GetCurrent()->Global(), 1, successCallbackVals);
			}
		}
	}
	catch (Exception ex) {
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			errorCallbackArg = String::New("An error occured while trying to handle a Steam UGC Download Request!\n\nSource: Greenworks::ugcDownloadItemWorkerComplete");
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}
	}
}

void Greenworks::ugcSynchronizeItemsWorker(uv_work_t *req) {
	// Workshop *ws = (Workshop*)req->data;
	CUtils::Sleep(100);
}

void Greenworks::ugcSynchronizeItemsWorkerComplete(uv_work_t *req) {
	// Get Workshop object from worker
	Workshop *ws = (Workshop*)req->data;

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Value> errorCallbackVals[1];
	Handle<Value> successCallbackVals[1];

	Handle<Object> progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);
	Handle<Object> successCallbackArg = Object::New();
	Handle<String> errorCallbackArg = String::New("");

	progressCallbackVals[0] = progressCallbackArg;
	errorCallbackVals[0] = errorCallbackArg;
	successCallbackVals[0] = successCallbackArg;

	// Call Progress Callback on JS
	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	try {
		if (ws->steamWs->IsRequesting()){
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Waiting for Steam Request to complete....");

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcSynchronizeItemsWorker, (uv_after_work_cb)ugcSynchronizeItemsWorkerComplete);
		}
		else {
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Steam Request completed.");
			CUtils::WriteToJsConsoleAndLogFile("Processing results...");

			// Process result(s)
			if (ws->steamWs->m_iNumWorkshopItems > 0){

				// Increment current file index for the file to process
				ws->fileIndex++;

				if (ws->fileIndex < ws->steamWs->m_iNumWorkshopItems){
					// Acquire our file informations
					CSteamWorkshopItem item = ws->steamWs->m_wsItems[ws->fileIndex];

					// Prepare our JS object to push into context
					// --> Handle<Object> result = GetObjectFromWsItem(item);

					// Store current item into local WS struct (will be used for success push)
					// --> ws->wsItems->Set(ws->fileIndex, result);

					// Update Progress Value
					ws->progress = GetProgressPerc(ws->steamWs->m_iNumWorkshopItems, ws->fileIndex);

					// Update Reason 
					ws->reason = CUtils::GetFilename(ws->steamWs->m_wsItems[ws->fileIndex].Details->m_pchFileName);

					// Request Steam Download
					ws->steamWs->Download(item.Details->m_hFile, ws->targetFolder);

					// Call the next step in the chain for downloading the Items now
					uv_queue_work(uv_default_loop(), req, ugcSynchronizeItemsWorker, (uv_after_work_cb)ugcSynchronizeItemsWorkerComplete);
				}
				else {
					// Update / Set Callback Arg Object
					ws->progress = 100;
					progressCallbackArg = GetProgressCallbackObject("steam_request_complete", ws->reason, ws->progress);

					// Call JS callback
					if (ws->steamWs->ProgressDelegate->IsCallable()){
						ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
					}

					// Write to JS console if appliable
					CUtils::WriteToJsConsoleAndLogFile("Done. Calling SUCCESS callback.");

					if (ws->steamWs->SuccessDelegate->IsCallable()){

						// Update item information to contain all information incl. extended info (isNew, hasChanged, targetFile)
						for (int i = 0; i < ws->steamWs->m_iNumWorkshopItems; i++){
							CSteamWorkshopItem item = ws->steamWs->m_wsItems[i];
							ws->wsItems->Set(i, GetObjectFromWsItem(item));
						}

						// Update / Set Callback Arg Object
						successCallbackArg->Set(String::New("items"), ws->wsItems);
						successCallbackArg->Set(String::New("count"), Integer::New(ws->steamWs->m_iNumWorkshopItems));

						// Call JS Callback (success / finished)
						ws->steamWs->SuccessDelegate->Call(Context::GetCurrent()->Global(), 1, successCallbackVals);
					}
				}
			}
			else {
				if (ws->steamWs->SuccessDelegate->IsCallable()){
					// Update / Set Callback Arg Object
					successCallbackArg->Set(String::New("items"), Array::New(0));
					successCallbackArg->Set(String::New("count"), Integer::New(0));

					ws->steamWs->SuccessDelegate->Call(Context::GetCurrent()->Global(), 1, successCallbackVals);
				}
			}

		}
	}
	catch (Exception ex) {
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			errorCallbackArg = String::New("An error occured while trying to handle a Steam UGC User Request!\n\nSource: Greenworks::ugcGetUserItemsWorkerComplete");
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}
	}
}

void Greenworks::ugcSynchronizeItemsDownloadWorkerComplete(uv_work_t *req) {
	// Get Workshop object from worker
	Workshop *ws = (Workshop*)req->data;

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Value> errorCallbackVals[1];
	Handle<Value> successCallbackVals[1];

	Handle<Object> progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);
	Handle<Object> successCallbackArg = Object::New();
	Handle<String> errorCallbackArg = String::New("");

	progressCallbackVals[0] = progressCallbackArg;
	errorCallbackVals[0] = errorCallbackArg;
	successCallbackVals[0] = successCallbackArg;

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	try {
		if (ws->steamWs->IsRequesting()){
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Waiting for Steam Download Request to complete....");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

			// Call Progress Callback on JS
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcSynchronizeItemsWorkerComplete, (uv_after_work_cb)ugcSynchronizeItemsDownloadWorkerComplete);
		}
		else {
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Steam Download Request completed.");


			if (ws->steamWs->m_iNumWorkshopItems > 0 && ws->fileIndex < ws->steamWs->m_iNumWorkshopItems) {
				progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

				if (ws->steamWs->ProgressDelegate->IsCallable()){
					ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
				}

				// Go for another thread-roundtrip and return afterwards
				uv_queue_work(uv_default_loop(), req, ugcSynchronizeItemsWorkerComplete, (uv_after_work_cb)ugcSynchronizeItemsDownloadWorkerComplete);
			}
			else {
				progressCallbackArg = GetProgressCallbackObject("steam_request_complete", ws->reason, ws->progress);

				if (ws->steamWs->ProgressDelegate->IsCallable()){
					ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
				}

				/* Success is handled by main calling complete callback ugcSynchronizeItemsWorkerComplete */
			}

			CUtils::WriteToJsConsoleAndLogFile("Steam Download Request completed.");
		}
	}
	catch (Exception ex) {
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			errorCallbackArg = String::New("An error occured while trying to handle a Steam UGC Download Request!\n\nSource: Greenworks::ugcDownloadItemWorkerComplete");
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}
	}
}

void Greenworks::ugcPublishWorker(uv_work_t *req) {
	Workshop *ws = (Workshop*)req->data;

	// End worker if file doesn't exist
	if (!CUtils::FileExists(ws->targetFile.c_str())){
		ws->bSuccess = false;
		return;
	}

	// If we're publishing, return
	if (ws->bIsPublishing){
		CUtils::Sleep(100);
		return;
	}

	FILE *file = NULL;			// File pointer
	FILE *imageFile = NULL;     // Image file pointer

	// Open the file in binary mode using the "rb" format string
	// This also checks if the file exists and/or can be opened for reading correctly
	if ((file = fopen(ws->targetFile.c_str(), "rb")) == NULL){
		ws->bSuccess = false;
		// cout << "Could not open specified file" << endl;
	}
	else{
		ws->bSuccess = true;

		// We could open the main file, now try the image file (if it exists!)
		if ((imageFile = fopen(ws->targetImageFile.c_str(), "rb")) != NULL){

			// Get the size of the image file in bytes
			ws->imageFileSize = CUtils::FileSize(imageFile);

			// Allocate space in the buffer for the whole image file
			ws->imageFileBuf = new BYTE[ws->imageFileSize];

			// Read the image file into the buffer
			fread(ws->imageFileBuf, ws->imageFileSize, 1, imageFile);

			// Close image file 
			fclose(imageFile);
		}
		// cout << "File opened successfully" << endl;
	}

	// Get the size of the file in bytes
	ws->fileSize = CUtils::FileSize(file);

	// Allocate space in the buffer for the whole file
	ws->fileBuf = new BYTE[ws->fileSize];

	// Read the file into the buffer
	fread(ws->fileBuf, ws->fileSize, 1, file);
		
	// Close file 
	fclose(file);

	// We need publish now, flag it
	ws->bNeedsPublish = true;
}

void Greenworks::ugcPublishWorkerComplete(uv_work_t *req) {
	Workshop *ws = (Workshop*)req->data;

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Value> errorCallbackVals[1];
	Handle<Value> successCallbackVals[1];

	Handle<Object> progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);
	Handle<Object> successCallbackArg = Object::New();
	Handle<String> errorCallbackArg = String::New("");

	progressCallbackVals[0] = progressCallbackArg;
	errorCallbackVals[0] = errorCallbackArg;
	successCallbackVals[0] = successCallbackArg;

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	if (!ws->bSuccess){
		// File Read failed in worker, return now
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			string errMsg = "An error occured while trying to handle a Steam Publish Request! Could not read file: " + ws->targetFile + "\n\nSource: Greenworks::ugcPublishWorkerComplete";
			errorCallbackArg = String::New(errMsg.c_str());
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}

		ws->bIsPublishing = false;
		ws->bNeedsPublish = false;

		ws->fileSize = 0;

		// Clear buffer
		// Note: Normally C++ guarantees that delete will not throw any exception in case of values being NULL
		//		 On a non constant pattern, this causes a crash, try...catch does not help to prevent this issues with v8/js
		//		 Hence, we test explictly on NULL pointer before using delete[]. This seems to work.
		CUtils::WriteToJsConsoleAndLogFile("Clearing buffers:");

		if (ws->fileBuf != NULL){
			CUtils::WriteToJsConsoleAndLogFile("- File Buffer...");
			delete[] ws->fileBuf;
			CUtils::WriteToJsConsoleAndLogFile("- File Buffer: Done!");
		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("- File Buffer: Not used.");
		}

		if (ws->imageFileBuf != NULL){
			CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer...");
			delete[] ws->imageFileBuf;
			CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer: Done!");
		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer: Not used.");
		}

		// Exit
		return;
	}


	try {
		if (ws->bNeedsPublish){
			// Publish to Steam now
			ws->steamWs->Publish(ws->targetFile, ws->title, ws->description, ws->fileBuf, ws->fileSize, ws->targetImageFile, ws->imageFileBuf, ws->imageFileSize);

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

			// Call Progress Callback on JS
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			ws->bIsPublishing = true;
			ws->bNeedsPublish = false;

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcPublishWorker, (uv_after_work_cb)ugcPublishWorkerComplete);
		}
		else if (ws->steamWs->IsRequesting()){
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Waiting for Steam Publish Request to complete....");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

			// Call Progress Callback on JS
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcPublishWorker, (uv_after_work_cb)ugcPublishWorkerComplete);
		}
		else {
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Steam Publish Request completed.");

			// Update / Set Callback Arg Object
			ws->progress = 100;
			progressCallbackArg = GetProgressCallbackObject("steam_request_complete", ws->reason, ws->progress);

			// Call JS callback
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Reset flags
			ws->bIsPublishing = false;
			ws->bNeedsPublish = false;

			// Clear buffer
			// Note: Normally C++ guarantees that delete will not throw any exception in case of values being NULL
			//		 On a non constant pattern, this causes a crash, try...catch does not help to prevent this issues with v8/js
			//		 Hence, we test explictly on NULL pointer before using delete[]. This seems to work.
			CUtils::WriteToJsConsoleAndLogFile("Clearing buffers:");

			if (ws->fileBuf != NULL){
				CUtils::WriteToJsConsoleAndLogFile("- File Buffer...");
				delete[] ws->fileBuf;
				CUtils::WriteToJsConsoleAndLogFile("- File Buffer: Done!");
			}
			else {
				CUtils::WriteToJsConsoleAndLogFile("- File Buffer: Not used.");
			}

			if (ws->imageFileBuf != NULL){
				CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer...");
				delete[] ws->imageFileBuf;
				CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer: Done!");
			}
			else {
				CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer: Not used.");
			}

			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Done. Calling SUCCESS callback.");

			if (ws->steamWs->SuccessDelegate->IsCallable()){
				// Get the last published file id to return in callback
				successCallbackArg->Set(String::New("id"), Integer::New(ws->steamWs->GetLastPublishedFileId()));

				// Call JS Callback (success / finished)
				ws->steamWs->SuccessDelegate->Call(Context::GetCurrent()->Global(), 1, successCallbackVals);
			}
		}
	}
	catch (Exception ex) {
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			errorCallbackArg = String::New("An error occured while trying to handle a Steam Publish Request!\n\nSource: Greenworks::ugcPublishWorkerComplete");
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}
	}
}
	
void Greenworks::ugcPublishUpdateWorker(uv_work_t *req) {
	Workshop *ws = (Workshop*)req->data;

	// If we're publishing, return
	if (ws->bIsPublishing){
		CUtils::Sleep(100);
		return;
	}

	FILE *file = NULL;			// File pointer
	FILE *imageFile = NULL;     // Image file pointer

	// Open the file in binary mode using the "rb" format string
	// This also checks if the file exists and/or can be opened for reading correctly
	if (CUtils::FileExists(ws->targetFile.c_str()) && (file = fopen(ws->targetFile.c_str(), "rb")) != NULL){
		// Get the size of the file in bytes
		ws->fileSize = CUtils::FileSize(file);

		// Allocate space in the buffer for the whole file
		ws->fileBuf = new BYTE[ws->fileSize];

		// Read the file into the buffer
		fread(ws->fileBuf, ws->fileSize, 1, file);

		// Close file 
		fclose(file);
	}

	// Now try the image file (if it exists!)
	if (CUtils::FileExists(ws->targetImageFile.c_str()) && (imageFile = fopen(ws->targetImageFile.c_str(), "rb")) != NULL){

		// Get the size of the image file in bytes
		ws->imageFileSize = CUtils::FileSize(imageFile);

		// Allocate space in the buffer for the whole image file
		ws->imageFileBuf = new BYTE[ws->imageFileSize];

		// Read the image file into the buffer
		fread(ws->imageFileBuf, ws->imageFileSize, 1, imageFile);

		// Close image file 
		fclose(imageFile);
	}

	// We need publish now, flag it
	ws->bSuccess = true;
	ws->bNeedsPublish = true;
}
	
void Greenworks::ugcPublishUpdateWorkerComplete(uv_work_t *req) {
	Workshop *ws = (Workshop*)req->data;

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Value> errorCallbackVals[1];
	Handle<Value> successCallbackVals[1];

	Handle<Object> progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);
	Handle<Object> successCallbackArg = Object::New();
	Handle<String> errorCallbackArg = String::New("");

	progressCallbackVals[0] = progressCallbackArg;
	errorCallbackVals[0] = errorCallbackArg;
	successCallbackVals[0] = successCallbackArg;

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();


	if (!ws->bSuccess){
		// File Read failed in worker, return now
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			string errMsg = "An error occured while trying to handle a Steam Publish Request! Could not read file: " + ws->targetFile + "\n\nSource: Greenworks::ugcPublishWorkerComplete";
			errorCallbackArg = String::New(errMsg.c_str());
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}

		ws->bIsPublishing = false;
		ws->bNeedsPublish = false;

		ws->fileSize = 0;

		// Clear buffer
		// Note: Normally C++ guarantees that delete will not throw any exception in case of values being NULL
		//		 On a non constant pattern, this causes a crash, try...catch does not help to prevent this issues with v8/js
		//		 Hence, we test explictly on NULL pointer before using delete[]. This seems to work.
		CUtils::WriteToJsConsoleAndLogFile("Clearing buffers:");

		if (ws->fileBuf != NULL){
			CUtils::WriteToJsConsoleAndLogFile("- File Buffer...");
			delete[] ws->fileBuf;
			CUtils::WriteToJsConsoleAndLogFile("- File Buffer: Done!");
		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("- File Buffer: Not used.");
		}

		if (ws->imageFileBuf != NULL){
			CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer...");
			delete[] ws->imageFileBuf;
			CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer: Done!");
		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer: Not used.");
		}

		CUtils::WriteToJsConsoleAndLogFile("ugcPublishUpdateWorkerComplete::ERROR");

		// Exit
		return;
	}


	try {
		if (ws->bNeedsPublish){
			// Publish to Steam now
			ws->steamWs->PublishUpdate(ws->targetFile, ws->title, ws->description, ws->fileBuf, ws->fileSize, ws->targetImageFile, ws->imageFileBuf, ws->imageFileSize, ws->publishedFileId);

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

			// Call Progress Callback on JS
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			ws->bIsPublishing = true;
			ws->bNeedsPublish = false;

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcPublishUpdateWorker, (uv_after_work_cb)ugcPublishUpdateWorkerComplete);
		}
		else if (ws->steamWs->IsRequesting()){
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Waiting for Steam Update Request to complete....");

			// Update / Set Callback Arg Object
			progressCallbackArg = GetProgressCallbackObject("steam_request_running", ws->reason, ws->progress);

			// Call Progress Callback on JS
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcPublishUpdateWorker, (uv_after_work_cb)ugcPublishUpdateWorkerComplete);
		}
		else {
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Steam Update Request completed.");

			// Update / Set Callback Arg Object
			ws->progress = 100;
			progressCallbackArg = GetProgressCallbackObject("steam_request_complete", ws->reason, ws->progress);

			// Call JS callback
			if (ws->steamWs->ProgressDelegate->IsCallable()){
				ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}

			// Reset flags
			ws->bIsPublishing = false;
			ws->bNeedsPublish = false;

			// Clear buffer
			// Note: Normally C++ guarantees that delete will not throw any exception in case of values being NULL
			//		 On a non constant pattern, this causes a crash, try...catch does not help to prevent this issues with v8/js
			//		 Hence, we test explictly on NULL pointer before using delete[]. This seems to work.
			CUtils::WriteToJsConsoleAndLogFile("Clearing buffers:");

			if (ws->fileBuf != NULL){
				CUtils::WriteToJsConsoleAndLogFile("- File Buffer...");
				delete[] ws->fileBuf;
				CUtils::WriteToJsConsoleAndLogFile("- File Buffer: Done!");
			}
			else {
				CUtils::WriteToJsConsoleAndLogFile("- File Buffer: Not used.");
			}

			if (ws->imageFileBuf != NULL){
				CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer...");
				delete[] ws->imageFileBuf;
				CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer: Done!");
			}
			else {
				CUtils::WriteToJsConsoleAndLogFile("- Image File Buffer: Not used.");
			}

			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Done. Calling SUCCESS callback.");

			if (ws->steamWs->SuccessDelegate->IsCallable()){
				// Call JS Callback (success / finished)
				ws->steamWs->SuccessDelegate->Call(Context::GetCurrent()->Global(), 1, successCallbackVals);
			}

		}
	}
	catch (Exception ex) {
		if (ws->steamWs->ErrorDelegate->IsCallable()){
			errorCallbackArg = String::New("An error occured while trying to handle a Steam Update  Request!\n\nSource: Greenworks::ugcPublishUpdateWorkerComplete");
			ws->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}
	}
}
	
void Greenworks::ugcUnsubscribeWorker(uv_work_t *req) {
	// Workshop *ws = (Workshop*)req->data;
	CUtils::Sleep(100);
}
	
void Greenworks::ugcUnsubscribeWorkerComplete(uv_work_t *req) {
	// Get Unsubscribe object from worker
	Unsubscribe *us = (Unsubscribe*)req->data;

	// Prepare Callback argument object
	Handle<Value> errorCallbackVals[1];
	Handle<Value> successCallbackVals[1];

	Handle<Boolean> successCallbackArg = Boolean::New(true);
	Handle<String> errorCallbackArg = String::New("");

	errorCallbackVals[0] = errorCallbackArg;
	successCallbackVals[0] = successCallbackArg;

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	try {
		if (us->steamWs->IsRequesting()){
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Waiting for Steam Request to complete....");

			// Go for another thread-roundtrip and return afterwards
			uv_queue_work(uv_default_loop(), req, ugcUnsubscribeWorker, (uv_after_work_cb)ugcUnsubscribeWorkerComplete);
		}
		else {
			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Steam Request completed.");

			// Write to JS console if appliable
			CUtils::WriteToJsConsoleAndLogFile("Preparing data to return...");

			if (us->steamWs->SuccessDelegate->IsCallable()){
				successCallbackArg = Boolean::New(us->steamWs->Response);

				us->steamWs->SuccessDelegate->Call(Context::GetCurrent()->Global(), 1, successCallbackVals);
			}
		}
	}
	catch (Exception ex) {
		if (us->steamWs->ErrorDelegate->IsCallable()){
			errorCallbackArg = String::New("An error occured while trying to handle a Steam UGC User Request!\n\nSource: Greenworks::ugcGetUserItemsWorkerComplete");
			us->steamWs->ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
		}
	}


}

/* ********************************************************************************************************************************** */
/* PUBLIC MEMBERS */
/* ********************************************************************************************************************************** */
Greenworks::Greenworks() {}
Greenworks::~Greenworks() {}

Handle<Value> Greenworks::GetUtils(Local<String> property, const AccessorInfo& info) {
	if (_Utils.IsEmpty()){
		_Utils = Persistent<Value>::New(Greenutils::NewInstance());
	}
	return _Utils;
}

Handle<Value> Greenworks::initAPI(const Arguments& args) {
	HandleScope scope;

	// Initializing Steam API
	bool bSuccess = SteamAPI_Init();

	if (bSuccess) {
		ISteamUserStats *pSteamUserStats = SteamUserStats();
		pSteamUserStats->RequestCurrentStats();
	}

	return scope.Close(Boolean::New(bSuccess));
}
	
Handle<Value> Greenworks::enableCloud(const Arguments& args){
	HandleScope scope;

	bool bEnableCloud = args[0]->ToBoolean()->Value();
	ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

	pSteamRemoteStorage->SetCloudEnabledForApp(bEnableCloud);

	return scope.Close(Undefined());
}

Handle<Value> Greenworks::isCloudEnabled(const Arguments& args){
	HandleScope scope;
	ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();
	return scope.Close(Boolean::New(pSteamRemoteStorage->IsCloudEnabledForApp()));
}

Handle<Value> Greenworks::isCloudEnabledForUser(const Arguments& args){
	HandleScope scope;
	ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();
	return scope.Close(Boolean::New(pSteamRemoteStorage->IsCloudEnabledForAccount()));
}
	
Handle<Value> Greenworks::getCloudQuota(const Arguments& args) {
	HandleScope scope;
	ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

	Local<Function>successCallback = Local<Function>::Cast(args[0]);
	Local<Function>errorCallback = Local<Function>::Cast(args[1]);

	CloudQuota *quotaData = new CloudQuota;
	quotaData->successCallback = Persistent<Function>::New(successCallback);
	quotaData->errorCallback = Persistent<Function>::New(errorCallback);

	uv_work_t *req = new uv_work_t;
	req->data = quotaData;

	// Call separate thread work
	uv_queue_work(uv_default_loop(), req, steamGetCloudQuota, (uv_after_work_cb)gotQuota);

	return scope.Close(Undefined());
}

Handle<Value> Greenworks::saveTextToFile(const Arguments& args) {
	HandleScope scope;
	ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();

	// Convert passed string arguments directly to std::string
	string sFilename(*String::Utf8Value(args[0]));
	string sContent(*String::Utf8Value(args[1]));
	Local<Function>successCallback = Local<Function>::Cast(args[2]);
	Local<Function>errorCallback = Local<Function>::Cast(args[3]);

	FileIOAsync *writeData = new FileIOAsync;
	writeData->successCallback = Persistent<Function>::New(successCallback);
	writeData->errorCallback = Persistent<Function>::New(errorCallback);
	writeData->sFilename = sFilename;
	writeData->sContent = sContent;

	// Preparing separate thread work call
	uv_work_t *req = new uv_work_t;
	req->data = writeData;

	// Call separate thread work
	uv_queue_work(uv_default_loop(), req, steamWriteToFile, (uv_after_work_cb)fileWrote);

	return scope.Close(Undefined());
}

Handle<Value> Greenworks::readTextFromFile(const Arguments& args) {
	HandleScope scope;

	// Convert passed string arguments directly to std::string
	string sFilename(*String::Utf8Value(args[0]));
	Local<Function>successCallback = Local<Function>::Cast(args[1]);
	Local<Function>errorCallback = Local<Function>::Cast(args[2]);

	ISteamRemoteStorage *pSteamRemoteStorage = SteamRemoteStorage();
	FileIOAsync *readData = new FileIOAsync;
	readData->successCallback = Persistent<Function>::New(successCallback);
	readData->errorCallback = Persistent<Function>::New(errorCallback);
	readData->sFilename = sFilename;

	// Preparing separate thread work call
	uv_work_t *req = new uv_work_t;
	req->data = readData;

	// Call separate thread work
	uv_queue_work(uv_default_loop(), req, steamReadFromFile, (uv_after_work_cb)fileLoaded);

	return scope.Close(Undefined());
}

Handle<Value> Greenworks::activateAchievement(const Arguments& args) {
	HandleScope scope;

	string sAchievementId(*String::Utf8Value(args[0]));
	Local<Function>successCallback = Local<Function>::Cast(args[1]);
	Local<Function>errorCallback = Local<Function>::Cast(args[2]);

	Achievement *achievementData = new Achievement;
	achievementData->successCallback = Persistent<Function>::New(successCallback);
	achievementData->errorCallback = Persistent<Function>::New(errorCallback);
	achievementData->sAchievementId = sAchievementId;

	// Preparing separate thread work call
	uv_work_t *req = new uv_work_t;
	req->data = achievementData;

	// Call separate thread work
	uv_queue_work(uv_default_loop(), req, steamSetAchievement, (uv_after_work_cb)achievementStored);

	return scope.Close(Undefined());
}
	
Handle<Value> Greenworks::ugcPublish(const Arguments& args) {
	HandleScope scope;

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Steam Workshop Publish Arbitrary File");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Acquire function args (fileName, title, description, thumbnail location)
	string sFilename(*String::Utf8Value(args[0]));
	string sTitle(*String::Utf8Value(args[1]));
	string sDescription(*String::Utf8Value(args[2]));
	string sThumbnail(*String::Utf8Value(args[3]));

	// And cast/transform them to our destination types
	sFilename = CUtils::Escape(sFilename);
	sTitle = CUtils::Escape(sTitle);
	sDescription = CUtils::Escape(sDescription);
	sThumbnail = CUtils::Escape(sThumbnail);

	CUtils::WriteToJsConsoleAndLogFile("Preparing internal Workshop object");
	CUtils::WriteToJsConsoleAndLogFile("-----------------------------------------------------------");

	// Prepare Workshop object
	Workshop *ws = new Workshop;
	ws->steamWs = new CSteamWorkshop;
	ws->steamWs->SuccessDelegate = Persistent<Function>::New(Local<Function>::Cast(args[4]));
	ws->steamWs->ErrorDelegate = Persistent<Function>::New(Local<Function>::Cast(args[5]));
	ws->steamWs->ProgressDelegate = Persistent<Function>::New(Local<Function>::Cast(args[6]));
	ws->targetFile = sFilename;
	ws->title = sTitle;
	ws->description = sDescription;
	ws->targetImageFile = sThumbnail;
	ws->wsItems = Persistent<Array>::New(Array::New(0));
	ws->progress = 0;
	ws->reason = "";
	ws->fileBuf = new BYTE[0];
	ws->fileSize = 0;
	ws->imageFileBuf = new BYTE[0];
	ws->imageFileSize = 0;
	ws->bNeedsPublish = false;
	ws->bIsPublishing = false;
	ws->bRet = false;

	CUtils::WriteToJsConsoleAndLogFile("ws->targetFile = " + ws->targetFile);
	CUtils::WriteToJsConsoleAndLogFile("ws->title = " + ws->title);
	CUtils::WriteToJsConsoleAndLogFile("ws->targetImageFile = " + ws->targetImageFile);
	CUtils::WriteToJsConsoleAndLogFile("-----------------------------------------------------------");

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Object> progressCallbackArg = Object::New();

	progressCallbackVals[0] = progressCallbackArg;

	// Update / Set Callback Arg Object
	progressCallbackArg = GetProgressCallbackObject("steam_request_begin", ws->reason, ws->progress);

	// Call Progress Callback on JS
	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = ws;

	// We're ready to publish, start our background thread round-trip
	uv_queue_work(uv_default_loop(), req, ugcPublishWorker, (uv_after_work_cb)ugcPublishWorkerComplete);

	CUtils::WriteToJsConsoleAndLogFile("Request sent. Returning to caller.");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Signalize that the Steam request has ended
	// NOTE: This is NOT to be confused with steam_request_complete, as this refers only to the request sent
	//		 to Steam, which, in fact, ends at this point (which does imply that it is completed).
	// 
	progressCallbackArg = GetProgressCallbackObject("steam_request_end", ws->reason, ws->progress);

	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	return scope.Close(Undefined());

}
	
Handle<Value> Greenworks::ugcPublishUpdate(const Arguments& args) {
	HandleScope scope;

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Steam Workshop Update Published File");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Acquire function args (fileName, title, description, thumbnail location)
	string sId(*String::Utf8Value(args[0]));
	string sFilename(*String::Utf8Value(args[1]->IsString() ? args[1]->ToString() : String::New("")));
	string sTitle(*String::Utf8Value(args[2]->IsString() ? args[2]->ToString() : String::New("")));
	string sDescription(*String::Utf8Value(args[3]->IsString() ? args[3]->ToString() : String::New("")));
	string sThumbnail(*String::Utf8Value(args[4]->IsString() ? args[4]->ToString() : String::New("")));

	// And cast/transform them to our destination types
	sFilename = CUtils::Escape(sFilename);
	sTitle = CUtils::Escape(sTitle);
	sDescription = CUtils::Escape(sDescription);
	sThumbnail = CUtils::Escape(sThumbnail);

	uint64 lId = CUtils::ToUInt64(sId.c_str());

	CUtils::WriteToJsConsoleAndLogFile("Preparing internal Workshop object");
	CUtils::WriteToJsConsoleAndLogFile("-----------------------------------------------------------");

	// Prepare Workshop object
	Workshop *ws = new Workshop;
	ws->steamWs = new CSteamWorkshop;
	ws->steamWs->SuccessDelegate = Persistent<Function>::New(Local<Function>::Cast(args[5]));
	ws->steamWs->ErrorDelegate = Persistent<Function>::New(Local<Function>::Cast(args[6]));
	ws->steamWs->ProgressDelegate = Persistent<Function>::New(Local<Function>::Cast(args[7]));
	ws->targetFile = sFilename;
	ws->title = sTitle;
	ws->description = sDescription;
	ws->targetImageFile = sThumbnail;
	ws->wsItems = Persistent<Array>::New(Array::New(0));
	ws->progress = 0;
	ws->reason = "";
	ws->fileBuf = new BYTE[0];
	ws->fileSize = 0;
	ws->imageFileBuf = new BYTE[0];
	ws->imageFileSize = 0;
	ws->bNeedsPublish = false;
	ws->bIsPublishing = false;
	ws->publishedFileId = lId;
	ws->bRet = false;

	CUtils::WriteToJsConsoleAndLogFile("ws->targetFile = " + ws->targetFile);
	CUtils::WriteToJsConsoleAndLogFile("ws->title = " + ws->title);
	CUtils::WriteToJsConsoleAndLogFile("ws->targetImageFile = " + ws->targetImageFile);
	CUtils::WriteToJsConsoleAndLogFile("ws->publishedFileId = " + to_string(ws->publishedFileId));
	CUtils::WriteToJsConsoleAndLogFile("-----------------------------------------------------------");

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Object> progressCallbackArg = Object::New();

	progressCallbackVals[0] = progressCallbackArg;

	// Update / Set Callback Arg Object
	progressCallbackArg = GetProgressCallbackObject("steam_request_begin", ws->reason, ws->progress);

	// Call Progress Callback on JS
	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = ws;

	// We're ready to publish, start our background thread round-trip
	uv_queue_work(uv_default_loop(), req, ugcPublishUpdateWorker, (uv_after_work_cb)ugcPublishUpdateWorkerComplete);

	CUtils::WriteToJsConsoleAndLogFile("Request sent. Returning to caller.");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Signalize that the Steam request has ended
	// NOTE: This is NOT to be confused with steam_request_complete, as this refers only to the request sent
	//		 to Steam, which, in fact, ends at this point (which does imply that it is completed).
	// 
	progressCallbackArg = GetProgressCallbackObject("steam_request_end", ws->reason, ws->progress);

	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	return scope.Close(Undefined());

}
	
Handle<Value> Greenworks::ugcGetItems(const Arguments& args){
	HandleScope scope;

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Steam UGC Query Request");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Acquire function args (iType, iSort, iFilter)
	String::Utf8Value param1(args[0]->ToString());
	String::Utf8Value param2(args[1]->ToString());

	// And cast/transform them to our destination type
	string sType = string(*param1);
	string sSort = string(*param2);

	int iType = CUtils::ToInt(sType.c_str());
	int iSort = CUtils::ToInt(sSort.c_str());

	// Prepare Workshop object
	Workshop *ws = new Workshop;
	ws->bRet = false;
	ws->steamWs = new CSteamWorkshop;
	ws->steamWs->SuccessDelegate = Persistent<Function>::New(Local<Function>::Cast(args[2]));
	ws->steamWs->ErrorDelegate = Persistent<Function>::New(Local<Function>::Cast(args[3]));
	ws->steamWs->ProgressDelegate = Persistent<Function>::New(Local<Function>::Cast(args[4]));

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Object> progressCallbackArg = Object::New();


	// Update / Set Callback Arg Object
	progressCallbackArg = GetProgressCallbackObject("steam_request_begin", ws->reason, ws->progress);
	progressCallbackVals[0] = progressCallbackArg;

	// Call Progress Callback on JS
	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	// Send a User-UGC request to Steam using specified args
	ws->steamWs->RequestUGC(iType, iSort);

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = ws;

	// Call background worker thread 
	uv_queue_work(uv_default_loop(), req, ugcGetItemsWorker, (uv_after_work_cb)ugcGetItemsWorkerComplete);

	CUtils::WriteToJsConsoleAndLogFile("Request sent. Returning to caller.");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Signalize that the Steam request has ended
	// NOTE: This is NOT to be confused with steam_request_complete, as this refers only to the request sent
	//		 to Steam, which, in fact, ends at this point (which does imply that it is completed).
	// 
	progressCallbackArg = GetProgressCallbackObject("steam_request_end", ws->reason, ws->progress);

	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	return scope.Close(Undefined());
}
	
Handle<Value> Greenworks::ugcGetUserItems(const Arguments& args){
	HandleScope scope;

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Steam UGC Query Request For User");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");
		
	// Acquire function args (iType, iSort, iFilter)
	String::Utf8Value param1(args[0]->ToString());
	String::Utf8Value param2(args[1]->ToString());
	String::Utf8Value param3(args[2]->ToString());

	// And cast/transform them to our destination type
	string sType = string(*param1);
	string sSort = string(*param2);
	string sFilter = string(*param3);

	int iType = CUtils::ToInt(sType.c_str());
	int iSort = CUtils::ToInt(sSort.c_str());
	int iFilter = CUtils::ToInt(sFilter.c_str());

	// Prepare Workshop object
	Workshop *ws = new Workshop;
	ws->bRet = false;
	ws->steamWs = new CSteamWorkshop;
	ws->steamWs->SuccessDelegate = Persistent<Function>::New(Local<Function>::Cast(args[3]));
	ws->steamWs->ErrorDelegate = Persistent<Function>::New(Local<Function>::Cast(args[4]));
	ws->steamWs->ProgressDelegate = Persistent<Function>::New(Local<Function>::Cast(args[5]));

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Object> progressCallbackArg = Object::New();


	// Update / Set Callback Arg Object
	progressCallbackArg = GetProgressCallbackObject("steam_request_begin", ws->reason, ws->progress);
	progressCallbackVals[0] = progressCallbackArg;

	// Call Progress Callback on JS
	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	// Send a User-UGC request to Steam using specified args
	ws->steamWs->RequestUserUGC(iType, iSort, iFilter);

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = ws;

	// Call background worker thread 
	uv_queue_work(uv_default_loop(), req, ugcGetUserItemsWorker, (uv_after_work_cb)ugcGetUserItemsWorkerComplete);

	CUtils::WriteToJsConsoleAndLogFile("Request sent. Returning to caller.");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Signalize that the Steam request has ended
	// NOTE: This is NOT to be confused with steam_request_complete, as this refers only to the request sent
	//		 to Steam, which, in fact, ends at this point (which does imply that it is completed).
	// 
	progressCallbackArg = GetProgressCallbackObject("steam_request_end", ws->reason, ws->progress);

	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	return scope.Close(Undefined());
}
	
Handle<Value> Greenworks::ugcDownloadItem(const Arguments& args){
	HandleScope scope;

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Steam UGC Download Request");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Acquire function args (sFileName, hFile)
	String::Utf8Value param1(args[0]->ToString());
	String::Utf8Value param2(args[1]->ToString());
	String::Utf8Value param3(args[2]->ToString());

	string sFileName = string(*param1);
	string sFileHandle = string(*param2);
	string sTargetFolder = string(*param3);
		
	uint64 iFileHandle = CUtils::ToUInt64(sFileHandle.c_str());

	// Prepare Workshop object
	Workshop *ws = new Workshop;
	ws->bRet = false;
	ws->steamWs = new CSteamWorkshop;
	ws->steamWs->SuccessDelegate = Persistent<Function>::New(Local<Function>::Cast(args[3]));
	ws->steamWs->ErrorDelegate = Persistent<Function>::New(Local<Function>::Cast(args[4]));
	ws->steamWs->ProgressDelegate = Persistent<Function>::New(Local<Function>::Cast(args[5]));
	ws->targetFolder = sTargetFolder;

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Requesting the file: " + sFileName);

	// Send download request
	ws->steamWs->Download(iFileHandle, ws->targetFolder);

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = ws;

	// Call background worker thread 
	uv_queue_work(uv_default_loop(), req, ugcDownloadItemWorker, (uv_after_work_cb)ugcDownloadItemWorkerComplete);

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Request sent. Returning to caller.");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	return scope.Close(Undefined());
}
	
Handle<Value> Greenworks::ugcSynchronizeItems(const Arguments& args) {
	HandleScope scope;

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Steam UGC Synchronize Request");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Acquire function args (targetFolder)
	String::Utf8Value param1(args[0]->ToString());

	// And cast/transform them to our destination type
	string sTargetFolder = string(*param1);
		
	// Set UGC User Request Params
	int iType = 0;
	int iSort = 6;
	int iFilter = 3;

	// Prepare Workshop object
	Workshop *ws = new Workshop;
	ws->bRet = false;
	ws->steamWs = new CSteamWorkshop;
	ws->steamWs->SuccessDelegate = Persistent<Function>::New(Local<Function>::Cast(args[1]));
	ws->steamWs->ErrorDelegate = Persistent<Function>::New(Local<Function>::Cast(args[2]));
	ws->steamWs->ProgressDelegate = Persistent<Function>::New(Local<Function>::Cast(args[3]));
	ws->targetFolder = CUtils::Escape(sTargetFolder);
	ws->fileIndex = -1;
	ws->wsItems = Persistent<Array>::New(Array::New(0));
	ws->progress = 0;
	ws->reason = "";

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Object> progressCallbackArg = Object::New();

	progressCallbackVals[0] = progressCallbackArg;

	// Update / Set Callback Arg Object
	progressCallbackArg = GetProgressCallbackObject("steam_request_begin", ws->reason, ws->progress);

	// Call Progress Callback on JS
	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	// At first, send a User-UGC request to Steam using specified args (will handle download from respective thread methods)
	ws->steamWs->RequestUserUGC(iType, iSort, iFilter);

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = ws;

	// Call background worker thread 
	uv_queue_work(uv_default_loop(), req, ugcSynchronizeItemsWorker, (uv_after_work_cb)ugcSynchronizeItemsWorkerComplete);

	CUtils::WriteToJsConsoleAndLogFile("Request sent. Returning to caller.");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Signalize that the Steam request has ended
	// NOTE: This is NOT to be confused with steam_request_complete, as this refers only to the request sent
	//		 to Steam, which, in fact, ends at this point (which does imply that it is completed).
	// 
	progressCallbackArg = GetProgressCallbackObject("steam_request_end", ws->reason, ws->progress);

	if (ws->steamWs->ProgressDelegate->IsCallable()){
		ws->steamWs->ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
	}

	return scope.Close(Undefined());
}
	
Handle<Value> Greenworks::ugcShowOverlay(const Arguments& args){
	HandleScope scope;

	CUtils::WriteToJsConsoleAndLogFile("Steam UGC Overlay");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// http://steamcommunity.com/groups/steamworks/discussions/0/864956554953618065/
	
	string url = "";

	// Check if there's the optional argument "id" that is the workshop item id
	if (args.Length() > 0)
	{
		String::Utf8Value param1(args[0]->ToString());
		string itemId = string(*param1);

		url = "http://steamcommunity.com/sharedfiles/filedetails/?id=" + itemId;

		CUtils::WriteToJsConsoleAndLogFile("Steam Workshop Overlay for Workshop Item ID (" + itemId + ") request sent.");

	}
	else {
		int appId = SteamUtils()->GetAppID();
		url = "http://steamcommunity.com/app/" + to_string(appId) + "/workshop/";

		CUtils::WriteToJsConsoleAndLogFile("Steam Workshop Overlay for APP_ID(" + to_string(appId) + ") request sent.");

	}

	SteamFriends()->ActivateGameOverlayToWebPage(url.c_str());

	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	return scope.Close(Undefined());
}

Handle<Value> Greenworks::ugcUnsubscribe(const Arguments& args){
	HandleScope scope;

	// Write to JS console if appliable
	CUtils::WriteToJsConsoleAndLogFile("Steam Workshop Unsubscribe File");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// Acquire function args (publishedFileId)
	string sPublishedFileId(*String::Utf8Value(args[0]));

	// Cast to proper format
	PublishedFileId_t iPublishedFileId = CUtils::ToUInt64(sPublishedFileId.c_str());


	CUtils::WriteToJsConsoleAndLogFile("Preparing internal Unsubscribe object");

	// Prepare Workshop object
	Unsubscribe *us = new Unsubscribe;
	us->steamWs = new CSteamWorkshop;
	us->steamWs->SuccessDelegate = Persistent<Function>::New(Local<Function>::Cast(args[1]));
	us->steamWs->ErrorDelegate = Persistent<Function>::New(Local<Function>::Cast(args[2]));
	us->steamWs->ProgressDelegate = Persistent<Function>::New(Local<Function>::Cast(args[3]));
	us->bRet = false;
	us->bSuccess = false;

	// Prepare Callback argument object
	Handle<Value> progressCallbackVals[1];
	Handle<Object> progressCallbackArg = Object::New();

	progressCallbackVals[0] = progressCallbackArg;

	// Let STEAM run its callbacks
	SteamAPI_RunCallbacks();

	// Prepare background worker thread
	uv_work_t *req = new uv_work_t;
	req->data = us;

	// We're ready to publish, start our background thread round-trip
	uv_queue_work(uv_default_loop(), req, ugcUnsubscribeWorker, (uv_after_work_cb)ugcUnsubscribeWorkerComplete);

	CUtils::WriteToJsConsoleAndLogFile("Request sent. Returning to caller.");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	return scope.Close(Undefined());
}

Handle<Value> Greenworks::getSteamId(const Arguments& args){
	HandleScope scope;

	CUtils::WriteToJsConsoleAndLogFile("Steam Acquire User Steam ID Information");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	int appId = SteamUtils()->GetAppID();
	CSteamID steamUser = SteamUser()->GetSteamID();

	Handle<Object> resFlags = Object::New();
	Handle<Object> resMain = Object::New();

	resFlags->Set(String::New("anonymous"), Boolean::New(steamUser.BAnonAccount()));
	resFlags->Set(String::New("anonymousGameServer"), Boolean::New(steamUser.BAnonGameServerAccount()));
	resFlags->Set(String::New("anonymousGameServerLogin"), Boolean::New(steamUser.BBlankAnonAccount()));
	resFlags->Set(String::New("anonymousUser"), Boolean::New(steamUser.BAnonUserAccount()));
	resFlags->Set(String::New("chat"), Boolean::New(steamUser.BChatAccount()));
	resFlags->Set(String::New("clan"), Boolean::New(steamUser.BClanAccount()));
	resFlags->Set(String::New("consoleUser"), Boolean::New(steamUser.BConsoleUserAccount()));
	resFlags->Set(String::New("contentServer"), Boolean::New(steamUser.BContentServerAccount()));
	resFlags->Set(String::New("gameServer"), Boolean::New(steamUser.BGameServerAccount()));
	resFlags->Set(String::New("individual"), Boolean::New(steamUser.BIndividualAccount()));
	resFlags->Set(String::New("gameServerPersistent"), Boolean::New(steamUser.BPersistentGameServerAccount()));
	resFlags->Set(String::New("lobby"), Boolean::New(steamUser.IsLobby()));

	resMain->Set(String::New("flags"), resFlags);
	resMain->Set(String::New("type"), GetObjectFromSteamAccountType(steamUser.GetEAccountType()));

	resMain->Set(String::New("accountId"), Integer::New((uint64)steamUser.GetAccountID()));
	resMain->Set(String::New("staticAccountId"), Integer::New(steamUser.GetStaticAccountKey()));
	resMain->Set(String::New("isValid"), Boolean::New(steamUser.IsValid()));

	if (!SteamFriends()->RequestUserInformation(steamUser, true))
	{
		resMain->Set(String::New("screenName"), String::New(SteamFriends()->GetFriendPersonaName(steamUser)));
	}
	else {
		resMain->Set(String::New("screenName"), String::New(to_string((uint64)steamUser.GetAccountID()).c_str()));
	}
		
	resMain->Set(String::New("level"), Integer::New(SteamUser()->GetPlayerSteamLevel()));

	CUtils::WriteToJsConsoleAndLogFile("Steam User Steam ID Information gathering done.");
	CUtils::WriteToJsConsoleAndLogFile("Returning result to JS context (caller).");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	return scope.Close(resMain);
}
		
Handle<Value> Greenworks::getCurrentGameInstallDir(const Arguments& args){
	HandleScope scope;

	CUtils::WriteToJsConsoleAndLogFile("Steam Game Install Directory Query");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	string sDir = "todo...";
	Handle<String> dir = String::New(sDir.c_str());

	CUtils::WriteToJsConsoleAndLogFile("Install Directory detected (Current Game InstallDir): " + sDir);
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	return scope.Close(dir);
}
	
Handle<Value> Greenworks::getCurrentGameLanguage(const Arguments& args){
	HandleScope scope;

	CUtils::WriteToJsConsoleAndLogFile("Steam Game Language Query");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// http://steamcommunity.com/groups/steamworks/discussions/0/864956554953618065/

	string sLang = SteamApps()->GetCurrentGameLanguage();

	Handle<String> lang = String::New(sLang.c_str());

	CUtils::WriteToJsConsoleAndLogFile("Language detected (Current Game Language): " + sLang);
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	return scope.Close(lang);
}

Handle<Value> Greenworks::getCurrentUILanguage(const Arguments& args){
	HandleScope scope;

	CUtils::WriteToJsConsoleAndLogFile("Steam Game UI Language Query");
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	// http://steamcommunity.com/groups/steamworks/discussions/0/864956554953618065/

	string sLang = SteamUtils()->GetSteamUILanguage();

	Handle<String> lang = String::New(sLang.c_str());

	CUtils::WriteToJsConsoleAndLogFile("UI Language detected (Current UI Language): " + sLang);
	CUtils::WriteToJsConsoleAndLogFile("***************************************************************************");

	return scope.Close(lang);
}

Handle<Value> Greenworks::getNumberOfPlayers(const Arguments& args){
	HandleScope scope;
	return scope.Close(Integer::New(SteamUserStats()->GetNumberOfCurrentPlayers()));
}

Handle<Value> Greenworks::runCallbacks(const Arguments& args){
	HandleScope scope;
	SteamAPI_RunCallbacks();
	return scope.Close(Undefined());
}


