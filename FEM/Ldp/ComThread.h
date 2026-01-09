#pragma once

#include <vector>

#include "LPObj.h"

struct CSMOL;
struct CSMHdr;
class CComBase;

class CComThread : public CWinThread/*, public CComTDK*/
{
	//	DECLARE_DYNCREATE(CComThread)

	CCriticalSection CS;

public:
	std::vector <CSMHdr> SMlist;
	BOOL bSimu;
	BOOL bRun;
	// Operations

	BOOL bStarted;
	CString KillName;
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CComThread)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

	//	void Stop();

	//	void PushButton2();

	void WaitTillStart();
	void CheckQueOut();
	void CheckComIn();
	BOOL Connect(short port);
	BOOL Disconnect();
	BOOL SendMsg(CSMHdr& SM);
	BOOL AddToSMList(CSMHdr& SM);

	CComThread();           // protected constructor used by dynamic creation
	virtual ~CComThread();

	// Generated message map functions
	//{{AFX_MSG(CComThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	BOOL Abort();
	void Reply1(CString Caller, CSMOL& SM);
	void Log(CString msg, short lvl = 5);

	BOOL StartCom(short unit);

	CString GetSlotMap();

	BOOL GetStatus(BOOL& bRdy, BOOL& bInP, BOOL& bAlr);

	BOOL NoVac();

	BOOL NeedVac();

	BOOL Auto();

	BOOL Reset(BOOL bCalib = FALSE);

	BOOL Calib();

	BOOL Open();

	BOOL Reply(CSMOL& SM, BOOL ret);

	BOOL Reply(CSMHdr& SM, BOOL ret);

	BOOL Close();

	BOOL SetDefault();

	BOOL CloseNoRelease();

	BOOL Map();

	BOOL Unclamp();

	BOOL Clamp();

	BOOL Status();

	BOOL AccessModeChange(CLPObj::ACCESSMODE mode);

	//	BOOL LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState);

	BOOL ClearAllLED();

	BOOL LEDReserved(BOOL on);

	BOOL LEDManual(BOOL on);

	BOOL EnableHOAvbl();

	BOOL EnableButtonReport();

	BOOL LEDAlarm(BOOL on);

	CComBase* pCom;
};

/////////////////////////////////////////////////////////////////////////////
