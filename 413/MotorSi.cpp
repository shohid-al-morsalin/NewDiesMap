// MotorSi.cpp: implementation of the CMotorSi class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MotorSi.h"
#include "SIBase.h"

const int DEFINE_LIMITS = 2;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMotorSi::CMotorSi() {}

CMotorSi::~CMotorSi() {}

CMotorSi::CMotorSi(int iAxis, CSiBase* SiMC, bool isHub) {
	m_StopRequested = FALSE;
	pSiHub = SiMC;
	if (isHub) pSiHub->moAxis[iAxis - 1].bmpStatus = CAxis::AS_ACTIVE;
	else pSiHub->moAxis[0].bmpStatus = CAxis::AS_ACTIVE;
	MotorID = iAxis;
}

BOOL CMotorSi::Init(CListBox* pMessage) {
	CString str;
	if (pSiHub) {
		if (pSiHub->DefineLimits(MotorID, DEFINE_LIMITS)) {
			float pX, pY;
			pSiHub->devGetXYPos(&pX, &pY, 10000);
			str.Format("Axis %d (Si) initialized", MotorID);
			if (pMessage) pMessage->InsertString(pMessage->GetCount(), str);
			return TRUE;
		}
	}
	str.Format("Axis %d (Si) initialize FAILED", MotorID);
	if (pMessage) pMessage->InsertString(pMessage->GetCount(), str);
	return FALSE;
}