#if !defined(AFX_CHANGETEXTDLG_H__D84091E4_879A_11D3_888D_0020182D5632__INCLUDED_)
#define AFX_CHANGETEXTDLG_H__D84091E4_879A_11D3_888D_0020182D5632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeTextDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CChangeTextDlg dialog

class CChangeTextDlg : public CDialog
{
// Construction
public:
	CChangeTextDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChangeTextDlg)
	enum { IDD = IDD_CHOOSETEXT };
	CString	m_from;
	CString	m_to;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeTextDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangeTextDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGETEXTDLG_H__D84091E4_879A_11D3_888D_0020182D5632__INCLUDED_)
