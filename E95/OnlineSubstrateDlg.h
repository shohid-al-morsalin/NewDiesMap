#pragma once
// OnlineSubstrateDlg.h : header file
//

#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// COnlineSubstrateDlg dialog

class COnlineSubstrateDlg : public CResizableDialog {
	// Construction
public:
	COnlineSubstrateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COnlineSubstrateDlg)
	enum { IDD = IDD_ONLINESUBSTRATEDLG };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineSubstrateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COnlineSubstrateDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
