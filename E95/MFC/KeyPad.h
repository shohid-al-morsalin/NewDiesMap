#if !defined(AFX_KEYPAD_H__A09F9006_CFEF_11D1_9EB2_445102C10000__INCLUDED_)
#define AFX_KEYPAD_H__A09F9006_CFEF_11D1_9EB2_445102C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// KeyPad.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CKeyPad dialog

class CKeyPad : public CDialog
{
// Construction
public:
	BOOL Clear;
	void ClearSelect();
	static BOOL Edit (long &value);
	static BOOL Edit (short &value);
	static BOOL Edit (float &value);
	static BOOL Edit (double &value);
	void SetValue (short value);
	void SetValue(double value);
	CKeyPad(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyPad)
	enum { IDD = IDD_KEYPAD };
	CEdit	m_cValue;
	//}}AFX_DATA
	CString	m_Value;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyPad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyPad)
	afx_msg void OnBut0();
	afx_msg void OnBut1();
	afx_msg void OnBut2();
	afx_msg void OnBut3();
	afx_msg void OnBut4();
	afx_msg void OnBut5();
	afx_msg void OnBut6();
	afx_msg void OnBut7();
	afx_msg void OnBut8();
	afx_msg void OnBut9();
	afx_msg void OnButclr();
	afx_msg void OnButd();
	afx_msg void OnButexp();
	afx_msg void OnButsign();
	afx_msg void OnChangeValue();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetfocusValue();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYPAD_H__A09F9006_CFEF_11D1_9EB2_445102C10000__INCLUDED_)
