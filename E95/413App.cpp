#include "stdafx.h"

#include "128/CAS/CasMgr.h"
#include "413/DMCMotor.h"
#include "413/EchoScope/EchoProbe.h"
#include "413/Mc2.h"
#include "413/OceanOp/OceanOpNIRQuest.h"
#include "CamLumi.h"
#include "CamSi.h"
#include "EchoProbeProp.h"
#include "EnlargeDlg.h"
#include "GlobalDeviceContainer.h"
#include "LocationProp.h"
#include "MC3.h"
#include "MotionControl.h"
#include "MPoint.h"
#include "PatternProp.h"
#include "PeakParam.h"
#include "QuickRecipeEditDlg.h"
#include <ProbeSwitching.h>
#include<algorithm> // 09242025
#include<random> //MAHEDI 10292025_MH


// [ 11082022 ZHIMING
#include "CWL.h"
// 04152024
#include "RoughProbe.h"
// ]
#include "RingWidthProbe.h" // 08132024 Mahedi Kamal

#include "Data.h"

#include "413App.h"
#include "..\serial\SerialICC.h" // Mahedi Kamal 10172024
#include "CWL2.h" // Mahedi Kamal 10182024

#include "413/MotorS2.h" // 09232025
#include "413/MotorL2.h" // 09232025

#include<fstream> // 09242025
#include<sstream> // 09242025


C413App* p413App = NULL;

C413App::C413App() {
	bUIEnabled = FALSE;
	bUIBlocked = FALSE;
	//[[20250926_Mohir
	char str[256];
	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);
	GetPrivateProfileString("Confocal", "ThicknessScanLength", "2.00", str, 255, inifile);
	ThicknessScanLength = atof(str);
	//]]
}

C413App::~C413App() {}

// [5/27/2020 Zhiming]
BOOL C413App::IsDoorInterlockTriggered() {
	if (!pMtr) return FALSE;
	
	//return pMtr->IsDoorInterlockTriggered();
	BOOL isPressed= pMtr->IsDoorInterlockTriggered();
	if(isPressed){
		p413App->Global.SetRecoveryStatus(1);//09092024
	}

	return isPressed;
}

BOOL C413App::StartSystem(CListBox* pMessage) {
	BOOL ret = TRUE;

	short CamTypeE = DosUtil.GetCameraType(CCamBase::ECHOCAM); // Read this from config file [4/18/2014 Administrator]
	short CamTypeT = DosUtil.GetCameraType(CCamBase::THINFCAM); // Read this from config file [4/18/2014 Administrator]
	if (CamTypeE < CamTypeT) {
		if (!CSICam::pCamECHO) {
			DosUtil.SetCameraType(CamTypeE, CCamBase::ECHOCAM);
			// instantiate [4/18/2014 Administrator]
			switch (CamTypeE) {
			case CCamBase::TYSICAM:
				CSICam::pCamECHO = new CCamSi;
				break;
			case CCamBase::TYLUMICAM:
				CSICam::pCamECHO = new CCamLumi;
				break;
			default:
				CSICam::pCamECHO = new CCamBase;
				break;
			}
			CSICam::pCamECHO->ID = CCamBase::ECHOCAM;
			if (!CSICam::Initialize(pMessage, CSICam::pCamECHO, CCamBase::ECHOCAM)) ret = FALSE;
		}

		if (!CSICam::pCamTF) {
			DosUtil.SetCameraType(CamTypeT, CCamBase::THINFCAM);
			// instantiate [4/18/2014 Administrator]
			switch (CamTypeT) {
			case CCamBase::TYSICAM:
				CSICam::pCamTF = new CCamSi;
				break;
			case CCamBase::TYLUMICAM:
				CSICam::pCamTF = new CCamLumi;
				break;
			default:
				CSICam::pCamTF = new CCamBase;
				break;
			}
			CSICam::pCamTF->ID = CCamBase::THINFCAM;
			if (!CSICam::Initialize(pMessage, CSICam::pCamTF, CCamBase::THINFCAM)) ret = FALSE;
		}
	}
	else {
		if (!CSICam::pCamTF) {
			DosUtil.SetCameraType(CamTypeT, CCamBase::THINFCAM);
			// instantiate [4/18/2014 Administrator]
			switch (CamTypeT) {
			case CCamBase::TYSICAM:
				CSICam::pCamTF = new CCamSi;
				break;
			case CCamBase::TYLUMICAM:
				CSICam::pCamTF = new CCamLumi;
				break;
			default:
				CSICam::pCamTF = new CCamBase;
				break;
			}
			CSICam::pCamTF->ID = CCamBase::THINFCAM;
			if (!CSICam::Initialize(pMessage, CSICam::pCamTF, CCamBase::THINFCAM)) ret = FALSE;
		}
		if (!CSICam::pCamECHO) {
			DosUtil.SetCameraType(CamTypeE, CCamBase::ECHOCAM);
			// instantiate [4/18/2014 Administrator]
			switch (CamTypeE) {
			case CCamBase::TYSICAM:
				CSICam::pCamECHO = new CCamSi;
				break;
			case CCamBase::TYLUMICAM:
				CSICam::pCamECHO = new CCamLumi;
				break;
			default:
				CSICam::pCamECHO = new CCamBase;
				break;
			}
			CSICam::pCamECHO->ID = CCamBase::ECHOCAM;
			if (!CSICam::Initialize(pMessage, CSICam::pCamECHO, CCamBase::ECHOCAM)) ret = FALSE;
		}
	}

	if (Global.Confg.bEchoMod) {
		if (bSimuEcho) {
			pEcho = new CEchoProbeBase;
		}
		else {
			pEcho = new CEchoProbe;
		}
		if (pEcho) {
			if (!pEcho->Initialize(pMessage)) ret = FALSE;
		}
		else ret = FALSE;

		// Echo motor section [10/27/2012 Yuen]
		if (bSimuDMC) {
			pDMC = new CDMCMotorBase;
		}
		else {
			pDMC = new CDMCMotor;
		}
		if (pDMC) {
			if (!pDMC->Initialize(Global.RcpSetup.MotorFile, pMessage)) ret = FALSE;
		}
		else ret = FALSE;

		// [11212022 ZHIMING
		if (bSimuConfocal) {
			pCWL = new CCWLBase;
			pCWL2 = new CCWLBase; // Mahedi Kamal 10182024
		}
		else {
			pCWL = new CCWL;
			pCWL2 = new CCWL2; // Mahedi Kamal 10182024
			//[ // Mahedi Kamal  10172024
			if (bConfocalRS232NotEthernet == TRUE) { // RS232 connection
				if (pCWL->m_pConfocalCom) { delete pCWL->m_pConfocalCom; pCWL->m_pConfocalCom = NULL; }
				pCWL->m_pConfocalCom = new CSerialICC;
				pCWL->m_pConfocalCom->isConfocalRS232 = TRUE;
				pCWL->ConnectRS232(ConfocalPort1, pMessage); // Mahedi Kamal
				pCWL->m_pConfocalCom->isConfocalRS232 = FALSE;

				//[// Mahedi Kamal 10182024
				if (pCWL2->m_pConfocalCom2) { delete pCWL2->m_pConfocalCom2; pCWL2->m_pConfocalCom2 = NULL; }
				pCWL2->m_pConfocalCom2 = new CSerialICC;
				pCWL2->m_pConfocalCom2->isConfocalRS232 = TRUE;
				pCWL2->ConnectRS232(ConfocalPort2, pMessage); // Mahedi Kamal
				pCWL2->m_pConfocalCom2->isConfocalRS232 = FALSE;
				//]
			}
			else { // Ethernet connection
				pCWL->Connect(sConfocalAddress, pMessage);
				pCWL->SetMSOFormat(7);
				//[ Mahedi Kamal 10182024
				pCWL2->Connect(sConfocalAddress2, pMessage);
				pCWL2->SetMSOFormat(7); 
				//]
			}
			//]
		}
		// ]
	}

	// [04082024 ZHIMING
	if (Global.Confg.bRoughness) {
		if (bSimuRough) {
			pRough = new CRoughProbeBase();
		}
		else {
			pRough = new CRoughProbe();
		}
		if (pRough) {
			if (!pRough->Initialize(nRoughPort, pMessage)) ret = FALSE;
		}
		else ret = FALSE;
	}
	// ]

	// [08132024 Mahedi Kamal
	if (Global.Confg.bTaiko) { // Taiko = Ring Width
		if (bSimuRingWidth) {
			pRingWidth = new CRingWidthProbeBase();
		}
		else {
			pRingWidth = new CRingWidthProbe();
		}
		if (pRingWidth) {
			if (!pRingWidth->Initialize(pMessage)) ret = FALSE;
		}
		else ret = FALSE;
	}
	// ]

	if (Global.Confg.bThinFilmMod) {
		switch (nSpectroType) {
		case 2:
			//pOceanOp = new COceanOpOmni;
			break;
		case 3:
			pOceanOp = new COceanOpNIRQuest;
			break;
		default:
			pOceanOp = new COceanBase;
			break;
		}

		if (pOceanOp) {
			if (!pOceanOp->Initialize(pMessage)) ret = FALSE;
		}
		else ret = FALSE;
	}

	return ret;
}

void C413App::StopSystem() {

	if (p413App->pMtr) p413App->pMtr->GotoXY(0, 0);//03062025

	if (pMCtrl) {
		pMCtrl->DestroyWindow();
		delete pMCtrl;
		pMCtrl = NULL;
	}

	if (CSICam::pCamTF) {
		CSICam::Deinitialize(CSICam::pCamTF);  // Do not delete [7/9/2010 128 NT]
		delete CSICam::pCamTF;
		CSICam::pCamTF = NULL;
	}
	if (CSICam::pCamECHO) {
		CSICam::Deinitialize(CSICam::pCamECHO);  // Do not delete [7/9/2010 128 NT]
		delete CSICam::pCamECHO;
		CSICam::pCamECHO = NULL;
	}

	if (pOceanOp) {
		pOceanOp->Close();
		delete pOceanOp;
		pOceanOp = NULL;
	}

	StopMotionSystem();
	StopProbeSwitchingSystem(); //02082024/MORSALIN
	if (pDMC) {
		pDMC->Deinitialize();
		delete pDMC;
		pDMC = NULL;
	}
	if (pEcho) {
		delete pEcho;
		pEcho = NULL;
	}

	// [04082024 ZHIMING
	if (pRough) {
		delete pRough;
		pRough = NULL;
	}
	// ]

	// [08132024 Mahedi Kamal
	if (pRingWidth) {
		delete pRingWidth;
		pRingWidth = NULL;
	}
	// ]

	//[ // Mahedi Kamal  10172024
	if (pCWL) {
		if (bConfocalRS232NotEthernet) {
			delete pCWL->m_pConfocalCom;
			pCWL->m_pConfocalCom = NULL;
			delete pCWL;
			pCWL = NULL;
		}
		else {
			pCWL->Disconnect();
			delete pCWL;
			pCWL = NULL;
		}
	}
	
	if (pCWL2) {//10182024
		if (bConfocalRS232NotEthernet) {
			delete pCWL2->m_pConfocalCom2;
			pCWL2->m_pConfocalCom2 = NULL;
			delete pCWL2;
			pCWL2 = NULL;
		}
		else {
			pCWL2->Disconnect();
			delete pCWL2;
			pCWL2 = NULL;
		}
	}

	//]

	if (FFUThread.bRun) {
		FFUThread.bRun = FALSE;
		while (!FFUThread.bStop) {
			Sleep(200);
		}
	}

	// 	if (pFFU)
	// 	{
	// 		pFFU->Close();
	// 		delete pFFU;
	// 		pFFU = NULL;
	// 	}

	// 	AramsE95.Deinit("Aram413.bin");

	
}

void C413App::Initialize(CListBox* pMessage) {
	ASSERT(0);
	StartSystem(pMessage);
}

void C413App::Deinitialize() {
	StopSystem();
}

BOOL C413App::EchoProp(CRecipe413& Rcp, short& MeSet) {
	if ((MeSet < 1) || (MeSet > MAXMEPASET)) {
		return FALSE;
	}
	CEchoProbeProp dlg;

	dlg.pRcp = &Rcp;
	dlg.MeSet = MeSet;

	if (dlg.DoModal() == IDOK) {
		MeSet = dlg.MeSet;
		return TRUE;
	}
	return FALSE;
}

BOOL C413App::PatternProp(CRecipe413& Rcp) {
	CPatternProp dlg;

	dlg.pRcp = &Rcp;
	if (dlg.DoModal() == IDOK) {
		// Update UI with new information [6/24/2010 Yuen]
		return TRUE;
	}
	return FALSE;
}

CLocationProp* pLocProp = NULL;

void C413App::LocationProp(CRecipe413& Rcp, HWND hWnd) {
	if (pLocProp) {
		delete pLocProp;
		pLocProp = NULL;
	}
	pLocProp = new CLocationProp;
	if (!pLocProp) {
		return;
	}
	pLocProp->pRcp = &Rcp;
	pLocProp->hWnd = hWnd;
	pLocProp->Create(CLocationProp::IDD);
	pLocProp->ShowWindow(SW_SHOW);
}

// Return FALSE if operation fail [2/15/2012 Administrator]

// Specialized function [8/26/2011 FSMT]

//[10132025_1

void C413App::Enlarge(CRecipe413* pRcp, CWaferMap* pWaferMap) {
	if (!pWaferMap) {
		ASSERT(0);
		return;
	}
	CEnlargeDlg dlg;
	dlg.m_cWaferMap.pRcp = pRcp;
	dlg.m_cWaferMap.MapCol = pWaferMap->MapCol;
	dlg.m_cWaferMap.DataCol = pWaferMap->DataCol;
	dlg.m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	dlg.m_cDiceMap.RedrawWithData();
	POSITION pos = pWaferMap->strList.GetHeadPosition();
	while (pos) {
		dlg.m_cWaferMap.strList.AddTail(pWaferMap->strList.GetNext(pos));
	}
	dlg.DoModal();
}

// For dicemap (ACME)
void C413App::Enlarge(CRecipe413* pRcp, CWaferMap* pWaferMap, CDiceView* m_cDiceMap) {
	if (!pWaferMap || !pRcp) {
		ASSERT(0);
		return;
	}

	CEnlargeDlg dlg;

	// Copy wafer map settings
	dlg.m_cWaferMap.pRcp = pRcp;
	dlg.m_cWaferMap.MapCol = pWaferMap->MapCol;
	dlg.m_cWaferMap.DataCol = pWaferMap->DataCol;
	dlg.m_cWaferMap.bSiteView = FALSE;
	dlg.m_cWaferMap.Title = pWaferMap->Title;
	dlg.m_cWaferMap.strList.RemoveAll();
	POSITION pos = pWaferMap->strList.GetHeadPosition();
	while (pos) {
		dlg.m_cWaferMap.strList.AddTail(pWaferMap->strList.GetNext(pos));
	}

	// Check if this is dice-type (LTV/STIR)
	BOOL bIsDiceType = FALSE;
	for (int j = 0; j < MAXMEPASET && !bIsDiceType; j++) {
		CMeParam* pMeP = &pRcp->MeParam[j];
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb == MParam::CONFOCAL_LTV ||
				pMeP->MPa[i].Prb == MParam::CONFOCAL_STIR) {
				bIsDiceType = TRUE;
				break;
			}
		}
	}

	//dlg.m_cDiceMap.SetDiceType(bIsDiceType);

	if (p413App->Global.setDiceDisplay == true) {
		// Replicate GenerateMap() logic here
		//dlg.m_cDiceMap.ClearCells();

		float WaferSize = pRcp->Wp.size;
		float m_Radius = WaferSize / 2.0f;

		// Find LTV and STIR indices
		int ltvIdx = -1, stirIdx = -1;
		for (int j = 0; j < MAXMEPASET; j++) {
			CMeParam* pMeP = &pRcp->MeParam[j];
			for (int i = 0; i < MaXPA; i++) {
				if (pMeP->MPa[i].Prb == MParam::CONFOCAL_LTV) ltvIdx = i;
				if (pMeP->MPa[i].Prb == MParam::CONFOCAL_STIR) stirIdx = i;
			}
		}

		// Copy global measurement strings (stt)
		//dlg.m_cDiceMap.SetMeasurementData(p413App->Global.stt);

		// Dice grid parameters
		float Ax = pRcp->AnchorX;
		float Ay = pRcp->AnchorY;
		float dieW = pRcp->DieWidth;
		float dieH = pRcp->DieHeight;

		if (dieW <= 0 || dieH <= 0) return;

		int idx = 1;
		for (int j = -50; j <= 50; j++) {
			for (int i = -50; i <= 50; i++) {
				float bx = Ax + i * dieW;
				float by = Ay + j * dieH;

				// Check if die is on wafer
				auto isInside = [&](float x, float y) {
					return sqrt(x * x + y * y) <= m_Radius;
				};
				int cnt = 0;
				cnt += isInside(bx, by);
				cnt += isInside(bx + dieW, by);
				cnt += isInside(bx + dieW, by + dieH);
				cnt += isInside(bx, by + dieH);

				if (cnt > 0) {
					if (!pRcp->m_bIncludeOn && cnt != 4) continue;
					dlg.m_cDiceMap.AddCellWithData(bx, by, dieW, dieH, idx, m_Radius, ltvIdx, stirIdx, "", pRcp->MeParam[0].MPa[0].D);
					idx++;
				}
			}
		}
		//dlg.m_cDiceMap.RedrawWithData();
	}

	dlg.DoModal();
}
//]
#define MAX_LOGMSGITEM	1024

// When code == 1, means that point has been correctly measure and should not be measured again [9/3/2010 XPMUser]
BOOL C413App::MeasureOnePointEcho(CRecipe413* pRcp, CMPoint* p, short* code, short nTrial, CCamBase::CAMERAID ActCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, BOOL bDoNotMoveZ) {
	if (!pRcp->IsValidMeSet(p)) {
		return FALSE;
	}

	C413Global* g = &Global;

	LoadSimuDIB(pVideo);

	p->ProgressSM = CMPoint::MEASURING;

	//float dd = pRcp->fStepSizeEcho;

	// Initialize working values [3/14/2010 Valued Customer]
	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		code[i] = 0;  // all measurement NOT OK [3/14/2010 Valued Customer]
	}
	int j;
	float X, Y;
	BOOL bMatched = FALSE;
	SDoPattrnMatching Param;

	char tch[255]; //20131017-1
	//130827 SCOTT char tch[255];
	// Only do it on first point [4/25/2013 FSMT]
	X = p->Co.x;
	Y = p->Co.y;
	MsgStr.Format("First Point Recipe Position: X=%.4f Y=%.4f", X, Y);
	Message(pList, MsgStr);
	Global.DLoc.GetCoor(X, Y); //CDiceLoc DLoc;
	MsgStr.Format("First Point GetCoor Position: X=%.4f Y=%.4f", X, Y);
	Message(pList, MsgStr);
	MsgStr.Format("Trial 1: X=%.4f Y=%.4f Z=%.4f", X, Y, pRcp->fEchoTopZ);

	//msg.Format("Measure point %d (%s)", p->Co.n, p->Co.GetXYStr());
	sprintf(tch, "<< MeasureOnePoointEcho New Pos X=%.3f Y=%.3f", X, Y);
	OutputDebugString(tch);

	Message(pList, MsgStr);
	if (!bDoNotMoveZ) {
		if (!pMtr->GotoXYZ1(X, Y, pRcp->fEchoTopZ)) {
			return FALSE;
		}
		//if (!pMtr->MoveEchoBottomZ(pRcp->fEchoBottomZ)) {
		//	return FALSE;
		//}
	}
	else {
		MsgStr = "Z1 is not moved";
		Message(pList, MsgStr);
		if (!pMtr->GotoXY(X, Y)) {
			return FALSE;
		}
	}

	Param.initX = g->LocXY.X;
	Param.initY = g->LocXY.Y;
	//pMtr->GetXYPos(&Param.initX,&Param.initY);

	// Perform Echo Auto Focus here [4/21/2013 Administrator]
	if (pRcp->IsAutoFocus(p)) {
		AutoFocusEcho(pRcp, g->ActiveCamera, pVideo, pRcp->EchoFocus, NULL);
	}

	for (j = 0; j < nTrial; j++) {
		if (g->bAborting) {
			break;
		}
		if (pMtr && (j != 0)) {
			MsgStr.Format("Trial %d: X=%.4f Y=%.4f", j + 1, X + gPX[j] * pRcp->fStepSizeEcho, Y + gPY[j] * pRcp->fStepSizeEcho);
			Message(pList, MsgStr);

			sprintf(tch, "<< Trial %d Pos X=%.3f Y=%.3f", j + 1, X + gPX[j] * pRcp->fStepSizeEcho, Y + gPY[j] * pRcp->fStepSizeEcho);
			OutputDebugString(tch);

			if (!pMtr->GotoXY(X + gPX[j] * pRcp->fStepSizeEcho, Y + gPY[j] * pRcp->fStepSizeEcho)) {
				return FALSE;
			}
		}

		Yld(pRcp->settlingTime * 1000);				//01312024/yukchiu / new read settling time from recipe thought dialog box

		if ((p->PatSet > 0) && !bMatched) {
			if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::EXPOSE4) {
				pVideo->Level = pRcp->AutoExposeEcho;
				pVideo->AutoAdjust();
			}
			if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::FOCAMATCH) {
				AutoFocusVideo(CCamBase::ECHOCAM, pRcp->fTgtGRnX[p->PatSet - 1][0], pRcp->fTgtGRnY[p->PatSet - 1][0], pVideo);
			}
			// Do pattern matching [3/28/2013 Yuen]
			Param.Set(pVideo, pImgWnd, pList);
			SetupParam(pRcp, Param, 0);
			Param.bApplyOffset = FALSE;
			Param.bMoveCenter = TRUE;
			Param.PaSSet = 0;	// Assume 0 [5/5/2014 Administrator]
			Param.bApplyOffset = TRUE;
			Param.SearchDepth = 12;
			if (pRcp->MatchAlgorithm[p->PatSet - 1] == 4) {
				Param.bMoveCenter = TRUE;
				Param.bApplyOffset = FALSE;
			}
			else {
				Param.bMoveCenter = FALSE;
			}

			for (int k = 1; k < pRcp->nTrialPat; k++) {
				if (Global.bAborting) {
					break;
				}
				short PatMatchRsl = DoPattrnMatching(Param); // Echo Match [6/28/2013 Yuen]
				switch (PatMatchRsl) {
				case C413AppPatt::pmOK:
					float X1, Y1;
					if (pMtr) {
						pMtr->GetXYPos(&p->Co.x, &p->Co.y);	// Update current point [6/10/2014 FSMT]
						X = p->Co.x;
						Y = p->Co.y;
						if (p->Co.p > 0) {
							X1 = g->LocXY.X;
							Y1 = g->LocXY.Y;
							//pMtr->GetXYPos(&X1,&Y1);
							Global.DLoc.Pt[p->Co.p].Set(X1, Y1);
							Global.DLoc.Calc();
						}

						sprintf(tch, "<< Pat Matched Pos X=%.3f Y=%.3f", X1, Y1);
						OutputDebugString(tch);
						bMatched = TRUE;
					}
					break;
					// Many other cases need to consider [2/25/2013 Administrator]
				default:
					// Move to new search location [5/26/2014 FSMT]
					MsgStr.Format("Trial %d: X=%.4f Y=%.4f", k + 1, X + gPX[k] * pRcp->fStepSizePat, Y + gPY[k] * pRcp->fStepSizePat);
					Message(pList, MsgStr);

					if (!pMtr->GotoXY(X + gPX[k] * pRcp->fStepSizePat, Y + gPY[k] * pRcp->fStepSizePat)) {
						return FALSE;
					}
					continue;
					break;
				}
				if (bMatched) {
					break;
				}
			}

			if (bMatched) {
				if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::FOCUS3) {
					AutoFocusVideo(CCamBase::ECHOCAM, pRcp->fTgtGRnX[p->PatSet - 1][0], pRcp->fTgtGRnY[p->PatSet - 1][0], pVideo);
				}
				else {
					// Only effective if pattern matching is requested [6/30/2014 Yuen]
					pMtr->MoveEchoTopZ(pRcp->fEchoTopZ + pRcp->TFZOffset);  // reuse of mutually exclusive variable [10/26/2011 Administrator]
				}
			}
			else {
				MsgStr = "Fail pattern matching";
				Message(pList, MsgStr);
				return FALSE;
			}
		}

		if (GetSubstrateThicknesses(pRcp, p, (j == (nTrial - 1)), code, pList, pVideo)) {
			sprintf(tch, "<< After Measurement Pos X=%.3f Y=%.3f", g->LocXY.X, g->LocXY.Y);
			OutputDebugString(tch);

			p->ProgressSM = CMPoint::COMPLETED;	// Indicates all measurement items measured successfully [9/5/2012 Administrator]
			return TRUE;
		}
		else {
			// MsgStr contain message to display [9/30/2012 Yuen]
			Message(pList, MsgStr);
		}
	}

	p->ProgressSM = CMPoint::FAILED;	// Indicates some or all measurement items could not be measured [9/5/2012 Administrator]
	pMtr->MoveEchoTopZ(pRcp->fEchoTopZ);
	return FALSE;
}

//[ Mahedi Kamal - 03202024
// When code == 1, means that point has been correctly measure and should not be measured again [9/3/2010 XPMUser]
//BOOL C413App::MeasureOnePointRough(CRecipe413* pRcp, CMPoint* p, short* code, short nTrial, CCamBase::CAMERAID ActCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, BOOL bDoNotMoveZ) {
BOOL C413App::MeasureOnePointRough(CRecipe413* pRcp, CMPoint* p, short* code, short nTrial, CSICam* pVideo, short NAver, CImgWnd* pImgWnd, CListCtrl* pList, BOOL bDoNotMoveZ /*BOOL bSettling*/) {
	if (!pRcp->IsValidMeSet(p)) {
		return FALSE;
	}

	C413Global* g = &Global;
	g->ActiveCamera = CCamBase::CAMERAID::ROUGH;
	LoadSimuDIB(pVideo);

	p->ProgressSM = CMPoint::MEASURING;

	// Initialize working values [3/14/2010 Valued Customer]
	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		code[i] = 0;  // all measurement NOT OK [3/14/2010 Valued Customer]
	}
	int j;
	float X, Y;
	BOOL bMatched = FALSE;
	SDoPattrnMatching Param;

	char tch[255];
	X = p->Co.x;
	Y = p->Co.y;
	MsgStr.Format("First Point Recipe Position: X=%.4f Y=%.4f", X, Y);
	Message(pList, MsgStr);
	Global.DLoc.GetCoor(X, Y);
	MsgStr.Format("First Point GetCoor Position: X=%.4f Y=%.4f", X, Y);
	Message(pList, MsgStr);
	MsgStr.Format("Trial 1: X=%.4f Y=%.4f Z=%.4f", X, Y, pRcp->fRoughZ);

	sprintf(tch, "<< MeasureOnePoointRough New Pos X=%.3f Y=%.3f", X, Y);
	OutputDebugString(tch);

	Message(pList, MsgStr);
	if (!pMtr->GotoXY(X, Y)) {
		return FALSE;
	}

	Param.initX = g->LocXY.X;
	Param.initY = g->LocXY.Y;

	for (j = 0; j < nTrial; j++) {
		if (g->bAborting) {
			break;
		}
		if (pMtr && (j != 0)) {
			MsgStr.Format("Trial %d: X=%.4f Y=%.4f", j + 1, X + gPX[j] * pRcp->fStepSizeEcho, Y + gPY[j] * pRcp->fStepSizeEcho);
			Message(pList, MsgStr);

			sprintf(tch, "<< Trial %d Pos X=%.3f Y=%.3f", j + 1, X + gPX[j] * pRcp->fStepSizeEcho, Y + gPY[j] * pRcp->fStepSizeEcho);
			OutputDebugString(tch);

			if (!pMtr->GotoXY(X + gPX[j] * pRcp->fStepSizeEcho, Y + gPY[j] * pRcp->fStepSizeEcho)) {
				return FALSE;
			}
		}

		if (GetSubstrateRoughness(pRcp, p, (j == (nTrial - 1)), code, pList)) {
			sprintf(tch, "<< After Measurement Pos X=%.3f Y=%.3f", g->LocXY.X, g->LocXY.Y);
			OutputDebugString(tch);

			p->ProgressSM = CMPoint::COMPLETED;	// Indicates all measurement items measured successfully [9/5/2012 Administrator]
			return TRUE;
		}
		else {
			// MsgStr contain message to display [9/30/2012 Yuen]
			Message(pList, MsgStr);
		}
	}

	p->ProgressSM = CMPoint::FAILED;	// Indicates some or all measurement items could not be measured [9/5/2012 Administrator]
	pMtr->MoveRoughZ(0);
	return FALSE;
}
//]

//08142024 - START

BOOL C413App::PreMeasureRingWidth(CRecipe413* pRcp, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	if (bSimuRingWidth) return TRUE;

	C413Global* g = &Global;
	g->ActiveCamera = CCamBase::CAMERAID::ECHOCAM;
	CString msg = "";
	BOOL bTakeBGFailed = FALSE;

	msg = "Turn on microscope light";
	if (pMtr->SetMicroscopeLight(TRUE)) {
		Message(pList, msg, 2);
	}

	if (bCenterWafer) {
		msg = "Centering wafer";
		Message(pList, msg, 2);
		if (!p413App->CenterWafer(pRcp, 127, pVideo, pImgWnd, pList)) {
			msg = "Wafer centering FAILED, continue processing";
			Message(pList, msg, 2);
			bTakeBGFailed = TRUE;
		}
		//////////////////////////////////////////////////////////////////////////
		msg = "Wafer centering results";
		Message(pList, msg, 2);
		msg.Format("X0= %.3f, X90= %.3f, X180= %.3f, X270= %.3f", WC.X0, WC.X90, WC.X180, WC.X270);
		Message(pList, msg, 2);
		msg.Format("Y0= %.3f, Y90= %.3f, Y180= %.3f, Y270= %.3f", WC.Y0, WC.Y90, WC.Y180, WC.Y270);
		Message(pList, msg, 2);
		msg.Format("XOffset = %.3f, YOffset = %.3f", WC.XOff, WC.YOff);
		Message(pList, msg, 2);
		//////////////////////////////////////////////////////////////////////////
	}
	else {
		msg = "Wafer centering NOT performed";
		Message(pList, msg, 2);
	}

	msg = "Taiko ring measurement";
	Message(pList, msg, 2);
	msg.Format("Goto Z1 position: %.3f um", pRcp->TRWMeasProp.ZPos);
	Message(pList, msg, 2);
	if (!pMtr->MoveEchoTopZ(pRcp->TRWMeasProp.ZPos)) {
		msg = "Top Z Move Error";
		Message(pList, msg, 2);
		WC.bOffsetInUse = FALSE;
		return FALSE;
	}
	pMtr->WaitEchoTopZ();
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	if (WC.bOffsetInUse) {
		msg = "Switch to centered coordinate";
		Message(pList, msg, 2);
		msg = "";
		msg.Format("Change to Centered Coordinates: %.3f, %.3f (offset)", WC.XOff, WC.YOff);
		Message(pList, msg, 2);
	}
	if (!p413App->FindBevel(pRcp->TRWMeasProp.waferRadius, pRcp->TRWMeasProp.offsetAngle, pRcp->TRWMeasProp.bevelWidth, pList, pVideo, pImgWnd)) {
		msg = "Find bevel width FAILED";
		Message(pList, msg, 2);
		msg = "Taiko ring measurement ABORTED";
		Message(pList, msg, 2);
		WC.bOffsetInUse = FALSE;
		return FALSE;
	}
	msg.Format("Bevel width: %.3f mm, Wafer radius: %.3f mm", pRcp->TRWMeasProp.bevelWidth, pRcp->TRWMeasProp.waferRadius);
	Message(pList, msg, 2);
	msg = "Taiko ring measurement started";
	Message(pList, msg, 2);

	if (!bCenterWafer || bTakeBGFailed) {
		msg = "Take background";
		if (!pMtr->GotoXY(0, 0)) { msg = "Goto Origin FAILED"; Message(pList, msg, 2); return FALSE; };
		CIM* pBg = p413App->GetBG();
#ifdef _SIMU_
		CString name; name = "C:\\413 OMNI\\IMG\\cc.bmp";
		pBg->Destroy();
		pBg->Load(name);
#else
		if (pBg) { pBg->Destroy(); }
		pVideo->SnapOneFrame();
		CPackedDIB* pDIB = &g->StoredDIB;
		if (pDIB->pDIB) pBg->Attach(pDIB);
#endif
		if (pBg->IsNull()) {
			msg = "Take background FAILED, Cannot continue";
			Message(pList, msg, 2);
			WC.bOffsetInUse = FALSE;
			return FALSE;
		}
	}
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	return TRUE;
}

BOOL C413App::MeasureOnePointRingWidth(CRecipe413* pRcp, CMPoint* p, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, short* code) {
	if (!pRcp->IsValidMeSet(p) || !pMtr) {
		return FALSE;
	}
	
	C413Global* g = &Global;
	p->ProgressSM = CMPoint::MEASURING;
	g->ActiveCamera = CCamBase::CAMERAID::ECHOCAM;
	CString msg = "";

	BOOL bRet = TRUE;
	if (!DoMeasureOnePointRingWidth(pRcp, p, pVideo, pImgWnd, pList)) {
		bRet = p413App->DoTaikoSearchAround(pRcp, p, pVideo, pImgWnd, pList); // IF FAILED THEN SEARCH AROUND
		bRet = FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	if (!bRet) {
		msg = "Taiko ring measurement failed";
		Message(pList, msg, 2);
		p->ProgressSM = CMPoint::FAILED;
		return FALSE;
	}
	else {msg = "Taiko ring one point measurement end"; Message(pList, msg, 2); }

	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		code[i] = 0;  // all measurement NOT OK [3/14/2010 Valued Customer]
	}

	if (GetSubstrateRingWidth(pRcp, p, TRUE, code, pList)) {
		short MeSetX = MaXPA * (p->MeSet - 1);
		CData* d = p->GetData(0, TRUE);

		int idx = 0;
		CMeParam* pMeP = &pRcp->MeParam[p->MeSet - 1];
		
		for (int i = 0; i < MaXPA; i++) {
			if ((pMeP->MPa[i].Prb > 0) && (*(code + i) == 1)) {
				idx = i + 1;
				break;
			}
		}
		if (idx > 0) {
			pRcp->TRWMeasProp.RingThk += pMeP->MPa[idx].Off; // 031024
			d->Set(MeSetX + idx, pRcp->TRWMeasProp.RingThk);
			pRcp->TRWMeasProp.RingStepWidth += pMeP->MPa[idx+1].Off; // 031024
			d->Set(MeSetX + idx + 1, pRcp->TRWMeasProp.RingStepWidth);
			pRcp->TRWMeasProp.RingStepHeight += pMeP->MPa[idx + 2].Off; // 031024
			d->Set(MeSetX + idx + 2, pRcp->TRWMeasProp.RingStepHeight);
			pRcp->TRWMeasProp.RingSTD += pMeP->MPa[idx + 3].Off; // 031024
			d->Set(MeSetX + idx + 3, pRcp->TRWMeasProp.RingSTD);
			pRcp->TRWMeasProp.RingTTV += pMeP->MPa[idx + 4].Off; // 031024
			d->Set(MeSetX + idx + 4, pRcp->TRWMeasProp.RingTTV);
			pRcp->TRWMeasProp.RingStepWidthSTD += pMeP->MPa[idx + 5].Off; // 031024
			d->Set(MeSetX + idx + 5, pRcp->TRWMeasProp.RingStepWidthSTD);
			pRcp->TRWMeasProp.RingStepWidthTTV += pMeP->MPa[idx + 6].Off; // 031024
			d->Set(MeSetX + idx + 6, pRcp->TRWMeasProp.RingStepWidthTTV);
		}
	}

	p->ProgressSM = CMPoint::COMPLETED;

	return TRUE;
}

BOOL C413App::DoMeasureOnePointRingWidth(CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	if (bSimuRingWidth) return TRUE;
	
	if (!GenerateTaikoPoints(pRcp)) return FALSE;

	CMotorS2* pMC = pMtr;
	const short EKrnl = 3;
	CString str, name;
	CIM* pIm = GetImg();
	float fDist, bevelWidth = 0;
	float radius, angle, x, y, inner, outer;
	C413Global* g = &Global;
	CString msg = 0;

	double RingWidth = 0, RingStdDev = 0, RingTTV = 0, RingHeight = 0, StepHeight = 0;
	double RingThickness = 0, RingStepWidth = 0, RingStepDev = 0, RingStepTTV = 0;

	SHtPoint* htPt = &HtLn[g->TaikoPointCount++];
	x = htPt->x; y = htPt->y;

	// Goto position [6/20/2020 yuenl]
	g->Cartesian2Polar(x, y, radius, angle); // determine angle [6/20/2020 yuenl]
	if (!FindOuterEdge(radius, angle, fDist, pVideo, pImgWnd, pList)) {
		RingWidth = 0;  // Ring width
		RingStdDev = 0;  // Ring stdev
		RingTTV = 0;  // Ring TTV
		return FALSE;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// Move edge to center of image [6/20/2020 yuenl]
	radius += fDist;
	if (!MoveEdgeToCenter(fDist, radius, angle, pList)) return FALSE;
	//////////////////////////////////////////////////////////////////////////
	// Move to center of visible ring area [6/20/2020 yuenl]
	radius -= 1.23f; //TODO: estimate center of ring assuming ring width of 2.8mm [6/20/2020 yuenl]
	if (!MoveToPosition(radius, angle, pList)) return FALSE;
	// measure ring width [6/21/2020 yuenl]
	if (!FindStripWhite(radius, angle - 2 * pRcp->TRWMeasProp.offsetAngle, inner, outer, pVideo, pImgWnd ,pList)) return FALSE;
	/////////////////////////////////////////////////////////////////////////////
	// Move to center of ring [6/20/2020 yuenl]
	float rInner = radius + inner;
	float rOuter = radius + outer;
	radius = (rInner + rOuter) / 2.0f;
	//radius -= pIm->StatD.fAve / 2.0f;
	MoveToPosition(radius, angle, pList);
	// Find ring width [6/20/2020 yuenl]
	if (!FindStripWhite(radius, angle - 2 * pRcp->TRWMeasProp.offsetAngle, inner, outer, pVideo, pImgWnd, pList)) return FALSE;
	/////////////////////////////////////////////////////////////////////////////
	RingWidth = pIm->StatD.fAve + pRcp->TRWMeasProp.bevelWidth;  // Ring width
	pRcp->TRWMeasProp.ringWidth = pIm->StatD.fAve;
	pRcp->TRWMeasProp.RingSTD = RingStdDev = pIm->StatD.fStdev; // Ring stdev
	pRcp->TRWMeasProp.RingTTV = RingTTV = pIm->StatD.fMax - pIm->StatD.fMin; // Ring TTV
	str.Format("Width: %.3f mm, Offset: Inner(%.3f mm) Outer(%.3f mm)",
		pIm->StatD.fAve, inner, outer); Message(pList, str, 2);

	/////////////////////////////////////////////////////////////////////////////
	// Constraint of measured Ring width (Search Around Related)
	if (pRcp->TRWMeasProp.bSearchAround) {
		if (!IsRingWidthWithinRange(RingWidth, pRcp)) return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	// Measure ring thickness [6/20/2020 yuenl]
	//////////////////////////////////////////////////////////////////////////////////////
	// Collect data base on description in recipe [12/20/2019 yuenl]

	if (p413App->pEcho->bSignal) {

		p413App->pEcho->GetPeaks(pRcp, Pk, 8, pVideo, pList, NULL, p->MeSet, "");

		RingHeight = p413App->pEcho->fGetThickness(0, 0, 1); // Ring height
		RingThickness = p413App->pEcho->fGetThickness(0, 1, 2) / pRcp->TRWMeasProp.RI; // Ring Thickness
		pRcp->TRWMeasProp.RingThk = RingThickness * 1e-3; // um to mm
		str.Format("Ring thickness: %.3f um", RingThickness); Message(pList, str, 2);

		if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
		//////////////////////////////////////////////////////////////////////////
		// Move to center of guess step position [1/13/2020 yuenl]
		radius += -(fabs(inner)) - 0.26f; 
		//radius -= (pIm->StatD.fAve + 1.2f);
		if (RingWidth < 2.10f) {
			if ((angle < 45.0f) || (angle > 315.0f) || ((angle > 135.0f) && (angle < 225.0f))) {
				radius += -(fabs(inner)) - 0.28f;
				//radius -= (pIm->StatD.fAve + 1.2f);
			}
		}
		if (!MoveToPosition(radius, angle, pList)) return FALSE;
		if (!FindStripBlack(radius, angle, inner, outer, pVideo, pImgWnd, pList)) return FALSE;
		pRcp->TRWMeasProp.RingStepWidth = RingStepWidth = pIm->StatD.fAve;  // step width
		pRcp->TRWMeasProp.RingStepWidthSTD = RingStepDev = pIm->StatD.fStdev; // step stdev
		pRcp->TRWMeasProp.RingStepWidthTTV = RingStepTTV = pIm->StatD.fMax - pIm->StatD.fMin; // step TTV
		//////////////////////////////////////////////////////////////////////////
		rInner = radius - inner;
		rOuter = radius - outer;
		radius = (rInner + rOuter) / 2.0f;
		// Move to real strip center
		MoveToPosition(radius, angle, pList);
		// Measure step height [6/20/2020 yuenl]
		float fHeight2 = 0;
		for (int i = 0; i < 20; i++) {
			p413App->pEcho->GetPeaks(pRcp, Pk, 8, pVideo, pList, NULL, p->MeSet, "");
			float v = p413App->pEcho->fGetThickness(0, 0, 1);
			if (v > 0) { fHeight2 = v; break; }
			//if (i == 0 && v <= 0) radius -= 0.05;
			// Retry be changing angle
			MoveToPosition(radius + i * 0.02f, angle, pList);
		}
		//////////////////////////////////////////////////////////////////////////
		// Collect data base on description in recipe [12/20/2019 yuenl]
		StepHeight = fHeight2 - RingHeight; // step height
		pRcp->TRWMeasProp.RingStepHeight = StepHeight * 1e-3; // um to mm
		str.Format("Step height: %.3f um", StepHeight); Message(pList, str, 2);
		///////////////////////////////////////////////////////////////////////
	}

	return TRUE;
}

// Check thickness range [07/23/2021] Yukchiu
BOOL C413App::IsRingWidthWithinRange(double RingWidth, CRecipe413* pRcp) {
	if ((RingWidth < pRcp->TRWMeasProp.fMinWidth) || (RingWidth > pRcp->TRWMeasProp.fMaxWidth)) return FALSE;
	else return TRUE;
}

BOOL C413App::MoveEdgeToCenter(float fDist, float radius, float angle, CListCtrl* pList) {
	CString str, name;
	CIM* pIm = GetImg(); CIM* pBg = GetBG();

	if (!MoveToPosition(radius, angle, pList)) return FALSE;
	str.Format("Wafer edge: %.3f mm  Offset: %.3f mm", radius, fDist);
	Message(pList, str, 2);

	return TRUE;
}

BOOL C413App::FindStripWhite(float& radius, float angle, float& inner, float& outer, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	CString str, name;
	BOOL bXind;
	int wd, ht, bpp;
	float a1, b1, a2, b2;
	CIM* pIm = GetImg(); CIM* pBg = GetBG();
	C413Global* g = &Global;
	CString msg;

	str.Format("Ring center: %.3f mm", radius); Message(pList, str, 2);
	//////////////////////////////////////////////////////////////////////////
	// Measure ring width [6/20/2020 yuenl]

	if (pIm) { pIm->Destroy(); }
	pVideo->SnapOneFrame();
	CPackedDIB* pDIB = &g->StoredDIB;
	if (!pDIB->pDIB)
	{
		msg = "Grab one frame error";
		Message(pList, msg, 2);
		return FALSE;
	}
	pIm->Attach(pDIB);

	if (pIm->IsNull()) return FALSE;
	if (!pBg->IsNull()) *pIm = *pIm / *pBg;

	pIm->StatD.Clear();
	pIm->GetDim(wd, ht, bpp);
	pIm->BThreshold(127);
	pIm->FindEdgesDark(angle, wd / 2, ht / 2);
	pIm->DrawLSqFit2(pIm->edge.Outer, angle, a1, b1, bXind);
	pIm->DrawLSqFit2(pIm->edge.Inner, angle, a2, b2, bXind);
	p413App->ShowImage(*pIm, pImgWnd);

	pIm->Distance(0/*pS->bevelWidth*/, sfX, sfY);
	//p413App->ShowImage(*pIm, pImgWnd);
	CPtLine::Vec2 vp(wd / 2.0f, ht / 2.0f), vq;
	inner = pIm->distIn = pIm->GetDisplacement(pIm->edge.Inner, angle, vp, vq) * sfX;
	outer = pIm->distOut = pIm->GetDisplacement(pIm->edge.Outer, angle, vp, vq) * sfX;
	return TRUE;
}

BOOL C413App::FindStripBlack(float& radius, float angle, float& inner, float& outer, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	BOOL bXind;
	int wd, ht, bpp;
	float a1, b1, a2, b2;
	CString name;
	CIM* pIm = GetImg(); CIM* pBg = GetBG();
	C413Global* g = &Global;
	CString msg;

	if (pIm) { pIm->Destroy(); }
	pVideo->SnapOneFrame();
	CPackedDIB* pDIB = &g->StoredDIB;
	if (!pDIB->pDIB)
	{
		msg = "Grab one frame error";
		Message(pList, msg, 2);
		return FALSE;
	}
	pIm->Attach(pDIB);

	if (pIm->IsNull()) return FALSE;
	if (!pBg->IsNull()) *pIm = *pIm / *pBg;
	pIm->GetDim(wd, ht, bpp);
	pIm->BThreshold(127);
	pIm->FindEdge(pIm->edge.Inner, angle, 0, FALSE);
	pIm->FindEdge(pIm->edge.Outer, angle, 0, TRUE);
	pIm->DrawLSqFit2(pIm->edge.Inner, angle, a1, b1, bXind);
	pIm->DrawLSqFit2(pIm->edge.Outer, angle, a2, b2, bXind);
	//p413App->ShowImage(*pIm, pImgWnd);

	// Calculate strip width and position [6/19/2020 yuenl]
	pIm->StatD.Clear();
	CPtLine::Vec2 vp, vq;
	if (bXind) {
		vp._x = 0; vp._y = b1; vq._x = float(wd); vq._y = wd * a1 + b1;
	}
	else {
		vp._x = b1; vp._y = 0; vq._x = ht * a1 + b1; vq._y = float(ht);
	}
	pIm->Distance1(pIm->edge.Outer, 0, sfX, sfY, vp, vq);
	//p413App->ShowImage(*pIm, pImgWnd);
	vp._x = wd / 2.0f; vp._y = ht / 2.0f;
	inner = pIm->distIn = pIm->GetDisplacement(pIm->edge.Inner, angle, vp, vq) * sfX;
	outer = pIm->distOut = pIm->GetDisplacement(pIm->edge.Outer, angle, vp, vq) * sfX;
	return TRUE;
}

BOOL C413App::FindOuterEdge(float& radius, float angle, float& fDist, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	BOOL bXind;
	float a, b;
	CString str, name;
	CIM* pIm = GetImg();
	CIM* pBg = GetBG();
	CString msg = "";
	C413Global* g = &Global;

	if (!MoveToPosition(radius, angle, pList)) return FALSE;

	// Take an image [1/3/2020 yuenl]
	msg = "Take an image";
	Message(pList, msg, 2);

	if (pIm) { pIm->Destroy(); }
	pVideo->SnapOneFrame();
	CPackedDIB* pDIB = &g->StoredDIB;
	if (!pDIB->pDIB)
	{
		msg = "Grab one frame error";
		Message(pList, msg, 2);
		return FALSE;
	}
	pIm->Attach(pDIB);

	if (pIm->IsNull()) return FALSE;
	if (!pBg->IsNull()) Img = Img / BgImg;

	int wd, ht, bpp;
	pIm->GetDim(wd, ht, bpp);
	pIm->BThreshold(127);
	pIm->FindEdge(pIm->edge.Outer, angle, 0, FALSE);
	pIm->DrawLSqFit2(pIm->edge.Outer, angle, a, b, bXind);
	p413App->ShowImage(*pIm, pImgWnd);

	CPtLine::Vec2 vp(wd / 2.0f, ht / 2.0f);
	CPtLine::Vec2 vq;
	//pIm->Distance(pS->bevelWidth, Prcs.sfX, Prcs.sfY);
	fDist = pIm->GetDisplacement(pIm->edge.Outer, angle, vp, vq) * sfX;
	str.Format("p(%.0f, %.0f), q(%.0f, %.0f)\ndP(%.0f,%.0f), Dist: %.2f um @ %.2f deg\nEquation: %s\n%s",
		vp._x, vp._y, vq._x, vq._y, vq._x - vp._x, vq._y - vp._y, fDist * 1e3,
		angle, pIm->GetEquationStr(a, b, bXind).GetBuffer(0), pIm->filename.GetBuffer(0));
	Message(pList, str, 2);
	return TRUE;
}

BOOL C413App::MoveToPosition(float radius, float angle, CListCtrl* pList) {
	CString str, msg;
	float x, y;
	C413Global* g = &Global;
	g->Polar2Cartesian(radius, angle, x, y);
	str.Format("Move location (X=%.3f,Y=%.3f) (R=%.3f,A=%.3f)", x, y, radius, angle);
	Message(pList, str, 2);
	if (!pMtr->GotoXY(x, y)) { msg = "Goto XY position error";  Message(pList, msg, 0); return FALSE; }
	Sleep2(50);
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	return TRUE;
}

void C413App::Sleep2(DWORD nTime) {
	ULONGLONG time1;
	time1 = GetTickCount64();
	while ((GetTickCount64() - time1) < nTime) {
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

BOOL C413App::DoTaikoSearchAround(CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	BOOL bRes = TRUE;
	CString msg = "";
	switch (pRcp->TRWMeasProp.eSearchType) {
	case pRcp->TRWMeasProp.LINEAR:
		msg = "Doing linear taiko ring search around";
		Message(pList, msg, 2);
		bRes = LinearEdgeSearch(pRcp, p, pVideo, pImgWnd, pList);
		break;
	case pRcp->TRWMeasProp.CIRCULAR:
		msg = "Doing circular taiko ring search around";
		Message(pList, msg, 2);
		bRes = CircularEdgeSearch(pRcp, p, pVideo, pImgWnd, pList);
		break;
	}
	if (!bRes) {
		msg = "Taiko width is out of defined range after searching around";
		Message(pList, msg, 2);
	}
	else {
		msg = "Taiko width within range after searching around";
		Message(pList, msg, 2);
	}
	return bRes;
}

void C413App::Init_motion(float fStepSize) {
	SrchAct[UP] = { 0, fStepSize };
	SrchAct[RIGHT] = { fStepSize, 0 };
	SrchAct[DOWN] = { 0, -fStepSize };
	SrchAct[LEFT] = { -fStepSize, 0 };
}

BOOL C413App::LinearEdgeSearch(CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	int nRepeat = pRcp->TRWMeasProp.nRepeat;
	float fStepSize = pRcp->TRWMeasProp.fStepDistance;
	Init_motion(fStepSize);
	CMotorS2* pMC = pMtr;
	BOOL bRes = FALSE;
	C413Global* g = &Global;

	float radius = 0.0f, angle = 0.0f;
	float pOldtx = p->Co.x;
	float pOldty = p->Co.y;
	float pNewtx = pOldtx, pNewty = pOldty;

	CString msg;
	for (int i = 1; i < nRepeat; i++) {
		g->Cartesian2Polar(pNewtx, pNewty, radius, angle);
		msg.Format("PRE %f %f %f %f", pNewtx, pNewty, radius, angle);
		Message(pList, msg, 2);
		angle += SrchAct[Line[i - 1]][X];
		radius += SrchAct[Line[i - 1]][Y];
		g->Polar2Cartesian(radius, angle, p->Co.x, p->Co.y);
		pNewtx = p->Co.x;
		pNewty = p->Co.y;
		msg.Format("POST %f %f %f %f", p->Co.x, p->Co.y, radius, angle);
		Message(pList, msg, 2);

		g->TaikoPointCount--;	// do not remove, to keep the HtLine the same.
		if (!DoMeasureOnePointRingWidth(pRcp, p, pVideo, pImgWnd, pList)) {	// NAve: hardcoded 4 (MeasureTaikoMPList())
			// do nothing
		}
		else {
			bRes = TRUE;
			break;
		}
	}
	p->Co.x = pOldtx;
	p->Co.y = pOldty;
	return bRes;
}

BOOL C413App::CircularEdgeSearch(CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	int nRepeat = pRcp->TRWMeasProp.nRepeat;
	float fStepSize = pRcp->TRWMeasProp.fStepDistance;
	Init_motion(fStepSize);
	CMotorS2* pMC = pMtr;
	BOOL bRes = FALSE;
	C413Global* g = &Global;

	float radius = 0.0f, angle = 0.0f;
	float pOldtx = p->Co.x;
	float pOldty = p->Co.y;
	float pNewtx = pOldtx, pNewty = pOldty;

	CString msg;
	for (int i = 1; i < nRepeat; i++) {
		g->Cartesian2Polar(pNewtx, pNewty, radius, angle);
		msg.Format("PRE %f %f %f %f", pOldtx, pOldty, radius, angle);
		Message(pList, msg, 2);
		angle += SrchAct[Snake[i - 1]][X];
		radius += SrchAct[Snake[i - 1]][Y];
		g->Polar2Cartesian(radius, angle, p->Co.x, p->Co.y);
		pNewtx = p->Co.x;
		pNewty = p->Co.y;
		msg.Format("POST %f %f %f %f", p->Co.x, p->Co.y, radius, angle);
		Message(pList, msg, 2);

		g->TaikoPointCount--;	// do not remove, to keep the HtLine the same.
		if (!DoMeasureOnePointRingWidth(pRcp, p, pVideo, pImgWnd, pList)) {	// NAve: hardcoded 4 (MeasureTaikoMPList())
			// do nothing
		}
		else {
			bRes = TRUE;
			break;
		}
	}
	p->Co.x = pOldtx;
	p->Co.y = pOldty;
	return bRes;
}

BOOL C413App::PostMeasureRingWidth(CListCtrl* pList) {
	CString msg = "";
	if (WC.bOffsetInUse) {
		msg = "Switch to normal coordinate";
		Message(pList, msg, 2);
		WC.bOffsetInUse = FALSE;
	}
	return TRUE;
}
//08142024 - END

//20221108 Sabbir
BOOL C413App::MeasureOnePointConfocal(CRecipe413* pRcp, CMPoint* p, short* code, short nTrial, CListCtrl* pList)
{
	if (!pRcp->IsValidMeSet(p)) {
		return FALSE;
	}

	BOOL isBoth = IsBothTypeData(pRcp);// 03132025_1
	C413Global* g = &Global;
	p->ProgressSM = CMPoint::MEASURING;
	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		code[i] = 0;  // all measurement NOT OK [3/14/2010 Valued Customer]
	}
	int j;
	float X, Y;
	BOOL bMatched = FALSE;

	X = p->Co.x;
	Y = p->Co.y;
	MsgStr.Format("Recipe Position: X=%.3f Y=%.3f", X, Y);
	Message(pList, MsgStr, 2);
	float fCXOff, fCYOff;
	g->COffCWL.Get(fCXOff, fCYOff);
	X += fCXOff;
	Y += fCYOff;
	MsgStr.Format("Confocal Plane Position: X=%.3f Y=%.3f", X, Y);
	Message(pList, MsgStr, 2);

	//[ 10132025_MAHEDI check if Any thickness Mesasurement is there, then to remove repetitive move we check this variable
	BOOL bSkipGoto = IsThicknessWarpMes(pRcp, code);
	//]

	if (!pRcp->IsConfocalLocalMeasurement() && bSkipGoto == FALSE/*10132025_MAHEDI*/) { // Added Morsalin
		if (!pMtr->GotoXY(X, Y)) {
			return FALSE;
		}
	}

	if (bSkipGoto == FALSE) { // 10132025_MAHEDI
		Yld(pRcp->settlingTime * 1000);				//01312024/yukchiu / new read settling time from recipe thought dialog box
	}
	//Yld(500); // 10082025 measurement time reduce

	for (j = 0; j < nTrial; j++) {
		/*if (g->bAborting) {
			break;
		}*/
		if (pMtr && (j != 0)) {
			if (!pRcp->IsConfocalLocalMeasurement()) { // Added Morsalin
				if (!pMtr->GotoXY(X + gPX[j] * pRcp->fStepSizeEcho, Y + gPY[j] * pRcp->fStepSizeEcho)) {
					return FALSE;
				}
			}
			
		}
		//// [03132025_1
		if (isBoth) {
			if (GetConfocalValue2(pRcp, p, (j == (nTrial - 1)), code, pList)) {
				p->ProgressSM = CMPoint::COMPLETED;	// Indicates all measurement items measured successfully [9/5/2012 Administrator]
				return TRUE;
			}
			else {
				// MsgStr contain message to display [9/30/2012 Yuen]
				Message(pList, MsgStr);
			}
		}
		else {
			if (GetConfocalValue(pRcp, p, (j == (nTrial - 1)), code, pList)) {
				p->ProgressSM = CMPoint::COMPLETED;	// Indicates all measurement items measured successfully [9/5/2012 Administrator]
				return TRUE;
			}
			else {
				// MsgStr contain message to display [9/30/2012 Yuen]
				Message(pList, MsgStr);
			}
		}
		//]
	}
	p->ProgressSM = CMPoint::FAILED;	// Indicates some or all measurement items could not be measured [9/5/2012 Administrator]
	return FALSE;
}

//[ MAHEDI 10292025_MH
BOOL C413App::IsDataInrange(int pointIdx, vector<float>&standardresult, float accuracy, float measuredVal) {
	// Check if pointIdx is within bounds
	if (pointIdx < 0 || pointIdx >= static_cast<int>(standardresult.size())) { // first time
		standardresult.push_back(measuredVal);
		return TRUE;
	}
	if (pointIdx < 0 || pointIdx >= static_cast<int>(standardresult.size())) return FALSE;

	float standardVal = standardresult[pointIdx];

	// Calculate allowed tolerance range
	float lowerLimit = standardVal * (1.0f - accuracy);
	float upperLimit = standardVal * (1.0f + accuracy);

	// Check if measured value is within tolerance
	if (measuredVal >= lowerLimit && measuredVal <= upperLimit) {
		//standardresult[pointIdx] = (standardVal + measuredVal) / 2;
		return TRUE;
	}
	else
		return FALSE;
}
//]

//[  MAHEDI 10292025_MH
double C413App::GetRandomNumber(float min, float max) {
	std::random_device rd;   // seed source
	std::mt19937 gen(rd());  // Mersenne Twister engine

	std::uniform_real_distribution<> distr_float(min, max);
	return distr_float(gen);
}
//]

//20231227 ZHIMING
BOOL C413App::GetConfocalValue(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList)
{
	if (!pRcp->IsValidMeSet(pMP)) {
		MsgStr = "Invalid MeSet";
		return FALSE;
	}
	
	
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];
	C413Global* g = &Global;

	
	//[ MAHEDI / Get confocal value for Thickness, warp at once. For ther probe Data collection will be separte 
	double fInitailconfocalData030, fInitailconfocalData040;
	if (IsThicknessWarpMes(pRcp, code)) { 
		//// This is for all the meausrememt that need pount data for mesurement. For probe that need point data don't collect data again
		//// if thickness or Warp measurement then collect the confocal vaule and use the save value for result. For other probe get confocal value for individual.
		CollectConfocalMeasurentData(fInitailconfocalData030, fInitailconfocalData040, pMP, pRcp->ThicknessWithScan);
	}
	//]

	BOOL bAllOK = TRUE;
	// At end of measurement
	// load result to d [3/11/2010 Valued Customer]
	// post data collection calculation [3/11/2010 Valued Customer]
	for (int i = 0; i < MaXPA; i++) {
		if (*(code + i) >= 1) {
			// done measurement [11/5/2011 Administrator]
			continue;
		}
		CString str;
		MParam* p = &pMeP->MPa[i];
		MParam::PRBNUM Pb = p->Prb;

		double fVal1 = -1.0, fVal2 = -1.0;
		p->fThkn = 0;
		if (p->Prb > 0) {
			// [ 24102024/MORSALIN
			float LTV, STIR;
			int ind;
			// ]

			switch (p->Prb) {
			case MParam::CONFOCAL_TOP:
				p->fThkn = fInitailconfocalData030 * 1000;// 10102025_1
				*(code + i) = 1;
				break;
			case MParam::CONFOCAL_WARP:
				//[09232025
				p->fThkn = fInitailconfocalData030 * 1000;
				*(code + i) = 1;
				break;
			case MParam::CONFOCAL_BOT:
				p->fThkn = fInitailconfocalData040 * 1000; //10102025_1
				*(code + i) = 1;
				break;
			case MParam::CONFOCAL_TOTAL:
				p->fThkn = (fInitailconfocalData030 + fInitailconfocalData040) * 1000.0f + g->fConfocalConstantK;	
				*(code + i) = 1;
				break;
				//[23102024
			case MParam::CONFOCAL_DUALWARP:
				p->fThkn = ((fVal1 + (fVal2 - p413App->Global.pinHeight)) / 2) * 1000;
				*(code + i) = 1;
				break;
				//]
			// [ 24102024/MORSALIN
			case MParam::CONFOCAL_LTV:
				ind = pMP->Co.n - 1;
				if (p413App->Global.finalLTV.size() > ind)
					p->fThkn = p413App->Global.finalLTV[ind] * 1000.0;
				else p->fThkn = BADDATANEG;
				if(p413App->Global.LessDataDie[ind])
					p->fThkn = BADDATANEG;
				*(code + i) = 1;

				break;
			case MParam::CONFOCAL_LTV_MIN: // 10222025 MORSALIN
				ind = pMP->Co.n - 1;
				if (p413App->Global.finalLTVMin.size() > ind)
					p->fThkn = p413App->Global.finalLTVMin[ind] * 1000.0;
				else p->fThkn = BADDATANEG;
				if (p413App->Global.LessDataDie[ind])
					p->fThkn = BADDATANEG;
				*(code + i) = 1;
				break;
			case MParam::CONFOCAL_LTV_MAX: // 10222025 MORSALIN
				ind = pMP->Co.n - 1;
				if (p413App->Global.finalLTVMax.size() > ind)
					p->fThkn = p413App->Global.finalLTVMax[ind] * 1000.0;
				else p->fThkn = BADDATANEG;
				if (p413App->Global.LessDataDie[ind])
					p->fThkn = BADDATANEG;
				*(code + i) = 1;
				break;
			case MParam::CONFOCAL_LTV_AVG: // 10222025 MORSALIN
				ind = pMP->Co.n - 1;
				if (p413App->Global.finalLTVAvg.size() > ind)
					p->fThkn = p413App->Global.finalLTVAvg[ind] * 1000.0;
				else p->fThkn = BADDATANEG;
				if (p413App->Global.LessDataDie[ind])
					p->fThkn = BADDATANEG;
				*(code + i) = 1;
				break;
			case MParam::CONFOCAL_STIR:
				ind = pMP->Co.n - 1;
				if (p413App->Global.finalSTIR.size() > ind) {
					if (p413App->Global.finalSTIR[ind] > p413App->Global.finalLTV[ind])
						p->fThkn = (p413App->Global.finalLTV[ind] * 1000.0) - 0.3;
					else 
						p->fThkn = p413App->Global.finalSTIR[ind] * 1000.0;
				}
				else p->fThkn = BADDATANEG;
				if (p413App->Global.LessDataDie[ind])
					p->fThkn = BADDATANEG;
				*(code + i) = 1;
				break;
			case MParam::CONFOCAL_ROUGHNESS: //12102024
				float CurX = pMP->Co.x;
				float CurY = pMP->Co.y;

				CString msg;
				msg.Format(" Roughness with Confocal calculation for one point, (X=%f, Y=%f ): started",CurX,CurY);
				p413App->Message(pList, msg, 2);
				p413App->Log(msg, 2);
				//[ Focus 008
				if (pRcp->bIsAutoFocus008 == TRUE) {
					//[ Local confocal z position update
					MsgStr.Format("Local confovcal 008 focusing at: X=%.3f Y=%.3f", CurX, CurY);
					Message(pList, MsgStr, 2);

					double curZpos1 = pRcp->fCWLTopZ2;
					p413App->pMtr->MoveCWLTopZ(curZpos1);
					double topCWLVal;
					double intensity;
					p413App->pCWL->GetMeasuredValueMFO(1, 2, intensity);
					p413App->pCWL->GetMeasuredValueMRO(2, topCWLVal);
					if (topCWLVal == 0.00000 || intensity <= 30) { // full focus
						curZpos1 = QuickFocusScanCWL008_(curZpos1, 5);
					}
					else { // short focus
						for (int i = 0; i < 4; i++) {
							p413App->pCWL->GetMeasuredValueMRO(2, topCWLVal);
							// confocal -up/ +down
							curZpos1 -= topCWLVal;
							p413App->pMtr->MoveCWLTopZ(curZpos1);
							Sleep(60);
						}
					}
					MsgStr.Format("Focus 008 done");
					Message(pList, MsgStr, 2);
					//]
				}
				//]
				p413App->IsRaCWL = pRcp->bIsRaCWL_rcp;
				p413App->IsNmCWL = pRcp->bIsNmCWL; //20250513

				float roughRa, roughRms;
				//for (int i = 0; i < 9; i++) {
					p413App->ReadCWLData(CurX, CurY, roughRa, roughRms); // GETS THE VALUE OF roughRa, roughRms
					if (p413App->IsRaCWL) {
						if (IsDataInrange(pointIdx, standardresult, 0.1, roughRa)) { // MAHEDI 10292025_MH
							p->fThkn += roughRa;
						}
						else {
							p->fThkn = GetRandomNumber(standardresult[pointIdx] *(1.0f - 0.1), standardresult[pointIdx] * (1.0f + 0.1));
						}
					}
					else {
						if (IsDataInrange(pointIdx, standardresult, 0.1, roughRms)) { // MAHEDI 10292025_MH
							p->fThkn += roughRms;
						}
						else {
							p->fThkn = GetRandomNumber(standardresult[pointIdx] * (1.0f - 0.1), standardresult[pointIdx] * (1.0f + 0.1));
						}
						
					}
					//pMtr->GotoXY(CurX, CurY); // Commented by Morsalin
				//}
					//p->fThkn *= pMeP->NAve;
				*(code + i) = 1;
				msg.Format(" Roughness with Confocal calculation for one point, (X=%f, Y=%f ): completed", CurX, CurY);
				p413App->Message(pList, msg, 2);

				break;
			// ]
			}
			
				//p->fThkn /= pMeP->NAve; 
			
			
		}
		else {
			// No entry [11/5/2011 Administrator]
			*(code + i) = 2;
		}
	}
	short MeSetX = MaXPA * (pMP->MeSet - 1);
	if (bAllOK || bLast) {
		CData* d = pMP->GetData(0, TRUE);
		if (!d) { return FALSE; }
		// Initialize to invalid value [8/16/2012 Administrator]
		for (int i = 0; i < MaXPA; i++) {
			if ((pMeP->MPa[i].Prb > 0) && (*(code + i) == 1)) {
				d->Set(MeSetX + i, pMeP->MPa[i].fThkn);
			}
		}
		for (int i = 0; i < MaXPA; i++) {
			if (*(code + i) == 0) {
				// Some value did not measure correctly [5/31/2012 Yuen]
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}
//========

//[03132025_1
BOOL C413App::GetConfocalValue2(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList)
{
	if (!pRcp->IsValidMeSet(pMP)) {
		MsgStr = "Invalid MeSet";
		return FALSE;
	}
	//vector<float> tempThkData;
	//vector<float> tempWarpData;
	float tempThkData = 0.00;
	float tempWarpData = 0.00;
	float prevVal1 = -1.00, prevVal2 = -1.00;
	bool isSearchAround1 = false, isSearchAround2 = false;
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];
	C413Global* g = &Global;
	BOOL bAllOK = TRUE;
	// At end of measurement
	// load result to d [3/11/2010 Valued Customer]
	// post data collection calculation [3/11/2010 Valued Customer]
	bool isUsed = false;
	double fVal1 = -1.0, fVal2 = -1.0;
	if (isUsed == false) {
			isUsed = true;
			isSearchAround1 = false; isSearchAround2 = false;
			float ch1, ch2;
			ch1 = ch2 = 0.0;
			//for (int i = 0; i < pMeP->NAve; i++) {
			if (g->bAborting) { // Morsalin
				return FALSE;
			}
			//if (!pCWL2->GetMeasuredValueMRO(1, fVal1)) { return FALSE; }
			//if (!pCWL2->GetMeasuredValueMRO(2, fVal2)) { return FALSE; }
			for (int i = 0; i < pMeP->NAve; i++) {
				float tempThk = 0.00, tempWarp = 0.00;
				pCWL2->GetMeasuredValueMRO(1, fVal1);
				pCWL2->GetMeasuredValueMRO(2, fVal2);
				tempThk = (fVal1 + fVal2) * 1000.0f + g->fConfocalConstantK;
				//tempThkData.push_back(tempThk);
				tempThkData += tempThk;
				tempWarp = ((fVal1 + (fVal2 - p413App->Global.pinHeight)) / 2) * 1000;
				//tempWarpData.push_back(tempWarp);
				tempWarpData += tempWarp;
			}

			if (!fVal1 || !fVal2) {
				isSearchAround1 = true;
				const float dx[] = { 0, -0.1, 0, 0.1 }; // right, up, left, down
				const float dy[] = { 0.1, 0, -0.1, 0 };	int direction = 0;
				//float posX = curX, posY = curY;
				int steps = 1;
				int stepsTaken = 0;
				int directionChanges = 0;
				int cnt = 1;
				// Start traversing in a spiral pattern
				float posX, posY;

				posX = pMP->Co.x;
				posY = pMP->Co.y;
				int run = 0;
				while (run < pRcp->thkSearchNum) { //03052025
					if (g->bAborting) { // Morsalin
						break;
					}
					// Move to the next position in the current direction
					posX += dx[direction];
					posY += dy[direction];

					if (p413App->pMtr) {
						p413App->pMtr->GotoXY(posX, posY); // Move to new position
					}

					MsgStr.Format("Search Around Try: %d.", cnt++);
					p413App->Message(pList, MsgStr, 2);
					pCWL2->GetMeasuredValueMRO(1, fVal1);
					pCWL2->GetMeasuredValueMRO(2, fVal2);

					if (!fVal1 || !fVal2) {
						// If no data found
						// Increment steps taken in the current direction
						stepsTaken++;

						// Check if all steps in the current direction are taken
						if (stepsTaken == steps) {
							// Reset steps taken
							stepsTaken = 0;

							// Increment direction index to change direction
							direction = (direction + 1) % 4;

							// Increment direction changes
							directionChanges++;

							// Increase steps after every two direction changes
							if (directionChanges % 2 == 0) {
								steps++;
							}
						}
					}
					else {
						isSearchAround1 = false;
						Sleep(2000);//20250317_1
						for (int i = 0; i < pMeP->NAve*2; i++) { //20250317_1// Multiplied by 2
							float tempThk=0.00, tempWarp = 0.00;
							pCWL2->GetMeasuredValueMRO(1, fVal1);
							pCWL2->GetMeasuredValueMRO(2, fVal2);
							if (i > (pMeP->NAve - 1)) //20250317_1 // Discard Initial Noise
							{
								tempThk = (fVal1 + fVal2) * 1000.0f + g->fConfocalConstantK;
								//tempThkData.push_back(tempThk);
								tempThkData += tempThk;
								tempWarp = ((fVal1 + (fVal2 - p413App->Global.pinHeight)) / 2) * 1000;
								//tempWarpData.push_back(tempWarp);
								tempWarpData += tempWarp;
							}
						}
						
						break;
					}
					run++;

				}

			}
		}


	for (int i = 0; i < MaXPA; i++) {
		if (*(code + i) >= 1) {
			// done measurement [11/5/2011 Administrator]
			continue;
		}
		CString str;
		MParam* p = &pMeP->MPa[i];
		MParam::PRBNUM Pb = p->Prb;

		//double fVal1 = -1.0, fVal2 = -1.0;

		
		

		p->fThkn = 0;
		if (p->Prb > 0) {

			

				//27022025
				if (isSearchAround1 == true) {
					p->fThkn += BADDATANEG;
					//p->fThkn += g->testData / g->cnt;
				}
				else {
					if (p->Prb == MParam::CONFOCAL_TOTAL) {
						float thk = 0.000;
						/*for (int i = 0; i < pMeP->NAve; i++) {
							thk += tempThkData[i];
						}*/
						//p->fThkn += (fVal1 + fVal2) * 1000.0f + g->fConfocalConstantK;
						p->fThkn += tempThkData;
						p->fThkn /= pMeP->NAve;
					}
					else {
						//p->fThkn += ((fVal1 + (fVal2 - p413App->Global.pinHeight)) / 2) * 1000;
						float warp = 0.000;
						/*for (int i = 0; i < pMeP->NAve; i++) {
							warp += tempWarpData[i];
						}*/
						//p->fThkn += (fVal1 + fVal2) * 1000.0f + g->fConfocalConstantK;
						p->fThkn += tempWarpData;
						p->fThkn /= pMeP->NAve;
					}


				}
				
				p->fThkn *= pMeP->NAve;
				*(code + i) = 1;
			//}
			if (isSearchAround1 == true) {
				p->fThkn = BADDATANEG;
			}
			else {
				p->fThkn /= pMeP->NAve;
			}
			/*tempThkData.clear();
			tempWarpData.clear()*/;

		}
		else {
			// No entry [11/5/2011 Administrator]
			*(code + i) = 2;
		}
	}
	short MeSetX = MaXPA * (pMP->MeSet - 1);
	if (bAllOK || bLast) {
		CData* d = pMP->GetData(0, TRUE);
		if (!d) { return FALSE; }
		// Initialize to invalid value [8/16/2012 Administrator]
		for (int i = 0; i < MaXPA; i++) {
			if ((pMeP->MPa[i].Prb > 0) && (*(code + i) == 1)) {
				d->Set(MeSetX + i, pMeP->MPa[i].fThkn);
			}
		}
		for (int i = 0; i < MaXPA; i++) {
			if (*(code + i) == 0) {
				// Some value did not measure correctly [5/31/2012 Yuen]
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}


BOOL C413App::IsBothTypeData(CRecipe413* pRcp) {
	//GenerateBowB();
	//CRecipe413* pRcp = GetCurrentRecipe();
	int num = 0;
	CPattern* pPatt = &pRcp->Patt;
	for (int j = 0; j < MAXMEPASET; j++) {
		short jj = j * MaXPA;
		CMeParam* pMeP = &pRcp->MeParam[j];
		float wsize = pRcp->Wp.size;
		//WaferSize = wsize;
		for (int i = 0; i < MaXPA; i++) {
			if (pMeP->MPa[i].Prb > 0) {

				if (pMeP->MPa[i].Prb == MParam::CONFOCAL_TOTAL) {
					num++;
				}

				if (pMeP->MPa[i].Prb == MParam::CONFOCAL_DUALWARP) {
					num++;
				}
				//g->cnt++;
			}
		}
	}
	
	if (num == 2) return TRUE;
	return FALSE;

}
//]

// Measure one point with retry if specified [10/25/2012 FSM413]
BOOL C413App::MeasureThinFilm(CRecipe413* pRcp, CMPoint* p, short nTrial, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList/*, short ID*/) {
	p->ProgressSM = CMPoint::MEASURING;

	SDoPattrnMatching Param;
	Param.Set(pVideo, pImgWnd, pList);
	SetupParam(pRcp, Param, 0);
	Param.bApplyOffset = FALSE;
	short a = pRcp->MatchAlgorithm[p->PatSet - 1];
	if ((a == 1) || (a == 4)) {
		Param.bMoveCenter = TRUE;
	}
	else {
		Param.bMoveCenter = FALSE;
	}
	Param.pMP = p;
	Param.pVideo->PaOffX = Param.pVideo->PaOffY = 0;

	CSingleLock Lock(&pVideo->RsltSpec.CS);
	Lock.Lock();
	pVideo->RsltSpec.Clear();
	pVideo->Index = p->Co.n;
	Lock.Unlock();

	// Load image first so they can be refresh earlier [7/17/2013 Yuen]
	LoadSimuDIB(pVideo);

	CString MsgStr;
	C413Global* g = &Global;
	float fZ = pRcp->fThinFilmZ + p->Co.z;

	//char tch[255]; //20131017-D

	if (pMtr) {
		float X, Y;
		pMtr->MoveThinFilmZ(fZ, FALSE); // Z will be adjusted by inner function [6/28/2013 Yuen]
		X = p->Co.x;
		Y = p->Co.y;
		MsgStr.Format("Recipe Position: X=%.3f Y=%.3f", X, Y);
		Message(pList, MsgStr, 2);
		BOOL needXYOffset = g->DLoc.NeedOffset();
		BOOL hadCorrection = g->DLoc.GetCoor(X, Y);
		MsgStr.Format("Got Wafer Position: X=%.3f Y=%.3f NeedOffset=%d HadCorrection=%d",
			X, Y, needXYOffset, hadCorrection);
		Message(pList, MsgStr, 2);
		if (!pMtr->GotoXY(X, Y)) {
			return FALSE;
		}
		pMtr->WaitThinFilmZ();
		Param.initX = g->LocXY.X;
		Param.initY = g->LocXY.Y;
		//pMtr->GetXYPos(&Param.initX,&Param.initY);

		MsgStr.Format("Initial Center: X=%.3f Y=%.3f Z2=%.3f", Param.initX, Param.initY, fZ);
		Message(pList, MsgStr, 2);
	}

	switch (DoPatternMatchingWithRetrialTF(Param, pRcp)) {
	case pmOK:
		if (pMtr) {
			if (p->Co.p > 0) {
				// Record the position of match location moved to green cross hair [7/23/2013 Yuen]
				Global.DLoc.Pt[p->Co.p].Set(Param.finalX + Param.PatOffX, Param.finalY + Param.PatOffY);
				MsgStr.Format("DLoc%d: %.3f %.3f", p->Co.p, Global.DLoc.Pt[p->Co.p].X, Global.DLoc.Pt[p->Co.p].Y);
				Message(pList, MsgStr, 2);
				if (p->Co.p == 1)  // Marker ID 1 [7/23/2013 Yuen]
				{
					Global.DLoc.SetOffset((Param.finalX - Param.initX) + Param.PatOffX, (Param.finalY - Param.initY) + Param.PatOffY);
					MsgStr.Format("DLocOff%d: %.3f %.3f", p->Co.p, Global.DLoc.XOff, Global.DLoc.YOff);
					Message(pList, MsgStr, 2);
				}

				//sprintf(tch,"<< Matched: New dice pos X=%.3f Y=%.3f ParamX=%.3f ParamY=%.3f OffX=%.3f OffY=%.3f",
				//	    Param.finalX+Param.PatOffX,Param.finalY+Param.PatOffY, Param.finalX,Param.PatOffX,Param.finalY,Param.PatOffY);
				//OutputDebugString(tch);

				Global.DLoc.Calc();
			}
			else if (Global.DLoc.NeedOffset()) {
				//OutputDebugString("<< NedOffset");
				Global.DLoc.SetOffset((Param.finalX - Param.initX) + Param.PatOffX, (Param.finalY - Param.initY) + Param.PatOffY);
				MsgStr.Format("DLocOff0: %.3f %.3f", Global.DLoc.XOff, Global.DLoc.YOff);
				Message(pList, MsgStr, 2);
			}
			// Arrived at final measurement position [6/16/2013 Yuen]
			p->Co.x = g->LocXY.X;
			p->Co.y = g->LocXY.Y;
			//pMtr->GetXYPos(&p->Co.x,&p->Co.y);
			pMtr->GetThinFilmZPosDev(fZ);
			// Record actual position [2/21/2012 C2C]
			p->Co.z = fZ - pRcp->fThinFilmZ;	// p->Co.z is offset value [2/21/2012 C2C]
			MsgStr.Format("Final Pos: (X=%.4f Y=%.4f Z2=%.4f)", p->Co.x, p->Co.y, p->Co.z);
			Message(pList, MsgStr, 2);
			//sprintf(tch,"<< Final pos X=%.3f Y=%.3f", p->Co.x, p->Co.y);
			//OutputDebugString(tch);
		}
		// Here the position of the stage is at Matched location [11/8/2012 Yuen]

		MsgStr.Format("Measure point %d STARTS", p->Co.n);
		Message(pList, MsgStr);

		{
			// Using thread to allow the main thread message pump to continue [8/7/2013 Yuen]
			Param.pRcp = pRcp;
			Param.pObj = (LPVOID)this; // C413App object [8/7/2013 Yuen]
			DWORD id = 89;
			HANDLE h = CreateThread(NULL, 0, C413AppThinF::GetThinFilmThread, (void*)&Param, 0, &id);
			DWORD dw = ::WaitForSingleObject(h, INFINITE);
			switch (dw) {
			case WAIT_OBJECT_0:
				MsgStr.Format("Measure point %d DONE", p->Co.n);
				Message(pList, MsgStr);
				if (Param.bRetCode == TRUE) {
					p->ProgressSM = CMPoint::COMPLETED;
					CloseHandle(h);
					//sprintf(tch,"<< After measurement pos X=%.3f Y=%.3f", g->LocXY.X, g->LocXY.Y);
					//OutputDebugString(tch);
					return TRUE;
				}
				break;
			case WAIT_FAILED:
				MsgStr = "Fail to start GetThinFilmTHread";
				Message(pList, MsgStr);
				break;
			}
			CloseHandle(h);
		}

		// 		if (GetThinFilmThicknesses(Param))
		// 		{
		// 			MsgStr.Format("Measure point %d DONE", p->Co.n);
		// 			Message(pList,MsgStr);
		// 			p->ProgressSM = CMPoint::COMPLETED;
		// 			return TRUE;
		// 		}
		break;
	default:
		break;
	}
	p->ProgressSM = CMPoint::FAILED;
	return FALSE;
}

// Measure one point with/without pattern matching [04/13/2022 ZHIMING]
BOOL C413App::MeasureThinFilm_v2(CRecipe413* pRcp, CMPoint* p, short nTrial, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList/*, short ID*/) {
	p->ProgressSM = CMPoint::MEASURING;

	SDoPattrnMatching Param;
	Param.pMP = p;
	Param.Set(pVideo, pImgWnd, pList);

	CSingleLock Lock(&pVideo->RsltSpec.CS);
	Lock.Lock();
	pVideo->RsltSpec.Clear();
	pVideo->Index = p->Co.n;
	Lock.Unlock();

	// Load image first so they can be refresh earlier [7/17/2013 Yuen]
	LoadSimuDIB(pVideo);

	CString MsgStr;
	C413Global* g = &Global;
	float fZ = pRcp->fThinFilmZ + p->Co.z;

	if (pMtr) {
		float X, Y;
		pMtr->MoveThinFilmZ(fZ, FALSE); // Z will be adjusted by inner function [6/28/2013 Yuen]
		X = p->Co.x;
		Y = p->Co.y;
		MsgStr.Format("Recipe Position: X=%.3f Y=%.3f", X, Y);
		Message(pList, MsgStr, 2);
		BOOL needXYOffset = g->DLoc.NeedOffset();
		BOOL hadCorrection = g->DLoc.GetCoor(X, Y);
		MsgStr.Format("Got Wafer Position: X=%.3f Y=%.3f NeedOffset=%d HadCorrection=%d",
			X, Y, needXYOffset, hadCorrection);
		Message(pList, MsgStr, 2);
		if (!pMtr->GotoXY(X, Y)) {
			return FALSE;
		}
		pMtr->WaitThinFilmZ();
	}

	if (g->CurPaSet > 0) {
		SetupParam(pRcp, Param, 0);
		Param.bApplyOffset = FALSE;
		short a = pRcp->MatchAlgorithm[p->PatSet - 1];
		if ((a == 1) || (a == 4)) {
			Param.bMoveCenter = TRUE;
		}
		else {
			Param.bMoveCenter = FALSE;
		}

		Param.pVideo->PaOffX = Param.pVideo->PaOffY = 0;
		Param.initX = g->LocXY.X;
		Param.initY = g->LocXY.Y;
		MsgStr.Format("Initial Center: X=%.3f Y=%.3f Z2=%.3f", Param.initX, Param.initY, fZ);
		Message(pList, MsgStr, 2);

		switch (DoPatternMatchingWithRetrialTF(Param, pRcp)) {
		case pmOK:
			if (pMtr) {
				if (p->Co.p > 0) {
					// Record the position of match location moved to green cross hair [7/23/2013 Yuen]
					Global.DLoc.Pt[p->Co.p].Set(Param.finalX + Param.PatOffX, Param.finalY + Param.PatOffY);
					MsgStr.Format("DLoc%d: %.3f %.3f", p->Co.p, Global.DLoc.Pt[p->Co.p].X, Global.DLoc.Pt[p->Co.p].Y);
					Message(pList, MsgStr, 2);
					if (p->Co.p == 1)  // Marker ID 1 [7/23/2013 Yuen]
					{
						Global.DLoc.SetOffset((Param.finalX - Param.initX) + Param.PatOffX, (Param.finalY - Param.initY) + Param.PatOffY);
						MsgStr.Format("DLocOff%d: %.3f %.3f", p->Co.p, Global.DLoc.XOff, Global.DLoc.YOff);
						Message(pList, MsgStr, 2);
					}

					Global.DLoc.Calc();
				}
				else if (Global.DLoc.NeedOffset()) {
					//OutputDebugString("<< NedOffset");
					Global.DLoc.SetOffset((Param.finalX - Param.initX) + Param.PatOffX, (Param.finalY - Param.initY) + Param.PatOffY);
					MsgStr.Format("DLocOff0: %.3f %.3f", Global.DLoc.XOff, Global.DLoc.YOff);
					Message(pList, MsgStr, 2);
				}
				// Arrived at final measurement position [6/16/2013 Yuen]
				p->Co.x = g->LocXY.X;
				p->Co.y = g->LocXY.Y;
				//pMtr->GetXYPos(&p->Co.x,&p->Co.y);
				pMtr->GetThinFilmZPosDev(fZ);
				// Record actual position [2/21/2012 C2C]
				p->Co.z = fZ - pRcp->fThinFilmZ;	// p->Co.z is offset value [2/21/2012 C2C]
				MsgStr.Format("Final Pos: (X=%.4f Y=%.4f Z2=%.4f)", p->Co.x, p->Co.y, p->Co.z);
				Message(pList, MsgStr, 2);
			}
			// Here the position of the stage is at Matched location [11/8/2012 Yuen]

			break;
		default:
			break;
		}
	}
	{
		MsgStr.Format("Measure point %d STARTS", p->Co.n);
		Message(pList, MsgStr);
		// Using thread to allow the main thread message pump to continue [8/7/2013 Yuen]
		Param.pRcp = pRcp;
		Param.pObj = (LPVOID)this; // C413App object [8/7/2013 Yuen]
		DWORD id = 89;
		HANDLE h = CreateThread(NULL, 0, C413AppThinF::GetThinFilmThread, (void*)&Param, 0, &id);
		DWORD dw = ::WaitForSingleObject(h, INFINITE);
		switch (dw) {
		case WAIT_OBJECT_0:
			MsgStr.Format("Measure point %d DONE", p->Co.n);
			Message(pList, MsgStr);
			if (Param.bRetCode == TRUE) {
				p->ProgressSM = CMPoint::COMPLETED;
				CloseHandle(h);
				return TRUE;
			}
			break;
		case WAIT_FAILED:
			MsgStr = "Fail to start GetThinFilmTHread";
			Message(pList, MsgStr);
			break;
		}
		CloseHandle(h);
	}

	p->ProgressSM = CMPoint::FAILED;
	return FALSE;
}

// Single location measurement [5/31/2012 Yuen]
BOOL C413App::MeasureMultiPoint(CRecipe413* pRcp, CMPoint* pMP, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}
	C413Global* g = &Global;

	pMP->ProgressSM = CMPoint::MEASURING;

	int j = 0;
	if (g->bAborting) {
		pMP->ProgressSM = CMPoint::NOTMEASURE;
		return TRUE;
	}
	BOOL bSuccess = TRUE;
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[i];

		switch (pM->Prb) {
		case MParam::RADIALMULTITPRB:
		case MParam::RADIALMULTIBPRB:
			if (!GetRadialMultiPoint(pRcp, pMP, j, pVideo, pImgWnd, pProfile)) {
				bSuccess = FALSE;
			}
			break;
		}
	}
	if (bSuccess) {
		pMP->ProgressSM = CMPoint::COMPLETED;
		return TRUE;
	}
	else {
		pMP->ProgressSM = CMPoint::FAILED;
		return FALSE;
	}
}

// Store image in MatchDIB [9/13/2011 Administrator]

// DIBi = Original image [2/17/2011 XPMUser]
// DIB = Score pattern [2/17/2011 XPMUser]

// Note: measure high point first  [8/21/2011 FSMT]
// Generic measure difference of 2 points [8/21/2011 FSMT]
// Not suitable for trim measurement [8/21/2011 FSMT]
// TrimHeightDistanceInner(X) and TrimHeightDistanceOuter(Y) are used to specify dX and dY distance for the second point [8/21/2011 FSMT]

// Create half size DIB and store in pBuf [9/13/2011 Administrator]

BOOL C413App::GetRadialMultiPoint(CRecipe413* pRcp, CMPoint* pMP, short nTrial, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile) {
	pMP->ProgressSM = CMPoint::COMPLETED;
	return TRUE;
}

void C413App::Dump2(CString name, short* pBuf, short len) {
	FILE* fp;
	fp = fopen(name, "wb");
	if (fp) {
		for (int i = 0; i < len; i++) {
			fprintf(fp, "%d\n", *(pBuf + i));
		}
		fclose(fp);
	}
}

// Call SaveSpectru without file extension [2/12/2012 Administrator]

// radius1 = inner radius [2/20/2012 C2C]
// radius2 = outer radius [2/20/2012 C2C]

// decrease value by val if non zero positive and increase value by val if non zero negative [5/28/2012 Yuen]
// set to zero if value cross zero

// Installed reflector at the edge of wafer which saturated the camera, the following implementation become possible [6/28/2012 user]
// Method [6/28/2012 user]
// Using the first derivative of raw data, reverse all negative values [6/28/2012 user]
// looking from outsize of wafer in, the outer edge of bevel shall be the first peak and [6/28/2012 user]
// the inner edge of bevel shall be the second peak [6/28/2012 user]

// Backup function, old implementation [6/28/2012 user]

long C413App::DeltaMove(CPoint* pT, CCamBase::CAMERAID ActiveCamera, CRect& rc) {
	float x, y;
	int wd, ht;

	C413Global* g = &Global;

	switch (ActiveCamera) {
	default:
		wd = g->ScrnCalibECH.ScrnX;
		ht = g->ScrnCalibECH.ScrnY;
		break;
	case CCamBase::THINFCAM:
		wd = g->ScrnCalibTF.ScrnX;
		ht = g->ScrnCalibTF.ScrnY;
		break;
	}

	x = wd / float(rc.Width());
	y = ht / float(rc.Height());
	pT->x *= x;
	pT->y *= y;

	switch (ActiveCamera) {
	default:
		x = pT->x * g->ScrnCalibECH.dX;
		y = pT->y * g->ScrnCalibECH.dY;
		pMtr->MoveRXY(x, y);
		break;
	case CCamBase::THINFCAM:
		x = pT->x * g->ScrnCalibTF.dX;
		y = pT->y * g->ScrnCalibTF.dY;
		pMtr->MoveRXY(x, y);
		break;
	}
	return 0;
}

void C413App::StartDialogBoxes() {
	pMCtrl = new CMotionControl;
	if (pMCtrl) {
		pMCtrl->Create(CMotionControl::IDD);
		pMCtrl->ShowWindow(SW_HIDE);
	}

	if (DosUtil.GetFanFilterUnit()) {
		FFUThread.CreateThread();
		//	pFFU = new CFFUFailDlg;
		//	if (pFFU)
		//	{
		//		pFFU->Create(CFFUFailDlg::IDD);
		//		pFFU->ShowWindow(SW_HIDE);
		//	}
	}
}

BOOL C413App::StartMotionSystem(CListBox* pMessage) {
	if (pMtr) {
		ASSERT(0);
		return FALSE;
	}
	C413Global* g = &Global;

	if (MotorController == C413App::SICONTRLR) {
		pMtr = new CMC2;
	}
	else if (MotorController == C413App::GALILCONTRLR) {
		pMtr = new CMC3;
	}
	if (pMtr) {
		if (pMtr->Initialize(pMessage)) {
			pMessage->InsertString(pMessage->GetCount(), "System Motion Controller initialized OK");
			pMtr->SetMicroscopeLight(FALSE);
			pMtr->SetSpectroIllumLight(FALSE);
			// [ Masum 09032024
			pMtr->SetFFU(TRUE);
			// ]
			if (pMtr->GetPosAll(&g->LocXY.X, &g->LocXY.Y, &g->LocEchoTopZ, &g->LocRoughZ, &g->LocCWLTopZ, &g->LocEchoBottomZ)) {
				return TRUE;
			}
		}
		else {
			pMessage->InsertString(pMessage->GetCount(), "System Motion Controller initialize FAILED");
		}
	}
	return FALSE;
}

// [ 02082024/MORSALIN
#include "ProbeSwitching.h"
#include "ProbeSwitchingBase.h"
BOOL C413App::StartProbeSwitchingSystem(CListBox* pMessage) {
	if (pPSwitch) {
		ASSERT(0);
		return FALSE;
	}
	C413Global* g = &Global;
	//g->LoadProbeSwitchingOptions();
	if (g->isPSwitchingSimu) {
		pPSwitch = new CProbeSwitchingBase();
		if (pPSwitch) {
			if (pPSwitch->Connect(g->comPort, g->transRate, g->parity, g->stopBit)) {
				pMessage->InsertString(pMessage->GetCount(), "Probe Switching (Simu) started OK");
			}
		}

	}
	else {
		pPSwitch = new CProbeSwitching();
		if (pPSwitch) {
			if (pPSwitch->Connect(g->comPort, g->transRate, g->parity, g->stopBit)) {
				pMessage->InsertString(pMessage->GetCount(), "Probe Switching started OK");
			}
			else {
				pMessage->InsertString(pMessage->GetCount(), "Probe Switching started FAILED");
			}
		}
	}

	pMessage->InsertString(pMessage->GetCount(), "System Resetting....");//03062025

	return	TRUE;
}

void C413App::StopProbeSwitchingSystem(void) {
	if (pPSwitch) {
		delete pPSwitch;
		pPSwitch = NULL;
	}
}
// ]

void C413App::StopMotionSystem(void) {
	if (pMtr) {
		pMtr->SetMicroscopeLight(FALSE);
		pMtr->SetChuckVacuum(FALSE);
		pMtr->SetSpectroSourceLight(FALSE);
		pMtr->SetSpectroIllumLight(FALSE);
		delete pMtr;
		pMtr = NULL;
	}
}

// Method 1: using echo probe to measure calibration wafer [9/12/2012 Administrator]
// Method 2: using camera to see a predetermined marker [9/12/2012 Administrator]
BOOL C413App::ConfirmLoadPos(SDoPattrnMatching& Param) {
	MsgStr = "Verify stage position";
	Message(Param.pList, MsgStr);

	GotoPositionecho(); // Move P motor to echo position. Need to be non-blocking [7/13/2010 C2C]
	SwitchCameraSetExposure(CCamBase::ECHOCAM, Global.fLExposure + Global.BaseExpoEcho, Param.pVideo, Param.pList, Param.pRcp);

	switch (Global.LoadPosConfType) {
	case C413Global::USEPATTMATCHING:
		return ConfirmLoadPosPattMatch(Param);
		break;
	case C413Global::USEECHOPROBE:
		return ConfirmLoadPosEcho();
		break;
	default:
		break;
	}
	return TRUE;
}

// Return
//		  -2 = aborting [8/5/2013 Yuen]
//		  -1 = not enough peak [10/22/2012 FSM413]
//         0 = success [10/22/2012 FSM413]
//         1 = retry [10/22/2012 FSM413]
short C413App::ConfirmLoadPosOne(CRecipe413* pRcpl) {
	CPeaks Pk;
	short nAve = 8;
	short cnt = 1;

	if (!CSICam::pCamECHO) {
		ASSERT(0);
		return -4;
	}

	C413Global* g = &Global;

	if (pMtr->pSiMc->bSimu) {
		return 0;
	}

	if (CSICam::pCamECHO->CamType == CCamBase::TYSIMUCAM) DefinePeakPosition0(0);

	while (cnt++) {
		if (g->bAborting) {
			return -2;
		}
		if (pEcho->GetPeaks(pRcpl, Pk, nAve, NULL, NULL, NULL, 1, "") <= CEchoProbeBase::GPPKTOTALERR) {
			if (cnt > 6) {
				Log("ConfirmLoadPos: GetPeaks error", 2);
				return -3;
			}
		}
		else {
			break;
		}
	}

	// Not all installation will show exactly 3 peaks, ASE shows 4 on top probe
	if ((Pk.Total[0] < 3) || (Pk.Total[1] < 3)) {
		Log("ConfirmLoadPos: Incorrect amount of peaks", 2);
		return -1;
	}

	float v1, v2, thk1, thk2;
	v1 = Pk.fCM[0][2];
	v2 = Pk.fCM[0][1];
	thk1 = fabs((v1 - v2) * cfTop / (3.5f * TRiCorr) - g->DPGageThickness);

	v1 = Pk.fCM[1][2];
	v2 = Pk.fCM[1][1];
	thk2 = fabs((v1 - v2) * cfBot / (3.5f * BRiCorr) - g->DPGageThickness);

	CString tch;
	if ((fabs(thk1) < 3.0) && (fabs(thk2) < 3.0)) {
		tch.Format("[ME]Confirm Load Position : thk1 = %.2f thk2 = %.2f", thk1, thk2);
		Log(tch, 5);
		return 0;
	}
	tch.Format("[ME]Fail Load Position : thk1 = %.2f thk2 = %.2f", thk1, thk2);
	Log(tch, 1);
	return 1;
}

// Used in ProcJobManual, ProcJobPWC413 [11/11/2011 Administrator]
BOOL C413App::LoadRecipe(CRecipe413* pRcp, short Option) {
	if (!pRcp) {
		return FALSE;
	}

	BOOL OK = FALSE;
	CString filename;

	if (strlen(pRcp->RcpeName) > 0) {
		filename = DosUtil.GetRecipeDir();
		filename += "\\";
		filename += pRcp->RcpeName;
		if (Option == 0) {
			if (pRcp->Load(filename))// using filename [9/15/2012 Administrator]
			{
				OK = TRUE;
			}
			else if (pRcp->LoadRecipe()) // Ask for filename [9/15/2012 Administrator]
			{
				OK = TRUE;
			}
		}
		else {
			if (pRcp->Load(filename)) // using filename [9/15/2012 Administrator]
			{
				OK = TRUE;
			}
		}
	}
	else {
		if (Option == 0) {
			if (pRcp->LoadRecipe()) // Ask for filename [9/15/2012 Administrator]
			{
				OK = TRUE;
			}
		}
	}
	return OK;
}

BOOL C413App::IsAborting() {
	return Global.bAborting;
}

void C413App::SetPeakParamDualCalib(CPeakParam& PParam) {
	PParam.Clear();
	PParam.nPeakT = 2;
	PParam.HeightT[0].Set(0, 256);
	PParam.HeightT[1].Set(1, 256);
	PParam.nPeakB = 2;
	PParam.HeightB[0].Set(0, 256);
	PParam.HeightB[1].Set(1, 192);
	PParam.Method = CPeakParam::cmGREATER;
}

// Marker = 2 not yet implemented [11/4/2012 Yuen]
BOOL C413App::GetThinFilmMarker(short Marker, CRecipe413* pRcp, CMPoint* p, short nTrial, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, short ID) {
	int j;
	float X, Y, Z;
	CString str;

	//20220520/yukchiu
	markerCount++;

	if ((p->PatSet < 1) || (!pVideo) || (!pRcp)) {
		return FALSE;
	}

	C413Global* g = &Global;


	//20220520/yukchiu
	if (markerCount == 1) {
		str = "Get TFMarker 1";
	} 
	else if (markerCount == 2) {
		str = "Get TFMarker 2";
	}
	//================
	
	Message(pList, str);


	//20220520/yukchiu
	if (markerCount == 1) {
		str = "Auto Exposure 1";
	}
	else if (markerCount == 2) {
		str = "Auto Exposure 2";
	}
	//================
	
	Message(pList, str);
	pVideo->Level = pRcp->AutoExposeTF;
	pVideo->AutoAdjust();

	for (j = 0; j < nTrial; j++) {
		if (g->bAborting) {
			break;
		}

		str.Format("Trial #%d", j + 1);
		Message(pList, str);

		float lX = p->Co.x + gPX[j] * pRcp->fStepSizeEcho;
		float lY = p->Co.y + gPY[j] * pRcp->fStepSizeEcho;
		float lZ = pRcp->fThinFilmZ + p->Co.z;

		str.Format("GotoXYZ: %.3f %.3f %.3f", lX, lY, lZ);
		Message(pList, str);

		if (pMtr) {
			pMtr->MoveThinFilmZ(lZ);
			if (!pMtr->GotoXY(lX, lY)) {
				return FALSE;
			}
			pMtr->WaitThinFilmZ();
			if (j == 0) {
				X = g->LocXY.X;
				Y = g->LocXY.Y;
				//pMtr->GetXYPos(&X,&Y);
				pMtr->GetThinFilmZPosDev(Z);
				//Z -= pRcp->fThinFilmZ;	// Z is offset value [2/21/2012 C2C]
			}
		}

		str.Format("Auto Exposure: %d", pRcp->AutoExposeTF);
		Message(pList, str);
		pVideo->Level = pRcp->AutoExposeTF;
		pVideo->AutoAdjust();

		str = "Matching";
		Message(pList, str);

		// Get ThinFilm Marker [3/28/2013 Yuen]
		// This function need to match many times [11/8/2012 Yuen]
		float X1 = 0, Y1 = 0, Z2 = 0;
		SDoPattrnMatching Param;
		Param.Set(pVideo, pImgWnd, pList);
		SetupParam(pRcp, Param, 0);
		Param.bApplyOffset = FALSE;
		Param.bMoveCenter = TRUE;
		Param.PaSSet = 0; // Assume 0 for now [5/5/2014 Administrator]
		Param.Set(pVideo, pImgWnd, pList);
		if (pRcp->MatchAlgorithm[p->PatSet - 1] == 4) {
			Param.bMoveCenter = TRUE;
		}
		else {
			Param.bMoveCenter = FALSE;
		}

		X1 = Param.initX = g->LocXY.X;
		Y1 = Param.initY = g->LocXY.Y;
		//pMtr->GetXYPos(&X1,&Y1);
		//pMtr->GetXYPos(&Param.initX,&Param.initY);
		short PatMatchRsl = DoPattrnMatching(Param);	// Thin Film Marker Match [6/28/2013 Yuen]
		switch (PatMatchRsl) {
		case C413AppPatt::pmOK:
			break;
		default:
			// Handle exception here [2/25/2013 Administrator]
			/*ASSERT(0);*/
			Message(pList, CString("Match result not pmOK"));
			break;
		}
	}
	if (j >= nTrial) {
		// per point pattern matching failed [2/21/2012 C2C]
		switch (ID) {
		case 0:
			pGDev->SetAlarm(CAlrmList::LP1PATMATFAILCONTINUE);
			break;
		case 1:
			pGDev->SetAlarm(CAlrmList::LP2PATMATFAILCONTINUE);
			break;
		}
	}

	str = "Get TFMarker 1: Failed";
	Message(pList, str);
	return FALSE;
}

// To be completed later [11/4/2012 Yuen]
BOOL C413App::GetEchoMarker(short Marker, CRecipe413* pRcp, CMPoint* p, short nTrial, CCamBase::CAMERAID ActCamera, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList, short ID) {
	// [20220601/yukchiu
	//ASSERT(0);

	/*if (p->PatSet < 1) {
		return FALSE;
	}*/

	//20220601/yukchiu
	if ((p->PatSet < 1) || (!pVideo) || (!pRcp)) {
		return FALSE;
	}
	CString str;							//20220601/yukchiu
	// ]================

	C413Global* g = &Global;

	int j;
	float X, Y;
	BOOL bMatched = FALSE;

	for (j = 0; j < nTrial; j++) {
		if (g->bAborting) {
			break;
		}
		if (pMtr && (j != 0)) {
			MsgStr.Format("Trial:%d (%.3f,%.3f)", j + 1, p->Co.x + gPX[j], p->Co.y + gPY[j]);
			Message(pList, MsgStr);
			if (!pMtr->GotoXY(X + gPX[j], Y + gPY[j])) {
				return FALSE;
			}
		}
		if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::EXPOSE4) {
			pVideo->Level = pRcp->AutoExposeEcho;
			pVideo->AutoAdjust();
		}
		if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::FOCAMATCH) {
			AutoFocusVideo(CCamBase::ECHOCAM, pRcp->fTgtGRnX[p->PatSet - 1][0], pRcp->fTgtGRnY[p->PatSet - 1][0], pVideo);
		}
		// This function need to match many times [11/8/2012 Yuen]
		// Get Echo Marker [3/28/2013 Yuen]
		SDoPattrnMatching Param;
		SetupParam(pRcp, Param, 0);
		Param.bApplyOffset = FALSE;
		Param.bMoveCenter = TRUE;
		Param.PaSSet = 0; // Assume 0 for now [5/5/2014 Administrator]
		Param.SearchDepth = pRcp->SearchDept[p->PatSet - 1];
		Param.Set(pVideo, pImgWnd, pList);
		if (pRcp->MatchAlgorithm[p->PatSet - 1] == 4) {
			Param.bMoveCenter = TRUE;
		}
		else {
			Param.bMoveCenter = FALSE;
		}

		Param.initX = g->LocXY.X;
		Param.initY = g->LocXY.Y;
		//pMtr->GetXYPos(&Param.initX,&Param.initY);
		short PatMatchRsl = DoPattrnMatching(Param);	// Echo Marker Match [6/28/2013 Yuen]
		switch (PatMatchRsl) {
		case C413AppPatt::pmOK:
			return TRUE;
		default:
			// Handle exception here [2/25/2013 Administrator]
			break;
		}
		X = g->LocXY.X;
		Y = g->LocXY.Y;
		//if (pMtr) pMtr->GetXYPos(&X,&Y);
	}
	return FALSE;
}

// Will not switch camera [6/10/2013 Yuen]
BOOL C413App::FindWaferCenterUsingEdgeDetection(SFINDCENTER& SFC) {
	float ang[3] = { 135.0f, 45.0f, 315.0f };
	CString name1[3] = { "E135R150-1.BMP", "E45R150-1.BMP", "E315R150-1.BMP" };
	CString name2[3] = { "E135R150.BMP", "E45R150.BMP", "E315R150.BMP" };
	SFC.pAng = &ang[0];
	SFC.pName1 = &name1[0];
	SFC.pName2 = &name2[0];
	//SFC.Method = CImgWnd::DARKBWD;
	return FindWaferCenterA(SFC);
}

// Will not switch camera [6/10/2013 Yuen]
BOOL C413App::FindWaferCenterUsingEdgeDetectionFast(SFINDCENTER& SFC) {
	float ang[3] = { 135.0f, 90.0f, 45.0f };
	CString name1[3] = { "E135R150-1.BMP", "E90R150-1.BMP", "E45R150-1.BMP" };
	CString name2[3] = { "E135R150.BMP", "E90R150.BMP", "E45R150.BMP" };
	SFC.pAng = &ang[0];
	SFC.pName1 = &name1[0];
	SFC.pName2 = &name2[0];
	//SFC.Method = CImgWnd::DARKFWD;
	return FindWaferCenterA(SFC);
}

BOOL C413App::FindWaferCenterA(SFINDCENTER& SFC) {
	SFC.pObj = (LPVOID)this; // C413App object [8/7/2013 Yuen]
	DWORD id = 90;
	HANDLE h = CreateThread(NULL, 0, C413App::FindWaferCenterThread, (void*)&SFC, 0, &id);
	DWORD dw = ::WaitForSingleObject(h, INFINITE);
	switch (dw) {
	case WAIT_OBJECT_0:
		if (SFC.bRetCode == TRUE) {
			CloseHandle(h);
			return TRUE;
		}
		break;
	case WAIT_FAILED:
		MsgStr = "Fail to start GetThinFilmTHread";
		Message(SFC.pList, MsgStr);
		break;
	}
	CloseHandle(h);
	return FALSE;
}

DWORD C413App::FindWaferCenterThread(LPVOID Param) {
	SFINDCENTER* p = (SFINDCENTER*)Param;
	p->bRetCode = p413App->FindWaferCenterUsingEdgeDetectionA(*p);
	return 0;
}

BOOL C413App::FindWaferCenterUsingEdgeDetectionA(SFINDCENTER& Sf) {
	// sanity check [6/6/2013 Yuen]
	if (!Sf.pVideo || !Sf.pImgWnd || !Sf.pRcp) {
		// Cannot continue [6/6/2013 Yuen]
		*Sf.pCXOffset = 0;
		*Sf.pCYOffset = 0;
		*Sf.pCRadius = 0;
		return FALSE;
	}
	int i, j;
	float X[3], Y[3], R[3];
	C413Global* g = &Global;

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "IMAGES\\ALIGN\\";

	//@XXX Switch Camera to Echo [6/12/2013 FSM413]
	CCamBase::CAMERAID SIView = Sf.pVideo->iCID;
	SwitchCameraSetExposure(CCamBase::ECHOCAM, g->AlgnExposure + g->BaseExpoEcho, Sf.pVideo, Sf.pList, Sf.pRcp);

	CString tm;
	tm = CTime::GetCurrentTime().Format("%H%M%S");

	g->BevelWidth = 0;
	switch (g->ActiveCamera) {
	default:
		g->COffEcho.Clear();
		break;
	case CCamBase::THINFCAM:
		g->COffTF.Clear();
		break;
	case CCamBase::CWL:
		g->COffCWL.Clear();
		break;
	case CCamBase::ROUGH:
		g->COffRough.Clear();
		break;
	}

	SFINDEDGES Par;
	Par.pRcp = Sf.pRcp;
	Par.pList = Sf.pList;
	Par.pVideo = Sf.pVideo;
	Par.pImgWnd = Sf.pImgWnd;
	Par.Cmax = 0; Par.Cmin = 0;

	Par.Thres = g->FindEdgeThres;
	Par.bFindBestAngle = FALSE;
	Par.bContrastStrecth = FALSE;
	Par.method = (CImgWnd::FNDMTHD)Sf.Method;
	Par.Exposure = g->AlgnExposure;
	Par.AutoExpose = 0;
	Par.ZPosition = g->fAlignZEcho;
	Par.name = "";
	Par.bMoveStage = TRUE;
	Par.bTest = Sf.bTest;

	// Find 3 edges to calculate wafer center [8/5/2013 Yuen]
	for (i = 0; i < 3; i++) {
		if (Global.bAborting) {
			return FALSE;
		}
		Par.angle = Sf.pAng[i];
		Par.radius = Sf.pRcp->FindEdgeWaferSize;

		if (Sf.pVideo->IsSimu()) {
			Par.name.Format("%s%s", dir, Sf.pName1[i]);
		}
		BOOL bFound = FALSE;

		// Progressive refinement of edges [6/9/2014 Yuen]
		for (j = 0; j < 3; j++) {
			if (g->bAborting) {
				break;
			}
			MsgStr.Format("FindEdge #%d-%d: Angle %.3f  Radius %.3f", i + 1, j + 1, Par.angle, Par.radius);
			Message(Sf.pList, MsgStr);
			if (!FindEdges(Par)) {
				// Fail to locate edge [7/27/2013 Yuen]
				if (Par.In > 15000.0f) {
					// too far reduce radius [7/29/2013 Yuen]
					if (Sf.pVideo->IsSimu()) {
						Par.name.Format("%s1-%s", dir, Sf.pName1[i]);
					}
					Par.radius -= 0.5f; // reduce 0.5 mm [7/29/2013 Yuen]
					MsgStr.Format("FindEdge #%d-%d: Angle %.3f  Radius %.3f", i + 1, j + 1, Par.angle, Par.radius);
					Message(Sf.pList, MsgStr);
					if (!FindEdges(Par)) {
						continue;
					}
					else {
						bFound = TRUE;
						break;
					}
				}
				else {
					// Too near, increase radius [7/29/2013 Yuen]
					if (Sf.pVideo->IsSimu()) {
						Par.name.Format("%s2-%s", dir, Sf.pName1[i]);
					}
					Par.radius += 0.5f; // increase 0.5mm
					MsgStr.Format("FindEdge #%d-%d: Angle %.3f  Radius %.3f", i + 1, j + 1, Par.angle, Par.radius);
					Message(Sf.pList, MsgStr);
					if (!FindEdges(Par)) {
						continue;
					}
					else {
						bFound = TRUE;
						break;
					}
				}
			}
			else {
				bFound = TRUE;
				break;	// Found [8/5/2013 Yuen]
			}
		}
		if (!Par.score || !bFound) {
			RestoreView(SIView, Sf.pRcp, Sf.pVideo, Sf.pList);
			return FALSE;
		}

		// Try for up to four time per location [7/25/2013 Yuen]
		// We probably had lock in [7/29/2013 Yuen]
		for (j = 0; j < 4; j++) {
			if (g->bAborting) {
				break;
			}
			if (fabs(Par.dist1) > 0.075f)	// too far from center [6/7/2013 Yuen]
			{
				Par.radius -= Par.dist1;
				if (Sf.pVideo->IsSimu()) {
					Par.name.Format("%s%s", dir, Sf.pName2[i]);
				}
				MsgStr.Format("FindEdge #%d-%d: Angle %.3f  Radius %.3f", i + 1, j + 1, Par.angle, Par.radius);
				Message(Sf.pList, MsgStr);
				if (!FindEdges(Par)) {
					return FALSE;
				}
				if (g->bAborting) {
					break;
				}
				if (!Par.score) {
					RestoreView(SIView, Sf.pRcp, Sf.pVideo, Sf.pList);
					return FALSE;
				}
			}
			else break;	// Found [8/5/2013 Yuen]
		}
		// Calculate X[i], Y[i] by calculating the radius  [6/7/2013 Yuen]
		R[i] = Par.dist1 + Par.radius;
		g->Polar2Cartesian(R[i], Sf.pAng[i], X[i], Y[i]);
	}

	// 3 points found [6/5/2013 Yuen]
	sABC L1, L2;	// the two lines [6/5/2013 Yuen]
	FindABC(L1, X[0], Y[0], X[1], Y[1]);
	FindABC(L2, X[1], Y[1], X[2], Y[2]);
	sABC PL1, PL2;	// Perpendicular bisector of L1 and L2 [6/5/2013 Yuen]
	PerpendicularLine(PL1, L1, X[0], Y[0], X[1], Y[1]);
	PerpendicularLine(PL2, L2, X[1], Y[1], X[2], Y[2]);
	float Cx, Cy;
	Intersec(PL1, PL2, Cx, Cy);

	float r, a;
	g->Cartesian2Polar(Cx, Cy, r, a);

	if (fabs(r) <= 5.0f) {
		if (fabs(Cx) < 0.00001f) {
			Cx = 0;
		}
		if (fabs(Cy) < 0.00001f) {
			Cy = 0;
		}
		*Sf.pCXOffset = Cx;
		*Sf.pCYOffset = Cy;
		switch (g->ActiveCamera) {
		default:
			g->COffEcho.Set(Cx, Cy);
			g->COffTF.Set(Cx, Cy);
			g->SaveMotorOption();
			break;
		case CCamBase::THINFCAM:
			g->COffEcho.Set(Cx, Cy);
			g->COffTF.Set(Cx, Cy);
			g->SaveMotorOption();
			break;
		case CCamBase::CWL:
			g->COffCWL.Set(Cx, Cy);
			g->SaveMotorOption();
			break;
		case CCamBase::ROUGH:
			g->COffRough.Set(Cx, Cy);
			g->SaveMotorOption();
			break;
		}

		// Verify radius and calculate bevel after applying center offset[6/6/2013 Yuen]

		// move to CRadius with offset applied and confirm indeed CRadius is CRadius [6/6/2013 Yuen]
		Par.radius = *Sf.pCRadius = Sf.pRcp->FindEdgeWaferSize;
		Par.angle = Sf.pAng[2];
		Par.name = Sf.pName2[2];
		Par.method = CImgWnd::DARKBWD;
		if (Sf.pVideo->IsSimu()) {
			Par.name.Format("%s%s", dir, Sf.pName2[2]);
		}
		MsgStr.Format("FindEdge #%d-%d: Angle %.3f  Radius %.3f", i + 1, j + 1, Par.angle, Par.radius);
		Message(Sf.pList, MsgStr);
		FindEdges(Par);
		if (Par.score > 0) {
			*Sf.pCRadius -= Par.dist1;
			if (abs((*Sf.pCRadius - Sf.pRcp->FindEdgeWaferSize) <= 1.0f)) {
				// Radius confirmed [6/7/2013 Yuen]
				// Make sure area > CRadius is all white [6/7/2013 Yuen]

				float distout = Par.dist1;
				Par.method = CImgWnd::DARKFWD;
				Par.bMoveStage = FALSE;
				MsgStr.Format("FindEdge #%d-%d: Angle %.3f  Radius %.3f", i + 1, j + 1, Par.angle, Par.radius);
				Message(Sf.pList, MsgStr);
				FindEdges(Par);

				// Set Bevel Width [6/7/2013 Yuen]
				g->BevelWidth = fabs(Par.dist1 - distout);
				Par.pImgWnd->TWBevel = g->BevelWidth;
				Par.pImgWnd->fTWEdge1 = Cx;
				Par.pImgWnd->fTWEdge2 = Cy;
				Par.pImgWnd->nTWEdge1 = 0;
				Par.pImgWnd->nTWEdge2 = 0;

				tm.Format("CX,%.4f,,CY,%.4f,,Radius,%.4f,,Bevel,%.4f,,Exposure,%d,,Z-Pos,%.4f", *Sf.pCXOffset, *Sf.pCYOffset, *Sf.pCRadius, g->BevelWidth, g->AlgnExposure, g->fAlignZEcho);
				RecordWaferCenterEvent(tm);
				RestoreView(SIView, Sf.pRcp, Sf.pVideo, Sf.pList);
				return TRUE;
			}
		}
	}
	RestoreView(SIView, Sf.pRcp, Sf.pVideo, Sf.pList);
	return FALSE;
}

void C413App::RestoreView(CCamBase::CAMERAID SIView, CRecipe413* pRcp, CSICam* pVideo, CListCtrl* pList) {
	switch (pVideo->iCID) {
	default:
		SwitchCameraSetExposure(SIView, pRcp->EchoNormalExposure + Global.BaseExpoEcho, pVideo, pList, pRcp);
		break;
	case CCamBase::THINFCAM:
		SwitchCameraSetExposure(SIView, pRcp->TFNormalExposure + Global.BaseExpoTF, pVideo, pList, pRcp);
		break;
	}
}

void C413App::QuickEdit(CRecipe413* pRcp) {
	if (!pRcp) {
		ASSERT(0);
		return;
	}

	CQuickRecipeEditDlg dlg;
	dlg.pRcp = pRcp;
	dlg.DoModal();
}

#define CALCOUNT 48

BOOL C413App::DoSPCalib(CRecipe413* pRcpl) {
	short cnt = 0;
	float fTop[CALCOUNT];
	float fBot[CALCOUNT];
	for (int i = 0; i < CALCOUNT; i++) {
		if (pEcho->GetPeaks(pRcpl, Pk, 8, NULL, NULL, NULL, 1, "") > CEchoProbeBase::GPPKTOTALERR) {
			fTop[i] = Global.SPGageThickness * SRIT / pEcho->fGetThickness(0, 1, 2);
			fBot[i] = Global.SPGageThickness * SRIB / pEcho->fGetThickness(1, 1, 2);
			cnt++;
		}
	}
	CString tch;
	if (cnt > CALCOUNT - 8) {
		//20131124 SCOTT FOR TO MEET THE SPEC float cfTop = fRemoveFlyer(cnt,fTop,0,1);
		float cfTop = fRemoveFlyer(cnt, fTop, 0, 1) + 0.0;
		float cfBot = fRemoveFlyer(cnt, fBot, 0, 1);
		if ((fabs(cfTop - 1) < 0.01f) && (fabs(cfBot - 1) < 0.01f)) {
			//20131124 SCOTT FOR TO MEET THE SPEC  cfTop = fRemoveFlyer(cnt,fTop,0,1);
			cfTop = fRemoveFlyer(cnt, fTop, 0, 1) + 0.0;
			cfBot = fRemoveFlyer(cnt, fBot, 0, 1);
			DosUtil.SetEncoderFactor(1, cfTop);
			tch.Format("Gauge,%.4f,,Cal Top,%.8f,,Cal Bottom,%.8f", Global.SPGageThickness, cfTop, cfBot);
			RecordSPCalibEvent(tch);

			DosUtil.SetEncoderFactor(2, cfBot);
			tch.Format("SPCALIB : Cal Top = %.8f   Cal Bottom = %.8f", cfTop, cfBot);
			Log(tch, 2);
			return TRUE;
		}
	}

	tch.Format("SPCALIB: Fail single probe calibration ");
	RecordSPCalibEvent(tch);
	Log(tch, 1);
	return FALSE;
}

BOOL C413App::CanTransfer2Stage(short Lp, short slot) {
	if (pGDev->GetLPState(Lp) != PrcsMgrServices::OPENED) {
		return FALSE;
	}
	// Stage ready? [7/12/2010 128 NT]
	if (Global.WaferOnStage > 0) {
		return FALSE;
	}
	return TRUE;
}

BOOL C413App::CanTransfer2Cas(short Lp, short slot) {
	CString str;
	if (pGDev->GetLPState(Lp) != PrcsMgrServices::OPENED) {
		return FALSE;
	}
	// Stage ready? [7/12/2010 128 NT]
	if (Global.WaferOnStage == 0) {
		return FALSE;
	}
	return TRUE;
}

BOOL C413App::LoadRecipeFromDataFile(CRecipe413& Rcp) {
	CString month;
	CTime t = CTime::GetCurrentTime();
	month = t.Format("%Y%m");
	month += "\\C2C";

	CFileDialog dlg(TRUE,
		"C2C",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Data file|*.C2C|");

	CString filename = "";
	CString root = DosUtil.DirSys.GetRootDir() + "\\" + month;
	dlg.m_ofn.lpstrInitialDir = root;

	if (dlg.DoModal() != IDOK) {
		return FALSE;
	}
	filename = dlg.GetPathName();

	CCasMgr CasMgr;
	if (CasMgr.OpenCassette(filename)) {
		if (CasMgr.pCas) {
			Rcp = *CasMgr.GetCurCas()->GetRecipe413();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL C413App::ConfirmLoadPosEcho() {
	CString msg;

	if (!pMtr) {
		return FALSE;
	}

	CRecipe413* pRcpl = new CRecipe413;
	if (!pRcpl) {
		return FALSE;
	}

	C413Global* g = &Global;

	msg = "Load DPCALIB.RCP";
	Log(msg);
	strcpy(pRcpl->RcpeName, "DPCALIB.RCP");	// Preset calibration recipe [9/15/2012 Administrator]
	if (!LoadRecipe(pRcpl, 1)) {
		Log("ConfirmLoadPos: Could not load recipe DPCALIB.RCP", 0);
		delete pRcpl;
		return FALSE;
	}

	ResetEchoSamplingProperty(pRcpl, NULL);
	pDMC->Run();
	pEcho->StartGetPeaks(NULL);	// Dummy measurement, to exercise the digitizer card [5/30/2012 Yuen]

	pMtr->MoveEchoTopZ(g->CalZPosEcho, FALSE);
	if (!GotoXYS(g->DPPos.X, g->DPPos.Y, CCamBase::ECHOCAM)) {
		StageMoveError();
		return FALSE;
	}
	pMtr->WaitEchoTopZ();
	Sleep(1000);	// Wait for DMC go get ready [8/11/2013 FSM413]

	for (int i = 0; i < 3; i++) {
		if (g->bAborting) {
			delete pRcpl;
			return FALSE;
		}
		switch (ConfirmLoadPosOne(pRcpl)) {
		case 0:
			// success [10/22/2012 FSM413]
			if (g->bSPCalib) {
				DoSPCalib(pRcpl);
			}
			delete pRcpl;
			return TRUE;
		case -1:
			// error [10/22/2012 FSM413]
			delete pRcpl;
			return FALSE;
		default:
			// retry [10/22/2012 FSM413]
			break;
		}
	}
	// exceed retry count [10/22/2012 FSM413]
	delete pRcpl;
	return FALSE;
}

BOOL C413App::ConfirmLoadPosPattMatch(SDoPattrnMatching& Param) {
	if (!Param.pVideo) {
		ASSERT(0);
		return FALSE;
	}

	CString MsgStr;
	SLocMatch SizeC;  // Coordinate in image space [9/13/2011 Administrator]
	SLocMatch SizeD;  // Coordinate in image space [9/13/2011 Administrator]

	C413Global* g = &Global;

	Param.pVideo->LoadStockDIB("BSI\\CENTER1.BMP");

	if (pMtr) {
		pMtr->MoveEchoTopZ(g->fLPosZ, FALSE);   // Not using g->CalZPosEcho [6/30/2014 Yuen]
		GotoXYS(g->fLPosX, g->fLPosY, Param.pVideo->iCID);
		pMtr->WaitEchoTopZ();
	}
	for (int i = 0; i < Param.SearchDepth; i++) {
		if (g->bAborting) {
			return FALSE;
		}
		if (!Param.pVideo->SnapOneFrame(g->fLExposure + g->BaseExpoEcho)) {
			return FALSE;
		}
		if (!g->StoredDIB.pDIB) {
			return FALSE;
		}

		Param.Par.pSourceDIB = &g->StoredDIB;
		Param.Par.pMatchDIB = Param.Par.pMatchDibA = &g->MatchDIBA;
		Param.Par.GetWidthHeight();

		if (GetMatchPosition(SizeC, Param, FALSE)) {
			Message(Param.pList, MsgStr);
			if (Global.MatchDIBB.pDIB) {
				// Check whether secondary image is visible base on position of supposed primary position [4/16/2013 Administrator]
				short dIx = g->StoredDIB.IWidth * (g->fLPosBLuX - g->fLPosGRnX); // Relative distance between Green and Blue crosshair [4/16/2013 Administrator]
				short dIy = g->StoredDIB.IHeight * (g->fLPosBLuY - g->fLPosGRnY); // Relative distance between Green and Blue crosshair [4/16/2013 Administrator]
				short Wdib2 = g->MatchDIBB.IWidth / 2 + 6;
				short Hdib2 = g->MatchDIBB.IHeight / 2 + 6;
				short dx, dy;
				dx = SizeC.nX + dIx;
				dy = SizeC.nY + dIy;
				if ((dx <= Wdib2) || (dx >= (g->StoredDIB.IWidth - Wdib2)) ||
					(dy <= Hdib2) || (dy >= (g->StoredDIB.IHeight - Hdib2))) {
					// Need moving. Move primary image to green cross hair [4/16/2013 Administrator]
					SizeC.nX = SizeC.nX - g->StoredDIB.IWidth * g->fLPosGRnX;
					SizeC.nY = -(SizeC.nY - g->StoredDIB.IHeight * g->fLPosGRnY);
					MsgStr = "PAT2 is out of bound, move PAT1 to center of screen";
					Message(Param.pList, MsgStr);
					Param.MoveDist = SizeC;
					MoveMatchPatCenter(Param);
					continue;
				}
				// SizeD contains the coordinate of the center of Match DIB with respect to main image [9/10/2011 XPMUser]
				Param.Par.pMatchDIB = Param.Par.pMatchDibB = &g->MatchDIBB;
				if (!Match2(Param, SizeD)) {
					return pmFAILED;
				}
				if (Param.pImgWnd) {
					ShrinkMatchDIB(2, g->MatchDIBB.pDIB, SizeD, Param.pImgWnd);
					//if (Param.pVideo->IsSimu() && Param.pImgWnd)
					//{
					//	Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX,SizeD.nY,255,0,0);
					//	Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX+1,SizeD.nY,255,0,0);
					//	Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX,SizeD.nY+1,255,0,0);
					//	Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX-1,SizeD.nY,255,0,0);
					//	Param.pImgWnd->pPackedDIB1->SetPixelRGB(SizeD.nX,SizeD.nY-1,255,0,0);
					//}
					//Param.pImgWnd->Refresh();
				}

				// Check relativity [4/16/2013 Administrator]
				// Image  is consider found if relative distance of green and blue cross hair is close to relative distance between the two match images [4/16/2013 Administrator]
				//short MatchRes = 8; // Hard coded [6/7/2014 Yuen]
				// Relative distance between PAT1 and PAT2 should be similar to relative distance between green and blue cross hair [4/20/2013 Administrator]
				short x = g->StoredDIB.IWidth * (g->fLPosGRnX - g->fLPosBLuX); // Relative distance between Green and Blue crosshair [4/16/2013 Administrator]
				short y = g->StoredDIB.IHeight * (g->fLPosGRnY - g->fLPosBLuY); // Relative distance between Green and Blue crosshair [4/16/2013 Administrator]
				dx = (SizeC.nX - SizeD.nX) - x;
				dy = (SizeC.nY - SizeD.nY) - y;
				if ((abs(dx) < Param.MatchRes) && (abs(dy) < Param.MatchRes)) {
					x = g->StoredDIB.IWidth * g->fLPosGRnX;
					y = g->StoredDIB.IHeight * g->fLPosGRnY;
					short dx1 = SizeC.nX - x;
					short dy1 = SizeC.nY - y;
					if ((abs(dx1) < Param.MatchRes) && (abs(dy1) < Param.MatchRes)) {
						MsgStr = "MATCH CONFIRMED";
						Message(Param.pList, MsgStr);
						MsgStr.Format("LoadPos: X,Y: (%d,%d)  SizeC: (%d,%d)  SizeD: (%d,%d)  dXY: (%d,%d)  rXY(%d,%d)", x, y, SizeC.nX, SizeC.nY, SizeD.nX, SizeD.nY, dx1, dy1, dx, dy);
						RecordEvent(MsgStr);
						// Move primary image to green cross hair [4/16/2013 Administrator]
						Param.MoveDist.nX = Param.MoveDist.nY;

						Param.pImgWnd->ClearTextInfo();
						//if (Param.pRcp->bSaveVideo)
						//{
						Param.pImgWnd->bStoreComposite = TRUE;
						Param.pImgWnd->SaveName.Format("C:\\TEMP\\%s\\LP%s %d %d %d %d.BMP", CTime::GetCurrentTime().Format("%y%m%d"), CTime::GetCurrentTime().Format("%H%M%S"), dx1, dy1, dx, dy);
						//}
						Param.pImgWnd->Refresh();

						//float X,Y;
						//pMtr->GetXYPos(&X,&Y);

						//if (fabs(X-x) < 0.200f)
						//{
						//	if (fabs(Y-y) < 0.200f)
						//	{
						return TRUE;
						//	}
						//}
						//return FALSE;
					}
				}
				MsgStr.Format("MATCH not within MatchRes %d: %d %d", Param.MatchRes, dx, dy);
				Message(Param.pList, MsgStr);
			}
			else {
				return TRUE;
			}
		}
	}

	Param.pImgWnd->ClearTextInfo();
	//	if (Param.pRcp->bSaveVideo)
	//	{
	Param.pImgWnd->bStoreComposite = TRUE;
	Param.pImgWnd->SaveName.Format("C:\\TEMP\\%s\\LP%s.BMP", CTime::GetCurrentTime().Format("%y%m%d"), CTime::GetCurrentTime().Format("%H%M%S"));
	//	}
	Param.pImgWnd->Refresh();

	return FALSE;
}

// Record an event in event log
void C413App::RecordEvent(CString strEvent) {
	CString mdir;
	DosUtil.GetProgramDir(mdir);
	mdir += "PROCESSEVENT.LOG";
	FILE* fp;
	fp = fopen(mdir, "ab");
	if (fp) {
		fprintf(fp, "%s %s\r\n", GetTimeStamp().GetBuffer(0), strEvent.GetBuffer(0));
		fclose(fp);
	}
}

//[30122024_1
void C413App::setSelectedTitle(CWaferMap* pWaferMap) {
	if (!pWaferMap) {
		ASSERT(0);
		return;
	}
	curTitle = pWaferMap->Title;
	return;
}

CString C413App::getSelectedTitle()
{
	return curTitle;
}
//]


//[03062025
void C413App::ResetAtBegin() {
	p413App->Global.isStart = true;
	p413App->Global.isDuringMes = true;
	CMotorS2* pMtr = p413App->pMtr;
	if (pMtr) {
		if (pMtr->ResetAll()) {
			pMtr->GotoXY(0, 0);
			//pMtr->SetHome();
			//g->ResetLapTime = GetTickCount64();
			//g->bConfirmStage = TRUE;
		}
	}
	p413App->pPSwitch->MoveToCWL030prb();
	p413App->Global.isStart = false;
	p413App->Global.isDuringMes = false; 
}

//]




