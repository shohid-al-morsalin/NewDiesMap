#if !defined(AFX_RECIPEDLG_H__A5A69F71_D5A3_482E_9117_C01E15C70282__INCLUDED_)
#define AFX_RECIPEDLG_H__A5A69F71_D5A3_482E_9117_C01E15C70282__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RecipeDlg.h : header file
//

#include "..\..\RCP\Recipe.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CRecipeDlg dialog

class CRecipeDlg : public CDialog
{
// Construction
public:
	void EnableDlgItems();
	static CString FileExText(int Cause);
	BOOL CheckRCPFolder(CString &RcpFolder);
	CRecipeDlg(CWnd* pParent = NULL);   // standard constructor
	CRecipe Rcp;
	BOOL bEditing;
// Dialog Data
	//{{AFX_DATA(CRecipeDlg)
	enum { IDD = IDD_RECIPE };
	CComboBox	m_WaferDia;
	CString	m_RecipeName;
	CString	m_CreatorName;
	UINT	m_ScanSize;
	UINT	m_ScanLines;
	UINT	m_ScanNo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecipeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecipeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonHostUpload();
	afx_msg void OnLogin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RECIPEDLG_H__A5A69F71_D5A3_482E_9117_C01E15C70282__INCLUDED_)
