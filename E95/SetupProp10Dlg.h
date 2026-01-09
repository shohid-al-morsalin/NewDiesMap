#pragma once

#include "ImgWnd.h"
#include "SetupPropComm.h"
#include "SICam.h"
#include "SRC/ResizableDialog.h"

class CInfoPanelMain;

class CSetupProp10Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	int LRID[25];	// Resource ID [7/24/2013 Yuen]
	int RRID[25];	// Resource ID [7/24/2013 Yuen]

	CString SlotMap;
	BOOL bRelease;
	static UINT StartDryRunProc(LPVOID p);		// [5/27/2020 Zhiming]
	BOOL bResetting, bTransferInProgress;

	BOOL TransferToCassette();
	BOOL TransferToStage();
	void SetSlot(short port, short slot);
	// 	BOOL CanTransfer2Cas(short Lp, short slot);
	// 	BOOL CanTransfer2Stage(short Lp, short slot);
	BOOL TransferStage2Cas(short slot);
	BOOL TransferCas2Stage(short slot, BOOL bVacOn);
	BOOL IsStageInLoadPosition(float fX, float fY);
	CSetupProp10Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp10Dlg)
	enum { IDD = IDD_SETUPPROP10 };
	CButton	m_cUnclamp2;
	CButton	m_cUnclamp1;
	CButton	m_cRevert;
	CButton	m_cInvert;
	CImgWnd	m_cImgWnd;
	CSICam	m_cVideo;
	CButton	m_cAlgn;
	CButton	m_cResetXY;
	CButton	m_cGotoOrigin;
	CButton	m_cTransfer2stage;
	CButton	m_cTransfer2loadport;
	CButton	m_cOpenFoup2;
	CButton	m_cOpenFoup1;

	// [5/27/2020 Zhiming]
	CButton m_cDryRun;
	HANDLE hDryRun;
	BOOL bDryRunEnd;
	//}}AFX_DATA

// 	CInfoPanelMain * m_Parent;
	short SLP1, SLP2;

	enum SETUPPROP10STATE {
		s10ALLCLOSE, s10FOUPOPENING, s10FOUPOPENED, s10WAFERLOADED, s10WAFERRETURNED, s10FOUPCLOSING
	};
	SETUPPROP10STATE State;
	short ActiveFoup;

	BOOL CloseFoup(short Port);
	BOOL OpenFoup(short Port);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp10Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp10Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnOpenfoup1();
	afx_msg void OnOpenfoup2();
	afx_msg void OnTransfertostage();
	afx_msg void OnTransfertoloadport();
	afx_msg void OnLpFoupopenfail();
	afx_msg void OnLpFoupopenok();
	afx_msg void OnComEndopen();
	afx_msg void OnComAbortopen();
	afx_msg void OnLpFoupcloseok();
	afx_msg void OnLpFoupclosefail();
	afx_msg void OnComAbortclose();
	afx_msg void OnComEndclose();
	afx_msg void OnGotoloadpos();
	afx_msg void OnLslot1();
	afx_msg void OnRslot2();
	afx_msg void OnRslot3();
	afx_msg void OnRslot4();
	afx_msg void OnRslot5();
	afx_msg void OnRslot6();
	afx_msg void OnRslot7();
	afx_msg void OnRslot8();
	afx_msg void OnRslot9();
	afx_msg void OnRslot10();
	afx_msg void OnRslot11();
	afx_msg void OnRslot12();
	afx_msg void OnRslot13();
	afx_msg void OnRslot14();
	afx_msg void OnRslot15();
	afx_msg void OnRslot16();
	afx_msg void OnRslot17();
	afx_msg void OnRslot18();
	afx_msg void OnRslot19();
	afx_msg void OnRslot20();
	afx_msg void OnRslot21();
	afx_msg void OnRslot22();
	afx_msg void OnRslot23();
	afx_msg void OnRslot24();
	afx_msg void OnRslot25();
	afx_msg void OnLslot10();
	afx_msg void OnLslot11();
	afx_msg void OnLslot12();
	afx_msg void OnLslot13();
	afx_msg void OnLslot14();
	afx_msg void OnLslot15();
	afx_msg void OnLslot16();
	afx_msg void OnLslot17();
	afx_msg void OnLslot18();
	afx_msg void OnLslot19();
	afx_msg void OnLslot2();
	afx_msg void OnLslot20();
	afx_msg void OnLslot21();
	afx_msg void OnLslot22();
	afx_msg void OnLslot23();
	afx_msg void OnLslot24();
	afx_msg void OnLslot25();
	afx_msg void OnLslot3();
	afx_msg void OnLslot4();
	afx_msg void OnLslot5();
	afx_msg void OnLslot6();
	afx_msg void OnLslot7();
	afx_msg void OnLslot8();
	afx_msg void OnLslot9();
	afx_msg void OnRslot1();
	afx_msg void OnResetxy();
	afx_msg void OnKillfocusWaferrepeat2();
	afx_msg void OnKillfocusCassetterepeat();
	afx_msg void OnRelease();
	afx_msg void OnInitialize();
	afx_msg void OnComClosenoreleasecmpl();
	afx_msg void OnUnclamp();
	afx_msg void OnUnclamp3();
	afx_msg void OnClearwinter();
	afx_msg LRESULT OnComSlotMap(WPARAM wP, LPARAM lP);
	afx_msg LRESULT OnComSlotMapFail(WPARAM wP, LPARAM lP);
	afx_msg void OnInvert();
	afx_msg void OnRevert();
	afx_msg void OnFindcxcy();
	afx_msg void OnDryrun();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	// 	long OnComSlotMap(WPARAM wP, LPARAM lP);
	// 	long OnComSlotMapFail(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	void SetCheck(short unit, short slt);
	void ClearChecks(short unit);

	void LocalToRecipe() {};
	void RecipeToLocal() {};
	void ClearAllSlots(BOOL bEn = FALSE);
	void EnableSlots(short unit, CString slotmap, char bEn, short slt);
	void EnableButton(BOOL bEna);
	afx_msg void OnBnClickedUnclamp();
	afx_msg void OnBnClickedUnclamp3();
	void Unclamp(short Port);

	//[ TUSHAR testing for user data
	afx_msg void OnBnClickedLpnum();
	CEdit m_LPNum;
	CEdit m_SlotNums;
	CString sLPNum;
	CString sSlotNums;
	int iLPNum;
	vector<int> slots;

	bool bStop = false;
	bool bRun = false;
	//]
	afx_msg void OnBnClickedTestrun();
	static UINT TransferPrep(LPVOID pdlg);
	BOOL TransferFull(short slot, BOOL bVacOn);
	afx_msg void OnBnClickedResponse();
	afx_msg void OnBnClickedPresence();
};
#define WM_STOPDRYRUN (WM_USER+1234)