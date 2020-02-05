// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <string>

#include "nan.h"
#include "v8.h"

#include "greenworks_async_workers.h"
#include "steam_api_registry.h"

namespace greenworks {
namespace api {
namespace {

NAN_METHOD(CreateArchive) {
  Nan::HandleScope scope;
  if (info.Length() < 5 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsInt32() || !info[4]->IsFunction()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string zip_file_path = *(Nan::Utf8String(info[0]));
  std::string source_dir = *(Nan::Utf8String(info[1]));
  std::string password = *(Nan::Utf8String(info[2]));
  int compress_level = Nan::To<int>(info[3]).FromJust();

  Nan::Callback* success_callback =
      new Nan::Callback(info[4].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 5 && info[5]->IsFunction())
    error_callback = new Nan::Callback(info[5].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::CreateArchiveWorker(
      success_callback, error_callback, zip_file_path, source_dir, password,
      compress_level));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ExtractArchive) {
  Nan::HandleScope scope;
  if (info.Length() < 4 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsFunction()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string zip_file_path = *(Nan::Utf8String(info[0]));
  std::string extract_dir = *(Nan::Utf8String(info[1]));
  std::string password = *(Nan::Utf8String(info[2]));

  Nan::Callback* success_callback =
      new Nan::Callback(info[3].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 4 && info[4]->IsFunction())
    error_callback = new Nan::Callback(info[4].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::ExtractArchiveWorker(
      success_callback, error_callback, zip_file_path, extract_dir, password));
  info.GetReturnValue().Set(Nan::Undefined());
}

void RegisterAPIs(v8::Local<v8::Object> exports) {
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
  Nan::SetMethod(tpl, "createArchive", CreateArchive);
  Nan::SetMethod(tpl, "extractArchive", ExtractArchive);
  Nan::Persistent<v8::Function> constructor;
  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(exports, Nan::New("Utils").ToLocalChecked(),
           Nan::GetFunction(tpl).ToLocalChecked());
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
