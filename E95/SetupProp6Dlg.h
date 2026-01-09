#pragma once
// SetupProp6.h : header file
//

#include "SetupPropComm.h"
#include "SICam.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp6Dlg dialog

class CInfoPanelMain;

class CSetupProp6Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	float radius;  // temp variable [8/7/2010 XPMUser]
	float XFac, YFac, EchoTopZFac, RoughZFac;
	CSetupProp6Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp6Dlg)
	enum { IDD = IDD_SETUPPROP6 };
	CButton	m_cRightP;
	CButton	m_cLeftP;
	CButton	m_cReconnect;
	CButton	m_cCOffset;
	CButton	m_cPosRoughZ;
	CButton	m_cPosEchoTopZ;
	CStatic	m_cMessage;
	CListBox	m_cLogMsg;
	CEdit	m_cOffsetP;
	CEdit	m_cPolarityP;
	CEdit	m_cScaleP;
	CEdit	m_cCurP;
	CEdit	m_cSpdP;
	CEdit	m_cAccP;
	CEdit	m_cRoughOffset;
	CEdit	m_cRoughPolarity;
	CEdit	m_cRoughScale;
	CEdit	m_cRoughCur;
	CEdit	m_cRoughSpd;
	CEdit	m_cRoughAcc;
	CButton	m_cDownRoughZ;
	CButton	m_cUpRoughZ;
	CEdit	m_cEchoTopOffset;
	CEdit	m_cEchoTopPolarity;
	CEdit	m_cEchoTopScale;
	CEdit	m_cEchoTopCur;
	CEdit	m_cEchoTopSpd;
	CEdit	m_cEchoTopAcc;
	CButton	m_cDownEchoTopZ;
	CButton	m_cUpEchoTopZ;
	CButton	m_cCameraSetting;
	CButton	m_cResetP;
	CButton	m_cResetY;
	CButton	m_cResetX;
	CButton	m_cRoughReset;
	CButton	m_cEchoTopReset;
	CButton	m_cThinFilmPos;
	CButton	m_cEchoPos;
	CButton	m_cResetAll;
	CButton	m_cResetXY;
	CComboBox	m_cRadius;
	CButton	m_cCameraB;
	CButton	m_cCameraA;
	CSICam	m_cVideo;
	CStatic	m_cMessage2;
	CComboBox	m_YStep;
	CComboBox	m_RoughZStep;
	CComboBox	m_EchoTopZStep;
	CComboBox	m_XStep;
	float	m_AccEchoTop;
	float	m_AccRough;
	float	m_AccX;
	float	m_AccP;
	float	m_CurEchoTop;
	float	m_CurRough;
	float	m_CurX;
	float	m_CurP;
	float	m_SpdEchoTop;
	float	m_SpdRough;
	float	m_SpdX;
	float	m_SpdP;
	double	m_ScaleEchoTop;
	double	m_ScaleRough;
	double	m_ScaleX;
	double	m_ScaleP;
	BOOL	m_PolarityEchoTop;
	BOOL	m_PolarityRough;
	BOOL	m_PolarityX;
	BOOL	m_PolarityP;
	float	m_HomeEchoTop;
	float	m_HomeRough;
	float	m_HomeX;
	float	m_HomeP;
	long	m_MaxEchoTop;
	long	m_MaxRough;
	long	m_MaxX;
	long	m_MaxP;
	long	m_MinEchoTop;
	long	m_MinRough;
	long	m_MinX;
	long	m_MinP;
	float	m_CurY;
	float	m_AccY;
	float	m_HomeY;
	long	m_MaxY;
	long	m_MinY;
	BOOL	m_PolarityY;
	double	m_ScaleY;
	float	m_SpdY;
	int		m_OffsetEchoTop;
	int		m_OffsetRough;
	int		m_OffsetX;
	int		m_OffsetY;
	int		m_OffsetP;
	float	m_XBacklash;
	float	m_YBacklash;
	float	m_ZBacklash;
	float	m_CXOffset;
	float	m_CYOffset;
	// [05132024 ZHIMING
	CEdit	m_cEchoBottomOffset;
	CEdit	m_cEchoBottomPolarity;
	CEdit	m_cEchoBottomScale;
	CEdit	m_cEchoBottomCur;
	CEdit	m_cEchoBottomSpd;
	CEdit	m_cEchoBottomAcc;
	CButton	m_cDownEchoBottomZ;
	CButton	m_cUpEchoBottomZ;
	CComboBox	m_EchoBottomZStep;
	CButton	m_cEchoBottomReset;
	CButton	m_cPosEchoBottomZ;
	float	m_AccEchoBottom;
	float	m_CurEchoBottom;
	float	m_SpdEchoBottom;
	double	m_ScaleEchoBottom;
	BOOL	m_PolarityEchoBottom;
	float	m_HomeEchoBottom;
	long	m_MaxEchoBottom;
	long	m_MinEchoBottom;
	int		m_OffsetEchoBottom;
	float	EchoBottomZFac;
	// ]

	// [ 06112024/MORSALIN  --> For CWL
	CEdit	m_cCWLOffset;
	CEdit	m_cCWLPolarity;
	CEdit	m_cCWLScale;
	CEdit	m_cCWLCur;
	CEdit	m_cCWLSpd;
	CEdit	m_cCWLAcc;
	CButton	m_cDownCWLZ;
	CButton	m_cUpCWLZ;
	CComboBox	m_CWLZStep;
	CButton	m_cCWLReset;
	CButton	m_cPosCWLZ;
	float	m_AccCWL;
	float	m_CurCWL;
	float	m_SpdCWL;
	double	m_ScaleCWL;
	BOOL	m_PolarityCWL;
	float	m_HomeCWL;
	long	m_MaxCWL;
	long	m_MinCWL;
	int		m_OffsetCWL;
	float	CWLZFac;
	// ]
	
	//}}AFX_DATA

public:
	BOOL bBusy;

	void Log(CString msg, short lvl = 5);
	static void CALLBACK OnMessageProc(int wParam, LPVOID lParam);
	void PositionEcho();
	void Gotozeroxy();
	void UpdatePosition();
	void SaveMotorParam();
	void SetMotorParam();
	void RecipeToLocal();
	void LocalToRecipe();
	// 	CInfoPanelMain * m_Parent;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp6Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp6Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSet();
	afx_msg void OnResetEchoTopZ();
	afx_msg void OnResetRoughZ();
	afx_msg void OnResetCWLTopZ(); // 06112024/MORSALIN
	afx_msg void OnResetX();
	afx_msg void OnResetP();
	afx_msg void OnUpzechotop();
	afx_msg void OnUpzrough();
	afx_msg void OnUpzTopCWL();
	afx_msg void OnUp();
	afx_msg void OnDownzechotop();
	afx_msg void OnDownzrough();
	afx_msg void OnDownzTopCWL();
	afx_msg void OnDown();
	afx_msg void OnLeft();
	afx_msg void OnRight();
	afx_msg void OnSelchangeeEchotopZstep();
	afx_msg void OnSelchangeXystep();
	afx_msg void OnSelchangeRoughZstep();
	afx_msg void OnSelchangeCWLTopZstep();
	afx_msg void OnEcho();
	afx_msg void OnThinfilm();
	afx_msg void OnAllstop();
	afx_msg void OnResetY();
	afx_msg void OnSelchangeXystep2();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGotozeroxy();
	afx_msg void OnGotoloadpos();
	afx_msg void OnResetall();
	afx_msg void OnBut1();
	afx_msg void OnBut2();
	afx_msg void OnBut3();
	afx_msg void OnBut4();
	afx_msg void OnGotodppos();
	afx_msg void OnCameraa();
	afx_msg void OnCamerab();
	afx_msg void OnClose();
	afx_msg void OnMotioncontrol2();
	afx_msg void OnCamerasetting();
	afx_msg void OnGotobgpos();
	afx_msg void OnResetxy();
	afx_msg void OnAdvancesetup();
	afx_msg void OnBut5();
	afx_msg void OnBut6();
	afx_msg void OnBut7();
	afx_msg void OnBut8();
	afx_msg void OnBut9();
	afx_msg void OnSelchangeRadius();
	afx_msg void OnChuckvacon2();
	afx_msg void OnChuckvacoff2();
	afx_msg void OnPosx();
	afx_msg void OnPosy();
	afx_msg void OnPosz1();
	afx_msg void OnPosz2();
	afx_msg void OnPosCWLTopZ();
	afx_msg void OnAlignment();
	afx_msg void OnWaitforxy();
	afx_msg void OnWaitforall();
	afx_msg void OnGetposall();
	afx_msg void OnKillfocusCxoffset();
	afx_msg void OnKillfocusCyoffset();
	afx_msg void OnSavecx();
	afx_msg void OnReconnect();
	afx_msg void OnLeftp();
	afx_msg void OnRightp();
	//}}AFX_MSG
	long OnButClick(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	afx_msg void OnSelchangeEchobottomZstep();
	afx_msg void OnResetEchoBottomZ();
	
	DECLARE_MESSAGE_MAP()
};
