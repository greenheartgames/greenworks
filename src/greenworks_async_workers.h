// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_GREENWORK_ASYNC_WORKERS_H_
#define SRC_GREENWORK_ASYNC_WORKERS_H_

#include "steam_async_worker.h"

namespace greenworks {

class FileSaveWorker : public SteamAsyncWorker {
 public:
  FileSaveWorker(NanCallback* success_callback, NanCallback* error_callback,
      std::string file_name, std::string content);

  // Override NanAsyncWorker methods.
  virtual void Execute();

 private:
  std::string file_name_;
  std::string content_;
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

}  // namespace greenworks

#endif  // SRC_GREENWORK_ASYNC_WORKERS_H_
