
#include "CSteamWorkshop.h"

CSteamWorkshop::CSteamWorkshop() : 
m_iAppID(0), 
m_title("Your title here"),
m_description("Your description here"),
m_fileName(""),
m_imageFileName(""),
m_targetDirForDownload(""),
HasChanged(false),
Response(false)
{
	Requesting = false; 
	// m_wsItems =  new vector<SteamUGCDetails_t>();

	m_iNumWorkshopItems = 0;
	m_iNumWorkshopItemsProcessed = 0;
	m_iAppID = SteamUtils()->GetAppID();
}

CSteamWorkshopItem CSteamWorkshop::GetItemByFileName(string fileName){
	if (m_wsItems.size() > 0){
		int siz = m_wsItems.size();
		for (int i = 0; i < siz; i++){
			if (m_wsItems[i].Details->m_pchFileName == fileName){
				return m_wsItems[i];
			}
		}
	}
	return CSteamWorkshopItem();
}

int CSteamWorkshop::GetItemIndexByFileName(string fileName){
	if (m_wsItems.size() > 0){
		int siz = m_wsItems.size();
		for (int i = 0; i < siz; i++){
			if (m_wsItems[i].Details->m_pchFileName == fileName){
				return i;
			}
		}
	}
	return -1;
}

bool CSteamWorkshop::RequestUGC(int type, int sort)
{
	Requesting = true;

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestUGC >> Call (Type = " + to_string(type) + ", Sort = " + to_string(sort) + ")");

	m_Handle = SteamUGC()->CreateQueryAllUGCRequest(
		(EUGCQuery) sort,
		(EUGCMatchingUGCType) type,
		m_iAppID,
		m_iAppID,
		1);

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestUGC >> CreateQueryAllUGCRequest = " + to_string(m_Handle));

	// m_Handle = SteamUGC()->CreateQueryUserUGCRequest(SteamUser()->GetSteamID().GetAccountID(), k_EUserUGCList_Published, k_EUGCMatchingUGCType_Items, k_EUserUGCListSortOrder_LastUpdatedDesc, 0, SteamUtils()->GetAppID(), 1);

	//std::string result;
	//char numstr[21]; // enough to hold all numbers up to 64-bits
	//sprintf(numstr, "console.log('%d');", m_Handle);

	// Request ugc
	SteamAPICall_t hSteamAPICall = SteamUGC()->SendQueryUGCRequest(m_Handle);
	m_CallResultOnUGCQueryCompleted.Set(hSteamAPICall, this, &CSteamWorkshop::OnUGCQueryCompleted);

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestUGC >> SendQueryUGCRequest = " + to_string(hSteamAPICall));

	return true;
}

bool CSteamWorkshop::RequestUserUGC(int type, int sort, int filter)
{
	Requesting = true;

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestUserUGC >> Call (Type = " + to_string(type) + ", Sort = " + to_string(sort) + ", Filter = " + to_string(filter) + ")");


	m_Handle = SteamUGC()->CreateQueryUserUGCRequest(
		SteamUser()->GetSteamID().GetAccountID(),
		(EUserUGCList)sort, //k_EUserUGCList_Subscribed
		(EUGCMatchingUGCType) type, // k_EUGCMatchingUGCType_Items,
		(EUserUGCListSortOrder)filter, // k_EUserUGCListSortOrder_SubscriptionDateDesc,
		SteamUtils()->GetAppID(),
		SteamUtils()->GetAppID(),
		1);

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestUserUGC >> Handle = " + to_string(m_Handle));

	/*
	Handle<Value> callbackResultArgs[2];
	callbackResultArgs[0] = v8::String::New("value1");
	callbackResultArgs[1] = v8::Integer::New(0);

	if (ProgressDelegate->IsCallable()){
		ProgressDelegate->Call(Context::GetCurrent()->Global(), 2, callbackResultArgs);
	}
	*/

	// Request ugc
	SteamAPICall_t hSteamAPICall = SteamUGC()->SendQueryUGCRequest(m_Handle);
	m_CallResultOnUGCUserQueryCompleted.Set(hSteamAPICall, this, &CSteamWorkshop::OnUGCUserQueryCompleted);

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestUserUGC >> Query Handle = " + to_string(hSteamAPICall));

	SteamAPI_RunCallbacks();

	return true;
}

bool CSteamWorkshop::RequestAndSynchronizeUserUGC(string targetDir)
{
	Requesting = true;

	// CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestAndSynchronizeUserUGC >> Call");

	m_targetDirForDownload = targetDir;
	m_iNumWorkshopItems = 0;
	m_iNumWorkshopItemsProcessed = 0;

	m_Handle = SteamUGC()->CreateQueryUserUGCRequest(
		SteamUser()->GetSteamID().GetAccountID(),
		(EUserUGCList)6, //k_EUserUGCList_Subscribed
		(EUGCMatchingUGCType)0, // k_EUGCMatchingUGCType_Items,
		(EUserUGCListSortOrder)3, // k_EUserUGCListSortOrder_SubscriptionDateDesc,
		SteamUtils()->GetAppID(),
		SteamUtils()->GetAppID(),
		1);

	// CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestAndSynchronizeUserUGC >> Handle = " + to_string(m_Handle));

	// Request ugc
	SteamAPICall_t hSteamAPICall = SteamUGC()->SendQueryUGCRequest(m_Handle);
	m_CallResultOnUGCQueryAndSyncCompleted.Set(hSteamAPICall, this, &CSteamWorkshop::OnUGCQueryAndSyncCompleted);

	// CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::RequestAndSynchronizeUserUGC >> Query Handle = " + to_string(hSteamAPICall));

	return true;
}

void CSteamWorkshop::PublishUpdate(string fileName, string title, string description, void *content, int contentLength, string imageFileName, void *contentImage, int contentImageLength, uint64 publishedFileId){
	// To publish update we need to:
	// 1) Write file
	// 2) Share file
	// 3) GetPublishedFileDetails
	// 4) CreatePublishedFileUpdateRequest
	// 5) UpdatePublishedFile(s)
	// 6) CommitPublishedFileUpdate

	Requesting = true;

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> Call");

	m_fileName = !fileName.empty() ? CUtils::GetFilename(fileName) : "";
	m_title = title;
	m_description = description;
	m_imageFileName = !imageFileName.empty() ? CUtils::GetFilename(imageFileName) : "";
	m_publishedFileId = publishedFileId;

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> Updating and publishing file " + m_fileName + " (" + title + ") id: " + to_string(m_publishedFileId));

	if (!m_imageFileName.empty()){
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> Updating and publishing image file " + m_imageFileName);

		// 1a) Write image file
		if (SteamRemoteStorage()->FileWrite(m_imageFileName.c_str(), contentImage, contentImageLength)){
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> Image FileWrite successful.");
		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> Image FileWrite failed... continuing trying to write main file");
		}

	}
	else{
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> No Image File to write specified.");
	}

	// 1b) Write file
	if (!m_fileName.empty()){
		if (SteamRemoteStorage()->FileWrite(m_fileName.c_str(), content, contentLength)){

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> FileWrite successful. Calling FileShare...");

			// 2) Share file upon successful file write operation
			SteamAPICall_t shareresult = SteamRemoteStorage()->FileShare(m_fileName.c_str());
			m_CallResultOnFileShareUpdateCompleted.Set(shareresult, this, &CSteamWorkshop::OnFileShareUpdateCompleted);

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> FileShare handle is " + to_string(shareresult));
		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> FileWrite failed");
			Requesting = false;
		}
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> No File to write specified.");
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::PublishUpdate >> Proceeding to Call CreatePublishedFileUpdateRequest");

		SteamAPICall_t filedetailsresult = SteamRemoteStorage()->GetPublishedFileDetails(m_publishedFileId, 0);
		m_CallResultOnGetPublishedFileDetailsCompleted.Set(filedetailsresult, this, &CSteamWorkshop::OnGetPublishedFileDetailsCompleted);
	}
	
	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
}

void CSteamWorkshop::Publish(string fileName, string title, string description, void *content, int contentLength, string imageFileName, void *contentImage, int contentImageLength){
	// To publish we need to:
	// 1) Write file
	// 2) Share file
	// 3) Publish file

	Requesting = true;
	m_publishedFileId = 0;

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> Call");

	m_fileName = !fileName.empty() ? CUtils::GetFilename(fileName) : "";
	m_title = title;
	m_description = description;
	m_imageFileName = !imageFileName.empty() ? CUtils::GetFilename(imageFileName) : "";

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> Publishing file " + m_fileName + " (" + title + ")");

	if (m_imageFileName.length() > 0){
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> Publishing image file " + m_imageFileName);

		// 1a) Write image file
		if (SteamRemoteStorage()->FileWrite(m_imageFileName.c_str(), contentImage, contentImageLength)){
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> Image FileWrite successful.");
		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> Image FileWrite failed... continuing trying to write main file");
		}

	}
	else{
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> No Image File to write specified.");
	}


	// 1b) Write file
	if (m_fileName.length() > 0){
		if (SteamRemoteStorage()->FileWrite(m_fileName.c_str(), content, contentLength)){

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> FileWrite successful. Calling FileShare...");

			// 2) Share file upon successful file write operation
			SteamAPICall_t shareresult = SteamRemoteStorage()->FileShare(m_fileName.c_str());
			m_CallResultOnFileShared.Set(shareresult, this, &CSteamWorkshop::OnFileShareCompleted);

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> FileShare handle is " + to_string(shareresult));

		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> FileWrite failed");
			Requesting = false;
		}
	}
	else{
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Publish >> No File to write specified.");
		Requesting = false;
	}

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

}

void CSteamWorkshop::Download(UGCHandle_t hFile, string targetDir){
	Requesting = true;
	HasChanged = false;

	m_targetDirForDownload = targetDir;

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Download >> " + to_string(hFile) + " " + m_targetDirForDownload);

	/*
	Handle<Value> callbackResultArgs[1]; 
	Handle<Object> callbackObj = Object::New();
	callbackObj->Set(String::New("targetDir"), String::New(targetDir.c_str()));
	callbackObj->Set(String::New("hFile"), String::New(to_string(hFile).c_str()));
	
	callbackResultArgs[0] = callbackObj;

	ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
	*/

	SteamAPICall_t hSteamAPICall = SteamRemoteStorage()->UGCDownload(hFile, 0);
	m_CallResultOnFileDownloaded.Set(hSteamAPICall, this, &CSteamWorkshop::OnDownloadCompleted);
	
	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Download Call Handle = " + to_string(hSteamAPICall));
}

void CSteamWorkshop::DownloadFromUGCSynchronize(UGCHandle_t hFile){
	Requesting = true;
	HasChanged = false;


	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::DownloadFromUGCSynchronize >> " + to_string(hFile) + " " + m_targetDirForDownload);

	/*
	Handle<Value> callbackResultArgs[1];
	Handle<Object> callbackObj = Object::New();
	callbackObj->Set(String::New("targetDir"), String::New(m_targetDirForDownload.c_str()));
	callbackObj->Set(String::New("hFile"), String::New(to_string(hFile).c_str()));

	callbackResultArgs[0] = callbackObj;

	ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);
	*/

	SteamAPICall_t hSteamAPICall = SteamRemoteStorage()->UGCDownload(hFile, 0);
	m_CallResultOnFileFromUGCSyncDownloaded.Set(hSteamAPICall, this, &CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted);

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::DownloadFromUGCSynchronize Call Handle = " + to_string(hSteamAPICall));
}

void CSteamWorkshop::Unsubscribe(uint64 publishedFileId){
	Requesting = true;
	m_publishedFileId = 0;

	m_publishedFileId = publishedFileId;

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Unsubscribe >> Trying to unsbscribe UGC item with id " + to_string(m_publishedFileId));
	
	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Unsubscribe >> Calling Unsubscribe...");

	// Try to unsubscribe file
	SteamAPICall_t unsubscriberesult = SteamRemoteStorage()->UnsubscribePublishedFile(m_publishedFileId);
	m_CallResultOnUnsubscribeCompleted.Set(unsubscriberesult, this, &CSteamWorkshop::OnUnsubscribeCompleted);

	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::Unsubscribe >> Result handle is " + to_string(unsubscriberesult));

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

}


void CSteamWorkshop::OnFileShareCompleted(RemoteStorageFileShareResult_t *pCallback, bool bIOFailure)
{
	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnFileShareCompleted >> File Share Operation completed for " + m_fileName);

	if (pCallback->m_eResult != k_EResultOK){
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnFileShareCompleted >> Could not share file. PublishWorkshopFile will not be called!");
		Requesting = false;
	}
	else {
		SteamParamStringArray_t tags_;
		tags_.m_nNumStrings = 0;
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnFileShareCompleted >> Calling PublishWorkshopFile");
		SteamAPICall_t publishresult = SteamRemoteStorage()->PublishWorkshopFile(m_fileName.c_str(), m_imageFileName.empty() ? NULL : m_imageFileName.c_str(), m_iAppID, m_title.c_str(), m_description.empty() ? NULL : m_description.c_str(), k_ERemoteStoragePublishedFileVisibilityPublic, &tags_, k_EWorkshopFileTypeCommunity);

		m_CallResultOnFilePublished.Set(publishresult, this, &CSteamWorkshop::OnFilePublishCompleted);
	}


	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

}

void CSteamWorkshop::OnFileShareUpdateCompleted(RemoteStorageFileShareResult_t *pCallback, bool bIOFailure)
{
	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
	CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnFileShareUpdateCompleted >> File Share Operation completed for " + m_fileName);

	// Check if it's a publish update call or if we add a new item
	if (pCallback->m_eResult != k_EResultOK){
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnFileShareUpdateCompleted (PublishUpdate) >> Could not share file. CreatePublishedFileUpdateRequest will not be called!");
		Requesting = false;
	}
	else {
		SteamParamStringArray_t tags_;
		tags_.m_nNumStrings = 0;
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnFileShareUpdateCompleted (PublishUpdate) >> Calling CreatePublishedFileUpdateRequest");

		SteamAPICall_t filedetailsresult = SteamRemoteStorage()->GetPublishedFileDetails(m_publishedFileId, 0);
		m_CallResultOnGetPublishedFileDetailsCompleted.Set(filedetailsresult, this, &CSteamWorkshop::OnGetPublishedFileDetailsCompleted);
	}


	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

}

void CSteamWorkshop::OnFilePublishCompleted(RemoteStoragePublishFileResult_t *pCallback, bool bIOFailure)
{
	if (pCallback->m_eResult != k_EResultOK){
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnFilePublishCompleted >> File Publish not possible (failed).");
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnFilePublishCompleted >> File Publish Operation completed.");
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

		m_publishedFileId = pCallback->m_nPublishedFileId;
	}
	Requesting = false;
}

void CSteamWorkshop::OnUGCQueryCompleted(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure)
{
	std::cerr << "------------------------------------------------------------------------\n";

	// we may get callbacks for other games' stats arriving, ignore them
	if (k_EResultOK == pCallback->m_eResult)
	{
		m_bInitialized = true;

		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCQueryCompleted = Receiving stats and achievements from Steam... :-)");


		ISteamUGC *steamUGC = SteamUGC();
		
		m_wsItems.clear();

		if (pCallback->m_unNumResultsReturned > 0){

			// load ugcs
			for (int iUGC = 0; iUGC < pCallback->m_unNumResultsReturned; ++iUGC)
			{
				SteamUGCDetails_t *pDetails = new SteamUGCDetails_t;
				SteamUGC()->GetQueryUGCResult(m_Handle, iUGC, pDetails);

				CSteamWorkshopItem item = CSteamWorkshopItem();
				item.Details = pDetails;

				m_wsItems.push_back(item);


				//m_wsItems->Set(iUGC, result);

				// pDetails->m_bAcceptedForUse;
				// pDetails->m_bBanned;
				// pDetails->m_bTagsTruncated;
				// pDetails->m_eFileType;
				// pDetails->m_eResult;
				// pDetails->m_eVisibility;
				// pDetails->m_flScore;
				// pDetails->m_hFile;
				// pDetails->m_hPreviewFile;
				// pDetails->m_nConsumerAppID;
				// pDetails->m_nCreatorAppID;
				// pDetails->m_nFileSize;
				// pDetails->m_nPreviewFileSize;
				// pDetails->m_nPublishedFileId;
				// pDetails->m_pchFileName;
				// pDetails->m_rgchDescription;
				// pDetails->m_rgchTags;
				// pDetails->m_rgchTitle;
				// pDetails->m_rgchURL;
				// pDetails->m_rtimeAddedToUserList;
				// pDetails->m_rtimeCreated;
				// pDetails->m_rtimeUpdated;
				// pDetails->m_ulSteamIDOwner;
				// pDetails->m_unVotesDown;
				// pDetails->m_unVotesUp;


			}
		}

		m_iNumWorkshopItems = pCallback->m_unNumResultsReturned;

		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCQueryCompleted = Workshop items found. " + to_string(m_iNumWorkshopItems) + " items :-)");
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCQueryCompleted = No Workshop items found matching criterias");
		m_iNumWorkshopItems = 0;
	}

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

	SteamUGC()->ReleaseQueryUGCRequest(m_Handle);
	Requesting = false;
}

void CSteamWorkshop::OnUGCUserQueryCompleted(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure)
{
	int perc = 1;

	// Prepare Callback argument object
	Handle<Value> errorCallbackVals[1];
	Handle<String> errorCallbackArg = String::New("");
	errorCallbackVals[0] = errorCallbackArg;

	/*
	Handle<Value> progressCallbackVals[1];
	Handle<Object> progressCallbackArg = Object::New();

	progressCallbackVals[0] = progressCallbackArg;

	// Update / Set Callback Arg Object
	progressCallbackArg->Set(String::New("status"), String::New("steam_request_running"));
	progressCallbackArg->Set(String::New("value"), Integer::New(perc));
	*/

	try {
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

		// we may get callbacks for other games' stats arriving, ignore them
		if (k_EResultOK == pCallback->m_eResult)
		{
			m_bInitialized = true;

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCUserQueryCompleted = Receiving data from Steam... :-)");


			ISteamUGC *steamUGC = SteamUGC();

			m_wsItems.clear();


			if (pCallback->m_unNumResultsReturned > 0){
				int percStep = 100 / pCallback->m_unNumResultsReturned;

				// load ugcs
				for (int iUGC = 0; iUGC < pCallback->m_unNumResultsReturned; ++iUGC)
				{
					// pDetails->m_bAcceptedForUse;
					// pDetails->m_bBanned;
					// pDetails->m_bTagsTruncated;
					// pDetails->m_eFileType;
					// pDetails->m_eResult;
					// pDetails->m_eVisibility;
					// pDetails->m_flScore;
					// pDetails->m_hFile;
					// pDetails->m_hPreviewFile;
					// pDetails->m_nConsumerAppID;
					// pDetails->m_nCreatorAppID;
					// pDetails->m_nFileSize;
					// pDetails->m_nPreviewFileSize;
					// pDetails->m_nPublishedFileId;
					// pDetails->m_pchFileName;
					// pDetails->m_rgchDescription;
					// pDetails->m_rgchTags;
					// pDetails->m_rgchTitle;
					// pDetails->m_rgchURL;
					// pDetails->m_rtimeAddedToUserList;
					// pDetails->m_rtimeCreated;
					// pDetails->m_rtimeUpdated;
					// pDetails->m_ulSteamIDOwner;
					// pDetails->m_unVotesDown;
					// pDetails->m_unVotesUp;
					
					
					SteamUGCDetails_t *pDetails = new SteamUGCDetails_t;
					SteamUGC()->GetQueryUGCResult(m_Handle, iUGC, pDetails);
					
					CSteamWorkshopItem item = CSteamWorkshopItem();
					item.Details = pDetails;

					m_wsItems.push_back(item);

					/*
					if (ProgressDelegate->IsCallable()){
						ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
					}

					perc += percStep;
					progressCallbackArg->Set(String::New("value"), Integer::New(perc));
					*/
				}

			}
			/*
			perc = 100;
			progressCallbackArg->Set(String::New("value"), Integer::New(perc));

			if (ProgressDelegate->IsCallable()){
				ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}
			*/

			m_iNumWorkshopItems = pCallback->m_unNumResultsReturned;

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCUserQueryCompleted = Workshop items found. " + to_string(m_iNumWorkshopItems) + " items :-)");
		}
		else {
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCUserQueryCompleted = No Workshop items found matching criterias");
			m_iNumWorkshopItems = 0;
		}

		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

		SteamUGC()->ReleaseQueryUGCRequest(m_Handle);

	}
	catch (Exception ex) {
		if (ErrorDelegate->IsCallable()){
			/*
			if (ProgressDelegate->IsCallable()){
				progressCallbackArg->Set(String::New("value"), Integer::New(perc));
				progressCallbackArg->Set(String::New("status"), String::New("steam_request_error"));
				ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, progressCallbackVals);
			}
			*/

			if (ErrorDelegate->IsCallable()){
				errorCallbackArg = String::New("An error occured while trying to handle a Steam UGC User Request!\n\nSource: CSteamWorkshop::OnUGCUserQueryCompleted");
				errorCallbackVals[0] = errorCallbackArg;
				ErrorDelegate->Call(Context::GetCurrent()->Global(), 1, errorCallbackVals);
			}
		}
	}

	Requesting = false;
}

void CSteamWorkshop::OnUGCQueryAndSyncCompleted(SteamUGCQueryCompleted_t *pCallback, bool bIOFailure)
{
	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

	// We may get callbacks for other games' stats arriving, ignore them
	if (k_EResultOK == pCallback->m_eResult)
	{
		m_bInitialized = true;

		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCQueryAndSyncCompleted = Receiving data from Steam... :-)");


		ISteamUGC *steamUGC = SteamUGC();

		m_wsItems.clear();

		if (pCallback->m_unNumResultsReturned > 0){

			// load ugcs
			for (int iUGC = 0; iUGC < pCallback->m_unNumResultsReturned; ++iUGC)
			{
				SteamUGCDetails_t *pDetails = new SteamUGCDetails_t;
				SteamUGC()->GetQueryUGCResult(m_Handle, iUGC, pDetails);

				DownloadFromUGCSynchronize(pDetails->m_hFile);
				SteamAPI_RunCallbacks();
				CUtils::Sleep(25);
			}
		}

		m_iNumWorkshopItems = pCallback->m_unNumResultsReturned;

		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCQueryAndSyncCompleted = Subscribed Workshop items found. " + to_string(m_iNumWorkshopItems) + " items.");
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUGCQueryAndSyncCompleted = No subscribed Workshop items found.");
		m_iNumWorkshopItems = 0;
	}

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

	SteamUGC()->ReleaseQueryUGCRequest(m_Handle);
	Requesting = false;
}

void CSteamWorkshop::OnDownloadCompleted(RemoteStorageDownloadUGCResult_t *pCallback, bool bIOFailure)
{
	bool isNew = false;
	int fileSize = 0;
	int lastModified = 0;

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

	// we may get callbacks for other games' stats arriving, ignore them
	if (k_EResultOK == pCallback->m_eResult)
	{
		string sFilename = CUtils::GetFilename(pCallback->m_pchFileName);
		string targetFile = CUtils::PathCombine(m_targetDirForDownload, sFilename);

		int iSize = pCallback->m_nSizeInBytes;
		UGCHandle_t hFile = pCallback->m_hFile;
		
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = " + sFilename + " (" + to_string(iSize) + " bytes) " + to_string(hFile));

		byte *data = new byte[iSize];

		SteamRemoteStorage()->UGCRead(hFile, data, iSize, 0, k_EUGCRead_Close);

		// Acquire steam file details
		CSteamWorkshopItem pDetails = GetItemByFileName(pCallback->m_pchFileName);
		int iWsItemIndex = GetItemIndexByFileName(pCallback->m_pchFileName);
		
		int timeUgcFile = 0;
		int timeExistingFile = CUtils::GetFileTime(targetFile.c_str());

		if (pDetails.Details->m_hFile != 0){
			timeUgcFile = pDetails.Details->m_rtimeUpdated;
		}
		else {
			timeUgcFile = SteamRemoteStorage()->GetFileTimestamp(pCallback->m_pchFileName);
		}
		
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = Check for changes...(" + to_string(timeUgcFile) + " > " + to_string(timeExistingFile) + ")");
		HasChanged = (timeUgcFile > timeExistingFile) || (timeUgcFile == 0 && timeExistingFile == 0);
		
		if (HasChanged){
			if (timeExistingFile <= 0){
				CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = ...first download.");
				isNew = true;
			}
			else {
				CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = ...changes detected.");
			}
            
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = Trying to write contents to the file " + targetFile);

			FILE* hTargetFile = fopen(targetFile.c_str(), "wb");
			fwrite(data, 1, iSize, hTargetFile);
			fclose(hTargetFile);

			// Synchronize DateTimeStamp (set modified to correspond to Steam value)
			// Note: This is done for cross-platform compatibility, as on *nix systems it is 
			// not possible to easily get a reliable create date-time stamp of a file.
			//
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = File written.");


			struct utimbuf *ut = new utimbuf;
			time_t rawtime = timeUgcFile;

			// Convert tm to time_t
			ut->actime = rawtime;
			ut->modtime = rawtime;
			
			if (utime(targetFile.c_str(), ut) == 0){
				CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = Date/Time Stamp updated.");
			}
			else {
				CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = Date/Time Stamp update failed!");
			}

		}
		else {

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = ...NO changes detected.");
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = Local file already existent and up-to-date. Skipped content processing.");
		}

		m_wsItems[iWsItemIndex].HasChanged = HasChanged;
		m_wsItems[iWsItemIndex].Size = iSize;
		m_wsItems[iWsItemIndex].Modified = timeUgcFile;
		m_wsItems[iWsItemIndex].TargetFile = targetFile;
		m_wsItems[iWsItemIndex].IsNew = isNew;
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadCompleted = Error or problem occured!");
	}

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

	// Call AFTER
	// callbackResultArgs[0] = callbackObj;

	// ProgressDelegate->Call(Context::GetCurrent()->Global(), 1, callbackResultArgs);

	Requesting = false;
}

void CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted(RemoteStorageDownloadUGCResult_t *pCallback, bool bIOFailure)
{
	
	bool isNew = false;
	int fileSize = 0;
	int lastModified = 0;

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

	string sFilename = CUtils::GetFilename(pCallback->m_pchFileName);
	string targetFile = CUtils::PathCombine(m_targetDirForDownload, sFilename);

	// we may get callbacks for other games' stats arriving, ignore them
	if (k_EResultOK == pCallback->m_eResult)
	{
		int iSize = pCallback->m_nSizeInBytes;
		UGCHandle_t hFile = pCallback->m_hFile;

		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = " + sFilename + " (" + to_string(iSize) + " bytes) " + to_string(hFile));

		byte *data = new byte[iSize];

		SteamRemoteStorage()->UGCRead(hFile, data, iSize, 0, k_EUGCRead_Close);

		int timeUgcFile = SteamRemoteStorage()->GetFileTimestamp(pCallback->m_pchFileName);
		int timeExistingFile = CUtils::GetFileTime(targetFile.c_str());


		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = Check for changes...(" + to_string(timeUgcFile) + " > " + to_string(timeExistingFile) + ")");
		HasChanged = (timeUgcFile > timeExistingFile) || (timeUgcFile == 0 && timeExistingFile == 0);


		if (HasChanged){
			if (timeExistingFile <= 0){
				CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = ...first download.");
				isNew = true;
			}
			else {
				CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = ...changes detected.");
			}

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = Trying to write contents to the file " + m_targetDirForDownload);


			FILE* hTargetFile = fopen(targetFile.c_str(), "wb");
			fwrite(data, 1, iSize, hTargetFile);
			fclose(hTargetFile);

			// Synchronize DateTimeStamp (set modified to correspond to Steam value)
			// Note: This is done for cross-platform compatibility, as on *nix systems it is 
			// not possible to easily get a reliable create date-time stamp of a file.
			//
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = File written.");


			struct utimbuf *ut = new utimbuf;
			time_t rawtime = timeUgcFile;

			// Convert tm to time_t
			ut->actime = rawtime;
			ut->modtime = rawtime;

			if (utime(targetFile.c_str(), ut) == 0){
				CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = Date/Time Stamp updated.");
			}
			else {
				CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = Date/Time Stamp update failed!");
			}

		}
		else {

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = ...NO changes detected.");
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = Local file already existent and up-to-date. Skipped content processing.");
		}

		lastModified = timeUgcFile;
		fileSize = iSize;
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = Error or problem occured!");
	}

	CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

	try{
		m_wsItems[m_iNumWorkshopItemsProcessed].HasChanged = HasChanged;
		m_wsItems[m_iNumWorkshopItemsProcessed].IsNew = isNew;

		m_wsItems[m_iNumWorkshopItemsProcessed].Modified = lastModified;
		m_wsItems[m_iNumWorkshopItemsProcessed].Size = fileSize;
		m_wsItems[m_iNumWorkshopItemsProcessed].TargetFile = targetFile;
	}
	catch (Exception ex){
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnDownloadFromUGCSynchronizeCompleted = An error occured trying to acquire additional data for this item.");
	}

	CUtils::WriteToJsConsoleAndLogFile("len: " + to_string(m_wsItems.size()));
	CUtils::WriteToJsConsoleAndLogFile("idx: " + to_string(m_iNumWorkshopItemsProcessed));

	CUtils::WriteToJsConsoleAndLogFile("...next please...");
	m_iNumWorkshopItemsProcessed++;

	// End request
	Requesting = false;
}

void CSteamWorkshop::OnGetPublishedFileDetailsCompleted(RemoteStorageGetPublishedFileDetailsResult_t *pCallback, bool bIOFailure){
	if (pCallback->m_eResult != k_EResultOK){
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> File Publish not possible (failed).");
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

		Requesting = false;
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> File Details aquired.");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> Create publish update request with acquired data. (id: " + to_string(pCallback->m_nPublishedFileId) + ")");

		bool skipUpdate = m_title.empty() && m_description.empty() && m_imageFileName.empty() && m_fileName.empty();
		
		if (skipUpdate){
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> No changes detected. Skipping update.");
			CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

			Requesting = false;
		}
		else {
			PublishedFileUpdateHandle_t pfileupdatehandle = SteamRemoteStorage()->CreatePublishedFileUpdateRequest(pCallback->m_nPublishedFileId);

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> And apply changes using the file update handle " + to_string(pfileupdatehandle));


			string titleOk = "No update";
			if (!m_title.empty()){
				titleOk = SteamRemoteStorage()->UpdatePublishedFileTitle(pfileupdatehandle, m_title.c_str()) ? "Ok" : "Error";
			}

			string descOk = "No update";
			if (!m_description.empty()){
				descOk = SteamRemoteStorage()->UpdatePublishedFileDescription(pfileupdatehandle, m_description.c_str()) ? "Ok" : "Error";
			}

			string imgOk = "No update";
			if (!m_imageFileName.empty()){
				imgOk = SteamRemoteStorage()->UpdatePublishedFilePreviewFile(pfileupdatehandle, m_imageFileName.c_str()) ? "Ok" : "Error";
			}

			string fileOk = "No update";
			if (!m_fileName.empty()){
				fileOk = SteamRemoteStorage()->UpdatePublishedFileFile(pfileupdatehandle, m_fileName.c_str()) ? "Ok" : "Error";
			}

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> Update Result for Title: " + titleOk);
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> Update Result for Description: " + descOk);
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> Update Result for Image: " + imgOk);
			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> Update Result for File: " + fileOk);

			CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> Committing Update now...");
			CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");

			SteamAPICall_t commitupdateresult = SteamRemoteStorage()->CommitPublishedFileUpdate(pfileupdatehandle);
			m_CallResultOnCommitPublishedFileUpdateCompleted.Set(commitupdateresult, this, &CSteamWorkshop::OnCommitPublishedFileUpdateCompleted);
		}

	}
}

void CSteamWorkshop::OnCommitPublishedFileUpdateCompleted(RemoteStorageUpdatePublishedFileResult_t *pCallback, bool bIOFailure){
	if (pCallback->m_eResult != k_EResultOK){
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnCommitPublishedFileUpdateCompleted >> Update Commit not possible (failed).");
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnGetPublishedFileDetailsCompleted >> Update succesfully committed.");
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
	}

	Requesting = false;
}

void CSteamWorkshop::OnUnsubscribeCompleted(RemoteStoragePublishedFileUnsubscribed_t *pCallback, bool bIOFailure){
	if (pCallback->m_nPublishedFileId != 0){
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUnsubscribeCompleted >> Unsubscription succesfully performed for id " + to_string(pCallback->m_nPublishedFileId) + ".");
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		Response = true;
	}
	else {
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		CUtils::WriteToJsConsoleAndLogFile("CSteamWorkshop::OnUnsubscribeCompleted >> Unsubscription not possible (failed).");
		CUtils::WriteToJsConsoleAndLogFile("------------------------------------------------------------------------");
		Response = false;
	}

	Requesting = false;
}


PublishedFileId_t CSteamWorkshop::GetLastPublishedFileId(){
	return m_publishedFileId;
}