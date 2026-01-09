#include "stdafx.h"

#include "413/MotorL2.h"
#include "413/MotorS2.h"

#include "413AppMtr.h"

C413AppMtr::C413AppMtr() {}

C413AppMtr::~C413AppMtr() {}

BOOL C413AppMtr::GotoPositionechoNonBlocking() {
	// Not implemented for safety reason [6/23/2014 Yuen]
	return GotoPositionecho();
}

BOOL C413AppMtr::GotoPositionecho() {
	CString msg;
	if (pMtr && pMtr->pMP) {
		float pos = pMtr->pMP->Param.HomePos1; // unit in mm [5/14/2013 user]
		if (pMtr && pMtr->pMP) {
			if (pMtr->MoveRP(pos)) {
				msg.Format("AppMtr:GotoPositionEcho: P=%.3f", pos);
				Log(msg);
				return TRUE;
			}
		}
		msg.Format("AppMtr:GotoPositionEcho: P=%.3f FAILED", pos);
		Log(msg, 1);
		return FALSE;
	}
	// [7/23/2020]
	if (!bSimuP) {
		msg = "AppMtr:GotoPositionEcho: No P axis";
		Log(msg, 1);
	}
	// ===========
	return TRUE;
}

BOOL C413AppMtr::GotoPositionthinfilm() {
	CString msg;
	if (pMtr && pMtr->pMP) {
		float pos = pMtr->pMP->Param.HomePos2;
		if (pMtr && pMtr->pMP) {
			if (pMtr->MoveRP(pos)) {
				msg.Format("AppMtr:GotoPositionThinFilm: P=%.3f", pos);
				Log(msg);
				return TRUE;
			}
		}
		msg.Format("AppMtr:GotoPositionThinFilm: P=%.3f FAILED", pos);
		Log(msg, 1);
		return FALSE;
	}
	// [7/23/2020]
	if (!bSimuP) {
		msg = "AppMtr:GotoPositionThinFilm: No P axis";
		Log(msg, 1);
	}
	// ===========
	return TRUE;
}

float C413AppMtr::GetAngle(float x, float y) {
	float a = 0;

	if (x == 0) {
		x = 1.0e-9f;	// Very small number [3/26/2013 FSMT]
	}
	a = atan(fabs(y / x)) * 57.29577951308230;
	if (x < 0) {
		if (y < 0) {
			a += 180;
		}
		else {
			a = 180 - a;
		}
	}
	else {
		if (y < 0) {
			a = 360 - a;
		}
	}
	return a;
}

// Similar to GotoXY but overrides default camera [4/21/2013 Administrator]
BOOL C413AppMtr::GotoXYS(float fX, float fY, CCamBase::CAMERAID ActCam) {
	BOOL ret = FALSE;
	C413Global* g = &Global;

	if (pMtr) {
		CString msg;
		CCamBase::CAMERAID ac = g->ActiveCamera;
		g->ActiveCamera = ActCam;
		msg.Format("GotoXYS: %.4f  %.4f", fX, fY);
		Log(msg);

		CFPoint COffEcho = g->COffEcho;
		CFPoint COffTF = g->COffTF;
		CFPoint COffCWL = g->COffCWL;
		CFPoint COffRough = g->COffRough;

		g->COffTF.Clear();
		g->COffEcho.Clear();
		g->COffCWL.Clear();
		g->COffRough.Clear();
		switch (ActCam)
		{
		case CCamBase::THINFCAM:
			fX += COffTF.X;
			fY += COffTF.Y;
			break;
		case CCamBase::CWL:
			fX += COffCWL.X;
			fY += COffCWL.Y;
			break;
		case CCamBase::ROUGH:
			fX += COffRough.X;
			fY += COffRough.Y;
			break;
		default:
			break;
		}

		ret = pMtr->GotoXY(fX, fY);
		g->COffEcho = COffEcho;
		g->COffTF = COffTF;
		g->COffCWL = COffCWL;
		g->COffRough = COffRough;

		if (ret) {
			msg.Format("GotoXYS OK: X=%.4f  Y=%.4f", g->LocXY.X, g->LocXY.Y);
		}
		else {
			msg.Format("GotoXYS FAIL: X=%.4f  Y=%.4f", g->LocXY.X, g->LocXY.Y);
		}
		Log(msg);
		g->ActiveCamera = ac;
	}
	return ret;
}

BOOL C413AppMtr::ResetPMotor() {
	if (pMtr) {
		if (pMtr->ResetP()) {
			return TRUE;
		}
	}
	return FALSE;
}