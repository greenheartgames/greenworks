// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_utils.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "nan.h"
#include "steam/steam_api.h"

#if defined(_WIN32)
#include <sys/utime.h>
#include <windows.h>
#else
#include <unistd.h>
#include <utime.h>
#endif

namespace utils {

#define SET_TYPE(obj, type_name, type) \
  obj->Set(Nan::New(type_name).ToLocalChecked(), \
           Nan::New(type))

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

void InitFriendFlags(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> friend_flags = Nan::New<v8::Object>();
  SET_TYPE(friend_flags, "None", k_EFriendFlagNone);
  SET_TYPE(friend_flags, "Blocked", k_EFriendFlagBlocked);
  SET_TYPE(friend_flags, "FriendshipRequested",
           k_EFriendFlagFriendshipRequested);
  SET_TYPE(friend_flags, "Immediate", k_EFriendFlagImmediate);
  SET_TYPE(friend_flags, "ClanMember", k_EFriendFlagClanMember);
  SET_TYPE(friend_flags, "OnGameServer", k_EFriendFlagOnGameServer);
  SET_TYPE(friend_flags, "RequestingFriendship",
           k_EFriendFlagRequestingFriendship);
  SET_TYPE(friend_flags, "RequestingInfo", k_EFriendFlagRequestingInfo);
  SET_TYPE(friend_flags, "Ignored", k_EFriendFlagIgnored);
  SET_TYPE(friend_flags, "IgnoredFriend", k_EFriendFlagIgnoredFriend);
  SET_TYPE(friend_flags, "Suggested", k_EFriendFlagSuggested);
  SET_TYPE(friend_flags, "ChatMember", k_EFriendFlagChatMember);
  SET_TYPE(friend_flags, "All", k_EFriendFlagAll);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(friend_flags);
  Nan::Set(exports,
           Nan::New("FriendFlags").ToLocalChecked(),
           friend_flags);
}

void InitFriendRelationship(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> relationship = Nan::New<v8::Object>();
  SET_TYPE(relationship, "None", k_EFriendRelationshipNone);
  SET_TYPE(relationship, "Blocked", k_EFriendRelationshipBlocked);
  SET_TYPE(relationship, "RequestRecipient",
           k_EFriendRelationshipRequestRecipient);
  SET_TYPE(relationship, "Friend", k_EFriendRelationshipFriend);
  SET_TYPE(relationship, "RequestInitiator",
           k_EFriendRelationshipRequestInitiator);
  SET_TYPE(relationship, "Ignored", k_EFriendRelationshipIgnored);
  SET_TYPE(relationship, "IgnoredFriend", k_EFriendRelationshipIgnoredFriend);
  SET_TYPE(relationship, "Suggested", k_EFriendRelationshipSuggested);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(relationship);
  Nan::Set(exports,
           Nan::New("FriendRelationship").ToLocalChecked(),
           relationship);
}

void InitAccountType(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> account_type = Nan::New<v8::Object>();
  SET_TYPE(account_type, "Invalid", k_EAccountTypeInvalid);
  SET_TYPE(account_type, "Individual", k_EAccountTypeIndividual);
  SET_TYPE(account_type, "Multiseat", k_EAccountTypeMultiseat);
  SET_TYPE(account_type, "GameServer", k_EAccountTypeGameServer);
  SET_TYPE(account_type, "AnonymousGameServer", k_EAccountTypeAnonGameServer);
  SET_TYPE(account_type, "Pending", k_EAccountTypePending);
  SET_TYPE(account_type, "ContentServer", k_EAccountTypeContentServer);
  SET_TYPE(account_type, "Clan", k_EAccountTypeClan);
  SET_TYPE(account_type, "Chat", k_EAccountTypeChat);
  SET_TYPE(account_type, "ConsoleUser", k_EAccountTypeConsoleUser);
  SET_TYPE(account_type, "AnonymousUser", k_EAccountTypeAnonUser);
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(account_type);
  Nan::Set(exports,
           Nan::New("AccountType").ToLocalChecked(),
           account_type);
}

void sleep(int milliseconds) {
#if defined(_WIN32)
  Sleep(milliseconds);
#else
  usleep(milliseconds*1000);
#endif
}

bool ReadFile(const char* path, char* &content, int& length) {
  std::ifstream fin(path, std::ios::in|std::ios::binary|std::ios::ate);
  if (!fin.is_open()) {
    return false;
  }
  length = static_cast<int>(fin.tellg());
  content = new char[length];
  fin.seekg(0, std::ios::beg);
  fin.read(content, length);
  return true;
}

bool WriteFile(const std::string& target_path, char* content, int length) {
  std::ofstream fout(target_path.c_str(), std::ios::binary);
  fout.write(content, length);
  return fout.good();
}

std::string GetFileNameFromPath(const std::string& file_path) {
  size_t pos = file_path.find_last_of("/\\");
  if (pos == std::string::npos)
    return file_path;
  return file_path.substr(pos + 1);
}

bool UpdateFileLastUpdatedTime(const char* file_path, time_t time) {
  utimbuf utime_buf;
  utime_buf.actime = time;
  utime_buf.modtime = time;
  return utime(file_path, &utime_buf) == 0;
}

int64 GetFileLastUpdatedTime(const char* file_path) {
    struct stat st;
  if (stat(file_path, &st))
    return -1;
  return st.st_mtime;
}

std::string uint64ToString(uint64 value) {
  std::ostringstream sout;
  sout << value;
  return sout.str();
}

uint64 strToUint64(std::string str) {
  std::stringstream sin(str);
  uint64 result;
  sin >> result;
  return result;
}

}  // namespace utils
