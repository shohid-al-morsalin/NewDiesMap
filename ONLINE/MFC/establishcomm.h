#pragma once
// establishcomm.h : header file
//
#include "resource.h"
#include "..\GEM\GemFactory.h"

/////////////////////////////////////////////////////////////////////////////
// CEstablishComm dialog

class CEstablishComm : public CDialog {
	// Construction
public:
	void Log(CString msg, short lvl = 0);
	void Message(CString msg);
	void EstablishCommunication();
	CEstablishComm(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEstablishComm)
	enum { IDD = IDD_ESTABLISH_COMM };
	CListCtrl	m_clist;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEstablishComm)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//CGemFactory GemFactory;

	// Generated message map functions
	//{{AFX_MSG(CEstablishComm)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
