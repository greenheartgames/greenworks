// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SRC_GREENWORKS_UTILS_H_
#define SRC_GREENWORKS_UTILS_H_

#include <string>

#include "steam/steamtypes.h"

namespace utils {

void sleep(int milliseconds);

bool ReadFile(const char* path, char** content, int* length);

bool WriteFile(const std::string& target_path, char* content, int length);

std::string GetFileNameFromPath(const std::string& file_path);

bool UpdateFileLastUpdatedTime(const char* file_path, time_t time);

int64 GetFileLastUpdatedTime(const char* file_path);

std::string uint64ToString(uint64 value);

uint64 strToUint64(std::string);

}  // namespace utils

#endif  // SRC_GREENWORKS_UTILS_H_
