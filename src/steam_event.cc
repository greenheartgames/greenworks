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
  Nan::MakeCallback(
      Nan::New(persistent_steam_events_), "on", 2, argv);
}

void SteamEvent::OnSteamServersConnected() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-servers-connected").ToLocalChecked() };
  Nan::MakeCallback(
      Nan::New(persistent_steam_events_), "on", 1, argv);
}

void SteamEvent::OnSteamServersDisconnected() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-servers-disconnected").ToLocalChecked() };
  Nan::MakeCallback(
      Nan::New(persistent_steam_events_), "on", 1, argv);
}

void SteamEvent::OnSteamServerConnectFailure(int status_code) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-server-connect-failure").ToLocalChecked(),
      Nan::New(status_code) };
  Nan::MakeCallback(
      Nan::New(persistent_steam_events_), "on", 2, argv);
}

void SteamEvent::OnSteamShutdown() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = { Nan::New("steam-shutdown").ToLocalChecked() };
  Nan::MakeCallback(
      Nan::New(persistent_steam_events_), "on", 1, argv);
}

void SteamEvent::OnPersonaStateChange(uint64 raw_steam_id,
                                      int persona_change_flag) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("persona-state-change").ToLocalChecked(),
      greenworks::SteamID::Create(raw_steam_id), Nan::New(persona_change_flag),
  };
  Nan::MakeCallback(
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
  Nan::MakeCallback(
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
  Nan::MakeCallback(
      Nan::New(persistent_steam_events_), "on", 3, argv);
}

void SteamEvent::OnDLCInstalled(AppId_t dlc_app_id) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("dlc-installed").ToLocalChecked(),
      Nan::New(dlc_app_id),
  };
  Nan::MakeCallback(
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
  Nan::MakeCallback(
      Nan::New(persistent_steam_events_), "on", 4, argv);
}
}  // namespace greenworks
