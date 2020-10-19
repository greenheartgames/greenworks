// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "nan.h"
#include "v8.h"

#include "greenworks_async_workers.h"
#include "steam/steam_api.h"
#include "steam_api_registry.h"

namespace greenworks {
namespace api {
namespace {

NAN_METHOD(ActivateAchievement) {
  Nan::HandleScope scope;

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string achievement = (*(Nan::Utf8String(info[0])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::ActivateAchievementWorker(
      success_callback, error_callback, achievement));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(IndicateAchievementProgress) {
  Nan::HandleScope scope;

  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsUint32() || !info[2]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string achievement = (*(Nan::Utf8String(info[0])));
  uint32 current = Nan::To<uint32>(info[1]).FromJust();
  uint32 max = Nan::To<uint32>(info[2]).FromJust();

  info.GetReturnValue().Set(
      Nan::New(SteamUserStats()->IndicateAchievementProgress(achievement.c_str(), current, max)));
}

NAN_METHOD(GetAchievement) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string achievement = (*(Nan::Utf8String(info[0])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::AsyncQueueWorker(new greenworks::GetAchievementWorker(
      success_callback, error_callback, achievement));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ClearAchievement) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string achievement = (*(Nan::Utf8String(info[0])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::ClearAchievementWorker(
      success_callback, error_callback, achievement));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetAchievementNames) {
  Nan::HandleScope scope;
  auto count = static_cast<int>(SteamUserStats()->GetNumAchievements());
  v8::Local<v8::Array> names = Nan::New<v8::Array>(count);
  for (int i = 0; i < count; ++i) {
    Nan::Set(
        names, i,
        Nan::New(SteamUserStats()->GetAchievementName(i)).ToLocalChecked());
  }
  info.GetReturnValue().Set(names);
}

NAN_METHOD(GetNumberOfAchievements) {
  Nan::HandleScope scope;
  ISteamUserStats* steam_user_stats = SteamUserStats();
  info.GetReturnValue().Set(steam_user_stats->GetNumAchievements());
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  SET_FUNCTION("activateAchievement", ActivateAchievement);
  SET_FUNCTION("indicateAchievementProgress", IndicateAchievementProgress);
  SET_FUNCTION("getAchievement", GetAchievement);
  SET_FUNCTION("clearAchievement", ClearAchievement);
  SET_FUNCTION("getAchievementNames", GetAchievementNames);
  SET_FUNCTION("getNumberOfAchievements", GetNumberOfAchievements);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
