// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_workshop_workers.h"

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

}

namespace greenworks {

FileShareWorker::FileShareWorker(NanCallback* success_callback,
    NanCallback* error_callback, const std::string& file_name)
        :SteamCallbackAsyncWorker(success_callback, error_callback),
         file_name_(file_name) {
}

void FileShareWorker::Execute() {
  // Ignore empty path.
  if (file_name_.empty()) return;

  SteamAPICall_t share_result = SteamRemoteStorage()->FileShare(
      file_name_.c_str());
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
    const std::string& file_name, const std::string& image_name,
    const std::string& title, const std::string& description):
        SteamCallbackAsyncWorker(success_callback, error_callback),
        file_name_(file_name),
        image_name_(image_name),
        title_(title),
        description_(description) {
}

void PublishWorkshopFileWorker::Execute() {
  SteamParamStringArray_t tags;
  tags.m_nNumStrings = 0;
  SteamAPICall_t publish_result = SteamRemoteStorage()->PublishWorkshopFile(
      file_name_.c_str(),
      image_name_.empty()? NULL:image_name_.c_str(),
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
    PublishedFileId_t published_file_id, const std::string& file_name,
    const std::string& image_name, const std::string& title,
    const std::string& description):
        SteamCallbackAsyncWorker(success_callback, error_callback),
        published_file_id_(published_file_id),
        file_name_(file_name),
        image_name_(image_name),
        title_(title),
        description_(description) {
}

void UpdatePublishedWorkshopFileWorker::Execute() {
  PublishedFileUpdateHandle_t update_handle =
      SteamRemoteStorage()->CreatePublishedFileUpdateRequest(
          published_file_id_);

  if (!file_name_.empty())
    SteamRemoteStorage()->UpdatePublishedFileFile(update_handle,
        file_name_.c_str());
  if (!image_name_.empty())
    SteamRemoteStorage()->UpdatePublishedFilePreviewFile(update_handle,
        image_name_.c_str());
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
  v8::Local<v8::Value> argv[] = {
      NanNew<v8::Uint32>(static_cast<unsigned int>(ugc_items_.size())), items };
  callback->Call(2, argv);
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

}  // namespace greenworks