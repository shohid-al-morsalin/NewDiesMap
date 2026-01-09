// WaferInfoMgr.h: interface for the CWaferInfoMgr class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
// #include "..\IPC\SMHdr.h"
#include "..\..\IPC\SMOL.h"

#include "WaferInfo.h"

class CNCas;

class CWaferInfoMgr {
public:
	short magic;
	CPtrList winfo;

private:

	/** @link dependency */
	/*#  CWaferInfo lnkCWaferInfo; */
public:
	void SetLotID(short wafernum, CString ID);
	CString GetLotID(short wafernum);
	void SetWaferID(short wafernum, CString ID);
	void SetContentMap(short wafernum, CContentMap& Ct);
	void SetLPStation(short In, short Out);
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();
	short enPort;
	short OutPort;
	short InPort;
	CString GetWaferID(short wafernum);
	void Clear();
	void Add(CWaferInfo* p, BOOL bCreate);
	BOOL Remove(short wafernum);
	CWaferInfo* FindWafer(short wafernum);
	CWaferInfo* AddWafer(short wafernum);

	CWaferInfoMgr();
	virtual ~CWaferInfoMgr();
	void Serialize(CArchive& ar);
	CWaferInfoMgr& operator =(CWaferInfoMgr& co);
};
