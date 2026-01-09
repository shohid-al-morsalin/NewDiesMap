// MotorSi.h: interface for the CMotorSi class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "MotorL2.h"

class CMotorSi : public CMotorL2 {
public:
	BOOL Init(CListBox* pMessage);

	CMotorSi(int iAxis, CSiBase* SiMC, bool isHub = true);
	CMotorSi();
	virtual ~CMotorSi();
};