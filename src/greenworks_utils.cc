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

void InitUgcMatchingTypes(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> ugc_matching_type = Nan::New<v8::Object>();
  ugc_matching_type->Set(Nan::New("Items").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_Items));
  ugc_matching_type->Set(Nan::New("ItemsMtx").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_Items_Mtx));
  ugc_matching_type->Set(Nan::New("ItemsReadyToUse").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_Items_ReadyToUse));
  ugc_matching_type->Set(Nan::New("Collections").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_Collections));
  ugc_matching_type->Set(Nan::New("Artwork").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_Artwork));
  ugc_matching_type->Set(Nan::New("Videos").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_Videos));
  ugc_matching_type->Set(Nan::New("Screenshots").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_Screenshots));
  ugc_matching_type->Set(Nan::New("AllGuides").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_AllGuides));
  ugc_matching_type->Set(Nan::New("WebGuides").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_WebGuides));
  ugc_matching_type->Set(Nan::New("IntegratedGuides").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_IntegratedGuides));
  ugc_matching_type->Set(Nan::New("UsableInGame").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_UsableInGame));
  ugc_matching_type->Set(Nan::New("ControllerBindings").ToLocalChecked(),
                         Nan::New(k_EUGCMatchingUGCType_ControllerBindings));
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_matching_type);
  Nan::Set(exports,
           Nan::New("UGCMatchingType").ToLocalChecked(),
           ugc_matching_type);
}

void InitUgcQueryTypes(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> ugc_query_type = Nan::New<v8::Object>();
  ugc_query_type->Set(Nan::New("RankedByVote").ToLocalChecked(),
                      Nan::New(k_EUGCQuery_RankedByVote));
  ugc_query_type->Set(Nan::New("RankedByPublicationDate").ToLocalChecked(),
                      Nan::New(k_EUGCQuery_RankedByPublicationDate));
  ugc_query_type->Set(
      Nan::New("AcceptedForGameRankedByAcceptanceDate").ToLocalChecked(),
      Nan::New(k_EUGCQuery_AcceptedForGameRankedByAcceptanceDate));
  ugc_query_type->Set(Nan::New("RankedByTrend").ToLocalChecked(),
                      Nan::New(k_EUGCQuery_RankedByTrend));
  ugc_query_type->Set(
      Nan::New("FavoritedByFriendsRankedByPublicationDate").ToLocalChecked(),
      Nan::New(k_EUGCQuery_FavoritedByFriendsRankedByPublicationDate));
  ugc_query_type->Set(
      Nan::New("CreatedByFriendsRankedByPublicationDate").ToLocalChecked(),
      Nan::New(k_EUGCQuery_CreatedByFriendsRankedByPublicationDate));
  ugc_query_type->Set(
      Nan::New("RankedByNumTimesReported").ToLocalChecked(),
      Nan::New(k_EUGCQuery_RankedByNumTimesReported));
  ugc_query_type->Set(
      Nan::New("CreatedByFollowedUsersRankedByPublicationDate")
          .ToLocalChecked(),
      Nan::New(k_EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate));
  ugc_query_type->Set(Nan::New("NotYetRated").ToLocalChecked(),
                      Nan::New(k_EUGCQuery_NotYetRated));
  ugc_query_type->Set(Nan::New("RankedByTotalVotesAsc").ToLocalChecked(),
                      Nan::New(k_EUGCQuery_RankedByTotalVotesAsc));
  ugc_query_type->Set(Nan::New("RankedByVotesUp").ToLocalChecked(),
                      Nan::New(k_EUGCQuery_RankedByVotesUp));
  ugc_query_type->Set(Nan::New("RankedByTextSearch").ToLocalChecked(),
                      Nan::New(k_EUGCQuery_RankedByTextSearch));

  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_query_type);
  Nan::Set(exports,
           Nan::New("UGCQueryType").ToLocalChecked(),
           ugc_query_type);
}

void InitUserUgcList(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> ugc_list = Nan::New<v8::Object>();
  ugc_list->Set(Nan::New("Published").ToLocalChecked(),
                Nan::New(k_EUserUGCList_Published));
  ugc_list->Set(Nan::New("VotedOn").ToLocalChecked(),
                Nan::New(k_EUserUGCList_VotedOn));
  ugc_list->Set(Nan::New("VotedUp").ToLocalChecked(),
                Nan::New(k_EUserUGCList_VotedUp));
  ugc_list->Set(Nan::New("VotedDown").ToLocalChecked(),
                Nan::New(k_EUserUGCList_VotedDown));
  ugc_list->Set(Nan::New("WillVoteLater").ToLocalChecked(),
                Nan::New(k_EUserUGCList_WillVoteLater));
  ugc_list->Set(Nan::New("Favorited").ToLocalChecked(),
                Nan::New(k_EUserUGCList_Favorited));
  ugc_list->Set(Nan::New("Subscribed").ToLocalChecked(),
                Nan::New(k_EUserUGCList_Subscribed));
  ugc_list->Set(Nan::New("UsedOrPlayer").ToLocalChecked(),
                Nan::New(k_EUserUGCList_UsedOrPlayed));
  ugc_list->Set(Nan::New("Followed").ToLocalChecked(),
                Nan::New(k_EUserUGCList_Followed));
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_list);
  Nan::Set(exports, Nan::New("UserUGCList").ToLocalChecked(), ugc_list);
}

void InitUserUgcListSortOrder(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> ugc_list_sort_order = Nan::New<v8::Object>();
  ugc_list_sort_order->Set(Nan::New("CreationOrderDesc").ToLocalChecked(),
      Nan::New(static_cast<int>(k_EUserUGCListSortOrder_CreationOrderDesc)));
  ugc_list_sort_order->Set(Nan::New("CreationOrderAsc").ToLocalChecked(),
      Nan::New(k_EUserUGCListSortOrder_CreationOrderDesc));
  ugc_list_sort_order->Set(Nan::New("TitleAsc").ToLocalChecked(),
      Nan::New(k_EUserUGCListSortOrder_TitleAsc));
  ugc_list_sort_order->Set(Nan::New("LastUpdatedDesc").ToLocalChecked(),
      Nan::New(k_EUserUGCListSortOrder_LastUpdatedDesc));
  ugc_list_sort_order->Set(Nan::New("SubscriptionDateDesc").ToLocalChecked(),
      Nan::New(k_EUserUGCListSortOrder_SubscriptionDateDesc));
  ugc_list_sort_order->Set(Nan::New("VoteScoreDesc").ToLocalChecked(),
      Nan::New(k_EUserUGCListSortOrder_VoteScoreDesc));
  ugc_list_sort_order->Set(Nan::New("ForModeration").ToLocalChecked(),
      Nan::New(k_EUserUGCListSortOrder_ForModeration));
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(ugc_list_sort_order);
  Nan::Set(exports,
           Nan::New("UserUGCListSortOrder").ToLocalChecked(),
           ugc_list_sort_order);
}

void InitFriendFlags(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> friend_flags = Nan::New<v8::Object>();
  friend_flags->Set(Nan::New("None").ToLocalChecked(),
                         Nan::New(k_EFriendFlagNone));
  friend_flags->Set(Nan::New("Blocked").ToLocalChecked(),
                         Nan::New(k_EFriendFlagBlocked));
  friend_flags->Set(Nan::New("FriendshipRequested").ToLocalChecked(),
                         Nan::New(k_EFriendFlagFriendshipRequested));
  friend_flags->Set(Nan::New("Immediate").ToLocalChecked(),
                         Nan::New(k_EFriendFlagImmediate));
  friend_flags->Set(Nan::New("ClanMember").ToLocalChecked(),
                         Nan::New(k_EFriendFlagClanMember));
  friend_flags->Set(Nan::New("OnGameServer").ToLocalChecked(),
                         Nan::New(k_EFriendFlagOnGameServer));
  friend_flags->Set(Nan::New("RequestingFriendship").ToLocalChecked(),
                         Nan::New(k_EFriendFlagRequestingFriendship));
  friend_flags->Set(Nan::New("RequestingInfo").ToLocalChecked(),
                         Nan::New(k_EFriendFlagRequestingInfo));
  friend_flags->Set(Nan::New("Ignored").ToLocalChecked(),
                         Nan::New(k_EFriendFlagIgnored));
  friend_flags->Set(Nan::New("IgnoredFriend").ToLocalChecked(),
                         Nan::New(k_EFriendFlagIgnoredFriend));
  friend_flags->Set(Nan::New("Suggested").ToLocalChecked(),
                         Nan::New(k_EFriendFlagSuggested));
  friend_flags->Set(Nan::New("ChatMember").ToLocalChecked(),
                         Nan::New(k_EFriendFlagChatMember));
  friend_flags->Set(Nan::New("All").ToLocalChecked(),
                         Nan::New(k_EFriendFlagAll));
  Nan::Persistent<v8::Object> constructor;
  constructor.Reset(friend_flags);
  Nan::Set(exports,
           Nan::New("FriendFlags").ToLocalChecked(),
           friend_flags);
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
