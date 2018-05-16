// Copyright (c) 2015 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "steam_client.h"

#include <algorithm>

#include "nan.h"

namespace greenworks {

namespace {

SteamClient* g_steam_client = nullptr;
uv_timer_t* g_steam_timer = nullptr;

void on_timer_close_complete(uv_handle_t* handle) {
  delete reinterpret_cast<uv_timer_t*>(handle);
}

// uv v0.11.23 has changed uv_timer_cb interface by removing status_code.
#if NAUV_UVVERSION < 0x000b17
void RunSteamAPICallback(uv_timer_t* handle, int status_code) {
#else
void RunSteamAPICallback(uv_timer_t* handle) {
#endif
  SteamAPI_RunCallbacks();
}

}  // namespace

SteamClient::SteamClient()
    : game_overlay_activated_(this, &SteamClient::OnGameOverlayActivated),
      steam_servers_connected_(this, &SteamClient::OnSteamServersConnected),
      steam_servers_disconnected_(this,
                                  &SteamClient::OnSteamServersDisconnected),
      steam_server_connect_failure_(this,
                                    &SteamClient::OnSteamServerConnectFailure),
      steam_shutdown_(this, &SteamClient::OnSteamShutdown),
      steam_persona_state_change_(this, &SteamClient::OnPeronaStateChange),
      avatar_image_loaded_(this, &SteamClient::OnAvatarImageLoaded),
      game_connected_friend_chat_msg_(
          this,
          &SteamClient::OnGameConnectedFriendChatMessage),
      dlc_installed_(this, &SteamClient::OnDLCInstalled),
      MicroTxnAuthorizationResponse_(
          this,
          &SteamClient::OnMicroTxnAuthorizationResponse) {}

SteamClient::~SteamClient() {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    delete observer_list_[i];
  }
  if (g_steam_client) {
    uv_timer_stop(g_steam_timer);
    uv_close(reinterpret_cast<uv_handle_t*>(g_steam_timer),
             on_timer_close_complete);
  }
}

SteamClient* SteamClient::GetInstance() {
  if (!g_steam_client) {
    g_steam_client = new SteamClient();
  }
  return g_steam_client;
}

void SteamClient::OnGameOverlayActivated(GameOverlayActivated_t* callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnGameOverlayActivated(
        static_cast<bool>(callback->m_bActive));
  }
}

void SteamClient::OnSteamServersConnected(SteamServersConnected_t* callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnSteamServersConnected();
  }
}

void SteamClient::OnSteamServersDisconnected(
    SteamServersDisconnected_t* callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnSteamServersDisconnected();
  }
}

void SteamClient::OnSteamServerConnectFailure(
    SteamServerConnectFailure_t* callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnSteamServerConnectFailure(
        static_cast<int>(callback->m_eResult));
  }
}

void SteamClient::OnSteamShutdown(SteamShutdown_t* callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnSteamShutdown();
  }
}

void SteamClient::OnPeronaStateChange(PersonaStateChange_t* callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnPersonaStateChange(callback->m_ulSteamID,
                                            callback->m_nChangeFlags);
  }
}

void SteamClient::OnAvatarImageLoaded(AvatarImageLoaded_t* callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnAvatarImageLoaded(
        callback->m_steamID.ConvertToUint64(), callback->m_iImage,
        callback->m_iTall, callback->m_iWide);
  }
}

void SteamClient::OnGameConnectedFriendChatMessage(
    GameConnectedFriendChatMsg_t* callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnGameConnectedFriendChatMessage(
        callback->m_steamIDUser.ConvertToUint64(), callback->m_iMessageID);
  }
}

void SteamClient::OnDLCInstalled(DlcInstalled_t *callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnDLCInstalled(
        callback->m_nAppID);
  }
}

void SteamClient::OnMicroTxnAuthorizationResponse(
                          MicroTxnAuthorizationResponse_t *callback) {
  for (size_t i = 0; i < observer_list_.size(); ++i) {
    observer_list_[i]->OnMicroTxnAuthorizationResponse(
        callback->m_unAppID, callback->m_ulOrderID,
        callback->m_bAuthorized);
  }
}

void SteamClient::StartSteamLoop() {
  if (g_steam_timer)
    return;
  SteamClient::GetInstance();
  g_steam_timer = new uv_timer_t();
  uv_timer_init(uv_default_loop(), g_steam_timer);
  uv_timer_start(g_steam_timer, &RunSteamAPICallback, 0, 100);
}

void SteamClient::AddObserver(Observer* observer) {
  if (std::find(observer_list_.begin(), observer_list_.end(), observer) ==
      observer_list_.end()) {
    observer_list_.push_back(observer);
  }
}

}  // namespace greenworks
