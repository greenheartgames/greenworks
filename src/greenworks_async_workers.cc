// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_async_workers.h"

#include <sstream>
#include <iomanip>
#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

#include "greenworks_unzip.h"
#include "greenworks_zip.h"


namespace {

struct FilesContentContainer {
  std::vector<char*> files_content;
  ~FilesContentContainer() {
    for (size_t i = 0; i < files_content.size(); ++i) {
      delete[] files_content[i];
    }
  }
};

};  // namespace

namespace greenworks {

FileContentSaveWorker::FileContentSaveWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, std::string file_name, std::string content):
        SteamAsyncWorker(success_callback, error_callback),
        file_name_(file_name),
        content_(content) {
}

void FileContentSaveWorker::Execute() {
  if (!SteamRemoteStorage()->FileWrite(
      file_name_.c_str(), content_.c_str(), content_.size()))
    SetErrorMessage("Error on writing to file.");
}

FilesSaveWorker::FilesSaveWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, const std::vector<std::string>& files_path):
        SteamAsyncWorker(success_callback, error_callback),
        files_path_(files_path) {
}

void FilesSaveWorker::Execute() {
  FilesContentContainer container;
  std::vector<int> files_content_length;
  for (size_t i = 0; i < files_path_.size(); ++i) {
    char* content = nullptr;
    int length = 0;
    if (!utils::ReadFile(files_path_[i].c_str(), &content, &length))
      break;
    container.files_content.push_back(content);
    files_content_length.push_back(length);
  }
  if (container.files_content.size() != files_path_.size()) {
    SetErrorMessage("Error on reading files.");
    return;
  }
  for (size_t i = 0; i < files_path_.size(); ++i) {
    std::string file_name = utils::GetFileNameFromPath(files_path_[i]);
    if (!SteamRemoteStorage()->FileWrite(file_name.c_str(),
        container.files_content[i], files_content_length[i])) {
      SetErrorMessage("Error on writing file on Steam Cloud.");
      return;
    }
  }
}

FileDeleteWorker::FileDeleteWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, std::string file_name):
        SteamAsyncWorker(success_callback, error_callback),
        file_name_(file_name) {
}

void FileDeleteWorker::Execute() {
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();

  if (!steam_remote_storage->FileExists(file_name_.c_str())) {
    SetErrorMessage("File doesn't exist.");
    return;
  }

  if (!steam_remote_storage->FileDelete(file_name_.c_str())) {
    SetErrorMessage("Error on deleting file.");
    return;
  }
}

FileReadWorker::FileReadWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, std::string file_name):
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

  auto* content = new char[file_size+1];
  int32 end_pos = steam_remote_storage->FileRead(
      file_name_.c_str(), content, file_size);
  content[end_pos] = '\0';

  if (end_pos == 0 && file_size > 0) {
    SetErrorMessage("Error on reading file.");
  } else {
    content_ = std::string(content);
  }

  delete[] content;
}

void FileReadWorker::HandleOKCallback() {
  Nan::HandleScope scope;

  v8::Local<v8::Value> argv[] = { Nan::New(content_).ToLocalChecked() };
  callback->Call(1, argv);
}

CloudQuotaGetWorker::CloudQuotaGetWorker(Nan::Callback* success_callback,
      Nan::Callback* error_callback):SteamAsyncWorker(success_callback,
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
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::New(utils::uint64ToString(total_bytes_)).ToLocalChecked(),
      Nan::New(utils::uint64ToString(available_bytes_)).ToLocalChecked()};
  callback->Call(2, argv);
}

ActivateAchievementWorker::ActivateAchievementWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback,
    const std::string& achievement):
        SteamAsyncWorker(success_callback,
        error_callback), achievement_(achievement) {
}

void ActivateAchievementWorker::Execute() {
  ISteamUserStats* steam_user_stats = SteamUserStats();

  if (!steam_user_stats->SetAchievement(achievement_.c_str())) {
    SetErrorMessage("Achievement name is not valid.");
    return;
  }
  if (!steam_user_stats->StoreStats()) {
    SetErrorMessage("Error on storing user achievement");
    return;
  }
}

GetAchievementWorker::GetAchievementWorker(
    Nan::Callback* success_callback,
    Nan::Callback* error_callback,
    const std::string& achievement):
        SteamAsyncWorker(success_callback, error_callback),
        achievement_(achievement),
        is_achieved_(false) {
}

void GetAchievementWorker::Execute() {
  ISteamUserStats* steam_user_stats = SteamUserStats();
  bool success = steam_user_stats->GetAchievement(achievement_.c_str(),
                                                  &is_achieved_);
  if (!success) {
    SetErrorMessage("Achivement name is not valid.");
  }
}

void GetAchievementWorker::HandleOKCallback() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = { Nan::New(is_achieved_) };
  callback->Call(1, argv);
}

ClearAchievementWorker::ClearAchievementWorker(
    Nan::Callback* success_callback,
    Nan::Callback* error_callback,
    const std::string& achievement):
        SteamAsyncWorker(success_callback, error_callback),
        achievement_(achievement),
        success_(false) {
}

void ClearAchievementWorker::Execute() {
  ISteamUserStats* steam_user_stats = SteamUserStats();
  success_ = steam_user_stats->ClearAchievement(achievement_.c_str());
  if (!success_) {
    SetErrorMessage("Achievement name is not valid.");
    return;
  }
  if (!steam_user_stats->StoreStats()) {
    SetErrorMessage("Fails on uploading user stats to server.");
    return;
  }
}

GetNumberOfPlayersWorker::GetNumberOfPlayersWorker(
    Nan::Callback* success_callback, Nan::Callback* error_callback)
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
  Nan::HandleScope scope;

  v8::Local<v8::Value> argv[] = { Nan::New(num_of_players_) };
  callback->Call(1, argv);
}

CreateArchiveWorker::CreateArchiveWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, const std::string& zip_file_path,
    const std::string& source_dir, const std::string& password,
    int compress_level)
        :SteamAsyncWorker(success_callback, error_callback),
         zip_file_path_(zip_file_path),
         source_dir_(source_dir),
         password_(password),
         compress_level_(compress_level) {
}

void CreateArchiveWorker::Execute() {
  int result = zip(zip_file_path_.c_str(),
                   source_dir_.c_str(),
                   compress_level_,
                   password_.empty()?nullptr:password_.c_str());
  if (result)
    SetErrorMessage("Error on creating zip file.");
}

ExtractArchiveWorker::ExtractArchiveWorker(Nan::Callback* success_callback,
    Nan::Callback* error_callback, const std::string& zip_file_path,
    const std::string& extract_path, const std::string& password)
        : SteamAsyncWorker(success_callback, error_callback),
          zip_file_path_(zip_file_path),
          extract_path_(extract_path),
          password_(password) {
}

void ExtractArchiveWorker::Execute() {
  int result = unzip(zip_file_path_.c_str(), extract_path_.c_str(),
      password_.empty()?nullptr:password_.c_str());
  if (result)
    SetErrorMessage("Error on extracting zip file.");
}

GetAuthSessionTicketWorker::GetAuthSessionTicketWorker(
  Nan::Callback* success_callback,
  Nan::Callback* error_callback )
    : SteamCallbackAsyncWorker(success_callback, error_callback),
      result(this, &GetAuthSessionTicketWorker::OnGetAuthSessionCompleted),
      handle_(0), ticket_buf_size_(0) {
}

void GetAuthSessionTicketWorker::Execute() {
  handle_ = SteamUser()->GetAuthSessionTicket(ticket_buf_,
                                              sizeof(ticket_buf_),
                                              &ticket_buf_size_);
  WaitForCompleted();
}

void GetAuthSessionTicketWorker::OnGetAuthSessionCompleted(
    GetAuthSessionTicketResponse_t *inCallback) {
  if (inCallback->m_eResult != k_EResultOK)
    SetErrorMessage("Error on getting auth session ticket.");
  is_completed_ = true;
}

void GetAuthSessionTicketWorker::HandleOKCallback() {
  Nan::HandleScope scope;
  v8::Local<v8::Object> ticket = Nan::New<v8::Object>();
  Nan::Set(
      ticket, Nan::New("ticket").ToLocalChecked(),
      Nan::CopyBuffer(reinterpret_cast<char *>(ticket_buf_), ticket_buf_size_)
          .ToLocalChecked());
  Nan::Set(ticket, Nan::New("handle").ToLocalChecked(), Nan::New(handle_));
  v8::Local<v8::Value> argv[] = { ticket };
  callback->Call(1, argv);
}

RequestEncryptedAppTicketWorker::RequestEncryptedAppTicketWorker(
  std::string user_data,
  Nan::Callback* success_callback,
  Nan::Callback* error_callback)
    : SteamCallbackAsyncWorker(success_callback, error_callback),
      user_data_(user_data), ticket_buf_size_(0) {
}

void RequestEncryptedAppTicketWorker::Execute() {
  SteamAPICall_t steam_api_call = SteamUser()->RequestEncryptedAppTicket(
      static_cast<void*>(const_cast<char*>(user_data_.c_str())),
      user_data_.length());
  call_result_.Set(steam_api_call, this,
      &RequestEncryptedAppTicketWorker::OnRequestEncryptedAppTicketCompleted);
  WaitForCompleted();
}

void RequestEncryptedAppTicketWorker::OnRequestEncryptedAppTicketCompleted(
    EncryptedAppTicketResponse_t *inCallback, bool io_failure) {
  if (!io_failure && inCallback->m_eResult == k_EResultOK) {
    SteamUser()->GetEncryptedAppTicket(ticket_buf_, sizeof(ticket_buf_),
        &ticket_buf_size_);
  } else {
    SetErrorMessage("Error on getting encrypted app ticket.");
  }
  is_completed_ = true;
}

void RequestEncryptedAppTicketWorker::HandleOKCallback() {
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[] = {
      Nan::CopyBuffer(reinterpret_cast<char *>(ticket_buf_), ticket_buf_size_)
          .ToLocalChecked() };
  callback->Call(1, argv);
}

}  // namespace greenworks
