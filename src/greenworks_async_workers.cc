// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_async_workers.h"

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

namespace {

struct FilesContentContainer {
  std::vector<char*> files_content;
  ~FilesContentContainer() {
    for (size_t i = 0; i < files_content.size(); ++i) {
      delete files_content[i];
    }
  }
};

};

namespace greenworks {

FileContentSaveWorker::FileContentSaveWorker(NanCallback* success_callback,
    NanCallback* error_callback, std::string file_name, std::string content):
        SteamAsyncWorker(success_callback, error_callback),
        file_name_(file_name),
        content_(content) {
}

void FileContentSaveWorker::Execute() {
  if (!SteamRemoteStorage()->FileWrite(
      file_name_.c_str(), content_.c_str(), content_.size()))
    SetErrorMessage("Error on writing to file.");
}

FilesSaveWorker::FilesSaveWorker(NanCallback* success_callback,
    NanCallback* error_callback, const std::vector<std::string>& files_path):
        SteamAsyncWorker(success_callback, error_callback),
        files_path_(files_path) {
}

void FilesSaveWorker::Execute() {
  FilesContentContainer container;
  std::vector<int> files_content_length;
  for (size_t i = 0; i < files_path_.size(); ++i) {
    char* content = NULL;
    int length = 0;
    if (!utils::ReadFile(files_path_[i].c_str(), content, length))
      break;
    container.files_content.push_back(content);
    files_content_length.push_back(length);
  }
  if (container.files_content.size() != files_path_.size()) {
    SetErrorMessage("Error on reading files.");
    return;
  }
  for (size_t i = 0; i < files_path_.size(); ++i) {
    if (!SteamRemoteStorage()->FileWrite(files_path_[i].c_str(),
        container.files_content[i], files_content_length[i])) {
      SetErrorMessage("Error on writing file on Steam Cloud.");
      return;
    }
  }
}

FileReadWorker::FileReadWorker(NanCallback* success_callback,
    NanCallback* error_callback, std::string file_name):
        SteamAsyncWorker(success_callback, error_callback),
        file_name_(file_name) {
}

void FileReadWorker::Execute() {
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();

  if (!steam_remote_storage->FileExists(file_name_.c_str())) {
    SetErrorMessage("File doesn't exist.");
    return;
  }

  int32 file_size = steam_remote_storage->GetFileSize(file_name_.c_str());

  char* content = new char[file_size+1];
  int32 end_pos = steam_remote_storage->FileRead(
      file_name_.c_str(), content, file_size);
  content[end_pos] = '\0';

  if (end_pos == 0 && file_size > 0) {
    SetErrorMessage("Error on reading file.");
  } else {
    content_ = std::string(content);
  }

  delete content;
}

void FileReadWorker::HandleOKCallback() {
  NanScope();

  v8::Local<v8::Value> argv[] = { NanNew<v8::String>(content_) };
  callback->Call(1, argv);
}

CloudQuotaGetWorker::CloudQuotaGetWorker(NanCallback* success_callback,
      NanCallback* error_callback):SteamAsyncWorker(success_callback,
          error_callback), total_bytes_(-1), available_bytes_(-1) {
}

void CloudQuotaGetWorker::Execute() {
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();

  if (!steam_remote_storage->GetQuota(&total_bytes_, &available_bytes_)) {
    SetErrorMessage("Error on getting cloud quota.");
    return;
  }
}

void CloudQuotaGetWorker::HandleOKCallback() {
  NanScope();
  v8::Local<v8::Value> argv[] = { NanNew<v8::Integer>(total_bytes_),
                                  NanNew<v8::Integer>(available_bytes_) };
  callback->Call(2, argv);
}


ActivateAchievementWorker::ActivateAchievementWorker(
    NanCallback* success_callback, NanCallback* error_callback,
    const std::string& achievement):
        SteamAsyncWorker(success_callback,
        error_callback), achievement_(achievement) {
}

void ActivateAchievementWorker::Execute() {
  ISteamUserStats* steam_user_stats = SteamUserStats();

  steam_user_stats->SetAchievement(achievement_.c_str());
  if (!steam_user_stats->StoreStats())
    SetErrorMessage("Error on storing user achievement");
}

GetNumberOfPlayersWorker::GetNumberOfPlayersWorker(
    NanCallback* success_callback, NanCallback* error_callback)
       :SteamCallbackAsyncWorker(success_callback, error_callback),
        num_of_players_(-1) {
}

void GetNumberOfPlayersWorker::Execute() {
  SteamAPICall_t steam_api_call = SteamUserStats()->GetNumberOfCurrentPlayers();
  call_result_.Set(steam_api_call, this,
      &GetNumberOfPlayersWorker::OnGetNumberOfPlayersCompleted);

  WaitForCompleted();
}

void GetNumberOfPlayersWorker::OnGetNumberOfPlayersCompleted(
    NumberOfCurrentPlayers_t* result, bool io_failure) {
  if (io_failure) {
    SetErrorMessage("Error on getting number of players: Steam API IO Failure");
  } else if (result->m_bSuccess) {
    num_of_players_ = result->m_cPlayers;
  } else {
    SetErrorMessage("Error on getting number of players.");
  }
  is_completed_ = true;
}

void GetNumberOfPlayersWorker::HandleOKCallback() {
  NanScope();

  v8::Local<v8::Value> argv[] = { NanNew(num_of_players_) };
  callback->Call(1, argv);
}

}  // namespace greenworks
