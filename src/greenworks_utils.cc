// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "greenworks_utils.h"

#include <iostream>
#include <fstream>

#include "nan.h"

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

}  // namespace utils
