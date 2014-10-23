// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "steam_async_worker.h"

#include "v8.h"

#include "steam/steam_api.h"
#include "greenworks_utils.h"

namespace greenworks {

SteamAsyncWorker::SteamAsyncWorker(NanCallback* success_callback,
    NanCallback* error_callback): NanAsyncWorker(success_callback),
                                  error_callback_(error_callback) {
}

SteamAsyncWorker::~SteamAsyncWorker() {
  delete error_callback_;
}

void SteamAsyncWorker::HandleErrorCallback() {
  if (!error_callback_) return;
  NanScope();
  v8::Local<v8::Value> argv[] = { NanNew(ErrorMessage()) };
  error_callback_->Call(1, argv);
}

SteamCallbackAsyncWorker::SteamCallbackAsyncWorker(
    NanCallback* success_callback, NanCallback* error_callback):
        SteamAsyncWorker(success_callback, error_callback),
        is_completed_(false) {
}

void SteamCallbackAsyncWorker::WaitForCompleted() {
  while (!is_completed_) {
    SteamAPI_RunCallbacks();
    // sleep 100ms.
    utils::sleep(100);
  }
}

}  // namespace greenworks
