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
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace utils {

void InitUtilsObject(v8::Handle<v8::Object> exports) {
  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>();
  v8::Persistent<v8::Function> constructor;
  NanAssignPersistent(constructor, tpl->GetFunction());
  exports->Set(NanNew("Utils"), tpl->GetFunction());
}

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
  std::streampos size = fin.tellg();
  content = new char[size];
  fin.seekg(0, std::ios::beg);
  fin.read(content, size);
  length = size;
  return true;
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
