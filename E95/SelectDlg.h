#pragma once
// SelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectDlg dialog

class CSelectDlg : public CDialog {
	// Construction
public:
	CSelectDlg(CWnd* pParent = NULL);   // standard constructor

	CStringArray m_saItemList;
	int m_nSel;
	// Dialog Data
		//{{AFX_DATA(CSelectDlg)
	enum { IDD = IDD_DIALOG_SELECT };
	CListBox	m_Items;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectDlg)
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
