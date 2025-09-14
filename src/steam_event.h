// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_STEAM_EVENT_H_
#define SRC_STEAM_EVENT_H_

#include "nan.h"
#include "steam_client.h"
#include "v8.h"

namespace greenworks {

class SteamEvent : public greenworks::SteamClient::Observer {
 public:
  explicit SteamEvent(
      const Nan::Persistent<v8::Object>& persistent_steam_events)
      : persistent_steam_events_(persistent_steam_events) {}

  void OnGameOverlayActivated(bool is_active) override;
  void OnSteamServersConnected() override;
  void OnSteamServersDisconnected() override;
  void OnSteamServerConnectFailure(int status_code) override;
  void OnSteamShutdown() override;
  void OnPersonaStateChange(uint64 raw_steam_id,
                                    int persona_change_flag) override;
  void OnAvatarImageLoaded(uint64 raw_steam_id,
                           int image_handle,
                           int height,
                           int width) override;
  void OnGameConnectedFriendChatMessage(uint64 raw_steam_id,
                                        int message_id) override;
  void OnDLCInstalled(AppId_t dlc_app_id) override;
  void OnMicroTxnAuthorizationResponse(uint32 AppID, uint64 OrderID,
                                       bool Autorized) override;
  void OnLobbyCreated(int status_code, uint64 SteamIdLobby) override;
  void OnLobbyDataUpdate(uint64 SteamIdLobby, uint64 SteamIdMember,
                         bool Success) override;
  void OnLobbyEnter(uint64 SteamIdLobby, int ChatPermissions, bool Locked,
                    int ChatRoomEnterResponse) override;
  void OnLobbyInvite(uint64 SteamIdUser, uint64 SteamIdLobby,
                     uint64 GameId) override;
  void OnGameLobbyJoinRequested(uint64 SteamIdLobby,
                                uint64 SteamIdUser) override;
  void OnGameRichPresenceJoinRequested(uint64 steamIDFriend,
                                       std::string rgchConnect) override;
  void OnNewUrlLaunchParameters() override;
  void OnFloatingGamepadTextInputDismissed() override;

  void OnLobbyMatchList(uint32 LobbiesMatching) override;
  void OnP2PSessionRequest(uint64 steamIDRemote) override;
  void OnP2PSessionConnectFail(uint64 steamIDRemote,
                               uint8 eP2PSessionError) override;
  void OnLobbyChatMsg(uint64 steamIDLobby, uint64 steamIDUser,
                      uint8 chatEntryType, uint32 chatID) override;
  void OnLobbyChatUpdate(uint64 SteamIDLobby, uint64 SteamIDUserChanged,
                         uint64 SteamIDMakingChange,
                         uint32 ChatMemberStateChange) override;

  void OnValidateAuthTicketResponse(CSteamID m_SteamID,
                         EAuthSessionResponse m_eAuthSessionResponse,
                         CSteamID m_OwnerSteamID) override;

private:
  const Nan::Persistent<v8::Object>& persistent_steam_events_;
};

}  // namespace greenworks

#endif  // SRC_STEAM_EVENT_H_
