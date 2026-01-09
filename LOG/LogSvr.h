#pragma once

#include "IPC/SvrThread.h"
#include "LogMsgList.h"
// #include "CLogMsgItem.h"

#define MAX_LOGFILEENTRY	50000

/////////////////////////////////////////////////////////////////////////////
// CLogSvr thread
class CLogSvr : public CSvrThread {
	BOOL ProcessCmd(CSMHdr& SM);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogSvr)
public:
	virtual BOOL InitInstance();

	//}}AFX_VIRTUAL
	// Implementation
public:
	CLogSvr(); // protected constructor used by dynamic creation
	virtual ~CLogSvr();

	// Generated message map functions
	//{{AFX_MSG(CLogSvr)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
DECLARE_MESSAGE_MAP() private: CLogMsgList LogMsgList;

public:
	HWND hWnd;
	short MsgCount;

	CString LogFileName;
	CString ErrFileName;
	CString WarnFileName;
	CString NoticeFileName;

	void CleanUp();
	void Save(CLogMsgItem* pMsgItem);
	int ChgDirec(CString dir);

	CString MakeNewLogFileName(CString ext);
};
/////////////////////////////////////////////////////////////////////////////