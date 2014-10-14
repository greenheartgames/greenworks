// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_GREENWORKS_UTILS_H_
#define SRC_GREENWORKS_UTILS_H_

#include <string>

#include "node.h"
#include "steam/steamtypes.h"
#include "v8.h"

namespace utils {

void InitUtilsObject(v8::Handle<v8::Object> exports);

void InitUgcQueryTypes(v8::Handle<v8::Object> exports);

void InitUgcMatchingTypes(v8::Handle<v8::Object> exports);

void sleep(int milliseconds);

bool ReadFile(const char* path, char* &content, int& length);

std::string uint64ToString(uint64 value);

uint64 strToUint64(std::string);

}  // namespace utils

#endif  // SRC_GREENWORKS_UTILS_H_
