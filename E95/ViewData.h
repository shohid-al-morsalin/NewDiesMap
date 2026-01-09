#pragma once
// ViewData.h : header file
//

#include "MParam.h"

class CMPoint;
class CRecipe413;

/////////////////////////////////////////////////////////////////////////////
// CViewData dialog

class CViewData : public CDialog {
	// Construction
public:
	CViewData(CWnd* pParent = NULL);   // standard constructor

	short nCjName;
	CString dwCjName[MAXMEPASET * MaXPA];

	void UpdateReport();
	CPtrList* GetMPList();
	void UpdateReportPoint(CRecipe413* pRcp, CMPoint* p);

	// Dialog Data
		//{{AFX_DATA(CViewData)
	enum { IDD = IDD_VIEWDATA };
	CListCtrl	m_cReport;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewData)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewData)
	virtual BOOL OnInitDialog();
	afx_msg void OnClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
