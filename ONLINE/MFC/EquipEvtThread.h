#pragma once
// EquipEvtThread.h : header file
//

#include "..\GEM\GemFactory.h"
#include "..\ShareMemory\Evt.h"
#include "..\ShareMemory\GemHead.h"
#include "..\ShareMemory\WaitMsgList.h"
#include "..\ShareMemory\SharedMemQue.h"

#include <vector>
#include <list>
#include <algorithm>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CEquipEvtThread thread

class CEquipEvtThread : public CWinThread {
	DECLARE_DYNCREATE(CEquipEvtThread)

	GEM_HEADER* pnGemhead;

protected:
	virtual int Run();

public:
	CEquipEvtThread();           // protected constructor used by dynamic creation

	// Attributes
public:
	int EVTMSG;
	CSharedMemQue	 m_EvtQue;
	CWaitMsgList WaitMsgLst;
	BYTE* pdwBuf;

	// Operations
public:
	void StopThread();
	BOOL OpenEvtQue(char* pName);
	CEvt* Get_Event();
	BOOL EquipEvtProcedure();
	void MainProcedure();
	void PostMessageDlg(int msg, BOOL flag);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEquipEvtThread)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	// Implementation
public:
	void Clear();
	BOOL bDone;
	BOOL bAbort;
	BOOL* pbRun;
	void Log(CString msg, short lvl = 0);
	void GetWaferPickup(CEvt* ehead);

	int Get_MsgLen(BYTE* pBuf);
	long Get_SysByte(BYTE* pBuf);
	BOOL EventResultCedure(BYTE* szBuf, long sysbyte1, CEvt** Event);
	BOOL CheckTimeOut(BYTE* szBuf);
	virtual ~CEquipEvtThread();

	// Generated message map functions
	//{{AFX_MSG(CEquipEvtThread)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	BOOL AbortTrans(UINT AbortSysByte);
};

/////////////////////////////////////////////////////////////////////////////
