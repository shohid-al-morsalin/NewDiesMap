#include "stdafx.h"
#include <direct.h>
#include "DosUtil.h"
#include "FolderUtils.h"

CDirSys::CDirSys() {
	rDir = "C:\\DATA";
}

CDirSys::~CDirSys() {}

CString CDirSys::GetRootDir() {
	return rDir;
}

void CDirSys::SetRootDir(CString str) {
	rDir = str;
}

BOOL CDirSys::LocateFile(CString& filename) {
	FILE* f = fopen(filename, "r");
	if (f) {
		fclose(f);
		return TRUE;
	}
	return FALSE;
}

// Entry Point YFL [4/2/2003]
// Use this when need to write to a file
// Input filename should have no path info
// output filename should have full path info
// does not change login directory
//[[ 20251013_Mohir comment this
//BOOL CDirSys::GetC2CFileName(CString& filename, BOOL bCurDir) {
//	ChgExt(filename, ".C2C");
//
//	// Search for file in current and pass directory, if found return the full path name
//	CString name;
//	if (bCurDir) {
//		if (MakeCurC2CPath(name)) {
//			name += "\\" + filename;
//			filename = name;
//			return TRUE;
//		}
//	}
//	else {
//		if (IsFileEntryExist(filename)) {
//			return TRUE;
//		}
//		if (MakeCurC2CPath(name)) {
//			name += "\\" + filename;
//			filename = name;
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
// //]]
//[[20251013_Mohir
BOOL CDirSys::MakeCurC2CPath(CString& curPath, CString rcpName, CString OpName) {
	CString month;
	CString rootdir;
	CTime t = CTime::GetCurrentTime();
	month = t.Format("%Y\\%m"); 
	month.Format("%s\\%s\\%s", OpName, rcpName, month);  
	month += "\\C2C";

	rootdir = rDir;

	if (rootdir.GetAt(rootdir.GetLength() - 1) != '\\') {
		curPath = rootdir + "\\";
	}
	else {
		curPath = rootdir;
	}
	curPath += month;
	return TRUE;
}

BOOL CDirSys::IsFileEntryExist(CString& filename, CString rcpName, CString OpName) {
	int i;
	int mcnt = 0;	// pass month counter
	CString dir, month, rootdir;

	// Look at current month
	CTime t = CTime::GetCurrentTime();
	month = t.Format("%Y\\%m");  
	month.Format("%s\\%s\\%s", OpName, rcpName, month);  
	if (filename.Find(".C2C") != -1) {
		month += "\\C2C";
	}
	else {
		month += "\\TXT";
	}
	rootdir = rDir;

	CString name;
	name = rootdir + "\\";
	name += month + "\\";
	name += filename;
	if (LocateFile(name)) {
		filename = name;
		return TRUE;
	}
	// Look at pass month
	for (i = t.GetMonth() - 1; i >= 1; i--) {
		mcnt++;
		month = ""; 
		if (mcnt > 6) {
			return FALSE;
		}
		month.Format("%s\\%02d", t.Format("%Y"), i); 
		month.Format("%s\\%s\\%s", OpName, rcpName, month);  
		if (filename.Find(".C2C") != -1) {
			month += "\\C2C";
		}
		else {
			month += "\\TXT";
		}
		name = rootdir + "\\";
		name += month + "\\";
		name += filename;
		if (LocateFile(name)) {
			filename = name;
			return TRUE;
		}
	}
	// Look at pass year
	int y = t.GetYear() - 1;
	if (y <= 2003) {
		// Starting year, skip search
		return FALSE;
	}
	for (i = 12; i >= 1; i--) {
		month = "";  
		mcnt++;
		if (mcnt > 6) {
			return FALSE;
		}
		month.Format("%d\\%02d", y, i);  
		month.Format("%s\\%s\\%s", OpName, rcpName, month);  
		if (filename.Find(".C2C") != -1) {
			month += "\\C2C";
		}
		else {
			month += "\\TXT";
		}
		name = rootdir + "\\";
		name += month + "\\";
		name += filename;
		if (LocateFile(name)) {
			filename = name;
			return TRUE;
		}
	}
	return FALSE;
}





BOOL CDirSys::GetC2CFileName(CString& filename, BOOL bCurDir, CString rcpName, CString OpName) {
	ChgExt(filename, ".C2C");

	// Search for file in current and pass directory, if found return the full path name
	CString name;
	if (bCurDir) {
		if (MakeCurC2CPath(name, rcpName, OpName)) {
			name += "\\" + filename;
			filename = name;
			return TRUE;
		}
	}
	else {
		if (IsFileEntryExist(filename, rcpName, OpName)) {
			return TRUE;
		}
		if (MakeCurC2CPath(name, rcpName, OpName)) {
			name += "\\" + filename;
			filename = name;
			return TRUE;
		}
	}
	return FALSE;
}


//]]




// Entry Point YFL [4/2/2003]
// Use this when need to write to a file
// Note: It does not create the directory
//		but return filename modified to contain full path info
//		does not change the login directory
BOOL CDirSys::GetTxTFileName(CString& filename) {
	ChgExt(filename, ".TXT");

	// Search for file in current and pass directory, if found return the full path name
	if (IsFileEntryExist(filename)) {
		return TRUE;
	}
	CString name;
	if (MakeCurTxtPath(name)) {
		name += "\\" + filename;
		filename = name;
	}
	return FALSE;
}

// Will not create directory or change current directory
// Use this to check for file existence only
// Will also search pass month directory
// and one year before this year
// return found filename containing full path, path could be different from initial current directory
//[[ 20251013_Mohir comment this
//BOOL CDirSys::IsFileEntryExist(CString& filename) {
//	int i;
//	int mcnt = 0;	// pass month counter
//	CString dir, month, rootdir;
//
//	// Look at current month
//	CTime t = CTime::GetCurrentTime();
//	month = t.Format("%Y%m");
//	if (filename.Find(".C2C") != -1) {
//		month += "\\C2C";
//	}
//	else {
//		month += "\\TXT";
//	}
//	rootdir = rDir;
//
//	CString name;
//	name = rootdir + "\\";
//	name += month + "\\";
//	name += filename;
//	if (LocateFile(name)) {
//		filename = name;
//		return TRUE;
//	}
//	// Look at pass month
//	for (i = t.GetMonth() - 1; i >= 1; i--) {
//		mcnt++;
//		if (mcnt > 6) {
//			return FALSE;
//		}
//		month.Format("%s%02d", t.Format("%Y"), i);
//		if (filename.Find(".C2C") != -1) {
//			month += "\\C2C";
//		}
//		else {
//			month += "\\TXT";
//		}
//		name = rootdir + "\\";
//		name += month + "\\";
//		name += filename;
//		if (LocateFile(name)) {
//			filename = name;
//			return TRUE;
//		}
//	}
//	// Look at pass year
//	int y = t.GetYear() - 1;
//	if (y <= 2003) {
//		// Starting year, skip search
//		return FALSE;
//	}
//	for (i = 12; i >= 1; i--) {
//		mcnt++;
//		if (mcnt > 6) {
//			return FALSE;
//		}
//		month.Format("%d%02d", y, i);
//		if (filename.Find(".C2C") != -1) {
//			month += "\\C2C";
//		}
//		else {
//			month += "\\TXT";
//		}
//		name = rootdir + "\\";
//		name += month + "\\";
//		name += filename;
//		if (LocateFile(name)) {
//			filename = name;
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
//]]

void CDirSys::ChgExt(CString& filename, CString extsn) {
	char dirname[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	strcpy(dirname, filename.GetBuffer(filename.GetLength()));
	_splitpath(dirname, drive, dir, fname, ext);
	_makepath(dirname, drive, dir, fname, extsn.GetBuffer(0));
	filename = dirname;
}

BOOL CDirSys::GotoDirec(CString fullpath) {
	char path[512];

	strcpy(path, fullpath.GetBuffer(0));
	BOOL ret = CFolderUtils::CreateDir(path);
	return ret;
}

//[[20251013_Mohir
//BOOL CDirSys::MakeCurC2CPath(CString& curPath) {
//	CString month;
//	CString rootdir;
//	CTime t = CTime::GetCurrentTime();
//	month = t.Format("%Y%m");
//	month += "\\C2C";
//
//	rootdir = rDir;
//
//	if (rootdir.GetAt(rootdir.GetLength() - 1) != '\\') {
//		curPath = rootdir + "\\";
//	}
//	else {
//		curPath = rootdir;
//	}
//	curPath += month;
//	return TRUE;
//}
//BOOL CDirSys::MakeCurTxtPath(CString& curPath) {
//	CString month;
//	CString rootdir;
//	CTime t = CTime::GetCurrentTime();
//	month = t.Format("%Y%m");
//	month += "\\TXT";
//	rootdir = rDir;
//	curPath = rootdir + "\\";
//	curPath += month;
//	return TRUE;
//}

BOOL CDirSys::MakeCurTxtPath(CString& curPath, CString rcpName) {
	CString month;
	CString rootdir;
	CTime t = CTime::GetCurrentTime();
	month = t.Format("%Y\\%m");  
	month.Format("%s\\%s", rcpName, month);  
	month += "\\TXT";
	rootdir = rDir;
	curPath = rootdir + "\\";
	curPath += month;
	return TRUE;
}
//]]
CString CDirSys::ExtractFilePath(CString& fullname) {
	CString path;
	char dirname[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(fullname, drive, dir, fname, ext);
	_makepath(dirname, drive, dir, "", "");
	int len = strlen(dirname) - 1;
	if (dirname[len] == '\\') {
		dirname[len] = 0;
	}
	return dirname;
}

void CDirSys::ExtractFileName(CString& name) {
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(name, drive, dir, fname, ext);
	name = fname;
	name += ext;
}