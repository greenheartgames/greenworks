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

NAN_METHOD(IsDLCInstalled) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto dlc_app_id = static_cast<AppId_t>(info[0]->Uint32Value());
  info.GetReturnValue().Set(SteamApps()->BIsDlcInstalled(dlc_app_id));
}

NAN_METHOD(installDLC) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto dlc_app_id = static_cast<AppId_t>(info[0]->Uint32Value());
  SteamApps()->InstallDLC(dlc_app_id);
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(uninstallDLC) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto dlc_app_id = static_cast<AppId_t>(info[0]->Uint32Value());
  SteamApps()->UninstallDLC(dlc_app_id);
  info.GetReturnValue().Set(Nan::Undefined());
}

void RegisterAPIs(v8::Handle<v8::Object> target) {
  Nan::Set(target, Nan::New("getDLCCount").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetDLCCount)->GetFunction());
  Nan::Set(target, Nan::New("isDLCInstalled").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(IsDLCInstalled)->GetFunction());
  Nan::Set(target, Nan::New("installDLC").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(installDLC)->GetFunction());
  Nan::Set(target, Nan::New("uninstallDLC").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(uninstallDLC)->GetFunction());
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
