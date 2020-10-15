// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <map>
#include <sstream>

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "greenworks_async_workers.h"
#include "greenworks_utils.h"
#include "greenworks_version.h"
#include "steam_api_registry.h"
#include "steam_client.h"
#include "steam_event.h"
#include "steam_id.h"

namespace greenworks {
namespace api {
namespace {

#define MAKE_ENUM_PAIR(name) \
    { name, #name }

void FreeCallback(char* data, void* hint) {
  delete data;
}

v8::Local<v8::Object> GetSteamUserCountType(int type_id) {
  if (type_id > k_EAccountTypeMax) {
    Nan::ThrowTypeError("Bad argument");
    return Nan::New<v8::Object>();
  }
  auto type = static_cast<EAccountType>(type_id);
  std::map<EAccountType, std::string> account_types = {
    MAKE_ENUM_PAIR(k_EAccountTypeInvalid),
    MAKE_ENUM_PAIR(k_EAccountTypeIndividual),
    MAKE_ENUM_PAIR(k_EAccountTypeMultiseat),
    MAKE_ENUM_PAIR(k_EAccountTypeGameServer),
    MAKE_ENUM_PAIR(k_EAccountTypeAnonGameServer),
    MAKE_ENUM_PAIR(k_EAccountTypePending),
    MAKE_ENUM_PAIR(k_EAccountTypeContentServer),
    MAKE_ENUM_PAIR(k_EAccountTypeClan),
    MAKE_ENUM_PAIR(k_EAccountTypeChat),
    MAKE_ENUM_PAIR(k_EAccountTypeConsoleUser),
    MAKE_ENUM_PAIR(k_EAccountTypeAnonUser),
    MAKE_ENUM_PAIR(k_EAccountTypeMax)
  };
  std::string name = account_types[type];
  v8::Local<v8::Object> account_type = Nan::New<v8::Object>();
  Nan::Set(account_type, Nan::New("name").ToLocalChecked(),
           Nan::New(name).ToLocalChecked());
  Nan::Set(account_type, Nan::New("value").ToLocalChecked(), Nan::New(type_id));
  return account_type;
}

NAN_METHOD(RestartAppIfNecessary) {
  Nan::HandleScope scope;

  if (info.Length() < 1) {
    Nan::ThrowTypeError("You must pass your app ID to RestartAppIfNecessary");
    return;
  }

  if (!info[0]->IsUint32()) {
    Nan::ThrowTypeError("Your app ID argument should be an integer");
    return;
  }

  uint32 arg0 = Nan::To<uint32>(info[0]).FromJust();

  bool restarting = SteamAPI_RestartAppIfNecessary(arg0);
  info.GetReturnValue().Set(Nan::New(restarting));
}

NAN_METHOD(IsSteamRunning) {
  Nan::HandleScope scope;

  bool running = SteamAPI_IsSteamRunning();
  info.GetReturnValue().Set(Nan::New(running));
}

NAN_METHOD(GetSteamId) {
  Nan::HandleScope scope;
  CSteamID user_id = SteamUser()->GetSteamID();
  v8::Local<v8::Object> flags = Nan::New<v8::Object>();
  Nan::Set(flags, Nan::New("anonymous").ToLocalChecked(),
           Nan::New(user_id.BAnonAccount()));
  Nan::Set(flags, Nan::New("anonymousGameServer").ToLocalChecked(),
           Nan::New(user_id.BAnonGameServerAccount()));
  Nan::Set(flags, Nan::New("anonymousGameServerLogin").ToLocalChecked(),
           Nan::New(user_id.BBlankAnonAccount()));
  Nan::Set(flags, Nan::New("anonymousUser").ToLocalChecked(),
           Nan::New(user_id.BAnonUserAccount()));
  Nan::Set(flags, Nan::New("chat").ToLocalChecked(),
           Nan::New(user_id.BChatAccount()));
  Nan::Set(flags, Nan::New("clan").ToLocalChecked(),
           Nan::New(user_id.BClanAccount()));
  Nan::Set(flags, Nan::New("consoleUser").ToLocalChecked(),
           Nan::New(user_id.BConsoleUserAccount()));
  Nan::Set(flags, Nan::New("contentServer").ToLocalChecked(),
           Nan::New(user_id.BContentServerAccount()));
  Nan::Set(flags, Nan::New("gameServer").ToLocalChecked(),
           Nan::New(user_id.BGameServerAccount()));
  Nan::Set(flags, Nan::New("individual").ToLocalChecked(),
           Nan::New(user_id.BIndividualAccount()));
  Nan::Set(flags, Nan::New("gameServerPersistent").ToLocalChecked(),
           Nan::New(user_id.BPersistentGameServerAccount()));
  Nan::Set(flags, Nan::New("lobby").ToLocalChecked(),
           Nan::New(user_id.IsLobby()));

  v8::Local<v8::Object> result = greenworks::SteamID::Create(user_id);
  // For backwards compatiblilty.
  Nan::Set(result, Nan::New("flags").ToLocalChecked(), flags);
  Nan::Set(result, Nan::New("type").ToLocalChecked(),
           GetSteamUserCountType(user_id.GetEAccountType()));
  Nan::Set(result, Nan::New("accountId").ToLocalChecked(),
           Nan::New<v8::Integer>(user_id.GetAccountID()));
  Nan::Set(result, Nan::New("steamId").ToLocalChecked(),
           Nan::New(utils::uint64ToString(user_id.ConvertToUint64()))
               .ToLocalChecked());
  Nan::Set(result, Nan::New("staticAccountId").ToLocalChecked(),
           Nan::New(utils::uint64ToString(user_id.GetStaticAccountKey()))
               .ToLocalChecked());
  Nan::Set(result, Nan::New("isValid").ToLocalChecked(),
           Nan::New<v8::Integer>(user_id.IsValid()));
  Nan::Set(result, Nan::New("level").ToLocalChecked(),
           Nan::New<v8::Integer>(SteamUser()->GetPlayerSteamLevel()));

  if (!SteamFriends()->RequestUserInformation(user_id, true)) {
    Nan::Set(result, Nan::New("screenName").ToLocalChecked(),
             Nan::New(SteamFriends()->GetFriendPersonaName(user_id))
                 .ToLocalChecked());
  } else {
    std::ostringstream sout;
    sout << user_id.GetAccountID();
    Nan::Set(result, Nan::New("screenName").ToLocalChecked(),
             Nan::New(sout.str()).ToLocalChecked());
  }
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetAppId) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(
      Nan::New(SteamUtils()->GetAppID()));
}

NAN_METHOD(GetAppBuildId) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(
      Nan::New(SteamApps()->GetAppBuildId()));
}

NAN_METHOD(GetCurrentGameLanguage) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(
      Nan::New(SteamApps()->GetCurrentGameLanguage()).ToLocalChecked());
}

NAN_METHOD(GetCurrentUILanguage) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(
      Nan::New(SteamUtils()->GetSteamUILanguage()).ToLocalChecked());
}

// TODO(hokein): Implement get game install directory.
NAN_METHOD(GetCurrentGameInstallDir) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(Nan::New("NOT IMPLEMENTED").ToLocalChecked());
}

NAN_METHOD(GetAppInstallDir) {
  Nan::HandleScope scope;

  if (info.Length() < 1 || !info[0]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments; expected: appid [uint32]");
  }

  AppId_t app_id = static_cast<AppId_t>(Nan::To<int32>(info[0]).FromJust());
  const int buffer_size =
      260;  // MAX_PATH on 32bit Windows according to MSDN documentation
  char buffer[buffer_size];
  uint32 length = SteamApps()->GetAppInstallDir(app_id, buffer, buffer_size);

  // The length takes \0 termination into account, we don't need it in JS.
  info.GetReturnValue().Set(Nan::New(buffer, length - 1).ToLocalChecked());
}

NAN_METHOD(GetNumberOfPlayers) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::Callback* success_callback =
      new Nan::Callback(info[0].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 1 && info[1]->IsFunction())
    error_callback = new Nan::Callback(info[1].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::GetNumberOfPlayersWorker(
      success_callback, error_callback));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(IsGameOverlayEnabled) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(Nan::New(SteamUtils()->IsOverlayEnabled()));
}

NAN_METHOD(IsSteamInBigPictureMode) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(Nan::New(SteamUtils()->IsSteamInBigPictureMode()));
}

NAN_METHOD(ActivateGameOverlay) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string option(*(Nan::Utf8String(info[0])));
  SteamFriends()->ActivateGameOverlay(option.c_str());
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ActivateGameOverlayToWebPage) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string url = *(Nan::Utf8String(info[0]));
  SteamFriends()->ActivateGameOverlayToWebPage(url.c_str());
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(IsSubscribedApp) {
  Nan::HandleScope scope;
  if (info.Length() < 1) {
    Nan::ThrowTypeError("You must pass an app ID to IsSubscribedApp");
    return;
  }

  if (!info[0]->IsUint32()) {
    Nan::ThrowTypeError("Your app ID argument should be an integer");
    return;
  }

  uint32 arg0 = Nan::To<uint32>(info[0]).FromJust();

  bool subscribed = SteamApps()->BIsSubscribedApp(arg0);
  info.GetReturnValue().Set(Nan::New(subscribed));
}

NAN_METHOD(IsAppInstalled) {
  Nan::HandleScope scope;
  if (info.Length() < 1 && !info[0]->IsUint32()) {
    THROW_BAD_ARGS("Bad arguments; expected: appid [uint32]");
  }
  AppId_t app_id = static_cast<AppId_t>(Nan::To<uint32>(info[0]).FromJust());
  bool installed = SteamApps()->BIsAppInstalled(app_id);
  info.GetReturnValue().Set(Nan::New(installed));
}

NAN_METHOD(GetImageSize) {
  Nan::HandleScope scope;
  if (info.Length() < 1 && !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  int image_handle = Nan::To<int32>(info[0]).FromJust();
  uint32 width = 0;
  uint32 height = 0;
  if (!SteamUtils()->GetImageSize(image_handle, &width, &height)) {
    THROW_BAD_ARGS("Fail to get image size");
  }
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("width").ToLocalChecked(), Nan::New(width));
  Nan::Set(result, Nan::New("height").ToLocalChecked(), Nan::New(height));
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetImageRGBA) {
  Nan::HandleScope scope;
  if (info.Length() < 1 && !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  int image_handle = Nan::To<int32>(info[0]).FromJust();
  uint32 width = 0;
  uint32 height = 0;
  if (!SteamUtils()->GetImageSize(image_handle, &width, &height)) {
    THROW_BAD_ARGS("Fail to get image size");
  }
  int buffer_size = 4 * width * height;
  auto* image_buffer = new char[buffer_size];
  if (!SteamUtils()->GetImageRGBA(image_handle,
                                  reinterpret_cast<uint8*>(image_buffer),
                                  buffer_size)) {
    THROW_BAD_ARGS("Fail to get image");
  }
  info.GetReturnValue().Set(
      Nan::NewBuffer(image_buffer, buffer_size, FreeCallback, nullptr)
          .ToLocalChecked());
}

NAN_METHOD(GetIPCountry) {
  Nan::HandleScope scope;
  const char* countryCode = SteamUtils()->GetIPCountry();
  info.GetReturnValue().Set(Nan::New(countryCode, 2).ToLocalChecked());
}

NAN_METHOD(GetLaunchCommandLine) {
  Nan::HandleScope scope;
  const int buffer_size = 260; // Same value used by Valve docs.
  char buffer[buffer_size];
  uint32 length = SteamApps()->GetLaunchCommandLine(buffer, buffer_size);

  info.GetReturnValue().Set(Nan::New(buffer, length - 1).ToLocalChecked());
}

NAN_METHOD(RunCallbacks) {
  Nan::HandleScope scope;
  SteamAPI_RunCallbacks();
  info.GetReturnValue().Set(Nan::Undefined());
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  Nan::Set(target,
           Nan::New("_version").ToLocalChecked(),
           Nan::New(GREENWORKS_VERSION).ToLocalChecked());

  SET_FUNCTION("restartAppIfNecessary", RestartAppIfNecessary);
  SET_FUNCTION("isSteamRunning", IsSteamRunning);
  SET_FUNCTION("getSteamId", GetSteamId);
  SET_FUNCTION("getAppId", GetAppId);
  SET_FUNCTION("getAppBuildId", GetAppBuildId);
  SET_FUNCTION("getCurrentGameLanguage", GetCurrentGameLanguage);
  SET_FUNCTION("getCurrentUILanguage", GetCurrentUILanguage);
  SET_FUNCTION("getCurrentGameInstallDir", GetCurrentGameInstallDir);
  SET_FUNCTION("getAppInstallDir", GetAppInstallDir);
  SET_FUNCTION("getNumberOfPlayers", GetNumberOfPlayers);
  SET_FUNCTION("isGameOverlayEnabled", IsGameOverlayEnabled);
  SET_FUNCTION("isSteamInBigPictureMode", IsSteamInBigPictureMode);
  SET_FUNCTION("activateGameOverlay", ActivateGameOverlay);
  SET_FUNCTION("activateGameOverlayToWebPage", ActivateGameOverlayToWebPage);
  SET_FUNCTION("isAppInstalled", IsAppInstalled);
  SET_FUNCTION("isSubscribedApp", IsSubscribedApp);
  SET_FUNCTION("getImageSize", GetImageSize);
  SET_FUNCTION("getImageRGBA", GetImageRGBA);
  SET_FUNCTION("getIPCountry", GetIPCountry);
  SET_FUNCTION("getLaunchCommandLine", GetLaunchCommandLine);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
