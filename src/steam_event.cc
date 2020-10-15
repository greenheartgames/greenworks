// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "steam_event.h"

#include "nan.h"
#include "steam_id.h"
#include "v8.h"

#include "greenworks_utils.h"

namespace greenworks {

void SteamEvent::OnGameOverlayActivated(bool is_active) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("game-overlay-activated").ToLocalChecked(),
      Nan::New(is_active) }; 
  Nan::AsyncResource ar("greenworks:SteamEvent.OnGameOverlayActivated");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 2, argv);
}

void SteamEvent::OnSteamServersConnected() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-servers-connected").ToLocalChecked() };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnSteamServersConnected");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 1, argv);
}

void SteamEvent::OnSteamServersDisconnected() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-servers-disconnected").ToLocalChecked() };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnSteamServersDisconnected");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 1, argv);
}

void SteamEvent::OnSteamServerConnectFailure(int status_code) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-server-connect-failure").ToLocalChecked(),
      Nan::New(status_code) };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnSteamServerConnectFailure");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 2, argv);
}

void SteamEvent::OnSteamShutdown() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = { Nan::New("steam-shutdown").ToLocalChecked() };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnSteamShutdown");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 1, argv);
}

void SteamEvent::OnPersonaStateChange(uint64 raw_steam_id,
                                      int persona_change_flag) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("persona-state-change").ToLocalChecked(),
      greenworks::SteamID::Create(raw_steam_id), Nan::New(persona_change_flag),
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnPersonaStateChange");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 3, argv);
}

void SteamEvent::OnAvatarImageLoaded(uint64 raw_steam_id,
                                     int image_handle,
                                     int height,
                                     int width) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("avatar-image-loaded").ToLocalChecked(),
      greenworks::SteamID::Create(raw_steam_id),
      Nan::New(image_handle),
      Nan::New(height),
      Nan::New(width),
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnAvatarImageLoaded");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 5, argv);
}

void SteamEvent::OnGameConnectedFriendChatMessage(uint64 raw_steam_id,
                                                  int message_id) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("game-connected-friend-chat-message").ToLocalChecked(),
      greenworks::SteamID::Create(raw_steam_id),
      Nan::New(message_id),
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnGameConnectedFriendChatMessage");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 3, argv);
}

void SteamEvent::OnDLCInstalled(AppId_t dlc_app_id) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("dlc-installed").ToLocalChecked(),
      Nan::New(dlc_app_id),
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnDLCInstalled");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 2, argv);
}

void SteamEvent::OnMicroTxnAuthorizationResponse(uint32 AppID,
                                                 uint64 OrderID,
                                                 bool Autorized) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("micro-txn-authorization-response").ToLocalChecked(),
      Nan::New(AppID),
      Nan::New(utils::uint64ToString(OrderID)).ToLocalChecked(),
      Nan::New(Autorized),
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnMicroTxnAuthorizationResponse");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 4, argv);
}

void SteamEvent::OnLobbyCreated(int status_code, uint64 SteamIdLobby) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("lobby-created").ToLocalChecked(),
      Nan::New(status_code),
      Nan::New(utils::uint64ToString(SteamIdLobby)).ToLocalChecked()
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnLobbyCreated");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 3, argv);
}

void SteamEvent::OnLobbyDataUpdate(uint64 SteamIdLobby, uint64 SteamIdMember, bool Success) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("lobby-data-update").ToLocalChecked(),
      Nan::New(utils::uint64ToString(SteamIdLobby)).ToLocalChecked(),
      Nan::New(utils::uint64ToString(SteamIdMember)).ToLocalChecked(),
      Nan::New(Success),
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnLobbyDataUpdate");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 4, argv);
}

void SteamEvent::OnLobbyEnter(uint64 SteamIdLobby, int ChatPermissions, bool Locked, int ChatRoomEnterResponse) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("lobby-enter").ToLocalChecked(),
      Nan::New(utils::uint64ToString(SteamIdLobby)).ToLocalChecked(),
      Nan::New(ChatPermissions),
      Nan::New(Locked),
      Nan::New(ChatRoomEnterResponse),
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnLobbyEnter");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 5, argv);
}

void SteamEvent::OnLobbyInvite(uint64 SteamIdUser, uint64 SteamIdLobby, uint64 GameId) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("lobby-invite").ToLocalChecked(),
      Nan::New(utils::uint64ToString(SteamIdUser)).ToLocalChecked(),
      Nan::New(utils::uint64ToString(SteamIdLobby)).ToLocalChecked(),
      Nan::New(utils::uint64ToString(GameId)).ToLocalChecked()
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnLobbyInvite");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 4, argv);
}

void SteamEvent::OnGameLobbyJoinRequested(uint64 SteamIdLobby, uint64 SteamIdUser) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("lobby-join-requested").ToLocalChecked(),
      Nan::New(utils::uint64ToString(SteamIdLobby)).ToLocalChecked(),
      Nan::New(utils::uint64ToString(SteamIdUser)).ToLocalChecked()
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnGameLobbyJoinRequested");
  ar.runInAsyncScope(
      Nan::New(persistent_steam_events_), "on", 3, argv);
}

void SteamEvent::OnGameRichPresenceJoinRequested(uint64 steamIDFriend, std::string rgchConnect) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
    Nan::New("rich-presence-join-requested").ToLocalChecked(),
    Nan::New(utils::uint64ToString(steamIDFriend)).ToLocalChecked(),
    Nan::New(rgchConnect).ToLocalChecked()
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnGameRichPresenceJoinRequested");
  ar.runInAsyncScope(
    Nan::New(persistent_steam_events_), "on", 3, argv);
}

void SteamEvent::OnNewUrlLaunchParameters() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
    Nan::New("new-url-launch-parameters").ToLocalChecked()
  };
  Nan::AsyncResource ar("greenworks:SteamEvent.OnNewUrlLaunchParameters");
  ar.runInAsyncScope(
    Nan::New(persistent_steam_events_), "on", 1, argv);
}

}  // namespace greenworks
