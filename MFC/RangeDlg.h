#pragma once

// RangeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRangeDlg dialog

class CRangeDlg : public CDialog {
	// Construction
public:
	short lock;
	short ID;
	CRangeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRangeDlg)
	enum { IDD = IDD_RANGE };
	double	m_max;
	double	m_min;
	long	m_div;
	//}}AFX_DATA
	double	bk_max;
	double	bk_min;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CRangeDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRangeDlg)
	afx_msg void OnMaximumbut();
	afx_msg void OnMinimumbut();
	afx_msg void OnDivisionbut();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
