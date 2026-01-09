#include "stdafx.h"

#include "413/DMCMotorBase.h"
#include "413/EchoScope/EchoProbeBase.h"
#include "413/MotorL2.h"
#include "413/MotorS2.h"
#include "Data.h"
#include "MPoint.h"

#include "ProbeSwitchingBase.h"

#include "413AppEcho.h"

C413AppEcho::C413AppEcho() {}

C413AppEcho::~C413AppEcho() {}

BOOL C413AppEcho::DoDualProbeCalibration(float CalibThickness, float DPZPos, CRecipe413* pRcp, short MeSet, CListCtrl* pList, CSICam* pVideo, CPeakParam* pParam, BOOL bResetX, short SimuPk) {
	// Start echo capture [3/11/2010 Valued Customer]
	short SimuPkBak = pEcho->SimuPkNum;
	pEcho->SimuPkNum = SimuPk;

	if (pEcho->bSimu) {
		DefinePeakPosition0(SimuPk);
	}
	pEcho->SetRecipe(pRcp);

	if (bResetX) {
		MsgStr = "Reset X motor";
		Message(pList, MsgStr);
		if (!pMtr->ResetX()) {
			MsgStr = "Reset X motor FAILED";
			Message(pList, MsgStr);
			return FALSE;
		}
		MsgStr = "Return to calibration position";
		Message(pList, MsgStr);
		GotoXYS(Global.DPPos.X, Global.DPPos.Y, CCamBase::ECHOCAM);
	}

	Global.DPZPos = 0;
	Global.DPDistance = 0;

	float fThkT, fThkB;
	short NAve = 0;
	BOOL bOK = FALSE;

	short nTrial = 32;
	while (nTrial--) {
		if (Global.bAborting) {
			break;
		}
		CPeaks Pk;
		NAve = 16;
		pEcho->fCalibProbeDistance[0] = pEcho->fCalibProbeDistance[1] = 0;
		if (pRcp->bEchoPeakMonitor && pVideo) {
			if (pEcho->GetPeaks(pRcp, Pk, NAve, pVideo, pList, NULL, MeSet, "DPCal", pParam) != CEchoProbeBase::GPALLHOK) {
				continue;
			}
		}
		else {
			if (pEcho->GetPeaks(pRcp, Pk, NAve, NULL, pList, NULL, MeSet, "", pParam) != CEchoProbeBase::GPALLHOK) {
				continue;
			}
		}
		if (pEcho->Cnt1 < NAve / 3) {
			continue;
		}
		if (pEcho->Cnt2 < NAve / 3) {
			continue;
		}

		fThkT = (Pk.fCM[TOPPRB][1]);
		fThkB = (Pk.fCM[BOTPRB][1]);
		pEcho->fCalibProbeDistance[TOPPRB] = fRemoveFlyer(pEcho->Cnt1, pEcho->fPeak0[TOPPRB], 0, 1);	// Top first peak position [5/3/2013 user]
		pEcho->fCalibProbeDistance[BOTPRB] = fRemoveFlyer(pEcho->Cnt2, pEcho->fPeak0[BOTPRB], 0, 1);	// Bottom first peak position [5/3/2013 user]

		int a;
		float aTTk = 0;
		if (NAve > 0) {
			for (int a = 0; a < NAve; a++) {
				pEcho->fPeak0[TOPPRB][a] = (pEcho->fPeak0[TOPPRB][a] + pEcho->fPeak0[BOTPRB][a]);
			}
			aTTk = fRemoveFlyer(NAve, pEcho->fPeak0[TOPPRB], 0, 1);
		}
		else {
			continue;
		}

		Global.DPDistance = (aTTk + CalibThickness); //  [5/2/2013 user]
		//Global.DPDistance += ((ThkT + ThkB) + CalibThickness);

		// dCalibProbeDistance is now active [5/3/2013 user]
		if (pEcho->GetPeaks(pRcp, Pk, NAve, NULL, pList, NULL, MeSet, "", pParam) <= CEchoProbeBase::GPPKTOTALERR) {
			continue;
		}

		if (NAve > pEcho->Cnt1) {
			NAve = pEcho->Cnt1;
		}
		if (NAve > pEcho->Cnt2) {
			NAve = pEcho->Cnt2;
		}

		aTTk = 0;
		if (NAve > 0) {
			for (a = 0; a < NAve; a++) {
				pEcho->fPeak0[TOPPRB][a] = (pEcho->fPeak0[TOPPRB][a] + pEcho->fPeak0[BOTPRB][a]);
			}
			aTTk = fRemoveFlyer(NAve, pEcho->fPeak0[TOPPRB], 0, 1);
		}
		else {
			continue;
		}

		MsgStr.Format("TT=%.3f", aTTk);
		Message(pList, MsgStr);

		if (fabs(aTTk) < 0.20f) {
			bOK = TRUE;
			break;
		}
	}
	pEcho->SimuPkNum = SimuPkBak;
	if (bOK) {
		Global.DPDistance /= NAve;
		Global.DPZPos = DPZPos * 1000;	// Convert to micron [3/28/2013 Yuen]
		Global.DPZOff = 0;
		MsgStr.Format("Calibration: T=%.3f  B=%.3f DPZOff=%.4f DPZPos=%.4f", pEcho->fCalibProbeDistance[TOPPRB], pEcho->fCalibProbeDistance[BOTPRB], Global.DPZOff, Global.DPZPos);
		Message(pList, MsgStr);
		return TRUE;
	}
	else {
		Global.DPZPos = 0;
		Global.DPZOff = 0;
		Global.DPDistance = 0;
		if (pList) {
			MsgStr = "Failed calibration";
			Message(pList, MsgStr);
		}
	}
	return FALSE;
}

BOOL C413AppEcho::GetSubstrateThicknesses(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList, CSICam* pVideo) {
	if (!pRcp->IsValidMeSet(pMP)) {
		MsgStr = "Invalid MeSet";
		return FALSE;
	}

	int i;
	BOOL bIsStress = pRcp->IsStressMeasurement();

	if (pEcho->bSimu) {
		if (pRcp->IsEchoBowWarpMeasurement(pMP)) {
			DefinePeakPosition1(pMP, 0);
		}
	}

	if (pRcp->bEnableAutoZ) {
		// Adjust Z height [6/1/2011 XPMUser]
	}

	if (pVideo) {
		pVideo->SetAve(NULL);
	}
	// Start echo capture [3/11/2010 Valued Customer]
	pEcho->SetRecipe(pRcp);

	CEchoProbeBase::GETPEAKSERR ret = CEchoProbeBase::GPPKTOTALERR;

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];
	CString Name = "";
	if (pRcp->bEchoPeakMonitor && pVideo) {
		Name.Format("%03d", pMP->Co.n);
	}

	ret = pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, pVideo, pList, pMeP->MPa, pMP->MeSet, Name);
	if (pRcp->bAllOrNothing) {
		if (ret != CEchoProbeBase::GPALLHOK) {
			Message(pList, MsgStr);	// Show return message from GetPeaks [9/30/2012 Yuen]
			// Perform search around [5/11/2011 FSMT]
			MsgStr.Format("GPS %d", ret);
			Message(pList, MsgStr);
			return FALSE;
		}
	}
	else {
		if (ret <= CEchoProbeBase::GPPKTOTALERR) {
			Message(pList, MsgStr);	// Show return message from GetPeaks [9/30/2012 Yuen]
			// Perform search around [5/11/2011 FSMT]
			MsgStr.Format("GPS %d", ret);
			Message(pList, MsgStr);
			return FALSE;
		}
	}

	// At end of measurement
	// load result to d [3/11/2010 Valued Customer]
	BOOL bAllOK = TRUE;
	// post data collection calculation [3/11/2010 Valued Customer]
	for (i = 0; i < MaXPA; i++) {
		if (*(code + i) >= 1) {
			// done measurement [11/5/2011 Administrator]
			continue;
		}
		CString str;
		MParam* p = &pMeP->MPa[i];
		MParam::PRBNUM Pb = p->Prb;
		if (Pb > 0) {
			float thkn = -1.0f;
			if (p->DF) {
				// if default value, simply assigned it [7/16/2010 C2C]
				p->fThkn = p->DF;
				MsgStr.Format("%s at (%s) set to default %.3f", p->Name, pMP->Co.GetXYStr(), p->DF);
				Message(pList, MsgStr);
				*(code + i) = 1;	// OK [5/31/2012 Yuen]
			}
			else if (p->IsTotalThicknessMeasurement()) {
				// Dual Probe measurement [3/10/2010 Valued Customer]
				float ThkT = 0;
				if ((Pk.Total[TOPPRB] > 1) && (Pk.Total[BOTPRB] > 1)) {
					short nAve = pMeP->NAve;
					if (nAve > pEcho->Cnt1) {
						nAve = pEcho->Cnt1;
					}
					if (nAve > pEcho->Cnt2) {
						nAve = pEcho->Cnt2;
					}
					for (int a = 0; a < nAve; a++) {
						pEcho->fPeak0[TOPPRB][a] = (pEcho->fPeak0[TOPPRB][a]) + (pEcho->fPeak0[BOTPRB][a]);
					}
					if (nAve > 0) {
						ThkT = fRemoveFlyer(nAve, pEcho->fPeak0[TOPPRB], 0, 1);
						ThkT += Global.DPGageThickness + Global.DPZOff;
					}
					else {
						MsgStr.Format("%s at (%s) has no value", p->Name, pMP->Co.GetXYStr());
						Message(pList, MsgStr);
						ThkT = -1;
					}
				}
				else {
					MsgStr.Format("%s at (%s) has not enough peaks", p->Name, pMP->Co.GetXYStr());
					Message(pList, MsgStr);
					ThkT = -1;
				}
				if (ThkT != -1) {
					MsgStr.Format("%s (%s) TT=%.3f  DPZOff=%.4f", p->Name, pMP->Co.GetXYStr(), ThkT, Global.DPZOff);
					Message(pList, MsgStr);
					thkn = ThkT + p->Off;
				}
				else {
					thkn = -1;
				}
				if (p->Lo && p->Hi) {
					if (thkn < p->Lo) {
						p->fThkn = BADDATANEG;
						MsgStr.Format("%s[%.3f] at (%s) did not pass Lo limit %.3f", p->Name, thkn, pMP->Co.GetXYStr(), p->Lo);
						Message(pList, MsgStr);
						bAllOK = FALSE;
					}
					else if (thkn > p->Hi) {
						p->fThkn = BADDATANEG;
						bAllOK = FALSE;
						MsgStr.Format("%s[%.3f] at (%s) did not pass Hi limit %.3f", p->Name, thkn, pMP->Co.GetXYStr(), p->Hi);
						Message(pList, MsgStr);
					}
					else {
						p->fThkn = thkn;
						*(code + i) = 1;
					}
				}
				else {
					if (thkn < 0) {
						thkn = -1;
					}
					else {
						*(code + i) = 1;
					}
					p->fThkn = thkn;
				}
			}
			else if (p->IsThicknessMeasurement()) {
				if ((Pb == MParam::ECHO1PRB) || (Pb == MParam::ECHO1AFPRB)) {
					// Single probe measurement [3/10/2010 Valued Customer]
					float fTh = pEcho->fGetThickness(0, p->Fr - 1, p->To - 1);
					if (p->Fr == 1) {
						// Reference measurement [5/6/2013 user]
						if (Pk.Total[0] > 1) {
							thkn = fTh + p->Off + pMP->baseline;
						}
						else {
							MsgStr.Format("%s[%.3f] at (%s) has not enough peaks", p->Name, thkn, pMP->Co.GetXYStr());
							Message(pList, MsgStr);
							thkn = -1;
						}
					}
					else {
						// Thickness measurement [5/6/2013 user]
						if (Pk.Total[0] >= p->To) {
							if (p->RI != 1) {
								thkn = fTh * cfTop / (p->RI * TRiCorr) + p->Off;
							}
							else {
								thkn = fTh * cfTop + p->Off;
							}
						}
						else {
							MsgStr.Format("%s[%.3f] at (%s) has not enough peaks", p->Name, thkn, pMP->Co.GetXYStr());
							Message(pList, MsgStr);
							thkn = -1;
						}
					}
				}
				else if ((Pb == MParam::ECHO2PRB) || (Pb == MParam::ECHO2AFPRB)) {
					// Single probe measurement [3/10/2010 Valued Customer]
					float fTh = pEcho->fGetThickness(1, p->Fr - 1, p->To - 1);
					if (p->Fr == 1) {
						if (Pk.Total[1] > 1) {
							thkn = fTh + p->Off - pMP->baseline;
						}
						else {
							MsgStr.Format("%s at (%s) has not enough peaks", p->Name, pMP->Co.GetXYStr());
							Message(pList, MsgStr);
							thkn = -1;
						}
					}
					else {
						if (Pk.Total[1] >= p->To) {
							if (p->RI != 1) {
								thkn = fTh * cfBot / (p->RI * BRiCorr) + p->Off;
							}
							else {
								thkn = fTh * cfBot + p->Off;
							}
						}
						else {
							MsgStr.Format("%s at (%s) has not enough peaks", p->Name, pMP->Co.GetXYStr());
							Message(pList, MsgStr);
							thkn = -1;
						}
					}
				}

				// Condition measured value [7/16/2010 C2C]
				if (p->Lo && p->Hi) {
					if (thkn < p->Lo) {
						MsgStr.Format("%s[%.3f] at (%s) did not pass Lo limit %.3f", p->Name, thkn, pMP->Co.GetXYStr(), p->Lo);
						Message(pList, MsgStr);
						p->fThkn = BADDATANEG;
						bAllOK = FALSE;
						return FALSE; // 011724 MASUM - Total Thickness when perform search around
					}
					else if (thkn > p->Hi) {
						MsgStr.Format("%s[%.3f] at (%s) did not pass Hi limit %.3f", p->Name, thkn, pMP->Co.GetXYStr(), p->Hi);
						Message(pList, MsgStr);
						p->fThkn = BADDATANEG;
						bAllOK = FALSE;
						return FALSE;  // 011724 MASUM - Total Thickness when perform search around 
					}
					else {
						p->fThkn = thkn;
						*(code + i) = 1;
					}
				}
				else {
					if (thkn < 0) {
						thkn = -1;
						bAllOK = FALSE;	// Make an option for retry [3/29/2012 C2C]
					}
					else {
						p->fThkn = thkn;
						*(code + i) = 1;
					}
				}
			}
			else if (p->IsEchoBowWarpMeasurement()) {
				p->fThkn = GetBowWarpRef(Pk, p, pMP, pList);
				*(code + i) = 1;
				bAllOK = TRUE;
				if (p->fThkn < 0) {
					*(code + i) = 0;
					bAllOK = FALSE;
				}
			}
			else if (Pb == MParam::STRESSPRB) {
				p->fThkn = GetTopProbeRef(Pk, p, pMP, pList);
				*(code + i) = 1;
				bAllOK = TRUE;
				if (p->fThkn < 0) {
					*(code + i) = 0;
					bAllOK = FALSE;
				}
			}
			else if (Pb == MParam::INTENSI1PRB) {
				p->fThkn = Pk.Inten[0][p->Fr - 1];
				*(code + i) = 1;
			}
			else if (Pb == MParam::INTENSI2PRB) {
				p->fThkn = Pk.Inten[1][p->Fr - 1];
				*(code + i) = 1;
			}
			else if (Pb == MParam::PPOS1PRB) {
				p->fThkn = Pk.fCM[0][p->Fr];
				*(code + i) = 1;
			}
			else if (Pb == MParam::PPOS2PRB) {
				p->fThkn = Pk.fCM[1][p->Fr];
				*(code + i) = 1;
			}
			else if (Pb == MParam::SHAPEFAC1PRB) {
				p->fThkn = Pk.fMiddle[0][p->Fr - 1] - Pk.fCM[0][p->Fr - 1];
				*(code + i) = 1;
			}
			else if (Pb == MParam::SHAPEFAC2PRB) {
				p->fThkn = Pk.fMiddle[1][p->Fr - 1] - Pk.fCM[1][p->Fr - 1];
				*(code + i) = 1;
			}
			else {
				// No entry [11/5/2011 Administrator]
				*(code + i) = 2;
			}
		}
		else {
			// No entry [11/5/2011 Administrator]
			*(code + i) = 2;
		}
	}
	if (bAllOK || bLast) {
		CData* d = pMP->GetData(0, TRUE);
		if (!d) {
			return FALSE;
		}
		// Initialize to invalid value [8/16/2012 Administrator]
		if (bIsStress) {
			for (i = 0; i < MaXPA; i++) {
				if ((pMeP->MPa[i].Prb > 0) && (*(code + i) == 1)) {
					d->Set(MeSetX + i + pRcp->ScanNo, pMeP->MPa[i].fThkn);
				}
			}
		}
		else {
			for (i = 0; i < MaXPA; i++) {
				if ((pMeP->MPa[i].Prb > 0) && (*(code + i) == 1)) {
					d->Set(MeSetX + i, pMeP->MPa[i].fThkn);
				}
			}
		}
		if (pVideo) {
			if (pRcp->bSaveVideo) {
				pVideo->SetAve(d->DVal);
				pVideo->FileBase.Format("%04d.BMP", pMP->Co.n);
				SaveVideo(pVideo, pRcp);
			}
		}
		for (i = 0; i < MaXPA; i++) {
			if (*(code + i) == 0) {
				// Some value did not measure correctly [5/31/2012 Yuen]
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}
void C413AppEcho::ResetEchoSamplingProperty(CRecipe413* pRcp, CListCtrl* pList) {
	CString str;
	if (Global.bAborting) {
		str = "Aborting, not perform ResetEchoSamplingProperty";
		Message(pList, str);
		return;
	}
	if (!pRcp) {
		ASSERT(0);
		return;
	}

	str = "Reload sampling parameters";
	Message(pList, str);

	if (pRcp->bUseMotorFile) {
		EPMAX_RATE = pRcp->SamplingFreq;
		NUMBER_OF_INTEREST_DATASETS = pRcp->NumberOfInterestedData;
		pEcho->ReAllocMemory(NUMBER_OF_INTEREST_DATASETS);
		pDMC->Reload(pRcp->MotorFile);

		str.Format("Values: %d  %d  %s", EPMAX_RATE, NUMBER_OF_INTEREST_DATASETS, pRcp->MotorFile);
		Message(pList, str);
		//OutputDebugString("~~~~~~ DOWNLOAD RECIPE PARAMETER");
	}
	else {
		EPMAX_RATE = DEFFREQ;
		NUMBER_OF_INTEREST_DATASETS = DEFDATASET;
		pEcho->ReAllocMemory(NUMBER_OF_INTEREST_DATASETS);
		pDMC->Reload("MOTOR.TXT");

		str.Format("Values (Default): %d  %d  %s", EPMAX_RATE, NUMBER_OF_INTEREST_DATASETS, pRcp->MotorFile);
		Message(pList, str);
		//OutputDebugString("~~~~~~ DOWNLOAD DEFAULT PARAMETER");
	}
}

float C413AppEcho::GetBowWarpRef(CPeaks& Pk, MParam* p, CMPoint* pMP, CListCtrl* pList) {
	float thkn = BADDATANEG;
	if (Pk.Total[0] > 1) {
		if (p->IsTopProbe()) {
			thkn = Pk.fCM[0][1] + p->Off + pMP->baseline;
		}
		else {
			thkn = Pk.fCM[1][1] + p->Off - pMP->baseline;
		}
	}
	else {
		MsgStr.Format("%s at (%s) has not enough peaks", p->Name, pMP->Co.GetXYStr());
		Message(pList, MsgStr);
		return thkn;
	}

	// Condition measured value [7/16/2010 C2C]
	if (p->Lo && p->Hi) {
		if (thkn < p->Lo) {
			MsgStr.Format("%s[%.3f] at (%s) did not pass Lo limit %.3f", p->Name, thkn, pMP->Co.GetXYStr(), p->Lo);
			Message(pList, MsgStr);
			return BADDATANEG;
		}
		else if (thkn > p->Hi) {
			MsgStr.Format("%s[%.3f] at (%s) did not pass Hi limit %.3f", p->Name, thkn, pMP->Co.GetXYStr(), p->Hi);
			Message(pList, MsgStr);
			return BADDATANEG;
		}
	}
	else {
		if (thkn < 0) {
			return BADDATANEG;
		}
	}
	return thkn;
}
float C413AppEcho::GetTopProbeRef(CPeaks& Pk, MParam* p, CMPoint* pMP, CListCtrl* pList) {
	float thkn = BADDATANEG;

	if (Pk.Total[0] > 1) {
		thkn = Pk.fCM[0][1] + p->Off;
	}
	else {
		MsgStr.Format("%s at (%s) has not enough peaks", p->Name, pMP->Co.GetXYStr());
		Message(pList, MsgStr);
		return BADDATANEG;
	}

	// Condition measured value [7/16/2010 C2C]
	if (p->Lo && p->Hi) {
		if (thkn < p->Lo) {
			MsgStr.Format("%s[%.3f] at (%s) did not pass Lo limit %.3f", p->Name, thkn, pMP->Co.GetXYStr(), p->Lo);
			Message(pList, MsgStr);
			return BADDATANEG;
		}
		else if (thkn > p->Hi) {
			MsgStr.Format("%s[%.3f] at (%s) did not pass Hi limit %.3f", p->Name, thkn, pMP->Co.GetXYStr(), p->Hi);
			Message(pList, MsgStr);
			return BADDATANEG;
		}
	}
	else {
		if (thkn < 0) {
			return BADDATANEG;
		}
	}

	return thkn;
}

BOOL C413AppEcho::WaferOnStageCheck(CRecipe413* pRcp, bool* waferOnStage, CSICam* pVideo, CListCtrl* pList) {
	pPSwitch->MoveToECHOprb();
	
	pDMC->Run();
	pEcho->StartGetPeaks(NULL);

	CPeaks Pk;
	short nAve = 8;
	DWORD tick = GetTickCount64();
	while (!pEcho->bSignal && GetTickCount64() - tick < 30000 && Pk.Total[1] < 1) {
		Global.bAborting = 0;
		pEcho->GetPeaks(pRcp, Pk, nAve, pVideo, pList, NULL, 1, "");
		Yld(20);
	}

	if (Pk.Total[1] > 1) {
		Log(_T("Bottom got more than 1 peak, there is a wafer"));
		*waferOnStage = true;
	}
	else *waferOnStage = false;
	pDMC->Stop();
	return TRUE;
}

void C413AppEcho::AutoFocusEcho(CRecipe413* pRcp, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, float fReff, CImgWnd* pImage) {
	if (!(Global.Confg.bEchoMicroscopeMod || Global.Confg.bThinFilmMicroscopeMod)) {
		return;
	}
	if (!(Global.Confg.bEchoTopMotorMod || Global.Confg.bThinFilmMotorMod)) {
		return;
	}

	float z1, z2, z1b, z2b;
	pMtr->GetZPosDev(z1, z2);
	z1b = z1;
	z2b = z2;

	short cnt = 100;
	while (cnt--) {
		CPeaks Pk;
		if (pRcp->bEchoPeakMonitor && pVideo) {
			if (pEcho->GetPeaks(pRcp, Pk, 8, pVideo, NULL, NULL, 1, "FOCUS") <= CEchoProbeBase::GPPKTOTALERR) {
				cnt++;
				continue;
			}
		}
		else {
			if (pEcho->GetPeaks(pRcp, Pk, 8, NULL, NULL, NULL, 1, "") <= CEchoProbeBase::GPPKTOTALERR) {
				cnt++;
				continue;
			}
		}

		// Always echo camera [8/24/2011 FSMT]
		float fFinal = 6.0f;	// move within 6 microns [5/2/2013 user]
		float fRefNow = pEcho->fGetThickness(0, 0, 1);
		float dZ = fRefNow - fReff;		// unit is micron [5/2/2013 user]
		if (fabs(dZ) < fFinal) {
			return;
		}
		else if (fabs(dZ) < (5 * fFinal)) {
			pMtr->MoveREchoTopZ(dZ * 0.5f * 1e-3);
		}
		else {
			pMtr->MoveREchoTopZ(dZ * 1e-3);
		}
		// For simulation use only
		if (pMtr->pMEchoTopZ->IsSimu()) {
			// go through the loop once than return under simulation [4/21/2013 Administrator]
			return;
		}
	}
}

BOOL C413AppEcho::GetDMCStatus() {
	if (pDMC) {
		return pDMC->bRun;
	}
	return FALSE;
}