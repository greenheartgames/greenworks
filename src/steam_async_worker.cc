// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "steam_async_worker.h"

#include "v8.h"

#include "steam/steam_api.h"
#include "greenworks_utils.h"

namespace greenworks {

SteamAsyncWorker::SteamAsyncWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback): Nan::AsyncWorker(success_callback),
                                    error_callback_(error_callback) {
}

SteamAsyncWorker::~SteamAsyncWorker() {
  delete error_callback_;
}

void SteamAsyncWorker::HandleErrorCallback() {
  if (!error_callback_) return;
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = { Nan::New(ErrorMessage()).ToLocalChecked() };
  error_callback_->Call(1, argv);
}

SteamCallbackAsyncWorker::SteamCallbackAsyncWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback):
        SteamAsyncWorker(success_callback, error_callback),
        is_completed_(false) {
}

void SteamCallbackAsyncWorker::WaitForCompleted() {
  while (!is_completed_) {
    // sleep 100ms.
    utils::sleep(100);
  }
}

}  // namespace greenworks
