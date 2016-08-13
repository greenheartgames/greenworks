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

  SteamID* obj = new SteamID(steam_id);
  v8::Local<v8::Object> instance =
      Nan::NewInstance(tpl->GetFunction()).ToLocalChecked();
  Nan::SetInternalFieldPointer(instance, 0, obj);
  return scope.Escape(instance);
}

NAN_METHOD(SteamID::IsAnonymous) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(obj->steam_id_.BAnonAccount());
}

NAN_METHOD(SteamID::IsAnonymousGameServer) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(obj->steam_id_.BAnonGameServerAccount());
}

NAN_METHOD(SteamID::IsAnonymousGameServerLogin) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BBlankAnonAccount()));
}

NAN_METHOD(SteamID::IsAnonymousUser) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BAnonUserAccount()));
}

NAN_METHOD(SteamID::IsChatAccount) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BChatAccount()));
}

NAN_METHOD(SteamID::IsClanAccount) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BClanAccount()));
}

NAN_METHOD(SteamID::IsConsoleUserAccount) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BConsoleUserAccount()));
}

NAN_METHOD(SteamID::IsContentServerAccount) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BContentServerAccount()));
}

NAN_METHOD(SteamID::IsGameServerAccount) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BGameServerAccount()));
}

NAN_METHOD(SteamID::IsIndividualAccount) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.BIndividualAccount()));
}

NAN_METHOD(SteamID::IsPersistentGameServerAccount) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(obj->steam_id_.BPersistentGameServerAccount()));
}

NAN_METHOD(SteamID::IsLobby) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.IsLobby()));
}

NAN_METHOD(SteamID::GetAccountID) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New<v8::Integer>(obj->steam_id_.GetAccountID()));
}

NAN_METHOD(SteamID::GetRawSteamID) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(utils::uint64ToString(obj->steam_id_.ConvertToUint64()))
          .ToLocalChecked());
}

NAN_METHOD(SteamID::GetAccountType) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.GetEAccountType()));
}

NAN_METHOD(SteamID::IsValid) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(Nan::New(obj->steam_id_.IsValid()));
}

NAN_METHOD(SteamID::GetStaticAccountKey) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(utils::uint64ToString(
          obj->steam_id_.GetStaticAccountKey())).ToLocalChecked());
}

NAN_METHOD(SteamID::GetPersonaName) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetFriendPersonaName(obj->steam_id_))
          .ToLocalChecked());
}

NAN_METHOD(SteamID::GetNickname) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetPlayerNickname(obj->steam_id_))
          .ToLocalChecked());
}

NAN_METHOD(SteamID::GetRelationship) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetFriendRelationship(obj->steam_id_)));
}

NAN_METHOD(SteamID::GetSteamLevel) {
  SteamID* obj = ObjectWrap::Unwrap<SteamID>(info.Holder());
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetFriendSteamLevel(obj->steam_id_)));
}

}  // namespace greenworks
