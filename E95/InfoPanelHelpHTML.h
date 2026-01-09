#pragma once
#include "webbrowser2.h"	// Added by ClassView
#include "ResizableFormView.h"
// InfoPanelHelpHTML.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelHelpHTML form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CInfoPanelHelpHTML : public CResizableFormView {
protected:
	CInfoPanelHelpHTML();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CInfoPanelHelpHTML)

	// Form Data
public:
	//{{AFX_DATA(CInfoPanelHelpHTML)
	enum { IDD = IDD_INFOPAANELHELPHTML };
	CWebBrowser2 m_cBrowser;
	//}}AFX_DATA

// Attributes
public:

	// Operations
public:
	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CInfoPanelHelpHTML)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CInfoPanelHelpHTML();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CInfoPanelHelpHTML)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPrev();
	afx_msg void OnNext();
	afx_msg void OnHome();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnCommandStateChange(long Command, BOOL Enable);

	DECLARE_EVENTSINK_MAP()
public:
	afx_msg void OnStnClickedFrame();
};

/////////////////////////////////////////////////////////////////////////////
