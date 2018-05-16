// Copyright (c) 2014 Greenheart Games Pty. Ltd. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

/*
   minizip.c
   Version 1.1, February 14h, 2010
   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications of Unzip for Zip64
         Copyright (C) 2007-2008 Even Rouault

         Modifications for Zip64 support on both zip and unzip
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )
*/

#include "greenworks_zip.h"

#include <string>
#include <vector>
#include <cstring>

#include "zlib/zlib.h"
#include "zlib/contrib/minizip/zip.h"

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
  #include "misc/dirent.h"
#else
  #include <dirent.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

#ifdef _WIN32
#define USEWIN32IOAPI
#include "zlib/contrib/minizip/iowin32.h"
#endif

#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

namespace {

#ifdef _WIN32
/* char *f: name of file to get info on */
/* tm_zip *tmzip: return value: access, modific. and creation times */
/* uLong *dt: dostime */
uLong filetime(const char *f, tm_zip *tmzip, uLong *dt)
{
  int ret = 0;
  {
    FILETIME ftLocal;
    HANDLE hFind;
    WIN32_FIND_DATAA ff32;

    hFind = FindFirstFileA(f, &ff32);
    if (hFind != INVALID_HANDLE_VALUE)
    {
      FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
      FileTimeToDosDateTime(&ftLocal, ((LPWORD)dt) + 1, ((LPWORD)dt) + 0);
      FindClose(hFind);
      ret = 1;
    }
  }
  return ret;
}
#else
uLong filetime(const char *f, tm_zip *tmzip, uLong *dt)
{
  int ret = 0;
  struct stat s;        /* results of stat() */
  struct tm* filedate;
  time_t tm_t = 0;

  if (strcmp(f, "-") != 0)
  {
    char name[MAXFILENAME + 1];
    int len = strlen(f);
    if (len > MAXFILENAME)
      len = MAXFILENAME;

    strncpy(name, f, MAXFILENAME - 1);
    /* strncpy doesnt append the trailing NULL, of the string is too long. */
    name[MAXFILENAME] = '\0';

    if (name[len - 1] == '/')
      name[len - 1] = '\0';
    /* not all systems allow stat'ing a file with / appended */
    if (stat(name, &s) == 0)
    {
      tm_t = s.st_mtime;
      ret = 1;
    }
  }
  filedate = localtime(&tm_t);

  tmzip->tm_sec = filedate->tm_sec;
  tmzip->tm_min = filedate->tm_min;
  tmzip->tm_hour = filedate->tm_hour;
  tmzip->tm_mday = filedate->tm_mday;
  tmzip->tm_mon = filedate->tm_mon;
  tmzip->tm_year = filedate->tm_year;

  return ret;
}
#endif

/* calculate the CRC32 of a file, because to encrypt a file, we need known the CRC32 of the file before */
int getFileCrc(const char* filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc)
{
  unsigned long calculate_crc = 0;
  int err = ZIP_OK;
  FILE * fin = fopen(filenameinzip, "rb");
  unsigned long size_read = 0;
  unsigned long total_read = 0;
  if (fin == nullptr)
  {
    err = ZIP_ERRNO;
  }

  if (err == ZIP_OK)
    do
    {
      err = ZIP_OK;
      size_read = (int)fread(buf, 1, size_buf, fin);
      if (size_read < size_buf)
        if (feof(fin) == 0)
        {
          err = ZIP_ERRNO;
        }

      if (size_read>0)
        calculate_crc = crc32(calculate_crc, (const Bytef*)buf, size_read);
      total_read += size_read;

    } while ((err == ZIP_OK) && (size_read>0));

    if (fin)
      fclose(fin);

    *result_crc = calculate_crc;
    return err;
}

int isLargeFile(const char* filename) {
  int largeFile = 0;
  ZPOS64_T pos = 0;
  FILE* pFile = fopen64(filename, "rb");

  if (pFile != nullptr) {
    fseeko64(pFile, 0, SEEK_END);
    pos = ftello64(pFile);

    if (pos >= 0xffffffff)
      largeFile = 1;

    fclose(pFile);
  }

  return largeFile;
}

std::string PathCombine(std::string path1, std::string path2) {
  char path[PATH_MAX];

  strcpy(path, path1.c_str());
#ifdef _WIN32
  strcat(path, "\\\\");
#else
  strcat(path, "/");
#endif
  strcat(path, path2.c_str());

  //puts(path);

  return path;
}

std::vector<std::string> GetDirectoryList(const std::string& dir)
{
  DIR * d;
  std::vector<std::string> ret;

  /* Open the directory specified by "dir_name". */
  d = opendir(dir.c_str());

  /* Check it was opened. */
  if (!d) {
    // fprintf(stderr, "Cannot open directory '%s': %s\n", dir, strerror(errno));
    exit(EXIT_FAILURE);
  }

  while (1) {
    struct dirent * entry;
    const char * d_name;

    /* "Readdir" gets subsequent entries from "d". */
    entry = readdir(d);

    if (!entry) {
      /* There are no more entries in this directory, so break out of the while loop. */
      break;
    }

    d_name = entry->d_name;

    /* See if "entry" is a subdirectory of "d". */
    if (entry->d_type & DT_DIR) {

      /* Check that the directory is not "d" or d's parent. */
      if (strcmp(d_name, "..") != 0 && strcmp(d_name, ".") != 0) {
        int path_length;
        char path[PATH_MAX];

        strcpy(path, dir.c_str());
#ifdef WIN32
        strcat(path, "\\\\");
#else
        strcat(path, "/");
#endif
        strcat(path, d_name);

        //puts(path);

        path_length = strlen(path);

        if (path_length >= PATH_MAX) {
          fprintf(stderr, "Path length has got too long.\n");
          exit(EXIT_FAILURE);
        }
        std::vector<std::string> rret = GetDirectoryList(path);
        ret.insert(ret.end(), rret.begin(), rret.end());
      }
    }
    else {
      ret.push_back(PathCombine(dir, entry->d_name));
    }
  }
  /* After going through all the entries, close the directory. */
  if (closedir(d)) {
    // fprintf(stderr, "Could not close '%s': %s\n", dir, strerror(errno));
    exit(EXIT_FAILURE);
  }

  return ret;
}

}

namespace greenworks {

int zip(const char* targetFile, const char* sourceDir, int compressionLevel, const char* password) {
  // compressionLevel 0-9 (store only - best)
  int opt_overwrite = 1;// Overwrite existing zip file
  int opt_compress_level = compressionLevel;
  char filename_try[MAXFILENAME + 16];
  int err = 0;
  int size_buf = 0;
  void* buf = nullptr;
  int i, len;
  int dot_found = 0;

  size_buf = WRITEBUFFERSIZE;
  buf = (void*)malloc(size_buf);
  if (buf == nullptr)
    return ZIP_INTERNALERROR;

  strncpy(filename_try, targetFile, MAXFILENAME - 1);
  // strncpy doesnt append the trailing NULL, of the string is too long.
  filename_try[MAXFILENAME] = '\0';

  len = (int)strlen(filename_try);
  for (i = 0; i < len; i++) {
    if (filename_try[i] == '.')
      dot_found = 1;
  }

  if (dot_found == 0) {
    strcat(filename_try, ".zip");
  }

  zipFile zf;

#ifdef USEWIN32IOAPI
  zlib_filefunc64_def ffunc;
  fill_win32_filefunc64A(&ffunc);
  zf = zipOpen2_64(filename_try, (opt_overwrite == 2) ? 2 : 0, NULL, &ffunc);
#else
  zf = zipOpen64(filename_try, (opt_overwrite == 2) ? 2 : 0);
#endif

  if (zf == nullptr)
    err = ZIP_ERRNO;

  std::vector<std::string> files = GetDirectoryList(sourceDir);
  if (files.size() <= 0) {
    err = ZIP_PARAMERROR;
  } else {
    std::vector<std::string>::iterator itr;
    for (itr = files.begin(); itr < files.end(); ++itr) {
      const char* filenameinzip = itr->c_str();
      const char *savefilenameinzip;

      FILE * fin;
      int size_read;
      zip_fileinfo zi;
      unsigned long crcFile = 0;
      int zip64 = 0;

      zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
      zi.dosDate = 0;
      zi.internal_fa = 0;
      zi.external_fa = 0;

      filetime(filenameinzip, &zi.tmz_date, &zi.dosDate);

      if ((password != nullptr && strlen(password) > 0) && (err == ZIP_OK))
        err = getFileCrc(filenameinzip, buf, size_buf, &crcFile);

      zip64 = isLargeFile(filenameinzip);

      // The path name saved, should not include a leading slash.
      // if it did, windows/xp and dynazip couldn't read the zip file.
#ifdef WIN32
      std::string baseDir = itr->substr(std::string(sourceDir).rfind('\\') + 1);
#else
      std::string baseDir = itr->substr(std::string(sourceDir).rfind('/') + 1);
#endif

      savefilenameinzip = baseDir.c_str();

      while (savefilenameinzip[0] == '\\' || savefilenameinzip[0] == '/')
        savefilenameinzip++;

      // Using 4 for unicode compatibility (UTF8) -- tested with chinese, does not work as expected
      err = zipOpenNewFileInZip4_64(zf, savefilenameinzip, &zi, nullptr, 0, nullptr, 0, nullptr, (opt_compress_level != 0) ? Z_DEFLATED : 0, opt_compress_level, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, password, crcFile, 36, 1 << 11, zip64);

      if (err != ZIP_OK)
        break;

      fin = fopen64(filenameinzip, "rb");
      if (fin == nullptr)
        err = ZIP_ERRNO;

      if (err == ZIP_OK) {
        do {
          err = ZIP_OK;
          size_read = (int)fread(buf, 1, size_buf, fin);
          if (size_read < size_buf)
            if (feof(fin) == 0)
              err = ZIP_ERRNO;

          if (size_read>0) {
            err = zipWriteInFileInZip(zf, buf, size_read);
          }
        } while ((err == ZIP_OK) && (size_read>0));
      }
      if (fin)
        fclose(fin);
      if (err < 0)
        err = ZIP_ERRNO;
      else
        err = zipCloseFileInZip(zf);
    }
  }
  zipClose(zf, nullptr);
  free(buf);
  return err;
}

}  // namespace greenworks

