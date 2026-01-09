#pragma once
// SimuSpecDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimuSpecDlg dialog

class CSimuSpecDlg : public CDialog {
	// Construction
public:
	CSimuSpecDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSimuSpecDlg)
	enum { IDD = IDD_TFSIMULATE };
	//}}AFX_DATA
	float	m_Phase[4];
	float	m_Cycle[4];
	float	m_Amp[4];
	short Spec;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSimuSpecDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimuSpecDlg)
	afx_msg void OnOk();
	afx_msg void OnOk2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
