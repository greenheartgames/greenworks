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
  SteamEvent(const Nan::Persistent<v8::Object>& persistent_steam_events)
      : persistent_steam_events_(persistent_steam_events) {}

  // Override SteamClient::Observer methods.
  virtual void OnGameOverlayActivated(bool is_active);
  virtual void OnSteamServersConnected();
  virtual void OnSteamServersDisconnected();
  virtual void OnSteamServerConnectFailure(int status_code);
  virtual void OnSteamShutdown();
  virtual void OnPersonaStateChange(uint64 raw_steam_id,
                                    int persona_change_flag);
  virtual void OnAvatarImageLoaded(uint64 raw_steam_id,
                                   int image_handle,
                                   int height,
                                   int width);
 private:
  const Nan::Persistent<v8::Object>& persistent_steam_events_;
};

}  // namespace greenworks

#endif  // SRC_STEAM_EVENT_H_
