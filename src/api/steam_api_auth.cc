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

NAN_METHOD(GetAuthSessionTicket) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsFunction()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  Nan::Callback* success_callback =
      new Nan::Callback(info[0].As<v8::Function>());
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
  Nan::Callback* success_callback =
      new Nan::Callback(info[1].As<v8::Function>());
  Nan::Callback* error_callback = NULL;
  if (info.Length() > 2 && info[2]->IsFunction())
    error_callback = new Nan::Callback(info[2].As<v8::Function>());
  Nan::AsyncQueueWorker(new greenworks::RequestEncryptedAppTicketWorker(
    user_data, success_callback, error_callback));
  info.GetReturnValue().Set(Nan::Undefined());
}

void RegisterAPIs(v8::Handle<v8::Object> target) {
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
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
