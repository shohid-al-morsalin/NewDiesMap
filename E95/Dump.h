// Dump.h: interface for the CDump class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CDump {
	FILE* fp;

public:
	BOOL bOK;
	CString T0, T1, T2;

public:
	void D(char* msg);
	void D(short N);
	void D(CString& msg);
	void NewFile();
	CDump();
	virtual ~CDump();
};
