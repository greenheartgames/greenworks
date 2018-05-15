// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_GREENWORKS_WORKSHOP_WORKERS_H_
#define SRC_GREENWORKS_WORKSHOP_WORKERS_H_

#include "steam_async_worker.h"

#include <vector>

#include "steam/steam_api.h"

namespace greenworks {

class FileShareWorker : public SteamCallbackAsyncWorker {
 public:
  FileShareWorker(Nan::Callback* success_callback,
                  Nan::Callback* error_callback,
                  const std::string& file_path);
  void OnFileShareCompleted(RemoteStorageFileShareResult_t* result,
                            bool io_failure);

  // Override Nan::AsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  const std::string file_path_;
  UGCHandle_t share_file_handle_;
  CCallResult<FileShareWorker, RemoteStorageFileShareResult_t> call_result_;
};

class PublishWorkshopFileWorker : public SteamCallbackAsyncWorker {
 public:
  PublishWorkshopFileWorker(Nan::Callback* success_callback,
                            Nan::Callback* error_callback,
                            const std::string& file_path,
                            const std::string& image_path,
                            const std::string& title,
                            const std::string& description);
  void OnFilePublishCompleted(RemoteStoragePublishFileResult_t* result,
                              bool io_failure);

  // Override Nan::AsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  std::string file_path_;
  std::string image_path_;
  std::string title_;
  std::string description_;

  PublishedFileId_t publish_file_id_;
  CCallResult<PublishWorkshopFileWorker,
      RemoteStoragePublishFileResult_t> call_result_;
};

class UpdatePublishedWorkshopFileWorker : public SteamCallbackAsyncWorker {
 public:
  UpdatePublishedWorkshopFileWorker(Nan::Callback* success_callback,
                                    Nan::Callback* error_callback,
                                    PublishedFileId_t published_file_id,
                                    const std::string& file_path,
                                    const std::string& image_path,
                                    const std::string& title,
                                    const std::string& description);
  void OnCommitPublishedFileUpdateCompleted(
      RemoteStorageUpdatePublishedFileResult_t* result, bool io_failure);

  // Override Nan::AsyncWorker methods.
  virtual void Execute();

 private:
  PublishedFileId_t published_file_id_;
  std::string file_path_;
  std::string image_path_;
  std::string title_;
  std::string description_;

  CCallResult<UpdatePublishedWorkshopFileWorker,
      RemoteStorageUpdatePublishedFileResult_t>
          update_published_file_call_result_;
};

// A base worker class for querying (user/all) ugc.
class QueryUGCWorker : public SteamCallbackAsyncWorker {
 public:
  QueryUGCWorker(Nan::Callback* success_callback, Nan::Callback* error_callback,
                 EUGCMatchingUGCType ugc_matching_type, uint32 app_id,
                 uint32 page_num);
  void OnUGCQueryCompleted(SteamUGCQueryCompleted_t* result,
                           bool io_failure);

  // Override Nan::AsyncWorker methods.
  virtual void HandleOKCallback();

 protected:
  EUGCMatchingUGCType ugc_matching_type_;
  std::vector<SteamUGCDetails_t> ugc_items_;
  uint32 app_id_;
  uint32 page_num_;

  CCallResult<QueryUGCWorker,
      SteamUGCQueryCompleted_t> ugc_query_call_result_;
};

class QueryAllUGCWorker : public QueryUGCWorker {
 public:
  QueryAllUGCWorker(Nan::Callback* success_callback,
                    Nan::Callback* error_callback,
                    EUGCMatchingUGCType ugc_matching_type,
                    EUGCQuery ugc_query_type, uint32 app_id, uint32 page_num);

  // Override Nan::AsyncWorker methods.
  virtual void Execute();

 private:
  EUGCQuery ugc_query_type_;
};

class QueryUserUGCWorker : public QueryUGCWorker {
 public:
  QueryUserUGCWorker(Nan::Callback* success_callback,
                     Nan::Callback* error_callback,
                     EUGCMatchingUGCType ugc_matching_type,
                     EUserUGCList ugc_list,
                     EUserUGCListSortOrder ugc_list_sort_order, uint32 app_id,
                     uint32 page_num);

  // Override Nan::AsyncWorker methods.
  virtual void Execute();

 private:
  EUserUGCList ugc_list_;
  EUserUGCListSortOrder ugc_list_sort_order_;
};

class DownloadItemWorker : public SteamCallbackAsyncWorker {
 public:
  DownloadItemWorker(Nan::Callback* success_callback,
                     Nan::Callback* error_callback,
                     UGCHandle_t download_file_handle,
                     const std::string& download_dir);

  void OnDownloadCompleted(RemoteStorageDownloadUGCResult_t* result,
      bool io_failure);

  // Override Nan::AsyncWorker methods.
  virtual void Execute();

 private:
  UGCHandle_t download_file_handle_;
  std::string download_dir_;
  CCallResult<DownloadItemWorker,
      RemoteStorageDownloadUGCResult_t> call_result_;
};

class SynchronizeItemsWorker : public SteamCallbackAsyncWorker {
 public:
  SynchronizeItemsWorker(Nan::Callback* success_callback,
                         Nan::Callback* error_callback,
                         const std::string& download_dir);

  void OnUGCQueryCompleted(SteamUGCQueryCompleted_t* result,
                           bool io_failure);
  void OnDownloadCompleted(RemoteStorageDownloadUGCResult_t* result,
      bool io_failure);

  // Override Nan::AsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  size_t current_download_items_pos_;
  std::string download_dir_;
  std::vector<SteamUGCDetails_t> ugc_items_;
  std::vector<UGCHandle_t> download_ugc_items_handle_;
  CCallResult<SynchronizeItemsWorker,
      RemoteStorageDownloadUGCResult_t> download_call_result_;
  CCallResult<SynchronizeItemsWorker,
      SteamUGCQueryCompleted_t> ugc_query_call_result_;
};

class UnsubscribePublishedFileWorker : public SteamCallbackAsyncWorker {
 public:
  UnsubscribePublishedFileWorker(Nan::Callback* success_callback,
                                 Nan::Callback* error_callback,
                                 PublishedFileId_t unsubscribe_file_id);

  void OnUnsubscribeCompleted(RemoteStoragePublishedFileUnsubscribed_t* result,
      bool io_failure);

  // Override Nan::AsyncWorker methods.
  virtual void Execute();

 private:
  PublishedFileId_t unsubscribe_file_id_;

  CCallResult<UnsubscribePublishedFileWorker,
      RemoteStoragePublishedFileUnsubscribed_t> unsubscribe_call_result_;
};

}  // namespace greenworks

#endif  // SRC_GREENWORKS_WORKSHOP_WORKERS_H_
