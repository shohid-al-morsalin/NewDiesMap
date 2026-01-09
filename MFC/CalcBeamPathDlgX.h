#if !defined(AFX_CALCBEAMPATHDLG_H__83AF3B04_0B27_4F3C_BB29_81C7BACB8A01__INCLUDED_)
#define AFX_CALCBEAMPATHDLG_H__83AF3B04_0B27_4F3C_BB29_81C7BACB8A01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalcBeamPathDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CCalcBeamPathDlg dialog

class CCalcBeamPathDlg : public CDialog
{
// Construction
public:
	CCalcBeamPathDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCalcBeamPathDlg)
	enum { IDD = IDD_CALCBEAMPATH };
	float	m_Radius;
	short	m_Wafer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalcBeamPathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCalcBeamPathDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALCBEAMPATHDLG_H__83AF3B04_0B27_4F3C_BB29_81C7BACB8A01__INCLUDED_)
