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
    SteamNetworking()->AcceptP2PSessionWithUser(steam_id)
  );
}


NAN_METHOD(IsP2PPacketAvailable) {
  Nan::HandleScope scope;
  
  uint32 cubMsgSize = 0;
  int nChannel = 0;
  
  if (info.Length() >= 1) {
    if(!info[0]->IsInt32()){
      THROW_BAD_ARGS("Bad arguments");
    }
    nChannel=info[0]->NumberValue(Nan::GetCurrentContext()).FromJust();
  }


  bool available = SteamNetworking()->IsP2PPacketAvailable(&cubMsgSize, nChannel);

    if (available) {
        info.GetReturnValue().Set(Nan::New(cubMsgSize));
    } else {
        info.GetReturnValue().Set(Nan::New(0));
    }
  /*
  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("available").ToLocalChecked(), Nan::New(available));
  Nan::Set(result, Nan::New("msgSize").ToLocalChecked(), Nan::New(cubMsgSize));

  info.GetReturnValue().Set(result);

  info.GetReturnValue().Set(
    SteamNetworking()->IsP2PPacketAvailable(&cubMsgSize,nChannel)
  );
  */
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
  //v8::String::Utf8Value utf8Str(Nan::GetCurrentContext()->GetIsolate(), info[0]);
  //const char* targetUserIdStr = *utf8Str;
  //CSteamID targetUserID((uint64)std::stoull(targetUserIdStr));
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
  v8::Local<v8::Object> bufferObj = info[2]->ToObject(Nan::GetCurrentContext()).ToLocalChecked();
  char* bufferData = node::Buffer::Data(bufferObj);
  size_t bufferLength = node::Buffer::Length(bufferObj);

  bool success = SteamNetworking()->SendP2PPacket(targetUserID, bufferData, static_cast<uint32>(bufferLength), static_cast<EP2PSend>(eP2PSendType));

  info.GetReturnValue().Set(Nan::New(success));
}





NAN_METHOD(ReadP2PPacket) {
    if (info.Length() <1) {
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
      if(!info[1]->IsInt32()){
        THROW_BAD_ARGS("Bad arguments: nChannel must be a number");
      }
      nChannel=info[1]->NumberValue(Nan::GetCurrentContext()).FromJust();
    }
    
    char* pubDest = new char[cubDest];
    uint32 cubMsgSize = 0;
    CSteamID steamIDRemote;

    bool success = SteamNetworking()->ReadP2PPacket(pubDest, cubDest, &cubMsgSize, &steamIDRemote, nChannel);

    if (success) {
        v8::Local<v8::Object> result = Nan::New<v8::Object>();
        Nan::Set(result, Nan::New("data").ToLocalChecked(), Nan::CopyBuffer(pubDest, cubMsgSize).ToLocalChecked());
        Nan::Set(result, Nan::New("steamIDRemote").ToLocalChecked(), Nan::New(std::to_string(steamIDRemote.ConvertToUint64())).ToLocalChecked());
        //Nan::Set(result, Nan::New("steamIDRemote").ToLocalChecked(), Nan::New<v8::BigInt>(Nan::GetCurrentContext()->GetIsolate(), steamIDRemote.ConvertToUint64()));
        info.GetReturnValue().Set(result);
    } else {
        info.GetReturnValue().Set(Nan::Null());
    }

    delete[] pubDest;
}



void RegisterAPIs(v8::Local<v8::Object> target) {
  SET_FUNCTION("acceptP2PSessionWithUser", AcceptP2PSessionWithUser);
  SET_FUNCTION("isP2PPacketAvailable", IsP2PPacketAvailable);
  SET_FUNCTION("sendP2PPacket", SendP2PPacket);
  SET_FUNCTION("readP2PPacket", ReadP2PPacket);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks