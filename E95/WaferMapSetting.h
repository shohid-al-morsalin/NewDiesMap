#pragma once
// WaferMapSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaferMapSetting dialog

class CWaferMapSetting : public CDialog {
	// Construction
public:
	CWaferMapSetting(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaferMapSetting)
	enum { IDD = IDD_SETTING };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaferMapSetting)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaferMapSetting)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
