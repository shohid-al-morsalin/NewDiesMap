// MC3.cpp: implementation of the CMC3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "MC3.h"
#include "Global413.h"
#include "413App.h"
#include "GlobalDeviceContainer.h"

#include "..\413\MotorGl.h"

// [6/17/2020]
#include "..\413\GLMotor.h"
#include "..\413\GLMotor_v1.h"
// ===========

#include "..\IPC\Lock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMC3::CMC3() {
	Limit = 7 * 8;  // roughly 8 microns [7/20/2013 FSM413]
}

CMC3::~CMC3() {}

// [5/27/2020]
BOOL CMC3::IsDoorInterlockTriggered() {
	if (pSiMc) {
		return pSiMc->IsDoorInterlockTriggered();
	}
	return FALSE;
}

BOOL CMC3::SetSpectroSourceLight(BOOL bOnOff) {
	if (pSiMc) {
		return pSiMc->SetSpectroSourceLight(bOnOff);
	}
	return FALSE;
}

// Blocking [6/25/2012 FSM413]
BOOL CMC3::MoveRXY1(float fX, float fY) {
	if (pSiMc) {
		C413Global* g = &p413App->Global;
		CMotorS2::STAGECOORSPACE csp = ActvCam2CoorSpace(g->ActiveCamera);
		// No transformation needed [6/25/2012 FSM413]
		float iX = pMX->Real2Dev(pMX->Param.bPolarity ? fX : -fX);
		float iY = pMY->Real2Dev(pMY->Param.bPolarity ? fY : -fY);
		pSiMc->devMoveRXY(iX, iY);
		BOOL bPos = FALSE;
		if (pSiMc->_WaitAllStop(devGetTOutDelay(iX, iY), bPos)) {
			BOOL bRet = FALSE;
			if (bPos) {
				iX = pSiMc->moAxis[g->X_Axis - 1].fPos;
				iY = pSiMc->moAxis[g->Y_Axis - 1].fPos;
				bRet = TRUE;
			}
			else {
				bRet = pSiMc->devGetXYPos(&iX, &iY, 10000);
			}
			if (bRet) {
				fX = pMX->fGetPos(iX);
				fY = pMY->fGetPos(iY);
				ReverseTransform(fX, fY, csp);
				switch (g->ActiveCamera) {
				default:
					g->LocXY.Set(fX - g->COffEcho.X, fY - g->COffEcho.Y);
					break;
				case CCamBase::THINFCAM:
					g->LocXY.Set(fX - g->COffTF.X, fY - g->COffTF.Y);
					break;
				// [05072024 ZHIMING
				case CCamBase::CWL:
					g->LocXY.Set(fX - g->COffCWL.X, fY - g->COffCWL.Y);
					break;
				case CCamBase::ROUGH:
					g->LocXY.Set(fX - g->COffRough.X, fY - g->COffRough.Y);
					break;
				// ]
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Blocking [6/25/2012 FSM413]
// Perform second fine move [8/5/2013 Yuen]
// Perform final position error check [8/5/2013 Yuen]
BOOL CMC3::MoveAXY(float fX, float fY) {
	CString msg;
	if (pSiMc) {
		C413Global* g = &p413App->Global;

		float fCXOff, fCYOff;
		GetActiveCameraOffset(fCXOff, fCYOff);

		fX += fCXOff;
		fY += fCYOff;

		float dX, dY;
		float fXi = fX;
		float fYi = fY;

		CFPoint iXY;
		pSiMc->devGetXYPos(&iXY.X, &iXY.Y, 10000);	// initial X,Y pos [8/5/2013 Yuen]

		CMotorS2::STAGECOORSPACE csp = ActvCam2CoorSpace(g->ActiveCamera);
		Transform(fX, fY, csp);

		float iX = pMX->Real2Dev(pMX->Param.bPolarity ? fX : -fX);
		float iY = pMY->Real2Dev(pMY->Param.bPolarity ? fY : -fY);

		if (!pSiMc->devMoveAXY(iX, iY)) {
			return FALSE;
		}
		BOOL bPos = FALSE;
		if (!pSiMc->_WaitAllStop(devGetTOutDelay(iX - iXY.X, iY - iXY.Y), bPos)) {
			return FALSE;
		}

		float ifX, ifY;
		BOOL bRet = FALSE;
		if (bPos) {
			ifX = pSiMc->moAxis[g->X_Axis - 1].fPos;
			ifY = pSiMc->moAxis[g->Y_Axis - 1].fPos;
			bRet = TRUE;
		}
		else {
			bRet = pSiMc->devGetXYPos(&ifX, &ifY, 10000);
		}
		if (!bRet) {
			return FALSE;
		}

		bRet = FALSE;
		dX = ifX - iX;
		dY = ifY - iY;
		if ((fabs(dX) > Limit) || (fabs(dY) > Limit)) {
			if (pSiMc->devMoveRXY(dX, dY)) {
				if (pSiMc->_WaitAllStop(devGetTOutDelay(dX, dY), bPos)) {
					if (bPos) {
						ifX = pSiMc->moAxis[g->X_Axis - 1].fPos;
						ifY = pSiMc->moAxis[g->Y_Axis - 1].fPos;
					}
					else {
						if (pSiMc->devGetXYPos(&ifX, &ifY, 10000)) {
							bRet = TRUE;
						}
					}
				}
			}
		}
		else {
			bRet = TRUE;
		}
		if (!bRet) {
			return FALSE;
		}
		// End point correction is optional [7/18/2013 Yuen]
		float fXf = pMX->fGetPos(ifX);
		float fYf = pMY->fGetPos(ifY);
		ReverseTransform(fXf, fYf, csp);
		// @XXX [6/12/2013 FSM413]
		if ((fabs(fXf - fXi) > 0.5f) || (fabs(fYf - fYi) > 0.5f)) {
			msg.Format("Stage position gap [X Axis = %ld, Y Axis = %ld]", (fXf - fXi), (fYf - fYi));
			p413App->Log(msg, 2);
			return FALSE;
		}
		g->LocXY.Set(fXf - fCXOff, fYf - fCYOff);
		return TRUE;
	}

	msg.Format("Stage MoveAXY Error");
	p413App->Log(msg, 2);
	return FALSE;
}

// Blocking [6/25/2012 FSM413]
BOOL CMC3::MoveAXYZ1(float fX, float fY, float fZ1) {
	CString msg;
	if (pSiMc) {
		C413Global* g = &p413App->Global;

		float fCXOff, fCYOff;
		GetActiveCameraOffset(fCXOff, fCYOff);

		fX += fCXOff;
		fY += fCYOff;

		//08142024 - START
		if (WC.bOffsetInUse) {
			fX += WC.XOff;
			fY += WC.YOff;
		}
		//08142024 - END

		float dX, dY;
		float fXi = fX;
		float fYi = fY;

		CFPoint iXY;
		pSiMc->devGetXYPos(&iXY.X, &iXY.Y, 10000);	// initial X,Y pos [8/5/2013 Yuen]

		CMotorS2::STAGECOORSPACE csp = ActvCam2CoorSpace(g->ActiveCamera);
		Transform(fX, fY, csp);

		float iX = pMX->Real2Dev(pMX->Param.bPolarity ? fX : -fX);
		float iY = pMY->Real2Dev(pMY->Param.bPolarity ? fY : -fY);
		float iZ1 = pMEchoTopZ->Real2Dev(pMEchoTopZ->Param.bPolarity ? fZ1 : -fZ1);
		float iZ2 = 0;	// Not use [4/20/2013 Administrator]

		if (!pSiMc->devMoveAXYZ1(iX, iY, iZ1)) {
			return FALSE;
		}
		BOOL bPos = FALSE;
		if (!pSiMc->_WaitAllStop(devGetTOutDelay(iX - iXY.X, iY - iXY.Y), bPos)) {
			return FALSE;
		}
		float ifX, ifY;
		BOOL bRet = FALSE;
		if (bPos) {
			ifX = pSiMc->moAxis[g->X_Axis - 1].fPos;
			ifY = pSiMc->moAxis[g->Y_Axis - 1].fPos;
			bRet = TRUE;
		}
		else {
			bRet = pSiMc->devGetXYPos(&ifX, &ifY, 10000);
		}
		if (!bRet) {
			return FALSE;
		}

		bRet = FALSE;
		dX = ifX - iX;
		dY = ifY - iY;
		if ((fabs(dX) > Limit) || (fabs(dY) > Limit)) {
			if (pSiMc->devMoveRXY(dX, dY)) {
				if (pSiMc->_WaitAllStop(devGetTOutDelay(dX, dY), bPos)) {
					if (bPos) {
						ifX = pSiMc->moAxis[g->X_Axis - 1].fPos;
						ifY = pSiMc->moAxis[g->Y_Axis - 1].fPos;
					}
					else {
						if (pSiMc->devGetXYPos(&ifX, &ifY, 10000)) {
							bRet = TRUE;
						}
					}
				}
			}
		}
		else {
			bRet = TRUE;
		}

		if (!bRet) return FALSE;
		// End point correction is optional [7/18/2013 Yuen]
		float fXf = pMX->fGetPos(ifX);
		float fYf = pMY->fGetPos(ifY);
		ReverseTransform(fXf, fYf, csp);

		if ((fabs(fXf - fXi) > 0.5f) || (fabs(fYf - fYi) > 0.5f)) {
			msg.Format("Stage position gap [X Axis = %ld, Y Axis = %ld]", (fXf - fXi), (fYf - fYi));
			p413App->Log(msg, 2);
			return FALSE;
		}
		g->LocEchoTopZ = pMEchoTopZ->fGetPos(iZ1);
		g->LocXY.Set(fXf - fCXOff, fYf - fCYOff);
		return TRUE;
	}

	msg.Format("Stage MoveAXY Error");
	p413App->Log(msg, 2);
	return FALSE;
}

// Blocking [6/25/2012 FSM413]
// Absolute move [6/25/2012 FSM413]
// No backlash correction [6/25/2012 FSM413]
// Transformation [6/25/2012 FSM413]
BOOL CMC3::MoveX(float fX) {
	//pLock->Lock();

	C413Global* g = &p413App->Global;
	CMotorS2::STAGECOORSPACE csp = ActvCam2CoorSpace(g->ActiveCamera);
	float fCXOff, fCYOff;
	GetActiveCameraOffset(fCXOff, fCYOff);

	fX += fCXOff;
	float fY = g->LocXY.Y + fCYOff;

	Transform(fX, fY, csp);
	float iX = pMX->Real2Dev(pMX->Param.bPolarity ? fX : -fX);
	float iY = pMY->Real2Dev(pMY->Param.bPolarity ? fY : -fY);
	if (pSiMc->devMoveAXY(iX, iY))	// Absolute move [6/25/2012 FSM413]
	{
		BOOL bPos = FALSE;
		if (pSiMc->_WaitForXY(120000, bPos)) {
			float iX1, iY1, dX, dY;
			pSiMc->devGetXYPos(&iX1, &iY1, 5000);
			dX = iX - iX1;
			dY = iY - iY1;
			if ((fabs(dX) > Limit) || (fabs(dY) > Limit)) {
				if (!pSiMc->devMoveRXY(dX, dY)) {
					return FALSE;
				}
				if (!pSiMc->_WaitForXY(120000, bPos)) {
					return FALSE;
				}
			}
			if (pSiMc->devGetXYPos(&iX, &iY, 5000)) {
				fX = pMX->fGetPos(iX);
				fY = pMY->fGetPos(iY);
				ReverseTransform(fX, fY, csp);
				g->LocXY.Set(fX - fCXOff, fY - fCYOff);
				//g->LocY = fY - fCYOff;
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Blocking [6/25/2012 FSM413]
// Absolute move [6/25/2012 FSM413]
// No backlash correction [6/25/2012 FSM413]
// Transformation [6/25/2012 FSM413]
BOOL CMC3::MoveY(float fY) {
	C413Global* g = &p413App->Global;
	CMotorS2::STAGECOORSPACE csp = ActvCam2CoorSpace(g->ActiveCamera);
	float fCXOff, fCYOff;
	GetActiveCameraOffset(fCXOff, fCYOff);

	fY += fCYOff;
	float fX = g->LocXY.X + fCXOff;
	Transform(fX, fY, csp);
	float iX = pMX->Real2Dev(pMX->Param.bPolarity ? fX : -fX);
	float iY = pMY->Real2Dev(pMY->Param.bPolarity ? fY : -fY);
	if (pSiMc->devMoveAXY(iX, iY))	// Absolute move [6/25/2012 FSM413]
	{
		BOOL bPos = FALSE;
		if (pSiMc->_WaitForXY(120000, bPos)) {
			float iX1, iY1, dX, dY;
			pSiMc->devGetXYPos(&iX1, &iY1, 5000);
			dX = iX - iX1;
			dY = iY - iY1;
			if ((fabs(dX) > Limit) || (fabs(dY) > Limit)) {
				if (!pSiMc->devMoveRXY(dX, dY)) {
					return FALSE;
				}
				if (!pSiMc->_WaitForXY(120000, bPos)) {
					return FALSE;
				}
			}
			if (pSiMc->devGetXYPos(&iX, &iY, 5000)) {
				fX = pMX->fGetPos(iX);
				fY = pMY->fGetPos(iY);
				ReverseTransform(fX, fY, csp);
				g->LocXY.Set(fX - fCXOff, fY - fCYOff);
				//g->LocY = fY-fCYOff;
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Relative move [6/25/2012 FSM413]
BOOL CMC3::MoveRX(float dist) {
	C413Global* g = &p413App->Global;
	CMotorS2::STAGECOORSPACE csp = ActvCam2CoorSpace(g->ActiveCamera);
	// Convert to absolute move [6/25/2012 FSM413]
	float fCXOff, fCYOff;
	GetActiveCameraOffset(fCXOff, fCYOff);

	float fX = g->LocXY.X + dist + fCXOff;
	float fY = g->LocXY.Y + fCYOff;

	Transform(fX, fY, csp);
	float iX = pMX->Real2Dev(pMX->Param.bPolarity ? fX : -fX);
	float iY = pMY->Real2Dev(pMY->Param.bPolarity ? fY : -fY);
	if (pSiMc->devMoveAXY(iX, iY))	// Absolute move [6/25/2012 FSM413]
	{
		BOOL bPos = FALSE;
		if (pSiMc->_WaitForXY(120000, bPos)) {
			float iX1, iY1, dX, dY;
			pSiMc->devGetXYPos(&iX1, &iY1, 5000);
			dX = iX - iX1;
			dY = iY - iY1;
			if ((fabs(dX) > Limit) || (fabs(dY) > Limit)) {
				if (!pSiMc->devMoveRXY(dX, dY)) {
					return FALSE;
				}
				if (!pSiMc->_WaitForXY(120000, bPos)) {
					return FALSE;
				}
			}
			if (pSiMc->devGetXYPos(&iX, &iY, 5000)) {
				fX = pMX->fGetPos(iX);
				fY = pMY->fGetPos(iY);
				ReverseTransform(fX, fY, csp);
				g->LocXY.Set(fX - fCXOff, fY - fCYOff);
				//g->LocY = fY-fCYOff;
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Relative move [6/25/2012 FSM413]
BOOL CMC3::MoveRY(float dist) {
	C413Global* g = &p413App->Global;
	CMotorS2::STAGECOORSPACE csp = ActvCam2CoorSpace(g->ActiveCamera);
	// Convert to absolute move [6/25/2012 FSM413]
	float fCXOff, fCYOff;
	GetActiveCameraOffset(fCXOff, fCYOff);

	float fX = g->LocXY.X + fCXOff;
	float fY = g->LocXY.Y + dist + fCYOff;
	Transform(fX, fY, csp);
	float iX = pMX->Real2Dev(pMX->Param.bPolarity ? fX : -fX);
	float iY = pMY->Real2Dev(pMY->Param.bPolarity ? fY : -fY);
	if (pSiMc->devMoveAXY(iX, iY))	// Absolute move [6/25/2012 FSM413]
	{
		BOOL bPos = FALSE;
		if (pSiMc->_WaitForXY(120000, bPos)) {
			float iX1, iY1, dX, dY;
			pSiMc->devGetXYPos(&iX1, &iY1, 5000);
			dX = iX - iX1;
			dY = iY - iY1;
			if ((fabs(dX) > Limit) || (fabs(dY) > Limit)) {
				if (!pSiMc->devMoveRXY(dX, dY)) {
					return FALSE;
				}
				if (!pSiMc->_WaitForXY(120000, bPos)) {
					return FALSE;
				}
			}
			if (pSiMc->devGetXYPos(&iX, &iY, 5000)) {
				fX = pMX->fGetPos(iX);
				fY = pMY->fGetPos(iY);
				ReverseTransform(fX, fY, csp);
				g->LocXY.Set(fX - fCXOff, fY - fCYOff);
				//g->LocY = fY-fCYOff;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CMC3::ResetZD(CMotorL2* pZ) {
	if (pSiMc) {
		CSiBase::AXISID a = pSiMc->GetAxisNo(pZ->MotorID);
		switch (a) {
		case CSiBase::ECHOTOPZAXIS:
			pSiMc->ResetEchoTopZ();
			break;
		case CSiBase::ROUGHZAXIS:
			pSiMc->ResetRoughZ();
			break;
		}
		pZ->SetProperties();
	}
}

BOOL CMC3::SetSpectroIllumLight(BOOL bOnOff) {
	if (pSiMc) {
		return pSiMc->SetSpectroIllumLight(bOnOff);
	}
	return FALSE;
}

BOOL CMC3::ResetXY() {
	C413Global* g = &p413App->Global;
	g->bStageReset = FALSE;
	if (pSiMc) {
		if (pSiMc->ResetXY()) {
			float iX, iY;
			// Check for all zeros [4/2/2013 Yuen]
			pSiMc->devGetXYPos(&iX, &iY, 10000);
			//20130426 SCOTT DELif ((abs(iX) < 500) && (abs(iY) < 500))
			{
				// Reset confirmed [4/3/2013 FSM413]
				pMX->SetProperties();
				pMY->SetProperties();
				if (pSiMc->devGetXYPos(&iX, &iY, 10000)) {
					float fX, fY;
					fX = pMX->fGetPos(iX);
					fY = pMY->fGetPos(iY);
					ReverseTransform(fX, fY, ActvCam2CoorSpace(g->ActiveCamera));
					switch (g->ActiveCamera) {
					default:
						g->LocXY.X = fX - g->COffEcho.X;
						g->LocXY.Y = fY - g->COffEcho.Y;
						break;
					case CCamBase::THINFCAM:
						g->LocXY.X = fX - g->COffTF.X;
						g->LocXY.Y = fY - g->COffTF.Y;
						break;
					// [05072024 ZHIMING
					case CCamBase::CWL:
						g->LocXY.X = fX - g->COffCWL.X;
						g->LocXY.Y = fY - g->COffCWL.Y;
						break;
					case CCamBase::ROUGH:
						g->LocXY.X = fX - g->COffRough.X;
						g->LocXY.Y = fY - g->COffRough.Y;
						break;
					// ]
					}
					g->bStageReset = TRUE;
				}
			}
		}
	}
	return g->bStageReset;
}

BOOL CMC3::Initialize(CListBox* pMessage) {
	if (pSiMc || pSiMcSimu) {
		ASSERT(0);
		return FALSE;
	}
	if (!p413App->bSimuStage) {
		// [6/22/2020]
		CString inifile, str;
		DosUtil.GetLocalCfgFile(inifile);
		int ver = GetPrivateProfileInt("Stage", "GLVersion", 0, inifile);
		if (ver == 1) {
			pSiMc = new CGLMotor_v1();
		}
		else {
			pSiMc = new CGLMotor();	// Actual comm class, shared by all motors, potential re-entrance candidates [6/14/2013 Yuen]
		}
		str.Format("%d", ver);
		WritePrivateProfileString("Stage", "GLVersion", str, inifile);
	}
	else {
		pSiMc = new CSiBase;
	}

	if (!pSiMc) { ASSERT(0); }

	if (!p413App->bSimuStage) {
		if (!pSiMc->OpenPort(p413App->SiHubPort)) // Open telnet port [6/1/2012 Yuen]
		{
			if (pMessage) pMessage->InsertString(pMessage->GetCount(), "System Motion Controller start FAILED");
			return FALSE;
		}
		// Wait a while for the network to get connected [8/5/2013 Yuen]
		pSiMc->WaitConnect();
	}
	else {
		pSiMc->bSimu = TRUE;
	}
	if (pMessage) pMessage->InsertString(pMessage->GetCount(), "System Motion Controller started OK");

	pMX = NULL;
	if (!p413App->bSimuX) // feature to bypass a single axis [6/14/2013 Yuen]
	{
		pMX = new CMotorGL(p413App->Global.X_Axis, pSiMc);
	}
	else {
		pMX = new CMotorL2(p413App->Global.X_Axis, pSiMc);
	}
	if (pMX) {
		pMX->vGetParameter("XMotor");
		pMX->vSaveParameter("XMotor");
		if (!pMX->Init(pMessage)) { delete pMX; pMX = NULL; }
	}

	pMY = NULL;
	if (!p413App->bSimuY) {
		pMY = new CMotorGL(p413App->Global.Y_Axis, pSiMc);
	}
	else {
		pMY = new CMotorL2(p413App->Global.Y_Axis, pSiMc);
	}
	if (pMY) {
		pMY->vGetParameter("YMotor");
		pMY->vSaveParameter("YMotor");
		if (!pMY->Init(pMessage)) { delete pMY; pMY = NULL; }
	}

	pMEchoTopZ = NULL;
	if (!p413App->bSimuEchoTopZ) {
		pMEchoTopZ = new CMotorGL(p413App->Global.EchoTop_Axis, pSiMc);
	}
	else {
		pMEchoTopZ = new CMotorL2(p413App->Global.EchoTop_Axis, pSiMc);
	}
	if (pMEchoTopZ) {
		pMEchoTopZ->vGetParameter("EchoTopMotor");
		pMEchoTopZ->vSaveParameter("EchoTopMotor");
		if (!pMEchoTopZ->Init(pMessage)) { delete pMEchoTopZ; pMEchoTopZ = NULL; }
	}

	// [08122024 ZHIMING
	if (p413App->Global.Confg.bThinFilmMotorMod) {
		if (!p413App->bSimuThinFilmZ) {
			pMThinFilmZ = new CMotorGL(p413App->Global.ThinFilm_Axis, pSiMc);
		}
		else {
			pMThinFilmZ = new CMotorL2(p413App->Global.ThinFilm_Axis, pSiMc);
		}
		if (pMThinFilmZ) {
			pMThinFilmZ->vGetParameter("ThinFilmMotor");
			pMThinFilmZ->vSaveParameter("ThinFilmMotor");
			if (!pMThinFilmZ->Init(pMessage)) { delete pMThinFilmZ; pMThinFilmZ = NULL; }
		}
	}

	if (p413App->Global.Confg.bRoughMotorMod) {
		if (!p413App->bSimuRoughZ) {
			pMRoughZ = new CMotorGL(p413App->Global.RoughZ_Axis, pSiMc);
		}
		else {
			pMRoughZ = new CMotorL2(p413App->Global.RoughZ_Axis, pSiMc);
		}
		if (pMRoughZ) {
			pMRoughZ->vGetParameter("RoughMotor");
			pMRoughZ->vSaveParameter("RoughMotor");
			if (!pMRoughZ->Init(pMessage)) {
				delete pMRoughZ;
				pMRoughZ = NULL;
			}
		}
	}
	//]
	return TRUE;
}
// [ 06112024/MORSALIN --> commented
//BOOL CMC3::GetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2) {
//	if (pSiMc) {
//		float iX, iY, iZ1, iZ2;
//		if (pSiMc->devGetPosAll(&iX, &iY, &iZ1, &iZ2)) {
//			// Convert reading base on polarity [6/5/2012 Administrator]
//			*pfX = pMX->fGetPos(iX);
//			*pfY = pMY->fGetPos(iY);
//			ReverseTransform(*pfX, *pfY, ActvCam2CoorSpace(p413App->Global.ActiveCamera));
//			*pfZ1 = pMEchoTopZ->fGetPos(iZ1);
//			*pfZ2 = pMThinFilmZ->fGetPos(iZ2);
//			CString msg;
//			msg.Format("MC3:GetPosAll: X=%.4f  Y=%.4f  Z1=%.4f  Z2=%.4f", *pfX, *pfY, *pfZ1, *pfZ2);
//			p413App->Log(msg);
//			return TRUE;
//		}
//		p413App->Log("MC3:GetPosAll: timeout", 2);
//		return FALSE;
//	}
//	p413App->Log("MC3:GetPosAll: fail", 2);
//	return FALSE;
//}
// ]

BOOL CMC3::GetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2, float* pfZ3) {
	if (pSiMc) {
		float iX, iY, iZ1, iZ2, iZ3;
		if (pSiMc->devGetPosAll(&iX, &iY, &iZ1, &iZ2, &iZ3)) {
			// Convert reading base on polarity [6/5/2012 Administrator]
			*pfX = pMX->fGetPos(iX);
			*pfY = pMY->fGetPos(iY);
			ReverseTransform(*pfX, *pfY, ActvCam2CoorSpace(p413App->Global.ActiveCamera));
			*pfZ1 = pMEchoTopZ->fGetPos(iZ1);
			*pfZ2 = pMThinFilmZ->fGetPos(iZ2);
			CString msg;
			msg.Format("MC3:GetPosAll: X=%.4f  Y=%.4f  Z1=%.4f  Z2=%.4f", *pfX, *pfY, *pfZ1, *pfZ2);
			p413App->Log(msg);
			return TRUE;
		}
		p413App->Log("MC3:GetPosAll: timeout", 2);
		return FALSE;
	}
	p413App->Log("MC3:GetPosAll: fail", 2);
	return FALSE;
}

BOOL CMC3::GetXYPos(float* pfX, float* pfY) {
	if (pSiMc) {
		float iX, iY;
		C413Global* g = &p413App->Global;
		if (pSiMc->devGetXYPos(&iX, &iY, 10000)) {
			// Convert reading base on polarity [6/5/2012 Administrator]
			*pfX = pMX->fGetPos(iX);
			*pfY = pMY->fGetPos(iY);
			ReverseTransform(*pfX, *pfY, ActvCam2CoorSpace(g->ActiveCamera));
			switch (g->ActiveCamera) {
			default:
				g->LocXY.X = *pfX = *pfX - g->COffEcho.X;
				g->LocXY.Y = *pfY = *pfY - g->COffEcho.Y;
				break;
			case CCamBase::THINFCAM:
				g->LocXY.X = *pfX = *pfX - g->COffTF.X;
				g->LocXY.Y = *pfY = *pfY - g->COffTF.Y;
				break;
			// [05072024 ZHIMING
			case CCamBase::CWL:
				g->LocXY.X = *pfX = *pfX - g->COffCWL.X;
				g->LocXY.Y = *pfY = *pfY - g->COffCWL.Y;
				break;
			case CCamBase::ROUGH:
				g->LocXY.X = *pfX = *pfX - g->COffRough.X;
				g->LocXY.Y = *pfY = *pfY - g->COffRough.Y;
				break;
			// ]
			}
			return TRUE;
		}
	}
	return FALSE;
}

float CMC3::GetFFUPressure() {
	if (pSiMc) {
		C413Global* g = &p413App->Global;
		g->FFUPressure = pSiMc->fGetFFUPressNow() / 10;
		return g->FFUPressure;
	}
	return 0;
}

BOOL CMC3::ResetX() {
	if (pSiMc) {
		if (pSiMc->ResetX()) {
			pMX->SetProperties();
			float pos;
			if (pMX->bGetPos(&pos)) {
				p413App->Global.LocXY.X = pos;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CMC3::ResetY() {
	if (pSiMc) {
		if (pSiMc->ResetY()) {
			pMY->SetProperties();
			float pos;
			if (pMY->bGetPos(&pos)) {
				p413App->Global.LocXY.Y = pos;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CMC3::SetMicroscopeLight(BOOL bOnOff) {
	if (pSiMc) {
		BOOL ret = pSiMc->SetMicroscopeLight(bOnOff);
		return ret;
	}
	return FALSE;
}

BOOL CMC3::SetChuckVacuum(BOOL bOnOff, int nWaferDiameter) {
	if (pSiMc) {
		BOOL ret = pSiMc->SetChuckVacuum(bOnOff, nWaferDiameter);
		return ret;
	}
	return FALSE;
}

// 20240814/Masum

BOOL CMC3::SetFFU(BOOL bOnOff)
{
	if (pSiMc) {
		BOOL ret = pSiMc->SetFFU(bOnOff);
		return ret;
	}
	return FALSE;
}

BOOL CMC3::SetSpectroPower(BOOL bOnOff) {
	if (pSiMc) {
		BOOL ret = pSiMc->SetSpectrometerPower(bOnOff);
		return ret;
	}
	return FALSE;
}

BOOL CMC3::ResetAll() {
	BOOL bRet = FALSE;
	if (pSiMc) {
		if (pSiMc->ResetAll()) {
			float iX, iY;
			pSiMc->devGetXYPos(&iX, &iY, 10000);
			//20130426 SCOTT DEL  //	if ((abs(iX) < 100) && (abs(iY) < 100))
			{
				// Reset confirmed [4/3/2013 FSM413]
				pMX->SetProperties();
				pMY->SetProperties();
				pMEchoTopZ->SetProperties();
				pMRoughZ->SetProperties();  

				pMX->setEncoderPos(pMX->Param.HomePos1 * 10000);
				pMY->setEncoderPos(pMY->Param.HomePos1 * 10000);

				if (pSiMc->devGetXYPos(&iX, &iY, 10000)) {
					float fX = pMX->fGetPos(iX);
					float fY = pMY->fGetPos(iY);
					C413Global* g = &p413App->Global;
					ReverseTransform(fX, fY, ActvCam2CoorSpace(g->ActiveCamera));
					switch (g->ActiveCamera) {
					default:
						g->LocXY.Set(fX - g->COffEcho.X, fY - g->COffEcho.Y);
						break;
					case CCamBase::THINFCAM:
						g->LocXY.Set(fX - g->COffTF.X, fY - g->COffTF.Y);
						break;
					// [05072024 ZHIMING
					case CCamBase::CWL:
						g->LocXY.Set(fX - g->COffCWL.X, fY - g->COffCWL.Y);
						break;
					case CCamBase::ROUGH:
						g->LocXY.Set(fX - g->COffRough.X, fY - g->COffRough.Y);
						break;
					// ]
					}
					g->LocEchoTopZ = g->LocRoughZ = 0;
					bRet = TRUE;
					g->bStageReset = TRUE;
				}
			}
		}
	}
	return bRet;
}

BOOL CMC3::IsWaferPresent() {
	if (pSiMc) {
		BOOL ret = !pSiMc->IsWaferPresent();
		return ret;
	}
	return TRUE;
}

void CMC3::ReconnectTCP() {
	if (pSiMc) {
		pSiMc->ReconnectTCP();
	}
}

// dX,dY in device coordinate [8/5/2013 Yuen]
long CMC3::devGetTOutDelay(float dX, float dY) {
	long v = 5000;
	float sps = 150.0f;	// steps per msec  [8/5/2013 Yuen]

	dX = fabs(dX);
	dY = fabs(dY);

	if (dX > dY) {
		v = 2 * dX / sps;
	}
	else {
		v = 2 * dY / sps;
	}
	if (v < 5000) {
		v = 5000;
	}
	return v;
}