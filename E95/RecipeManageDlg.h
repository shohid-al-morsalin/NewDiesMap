#pragma once
// RecipeManageDlg.h : header file
//

#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CRecipeManageDlg dialog

class CRecipeManageDlg : public CResizableDialog {
	// Construction
public:
	void EnaDlgItems(BOOL bEn);
	void LockView();
	BOOL bLock;
	CRecipeManageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRecipeManageDlg)
	enum { IDD = IDD_RECIPEMANAGEDLG };
	CButton	m_cLogin;
	CButton	m_cRefresh;
	CButton	m_cDelete;
	CListBox	m_cList;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecipeManageDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecipeManageDlg)
	afx_msg void OnDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnRefresh();
	afx_msg void OnDblclkList1();
	afx_msg void OnLogin();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
