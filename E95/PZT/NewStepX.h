// NewStep.h: interface for the CNewStep class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWSTEP_H__DBA4D610_EDC8_4C79_8454_23F4ED788E2C__INCLUDED_)
#define AFX_NEWSTEP_H__DBA4D610_EDC8_4C79_8454_23F4ED788E2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern "C"
{
//#include "../Serial.h"
}

class CNewStep
{
public:
	
	CNewStep();
	virtual ~CNewStep();

public:
	BOOL HomeActuator(int nAxis);
	BOOL MoveActuator(int nAxis, int nPosition);
	BOOL Initialize();
	int  GetPosition(int nAxis);
protected:

	BOOL IsHomed(int nAxis);
	BOOL SetAxis(int nAxis);
	SYSTEMTIME AuxilaryStartTime;
	SYSTEMTIME AuxilaryStopTime;
	HWND hWnd;
	HINSTANCE hInstance;
	
	BOOL bRegistered;
	BOOL bBusy;

	BOOL bSimu;
	CSerial Port;
	int nPort;
	int nRepeat;
	int nWaittime;
	int nController;

	char StrOut[550];
	char StrIn[550];
};

#endif // !defined(AFX_NEWSTEP_H__DBA4D610_EDC8_4C79_8454_23F4ED788E2C__INCLUDED_)
