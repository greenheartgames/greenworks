// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <map>
#include <string>
#include <sstream>
#include <vector>

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "api/steam_api_registry.h"
#include "greenworks_async_workers.h"
#include "greenworks_utils.h"
#include "greenworks_version.h"
#include "greenworks_workshop_workers.h"
#include "steam_client.h"
#include "steam_event.h"
#include "steam_id.h"

namespace {

#define THROW_BAD_ARGS(msg)    \
    do {                       \
       Nan::ThrowTypeError(msg); \
       return;                   \
    } while (0);

#define MAKE_ENUM_PAIR(name) \
    { name, #name }

Nan::Persistent<v8::Object> g_persistent_steam_events;

NAN_METHOD(InitAPI) {
  Nan::HandleScope scope;

  bool success = SteamAPI_Init();

  if (success) {
    ISteamUserStats* stream_user_stats = SteamUserStats();
    stream_user_stats->RequestCurrentStats();
  }

  greenworks::SteamClient::GetInstance()->AddObserver(
      new greenworks::SteamEvent(g_persistent_steam_events));
  greenworks::SteamClient::StartSteamLoop();
  info.GetReturnValue().Set(Nan::New(success));
}

NAN_METHOD(CreateArchive) {
  Nan::HandleScope scope;
  if (info.Length() < 5 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsInt32() || !info[4]->IsFunction()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string zip_file_path = *(v8::String::Utf8Value(info[0]));
  std::string source_dir = *(v8::String::Utf8Value(info[1]));
  std::string password = *(v8::String::Utf8Value(info[2]));
  int compress_level = info[3]->Int32Value();

  Nan::Callback* success_callback =
      new Nan::Callback(info[4].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

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
  std::string zip_file_path = *(v8::String::Utf8Value(info[0]));
  std::string extract_dir = *(v8::String::Utf8Value(info[1]));
  std::string password = *(v8::String::Utf8Value(info[2]));

  Nan::Callback* success_callback =
      new Nan::Callback(info[3].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 4 && info[4]->IsFunction())
    error_callback = new Nan::Callback(info[4].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::ExtractArchiveWorker(
      success_callback, error_callback, zip_file_path, extract_dir, password));
  info.GetReturnValue().Set(Nan::Undefined());
}


void InitUtilsObject(v8::Handle<v8::Object> exports) {
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
  Nan::SetMethod(tpl, "createArchive", CreateArchive);
  Nan::SetMethod(tpl, "extractArchive", ExtractArchive);
  Nan::Persistent<v8::Function> constructor;
  constructor.Reset(tpl->GetFunction());
  Nan::Set(exports, Nan::New("Utils").ToLocalChecked(), tpl->GetFunction());
}

NAN_MODULE_INIT(init) {
  // Set internal steam event handler.
  v8::Local<v8::Object> steam_events = Nan::New<v8::Object>();
  g_persistent_steam_events.Reset(steam_events);
  Nan::Set(target, Nan::New("_steam_events").ToLocalChecked(), steam_events);

  greenworks::api::SteamAPIRegistry::GetInstance()->RegisterAllAPIs(target);

  // Set versions.
  Nan::Set(target,
           Nan::New("_version").ToLocalChecked(),
           Nan::New(GREENWORKS_VERSION).ToLocalChecked());
  // Common APIs.
  Nan::Set(target,
           Nan::New("initAPI").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(InitAPI)->GetFunction());

  utils::InitAccountType(target);

  // Utils related APIs.
  InitUtilsObject(target);
}

}  // namespace

#if defined(_WIN32)
  #if defined(_M_IX86)
    NODE_MODULE(greenworks_win32, init)
  #elif defined(_M_AMD64)
    NODE_MODULE(greenworks_win64, init)
  #endif
#elif defined(__APPLE__)
  #if defined(__x86_64__) || defined(__ppc64__)
    NODE_MODULE(greenworks_osx64, init)
  #else
    NODE_MODULE(greenworks_osx32, init)
  #endif
#elif defined(__linux__)
  #if defined(__x86_64__) || defined(__ppc64__)
    NODE_MODULE(greenworks_linux64, init)
  #else
    NODE_MODULE(greenworks_linux32, init)
  #endif
#endif
