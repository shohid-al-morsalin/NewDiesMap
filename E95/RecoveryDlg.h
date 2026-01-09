#pragma once

class CInfoPanelMain;
class CInfoPanelSystem;

class CRecoveryDlg : public CDialog {
public:
	BOOL bRecoverOK;
	long AckMsg;
	BOOL bAutoClose;
	BOOL bIsOpenOp;
	// 	short Empty;
	BOOL bEnable;
	BOOL bStageHome;
	short type;
	short Port, Slot;
	CInfoPanelMain* m_ParentMain;
	CInfoPanelSystem* m_ParentSystem;
	CString m_ViewName;

	enum WAFERLOC {
		NOTDEFINE, ROBOTARM, ALIGNER, STAGE
	};
	WAFERLOC WaferLoc;

	short ID;

	// Construction
public:
	void DspSlotMessage();
	void Message(CString msg);
	void ConfirmNoWaferOnStage();
	BOOL RecoverWafer413();
	int GetWaferPresent();
	BOOL CheckWaferOnAligner(short arm);
	BOOL CheckWaferOnAligner();
	BOOL CheckWaferOnArm(short arm);
	BOOL CheckWaferOnArm();
	void SetWaferStation413();
	void StatusCheck413();
	void CloseFoup();
	BOOL WaferXfer(WAFERLOC Loc, short Port, short Slt);
	BOOL WaferPlace(WAFERLOC Loc, short Port, short Slt);
	CRecoveryDlg(CWnd* pParent = NULL);   // standard constructor
	~CRecoveryDlg();

	// Dialog Data
		//{{AFX_DATA(CRecoveryDlg)
	enum { IDD = IDD_RECOVERY };
	CButton	m_cOpen;
	//CButton	m_cLoadPos;
	CButton	m_cHomeRobot;
	CButton	m_cResetAll;
	CButton	m_cCloseFoup;
	CButton	m_cCloseTab;
	CButton	m_cMap;
	CButton	m_cScanner;
	CButton	m_cArm;
	CButton	m_cAligner;
	CListBox m_cList;
	CButton	m_cRecover;
	//}}AFX_DATA
	CButton m_csl[25];

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CRecoveryDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecoveryDlg)
	afx_msg LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	afx_msg void OnClosetab();
	afx_msg void OnMap();
	afx_msg void OnClosefoup();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnComSlotmap(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComSlotmapfail(WPARAM wP, LPARAM lP);
	afx_msg void OnSlot1();
	afx_msg void OnSlot10();
	afx_msg void OnSlot11();
	afx_msg void OnSlot12();
	afx_msg void OnSlot13();
	afx_msg void OnSlot14();
	afx_msg void OnSlot15();
	afx_msg void OnSlot16();
	afx_msg void OnSlot17();
	afx_msg void OnSlot18();
	afx_msg void OnSlot19();
	afx_msg void OnSlot2();
	afx_msg void OnSlot20();
	afx_msg void OnSlot21();
	afx_msg void OnSlot22();
	afx_msg void OnSlot23();
	afx_msg void OnSlot24();
	afx_msg void OnSlot25();
	afx_msg void OnSlot3();
	afx_msg void OnSlot4();
	afx_msg void OnSlot5();
	afx_msg void OnSlot6();
	afx_msg void OnSlot7();
	afx_msg void OnSlot8();
	afx_msg void OnSlot9();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLpMapok();
	afx_msg void OnLpMapfailed();
	afx_msg void OnComEndclose();
	afx_msg void OnRecover();
	afx_msg void OnLpFoupcloseok();
	afx_msg void OnResetstage();
	afx_msg void OnResetrobot();
	afx_msg void OnRadio1();
	afx_msg void OnRadio7();
	afx_msg void OnRadio8();
	//afx_msg void OnLoadposition();
	afx_msg void OnOpen();
	//}}AFX_MSG
	afx_msg void OnLpFoupopenfail();
	afx_msg void OnComEndopen();
	DECLARE_MESSAGE_MAP()
	BOOL isHomeRobotDone;
	BOOL isHomeAlignerDone;
	BOOL isRestStageDone;
	BOOL isGotoLoadDone;
public:
	//afx_msg void OnBnClickedResetaligner();
	//CButton m_cResetAligner;
};
