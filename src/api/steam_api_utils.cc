// Copyright (c) 2024 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "nan.h"
#include "v8.h"

#include "steam/steam_api.h"

#include "steam_api_registry.h"

namespace greenworks {
namespace api {
namespace {

NAN_METHOD(ShowFloatingGamepadTextInput) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
   if (info.Length() < 5 || !info[0]->IsInt32() || !info[1]->IsInt32() ||
      !info[2]->IsInt32() || !info[3]->IsInt32() || !info[4]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  EFloatingGamepadTextInputMode input_mode =
      static_cast<EFloatingGamepadTextInputMode>(
          Nan::To<int32>(info[0]).FromJust());
  info.GetReturnValue().Set(SteamUtils()->ShowFloatingGamepadTextInput(
      input_mode, Nan::To<int32>(info[1]).FromJust(),
      Nan::To<int32>(info[2]).FromJust(), Nan::To<int32>(info[3]).FromJust(),
      Nan::To<int32>(info[4]).FromJust()));
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  SET_FUNCTION("showFloatingGamepadTextInput", ShowFloatingGamepadTextInput);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
