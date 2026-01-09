#pragma once

#include "413AppCam.h"

class C413AppMtr : public C413AppCam {
public:
	BOOL ResetPMotor();

	BOOL GotoPositionechoNonBlocking();
	BOOL GotoPositionecho();
	BOOL GotoPositionthinfilm();
	float GetAngle(float x, float y);
	BOOL GotoXYS(float fX, float fY, CCamBase::CAMERAID ActCam);

	C413AppMtr();
	virtual ~C413AppMtr();
};
