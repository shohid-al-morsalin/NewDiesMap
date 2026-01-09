// StatusReq.h: interface for the CStatusReq class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
//#include "..\IPC\128SVar.h"
//#include "..\IPC\900SVar.h"
#include "413SVar.h"

class CInfoPanelMain;

class CStatusRequest {
protected:
	short srID;
	CInfoPanelMain* srParent;
	// 	C128SVar SV128;
	// 	C900SVar SV900;
	C413SVar SV413;

public:
	CString m_L1;
	CString m_L2;
	CString m_L3;
	CString m_L4;
	CString m_L5;
	CString m_L6;
	CString m_L7;
	CString m_L8;
	CString m_L9;
	CString m_L10;
	CString m_L11;
	CString m_L12;
	CString m_Message;

	void DataReqAck413(LPARAM lP);
	virtual void SetDataReqTimeOut(long dur) = 0; //  [10/27/2008 Yuen]
	virtual int GetHWnd() = 0;
	CStatusRequest();
	virtual ~CStatusRequest();
};