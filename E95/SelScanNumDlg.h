#pragma once
// SelScanNumDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelScanNumDlg dialog

class CSelScanNumDlg : public CDialog {
	// Construction
public:
	void ExtractIDs(CString& CarrierID, CString& WaferID, CString filename);
	CString GetC2CFile();
	void ExitCheck();
	CSelScanNumDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSelScanNumDlg)
	enum { IDD = IDD_INPUTSCANNUM };
	CString	m_CarrierID;
	CString	m_WaferID;
	 
	//}}AFX_DATA

	short ScanNum;
	CString filename;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSelScanNumDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelScanNumDlg)
	afx_msg void OnScanfirst();
	afx_msg void OnScansecond();
	afx_msg void OnScanthird();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
