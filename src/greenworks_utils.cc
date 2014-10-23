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
  v8::Local<v8::Object> ugc_matching_type = NanNew<v8::Object>();
  ugc_matching_type->Set(NanNew("Items"), NanNew(k_EUGCMatchingUGCType_Items));
  ugc_matching_type->Set(NanNew("ItemsMtx"),
                         NanNew(k_EUGCMatchingUGCType_Items_Mtx));
  ugc_matching_type->Set(NanNew("ItemsReadyToUse"),
                         NanNew(k_EUGCMatchingUGCType_Items_ReadyToUse));
  ugc_matching_type->Set(NanNew("Collections"),
                         NanNew(k_EUGCMatchingUGCType_Collections));
  ugc_matching_type->Set(NanNew("Artwork"),
                         NanNew(k_EUGCMatchingUGCType_Artwork));
  ugc_matching_type->Set(NanNew("Videos"),
                         NanNew(k_EUGCMatchingUGCType_Videos));
  ugc_matching_type->Set(NanNew("Screenshots"),
                         NanNew(k_EUGCMatchingUGCType_Screenshots));
  ugc_matching_type->Set(NanNew("AllGuides"),
                         NanNew(k_EUGCMatchingUGCType_AllGuides));
  ugc_matching_type->Set(NanNew("WebGuides"),
                         NanNew(k_EUGCMatchingUGCType_WebGuides));
  ugc_matching_type->Set(NanNew("IntegratedGuides"),
                         NanNew(k_EUGCMatchingUGCType_IntegratedGuides));
  ugc_matching_type->Set(NanNew("UsableInGame"),
                         NanNew(k_EUGCMatchingUGCType_UsableInGame));
  ugc_matching_type->Set(NanNew("ControllerBindings"),
                         NanNew(k_EUGCMatchingUGCType_ControllerBindings));
  v8::Persistent<v8::Object> constructor;
  NanAssignPersistent(constructor, ugc_matching_type);
  exports->Set(NanNew("UGCMatchingType"), ugc_matching_type);
}

void InitUgcQueryTypes(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> ugc_query_type = NanNew<v8::Object>();
  ugc_query_type->Set(NanNew("RankedByVote"),
                      NanNew(k_EUGCQuery_RankedByVote));
  ugc_query_type->Set(NanNew("RankedByPublicationDate"),
                      NanNew(k_EUGCQuery_RankedByPublicationDate));
  ugc_query_type->Set(NanNew("AcceptedForGameRankedByAcceptanceDate"),
      NanNew(k_EUGCQuery_AcceptedForGameRankedByAcceptanceDate));
  ugc_query_type->Set(NanNew("RankedByTrend"),
                      NanNew(k_EUGCQuery_RankedByTrend));
  ugc_query_type->Set(NanNew("FavoritedByFriendsRankedByPublicationDate"),
      NanNew(k_EUGCQuery_FavoritedByFriendsRankedByPublicationDate));
  ugc_query_type->Set(NanNew("CreatedByFriendsRankedByPublicationDate"),
      NanNew(k_EUGCQuery_CreatedByFriendsRankedByPublicationDate));
  ugc_query_type->Set(NanNew("RankedByNumTimesReported"),
      NanNew(k_EUGCQuery_RankedByNumTimesReported));
  ugc_query_type->Set(NanNew("CreatedByFollowedUsersRankedByPublicationDate"),
      NanNew(k_EUGCQuery_CreatedByFollowedUsersRankedByPublicationDate));
  ugc_query_type->Set(NanNew("NotYetRated"), NanNew(k_EUGCQuery_NotYetRated));
  ugc_query_type->Set(NanNew("RankedByTotalVotesAsc"),
                      NanNew(k_EUGCQuery_RankedByTotalVotesAsc));
  ugc_query_type->Set(NanNew("RankedByVotesUp"),
                      NanNew(k_EUGCQuery_RankedByVotesUp));
  ugc_query_type->Set(NanNew("RankedByTextSearch"),
                      NanNew(k_EUGCQuery_RankedByTextSearch));

  v8::Persistent<v8::Object> constructor;
  NanAssignPersistent(constructor, ugc_query_type);
  exports->Set(NanNew("UGCQueryType"), ugc_query_type);
}

void InitUserUgcList(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> ugc_list = NanNew<v8::Object>();
  ugc_list->Set(NanNew("Published"), NanNew(k_EUserUGCList_Published));
  ugc_list->Set(NanNew("VotedOn"), NanNew(k_EUserUGCList_VotedOn));
  ugc_list->Set(NanNew("VotedUp"), NanNew(k_EUserUGCList_VotedUp));
  ugc_list->Set(NanNew("VotedDown"), NanNew(k_EUserUGCList_VotedDown));
  ugc_list->Set(NanNew("WillVoteLater"), NanNew(k_EUserUGCList_WillVoteLater));
  ugc_list->Set(NanNew("Favorited"), NanNew(k_EUserUGCList_Favorited));
  ugc_list->Set(NanNew("Subscribed"), NanNew(k_EUserUGCList_Subscribed));
  ugc_list->Set(NanNew("UsedOrPlayer"), NanNew(k_EUserUGCList_UsedOrPlayed));
  ugc_list->Set(NanNew("Followed"), NanNew(k_EUserUGCList_Followed));
  v8::Persistent<v8::Object> constructor;
  NanAssignPersistent(constructor, ugc_list);
  exports->Set(NanNew("UserUGCList"), ugc_list);
}

void InitUserUgcListSortOrder(v8::Handle<v8::Object> exports) {
  v8::Local<v8::Object> ugc_list_sort_order = NanNew<v8::Object>();
  ugc_list_sort_order->Set(NanNew("CreationOrderDesc"),
      NanNew((int)k_EUserUGCListSortOrder_CreationOrderDesc));
  ugc_list_sort_order->Set(NanNew("CreationOrderAsc"),
      NanNew(k_EUserUGCListSortOrder_CreationOrderDesc));
  ugc_list_sort_order->Set(NanNew("TitleAsc"),
      NanNew(k_EUserUGCListSortOrder_TitleAsc));
  ugc_list_sort_order->Set(NanNew("LastUpdatedDesc"),
      NanNew(k_EUserUGCListSortOrder_LastUpdatedDesc));
  ugc_list_sort_order->Set(NanNew("SubscriptionDateDesc"),
      NanNew(k_EUserUGCListSortOrder_SubscriptionDateDesc));
  ugc_list_sort_order->Set(NanNew("VoteScoreDesc"),
      NanNew(k_EUserUGCListSortOrder_VoteScoreDesc));
  ugc_list_sort_order->Set(NanNew("ForModeration"),
      NanNew(k_EUserUGCListSortOrder_ForModeration));
  v8::Persistent<v8::Object> constructor;
  NanAssignPersistent(constructor, ugc_list_sort_order);
  exports->Set(NanNew("UserUGCListSortOrder"), ugc_list_sort_order);
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
