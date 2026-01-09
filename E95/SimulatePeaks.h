#pragma once
// SimulatePeaks.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimulatePeaks dialog

class CSimulatePeaks : public CDialog {
	// Construction
public:
	CSimulatePeaks(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSimulatePeaks)
	enum { IDD = IDD_SIMULATEDPEAKS };
	CListCtrl	m_cList2;
	CListCtrl	m_cList1;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimulatePeaks)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimulatePeaks)
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDefault();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDefault2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
