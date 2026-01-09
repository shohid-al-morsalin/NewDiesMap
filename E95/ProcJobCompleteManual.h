#pragma once
// ProcJobCompleteManual.h : header file
//

#include "..\SRC\ResizableDialog.h"

class CInfoPanelMain;

/////////////////////////////////////////////////////////////////////////////
// CProcJobCompleteManual dialog

class CProcJobCompleteManual : public CResizableDialog {
public:
	short ID;
	CInfoPanelMain* m_Parent;

	// Construction
public:
	void ProcessingComplete();
	void Message(CString msg, short lvl = 5);
	void Log(CString msg, short lvl);
	CProcJobCompleteManual(CWnd* pParent = NULL);   // standard constructor
	~CProcJobCompleteManual();

	// Dialog Data
		//{{AFX_DATA(CProcJobCompleteManual)
	enum { IDD = IDD_PROCJOBCOMPLETEMANUAL };
	CButton	m_cDone;
	CListBox	m_cList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJobCompleteManual)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJobCompleteManual)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
