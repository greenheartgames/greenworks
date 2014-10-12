// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_GREENWORK_ASYNC_WORKERS_H_
#define SRC_GREENWORK_ASYNC_WORKERS_H_

#include <string>
#include <vector>

#include "steam/steam_api.h"

#include "steam_async_worker.h"
#include "greenworks_utils.h"

namespace greenworks {

class FileContentSaveWorker : public SteamAsyncWorker {
 public:
  FileContentSaveWorker(NanCallback* success_callback,
      NanCallback* error_callback, std::string file_name, std::string content);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::string file_name_;
  std::string content_;
};

class FilesSaveWorker : public SteamAsyncWorker {
 public:
  FilesSaveWorker(NanCallback* success_callback, NanCallback* error_callback,
      const std::vector<std::string>& files_path);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::vector<std::string> files_path_;
};

class FileReadWorker : public SteamAsyncWorker {
 public:
  FileReadWorker(NanCallback* success_callback, NanCallback* error_callback,
      std::string file_name);

  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  std::string file_name_;
  std::string content_;
};

class CloudQuotaGetWorker : public SteamAsyncWorker {
 public:
  CloudQuotaGetWorker(NanCallback* success_callback,
      NanCallback* error_callback);

  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
	int total_bytes_;
	int available_bytes_;
};

class ActivateAchievementWorker : public SteamAsyncWorker {
 public:
  ActivateAchievementWorker(NanCallback* success_callback,
      NanCallback* error_callback, const std::string& achievement);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::string achievement_;
};

class GetNumberOfPlayersWorker : public SteamAsyncWorker {
 public:
  GetNumberOfPlayersWorker(NanCallback* success_callback,
                          NanCallback* error_callback);
	void OnGetNumberOfPlayersCompleted(NumberOfCurrentPlayers_t* result,
                                     bool io_failure);
  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  bool is_completed_;
  int num_of_players_;
  CCallResult<GetNumberOfPlayersWorker, NumberOfCurrentPlayers_t> call_result_;
};

class FileShareWorker : public SteamAsyncWorker {
 public:
  FileShareWorker(NanCallback* success_callback,
                  NanCallback* error_callback,
                  const std::string& file_name);
  void OnFileShareCompleted(RemoteStorageFileShareResult_t* result,
                            bool io_failure);

  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  bool is_completed_;
  const std::string file_name_;
  UGCHandle_t share_file_handle_;
  CCallResult<FileShareWorker, RemoteStorageFileShareResult_t> call_result_;
};

class PublishWorkshopFileWorker : public SteamAsyncWorker {
 public:
  PublishWorkshopFileWorker(NanCallback* success_callback,
                            NanCallback* error_callback,
                            const std::string& file_name,
                            const std::string& image_name,
                            const std::string& title,
                            const std::string& description);
  void OnFilePublishCompleted(RemoteStoragePublishFileResult_t* result,
                              bool io_failure);

  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  bool is_completed_;
  std::string file_name_;
  std::string image_name_;
  std::string title_;
  std::string description_;

  PublishedFileId_t publish_file_id_;
  CCallResult<PublishWorkshopFileWorker,
      RemoteStoragePublishFileResult_t> call_result_;
};

class UpdatePublishedWorkshopFileWorker : public SteamAsyncWorker {
 public:
  UpdatePublishedWorkshopFileWorker(NanCallback* success_callback,
                                  NanCallback* error_callback,
                                  unsigned int published_file_id,
                                  const std::string& file_name,
                                  const std::string& image_name,
                                  const std::string& title,
                                  const std::string& description);
  void OnCommitPublishedFileUpdateCompleted(
      RemoteStorageUpdatePublishedFileResult_t* result, bool io_failure);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  bool is_completed_;
  PublishedFileId_t published_file_id_;
  std::string file_name_;
  std::string image_name_;
  std::string title_;
  std::string description_;

  CCallResult<UpdatePublishedWorkshopFileWorker,
      RemoteStorageUpdatePublishedFileResult_t>
          update_published_file_call_result_;
};

}  // namespace greenworks

#endif  // SRC_GREENWORK_ASYNC_WORKERS_H_
