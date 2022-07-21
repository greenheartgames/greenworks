// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "api/steam_api_registry.h"
#include "steam_client.h"
#include "steam_event.h"

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
    ISteamUserStats* steam_user_stats = SteamUserStats();
    steam_user_stats->RequestCurrentStats();
  }

  greenworks::SteamClient::GetInstance()->AddObserver(
      new greenworks::SteamEvent(g_persistent_steam_events));
  greenworks::SteamClient::StartSteamLoop();
  info.GetReturnValue().Set(Nan::New(success));
}

NAN_MODULE_INIT(init) {
  // Set internal steam event handler.
  v8::Local<v8::Object> steam_events = Nan::New<v8::Object>();
  g_persistent_steam_events.Reset(steam_events);
  Nan::Set(target, Nan::New("_steam_events").ToLocalChecked(), steam_events);

  greenworks::api::SteamAPIRegistry::GetInstance()->RegisterAllAPIs(target);

  SET_FUNCTION("initAPI", InitAPI);
}

}  // namespace

#if defined(_WIN32)
  #if defined(_M_IX86)
    NAN_MODULE_WORKER_ENABLED(greenworks_win32, init)
  #elif defined(_M_AMD64)
    NAN_MODULE_WORKER_ENABLED(greenworks_win64, init)
  #endif
#elif defined(__APPLE__)
  #if defined(__x86_64__) || defined(__ppc64__)
    NAN_MODULE_WORKER_ENABLED(greenworks_osx64, init)
  #else
    NAN_MODULE_WORKER_ENABLED(greenworks_osx32, init)
  #endif
#elif defined(__linux__)
  #if defined(__x86_64__) || defined(__ppc64__)
    NAN_MODULE_WORKER_ENABLED(greenworks_linux64, init)
  #else
    NAN_MODULE_WORKER_ENABLED(greenworks_linux32, init)
  #endif
#endif
