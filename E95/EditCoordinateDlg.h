#pragma once
// EditCoordinateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditCoordinateDlg dialog

class CEditCoordinateDlg : public CDialog {
	// Construction
public:
	CEditCoordinateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditCoordinateDlg)
	enum { IDD = IDD_EDITCOORDINATES };
	// NOTE: the ClassWizard will add data members here
//}}AFX_DATA

	enum EDITCOOR {
		NOACTION, ZEROZ, OFFSETZ, REVERSEZ, APPLYXYOFFSET
	};
	EDITCOOR cmd;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEditCoordinateDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditCoordinateDlg)
	afx_msg void OnReversez();
	afx_msg void OnApplyXYOffset();
	afx_msg void OnZeroz();
	afx_msg void OnOffsetz();
	afx_msg void OnExport();
	afx_msg void OnExport4();
	afx_msg void OnImport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
