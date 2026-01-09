// StepMotor.cpp: implementation of the CStepMotor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StepMotor.h"
#include "..\SRC\DOSUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// CStepMotor::CStepMotor(LPCTSTR lpName) : CController(lpName)
CStepMotor::CStepMotor()
{
}

CStepMotor::~CStepMotor()
{
}

// void CStepMotor::GetDefaultSetting()
// {
// 	CString inifile;
// 	DosUtil.GetLocalCfgFile(inifile);
// 
// 	m_XYCurrent = (float)GetPrivateProfileInt("XMotor", "Current", 2000, inifile);
// 	m_ZCurrent  = (float)GetPrivateProfileInt("Z1Motor","Current", 1000, inifile);
// 	m_XYSpeed   = (float)GetPrivateProfileInt("XMotor",  "Speed", 10000, inifile);
// 	m_ZSpeed	= (float)GetPrivateProfileInt("Z1Motor", "Speed", 10000, inifile);
// 	m_ZJogSpeed	= (float)GetPrivateProfileInt("Z1Motor", "JogSpeed", 10000, inifile);
// }
