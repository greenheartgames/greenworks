// Copyright (c) 2015 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_STEAM_CLIENT_H_
#define SRC_STEAM_CLIENT_H_

#include <vector>

#include "steam/steam_api.h"
#include "uv.h"

namespace greenworks {

class SteamClient {
 public:
  class Observer {
   public:
    virtual void OnGameOverlayActivated(bool is_active);
    virtual ~Observer() {}
  };

  void AddObserver(Observer* observer);
  void RemoveObserver(Observer* observer);

  static SteamClient* GetInstance();
  static void StartSteamLoop();
  static void RunSteamAPICallback(uv_timer_t* handle);

 private:
  SteamClient();
  ~SteamClient();

  // SteamClient owns observer object
  std::vector<Observer*> observer_list_;

  STEAM_CALLBACK(SteamClient, OnGameOverlayActivated, GameOverlayActivated_t, game_overlay_activated_);
};

}  // namespace greenworks

#endif  // SRC_STEAM_CLIENT_H_
