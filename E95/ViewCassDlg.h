#pragma once
// ViewCassDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewCassDlg dialog

class CInfoPanelView;

class CViewCassDlg : public CDialog {
	// Construction
public:
	CInfoPanelView* m_Parent;
	CViewCassDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CViewCassDlg)
	enum { IDD = IDD_VIEWCASSDLG };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewCassDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewCassDlg)
	afx_msg void OnLoad();
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
