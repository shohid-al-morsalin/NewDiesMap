#if !defined(AFX_DELIMITERDLG_H__22F73862_E0BA_47E1_99B0_55F0ECE56B1E__INCLUDED_)
#define AFX_DELIMITERDLG_H__22F73862_E0BA_47E1_99B0_55F0ECE56B1E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelimiterDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDelimiterDlg dialog

class CDelimiterDlg : public CDialog
{
// Construction
public:
	char term;
	CDelimiterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelimiterDlg)
	enum { IDD = IDD_DELIMITER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelimiterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelimiterDlg)
	afx_msg void OnTab();
	afx_msg void OnSpaces();
	afx_msg void OnComma();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELIMITERDLG_H__22F73862_E0BA_47E1_99B0_55F0ECE56B1E__INCLUDED_)
