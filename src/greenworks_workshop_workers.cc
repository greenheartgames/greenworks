// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_workshop_workers.h"

#include <algorithm>

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "greenworks_utils.h"

namespace {
v8::Local<v8::Object> ConvertToJsObject(const SteamUGCDetails_t& item) {
  v8::Local<v8::Object> result = Nan::New<v8::Object>();

  Nan::Set(result, Nan::New("acceptedForUse").ToLocalChecked(),
           Nan::New(item.m_bAcceptedForUse));
  Nan::Set(result, Nan::New("banned").ToLocalChecked(),
           Nan::New(item.m_bBanned));
  Nan::Set(result, Nan::New("tagsTruncated").ToLocalChecked(),
           Nan::New(item.m_bTagsTruncated));
  Nan::Set(result, Nan::New("fileType").ToLocalChecked(),
           Nan::New(item.m_eFileType));
  Nan::Set(result, Nan::New("result").ToLocalChecked(),
           Nan::New(item.m_eResult));
  Nan::Set(result, Nan::New("visibility").ToLocalChecked(),
           Nan::New(item.m_eVisibility));
  Nan::Set(result, Nan::New("score").ToLocalChecked(),
           Nan::New(item.m_flScore));

  Nan::Set(result, Nan::New("file").ToLocalChecked(),
           Nan::New(utils::uint64ToString(item.m_hFile)).ToLocalChecked());
  Nan::Set(result, Nan::New("fileName").ToLocalChecked(),
           Nan::New(item.m_pchFileName).ToLocalChecked());
  Nan::Set(result, Nan::New("fileSize").ToLocalChecked(),
           Nan::New(item.m_nFileSize));

  Nan::Set(
      result, Nan::New("previewFile").ToLocalChecked(),
      Nan::New(utils::uint64ToString(item.m_hPreviewFile)).ToLocalChecked());
  Nan::Set(result, Nan::New("previewFileSize").ToLocalChecked(),
           Nan::New(item.m_nPreviewFileSize));

  Nan::Set(
      result, Nan::New("steamIDOwner").ToLocalChecked(),
      Nan::New(utils::uint64ToString(item.m_ulSteamIDOwner)).ToLocalChecked());
  Nan::Set(result, Nan::New("consumerAppID").ToLocalChecked(),
           Nan::New(item.m_nConsumerAppID));
  Nan::Set(result, Nan::New("creatorAppID").ToLocalChecked(),
           Nan::New(item.m_nCreatorAppID));
  Nan::Set(result, Nan::New("publishedFileId").ToLocalChecked(),
           Nan::New(utils::uint64ToString(item.m_nPublishedFileId))
               .ToLocalChecked());

  Nan::Set(result, Nan::New("title").ToLocalChecked(),
           Nan::New(item.m_rgchTitle).ToLocalChecked());
  Nan::Set(result, Nan::New("description").ToLocalChecked(),
           Nan::New(item.m_rgchDescription).ToLocalChecked());
  Nan::Set(result, Nan::New("URL").ToLocalChecked(),
           Nan::New(item.m_rgchURL).ToLocalChecked());
  Nan::Set(result, Nan::New("tags").ToLocalChecked(),
           Nan::New(item.m_rgchTags).ToLocalChecked());

  Nan::Set(result, Nan::New("timeAddedToUserList").ToLocalChecked(),
           Nan::New(item.m_rtimeAddedToUserList));
  Nan::Set(result, Nan::New("timeCreated").ToLocalChecked(),
           Nan::New(item.m_rtimeCreated));
  Nan::Set(result, Nan::New("timeUpdated").ToLocalChecked(),
           Nan::New(item.m_rtimeUpdated));
  Nan::Set(result, Nan::New("votesDown").ToLocalChecked(),
           Nan::New(item.m_unVotesDown));
  Nan::Set(result, Nan::New("votesUp").ToLocalChecked(),
           Nan::New(item.m_unVotesUp));

  return result;
}

v8::Local<v8::Object> ConvertToJsObject(const UGCItem& item) {
    v8::Local<v8::Object> result = ConvertToJsObject(item.m_details);

    Nan::Set(
        result, Nan::New("NumSubscriptions").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumSubscriptions)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumFavorites").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumFavorites)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumFollowers").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumFollowers)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumUniqueSubscriptions").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumUniqueSubscriptions)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumUniqueFavorites").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumUniqueFavorites)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumUniqueFollowers").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumUniqueFollowers)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumUniqueWebsiteViews").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumUniqueWebsiteViews)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumSecondsPlayed").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumSecondsPlayed)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumPlaytimeSessions").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumPlaytimeSessions)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumComments").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumComments)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumSecondsPlayedDuringTimePeriod").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumSecondsPlayedDuringTimePeriod)).ToLocalChecked());

    Nan::Set(
        result, Nan::New("NumPlaytimeSessionsDuringTimePeriod").ToLocalChecked(),
        Nan::New(utils::uint64ToString(item.m_ulNumPlaytimeSessionsDuringTimePeriod)).ToLocalChecked());

    Nan::Set(result, Nan::New("PreviewImageUrl").ToLocalChecked(),
        Nan::New(item.m_rgchPreviewImageUrl).ToLocalChecked());

    Nan::Set(result, Nan::New("Metadata").ToLocalChecked(),
        Nan::New(item.m_rgchMetadata).ToLocalChecked());

    return result;
}

inline std::string GetAbsoluteFilePath(const std::string& file_path,
    const std::string& download_dir) {
  std::string file_name = file_path.substr(file_path.find_last_of("/\\") + 1);
  return download_dir + "/" + file_name;
}

}  // namespace

namespace greenworks {

FileShareWorker::FileShareWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, const std::string& file_path)
        :SteamCallbackAsyncWorker(success_callback, error_callback),
         file_path_(file_path) {
}

void FileShareWorker::Execute() {
  // Ignore empty path.
  if (file_path_.empty()) return;

  std::string file_name = utils::GetFileNameFromPath(file_path_);
  SteamAPICall_t share_result = SteamRemoteStorage()->FileShare(
      file_name.c_str());
  call_result_.Set(share_result, this, &FileShareWorker::OnFileShareCompleted);

  // Wait for FileShare callback result.
  WaitForCompleted();
}

void FileShareWorker::OnFileShareCompleted(
    RemoteStorageFileShareResult_t* result, bool io_failure) {
  if (io_failure) {
    SetErrorMessage("Error on sharing file: Steam API IO Failure");
  } else if (result->m_eResult == k_EResultOK) {
    share_file_handle_ = result->m_hFile;
  } else {
    SetErrorMessage("Error on sharing file on Steam cloud.");
  }
  is_completed_ = true;
}

void FileShareWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  v8::Local<v8::Value> argv[] = {
      Nan::New(utils::uint64ToString(share_file_handle_)).ToLocalChecked() };
  Nan::AsyncResource resource("greenworks:FileShareWorker.HandleOKCallback");
  callback->Call(1, argv, &resource);
}

CreateWorkshopItemWorker::CreateWorkshopItemWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    uint32 app_id, const WorkshopFileProperties& properties)
    : SteamCallbackAsyncWorker(success_callback, error_callback),
    app_id_(app_id),
    properties_(properties) {}

void CreateWorkshopItemWorker::Execute() {
    SteamAPICall_t create_result = SteamUGC()->CreateItem(app_id_, EWorkshopFileType::k_EWorkshopFileTypeCommunity);

    call_result_.Set(create_result, this,
        &CreateWorkshopItemWorker::OnItemCreateCompleted);

    // Wait for ItemCreate callback result.
    WaitForCompleted();
}

void CreateWorkshopItemWorker::OnItemCreateCompleted(
    CreateItemResult_t* result, bool io_failure) {
    if (io_failure) {
        SetErrorMessage("Error on publishing workshop file: Steam API IO Failure");
    }
    else if (result->m_eResult == k_EResultOK) {
        publish_file_id_ = result->m_nPublishedFileId;

        SteamParamStringArray_t tags;
        tags.m_nNumStrings = properties_.tags_scratch.size();
        tags.m_ppStrings = new const char* [tags.m_nNumStrings];
        for (int i = 0; i < tags.m_nNumStrings; i++) {
            tags.m_ppStrings[i] = properties_.tags_scratch[i].c_str();
        }

        if (result->m_bUserNeedsToAcceptWorkshopLegalAgreement) {
            // Inform user legal agreement is required to make workshop item visible to public
        }

        UGCUpdateHandle_t update_handle = SteamUGC()->StartItemUpdate(app_id_, publish_file_id_);
        
        if (!properties_.title.empty())
            SteamUGC()->SetItemTitle(update_handle, properties_.title.c_str());
        if (!properties_.description.empty())
            SteamUGC()->SetItemDescription(update_handle, properties_.description.c_str());
        SteamUGC()->SetItemVisibility(update_handle, ERemoteStoragePublishedFileVisibility::k_ERemoteStoragePublishedFileVisibilityPublic);
        SteamUGC()->SetItemTags(update_handle, &tags);
        if (!properties_.file_path.empty())
            SteamUGC()->SetItemContent(update_handle, properties_.file_path.c_str());
        if (!properties_.image_path.empty())
            SteamUGC()->SetItemPreview(update_handle, properties_.image_path.c_str());

        if (tags.m_ppStrings) delete tags.m_ppStrings;

        SteamAPICall_t submit_result = SteamUGC()->SubmitItemUpdate(update_handle, nullptr);
    }
    else {
        char buffer[100];
        sprintf(buffer, "%d: Error on publishing workshop file.", (int)result->m_eResult);
        SetErrorMessage(buffer);
    }
    is_completed_ = true;
}

void CreateWorkshopItemWorker::HandleOKCallback() {
    Nan::HandleScope scope;

    v8::Local<v8::Value> argv[] = {
        Nan::New(utils::uint64ToString(publish_file_id_)).ToLocalChecked() };
    Nan::AsyncResource resource("greenworks:CreateWorkshopItemWorker.HandleOKCallback");
    callback->Call(1, argv, &resource);
}

PublishWorkshopFileWorker::PublishWorkshopFileWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    uint32 app_id, const WorkshopFileProperties& properties)
    : SteamCallbackAsyncWorker(success_callback, error_callback),
      app_id_(app_id),
      properties_(properties) {}

void PublishWorkshopFileWorker::Execute() {
    SteamParamStringArray_t tags;
    tags.m_nNumStrings = properties_.tags_scratch.size();
    tags.m_ppStrings = new const char* [tags.m_nNumStrings];
    for (int i = 0; i < tags.m_nNumStrings; i++) {
        tags.m_ppStrings[i] = properties_.tags_scratch[i].c_str();
    }

    std::string file_name = utils::GetFileNameFromPath(properties_.file_path);
    std::string image_name = utils::GetFileNameFromPath(properties_.image_path);
    SteamAPICall_t publish_result = SteamRemoteStorage()->PublishWorkshopFile(
        file_name.c_str(),
        image_name.empty()? nullptr:image_name.c_str(),
        app_id_,
        properties_.title.c_str(),
        properties_.description.empty()? nullptr:properties_.description.c_str(),
        k_ERemoteStoragePublishedFileVisibilityPublic,
        &tags,
        k_EWorkshopFileTypeCommunity);

    call_result_.Set(publish_result, this,
    &PublishWorkshopFileWorker::OnFilePublishCompleted);

    if (tags.m_ppStrings) delete tags.m_ppStrings;
    // Wait for FileShare callback result.
    WaitForCompleted();
}

void PublishWorkshopFileWorker::OnFilePublishCompleted(
    RemoteStoragePublishFileResult_t* result, bool io_failure) {
  if (io_failure) {
    SetErrorMessage("Error on publishing workshop file: Steam API IO Failure");
  }
  else if (result->m_eResult == k_EResultOK) {
      publish_file_id_ = result->m_nPublishedFileId;
  }
  else {
    char buffer[100];
    sprintf(buffer, "%d: Error on publishing workshop file.", (int)result->m_eResult);
    SetErrorMessage(buffer);
  }
  is_completed_ = true;
}

void PublishWorkshopFileWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  v8::Local<v8::Value> argv[] = {
      Nan::New(utils::uint64ToString(publish_file_id_)).ToLocalChecked() };
  Nan::AsyncResource resource("greenworks:PublishWorkshopFileWorker.HandleOKCallback");
  callback->Call(1, argv, &resource);
}

UpdatePublishedWorkshopFileWorker::UpdatePublishedWorkshopFileWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    PublishedFileId_t published_file_id,
    const WorkshopFileProperties& properties)
    : SteamCallbackAsyncWorker(success_callback, error_callback),
      published_file_id_(published_file_id),
      properties_(properties) {}

void UpdatePublishedWorkshopFileWorker::Execute() {
  PublishedFileUpdateHandle_t update_handle =
      SteamRemoteStorage()->CreatePublishedFileUpdateRequest(
          published_file_id_);

  const std::string file_name =
      utils::GetFileNameFromPath(properties_.file_path);
  const std::string image_name =
      utils::GetFileNameFromPath(properties_.image_path);
  SteamParamStringArray_t tags;
  if (!file_name.empty())
    SteamRemoteStorage()->UpdatePublishedFileFile(update_handle,
        file_name.c_str());
  if (!image_name.empty())
    SteamRemoteStorage()->UpdatePublishedFilePreviewFile(update_handle,
        image_name.c_str());
  if (!properties_.title.empty())
    SteamRemoteStorage()->UpdatePublishedFileTitle(update_handle,
        properties_.title.c_str());
  if (!properties_.description.empty())
    SteamRemoteStorage()->UpdatePublishedFileDescription(update_handle,
        properties_.description.c_str());
  if (!properties_.tags_scratch.empty()) {
      if (properties_.tags_scratch.size() == 1 &&
          properties_.tags_scratch.front().empty()) {  // clean the tag.
          tags.m_nNumStrings = 0;
          tags.m_ppStrings = nullptr;
      }
      else {

          tags.m_nNumStrings = properties_.tags_scratch.size();
          tags.m_ppStrings = new const char* [tags.m_nNumStrings];
          for (int i = 0; i < tags.m_nNumStrings; i++) {
              tags.m_ppStrings[i] = properties_.tags_scratch[i].c_str();
          }
      }
      SteamRemoteStorage()->UpdatePublishedFileTags(update_handle, &tags);
  }
  SteamAPICall_t commit_update_result =
      SteamRemoteStorage()->CommitPublishedFileUpdate(update_handle);
  update_published_file_call_result_.Set(commit_update_result, this,
      &UpdatePublishedWorkshopFileWorker::
           OnCommitPublishedFileUpdateCompleted);

  if (tags.m_ppStrings) delete tags.m_ppStrings;
  // Wait for published workshop file updated.
  WaitForCompleted();
}

void UpdatePublishedWorkshopFileWorker::OnCommitPublishedFileUpdateCompleted(
    RemoteStorageUpdatePublishedFileResult_t* result, bool io_failure) {
  if (io_failure) {
    SetErrorMessage(
        "Error on committing published file update: Steam API IO Failure");
  } else if (result->m_eResult == k_EResultOK) {
  } else {
    SetErrorMessage("Error on getting published file details.");
  }
  is_completed_ = true;
}

QueryUGCWorker::QueryUGCWorker(Nan::Callback* success_callback,
                               Nan::Callback* error_callback,
                               EUGCMatchingUGCType ugc_matching_type,
                               uint32 app_id, uint32 page_num)
    : SteamCallbackAsyncWorker(success_callback, error_callback),
      ugc_matching_type_(ugc_matching_type),
      app_id_(app_id),
      page_num_(page_num) {}

void QueryUGCWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  v8::Local<v8::Array> items = Nan::New<v8::Array>(
      static_cast<int>(ugc_items_.size()));
  for (size_t i = 0; i < ugc_items_.size(); ++i)
    Nan::Set(items, i, ConvertToJsObject(ugc_items_[i]));

  v8::Local<v8::Uint32> numResults = Nan::New<v8::Uint32>(num_results_);
  v8::Local<v8::Uint32> numTotalResults = Nan::New<v8::Uint32>(num_total_results_);

  v8::Local<v8::Value> argv[] = { items, numResults, numTotalResults };
  Nan::AsyncResource resource("greenworks:QueryUGCWorker.HandleOKCallback");
  callback->Call(3, argv, &resource);
}

void QueryUGCWorker::OnUGCQueryCompleted(SteamUGCQueryCompleted_t* result,
    bool io_failure) {
  if (io_failure) {
    SetErrorMessage("Error on querying all ugc: Steam API IO Failure");
  } else if (result->m_eResult == k_EResultOK) {
    uint32 count = result->m_unNumResultsReturned;
    num_results_ = count;
    num_total_results_ = result->m_unTotalMatchingResults;
    SteamUGCDetails_t item;
    for (uint32 i = 0; i < count; ++i) {
      SteamUGC()->GetQueryUGCResult(result->m_handle, i, &item);
      UGCItem ugcItem;
      ugcItem.m_details = item;
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumSubscriptions, &ugcItem.m_ulNumSubscriptions);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumFavorites, &ugcItem.m_ulNumFavorites);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumFollowers, &ugcItem.m_ulNumFollowers);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueSubscriptions, &ugcItem.m_ulNumUniqueSubscriptions);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueFavorites, &ugcItem.m_ulNumUniqueFavorites);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueFollowers, &ugcItem.m_ulNumUniqueFollowers);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueWebsiteViews, &ugcItem.m_ulNumUniqueWebsiteViews);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_ReportScore, &ugcItem.m_ulReportScore);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumSecondsPlayed, &ugcItem.m_ulNumSecondsPlayed);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumPlaytimeSessions, &ugcItem.m_ulNumPlaytimeSessions);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumComments, &ugcItem.m_ulNumComments);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumSecondsPlayedDuringTimePeriod, &ugcItem.m_ulNumSecondsPlayedDuringTimePeriod);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumPlaytimeSessionsDuringTimePeriod, &ugcItem.m_ulNumPlaytimeSessionsDuringTimePeriod);
      SteamUGC()->GetQueryUGCPreviewURL(result->m_handle, i, &ugcItem.m_rgchPreviewImageUrl[0], 256);
      SteamUGC()->GetQueryUGCMetadata(result->m_handle, i, &ugcItem.m_rgchMetadata[0], (1024 * 32));
      ugc_items_.push_back(ugcItem);
    }
    SteamUGC()->ReleaseQueryUGCRequest(result->m_handle);
  } else {
    char buffer[100];
    sprintf(buffer, "%d: Error on querying ugc.", (int)result->m_eResult);
    SetErrorMessage(buffer);
  }
  is_completed_ = true;
}

QueryAllUGCWorker::QueryAllUGCWorker(Nan::Callback* success_callback,
                                     Nan::Callback* error_callback,
                                     EUGCMatchingUGCType ugc_matching_type,
                                     EUGCQuery ugc_query_type, uint32 app_id,
                                     const SearchOptions& options)
    : QueryUGCWorker(success_callback, error_callback, ugc_matching_type,
                     app_id, options.page_num),
                    ugc_query_type_(ugc_query_type),
                    ugc_search_options_(options){}

void QueryAllUGCWorker::Execute() {
  uint32 invalid_app_id = 0;
  // Set "creator_app_id" parameter to an invalid id to make Steam API return
  // all ugc items, otherwise the API won't get any results in some cases.

  UGCQueryHandle_t ugc_handle = SteamUGC()->CreateQueryAllUGCRequest(
      ugc_query_type_, ugc_matching_type_, /*creator_app_id=*/invalid_app_id,
      /*consumer_app_id=*/app_id_, page_num_);

  // keyword
  SteamUGC()->SetSearchText(ugc_handle, ugc_search_options_.keyword.c_str());

  // tags
  SteamParamStringArray_t tags;
  tags.m_nNumStrings = ugc_search_options_.tags_scratch.size();
  tags.m_ppStrings = new const char* [tags.m_nNumStrings];
  for (int i = 0; i < tags.m_nNumStrings; i++) {
      tags.m_ppStrings[i] = ugc_search_options_.tags_scratch[i].c_str();
  }
  SteamUGC()->AddRequiredTagGroup(ugc_handle, &tags);

  // excluded tags
  const size_t numExTags = ugc_search_options_.excluded_tags_scratch.size();
  for (int i = 0; i < numExTags; i++) {
      SteamUGC()->AddExcludedTag(ugc_handle, ugc_search_options_.excluded_tags_scratch[i].c_str());
  }
  SteamAPICall_t ugc_query_result = SteamUGC()->SendQueryUGCRequest(ugc_handle);

  ugc_query_call_result_.Set(ugc_query_result, this,
      &QueryAllUGCWorker::OnUGCQueryCompleted);

  // Wait for query all ugc completed.
  WaitForCompleted();
}

QueryUserUGCWorker::QueryUserUGCWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    EUGCMatchingUGCType ugc_matching_type, EUserUGCList ugc_list,
    EUserUGCListSortOrder ugc_list_sort_order, uint32 app_id, uint32 page_num)
    : QueryUGCWorker(success_callback, error_callback, ugc_matching_type,
                     app_id, page_num),
      ugc_list_(ugc_list),
      ugc_list_sort_order_(ugc_list_sort_order) {}

void QueryUserUGCWorker::Execute() {
  UGCQueryHandle_t ugc_handle = SteamUGC()->CreateQueryUserUGCRequest(
      SteamUser()->GetSteamID().GetAccountID(),
      ugc_list_,
      ugc_matching_type_,
      ugc_list_sort_order_,
      app_id_,
      app_id_,
      page_num_);
  SteamAPICall_t ugc_query_result = SteamUGC()->SendQueryUGCRequest(ugc_handle);
  ugc_query_call_result_.Set(ugc_query_result, this,
      &QueryUserUGCWorker::OnUGCQueryCompleted);

  // Wait for query all ugc completed.
  WaitForCompleted();
}

DownloadItemWorker::DownloadItemWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, UGCHandle_t download_file_handle,
    const std::string& download_dir)
        :SteamCallbackAsyncWorker(success_callback, error_callback),
         download_file_handle_(download_file_handle),
         download_dir_(download_dir),
    result(this, &DownloadItemWorker::OnDownloadCompleted)
{
}

void DownloadItemWorker::Execute() {
  bool success = SteamUGC()->DownloadItem(download_file_handle_, true);
  if (success) {
    // Wait for downloading file completed.
    WaitForCompleted();
  }
  else {
      SetErrorMessage("given file id is invalid or the you are not logged on.");
  }
}

void DownloadItemWorker::OnDownloadCompleted(
    DownloadItemResult_t* result) {
    
    if (result->m_eResult == k_EResultOK) {
    app_id_ = result->m_unAppID;
    file_id_ = result->m_nPublishedFileId;
  } else {
      char buffer[100];
      sprintf(buffer, "%d: Error on downloading file.", (int)result->m_eResult);
      SetErrorMessage(buffer);
  }
  is_completed_ = true;
}

void DownloadItemWorker::HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Value> argv[] = {
        Nan::New(utils::uint64ToString(app_id_)).ToLocalChecked(),
        Nan::New(utils::uint64ToString(file_id_)).ToLocalChecked() };
    Nan::AsyncResource resource("greenworks:DownloadItemWorker.HandleOKCallback");
    callback->Call(2, argv, &resource);
}

SynchronizeItemsWorker::SynchronizeItemsWorker(Nan::Callback* success_callback,
                                               Nan::Callback* error_callback,
                                               const std::string& download_dir,
                                               uint32 app_id, uint32 page_num)
    : SteamCallbackAsyncWorker(success_callback, error_callback),
      current_download_items_pos_(0),
      download_dir_(download_dir),
      app_id_(app_id),
      page_num_(page_num) {}

void SynchronizeItemsWorker::Execute() {
  UGCQueryHandle_t ugc_handle = SteamUGC()->CreateQueryUserUGCRequest(
      SteamUser()->GetSteamID().GetAccountID(),
      k_EUserUGCList_Subscribed,
      k_EUGCMatchingUGCType_Items,
      k_EUserUGCListSortOrder_SubscriptionDateDesc,
      app_id_,
      app_id_,
      page_num_);
  SteamAPICall_t ugc_query_result = SteamUGC()->SendQueryUGCRequest(ugc_handle);
  ugc_query_call_result_.Set(ugc_query_result, this,
      &SynchronizeItemsWorker::OnUGCQueryCompleted);

  // Wait for synchronization completed.
  WaitForCompleted();
}

void SynchronizeItemsWorker::OnUGCQueryCompleted(
    SteamUGCQueryCompleted_t* result, bool io_failure) {
  if (io_failure) {
    SetErrorMessage("Error on querying all ugc: Steam API IO Failure");
  } else if (result->m_eResult == k_EResultOK) {
    SteamUGCDetails_t item;
    for (uint32 i = 0; i < result->m_unNumResultsReturned; ++i) {
      SteamUGC()->GetQueryUGCResult(result->m_handle, i, &item);
      std::string target_path = GetAbsoluteFilePath(item.m_pchFileName,
          download_dir_);
      int64 file_update_time = utils::GetFileLastUpdatedTime(
          target_path.c_str());

      UGCItem ugcItem;
      ugcItem.m_details = item;
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumSubscriptions, &ugcItem.m_ulNumSubscriptions);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumFavorites, &ugcItem.m_ulNumFavorites);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumFollowers, &ugcItem.m_ulNumFollowers);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueSubscriptions, &ugcItem.m_ulNumUniqueSubscriptions);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueFavorites, &ugcItem.m_ulNumUniqueFavorites);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueFollowers, &ugcItem.m_ulNumUniqueFollowers);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumUniqueWebsiteViews, &ugcItem.m_ulNumUniqueWebsiteViews);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_ReportScore, &ugcItem.m_ulReportScore);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumSecondsPlayed, &ugcItem.m_ulNumSecondsPlayed);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumPlaytimeSessions, &ugcItem.m_ulNumPlaytimeSessions);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumComments, &ugcItem.m_ulNumComments);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumSecondsPlayedDuringTimePeriod, &ugcItem.m_ulNumSecondsPlayedDuringTimePeriod);
      SteamUGC()->GetQueryUGCStatistic(result->m_handle, i, EItemStatistic::k_EItemStatistic_NumPlaytimeSessionsDuringTimePeriod, &ugcItem.m_ulNumPlaytimeSessionsDuringTimePeriod);
      SteamUGC()->GetQueryUGCPreviewURL(result->m_handle, i, &ugcItem.m_rgchPreviewImageUrl[0], 256);
      SteamUGC()->GetQueryUGCMetadata(result->m_handle, i, &ugcItem.m_rgchMetadata[0], (1024 * 32));
      ugc_items_.push_back(ugcItem);
      // If the file is not existed or last update time is not equal to Steam,
      // download it.
      if (file_update_time == -1 || file_update_time != item.m_rtimeUpdated)
        download_ugc_items_handle_.push_back(item.m_hFile);
    }

    // Start download the file.
    if (download_ugc_items_handle_.size() > 0) {
      SteamAPICall_t download_item_result =
         SteamRemoteStorage()->UGCDownload(
             download_ugc_items_handle_[current_download_items_pos_], 0);
      download_call_result_.Set(download_item_result, this,
          &SynchronizeItemsWorker::OnDownloadCompleted);
      SteamUGC()->ReleaseQueryUGCRequest(result->m_handle);
      return;
    }
  } else {
    SetErrorMessage("Error on querying ugc.");
  }
  is_completed_ = true;
}

void SynchronizeItemsWorker::OnDownloadCompleted(
    RemoteStorageDownloadUGCResult_t* result, bool io_failure) {
  if (io_failure) {
    SetErrorMessage(
        "Error on downloading file: Steam API IO Failure");
  } else if (result->m_eResult == k_EResultOK) {
    std::string target_path = GetAbsoluteFilePath(result->m_pchFileName,
        download_dir_);

    int file_size_in_bytes = result->m_nSizeInBytes;
    auto* content = new char[file_size_in_bytes];

    SteamRemoteStorage()->UGCRead(result->m_hFile,
        content, file_size_in_bytes, 0, k_EUGCRead_Close);
    bool is_save_success = utils::WriteFile(target_path,
        content, file_size_in_bytes);
    delete[] content;

    if (!is_save_success) {
      SetErrorMessage("Error on saving file on local machine.");
      is_completed_ = true;
      return;
    }

    int64 file_updated_time =
        ugc_items_[current_download_items_pos_].m_details.m_rtimeUpdated;
    if (!utils::UpdateFileLastUpdatedTime(
            target_path.c_str(), static_cast<time_t>(file_updated_time))) {
      SetErrorMessage("Error on update file time on local machine.");
      is_completed_ = true;
      return;
    }
    ++current_download_items_pos_;
    if (current_download_items_pos_ < download_ugc_items_handle_.size()) {
      SteamAPICall_t download_item_result = SteamRemoteStorage()->UGCDownload(
          download_ugc_items_handle_[current_download_items_pos_], 0);
      download_call_result_.Set(download_item_result, this,
          &SynchronizeItemsWorker::OnDownloadCompleted);
      return;
    }
  } else {
    char buffer[100];
    sprintf(buffer, "%d: Error on downloading file.", (int)result->m_eResult);
    SetErrorMessage(buffer);
  }
  is_completed_ = true;
}

void SynchronizeItemsWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  v8::Local<v8::Array> items = Nan::New<v8::Array>(
      static_cast<int>(ugc_items_.size()));
  for (size_t i = 0; i < ugc_items_.size(); ++i) {
    v8::Local<v8::Object> item = ConvertToJsObject(ugc_items_[i]);
    bool is_updated = std::find(download_ugc_items_handle_.begin(),
        download_ugc_items_handle_.end(), ugc_items_[i].m_details.m_hFile) !=
        download_ugc_items_handle_.end();
    Nan::Set(item, Nan::New("isUpdated").ToLocalChecked(),
             Nan::New(is_updated));
    Nan::Set(items, i, item);
  }
  v8::Local<v8::Value> argv[] = { items };
  Nan::AsyncResource resource("greenworks:SynchronizeItemsWorker.HandleOKCallback");
  callback->Call(1, argv, &resource);
}

VotePublishedFileWorker::VotePublishedFileWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    PublishedFileId_t unsubscribe_file_id, bool vote_up)
    :SteamCallbackAsyncWorker(success_callback, error_callback),
    file_id_(unsubscribe_file_id), 
    vote_up_(vote_up)
{
}

void VotePublishedFileWorker::OnVoteCompleted(SetUserItemVoteResult_t* result,
    bool io_failure) {
    is_completed_ = true;
}

void VotePublishedFileWorker::Execute() {
    SteamAPICall_t voted_result =
        SteamUGC()->SetUserItemVote(file_id_, vote_up_);
    vote_call_result_.Set(voted_result, this,
        &VotePublishedFileWorker::OnVoteCompleted);

    // Wait for unsubscribing job completed.
    WaitForCompleted();
}


SubscribePublishedFileWorker::SubscribePublishedFileWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    PublishedFileId_t unsubscribe_file_id)
    :SteamCallbackAsyncWorker(success_callback, error_callback),
    subscribe_file_id_(unsubscribe_file_id) {
}

void SubscribePublishedFileWorker::Execute() {
    SteamAPICall_t subscribed_result =
        SteamUGC()->SubscribeItem(subscribe_file_id_);
    subscribe_call_result_.Set(subscribed_result, this,
        &SubscribePublishedFileWorker::OnSubscribeCompleted);

    // Wait for unsubscribing job completed.
    WaitForCompleted();
}

void SubscribePublishedFileWorker::OnSubscribeCompleted(
    RemoteStoragePublishedFileSubscribed_t* result, bool io_failure) {
    is_completed_ = true;
}

UnsubscribePublishedFileWorker::UnsubscribePublishedFileWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    PublishedFileId_t unsubscribe_file_id)
        :SteamCallbackAsyncWorker(success_callback, error_callback),
         unsubscribe_file_id_(unsubscribe_file_id) {
}

void UnsubscribePublishedFileWorker::Execute() {
  SteamAPICall_t unsubscribed_result =
      SteamRemoteStorage()->UnsubscribePublishedFile(unsubscribe_file_id_);
  unsubscribe_call_result_.Set(unsubscribed_result, this,
      &UnsubscribePublishedFileWorker::OnUnsubscribeCompleted);

  // Wait for unsubscribing job completed.
  WaitForCompleted();
}

void UnsubscribePublishedFileWorker::OnUnsubscribeCompleted(
    RemoteStoragePublishedFileUnsubscribed_t* result, bool io_failure) {
  is_completed_ = true;
}

GetItemStateWorker::GetItemStateWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, const PublishedFileId_t& file_id)
    :SteamCallbackAsyncWorker(success_callback, error_callback),
    file_id_(file_id) {
}

void GetItemStateWorker::Execute() {
    // Ignore empty path.
    SteamAPICall_t get_result = SteamUGC()->GetUserItemVote(file_id_);
    call_result_.Set(get_result, this, &GetItemStateWorker::OnGetItemStateCompleted);

    // Wait for FileShare callback result.
    WaitForCompleted();
}

void GetItemStateWorker::OnGetItemStateCompleted(
    GetUserItemVoteResult_t* result, bool io_failure) {
    if (io_failure) {
        SetErrorMessage("Error on sharing file: Steam API IO Failure");
    }
    else if (result->m_eResult == k_EResultOK) {
        item_state_ = SteamUGC()->GetItemState(file_id_);
        voted_up_ = result->m_bVotedUp;
        voted_down_ = result->m_bVotedDown;
    }
    else {
        SetErrorMessage("Error on sharing file on Steam cloud.");
    }
    is_completed_ = true;
}

void GetItemStateWorker::HandleOKCallback() {
    Nan::HandleScope scope;
    v8::Local<v8::Object> result = Nan::New<v8::Object>();

    Nan::Set(result, Nan::New("itemState").ToLocalChecked(), Nan::New(item_state_));
    Nan::Set(result, Nan::New("votedUp").ToLocalChecked(), Nan::New(voted_up_));
    Nan::Set(result, Nan::New("votedDown").ToLocalChecked(), Nan::New(voted_down_));

    v8::Local<v8::Value> argv[] = { result };
    Nan::AsyncResource resource("greenworks:GetItemStateWorker.HandleOKCallback");
    callback->Call(1, argv, &resource);
}

}  // namespace greenworks
