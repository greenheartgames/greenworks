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

namespace {

#define THROW_BAD_ARGS(msg)    \
    do {                       \
       NanThrowTypeError(msg); \
       NanReturnUndefined();   \
    } while (0);

v8::Local<v8::Object> GetSteamUserCountType(int type_id) {
  v8::Local<v8::Object> account_type = NanNew<v8::Object>();
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
  account_type->Set(NanNew("name"), NanNew(name));
  account_type->Set(NanNew("value"), NanNew(type_id));
  return account_type;
}

NAN_METHOD(InitAPI) {
  NanScope();

  bool success = SteamAPI_Init();

  if (success) {
    ISteamUserStats* stream_user_stats = SteamUserStats();
    stream_user_stats->RequestCurrentStats();
  }

  NanReturnValue(NanNew(success));
}

NAN_METHOD(GetSteamId) {
  NanScope();
  CSteamID user_id = SteamUser()->GetSteamID();
  v8::Local<v8::Object> flags = NanNew<v8::Object>();
  flags->Set(NanNew("anonymous"), NanNew(user_id.BAnonAccount()));
  flags->Set(NanNew("anonymousGameServer"),
      NanNew(user_id.BAnonGameServerAccount()));
  flags->Set(NanNew("anonymousGameServerLogin"),
      NanNew(user_id.BBlankAnonAccount()));
  flags->Set(NanNew("anonymousUser"), NanNew(user_id.BAnonUserAccount()));
  flags->Set(NanNew("chat"), NanNew(user_id.BChatAccount()));
  flags->Set(NanNew("clan"), NanNew(user_id.BClanAccount()));
  flags->Set(NanNew("consoleUser"), NanNew(user_id.BConsoleUserAccount()));
  flags->Set(NanNew("contentServer"), NanNew(user_id.BContentServerAccount()));
  flags->Set(NanNew("gameServer"), NanNew(user_id.BGameServerAccount()));
  flags->Set(NanNew("individual"), NanNew(user_id.BIndividualAccount()));
  flags->Set(NanNew("gameServerPersistent"),
      NanNew(user_id.BPersistentGameServerAccount()));
  flags->Set(NanNew("lobby"), NanNew(user_id.IsLobby()));

  v8::Local<v8::Object> result = NanNew<v8::Object>();
  result->Set(NanNew("flags"), flags);
  result->Set(NanNew("type"), GetSteamUserCountType(user_id.GetEAccountType()));
  result->Set(NanNew("accountId"), NanNew<v8::Integer>(user_id.GetAccountID()));
  result->Set(NanNew("staticAccountId"), NanNew<v8::String>(
      utils::uint64ToString(user_id.GetStaticAccountKey())));
  result->Set(NanNew("isValid"), NanNew<v8::Integer>(user_id.IsValid()));
  result->Set(NanNew("level"), NanNew<v8::Integer>(
        SteamUser()->GetPlayerSteamLevel()));

  if (!SteamFriends()->RequestUserInformation(user_id, true)) {
    result->Set(NanNew("screenName"),
                NanNew(SteamFriends()->GetFriendPersonaName(user_id)));
  } else {
    std::ostringstream sout;
    sout << user_id.GetAccountID();
    result->Set(NanNew("screenName"), NanNew<v8::String>(sout.str()));
  }
  NanReturnValue(result);
}

NAN_METHOD(SaveTextToFile) {
  NanScope();

  if (args.Length() < 3 || !args[0]->IsString() || !args[1]->IsString() ||
      !args[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string file_name(*(v8::String::Utf8Value(args[0])));
  std::string content(*(v8::String::Utf8Value(args[1])));
  NanCallback* success_callback = new NanCallback(args[2].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 3 && args[3]->IsFunction())
    error_callback = new NanCallback(args[3].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::FileContentSaveWorker(success_callback,
                                                            error_callback,
                                                            file_name,
                                                            content));
  NanReturnUndefined();
}

NAN_METHOD(SaveFilesToCloud) {
  NanScope();
  if (args.Length() < 2 || !args[0]->IsArray() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  v8::Local<v8::Array> files = args[0].As<v8::Array>();
  std::vector<std::string> files_path;
  for (uint32_t i = 0; i < files->Length(); ++i) {
    if (!files->Get(i)->IsString())
      THROW_BAD_ARGS("Bad arguments");
    v8::String::Utf8Value string_array(files->Get(i));
    // Ignore empty path.
    if (string_array.length() > 0)
      files_path.push_back(*string_array);
  }

  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());
  NanAsyncQueueWorker(new greenworks::FilesSaveWorker(success_callback,
                                                      error_callback,
                                                      files_path));
  NanReturnUndefined();
}

NAN_METHOD(ReadTextFromFile) {
  NanScope();

  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string file_name(*(v8::String::Utf8Value(args[0])));
  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::FileReadWorker(success_callback,
                                                     error_callback,
                                                     file_name));
  NanReturnUndefined();
}

NAN_METHOD(IsCloudEnabled) {
  NanScope();
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();
  NanReturnValue(NanNew<v8::Boolean>(
      steam_remote_storage->IsCloudEnabledForApp()));
}

NAN_METHOD(IsCloudEnabledForUser) {
  NanScope();
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();
  NanReturnValue(NanNew<v8::Boolean>(
      steam_remote_storage->IsCloudEnabledForAccount()));
}

NAN_METHOD(EnableCloud) {
  NanScope();

  if (args.Length() < 1) {
    THROW_BAD_ARGS("Bad arguments");
  }
  bool enable_flag = args[0]->BooleanValue();
  SteamRemoteStorage()->SetCloudEnabledForApp(enable_flag);
  NanReturnUndefined();
}

NAN_METHOD(GetCloudQuota) {
  NanScope();

  if (args.Length() < 1 || !args[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  NanCallback* success_callback = new NanCallback(args[0].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[1]->IsFunction())
    error_callback = new NanCallback(args[1].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::CloudQuotaGetWorker(success_callback,
                                                          error_callback));
  NanReturnUndefined();
}

NAN_METHOD(ActivateAchievement) {
  NanScope();

  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string achievement = (*(v8::String::Utf8Value(args[0])));
  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::ActivateAchievementWorker(
      success_callback, error_callback, achievement));
  NanReturnUndefined();
}

NAN_METHOD(GetAchievement) {
  NanScope();
  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string achievement = (*(v8::String::Utf8Value(args[0])));
  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());
  NanAsyncQueueWorker(new greenworks::GetAchievementWorker(
      success_callback, error_callback, achievement));
  NanReturnUndefined();
}

NAN_METHOD(ClearAchievement) {
  NanScope();
  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string achievement = (*(v8::String::Utf8Value(args[0])));
  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::ClearAchievementWorker(
      success_callback, error_callback, achievement));
  NanReturnUndefined();
}

NAN_METHOD(GetAchievementNames) {
  NanScope();
  int count = static_cast<int>(SteamUserStats()->GetNumAchievements());
  v8::Local<v8::Array> names = NanNew<v8::Array>(count);
  for (int i = 0; i < count; ++i) {
    names->Set(i, NanNew(SteamUserStats()->GetAchievementName(i)));
  }
  NanReturnValue(names);
}

NAN_METHOD(GetNumberOfAchievements) {
  NanScope();
  ISteamUserStats* steam_user_stats = SteamUserStats();
  NanReturnValue(steam_user_stats->GetNumAchievements());
}

NAN_METHOD(GetCurrentGameLanguage) {
  NanScope();
  NanReturnValue(NanNew(SteamApps()->GetCurrentGameLanguage()));
}

NAN_METHOD(GetCurrentUILanguage) {
  NanScope();
  NanReturnValue(NanNew(SteamUtils()->GetSteamUILanguage()));
}

// TODO: Implement get game install directory.
NAN_METHOD(GetCurrentGameInstallDir) {
  NanScope();
  NanReturnValue(NanNew("NOT IMPLEMENTED"));
}

NAN_METHOD(GetNumberOfPlayers) {
  NanScope();
  if (args.Length() < 1 || !args[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  NanCallback* success_callback = new NanCallback(args[0].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 1 && args[1]->IsFunction())
    error_callback = new NanCallback(args[1].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::GetNumberOfPlayersWorker(
      success_callback, error_callback));
  NanReturnUndefined();
}

NAN_METHOD(IsGameOverlayEnabled) {
  NanScope();
  NanReturnValue(NanNew(SteamUtils()->IsOverlayEnabled()));
}

NAN_METHOD(ActivateGameOverlay) {
  NanScope();
  if (args.Length() < 1 || !args[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string option(*(v8::String::Utf8Value(args[0])));
  SteamFriends()->ActivateGameOverlay(option.c_str());
  NanReturnUndefined();
}

NAN_METHOD(FileShare) {
  NanScope();

  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string file_name(*(v8::String::Utf8Value(args[0])));
  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::FileShareWorker(
      success_callback, error_callback, file_name));
  NanReturnUndefined();
}

NAN_METHOD(PublishWorkshopFile) {
  NanScope();

  if (args.Length() < 5 || !args[0]->IsString() || !args[1]->IsString() ||
      !args[2]->IsString() || !args[3]->IsString() || !args[4]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string file_name(*(v8::String::Utf8Value(args[0])));
  std::string image_name(*(v8::String::Utf8Value(args[1])));
  std::string title(*(v8::String::Utf8Value(args[2])));
  std::string description(*(v8::String::Utf8Value(args[3])));

  NanCallback* success_callback = new NanCallback(args[4].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 5 && args[5]->IsFunction())
    error_callback = new NanCallback(args[5].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::PublishWorkshopFileWorker(
      success_callback, error_callback, file_name, image_name, title,
      description));
  NanReturnUndefined();
}

NAN_METHOD(UpdatePublishedWorkshopFile) {
  NanScope();

  if (args.Length() < 6 || !args[0]->IsString() || !args[1]->IsString() ||
      !args[2]->IsString() || !args[3]->IsString() || !args[4]->IsString() ||
      !args[5]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  PublishedFileId_t published_file_id = utils::strToUint64(
      *(v8::String::Utf8Value(args[0])));
  std::string file_name(*(v8::String::Utf8Value(args[1])));
  std::string image_name(*(v8::String::Utf8Value(args[2])));
  std::string title(*(v8::String::Utf8Value(args[3])));
  std::string description(*(v8::String::Utf8Value(args[4])));

  NanCallback* success_callback = new NanCallback(args[5].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 6 && args[6]->IsFunction())
    error_callback = new NanCallback(args[6].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::UpdatePublishedWorkshopFileWorker(
      success_callback, error_callback, published_file_id, file_name,
      image_name, title, description));
  NanReturnUndefined();
}

NAN_METHOD(UGCGetItems) {
  NanScope();
  if (args.Length() < 3 || !args[0]->IsInt32() || !args[1]->IsInt32() ||
      !args[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  EUGCMatchingUGCType ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      args[0]->Int32Value());
  EUGCQuery ugc_query_type = static_cast<EUGCQuery>(args[1]->Int32Value());

  NanCallback* success_callback = new NanCallback(args[2].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 3 && args[3]->IsFunction())
    error_callback = new NanCallback(args[3].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::QueryAllUGCWorker(
      success_callback, error_callback, ugc_matching_type, ugc_query_type));
  NanReturnUndefined();
}

NAN_METHOD(UGCGetUserItems) {
  NanScope();
  if (args.Length() < 4 || !args[0]->IsInt32() || !args[1]->IsInt32() ||
      !args[2]->IsInt32() || !args[3]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  EUGCMatchingUGCType ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      args[0]->Int32Value());
  EUserUGCListSortOrder ugc_list_order = static_cast<EUserUGCListSortOrder>(
      args[1]->Int32Value());
  EUserUGCList ugc_list = static_cast<EUserUGCList>(args[2]->Int32Value());

  NanCallback* success_callback = new NanCallback(args[3].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 4 && args[4]->IsFunction())
    error_callback = new NanCallback(args[4].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::QueryUserUGCWorker(
      success_callback, error_callback, ugc_matching_type, ugc_list,
      ugc_list_order));
  NanReturnUndefined();
}

NAN_METHOD(UGCDownloadItem) {
  NanScope();
  if (args.Length() < 3 || !args[0]->IsString() || !args[1]->IsString() ||
      !args[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  UGCHandle_t download_file_handle = utils::strToUint64(
      *(v8::String::Utf8Value(args[0])));
  std::string download_dir = *(v8::String::Utf8Value(args[1]));

  NanCallback* success_callback = new NanCallback(args[2].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 3 && args[3]->IsFunction())
    error_callback = new NanCallback(args[3].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::DownloadItemWorker(
      success_callback, error_callback, download_file_handle, download_dir));
  NanReturnUndefined();
}

NAN_METHOD(UGCSynchronizeItems) {
  NanScope();
  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string download_dir = *(v8::String::Utf8Value(args[0]));

  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::SynchronizeItemsWorker(
      success_callback, error_callback, download_dir));
  NanReturnUndefined();
}

NAN_METHOD(UGCShowOverlay) {
  NanScope();
  std::string steam_store_url;
  if (args.Length() < 1) {
    uint32 appId = SteamUtils()->GetAppID();
    steam_store_url = "http://steamcommunity.com/app/" +
        utils::uint64ToString(appId) + "/workshop/";
  } else {
    if (!args[0]->IsString()) {
      THROW_BAD_ARGS("Bad arguments");
    }
    std::string item_id = *(v8::String::Utf8Value(args[0]));
    steam_store_url = "http://steamcommunity.com/sharedfiles/filedetails/?id="
      + item_id;
  }

  SteamFriends()->ActivateGameOverlayToWebPage(steam_store_url.c_str());
  NanReturnUndefined();
}

NAN_METHOD(UGCUnsubscribe) {
  NanScope();
  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  PublishedFileId_t unsubscribed_file_id = utils::strToUint64(
      *(v8::String::Utf8Value(args[0])));
  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::UnsubscribePublishedFileWorker(
      success_callback, error_callback, unsubscribed_file_id));
  NanReturnUndefined();
}

NAN_METHOD(CreateArchive) {
  NanScope();
  if (args.Length() < 5 || !args[0]->IsString() || !args[1]->IsString() ||
      !args[2]->IsString() || !args[3]->IsInt32() || !args[4]->IsFunction()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string zip_file_path = *(v8::String::Utf8Value(args[0]));
  std::string source_dir = *(v8::String::Utf8Value(args[1]));
  std::string password = *(v8::String::Utf8Value(args[2]));
  int compress_level = args[3]->Int32Value();

  NanCallback* success_callback = new NanCallback(args[4].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 5 && args[5]->IsFunction())
    error_callback = new NanCallback(args[5].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::CreateArchiveWorker(
      success_callback, error_callback, zip_file_path, source_dir, password,
      compress_level));
  NanReturnUndefined();
}

NAN_METHOD(ExtractArchive) {
  NanScope();
  if (args.Length() < 4 || !args[0]->IsString() || !args[1]->IsString() ||
      !args[2]->IsString() || !args[3]->IsFunction()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string zip_file_path = *(v8::String::Utf8Value(args[0]));
  std::string extract_dir = *(v8::String::Utf8Value(args[1]));
  std::string password = *(v8::String::Utf8Value(args[2]));

  NanCallback* success_callback = new NanCallback(args[3].As<v8::Function>());
  NanCallback* error_callback = NULL;

  if (args.Length() > 4 && args[4]->IsFunction())
    error_callback = new NanCallback(args[4].As<v8::Function>());

  NanAsyncQueueWorker(new greenworks::ExtractArchiveWorker(
      success_callback, error_callback, zip_file_path, extract_dir, password));
  NanReturnUndefined();
}

NAN_METHOD(GetAuthSessionTicket) {
  NanScope();
  if (args.Length() < 1 || !args[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  NanCallback* success_callback = new NanCallback(args[0].As<v8::Function>());
  NanCallback* error_callback = NULL;
  if (args.Length() > 1 && args[1]->IsFunction())
    error_callback = new NanCallback(args[1].As<v8::Function>());
  NanAsyncQueueWorker(new greenworks::GetAuthSessionTicketWorker(
    success_callback, error_callback));
  NanReturnUndefined();
}

NAN_METHOD(CancelAuthTicket) {
  NanScope();
  if (args.Length() < 1 || !args[0]->IsNumber()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  HAuthTicket h = args[1].As<v8::Number>()->Int32Value();
  SteamUser()->CancelAuthTicket(h);
  NanReturnUndefined();
}

NAN_METHOD(GetEncryptedAppTicket) {
  NanScope();
  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  char* user_data = *(static_cast<v8::String::Utf8Value>(args[0]->ToString()));
  if (!user_data) {
    THROW_BAD_ARGS("Bad arguments");
  }
  NanCallback* success_callback = new NanCallback(args[1].As<v8::Function>());
  NanCallback* error_callback = NULL;
  if (args.Length() > 2 && args[2]->IsFunction())
    error_callback = new NanCallback(args[2].As<v8::Function>());
  NanAsyncQueueWorker(new greenworks::RequestEncryptedAppTicketWorker(
    user_data, success_callback, error_callback));
  NanReturnUndefined();
}

NAN_METHOD(ActivateGameOverlayToWebPage) {
  NanScope();
  if (args.Length() < 1 || !args[0]->IsString()) {
    THROW_BAD_ARGS("bad arguments");
  }
  std::string url = *(v8::String::Utf8Value(args[0]));
  SteamFriends()->ActivateGameOverlayToWebPage(url.c_str());
  NanReturnUndefined();
}

void InitUtilsObject(v8::Handle<v8::Object> exports) {
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>();
  tpl->Set(NanNew("createArchive"),
      NanNew<v8::FunctionTemplate>(CreateArchive)->GetFunction());
  tpl->Set(NanNew("extractArchive"),
      NanNew<v8::FunctionTemplate>(ExtractArchive)->GetFunction());
  v8::Persistent<v8::Function> constructor;
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew("Utils"), tpl->GetFunction());
}

void init(v8::Handle<v8::Object> exports) {
  // Common APIs.
  exports->Set(NanNew("initAPI"),
               NanNew<v8::FunctionTemplate>(InitAPI)->GetFunction());
  exports->Set(NanNew("getSteamId"),
               NanNew<v8::FunctionTemplate>(GetSteamId)->GetFunction());
  // File related APIs.
  exports->Set(NanNew("saveTextToFile"),
               NanNew<v8::FunctionTemplate>(SaveTextToFile)->GetFunction());
  exports->Set(NanNew("readTextFromFile"),
               NanNew<v8::FunctionTemplate>(ReadTextFromFile)->GetFunction());
  exports->Set(NanNew("saveFilesToCloud"),
               NanNew<v8::FunctionTemplate>(SaveFilesToCloud)->GetFunction());
  // Cloud related APIs.
  exports->Set(NanNew("isCloudEnabled"),
               NanNew<v8::FunctionTemplate>(IsCloudEnabled)->GetFunction());
  exports->Set(NanNew("isCloudEnabledForUser"),
               NanNew<v8::FunctionTemplate>(
                   IsCloudEnabledForUser)->GetFunction());
  exports->Set(NanNew("enableCloud"),
               NanNew<v8::FunctionTemplate>(EnableCloud)->GetFunction());
  exports->Set(NanNew("getCloudQuota"),
               NanNew<v8::FunctionTemplate>(GetCloudQuota)->GetFunction());
  // Achievement related APIs.
  exports->Set(NanNew("activateAchievement"),
               NanNew<v8::FunctionTemplate>(
                   ActivateAchievement)->GetFunction());
  exports->Set(NanNew("getAchievement"),
               NanNew<v8::FunctionTemplate>(GetAchievement)->GetFunction());
  exports->Set(NanNew("clearAchievement"),
               NanNew<v8::FunctionTemplate>(ClearAchievement)->GetFunction());
  exports->Set(NanNew("getAchievementNames"),
               NanNew<v8::FunctionTemplate>(
                   GetAchievementNames)->GetFunction());
  exports->Set(NanNew("getNumberOfAchievements"),
               NanNew<v8::FunctionTemplate>(
                   GetNumberOfAchievements)->GetFunction());
  // Game setting related APIs.
  exports->Set(NanNew("getCurrentGameLanguage"),
                      NanNew<v8::FunctionTemplate>(
                          GetCurrentGameLanguage)->GetFunction());
  exports->Set(NanNew("getCurrentUILanguage"),
               NanNew<v8::FunctionTemplate>(
                   GetCurrentUILanguage)->GetFunction());
  exports->Set(NanNew("getCurrentGameInstallDir"),
               NanNew<v8::FunctionTemplate>(
                   GetCurrentGameInstallDir)->GetFunction());
  exports->Set(NanNew("getNumberOfPlayers"),
               NanNew<v8::FunctionTemplate>(GetNumberOfPlayers)->GetFunction());
  exports->Set(NanNew("isGameOverlayEnabled"),
               NanNew<v8::FunctionTemplate>(
                   IsGameOverlayEnabled)->GetFunction());
  exports->Set(NanNew("activateGameOverlay"),
               NanNew<v8::FunctionTemplate>(
                   ActivateGameOverlay)->GetFunction());
  exports->Set(NanNew("activateGameOverlayToWebPage"),
               NanNew<v8::FunctionTemplate>(
                   ActivateGameOverlayToWebPage)->GetFunction());
  // WorkShop related APIs
  exports->Set(NanNew("fileShare"),
               NanNew<v8::FunctionTemplate>(FileShare)->GetFunction());
  exports->Set(NanNew("publishWorkshopFile"),
               NanNew<v8::FunctionTemplate>(
                   PublishWorkshopFile)->GetFunction());
  exports->Set(NanNew("updatePublishedWorkshopFile"),
               NanNew<v8::FunctionTemplate>(
                   UpdatePublishedWorkshopFile)->GetFunction());
  exports->Set(NanNew("ugcGetItems"),
               NanNew<v8::FunctionTemplate>(UGCGetItems)->GetFunction());
  exports->Set(NanNew("ugcGetUserItems"),
               NanNew<v8::FunctionTemplate>(UGCGetUserItems)->GetFunction());
  exports->Set(NanNew("ugcDownloadItem"),
               NanNew<v8::FunctionTemplate>(UGCDownloadItem)->GetFunction());
  exports->Set(NanNew("ugcSynchronizeItems"),
               NanNew<v8::FunctionTemplate>(
                   UGCSynchronizeItems)->GetFunction());
  exports->Set(NanNew("ugcShowOverlay"),
               NanNew<v8::FunctionTemplate>(UGCShowOverlay)->GetFunction());
  exports->Set(NanNew("ugcUnsubscribe"),
               NanNew<v8::FunctionTemplate>(UGCUnsubscribe)->GetFunction());
  // Authentication related APIs
  exports->Set(NanNew("getAuthSessionTicket"),
               NanNew<v8::FunctionTemplate>(
                   GetAuthSessionTicket)->GetFunction());
  exports->Set(NanNew("getEncryptedAppTicket"),
               NanNew<v8::FunctionTemplate>(
                   GetEncryptedAppTicket)->GetFunction());
  exports->Set(NanNew("cancelAuthTicket"),
               NanNew<v8::FunctionTemplate>(CancelAuthTicket)->GetFunction());

  utils::InitUgcMatchingTypes(exports);
  utils::InitUgcQueryTypes(exports);
  utils::InitUserUgcListSortOrder(exports);
  utils::InitUserUgcList(exports);

  // Utils related APIs.
  InitUtilsObject(exports);
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
