#pragma once
#include "resource.h"

// ParaEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParaEditDlg dialog

class CParaEditDlg : public CDialog {
public:

	CString m_Value;
	CString m_ParaName;
	CString m_ParaRangeMin;
	CString m_ParaRangeMax;
	CString m_ParaDesc;

	// Construction
public:
	CParaEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParaEditDlg)
	enum { IDD = IDD_PARAEDIT };
	CEdit	m_CDesc;
	CEdit	m_CRangeMax;
	CEdit	m_CParameter;
	CEdit	m_CRange;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParaEditDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParaEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedCancel2();
};
