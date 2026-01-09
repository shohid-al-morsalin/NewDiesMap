#if !defined(AFX_COMTHREAD_H__F8682F17_C342_44A4_8FCD_C52C430D04ED__INCLUDED_)
#define AFX_COMTHREAD_H__F8682F17_C342_44A4_8FCD_C52C430D04ED__INCLUDED_

//#include "combase.h"
//#include "ComTDK.h"
#include <afxmt.h>

#include <vector>
using namespace std;


#include "..\IPC\SMOL.h"

//using namespace std;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComThread thread
struct CSMOL;
struct CSMHdr;

class CComBase;

class CComThread : public CWinThread/*, public CComTDK*/
{
//	DECLARE_DYNCREATE(CComThread)

	CCriticalSection CS;

public:
	vector <CSMHdr> SMlist;
	BOOL bSimu;
// Operations
	
	BOOL bStarted;
	CString KillName;	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComThread)
	


	void CheckQueOut();

	void CheckComIn();

	BOOL Connect(short port);

	BOOL Disconnect();

		virtual BOOL InitInstance();
	virtual int ExitInstance();

	void WaitTillStart();

	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL
	
	//	void Stop();

	//	void PushButton2();

	BOOL SendMsg(CSMHdr &SM);
	BOOL AddToSMList(CSMHdr &SM);
		
	CComThread();           // protected constructor used by dynamic creation
	virtual ~CComThread();

	// Generated message map functions
	//{{AFX_MSG(CComThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()	
public:
	BOOL Abort();
	void Reply1(CString Caller, CSMOL &SM);
	void Log(CString msg, short lvl);

	BOOL StartCom();

	CString GetSlotMap();

	BOOL GetStatus(BOOL & bRdy, BOOL & bInP, BOOL & bAlr);

	BOOL NoVac();

	BOOL NeedVac();

	BOOL Auto();

	BOOL Reset(BOOL bCalib = FALSE);

	BOOL Calib();

	BOOL Open();

	BOOL Reply(CSMOL &SM, BOOL ret);

	BOOL Reply(CSMHdr &SM, BOOL ret);

	BOOL Close();

    BOOL SetDefault();

	BOOL CloseNoRelease();

	BOOL Map();

	BOOL Unclamp();

	BOOL Clamp();

	BOOL Status();

	BOOL AccessModeChange(LoadPortObj::ACCESSMODE mode);

	BOOL LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState);

	BOOL ClearAllLED();

	BOOL LEDReserved(BOOL on);

	BOOL LEDManual(BOOL on);

	BOOL EnableHOAvbl(short port);

	BOOL LEDAlarm(BOOL on);

private:
	CComBase *pCom;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMTHREAD_H__F8682F17_C342_44A4_8FCD_C52C430D04ED__INCLUDED_)
