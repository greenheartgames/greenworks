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

void InitUgcMatchingTypes(v8::Handle<v8::Object> exports) {
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

void InitUgcQueryTypes(v8::Handle<v8::Object> exports) {
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

void InitUserUgcList(v8::Handle<v8::Object> exports) {
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

void InitUserUgcListSortOrder(v8::Handle<v8::Object> exports) {
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

NAN_METHOD(FileShare) {
  Nan::HandleScope scope;

  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string file_name(*(v8::String::Utf8Value(info[0])));
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
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

  Nan::Callback* success_callback =
      new Nan::Callback(info[4].As<v8::Function>());
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

  Nan::Callback* success_callback =
      new Nan::Callback(info[5].As<v8::Function>());
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
  if (info.Length() < 3 || !info[0]->IsInt32() || !info[1]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  EUGCMatchingUGCType ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      info[0]->Int32Value());
  EUGCQuery ugc_query_type = static_cast<EUGCQuery>(info[1]->Int32Value());
  
  Nan::Callback* success_callback = NULL;
  uint32 unPage = 1;

  if(info[2]->IsFunction()) {

    success_callback =
      new Nan::Callback(info[2].As<v8::Function>());

  } else if(info[2]->IsUint32()) {

    unPage = info[2]->Uint32Value();
    if(unPage < 1)
      THROW_BAD_ARGS("unPage must be atleast 1!");

    if(info.Length() < 4 || !info[3]->IsFunction())
      THROW_BAD_ARGS("No success_callback");

    success_callback =
      new Nan::Callback(info[3].As<v8::Function>());

  } else {
    THROW_BAD_ARGS("Bad arguments");
  }

  Nan::Callback* error_callback = NULL;

  if (info.Length() > 3 && info[4]->IsFunction())
    error_callback = new Nan::Callback(info[4].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::QueryAllUGCWorker(
      success_callback, error_callback, ugc_matching_type, ugc_query_type,
      unPage));
  info.GetReturnValue().Set(Nan::Undefined());
}

NAN_METHOD(UGCGetUserItems) {
  Nan::HandleScope scope;
  if (info.Length() < 4 || !info[0]->IsInt32() || !info[1]->IsInt32() ||
      !info[2]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  EUGCMatchingUGCType ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      info[0]->Int32Value());
  EUserUGCListSortOrder ugc_list_order = static_cast<EUserUGCListSortOrder>(
      info[1]->Int32Value());
  EUserUGCList ugc_list = static_cast<EUserUGCList>(info[2]->Int32Value());

  Nan::Callback* success_callback = NULL;
  uint32 unPage = 1;

  if(info[3]->IsFunction()) {
    success_callback =
      new Nan::Callback(info[3].As<v8::Function>());

  } else if(info[3]->IsUint32()) {

    unPage = info[3]->Uint32Value();
    if(unPage < 1)
      THROW_BAD_ARGS("unPage must be atleast 1!");

    if(info.Length() < 5 || !info[4]->IsFunction())
      THROW_BAD_ARGS("No success_callback");

    success_callback =
      new Nan::Callback(info[4].As<v8::Function>());

  } else {
    THROW_BAD_ARGS("Bad arguments");
  }

  Nan::Callback* error_callback = NULL;

  if (info.Length() > 4 && info[5]->IsFunction())
    error_callback = new Nan::Callback(info[5].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::QueryUserUGCWorker(
      success_callback, error_callback, ugc_matching_type, ugc_list,
      ugc_list_order, unPage));
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

  Nan::Callback* success_callback =
      new Nan::Callback(info[2].As<v8::Function>());
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

  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
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
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;

  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());

  Nan::AsyncQueueWorker(new greenworks::UnsubscribePublishedFileWorker(
      success_callback, error_callback, unsubscribed_file_id));
  info.GetReturnValue().Set(Nan::Undefined());
}

void RegisterAPIs(v8::Handle<v8::Object> exports) {
  InitUgcMatchingTypes(exports);
  InitUgcQueryTypes(exports);
  InitUserUgcListSortOrder(exports);
  InitUserUgcList(exports);

  Nan::Set(exports,
           Nan::New("fileShare").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(FileShare)->GetFunction());
  Nan::Set(exports,
           Nan::New("publishWorkshopFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(PublishWorkshopFile)->GetFunction());
  Nan::Set(exports,
           Nan::New("updatePublishedWorkshopFile").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(
               UpdatePublishedWorkshopFile)->GetFunction());
  Nan::Set(exports,
           Nan::New("ugcGetItems").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCGetItems)->GetFunction());
  Nan::Set(exports,
           Nan::New("ugcGetUserItems").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCGetUserItems)->GetFunction());
  Nan::Set(exports,
           Nan::New("ugcDownloadItem").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCDownloadItem)->GetFunction());
  Nan::Set(exports,
           Nan::New("ugcSynchronizeItems").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCSynchronizeItems)->GetFunction());
  Nan::Set(exports,
           Nan::New("ugcShowOverlay").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCShowOverlay)->GetFunction());
  Nan::Set(exports,
           Nan::New("ugcUnsubscribe").ToLocalChecked(),
           Nan::New<v8::FunctionTemplate>(UGCUnsubscribe)->GetFunction());
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
