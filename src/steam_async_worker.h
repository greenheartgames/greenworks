// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_STEAM_ASYNC_WORKER_H_
#define SRC_STEAM_ASYNC_WORKER_H_

#include "nan.h"

namespace greenworks {

// Extend Nan::AsyncWorker with custom error callback supports.
class SteamAsyncWorker : public Nan::AsyncWorker {
 public:
  SteamAsyncWorker(Nan::Callback* success_callback,
                   Nan::Callback* error_callback);

  ~SteamAsyncWorker() override;

  void HandleErrorCallback() override;

 protected:
  Nan::Callback* error_callback_;
};

// An abstract SteamAsyncWorker for Steam callback API.
class SteamCallbackAsyncWorker : public SteamAsyncWorker {
 public:
  SteamCallbackAsyncWorker(Nan::Callback* success_callback,
      Nan::Callback* error_callback);

  void WaitForCompleted();

 protected:
  bool is_completed_;
};

}  // namespace greenworks

#endif  // SRC_STEAM_ASYNC_WORKER_H_
