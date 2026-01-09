#pragma once

// #include "SRC/Led.h"
#include "CLPXChDat.h"

/////////////////////////////////////////////////////////////////////////////
// CLoadPortDlg dialog

class CLoadPortDlg : public CDialog {
	// Construction
public:
	CLoadPortDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoadPortDlg)
	enum { IDD = IDD_LOADPORT };
	CStatic	m_cActiveCarrier;
	CButton	m_cClearBlock;
	CStatic	m_cModule;
	CStatic	m_cCarrierID;
	CButton	m_cService2;
	CButton	m_cBypass;
	CButton	m_cAccessModeB;
	CButton	m_cService;
	// 	CString	m_TransferStatus;
	// 	CString	m_AccessingStatus;
	CString	m_SlotMapStatus;
	CString	m_IDStatus;
	CString	m_AccessMode;
	// 	CString	m_Association;
	// 	CString	m_Reservation;
	CString	m_CIDRStatus;
	CString	m_Bypass;
	//}}AFX_DATA

	CLPXChDat Xch;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CLoadPortDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadPortDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAccessmodeb();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnService();
	afx_msg void OnBypasscidr();
	afx_msg void OnService2();
	afx_msg void OnClearblock();
	afx_msg void OnCheck();
	afx_msg void OnReadcidr2();
	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnHoavbl();
	afx_msg void OnUnclamp();
	afx_msg void OnReset();
	afx_msg void OnCancelcarrier();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void Refresh();
	short ID;
};
