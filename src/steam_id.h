// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_STEAM_ID_H_
#define SRC_STEAM_ID_H_

#include "nan.h"
#include "steam/steam_api.h"

namespace greenworks {

class SteamID : public Nan::ObjectWrap {
 public:
  static v8::Local<v8::Object> Create(CSteamID steam_id);

  static NAN_METHOD(IsAnonymous);
  static NAN_METHOD(IsAnonymousGameServer);
  static NAN_METHOD(IsAnonymousGameServerLogin);
  static NAN_METHOD(IsAnonymousUser);
  static NAN_METHOD(IsChatAccount);
  static NAN_METHOD(IsClanAccount);
  static NAN_METHOD(IsConsoleUserAccount);
  static NAN_METHOD(IsContentServerAccount);
  static NAN_METHOD(IsGameServerAccount);
  static NAN_METHOD(IsIndividualAccount);
  static NAN_METHOD(IsPersistentGameServerAccount);
  static NAN_METHOD(IsLobby);
  static NAN_METHOD(GetAccountID);
  static NAN_METHOD(GetAccountType);
  static NAN_METHOD(GetRawSteamID);
  static NAN_METHOD(IsValid);
  static NAN_METHOD(GetStaticAccountKey);
  static NAN_METHOD(GetPersonaName);
  static NAN_METHOD(GetNickname);
  static NAN_METHOD(GetRelationship);
  static NAN_METHOD(GetSteamLevel);

 private:
  explicit SteamID(CSteamID steam_id) : steam_id_(steam_id) {}
  ~SteamID() override {}

  CSteamID steam_id_;
};

}  // namespace greenworks

#endif  // SRC_STEAM_ID_H_
