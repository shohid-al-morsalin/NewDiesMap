#pragma once

#include "WaferMap.h"
#include "SetupPropComm.h"
#include "SRC/ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp7Dlg dialog
class CMPoint;
class CInfoPanelMain;

class CSetupProp7Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	CSetupProp7Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp7Dlg)
	enum { IDD = IDD_SETUPPROP7 };
	CStatic	m_cCoorText2;
	CStatic	m_cCoorText;
	CButton	m_cResetAll;
	CStatic	m_cSelPoint;
	CStatic	m_cMessage;
	CButton	m_cUseThis;
	CButton	m_cUse;
	CEdit	m_cAutoZTarget;
	CButton	m_cMeSet;
	CListCtrl	m_cPoint;
	CWaferMap	m_cWaferMap;
	short	m_NAve = 0;
	short	m_Cln = 0;
	BOOL	m_EnableAutoZ = FALSE;
	float	m_AutoZTarget = 0.0f;
	//}}AFX_DATA

public:
	short m_P[MaXPA] = { 0 };
	short m_F[MaXPA] = { 0 };
	short m_T[MaXPA] = { 0 };
	float m_R[MaXPA] = { 0.0f };
	CString m_N[MaXPA] = { "" };

public:
	void Renumber();
	void PositionThinFilm();
	void PositionEcho();
	void UpdatePoint(CCoor* p);
	void RelocateToTop(CCoor* p);
	void RelocateToBack(CCoor* p);
	void RecipeToLocal();
	void LocalToRecipe();
	// 	void SaveDialog(short MeS);
	// 	void LoadDialog(short MeS);
	void SwapMeSet(short next);
	void Addzposition(CMPoint* pMP);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp7Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp7Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnSet4();
	afx_msg void OnSet5();
	afx_msg void OnSet6();
	afx_msg void OnSet7();
	afx_msg void OnSet8();
	afx_msg void OnSet9();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLocations();
	afx_msg void OnMark();
	afx_msg void OnTest();
	afx_msg void OnHelpme();
	afx_msg void OnMatchpattern();
	afx_msg void OnRenumber();
	afx_msg void OnCoordinates();
	afx_msg void OnEnableautoz();
	afx_msg void OnKillfocusAutoztarget();
	afx_msg void OnKillfocusRcpPeaknear1();
	afx_msg void OnNewrecipe();
	afx_msg void OnRefreshui();
	afx_msg void OnMore();
	afx_msg void OnResetall();
	afx_msg void OnKeydownList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSortdata();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	long OnDuplicate(WPARAM wP, LPARAM lP);
	long OnDeletepoint(WPARAM wP, LPARAM lP);
	long OnRelocateToTop(WPARAM wP, LPARAM lP);
	long OnRelocateToBack(WPARAM wP, LPARAM lP);
	long OnEditpoint(WPARAM wP, LPARAM lP);
	long OnAddpoint(WPARAM wP, LPARAM lP);
	long OnAddalgnpoint(WPARAM wP, LPARAM lP);
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	LRESULT OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	//[[20241213/Sihab
	
	afx_msg void OnBnClickedCheck1();
	//]]
	afx_msg void OnBnClickedMotioncontrol1();
};