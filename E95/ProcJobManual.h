#pragma once

#include "ProcJobCommon.h"
#include "SRC/ResizableDialog.h"

class CInfoPanelMain;

class CProcJobManual : public CResizableDialog
	, public CProcJobCommon {
public:
	CString GetRecipeName(int RcpNo);
	BOOL ExecuteRecipe(short RcpNo);
	BOOL ExecuteRecipe(CString RcpName);
	BOOL AssignRecipe(short RcpNo);
	BOOL DeleteRecipe(short RcpNo);
	BOOL Run(short RcpNo);
	CProcJobManual(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProcJobManual();

	// Dialog Data
		//{{AFX_DATA(CProcJobManual)
	enum { IDD = IDD_PROCJOBMANUAL };
	CButton	m_cAutoFocus;
	CString	m_WaferId;
	CString m_WaferSize; // 20251014_Mohir
	CString m_Station; // 20251014_Mohir
	CString m_WaferType; // 20251014_Mohir
	CString	m_Operator;
	CString	m_LotID;
	CButton	m_R9;
	CButton	m_R8;
	CButton	m_R7;
	CButton	m_R6;
	CButton	m_R5;
	CButton	m_R4;
	CButton	m_R3;
	CButton	m_R20;
	CButton	m_R2;
	CButton	m_R19;
	CButton	m_R18;
	CButton	m_R17;
	CButton	m_R16;
	CButton	m_R15;
	CButton	m_R14;
	CButton	m_R13;
	CButton	m_R12;
	CButton	m_R11;
	CButton	m_R10;
	CButton	m_R1;
	CButton	m_cAssign;
	CString	CarrierID;
	CButton	m_R21;
	CButton	m_R22;
	CButton	m_R23;
	CButton	m_R24;
	CButton	m_R25;
	CButton	m_R26;
	CButton	m_R27;
	CButton	m_R28;
	CButton	m_R29;
	CButton	m_R30;
	CButton	m_R31;
	CButton	m_R32;
	CButton	m_R33;
	CButton	m_R34;
	CButton	m_R35;
	CButton	m_R36;
	//}}AFX_DATA

	CInfoPanelMain* GetParentWnd();
	void Log(CString msg, short lvl = 5);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CProcJobManual)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL bAssign;

	// Generated message map functions
	//{{AFX_MSG(CProcJobManual)
	virtual BOOL OnInitDialog();
	afx_msg void OnAssign();
	afx_msg void OnRecipe1();
	afx_msg void OnRecipe2();
	afx_msg void OnRecipe3();
	afx_msg void OnRecipe4();
	afx_msg void OnRecipe5();
	afx_msg void OnRecipe6();
	afx_msg void OnRecipe7();
	afx_msg void OnRecipe8();
	afx_msg void OnRecipe9();
	afx_msg void OnRecipe10();
	afx_msg void OnRecipe11();
	afx_msg void OnRecipe12();
	afx_msg void OnRecipe13();
	afx_msg void OnRecipe14();
	afx_msg void OnRecipe15();
	afx_msg void OnRecipe16();
	afx_msg void OnRecipe17();
	afx_msg void OnRecipe18();
	afx_msg void OnRecipe19();
	afx_msg void OnRecipe20();
	afx_msg void OnLoadwafer();
	afx_msg void OnRecipe21();
	afx_msg void OnRecipe22();
	afx_msg void OnRecipe23();
	afx_msg void OnRecipe24();
	afx_msg void OnRecipe25();
	afx_msg void OnRecipe26();
	afx_msg void OnRecipe27();
	afx_msg void OnRecipe28();
	afx_msg void OnRecipe29();
	afx_msg void OnRecipe30();
	afx_msg void OnRunrecipe();
	afx_msg void OnRecipe31();
	afx_msg void OnRecipe32();
	afx_msg void OnRecipe33();
	afx_msg void OnRecipe34();
	afx_msg void OnRecipe35();
	afx_msg void OnRecipe36();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
