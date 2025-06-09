#include "steam/steam_api.h"
#include "nan.h"
#include "v8.h"
#include "greenworks_utils.h"
#include "steam_api_registry.h"
#include "steam/isteamdualsense.h" // Required for SetDualSenseTriggerEffect

namespace greenworks {
namespace api {
namespace input {

NAN_METHOD(Init) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    info.GetReturnValue().Set(Nan::New(false));
    return;
  }
  info.GetReturnValue().Set(Nan::New(SteamInput()->Init()));
}

NAN_METHOD(Shutdown) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    info.GetReturnValue().Set(Nan::New(false));
    return;
  }
  info.GetReturnValue().Set(Nan::New(SteamInput()->Shutdown()));
}

NAN_METHOD(GetActionSetHandle) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Expected 1 string argument: actionSetName");
    return;
  }
  Nan::Utf8String actionSetName(info[0]);
  InputActionSetHandle_t handle = SteamInput()->GetActionSetHandle(*actionSetName);
  info.GetReturnValue().Set(Nan::New<v8::Number>(static_cast<double>(handle)));
}

NAN_METHOD(ActivateActionSet) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    THROW_BAD_ARGS("Expected 2 number arguments: inputHandle, actionSetHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputActionSetHandle_t actionSetHandle = static_cast<InputActionSetHandle_t>(Nan::To<int64_t>(info[1]).FromJust());
  SteamInput()->ActivateActionSet(inputHandle, actionSetHandle);
}

NAN_METHOD(GetCurrentActionSet) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: inputHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputActionSetHandle_t handle = SteamInput()->GetCurrentActionSet(inputHandle);
  info.GetReturnValue().Set(Nan::New<v8::Number>(static_cast<double>(handle)));
}

NAN_METHOD(ActivateActionSetLayer) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    THROW_BAD_ARGS("Expected 2 number arguments: inputHandle, actionSetLayerHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputActionSetHandle_t actionSetLayerHandle = static_cast<InputActionSetHandle_t>(Nan::To<int64_t>(info[1]).FromJust());
  SteamInput()->ActivateActionSetLayer(inputHandle, actionSetLayerHandle);
}

NAN_METHOD(DeactivateActionSetLayer) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    THROW_BAD_ARGS("Expected 2 number arguments: inputHandle, actionSetLayerHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputActionSetHandle_t actionSetLayerHandle = static_cast<InputActionSetHandle_t>(Nan::To<int64_t>(info[1]).FromJust());
  SteamInput()->DeactivateActionSetLayer(inputHandle, actionSetLayerHandle);
}

NAN_METHOD(DeactivateAllActionSetLayers) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: inputHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  SteamInput()->DeactivateAllActionSetLayers(inputHandle);
}

NAN_METHOD(GetActiveActionSetLayers) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: inputHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputActionSetHandle_t handlesOut[STEAM_INPUT_MAX_ACTIVE_LAYERS];
  int numLayers = SteamInput()->GetActiveActionSetLayers(inputHandle, handlesOut);

  v8::Local<v8::Array> jsArray = Nan::New<v8::Array>(numLayers);
  for (int i = 0; i < numLayers; ++i) {
    Nan::Set(jsArray, i, Nan::New<v8::Number>(static_cast<double>(handlesOut[i])));
  }
  info.GetReturnValue().Set(jsArray);
}

NAN_METHOD(GetAnalogActionHandle) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Expected 1 string argument: actionName");
    return;
  }
  Nan::Utf8String actionName(info[0]);
  InputAnalogActionHandle_t handle = SteamInput()->GetAnalogActionHandle(*actionName);
  info.GetReturnValue().Set(Nan::New<v8::Number>(static_cast<double>(handle)));
}

NAN_METHOD(GetDigitalActionHandle) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Expected 1 string argument: actionName");
    return;
  }
  Nan::Utf8String actionName(info[0]);
  InputDigitalActionHandle_t handle = SteamInput()->GetDigitalActionHandle(*actionName);
  info.GetReturnValue().Set(Nan::New<v8::Number>(static_cast<double>(handle)));
}

NAN_METHOD(GetAnalogActionData) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    THROW_BAD_ARGS("Expected 2 number arguments: inputHandle, analogActionHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputAnalogActionHandle_t analogActionHandle = static_cast<InputAnalogActionHandle_t>(Nan::To<int64_t>(info[1]).FromJust());

  InputAnalogActionData_t data = SteamInput()->GetAnalogActionData(inputHandle, analogActionHandle);

  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("eMode").ToLocalChecked(), Nan::New(data.eMode));
  Nan::Set(result, Nan::New("x").ToLocalChecked(), Nan::New(data.x));
  Nan::Set(result, Nan::New("y").ToLocalChecked(), Nan::New(data.y));
  Nan::Set(result, Nan::New("bActive").ToLocalChecked(), Nan::New(data.bActive));

  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetDigitalActionData) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    THROW_BAD_ARGS("Expected 2 number arguments: inputHandle, digitalActionHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputDigitalActionHandle_t digitalActionHandle = static_cast<InputDigitalActionHandle_t>(Nan::To<int64_t>(info[1]).FromJust());

  InputDigitalActionData_t data = SteamInput()->GetDigitalActionData(inputHandle, digitalActionHandle);

  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("bState").ToLocalChecked(), Nan::New(data.bState));
  Nan::Set(result, Nan::New("bActive").ToLocalChecked(), Nan::New(data.bActive));

  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetMotionData) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: inputHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());

  InputMotionData_t data = SteamInput()->GetMotionData(inputHandle);

  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("rotQuatX").ToLocalChecked(), Nan::New(data.rotQuatX));
  Nan::Set(result, Nan::New("rotQuatY").ToLocalChecked(), Nan::New(data.rotQuatY));
  Nan::Set(result, Nan::New("rotQuatZ").ToLocalChecked(), Nan::New(data.rotQuatZ));
  Nan::Set(result, Nan::New("rotQuatW").ToLocalChecked(), Nan::New(data.rotQuatW));
  Nan::Set(result, Nan::New("posAccelX").ToLocalChecked(), Nan::New(data.posAccelX));
  Nan::Set(result, Nan::New("posAccelY").ToLocalChecked(), Nan::New(data.posAccelY));
  Nan::Set(result, Nan::New("posAccelZ").ToLocalChecked(), Nan::New(data.posAccelZ));
  Nan::Set(result, Nan::New("rotVelX").ToLocalChecked(), Nan::New(data.rotVelX));
  Nan::Set(result, Nan::New("rotVelY").ToLocalChecked(), Nan::New(data.rotVelY));
  Nan::Set(result, Nan::New("rotVelZ").ToLocalChecked(), Nan::New(data.rotVelZ));

  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetConnectedControllers) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  InputHandle_t handlesOut[STEAM_INPUT_MAX_COUNT];
  int count = SteamInput()->GetConnectedControllers(handlesOut);

  v8::Local<v8::Array> result = Nan::New<v8::Array>(count);
  for (int i = 0; i < count; ++i) {
    Nan::Set(result, i, Nan::New<v8::Number>(static_cast<double>(handlesOut[i])));
  }
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetControllerForGamepadIndex) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Expected 1 integer argument: nIndex");
    return;
  }
  int nIndex = Nan::To<int32_t>(info[0]).FromJust();
  InputHandle_t handle = SteamInput()->GetControllerForGamepadIndex(nIndex);
  info.GetReturnValue().Set(Nan::New<v8::Number>(static_cast<double>(handle)));
}

NAN_METHOD(GetGamepadIndexForController) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: ulControllerHandle");
    return;
  }
  InputHandle_t ulControllerHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  int index = SteamInput()->GetGamepadIndexForController(ulControllerHandle);
  info.GetReturnValue().Set(Nan::New(index));
}

NAN_METHOD(GetInputTypeForHandle) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: inputHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  ESteamInputType inputType = SteamInput()->GetInputTypeForHandle(inputHandle);
  info.GetReturnValue().Set(Nan::New(static_cast<int>(inputType)));
}

NAN_METHOD(GetAnalogActionOrigins) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 3 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber()) {
    THROW_BAD_ARGS("Expected 3 number arguments: inputHandle, actionSetHandle, analogActionHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputActionSetHandle_t actionSetHandle = static_cast<InputActionSetHandle_t>(Nan::To<int64_t>(info[1]).FromJust());
  InputAnalogActionHandle_t analogActionHandle = static_cast<InputAnalogActionHandle_t>(Nan::To<int64_t>(info[2]).FromJust());

  EInputActionOrigin originsOut[STEAM_INPUT_MAX_ORIGINS];
  int count = SteamInput()->GetAnalogActionOrigins(inputHandle, actionSetHandle, analogActionHandle, originsOut);

  v8::Local<v8::Array> result = Nan::New<v8::Array>(count);
  for (int i = 0; i < count; ++i) {
    Nan::Set(result, i, Nan::New(static_cast<int>(originsOut[i])));
  }
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetDigitalActionOrigins) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 3 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsNumber()) {
    THROW_BAD_ARGS("Expected 3 number arguments: inputHandle, actionSetHandle, digitalActionHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputActionSetHandle_t actionSetHandle = static_cast<InputActionSetHandle_t>(Nan::To<int64_t>(info[1]).FromJust());
  InputDigitalActionHandle_t digitalActionHandle = static_cast<InputDigitalActionHandle_t>(Nan::To<int64_t>(info[2]).FromJust());

  EInputActionOrigin originsOut[STEAM_INPUT_MAX_ORIGINS];
  int count = SteamInput()->GetDigitalActionOrigins(inputHandle, actionSetHandle, digitalActionHandle, originsOut);

  v8::Local<v8::Array> result = Nan::New<v8::Array>(count);
  for (int i = 0; i < count; ++i) {
    Nan::Set(result, i, Nan::New(static_cast<int>(originsOut[i])));
  }
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetStringForActionOrigin) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: eOrigin");
    return;
  }
  EInputActionOrigin eOrigin = static_cast<EInputActionOrigin>(Nan::To<int32_t>(info[0]).FromJust());
  const char* originString = SteamInput()->GetStringForActionOrigin(eOrigin);
  if (originString) {
    info.GetReturnValue().Set(Nan::New(originString).ToLocalChecked());
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
}

NAN_METHOD(GetGlyphForActionOrigin) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: eOrigin");
    return;
  }
  EInputActionOrigin eOrigin = static_cast<EInputActionOrigin>(Nan::To<int32_t>(info[0]).FromJust());
  const char* glyphPath = SteamInput()->GetGlyphForActionOrigin(eOrigin);
  if (glyphPath) {
    info.GetReturnValue().Set(Nan::New(glyphPath).ToLocalChecked());
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
}

NAN_METHOD(GetActionOriginFromXboxOrigin) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    THROW_BAD_ARGS("Expected 2 number arguments: inputHandle, eOrigin");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  EXboxOrigin eOrigin = static_cast<EXboxOrigin>(Nan::To<int32_t>(info[1]).FromJust());
  EInputActionOrigin actionOrigin = SteamInput()->GetActionOriginFromXboxOrigin(inputHandle, eOrigin);
  info.GetReturnValue().Set(Nan::New(static_cast<int>(actionOrigin)));
}

NAN_METHOD(TranslateActionOrigin) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    THROW_BAD_ARGS("Expected 2 number arguments: eDestinationInputType, eSourceOrigin");
    return;
  }
  ESteamInputType eDestinationInputType = static_cast<ESteamInputType>(Nan::To<int32_t>(info[0]).FromJust());
  EInputActionOrigin eSourceOrigin = static_cast<EInputActionOrigin>(Nan::To<int32_t>(info[1]).FromJust());
  EInputActionOrigin translatedOrigin = SteamInput()->TranslateActionOrigin(eDestinationInputType, eSourceOrigin);
  info.GetReturnValue().Set(Nan::New(static_cast<int>(translatedOrigin)));
}

NAN_METHOD(SetLEDColor) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 5 || !info[0]->IsNumber() || !info[1]->IsUint32() || !info[2]->IsUint32() || !info[3]->IsUint32() || !info[4]->IsUint32()) {
    THROW_BAD_ARGS("Expected 5 arguments: inputHandle (number), R (uint8), G (uint8), B (uint8), flags (uint)");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  uint8 nColorR = static_cast<uint8>(Nan::To<uint32_t>(info[1]).FromJust());
  uint8 nColorG = static_cast<uint8>(Nan::To<uint32_t>(info[2]).FromJust());
  uint8 nColorB = static_cast<uint8>(Nan::To<uint32_t>(info[3]).FromJust());
  unsigned int nFlags = Nan::To<uint32_t>(info[4]).FromJust();

  SteamInput()->SetLEDColor(inputHandle, nColorR, nColorG, nColorB, nFlags);
}

NAN_METHOD(TriggerHapticPulse) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 3 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsUint32()) {
    THROW_BAD_ARGS("Expected 3 arguments: inputHandle (number), targetPad (enum), durationMicroSec (ushort)");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  ESteamControllerPad eTargetPad = static_cast<ESteamControllerPad>(Nan::To<int32_t>(info[1]).FromJust());
  unsigned short usDurationMicroSec = static_cast<unsigned short>(Nan::To<uint32_t>(info[2]).FromJust());

  SteamInput()->TriggerHapticPulse(inputHandle, eTargetPad, usDurationMicroSec);
}

NAN_METHOD(TriggerRepeatedHapticPulse) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 6 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsUint32() || !info[3]->IsUint32() || !info[4]->IsUint32() || !info[5]->IsUint32()) {
    THROW_BAD_ARGS("Expected 6 arguments: inputHandle (number), targetPad (enum), durationMicroSec (ushort), offMicroSec (ushort), repeat (ushort), flags (uint)");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  ESteamControllerPad eTargetPad = static_cast<ESteamControllerPad>(Nan::To<int32_t>(info[1]).FromJust());
  unsigned short usDurationMicroSec = static_cast<unsigned short>(Nan::To<uint32_t>(info[2]).FromJust());
  unsigned short usOffMicroSec = static_cast<unsigned short>(Nan::To<uint32_t>(info[3]).FromJust());
  unsigned short unRepeat = static_cast<unsigned short>(Nan::To<uint32_t>(info[4]).FromJust());
  unsigned int nFlags = Nan::To<uint32_t>(info[5]).FromJust();

  SteamInput()->TriggerRepeatedHapticPulse(inputHandle, eTargetPad, usDurationMicroSec, usOffMicroSec, unRepeat, nFlags);
}

NAN_METHOD(TriggerVibration) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 3 || !info[0]->IsNumber() || !info[1]->IsUint32() || !info[2]->IsUint32()) {
    THROW_BAD_ARGS("Expected 3 arguments: inputHandle (number), leftSpeed (ushort), rightSpeed (ushort)");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  unsigned short usLeftSpeed = static_cast<unsigned short>(Nan::To<uint32_t>(info[1]).FromJust());
  unsigned short usRightSpeed = static_cast<unsigned short>(Nan::To<uint32_t>(info[2]).FromJust());

  SteamInput()->TriggerVibration(inputHandle, usLeftSpeed, usRightSpeed);
}

NAN_METHOD(TriggerVibrationExtended) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 5 || !info[0]->IsNumber() || !info[1]->IsUint32() || !info[2]->IsUint32() || !info[3]->IsUint32() || !info[4]->IsUint32()) {
    THROW_BAD_ARGS("Expected 5 arguments: inputHandle (number), leftSpeed (ushort), rightSpeed (ushort), leftTriggerSpeed (ushort), rightTriggerSpeed (ushort)");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  unsigned short usLeftSpeed = static_cast<unsigned short>(Nan::To<uint32_t>(info[1]).FromJust());
  unsigned short usRightSpeed = static_cast<unsigned short>(Nan::To<uint32_t>(info[2]).FromJust());
  unsigned short usLeftTriggerSpeed = static_cast<unsigned short>(Nan::To<uint32_t>(info[3]).FromJust());
  unsigned short usRightTriggerSpeed = static_cast<unsigned short>(Nan::To<uint32_t>(info[4]).FromJust());

  SteamInput()->TriggerVibrationExtended(inputHandle, usLeftSpeed, usRightSpeed, usLeftTriggerSpeed, usRightTriggerSpeed);
}

// Helper to get uint8 from object property
uint8 GetUint8FromObj(v8::Local<v8::Object> obj, const char* key) {
    return static_cast<uint8>(Nan::To<uint32_t>(Nan::Get(obj, Nan::New(key).ToLocalChecked()).ToLocalChecked()).FromMaybe(0));
}

// Helper to get int32 from object property
int32 GetInt32FromObj(v8::Local<v8::Object> obj, const char* key) {
    return Nan::To<int32_t>(Nan::Get(obj, Nan::New(key).ToLocalChecked()).ToLocalChecked()).FromMaybe(0);
}


NAN_METHOD(SetDualSenseTriggerEffect) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsObject()) {
    THROW_BAD_ARGS("Expected 2 arguments: inputHandle (number), ScePadTriggerEffectParam (object)");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  v8::Local<v8::Object> jsParam = Nan::To<v8::Object>(info[1]).ToLocalChecked();

  ScePadTriggerEffectParam param;
  memset(&param, 0, sizeof(ScePadTriggerEffectParam)); // Zero out the struct

  param.triggerMask = GetInt32FromObj(jsParam, "triggerMask");

  v8::Local<v8::Value> jsCommandVal = Nan::Get(jsParam, Nan::New("command").ToLocalChecked()).ToLocalChecked();
  if (jsCommandVal->IsArray()) {
    v8::Local<v8::Array> jsCommandArray = Nan::To<v8::Array>(jsCommandVal).ToLocalChecked();
    uint32_t numCommands = jsCommandArray->Length();
    if (numCommands > SCE_PAD_TRIGGER_EFFECT_MAX_COMMAND_NUM) {
        numCommands = SCE_PAD_TRIGGER_EFFECT_MAX_COMMAND_NUM; // Cap at max
    }

    for (uint32_t i = 0; i < numCommands; ++i) {
      v8::Local<v8::Value> jsCommandElementVal = Nan::Get(jsCommandArray, i).ToLocalChecked();
      if (jsCommandElementVal->IsObject()) {
        v8::Local<v8::Object> jsCommand = Nan::To<v8::Object>(jsCommandElementVal).ToLocalChecked();
        param.command[i].mode = static_cast<ScePadTriggerEffectMode>(GetInt32FromObj(jsCommand, "mode"));

        v8::Local<v8::Value> jsCommandDataVal = Nan::Get(jsCommand, Nan::New("commandData").ToLocalChecked()).ToLocalChecked();
        if (jsCommandDataVal->IsObject()) {
            v8::Local<v8::Object> jsCommandData = Nan::To<v8::Object>(jsCommandDataVal).ToLocalChecked();
            // Populate based on mode
            switch (param.command[i].mode) {
              case SCE_PAD_TRIGGER_EFFECT_MODE_VIBRATION: {
                v8::Local<v8::Object> jsVibrationParam = Nan::To<v8::Object>(Nan::Get(jsCommandData, Nan::New("vibrationParam").ToLocalChecked()).ToLocalChecked()).ToLocalChecked();
                param.command[i].commandData.vibrationParam.position = GetUint8FromObj(jsVibrationParam, "position");
                param.command[i].commandData.vibrationParam.amplitude = GetUint8FromObj(jsVibrationParam, "amplitude");
                param.command[i].commandData.vibrationParam.frequency = GetUint8FromObj(jsVibrationParam, "frequency");
                break;
              }
              case SCE_PAD_TRIGGER_EFFECT_MODE_FEEDBACK: {
                v8::Local<v8::Object> jsFeedbackParam = Nan::To<v8::Object>(Nan::Get(jsCommandData, Nan::New("feedbackParam").ToLocalChecked()).ToLocalChecked()).ToLocalChecked();
                param.command[i].commandData.feedbackParam.position = GetUint8FromObj(jsFeedbackParam, "position");
                param.command[i].commandData.feedbackParam.strength = GetUint8FromObj(jsFeedbackParam, "strength");
                break;
              }
              case SCE_PAD_TRIGGER_EFFECT_MODE_WEAPON: {
                v8::Local<v8::Object> jsWeaponParam = Nan::To<v8::Object>(Nan::Get(jsCommandData, Nan::New("weaponParam").ToLocalChecked()).ToLocalChecked()).ToLocalChecked();
                param.command[i].commandData.weaponParam.startPosition = GetUint8FromObj(jsWeaponParam, "startPosition");
                param.command[i].commandData.weaponParam.endPosition = GetUint8FromObj(jsWeaponParam, "endPosition");
                param.command[i].commandData.weaponParam.strength = GetUint8FromObj(jsWeaponParam, "strength");
                break;
              }
              // SCE_PAD_TRIGGER_EFFECT_MODE_OFF and other modes might not need specific params or have different ones.
              // Add more cases if other modes have specific data structures.
              default:
                // For SCE_PAD_TRIGGER_EFFECT_MODE_OFF or other modes, commandData might be all zeros or not used.
                break;
            }
        }
      }
    }
  }
  SteamInput()->SetDualSenseTriggerEffect(inputHandle, &param);
}

NAN_METHOD(GetDeviceBindingRevision) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: inputHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());

  int major, minor;
  bool success = SteamInput()->GetDeviceBindingRevision(inputHandle, &major, &minor);

  v8::Local<v8::Object> result = Nan::New<v8::Object>();
  Nan::Set(result, Nan::New("success").ToLocalChecked(), Nan::New(success));
  if (success) {
    Nan::Set(result, Nan::New("major").ToLocalChecked(), Nan::New(major));
    Nan::Set(result, Nan::New("minor").ToLocalChecked(), Nan::New(minor));
  }
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetRemotePlaySessionID) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: inputHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  uint32 sessionID = SteamInput()->GetRemotePlaySessionID(inputHandle);
  info.GetReturnValue().Set(Nan::New(sessionID));
}

NAN_METHOD(RunFrame) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    // Optionally THROW_BAD_ARGS, but RunFrame is often called repeatedly.
    // Depending on desired behavior, might just return if not initialized.
    return;
  }
  SteamInput()->RunFrame();
}

NAN_METHOD(ShowBindingPanel) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: inputHandle");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  bool success = SteamInput()->ShowBindingPanel(inputHandle);
  info.GetReturnValue().Set(Nan::New(success));
}

NAN_METHOD(StopAnalogActionMomentum) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsNumber()) {
    THROW_BAD_ARGS("Expected 2 number arguments: inputHandle, eAction");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  InputAnalogActionHandle_t eAction = static_cast<InputAnalogActionHandle_t>(Nan::To<int64_t>(info[1]).FromJust());
  SteamInput()->StopAnalogActionMomentum(inputHandle, eAction);
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  SET_FUNCTION("init", Init);
  SET_FUNCTION("shutdown", Shutdown);
  SET_FUNCTION("getActionSetHandle", GetActionSetHandle);
  SET_FUNCTION("activateActionSet", ActivateActionSet);
  SET_FUNCTION("getCurrentActionSet", GetCurrentActionSet);
  SET_FUNCTION("activateActionSetLayer", ActivateActionSetLayer);
  SET_FUNCTION("deactivateActionSetLayer", DeactivateActionSetLayer);
  SET_FUNCTION("deactivateAllActionSetLayers", DeactivateAllActionSetLayers);
  SET_FUNCTION("getActiveActionSetLayers", GetActiveActionSetLayers);
  SET_FUNCTION("getAnalogActionHandle", GetAnalogActionHandle);
  SET_FUNCTION("getDigitalActionHandle", GetDigitalActionHandle);
  SET_FUNCTION("getAnalogActionData", GetAnalogActionData);
  SET_FUNCTION("getDigitalActionData", GetDigitalActionData);
  SET_FUNCTION("getMotionData", GetMotionData);
  SET_FUNCTION("getConnectedControllers", GetConnectedControllers);
  SET_FUNCTION("getControllerForGamepadIndex", GetControllerForGamepadIndex);
  SET_FUNCTION("getGamepadIndexForController", GetGamepadIndexForController);
  SET_FUNCTION("getInputTypeForHandle", GetInputTypeForHandle);
  SET_FUNCTION("getAnalogActionOrigins", GetAnalogActionOrigins);
  SET_FUNCTION("getDigitalActionOrigins", GetDigitalActionOrigins);
  SET_FUNCTION("getStringForActionOrigin", GetStringForActionOrigin);
  SET_FUNCTION("getGlyphForActionOrigin", GetGlyphForActionOrigin);
  SET_FUNCTION("getActionOriginFromXboxOrigin", GetActionOriginFromXboxOrigin);
  SET_FUNCTION("translateActionOrigin", TranslateActionOrigin);
  SET_FUNCTION("setLEDColor", SetLEDColor);
  SET_FUNCTION("triggerHapticPulse", TriggerHapticPulse);
  SET_FUNCTION("triggerRepeatedHapticPulse", TriggerRepeatedHapticPulse);
  SET_FUNCTION("triggerVibration", TriggerVibration);
  SET_FUNCTION("triggerVibrationExtended", TriggerVibrationExtended);
  SET_FUNCTION("setDualSenseTriggerEffect", SetDualSenseTriggerEffect);
  SET_FUNCTION("getDeviceBindingRevision", GetDeviceBindingRevision);
  SET_FUNCTION("getRemotePlaySessionID", GetRemotePlaySessionID);
  SET_FUNCTION("runFrame", RunFrame);
  SET_FUNCTION("showBindingPanel", ShowBindingPanel);
  SET_FUNCTION("stopAnalogActionMomentum", StopAnalogActionMomentum);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace input
}  // namespace api
}  // namespace greenworks
