#pragma once

// #include <afxmt.h>
#include "IPC/SArams.h"

class CArams {
	CCriticalSection CS;
public:
	BOOL bSvr;
	SArams Attr;
	CTime LastDate;
	CTime StartDate;

public:
	//	void LogState();
	BOOL IsOperatable();
	BOOL IsNonScheduleState();
	BOOL IsEngineeringState();
	void Deinit(CString filename);
	void Init(CString filename);
	CString GetCurrentStateStr();
	SArams::ARAMSSTATE GetCurrentState();
	void SetAttr(SArams& Ar);
	BOOL IsEqpSetDownTime();
	BOOL IsUnscheduledDown();
	void SetCurrentState(SArams::ARAMSSTATE state, BOOL bEqpSet = FALSE);
	BOOL IsProductiveState();
	void UpdateState();
	void Serialize(CArchive& ar);
	void Save(CString filename);
	BOOL Load(CString filename);
	CArams();
	virtual ~CArams();
};

// extern CArams *pARAMS;
