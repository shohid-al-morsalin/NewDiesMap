#pragma once

#include "ProcJobCommon.h"
#include "StatusReq.h"
// #include "IPC/Smol.h"	// Added by ClassView

#include "SRC/ResizableDialog.h"

struct SPrJobExec;
class CInfoPanelMain;

/////////////////////////////////////////////////////////////////////////////
// CProcJobQue413Dlg dialog

class CProcJobQue413Dlg : public CResizableDialog,
	public CStatusRequest, public CProcJobCommon {
public:
	CInfoPanelMain* m_Parent;

	/**
	* Indicate which process job
	*/
	short ID;
	short CJMONTimeout;
	BOOL bOutOfServiceAlarmSet;
	short AreYouThereRetryCount;

	enum CANSTARTSTATE {
		NOTREADY, OKTOSTART, ABORT
	};

	// Construction
public:
	void StartProcessRequest();
	void SetDataReqTimeOut(long dur);
	int GetHWnd();

	void Message(CString msg, short lvl = 5);
	void Log(CString msg, short lvl = 5);
	BOOL AreYouThere();
	CANSTARTSTATE CanStart();

	CProcJobQue413Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcJobQue413Dlg)
	enum { IDD = IDD_PROCJOBQUE413DLG };
	CButton	m_cProceed;
	CButton	m_cCancel;
	CListBox m_cList;
	CString	m_Status;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobQue413Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJobQue413Dlg)
	afx_msg void OnCancel();
	afx_msg void OnProceed();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnIpcHelloack();
	afx_msg LRESULT OnIpcDatareqack(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnIpcDatareqnak(WPARAM wP, LPARAM lP);
	//}}AFX_MSG
	long OnSvrCancelcarrier(WPARAM wP, LPARAM lP);
	long OnSvrCancelcarrieratport(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrPjdefine(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnSvrCjdefine(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
