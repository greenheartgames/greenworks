// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_API_STEAM_API_REGISTRY_H_
#define SRC_API_STEAM_API_REGISTRY_H_

#include <functional>
#include <vector>

#include "v8.h"

#define THROW_BAD_ARGS(msg)      \
    do {                         \
       Nan::ThrowTypeError(msg); \
       return;                   \
    } while (0);

#define SET_TYPE(obj, type_name, type)                                         \
  Nan::Set(obj, Nan::New(type_name).ToLocalChecked(), Nan::New(type))

#define SET_FUNCTION(function_name, function)                 \
  Nan::Set(target, Nan::New(function_name).ToLocalChecked(),          \
           Nan::GetFunction(Nan::New<v8::FunctionTemplate>(function)) \
               .ToLocalChecked());

namespace greenworks {
namespace api {

class SteamAPIRegistry {
 public:
  typedef std::function<void(v8::Local<v8::Object>)> RegistryFactory;

  static SteamAPIRegistry* GetInstance() {
    static SteamAPIRegistry steam_api_registry;
    return &steam_api_registry;
  }

  void RegisterAllAPIs(v8::Local<v8::Object> exports) {
    for (const auto& factory : registry_factories_) {
      factory(exports);
    }
  }

  class Add {
   public:
    explicit Add(const SteamAPIRegistry::RegistryFactory& registry_factory) {
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

#endif  // SRC_API_STEAM_API_REGISTRY_H_
