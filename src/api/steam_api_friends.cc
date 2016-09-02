// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "greenworks_utils.h"
#include "steam_api_registry.h"
#include "steam_id.h"

namespace greenworks {
namespace api {
namespace {

NAN_METHOD(GetFriendCount) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  EFriendFlags friend_flag = static_cast<EFriendFlags>(info[0]->Int32Value());

  info.GetReturnValue().Set(Nan::New<v8::Integer>(
    SteamFriends()->GetFriendCount(friend_flag)));
}

NAN_METHOD(GetFriends) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  EFriendFlags friend_flag = static_cast<EFriendFlags>(info[0]->Int32Value());
  int friends_count = SteamFriends()->GetFriendCount(friend_flag);
  v8::Local<v8::Array> friends = Nan::New<v8::Array>(friends_count);

  for (int i = 0; i < friends_count; ++i) {
    CSteamID steam_id = SteamFriends()->GetFriendByIndex(i, friend_flag);
    friends->Set(i, greenworks::SteamID::Create(steam_id));
  }
  info.GetReturnValue().Set(friends);
}

NAN_METHOD(GetSmallFriendAvatar) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
      SteamFriends()->GetSmallFriendAvatar(steam_id));
}

NAN_METHOD(GetMediumFriendAvatar) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
      SteamFriends()->GetMediumFriendAvatar(steam_id));
}

NAN_METHOD(GetLargeFriendAvatar) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
      SteamFriends()->GetLargeFriendAvatar(steam_id));
}

NAN_METHOD(RequestUserInformation) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsBoolean()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  bool require_name_only = info[1]->BooleanValue();
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  SteamFriends()->RequestUserInformation(steam_id, require_name_only);
}

void RegisterAPIs(v8::Handle<v8::Object> exports) {
  utils::InitFriendFlags(exports);
  utils::InitFriendRelationship(exports);

  Nan::Set(exports,
           Nan::New("getFriendCount").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetFriendCount)->GetFunction());
  Nan::Set(exports,
           Nan::New("getFriends").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetFriends)->GetFunction());
  Nan::Set(exports, Nan::New("getSmallFriendAvatar").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetSmallFriendAvatar)->GetFunction());
  Nan::Set(
      exports, Nan::New("getMediumFriendAvatar").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(GetMediumFriendAvatar)->GetFunction());
  Nan::Set(
      exports, Nan::New("getLargeFriendAvatar").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(GetLargeFriendAvatar)->GetFunction());
  Nan::Set(
      exports, Nan::New("requestUserInformation").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(RequestUserInformation)->GetFunction());
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
