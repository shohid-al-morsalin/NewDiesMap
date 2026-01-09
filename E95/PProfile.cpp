// PProfile.cpp: implementation of the CPProfile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PProfile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPProfile::CPProfile() {
	MakeSystemFile();
	MakeLocalFile();
}

CPProfile::~CPProfile() {}

void CPProfile::MakeSystemFile() {
	char inifile[_MAX_PATH];
	GetWindowsDirectory(inifile, _MAX_PATH);
	fsmini = inifile;
	fsmini += "\\fsmapp.ini";
}

void CPProfile::MakeLocalFile() {
	char inifile[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetModuleFileName(NULL, inifile, _MAX_PATH);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "fsmlocalsys", "ini");
	fsmini = inifile;
}

int CPProfile::GetInt(CString& section, CString& name, int defvalue) {
	return GetPrivateProfileInt(section, name, defvalue, winini);
}

int CPProfile::GetIntP(CString& section, CString& name, int defvalue) {
	return GetPrivateProfileInt(section, name, defvalue, fsmini);
}

void CPProfile::PutInt(CString& section, CString& name, int value) {
	CString t;
	t.Format("%d", value);
	WritePrivateProfileString(section, name, t, winini);
}

void CPProfile::PutIntP(CString& section, CString& name, int value) {
	CString t;
	t.Format("%d", value);
	WritePrivateProfileString(section, name, t, fsmini);
}