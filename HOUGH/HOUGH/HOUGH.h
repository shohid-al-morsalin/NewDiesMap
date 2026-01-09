
// HOUGH.h : main header file for the HOUGH application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CHOUGHApp:
// See HOUGH.cpp for the implementation of this class
//

class CHOUGHApp : public CWinApp
{
public:
	CHOUGHApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CHOUGHApp theApp;
