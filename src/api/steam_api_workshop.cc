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
  if (info.Length() < 3 || !info[0]->IsInt32() || !info[1]->IsInt32() ||
      !info[2]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  EUGCMatchingUGCType ugc_matching_type = static_cast<EUGCMatchingUGCType>(
      info[0]->Int32Value());
  EUGCQuery ugc_query_type = static_cast<EUGCQuery>(info[1]->Int32Value());

  Nan::Callback* success_callback =
      new Nan::Callback(info[2].As<v8::Function>());
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

  Nan::Callback* success_callback =
      new Nan::Callback(info[3].As<v8::Function>());
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
  utils::InitUgcMatchingTypes(exports);
  utils::InitUgcQueryTypes(exports);
  utils::InitUserUgcListSortOrder(exports);
  utils::InitUserUgcList(exports);

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
