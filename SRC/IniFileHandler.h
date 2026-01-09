// IniFileHandler.h: interface for the CIniFileHandler class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "IniFile.h"	// Added by ClassView

// NOTE: THIS CALSS MUST BE INSTANTIATE GLOBALLY, IT IS NOT TREAD OR PROCESS SAVE
class CIniFileHandler {
	string IniFilename;
public:
	void Set(string key, string section, int val);
	int Get(string key, string section, int deflt);
	void Set(string key, string section, float val);
	float Get(string key, string section, float deflt);
	void Set(string key, string section, string val);
	string Get(string key, string section, string deflt);
	CIniFile IniFile;
	CIniFileHandler(string IFile);
	virtual ~CIniFileHandler();
};
