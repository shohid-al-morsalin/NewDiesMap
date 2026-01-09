#pragma once

#include "StatusReq.h"
#include "SRC/ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSystem413Dlg dialog

class CSystem413Dlg : public CResizableDialog,
	public CStatusRequest {
	// Construction
public:
	// 	BOOL bStop;
	void Stop();
	void SetDataReqTimeOut(long dur);  //  [10/27/2008 Yuen]
	int GetHWnd();
	void Refresh();
	CSystem413Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSystem413Dlg)
	enum { IDD = IDD_SYSTEM413DLG };
	CStatic	m_cConnection;
	CButton	m_cService;
	CString	m_Service;
	CString	m_Status;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystem413Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSystem413Dlg)
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnService();
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdate();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnIpcDatareqack(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnIpcDatareqnak(WPARAM wP, LPARAM lP);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
