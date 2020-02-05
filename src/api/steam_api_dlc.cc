// Copyright (c) 2017 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "nan.h"
#include "steam/steam_api.h"
#include "steam/isteamapps.h"
#include "v8.h"

#include "steam_api_registry.h"

namespace greenworks {
namespace api {
namespace {

NAN_METHOD(GetDLCCount) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(SteamApps()->GetDLCCount());
}

NAN_METHOD(GetDLCDataByIndex) {
  Nan::HandleScope scope;
  
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments; expected: index [int32]");
  }
  
  int32 index = Nan::To<int32>(info[0]).FromJust();
  AppId_t app_id;
  bool available;
  char name[128];
  bool success = SteamApps()->BGetDLCDataByIndex(index, &app_id, &available, name, 128);

  if (success) {
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("appId").ToLocalChecked(), Nan::New(static_cast<uint32>(app_id)));
    Nan::Set(result, Nan::New("available").ToLocalChecked(), Nan::New(available));
    Nan::Set(result, Nan::New("name").ToLocalChecked(), Nan::New(name).ToLocalChecked());
    info.GetReturnValue().Set(result);
  }
  else {
    info.GetReturnValue().Set(Nan::Undefined());
  }
}

NAN_METHOD(IsDLCInstalled) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto dlc_app_id = static_cast<AppId_t>(Nan::To<uint32>(info[0]).FromJust());
  info.GetReturnValue().Set(SteamApps()->BIsDlcInstalled(dlc_app_id));
}

NAN_METHOD(InstallDLC) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto dlc_app_id = static_cast<AppId_t>(Nan::To<uint32>(info[0]).FromJust());
  SteamApps()->InstallDLC(dlc_app_id);
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UninstallDLC) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto dlc_app_id = static_cast<AppId_t>(Nan::To<uint32>(info[0]).FromJust());
  SteamApps()->UninstallDLC(dlc_app_id);
  info.GetReturnValue().Set(Nan::Undefined());
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  SET_FUNCTION("getDLCCount", GetDLCCount);
  SET_FUNCTION("getDLCDataByIndex", GetDLCDataByIndex);
  SET_FUNCTION("isDLCInstalled", IsDLCInstalled);
  SET_FUNCTION("installDLC", InstallDLC);
  SET_FUNCTION("uninstallDLC", UninstallDLC);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
