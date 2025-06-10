#include "steam/steam_api.h"
#include "nan.h"
#include "v8.h"
#include "greenworks_utils.h"
#include "steam_api_registry.h"
#include "steam/isteamdualsense.h" // Required for SetDualSenseTriggerEffect
#include <vector> // Required for Nan::MakeCallback with argv

// For thread-safe event emitting
#include "uv.h" // Required for uv_async_send
#include <queue>
#include <mutex> // For std::mutex and std::lock_guard

namespace greenworks {
namespace api {
namespace input {

// Structure to hold event data for async emission
struct EventData {
  std::string event_name;
  v8::Local<v8::Object> data;

  EventData(std::string name, v8::Local<v8::Object> val) : event_name(name) {
    Nan::HandleScope scope;
    data.Reset(val); // This might need careful handling if 'val' is temporary.
                     // For simplicity, assuming it's appropriately managed or copied.
                     // A more robust solution might involve creating persistent handles
                     // or copying data into std::string/basic types.
  }
   // Simplified constructor for events without complex data, or where data is built later
  EventData(std::string name) : event_name(name) {}
};

static std::queue<EventData*> g_event_queue;
static std::mutex g_event_queue_mutex;
static uv_async_t g_event_async;
static Nan::Persistent<v8::Object> g_emitter; // To store 'greenworks' module object

// This function runs on the V8 main thread
static void ProcessEvents(uv_async_t *handle) {
  Nan::HandleScope scope;
  std::queue<EventData*> local_queue;

  {
    std::lock_guard<std::mutex> lock(g_event_queue_mutex);
    std::swap(g_event_queue, local_queue);
  }

  v8::Local<v8::Object> emitter = Nan::New(g_emitter);
  if (emitter.IsEmpty() || !emitter->IsObject()) {
    // Emitter not set or invalid, cannot proceed
    while (!local_queue.empty()) {
      delete local_queue.front();
      local_queue.pop();
    }
    return;
  }

  v8::Local<v8::Function> emit_fn;
  v8::Local<v8::Value> emit_val = Nan::Get(emitter, Nan::New("emit").ToLocalChecked()).ToLocalChecked();
  if (!emit_val->IsFunction()) {
     while (!local_queue.empty()) {
      delete local_queue.front();
      local_queue.pop();
    }
    return; // No emit function
  }
  emit_fn = Nan::To<v8::Function>(emit_val).ToLocalChecked();


  while (!local_queue.empty()) {
    EventData *event_item = local_queue.front();
    Nan::AsyncResource async_resource("greenworks:SteamInputCallback");

    v8::Local<v8::Value> argv[2];
    argv[0] = Nan::New(event_item->event_name.c_str()).ToLocalChecked();
    if (!event_item->data.IsEmpty()) {
        argv[1] = Nan::New(event_item->data);
    } else {
        argv[1] = Nan::Null(); // Or an empty object Nan::New<v8::Object>()
    }

    int argc = event_item->data.IsEmpty() ? 1 : 2;
    Nan::TryCatch try_catch;
    async_resource.runInAsyncScope(emitter, emit_fn, argc, argv);
    if (try_catch.HasCaught()) {
        Nan::FatalException(try_catch);
    }

    delete event_item;
    local_queue.pop();
  }
}


// Helper to emit events safely
void EmitEvent(EventData* event_data) {
  std::lock_guard<std::mutex> lock(g_event_queue_mutex);
  g_event_queue.push(event_data);
  uv_async_send(&g_event_async);
}

// Callback handler for SteamInputConfigurationLoaded_t
class SteamInputConfigLoadedCallback {
public:
    STEAM_CALLBACK(SteamInputConfigLoadedCallback, OnSteamInputConfigurationLoaded, SteamInputConfigurationLoaded_t);
};
void SteamInputConfigLoadedCallback::OnSteamInputConfigurationLoaded(SteamInputConfigurationLoaded_t* pParam) {
  Nan::HandleScope scope;
  v8::Local<v8::Object> data = Nan::New<v8::Object>();
  Nan::Set(data, Nan::New("m_unAppID").ToLocalChecked(), Nan::New(pParam->m_unAppID));
  Nan::Set(data, Nan::New("m_ulDeviceHandle").ToLocalChecked(), Nan::New<v8::Number>(static_cast<double>(pParam->m_ulDeviceHandle)));
  // m_ulMappingCreator is CSteamID, convert to string for JS
  char steamIDString[20]; // Max length for uint64 is 20 digits
  snprintf(steamIDString, sizeof(steamIDString), "%llu", pParam->m_ulMappingCreator.ConvertToUint64());
  Nan::Set(data, Nan::New("m_ulMappingCreator").ToLocalChecked(), Nan::New(steamIDString).ToLocalChecked());
  Nan::Set(data, Nan::New("m_unMajorRevision").ToLocalChecked(), Nan::New(pParam->m_unMajorRevision));
  Nan::Set(data, Nan::New("m_unMinorRevision").ToLocalChecked(), Nan::New(pParam->m_unMinorRevision));
  Nan::Set(data, Nan::New("m_bUsesSteamInputAPI").ToLocalChecked(), Nan::New(pParam->m_bUsesSteamInputAPI));
  Nan::Set(data, Nan::New("m_bUsesGamepadAPI").ToLocalChecked(), Nan::New(pParam->m_bUsesGamepadAPI));
  EmitEvent(new EventData("steam-input-configuration-loaded", data));
}
SteamInputConfigLoadedCallback* g_SteamInputConfigLoadedCallback = nullptr;

// Callback handler for SteamInputDeviceConnected_t
class SteamInputDeviceConnectedCallback {
public:
    STEAM_CALLBACK(SteamInputDeviceConnectedCallback, OnSteamInputDeviceConnected, SteamInputDeviceConnected_t);
};
void SteamInputDeviceConnectedCallback::OnSteamInputDeviceConnected(SteamInputDeviceConnected_t* pParam) {
  Nan::HandleScope scope;
  v8::Local<v8::Object> data = Nan::New<v8::Object>();
  Nan::Set(data, Nan::New("m_ulConnectedDeviceHandle").ToLocalChecked(), Nan::New<v8::Number>(static_cast<double>(pParam->m_ulConnectedDeviceHandle)));
  EmitEvent(new EventData("steam-input-device-connected", data));
}
SteamInputDeviceConnectedCallback* g_SteamInputDeviceConnectedCallback = nullptr;

// Callback handler for SteamInputDeviceDisconnected_t
class SteamInputDeviceDisconnectedCallback {
public:
    STEAM_CALLBACK(SteamInputDeviceDisconnectedCallback, OnSteamInputDeviceDisconnected, SteamInputDeviceDisconnected_t);
};
void SteamInputDeviceDisconnectedCallback::OnSteamInputDeviceDisconnected(SteamInputDeviceDisconnected_t* pParam) {
  Nan::HandleScope scope;
  v8::Local<v8::Object> data = Nan::New<v8::Object>();
  Nan::Set(data, Nan::New("m_ulDisconnectedDeviceHandle").ToLocalChecked(), Nan::New<v8::Number>(static_cast<double>(pParam->m_ulDisconnectedDeviceHandle)));
  EmitEvent(new EventData("steam-input-device-disconnected", data));
}
SteamInputDeviceDisconnectedCallback* g_SteamInputDeviceDisconnectedCallback = nullptr;

// Callback handler for SteamInputGamepadSlotChange_t
class SteamInputGamepadSlotChangeCallback {
public:
    STEAM_CALLBACK(SteamInputGamepadSlotChangeCallback, OnSteamInputGamepadSlotChange, SteamInputGamepadSlotChange_t);
};
void SteamInputGamepadSlotChangeCallback::OnSteamInputGamepadSlotChange(SteamInputGamepadSlotChange_t* pParam) {
  Nan::HandleScope scope;
  v8::Local<v8::Object> data = Nan::New<v8::Object>();
  Nan::Set(data, Nan::New("m_unAppID").ToLocalChecked(), Nan::New(pParam->m_unAppID));
  Nan::Set(data, Nan::New("m_ulDeviceHandle").ToLocalChecked(), Nan::New<v8::Number>(static_cast<double>(pParam->m_ulDeviceHandle)));
  Nan::Set(data, Nan::New("m_eDeviceType").ToLocalChecked(), Nan::New(static_cast<int>(pParam->m_eDeviceType)));
  Nan::Set(data, Nan::New("m_nOldGamepadSlot").ToLocalChecked(), Nan::New(pParam->m_nOldGamepadSlot));
  Nan::Set(data, Nan::New("m_nNewGamepadSlot").ToLocalChecked(), Nan::New(pParam->m_nNewGamepadSlot));
  EmitEvent(new EventData("steam-input-gamepad-slot-change", data));
}
SteamInputGamepadSlotChangeCallback* g_SteamInputGamepadSlotChangeCallback = nullptr;

// Static handler for SteamInputActionEvent_t
void SteamInputActionEventCallbackHandler(SteamInputActionEvent_t *pEvent) {
    Nan::HandleScope scope;
    v8::Local<v8::Object> eventData = Nan::New<v8::Object>();
    Nan::Set(eventData, Nan::New("controllerHandle").ToLocalChecked(), Nan::New<v8::Number>(static_cast<double>(pEvent->controllerHandle)));
    Nan::Set(eventData, Nan::New("eEventType").ToLocalChecked(), Nan::New(static_cast<int>(pEvent->eEventType)));

    if (pEvent->eEventType == ESteamInputActionEventType::ESteamInputActionEventType_DigitalAction) {
        v8::Local<v8::Object> digitalAction = Nan::New<v8::Object>();
        Nan::Set(digitalAction, Nan::New("actionHandle").ToLocalChecked(), Nan::New<v8::Number>(static_cast<double>(pEvent->digitalAction.actionHandle)));
        v8::Local<v8::Object> actionData = Nan::New<v8::Object>();
        Nan::Set(actionData, Nan::New("bState").ToLocalChecked(), Nan::New(pEvent->digitalAction.digitalActionData.bState));
        Nan::Set(actionData, Nan::New("bActive").ToLocalChecked(), Nan::New(pEvent->digitalAction.digitalActionData.bActive));
        Nan::Set(digitalAction, Nan::New("digitalActionData").ToLocalChecked(), actionData);
        Nan::Set(eventData, Nan::New("digitalAction").ToLocalChecked(), digitalAction);
    } else if (pEvent->eEventType == ESteamInputActionEventType::ESteamInputActionEventType_AnalogAction) {
        v8::Local<v8::Object> analogAction = Nan::New<v8::Object>();
        Nan::Set(analogAction, Nan::New("actionHandle").ToLocalChecked(), Nan::New<v8::Number>(static_cast<double>(pEvent->analogAction.actionHandle)));
        v8::Local<v8::Object> actionData = Nan::New<v8::Object>();
        Nan::Set(actionData, Nan::New("eMode").ToLocalChecked(), Nan::New(static_cast<int>(pEvent->analogAction.analogActionData.eMode)));
        Nan::Set(actionData, Nan::New("x").ToLocalChecked(), Nan::New(pEvent->analogAction.analogActionData.x));
        Nan::Set(actionData, Nan::New("y").ToLocalChecked(), Nan::New(pEvent->analogAction.analogActionData.y));
        Nan::Set(actionData, Nan::New("bActive").ToLocalChecked(), Nan::New(pEvent->analogAction.analogActionData.bActive));
        Nan::Set(analogAction, Nan::New("analogActionData").ToLocalChecked(), actionData);
        Nan::Set(eventData, Nan::New("analogAction").ToLocalChecked(), analogAction);
    }
    EmitEvent(new EventData("steam-input-action-event", eventData));
}


NAN_METHOD(Init) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    info.GetReturnValue().Set(Nan::New(false));
    return;
  }
  // In the header, Init takes a bool: bExplicitlyCallRunFrame
  // For now, defaulting to true, meaning RunFrame must be called by the game.
  // Or, we can expose this if necessary. For simplicity with current greenworks,
  // let's assume true is a reasonable default.
  bool bExplicitlyCallRunFrame = true;
  if (info.Length() > 0 && info[0]->IsBoolean()) {
     bExplicitlyCallRunFrame = Nan::To<bool>(info[0]).FromJust();
  }
  info.GetReturnValue().Set(Nan::New(SteamInput()->Init(bExplicitlyCallRunFrame)));
}

NAN_METHOD(Shutdown) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    info.GetReturnValue().Set(Nan::New(false));
    return;
  }
  info.GetReturnValue().Set(Nan::New(SteamInput()->Shutdown()));
}

NAN_METHOD(SetInputActionManifestFilePath) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Expected 1 string argument: pchInputActionManifestAbsolutePath");
    return;
  }
  Nan::Utf8String path(info[0]);
  info.GetReturnValue().Set(Nan::New(SteamInput()->SetInputActionManifestFilePath(*path)));
}

NAN_METHOD(WaitForData) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsBoolean() || !info[1]->IsUint32()) {
    THROW_BAD_ARGS("Expected 2 arguments: bWaitForever (boolean), unTimeout (uint32)");
    return;
  }
  bool bWaitForever = Nan::To<bool>(info[0]).FromJust();
  uint32 unTimeout = Nan::To<uint32_t>(info[1]).FromJust();
  info.GetReturnValue().Set(Nan::New(SteamInput()->BWaitForData(bWaitForever, unTimeout)));
}

NAN_METHOD(IsNewDataAvailable) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  info.GetReturnValue().Set(Nan::New(SteamInput()->BNewDataAvailable()));
}

NAN_METHOD(EnableDeviceCallbacks) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  SteamInput()->EnableDeviceCallbacks();
  // Register callbacks if not already done
  if (!g_SteamInputConfigLoadedCallback) g_SteamInputConfigLoadedCallback = new SteamInputConfigLoadedCallback();
  if (!g_SteamInputDeviceConnectedCallback) g_SteamInputDeviceConnectedCallback = new SteamInputDeviceConnectedCallback();
  if (!g_SteamInputDeviceDisconnectedCallback) g_SteamInputDeviceDisconnectedCallback = new SteamInputDeviceDisconnectedCallback();
  if (!g_SteamInputGamepadSlotChangeCallback) g_SteamInputGamepadSlotChangeCallback = new SteamInputGamepadSlotChangeCallback();
  // This function returns void, so no SetReturnValue needed unless for chaining/status.
}

NAN_METHOD(EnableActionEventCallbacks) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  SteamInput()->EnableActionEventCallbacks(SteamInputActionEventCallbackHandler);
  // This function returns void.
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

NAN_METHOD(GetStringForDigitalActionName) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: actionHandle");
    return;
  }
  InputDigitalActionHandle_t actionHandle = static_cast<InputDigitalActionHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  const char* actionName = SteamInput()->GetStringForDigitalActionName(actionHandle);
  if (actionName) {
    info.GetReturnValue().Set(Nan::New(actionName).ToLocalChecked());
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
}

NAN_METHOD(GetStringForAnalogActionName) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: actionHandle");
    return;
  }
  InputAnalogActionHandle_t actionHandle = static_cast<InputAnalogActionHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  const char* actionName = SteamInput()->GetStringForAnalogActionName(actionHandle);
  if (actionName) {
    info.GetReturnValue().Set(Nan::New(actionName).ToLocalChecked());
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
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

NAN_METHOD(GetGlyphPNGForActionOrigin) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 3 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsUint32()) {
    THROW_BAD_ARGS("Expected 3 arguments: eOrigin (number), eSize (enum), unFlags (uint32)");
    return;
  }
  EInputActionOrigin eOrigin = static_cast<EInputActionOrigin>(Nan::To<int32_t>(info[0]).FromJust());
  ESteamInputGlyphSize eSize = static_cast<ESteamInputGlyphSize>(Nan::To<int32_t>(info[1]).FromJust());
  uint32 unFlags = Nan::To<uint32_t>(info[2]).FromJust();
  const char* glyphPath = SteamInput()->GetGlyphPNGForActionOrigin(eOrigin, eSize, unFlags);
  if (glyphPath) {
    info.GetReturnValue().Set(Nan::New(glyphPath).ToLocalChecked());
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
}

NAN_METHOD(GetGlyphSVGForActionOrigin) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 2 || !info[0]->IsNumber() || !info[1]->IsUint32()) {
    THROW_BAD_ARGS("Expected 2 arguments: eOrigin (number), unFlags (uint32)");
    return;
  }
  EInputActionOrigin eOrigin = static_cast<EInputActionOrigin>(Nan::To<int32_t>(info[0]).FromJust());
  uint32 unFlags = Nan::To<uint32_t>(info[1]).FromJust();
  const char* glyphPath = SteamInput()->GetGlyphSVGForActionOrigin(eOrigin, unFlags);
  if (glyphPath) {
    info.GetReturnValue().Set(Nan::New(glyphPath).ToLocalChecked());
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
}

NAN_METHOD(GetGlyphForActionOriginLegacy) { // Renamed from GetGlyphForActionOrigin
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
  // This function in isteaminput.h is GetGlyphForActionOrigin_Legacy
  const char* glyphPath = SteamInput()->GetGlyphForActionOrigin_Legacy(eOrigin);
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

NAN_METHOD(TriggerSimpleHapticEvent) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 6 || !info[0]->IsNumber() || !info[1]->IsNumber() || !info[2]->IsUint32() || !info[3]->IsInt32() || !info[4]->IsUint32() || !info[5]->IsInt32()) {
    THROW_BAD_ARGS("Expected 6 arguments: inputHandle (number), eHapticLocation (enum), nIntensity (uint8), nGainDB (char), nOtherIntensity (uint8), nOtherGainDB (char)");
    return;
  }
  InputHandle_t inputHandle = static_cast<InputHandle_t>(Nan::To<int64_t>(info[0]).FromJust());
  EControllerHapticLocation eHapticLocation = static_cast<EControllerHapticLocation>(Nan::To<int32_t>(info[1]).FromJust());
  uint8 nIntensity = static_cast<uint8>(Nan::To<uint32_t>(info[2]).FromJust());
  char nGainDB = static_cast<char>(Nan::To<int32_t>(info[3]).FromJust());
  uint8 nOtherIntensity = static_cast<uint8>(Nan::To<uint32_t>(info[4]).FromJust());
  char nOtherGainDB = static_cast<char>(Nan::To<int32_t>(info[5]).FromJust());

  SteamInput()->TriggerSimpleHapticEvent(inputHandle, eHapticLocation, nIntensity, nGainDB, nOtherIntensity, nOtherGainDB);
}

NAN_METHOD(GetStringForXboxOrigin) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: eOrigin");
    return;
  }
  EXboxOrigin eOrigin = static_cast<EXboxOrigin>(Nan::To<int32_t>(info[0]).FromJust());
  const char* originString = SteamInput()->GetStringForXboxOrigin(eOrigin);
  if (originString) {
    info.GetReturnValue().Set(Nan::New(originString).ToLocalChecked());
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
}

NAN_METHOD(GetGlyphForXboxOrigin) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  if (info.Length() < 1 || !info[0]->IsNumber()) {
    THROW_BAD_ARGS("Expected 1 number argument: eOrigin");
    return;
  }
  EXboxOrigin eOrigin = static_cast<EXboxOrigin>(Nan::To<int32_t>(info[0]).FromJust());
  const char* glyphPath = SteamInput()->GetGlyphForXboxOrigin(eOrigin);
  if (glyphPath) {
    info.GetReturnValue().Set(Nan::New(glyphPath).ToLocalChecked());
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
}

NAN_METHOD(GetSessionInputConfigurationSettings) {
  Nan::HandleScope scope;
  if (!SteamInput()) {
    THROW_BAD_ARGS("SteamInput not initialized");
    return;
  }
  info.GetReturnValue().Set(Nan::New(SteamInput()->GetSessionInputConfigurationSettings()));
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
  bool bReservedValue = true; // Default as per header
  if (info.Length() > 0 && info[0]->IsBoolean()) {
    bReservedValue = Nan::To<bool>(info[0]).FromJust();
  }
  SteamInput()->RunFrame(bReservedValue);
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
  // Store the emitter for later use in EmitEvent
  // This should ideally be done once when the module initializes.
  // For simplicity, doing it here, but it might re-assign multiple times if Init is called more than once.
  // A better place would be in the module's main init function if available, or use a static bool flag.
  if (g_emitter.IsEmpty() && target->IsObject()) {
      g_emitter.Reset(target); // Assuming 'target' is the greenworks module object
      uv_async_init(uv_default_loop(), &g_event_async, ProcessEvents);
  }

  SET_FUNCTION("init", Init);
  SET_FUNCTION("shutdown", Shutdown);
  SET_FUNCTION("setInputActionManifestFilePath", SetInputActionManifestFilePath);
  SET_FUNCTION("waitForData", WaitForData);
  SET_FUNCTION("isNewDataAvailable", IsNewDataAvailable);
  SET_FUNCTION("enableDeviceCallbacks", EnableDeviceCallbacks); // Modified
  SET_FUNCTION("enableActionEventCallbacks", EnableActionEventCallbacks); // Modified
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
  SET_FUNCTION("getStringForDigitalActionName", GetStringForDigitalActionName);
  SET_FUNCTION("getStringForAnalogActionName", GetStringForAnalogActionName);
  SET_FUNCTION("getMotionData", GetMotionData);
  SET_FUNCTION("getConnectedControllers", GetConnectedControllers);
  SET_FUNCTION("getControllerForGamepadIndex", GetControllerForGamepadIndex);
  SET_FUNCTION("getGamepadIndexForController", GetGamepadIndexForController);
  SET_FUNCTION("getInputTypeForHandle", GetInputTypeForHandle);
  SET_FUNCTION("getAnalogActionOrigins", GetAnalogActionOrigins);
  SET_FUNCTION("getDigitalActionOrigins", GetDigitalActionOrigins);
  SET_FUNCTION("getStringForActionOrigin", GetStringForActionOrigin);
  SET_FUNCTION("getGlyphPNGForActionOrigin", GetGlyphPNGForActionOrigin);
  SET_FUNCTION("getGlyphSVGForActionOrigin", GetGlyphSVGForActionOrigin);
  SET_FUNCTION("getGlyphForActionOriginLegacy", GetGlyphForActionOriginLegacy); // Renamed
  SET_FUNCTION("getActionOriginFromXboxOrigin", GetActionOriginFromXboxOrigin);
  SET_FUNCTION("translateActionOrigin", TranslateActionOrigin);
  SET_FUNCTION("setLEDColor", SetLEDColor);
  SET_FUNCTION("triggerHapticPulse", TriggerHapticPulse); // Maps to Legacy_TriggerHapticPulse
  SET_FUNCTION("triggerRepeatedHapticPulse", TriggerRepeatedHapticPulse); // Maps to Legacy_TriggerRepeatedHapticPulse
  SET_FUNCTION("triggerVibration", TriggerVibration);
  SET_FUNCTION("triggerVibrationExtended", TriggerVibrationExtended);
  SET_FUNCTION("triggerSimpleHapticEvent", TriggerSimpleHapticEvent);
  SET_FUNCTION("getStringForXboxOrigin", GetStringForXboxOrigin);
  SET_FUNCTION("getGlyphForXboxOrigin", GetGlyphForXboxOrigin);
  SET_FUNCTION("setDualSenseTriggerEffect", SetDualSenseTriggerEffect);
  SET_FUNCTION("getDeviceBindingRevision", GetDeviceBindingRevision);
  SET_FUNCTION("getRemotePlaySessionID", GetRemotePlaySessionID);
  SET_FUNCTION("getSessionInputConfigurationSettings", GetSessionInputConfigurationSettings);
  SET_FUNCTION("runFrame", RunFrame); // Modified
  SET_FUNCTION("showBindingPanel", ShowBindingPanel);
  SET_FUNCTION("stopAnalogActionMomentum", StopAnalogActionMomentum);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace input
}  // namespace api
}  // namespace greenworks
