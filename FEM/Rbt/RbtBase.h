// RbtBase.h: interface for the CRbtBase class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\..\IPC\Constant.h"

class CRbtBase {
public:

	enum ERRCODE { NOTCONNECTED, CONNECTED, OPENERROR, ALLOCERROR, NOTCREATED, CREATED, SUCCESS };

	CString ID;
	char SlotMap[MAXCAPACITYLEN + 1];
	CString ErrorStr;
	CString ErrorStr2;
	unsigned long WaferAtLoc;

public:
	virtual BOOL SetPostPos(short AlgnNum, long PostPos, long OffsetPos);
	BOOL bSimu;
	virtual int GetDIOIN() {
		CString val;
		val = "Name                 State dio_in               1627389951(0x40ffffff)";
		int v = val.Find(")", 0);
		val = val.Mid(v - 1);
		val.SetAt(val.Find(")"), 0);
		val.MakeUpper();
		char ch = val.GetAt(0);
		if (ch > '9') {
			v = (ch - 'A') + 10;
		}
		else {
			v = ch - '0';
		}
		v = 0xFF & (~v);
		return v;
	}
	void SetWaferSize(short size);

	virtual BOOL Map();
	virtual BOOL Stop();
	virtual BOOL WaferPresentCheck();
	virtual BOOL GetTinyEye();
	virtual BOOL ReqStatus();
	virtual BOOL Xfer(char* text);
	virtual BOOL HomeRobot(short AlgnNum);
	virtual BOOL HomeAlign(short AlgnNum);
	virtual CString GetLastError();
	virtual BOOL Start(short port);
	virtual BOOL IsWaferOnArm();
	virtual BOOL IsWaferOnArm(short arm);
	virtual BOOL IsWaferOnAlgn();
	virtual BOOL IsWaferOnAlgn(short arm);
	virtual BOOL Align();
	virtual BOOL chkResponse();

	virtual BOOL SetComMon();
	CRbtBase();
	virtual ~CRbtBase();

protected:
	short WaferSize;
};