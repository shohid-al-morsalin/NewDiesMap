#if !defined(AFX_EXPORTCHART_H__31C5A035_38AF_49BB_B5B6_6F4831117998__INCLUDED_)
#define AFX_EXPORTCHART_H__31C5A035_38AF_49BB_B5B6_6F4831117998__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportChart.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExportChart dialog

class CExportChart : public CDialog
{
// Construction
public:
	enum EXPORTTYPE { EXPORTNONE, EXPORTNORMAL, EXPORTTABLE, EXPORTIMAGE };
	EXPORTTYPE extype;

	CExportChart(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExportChart)
	enum { IDD = IDD_EXPORTCHART };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportChart)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportChart)
	afx_msg void OnExport();
	afx_msg void OnTable();
	afx_msg void OnBitmap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTCHART_H__31C5A035_38AF_49BB_B5B6_6F4831117998__INCLUDED_)
