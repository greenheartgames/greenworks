// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <functional>
#include <vector>

#include "v8.h"

#define THROW_BAD_ARGS(msg)      \
    do {                         \
       Nan::ThrowTypeError(msg); \
       return;                   \
    } while (0);

namespace greenworks {
namespace api {

class SteamAPIRegistry {
 public:
  typedef std::function<void(v8::Handle<v8::Object>)> RegistryFactory;

  static SteamAPIRegistry* GetInstance() {
    static SteamAPIRegistry steam_api_registry;
    return &steam_api_registry;
  }

  void RegisterAllAPIs(v8::Handle<v8::Object> exports) {
    for (const auto& factory : registry_factories_) {
      factory(exports);
    }
  }

  class Add {
   public:
    Add(const SteamAPIRegistry::RegistryFactory& registry_factory) {
      SteamAPIRegistry::GetInstance()->AddRegistryFactory(registry_factory);
    }
  };

 private:
  void AddRegistryFactory(const RegistryFactory& register_api) {
    registry_factories_.push_back(register_api);
  }
  std::vector<RegistryFactory> registry_factories_;
};

}  // namespace api
}  // namespace greenworks
