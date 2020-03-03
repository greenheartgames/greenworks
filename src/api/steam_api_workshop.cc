// Copyright (c) 2016 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "greenworks_async_workers.h"
#include "steam_api_registry.h"

namespace greenworks {
namespace api {
namespace {

void InitUgcMatchingTypes(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> ugc_matching_type = Nan::New<v8::Object>();
  SET_TYPE(ugc_matching_type, "Items", k_EUGCMatchingUGCType_Items);
  SET_TYPE(ugc_matching_type, "ItemsMtx", k_EUGCMatchingUGCType_Items_Mtx);
  SET_TYPE(ugc_matching_type, "ItemsReadyToUse",
           k_EUGCMatchingUGCType_Items_ReadyToUse);
  SET_TYPE(ugc_matching_type, "Collections", k_EUGCMatchingUGCType_Collections);
  SET_TYPE(ugc_matching_type, "Artwork", k_EUGCMatchingUGCType_Artwork);
  SET_TYPE(ugc_matching_type, "Videos", k_EUGCMatchingUGCType_Videos);
  SET_TYPE(ugc_matching_type, "Screenshots", k_EUGCMatchingUGCType_Screenshots);
  SET_TYPE(ugc_matching_type, "AllGuides", k_EUGCMatchingUGCType_AllGuides);
  SET_TYPE(ugc_matching_type, "WebGuides", k_EUGCMatchingUGCType_WebGuides);
  SET_TYPE(ugc_matching_type, "IntegratedGuides",
           k_EUGCMatchingUGCType_IntegratedGuides);
  SET_TYPE(ugc_matching_type, "UsableInGame",
           k_EUGCMatchingUGCType_UsableInGame);
  SET_TYPE(ugc_matching_type, "ControllerBindings",
           k_EUGCMatchingUGCType_ControllerBindings);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_matching_type);
  Nan::Set(exports,
           Nan::New("UGCMatchingType").ToLocalChecked(),
           ugc_matching_type);
}

void InitUgcQueryTypes(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> ugc_query_type = Nan::New<v8::Object>();
  SET_TYPE(ugc_query_type, "RankedByVote", k_EUGCQuery_RankedByVote);
  SET_TYPE(ugc_query_type, "RankedByPublicationDate",
           k_EUGCQuery_RankedByPublicationDate);
  SET_TYPE(ugc_query_type, "AcceptedForGameRankedByAcceptanceDate",
           k_EUGCQuery_AcceptedForGameRankedByAcceptanceDate);
  SET_TYPE(ugc_query_type, "RankedByTrend", k_EUGCQuery_RankedByTrend);
  SET_TYPE(ugc_query_type, "FavoritedByFriendsRankedByPublicationDate",
           k_EUGCQuery_FavoritedByFriendsRankedByPublicationDate);
  SET_TYPE(ugc_query_type, "CreatedByFriendsRankedByPublicationDate",
           k_EUGCQuery_CreatedByFriendsRankedByPublicationDate);
  SET_TYPE(ugc_query_type, "RankedByNumTimesReported",
           k_EUGCQuery_RankedByNumTimesReported);
  SET_TYPE(ugc_query_type, "CreatedByFollowedUsersRankedByPublicationDate",
           k_EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate);
  SET_TYPE(ugc_query_type, "NotYetRated",  k_EUGCQuery_NotYetRated);
  SET_TYPE(ugc_query_type, "RankedByTotalVotesAsc",
           k_EUGCQuery_RankedByTotalVotesAsc);
  SET_TYPE(ugc_query_type, "RankedByVotesUp", k_EUGCQuery_RankedByVotesUp);
  SET_TYPE(ugc_query_type, "RankedByTextSearch",
           k_EUGCQuery_RankedByTextSearch);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_query_type);
  Nan::Set(exports,
           Nan::New("UGCQueryType").ToLocalChecked(),
           ugc_query_type);
}

void InitUserUgcList(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> ugc_list = Nan::New<v8::Object>();
  SET_TYPE(ugc_list, "Published", k_EUserUGCList_Published);
  SET_TYPE(ugc_list, "VotedOn", k_EUserUGCList_VotedOn);
  SET_TYPE(ugc_list, "VotedUp", k_EUserUGCList_VotedUp);
  SET_TYPE(ugc_list, "VotedDown", k_EUserUGCList_VotedDown);
  SET_TYPE(ugc_list, "WillVoteLater", k_EUserUGCList_WillVoteLater);
  SET_TYPE(ugc_list, "Favorited", k_EUserUGCList_Favorited);
  SET_TYPE(ugc_list, "Subscribed", k_EUserUGCList_Subscribed);
  SET_TYPE(ugc_list, "UsedOrPlayer", k_EUserUGCList_UsedOrPlayed);
  SET_TYPE(ugc_list, "Followed", k_EUserUGCList_Followed);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_list);
  Nan::Set(exports, Nan::New("UserUGCList").ToLocalChecked(), ugc_list);
}

void InitUserUgcListSortOrder(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> ugc_list_sort_order = Nan::New<v8::Object>();
  SET_TYPE(ugc_list_sort_order, "CreationOrderDesc",
           k_EUserUGCListSortOrder_CreationOrderDesc);
  SET_TYPE(ugc_list_sort_order, "CreationOrderAsc",
           k_EUserUGCListSortOrder_CreationOrderAsc);
  SET_TYPE(ugc_list_sort_order, "TitleAsc", k_EUserUGCListSortOrder_TitleAsc);
  SET_TYPE(ugc_list_sort_order, "LastUpdatedDesc",
           k_EUserUGCListSortOrder_LastUpdatedDesc);
  SET_TYPE(ugc_list_sort_order, "SubscriptionDateDesc",
           k_EUserUGCListSortOrder_SubscriptionDateDesc);
  SET_TYPE(ugc_list_sort_order, "VoteScoreDesc",
           k_EUserUGCListSortOrder_VoteScoreDesc);
  SET_TYPE(ugc_list_sort_order, "ForModeration",
           k_EUserUGCListSortOrder_ForModeration);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_list_sort_order);
  Nan::Set(exports,
           Nan::New("UserUGCListSortOrder").ToLocalChecked(),
           ugc_list_sort_order);
}

void InitUgcItemStates(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> ugc_item_state = Nan::New<v8::Object>();
  SET_TYPE(ugc_item_state, "None", k_EItemStateNone);
  SET_TYPE(ugc_item_state, "Subscribed", k_EItemStateSubscribed);
  SET_TYPE(ugc_item_state, "LegacyItem", k_EItemStateLegacyItem);
  SET_TYPE(ugc_item_state, "Installed", k_EItemStateInstalled);
  SET_TYPE(ugc_item_state, "NeedsUpdate", k_EItemStateNeedsUpdate);
  SET_TYPE(ugc_item_state, "Downloading", k_EItemStateDownloading);
  SET_TYPE(ugc_item_state, "DownloadPending", k_EItemStateDownloadPending);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_item_state);
  Nan::Set(exports, Nan::New("UGCItemState").ToLocalChecked(), ugc_item_state);
}

NAN_METHOD(FileShare) {
  Nan::HandleScope scope;

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string file_name(*(Nan::Utf8String(info[0])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::FileShareWorker(
      success_callback, error_callback, file_name));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(PublishWorkshopFile) {
  Nan::HandleScope scope;

  if (info.Length() < 6 || !info[0]->IsObject() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsString() || !info[4]->IsString() ||
      !info[5]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::MaybeLocal<v8::Object> maybe_opt = Nan::To<v8::Object>(info[0]);
  auto options = maybe_opt.ToLocalChecked();
  auto app_id = Nan::Get(options, Nan::New("app_id").ToLocalChecked());
  auto tags = Nan::Get(options, Nan::New("tags").ToLocalChecked());
  if (!app_id.ToLocalChecked()->IsInt32() ||
      !tags.ToLocalChecked()->IsArray()) {
    THROW_BAD_ARGS(
        "The object parameter must have 'app_id' and 'tags' field.");
  }
  greenworks::WorkshopFileProperties properties;

  v8::Local<v8::Array> tags_array = tags.ToLocalChecked().As<v8::Array>();
  if (tags_array->Length() > greenworks::WorkshopFileProperties::MAX_TAGS) {
    THROW_BAD_ARGS("The length of 'tags' must be less than 100.");
  }
  for (uint32_t i = 0; i < tags_array->Length(); ++i) {
    if (!Nan::Get(tags_array, i).ToLocalChecked()->IsString())
      THROW_BAD_ARGS("Bad arguments");
    Nan::Utf8String tag(Nan::Get(tags_array, (i)).ToLocalChecked());
    properties.tags_scratch.push_back(*tag);
    properties.tags[i] = properties.tags_scratch.back().c_str();
  }

  Nan::Callback* success_callback =
      new Nan::Callback(info[5].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 6 && info[6]->IsFunction())
    error_callback = new Nan::Callback(info[6].As<v8::Function>());

  properties.file_path = (*(Nan::Utf8String(info[1])));
  properties.image_path = (*(Nan::Utf8String(info[2])));
  properties.title = (*(Nan::Utf8String(info[3])));
  properties.description = (*(Nan::Utf8String(info[4])));

  Nan::AsyncQueueWorker(new greenworks::PublishWorkshopFileWorker(
      success_callback, error_callback, Nan::To<int32>(app_id.ToLocalChecked()).FromJust(),
      properties));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UpdatePublishedWorkshopFile) {
  Nan::HandleScope scope;

  if (info.Length() < 7 || !info[0]->IsObject() || !info[1]->IsString() ||
      !info[2]->IsString() || !info[3]->IsString() || !info[4]->IsString() ||
      !info[5]->IsString() || !info[6]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::MaybeLocal<v8::Object> maybe_opt = Nan::To<v8::Object>(info[0]);
  auto options = maybe_opt.ToLocalChecked();
  auto tags = Nan::Get(options, (Nan::New("tags").ToLocalChecked()));
  if (!tags.ToLocalChecked()->IsArray()) {
    THROW_BAD_ARGS("The object parameter must have 'tags' field.");
  }
  greenworks::WorkshopFileProperties properties;

  v8::Local<v8::Array> tags_array = tags.ToLocalChecked().As<v8::Array>();
  if (tags_array->Length() > greenworks::WorkshopFileProperties::MAX_TAGS) {
    THROW_BAD_ARGS("The length of 'tags' must be less than 100.");
  }
  for (uint32_t i = 0; i < tags_array->Length(); ++i) {
    if (!Nan::Get(tags_array, i).ToLocalChecked()->IsString())
      THROW_BAD_ARGS("Bad arguments");
    Nan::Utf8String tag(Nan::Get(tags_array, (i)).ToLocalChecked());
    properties.tags_scratch.push_back(*tag);
    properties.tags[i] = properties.tags_scratch.back().c_str();
  }
  Nan::Callback* success_callback =
      new Nan::Callback(info[6].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 7 && info[7]->IsFunction())
    error_callback = new Nan::Callback(info[7].As<v8::Function>());

  PublishedFileId_t published_file_id = utils::strToUint64(
      *(Nan::Utf8String(info[1])));
  properties.file_path = (*(Nan::Utf8String(info[2])));
  properties.image_path = (*(Nan::Utf8String(info[3])));
  properties.title = (*(Nan::Utf8String(info[4])));
  properties.description = (*(Nan::Utf8String(info[5])));

  Nan::AsyncQueueWorker(new greenworks::UpdatePublishedWorkshopFileWorker(
      success_callback, error_callback, published_file_id, properties));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCGetItems) {
  Nan::HandleScope scope;
  if (info.Length() < 4 || !info[0]->IsObject() || !info[1]->IsInt32() ||
      !info[2]->IsInt32() || !info[3]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::MaybeLocal<v8::Object> maybe_opt = Nan::To<v8::Object>(info[0]);
  auto options = maybe_opt.ToLocalChecked();
  auto app_id = Nan::Get(options, (Nan::New("app_id").ToLocalChecked()));
  auto page_num = Nan::Get(options, (Nan::New("page_num").ToLocalChecked()));
  if (!app_id.ToLocalChecked()->IsInt32() ||
      !page_num.ToLocalChecked()->IsInt32()) {
    THROW_BAD_ARGS(
        "The object parameter must have 'app_id' and 'page_num' fields.");
  }

  auto ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      Nan::To<int32>(info[1]).FromJust());
  auto ugc_query_type = static_cast<EUGCQuery>(Nan::To<int32>(info[2]).FromJust());

  Nan::Callback* success_callback =
      new Nan::Callback(info[3].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 4 && info[4]->IsFunction())
    error_callback = new Nan::Callback(info[4].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::QueryAllUGCWorker(
      success_callback, error_callback, ugc_matching_type, ugc_query_type,
      Nan::To<int32>(app_id.ToLocalChecked()).FromJust(),
      Nan::To<int32>(page_num.ToLocalChecked()).FromJust()));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCGetUserItems) {
  Nan::HandleScope scope;
  if (info.Length() < 5 || !info[0]->IsObject() || !info[1]->IsInt32() ||
      !info[2]->IsInt32() || !info[3]->IsInt32() || !info[4]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::MaybeLocal<v8::Object> maybe_opt = Nan::To<v8::Object>(info[0]);
  auto options = maybe_opt.ToLocalChecked();
  auto app_id = Nan::Get(options, (Nan::New("app_id").ToLocalChecked()));
  auto page_num = Nan::Get(options, (Nan::New("page_num").ToLocalChecked()));
  if (!app_id.ToLocalChecked()->IsInt32() ||
      !page_num.ToLocalChecked()->IsInt32()) {
    THROW_BAD_ARGS(
        "The object parameter must have 'app_id' and 'page_num' fields.");
  }

  auto ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      Nan::To<int32>(info[1]).FromJust());
  auto ugc_list_order = static_cast<EUserUGCListSortOrder>(
      Nan::To<int32>(info[2]).FromJust());
  auto ugc_list = static_cast<EUserUGCList>(Nan::To<int32>(info[3]).FromJust());

  Nan::Callback* success_callback =
      new Nan::Callback(info[4].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 5 && info[5]->IsFunction())
    error_callback = new Nan::Callback(info[4].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::QueryUserUGCWorker(
      success_callback, error_callback, ugc_matching_type, ugc_list,
      ugc_list_order, Nan::To<int32>(app_id.ToLocalChecked()).FromJust(),
          Nan::To<int32>(page_num.ToLocalChecked()).FromJust()));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCDownloadItem) {
  Nan::HandleScope scope;
  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsString() ||
      !info[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  UGCHandle_t download_file_handle = utils::strToUint64(
      *(Nan::Utf8String(info[0])));
  std::string download_dir = *(Nan::Utf8String(info[1]));

  Nan::Callback* success_callback =
      new Nan::Callback(info[2].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 3 && info[3]->IsFunction())
    error_callback = new Nan::Callback(info[3].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::DownloadItemWorker(
      success_callback, error_callback, download_file_handle, download_dir));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCSynchronizeItems) {
  Nan::HandleScope scope;
  if (info.Length() < 3 || !info[0]->IsObject() || !info[1]->IsString() ||
      !info[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  Nan::MaybeLocal<v8::Object> maybe_opt = Nan::To<v8::Object>(info[0]);
  auto options = maybe_opt.ToLocalChecked();
  auto app_id = Nan::Get(options, (Nan::New("app_id").ToLocalChecked()));
  auto page_num = Nan::Get(options, (Nan::New("page_num").ToLocalChecked()));
  if (!app_id.ToLocalChecked()->IsInt32() ||
      !page_num.ToLocalChecked()->IsInt32()) {
    THROW_BAD_ARGS(
        "The object parameter must have 'app_id' and 'page_num' fields.");
  }
  std::string download_dir = *(Nan::Utf8String(info[1]));

  Nan::Callback* success_callback =
      new Nan::Callback(info[2].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 3 && info[3]->IsFunction())
    error_callback = new Nan::Callback(info[3].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::SynchronizeItemsWorker(
      success_callback, error_callback, download_dir,
      Nan::To<int32>(app_id.ToLocalChecked()).FromJust(),
      Nan::To<int32>(page_num.ToLocalChecked()).FromJust()));
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
    std::string item_id = *(Nan::Utf8String(info[0]));
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
      *(Nan::Utf8String(info[0])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = nullptr;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::UnsubscribePublishedFileWorker(
      success_callback, error_callback, unsubscribed_file_id));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCGetItemState) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments; expected: publishedFileId [string]");
  }
  PublishedFileId_t file_id = utils::strToUint64(*(Nan::Utf8String(info[0])));
  info.GetReturnValue().Set(Nan::New(SteamUGC()->GetItemState(file_id)));
}

NAN_METHOD(UGCGetItemInstallInfo) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments; expected: publishedFileId [string]");
  }
  
  PublishedFileId_t file_id = utils::strToUint64(*(Nan::Utf8String(info[0])));
  uint64 size_on_disk;
  const int folder_path_size = 260;  // MAX_PATH on 32bit Windows according to MSDN documentation
  char folder_path[folder_path_size];
  uint32 timestamp;
  bool success = SteamUGC()->GetItemInstallInfo(file_id, &size_on_disk, folder_path, folder_path_size, &timestamp);
  
  if (!success) {
    info.GetReturnValue().Set(Nan::Undefined());
  }
  else {
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("sizeOnDisk").ToLocalChecked(), Nan::New(utils::uint64ToString(size_on_disk)).ToLocalChecked());
    Nan::Set(result, Nan::New("folder").ToLocalChecked(), Nan::New(folder_path).ToLocalChecked());
    Nan::Set(result, Nan::New("timestamp").ToLocalChecked(), Nan::New(timestamp));
    info.GetReturnValue().Set(result);
  }
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  InitUgcMatchingTypes(target);
  InitUgcQueryTypes(target);
  InitUserUgcListSortOrder(target);
  InitUserUgcList(target);
  InitUgcItemStates(target);

  SET_FUNCTION("fileShare", FileShare);
  SET_FUNCTION("_publishWorkshopFile", PublishWorkshopFile);
  SET_FUNCTION("_updatePublishedWorkshopFile", UpdatePublishedWorkshopFile);
  SET_FUNCTION("_ugcGetItems", UGCGetItems);
  SET_FUNCTION("_ugcGetUserItems", UGCGetUserItems);
  SET_FUNCTION("ugcDownloadItem", UGCDownloadItem);
  SET_FUNCTION("_ugcSynchronizeItems", UGCSynchronizeItems);
  SET_FUNCTION("ugcShowOverlay", UGCShowOverlay);
  SET_FUNCTION("ugcUnsubscribe", UGCUnsubscribe);
  SET_FUNCTION("ugcGetItemState", UGCGetItemState);
  SET_FUNCTION("ugcGetItemInstallInfo", UGCGetItemInstallInfo);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
