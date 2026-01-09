#pragma once
// SetupProp71.h : header file
//

#include "WaferMap.h"
#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp71Dlg dialog

class CSetupProp71Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	void LocalToRecipe();
	//	void LoadDialog();
	void RecipeToLocal();
	CSetupProp71Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp71Dlg)
	enum { IDD = IDD_SETUPPROP71 };
	CListCtrl	m_cPoint;
	CWaferMap	m_cWaferMap;
	short	m_NumLine;
	short	m_PointPerLine;
	short	m_ScanLength;
	//}}AFX_DATA

	void Renumber();
	void Addzposition(CMPoint* pMP);
	void UpdatePoint(CCoor* p);
	void RelocateToTop(CCoor* p);
	void RelocateToBack(CCoor* p);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp71Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp71Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSaverecipe();
	afx_msg void OnSet();
	afx_msg void OnClear();
	afx_msg void OnKillfocusNumlines();
	afx_msg void OnKillfocusPointsperline();
	afx_msg void OnKillfocusScanlength();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	long OnRelocateToTop(WPARAM wP, LPARAM lP);
	long OnRelocateToBack(WPARAM wP, LPARAM lP);
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	LRESULT OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};