// IniFileHandler.cpp: implementation of the CIniFileHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFileHandler.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFileHandler::CIniFileHandler(string IFile) {
	if (IniFile.Create(IFile)) {
		IniFilename = IFile;
	}
	else {
		IniFilename = "";  // Indicate creation failure
	}
}

CIniFileHandler::~CIniFileHandler() {}

string CIniFileHandler::Get(string key, string section, string deflt) {
	if (IniFilename.length() > 0) {
		if (IniFile.RecordExists(key, section, IniFilename)) {
			return IniFile.GetValue(key, section, IniFilename);
		}
		IniFile.SetValue(key, deflt, section, IniFilename);
		return deflt;
	}
	return deflt;
}

void CIniFileHandler::Set(string key, string section, string val) {
	IniFile.SetValue(key, val, section, IniFilename);
}

float CIniFileHandler::Get(string key, string section, float deflt) {
	if (IniFilename.length() > 0) {
		if (IniFile.RecordExists(key, section, IniFilename)) {
			return atof(IniFile.GetValue(key, section, IniFilename).c_str());
		}
		CString str;
		str.Format("%f", deflt);
		string val;
		val = str;
		IniFile.SetValue(key, val, section, IniFilename);
		return deflt;
	}
	return deflt;
}

void CIniFileHandler::Set(string key, string section, float val) {
	CString str;
	str.Format("%f", val);
	string sval;
	sval = str;
	IniFile.SetValue(key, sval, section, IniFilename);
}

int CIniFileHandler::Get(string key, string section, int deflt) {
	if (IniFilename.length() > 0) {
		if (IniFile.RecordExists(key, section, IniFilename)) {
			return atof(IniFile.GetValue(key, section, IniFilename).c_str());
		}
		CString str;
		str.Format("%d", deflt);
		string val;
		val = str;
		IniFile.SetValue(key, val, section, IniFilename);
		return deflt;
	}
	return deflt;
}

void CIniFileHandler::Set(string key, string section, int val) {
	CString str;
	str.Format("%d", val);
	string sval;
	sval = str;
	IniFile.SetValue(key, sval, section, IniFilename);
}