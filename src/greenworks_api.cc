// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "nan.h"
#include "steam/steam_api.h"
#include "steam/steam_gameserver.h"
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

  greenworks::SteamClient::GetInstance()->AddObserver(
      new greenworks::SteamEvent(g_persistent_steam_events));
  greenworks::SteamClient::StartSteamLoop();
  info.GetReturnValue().Set(Nan::New(success));
}

NAN_METHOD(InitGameServer) {
    if (info.Length() < 5 || 
        !info[0]->IsNumber() || 
        !info[1]->IsNumber() || 
        !info[2]->IsNumber() || 
        !info[3]->IsNumber() || 
        !info[4]->IsString()) {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    uint32 unIP = info[0]->Uint32Value(Nan::GetCurrentContext()).FromJust();
    uint16 usGamePort = info[1]->Uint32Value(Nan::GetCurrentContext()).FromJust();
    uint16 usQueryPort = info[2]->Uint32Value(Nan::GetCurrentContext()).FromJust();
    EServerMode eServerMode = static_cast<EServerMode>(info[3]->Uint32Value(Nan::GetCurrentContext()).FromJust());
    std::string pchVersionString = *(Nan::Utf8String(info[4]));

    // Initialize the game server
    bool success = SteamGameServer_Init(unIP, usGamePort, usQueryPort, eServerMode, pchVersionString.c_str());

    info.GetReturnValue().Set(Nan::New(success));
}

NAN_MODULE_INIT(init) {
  // Set internal steam event handler.
  v8::Local<v8::Object> steam_events = Nan::New<v8::Object>();
  g_persistent_steam_events.Reset(steam_events);
  Nan::Set(target, Nan::New("_steam_events").ToLocalChecked(), steam_events);

  greenworks::api::SteamAPIRegistry::GetInstance()->RegisterAllAPIs(target);

  SET_FUNCTION("initAPI", InitAPI);
  SET_FUNCTION("initGameServer", InitGameServer);
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
