// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <string>
#include <sstream>
#include <vector>

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "greenworks_async_workers.h"
#include "greenworks_workshop_workers.h"
#include "greenworks_utils.h"
#include "greenworks_version.h"
#include "steam_client.h"

namespace {

#define THROW_BAD_ARGS(msg)    \
    do {                       \
       Nan::ThrowTypeError(msg); \
       return;                   \
    } while (0);


Nan::Persistent<v8::Object> g_persistent_steam_events;

class SteamEvent : public greenworks::SteamClient::Observer {
 public:
  // Override SteamClient::Observer methods.
  virtual void OnGameOverlayActivated(bool is_active);
  virtual void OnSteamServersConnected();
  virtual void OnSteamServersDisconnected();
  virtual void OnSteamServerConnectFailure(int status_code);
  virtual void OnSteamShutdown();
};

void SteamEvent::OnGameOverlayActivated(bool is_active) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("game-overlay-activated").ToLocalChecked(),
      Nan::New(is_active) };
  Nan::MakeCallback(
      Nan::New(g_persistent_steam_events), "on", 2, argv);
}

void SteamEvent::OnSteamServersConnected() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-servers-connected").ToLocalChecked() };
  Nan::MakeCallback(
      Nan::New(g_persistent_steam_events),"on", 1, argv);
}

void SteamEvent::OnSteamServersDisconnected() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-servers-disconnected").ToLocalChecked() };
  Nan::MakeCallback(
      Nan::New(g_persistent_steam_events), "on", 1, argv);
}

void SteamEvent::OnSteamServerConnectFailure(int status_code) {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New("steam-server-connect-failure").ToLocalChecked(),
      Nan::New(status_code) };
  Nan::MakeCallback(
      Nan::New(g_persistent_steam_events), "on", 2, argv);
}

void SteamEvent::OnSteamShutdown() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = { Nan::New("steam-shutdown").ToLocalChecked() };
  Nan::MakeCallback(
      Nan::New(g_persistent_steam_events), "on", 1, argv);
}

v8::Local<v8::Object> GetSteamUserCountType(int type_id) {
  v8::Local<v8::Object> account_type = Nan::New<v8::Object>();
  std::string name;
  switch (type_id) {
    case k_EAccountTypeAnonGameServer:
      name = "k_EAccountTypeAnonGameServer";
      break;
    case k_EAccountTypeAnonUser:
      name = "k_EAccountTypeAnonUser";
      break;
    case k_EAccountTypeChat:
      name = "k_EAccountTypeChat";
      break;
    case k_EAccountTypeClan:
      name = "k_EAccountTypeClan";
      break;
    case k_EAccountTypeConsoleUser:
      name = "k_EAccountTypeConsoleUser";
      break;
    case k_EAccountTypeContentServer:
      name = "k_EAccountTypeContentServer";
      break;
    case k_EAccountTypeGameServer:
      name = "k_EAccountTypeGameServer";
      break;
    case k_EAccountTypeIndividual:
      name = "k_EAccountTypeIndividual";
      break;
    case k_EAccountTypeInvalid:
      name = "k_EAccountTypeInvalid";
      break;
    case k_EAccountTypeMax:
      name = "k_EAccountTypeMax";
      break;
    case k_EAccountTypeMultiseat:
      name = "k_EAccountTypeMultiseat";
      break;
    case k_EAccountTypePending:
      name = "k_EAccountTypePending";
      break;
  }
  account_type->Set(Nan::New("name").ToLocalChecked(),
                    Nan::New(name).ToLocalChecked());
  account_type->Set(Nan::New("value").ToLocalChecked(), Nan::New(type_id));
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

  uint32 arg0 = info[0]->Uint32Value();

  bool restarting = SteamAPI_RestartAppIfNecessary(arg0);
  info.GetReturnValue().Set(Nan::New(restarting));
}

NAN_METHOD(InitAPI) {
  Nan::HandleScope scope;

  bool success = SteamAPI_Init();

  if (success) {
    ISteamUserStats* stream_user_stats = SteamUserStats();
    stream_user_stats->RequestCurrentStats();
  }

  greenworks::SteamClient::GetInstance()->AddObserver(new SteamEvent());
  greenworks::SteamClient::StartSteamLoop();
  info.GetReturnValue().Set(Nan::New(success));
}

NAN_METHOD(GetSteamId) {
  Nan::HandleScope scope;
  CSteamID user_id = SteamUser()->GetSteamID();
  v8::Local<v8::Object> flags = Nan::New<v8::Object>();
  flags->Set(Nan::New("anonymous").ToLocalChecked(), Nan::New(user_id.BAnonAccount()));
  flags->Set(Nan::New("anonymousGameServer").ToLocalChecked(),
      Nan::New(user_id.BAnonGameServerAccount()));
  flags->Set(Nan::New("anonymousGameServerLogin").ToLocalChecked(),
      Nan::New(user_id.BBlankAnonAccount()));
  flags->Set(Nan::New("anonymousUser").ToLocalChecked(),
             Nan::New(user_id.BAnonUserAccount()));
  flags->Set(Nan::New("chat").ToLocalChecked(),
             Nan::New(user_id.BChatAccount()));
  flags->Set(Nan::New("clan").ToLocalChecked(),
             Nan::New(user_id.BClanAccount()));
  flags->Set(Nan::New("consoleUser").ToLocalChecked(),
             Nan::New(user_id.BConsoleUserAccount()));
  flags->Set(Nan::New("contentServer").ToLocalChecked(),
             Nan::New(user_id.BContentServerAccount()));
  flags->Set(Nan::New("gameServer").ToLocalChecked(),
             Nan::New(user_id.BGameServerAccount()));
  flags->Set(Nan::New("individual").ToLocalChecked(),
             Nan::New(user_id.BIndividualAccount()));
  flags->Set(Nan::New("gameServerPersistent").ToLocalChecked(),
             Nan::New(user_id.BPersistentGameServerAccount()));
  flags->Set(Nan::New("lobby").ToLocalChecked(), Nan::New(user_id.IsLobby()));

  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  result->Set(Nan::New("flags").ToLocalChecked(), flags);
  result->Set(Nan::New("type").ToLocalChecked(),
              GetSteamUserCountType(user_id.GetEAccountType()));
  result->Set(Nan::New("accountId").ToLocalChecked(),
              Nan::New<v8::Integer>(user_id.GetAccountID()));
  result->Set(Nan::New("staticAccountId").ToLocalChecked(),
      Nan::New(utils::uint64ToString(
          user_id.GetStaticAccountKey())).ToLocalChecked());
  result->Set(Nan::New("isValid").ToLocalChecked(),
              Nan::New<v8::Integer>(user_id.IsValid()));
  result->Set(Nan::New("level").ToLocalChecked(),
              Nan::New<v8::Integer>(SteamUser()->GetPlayerSteamLevel()));

  if (!SteamFriends()->RequestUserInformation(user_id, true)) {
    result->Set(Nan::New("screenName").ToLocalChecked(),
                Nan::New(SteamFriends()->GetFriendPersonaName(user_id)).ToLocalChecked());
  } else {
    std::ostringstream sout;
    sout << user_id.GetAccountID();
    result->Set(Nan::New("screenName").ToLocalChecked(),
                Nan::New(sout.str()).ToLocalChecked());
  }
  info.GetReturnValue().Set(result);
}

NAN_METHOD(SaveTextToFile) {
  Nan::HandleScope scope;

  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string file_name(*(v8::String::Utf8Value(info[0])));
  std::string content(*(v8::String::Utf8Value(info[1])));
  Nan::Callback* success_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 3 && info[3]->IsFunction())
    error_callback = new Nan::Callback(info[3].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::FileContentSaveWorker(success_callback,
                                                            error_callback,
                                                            file_name,
                                                            content));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(SaveFilesToCloud) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsArray() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  v8::Local<v8::Array> files = info[0].As<v8::Array>();
  std::vector<std::string> files_path;
  for (uint32_t i = 0; i < files->Length(); ++i) {
    if (!files->Get(i)->IsString())
      THROW_BAD_ARGS("Bad arguments");
    v8::String::Utf8Value string_array(files->Get(i));
    // Ignore empty path.
    if (string_array.length() > 0)
      files_path.push_back(*string_array);
  }

  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::AsyncQueueWorker(new greenworks::FilesSaveWorker(success_callback,
                                                      error_callback,
                                                      files_path));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ReadTextFromFile) {
  Nan::HandleScope scope;

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string file_name(*(v8::String::Utf8Value(info[0])));
  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::FileReadWorker(success_callback,
                                                     error_callback,
                                                     file_name));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(IsCloudEnabled) {
  Nan::HandleScope scope;
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(
      steam_remote_storage->IsCloudEnabledForApp()));
}

NAN_METHOD(IsCloudEnabledForUser) {
  Nan::HandleScope scope;
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(
      steam_remote_storage->IsCloudEnabledForAccount()));
}

NAN_METHOD(EnableCloud) {
  Nan::HandleScope scope;

  if (info.Length() < 1) {
    THROW_BAD_ARGS("Bad arguments");
  }
  bool enable_flag = info[0]->BooleanValue();
  SteamRemoteStorage()->SetCloudEnabledForApp(enable_flag);
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetCloudQuota) {
  Nan::HandleScope scope;

  if (info.Length() < 1 || !info[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::Callback* success_callback = new Nan::Callback(info[0].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[1]->IsFunction())
    error_callback = new Nan::Callback(info[1].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::CloudQuotaGetWorker(success_callback,
                                                          error_callback));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ActivateAchievement) {
  Nan::HandleScope scope;

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string achievement = (*(v8::String::Utf8Value(info[0])));
  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::ActivateAchievementWorker(
      success_callback, error_callback, achievement));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetAchievement) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string achievement = (*(v8::String::Utf8Value(info[0])));
  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::AsyncQueueWorker(new greenworks::GetAchievementWorker(
      success_callback, error_callback, achievement));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ClearAchievement) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string achievement = (*(v8::String::Utf8Value(info[0])));
  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::ClearAchievementWorker(
      success_callback, error_callback, achievement));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetAchievementNames) {
  Nan::HandleScope scope;
  int count = static_cast<int>(SteamUserStats()->GetNumAchievements());
  v8::Local<v8::Array> names = Nan::New<v8::Array>(count);
  for (int i = 0; i < count; ++i) {
    names->Set(i,
       Nan::New(SteamUserStats()->GetAchievementName(i)).ToLocalChecked());
  }
  info.GetReturnValue().Set(names);
}

NAN_METHOD(GetNumberOfAchievements) {
  Nan::HandleScope scope;
  ISteamUserStats* steam_user_stats = SteamUserStats();
  info.GetReturnValue().Set(steam_user_stats->GetNumAchievements());
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

// TODO: Implement get game install directory.
NAN_METHOD(GetCurrentGameInstallDir) {
  Nan::HandleScope scope;
  info.GetReturnValue().Set(Nan::New("NOT IMPLEMENTED").ToLocalChecked());
}

NAN_METHOD(GetNumberOfPlayers) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::Callback* success_callback = new Nan::Callback(info[0].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

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

NAN_METHOD(ActivateGameOverlay) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string option(*(v8::String::Utf8Value(info[0])));
  SteamFriends()->ActivateGameOverlay(option.c_str());
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(FileShare) {
  Nan::HandleScope scope;

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string file_name(*(v8::String::Utf8Value(info[0])));
  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::FileShareWorker(
      success_callback, error_callback, file_name));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(PublishWorkshopFile) {
  Nan::HandleScope scope;

  if (info.Length() < 5 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsString() || !info[4]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string file_name(*(v8::String::Utf8Value(info[0])));
  std::string image_name(*(v8::String::Utf8Value(info[1])));
  std::string title(*(v8::String::Utf8Value(info[2])));
  std::string description(*(v8::String::Utf8Value(info[3])));

  Nan::Callback* success_callback = new Nan::Callback(info[4].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 5 && info[5]->IsFunction())
    error_callback = new Nan::Callback(info[5].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::PublishWorkshopFileWorker(
      success_callback, error_callback, file_name, image_name, title,
      description));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UpdatePublishedWorkshopFile) {
  Nan::HandleScope scope;

  if (info.Length() < 6 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsString() || !info[4]->IsString() ||
      !info[5]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  PublishedFileId_t published_file_id = utils::strToUint64(
      *(v8::String::Utf8Value(info[0])));
  std::string file_name(*(v8::String::Utf8Value(info[1])));
  std::string image_name(*(v8::String::Utf8Value(info[2])));
  std::string title(*(v8::String::Utf8Value(info[3])));
  std::string description(*(v8::String::Utf8Value(info[4])));

  Nan::Callback* success_callback = new Nan::Callback(info[5].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 6 && info[6]->IsFunction())
    error_callback = new Nan::Callback(info[6].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::UpdatePublishedWorkshopFileWorker(
      success_callback, error_callback, published_file_id, file_name,
      image_name, title, description));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCGetItems) {
  Nan::HandleScope scope;
  if (info.Length() < 3 || !info[0]->IsInt32() || !info[1]->IsInt32() ||
      !info[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  EUGCMatchingUGCType ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      info[0]->Int32Value());
  EUGCQuery ugc_query_type = static_cast<EUGCQuery>(info[1]->Int32Value());

  Nan::Callback* success_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 3 && info[3]->IsFunction())
    error_callback = new Nan::Callback(info[3].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::QueryAllUGCWorker(
      success_callback, error_callback, ugc_matching_type, ugc_query_type));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCGetUserItems) {
  Nan::HandleScope scope;
  if (info.Length() < 4 || !info[0]->IsInt32() || !info[1]->IsInt32() ||
      !info[2]->IsInt32() || !info[3]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  EUGCMatchingUGCType ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      info[0]->Int32Value());
  EUserUGCListSortOrder ugc_list_order = static_cast<EUserUGCListSortOrder>(
      info[1]->Int32Value());
  EUserUGCList ugc_list = static_cast<EUserUGCList>(info[2]->Int32Value());

  Nan::Callback* success_callback = new Nan::Callback(info[3].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 4 && info[4]->IsFunction())
    error_callback = new Nan::Callback(info[4].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::QueryUserUGCWorker(
      success_callback, error_callback, ugc_matching_type, ugc_list,
      ugc_list_order));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCDownloadItem) {
  Nan::HandleScope scope;
  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  UGCHandle_t download_file_handle = utils::strToUint64(
      *(v8::String::Utf8Value(info[0])));
  std::string download_dir = *(v8::String::Utf8Value(info[1]));

  Nan::Callback* success_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 3 && info[3]->IsFunction())
    error_callback = new Nan::Callback(info[3].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::DownloadItemWorker(
      success_callback, error_callback, download_file_handle, download_dir));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCSynchronizeItems) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string download_dir = *(v8::String::Utf8Value(info[0]));

  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::SynchronizeItemsWorker(
      success_callback, error_callback, download_dir));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCShowOverlay) {
  Nan::HandleScope scope;
  std::string steam_store_url;
  if (info.Length() < 1) {
    uint32 appId = SteamUtils()->GetAppID();
    steam_store_url = "http://steamcommunity.com/app/" +
        utils::uint64ToString(appId) + "/workshop/";
  } else {
    if (!info[0]->IsString()) {
      THROW_BAD_ARGS("Bad arguments");
    }
    std::string item_id = *(v8::String::Utf8Value(info[0]));
    steam_store_url = "http://steamcommunity.com/sharedfiles/filedetails/?id="
      + item_id;
  }

  SteamFriends()->ActivateGameOverlayToWebPage(steam_store_url.c_str());
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCUnsubscribe) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  PublishedFileId_t unsubscribed_file_id = utils::strToUint64(
      *(v8::String::Utf8Value(info[0])));
  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::UnsubscribePublishedFileWorker(
      success_callback, error_callback, unsubscribed_file_id));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(CreateArchive) {
  Nan::HandleScope scope;
  if (info.Length() < 5 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsInt32() || !info[4]->IsFunction()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string zip_file_path = *(v8::String::Utf8Value(info[0]));
  std::string source_dir = *(v8::String::Utf8Value(info[1]));
  std::string password = *(v8::String::Utf8Value(info[2]));
  int compress_level = info[3]->Int32Value();

  Nan::Callback* success_callback = new Nan::Callback(info[4].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 5 && info[5]->IsFunction())
    error_callback = new Nan::Callback(info[5].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::CreateArchiveWorker(
      success_callback, error_callback, zip_file_path, source_dir, password,
      compress_level));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ExtractArchive) {
  Nan::HandleScope scope;
  if (info.Length() < 4 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsFunction()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string zip_file_path = *(v8::String::Utf8Value(info[0]));
  std::string extract_dir = *(v8::String::Utf8Value(info[1]));
  std::string password = *(v8::String::Utf8Value(info[2]));

  Nan::Callback* success_callback = new Nan::Callback(info[3].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 4 && info[4]->IsFunction())
    error_callback = new Nan::Callback(info[4].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::ExtractArchiveWorker(
      success_callback, error_callback, zip_file_path, extract_dir, password));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetAuthSessionTicket) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::Callback* success_callback = new Nan::Callback(info[0].As<v8::Function>());
  Nan::Callback* error_callback = NULL;
  if (info.Length() > 1 && info[1]->IsFunction())
    error_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::AsyncQueueWorker(new greenworks::GetAuthSessionTicketWorker(
    success_callback, error_callback));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(CancelAuthTicket) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  HAuthTicket h = info[1].As<v8::Number>()->Int32Value();
  SteamUser()->CancelAuthTicket(h);
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(GetEncryptedAppTicket) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  char* user_data = *(static_cast<v8::String::Utf8Value>(info[0]->ToString()));
  if (!user_data) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::Callback* success_callback = new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;
  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::AsyncQueueWorker(new greenworks::RequestEncryptedAppTicketWorker(
    user_data, success_callback, error_callback));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(ActivateGameOverlayToWebPage) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string url = *(v8::String::Utf8Value(info[0]));
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

  uint32 arg0 = info[0]->Uint32Value();

  bool subscribed = SteamApps()->BIsSubscribedApp();
  info.GetReturnValue().Set(Nan::New(subscribed));
}

void InitUtilsObject(v8::Handle<v8::Object> exports) {
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>();
  tpl->Set(Nan::New("createArchive").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(CreateArchive)->GetFunction());
  tpl->Set(Nan::New("extractArchive").ToLocalChecked(),
      Nan::New<v8::FunctionTemplate>(ExtractArchive)->GetFunction());
  Nan::Persistent<v8::Function> constructor;
  constructor.Reset(tpl->GetFunction());
  Nan::Set(exports, Nan::New("Utils").ToLocalChecked(), tpl->GetFunction());
}

NAN_MODULE_INIT(init) {
  // Set internal steam event handler.
  v8::Local<v8::Object> steam_events = Nan::New<v8::Object>();
  g_persistent_steam_events.Reset(steam_events);
  Nan::Set(target, Nan::New("_steam_events").ToLocalChecked(), steam_events);

  // Set versions.
  Nan::Set(target,
           Nan::New("_version").ToLocalChecked(),
           Nan::New(GREENWORKS_VERSION).ToLocalChecked());
  // Common APIs.
  Nan::Set(target,
           Nan::New("initAPI").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(InitAPI)->GetFunction());
  Nan::Set(target,
           Nan::New("restartAppIfNecessary").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(RestartAppIfNecessary)->GetFunction());
  Nan::Set(target,
           Nan::New("getSteamId").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetSteamId)->GetFunction());
  // File related APIs.
  Nan::Set(target,
           Nan::New("saveTextToFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(SaveTextToFile)->GetFunction());
  Nan::Set(target,
           Nan::New("readTextFromFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(ReadTextFromFile)->GetFunction());
  Nan::Set(target,
           Nan::New("saveFilesToCloud").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(SaveFilesToCloud)->GetFunction());
  // Cloud related APIs.
  Nan::Set(target,
           Nan::New("isCloudEnabled").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(IsCloudEnabled)->GetFunction());
  Nan::Set(target,
           Nan::New("isCloudEnabledForUser").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               IsCloudEnabledForUser)->GetFunction());
  Nan::Set(target,
           Nan::New("enableCloud").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(EnableCloud)->GetFunction());
  Nan::Set(target,
           Nan::New("getCloudQuota").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetCloudQuota)->GetFunction());
  // Achievement related APIs.
  Nan::Set(target,
           Nan::New("activateAchievement").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(ActivateAchievement)->GetFunction());
  Nan::Set(target,
           Nan::New("getAchievement").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetAchievement)->GetFunction());
  Nan::Set(target,
           Nan::New("clearAchievement").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(ClearAchievement)->GetFunction());
  Nan::Set(target,
           Nan::New("getAchievementNames").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetAchievementNames)->GetFunction());
  Nan::Set(target,
           Nan::New("getNumberOfAchievements").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               GetNumberOfAchievements)->GetFunction());
  // Game setting related APIs.
  Nan::Set(target,
           Nan::New("getCurrentGameLanguage").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               GetCurrentGameLanguage)->GetFunction());
  Nan::Set(target,
           Nan::New("getCurrentUILanguage").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetCurrentUILanguage)->GetFunction());
  Nan::Set(target,
           Nan::New("getCurrentGameInstallDir").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               GetCurrentGameInstallDir)->GetFunction());
  Nan::Set(target,
           Nan::New("getNumberOfPlayers").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetNumberOfPlayers)->GetFunction());
  Nan::Set(target,
           Nan::New("isGameOverlayEnabled").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(IsGameOverlayEnabled)->GetFunction());
  Nan::Set(target,
           Nan::New("activateGameOverlay").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(ActivateGameOverlay)->GetFunction());
  Nan::Set(target,
           Nan::New("activateGameOverlayToWebPage").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               ActivateGameOverlayToWebPage)->GetFunction());
  // WorkShop related APIs
  Nan::Set(target,
           Nan::New("fileShare").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(FileShare)->GetFunction());
  Nan::Set(target,
           Nan::New("publishWorkshopFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(PublishWorkshopFile)->GetFunction());
  Nan::Set(target,
           Nan::New("updatePublishedWorkshopFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               UpdatePublishedWorkshopFile)->GetFunction());
  Nan::Set(target,
           Nan::New("ugcGetItems").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCGetItems)->GetFunction());
  Nan::Set(target,
           Nan::New("ugcGetUserItems").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCGetUserItems)->GetFunction());
  Nan::Set(target,
           Nan::New("ugcDownloadItem").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCDownloadItem)->GetFunction());
  Nan::Set(target,
           Nan::New("ugcSynchronizeItems").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCSynchronizeItems)->GetFunction());
  Nan::Set(target,
           Nan::New("ugcShowOverlay").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCShowOverlay)->GetFunction());
  Nan::Set(target,
           Nan::New("ugcUnsubscribe").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCUnsubscribe)->GetFunction());
  // Authentication related APIs
  Nan::Set(target,
           Nan::New("getAuthSessionTicket").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(GetAuthSessionTicket)->GetFunction());
  Nan::Set(target,
           Nan::New("getEncryptedAppTicket").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               GetEncryptedAppTicket)->GetFunction());
  Nan::Set(target,
           Nan::New("cancelAuthTicket").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(CancelAuthTicket)->GetFunction());
Nan::Set(target,
           Nan::New("isSubscribedApp").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(IsSubscribedApp)->GetFunction());

  utils::InitUgcMatchingTypes(target);
  utils::InitUgcQueryTypes(target);
  utils::InitUserUgcListSortOrder(target);
  utils::InitUserUgcList(target);

  // Utils related APIs.
  InitUtilsObject(target);
}

}  // namespace

#if defined(_WIN32)
  #if defined(_M_IX86)
    NODE_MODULE(greenworks_win32, init)
  #elif defined(_M_AMD64)
    NODE_MODULE(greenworks_win64, init)
  #endif
#elif defined(__APPLE__)
  #if defined(__x86_64__) || defined(__ppc64__)
    NODE_MODULE(greenworks_osx64, init)
  #else
    NODE_MODULE(greenworks_osx32, init)
  #endif
#elif defined(__linux__)
  #if defined(__x86_64__) || defined(__ppc64__)
    NODE_MODULE(greenworks_linux64, init)
  #else
    NODE_MODULE(greenworks_linux32, init)
  #endif
#endif
