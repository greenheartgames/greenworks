// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_GREENWORKS_WORKSHOP_WORKERS_H_
#define SRC_GREENWORKS_WORKSHOP_WORKERS_H_

#include "steam_async_worker.h"

#include <vector>

#include "steam/steam_api.h"

// Super-set of SteamUGCDetails_t
struct UGCItem {
    SteamUGCDetails_t m_details;
    uint64 m_ulNumSubscriptions;
    uint64 m_ulNumFavorites;
    uint64 m_ulNumFollowers;
    uint64 m_ulNumUniqueSubscriptions;
    uint64 m_ulNumUniqueFavorites;
    uint64 m_ulNumUniqueFollowers;
    uint64 m_ulNumUniqueWebsiteViews;
    uint64 m_ulReportScore;
    uint64 m_ulNumSecondsPlayed;
    uint64 m_ulNumPlaytimeSessions;
    uint64 m_ulNumComments;
    uint64 m_ulNumSecondsPlayedDuringTimePeriod;
    uint64 m_ulNumPlaytimeSessionsDuringTimePeriod;
    char m_rgchPreviewImageUrl[k_cchPublishedFileURLMax];
    char m_rgchMetadata[1024 * 32];
};

namespace greenworks {

class FileShareWorker : public SteamCallbackAsyncWorker {
 public:
  FileShareWorker(Nan::Callback* success_callback,
                  Nan::Callback* error_callback,
                  const std::string& file_path);
  void OnFileShareCompleted(RemoteStorageFileShareResult_t* result,
                            bool io_failure);

  void Execute() override;
  void HandleOKCallback() override;

 private:
  const std::string file_path_;
  UGCHandle_t share_file_handle_;
  CCallResult<FileShareWorker, RemoteStorageFileShareResult_t> call_result_;
};

struct WorkshopFileProperties {
  static constexpr int MAX_TAGS = 100;
  std::string file_path;
  std::string image_path;
  std::string title;
  std::string description;

  std::vector<std::string> tags_scratch;
  const char* tags[MAX_TAGS];
};

struct WorkshopItemFilter {
    static constexpr int MAX_TAGS = 100;
    const char* tags[MAX_TAGS];
    const char* excludedTags[MAX_TAGS];
};

class PublishWorkshopFileWorker : public SteamCallbackAsyncWorker {
 public:
  PublishWorkshopFileWorker(Nan::Callback* success_callback,
                            Nan::Callback* error_callback,
                            uint32 app_id,
                            const WorkshopFileProperties& properties);
  void OnFilePublishCompleted(RemoteStoragePublishFileResult_t* result,
                              bool io_failure);

  void Execute() override;
  void HandleOKCallback() override;

 private:
  uint32 app_id_;
  WorkshopFileProperties properties_;

  PublishedFileId_t publish_file_id_;
  CCallResult<PublishWorkshopFileWorker,
      RemoteStoragePublishFileResult_t> call_result_;
};

class UpdatePublishedWorkshopFileWorker : public SteamCallbackAsyncWorker {
 public:
  UpdatePublishedWorkshopFileWorker(Nan::Callback* success_callback,
                                    Nan::Callback* error_callback,
                                    PublishedFileId_t published_file_id,
                                    const WorkshopFileProperties& properties);
  void OnCommitPublishedFileUpdateCompleted(
      RemoteStorageUpdatePublishedFileResult_t* result, bool io_failure);

  void Execute() override;

 private:
  PublishedFileId_t published_file_id_;
  WorkshopFileProperties properties_;

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

  void HandleOKCallback() override;

 protected:
  EUGCMatchingUGCType ugc_matching_type_;
  //std::vector<SteamUGCDetails_t> ugc_items_;
  std::vector<UGCItem> ugc_items_;
  uint32 num_results_;
  uint32 num_total_results_;
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

  void Execute() override;

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

  void Execute() override;

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

  void Execute() override;

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
                         const std::string& download_dir,
                         uint32 app_id,
                         uint32 page_num);

  void OnUGCQueryCompleted(SteamUGCQueryCompleted_t* result,
                           bool io_failure);
  void OnDownloadCompleted(RemoteStorageDownloadUGCResult_t* result,
      bool io_failure);

  void Execute() override;
  void HandleOKCallback() override;

 private:
  size_t current_download_items_pos_;
  std::string download_dir_;
  //std::vector<SteamUGCDetails_t> ugc_items_;
  std::vector<UGCItem> ugc_items_;
  uint32 num_results_;
  uint32 num_total_results_;
  std::vector<UGCHandle_t> download_ugc_items_handle_;
  uint32 app_id_;
  uint32 page_num_;
  CCallResult<SynchronizeItemsWorker,
      RemoteStorageDownloadUGCResult_t> download_call_result_;
  CCallResult<SynchronizeItemsWorker,
      SteamUGCQueryCompleted_t> ugc_query_call_result_;
};

class VotePublishedFileWorker : public SteamCallbackAsyncWorker {
public:
    VotePublishedFileWorker(Nan::Callback* success_callback,
        Nan::Callback* error_callback,
        PublishedFileId_t file_id, bool vote_up);

    void OnVoteCompleted(SetUserItemVoteResult_t* result,
        bool io_failure);

    void Execute() override;

private:
    PublishedFileId_t file_id_;
    bool vote_up_;

    CCallResult<VotePublishedFileWorker,
        SetUserItemVoteResult_t> vote_call_result_;
};

class SubscribePublishedFileWorker : public SteamCallbackAsyncWorker {
public:
    SubscribePublishedFileWorker(Nan::Callback* success_callback,
        Nan::Callback* error_callback,
        PublishedFileId_t subscribe_file_id);

    void OnSubscribeCompleted(RemoteStoragePublishedFileSubscribed_t* result,
        bool io_failure);

    void Execute() override;

private:
    PublishedFileId_t subscribe_file_id_;

    CCallResult<SubscribePublishedFileWorker,
        RemoteStoragePublishedFileSubscribed_t> subscribe_call_result_;
};

class UnsubscribePublishedFileWorker : public SteamCallbackAsyncWorker {
 public:
  UnsubscribePublishedFileWorker(Nan::Callback* success_callback,
                                 Nan::Callback* error_callback,
                                 PublishedFileId_t unsubscribe_file_id);

  void OnUnsubscribeCompleted(RemoteStoragePublishedFileUnsubscribed_t* result,
      bool io_failure);

  void Execute() override;

 private:
  PublishedFileId_t unsubscribe_file_id_;

  CCallResult<UnsubscribePublishedFileWorker,
      RemoteStoragePublishedFileUnsubscribed_t> unsubscribe_call_result_;
};

class GetItemStateWorker : public SteamCallbackAsyncWorker {
public:
    GetItemStateWorker(Nan::Callback* success_callback,
        Nan::Callback* error_callback,
        const PublishedFileId_t& file_id);
    void OnGetItemStateCompleted(GetUserItemVoteResult_t* result,
        bool io_failure);

    void Execute() override;
    void HandleOKCallback() override;

private:
    PublishedFileId_t file_id_;

    uint32 item_state_;
    bool voted_up_;
    bool voted_down_;

    CCallResult<GetItemStateWorker, GetUserItemVoteResult_t> call_result_;
};

}  // namespace greenworks

#endif  // SRC_GREENWORKS_WORKSHOP_WORKERS_H_
