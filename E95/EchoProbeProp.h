#pragma once
// EchoProbeProp.h : header file
//

#include "MParam.h"

/////////////////////////////////////////////////////////////////////////////
// CEchoProbeProp dialog
class CRecipe413;

class CEchoProbeProp : public CDialog {
public:
	CRecipe413* pRcp;

	short MeSet;
	short m_P[MaXPA];
	short m_F[MaXPA];
	short m_T[MaXPA];
	float m_R[MaXPA];
	float m_H[MaXPA];
	float m_L[MaXPA];
	short m_D[MaXPA];
	float m_DF[MaXPA];
	float m_Off[MaXPA];
	CString m_N[MaXPA];
	CString	m_Calc[MaXPA];

	short MapCol;

	// Construction
public:
	void SaveDialog(short MeS);
	void LoadDialog(short MeS);
	void SwapMeSet(short next);
	CEchoProbeProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEchoProbeProp)
	enum { IDD = IDD_ECHOPROP };
	short	m_NAve;
	//}}AFX_DATA
	float	m_ThresholdFactor[2];
	short	m_Level[2];
	short	m_PeakWidth[2];

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEchoProbeProp)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEchoProbeProp)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnRadio7();
	afx_msg void OnRadio8();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnSet4();
	afx_msg void OnSet5();
	afx_msg void OnSet6();
	afx_msg void OnSet7();
	afx_msg void OnSet8();
	afx_msg void OnSet9();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
