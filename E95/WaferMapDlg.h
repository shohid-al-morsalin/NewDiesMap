#pragma once
// WaferMapDlg.h : header file
//

#include "WaferMap.h"

/////////////////////////////////////////////////////////////////////////////
// CWaferMapDlg dialog

class CWaferMapDlg : public CDialog {
	// Construction
public:
	CWaferMapDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWaferMapDlg)
	enum { IDD = IDD_WAFERMAP };
	CWaferMap	m_cWaferMap;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWaferMapDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWaferMapDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	//}}AFX_MSG
// 	long OnDeletepoint(WPARAM wP, LPARAM lP);
// 	long OnUpdatePoint(WPARAM wP, LPARAM lP);
// 	long OnRelocateToBack(WPARAM wP, LPARAM lP);
// 	long OnEditpoint(WPARAM wP, LPARAM lP);
// 	long OnAddpoint(WPARAM wP, LPARAM lP);
// 	LONG OnAddalgnpoint(WPARAM wP, LPARAM lP);
// 	long OnGotopoint(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
