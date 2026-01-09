#pragma once
// SetupProp73Dlg.h : header file
//
#include "SetupPropComm.h"
#include "SICam.h"
#include "MParam.h"
#include "WaferMap.h"
#include "ImgWnd.h"
#include "..\SRC\ResizableDialog.h"
#include "DiceView.h"
#include "XYPair.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp73Dlg dialog
class CSetupProp73Dlg : public CResizableDialog, public CSetupPropComm {
	short Sel;
	POINT coord;
	float aX, aY, dOX, dOY;
	CStatic	m_cPos[3];
	CStatic m_cCoor;
	CDiceView m_cDiceWafer;
	CXYPair dX, dY;
	BOOL m_bRedrawDiceMap;
	CSiteItem* m_MarkerItems[3];

	// Construction
public:
	BOOL GetPatLocation(SDoPattrnMatching& Param);
	void GenerateMap();
	void CalcdXdY();
	void UpdateMeasurementPoints();
	void SetCellItem(short idx, float x, float y);
	void SelectDice(float fX, float fY);
	bool IsInDice(float fX, float fY, float X, float Y);
	void ReloadDiceWafer();
	void UpdatePointPos(short n);
	void Renumber();
	void LocalToRecipe();
	void RecipeToLocal();
	CSetupProp73Dlg(CWnd* pParent = NULL);   // standard constructor
	void SelectMarker();

	// Dialog Data
		//{{AFX_DATA(CSetupProp73Dlg)
	enum { IDD = IDD_SETUPPROP73 };
	CButton	m_cSetP3;
	CButton	m_cSetP2;
	CButton	m_cSetP1;
	CButton	m_cTestExposure;
	CButton	m_cOptimize;
	CButton	m_cCenter;
	CImgWnd	m_cImgWnd1;
	CButton	m_cMatch;
	CButton	m_cPointLoc;
	CListCtrl	m_cPoints;
	CListCtrl	m_cCells;
	CSICam	m_cVideo;
	CButton	m_cCameraA;
	CButton	m_cCameraB;
	CWaferMap	m_cWaferMap;
	float	m_Radius;
	CString	m_Pos3;
	CString	m_Pos2;
	CString	m_Pos1;
	BOOL	m_bIncludeOn;
	float	m_ZPos;
	short	m_AutoExpose;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp73Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp73Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSaverecipe();
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	afx_msg void OnCamerasetting4();
	afx_msg void OnMotioncontrol2();
	afx_msg void OnGoto1();
	afx_msg void OnGoto2();
	afx_msg void OnGoto3();
	afx_msg void OnSetp1();
	afx_msg void OnSetp2();
	afx_msg void OnSetp3();
	afx_msg void OnAddP1();
	afx_msg void OnAddP2();
	afx_msg void OnAddP3();
	afx_msg void OnGenerate();
	afx_msg void OnRclickCells(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCleardice();
	afx_msg void OnClickCells(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusRadius();
	afx_msg void OnMatch();
	afx_msg void OnCenter();
	afx_msg void OnAddpoint();
	afx_msg void OnClearpts();
	afx_msg void OnOrigin();
	afx_msg void OnCameraecho();
	afx_msg void OnCamerathinf();
	afx_msg void OnGenmap();
	afx_msg void OnClearmap();
	afx_msg void OnSelectall();
	afx_msg void OnClearall();
	afx_msg void OnExcludencomplete();
	afx_msg void OnOptimize();
	afx_msg void OnKillfocusZpositionSp4();
	afx_msg void OnReczpos();
	afx_msg void OnKillfocusExposure3();
	afx_msg void OnTestexposure();
	afx_msg void OnAnchor();
	afx_msg void OnApply();
	//}}AFX_MSG
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	LRESULT OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnAddalgnpoint(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	afx_msg void OnStnClickedDicewafer();
	afx_msg void OnStnDblclickDicewafer();
	afx_msg void OnStnClickedCoor();
	DECLARE_MESSAGE_MAP()
};