#include "CZip.h"

#ifdef _WIN32
/* char *f: name of file to get info on */
/* tm_zip *tmzip: return value: access, modific. and creation times */
/* uLong *dt: dostime */
uLong CZip::filetime(char *f, tm_zip *tmzip, uLong *dt)
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
uLong CZip::filetime(char *f, tm_zip *tmzip, uLong *dt)
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

int CZip::check_exist_file(const char* filename)
{
	FILE* ftestexist;
	int ret = 1;
	ftestexist = fopen64(filename, "rb");
	if (ftestexist == NULL)
		ret = 0;
	else
		fclose(ftestexist);
	return ret;
}

/* calculate the CRC32 of a file, because to encrypt a file, we need known the CRC32 of the file before */
int CZip::getFileCrc(const char* filenameinzip, void* buf, unsigned long size_buf, unsigned long* result_crc)
{
	unsigned long calculate_crc = 0;
	int err = ZIP_OK;
	FILE * fin = fopen(filenameinzip, "rb");
	unsigned long size_read = 0;
	unsigned long total_read = 0;
	if (fin == NULL)
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

int CZip::isLargeFile(const char* filename)
{
	int largeFile = 0;
	ZPOS64_T pos = 0;
	FILE* pFile = fopen64(filename, "rb");

	if (pFile != NULL)
	{
		int n = fseeko64(pFile, 0, SEEK_END);

		pos = ftello64(pFile);

		if (pos >= 0xffffffff)
			largeFile = 1;

		fclose(pFile);
	}

	return largeFile;
}

int CZip::zip(const char* targetFile, const char* sourceDir, int compressionLevel, const char* password){
	
	// compressionLevel 0-9 (store only - best)
	int opt_overwrite = 1;// Overwrite existing zip file
	int opt_compress_level = compressionLevel;
	int opt_exclude_path = 0; // Do not exclude path
	char filename_try[MAXFILENAME + 16];
	int zipok;
	int err = 0;
	int size_buf = 0;
	void* buf = NULL;
	int i, len;
	int dot_found = 0;

	size_buf = WRITEBUFFERSIZE;
	buf = (void*)malloc(size_buf);
	if (buf == NULL)
	{
		return ZIP_INTERNALERROR;
	}


	zipok = 1;

	strncpy(filename_try, targetFile, MAXFILENAME - 1);
	// strncpy doesnt append the trailing NULL, of the string is too long. 
	filename_try[MAXFILENAME] = '\0';

	len = (int)strlen(filename_try);
	for (i = 0; i < len; i++){
		if (filename_try[i] == '.'){
			dot_found = 1;
		}
	}

	if (dot_found == 0){
		strcat(filename_try, ".zip");
	}

	zipFile zf;
	int errclose;

#ifdef USEWIN32IOAPI
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64A(&ffunc);
	zf = zipOpen2_64(filename_try, (opt_overwrite == 2) ? 2 : 0, NULL, &ffunc);
#else
	zf = zipOpen64(filename_try, (opt_overwrite == 2) ? 2 : 0);
#endif

	if (zf == NULL)
	{
		err = ZIP_ERRNO;
	}

	
	vector<string> files = CUtils::GetDirectoryList(sourceDir);

	if (files.size() <= 0) {
		err = ZIP_PARAMERROR;
		return -1;
	}
	else {

		std::vector<string>::iterator itr;
		for (itr = files.begin(); itr < files.end(); ++itr)
		{
			char* fullPathToFile = (char*)itr->c_str();
			char* filenameinzip = (char*)itr->c_str();
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

			if ((password != NULL && strlen(password) > 0) && (err == ZIP_OK)){
				err = getFileCrc(filenameinzip, buf, size_buf, &crcFile);
			}

			zip64 = isLargeFile(filenameinzip);
			
			// The path name saved, should not include a leading slash. 
			// if it did, windows/xp and dynazip couldn't read the zip file. 

#ifdef WIN32
			string baseDir = itr->substr(CUtils::ToString(sourceDir).rfind('\\') + 1);
#else
			string baseDir = itr->substr(CUtils::ToString(sourceDir).rfind('/') + 1);
#endif

			savefilenameinzip = baseDir.c_str();

			while (savefilenameinzip[0] == '\\' || savefilenameinzip[0] == '/')
			{
				savefilenameinzip++;
			}

			// Using 4 for unicode compatibility (UTF8) -- tested with chinese, does not work as expected
			err = zipOpenNewFileInZip4_64(zf, savefilenameinzip, &zi, NULL, 0, NULL, 0, NULL, (opt_compress_level != 0) ? Z_DEFLATED : 0, opt_compress_level, 0, -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, password, crcFile, 36, 1 << 11, zip64);

			if (err != ZIP_OK){
				CUtils::WriteToLogFile("error in opening " + string(filenameinzip) + " in zipfile");
			}
			else
			{
				fin = fopen64(filenameinzip, "rb");
				if (fin == NULL)
				{
					err = ZIP_ERRNO;
					CUtils::WriteToLogFile("error in opening " + string(filenameinzip) + " for reading");
				}
			}

			if (err == ZIP_OK)
				do
				{
					err = ZIP_OK;
					size_read = (int)fread(buf, 1, size_buf, fin);
					if (size_read < size_buf)
						if (feof(fin) == 0)
						{
							CUtils::WriteToLogFile("error in reading " + string(filenameinzip));
							err = ZIP_ERRNO;
						}

					if (size_read>0)
					{
						err = zipWriteInFileInZip(zf, buf, size_read);
						if (err<0)
						{
							CUtils::WriteToLogFile("error in writing " + string(filenameinzip));
						}

					}
				} while ((err == ZIP_OK) && (size_read>0));

				if (fin) {
					fclose(fin);
				}

				if (err < 0) {
					err = ZIP_ERRNO;
				}
				else
				{
					err = zipCloseFileInZip(zf);
					if (err != ZIP_OK){
						CUtils::WriteToLogFile("error in closing " + string(filenameinzip));
					}
				}
				filenameinzip = "";
		}
	}


	errclose = zipClose(zf, NULL);
	if (errclose != ZIP_OK)
		CUtils::WriteToLogFile("error in closing " + string(filename_try));



	free(buf);
	return err; // 0
}