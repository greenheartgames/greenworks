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
#include "greenworks_workshop_workers.h"

namespace greenworks {

class FileContentSaveWorker : public SteamAsyncWorker {
 public:
  FileContentSaveWorker(Nan::Callback* success_callback,
      Nan::Callback* error_callback, std::string file_name, std::string content);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::string file_name_;
  std::string content_;
};

class FilesSaveWorker : public SteamAsyncWorker {
 public:
  FilesSaveWorker(Nan::Callback* success_callback, Nan::Callback* error_callback,
      const std::vector<std::string>& files_path);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::vector<std::string> files_path_;
};

class FileReadWorker : public SteamAsyncWorker {
 public:
  FileReadWorker(Nan::Callback* success_callback, Nan::Callback* error_callback,
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
  CloudQuotaGetWorker(Nan::Callback* success_callback,
      Nan::Callback* error_callback);

  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  int total_bytes_;
  int available_bytes_;
};

class ActivateAchievementWorker : public SteamAsyncWorker {
 public:
  ActivateAchievementWorker(Nan::Callback* success_callback,
      Nan::Callback* error_callback, const std::string& achievement);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::string achievement_;
};

class GetAchievementWorker : public SteamAsyncWorker {
 public:
  GetAchievementWorker(Nan::Callback* success_callback,
                       Nan::Callback* error_callback,
                       const std::string& achievement);

  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  std::string achievement_;
  bool is_achieved_;
};

class ClearAchievementWorker : public SteamAsyncWorker {
 public:
  ClearAchievementWorker(Nan::Callback* success_callback,
                       Nan::Callback* error_callback,
                       const std::string& achievement);

  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  std::string achievement_;
  bool success_;
};

class GetNumberOfPlayersWorker : public SteamCallbackAsyncWorker {
 public:
  GetNumberOfPlayersWorker(Nan::Callback* success_callback,
                           Nan::Callback* error_callback);
  void OnGetNumberOfPlayersCompleted(NumberOfCurrentPlayers_t* result,
                                     bool io_failure);
  // Override NanAsyncWorker methods.
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  int num_of_players_;
  CCallResult<GetNumberOfPlayersWorker, NumberOfCurrentPlayers_t> call_result_;
};

class CreateArchiveWorker : public SteamAsyncWorker {
 public:
  CreateArchiveWorker(Nan::Callback* success_callback,
                      Nan::Callback* error_callback,
                      const std::string& zip_file_path,
                      const std::string& source_dir,
                      const std::string& password,
                      int compress_level);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::string zip_file_path_;
  std::string source_dir_;
  std::string password_;
  int compress_level_;
};

class ExtractArchiveWorker : public SteamAsyncWorker {
 public:
  ExtractArchiveWorker(Nan::Callback* success_callback,
                       Nan::Callback* error_callback,
                       const std::string& zip_file_path,
                       const std::string& extract_path,
                       const std::string& password);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::string zip_file_path_;
  std::string extract_path_;
  std::string password_;
};

class GetAuthSessionTicketWorker : public SteamCallbackAsyncWorker {
 public:
  GetAuthSessionTicketWorker(Nan::Callback* success_callback,
                             Nan::Callback* error_callback);
  STEAM_CALLBACK(GetAuthSessionTicketWorker,
                 OnGetAuthSessionCompleted,
                 GetAuthSessionTicketResponse_t,
                 result);
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  std::string ticket_;
  HAuthTicket handle_;
  unsigned int ticket_buf_size_;
  unsigned char ticket_buf_[2048];
};

class RequestEncryptedAppTicketWorker : public SteamCallbackAsyncWorker {
 public:
  RequestEncryptedAppTicketWorker(std::string user_data,
                                  Nan::Callback* success_callback,
                                  Nan::Callback* error_callback);
  void OnRequestEncryptedAppTicketCompleted(
      EncryptedAppTicketResponse_t*, bool);
  virtual void Execute();
  virtual void HandleOKCallback();

 private:
  std::string user_data_;
  std::string ticket_;
  unsigned int ticket_buf_size_;
  unsigned char ticket_buf_[4096];
  CCallResult< RequestEncryptedAppTicketWorker, EncryptedAppTicketResponse_t >
      call_result_;
};

}  // namespace greenworks

#endif  // SRC_GREENWORKS_ASYNC_WORKERS_H_
