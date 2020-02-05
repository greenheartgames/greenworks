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

PublishWorkshopFileWorker::PublishWorkshopFileWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    uint32 app_id, const WorkshopFileProperties& properties)
    : SteamCallbackAsyncWorker(success_callback, error_callback),
      app_id_(app_id),
      properties_(properties) {}

void PublishWorkshopFileWorker::Execute() {
  SteamParamStringArray_t tags;
  tags.m_nNumStrings = properties_.tags_scratch.size();
  tags.m_ppStrings = reinterpret_cast<const char**>(&properties_.tags);

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

  // Wait for FileShare callback result.
  WaitForCompleted();
}

void PublishWorkshopFileWorker::OnFilePublishCompleted(
    RemoteStoragePublishFileResult_t* result, bool io_failure) {
  if (io_failure) {
    SetErrorMessage("Error on publishing workshop file: Steam API IO Failure");
  } else if (result->m_eResult == k_EResultOK) {
    publish_file_id_ = result->m_nPublishedFileId;
  } else {
    SetErrorMessage("Error on publishing workshop file.");
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
    SteamParamStringArray_t tags;
    if (properties_.tags_scratch.size() == 1 &&
        properties_.tags_scratch.front().empty()) {  // clean the tag.
      tags.m_nNumStrings = 0;
      tags.m_ppStrings = nullptr;
    } else {
      tags.m_nNumStrings = properties_.tags_scratch.size();
      tags.m_ppStrings = reinterpret_cast<const char**>(&properties_.tags);
    }
    SteamRemoteStorage()->UpdatePublishedFileTags(update_handle, &tags);
  }
  SteamAPICall_t commit_update_result =
      SteamRemoteStorage()->CommitPublishedFileUpdate(update_handle);
  update_published_file_call_result_.Set(commit_update_result, this,
      &UpdatePublishedWorkshopFileWorker::
           OnCommitPublishedFileUpdateCompleted);

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
  v8::Local<v8::Value> argv[] = { items };
  Nan::AsyncResource resource("greenworks:QueryUGCWorker.HandleOKCallback");
  callback->Call(1, argv, &resource);
}

void QueryUGCWorker::OnUGCQueryCompleted(SteamUGCQueryCompleted_t* result,
    bool io_failure) {
  if (io_failure) {
    SetErrorMessage("Error on querying all ugc: Steam API IO Failure");
  } else if (result->m_eResult == k_EResultOK) {
    uint32 count = result->m_unNumResultsReturned;
    SteamUGCDetails_t item;
    for (uint32 i = 0; i < count; ++i) {
      SteamUGC()->GetQueryUGCResult(result->m_handle, i, &item);
      ugc_items_.push_back(item);
    }
    SteamUGC()->ReleaseQueryUGCRequest(result->m_handle);
  } else {
    SetErrorMessage("Error on querying ugc.");
  }
  is_completed_ = true;
}

QueryAllUGCWorker::QueryAllUGCWorker(Nan::Callback* success_callback,
                                     Nan::Callback* error_callback,
                                     EUGCMatchingUGCType ugc_matching_type,
                                     EUGCQuery ugc_query_type, uint32 app_id,
                                     uint32 page_num)
    : QueryUGCWorker(success_callback, error_callback, ugc_matching_type,
                     app_id, page_num),
      ugc_query_type_(ugc_query_type) {}

void QueryAllUGCWorker::Execute() {
  uint32 invalid_app_id = 0;
  // Set "creator_app_id" parameter to an invalid id to make Steam API return
  // all ugc items, otherwise the API won't get any results in some cases.
  UGCQueryHandle_t ugc_handle = SteamUGC()->CreateQueryAllUGCRequest(
      ugc_query_type_, ugc_matching_type_, /*creator_app_id=*/invalid_app_id,
      /*consumer_app_id=*/app_id_, page_num_);
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
         download_dir_(download_dir) {
}

void DownloadItemWorker::Execute() {
  SteamAPICall_t download_item_result =
     SteamRemoteStorage()->UGCDownload(download_file_handle_, 0);
  call_result_.Set(download_item_result, this,
      &DownloadItemWorker::OnDownloadCompleted);

  // Wait for downloading file completed.
  WaitForCompleted();
}

void DownloadItemWorker::OnDownloadCompleted(
    RemoteStorageDownloadUGCResult_t* result, bool io_failure) {
  if (io_failure) {
    SetErrorMessage(
        "Error on downloading file: Steam API IO Failure");
  } else if (result->m_eResult == k_EResultOK) {
    std::string target_path = GetAbsoluteFilePath(result->m_pchFileName,
        download_dir_);

    int file_size_in_bytes = result->m_nSizeInBytes;
    auto* content = new char[file_size_in_bytes];

    SteamRemoteStorage()->UGCRead(download_file_handle_,
        content, file_size_in_bytes, 0, k_EUGCRead_Close);
    if (!utils::WriteFile(target_path, content, file_size_in_bytes)) {
      SetErrorMessage("Error on saving file on local machine.");
    }
    delete[] content;
  } else {
    SetErrorMessage("Error on downloading file.");
  }
  is_completed_ = true;
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
      ugc_items_.push_back(item);
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
        ugc_items_[current_download_items_pos_].m_rtimeUpdated;
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
    SetErrorMessage("Error on downloading file.");
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
        download_ugc_items_handle_.end(), ugc_items_[i].m_hFile) !=
        download_ugc_items_handle_.end();
    Nan::Set(item, Nan::New("isUpdated").ToLocalChecked(),
             Nan::New(is_updated));
    Nan::Set(items, i, item);
  }
  v8::Local<v8::Value> argv[] = { items };
  Nan::AsyncResource resource("greenworks:SynchronizeItemsWorker.HandleOKCallback");
  callback->Call(1, argv, &resource);
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

}  // namespace greenworks
