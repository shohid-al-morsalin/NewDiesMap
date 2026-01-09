#pragma once
// EditThickDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditThickDlg dialog

class CEditThickDlg : public CDialog {
	// Construction
public:
	BOOL bNoWaferThk;
	CString Title;
	short m_mode;
	CEditThickDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditThickDlg)
	enum { IDD = IDD_EDITTHICKNESS };
	CEdit	m_cWaferThk;
	CButton	m_cWaferThkBut;
	CStatic	m_cftunit;
	double	m_film;
	double	m_wafer;
	double	m_stressconst;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditThickDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditThickDlg)
	afx_msg void OnFilmthicknessbut();
	afx_msg void OnWaferthicknessbut();
	afx_msg void OnHeat();
	afx_msg void OnNormal();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};