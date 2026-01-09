// Dump.cpp: implementation of the CDump class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "Dump.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDump::CDump() {
	fp = NULL;
	bOK = TRUE;
	NewFile();
}

CDump::~CDump() {
	if (fp) {
		fclose(fp);
	}
}

// Create new dump file [3/30/2013 Yuen]
void CDump::NewFile() {
	if (fp) {
		fclose(fp);
	}

	CString str;
	str.Format("C:\\LOG\\D%s.LOG", CTime::GetCurrentTime().Format("%m%d-%H%M%S"));
	fp = fopen(str, "a+");
	if (!fp) {
		bOK = FALSE;
	}
}

void CDump::D(CString& msg) {
	if (!bOK) {
		// by passed [3/30/2013 Yuen]
		return;
	}
	if (!fp) {
		NewFile();
		if (!bOK) {
			return;
		}
	}
	fprintf(fp, "%s %s\r\n", CTime::GetCurrentTime().Format("%d-%H%M%S").GetBuffer(), msg.GetBuffer());
	fflush(fp);
}

void CDump::D(short N) {
	switch (N) {
	case 1:
		D(T1);
		break;
	case 2:
		D(T2);
		break;
	default:
		D(T0);
		break;
	}
}

void CDump::D(char* msg) {
	CString str = msg;
	D(str);
}