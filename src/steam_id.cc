// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_utils.h"
#include "steam_id.h"
#include "v8.h"

namespace greenworks {

v8::Local<v8::Object> SteamID::Create(CSteamID steam_id) {
  Nan::EscapableHandleScope scope;
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "isAnonymous", IsAnonymous);
  SetPrototypeMethod(tpl, "isAnonymousGameServer", IsAnonymousGameServer);
  SetPrototypeMethod(tpl, "isAnonymousGameServerLogin",
                     IsAnonymousGameServerLogin);
  SetPrototypeMethod(tpl, "isAnonymousUser", IsAnonymousUser);
  SetPrototypeMethod(tpl, "isChatAccount", IsChatAccount);
  SetPrototypeMethod(tpl, "isClanAccount", IsClanAccount);
  SetPrototypeMethod(tpl, "isConsoleUserAccount", IsConsoleUserAccount);
  SetPrototypeMethod(tpl, "isContentServerAccount", IsContentServerAccount);
  SetPrototypeMethod(tpl, "isGameServerAccount", IsGameServerAccount);
  SetPrototypeMethod(tpl, "isIndividualAccount", IsIndividualAccount);
  SetPrototypeMethod(tpl, "isPersistentGameServerAccount",
                     IsPersistentGameServerAccount);
  SetPrototypeMethod(tpl, "isLobby", IsLobby);
  SetPrototypeMethod(tpl, "getAccountID", GetAccountID);
  SetPrototypeMethod(tpl, "getRawSteamID", GetRawSteamID);
  SetPrototypeMethod(tpl, "getAccountType", GetAccountType);
  SetPrototypeMethod(tpl, "isValid", IsValid);
  SetPrototypeMethod(tpl, "getStaticAccountKey", GetStaticAccountKey);
  SetPrototypeMethod(tpl, "getPersonaName", GetPersonaName);
  SetPrototypeMethod(tpl, "getNickname", GetNickname);
  SetPrototypeMethod(tpl, "getRelationship", GetRelationship);
  SetPrototypeMethod(tpl, "getSteamLevel", GetSteamLevel);

  auto* obj = new SteamID(steam_id);
  v8::Local<v8::Object> instance =
      Nan::NewInstance(Nan::GetFunction(tpl).ToLocalChecked()).ToLocalChecked();
  Nan::SetInternalFieldPointer(instance, 0, obj);
  return scope.Escape(instance);
}

NAN_METHOD(SteamID::IsAnonymous) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(obj->steam_id_.BAnonAccount());
}

NAN_METHOD(SteamID::IsAnonymousGameServer) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(obj->steam_id_.BAnonGameServerAccount());
}

NAN_METHOD(SteamID::IsAnonymousGameServerLogin) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BBlankAnonAccount()));
}

NAN_METHOD(SteamID::IsAnonymousUser) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BAnonUserAccount()));
}

NAN_METHOD(SteamID::IsChatAccount) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BChatAccount()));
}

NAN_METHOD(SteamID::IsClanAccount) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BClanAccount()));
}

NAN_METHOD(SteamID::IsConsoleUserAccount) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BConsoleUserAccount()));
}

NAN_METHOD(SteamID::IsContentServerAccount) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BContentServerAccount()));
}

NAN_METHOD(SteamID::IsGameServerAccount) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BGameServerAccount()));
}

NAN_METHOD(SteamID::IsIndividualAccount) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BIndividualAccount()));
}

NAN_METHOD(SteamID::IsPersistentGameServerAccount) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(obj->steam_id_.BPersistentGameServerAccount()));
}

NAN_METHOD(SteamID::IsLobby) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.IsLobby()));
}

NAN_METHOD(SteamID::GetAccountID) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New<v8::Integer>(obj->steam_id_.GetAccountID()));
}

NAN_METHOD(SteamID::GetRawSteamID) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(utils::uint64ToString(obj->steam_id_.ConvertToUint64()))
          .ToLocalChecked());
}

NAN_METHOD(SteamID::GetAccountType) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.GetEAccountType()));
}

NAN_METHOD(SteamID::IsValid) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.IsValid()));
}

NAN_METHOD(SteamID::GetStaticAccountKey) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(utils::uint64ToString(
          obj->steam_id_.GetStaticAccountKey())).ToLocalChecked());
}

NAN_METHOD(SteamID::GetPersonaName) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetFriendPersonaName(obj->steam_id_))
          .ToLocalChecked());
}

NAN_METHOD(SteamID::GetNickname) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  const char* nick_name = SteamFriends()->GetPlayerNickname(obj->steam_id_);
  if (nick_name) {
    info.GetReturnValue().Set(Nan::New(nick_name).ToLocalChecked());
    return;
  }
  info.GetReturnValue().Set(Nan::EmptyString());
}

NAN_METHOD(SteamID::GetRelationship) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetFriendRelationship(obj->steam_id_)));
}

NAN_METHOD(SteamID::GetSteamLevel) {
  auto* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetFriendSteamLevel(obj->steam_id_)));
}

}  // namespace greenworks
