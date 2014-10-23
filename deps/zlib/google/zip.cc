// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/zlib/google/zip.h"

#include <string>
#include <vector>

#include "base/bind.h"
#include "base/file_util.h"
#include "base/files/file_enumerator.h"
#include "base/logging.h"
#include "base/strings/string16.h"
#include "base/strings/string_util.h"
#include "net/base/file_stream.h"
#include "third_party/zlib/google/zip_internal.h"
#include "third_party/zlib/google/zip_reader.h"

#if defined(USE_SYSTEM_MINIZIP)
#include <minizip/unzip.h>
#include <minizip/zip.h>
#else
#include "third_party/zlib/contrib/minizip/unzip.h"
#include "third_party/zlib/contrib/minizip/zip.h"
#endif

namespace {

// Returns a zip_fileinfo struct with the time represented by |file_time|.
zip_fileinfo TimeToZipFileInfo(const base::Time& file_time) {
  base::Time::Exploded file_time_parts;
  file_time.LocalExplode(&file_time_parts);

  zip_fileinfo zip_info = {};
  if (file_time_parts.year >= 1980) {
    // This if check works around the handling of the year value in
    // contrib/minizip/zip.c in function zip64local_TmzDateToDosDate
    // It assumes that dates below 1980 are in the double digit format.
    // Hence the fail safe option is to leave the date unset. Some programs
    // might show the unset date as 1980-0-0 which is invalid.
    zip_info.tmz_date.tm_year = file_time_parts.year;
    zip_info.tmz_date.tm_mon = file_time_parts.month - 1;
    zip_info.tmz_date.tm_mday = file_time_parts.day_of_month;
    zip_info.tmz_date.tm_hour = file_time_parts.hour;
    zip_info.tmz_date.tm_min = file_time_parts.minute;
    zip_info.tmz_date.tm_sec = file_time_parts.second;
  }

  return zip_info;
}

// Returns a zip_fileinfo with the last modification date of |path| set.
zip_fileinfo GetFileInfoForZipping(const base::FilePath& path) {
  base::Time file_time;
  base::File::Info file_info;
  if (base::GetFileInfo(path, &file_info))
    file_time = file_info.last_modified;
  return TimeToZipFileInfo(file_time);
}

bool AddFileToZip(zipFile zip_file, const base::FilePath& src_dir) {
  net::FileStream stream(NULL);
  int flags = base::PLATFORM_FILE_OPEN | base::PLATFORM_FILE_READ;
  if (stream.OpenSync(src_dir, flags) != 0) {
    DLOG(ERROR) << "Could not open stream for path "
                << src_dir.value();
    return false;
  }

  int num_bytes;
  char buf[zip::internal::kZipBufSize];
  do {
    num_bytes = stream.ReadSync(buf, zip::internal::kZipBufSize);
    if (num_bytes > 0) {
      if (ZIP_OK != zipWriteInFileInZip(zip_file, buf, num_bytes)) {
        DLOG(ERROR) << "Could not write data to zip for path "
                    << src_dir.value();
        return false;
      }
    }
  } while (num_bytes > 0);

  return true;
}

bool AddEntryToZip(zipFile zip_file, const base::FilePath& path,
                   const base::FilePath& root_path) {
  base::FilePath relative_path;
  bool result = root_path.AppendRelativePath(path, &relative_path);
  DCHECK(result);
  std::string str_path = relative_path.AsUTF8Unsafe();
#if defined(OS_WIN)
  ReplaceSubstringsAfterOffset(&str_path, 0u, "\\", "/");
#endif

  bool is_directory = base::DirectoryExists(path);
  if (is_directory)
    str_path += "/";

  // Section 4.4.4 http://www.pkware.com/documents/casestudies/APPNOTE.TXT
  // Setting the Language encoding flag so the file is told to be in utf-8.
  const uLong LANGUAGE_ENCODING_FLAG = 0x1 << 11;

  zip_fileinfo file_info = GetFileInfoForZipping(path);

  if (ZIP_OK != zipOpenNewFileInZip4(
                    zip_file,  // file
                    str_path.c_str(),  // filename
                    &file_info,  // zipfi
                    NULL,  // extrafield_local,
                    0u,  // size_extrafield_local
                    NULL,  // extrafield_global
                    0u,  // size_extrafield_global
                    NULL,  // comment
                    Z_DEFLATED,  // method
                    Z_DEFAULT_COMPRESSION,  // level
                    0,  // raw
                    -MAX_WBITS,  // windowBits
                    DEF_MEM_LEVEL,  // memLevel
                    Z_DEFAULT_STRATEGY,  // strategy
                    NULL,  // password
                    0,  // crcForCrypting
                    0,  // versionMadeBy
                    LANGUAGE_ENCODING_FLAG)) {  // flagBase
    DLOG(ERROR) << "Could not open zip file entry " << str_path;
    return false;
  }

  bool success = true;
  if (!is_directory) {
    success = AddFileToZip(zip_file, path);
  }

  if (ZIP_OK != zipCloseFileInZip(zip_file)) {
    DLOG(ERROR) << "Could not close zip file entry " << str_path;
    return false;
  }

  return success;
}

bool ExcludeNoFilesFilter(const base::FilePath& file_path) {
  return true;
}

bool ExcludeHiddenFilesFilter(const base::FilePath& file_path) {
  return file_path.BaseName().value()[0] != '.';
}

}  // namespace

namespace zip {

bool Unzip(const base::FilePath& src_file, const base::FilePath& dest_dir) {
  ZipReader reader;
  if (!reader.Open(src_file)) {
    DLOG(WARNING) << "Failed to open " << src_file.value();
    return false;
  }
  while (reader.HasMore()) {
    if (!reader.OpenCurrentEntryInZip()) {
      DLOG(WARNING) << "Failed to open the current file in zip";
      return false;
    }
    if (reader.current_entry_info()->is_unsafe()) {
      DLOG(WARNING) << "Found an unsafe file in zip "
                    << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.ExtractCurrentEntryIntoDirectory(dest_dir)) {
      DLOG(WARNING) << "Failed to extract "
                    << reader.current_entry_info()->file_path().value();
      return false;
    }
    if (!reader.AdvanceToNextEntry()) {
      DLOG(WARNING) << "Failed to advance to the next file";
      return false;
    }
  }
  return true;
}

bool ZipWithFilterCallback(const base::FilePath& src_dir,
                           const base::FilePath& dest_file,
                           const FilterCallback& filter_cb) {
  DCHECK(base::DirectoryExists(src_dir));

  zipFile zip_file = internal::OpenForZipping(dest_file.AsUTF8Unsafe(),
                                              APPEND_STATUS_CREATE);

  if (!zip_file) {
    DLOG(WARNING) << "couldn't create file " << dest_file.value();
    return false;
  }

  bool success = true;
  base::FileEnumerator file_enumerator(src_dir, true /* recursive */,
      base::FileEnumerator::FILES | base::FileEnumerator::DIRECTORIES);
  for (base::FilePath path = file_enumerator.Next(); !path.value().empty();
       path = file_enumerator.Next()) {
    if (!filter_cb.Run(path)) {
      continue;
    }

    if (!AddEntryToZip(zip_file, path, src_dir)) {
      success = false;
      return false;
    }
  }

  if (ZIP_OK != zipClose(zip_file, NULL)) {
    DLOG(ERROR) << "Error closing zip file " << dest_file.value();
    return false;
  }

  return success;
}

bool Zip(const base::FilePath& src_dir, const base::FilePath& dest_file,
         bool include_hidden_files) {
  if (include_hidden_files) {
    return ZipWithFilterCallback(
        src_dir, dest_file, base::Bind(&ExcludeNoFilesFilter));
  } else {
    return ZipWithFilterCallback(
        src_dir, dest_file, base::Bind(&ExcludeHiddenFilesFilter));
  }
}

#if defined(OS_POSIX)
bool ZipFiles(const base::FilePath& src_dir,
              const std::vector<base::FilePath>& src_relative_paths,
              int dest_fd) {
  DCHECK(base::DirectoryExists(src_dir));
  zipFile zip_file = internal::OpenFdForZipping(dest_fd, APPEND_STATUS_CREATE);

  if (!zip_file) {
    DLOG(ERROR) << "couldn't create file for fd " << dest_fd;
    return false;
  }

  bool success = true;
  for (std::vector<base::FilePath>::const_iterator iter =
           src_relative_paths.begin();
      iter != src_relative_paths.end(); ++iter) {
    const base::FilePath& path = src_dir.Append(*iter);
    if (!AddEntryToZip(zip_file, path, src_dir)) {
      // TODO(hshi): clean up the partial zip file when error occurs.
      success = false;
      break;
    }
  }

  if (ZIP_OK != zipClose(zip_file, NULL)) {
    DLOG(ERROR) << "Error closing zip file for fd " << dest_fd;
    success = false;
  }

  return success;
}
#endif  // defined(OS_POSIX)

}  // namespace zip
