#if !defined(AFX_GETNUMBER_H__2E7B9C0F_4894_40FD_8A15_5CC859690233__INCLUDED_)
#define AFX_GETNUMBER_H__2E7B9C0F_4894_40FD_8A15_5CC859690233__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetNumber.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetNumber dialog

class CGetNumber : public CDialog
{
// Construction
public:
	CString title;
	
	CGetNumber(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetNumber)
	enum { IDD = IDD_GETNUMBER };
	CString	m_Prompt;
	float	m_Value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetNumber)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetNumber)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETNUMBER_H__2E7B9C0F_4894_40FD_8A15_5CC859690233__INCLUDED_)
