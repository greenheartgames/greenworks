// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_workshop_workers.h"

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

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

  v8::Local<v8::Value> argv[] = { NanNew<v8::Uint32>(
      static_cast<unsigned int>(share_file_handle_)) };
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

  v8::Local<v8::Value> argv[] = { NanNew<v8::Uint32>(
      static_cast<unsigned int>(publish_file_id_)) };
  callback->Call(1, argv);
}

UpdatePublishedWorkshopFileWorker::UpdatePublishedWorkshopFileWorker(
    NanCallback* success_callback, NanCallback* error_callback,
    unsigned int published_file_id, const std::string& file_name,
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

}  // namespace greenworks
