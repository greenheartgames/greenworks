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

  std::string file_name(*(v8::String::Utf8Value(info[0])));
  std::string content(*(v8::String::Utf8Value(info[1])));
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

  std::string file_name(*(v8::String::Utf8Value(info[0])));
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
    if (!files->Get(i)->IsString())
      THROW_BAD_ARGS("Bad arguments");
    v8::String::Utf8Value string_array(files->Get(i));
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

  std::string file_name(*(v8::String::Utf8Value(info[0])));
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
  bool enable_flag = info[0]->BooleanValue();
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
  int32 index = info[0].As<v8::Number>()->Int32Value();
  int32 file_size = 0;
  const char* file_name =
      SteamRemoteStorage()->GetFileNameAndSize(index, &file_size);
  result->Set(Nan::New("name").ToLocalChecked(),
              Nan::New(file_name).ToLocalChecked());
  result->Set(Nan::New("size").ToLocalChecked(),
              Nan::New(file_size));
  info.GetReturnValue().Set(result);
}

void RegisterAPIs(v8::Handle<v8::Object> target) {
  Nan::Set(target,
           Nan::New("saveTextToFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(SaveTextToFile)->GetFunction());
  Nan::Set(target,
           Nan::New("deleteFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(DeleteFile)->GetFunction());
  Nan::Set(target,
           Nan::New("readTextFromFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(ReadTextFromFile)->GetFunction());
  Nan::Set(target,
           Nan::New("saveFilesToCloud").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(SaveFilesToCloud)->GetFunction());
  Nan::Set(target,
           Nan::New("isCloudEnabled").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(IsCloudEnabled)->GetFunction());
  Nan::Set(target,
           Nan::New("isCloudEnabledForUser").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               IsCloudEnabledForUser)->GetFunction());
  Nan::Set(target,
           Nan::New("enableCloud").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(EnableCloud)->GetFunction());
  Nan::Set(target,
           Nan::New("getCloudQuota").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetCloudQuota)->GetFunction());
  Nan::Set(target,
           Nan::New("getFileCount").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetFileCount)->GetFunction());
  Nan::Set(target,
           Nan::New("getFileNameAndSize").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetFileNameAndSize)->GetFunction());
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
