#pragma once
// ManualAlgnDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManualAlgnDlg dialog
#include "SICam.h"
#include "413Confg.h"

class CManualAlgnDlg : public CDialog {
	// Construction
public:
	CManualAlgnDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CManualAlgnDlg)
	enum { IDD = IDD_MANUALALIGNMENT };
	CButton	m_cOK;
	CStatic	m_cMouseMessage;
	CButton	m_cAutoFocus;
	CStatic	m_cImgWnd;
	CSICam	m_cVideo;
	CString	m_CarrierID;
	CString	m_WaferID;
	CString	m_LotID;
	CString	m_Operator;
	//[[20251022_Mohir
	CString	m_WaferType;
	CString	m_WaferSize;
	CString	m_Station;
	//]]

	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManualAlgnDlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	float dx;
	// 	CCamBase::CAMERAID ActiveCamera;

public:
	CRecipe413* pRcp;
	void StopCamera(/*CCamBase::CAMERAID No*/);
	void StartCamera(CCamBase::CAMERAID No);

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CManualAlgnDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnAutofocus1();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnZup();
	afx_msg void OnZdown();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnZup2();
	afx_msg void OnZdown2();
	//}}AFX_MSG
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
