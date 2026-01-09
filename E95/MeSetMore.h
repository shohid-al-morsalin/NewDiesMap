#pragma once
// MeSetMore.h : header file
//

#include "MParam.h"

/////////////////////////////////////////////////////////////////////////////
// CMeSetMore dialog

class CMeSetMore : public CDialog {
	// Construction
public:
	CMeSetMore(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMeSetMore)
	enum { IDD = IDD_MESETMORE };
	CButton	m_cMeSet;
	//}}AFX_DATA

	short MeS; // Measurement set [9/9/2012 Administrator]
	short m_D[MaXPA];
	float m_DF[MaXPA];
	float m_NP[MaXPA];
	float m_Off[MaXPA];
	float m_OffX[MaXPA];
	float m_OffY[MaXPA];
	float m_H[MaXPA];
	float m_L[MaXPA];
	CString	m_Calc[MaXPA];
	BOOL m_Rpt[MaXPA];

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMeSetMore)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMeSetMore)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
