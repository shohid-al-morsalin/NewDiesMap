#pragma once

// #include "SICam.h"
#include "SetupPropComm.h"
#include "SRC/ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp9Dlg dialog

class CInfoPanelMain;

class CSetupProp9Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	void RecipeToLocal();
	void LocalToRecipe();
	CSetupProp9Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp9Dlg)
	enum { IDD = IDD_SETUPPROP9 };
	CButton	m_cCameraA;
	CButton	m_cCameraB;
	CSICam	m_cVideo;
	CStatic	m_cWaferPresent;
	CEdit	m_cCurFFUPress;
	CEdit	m_cFFUPress;
	CButton	m_cSpecPowerOn;
	CButton	m_cSpecPowerOff;
	CButton	m_cSpecIllumOn;
	CButton	m_cSpecIllumOff;
	CButton	m_cConfiguration;
	CEdit	m_cThinFilmZPos;
	CEdit	m_cEchoTopZPos;
	CButton	m_cRecPos2;
	CButton	m_cRecPos1;
	CButton	m_cMicroscopeLightOn;
	CButton	m_cSpectroLightOn;
	CButton	m_cSpectroLightOff;
	CButton	m_cMicroscopeLightOff;
	CButton	m_cReverseSignal;
	CButton	m_cReverseAB;
	CButton	m_cEchoMotor;
	CButton	m_cLoadPort2;
	CButton	m_cCIDR2;
	CButton	m_cCIDR;
	CButton	m_cStage;
	CButton	m_cThinFilm;
	CButton	m_cRobot;
	CButton	m_cLoadPort;
	CButton	m_cEchoProbe;
	float	m_EchoTopZPos;
	float	m_ThinFilmZPos;
	float	m_TFScalingFactor;
	short	m_EchoAESteps;
	short	m_TFilmAESteps;
	float	m_TFilmAFSteps;
	float	m_EchoAFSteps;
	float	m_ScaleEcho;
	float	m_ScaleTF;
	int		m_Count;
	short	m_BaseExpoEcho;
	short	m_BaseExpoTF;
	float	m_ResetFreq;
	short	m_LevelTop;
	short	m_LevelBot;
	short	m_WidthTop;
	short	m_WidthBot;
	float	m_GainEchoCam;
	float	m_GainThinFCam;
	short	m_OffsetEchoCam;
	short	m_OffsetThinFCam;
	// [04262022 ZHIMING
	CButton m_AutoStartLatching;
	// 03152024
	CButton m_StageCheckWaferPresent;
	// ]
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
protected:
	afx_msg void OnChuckvacon();
	afx_msg void OnChuckvacoff();
	afx_msg void OnMicroscopelighton();
	afx_msg void OnMicroscopelightoff();
	afx_msg void OnSpectrometerlighton();
	afx_msg void OnSpectrometerlightoff();
	virtual BOOL OnInitDialog();
	afx_msg void OnEchoprobe();
	afx_msg void OnThinfilm();
	afx_msg void OnStage();
	afx_msg void OnRobot();
	afx_msg void OnLoadport();
	afx_msg void OnCidr();
	afx_msg void OnCidr2();
	afx_msg void OnEchomotor();
	afx_msg void OnLoadport2();
	afx_msg void OnReveerseab();
	afx_msg void OnReveersesignal();
	afx_msg void OnKillfocusZpos1();
	afx_msg void OnKillfocusZpos2();
	afx_msg void OnReczpos1();
	afx_msg void OnReczpos2();
	afx_msg void OnConfiguration();
	afx_msg void OnSpectrometerillumon();
	afx_msg void OnSpectrometerillumoff();
	afx_msg void OnSpectrometerpowon();
	afx_msg void OnSpectrometerpowoff();
	afx_msg void OnFfuread();
	afx_msg void OnSavemarker();
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	afx_msg void OnKillfocusTfaesteps();
	afx_msg void OnKillfocusEchoaesteps();
	afx_msg void OnKillfocusTfilmafsteps();
	afx_msg void OnKillfocusEchoafsteps();
	afx_msg void OnResetrnd();
	afx_msg void OnMotioncontrol6();
	afx_msg void OnCamerasetting6();
	afx_msg void OnKillfocusScaleecho();
	afx_msg void OnKillfocusScaletf();
	afx_msg void OnVideocamera();
	afx_msg void OnGetcount();
	afx_msg void OnKillfocusBaseexpecho();
	afx_msg void OnKillfocusBaseexpthinf();
	afx_msg void OnKillfocusResetfreq();
	afx_msg void OnKillfocusLeveloffbot();
	afx_msg void OnKillfocusLevelofftop();
	afx_msg void OnKillfocusWidthofftop();
	afx_msg void OnKillfocusWidthoffbot();
	afx_msg void OnKillfocusGainechocam();
	afx_msg void OnKillfocusGainthinfcam();
	afx_msg void OnKillfocusOffsetechocam();
	afx_msg void OnKillfocusOffsetthinfcam();
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnRedXHair(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	float m_FFUPressure;
	afx_msg void OnBnClickedLpAutoStartLatching();
	afx_msg void OnBnClickedSyssettingStageCheckWaferPresence();
};