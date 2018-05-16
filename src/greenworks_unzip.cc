// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

/*
   miniunz.c
   Version 1.1, February 14h, 2010
   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications of Unzip for Zip64
         Copyright (C) 2007-2008 Even Rouault

         Modifications for Zip64 support on both zip and unzip
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/

#include "greenworks_unzip.h"

#include "zlib/contrib/minizip/unzip.h"
#include "zlib/zlib.h"

#ifndef _WIN32
  #ifndef __USE_FILE_OFFSET64
    #define __USE_FILE_OFFSET64
  #endif
  #ifndef __USE_LARGEFILE64
    #define __USE_LARGEFILE64
  #endif
  #ifndef _LARGEFILE64_SOURCE
    #define _LARGEFILE64_SOURCE
  #endif
  #ifndef _FILE_OFFSET_BIT
    #define _FILE_OFFSET_BIT 64
  #endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef _WIN32
  #include <direct.h>
  #include <io.h>
#else
  #include <unistd.h>
  #include <utime.h>
  #include <sys/stat.h>
  #include <sys/types.h>
#endif

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef _WIN32
#define USEWIN32IOAPI
#include "zlib/contrib/minizip/iowin32.h"
#endif

namespace {

/* change_file_date : change the date/time of a file
filename : the filename of the file where date/time must be modified
dosdate : the new date at the MSDos format (4 bytes)
tmu_date : the SAME new date at the tm_unz format */
void change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date) {
#ifdef _WIN32
  HANDLE hFile;
  FILETIME ftm, ftLocal, ftCreate, ftLastAcc, ftLastWrite;
  hFile = CreateFileA(filename, GENERIC_READ | GENERIC_WRITE,
    0, NULL, OPEN_EXISTING, 0, NULL);
  GetFileTime(hFile, &ftCreate, &ftLastAcc, &ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate >> 16), (WORD)dosdate, &ftLocal);
  LocalFileTimeToFileTime(&ftLocal, &ftm);
  SetFileTime(hFile, &ftm, &ftLastAcc, &ftm);
  CloseHandle(hFile);
#else
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min = tmu_date.tm_min;
  newdate.tm_hour = tmu_date.tm_hour;
  newdate.tm_mday = tmu_date.tm_mday;
  newdate.tm_mon = tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
    newdate.tm_year = tmu_date.tm_year - 1900;
  else
    newdate.tm_year = tmu_date.tm_year;
  newdate.tm_isdst = -1;

  ut.actime = ut.modtime = mktime(&newdate);
  utime(filename, &ut);
#endif
}

int mymkdir(const char* dirname) {
  int ret = 0;
#ifdef _WIN32
  ret = _mkdir(dirname);
#else
  ret = mkdir(dirname, 0775);
#endif
  return ret;
}

int makedir(const char *newdir) {
  char *buffer;
  char *p;
  auto len = (int)strlen(newdir);

  if (len <= 0)
    return 0;

  buffer = (char*)malloc(len + 1);
  if (buffer == nullptr)
    return UNZ_INTERNALERROR;
  strcpy(buffer, newdir);

  if (buffer[len - 1] == '/')
    buffer[len - 1] = '\0';

  if (mymkdir(buffer) == 0) {
    free(buffer);
    return 1;
  }

  p = buffer + 1;
  while (1) {
    char hold;

    while (*p && *p != '\\' && *p != '/')
      p++;
    hold = *p;
    *p = 0;
    if ((mymkdir(buffer) == -1) && (errno == ENOENT)) {
      free(buffer);
      return 0;
    }
    if (hold == 0)
      break;
    *p++ = hold;
  }
  free(buffer);
  return 1;
}

int do_extract_currentfile(unzFile uf, const int* popt_extract_without_path,
    int* popt_overwrite, const char* password) {
  char filename_inzip[256];
  char* filename_withoutpath;
  char* p;
  FILE *fout = nullptr;
  void* buf;

  unz_file_info64 file_info;
  int err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip,
      sizeof(filename_inzip), nullptr, 0, nullptr, 0);

  if (err != UNZ_OK)
    return err;

  uInt size_buf = WRITEBUFFERSIZE;
  buf = (void*)malloc(size_buf);
  if (buf == nullptr)
    return UNZ_INTERNALERROR;

  p = filename_withoutpath = filename_inzip;
  while ((*p) != '\0') {
    if (((*p) == '/') || ((*p) == '\\')) {
#ifndef _WIN32
      (*p) = '/';
#endif
      filename_withoutpath = p + 1;
    }
    p++;
  }

  if ((*filename_withoutpath) == '\0') {
    if ((*popt_extract_without_path) == 0)
      mymkdir(filename_inzip);
  }
  else {
    const char* write_filename;
    if ((*popt_extract_without_path) == 0)
      write_filename = filename_inzip;
    else
      write_filename = filename_withoutpath;

    err = unzOpenCurrentFilePassword(uf, password);
    if (err == UNZ_OK) {
      fout = fopen64(write_filename, "wb");

      /* some zipfile don't contain directory alone before file */
      if ((fout == nullptr) && ((*popt_extract_without_path) == 0) &&
        (filename_withoutpath != (char*)filename_inzip))
      {
        char c = *(filename_withoutpath - 1);
        *(filename_withoutpath - 1) = '\0';
        makedir(write_filename);
        *(filename_withoutpath - 1) = c;
        fout = fopen64(write_filename, "wb");
      }
    }

    if (fout != nullptr) {
      do {
        err = unzReadCurrentFile(uf, buf, size_buf);
        if (err<0)
          break;
        if (err>0)
          if (fwrite(buf, err, 1, fout) != 1) {
            err = UNZ_ERRNO;
            break;
          }
      } while (err>0);
      if (fout)
        fclose(fout);

      if (err == 0)
        change_file_date(write_filename, file_info.dosDate,
        file_info.tmu_date);
    }

    if (err == UNZ_OK)
      err = unzCloseCurrentFile(uf);
    else
      unzCloseCurrentFile(uf); /* don't lose the error */
  }

  free(buf);
  return err;
}

int do_extract(unzFile uf, int opt_extract_without_path,
    int opt_overwrite, const char* password) {
  uLong i;
  unz_global_info64 gi;

  int err = unzGetGlobalInfo64(uf, &gi);
  if (err != UNZ_OK)
    return err;

  for (i = 0; i < gi.number_entry; i++) {
    err = do_extract_currentfile(uf, &opt_extract_without_path, &opt_overwrite,
        password);
    if (err != UNZ_OK)
      return err;

    if (i + 1 < gi.number_entry) {
      err = unzGoToNextFile(uf);
      if (err != UNZ_OK)
        return err;
    }
  }

  return 0;
}

}

namespace greenworks {

int unzip(const char *zipfilename, const char *dirname, const char *password) {
  char filename_try[MAXFILENAME + 16] = "";
  unzFile uf = nullptr;
  int ret_value = 0;

  if (zipfilename != nullptr) {
#ifdef USEWIN32IOAPI
    zlib_filefunc64_def ffunc;
#endif

    strncpy(filename_try, zipfilename, MAXFILENAME - 1);
    //strncpy doesnt append the trailing NULL, of the string is too long.
    filename_try[MAXFILENAME] = '\0';

#ifdef USEWIN32IOAPI
    fill_win32_filefunc64A(&ffunc);
    uf = unzOpen2_64(zipfilename, &ffunc);
#else
    uf = unzOpen64(zipfilename);
#endif
    if (uf == nullptr) {
      strcat(filename_try, ".zip");
#ifdef USEWIN32IOAPI
      uf = unzOpen2_64(filename_try, &ffunc);
#else
      uf = unzOpen64(filename_try);
#endif
    }
  }

  if (uf == nullptr)
    return 1;

#ifdef _WIN32
  if (_chdir(dirname)) {
#else
  if (chdir(dirname)) {
#endif
    return 1;
  }

  ret_value = do_extract(uf, 0, 1, password);
  unzClose(uf);

  return ret_value;
}

}  // namespace greenworks
