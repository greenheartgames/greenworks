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

}  // namespace greenworks

#endif  // SRC_STEAM_ASYNC_WORKER_H_
