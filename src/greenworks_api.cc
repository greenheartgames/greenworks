// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <string>

#include "nan.h"
#include "v8.h"

#include "greenworks_async_workers.h"

namespace {

NAN_METHOD(InitAPI) {
  NanScope();

  bool success = SteamAPI_Init();

  if (success) {
    ISteamUserStats* stream_user_stats = SteamUserStats();
    stream_user_stats->RequestCurrentStats();
  }

  NanReturnValue(NanNew(success));
}

NAN_METHOD(SaveTextToFile) {
  NanScope();

  if (args.Length() < 4) {
    NanThrowTypeError("Wrong numer of arguments, should be 4.");
    NanReturnUndefined();
  }

  std::string file_name(*(v8::String::Utf8Value(args[0])));
  std::string content(*(v8::String::Utf8Value(args[1])));
  NanCallback* successCallback = new NanCallback(args[2].As<v8::Function>());
  NanCallback* errorCallback = new NanCallback(args[3].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::FileSaveWorker(successCallback,
                                                     errorCallback,
                                                     file_name,
                                                     content));
  NanReturnUndefined();
}

NAN_METHOD(ReadTextFromFile) {
  NanScope();

  if (args.Length() < 3) {
    NanThrowTypeError("Wrong numer of arguments, should be 3.");
    NanReturnUndefined();
  }

  std::string file_name(*(v8::String::Utf8Value(args[0])));
  NanCallback* successCallback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* errorCallback = new NanCallback(args[2].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::FileReadWorker(successCallback,
                                                     errorCallback,
                                                     file_name));
  NanReturnUndefined();
}

NAN_METHOD(IsCloudEnabled) {
  NanScope();
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();
  NanReturnValue(NanNew<v8::Boolean>(
      steam_remote_storage->IsCloudEnabledForApp()));
}

NAN_METHOD(EnableCloud) {
  NanScope();

  if (args.Length() < 1) {
    NanThrowTypeError("Wrong numer of arguments, should be 1.");
    NanReturnUndefined();
  }
  bool enable_flag = args[0]->BooleanValue();
  SteamRemoteStorage()->SetCloudEnabledForApp(enable_flag);
  NanReturnUndefined();
}

NAN_METHOD(GetCloudQuota) {
  NanScope();

  if (args.Length() < 2) {
    NanThrowTypeError("Wrong numer of arguments, should be 2.");
    NanReturnUndefined();
  }
  NanCallback* success_callback = new NanCallback(args[0].As<v8::Function>());
  NanCallback* error_callback = new NanCallback(args[1].As<v8::Function>());
  NanAsyncQueueWorker(new greenworks::CloudQuotaGetWorker(success_callback,
                                                          error_callback));
  NanReturnUndefined();
}

NAN_METHOD(ActivateAchievement) {
  NanScope();

  if (args.Length() < 3) {
    NanThrowTypeError("Wrong numer of arguments, should be 3.");
    NanReturnUndefined();
  }
  std::string achievement = (*(v8::String::Utf8Value(args[0])));
  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = new NanCallback(args[2].As<v8::Function>());
  NanAsyncQueueWorker(new greenworks::ActivateAchievementWorker(
      success_callback, error_callback, achievement));
  NanReturnUndefined();
}

void init(v8::Handle<v8::Object> exports) {
  exports->Set(NanNew("initAPI"),
               NanNew<v8::FunctionTemplate>(InitAPI)->GetFunction());
  exports->Set(NanNew("saveTextToFile"),
               NanNew<v8::FunctionTemplate>(SaveTextToFile)->GetFunction());
  exports->Set(NanNew("readTextFromFile"),
               NanNew<v8::FunctionTemplate>(ReadTextFromFile)->GetFunction());
  exports->Set(NanNew("isCloudEnabled"),
               NanNew<v8::FunctionTemplate>(IsCloudEnabled)->GetFunction());
  exports->Set(NanNew("enableCloud"),
               NanNew<v8::FunctionTemplate>(EnableCloud)->GetFunction());
  exports->Set(NanNew("getCloudQuota"),
               NanNew<v8::FunctionTemplate>(GetCloudQuota)->GetFunction());
  exports->Set(NanNew("activateAchievement"),
               NanNew<v8::FunctionTemplate>(
                   ActivateAchievement)->GetFunction());
}

}  // namespace

#ifdef _WIN32
	NODE_MODULE(greenworks_win, init)
#elif __APPLE__
	NODE_MODULE(greenworks_osx, init)
#elif __linux__
  #if __x86_64__ || __ppc64__
    NODE_MODULE(greenworks_linux64, init)
  #else
    NODE_MODULE(greenworks_linux32, init)
  #endif
#endif
