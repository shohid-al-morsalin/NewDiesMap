#if !defined(AFX_EDITWAFERDLG_H__7BF345D9_E975_47B6_84D5_AF4FC39BB968__INCLUDED_)
#define AFX_EDITWAFERDLG_H__7BF345D9_E975_47B6_84D5_AF4FC39BB968__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditWaferDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CEditWaferDlg dialog

class CEditWaferDlg : public CDialog
{
// Construction
public:
	short m_wafernum;
	CEditWaferDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditWaferDlg)
	enum { IDD = IDD_EDITWAFER };
	CString	m_ID;
	CString	m_note;
//	double	m_stressconst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditWaferDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditWaferDlg)
	afx_msg void OnWaferidbut();
	virtual BOOL OnInitDialog();
	afx_msg void OnCommentbut();
//	afx_msg void OnStressconstbut();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITWAFERDLG_H__7BF345D9_E975_47B6_84D5_AF4FC39BB968__INCLUDED_)
