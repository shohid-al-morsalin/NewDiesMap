#pragma once

// OnlineMgr.h : header file
//

#include "resource.h"

#include "LogItem.h"
#include "OnlineMgrB.h"
#include "..\SRC\ResizableDialog.h"

class CEstablishComm;

/////////////////////////////////////////////////////////////////////////////
// COnlineMgr dialog

class COnlineMgr : public CResizableDialog, public COnlineMgrB {
	FILE* fp;
	int Cnt;
	CString LogFileName;

	// Construction
public:
	void SECSAbnormal(BYTE* pData, short len, char* pMsg, short* pTime);
	void EstablishConnection();
	void FolderName(CString& FolderName);
	void MakeNewLogFile();
	void SECSContent(BYTE* data, short len, BOOL bLcl, short* pTime);
	short MsgPtr;
	void SECSMessage(CString msg);
	void ShowMsg(CString msg, short lvl = 5);
	CWnd* GetCWnd();
	void Log(CString msg, short lvl = 5);
	void Message(CString msg, short lvl = 5);
	COnlineMgr(CWnd* pParent = NULL);   // standard constructor
	~COnlineMgr();

	// Dialog Data
		//{{AFX_DATA(COnlineMgr)
	enum { IDD = IDD_ONLINEMGR };
	CListBox	m_cSecsMsg;
	//}}AFX_DATA

	CEstablishComm* pEstComm;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(COnlineMgr)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
	HWND GetHWnd();

protected:

	// Generated message map functions
	//{{AFX_MSG(COnlineMgr)
	virtual BOOL OnInitDialog();
	afx_msg void OnMsgEstcomm();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LONG OnAsyncSelect(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};
