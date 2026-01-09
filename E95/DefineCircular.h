#pragma once

#include "resource.h"

class CDefineCircular : public CDialog {
	// Construction
public:
	void Generate();
	void Clear();
	CPtrList rings;
	~CDefineCircular();
	CDefineCircular(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefineCircular)
	enum { IDD = IDD_DEFINECIRCULAR };
	CListCtrl	m_clist;
	float	m_angle;
	float	m_distance;
	short	m_npoint;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefineCircular)
protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineCircular)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	afx_msg void OnClear();
	afx_msg void OnCircCDist();
	afx_msg void OnCircStAngle();
	afx_msg void OnCircNumOfPoints();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
