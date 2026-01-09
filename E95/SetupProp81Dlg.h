#pragma once
// SetupProp81Dlg.h : header file
//

#include "SetupPropComm.h"
#include "SICam.h"
#include "ImgWnd.h"
#include "ChartWnd.h"

#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp81Dlg dialog

class CCoor;
class CMPoint;
class CSICam;
class CInfoPanelMain;

class CSetupProp81Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
	enum { ECHO_CAMERA, THIN_FILM_CAMERA };
	int activeCamera;
public:
	CSetupProp81Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp81Dlg)
	enum { IDD = IDD_SETUPPROP81 };
	CButton	m_cAutoExpose;
	CButton	m_cCalc1;
	CButton	m_cCalc6;
	CButton	m_cCalc4;
	CButton	m_cAlgn;
	CListCtrl	m_cList;
	CStatic	m_cMouseMessage;
	CStatic	m_cSelectedPt;
	CListCtrl	m_cPoint;
	CChartWnd	m_cProfile2;
	CImgWnd	m_cImgWnd1;
	CImgWnd	m_cImgWnd2;
	CStatic	m_cMessage;
	CChartWnd	m_cProfile;
	CButton	m_cCameraB;
	CButton	m_cCameraA;
	CSICam	m_cVideo1;
	float	m_Angle1;
	float	m_Angle2;
	float	m_Angle3;
	float	m_Angle4;
	CSICam	m_cVideo2;
	float	m_CXOffset;
	float	m_CYOffset;
	float	m_CR;
	float	m_CX;
	float	m_CY;
	float	m_Bevel;
	short	m_AutoExposeValue;
	float	m_Radius;
	short	m_Probe1;
	CString	m_Name;
	//}}AFX_DATA
public:

	BOOL bRun;
	void LocalToRecipe();
	void RecipeToLocal();

	static UINT RunRecipe(LPVOID pObj);
	static UINT RunRecipeNarrowTrim(LPVOID pObj);
	static UINT RunRecipeWideTrim(LPVOID pObj);
	void SetupColumnPos();
	void SetupColumnMessage();
	// 	void GrabOneFrame(CSICam &m_Video, short Exposure);
	void Renumber();
	//	void LoadPoint();
	void Addzposition(CMPoint* pMP);
	void UpdatePoint(CCoor* p);
	void RelocateToTop(CCoor* p);
	void RelocateToBack(CCoor* p);
	void SwitchToEchoCamera();
	void SwitchToThinFilmCamera();
	void PreviousBevelAlgo();
	short sign(long value);
	//	short * Read(short &len);
	//	void Dump(short *pBuf, short len);

	float radius;
	// 	CInfoPanelMain * m_Parent;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp81Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp81Dlg)
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	virtual BOOL OnInitDialog();
	afx_msg void OnCamerasetting();
	afx_msg void OnMotioncontrol();
	afx_msg void OnBut1();
	afx_msg void OnBut2();
	afx_msg void OnBut3();
	afx_msg void OnBut4();
	afx_msg void OnBut5();
	afx_msg void OnBut6();
	afx_msg void OnBut7();
	afx_msg void OnBut8();
	afx_msg void OnBut9();
	afx_msg void OnCalculate();
	afx_msg void OnCalculate2();
	afx_msg void OnSaverecipe();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGo1();
	afx_msg void OnGo2();
	afx_msg void OnGo3();
	afx_msg void OnGo4();
	afx_msg void OnMatchpattern();
	afx_msg void OnCalculate3();
	afx_msg void OnCalculate3_v1();
	afx_msg void OnCalculate4();
	afx_msg void OnCalculate5();
	afx_msg void OnCalculate6();
	afx_msg void OnCalculate7();
	afx_msg void OnClear();
	afx_msg void OnKillfocusCxoffset();
	afx_msg void OnKillfocusCyoffset();
	afx_msg void OnFindcxcy();
	afx_msg void OnKillfocusAutoexposevalue();
	afx_msg void OnDoautoexpose();
	afx_msg void OnKillfocusRadius();
	afx_msg void OnKillfocusProbe1();
	afx_msg void OnKillfocusName1();
	afx_msg void OnViewdata();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCalculate8();
	afx_msg void OnBnClickedCalculate9();
	afx_msg void OnCalculate8();
};
