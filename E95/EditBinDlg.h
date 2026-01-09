#pragma once
// EditBinDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditBinDlg dialog

class CEditBinDlg : public CDialog {
	// Construction
public:
	void RefreshDialog();
	CEditBinDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditBinDlg)
	enum { IDD = IDD_EDITBINS };
	short	m_iHisto;
	short	m_Limit;
	//}}AFX_DATA

	int dx;
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEditBinDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditBinDlg)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
