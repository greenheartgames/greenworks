// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <string>

#include "nan.h"
#include "v8.h"

#include "greenworks_async_workers.h"
#include "steam/steam_api.h"
#include "steam_api_registry.h"

namespace greenworks {
namespace api {
namespace {

NAN_METHOD(SaveTextToFile) {
  Nan::HandleScope scope;

  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string file_name(*(Nan::Utf8String(info[0])));
  std::string content(*(Nan::Utf8String(info[1])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[2].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 3 && info[3]->IsFunction())
    error_callback = new Nan::Callback(info[3].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::FileContentSaveWorker(success_callback,
                                                              error_callback,
                                                              file_name,
                                                              content));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(DeleteFile) {
  Nan::HandleScope scope;

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string file_name(*(Nan::Utf8String(info[0])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::FileDeleteWorker(success_callback,
                                                         error_callback,
                                                         file_name));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(SaveFilesToCloud) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsArray() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  v8::Local<v8::Array> files = info[0].As<v8::Array>();
  std::vector<std::string> files_path;
  for (uint32_t i = 0; i < files->Length(); ++i) {
    if (!Nan::Get(files, i).ToLocalChecked()->IsString())
      THROW_BAD_ARGS("Bad arguments");
    Nan::Utf8String string_array(Nan::Get(files, i).ToLocalChecked());
    // Ignore empty path.
    if (string_array.length() > 0)
      files_path.push_back(*string_array);
  }

  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::AsyncQueueWorker(new greenworks::FilesSaveWorker(success_callback,
                                                        error_callback,
                                                        files_path));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ReadTextFromFile) {
  Nan::HandleScope scope;

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string file_name(*(Nan::Utf8String(info[0])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::FileReadWorker(success_callback,
                                                       error_callback,
                                                       file_name));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(IsCloudEnabled) {
  Nan::HandleScope scope;
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(
      steam_remote_storage->IsCloudEnabledForApp()));
}

NAN_METHOD(IsCloudEnabledForUser) {
  Nan::HandleScope scope;
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(
      steam_remote_storage->IsCloudEnabledForAccount()));
}

NAN_METHOD(EnableCloud) {
  Nan::HandleScope scope;

  if (info.Length() < 1) {
    THROW_BAD_ARGS("Bad arguments");
  }
  bool enable_flag = Nan::To<bool>(info[0]).FromJust();
  SteamRemoteStorage()->SetCloudEnabledForApp(enable_flag);
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetCloudQuota) {
  Nan::HandleScope scope;

  if (info.Length() < 1 || !info[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::Callback* success_callback =
      new Nan::Callback(info[0].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[1]->IsFunction())
    error_callback = new Nan::Callback(info[1].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::CloudQuotaGetWorker(success_callback,
                                                            error_callback));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetFileCount) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(SteamRemoteStorage()->GetFileCount());
}

NAN_METHOD(GetFileNameAndSize) {
  Nan::HandleScope scope;
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  int32 index = Nan::To<int32>(info[0].As<v8::Number>()).FromJust();
  int32 file_size = 0;
  const char* file_name =
      SteamRemoteStorage()->GetFileNameAndSize(index, &file_size);
  Nan::Set(result, Nan::New("name").ToLocalChecked(),
           Nan::New(file_name).ToLocalChecked());
  Nan::Set(result, Nan::New("size").ToLocalChecked(), Nan::New(file_size));
  info.GetReturnValue().Set(result);
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  SET_FUNCTION("saveTextToFile", SaveTextToFile);
  SET_FUNCTION("deleteFile", DeleteFile);
  SET_FUNCTION("readTextFromFile", ReadTextFromFile);
  SET_FUNCTION("saveFilesToCloud", SaveFilesToCloud);
  SET_FUNCTION("isCloudEnabled", IsCloudEnabled);
  SET_FUNCTION("isCloudEnabledForUser", IsCloudEnabledForUser);
  SET_FUNCTION("enableCloud", EnableCloud);
  SET_FUNCTION("getCloudQuota", GetCloudQuota);
  SET_FUNCTION("getFileCount", GetFileCount);
  SET_FUNCTION("getFileNameAndSize", GetFileNameAndSize);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
