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

void RegisterAPIs(v8::Handle<v8::Object> target) {
  Nan::Set(target, Nan::New("getDLCCount").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetDLCCount)->GetFunction());
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
