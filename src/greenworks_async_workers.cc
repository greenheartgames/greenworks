#include "greenworks_async_workers.h"

#include "nan.h"
#include "steam/steam_api.h"
#include "v8.h"

namespace greenworks {

FileSaveWorker::FileSaveWorker(NanCallback* success_callback,
    NanCallback* error_callback, std::string file_name, std::string content):
        SteamAsyncWorker(success_callback, error_callback),
        file_name_(file_name),
        content_(content) {
}

void FileSaveWorker::Execute() {
  ISteamRemoteStorage* steam_remote_storage = SteamRemoteStorage();

  // Checking quota (in the future we may need it)
  int nTotal = -1, nAvailable = -1;

  if (!steam_remote_storage->GetQuota(&nTotal, &nAvailable)) {
    SetErrorMessage("Error on getting Cloud quota");
    return;
  }

  bool success = steam_remote_storage ->FileWrite(
      file_name_.c_str(), content_.c_str(), content_.size());

  if (!success)
    SetErrorMessage("Error on writing to file.");

  return;
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

}  // namespace greenworks
