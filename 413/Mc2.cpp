// Mc2.cpp: implementation of the CMC2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "Mc2.h"
#include "SIMotor.h"
#include "MotorSi.h"
#include "413App.h"
#include "GlobalDeviceContainer.h"

#include "SIMotorNoHub.h"

#include "..\IPC\Lock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMC2::CMC2() {}

CMC2::~CMC2() {}

BOOL CMC2::SetSpectroSourceLight(BOOL bOnOff) {
	BOOL ret = FALSE;
	if (pMThinFilmZ) {
		ret = pMThinFilmZ->SetOutput(1, !bOnOff);
		p413App->Global.bSpecSource = bOnOff;
	}
	return ret;
}

// [01192024 ZHIMING
BOOL CMC2::IsDoorInterlockTriggered() {
	if (pMX) {
		BOOL ret = !pMX->GetStatus(1);
		return ret;
	}
	return FALSE;
}

BOOL CMC2::IsWaferPresent() {
	if (pMY) {
		BOOL ret = !pMY->GetStatus(1);
		return ret;
	}
	return FALSE;
}
// ]

BOOL CMC2::MoveRXY1(float fX, float fY) {
	if (!pMX || !pMY) return FALSE;
	BOOL xCor = FALSE;
	BOOL yCor = FALSE;

	if (fX > 0) {
		if (!pMX->MoveR(fX + p413App->backLashX)) {
			return FALSE;
		}
		xCor = TRUE;
	}
	else {
		if (fX && (!pMX->MoveR(fX))) {
			return FALSE;
		}
	}
	if (fY > 0) {
		if (!pMY->MoveR((fY + p413App->backLashY))) {
			return FALSE;
		}
		yCor = TRUE;
	}
	else {
		if (!pMY->MoveR((fY))) {
			return FALSE;
		}
	}

	if (xCor) {
		if (!pMX->WaitForMotor(60000)) return FALSE;
		if (!pMX->MoveR(-p413App->backLashX)) {
			return FALSE;
		}
	}
	if (yCor) {
		if (!pMY->WaitForMotor(60000)) return FALSE;
		if (!pMY->MoveR(-p413App->backLashY)) {
			return FALSE;
		}
	}
	if (!pMX->WaitForMotor(60000)) {
		return FALSE;
	}
	if (!pMY->WaitForMotor(60000)) {
		return FALSE;
	}

	C413Global* g = &p413App->Global;
	//float pos;
	if (pMX->bGetPos(&g->LocXY.X)) {
		//g->LocXY.X = pos;
		if (pMY->bGetPos(&g->LocXY.Y)) {
			//g->LocXY.Y = pos;
			return TRUE;
		}
	}
	return FALSE;
}

void CMC2::ResetZD(CMotorL2* pZ) {
	if (pZ && pZ->Reset(-305.0f, pZ->Param.Speed)) {
		if (pZ->WaitForMotor(60000)) {
			//20250320_2===============
			if (pZ->MotorID == 4)
				//pZ->SetSpeed(4.0);
			{
				pZ->Move(1.5);
				pZ->Reset(-305.0f, 1.5);
				pZ->Param.Speed = 4.0; // CURRENTLY HARD CODE
				
			
			}
			if (pZ->WaitForMotor(60000))
			{
				pZ->SetSpeed(pZ->Param.Speed);
				pZ->SetProperties();
			}
		}
		//=======================
	}
}

// Blocking move [5/21/2014 FSMT]
BOOL CMC2::MoveAXY(float X, float Y) {
	if (!pMX || !pMY) return FALSE;
	float posX, posY;
	BOOL xCor = FALSE;
	BOOL yCor = FALSE;

	if (!pMX->bGetPos(&posX)) {
		ErrStr = "GetPos X";
		return FALSE;
	}
	if (!pMY->bGetPos(&posY)) {
		ErrStr = "GetPos Y";
		return FALSE;
	}

	if ((X - posX) > 0) {
		if (!pMX->Move(X + p413App->backLashX)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move X + backlash";
			return FALSE;
		}
		xCor = TRUE;
	}
	else {
		if (!pMX->Move(X)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move X + backlash";
			return FALSE;
		}
	}

	if ((Y - posY) > 0) {
		if (!pMY->Move(Y + p413App->backLashY)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Y + backlash";
			return FALSE;
		}
		yCor = TRUE;
	}
	else {
		if (!pMY->Move(Y)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Y";
			return FALSE;
		}
	}

	if (xCor) {
		if (!pMX->WaitForMotor(60000)) return FALSE;
		if (!pMX->Move(X)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move X";
			return FALSE;
		}
	}
	if (yCor) {
		if (!pMY->WaitForMotor(60000)) {
			ErrStr = "Wait for motors";
			return FALSE;
		}
		if (!pMY->Move(Y)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Y";
			return FALSE;
		}
	}
	if (!pMX->WaitForMotor(60000)) {
		ErrStr = "Wait for X timeout";
		return FALSE;
	}
	if (!pMY->WaitForMotor(60000)) {
		ErrStr = "Wait for Y timeout";
		return FALSE;
	}

	bXYStageBusy = FALSE;

	float d = 0;
	C413Global* g = &p413App->Global;

	if (pMX->bGetPos(&g->LocXY.X)) {
		d = g->LocXY.X - X;
		if (fabs(d) > 0.01f) {
			ErrStr.Format("Final Position dX = %.3f", d);
			//ASSERT(0);	//20211101 ZHIMING
			return FALSE;
		}
	}
	if (pMY->bGetPos(&g->LocXY.Y)) {
		d = g->LocXY.Y - Y;
		if (fabs(d) > 0.01f) {
			ErrStr.Format("Final Position dY = %.3f", d);
			//ASSERT(0);	//20211101 ZHIMING
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CMC2::MoveAXYZ1(float X, float Y, float Z1) {
	if (!pMX || !pMY || !pMEchoTopZ) return FALSE;
	float posX, posY, posZ;
	BOOL xCor = FALSE;
	BOOL yCor = FALSE;
	BOOL zCor = FALSE;

	if (!pMX->bGetPos(&posX)) {
		ErrStr = "GetPos X";
		return FALSE;
	}
	if (!pMY->bGetPos(&posY)) {
		ErrStr = "GetPos Y";
		return FALSE;
	}

	if ((X - posX) > 0) {
		if (!pMX->Move(X + p413App->backLashX)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move X + backlash";
			return FALSE;
		}
		xCor = TRUE;
	}
	else {
		if (!pMX->Move(X)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move X";
			return FALSE;
		}
	}

	if ((Y - posY) > 0) {
		if (!pMY->Move(Y + p413App->backLashY)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Y + backlash";
			return FALSE;
		}
		yCor = TRUE;
	}
	else {
		if (!pMY->Move(Y)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Y";
			return FALSE;
		}
	}

	if (!pMEchoTopZ->bGetPos(&posZ)) {
		ErrStr = "GetPos Z1";
		return FALSE;
	}
	if ((Z1 - posZ) > 0) {
		if (!pMEchoTopZ->Move(Z1 + p413App->backLashZ)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Z1 + backlash";
			return FALSE;
		}
		zCor = TRUE;
	}
	else {
		if (!pMEchoTopZ->Move(Z1)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Z1";
			return FALSE;
		}
	}

	if (xCor) {
		if (!pMX->WaitForMotor(60000)) {
			ErrStr = "Wait X";
			return FALSE;
		}
		if (!pMX->Move(X)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move X";
			return FALSE;
		}
	}
	if (yCor) {
		if (!pMY->WaitForMotor(60000)) {
			ErrStr = "Wait Y";
			return FALSE;
		}
		if (!pMY->Move(Y)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Y";
			return FALSE;
		}
	}
	if (zCor) {
		if (!pMEchoTopZ->WaitForMotor(60000)) {
			ErrStr = "wait Z1";
			return FALSE;
		}
		if (!pMEchoTopZ->Move(Z1)) {
			bXYStageBusy = FALSE;
			ErrStr = "Move Z1";
			return FALSE;
		}
	}

	if (!pMX->WaitForMotor(60000)) {
		ErrStr = "wait X";
		return FALSE;
	}
	if (!pMY->WaitForMotor(60000)) {
		ErrStr = "wait Y";
		return FALSE;
	}
	if (!pMEchoTopZ->WaitForMotor(60000)) {
		ErrStr = "wait Z1";
		return FALSE;
	}

	bXYStageBusy = FALSE;
	C413Global* g = &p413App->Global;

	float d = 0;
	if (pMX->bGetPos(&g->LocXY.X)) {
		d = g->LocXY.X - X;
		if (fabs(d) > 0.01f) {
			ErrStr.Format("Final Position dX = %.3f", d);
			//ASSERT(0);	//20211101 ZHIMING
			return FALSE;
		}
	}
	if (pMY->bGetPos(&g->LocXY.Y)) {
		d = g->LocXY.Y - Y;
		if (fabs(d) > 0.01f) {
			ErrStr.Format("Final Position dY = %.3f", d);
			//ASSERT(0);	//20211101 ZHIMING
			return FALSE;
		}
	}
	if (pMEchoTopZ->bGetPos(&posZ)) {
		g->LocEchoTopZ = posZ;
	}
	return TRUE;
}

BOOL CMC2::ResetXYSi() {
	if (!pMX || !pMY) return FALSE;
	BOOL bRet = FALSE;
	float fSteps = -315.0f;
	float spX = pMX->Param.Speed;
	float spY = pMY->Param.Speed;
	if (p413App->bResetYFirst) {
		pMY->Reset(-fSteps, 4.0f);
		if (!pSiMc->bSimu) Sleep(4000);
		pMX->Reset(fSteps, 5.0f);
		if (pMY->WaitForMotor(60000)) {
			if (pMX->WaitForMotor(60000)) {
				pMY->MoveR(-7.0f);
				pMX->MoveR(4.0f);
				if (!pMY->WaitForMotor(60000)) return FALSE;
				if (!pMX->WaitForMotor(60000)) return FALSE;
				pMY->Reset(16.0f, 0.05f);
				pMX->Reset(-13.0f, 0.10f);
				if (!pMY->WaitForMotor(60000)) return FALSE;
				if (!pMX->WaitForMotor(60000)) return FALSE;
				pMY->SetProperties();
				pMY->Param.Speed = spY;
				pMY->SetNormalSpeed();
				pMX->SetProperties();
				pMX->Param.Speed = spX;
				pMX->SetNormalSpeed();
				bRet = TRUE;
			}
		}
	}
	else {
		if (pMX->Reset(fSteps, 5.0f)) {
			if (pMY->Reset(fSteps, 2.6f)) {
				bRet = TRUE;
			}
		}
		if (pMX->WaitForMotor(60000)) {
			pMX->MoveR(4.0f);
			if (!pMX->WaitForMotor(60000)) return FALSE;
			pMX->Reset(-10.0f, 0.10f);
			if (!pMX->WaitForMotor(60000)) return FALSE;
			pMX->SetProperties();
			pMX->Param.Speed = spX;
			pMX->SetNormalSpeed();
		}
		if (pMY->WaitForMotor(60000)) {
			pMY->MoveR(4.0f);
			if (!pMY->WaitForMotor(60000)) return FALSE;
			pMY->Reset(-10.0f, 0.10f);
			if (!pMY->WaitForMotor(60000)) return FALSE;
			pMY->SetProperties();
			pMY->Param.Speed = spY;
			pMY->SetNormalSpeed();
		}
	}
	return bRet;
}

BOOL CMC2::ResetXY() {
	if (!pMX || !pMY) return FALSE;
	C413Global* g = &p413App->Global;
	g->bStageReset = FALSE;
	if (ResetXYSi()) {
		if (pMX->bGetPos(&g->LocXY.X)) {
			if (pMY->bGetPos(&g->LocXY.Y)) {
				g->bStageReset = TRUE;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CMC2::Initialize(CListBox* pMessage) {
	if (pSiMc || pSiMcSimu) {
		ASSERT(0);
		return FALSE;
	}

	// Always instantiate a simulation class [9/26/2012 Yuen]
	pSiMcSimu = new CSiBase;

	if (p413App->bSimuStage) {
		pSiMc = new CSiBase();
		pSiMc2 = new CSiBase();
		pMessage->InsertString(pMessage->GetCount(), "Motion Controller (simu) started OK");
	}
	else {
		pMessage->InsertString(pMessage->GetCount(), "Starting Motion Controller (MC2) ...");
		pSiMc = new CSiMotor();
		if (!pSiMc) {
			return FALSE;
		}
		if (!pSiMc->OpenPort(p413App->SiHubPort)) {
			pMessage->InsertString(pMessage->GetCount(), "Motion Controller start FAILED");
			return FALSE;
		}
		////[20240426 Tushar
		//if (p413App->Global.Confg.bEchoBottomMotorMod) {
		//	pSiMc2 = new CSiMotorNoHub();
		//	if (!pSiMc2->OpenPort(p413App->SiHubPort2)) {
		//		pMessage->InsertString(pMessage->GetCount(), "Motion Controller 2 start FAILED");
		//		return FALSE;
		//	}
		//}
		////]
		pMessage->InsertString(pMessage->GetCount(), "Motion Controller started OK");
	}

	pMX = NULL;
	if (!p413App->bSimuX) {
		pMX = new CMotorSi(p413App->Global.X_Axis, pSiMc);
	}
	else {
		pMX = new CMotorL2(p413App->Global.X_Axis, pSiMcSimu);
	}
	if (pMX) {
		pMX->vGetParameter("XMotor");
		pMX->vSaveParameter("XMotor");
		if (!pMX->Init(pMessage)) {
			delete pMX;
			pMX = NULL;
		}
	}

	pMY = NULL;
	if (!p413App->bSimuY) {
		pMY = new CMotorSi(p413App->Global.Y_Axis, pSiMc);
	}
	else {
		pMY = new CMotorL2(p413App->Global.Y_Axis, pSiMcSimu);
	}
	if (pMY) {
		pMY->vGetParameter("YMotor");
		pMY->vSaveParameter("YMotor");
		if (!pMY->Init(pMessage)) {
			delete pMY;
			pMY = NULL;
		}
	}

	pMEchoTopZ = NULL;
	if (p413App->Global.Confg.bEchoTopMotorMod) {
		if (!p413App->bSimuEchoTopZ) {
			pMEchoTopZ = new CMotorSi(p413App->Global.EchoTop_Axis, pSiMc);
		}
		else {
			pMEchoTopZ = new CMotorL2(p413App->Global.EchoTop_Axis, pSiMcSimu);
		}
		if (pMEchoTopZ) {
			pMEchoTopZ->vGetParameter("EchoTopMotor");
			pMEchoTopZ->vSaveParameter("EchoTopMotor");
			if (!pMEchoTopZ->Init(pMessage)) {
				delete pMEchoTopZ;
				pMEchoTopZ = NULL;
			}
		}
	}

	pMThinFilmZ = NULL;
	if (p413App->Global.Confg.bThinFilmMotorMod) {
		if (!p413App->bSimuThinFilmZ) {
			pMThinFilmZ = new CMotorSi(p413App->Global.ThinFilm_Axis, pSiMc, false);
		}
		else {
			pMThinFilmZ = new CMotorL2(p413App->Global.ThinFilm_Axis, pSiMcSimu);
		}
		if (pMThinFilmZ) {
			pMThinFilmZ->vGetParameter("ThinFilmMotor");
			pMThinFilmZ->vSaveParameter("ThinFilmMotor");
			if (!pMThinFilmZ->Init(pMessage)) {
				delete pMThinFilmZ;
				pMThinFilmZ = NULL;
			}
		}
	}

	pMP = NULL;
	if (p413App->Global.Confg.bPMotorMod) {
		if (!p413App->bSimuP) {
			pMP = new CMotorSi(p413App->Global.P_Axis, pSiMc, false);
		}
		else {
			pMP = new CMotorL2(p413App->Global.P_Axis, pSiMcSimu);
		}
		if (pMP) {
			pMP->vGetParameter("PMotor");
			pMP->vSaveParameter("PMotor");
			if (!pMP->Init(pMessage)) {
				delete pMP;
				pMP = NULL;
			}
		}
	}

	////[20240426 Tushar
	pMEchoBottomZ = NULL;
	if (p413App->Global.Confg.bEchoBottomMotorMod) {
		if (!p413App->bSimuEchoBottomZ) {
			// echo bottom motor uses additional 1240i board which is different from X-Y
			pMEchoBottomZ = new CMotorSi(p413App->Global.EchoBottomZ_Axis, pSiMc2, false);
		}
		else {
			pMEchoBottomZ = new CMotorL2(p413App->Global.EchoBottomZ_Axis, pSiMcSimu);
		}
		if (pMEchoBottomZ) {
			pMEchoBottomZ->vGetParameter("EchoBottomMotor");
			pMEchoBottomZ->vSaveParameter("EchoBottomMotor");
			if (!pMEchoBottomZ->Init(pMessage)) {
				delete pMEchoBottomZ;
				pMEchoBottomZ = NULL;
			}
		}
	}

	// [ 06112024/MORSALIN
	pMCWLTopZ = NULL;
	if (p413App->Global.Confg.bConfocalTopMotorMod) {
		if (!p413App->bSimuTopCWLZ) {
			pMCWLTopZ = new CMotorSi(p413App->Global.TopCWLZ_Axis, pSiMc);
		}
		else {
			pMCWLTopZ = new CMotorL2(p413App->Global.TopCWLZ_Axis, pSiMcSimu);
		}
		if (pMCWLTopZ) {
			pMCWLTopZ->vGetParameter("CWLMotor");
			pMCWLTopZ->vSaveParameter("CWLMotor");
			if (!pMCWLTopZ->Init(pMessage)) {
				delete pMCWLTopZ;
				pMCWLTopZ = NULL;
			}
		}
	}

	/*pMCWLTopZ = NULL;
	if (p413App->Global.Confg.bEchoMicroscopeMod) {
		if (!p413App->bSimuTopMicroscopeZ) {
			pMCWLTopZ = new CMotorSi(p413App->Global.EchoTop_Axis, pSiMc);
		}
		else {
			pMCWLTopZ = new CMotorL2(p413App->Global.EchoTop_Axis, pSiMcSimu);
		}
		if (pMCWLTopZ) {
			pMCWLTopZ->vGetParameter("TopEchoMotor");
			pMCWLTopZ->vSaveParameter("TopEchoMotor");
			if (!pMCWLTopZ->Init(pMessage)) {
				delete pMCWLTopZ;
				pMCWLTopZ = NULL;
			}
		}
	}*/
	//]

	// [05092024 ZHIMING
	pMRoughZ = NULL;
	if (p413App->Global.Confg.bRoughMotorMod) {
		if (!p413App->bSimuRoughZ) {
			pMRoughZ = new CMotorSi(p413App->Global.RoughZ_Axis, pSiMc);
		}
		else {
			pMRoughZ = new CMotorL2(p413App->Global.RoughZ_Axis, pSiMcSimu);
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

// Blocking [6/25/2012 FSM413]
// Absolute move [6/25/2012 FSM413]
// Backlash correction [6/25/2012 FSM413]
BOOL CMC2::MoveX(float fX) {
	if (!pMX) return FALSE;
	float pos;
	BOOL xCor = FALSE;

	if (!pMX->bGetPos(&pos)) {
		ErrStr = "GetPos X";
		return FALSE;
	}
	if ((fX - pos) > 0) {
		if (!pMX->Move(fX + p413App->backLashX)) {
			ErrStr = "Move X + backlash";
			return FALSE;
		}
		xCor = TRUE;
	}
	else {
		if (!pMX->Move(fX)) {
			ErrStr = "Move X";
			return FALSE;
		}
	}
	if (xCor) {
		if (!pMX->WaitForMotor(60000)) {
			ErrStr = "Wait X";
			return FALSE;
		}
		if (!pMX->Move(fX)) {
			ErrStr = "Move X";
			return FALSE;
		}
	}
	if (!pMX->WaitForMotor(60000)) {
		ErrStr = "Wait X";
		return FALSE;
	}

	C413Global* g = &p413App->Global;
	if (!pMX->bGetPos(&g->LocXY.X)) {
		ErrStr = "GetPos X";
		return FALSE;
	}
	float d = g->LocXY.X - fX;
	if (fabs(d) > 0.01f) {
		ErrStr.Format("Final Position dX = %.3f", d);
		//ASSERT(0);	//20211101 ZHIMING
		return FALSE;
	}
	return TRUE;
}

// Blocking [6/25/2012 FSM413]
// Absolute move [6/25/2012 FSM413]
// Backlash correction [6/25/2012 FSM413]
BOOL CMC2::MoveY(float fY) {
	if (!pMY) return FALSE;
	float pos;
	BOOL yCor = FALSE;

	if (!pMY->bGetPos(&pos)) {
		ErrStr = "GetPos Y";
		return FALSE;
	}
	if ((fY - pos) > 0) {
		if (!pMY->Move(fY + p413App->backLashY)) {
			ErrStr = "Move Y + backlash";
			return FALSE;
		}
		yCor = TRUE;
	}
	else {
		if (!pMY->Move(fY)) {
			ErrStr = "Move Y";
			return FALSE;
		}
	}
	if (yCor) {
		if (!pMY->WaitForMotor(60000)) {
			ErrStr = "Wait Y";
			return FALSE;
		}
		if (!pMY->Move(fY)) {
			ErrStr = "Move Y";
			return FALSE;
		}
	}
	if (!pMY->WaitForMotor(60000)) {
		ErrStr = "Wait Y";
		return FALSE;
	}

	C413Global* g = &p413App->Global;
	if (!pMY->bGetPos(&g->LocXY.Y)) {
		ErrStr = "GetPos Y";
		return FALSE;
	}
	float d = g->LocXY.Y - fY;
	if (fabs(d) > 0.01f) {
		ErrStr.Format("Final Position dY = %.3f", d);
		//ASSERT(0);	//20211101 ZHIMING
		return FALSE;
	}
	return TRUE;
}

BOOL CMC2::Reset() {
	if (!pMX || !pMY) return FALSE;
	C413Global* g = &p413App->Global;
	if (bXYStageBusy) {
		int nCount = 100;
		while (!bXYStageBusy && (nCount < 100)) {
			g->Sleep2(100);
		}
		if (nCount >= 100) {
			return FALSE;
		}
	}
	bXYStageBusy = TRUE;
	pMY->Reset(-305.0f, 2.5f);
	Sleep(4000);
	pMX->Reset(-305.0f, 2.5f);
	if (!pMX->WaitForMotor(60000)) return FALSE;
	pMX->SetProperties();
	if (!pMY->WaitForMotor(60000)) return FALSE;
	pMY->SetProperties();
	pMX->SetNormalSpeed();
	pMY->SetNormalSpeed();
	bXYStageBusy = FALSE;

	//float pos;
	if (pMX->bGetPos(&g->LocXY.X)) {
		//g->LocXY.X = pos;
		if (pMY->bGetPos(&g->LocXY.Y)) {
			//g->LocXY.Y = pos;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMC2::ResetP() {
	if (pMP) {
		pMP->Reset(-305.0f, 2.5f);
		if (!pMP->WaitForMotor(60000)) {
			return FALSE;
		}
		pMP->SetProperties();
		pMP->SetNormalSpeed();
		C413Global* g = &p413App->Global;
		pMP->bGetPos(&p413App->Global.LocP);
		return TRUE;
	}
	return FALSE;
}

BOOL CMC2::GetXYPos(float* fX, float* fY) {
	if (!pMX || !pMY) return FALSE;
	C413Global* g = &p413App->Global;
	//float pos;
	if (pMX->bGetPos(&g->LocXY.X)) {
		*fX = g->LocXY.X /*= pos*/;
		if (pMY->bGetPos(&g->LocXY.Y)) {
			*fY = g->LocXY.Y /*= pos*/;
			return TRUE;
		}
	}
	return FALSE;
}

float CMC2::GetFFUPressure() {
	float ret = 0;
	if (pMX) {
		ret = pMX->GetAnalogInput();
	}
	return ret;
}

BOOL CMC2::MoveRX(float dist) {
	if (!pMX) return FALSE;
	float fX;
	BOOL Cor = FALSE;

	if (!pMX->bGetPos(&fX)) {
		ErrStr = "GetPos X";
	}
	if (dist > 0) {
		if (!pMX->MoveR(dist + p413App->backLashX)) {
			ErrStr = "MoveR X + backlash";
			return FALSE;
		}
		Cor = TRUE;
	}
	else {
		if (!pMX->MoveR(dist)) {
			ErrStr = "MoveR X";
			return FALSE;
		}
	}
	if (Cor) {
		if (!pMX->WaitForMotor(60000)) {
			ErrStr = "Wait X";
			return FALSE;
		}
		if (!pMX->MoveR(-p413App->backLashX)) {
			ErrStr = "MoveR X - backlash";
			return FALSE;
		}
	}
	if (!pMX->WaitForMotor(60000)) {
		ErrStr = "Wait X";
		return FALSE;
	}

	C413Global* g = &p413App->Global;
	if (!pMX->bGetPos(&g->LocXY.X)) {
		ErrStr = "GetPos X";
		return FALSE;
	}
	float d = g->LocXY.X - fX;
	if (fabs(d) > 0.01f) {
		ErrStr.Format("Final Position dX = %.3f", d);
		//ASSERT(0);	//20211101 ZHIMING
		return FALSE;
	}
	return TRUE;
}

BOOL CMC2::MoveRY(float dist) {
	if (!pMY) return FALSE;
	float fY;
	BOOL Cor = FALSE;

	if (!pMY->bGetPos(&fY)) {
		ErrStr = "GetPos Y";
		return FALSE;
	}
	if (dist > 0) {
		if (!pMY->MoveR(dist + p413App->backLashY)) {
			ErrStr = "MoveR Y + backlash";
			return FALSE;
		}
		Cor = TRUE;
	}
	else {
		if (!pMY->MoveR(dist)) {
			ErrStr = "MoveR Y";
			return FALSE;
		}
	}
	if (Cor) {
		if (!pMY->WaitForMotor(60000)) {
			ErrStr = "Wait Y";
			return FALSE;
		}
		if (!pMY->MoveR(-p413App->backLashY)) {
			ErrStr = "MoveR Y - backlash";
			return FALSE;
		}
	}
	if (!pMY->WaitForMotor(60000)) {
		ErrStr = "Waut Y";
		return FALSE;
	}

	C413Global* g = &p413App->Global;
	if (!pMY->bGetPos(&g->LocXY.Y)) {
		return FALSE;
	}
	float d = g->LocXY.X - fY;
	if (fabs(d) > 0.01f) {
		ErrStr.Format("Final Position dY = %.3f", d);
		//ASSERT(0);	//20211101 ZHIMING
		return FALSE;
	}
	return TRUE;
}

BOOL CMC2::MoveRP(float dist) {
	C413Global* g = &p413App->Global;
	if (!pMP) {
		if (pSiMc && pSiMc->bSimu) {
			pMP->bGetPos(&g->LocP);
			return TRUE;
		}
		return TRUE;// No all setup has P motor [2/29/2012 FSMT]
	}

	BOOL Cor = FALSE;
	if (dist > 0) {
		if (!pMP->MoveR(dist + p413App->backLashX)) {
			return FALSE;
		}
		Cor = TRUE;
	}
	else {
		if (!pMP->MoveR(dist)) {
			return FALSE;
		}
	}

	if (Cor) {
		if (!pMP->WaitForMotor(60000)) {
			return FALSE;
		}
		pMP->MoveR(-p413App->backLashX);
	}
	BOOL ret = pMP->WaitForMotor(60000);

	pMP->bGetPos(&g->LocP);

	return ret;
}

BOOL CMC2::ResetX() {
	if (!pMX) return FALSE;
	pMX->Reset(-305.0f, 2.5f);
	if (pMX->WaitForMotor(60000)) {
		pMX->SetProperties();
		pMX->SetNormalSpeed();
		//float pos;
		if (pMX->bGetPos(&p413App->Global.LocXY.X)) {
			//p413App->Global.LocXY.X = pos;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMC2::ResetY() {
	if (!pMY) return FALSE;
	//pMY->Reset(-305.0f, 2.5f); //20250811/Shamim/Commented
	pMY->Reset(305.0f, 2.5f); //20250811/Shamim/ changed the direction sign
	if (pMY->WaitForMotor(60000)) {
		pMY->SetProperties();
		pMY->SetNormalSpeed();
		//float pos;
		if (pMY->bGetPos(&p413App->Global.LocXY.Y)) {
			//p413App->Global.LocXY.Y = pos;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMC2::SetMicroscopeLight(BOOL bOnOff) {
	if (pMX) {
		BOOL ret = pMX->SetOutput(1, !bOnOff);	// [01192024 for going forward, this bit will be reversed L for open, H for close
		p413App->Global.bEchoLight = bOnOff;
		return ret;
	}
	return FALSE;
}

BOOL CMC2::SetChuckVacuum(BOOL bOnOff, int nWaferDiameter) {
	if (pMX && pMY) {
		p413App->Global.bChuckVac = bOnOff;
		if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413SA) {
			// [01192024 ZHIMING STANDARDIZED
			if (nWaferDiameter >= 300) pMY->SetOutput(2, !bOnOff);	// 12 inch
			if (nWaferDiameter >= 200) pMY->SetOutput(1, !bOnOff);	// 8 inch
			if (nWaferDiameter >= 150) pMX->SetOutput(3, !bOnOff);	// 6 inch
			if (nWaferDiameter >= 100) pMX->SetOutput(2, !bOnOff);	// 4 inch
			// ]
		}
		else {
			pMX->SetOutput(2, !bOnOff);
			pMX->SetOutput(3, !bOnOff);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CMC2::SetSpectroPower(BOOL bOnOff) {
	if (pMThinFilmZ) {
		p413App->Global.bSpecPower = bOnOff;
		BOOL ret = pMThinFilmZ->SetOutput(2, !bOnOff);
		return ret;
	}
	return FALSE;
}

//[03182025_1

//BOOL CMC2::ResetAll() {
//	ResetP();
//	ResetEchoTopZ();
//	ResetRoughZ();
//	//ResetEchoBottomZ();
//	ResetCWLTopZ(); // Morsalin
//	p413App->Global.bStageReset = ResetXY();
//	return p413App->Global.bStageReset;
//}

// If resetAll with out messagebox necessary then use the previous ReseAll()  function

BOOL CMC2::ResetAll() {
	//ResetP();
	//ResetEchoTopZ();
	/*if (p413App->Global.isStart == true) {
		return TRUE;
	}*/
	//if (p413App->Global.isStart != true /*|| p413App->Global.isDuringMes != true*/)//20112025_Tushar
	//{
		ResetRoughZ();
		//ResetEchoBottomZ();
		ResetCWLTopZ(); // Morsalin
	//}
	/*p413App->Global.bStageReset = ResetXY();
	return p413App->Global.bStageReset;*/
	if (p413App->Global.isStart == true) {
		if (AfxMessageBox(_T("Do you want to Reset Stage?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			p413App->Global.bStageReset = ResetXY();
			return p413App->Global.bStageReset;

		}
		//else
		//{
		//	// Code to execute if NO is pressed
		//	return TRUE;
		//}
		return TRUE;
	}

	p413App->Global.bStageReset = ResetXY();
	return p413App->Global.bStageReset;
	
}
//]

BOOL CMC2::MoveP(float fP) {
	if (pMP) {
		if (pMP->Move(fP)) {
			if (pMP->WaitForMotor(60000)) {
				if (pMP->bGetPos(&p413App->Global.LocP)) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CMC2::MovePR(float dist) {
	if (pMP) {
		if (pMP->MoveR(dist)) {
			if (pMP->WaitForMotor(60000)) {
				if (pMP->bGetPos(&p413App->Global.LocP)) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}