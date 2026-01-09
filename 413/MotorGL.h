// MotorGL.h: interface for the CMotorGL class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "MotorL2.h"

class CSiBase;

class CMotorGL : public CMotorL2 {
public:
	BOOL Init(CListBox* pMessage);
	CMotorGL(int iAxis, CSiBase* SiMC);
	CMotorGL();
	virtual ~CMotorGL();
};
