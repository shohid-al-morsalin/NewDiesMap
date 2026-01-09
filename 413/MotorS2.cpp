// MotorS2.cpp: implementation of the CMotorS2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "MotorS2.h"

#include "..\E95\413App.h"

#include "SIMotor.h"	// Added by ClassView
#include "GLMotor.h"	// Added by ClassView
#include "MotorSi.h"
#include "MotorGL.h"
#include "..\413\MotorParam.h"
#include "GlobalDeviceContainer.h"
#include "..\IPC\Lock.h"
#include "MoveParam.h"

// DWORD ResetLapTime = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CMotorS2::IsDoorInterlockTriggered() {
	return 0;
}

// [05142024
BOOL CMotorS2::MoveEchoBottomZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	CString msg;
	float pos = g->LocEchoBottomZ;
	if (MoveZ(z, pMEchoBottomZ, bBlock)) {
		if (bBlock) {
			g->LocEchoBottomZ = pMEchoBottomZ->curPos;
			p413App->fCurEchoBottomZPos = g->LocEchoBottomZ;
			msg.Format("MS2:MoveEchoBottomZ: Zi=%.4f Zf=%.4f  dZ=%.4f", pos, g->LocEchoBottomZ, g->LocEchoBottomZ - z);
			p413App->Log(msg);
		}
		else {
			msg.Format("MS2:MoveEchoBottomZ: %.4f", z);
			p413App->Log(msg);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CMotorS2::GetEchoBottomZPosDev(float& zPos) {
	if (pMEchoBottomZ) {
		if (pMEchoBottomZ->fReadPos(zPos)) {
			CString msg;
			msg.Format("MS2:GetEchoBottomZPos: EchoBotZ=%.4f", zPos);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMotorS2::WaitEchoBottomZ() {
	if (pMEchoBottomZ) {
		if (pMEchoBottomZ->WaitForMotor(60000)) {
			C413Global* g = &p413App->Global;
			g->LocEchoBottomZ = pMEchoBottomZ->curPos;
			p413App->fCurEchoBottomZPos = g->LocEchoBottomZ;
			CString msg;
			msg.Format("MS2:WaitEchoBottomZ: %.4f", g->LocEchoBottomZ);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

// 20240814/Masum

BOOL CMotorS2::SetFFU(BOOL bOnOff)
{
	p413App->Global.bSetFFU = bOnOff;
	return TRUE;
}

CMotorS2::CMotorS2() {
	pMX = NULL;
	pMY = NULL;
	pMEchoTopZ = NULL;
	pMThinFilmZ = NULL;
	pMP = NULL;
	pSiMc = NULL;
	pSiMcSimu = NULL;

	bXYStageBusy = FALSE;
	//[20240426 tushar
	//pMEchoBottomZ=NULL;
	// [05132024 ZHIMING
	pMCWLTopZ = NULL;
	pMRoughZ = NULL;
	pSiMc2 = NULL;
	//]
	ErrStr = "";

	LoadParam();
	SaveParam();
}

CMotorS2::~CMotorS2() {
	if (pMX) {
		delete pMX;
	}
	if (pMY) {
		delete pMY;
	}
	if (pMEchoTopZ) {
		delete pMEchoTopZ;
	}
	if (pMThinFilmZ) {
		delete pMThinFilmZ;
	}
	if (pMP) {
		delete pMP;
	}
	//[20240426
	if (pMRoughZ) {
		delete pMRoughZ;
	}
	// [ 06112024/MORSALIN
	if (pMCWLTopZ) {
		delete pMCWLTopZ;
	}
	// ]
	/*if (pMEchoBottomZ) {
		delete pMEchoBottomZ;
	}*/
	if (pSiMc) {
		delete pSiMc;
	}
	if (pSiMcSimu) {
		delete pSiMcSimu;
	}
	if (pSiMc2) {
		delete pSiMc2;
	}
	// ]
}

DWORD WINAPI MoveAXYThread(LPVOID Param) {
	CMoveParam* p = (CMoveParam*)Param;
	if (!p->pObj) {
		return -1;
	}
	CMotorS2* pM = (CMotorS2*)(p->pObj);
	if (!pM->pMX || !pM->pMY) return 0;
	if (p->X < pM->pMX->Param.MinPos1) {
		p->X = pM->pMX->Param.MinPos1;
		p413App->Log("X less than minimum position, assumed minimum position");
	}
	if (p->X > pM->pMX->Param.MaxPos1) {
		p->X = pM->pMX->Param.MaxPos1;
		p413App->Log("X greater than maximum position, assumed maximum position");
	}
	if (p->Y < pM->pMY->Param.MinPos1) {
		p->Y = pM->pMY->Param.MinPos1;
		p413App->Log("Y less than minimum position, assumed minimum position");
	}
	if (p->Y > pM->pMY->Param.MaxPos1) {
		p->Y = pM->pMY->Param.MaxPos1;
		p413App->Log("Y greater than maximum position, assumed maximum position");
	}
	p->bRet = pM->MoveAXY(p->X, p->Y);
	return 0;
}

// Absolute move [6/22/2012 FSM413]
// Blocking [6/25/2012 FSM413]
// CXOffset and CYOffset included in MoveAXY() [6/4/2013 Yuen]
BOOL CMotorS2::GotoXY(float x, float y) {
	CString msg;
	bXYStageBusy = TRUE;
	msg.Format("MS2:S:GotoAXY: %.3f,%.3f", x, y);
	p413App->Log(msg);

	//BOOL ret = MoveAXY(x,y);	// Transformation is done in derived class [6/25/2012 FSM413]

	CMoveParam Param;
	Param.pObj = (LPVOID)this; // C413App object [8/7/2013 Yuen]
	Param.id = 91;
	Param.X = x;
	Param.Y = y;
	Param.h = CreateThread(NULL, 0, MoveAXYThread, (void*)&Param, 0, &Param.id);
	Param.dw = ::WaitForSingleObject(Param.h, INFINITE);

	C413Global* g = &p413App->Global;
	if (Param.bRet) {
		msg.Format("MS2:E:GotoAXY: %.3f,%.3f", g->LocXY.X, g->LocXY.Y);
		p413App->Log(msg);
	}
	else {
		msg.Format("MS2:E:GotoAXY FAILED: %.3f,%.3f", g->LocXY.X, g->LocXY.Y);
		p413App->Log(msg, 2);
	}
	bXYStageBusy = FALSE;

	return Param.bRet;
}

// Absolute move [6/22/2012 FSM413]
// Blocking [6/25/2012 FSM413]
BOOL CMotorS2::GotoXYZ1(float x, float y, float z) {
	CString msg;

	bXYStageBusy = TRUE;
	MoveEchoTopZ(z, FALSE);
	msg.Format("MS2:S:GotoAXYZ1: %.3f,%.3f,%.3f", x, y, z);
	p413App->Log(msg);

	//BOOL ret = MoveAXY(x,y);	// Transformation is done in derived class [6/25/2012 FSM413]

	CMoveParam Param;
	Param.pObj = (LPVOID)this; // C413App object [8/7/2013 Yuen]
	Param.id = 92;
	Param.X = x;
	Param.Y = y;
	Param.h = CreateThread(NULL, 0, MoveAXYThread, (void*)&Param, 0, &Param.id);
	Param.dw = ::WaitForSingleObject(Param.h, INFINITE);

	C413Global* g = &p413App->Global;
	if (Param.bRet) {
		msg.Format("MS2:E:GotoAXYZ1: %.3f,%.3f,%.3f", g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ);
		p413App->Log(msg);
	}
	else {
		msg.Format("MS2:E:GotoAXYZ1 FAIL: %.3f,%.3f,%.3f", g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ);
		p413App->Log(msg, 2);
	}
	WaitEchoTopZ();
	bXYStageBusy = FALSE;
	return Param.bRet;
}

// Blocking call [5/21/2014 FSMT]
DWORD WINAPI MoveRXYThread(LPVOID Param) {
	CMoveParam* p = (CMoveParam*)Param;
	if (!p->pObj) {
		return -1;
	}
	CMotorS2* pM = (CMotorS2*)(p->pObj);
	p->bRet = pM->MoveRXY1(p->X, p->Y);
	return 0;
}

// relative XY movement
BOOL CMotorS2::MoveRXY(float fX, float fY) {
	float fXi, fYi;

	CString msg;
	bXYStageBusy = TRUE;
	GetXYPos(&fXi, &fYi);
	msg.Format("MS2:S:MoveRXY: %.3f,%.3f", fXi, fYi);
	p413App->Log(msg);

	//BOOL ret = MoveRXY1(fX,fY);	// Transformation is done in derived class [6/25/2012 FSM413]

	CMoveParam Param;
	Param.pObj = (LPVOID)this; // C413App object [8/7/2013 Yuen]
	Param.id = 93;
	Param.X = fX;
	Param.Y = fY;
	Param.h = CreateThread(NULL, 0, MoveRXYThread, (void*)&Param, 0, &Param.id);
	Param.dw = ::WaitForSingleObject(Param.h, INFINITE);

	C413Global* g = &p413App->Global;
	msg.Format("MS2:E:MoveRXY: %.3f,%.3f", g->LocXY.X, g->LocXY.Y);
	p413App->Log(msg);
	bXYStageBusy = FALSE;
	return Param.bRet;
}

BOOL CMotorS2::WaitForAll(long tout) {
	BOOL bPos;
	float pfX, pfY, pfZ1, pfZ2, pfZ3;
	C413Global* g = &p413App->Global;
	if (pSiMc) {
		pSiMc->_WaitAllStop(tout, bPos);
		if (bPos) {
			pfX = pSiMc->moAxis[g->X_Axis - 1].fPos;
			pfY = pSiMc->moAxis[g->Y_Axis - 1].fPos;
			pfZ1 = pSiMc->moAxis[g->EchoTop_Axis - 1].fPos;
			pfZ2 = pSiMc->moAxis[g->RoughZ_Axis - 1].fPos;
			pfZ3 = pSiMc->moAxis[g->TopCWLZ_Axis - 1].fPos;  //06112024 / MORSALIN
		}
		else {
			pSiMc->devGetPosAll(&pfX, &pfY, &pfZ1, &pfZ2, &pfZ3);
		}
		pfX = pMX->fGetPos(pfX);
		pfY = pMY->fGetPos(pfY);
		pfZ1 = pMEchoTopZ->fGetPos(pfZ1);
		pfZ2 = pMRoughZ->fGetPos(pfZ2);
		pfZ3 = pMCWLTopZ->fGetPos(pfZ3);  //06112024/MORSALIN
		
	}
	ReverseTransform(pfX, pfY, ActvCam2CoorSpace(g->ActiveCamera));
	g->LocXY.Set(pfX, pfY);
	g->LocEchoTopZ = pfZ1;
	g->LocRoughZ = pfZ2;
	g->LocCWLTopZ = pfZ3; //06112024/MORSALIN

	return TRUE;
}

// Blocking [6/25/2012 FSM413]
// Absolute move [6/25/2012 FSM413]
// Final position is retrieve in blocking mode [6/15/2013 Yuen]
BOOL CMotorS2::MoveZ(float x, CMotorL2* pZ, BOOL bBlock) {
	if (!pZ) return FALSE;
	C413Global* g = &p413App->Global;

	if (p413App->backLashZ > 0) {
		if (pZ->Move(x + p413App->backLashZ)) {
			if (!pZ->WaitForMotor(60000)) {
				return FALSE;
			}
		}
		else {
			return FALSE;
		}
	}
	if (pZ->Move(x)) {
		if (bBlock) {
			if (!pZ->WaitForMotor(60000)) {
				return FALSE;
			}
			if (pZ->MotorID == g->EchoTop_Axis) {
				g->LocEchoTopZ = pZ->curPos;
			}
			else if (pZ->MotorID == g->RoughZ_Axis) {
				g->LocRoughZ = pZ->curPos;
			}
		}
		return TRUE;
	}
	return FALSE;
}

// Blocking [6/25/2012 FSM413]
// Absolute move [6/25/2012 FSM413]
BOOL CMotorS2::MoveEchoTopZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	CString msg;
	float pos = g->LocEchoTopZ;
	if (MoveZ(z, pMEchoTopZ, bBlock)) {
		if (bBlock) {
			g->LocEchoTopZ = pMEchoTopZ->curPos;
			CalcDPZOff(g->LocEchoTopZ);
			p413App->fCurEchoTopZPos = g->LocEchoTopZ;
			msg.Format("MS2:MoveEchoTopZ: Zi=%.4f Zf=%.4f  dZ=%.4f", pos, g->LocEchoTopZ, g->LocEchoTopZ - z);
			p413App->Log(msg);
		}
		else {
			msg.Format("MS2:MoveEchoTopZ: %.4f", z);
			p413App->Log(msg);
		}
		return TRUE;
	}
	return FALSE;
}

// Blocking [6/25/2012 FSM413]
// Absolute move [6/25/2012 FSM413]
BOOL CMotorS2::MoveThinFilmZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	CString msg;
	float pos = g->LocThinFilmZ;
	if (MoveZ(z, pMThinFilmZ, bBlock)) {
		if (bBlock) {
			g->LocThinFilmZ = pMThinFilmZ->curPos;
			msg.Format("MS2:MoveThinFilmZ: Zi=%.4f Zf=%.4f  dZ=%.4f", pos, g->LocThinFilmZ, g->LocThinFilmZ - z);
			p413App->Log(msg);
		}
		else {
			msg.Format("MS2:MoveThinFilmZ: %.4f", z);
			p413App->Log(msg);
		}
		return TRUE;
	}
	return FALSE;
}

// Do not lock [7/16/2013 Yuen]
BOOL CMotorS2::WaitEchoTopZ() {
	if (pMEchoTopZ) {
		if (pMEchoTopZ->WaitForMotor(60000)) {
			// Reload location after motor stopped [6/25/2012 FSM413]
			C413Global* g = &p413App->Global;
			g->LocEchoTopZ = pMEchoTopZ->curPos;
			CalcDPZOff(g->LocEchoTopZ);
			p413App->fCurEchoTopZPos = g->LocEchoTopZ;
			CString msg;
			msg.Format("MS2:WaitEchoTopZ: %.4f", g->LocEchoTopZ);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

// Do not lock [7/16/2013 Yuen]
BOOL CMotorS2::WaitThinFilmZ() {
	if (pMThinFilmZ) {
		if (pMThinFilmZ->WaitForMotor(60000)) {
			// Reload location after motor stopped [6/25/2012 FSM413]
			C413Global* g = &p413App->Global;
			g->LocThinFilmZ = pMThinFilmZ->curPos;
			CString msg;
			msg.Format("MS2:WaitThinFilmZ: %.4f", g->LocThinFilmZ);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

// Blocking [6/25/2012 FSM413]
// Relative move [6/25/2012 FSM413]
// Record final position is recorded [6/15/2013 Yuen]
BOOL CMotorS2::MoveRZ(float z, CMotorL2* pZ, BOOL bBlock) {
	if (!pZ) {
		return FALSE;
	}
	BOOL ret = FALSE;
	if ((z > 0) && (p413App->backLashZ > 0)) {
		if (pZ && pZ->MoveR(z + p413App->backLashZ)) {
			if (pZ->WaitForMotor(30000)) {
				if (pZ->MoveR(-p413App->backLashZ)) {
					ret = pZ->WaitForMotor(30000);
				}
			}
		}
	}
	else {
		if (pZ && pZ->MoveR(z)) {
			if (bBlock) {
				ret = pZ->WaitForMotor(30000);
			}
			else {
				return TRUE;
			}
		}
	}
	return ret;
}

// Blocking [6/5/2012 Administrator]
// Relative move [6/25/2012 FSM413]
BOOL CMotorS2::MoveREchoTopZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	float pos1 = g->LocEchoTopZ;
	if (MoveRZ(z, pMEchoTopZ, bBlock)) {
		g->LocEchoTopZ = pMEchoTopZ->curPos;
		CalcDPZOff(g->LocEchoTopZ);
		CString msg;
		msg.Format("MS2:MoveREchoTopZ: Zi=%.3f  Zf=%.3f  dZ=%.3f", pos1, g->LocEchoTopZ, g->LocEchoTopZ - (pos1 + z));
		p413App->Log(msg);
		return TRUE;
	}
	return FALSE;
}

// Blocking [6/5/2012 Administrator]
// Relative move [6/25/2012 FSM413]
BOOL CMotorS2::MoveRThinFilmZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	float pos = g->LocThinFilmZ;
	if (MoveRZ(z, pMThinFilmZ, bBlock)) {
		g->LocThinFilmZ = pMThinFilmZ->curPos;
		CString msg;
		msg.Format("MS2:MoveRThinFilmZ: Zi=%.3f  Zf=%.3f  dZ=%.3f", pos, g->LocThinFilmZ, g->LocThinFilmZ - (pos + z));
		p413App->Log(msg);
		return TRUE;
	}
	return FALSE;
}

// Blocking [6/5/2012 Administrator]
// Relative move [6/25/2012 FSM413]
BOOL CMotorS2::MoveRRoughZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	float pos = g->LocRoughZ;
	if (MoveRZ(z, pMRoughZ, bBlock)) {
		g->LocRoughZ = pMRoughZ->curPos;
		CString msg;
		msg.Format("MS2:MoveRRoughZ: Zi=%.3f  Zf=%.3f  dZ=%.3f", pos, g->LocRoughZ, g->LocRoughZ - (pos + z));
		p413App->Log(msg);
		return TRUE;
	}
	return FALSE;
}
// [ 06112024/MORSALIN
BOOL CMotorS2::MoveRCWLTopZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	float pos = g->LocCWLTopZ;
	if (MoveRZ(z, pMCWLTopZ, bBlock)) {
		g->LocCWLTopZ = pMCWLTopZ->curPos;
		CString msg;
		msg.Format("MS2:MoveRCWLTopZ: Zi=%.3f  Zf=%.3f  dZ=%.3f", pos, g->LocCWLTopZ, g->LocCWLTopZ - (pos + z));
		p413App->Log(msg);
		return TRUE;
	}
	return FALSE;
}
// ]

//[20240426 tushar
/*BOOL CMotorS2::MoveREchoBottomZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	float pos = g->LocEchoBottomZ;
	if (MoveRZ(z, pMEchoBottomZ, bBlock)) {
		g->LocEchoBottomZ = pMEchoBottomZ->curPos;
		CString msg;
		msg.Format("MS2:MoveREchoBottomZ: Zi=%.3f  Zf=%.3f  dZ=%.3f", pos, g->LocEchoBottomZ, g->LocEchoBottomZ - (pos + z));
		p413App->Log(msg);
		return TRUE;
	}
	return FALSE;*/
//}

// [05012024 ZHIMING
void CMotorS2::ResetEchoBottomZ() {
	if (!pMEchoBottomZ) return;
	C413Global* g = &p413App->Global;

	ResetZD(pMEchoBottomZ);
	pMEchoBottomZ->SetNormalSpeed();
	pMEchoBottomZ->MoveR(pMEchoBottomZ->Param.HomePos1);
	pMEchoBottomZ->bGetPos(&g->LocEchoBottomZ);
}

// [05072024
void CMotorS2::GetActiveCameraOffset(float& xoff, float& yoff) {
	C413Global* g = &p413App->Global;
	switch (g->ActiveCamera) {
	default:
		g->COffEcho.Get(xoff, yoff);
		break;
	case CCamBase::THINFCAM:
		g->COffTF.Get(xoff, yoff);
		break;
	// [05072024 ZHIMING
	case CCamBase::CWL:
		g->COffCWL.Get(xoff, yoff);
		break;
	case CCamBase::ROUGH:
		g->COffRough.Get(xoff, yoff);
		break;
	// ]
	}
}

BOOL CMotorS2::GetCWLTopZPosDev(float& zPos) {
	if (pMCWLTopZ) {
		if (pMCWLTopZ->fReadPos(zPos)) {
			CString msg;
			msg.Format("MS2:GetCWLZPos: ConfocalZ=%.4f", zPos);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMotorS2::MoveCWLTopZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	CString msg;
	float pos = g->LocCWLTopZ;
	if (MoveZ(z, pMCWLTopZ, bBlock)) {
		if (bBlock) {
			g->LocCWLTopZ = pMCWLTopZ->curPos;
			msg.Format("MS2:MoveCWLTopZ: Zi=%.4f Zf=%.4f  dZ=%.4f", pos, g->LocCWLTopZ, g->LocCWLTopZ - z);
			p413App->Log(msg);
		}
		else {
			msg.Format("MS2:MoveCWLTopZ: %.4f", z);
			p413App->Log(msg);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CMotorS2::WaitCWLTopZ() {
	if (pMCWLTopZ) {
		if (pMCWLTopZ->WaitForMotor(60000)) {
			C413Global* g = &p413App->Global;
			g->LocCWLTopZ = pMCWLTopZ->curPos;
			CalcDPZOff(g->LocCWLTopZ);
			p413App->fCurCWLTopZPos = g->LocCWLTopZ;
			CString msg;
			msg.Format("MS2:WaitCWLTopZ: %.4f", g->LocCWLTopZ);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMotorS2::GetRoughZPosDev(float& zPos) {
	if (pMRoughZ) {
		if (pMRoughZ->fReadPos(zPos)) {
			CString msg;
			msg.Format("MS2:GetRoughZPos: RoughZ=%.4f", zPos);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMotorS2::MoveRoughZ(float z, BOOL bBlock) {
	C413Global* g = &p413App->Global;

	CString msg;
	float pos = g->LocRoughZ;
	if (MoveZ(z, pMRoughZ, bBlock)) {
		if (bBlock) {
			g->LocRoughZ = pMRoughZ->curPos;
			msg.Format("MS2:MoveRoughZ: Zi=%.4f Zf=%.4f  dZ=%.4f", pos, g->LocRoughZ, g->LocRoughZ - z);
			p413App->Log(msg);
		}
		else {
			msg.Format("MS2:MoveRoughZ: %.4f", z);
			p413App->Log(msg);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CMotorS2::WaitRoughZ() {
	if (pMRoughZ) {
		if (pMRoughZ->WaitForMotor(60000)) {
			C413Global* g = &p413App->Global;
			g->LocRoughZ = pMRoughZ->curPos;
			CalcDPZOff(g->LocRoughZ);
			p413App->fCurRoughZPos = g->LocRoughZ;
			CString msg;
			msg.Format("MS2:WaitRoughZ: %.4f", g->LocRoughZ);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

//]

// Blocking [6/25/2012 FSM413]
// Absolute move [6/25/2012 FSM413]
BOOL CMotorS2::MoveX(float fX) {
	p413App->Global.LocXY.X = fX;
	ASSERT(0);
	return TRUE;  // Motor simulated [10/23/2010 FSMT]
}

// Absolute move [6/25/2012 FSM413]
BOOL CMotorS2::MoveY(float fY) {
	p413App->Global.LocXY.Y = fY;
	ASSERT(0);
	return TRUE;  // Motor simulated [10/23/2010 FSMT]
}

double CMotorS2::GetSPMMX() {
	if (pMX) {
		return pMX->Param.Scale;
	}
	return 1.0f;
}

double CMotorS2::GetSPMMY() {
	if (pMY) {
		return pMY->Param.Scale;
	}
	return 1.0f;
}

// Reset XY stage [6/25/2012 FSM413]
// Not applicable with Galil Controller [6/25/2012 FSM413]
BOOL CMotorS2::Reset() {
	p413App->Global.LocXY.Clear();
	p413App->Global.bStageReset = TRUE;
	return TRUE;
}

// Not applicable with Galil Controller [6/25/2012 FSM413]
BOOL CMotorS2::ResetP() {
	return TRUE;
}

void CMotorS2::ResetEchoTopZ() {
	if (!pMEchoTopZ) return;
	C413Global* g = &p413App->Global;

	ResetZD(pMEchoTopZ);
	pMEchoTopZ->SetNormalSpeed();
	if (pMEchoTopZ->bGetPos(&g->LocEchoTopZ)) {
		CalcDPZOff(g->LocEchoTopZ);
	}
}

void CMotorS2::ResetRoughZ() {
	if (!pMRoughZ) return;
	C413Global* g = &p413App->Global;

	ResetZD(pMRoughZ);
	pMRoughZ->SetNormalSpeed();

	float pos;
	if (pMRoughZ->bGetPos(&pos)) {
		g->LocRoughZ = pos;
	}
}

// [ 06112024/MORSALIN
  void CMotorS2::ResetCWLTopZ() {
	if (!pMCWLTopZ) return;
	C413Global* g = &p413App->Global;

	ResetZD(pMCWLTopZ);
	pMCWLTopZ->SetNormalSpeed();

	float pos;
	if (pMCWLTopZ->bGetPos(&pos)) {
		g->LocCWLTopZ = pos;
	}
}
// ]

BOOL CMotorS2::ResetXY() {
	C413Global* g = &p413App->Global;
	g->LocXY.Clear();
	g->bStageReset = TRUE;
	return TRUE;
}

BOOL CMotorS2::AllStop() {
	BOOL ret = FALSE;
	if (pSiMc) {
		BOOL ret = pSiMc->StopAll();
	}
	return ret;
}

BOOL CMotorS2::Initialize(CListBox* pMessage) {
	pMessage->InsertString(pMessage->GetCount(), "System Motion Controller (simu) started OK");
	return TRUE;
}

BOOL CMotorS2::GetXYPos(float* fX, float* fY) {
	C413Global* g = &p413App->Global;
	*fX = g->LocXY.X;
	*fY = g->LocXY.Y;

	CString msg;
	msg.Format("MS2:GetXYPos: X=%.4f  Y=%.4f", fX, fY);
	p413App->Log(msg);

	return TRUE;
}

// Get Z1 position from encoder [5/2/2013 user]
BOOL CMotorS2::GetEchoTopZPosDev(float& zPos) {
	if (pMEchoTopZ) {
		if (pMEchoTopZ->fReadPos(zPos)) {
			CString msg;
			msg.Format("MS2:GetEchoTopZPos: EchoTopZ=%.4f", zPos);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMotorS2::GetThinFilmZPosDev(float& zPos) {
	if (pMThinFilmZ) {
		if (pMThinFilmZ->fReadPos(zPos)) {
			CString msg;
			msg.Format("MS2:GetThinFilmZPos: ThinFilmZ=%.4f", zPos);
			p413App->Log(msg);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMotorS2::SetEchoTopZSpeed(float fSpeed) {
	if (pMEchoTopZ) {
		pMEchoTopZ->SetSpeed(fSpeed);
		return TRUE;
	}
	else {
		return FALSE;
	}
}

float CMotorS2::GetFFUPressure() {
	// simulation [4/28/2013 Administrator]
	return rand() / float(RAND_MAX);
}

void CMotorS2::SetHome() {
	if (pMX) {
		pMX->vSetPos(0);
	}
	if (pMY) {
		pMY->vSetPos(0);
	}
}

void CMotorS2::SetEncoderPosition(int a, int b) {
	if (pMX) {
		pMX->setEncoderPos(a);
	}
	if (pMY) {
		pMY->setEncoderPos(b);
	}
}

// [02/10/2022 ZHIMING]

BOOL CMotorS2::XPolarity() {
	if (!pMX) return FALSE;
	return pMX->Param.bPolarity;
}

BOOL CMotorS2::YPolarity() {
	if (!pMY) return FALSE;
	return pMY->Param.bPolarity;
}

void CMotorS2::LoadParam() {}

void CMotorS2::SaveParam() {}

BOOL CMotorS2::MoveRX(float dist) {
	p413App->Global.LocXY.X += dist;
	return TRUE;
}

BOOL CMotorS2::MoveRY(float dist) {
	p413App->Global.LocXY.Y += dist;
	return TRUE;
}

BOOL CMotorS2::MoveRP(float dist) {
	return TRUE;
}

BOOL CMotorS2::ResetX() {
	if (!pMX) return FALSE;
	p413App->Global.LocXY.X = 0;
	pMX->SetProperties();
	pMX->SetNormalSpeed();
	return TRUE;
}

BOOL CMotorS2::ResetY() {
	if (!pMY) return FALSE;
	p413App->Global.LocXY.Y = 0;
	pMY->SetProperties();
	pMY->SetNormalSpeed();
	return TRUE;
}

BOOL CMotorS2::SetMicroscopeLight(BOOL bOnOff) {
	p413App->Global.bEchoLight = bOnOff;
	return TRUE;
}

BOOL CMotorS2::SetChuckVacuum(BOOL bOnOff, int nWaferDiameter) {
	p413App->Global.bChuckVac = bOnOff;
	return TRUE;
}

BOOL CMotorS2::SetSpectroPower(BOOL bOnOff) {
	p413App->Global.bSpecPower = bOnOff;
	return TRUE;
}

BOOL CMotorS2::SetSpectroSourceLight(BOOL bOnOff) {
	p413App->Global.bSpectLight = bOnOff;
	return TRUE;
}

BOOL CMotorS2::IsSimu() {
	if (pSiMc) {
		return pSiMc->IsSimu();
	}
	return TRUE;
}

// Return tracked values [6/25/2012 FSM413]
BOOL CMotorS2::GetPosAll(float* pfX, float* pfY, float * pfEchoTopZ, float * pfRoughZ, float* pfCWLZ, float* pfEchoBottomZ) {
	C413Global* g = &p413App->Global;
	*pfX = g->LocXY.X;
	*pfY = g->LocXY.Y;
	*pfEchoTopZ = g->LocEchoTopZ;
	*pfRoughZ = g->LocRoughZ;
	*pfCWLZ = g->LocCWLTopZ;
	*pfEchoBottomZ = g->LocEchoBottomZ;
	CString msg;
	//msg.Format("MS2:GetPosAll: X=%.2f  Y=%.2f  EchoTopZ=%.2f  RoughZ=%.2f  EchoBotZ=%.2f", *pfX, *pfY, *pfEchoTopZ, *pfRoughZ, *pfEchoBottomZ);
	msg.Format("MS2:GetPosAll: X=%.2f  Y=%.2f  MicroscopeZ=%.2f  RoughZ=%.2f CWLZ=%.2f  EchoBotZ=%.2f", *pfX, *pfY, *pfEchoTopZ, *pfRoughZ, *pfCWLZ, *pfEchoBottomZ);
	p413App->Log(msg);
	return TRUE;
}

BOOL CMotorS2::ResetAll() {
	C413Global* g = &p413App->Global;
	g->LocXY.Clear();
	g->LocEchoTopZ = g->LocRoughZ = g->LocEchoBottomZ = g->LocThinFilmZ = 0;
	//if (p413App->Global.isStart != true || p413App->Global.isDuringMes != true) {
		g->LocCWLTopZ = 0; //20112025_Tushar
	//}
	
	g->bStageReset = TRUE;
	return TRUE;
}

BOOL CMotorS2::ResetAllWithConfirmation() {
	C413Global* g = &p413App->Global;
	g->LocXY.Clear();
	g->LocEchoTopZ = g->LocRoughZ = g->LocEchoBottomZ = g->LocThinFilmZ = g->LocCWLTopZ = 0;
	g->bStageReset = TRUE;
	return TRUE;
}

BOOL CMotorS2::SetSpectroIllumLight(BOOL bOnOff) {
	p413App->Global.bSpectLight = bOnOff;
	return TRUE;
}

// Read position from device [6/15/2013 Yuen]
BOOL CMotorS2::GetZPosDev(float& z1Pos, float& z2Pos) {
	if (!GetEchoTopZPosDev(z1Pos)) {
		return FALSE;
	}
	if (!GetRoughZPosDev(z2Pos)) {
		return FALSE;
	}
	return TRUE;
}

BOOL CMotorS2::MoveRXY1(float fX, float fY) {
	return TRUE;
}

void CMotorS2::ResetZD(CMotorL2* pZ) {}

BOOL CMotorS2::MoveAXY(float X, float Y) {
	return TRUE;
}

BOOL CMotorS2::MoveAXYZ1(float X, float Y, float Z1) {
	return TRUE;
}

BOOL CMotorS2::Transform(float& fX, float& fY, STAGECOORSPACE cspc) {
	CMotorParam* pParaX = &p413App->pMtr->pMX->Param;
	CMotorParam* pParaY = &p413App->pMtr->pMY->Param;

	if (!pParaX || !pParaY) {
		return FALSE;
	}

	// Rotate [6/22/2012 FSM413]
	float X, Y, A;
	A = pParaX->TxR.Tr;
	X = fX * cos(A) - fY * sin(A);
	Y = fX * sin(A) + fY * cos(A);

	// Translate [6/22/2012 FSM413]
	switch (cspc) {
	case ECHOSPACE:
		fX = X + pParaX->TxR.Tx;
		fY = Y + pParaX->TxR.Ty;
		break;
	case THINFSPACE:
		fX = X + pParaY->TxR.Tx;
		fY = Y + pParaY->TxR.Ty;
		break;
	/* 05072024 may need to add CWL and ROUGH space translation */
	default:
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL CMotorS2::ReverseTransform(float& fX, float& fY, STAGECOORSPACE cspc) {
	CMotorParam* pParaX = &p413App->pMtr->pMX->Param;
	CMotorParam* pParaY = &p413App->pMtr->pMY->Param;

	if (!pParaX) {
		return FALSE;
	}

	// Translate [6/22/2012 FSM413]
	switch (cspc) {
	case ECHOSPACE:
		fX = fX - pParaX->TxR.Tx;
		fY = fY - pParaX->TxR.Ty;
		break;
	case THINFSPACE:
		fX = fX - pParaY->TxR.Tx;
		fY = fY - pParaY->TxR.Ty;
		break;
	/* 05072024 may need to add CWL and ROUGH space translation */
	default:
		return FALSE;
		break;
	}

	// Rotate [6/22/2012 FSM413]
	float X, Y, A;
	A = -pParaX->TxR.Tr;
	X = fX * cos(A) - fY * sin(A);
	Y = fX * sin(A) + fY * cos(A);

	fX = X;
	fY = Y;

	return TRUE;
}

CMotorS2::STAGECOORSPACE CMotorS2::ActvCam2CoorSpace(CCamBase::CAMERAID cspc) {
	switch (cspc) {
	case CCamBase::ECHOCAM:
		return ECHOSPACE;
		break;
	case CCamBase::THINFCAM:
		return THINFSPACE;
		break;
	case CCamBase::CWL:
		return CWLSPACE;
		break;
	case CCamBase::ROUGH:
		return ROUGHSPACE;
		break;
	}
	return ECHOSPACE;
}

BOOL CMotorS2::IsWaferPresent() {
	return FALSE;
}

// Wait for P axis to stop [8/5/2013 Yuen]
BOOL CMotorS2::WaitP() {
	if (pMP) {
		return pMP->WaitForMotor(60000);
	}
	return FALSE;
}

// Record Z1 position for the purpose of total thickness measurement [5/2/2013 user]
void CMotorS2::CalcDPZOff(float pos) {
	C413Global* g = &p413App->Global;
	g->LocEchoTopZ = pos;	// Unit i mm [3/28/2013 Yuen]
	if (g->DPZPos) {
		g->DPZOff = g->DPZPos - pos * 1000; // convert pos to micron [3/28/2013 Yuen]
	}
}

void CMotorS2::ReconnectTCP() {}

BOOL CMotorS2::MoveP(float fP) {
	C413Global* g = &p413App->Global;
	g->LocP = fP;
	return TRUE;
}

BOOL CMotorS2::MovePR(float dist) {
	C413Global* g = &p413App->Global;
	g->LocP += dist;
	return TRUE;
}