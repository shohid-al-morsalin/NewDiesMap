// ComBase.h: interface for the CComBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMBASE_H__A017B2D7_BF2F_4CB0_B21A_61D91EE79DE3__INCLUDED_)
#define AFX_COMBASE_H__A017B2D7_BF2F_4CB0_B21A_61D91EE79DE3__INCLUDED_

#include <afxmt.h>

//#include "QueOutMgr.h"
#include "..\IPC\SMOL.h"
//#include "..\..\IPC\SMHdr.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CComm;
struct CSMOL;
struct CSMHdr;

class CComBase  
{
public:
	virtual BOOL Abort();

	BOOL SendTDK(CString msg, int Rpy);

	char * GetResp();

	CString SvrName;
	HWND hWndLPBar;
	HWND hWnd;
	BOOL bReady;
	BOOL bInPlace;
	BOOL bAlarmed;
	char Slotmap[32];
	BOOL bSimu;
	BOOL bStarted;
	
	void Log(CString msg, short lvl);

	BOOL Reply(CSMOL &SM, BOOL ret);

	BOOL Reply(CSMHdr &SM, BOOL ret);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMIFThread)
//	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComThread)
//	
	//}}AFX_VIRTUAL

	void WaitTillStart();
	virtual BOOL GetRespLFFoup(char *msg, short max, int tout);
	virtual BOOL SendMsg(CSMHdr &SM) =0;
	virtual void Decode(CString &msg) =0;
	virtual BOOL SetDefault() =0;
	BOOL SendHCA(CString msg, int tout);

	BOOL Connect(short port);
	BOOL Disconnect();
	BOOL SendAck(CString msg, int tout);
	void QueOutAdd(CString msg);
	BOOL Read(CString msg, char *text, short max, int tout);

	BOOL ReadTDK(const CString &msg, char *text, short max);

// Attributes
	
//	BOOL AddToSMList(CSMHdr &SM);

//	void CheckQueOut();

	void CheckComIn();

	virtual BOOL NoVac();

	virtual BOOL NeedVac();

	virtual BOOL Auto();

	virtual BOOL Reset(BOOL bCalib = FALSE);

	virtual BOOL Calib();

	virtual BOOL Open();

	virtual BOOL Close();

	virtual BOOL CloseNoRelease();

	virtual BOOL Map();

	virtual BOOL Unclamp();

	virtual BOOL Clamp();

	virtual BOOL Status();

	virtual BOOL AccessModeChange(LoadPortObj::ACCESSMODE mode);

	virtual BOOL LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState);

	virtual BOOL ClearAllLED();

	virtual BOOL LEDReserved(BOOL on);

	virtual BOOL LEDManual(BOOL on);

	virtual BOOL LEDAlarm(BOOL on);

	virtual BOOL EnableHOAvbl(short port);

	CComBase();
	virtual ~CComBase();

private:
	CComm * pCom;
//	CQueOutMgr QueOut;
protected:
	CCriticalSection CS;
};

#endif // !defined(AFX_COMBASE_H__A017B2D7_BF2F_4CB0_B21A_61D91EE79DE3__INCLUDED_)
