#pragma once

class CDefineCircularSegment : public CDialog {
	// Construction
public:
	void Generate();
	void Clear();
	CPtrList rings;
	CDefineCircularSegment(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefineCircularSegment)
	enum { IDD = IDD_DEFINECIRCULARSEG };
	CListCtrl	m_clist;
	float	m_angle;
	float	m_distance;
	short	m_npoint;
	float	m_distance2;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefineCircularSegment)
protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineCircularSegment)
	afx_msg void OnCircCDist();
	afx_msg void OnCircStAngle();
	afx_msg void OnCircNumOfPoints();
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnClear();
	afx_msg void OnCircCDist2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
