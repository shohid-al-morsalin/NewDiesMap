// MotorGL.cpp: implementation of the CMotorGL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MotorGL.h"
#include "SIBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMotorGL::CMotorGL() {}

CMotorGL::~CMotorGL() {}

CMotorGL::CMotorGL(int iAxis, CSiBase* SiMC) {
	m_StopRequested = FALSE;
	//	m_Pos = 0;  // read from INI file in the  future

	pSiHub = SiMC;

	pSiHub->moAxis[iAxis - 1].bmpStatus = CAxis::AS_ACTIVE;

	MotorID = iAxis;
}

BOOL CMotorGL::Init(CListBox* pMessage) {
	CString str;
	if (pMessage) {
		str.Format("Axis %d (Gl) initialized", MotorID);
		pMessage->InsertString(pMessage->GetCount(), str);
	}
	return TRUE;
}