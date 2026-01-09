#pragma once
// SetupProp5Dlg.h : header file
//

#include "SetupPropComm.h"
#include "SICam.h"
#include "ImgWnd.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp5Dlg dialog

class CInfoPanelMain;

class CSetupProp5Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	CSetupProp5Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp5Dlg)
	enum { IDD = IDD_SETUPPROP5 };
	CImgWnd	m_cMarkD2;
	CImgWnd	m_cMarkD1;
	CButton	m_cPaSet4;
	CImgWnd	m_cImgWnd;
	CListCtrl	m_cList;
	CButton	m_cRed2;
	CButton	m_cRed;
	CButton	m_cPaSet1;
	CButton	m_cPaSet2;
	CButton	m_cPaSet3;
	CStatic	m_cMessage;
	CEdit	m_ciAutoExpose;
	CButton	m_cMatch;
	CButton	m_cAutoExposure;
	CButton	m_cAutoFocus;
	CButton	m_cMeasure;
	CListBox	m_cSpeed;
	CStatic	m_cMouseMessage;
	CImgWnd	m_cMarkA1;
	CImgWnd	m_cMarkA2;
	CImgWnd	m_cMarkB1;
	CImgWnd	m_cMarkB2;
	CImgWnd	m_cMarkC1;
	CImgWnd	m_cMarkC2;
	CButton	m_cCameraB;
	CButton	m_cCameraA;
	CSICam	m_cVideo;
	short	m_TemplateSize;
	short	m_AutoExpose;
	short	m_TemplateSize2;
	BOOL	m_Enhance;
	BOOL	m_BWTemplate;
	//}}AFX_DATA

public:
	float dx;
	// 	float fTGtGrnX,fTGtGrnY;
	// 	float fTGtBluX, fTGtBluY;
	// 	CInfoPanelMain * m_Parent;

public:
	void ClearRed();
	void LocalToRecipe();
	void RecipeToLocal();

	void UpdateMessage();
	void PositionThinFilm();
	void PositionEcho();

	void SetA1(int n);
	void SetA2(int n);
	void SetA3(int n);
	void SetA4(int n);

	void SetB1(int n);
	void SetB2(int n);
	void SetB3(int n);
	void SetB4(int n);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp5Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp5Dlg)
	afx_msg void OnMotioncontrol();
	virtual BOOL OnInitDialog();
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	afx_msg void OnClose();
	afx_msg void OnPositionecho();
	afx_msg void OnPositionthinfilm();
	afx_msg void OnSeta1();
	afx_msg void OnSeta2();
	afx_msg void OnSeta3();
	afx_msg void OnSetb1();
	afx_msg void OnSetb2();
	afx_msg void OnSetb3();
	afx_msg void OnClear1();
	afx_msg void OnClear2();
	afx_msg void OnClear3();
	afx_msg void OnCamerasetting();
	afx_msg void OnKillfocusTemplatesize();
	afx_msg void OnLoadrecipe();
	afx_msg void OnFirsttop();
	afx_msg void OnFirstbottom();
	afx_msg void OnFirstleft();
	afx_msg void OnFirstright();
	afx_msg void OnSecondtop();
	afx_msg void OnSecondbottom();
	afx_msg void OnSecondleft();
	afx_msg void OnSecondright();
	afx_msg void OnKillfocusSpeed();
	afx_msg void OnMeasuretf();
	afx_msg void OnAutofocus1();
	afx_msg void OnAutoexpose();
	afx_msg void OnMatch();
	afx_msg void OnCrosshair();
	afx_msg void OnMicroscopelighton();
	afx_msg void OnMicroscopelightoff();
	afx_msg void OnThirdtop();
	afx_msg void OnThirdbottom();
	afx_msg void OnThirdleft();
	afx_msg void OnThirdright();
	afx_msg void OnPatset1();
	afx_msg void OnPatset2();
	afx_msg void OnPatset3();
	afx_msg void OnPatset4();
	afx_msg void OnPatset5();
	afx_msg void OnPatset6();
	afx_msg void OnPatset7();
	afx_msg void OnPatset8();
	afx_msg void OnPatset9();
	afx_msg void OnKillfocusExposure4();
	afx_msg void OnKillfocusTemplatesize2();
	afx_msg void OnRedtgt();
	afx_msg void OnPptemplate();
	afx_msg void OnBwtemplate();
	afx_msg void OnRedtgt2();
	afx_msg void OnPatmethod1();
	afx_msg void OnPatmethod2();
	afx_msg void OnPatmethod4();
	afx_msg void OnSeta4();
	afx_msg void OnSetb4();
	afx_msg void OnClear4();
	afx_msg void OnRedtgt3();
	afx_msg void OnPatsseta();
	afx_msg void OnPatssetb();
	afx_msg void OnPatssetc();
	afx_msg void OnPatssetd();
	afx_msg void OnPatmethod5();
	afx_msg void OnClear11();
	afx_msg void OnClear22();
	afx_msg void OnClear32();
	afx_msg void OnClear42();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnBlueXHair(WPARAM wP, LPARAM lP);
	long OnGreenXHair(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};