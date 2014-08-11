#include "CUtils.h"
#include "CUnzip.h"

using namespace std;

/* change_file_date : change the date/time of a file
filename : the filename of the file where date/time must be modified
dosdate : the new date at the MSDos format (4 bytes)
tmu_date : the SAME new date at the tm_unz format */

void CUnzip::change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date)
{
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


/* mymkdir and change_file_date are not 100 % portable
As I don't know well Unix, I wait feedback for the unix portion */

int CUnzip::mymkdir(const char* dirname)
{
	int ret = 0;
	#ifdef _WIN32
		ret = _mkdir(dirname);
	#else
		ret = mkdir(dirname, 0775);
	#endif
		return ret;
}

int CUnzip::makedir(const char *newdir)
{
	char *buffer;
	char *p;
	int  len = (int)strlen(newdir);

	if (len <= 0)
		return 0;

	buffer = (char*)malloc(len + 1);
	if (buffer == NULL)
	{
		CUtils::WriteToLogFile("Error allocating memory\n");
		return UNZ_INTERNALERROR;
	}
	strcpy(buffer, newdir);

	if (buffer[len - 1] == '/') {
		buffer[len - 1] = '\0';
	}
	if (mymkdir(buffer) == 0)
	{
		free(buffer);
		return 1;
	}

	p = buffer + 1;
	while (1)
	{
		char hold;

		while (*p && *p != '\\' && *p != '/')
			p++;
		hold = *p;
		*p = 0;
		if ((mymkdir(buffer) == -1) && (errno == ENOENT))
		{
			CUtils::WriteToLogFile("couldn't create directory");
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

void CUnzip::Display64BitsSize(ZPOS64_T n, int size_char)
{
	/* to avoid compatibility problem , we do here the conversion */
	char number[21];
	int offset = 19;
	int pos_string = 19;
	number[20] = 0;
	for (;;) {
		number[offset] = (char)((n % 10) + '0');
		if (number[offset] != '0')
			pos_string = offset;
		n /= 10;
		if (offset == 0)
			break;
		offset--;
	}
	{
		int size_display_string = 19 - pos_string;
		while (size_char > size_display_string)
		{
			size_char--;
			CUtils::WriteToLogFile(" ");
		}
	}
}

int CUnzip::do_list(unzFile uf)
{
	uLong i;
	unz_global_info64 gi;
	int err;

	err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK){
		CUtils::WriteToLogFile("error with zipfile in unzGetGlobalInfo \n");
	}
	CUtils::WriteToLogFile("  Length  Method     Size Ratio   Date    Time   CRC-32     Name\n");
	CUtils::WriteToLogFile("  ------  ------     ---- -----   ----    ----   ------     ----\n");

	for (i = 0; i<gi.number_entry; i++)
	{
		char filename_inzip[256];
		unz_file_info64 file_info;
		uLong ratio = 0;
		const char *string_method;
		char charCrypt = ' ';
		err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
		if (err != UNZ_OK)
		{
			CUtils::WriteToLogFile("error with zipfile in unzGetCurrentFileInfo");
			break;
		}
		if (file_info.uncompressed_size>0)
			ratio = (uLong)((file_info.compressed_size * 100) / file_info.uncompressed_size);

		/* display a '*' if the file is crypted */
		if ((file_info.flag & 1) != 0)
			charCrypt = '*';

		if (file_info.compression_method == 0)
			string_method = "Stored";
		else
			if (file_info.compression_method == Z_DEFLATED)
			{
				uInt iLevel = (uInt)((file_info.flag & 0x6) / 2);
				if (iLevel == 0)
					string_method = "Defl:N";
				else if (iLevel == 1)
					string_method = "Defl:X";
				else if ((iLevel == 2) || (iLevel == 3))
					string_method = "Defl:F"; /* 2:fast , 3 : extra fast*/
			}
			else
				if (file_info.compression_method == Z_BZIP2ED)
				{
					string_method = "BZip2 ";
				}
				else
					string_method = "Unkn. ";

		Display64BitsSize(file_info.uncompressed_size, 7);
		printf("  %6s%c", string_method, charCrypt);
		Display64BitsSize(file_info.compressed_size, 7);
		printf(" %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
			ratio,
			(uLong)file_info.tmu_date.tm_mon + 1,
			(uLong)file_info.tmu_date.tm_mday,
			(uLong)file_info.tmu_date.tm_year % 100,
			(uLong)file_info.tmu_date.tm_hour, (uLong)file_info.tmu_date.tm_min,
			(uLong)file_info.crc, filename_inzip);
		if ((i + 1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK)
			{
				CUtils::WriteToLogFile("error with zipfile in unzGoToNextFile");
				break;
			}
		}
	}

	return 0;
}

int CUnzip::do_extract_currentfile(unzFile uf, const int* popt_extract_without_path, int* popt_overwrite, const char* password)
{
	char filename_inzip[256];
	char* filename_withoutpath;
	char* p;
	int err = UNZ_OK;
	FILE *fout = NULL;
	void* buf;
	uInt size_buf;

	unz_file_info64 file_info;
	uLong ratio = 0;
	err = unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);

	if (err != UNZ_OK)
	{
		CUtils::WriteToLogFile("error with zipfile in unzGetCurrentFileInfo");
		return err;
	}

	size_buf = WRITEBUFFERSIZE;
	buf = (void*)malloc(size_buf);
	if (buf == NULL)
	{
		CUtils::WriteToLogFile("Error allocating memory\n");
		return UNZ_INTERNALERROR;
	}

	p = filename_withoutpath = filename_inzip;
	while ((*p) != '\0')
	{
		if (((*p) == '/') || ((*p) == '\\'))
		{
#ifndef _WIN32
			(*p) = '/';
#endif
			filename_withoutpath = p + 1;
		}
		p++;
	}

	if ((*filename_withoutpath) == '\0')
	{
		if ((*popt_extract_without_path) == 0)
		{
			CUtils::WriteToLogFile("creating directory: " + string(filename_inzip));
			mymkdir(filename_inzip);
		}
	}
	else
	{
		const char* write_filename;
		int skip = 0;

		if ((*popt_extract_without_path) == 0)
			write_filename = filename_inzip;
		else
			write_filename = filename_withoutpath;

		err = unzOpenCurrentFilePassword(uf, password);
		if (err != UNZ_OK)
		{
			CUtils::WriteToLogFile("error with zipfile in unzOpenCurrentFilePassword");
		}

		if (((*popt_overwrite) == 0) && (err == UNZ_OK))
		{
			char rep = 0;
			FILE* ftestexist;
			ftestexist = fopen64(write_filename, "rb");
			if (ftestexist != NULL)
			{
				fclose(ftestexist);
				do
				{
					char answer[128];
					int ret;

					CUtils::WriteToLogFile("The file " + string(write_filename) + " exists. Overwrite ? [y]es, [n]o, [A]ll: ");

					ret = scanf("%1s", answer);
					if (ret != 1)
					{
						exit(EXIT_FAILURE);
					}
					rep = answer[0];
					if ((rep >= 'a') && (rep <= 'z'))
						rep -= 0x20;
				} while ((rep != 'Y') && (rep != 'N') && (rep != 'A'));
			}

			if (rep == 'N')
				skip = 1;

			if (rep == 'A')
				*popt_overwrite = 1;
		}

		if ((skip == 0) && (err == UNZ_OK))
		{
			fout = fopen64(write_filename, "wb");

			/* some zipfile don't contain directory alone before file */
			if ((fout == NULL) && ((*popt_extract_without_path) == 0) &&
				(filename_withoutpath != (char*)filename_inzip))
			{
				char c = *(filename_withoutpath - 1);
				*(filename_withoutpath - 1) = '\0';
				makedir(write_filename);
				*(filename_withoutpath - 1) = c;
				fout = fopen64(write_filename, "wb");
			}

			if (fout == NULL)
			{
				CUtils::WriteToLogFile("error opening " + string(write_filename));
			}
		}

		if (fout != NULL)
		{
			CUtils::WriteToLogFile(" extracting: " + string(write_filename));

			do
			{
				err = unzReadCurrentFile(uf, buf, size_buf);
				if (err<0)
				{
					CUtils::WriteToLogFile("error with zipfile in unzReadCurrentFile");
					break;
				}
				if (err>0)
					if (fwrite(buf, err, 1, fout) != 1)
					{
					CUtils::WriteToLogFile("error in writing extracted file");
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
		{
			err = unzCloseCurrentFile(uf);
			if (err != UNZ_OK)
			{
				CUtils::WriteToLogFile("error with zipfile in unzCloseCurrentFile");
			}
		}
		else
			unzCloseCurrentFile(uf); /* don't lose the error */
	}

	free(buf);
	return err;
}


int CUnzip::do_extract(unzFile uf, int opt_extract_without_path, int opt_overwrite, const char* password)
{
	uLong i;
	unz_global_info64 gi;
	int err;
	FILE* fout = NULL;

	err = unzGetGlobalInfo64(uf, &gi);
	if (err != UNZ_OK){
		CUtils::WriteToLogFile("error with zipfile in unzGetGlobalInfo ");
	}
	for (i = 0; i<gi.number_entry; i++)
	{
		if (do_extract_currentfile(uf, &opt_extract_without_path,
			&opt_overwrite,
			password) != UNZ_OK)
			break;

		if ((i + 1)<gi.number_entry)
		{
			err = unzGoToNextFile(uf);
			if (err != UNZ_OK)
			{
				CUtils::WriteToLogFile("error with zipfile in unzGoToNextFile");
				break;
			}
		}
	}

	return 0;
}

int CUnzip::do_extract_onefile(unzFile uf, const char* filename, int opt_extract_without_path, int opt_overwrite, const char* password)
{
	int err = UNZ_OK;
	if (unzLocateFile(uf, filename, CASESENSITIVITY) != UNZ_OK)
	{
		CUtils::WriteToLogFile("file " + string(filename) + " not found in the zipfile\n");

		return 2;
	}

	if (do_extract_currentfile(uf, &opt_extract_without_path,
		&opt_overwrite,
		password) == UNZ_OK)
		return 0;
	else
		return 1;
}

int CUnzip::unzip(const char *zipfilename, const char *dirname, const char *password) {
	char filename_try[MAXFILENAME + 16] = "";
	unzFile uf = NULL;
	int ret_value = 0;

	if (zipfilename != NULL)
	{

#        ifdef USEWIN32IOAPI
		zlib_filefunc64_def ffunc;
#        endif

		strncpy(filename_try, zipfilename, MAXFILENAME - 1);
		//strncpy doesnt append the trailing NULL, of the string is too long. 
		filename_try[MAXFILENAME] = '\0';

#        ifdef USEWIN32IOAPI
		fill_win32_filefunc64A(&ffunc);
		uf = unzOpen2_64(zipfilename, &ffunc);
#        else
		uf = unzOpen64(zipfilename);
#        endif
		if (uf == NULL)
		{
			strcat(filename_try, ".zip");
#            ifdef USEWIN32IOAPI
			uf = unzOpen2_64(filename_try, &ffunc);
#            else
			uf = unzOpen64(filename_try);
#            endif
		}
	}

	if (uf == NULL)
	{
		CUtils::WriteToLogFile("Cannot open " + string(zipfilename) + " or " + string(zipfilename) + ".zip");

		return 1;
	}
	CUtils::WriteToLogFile(string(filename_try) + " opened");

#ifdef _WIN32
	if (_chdir(dirname))
#else
	if (chdir(dirname))
#endif
		{
		CUtils::WriteToLogFile("Error changing into " + string(dirname) + ", aborting");
		exit(-1);
		}

		ret_value = do_extract(uf, 0, 1, password);

	unzClose(uf);

	return ret_value;
}