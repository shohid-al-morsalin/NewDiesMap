#if !defined(AFX_COLORCHARTSCALEDLG_H__4505C6CE_2363_4730_9BD5_CE0618457B2F__INCLUDED_)
#define AFX_COLORCHARTSCALEDLG_H__4505C6CE_2363_4730_9BD5_CE0618457B2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorChartScaleDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorChartScaleDlg dialog

class CColorChartScaleDlg : public CDialog
{
// Construction
public:
	CColorChartScaleDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CColorChartScaleDlg)
	enum { IDD = IDD_COLORCHARTSCALE };
	float	m_maxValue;
	float	m_minValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorChartScaleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorChartScaleDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCHARTSCALEDLG_H__4505C6CE_2363_4730_9BD5_CE0618457B2F__INCLUDED_)
