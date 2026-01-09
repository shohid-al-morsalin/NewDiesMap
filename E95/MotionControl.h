#pragma once
// MotionControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMotionControl dialog
class CSICam;

class CMotionControl : public CDialog {
public:
	CWnd* pWnd;

	BOOL bBusy;
	CSICam* pVideo;
	//short dirX,dirY;  // 1 or -1 only [8/23/2011 fsm]
	float Fac, EchoTopZFac, RoughZFac, CWLTopZFac;

public:
	void UpdateLocation(BOOL bUp = FALSE);

	// Construction
public:
	void HideWaferMap();
	//void GetMotorDirection();
	void EnableButtons();
	void DisableButtons();
	void SetPosControl(BOOL bSet);
	void EchoTopZMove(float dx);
	void RoughZMove(float dx);
	void CWLTopZMove(float dx);
	void Move(float dx, float dy);
	CMotionControl(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMotionControl)
	enum { IDD = IDD_MOTIONCONTROL };
	CButton	m_cdXRight;
	CButton	m_cdYDown;
	CButton	m_cdXLeft;
	CButton	m_cdYUp;
	CButton	m_cGotoRA;
	CButton	m_cGotoXY;
	CButton	m_cXYTopLeft;
	CButton	m_cXYLeft;
	CButton	m_cBotLeft;
	CButton	m_cXYDown;
	CButton	m_cXYBotRight;
	CButton	m_cXYRight;
	CButton	m_cXYTopRight;
	CButton	m_cXYUp;
	// [ Rough
	CButton	m_cGotoRoughZ;
	CEdit	m_cRoughZPos;
	CButton	m_cDownRoughZ;
	CButton	m_cUpRoughZ;
	// [ 06112024/MORSALIN --> CWL
	CButton	m_cGotoCWLTopZ;
	CEdit	m_cCWLTopZPos;
	CButton	m_cDownCWLTopZ;
	CButton	m_cUpCWLTopZ;
	// Echo top
	CEdit	m_cEchoTopZPos;
	CButton	m_cGotoEchoTopZ;
	CButton	m_cDownEchoTopZ;
	CButton	m_cUpEchoTopZ;
	// ]
	CButton	m_cGotoDPPos2;
	CButton	m_cGotoBGPos;
	CButton	m_cGotoDPPos;
	CButton	m_cGotoZeroXY;
	CButton	m_cLoadPos;
	CButton	m_cThinFilmPos;
	CButton	m_cEchoPos;
	float	m_XPos;
	float	m_YPos;
	float	m_EchoTopZPos;
	float	m_RoughZPos;
	float	m_CWLTopZPos; // 06112024/MORSALIN
	float	m_Angle;
	float	m_Radius;
	float	m_Dx;
	float	m_DxY;
	float	m_Dy;
	float	m_DyX;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMotionControl)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMotionControl)
	afx_msg void OnZ1x();
	afx_msg void OnZ2x();
	afx_msg void OnZ5x();
	afx_msg void OnZ10x();
	afx_msg void OnZ20x();
	afx_msg void OnZ1x2();
	afx_msg void OnZ2x2();
	afx_msg void OnZ5x2();
	afx_msg void OnZ10x2();
	afx_msg void OnZ20x2();
	afx_msg void OnX1();
	afx_msg void OnX2();
	afx_msg void OnX5();
	afx_msg void OnX10();
	afx_msg void OnX50();
	afx_msg void OnX100();
	afx_msg void OnX250();
	afx_msg void OnX500();
	afx_msg void OnX1000();
	afx_msg void OnUpz();
	afx_msg void OnDownz();
	afx_msg void OnUpzrough();
	afx_msg void OnDownzrough();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnLeft();
	afx_msg void OnRight();
	virtual BOOL OnInitDialog();
	afx_msg void OnGotoxy();
	afx_msg void OnGotoechotopz();
	afx_msg void OnAdvance();
	afx_msg void OnGotoroughz();
	afx_msg void OnEchopos();
	afx_msg void OnThinfilmpos();
	afx_msg void OnMark();
	afx_msg void OnAlign();
	afx_msg void OnLoadpos();
	afx_msg void OnOrigin();
	afx_msg void OnCalibpos();
	afx_msg void OnBgposition();
	afx_msg void OnZ20x3();
	afx_msg void OnCalibpos2();
	afx_msg void OnTopleft();
	afx_msg void OnTopright();
	afx_msg void OnBottomright();
	afx_msg void OnBottomleft();
	afx_msg void OnWafermap();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGotora();
	afx_msg void OnX3();
	afx_msg void OnX101();
	afx_msg void OnX1001();
	afx_msg void OnX4();
	afx_msg void OnLights();
	afx_msg void OnRecipe();
	afx_msg void OnEchocamera();
	afx_msg void OnThinfilmcamera();
	afx_msg void OnUpdate();
	afx_msg void OnKillfocusEdit1();
	afx_msg void OnKillfocusEdit2();
	afx_msg void OnKillfocusEdit3();
	afx_msg void OnKillfocusEdit4();
	afx_msg void OnAddX();
	afx_msg void OnMinusX();
	afx_msg void OnAddY();
	afx_msg void OnMinusY();
	afx_msg void OnFfu();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	// [20240426 tushar
	/*float	EchoBottomZFac;
	void EchoBottomZMove(float dx);
	afx_msg void OnDownEchoBottomZ();
	CButton m_cDownEchoBottomZ;
	CButton m_cUpEchoBottomZ;
	CEdit	m_cEchoBottomZPos;
	CButton	m_cGotoEchoBottomZ;
	afx_msg void OnUpEchoBottomZ();
	afx_msg void OnEchoBottomZx1();
	afx_msg void OnEchoBottomZx2();
	afx_msg void OnEchoBottomZx3();
	afx_msg void OnEchoBottomZx4();
	afx_msg void OnEchoBottomZx5();
	afx_msg void OnEchoBottomZx6();*/
	CButton	m_cGotoEchoBottomZ;
	// [05142024 ZHIMING
	float	m_EchoBottomZPos;
	// ]
	//afx_msg void OnBnClickedGotoechobottomz();
	afx_msg void OnBnClickedZ20x4();
	// [ 02082024/MORSALIN/
	afx_msg void OnBnClickedGoEchoProbe(); 
	afx_msg void OnBnClickedGoCwl008Probe(); // 10172024 Morsalin
	afx_msg void OnBnClickedGoCwl030Probe(); // 10172024 Morsalin
	afx_msg void OnBnClickedGoRoughnessProbe();
	CButton m_EchoPrb;
	CButton m_CWL008prb; // 10172024 Morsalin
	CButton m_CWL030prb; // 10172024 Morsalin
	CButton m_RoughnessPrb;
	CEdit m_MicroscopePrbPos; // 10172024 Morsalin
	CEdit m_CWL008PrbPos; // 10172024 Morsalin
	CEdit m_CWL030PrbPos; // 10172024 Morsalin
	CEdit m_RoughnessPrbPos;
	void UpdateButton(int pos);
	afx_msg void OnBnClickedSetEchoPos();
	afx_msg void OnBnClickedSetCwl008Pos(); // 10172024 Morsalin
	afx_msg void OnBnClickedSetCwl030Pos(); // 10172024 Morsalin
	afx_msg void OnBnClickedSetRoughnessPos();
	afx_msg void OnBnClickedGoAbsolute();
	afx_msg void OnBnClickedGoRelativeLeft();
	afx_msg void OnBnClickedGoRelativeRight();
	int movingStep, m_RadioBtn;
	CButton m_MoveAbtn;
	CButton m_MoveRLbtn;
	CButton m_MoveRRbtn;
	void OnBnClickedRadio();
	// ]
	afx_msg void OnBnClickedGotocwl();
	afx_msg void OnBnClickedZ3x1();
	afx_msg void OnBnClickedZ3x5();
	afx_msg void OnBnClickedZ3x10();
	afx_msg void OnBnClickedZ3x25();
	afx_msg void OnBnClickedZ3x50();
	afx_msg void OnBnClickedZ3x100();
	afx_msg void OnBnClickedZ3x500();
	afx_msg void OnBnClickedUpcwlz();
	afx_msg void OnBnClickedDowncwlz();
	afx_msg void OnBnClickedModifyprobepos();
};
