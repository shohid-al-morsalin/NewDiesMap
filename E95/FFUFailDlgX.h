#if !defined(AFX_FFUFAILDLG_H__4B263A31_28C9_45B4_8A47_0C78BC557F56__INCLUDED_)
#define AFX_FFUFAILDLG_H__4B263A31_28C9_45B4_8A47_0C78BC557F56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FFUFailDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFFUFailDlg dialog

class CFFUFailDlg : public CDialog
{
// Construction
public:
	void Close();

	long ChkDelay, CfnDelay;
	BOOL bOnScreen;
	static float FFUPressNow;
	CFFUFailDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFFUFailDlg)
	enum { IDD = IDD_FFUFAIL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFFUFailDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void PostNcDestroy();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFFUFailDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FFUFAILDLG_H__4B263A31_28C9_45B4_8A47_0C78BC557F56__INCLUDED_)
