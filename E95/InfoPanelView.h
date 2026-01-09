#pragma once

// CInfoPanelView form view

#include "ResizableFormView.h"
#include "XTabCtrl.h"

class CViewFileDlg;
class CViewCassDlg;
class CViewContentDlg413;
class CViewContentDlgBase;

class CInfoPanelView : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelView)

protected:
	CInfoPanelView();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelView();

public:
	CPtrList m_View;
	CXTabCtrl m_tabctrl;
	CViewFileDlg* m_FileDlg, * m_FileDlg413;
	CViewCassDlg* m_ViewCassDlg;
	CViewContentDlg413* m_ViewContentDlg413;

	CString m_FileToLoad, m_DirToLoad;

public:
	//{{AFX_DATA(CInfoPanelView)
	enum { IDD = IDD_INFOPANELVIEW };
	//}}AFX_DATA
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanelView)
protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CInfoPanelView)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void Load413File(WPARAM wP);
	void AddView(CViewContentDlgBase* pView);
	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);
	BOOL DeleteView(CViewContentDlgBase* pView);
	void ClearAllView();

	LRESULT OnLoadFile(WPARAM wP, LPARAM lP);
	LRESULT OnCloseView(WPARAM wP, LPARAM lP);

protected:
};