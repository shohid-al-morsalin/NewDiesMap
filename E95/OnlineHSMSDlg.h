#pragma once
#include "afxcmn.h"
#include "..\SRC\ResizableDialog.h"

// COnlineHSMSDlg dialog

class COnlineHSMSDlg : public CResizableDialog {
	//	DECLARE_DYNAMIC(COnlineHSMSDlg)
	void Init_Para_ListCtrl();
	BOOL ParaVal_Setting(CString& pName, CString& pVal, CString& RangeMin, CString& RangeMax);
	BOOL ParaDB_Saving(CString pNo, CString pVal);
	void Init_DataBase();
	void FlexCtrlInt();

public:
	COnlineHSMSDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COnlineHSMSDlg();

	// Dialog Data
		//{{AFX_DATA(COnlineHSMSDlg)
	enum { IDD = IDD_ONLINEHSMSDLG };
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COnlineHSMSDlg)
protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void Log(CString msg, short lvl = 5);
	// Generated message map functions
	//{{AFX_MSG(COnlineHSMSDlg)
	afx_msg void OnLvnItemchangedParalist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkParalist(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// List of all HSMS parameters
	CListCtrl m_ParaList;
	virtual BOOL OnInitDialog();
};
