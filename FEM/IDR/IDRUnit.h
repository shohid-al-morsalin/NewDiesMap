// IDRUnit.h: interface for the CIDRUnit class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CComm;
struct CSMOL;
struct CSMHdr;

class CIDRUnit {
public:
	CComm* pCom;

public:
	short port;
	short Unit;
	BOOL bSimul;
	BOOL bDisabled;
	CString ErrorStr;
	short IDR1, IDR2;	// ID of RFID unit [8/6/2010 XPMUser]

	BYTE Parity;
	BYTE bitSize;

public:
	void Log(CString msg, short lvl = 5);
	HWND hWnd;
	void Register(CSMHdr& SM);
	void Enable(BOOL bAction = TRUE);
	BOOL WriteCarrierID(char* id);

	CString GetLastError();
	void SetPort(short port);
	//	void Log(CString msg, short lvl);
	BOOL ReadCarrierID();
	BOOL Stop();
	BOOL Start(short unit, short port);

	virtual BOOL IsDeviceOK();
	virtual BOOL GetDevStatus() = 0;
	virtual BOOL ReadID() = 0;
	virtual CString ReadCID() = 0;
	virtual BOOL WriteID(char* id) = 0;
	virtual char* GetCID(char* cid, short len) = 0;

	CIDRUnit();
	virtual ~CIDRUnit();
};
