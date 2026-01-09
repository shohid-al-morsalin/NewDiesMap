// RbtUnit.h: interface for the CRbtUnit class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "RbtBase.h"

class CComm;

class CRbtUnit : public CRbtBase {
	CComm* pCom;
	CComm* pCom2;//tushar 21082024

	//	BOOL bSimu;

protected:
	BOOL bHomed;
	BOOL bConnected;

	BOOL bConnected2;
public:

	//	CString GetLastError();

	CString GetResp();
	BOOL SendBrk(char* text, int tout = 60000);
	BOOL SendPri(char* text);
	BOOL SendLogosol(char* text);//Tushar 21082024
	BOOL SendLogosolAction(char* text);//Tushar 21082024
	BOOL SetComMon();
	BOOL SendLogosolTest(char* text); // added by tushar just for testing 21082024
	short StopCom();
	CRbtBase::ERRCODE StartCom(short port);
	CRbtBase::ERRCODE StartCom2(short port);//tushar 21082024

public:
	CRbtUnit();
	virtual ~CRbtUnit();

public:
	void Log(CString msg, short lvl = 5);
	BOOL CanProceed(short level);
	virtual BOOL Stop();
	BOOL SendPri2(char* text);
	BOOL IsSimu();
	BOOL IsConnected();
};
