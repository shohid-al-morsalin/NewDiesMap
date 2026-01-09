#pragma once
// PeakControl.h : header file
//

class CRecipe413;

/////////////////////////////////////////////////////////////////////////////
// CPeakControl dialog

class CPeakControl : public CDialog {
	// Construction
public:
	void AcceptChanges();
	void LoadLists();
	void SetToGlobal(short PrbNo);
	CPeakControl(CWnd* pParent = NULL);   // standard constructor

	CRecipe413* pRcp;

	// Dialog Data
		//{{AFX_DATA(CPeakControl)
	enum { IDD = IDD_PEAKCONTROL };
	CButton	m_cAllOrNothing;
	CButton	m_cEqual2;
	CButton	m_cEqual1;
	CListCtrl	m_cList2;
	CListCtrl	m_cList1;
	short	m_NPeaks1;
	short	m_NPeaks2;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeakControl)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPeakControl)
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDefault();
	afx_msg void OnDefault2();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
