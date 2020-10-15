// Copyright (c) 2015 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_STEAM_CLIENT_H_
#define SRC_STEAM_CLIENT_H_

#include <string>
#include <vector>

#include "steam/steam_api.h"
#include "steam/isteamapps.h"
#include "uv.h"

namespace greenworks {

class SteamClient {
 public:
  class Observer {
   public:
    virtual void OnGameOverlayActivated(bool is_active) = 0;
    virtual void OnSteamServersConnected() = 0;
    virtual void OnSteamServersDisconnected() = 0;
    virtual void OnSteamServerConnectFailure(int status_code) = 0;
    virtual void OnSteamShutdown() = 0;
    virtual void OnPersonaStateChange(uint64 raw_steam_id,
                                      int persona_change_flag) = 0;
    virtual void OnAvatarImageLoaded(uint64 raw_steam_id,
                                     int image_handle,
                                     int height,
                                     int width) = 0;
    virtual void OnGameConnectedFriendChatMessage(uint64 raw_steam_id,
                                                  int message_id) = 0;
    virtual void OnDLCInstalled(AppId_t dlc_app_id) = 0;
    virtual void OnMicroTxnAuthorizationResponse(uint32 AppID,
                                                 uint64 OrderID,
                                                 bool Autorized) = 0;
    virtual void OnLobbyCreated(int status_code, uint64 SteamIdLobby) = 0;
    virtual void OnLobbyDataUpdate(uint64 SteamIdLobby, uint64 SteamIdMember, bool Success) = 0;
    virtual void OnLobbyEnter(uint64 SteamIdLobby, int ChatPermissions, bool Locked, int ChatRoomEnterResponse) = 0;
    virtual void OnLobbyInvite(uint64 SteamIdUser, uint64 SteamIdLobby, uint64 GameId) = 0;
    virtual void OnGameLobbyJoinRequested(uint64 SteamIdLobby, uint64 SteamIdUser) = 0;
    virtual void OnGameRichPresenceJoinRequested(uint64 steamIDFriend, std::string rgchConnect) = 0;
    virtual void OnNewUrlLaunchParameters() = 0;
    virtual ~Observer() {}
  };

  void AddObserver(Observer* observer);

  static SteamClient* GetInstance();
  static void StartSteamLoop();

 private:
  SteamClient();
  ~SteamClient();

  // SteamClient owns observer object
  std::vector<Observer*> observer_list_;

  STEAM_CALLBACK(SteamClient, OnGameOverlayActivated,
      GameOverlayActivated_t, game_overlay_activated_);
  STEAM_CALLBACK(SteamClient, OnSteamServersConnected,
      SteamServersConnected_t, steam_servers_connected_);
  STEAM_CALLBACK(SteamClient, OnSteamServersDisconnected,
      SteamServersDisconnected_t, steam_servers_disconnected_);
  STEAM_CALLBACK(SteamClient, OnSteamServerConnectFailure,
      SteamServerConnectFailure_t, steam_server_connect_failure_);
  STEAM_CALLBACK(SteamClient,
                 OnSteamShutdown,
                 SteamShutdown_t,
                 steam_shutdown_);
  STEAM_CALLBACK(SteamClient,
                 OnPeronaStateChange,
                 PersonaStateChange_t,
                 steam_persona_state_change_);
  STEAM_CALLBACK(SteamClient,
                 OnAvatarImageLoaded,
                 AvatarImageLoaded_t,
                 avatar_image_loaded_);
  STEAM_CALLBACK(SteamClient,
                 OnGameConnectedFriendChatMessage,
                 GameConnectedFriendChatMsg_t,
                 game_connected_friend_chat_msg_);
  STEAM_CALLBACK(SteamClient, OnDLCInstalled, DlcInstalled_t, dlc_installed_);
  STEAM_CALLBACK(SteamClient,
                 OnMicroTxnAuthorizationResponse,
                 MicroTxnAuthorizationResponse_t,
                 MicroTxnAuthorizationResponse_);
  STEAM_CALLBACK(SteamClient, OnLobbyCreated, LobbyCreated_t, OnLobbyCreated_);
  STEAM_CALLBACK(SteamClient, OnLobbyDataUpdate, LobbyDataUpdate_t, OnLobbyDataUpdate_);
  STEAM_CALLBACK(SteamClient, OnLobbyEnter, LobbyEnter_t, OnLobbyEnter_);
  STEAM_CALLBACK(SteamClient, OnLobbyInvite, LobbyInvite_t, OnLobbyInvite_);
  STEAM_CALLBACK(SteamClient, OnGameLobbyJoinRequested, GameLobbyJoinRequested_t, OnGameLobbyJoinRequested_);
  STEAM_CALLBACK(SteamClient, OnGameRichPresenceJoinRequested, GameRichPresenceJoinRequested_t, OnGameRichPresenceJoinRequested_);
  STEAM_CALLBACK(SteamClient, OnNewUrlLaunchParameters, NewUrlLaunchParameters_t, OnNewUrlLaunchParameters_);
};

}  // namespace greenworks

#endif  // SRC_STEAM_CLIENT_H_
