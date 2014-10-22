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
  v8::Local<v8::Object> result = NanNew<v8::Object>();

  result->Set(NanNew("acceptedForUse"), NanNew(item.m_bAcceptedForUse));
  result->Set(NanNew("banned"), NanNew(item.m_bBanned));
  result->Set(NanNew("tagsTruncated"), NanNew(item.m_bTagsTruncated));
  result->Set(NanNew("fileType"), NanNew(item.m_eFileType));
  result->Set(NanNew("result"), NanNew(item.m_eResult));
  result->Set(NanNew("visibility"), NanNew(item.m_eVisibility));
  result->Set(NanNew("score"), NanNew(item.m_flScore));

  result->Set(NanNew("file"),
              NanNew<v8::String>(utils::uint64ToString(item.m_hFile)));
  result->Set(NanNew("fileName"), NanNew(item.m_pchFileName));
  result->Set(NanNew("fileSize"), NanNew(item.m_nFileSize));

  result->Set(NanNew("previewFile"),
              NanNew<v8::String>(utils::uint64ToString(item.m_hPreviewFile)));
  result->Set(NanNew("previewFileSize"), NanNew(item.m_nPreviewFileSize));

  result->Set(NanNew("steamIDOwner"),
              NanNew<v8::String>(utils::uint64ToString(item.m_ulSteamIDOwner)));
  result->Set(NanNew("consumerAppID"), NanNew(item.m_nConsumerAppID));
  result->Set(NanNew("creatorAppID"), NanNew(item.m_nCreatorAppID));
  result->Set(NanNew("publishedFileId"),
              NanNew<v8::String>(utils::uint64ToString(
                  item.m_nPublishedFileId)));

  result->Set(NanNew("title"), NanNew(item.m_rgchTitle));
  result->Set(NanNew("description"), NanNew(item.m_rgchDescription));
  result->Set(NanNew("URL"), NanNew(item.m_rgchURL));
  result->Set(NanNew("tags"), NanNew(item.m_rgchTags));

  result->Set(NanNew("timeAddedToUserList"), NanNew(
      item.m_rtimeAddedToUserList));
  result->Set(NanNew("timeCreated"), NanNew(item.m_rtimeCreated));
  result->Set(NanNew("timeUpdated"), NanNew(item.m_rtimeUpdated));
  result->Set(NanNew("votesDown"), NanNew(item.m_unVotesDown));
  result->Set(NanNew("votesUp"), NanNew(item.m_unVotesUp));

  return result;
}

inline std::string GetAbsoluteFilePath(const std::string& file_path,
    const std::string& download_dir) {
  std::string file_name = file_path.substr(file_path.find_last_of("/\\") + 1);
  return download_dir + "/" + file_name;
}

}

namespace greenworks {

FileShareWorker::FileShareWorker(NanCallback* success_callback,
    NanCallback* error_callback, const std::string& file_path)
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
  NanScope();

  v8::Local<v8::Value> argv[] = { NanNew<v8::String>(
      utils::uint64ToString(share_file_handle_)) };
  callback->Call(1, argv);
}

PublishWorkshopFileWorker::PublishWorkshopFileWorker(
    NanCallback* success_callback, NanCallback* error_callback,
    const std::string& file_path, const std::string& image_path,
    const std::string& title, const std::string& description):
        SteamCallbackAsyncWorker(success_callback, error_callback),
        file_path_(file_path),
        image_path_(image_path),
        title_(title),
        description_(description) {
}

void PublishWorkshopFileWorker::Execute() {
  SteamParamStringArray_t tags;
  tags.m_nNumStrings = 0;
  std::string file_name = utils::GetFileNameFromPath(file_path_);
  std::string image_name = utils::GetFileNameFromPath(image_path_);
  SteamAPICall_t publish_result = SteamRemoteStorage()->PublishWorkshopFile(
      file_name.c_str(),
      image_name.empty()? NULL:image_name.c_str(),
      SteamUtils()->GetAppID(),
      title_.c_str(),
      description_.empty()? NULL:description_.c_str(),
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
  NanScope();

  v8::Local<v8::Value> argv[] = { NanNew<v8::String>(
      utils::uint64ToString(publish_file_id_)) };
  callback->Call(1, argv);
}

UpdatePublishedWorkshopFileWorker::UpdatePublishedWorkshopFileWorker(
    NanCallback* success_callback, NanCallback* error_callback,
    PublishedFileId_t published_file_id, const std::string& file_path,
    const std::string& image_path, const std::string& title,
    const std::string& description):
        SteamCallbackAsyncWorker(success_callback, error_callback),
        published_file_id_(published_file_id),
        file_path_(file_path),
        image_path_(image_path),
        title_(title),
        description_(description) {
}

void UpdatePublishedWorkshopFileWorker::Execute() {
  PublishedFileUpdateHandle_t update_handle =
      SteamRemoteStorage()->CreatePublishedFileUpdateRequest(
          published_file_id_);

  const std::string file_name = utils::GetFileNameFromPath(file_path_);
  const std::string image_name = utils::GetFileNameFromPath(image_path_);
  if (!file_name.empty())
    SteamRemoteStorage()->UpdatePublishedFileFile(update_handle,
        file_name.c_str());
  if (!image_name.empty())
    SteamRemoteStorage()->UpdatePublishedFilePreviewFile(update_handle,
        image_name.c_str());
  if (!title_.empty())
    SteamRemoteStorage()->UpdatePublishedFileTitle(update_handle,
        title_.c_str());
  if (!description_.empty())
    SteamRemoteStorage()->UpdatePublishedFileDescription(update_handle,
        description_.c_str());
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

QueryUGCWorker::QueryUGCWorker(NanCallback* success_callback,
    NanCallback* error_callback, EUGCMatchingUGCType ugc_matching_type)
        :SteamCallbackAsyncWorker(success_callback, error_callback),
         ugc_matching_type_(ugc_matching_type) {
}


void QueryUGCWorker::HandleOKCallback() {
  NanScope();

  v8::Local<v8::Array> items = NanNew<v8::Array>(ugc_items_.size());
  for (size_t i = 0; i < ugc_items_.size(); ++i)
    items->Set(i, ConvertToJsObject(ugc_items_[i]));
  v8::Local<v8::Value> argv[] = { items };
  callback->Call(1, argv);
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

QueryAllUGCWorker::QueryAllUGCWorker(NanCallback* success_callback,
    NanCallback* error_callback, EUGCMatchingUGCType ugc_matching_type,
    EUGCQuery ugc_query_type)
        :QueryUGCWorker(success_callback, error_callback, ugc_matching_type),
         ugc_query_type_(ugc_query_type) {
}

void QueryAllUGCWorker::Execute() {
  uint32 app_id = SteamUtils()->GetAppID();
  UGCQueryHandle_t ugc_handle = SteamUGC()->CreateQueryAllUGCRequest(
      ugc_query_type_, ugc_matching_type_, app_id, app_id, 1);
  SteamAPICall_t ugc_query_result = SteamUGC()->SendQueryUGCRequest(ugc_handle);
  ugc_query_call_result_.Set(ugc_query_result, this,
      &QueryAllUGCWorker::OnUGCQueryCompleted);

  // Wait for query all ugc completed.
  WaitForCompleted();
}

QueryUserUGCWorker::QueryUserUGCWorker(NanCallback* success_callback,
    NanCallback* error_callback, EUGCMatchingUGCType ugc_matching_type,
    EUserUGCList ugc_list, EUserUGCListSortOrder ugc_list_sort_order)
        :QueryUGCWorker(success_callback, error_callback, ugc_matching_type),
         ugc_list_(ugc_list),
         ugc_list_sort_order_(ugc_list_sort_order) {
}

void QueryUserUGCWorker::Execute() {
  uint32 app_id = SteamUtils()->GetAppID();

  UGCQueryHandle_t ugc_handle = SteamUGC()->CreateQueryUserUGCRequest(
      SteamUser()->GetSteamID().GetAccountID(),
      ugc_list_,
      ugc_matching_type_,
      ugc_list_sort_order_,
      app_id,
      app_id,
      1);
  SteamAPICall_t ugc_query_result = SteamUGC()->SendQueryUGCRequest(ugc_handle);
  ugc_query_call_result_.Set(ugc_query_result, this,
      &QueryUserUGCWorker::OnUGCQueryCompleted);

  // Wait for query all ugc completed.
  WaitForCompleted();
}

DownloadItemWorker::DownloadItemWorker(NanCallback* success_callback,
    NanCallback* error_callback, UGCHandle_t download_file_handle,
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
    char* content = new char[file_size_in_bytes];

    SteamRemoteStorage()->UGCRead(download_file_handle_,
        content, file_size_in_bytes, 0, k_EUGCRead_Close);
    if (!utils::WriteFile(target_path, content, file_size_in_bytes)) {
      SetErrorMessage("Error on saving file on local machine.");
    }
    delete content;
  } else {
    SetErrorMessage("Error on downloading file.");
  }
  is_completed_ = true;
}

SynchronizeItemsWorker::SynchronizeItemsWorker(NanCallback* success_callback,
    NanCallback* error_callback, const std::string& download_dir)
        :SteamCallbackAsyncWorker(success_callback, error_callback),
         current_download_items_pos_(0),
         download_dir_(download_dir) {
}

void SynchronizeItemsWorker::Execute() {
  uint32 app_id = SteamUtils()->GetAppID();

  UGCQueryHandle_t ugc_handle = SteamUGC()->CreateQueryUserUGCRequest(
      SteamUser()->GetSteamID().GetAccountID(),
      k_EUserUGCList_Subscribed,
      k_EUGCMatchingUGCType_Items,
      k_EUserUGCListSortOrder_SubscriptionDateDesc,
      app_id,
      app_id,
      1);
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
    char* content = new char[file_size_in_bytes];

    SteamRemoteStorage()->UGCRead(result->m_hFile,
        content, file_size_in_bytes, 0, k_EUGCRead_Close);
    bool is_save_success = utils::WriteFile(target_path,
        content, file_size_in_bytes);
    delete content;

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
  NanScope();

  v8::Local<v8::Array> items = NanNew<v8::Array>(ugc_items_.size());
  for (size_t i = 0; i < ugc_items_.size(); ++i) {
    v8::Local<v8::Object> item = ConvertToJsObject(ugc_items_[i]);
    bool is_updated = std::find(download_ugc_items_handle_.begin(),
        download_ugc_items_handle_.end(), ugc_items_[i].m_hFile) !=
        download_ugc_items_handle_.end();
    item->Set(NanNew("isUpdated"), NanNew(is_updated));
    items->Set(i, item);
  }
  v8::Local<v8::Value> argv[] = { items };
  callback->Call(1, argv);
}

UnsubscribePublishedFileWorker::UnsubscribePublishedFileWorker(
    NanCallback* success_callback, NanCallback* error_callback,
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
