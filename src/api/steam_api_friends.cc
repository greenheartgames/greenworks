// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <memory>

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "greenworks_utils.h"
#include "steam_api_registry.h"
#include "steam_id.h"

namespace greenworks {
namespace api {
namespace {

void InitFriendFlags(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> friend_flags = Nan::New<v8::Object>();
  SET_TYPE(friend_flags, "None", k_EFriendFlagNone);
  SET_TYPE(friend_flags, "Blocked", k_EFriendFlagBlocked);
  SET_TYPE(friend_flags, "FriendshipRequested",
           k_EFriendFlagFriendshipRequested);
  SET_TYPE(friend_flags, "Immediate", k_EFriendFlagImmediate);
  SET_TYPE(friend_flags, "ClanMember", k_EFriendFlagClanMember);
  SET_TYPE(friend_flags, "OnGameServer", k_EFriendFlagOnGameServer);
  SET_TYPE(friend_flags, "RequestingFriendship",
           k_EFriendFlagRequestingFriendship);
  SET_TYPE(friend_flags, "RequestingInfo", k_EFriendFlagRequestingInfo);
  SET_TYPE(friend_flags, "Ignored", k_EFriendFlagIgnored);
  SET_TYPE(friend_flags, "IgnoredFriend", k_EFriendFlagIgnoredFriend);
  // k_EFriendFlagSuggested has been removed since steamworks sdk 1.39.
  // SET_TYPE(friend_flags, "Suggested", k_EFriendFlagSuggested);
  SET_TYPE(friend_flags, "ChatMember", k_EFriendFlagChatMember);
  SET_TYPE(friend_flags, "All", k_EFriendFlagAll);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(friend_flags);
  Nan::Set(exports,
           Nan::New("FriendFlags").ToLocalChecked(),
           friend_flags);
}

void InitFriendRelationship(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> relationship = Nan::New<v8::Object>();
  SET_TYPE(relationship, "None", k_EFriendRelationshipNone);
  SET_TYPE(relationship, "Blocked", k_EFriendRelationshipBlocked);
  SET_TYPE(relationship, "RequestRecipient",
           k_EFriendRelationshipRequestRecipient);
  SET_TYPE(relationship, "Friend", k_EFriendRelationshipFriend);
  SET_TYPE(relationship, "RequestInitiator",
           k_EFriendRelationshipRequestInitiator);
  SET_TYPE(relationship, "Ignored", k_EFriendRelationshipIgnored);
  SET_TYPE(relationship, "IgnoredFriend", k_EFriendRelationshipIgnoredFriend);
  SET_TYPE(relationship, "Suggested",
           k_EFriendRelationshipSuggested_DEPRECATED);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(relationship);
  Nan::Set(exports,
           Nan::New("FriendRelationship").ToLocalChecked(),
           relationship);
}

void InitFriendPersonaChange(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> persona_change = Nan::New<v8::Object>();
  SET_TYPE(persona_change, "Name", k_EPersonaChangeName);
  SET_TYPE(persona_change, "Status", k_EPersonaChangeStatus);
  SET_TYPE(persona_change, "ComeOnline", k_EPersonaChangeComeOnline);
  SET_TYPE(persona_change, "GoneOffline", k_EPersonaChangeGoneOffline);
  SET_TYPE(persona_change, "GamePlayed", k_EPersonaChangeGamePlayed);
  SET_TYPE(persona_change, "GameServer", k_EPersonaChangeGameServer);
  SET_TYPE(persona_change, "Avatar", k_EPersonaChangeAvatar);
  SET_TYPE(persona_change, "JoinedSource", k_EPersonaChangeJoinedSource);
  SET_TYPE(persona_change, "LeftSource", k_EPersonaChangeLeftSource);
  SET_TYPE(persona_change, "RelationshipChanged",
           k_EPersonaChangeRelationshipChanged);
  SET_TYPE(persona_change, "NameFirstSet", k_EPersonaChangeNameFirstSet);
  SET_TYPE(persona_change, "NickName", k_EPersonaChangeNickname);
  SET_TYPE(persona_change, "SteamLevel", k_EPersonaChangeSteamLevel);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(persona_change);
  Nan::Set(exports,
           Nan::New("PersonaChange").ToLocalChecked(),
           persona_change);
}

void InitAccountType(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> account_type = Nan::New<v8::Object>();
  SET_TYPE(account_type, "Invalid", k_EAccountTypeInvalid);
  SET_TYPE(account_type, "Individual", k_EAccountTypeIndividual);
  SET_TYPE(account_type, "Multiseat", k_EAccountTypeMultiseat);
  SET_TYPE(account_type, "GameServer", k_EAccountTypeGameServer);
  SET_TYPE(account_type, "AnonymousGameServer", k_EAccountTypeAnonGameServer);
  SET_TYPE(account_type, "Pending", k_EAccountTypePending);
  SET_TYPE(account_type, "ContentServer", k_EAccountTypeContentServer);
  SET_TYPE(account_type, "Clan", k_EAccountTypeClan);
  SET_TYPE(account_type, "Chat", k_EAccountTypeChat);
  SET_TYPE(account_type, "ConsoleUser", k_EAccountTypeConsoleUser);
  SET_TYPE(account_type, "AnonymousUser", k_EAccountTypeAnonUser);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(account_type);
  Nan::Set(exports,
           Nan::New("AccountType").ToLocalChecked(),
           account_type);
}

void InitChatEntryType(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> chat_entry_type = Nan::New<v8::Object>();
  SET_TYPE(chat_entry_type, "Invalid", k_EChatEntryTypeInvalid);
  SET_TYPE(chat_entry_type, "ChatMsg", k_EChatEntryTypeChatMsg);
  SET_TYPE(chat_entry_type, "Typing", k_EChatEntryTypeTyping);
  SET_TYPE(chat_entry_type, "InviteGame", k_EChatEntryTypeInviteGame);
  SET_TYPE(chat_entry_type, "Emote", k_EChatEntryTypeEmote);
  SET_TYPE(chat_entry_type, "LeftConversation",
           k_EChatEntryTypeLeftConversation);
  SET_TYPE(chat_entry_type, "Entered", k_EChatEntryTypeEntered);
  SET_TYPE(chat_entry_type, "WasKicked", k_EChatEntryTypeWasKicked);
  SET_TYPE(chat_entry_type, "WasBanned", k_EChatEntryTypeWasBanned);
  SET_TYPE(chat_entry_type, "Disconnected", k_EChatEntryTypeDisconnected);
  SET_TYPE(chat_entry_type, "HistoricalChat", k_EChatEntryTypeHistoricalChat);
  SET_TYPE(chat_entry_type, "LinkBlocked", k_EChatEntryTypeLinkBlocked);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(chat_entry_type);
  Nan::Set(exports,
           Nan::New("ChatEntryType").ToLocalChecked(),
           chat_entry_type);
}

NAN_METHOD(GetFriendCount) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto friend_flag = static_cast<EFriendFlags>(Nan::To<int32>(info[0]).FromJust());

  info.GetReturnValue().Set(Nan::New<v8::Integer>(
    SteamFriends()->GetFriendCount(friend_flag)));
}

NAN_METHOD(GetFriends) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto friend_flag = static_cast<EFriendFlags>(Nan::To<int32>(info[0]).FromJust());
  int friends_count = SteamFriends()->GetFriendCount(friend_flag);
  v8::Local<v8::Array> friends = Nan::New<v8::Array>(friends_count);

  for (int i = 0; i < friends_count; ++i) {
    CSteamID steam_id = SteamFriends()->GetFriendByIndex(i, friend_flag);
    Nan::Set(friends, i, greenworks::SteamID::Create(steam_id));
  }
  info.GetReturnValue().Set(friends);
}

NAN_METHOD(GetSmallFriendAvatar) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
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
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
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
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
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
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  bool require_name_only = Nan::To<bool>(info[1]).FromJust();
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
      SteamFriends()->RequestUserInformation(steam_id, require_name_only));
}

NAN_METHOD(SetListenForFriendsMessages) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsBoolean()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  bool intercept_enabled = Nan::To<bool>(info[0]).FromJust();
  info.GetReturnValue().Set(
      SteamFriends()->SetListenForFriendsMessages(intercept_enabled));
}

NAN_METHOD(ReplyToFriendMessage) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  std::string message_being_sent(*(Nan::Utf8String(info[1])));
  info.GetReturnValue().Set(SteamFriends()->ReplyToFriendMessage(
      steam_id,
      message_being_sent.c_str()));
}

NAN_METHOD(GetFriendMessage) {
  Nan::HandleScope scope;
  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsInt32() ||
      !info[2]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  int message_id = Nan::To<int32>(info[1]).FromJust();
  int maximam_size = Nan::To<int32>(info[2]).FromJust();

  EChatEntryType chat_type;
  std::unique_ptr<char[]>message(new char[maximam_size]);

  int message_size = SteamFriends()->GetFriendMessage(
      steam_id, message_id, message.get(),
      maximam_size, &chat_type);

  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("message").ToLocalChecked(),
           Nan::New(message.get(), message_size).ToLocalChecked());
  Nan::Set(result, Nan::New("chatEntryType").ToLocalChecked(),
           Nan::New(chat_type));
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetFriendPersonaName) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetFriendPersonaName(steam_id))
          .ToLocalChecked());
}

NAN_METHOD(SetPlayedWith) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  SteamFriends()->SetPlayedWith(steam_id);
}

NAN_METHOD(SetRichPresence) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string pch_key_str(*(Nan::Utf8String(info[0])));
  std::string pch_value_str(*(Nan::Utf8String(info[1])));

  info.GetReturnValue().Set(
    SteamFriends()->SetRichPresence(pch_key_str.data(), pch_value_str.data())
  );
}

NAN_METHOD(GetFriendRichPresence) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  std::string pch_key_str(*(Nan::Utf8String(info[1])));
  info.GetReturnValue().Set(
      Nan::New(SteamFriends()->GetFriendRichPresence(steam_id, pch_key_str.data()))
          .ToLocalChecked());
}

NAN_METHOD(ClearRichPresence) {
  Nan::HandleScope scope;
  SteamFriends()->ClearRichPresence();
}

NAN_METHOD(GetFriendGamePlayed) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));

  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }

  FriendGameInfo_t friendInfo;
  if (!SteamFriends()->GetFriendGamePlayed(steam_id, &friendInfo)) {
    info.GetReturnValue().Set(Nan::Undefined());
    return;
  }

  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("m_gameID").ToLocalChecked(), Nan::New(
    utils::uint64ToString(friendInfo.m_gameID.ToUint64())
  ).ToLocalChecked());

  Nan::Set(result, Nan::New("m_unGameIP").ToLocalChecked(), Nan::New<v8::Integer>(friendInfo.m_unGameIP));
  Nan::Set(result, Nan::New("m_usGamePort").ToLocalChecked(), Nan::New<v8::Integer>(friendInfo.m_usGamePort));
  Nan::Set(result, Nan::New("m_usQueryPort").ToLocalChecked(), Nan::New<v8::Integer>(friendInfo.m_usQueryPort));

  Nan::Set(result, Nan::New("m_steamIDLobby").ToLocalChecked(), Nan::New(
    utils::uint64ToString(friendInfo.m_steamIDLobby.ConvertToUint64())
  ).ToLocalChecked());

  info.GetReturnValue().Set(result);
}

NAN_METHOD(ActivateGameOverlayInviteDialog) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  SteamFriends()->ActivateGameOverlayInviteDialog(steam_id);
}

NAN_METHOD(ActivateGameOverlayToUser) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string pch_dialog_str(*(Nan::Utf8String(info[0])));
  std::string steam_id_str(*(Nan::Utf8String(info[1])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  SteamFriends()->ActivateGameOverlayToUser(pch_dialog_str.data(), steam_id);
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  InitFriendFlags(target);
  InitFriendRelationship(target);
  InitFriendPersonaChange(target);
  InitAccountType(target);
  InitChatEntryType(target);

  SET_FUNCTION("getFriendCount", GetFriendCount);
  SET_FUNCTION("getFriends", GetFriends);
  SET_FUNCTION("getSmallFriendAvatar", GetSmallFriendAvatar);
  SET_FUNCTION("getMediumFriendAvatar", GetMediumFriendAvatar);
  SET_FUNCTION("getLargeFriendAvatar", GetLargeFriendAvatar);
  SET_FUNCTION("requestUserInformation", RequestUserInformation);
  SET_FUNCTION("setListenForFriendsMessage", SetListenForFriendsMessages);
  SET_FUNCTION("replyToFriendMessage", ReplyToFriendMessage);
  SET_FUNCTION("getFriendMessage", GetFriendMessage);
  SET_FUNCTION("getFriendPersonaName", GetFriendPersonaName);
  SET_FUNCTION("getFriendRichPresence", GetFriendRichPresence);
  SET_FUNCTION("setRichPresence", SetRichPresence);
  SET_FUNCTION("clearRichPresence", ClearRichPresence);
  SET_FUNCTION("getFriendGamePlayed", GetFriendGamePlayed);
  SET_FUNCTION("setPlayedWith", SetPlayedWith);
  SET_FUNCTION("activateGameOverlayInviteDialog", ActivateGameOverlayInviteDialog);
  SET_FUNCTION("activateGameOverlayToUser", ActivateGameOverlayToUser);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
