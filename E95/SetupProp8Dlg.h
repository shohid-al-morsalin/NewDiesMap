#pragma once
// SetupProp8Dlg.h : header file
//

#include "SetupPropComm.h"
#include "SICam.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp8Dlg dialog

class CInfoPanelMain;

class CSetupProp8Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	void SetupImageFlip();
	void RecipeToLocal();
	void LocalToRecipe() {};
	CSetupProp8Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp8Dlg)
	enum { IDD = IDD_SETUPPROP8 };
	CStatic	m_cMessage2;
	CStatic	m_cMessage;
	CButton	m_cCameraSetting;
	CListBox	m_cSpeed;
	CButton	m_cGotoZeroXY;
	CButton	m_cCameraB;
	CButton	m_cCameraA;
	CSICam	m_cVideo;
	float	TFdX;
	float	ECHdX;
	float	TFdY;
	float	ECHdY;
	float	m_MLocX1;
	float	m_MLocX2;
	float	m_MLocY1;
	float	m_MLocY2;
	float	m_ASizeThinFilm;
	float	m_ASizeEcho;	// Actual length of standard sample [4/15/2013 Administrator]
	float	TFmX;
	float	ECHmX;
	float	TFmY;
	float	ECHmY;
	//}}AFX_DATA

// 	HANDLE m_hLiveThread1;
// 	HANDLE m_hLiveThread2;

	float dx;
	float GLocX, GLocY, RLocXT, RLocYT, RLocXE, RLocYE;  // Backup cross hair location, green & red [6/2/2011 XPMUser]
	float GLocX1, GLocY1, RLocX1T, RLocY1T, RLocX1E, RLocY1E;  // Cross hair location, green & red [6/2/2011 XPMUser]

// 	short ActiveCamera;
// 	void StopCamera(short No);
// 	void StartCamera(short No);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupProp8Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp8Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	afx_msg void OnCamerasetting();
	afx_msg void OnMotioncontrol();
	afx_msg void OnFirstleft();
	afx_msg void OnFirstright();
	afx_msg void OnSecondleft();
	afx_msg void OnSecondright();
	afx_msg void OnFirsttop();
	afx_msg void OnFirstbottom();
	afx_msg void OnSecondtop();
	afx_msg void OnSecondbottom();
	afx_msg void OnKillfocusSpeed();
	afx_msg void OnSetscreen2motor();
	afx_msg void OnSetpixel2micron();
	afx_msg void OnMarkred();
	afx_msg void OnMarkgreen();
	afx_msg void OnSetpixel2microny();
	afx_msg void OnKillfocusOffsetx2();
	afx_msg void OnKillfocusOffsety2();
	afx_msg void OnKillfocusOffsetx();
	afx_msg void OnKillfocusOffsety();
	afx_msg void OnCrosshair();
	afx_msg void OnSetscreen2mmotor2();
	afx_msg void OnGotozeroxy();
	afx_msg void OnTakepicture();
	afx_msg void OnThirdtop();
	afx_msg void OnThirdbottom();
	afx_msg void OnThirdleft();
	afx_msg void OnThirdright();
	afx_msg void OnSetptmecho();
	afx_msg void OnSetptmtf();
	afx_msg void OnReversey();
	afx_msg void OnReverseytf();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};