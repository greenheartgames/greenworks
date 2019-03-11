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

void InitChatMemberStateChange(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> chat_member_state_change = Nan::New<v8::Object>();
  SET_TYPE(chat_member_state_change, "Entered", k_EChatMemberStateChangeEntered);
  SET_TYPE(chat_member_state_change, "Left", k_EChatMemberStateChangeLeft);
  SET_TYPE(chat_member_state_change, "Disconnected", k_EChatMemberStateChangeDisconnected);
  SET_TYPE(chat_member_state_change, "Kicked", k_EChatMemberStateChangeKicked);
  SET_TYPE(chat_member_state_change, "Banned", k_EChatMemberStateChangeBanned);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(chat_member_state_change);
  Nan::Set(exports, Nan::New("ChatMemberStateChange").ToLocalChecked(), chat_member_state_change);
}

void InitLobbyComparison(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> lobby_comparison = Nan::New<v8::Object>();
  SET_TYPE(lobby_comparison, "EqualToOrLessThan", k_ELobbyComparisonEqualToOrLessThan);
  SET_TYPE(lobby_comparison, "LessThan", k_ELobbyComparisonLessThan);
  SET_TYPE(lobby_comparison, "Equal", k_ELobbyComparisonEqual);
  SET_TYPE(lobby_comparison, "GreaterThan", k_ELobbyComparisonGreaterThan);
  SET_TYPE(lobby_comparison, "EqualToOrGreaterThan", k_ELobbyComparisonEqualToOrGreaterThan);
  SET_TYPE(lobby_comparison, "NotEqual", k_ELobbyComparisonNotEqual);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(lobby_comparison);
  Nan::Set(exports, Nan::New("LobbyComparison").ToLocalChecked(), lobby_comparison);
}

void InitLobbyDistanceFilter(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> lobby_distance_filter = Nan::New<v8::Object>();
  SET_TYPE(lobby_distance_filter, "Close", k_ELobbyDistanceFilterClose);
  SET_TYPE(lobby_distance_filter, "Default", k_ELobbyDistanceFilterDefault);
  SET_TYPE(lobby_distance_filter, "Far", k_ELobbyDistanceFilterFar);
  SET_TYPE(lobby_distance_filter, "Worldwide", k_ELobbyDistanceFilterWorldwide);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(lobby_distance_filter);
  Nan::Set(exports, Nan::New("LobbyDistanceFilter").ToLocalChecked(), lobby_distance_filter);
}

void InitLobbyType(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> lobby_type = Nan::New<v8::Object>();
  SET_TYPE(lobby_type, "Private", k_ELobbyTypePrivate);
  SET_TYPE(lobby_type, "FriendsOnly", k_ELobbyTypeFriendsOnly);
  SET_TYPE(lobby_type, "Public", k_ELobbyTypePublic);
  SET_TYPE(lobby_type, "Invisible", k_ELobbyTypeInvisible);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(lobby_type);
  Nan::Set(exports, Nan::New("LobbyType").ToLocalChecked(), lobby_type);
}

NAN_METHOD(CreateLobby) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsInt32() || !info[1]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto lobby_type = static_cast<ELobbyType>(info[0]->Int32Value());

  info.GetReturnValue().Set(
    Nan::New<v8::Integer>(
        SteamMatchmaking()->CreateLobby(lobby_type, info[1]->Int32Value())
    )
  );
}

NAN_METHOD(DeleteLobbyData) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  std::string pch_key_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->DeleteLobbyData(steam_id, pch_key_str.data())
  );
}

NAN_METHOD(GetLobbyByIndex) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  CSteamID lobby_id = SteamMatchmaking()->DeleteLobbyData(steam_id, pch_key_str);
  v8::Local<v8::Object> result = greenworks::SteamID::Create(lobby_id);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetLobbyData) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  std::string pch_key_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    Nan::New(
      SteamMatchmaking()->GetLobbyData(steam_id, pch_key_str.data())
    ).ToLocalChecked()
  );
}

NAN_METHOD(GetLobbyMemberByIndex) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  CSteamID user_id = SteamMatchmaking()->GetLobbyMemberByIndex(steam_id, info[1]->Int32Value());
  v8::Local<v8::Object> result = greenworks::SteamID::Create(user_id);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetNumLobbyMembers) {
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
    Nan::New<v8::Integer>(
        SteamMatchmaking()->GetNumLobbyMembers(steam_id)
    )
  );
}

NAN_METHOD(GetLobbyOwner) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  CSteamID user_id = SteamMatchmaking()->GetLobbyOwner(steam_id);
  v8::Local<v8::Object> result = greenworks::SteamID::Create(user_id);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(InviteUserToLobby) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_lobby_str(*(v8::String::Utf8Value(info[0])));
  std::string steam_id_user_str(*(v8::String::Utf8Value(info[1])));
  CSteamID steam_id_lobby(utils::strToUint64(steam_id_lobby_str));
  CSteamID steam_id_user(utils::strToUint64(steam_id_user_str));
  if (!steam_id_lobby.IsValid() || !steam_id_user.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->InviteUserToLobby(steam_id_lobby, steam_id_user)
  );
}

NAN_METHOD(JoinLobby) {
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
    Nan::New<v8::Integer>(
        SteamMatchmaking()->JoinLobby(steam_id)
    )
  );
}

NAN_METHOD(LeaveLobby) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  SteamMatchmaking()->LeaveLobby(steam_id);
}

NAN_METHOD(SetLobbyData) {
  Nan::HandleScope scope;
  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsString() || !info[2]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  std::string pch_key_str(*(v8::String::Utf8Value(info[1])));
  std::string pch_value_str(*(v8::String::Utf8Value(info[2])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->SetLobbyData(steam_id, pch_key_str.data(), pch_value_str.data())
  );
}

NAN_METHOD(SetLobbyJoinable) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsBoolean()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->SetLobbyJoinable(steam_id, info[1]->BooleanValue())
  );
}

NAN_METHOD(SetLobbyOwner) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_lobby_str(*(v8::String::Utf8Value(info[0])));
  std::string steam_id_user_str(*(v8::String::Utf8Value(info[1])));
  CSteamID steam_id_lobby(utils::strToUint64(steam_id_lobby_str));
  CSteamID steam_id_user(utils::strToUint64(steam_id_user_str));
  if (!steam_id_lobby.IsValid() || !steam_id_user.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->SetLobbyOwner(steam_id_lobby, steam_id_user)
  );
}

NAN_METHOD(SetLobbyType) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(v8::String::Utf8Value(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  auto lobby_type = static_cast<ELobbyType>(info[0]->Int32Value());
  info.GetReturnValue().Set(
    SteamMatchmaking()->SetLobbyType(steam_id, lobby_type)
  );
}

void RegisterAPIs(v8::Handle<v8::Object> exports) {
  InitChatMemberStateChange(exports);
  InitLobbyComparison(exports);
  InitLobbyDistanceFilter(exports);
  InitLobbyType(exports);

  Nan::Set(exports,
           Nan::New("createLobby").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(CreateLobby)->GetFunction());
  Nan::Set(exports,
           Nan::New("deleteLobbyData").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(DeleteLobbyData)->GetFunction());
  Nan::Set(exports,
           Nan::New("getLobbyByIndex").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetLobbyByIndex)->GetFunction());
  Nan::Set(exports,
           Nan::New("getLobbyData").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetLobbyData)->GetFunction());
  Nan::Set(exports,
           Nan::New("getLobbyMemberByIndex").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetLobbyMemberByIndex)->GetFunction());
  Nan::Set(exports,
           Nan::New("getNumLobbyMembers").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetNumLobbyMembers)->GetFunction());
  Nan::Set(exports,
           Nan::New("getLobbyOwner").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetLobbyOwner)->GetFunction());
  Nan::Set(exports,
           Nan::New("inviteUserToLobby").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(InviteUserToLobby)->GetFunction());
  Nan::Set(exports,
           Nan::New("joinLobby").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(JoinLobby)->GetFunction());
  Nan::Set(exports,
           Nan::New("LeaveLobby").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(LeaveLobby)->GetFunction());
  Nan::Set(exports,
           Nan::New("setLobbyData").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(SetLobbyData)->GetFunction());
  Nan::Set(exports,
           Nan::New("setLobbyJoinable").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(SetLobbyJoinable)->GetFunction());
  Nan::Set(exports,
           Nan::New("setLobbyOwner").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(SetLobbyOwner)->GetFunction());
  Nan::Set(exports,
           Nan::New("setLobbyType").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(SetLobbyType)->GetFunction());
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks

