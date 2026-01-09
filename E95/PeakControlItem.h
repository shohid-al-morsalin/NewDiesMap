#pragma once
// PeakControlItem.h : header file
//

class CPeakShape;

/////////////////////////////////////////////////////////////////////////////
// CPeakControlItem dialog

class CPeakControlItem : public CDialog {
	// Construction
public:
	CPeakControlItem(CWnd* pParent = NULL);   // standard constructor

	short PkNo;
	CPeakShape* PS;

	// Dialog Data
		//{{AFX_DATA(CPeakControlItem)
	enum { IDD = IDD_PEAKCONTROLITEM };
	CStatic	m_PeakNumber;
	float	m_Width;
	short	m_Method;
	short	m_Nearest;
	short	m_Thres;
	short	m_Height;
	float	m_Distance;
	float	m_DeltaDistance;
	BOOL	m_Active;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPeakControlItem)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPeakControlItem)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusWidth();
	afx_msg void OnKillfocusThreshold();
	afx_msg void OnKillfocusNearest();
	afx_msg void OnKillfocusMethod();
	afx_msg void OnActive();
	afx_msg void OnKillfocusDistance();
	afx_msg void OnKillfocusDistance2();
	afx_msg void OnKillfocusHeight();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
