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

void InitChatMemberStateChange(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> chat_member_state_change = Nan::New<v8::Object>();
  SET_TYPE(chat_member_state_change, "Entered", k_EChatMemberStateChangeEntered);
  SET_TYPE(chat_member_state_change, "Left", k_EChatMemberStateChangeLeft);
  SET_TYPE(chat_member_state_change, "Disconnected", k_EChatMemberStateChangeDisconnected);
  SET_TYPE(chat_member_state_change, "Kicked", k_EChatMemberStateChangeKicked);
  SET_TYPE(chat_member_state_change, "Banned", k_EChatMemberStateChangeBanned);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(chat_member_state_change);
  Nan::Set(exports, Nan::New("ChatMemberStateChange").ToLocalChecked(), chat_member_state_change);
}

void InitLobbyComparison(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> lobby_comparison = Nan::New<v8::Object>();
  SET_TYPE(lobby_comparison, "EqualToOrLessThan", k_ELobbyComparisonEqualToOrLessThan);
  SET_TYPE(lobby_comparison, "LessThan", k_ELobbyComparisonLessThan);
  SET_TYPE(lobby_comparison, "Equal", k_ELobbyComparisonEqual);
  SET_TYPE(lobby_comparison, "GreaterThan", k_ELobbyComparisonGreaterThan);
  SET_TYPE(lobby_comparison, "EqualToOrGreaterThan", k_ELobbyComparisonEqualToOrGreaterThan);
  SET_TYPE(lobby_comparison, "NotEqual", k_ELobbyComparisonNotEqual);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(lobby_comparison);
  Nan::Set(exports, Nan::New("LobbyComparison").ToLocalChecked(), lobby_comparison);
}

void InitLobbyDistanceFilter(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> lobby_distance_filter = Nan::New<v8::Object>();
  SET_TYPE(lobby_distance_filter, "Close", k_ELobbyDistanceFilterClose);
  SET_TYPE(lobby_distance_filter, "Default", k_ELobbyDistanceFilterDefault);
  SET_TYPE(lobby_distance_filter, "Far", k_ELobbyDistanceFilterFar);
  SET_TYPE(lobby_distance_filter, "Worldwide", k_ELobbyDistanceFilterWorldwide);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(lobby_distance_filter);
  Nan::Set(exports, Nan::New("LobbyDistanceFilter").ToLocalChecked(), lobby_distance_filter);
}

void InitLobbyType(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> lobby_type = Nan::New<v8::Object>();
  SET_TYPE(lobby_type, "Private", k_ELobbyTypePrivate);
  SET_TYPE(lobby_type, "FriendsOnly", k_ELobbyTypeFriendsOnly);
  SET_TYPE(lobby_type, "Public", k_ELobbyTypePublic);
  SET_TYPE(lobby_type, "Invisible", k_ELobbyTypeInvisible);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(lobby_type);
  Nan::Set(exports, Nan::New("LobbyType").ToLocalChecked(), lobby_type);
}

void InitResult(v8::Local<v8::Object> exports) {
  v8::Local<v8::Object> e_result = Nan::New<v8::Object>();
  SET_TYPE(e_result, "OK", k_EResultOK);
  SET_TYPE(e_result, "Fail", k_EResultFail);
  SET_TYPE(e_result, "NoConnection", k_EResultNoConnection);
  SET_TYPE(e_result, "InvalidPassword", k_EResultInvalidPassword);
  SET_TYPE(e_result, "LoggedInElsewhere", k_EResultLoggedInElsewhere);
  SET_TYPE(e_result, "InvalidProtocolVer", k_EResultInvalidProtocolVer);
  SET_TYPE(e_result, "InvalidParam", k_EResultInvalidParam);
  SET_TYPE(e_result, "FileNotFound", k_EResultFileNotFound);
  SET_TYPE(e_result, "Busy", k_EResultBusy);
  SET_TYPE(e_result, "InvalidState", k_EResultInvalidState);
  SET_TYPE(e_result, "InvalidName", k_EResultInvalidName);
  SET_TYPE(e_result, "InvalidEmail", k_EResultInvalidEmail);
  SET_TYPE(e_result, "DuplicateName", k_EResultDuplicateName);
  SET_TYPE(e_result, "AccessDenied", k_EResultAccessDenied);
  SET_TYPE(e_result, "Timeout", k_EResultTimeout);
  SET_TYPE(e_result, "Banned", k_EResultBanned);
  SET_TYPE(e_result, "AccountNotFound", k_EResultAccountNotFound);
  SET_TYPE(e_result, "InvalidSteamID", k_EResultInvalidSteamID);
  SET_TYPE(e_result, "ServiceUnavailable", k_EResultServiceUnavailable);
  SET_TYPE(e_result, "NotLoggedOn", k_EResultNotLoggedOn);
  SET_TYPE(e_result, "Pending", k_EResultPending);
  SET_TYPE(e_result, "EncryptionFailure", k_EResultEncryptionFailure);
  SET_TYPE(e_result, "InsufficientPrivilege", k_EResultInsufficientPrivilege);
  SET_TYPE(e_result, "LimitExceeded", k_EResultLimitExceeded);
  SET_TYPE(e_result, "Revoked", k_EResultRevoked);
  SET_TYPE(e_result, "Expired", k_EResultExpired);
  SET_TYPE(e_result, "AlreadyRedeemed", k_EResultAlreadyRedeemed);
  SET_TYPE(e_result, "DuplicateRequest", k_EResultDuplicateRequest);
  SET_TYPE(e_result, "AlreadyOwned", k_EResultAlreadyOwned);
  SET_TYPE(e_result, "IPNotFound", k_EResultIPNotFound);
  SET_TYPE(e_result, "PersistFailed", k_EResultPersistFailed);
  SET_TYPE(e_result, "LockingFailed", k_EResultLockingFailed);
  SET_TYPE(e_result, "LogonSessionReplaced", k_EResultLogonSessionReplaced);
  SET_TYPE(e_result, "ConnectFailed", k_EResultConnectFailed);
  SET_TYPE(e_result, "HandshakeFailed", k_EResultHandshakeFailed);
  SET_TYPE(e_result, "IOFailure", k_EResultIOFailure);
  SET_TYPE(e_result, "RemoteDisconnect", k_EResultRemoteDisconnect);
  SET_TYPE(e_result, "ShoppingCartNotFound", k_EResultShoppingCartNotFound);
  SET_TYPE(e_result, "Blocked", k_EResultBlocked);
  SET_TYPE(e_result, "Ignored", k_EResultIgnored);
  SET_TYPE(e_result, "NoMatch", k_EResultNoMatch);
  SET_TYPE(e_result, "AccountDisabled", k_EResultAccountDisabled);
  SET_TYPE(e_result, "ServiceReadOnly", k_EResultServiceReadOnly);
  SET_TYPE(e_result, "AccountNotFeatured", k_EResultAccountNotFeatured);
  SET_TYPE(e_result, "AdministratorOK", k_EResultAdministratorOK);
  SET_TYPE(e_result, "ContentVersion", k_EResultContentVersion);
  SET_TYPE(e_result, "TryAnotherCM", k_EResultTryAnotherCM);
  SET_TYPE(e_result, "PasswordRequiredToKickSession", k_EResultPasswordRequiredToKickSession);
  SET_TYPE(e_result, "AlreadyLoggedInElsewhere", k_EResultAlreadyLoggedInElsewhere);
  SET_TYPE(e_result, "Suspended", k_EResultSuspended);
  SET_TYPE(e_result, "Cancelled", k_EResultCancelled);
  SET_TYPE(e_result, "DataCorruption", k_EResultDataCorruption);
  SET_TYPE(e_result, "DiskFull", k_EResultDiskFull);
  SET_TYPE(e_result, "RemoteCallFailed", k_EResultRemoteCallFailed);
  SET_TYPE(e_result, "PasswordUnset", k_EResultPasswordUnset);
  SET_TYPE(e_result, "ExternalAccountUnlinked", k_EResultExternalAccountUnlinked);
  SET_TYPE(e_result, "PSNTicketInvalid", k_EResultPSNTicketInvalid);
  SET_TYPE(e_result, "ExternalAccountAlreadyLinked", k_EResultExternalAccountAlreadyLinked);
  SET_TYPE(e_result, "RemoteFileConflict", k_EResultRemoteFileConflict);
  SET_TYPE(e_result, "IllegalPassword", k_EResultIllegalPassword);
  SET_TYPE(e_result, "SameAsPreviousValue", k_EResultSameAsPreviousValue);
  SET_TYPE(e_result, "AccountLogonDenied", k_EResultAccountLogonDenied);
  SET_TYPE(e_result, "CannotUseOldPassword", k_EResultCannotUseOldPassword);
  SET_TYPE(e_result, "InvalidLoginAuthCode", k_EResultInvalidLoginAuthCode);
  SET_TYPE(e_result, "AccountLogonDeniedNoMail", k_EResultAccountLogonDeniedNoMail);
  SET_TYPE(e_result, "HardwareNotCapableOfIPT", k_EResultHardwareNotCapableOfIPT);
  SET_TYPE(e_result, "IPTInitError", k_EResultIPTInitError);
  SET_TYPE(e_result, "ParentalControlRestricted", k_EResultParentalControlRestricted);
  SET_TYPE(e_result, "FacebookQueryError", k_EResultFacebookQueryError);
  SET_TYPE(e_result, "ExpiredLoginAuthCode", k_EResultExpiredLoginAuthCode);
  SET_TYPE(e_result, "IPLoginRestrictionFailed", k_EResultIPLoginRestrictionFailed);
  SET_TYPE(e_result, "AccountLockedDown", k_EResultAccountLockedDown);
  SET_TYPE(e_result, "AccountLogonDeniedVerifiedEmailRequired", k_EResultAccountLogonDeniedVerifiedEmailRequired);
  SET_TYPE(e_result, "NoMatchingURL", k_EResultNoMatchingURL);
  SET_TYPE(e_result, "BadResponse", k_EResultBadResponse);
  SET_TYPE(e_result, "RequirePasswordReEntry", k_EResultRequirePasswordReEntry);
  SET_TYPE(e_result, "ValueOutOfRange", k_EResultValueOutOfRange);
  SET_TYPE(e_result, "UnexpectedError", k_EResultUnexpectedError);
  SET_TYPE(e_result, "Disabled", k_EResultDisabled);
  SET_TYPE(e_result, "InvalidCEGSubmission", k_EResultInvalidCEGSubmission);
  SET_TYPE(e_result, "RestrictedDevice", k_EResultRestrictedDevice);
  SET_TYPE(e_result, "RegionLocked", k_EResultRegionLocked);
  SET_TYPE(e_result, "RateLimitExceeded", k_EResultRateLimitExceeded);
  SET_TYPE(e_result, "AccountLoginDeniedNeedTwoFactor", k_EResultAccountLoginDeniedNeedTwoFactor);
  SET_TYPE(e_result, "ItemDeleted", k_EResultItemDeleted);
  SET_TYPE(e_result, "AccountLoginDeniedThrottle", k_EResultAccountLoginDeniedThrottle);
  SET_TYPE(e_result, "TwoFactorCodeMismatch", k_EResultTwoFactorCodeMismatch);
  SET_TYPE(e_result, "TwoFactorActivationCodeMismatch", k_EResultTwoFactorActivationCodeMismatch);
  SET_TYPE(e_result, "AccountAssociatedToMultiplePartners", k_EResultAccountAssociatedToMultiplePartners);
  SET_TYPE(e_result, "NotModified", k_EResultNotModified);
  SET_TYPE(e_result, "NoMobileDevice", k_EResultNoMobileDevice);
  SET_TYPE(e_result, "TimeNotSynced", k_EResultTimeNotSynced);
  SET_TYPE(e_result, "SmsCodeFailed", k_EResultSmsCodeFailed);
  SET_TYPE(e_result, "AccountLimitExceeded", k_EResultAccountLimitExceeded);
  SET_TYPE(e_result, "AccountActivityLimitExceeded", k_EResultAccountActivityLimitExceeded);
  SET_TYPE(e_result, "PhoneActivityLimitExceeded", k_EResultPhoneActivityLimitExceeded);
  SET_TYPE(e_result, "RefundToWallet", k_EResultRefundToWallet);
  SET_TYPE(e_result, "EmailSendFailure", k_EResultEmailSendFailure);
  SET_TYPE(e_result, "NotSettled", k_EResultNotSettled);
  SET_TYPE(e_result, "NeedCaptcha", k_EResultNeedCaptcha);
  SET_TYPE(e_result, "GSLTDenied", k_EResultGSLTDenied);
  SET_TYPE(e_result, "GSOwnerDenied", k_EResultGSOwnerDenied);
  SET_TYPE(e_result, "InvalidItemType", k_EResultInvalidItemType);
  SET_TYPE(e_result, "IPBanned", k_EResultIPBanned);
  SET_TYPE(e_result, "GSLTExpired", k_EResultGSLTExpired);
  SET_TYPE(e_result, "InsufficientFunds", k_EResultInsufficientFunds);
  SET_TYPE(e_result, "TooManyPending", k_EResultTooManyPending);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(e_result);
  Nan::Set(exports, Nan::New("Result").ToLocalChecked(), e_result);
}

NAN_METHOD(CreateLobby) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsInt32() || !info[1]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  auto lobby_type = static_cast<ELobbyType>(Nan::To<int32>(info[0]).FromJust());

  info.GetReturnValue().Set(
    Nan::New(
      utils::uint64ToString(
        SteamMatchmaking()->CreateLobby(lobby_type, Nan::To<int32>(info[1]).FromJust())
      )
    ).ToLocalChecked()
  );
}

NAN_METHOD(DeleteLobbyData) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  std::string pch_key_str(*(Nan::Utf8String(info[1])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->DeleteLobbyData(steam_id, pch_key_str.data())
  );
}

NAN_METHOD(GetLobbyByIndex) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  CSteamID lobby_id = SteamMatchmaking()->GetLobbyByIndex(Nan::To<int32>(info[0]).FromJust());
  v8::Local<v8::Object> result = greenworks::SteamID::Create(lobby_id);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetLobbyData) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  std::string pch_key_str(*(Nan::Utf8String(info[1])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    Nan::New(
      SteamMatchmaking()->GetLobbyData(steam_id, pch_key_str.data())
    ).ToLocalChecked()
  );
}

NAN_METHOD(GetLobbyMemberByIndex) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  CSteamID user_id = SteamMatchmaking()->GetLobbyMemberByIndex(steam_id, Nan::To<int32>(info[1]).FromJust());
  v8::Local<v8::Object> result = greenworks::SteamID::Create(user_id);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(GetNumLobbyMembers) {
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
    Nan::New<v8::Integer>(
        SteamMatchmaking()->GetNumLobbyMembers(steam_id)
    )
  );
}

NAN_METHOD(GetLobbyOwner) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  CSteamID user_id = SteamMatchmaking()->GetLobbyOwner(steam_id);
  v8::Local<v8::Object> result = greenworks::SteamID::Create(user_id);
  info.GetReturnValue().Set(result);
}

NAN_METHOD(InviteUserToLobby) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_lobby_str(*(Nan::Utf8String(info[0])));
  std::string steam_id_user_str(*(Nan::Utf8String(info[1])));
  CSteamID steam_id_lobby(utils::strToUint64(steam_id_lobby_str));
  CSteamID steam_id_user(utils::strToUint64(steam_id_user_str));
  if (!steam_id_lobby.IsValid() || !steam_id_user.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->InviteUserToLobby(steam_id_lobby, steam_id_user)
  );
}

NAN_METHOD(JoinLobby) {
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
    Nan::New(
      utils::uint64ToString(
        SteamMatchmaking()->JoinLobby(steam_id)
      )
    ).ToLocalChecked()
  );
}

NAN_METHOD(LeaveLobby) {
  Nan::HandleScope scope;
  if (info.Length() < 1 || !info[0]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  SteamMatchmaking()->LeaveLobby(steam_id);
}

NAN_METHOD(SetLobbyData) {
  Nan::HandleScope scope;
  if (info.Length() < 3 || !info[0]->IsString() || !info[1]->IsString() || !info[2]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  std::string pch_key_str(*(Nan::Utf8String(info[1])));
  std::string pch_value_str(*(Nan::Utf8String(info[2])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->SetLobbyData(steam_id, pch_key_str.data(), pch_value_str.data())
  );
}

NAN_METHOD(SetLobbyJoinable) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsBoolean()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->SetLobbyJoinable(steam_id, Nan::To<int32>(info[0]).FromJust())
  );
}

NAN_METHOD(SetLobbyOwner) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsString()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_lobby_str(*(Nan::Utf8String(info[0])));
  std::string steam_id_user_str(*(Nan::Utf8String(info[1])));
  CSteamID steam_id_lobby(utils::strToUint64(steam_id_lobby_str));
  CSteamID steam_id_user(utils::strToUint64(steam_id_user_str));
  if (!steam_id_lobby.IsValid() || !steam_id_user.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  info.GetReturnValue().Set(
    SteamMatchmaking()->SetLobbyOwner(steam_id_lobby, steam_id_user)
  );
}

NAN_METHOD(SetLobbyType) {
  Nan::HandleScope scope;
  if (info.Length() < 2 || !info[0]->IsString() || !info[1]->IsInt32()) {
    THROW_BAD_ARGS("Bad arguments");
  }
  std::string steam_id_str(*(Nan::Utf8String(info[0])));
  CSteamID steam_id(utils::strToUint64(steam_id_str));
  if (!steam_id.IsValid()) {
    THROW_BAD_ARGS("Steam ID is invalid");
  }
  auto lobby_type = static_cast<ELobbyType>(Nan::To<int32>(info[0]).FromJust());
  
  info.GetReturnValue().Set(
    SteamMatchmaking()->SetLobbyType(steam_id, lobby_type)
  );
}

void RegisterAPIs(v8::Local<v8::Object> target) {
  InitChatMemberStateChange(target);
  InitLobbyComparison(target);
  InitLobbyDistanceFilter(target);
  InitLobbyType(target);
  InitResult(target);

  SET_FUNCTION("createLobby", CreateLobby);
  SET_FUNCTION("deleteLobbyData", DeleteLobbyData);
  SET_FUNCTION("getLobbyByIndex", GetLobbyByIndex);
  SET_FUNCTION("getLobbyData", GetLobbyData);
  SET_FUNCTION("getLobbyMemberByIndex", GetLobbyMemberByIndex);
  SET_FUNCTION("getNumLobbyMembers", GetNumLobbyMembers);
  SET_FUNCTION("getLobbyOwner", GetLobbyOwner);
  SET_FUNCTION("inviteUserToLobby", InviteUserToLobby);
  SET_FUNCTION("joinLobby", JoinLobby);
  SET_FUNCTION("leaveLobby", LeaveLobby);
  SET_FUNCTION("setLobbyData", SetLobbyData);
  SET_FUNCTION("setLobbyJoinable", SetLobbyJoinable);
  SET_FUNCTION("setLobbyOwner", SetLobbyOwner);
  SET_FUNCTION("setLobbyType", SetLobbyType);
}

SteamAPIRegistry::Add X(RegisterAPIs);

}  // namespace
}  // namespace api
}  // namespace greenworks
