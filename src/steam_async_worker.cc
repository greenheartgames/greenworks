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
