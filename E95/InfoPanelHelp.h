#pragma once
//#include "webbrowser2.h"	// Added by ClassView
#include "acroaxdocshim.h"	// Added by ClassView
#include "ResizableFormView.h"

// CInfoPanelHelp form view

class CInfoPanelHelp : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelHelp)

protected:
	CInfoPanelHelp();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelHelp();

public:
	//	CWebBrowser2 *m_pExpr;
	//	enum VIEWERTYPE { NONE, ADOBEREADER, IEXPLORER, MODZILLA };
	//	VIEWERTYPE ViewerType;
	CAcroAXDocShim* m_pAcro;
	// Dialog Data
	//{{AFX_DATA(CInfoPanelHelp)
	enum { IDD = IDD_INFOPANELHELP };
	//}}AFX_DATA
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CInfoPanelHelp)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CInfoPanelHelp)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnQuickstart();
	afx_msg void OnRecover();
	afx_msg void OnColdstart();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
