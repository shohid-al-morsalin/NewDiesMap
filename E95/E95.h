// E95.h : main header file for the E95 application
//
#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// CE95App:
// See E95.cpp for the implementation of this class
//
class CAboutDlg : public CDialog {
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CStatic	m_cversion;
	CStatic m_cprogdir;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CE95App : public CWinApp {
	// 	BOOL LoadDriver(CString &msg);
	// 	BOOL IsNTBasedOS();
	BOOL SocketStartUp();
public:
	CE95App();

	// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CE95App theApp;