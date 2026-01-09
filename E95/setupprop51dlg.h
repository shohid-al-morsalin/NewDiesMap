#pragma once
// SetupProp51Dlg.h : header file
//

#include "SICam.h"
#include "ImgWnd.h"
#include "PattMatch.h"
#include "WaferMap.h"
#include "MPoint.h"
#include "Recipe413.h"	// Added by ClassView

#include "SetupPropComm.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp51Dlg dialog

class CRecipe413;
class CInfoPanelMain;

class CSetupProp51Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	CSetupProp51Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp51Dlg)
	enum { IDD = IDD_SETUPPROP51 };
	CButton	m_cFastMatching;
	CButton	m_cMatchAtOffset;
	CButton	m_cMatch7;
	CButton	m_cMatch6;
	CButton	m_cPaSet;
	CButton	m_cSaveVideo;
	CButton	m_cMatch5;
	CButton	m_cMatch4;
	CButton	m_cFindCxCy;
	CButton	m_cOptimize;
	CButton	m_cGotoAlgnPos;
	CButton	m_cAlignPos;
	CButton	m_cMatch2;
	CListCtrl	m_cList;
	CStatic	m_cMouseMessage;
	CButton	m_cInitialExposureAfterFocus2;
	CButton	m_cInitialFocus2;
	CButton	m_InitialExposureAfterFocus;
	CButton	m_cInitialFocus;
	CButton	m_cInitialExposeB4Focus;
	CStatic	m_cSelectedPt;
	CWaferMap	m_cWaferMap;
	CListCtrl	m_cPoint;
	CButton	m_cMatch;
	CButton	m_cPositionThinFilm;
	CButton	m_cPositionEcho;
	CButton	m_cCameraA;
	CButton	m_cCameraB;
	CImgWnd	m_cImgWnd;
	CImgWnd	m_cMark2;
	CImgWnd	m_cMark1;
	CSICam	m_cVideo;
	float	m_STIOffset;
	float	m_STIOffsetY;
	float	m_ODOffset;
	float	m_ODOffsetY;
	float	m_CX;
	float	m_CY;
	float	m_CR;
	float	Bevel;
	//}}AFX_DATA

	short nSel;

public:
	void LocalToRecipe();
	void RecipeToLocal();

	void UpdateSelectionItem(CMPoint* pMP);
	void SwapMeSet(short next);
	void DoMeasurement();
	static UINT ExecuteTF(LPVOID pObj);
	void GotoXYZ(CCoor* p, BOOL bZ);
	void OptimizeRecipe(CRecipe413* pRcp);
	void SetupColumn();
	void Renumber();
	void RelocateToTop(CCoor* p);
	void RelocateToBack(CCoor* p);
	void Addzposition(CMPoint* pMP);
	void UpdatePoint(CCoor* p);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp51Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp51Dlg)
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	afx_msg void OnCamerasetting();
	afx_msg void OnMotioncontrol();
	afx_msg void OnMatch();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnPositionecho();
	afx_msg void OnPositionthinfilm();
	afx_msg void OnLoadrecipe();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMatch2();
	afx_msg void OnInitialexposurebeforefocus();
	afx_msg void OnInitialfocus();
	afx_msg void OnInitialexposureafterfocus();
	afx_msg void OnInitialfocus2();
	afx_msg void OnInitialexposureafterfocus2();
	afx_msg void OnMark();
	afx_msg void OnMatchpattern();
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAlignmentpos();
	afx_msg void OnGotoalignmentpos();
	afx_msg void OnMicroscopelighton();
	afx_msg void OnMicroscopelightoff();
	afx_msg void OnPatset1();
	afx_msg void OnPatset2();
	afx_msg void OnPatset3();
	afx_msg void OnPatset4();
	afx_msg void OnPatset5();
	afx_msg void OnPatset6();
	afx_msg void OnPatset7();
	afx_msg void OnPatset8();
	afx_msg void OnPatset9();
	afx_msg void OnImg1();
	afx_msg void OnImg2();
	afx_msg void OnImg3();
	afx_msg void OnImg4();
	afx_msg void OnImg5();
	afx_msg void OnImg6();
	afx_msg void OnKillfocusOdoffset();
	afx_msg void OnKillfocusOdoffsety();
	afx_msg void OnKillfocusStioffset();
	afx_msg void OnKillfocusStioffsety();
	afx_msg void OnClearlist();
	afx_msg void OnMatch4();
	afx_msg void OnTestRun();
	afx_msg void OnFindcxcy();
	afx_msg void OnOptimize();
	afx_msg void OnSavevideo2();
	afx_msg void OnFocus();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnSet4();
	afx_msg void OnSet5();
	afx_msg void OnSet6();
	afx_msg void OnSet8();
	afx_msg void OnSet9();
	afx_msg void OnSet11();
	afx_msg void OnKeydownList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUp();
	afx_msg void OnDn();
	afx_msg void OnHere();
	afx_msg void OnAddzoffset();
	afx_msg void OnUpdatept();
	afx_msg void OnMatch6();
	afx_msg void OnMatch7();
	afx_msg void OnMatchatoffset();
	afx_msg void OnPatsseta();
	afx_msg void OnPatssetb();
	afx_msg void OnPatssetc();
	afx_msg void OnPatssetd();
	afx_msg void OnFastmatching();
	//}}AFX_MSG
	long OnDeletepoint(WPARAM wP, LPARAM lP);
	long OnRelocateToTop(WPARAM wP, LPARAM lP);
	long OnRelocateToBack(WPARAM wP, LPARAM lP);
	long OnEditpoint(WPARAM wP, LPARAM lP);
	long OnAddpoint(WPARAM wP, LPARAM lP);
	long OnAddalgnpoint(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};