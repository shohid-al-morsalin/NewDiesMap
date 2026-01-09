#if !defined(AFX_EDITLEGENDDLG_H__D84091E0_879A_11D3_888D_0020182D5632__INCLUDED_)
#define AFX_EDITLEGENDDLG_H__D84091E0_879A_11D3_888D_0020182D5632__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditLegendDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditLegendDlg dialog
#define MAXPLOTABLELINE  128

class CLnLsit;

class CEditLegendDlg : public CDialog
{
// Construction
public:
	short lut[MAXPLOTABLELINE];
	CLnLsit *pP;
	CEditLegendDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditLegendDlg)
	enum { IDD = IDD_EDITLEGEND };
	CListBox	m_legend;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditLegendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditLegendDlg)
	afx_msg void OnDblclkLegend();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITLEGENDDLG_H__D84091E0_879A_11D3_888D_0020182D5632__INCLUDED_)
