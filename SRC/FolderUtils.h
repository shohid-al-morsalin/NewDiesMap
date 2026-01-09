#ifndef _FOLDER_UTILS
#define _FOLDER_UTILS

#include <windows.h>
#include <stdio.h>
#include <string>
using std::string;

class CFolderUtils {
public:
	static BOOL	CreateFolder(LPCTSTR pFolder) {
		TCHAR szFolder[MAX_PATH];
		if (strlen(pFolder) < MAX_PATH) {
			_tcscpy(szFolder, pFolder);
		}
		TCHAR* pStart = szFolder;
		TCHAR* pEnd = pStart + _tcslen(szFolder);
		TCHAR* p = pEnd;

		// Try 10 times to create the directory
		for (int i = 0; i < 10; i++) {
			BOOL bOK = CreateDirectory(szFolder, NULL);
			DWORD dwLastError = GetLastError();
			if (!bOK && dwLastError == ERROR_PATH_NOT_FOUND) {
				while (*p != '\\') {
					if (p == pStart)
						return FALSE;
					p--;
				}
				*p = NULL;
			}
			else if (bOK || (ERROR_ALREADY_EXISTS == dwLastError)) {
				if (p == pEnd) {
					return TRUE;
				}

				*p = '\\';
				while (*p) {
					p++;
				}
			}
			else {
				break;
			}
		}

		return FALSE;
	}

	/*
	The CreateDir function creates folders and sub-folders thereby completing the whole path
	passed to it. If the folder already exists, it is left unaffected, but if it doesn't exist,
	it is created.
	However CreateDirectory WIN32 API lets us create a directory, but it works only when
	the parent directory already exists. This function overcomes this limitation.*/
	static BOOL CreateDir(char* Path) {
		char DirName[256];
		char* p = Path;
		char* q = DirName;

		while (*p) {
			if (('\\' == *p) || ('/' == *p)) {
				if (':' != *(p - 1)) {
					if (!CreateDirectory(DirName, NULL)) {
						//						return FALSE;
					}
				}
			}
			*q++ = *p++;
			*q = '\0';
		}
		if (!CreateDirectory(DirName, NULL)) {
			//			return FALSE;
		}
		return TRUE;
	}

	static DWORD GetSpace(LPCTSTR pFolder) {
		TCHAR szFullPathFileName[MAX_PATH];
		TCHAR szFilename[MAX_PATH];
		WIN32_FIND_DATA FileData = { 0 };
		BOOL bFinished = FALSE;
		DWORD dwSize = 0;

		_stprintf(szFullPathFileName, TEXT("%s\\*.*"), pFolder);
		HANDLE hSearch = FindFirstFile(szFullPathFileName, &FileData);
		if (hSearch == INVALID_HANDLE_VALUE)
			return 0;

		while (!bFinished) {
			_stprintf(szFilename, TEXT("%s\\%s"), pFolder, FileData.cFileName);
			if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (_tcscmp(FileData.cFileName, TEXT(".")) && _tcscmp(FileData.cFileName, TEXT("..")))
					dwSize += GetSpace(szFilename);
			}
			else {
				dwSize += FileData.nFileSizeLow;
			}

			if (!FindNextFile(hSearch, &FileData)) {
				if (GetLastError() == ERROR_NO_MORE_FILES) {
					bFinished = TRUE;
				}
			}
		}

		FindClose(hSearch);
		return dwSize;
	}

	static BOOL RemoveFolder(LPCTSTR pFolder) {
		TCHAR szFullPathFileName[MAX_PATH];
		TCHAR szFilename[MAX_PATH];

		if (!::RemoveDirectory(pFolder)) {
			DWORD err = GetLastError();
			if (err != ERROR_DIR_NOT_EMPTY) {
				return FALSE;
			}
		}

		// remove sub folders and files.

		WIN32_FIND_DATA FileData = { 0 };
		BOOL bFinished = FALSE;
		DWORD dwSize = 0;

		_stprintf(szFullPathFileName, TEXT("%s\\*.*"), pFolder);
		HANDLE hSearch = FindFirstFile(szFullPathFileName, &FileData);
		if (hSearch == INVALID_HANDLE_VALUE) {
			return 0;
		}

		while (!bFinished) {
			_stprintf(szFilename, TEXT("%s\\%s"), pFolder, FileData.cFileName);
			if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (_tcscmp(FileData.cFileName, TEXT(".")) && _tcscmp(FileData.cFileName, TEXT(".."))) {
					RemoveFolder(szFilename);
					RemoveDirectory(szFilename);
				}
			}
			else {
				if (FileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) {
					SetFileAttributes(szFilename, FILE_ATTRIBUTE_NORMAL);
				}

				if (!::DeleteFile(szFilename)) {
					FindClose(hSearch);
					return FALSE;
				}
			}
			if (!FindNextFile(hSearch, &FileData)) {
				if (GetLastError() == ERROR_NO_MORE_FILES) {
					bFinished = TRUE;
				}
			}
		}
		FindClose(hSearch);

		// Here the directory is empty.
		::RemoveDirectory(pFolder);
		return TRUE;
	}

	static BOOL CopyFolder(LPCTSTR pFromFolder, LPCTSTR pToFolder) {
		SHFILEOPSTRUCT fo = { 0 };

		fo.wFunc = FO_COPY;
		fo.pFrom = pFromFolder;
		fo.pTo = pToFolder;
		fo.fFlags = FOF_SILENT | FOF_NOERRORUI;

		int rc = SHFileOperation(&fo);
		return (rc == 0);
	}

	/*The DeleteAllFiles function deletes all the files (not subfolders) present in
		the specified path.*/
	static void DeleteAllFiles(char* folderPath) {
		char fileFound[256];
		WIN32_FIND_DATA info;
		HANDLE hp;

		sprintf(fileFound, "%s\\*.*", folderPath);
		hp = FindFirstFile(fileFound, &info);
		do {
			sprintf(fileFound, "%s\\%s", folderPath, info.cFileName);
			DeleteFile(fileFound);
		} while (FindNextFile(hp, &info));

		FindClose(hp);
	}

	/*
	The EmptyDirectory function deletes all the contents from a specified directory.
	However RemoveDirectory WIN32 API deletes an existing empty directory, it doesn't
	work when the directory isn't empty. This function overcomes this limitation.*/
	static void EmptyDirectory(char* folderPath) {
		char fileFound[256];
		WIN32_FIND_DATA info;
		HANDLE hp;

		sprintf(fileFound, "%s\\*.*", folderPath);
		hp = FindFirstFile(fileFound, &info);
		do {
			if (!((strcmp(info.cFileName, ".") == 0) || (strcmp(info.cFileName, "..") == 0))) {
				if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
					string subFolder = folderPath;
					subFolder.append("\\");
					subFolder.append(info.cFileName);
					EmptyDirectory((char*)subFolder.c_str());
					RemoveDirectory(subFolder.c_str());
				}
				else {
					sprintf(fileFound, "%s\\%s", folderPath, info.cFileName);
					BOOL retVal = DeleteFile(fileFound);
				}
			}
		} while (FindNextFile(hp, &info));

		FindClose(hp);
	}
};

#endif