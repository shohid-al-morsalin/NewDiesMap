#pragma once

#include "Comm.h"	// Added by ClassView
#include "IPC/Smol.h"

class CCommSvr : public CWinThread {
	DECLARE_DYNCREATE(CCommSvr)

	// Attributes
public:
	BOOL bRun;
	short port;
	CString buff;
	BOOL bStarted;

	CComm* pCom;
	CStringList OutStr;
	CPtrList InStr;
	CCriticalSection CSOut, CSIn;

	// Operations
public:
	BOOL WriteString(char* str, int tout);
	BOOL WriteList(CString& str);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CCommSvr)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
public:
	//	void Log(CString msg, short lvl);
	BOOL DeviceIsFree();
	BOOL SendString(ComMsg* msg);
	BOOL ReadString(CString& msg);
	int Ack;
	HWND HWin;
	BOOL StopUnit();
	BOOL Stop();
	BOOL StartUnit();
	BOOL bSimu;
	CCommSvr();           // protected constructor used by dynamic creation
	virtual ~CCommSvr();

	// Generated message map functions
	//{{AFX_MSG(CCommSvr)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
