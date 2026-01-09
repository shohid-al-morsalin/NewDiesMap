#pragma once
#include "MPoint.h"	// Added by ClassView

class CSelectMeasurementDlg : public CDialog {
public:
	CMPoint* pMP;
	CSelectMeasurementDlg(CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_SELMEASUREMENT };
	float	m_fValOffset;	// value offset [10/6/2020 zhimi]
	short	m_MeSet;
	short	m_PatSet;
	BOOL	m_ApplyAll;
	CString	m_Range;

	short PtNo;
	BOOL bClearPoint;
	BOOL bDeletePoint;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:

	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeletepoint();
	afx_msg void OnClear();
	afx_msg void OnKillfocusEdit1();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillfocusValueOffset();
};
