// PProfile.h: interface for the CPProfile class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CPProfile {
public:
	void PutIntP(CString& section, CString& name, int value);
	void PutInt(CString& section, CString& name, int value);
	int GetIntP(CString& section, CString& name, int defvalue);
	int GetInt(CString& section, CString& name, int defvalue);
	void MakeLocalFile();
	void MakeSystemFile();
	CString fsmini;
	CString winini;

	CPProfile();
	virtual ~CPProfile();
};