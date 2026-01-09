#if !defined(AFX_KEYBOARD_H__A09F9009_CFEF_11D1_9EB2_445102C10000__INCLUDED_)
#define AFX_KEYBOARD_H__A09F9009_CFEF_11D1_9EB2_445102C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Keyboard.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CKeyboard dialog

class CKeyboard : public CDialog
{
// Construction
public:
	void ClearSelect();
	short m_Caps;
	CString m_Value;
	void SetValue(CString value);
	CKeyboard(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyboard)
	enum { IDD = IDD_KEYBOARD };
	CEdit	m_cValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyboard)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyboard)
	afx_msg void OnSpace();
	afx_msg void OnCaps();
	afx_msg void OnButz();
	afx_msg void OnButy();
	afx_msg void OnButx();
	afx_msg void OnButw();
	afx_msg void OnButv();
	afx_msg void OnButu();
	afx_msg void OnButt9();
	afx_msg void OnButt1();
	afx_msg void OnButt0();
	afx_msg void OnButt();
	afx_msg void OnButslash();
	afx_msg void OnButs();
	afx_msg void OnButr();
	afx_msg void OnButq();
	afx_msg void OnButp();
	afx_msg void OnButo();
	afx_msg void OnButn();
	afx_msg void OnButm();
	afx_msg void OnButl();
	afx_msg void OnButk();
	afx_msg void OnButj();
	afx_msg void OnButi();
	afx_msg void OnButh();
	afx_msg void OnButg();
	afx_msg void OnButf();
	afx_msg void OnBute();
	afx_msg void OnButdot();
	afx_msg void OnButd();
	afx_msg void OnButcolon();
	afx_msg void OnButc();
	afx_msg void OnButbspc();
	afx_msg void OnButb();
	afx_msg void OnButa();
	afx_msg void OnBut8();
	afx_msg void OnBut7();
	afx_msg void OnBut6();
	afx_msg void OnBut5();
	afx_msg void OnBut4();
	afx_msg void OnBut3();
	afx_msg void OnBut2();
	afx_msg void OnKillfocusString();
	afx_msg void OnClear();
	afx_msg void OnQuit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYBOARD_H__A09F9009_CFEF_11D1_9EB2_445102C10000__INCLUDED_)
