#pragma once

class CDefineGrid : public CDialog {
	// Construction
public:
	CDefineGrid(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefineGrid)
	enum { IDD = IDD_DEFINEGRID };
	float	m_lineX;
	float	m_lineY;
	BOOL	m_DeletePrev;
	BOOL	m_UseMask;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefineGrid)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineGrid)
	afx_msg void OnLinesX();
	afx_msg void OnLinesY();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};