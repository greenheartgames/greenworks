// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "steam_async_worker.h"

#include "v8.h"

namespace greenworks {

SteamAsyncWorker::SteamAsyncWorker(NanCallback* success_callback,
    NanCallback* error_callback): NanAsyncWorker(success_callback),
                                  error_callback_(error_callback) {
}

SteamAsyncWorker::~SteamAsyncWorker() {
  delete error_callback_;
}

void SteamAsyncWorker::HandleErrorCallback() {
  NanScope();
  v8::Local<v8::Value> argv[] = { NanNew(ErrorMessage()) };
  error_callback_->Call(1, argv);
}

}  // namespace greenworks
