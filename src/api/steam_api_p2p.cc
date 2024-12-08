#include <memory>

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "greenworks_utils.h"
#include "steam_api_registry.h"
#include "steam_id.h"

namespace greenworks {
namespace api {
namespace {

NAN_METHOD(AcceptP2PSessionWithUser) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
      SteamNetworking()->AcceptP2PSessionWithUser(steam_id));
}

NAN_METHOD(IsP2PPacketAvailable) {
  Nan::HandleScope scope;

  uint32 cubMsgSize = 0;
  int nChannel = 0;

  if (info.Length() >= 1) {
    if (!info[0]->IsInt32()) {
      THROW_BAD_ARGS("Bad arguments");
    }
    nChannel = info[0]->NumberValue(Nan::GetCurrentContext()).FromJust();
  }

  bool available =
      SteamNetworking()->IsP2PPacketAvailable(&cubMsgSize, nChannel);

  if (available) {
    info.GetReturnValue().Set(Nan::New(cubMsgSize));
  } else {
    info.GetReturnValue().Set(Nan::New(0));
  }
}

NAN_METHOD(SendP2PPacket) {
  Nan::HandleScope scope;
  if (info.Length() < 3) {
    THROW_BAD_ARGS("Bad arguments");
  }

  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("steamID argument must be a string");
    return;
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID targetUserID(utils::strToUint64(steam_id_str));
  if (!targetUserID.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }

  if (!info[1]->IsInt32()) {
    Nan::ThrowTypeError("p2pType argument must be a number");
    return;
  }
  int eP2PSendType = info[1]->NumberValue(Nan::GetCurrentContext()).FromJust();

  if (!info[2]->IsObject() || !node::Buffer::HasInstance(info[2])) {
    Nan::ThrowTypeError("data argument must be a buffer");
    return;
  }

  int nChannel = 0;
  if (info.Length() > 1) {
    if (!info[3]->IsInt32()) {
      THROW_BAD_ARGS("Bad arguments: nChannel must be a number");
    }
    nChannel = info[3]->NumberValue(Nan::GetCurrentContext()).FromJust();
  }

  v8::Local<v8::Object> bufferObj =
      info[2]->ToObject(Nan::GetCurrentContext()).ToLocalChecked();
  char *bufferData = node::Buffer::Data(bufferObj);
  size_t bufferLength = node::Buffer::Length(bufferObj);

  bool success = SteamNetworking()->SendP2PPacket(
      targetUserID, bufferData, static_cast<uint32>(bufferLength),
      static_cast<EP2PSend>(eP2PSendType), nChannel);

  info.GetReturnValue().Set(Nan::New(success));
}

NAN_METHOD(ReadP2PPacket) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("First argument must be a number");
    return;
  }
  uint32 cubDest = info[0]->Uint32Value(Nan::GetCurrentContext()).FromJust();

  int nChannel = 0;
  if (info.Length() > 1) {
    if (!info[1]->IsInt32()) {
      THROW_BAD_ARGS("Bad arguments: nChannel must be a number");
    }
    nChannel = info[1]->NumberValue(Nan::GetCurrentContext()).FromJust();
  }

  char *pubDest = new char[cubDest];
  uint32 cubMsgSize = 0;
  CSteamID steamIDRemote;

  bool success = SteamNetworking()->ReadP2PPacket(pubDest, cubDest, &cubMsgSize,
                                                  &steamIDRemote, nChannel);

  if (success) {
    v8::Local<v8::Object> result = Nan::New<v8::Object>();
    Nan::Set(result, Nan::New("data").ToLocalChecked(),
             Nan::CopyBuffer(pubDest, cubMsgSize).ToLocalChecked());
    Nan::Set(result, Nan::New("steamIDRemote").ToLocalChecked(),
             Nan::New(std::to_string(steamIDRemote.ConvertToUint64()))
                 .ToLocalChecked());
    info.GetReturnValue().Set(result);
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }

  delete[] pubDest;
}

NAN_METHOD(CloseP2PSessionWithUser) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
      SteamNetworking()->CloseP2PSessionWithUser(steam_id));
}

NAN_METHOD(CloseP2PChannelWithUser) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }

  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }

  int nChannel = info[1]->NumberValue(Nan::GetCurrentContext()).FromJust();

  info.GetReturnValue().Set(
      SteamNetworking()->CloseP2PChannelWithUser(steam_id, nChannel));
}

NAN_METHOD(GetP2PSessionState) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steamIDRemote(utils::strToUint64(steam_id_str));
  if (!steamIDRemote.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }

  P2PSessionState_t pConnectionState;
  bool result =
      SteamNetworking()->GetP2PSessionState(steamIDRemote, &pConnectionState);

  v8::Local<v8::Object> sessionStateObj = Nan::New<v8::Object>();
  Nan::Set(sessionStateObj, Nan::New("m_bConnectionActive").ToLocalChecked(),
           Nan::New(pConnectionState.m_bConnectionActive));
  Nan::Set(sessionStateObj, Nan::New("m_bConnecting").ToLocalChecked(),
           Nan::New(pConnectionState.m_bConnecting));
  Nan::Set(sessionStateObj, Nan::New("m_eP2PSessionError").ToLocalChecked(),
           Nan::New(pConnectionState.m_eP2PSessionError));
  Nan::Set(sessionStateObj, Nan::New("m_bUsingRelay").ToLocalChecked(),
           Nan::New(pConnectionState.m_bUsingRelay));
  Nan::Set(sessionStateObj, Nan::New("m_nBytesQueuedForSend").ToLocalChecked(),
           Nan::New(pConnectionState.m_nBytesQueuedForSend));
  Nan::Set(sessionStateObj,
           Nan::New("m_nPacketsQueuedForSend").ToLocalChecked(),
           Nan::New(pConnectionState.m_nPacketsQueuedForSend));
  Nan::Set(sessionStateObj, Nan::New("m_nRemoteIP").ToLocalChecked(),
           Nan::New(pConnectionState.m_nRemoteIP));
  Nan::Set(sessionStateObj, Nan::New("m_nRemotePort").ToLocalChecked(),
           Nan::New(pConnectionState.m_nRemotePort));

  v8::Local<v8::Object> resultObj = Nan::New<v8::Object>();
  Nan::Set(resultObj, Nan::New("result").ToLocalChecked(), Nan::New(result));
  Nan::Set(resultObj, Nan::New("connectionState").ToLocalChecked(),
           sessionStateObj);

  info.GetReturnValue().Set(resultObj);
}

NAN_METHOD(BIsBehindNAT) {
  bool isBehindNAT = SteamUser()->BIsBehindNAT();
  info.GetReturnValue().Set(Nan::New(isBehindNAT));
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  SET_FUNCTION("acceptP2PSessionWithUser", AcceptP2PSessionWithUser);
  SET_FUNCTION("isP2PPacketAvailable", IsP2PPacketAvailable);
  SET_FUNCTION("sendP2PPacket", SendP2PPacket);
  SET_FUNCTION("readP2PPacket", ReadP2PPacket);

  SET_FUNCTION("closeP2PSessionWithUser", CloseP2PSessionWithUser);
  SET_FUNCTION("closeP2PChannelWithUser", CloseP2PChannelWithUser);
  SET_FUNCTION("getP2PSessionState", GetP2PSessionState);
  SET_FUNCTION("isBehindNAT", BIsBehindNAT);
}

SteamAPIRegistry::Add X(RegisterAPIs);

} // namespace
} // namespace api
} // namespace greenworks
