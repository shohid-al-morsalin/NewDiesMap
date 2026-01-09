#pragma once
// XTabCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXTabCtrl window
#include <afxtempl.h>

#define WM_XTABSELECTED		12987
#define WM_XTABDESELECTED	12988

class CXTabCtrl : public CTabCtrl {
	// Construction
public:
	CXTabCtrl();

	// Attributes
public:
	short curTab;
	BOOL bUIBlocked;

	// Operations
public:
	void AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage = 0);
	void AddTab(CWnd* pWnd, LPTSTR lpszCaption, int iImage, int cx, int cy);
	void EnableTab(int iIndex, BOOL bEnable = TRUE);
	void EnableAllTabs(BOOL bEnable = TRUE);
	BOOL SelectTab(int iIndex);
	void DeleteAllTabs();
	void DeleteTab(int iIndex);
	void SetTopLeftCorner(CPoint pt);
	BOOL IsTabEnabled(int iIndex);

	void SetDisabledColor(COLORREF cr);
	void SetSelectedColor(COLORREF cr);
	void SetNormalColor(COLORREF cr);
	void SetMouseOverColor(COLORREF cr);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CXTabCtrl)
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetUIState(BOOL bSt);
	void ChangeCaption(CString name/*, int curTab*/);
	BOOL ReselectTab(int iIndex);
	int FindTabIndex(CWnd* pWnd);
	void PostXTabSelected(CWnd* pWnd, int iTab);
	void PostXTabDeselected(CWnd* pWnd, int iTab);
	void ActivateCurrentTab();
	void DeactivateCurrentTab();
	BOOL SelectNextTab(BOOL bForward);
	BOOL SelectTabByName(LPCTSTR lpszTabCaption);
	short GetTabIndexByName(LPCTSTR lpszTabCaption);
	void ChangeTab(int iIndex, CWnd* pNewTab, LPTSTR lpszCaption, int iImage);
	void ChangeTab(int iIndex, CWnd* pNewTab, LPTSTR lpszCaption, int iImage, int cx, int cy);
	virtual ~CXTabCtrl();

	// Generated message map functions
protected:
	CArray<BOOL, BOOL> m_arrayStatusTab; //** enabled Y\N
	int m_iSelectedTab;
	POINT m_ptTabs;
	COLORREF m_crSelected;
	COLORREF m_crDisabled;
	COLORREF m_crNormal;
	COLORREF m_crMouseOver;

	int m_iIndexMouseOver;

	bool m_bMouseOver;
	bool m_bColorMouseOver;
	bool m_bColorNormal;
	bool m_bColorDisabled;
	bool m_bColorSelected;

	//{{AFX_MSG(CXTabCtrl)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
