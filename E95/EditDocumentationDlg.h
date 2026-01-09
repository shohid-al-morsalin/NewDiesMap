#pragma once
// EditDocumentationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditDocumentationDlg dialog

class CEditDocumentationDlg : public CDialog {
	// Construction
public:
	CEditDocumentationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditDocumentationDlg)
	enum { IDD = IDD_EDITDOCUMENT };
	CString	m_Comment;
	CString	m_LotID;
	CString	m_Operator;
	CString	m_Project;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditDocumentationDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditDocumentationDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
