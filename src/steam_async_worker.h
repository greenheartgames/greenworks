// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_STEAM_ASYNC_WORKER_H_
#define SRC_STEAM_ASYNC_WORKER_H_

#include "nan.h"

namespace greenworks {

// Extend NanAsyncWorker with custom error callback supports.
class SteamAsyncWorker : public NanAsyncWorker {
 public:
  SteamAsyncWorker(NanCallback* success_callback, NanCallback* error_callback);

  ~SteamAsyncWorker();

  // Override NanAsyncWorker methods:
  virtual void HandleErrorCallback();

 protected:
  NanCallback* error_callback_;
};

// An abstract SteamAsyncWorker for Steam callback API.
class SteamCallbackAsyncWorker : public SteamAsyncWorker {
 public:
  SteamCallbackAsyncWorker(NanCallback* success_callback,
      NanCallback* error_callback);

  void WaitForCompleted();

 protected:
  bool is_completed_;
};

}  // namespace greenworks

#endif  // SRC_STEAM_ASYNC_WORKER_H_
