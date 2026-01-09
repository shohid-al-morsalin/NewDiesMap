#if !defined(AFX_EDITSTRESSCONSTDLG_H__5E85AD89_ED2B_411C_B53F_AE4062423825__INCLUDED_)
#define AFX_EDITSTRESSCONSTDLG_H__5E85AD89_ED2B_411C_B53F_AE4062423825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditStressConstDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditStressConstDlg dialog

class CEditStressConstDlg : public CDialog
{
// Construction
public:
	short wafernum;
	CEditStressConstDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditStressConstDlg)
	enum { IDD = IDD_EDITSTRESSCONST };
	CComboBox	m_stresstable;
	double	m_stressconst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditStressConstDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditStressConstDlg)
	virtual void OnOK();
	afx_msg void OnStressconstbut();
	afx_msg void OnSelchangeStresstable();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSTRESSCONSTDLG_H__5E85AD89_ED2B_411C_B53F_AE4062423825__INCLUDED_)
