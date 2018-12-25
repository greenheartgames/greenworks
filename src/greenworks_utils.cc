// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

#if defined(_WIN32)
#include <sys/utime.h>
#include <windows.h>
#else
#include <unistd.h>
#include <utime.h>
#endif

namespace utils {

void sleep(int milliseconds) {
#if defined(_WIN32)
  Sleep(milliseconds);
#else
  usleep(milliseconds*1000);
#endif
}

bool ReadFile(const char* path, char** content, int* length) {
  std::ifstream fin(path, std::ios::in|std::ios::binary|std::ios::ate);
  if (!fin.is_open()) {
    return false;
  }
  *length = static_cast<int>(fin.tellg());
  *content = new char[*length];
  fin.seekg(0, std::ios::beg);
  fin.read(*content, *length);
  return true;
}

bool WriteFile(const std::string& target_path, char* content, int length) {
  std::ofstream fout(target_path.c_str(), std::ios::out|std::ios::binary);
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
