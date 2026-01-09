#include "stdafx.h"

#include "413/EchoScope/EchoProbeBase.h"
#include "413/MotorS2.h"
#include "Data.h"
#include "MPoint.h"
#include "PeakParam.h"
#include "ThkValues.h"

#include "413AppStep.h"

C413AppStep::C413AppStep() {}

C413AppStep::~C413AppStep() {}

// TSMC implementation [6/3/2013 Yuen]
// Trim height measurement [6/9/2014 Yuen]
BOOL C413AppStep::MeasureStepHeight(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, short kI, CImgWnd* pImgWnd, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}

	MsgStr = "MeasureStepHeight";
	Message(pList, MsgStr);

	pEcho->SetRecipe(pRcp);

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];
	MParam* pM = &pMeP->MPa[kI];

	// Change radius to trim edge [6/13/2014 Yuen]
	float angle, radius;
	Global.Cartesian2Polar(pMP->Co.x, pMP->Co.y, radius, angle);	// Retrieve radius and angle information [4/14/2013 Administrator]

	if (pRcp->bLocateEdgeStep) {
		radius = GetRadiusOfInnerEdge(pRcp, pMP, pVideo, pImgWnd, pList);	// Radius information is updated [6/30/2014 Yuen]
	}
	Global.Polar2Cartesian(radius, angle, pMP->Co.x, pMP->Co.y);	// Update pMP with new coordinate [6/30/2014 Yuen]

	// Get ready to measure with modified pMP [6/30/2014 C2C]
	if (pMtr) {
		MsgStr.Format("Move to fEchoTopZ+TrimHeightZOff: %.3f", pRcp->fEchoTopZ + pRcp->TrimHeightZOffset);
		Message(pList, MsgStr);
		pMtr->GotoXYZ1(pMP->Co.x, pMP->Co.y, pRcp->fEchoTopZ + pRcp->TrimHeightZOffset);
	}

	short success = 0;
	// High point [8/21/2011 FSMT]
	CThkValues thk1, thk2;
	float GageThickness = Global.DPGageThickness;
	short SimuPkBak = pEcho->SimuPkNum;
	pEcho->SimuPkNum = 6;

	MsgStr = "Measure outer trim area";
	Message(pList, MsgStr);

	CPeaks Pk;
	if (!GetStepHeightOuter(thk1, &GageThickness, pRcp, pM, pMP, pMeP, radius, angle, Pk, pVideo, pList)) // Outer step [6/29/2011 XPMUser]
	{
		MsgStr = "Fail measure outer area";
		Message(pList, MsgStr);
	}
	else {
		success++;
	}
	pEcho->SimuPkNum = 3;
	MsgStr = "Measure inner area";
	Message(pList, MsgStr);
	if (!GetStepHeightInner(thk2, GageThickness, pRcp, pM, pMP, pMeP, radius, angle, Pk, pVideo, pList)) // Inner step [6/29/2011 XPMUser]
	{
		MsgStr = "Fail measure inner area";
		Message(pList, MsgStr);
	}
	else {
		success++;
	}
	pEcho->SimuPkNum = SimuPkBak;

	// Save results [8/26/2011 FSMT]
	MsgStr = "Calculating results";
	Message(pList, MsgStr);
	CData* d = pMP->GetData(0, TRUE);
	if (!d) {
		return FALSE;
	}
	MParam* pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	if (thk1.Thk != BADDATANEG) {
		d->Set(MeSetX + kI++, thk1.Thk + pM1->Off);  // Outer thickness [8/30/2011 C2C]
	}
	else {
		d->Set(MeSetX + kI++, BADDATANEG);  // Outer thickness [8/30/2011 C2C]
	}
	pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	if (thk1.TThk != BADDATANEG) {
		d->Set(MeSetX + kI++, thk1.TThk + pM1->Off); // Outer total [8/30/2011 C2C]
	}
	else {
		d->Set(MeSetX + kI++, BADDATANEG); // Outer total [8/30/2011 C2C]
	}

	pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	if (thk2.Thk != BADDATANEG) {
		d->Set(MeSetX + kI++, thk2.Thk + pM1->Off);  // Inner thickness
	}
	else {
		d->Set(MeSetX + kI++, BADDATANEG);  // Inner thickness
	}
	pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	if (thk2.TThk != BADDATANEG) {
		d->Set(MeSetX + kI++, thk2.TThk + pM1->Off); // Inner total
	}
	else {
		d->Set(MeSetX + kI++, BADDATANEG); // Inner total
	}

	pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	if ((thk2.Thk != BADDATANEG) && (thk1.Thk != BADDATANEG)) {
		d->Set(MeSetX + kI++, (thk2.Thk - thk1.Thk) + pM1->Off); // TD using thickness [8/30/2011 C2C]
	}
	else {
		d->Set(MeSetX + kI++, BADDATANEG); // TD using thickness [8/30/2011 C2C]
	}
	pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	if ((thk2.TThk != BADDATANEG) && (thk1.Thk != BADDATANEG)) {
		d->Set(MeSetX + kI++, (thk2.TThk - thk1.Thk) + pM1->Off); // TD using total thickness [8/30/2011 C2C]
	}
	else {
		d->Set(MeSetX + kI++, BADDATANEG); // TD using total thickness [8/30/2011 C2C]
	}

	pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	if (thk1.Dist > 0 && thk2.Dist > 0) {
		d->Set(MeSetX + kI, (thk1.Dist - thk2.Dist) + pM1->Off);  // TD using distance [8/30/2011 C2C]
	}
	else {
		d->Set(MeSetX + kI, BADDATANEG);
	}
	pMP->ProgressSM = CMPoint::COMPLETED;
	return TRUE;
}

// Use by ASE for Trench depth measurement [6/5/2014 fsm]
BOOL C413AppStep::MeasureStepHeightG(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, int kI, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}

	//	if (pMtr) pMtr->MoveEchoTopZ(pRcp->fEchoTopZ+pRcp->TrimHeightZOffset);
	pEcho->SetRecipe(pRcp);
	//	if (pMtr) pMtr->WaitEchoTopZ();		// Just in case Z1 movement is slower [6/29/2011 XPMUser]

	CPeaks Pk;
	float thk1, thk2;
	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];
	MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];

	float x = pMP->Co.x;
	float y = pMP->Co.y;
	if (!Global.bAborting) {
		GetStepHeightG(thk1, pRcp, pM, pMP, pMeP, x, y, Pk, pVideo, pList);
	}
	if (!Global.bAborting) {
		GetStepHeightG(thk2, pRcp, pM, pMP, pMeP, x + pRcp->BumpOffsetX, y + pRcp->BumpOffsetY, Pk, pVideo, pList);
	}
	if (Global.bAborting) {
		return FALSE;
	}

	// Thickness information available, use it [6/29/2011 XPMUser]
	if ((thk1 != BADDATANEG) && (thk2 != BADDATANEG)) {
		if ((pM->Prb == MParam::TRENCHTOPPRB) || (pM->Prb == MParam::TRENCHBOTPRB)) {
			pM->fThkn = (thk1 - thk2) + pMeP->MPa[kI].Off;
		}
		else {
			pM->fThkn = (thk2 - thk1) + pMeP->MPa[kI].Off;
		}
	}
	else {
		pM->fThkn = BADDATANEG;
	}
	CData* d = pMP->GetData(0, TRUE);
	if (!d) {
		return FALSE;
	}
	d->Set(MeSetX + kI, pM->fThkn);

	return TRUE;
}
BOOL C413AppStep::GetStepHeightInner(CThkValues& Thk, float GageThickness, CRecipe413* pRcp, MParam* pM, CMPoint* pMP, CMeParam* pMeP, float radius, float angle, CPeaks& Pk, CSICam* pVideo, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}

	int i;
	float dX, dY;
	CString Name;
	CPeakParam PParam1;

	PParam1.Clear();
	PParam1.nPeakT = 2;
	PParam1.HeightT[0].Set(1, 256);
	PParam1.nPeakB = 3;
	PParam1.HeightB[0].Set(1, 192);
	PParam1.HeightB[1].Set(2, 192);
	PParam1.Method = CPeakParam::cmGREATER;

	for (i = 0; i < pRcp->nTrialEcho; i++) {
		if (Global.bAborting) {
			break;
		}
		// Scan in one line toward radius [8/25/2011 FSMT]
		Global.Polar2Cartesian(radius - pRcp->TrimHeightDistanceInner1 + (i * (pRcp->TrimHeightDistanceInner1 - pRcp->TrimHeightDistanceInner2) / float(3 * pRcp->nTrialEcho + 1)), angle, dX, dY);
		MsgStr.Format("Move to X:%.3f, Y:.3f, fEchoTopZ+TrimHeightZOffset:%.3f", dX, dY, pRcp->fEchoTopZ + pRcp->TrimHeightZOffset);
		Message(pList, MsgStr);
		if (!pMtr->GotoXYZ1(dX, dY, pRcp->fEchoTopZ + pRcp->TrimHeightZOffset)) {
			return FALSE;
		}
		pMtr->GetXYPos(&dX, &dY);
		MsgStr.Format("New pos %.3f,%.3f", dX, dY);
		Message(pList, MsgStr);
		// Top probe [8/26/2011 FSMT]
		if (pRcp->bEchoPeakMonitor && pVideo) {
			Name.Format("I%d %d-%d %d", pMP->Co.n, dX * 1000, dY * 1000, i + 1);
		}
		else {
			Name = "";
		}
		Pk.Clear();
		Thk.Dist = Thk.TThk = 0;
		if (pEcho->ChkPeaks(pRcp, Pk, 3, NULL, pMP->MeSet, &PParam1, pVideo, pList, Name) != CEchoProbeBase::GPALLHOK) {
			MsgStr = "Not enough peaks";
			Message(pList, MsgStr);
			continue;
		}
		if (pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, pVideo, pList, NULL, pMP->MeSet, Name, &PParam1) == CEchoProbeBase::GPALLHOK) {
			// found [12/17/2010 C2C]
			Thk.Dist = Pk.fCM[TOPPRB][1] - Pk.fCM[TOPPRB][0];
			if (pM->RI != 1) {
				Thk.Thk = pEcho->fGetThickness(1, 1, 2) * cfBot / (pM->RI * BRiCorr) + pM->Off;
			}
			else {
				Thk.Thk = pEcho->fGetThickness(1, 1, 2) * cfBot + pM->Off;
			}
			if (GageThickness > 0) {
				float sum = 0;
				for (int a = 0; a < pMeP->NAve; a++) {
					pEcho->fPeak0[TOPPRB][a] += pEcho->fPeak0[BOTPRB][a];
					// 					pEcho->fPeak0[TOPPRB][a] = ((pEcho->fCalibProbeDistance[TOPPRB]-pEcho->fPeak0[TOPPRB][a]) +
					// 												(pEcho->fCalibProbeDistance[BOTPRB]-pEcho->fPeak0[BOTPRB][a]));
				}
				if (pMeP->NAve > 0) {
					sum = fRemoveFlyer(pMeP->NAve, pEcho->fPeak0[TOPPRB], 0, 1);
				}
				Thk.TThk = sum + GageThickness /*+ Global.DPZOff*/;
			}
			else {
				Thk.TThk = 0;
			}
			if (pVideo) {
				if (pRcp->bSaveVideo) {
					pVideo->FileBase.Format("IN%04d.BMP", pMP->Co.n);
					SaveVideo(pVideo, pRcp);
				}
			}
			break;
		}
		else {
			Message(pList, MsgStr);
		}
	}

	return TRUE;
}

BOOL C413AppStep::GetStepHeightOuter(CThkValues& Thk, float* pGageThickness, CRecipe413* pRcp, MParam* pM, CMPoint* pMP, CMeParam* pMeP, float radius, float angle, CPeaks& Pk, CSICam* pVideo, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}

	int i, j;
	float dX, dY;
	float rd;
	CString Name;
	CPeakParam PParam1, PParam2, PParam3;
	float aThk = 0;
	float aDst = 0;
	float aTTk = 0;

	short cnt1 = 0, cnt2 = 0, cnt3 = 0;
	float dAng = 0.05f;
	//	BOOL bCalib = FALSE;
	Thk.Dist = Thk.Thk = 0;
	//*pGageThickness = 0;

	PParam1.Clear();
	PParam1.nPeakT = 3;
	PParam1.HeightT[0].Set(1, 256);
	PParam1.HeightT[1].Set(2, 256);

	PParam1.nPeakB = 3;
	PParam1.HeightB[0].Set(1, 256);
	PParam1.HeightB[1].Set(2, 192);

	PParam1.Method = CPeakParam::cmGREATER;

	PParam3.Clear();
	PParam3.nPeakT = 2;
	PParam3.HeightT[0].Set(1, 256);
	PParam3.HeightT[1].Set(2, 256);

	PParam3.nPeakB = 2;
	PParam3.HeightB[0].Set(1, 256);
	PParam3.HeightB[1].Set(2, 192);

	PParam3.Method = CPeakParam::cmGREATER;

	short m1 = 0, m2 = 0;
	for (j = 0; j < MAXPEAKCOUNT; j++) {
		CPeakShape* pS1 = &pMeP->PS1[j];
		CPeakShape* pS2 = &pMeP->PS2[j];
		if (pS1->bUse && pS1->Distance) {
			PParam3.DistanceT[m1++].Set(j, pS1->Distance, pS1->DeltaDistance);
		}
		if (pS2->bUse && pS2->Distance) {
			PParam3.DistanceB[m2++].Set(j, pS2->Distance, pS2->DeltaDistance);
		}
	}

	PParam2.Clear();
	PParam2.nPeakT = 2;
	PParam2.HeightT[0].Set(1, 256);
	PParam2.nPeakB = 2;
	PParam2.HeightB[0].Set(1, 256);
	PParam2.Method = CPeakParam::cmGREATER;

	CPeakParam PParam4;
	PParam4.Clear();
	PParam4.nPeakT = 2;
	//PParam4.HeightT[0].Set(1,256);
	PParam4.nPeakB = 2;
	//PParam4.HeightB[0].Set(1,256);
	PParam4.Method = CPeakParam::cmGREATER;

	DefinePeakPosition6(0);

	for (i = 0; i < pRcp->NMeasureOut; i++) {
		if (Global.bAborting) {
			break;
		}
		MsgStr.Format("Measurement: %d", i + 1);
		Message(pList, MsgStr);

		// Scan in one line toward radius [8/25/2011 FSMT]
		rd = radius + pRcp->TrimHeightDistanceOuter1 + (i * (pRcp->TrimHeightDistanceOuter2 - pRcp->TrimHeightDistanceOuter1) / float(pRcp->NMeasureOut + 1));
		Global.Polar2Cartesian(rd, angle, dX, dY);
		MsgStr.Format("Move to X:%.3f, Y:%.3f, fEchoTopZ+TrimHeightZOffset:%.3f", dX, dY, pRcp->fEchoTopZ + pRcp->TrimHeightZOffset);
		Message(pList, MsgStr);
		if (!pMtr->GotoXYZ1(dX, dY, pRcp->fEchoTopZ + pRcp->TrimHeightZOffset)) {
			return FALSE;
		}
		pMtr->GetXYPos(&dX, &dY);
		MsgStr.Format("New pos %.3f,%.3f", dX, dY);
		Message(pList, MsgStr);

		// Bottom probe, obtain thickness and top probe reference [8/26/2011 FSMT]
		if (pRcp->bEchoPeakMonitor && pVideo) {
			Name.Format("O%d %d-%d %d", pMP->Co.n, dX * 1000, dY * 1000, i + 1);
		}
		else {
			Name = "";
		}
		Thk.Dist = Thk.Thk = 0;
		CEchoProbeBase::GETPEAKSERR ret = CEchoProbeBase::GPPKTOTALERR;
		for (j = 0; j < pRcp->nTrialEcho; j++) {
			if (Global.bAborting) {
				break;
			}
			MsgStr.Format("Echo trial: %d", j + 1);
			Message(pList, MsgStr);

			MsgStr = "";
			Pk.Clear();
			if ((ret = pEcho->ChkPeaks(pRcp, Pk, 1, NULL, pMP->MeSet, &PParam3, pVideo, pList, Name)) <= CEchoProbeBase::GPPKTOTALERR) {
				Message(pList, MsgStr); // Message retrieved from ChkPeaks [6/9/2014 Yuen]
				MsgStr.Format("3 Peaks check failed: %d", ret);
				Message(pList, MsgStr);

				rd -= pRcp->fStepSizeEcho;
				Global.Polar2Cartesian(rd, angle + ((j % 2) ? 0 : dAng), dX, dY);
				if (!pMtr->GotoXY(dX, dY)) {
					return FALSE;
				}
				pMtr->GetXYPos(&dX, &dY);
				MsgStr.Format("New pos %.3f,%.3f", dX, dY);
				Message(pList, MsgStr);
				continue;
			}

			MsgStr = "";
			Pk.Clear();
			CEchoProbeBase::GETPEAKSERR ret2 = pEcho->ChkPeaks(pRcp, Pk, 1, NULL, pMP->MeSet, &PParam2, pVideo, pList, Name); // Check to see whether we have both the reference peak [10/23/2011 Administrator]
			if (ret2 != CEchoProbeBase::GPALLHOK) {
				Message(pList, MsgStr);  // Message from ChkPeaks [6/9/2014 Yuen]
				MsgStr.Format("2 Peaks check failed: %d", ret);
				Message(pList, MsgStr);

				// Search around if both reference peak are out of spec [10/23/2011 Administrator]
				rd -= pRcp->fStepSizeEcho;
				Global.Polar2Cartesian(rd, angle + ((j % 2) ? 0 : dAng), dX, dY);
				if (!pMtr->GotoXY(dX, dY)) {
					return FALSE;
				}
				pMtr->GetXYPos(&dX, &dY);
				MsgStr.Format("New pos %.3f,%.3f", dX, dY);
				Message(pList, MsgStr);
				continue;
			}

			MsgStr = "GetPeaks";
			Message(pList, MsgStr);
			if ((ret = pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, pVideo, pList, NULL, pMP->MeSet, Name, &PParam3)) > CEchoProbeBase::GPPKTOTALERR) {
				MsgStr.Format("GetPeaks return: %d", ret);
				Message(pList, MsgStr);

				int a;
				float fdT = 0;
				// Thickness measurement [10/23/2011 Administrator]
				switch (ret) {
				case CEchoProbeBase::GPTOPHOK:	// Top probe is valid [10/23/2011 Administrator]
					if (pM->RI != 1) {
						fdT = pEcho->fGetThickness(0, 1, 2) * cfTop / (pM->RI * TRiCorr);
					}
					else {
						fdT = pEcho->fGetThickness(0, 1, 2) * cfTop;
					}
					aThk += fdT; cnt2++;
					break;
				case CEchoProbeBase::GPBOTHOK:	// Bottom probe is valid [10/23/2011 Administrator]
					if (pM->RI != 1) {
						fdT = pEcho->fGetThickness(1, 1, 2) * cfBot / (pM->RI * BRiCorr);
					}
					else {
						fdT = pEcho->fGetThickness(1, 1, 2) * cfBot;
					}
					aThk += fdT; cnt2++;
					break;
				case CEchoProbeBase::GPALLHOK:	// Both probes are valid
					if (pM->RI != 1) {
						fdT = ((pEcho->fGetThickness(0, 1, 2) * cfTop / (pM->RI * TRiCorr)) + (pEcho->fGetThickness(1, 2, 1) * cfBot / (pM->RI * BRiCorr))) / 2.0f;
					}
					else {
						fdT = ((pEcho->fGetThickness(0, 1, 2) * cfTop) + (pEcho->fGetThickness(1, 2, 1) * cfBot)) / 2.0f;
					}
					aThk += fdT; cnt2++;
					break;
				}
				// Distance measurement [10/23/2011 Administrator]
				float sum = 0;
				short nAve = pMeP->NAve;
				if (nAve > pEcho->Cnt3TB) {
					nAve = pEcho->Cnt3TB;
				}
				for (a = 0; a < nAve; a++) {
					pEcho->fPeak0[TOPPRB][a] += pEcho->fPeak0[BOTPRB][a];
				}
				float gTT;
				if (nAve > 0) {
					sum = fRemoveFlyer(nAve, pEcho->fPeak0[TOPPRB], 0, 1);
					gTT = (sum + *pGageThickness /*+ pM->Off*/);
					//		if (fabs((sum + *pGageThickness) - fdT) > 5.0)  // This test is valid because the function is specialized [11/6/2011 Administrator]
					//		{
					//			MsgStr.Format("Failed thickness verification: %.2f > 5.00 um  %.2f  %.2f",fabs((sum + *pGageThickness) - fdT),fdT,(sum + *pGageThickness));
					//			Message(pList,MsgStr);
					//			MsgStr="Check calibration";
					//			Message(pList,MsgStr);
					//			// Impossible thickness [11/6/2011 Administrator]
					//			// Search around if both reference peak are out of spec [10/23/2011 Administrator]
					//			rd -= pRcp->fStepSizeEcho;
					//			Global.Polar2Cartesian(rd,angle+((j%2)?0:dAng),dX,dY);
					//			if (!pMtr->GotoXY(dX,dY))
					//			{
					//				return FALSE;
					//			}
					//			pMtr->GetXYPos(&dX,&dY);
					//			MsgStr.Format("New pos %.3f,%.3f",dX,dY);
					//			Message(pList,MsgStr);
					//			continue;
					//		}
					aTTk += gTT; cnt3++;
				}

				float gD = Pk.fCM[TOPPRB][1] - Pk.fCM[0][0];
				aDst += gD; cnt1++;

				if (pVideo) {
					if (pRcp->bSaveVideo) {
						pVideo->FileBase.Format("OT%04d-%d.BMP", pMP->Co.n, i + 1);
						SaveVideo(pVideo, pRcp);
					}
				}
				MsgStr.Format("Accepted, TThk:%.3f  Ave Thk:%.3f  RefT:%.3f", gTT, fdT, gD);
				Message(pList, MsgStr);
				break;
			}
			else {
				Message(pList, MsgStr);	// Show returned message from GetPeaks [9/30/2012 Yuen]
				rd -= pRcp->fStepSizeEcho;
				Global.Polar2Cartesian(rd, angle + ((j % 2) ? 0 : dAng), dX, dY);
				if (!pMtr->GotoXY(dX, dY)) {
					return FALSE;
				}
				pMtr->GetXYPos(&dX, &dY);
				MsgStr.Format("New pos %.3f,%.3f", dX, dY);
				Message(pList, MsgStr);
			}
		}
	}

	Thk.Dist = Thk.Thk = Thk.TThk = 0.0f;

	if (cnt1 > 0) Thk.Dist = aDst / float(cnt1); else Thk.Dist = 0;
	if (cnt2 > 0) Thk.Thk = aThk / float(cnt2) /*+ pM->Off*/; else Thk.Thk = 0;
	if (cnt3 > 0) Thk.TThk = aTTk / float(cnt3) /*+ pM->Off*/; else Thk.TThk = 0;

	return TRUE;
}

// Step height or bump height measurement [9/9/2012 Administrator]
BOOL C413AppStep::GetStepHeightG(float& Thk, CRecipe413* pRcp, MParam* pM, CMPoint* pMP, CMeParam* pMeP, float x, float y, CPeaks& Pk, CSICam* pVideo, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}

	int i;
	CString Name;

	BOOL bFound = FALSE;
	short Prb = 0; // To change according to STEPHEIGHTTPRB or STEPHEIGHTBPRB [8/21/2011 FSMT]
	switch (pM->Prb) {
	case MParam::TRENCHTOPPRB:
	case MParam::BUMPHEIGHTTPRB:
		Prb = 0;
		break;
	case MParam::TRENCHBOTPRB:
	case MParam::BUMPHEIGHTBPRB:
		Prb = 1;
		break;
	default:
		ASSERT(0);
		break;
	}
	for (i = 0; i < pRcp->nTrialEcho; i++) {
		if (Global.bAborting) {
			break;
		}
		Pk.Clear();
		if (!pMtr->GotoXY(x + gPX[i] * pRcp->fStepSizeEcho, y + gPY[i] * pRcp->fStepSizeEcho)) {
			return FALSE;
		}
		if (pRcp->bEchoPeakMonitor && pVideo) {
			Name.Format("GP%d %d-%d %d", pMP->Co.n, x * 1000, y * 1000, i + 1);
		}
		else {
			Name = "";
		}
		if (pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, pVideo, pList, NULL, pMP->MeSet, Name, NULL) > CEchoProbeBase::GPPKTOTALERR) {
			// found [12/17/2010 C2C]
			bFound = TRUE;
			break;
		}
		else {
			Message(pList, MsgStr); // Show message from GetPeaks [9/30/2012 Yuen]
		}
	}
	if (bFound) {
		if (Pk.Total[Prb] >= pM->To) {
			if (pM->Fr == 1) {
				switch (Prb) {
				case 0:
					Thk = pEcho->fGetThickness(Prb, pM->To - 1, pM->Fr - 1) + pM->Off;
					break;
				case 1:
					Thk = pEcho->fGetThickness(Prb, pM->To - 1, pM->Fr - 1) + pM->Off;
					break;
				}
			}
			else {
				switch (Prb) {
				case 0:
					if (pM->RI != 1) {
						Thk = pEcho->fGetThickness(Prb, pM->To - 1, pM->Fr - 1) * cfTop / (pM->RI * TRiCorr) + pM->Off;
					}
					else {
						Thk = pEcho->fGetThickness(Prb, pM->To - 1, pM->Fr - 1) * cfTop + pM->Off;
					}
					break;
				case 1:
					if (pM->RI != 1) {
						Thk = pEcho->fGetThickness(Prb, pM->To - 1, pM->Fr - 1) * cfBot / (pM->RI * BRiCorr) + pM->Off;
					}
					else {
						Thk = pEcho->fGetThickness(Prb, pM->To - 1, pM->Fr - 1) * cfBot + pM->Off;
					}
					break;
				}
			}
		}
	}
	return TRUE;
}

// Input: radius = Rin - TrimHeightDistanceInner [4/15/2013 Administrator]
// Total thickness and REFT measurement only [4/16/2013 Administrator]
// Only process a single MPa [4/15/2013 Administrator]
// GageThickness is required [4/15/2013 Administrator]
BOOL C413AppStep::GetStepHeightThickness(STrimWH& S, CRecipe413* pRcp, CMPoint* pMP, float radius, float angle, float GageThickness, CSICam* pVideo, CListCtrl* pList) {
	short cnt = 0;
	float TThk = 0;
	float Hgt = 0;

	for (int i = 0; i < S.nPt; i++) {
		if (Global.bAborting) {
			break;
		}
		if (GetStepHeightThicknessA(S, pRcp, pMP, radius, angle, GageThickness, pVideo, pList)) {
			TThk += S.TThk;
			Hgt += S.Hgt;
			cnt++;
		}
		radius += 0.01f;
	}

	if (cnt) {
		S.TThk = TThk / float(cnt);
		S.Hgt = Hgt / float(cnt);
		return TRUE;
	}
	return FALSE;
}

BOOL C413AppStep::GetStepHeightThicknessA(STrimWH& S, CRecipe413* pRcp, CMPoint* pMP, float radius, float angle, float GageThickness, CSICam* pVideo, CListCtrl* pList) {
	if ((!pRcp) || (!pMP)) {
		return FALSE;
	}
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}

	int i;
	float dX, dY;
	CString Name;
	CPeakParam PParam1;

	PParam1.Clear();
	PParam1.nPeakT = 2;
	PParam1.HeightT[0].Set(1, 192);
	PParam1.Method = CPeakParam::cmGREATER;

	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	BOOL bRet = FALSE;

	float sf = (pRcp->TrimHeightDistanceInner1 - pRcp->TrimHeightDistanceInner2) / float(3 * pRcp->nTrialTrim + 1);
	for (i = 0; i < pRcp->nTrialTrim; i++) {
		if (Global.bAborting) { break; }

		// Move to location i [4/15/2013 Administrator]
		Global.Polar2Cartesian(radius + i * sf, angle, dX, dY);
		if (pMtr) {
			MsgStr.Format("Move to fEchoTopZ+TrimHeightZOffse %.3f", pRcp->fEchoTopZ + pRcp->TrimHeightZOffset);
			Message(pList, MsgStr);
			if (!pMtr->GotoXYZ1(dX, dY, pRcp->fEchoTopZ + pRcp->TrimHeightZOffset)) {
				return FALSE;
			}
			pMtr->GetXYPos(&dX, &dY);
			MsgStr.Format("New pos %.3f,%.3f", dX, dY);
			Message(pList, MsgStr);
		}

		Name = "";
		if (pRcp->bEchoPeakMonitor && pVideo) {
			Name.Format("I%d %d-%d %d", pMP->Co.n, dX * 1000, dY * 1000, i + 1);
		}

		Pk.Clear();
		// Must have at least 2 peaks each probe with intensity > 192 [4/15/2013 Administrator]
		if (pEcho->ChkPeaks(pRcp, Pk, 3, NULL, pMP->MeSet, &PParam1, pVideo, pList, Name) != CEchoProbeBase::GPALLHOK) {
			MsgStr = "Not enough peaks";
			Message(pList, MsgStr);
			continue;
		}

		// Collect TopRef and TT [4/15/2013 Administrator]
		if (pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, pVideo, pList, NULL, pMP->MeSet, Name, &PParam1) == CEchoProbeBase::GPALLHOK) {
			S.Hgt = Pk.fCM[TOPPRB][1] - Pk.fCM[TOPPRB][0];	// Pk.CM are average values, unit in micron [4/15/2013 Administrator]

			// Calculate total thickness [4/15/2013 Administrator]
			float sum = ((pEcho->fCalibProbeDistance[TOPPRB] - pEcho->fGetThickness(TOPPRB, 0, 1)) +
				(pEcho->fCalibProbeDistance[BOTPRB] - pEcho->fGetThickness(BOTPRB, 0, 1)));
			S.TThk = sum + GageThickness + Global.DPZOff;

			if (pVideo && pRcp->bSaveVideo) {
				pVideo->FileBase.Format("TH%03d-%d.BMP", pMP->Co.n, S.n);
				SaveVideo(pVideo, pRcp);
			}
			bRet = TRUE;
			break; // Done. Break out from nTrial [4/15/2013 Administrator]
		}
	}
	return bRet;
}

// Base on the input point, find inner trim edge and return the radius offset of the edge [6/13/2014 Yuen]
float C413AppStep::GetRadiusOfEdge(CRecipe413* pRcp, BOOL bInner, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	float radius, angle;
	float x = pMP->Co.x;
	float y = pMP->Co.y;

	C413Global* g = &Global;

	g->Cartesian2Polar(x, y, radius, angle);	// Retrieve angle information [4/14/2013 Administrator]

	if (pMtr) {
		//radius = pRcp->TrimWidthInnerRadius;
		g->Polar2Cartesian(radius, angle, x, y);
		MsgStr.Format("GotoXYZ: %.3f,%.3f,%.3f", x, y, pRcp->fEchoTopZ);
		Message(pList, MsgStr);
		pMtr->GotoXYZ1(x, y, pRcp->fEchoTopZ);
	}

	if (bInner) {
		pVideo->LoadSimuDIB(GetInnerEdge2(angle));
		MsgStr.Format("Change exposure to: %.3f", pRcp->TWExposureInner + Global.BaseExpoEcho);
		Message(pList, MsgStr);
		pVideo->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho);	// pRcp->THExposure ??? [6/30/2014 C2C]
	}
	else {
		pVideo->LoadSimuDIB(GetOuterEdge2(angle));
		MsgStr.Format("Change exposure to: %.3f", pRcp->TWExposureOuter + Global.BaseExpoEcho);
		Message(pList, MsgStr);
		pVideo->SnapOneFrame(pRcp->TWExposureOuter + Global.BaseExpoEcho);	// pRcp->THExposure ??? [6/30/2014 C2C]
	}

	pImgWnd->DupStore1(g->StoredDIB);

	float rad = GetEdgeOffset(pRcp->TrimWidthLines, pRcp->TrimWidthSpacing, angle, bInner, pImgWnd);

	if (abs(rad) < 0.5f) {
		// Success, return modified radius [6/13/2014 Yuen]
		g->Polar2Cartesian(radius + rad, angle, x, y);	// Retrieve angle information [4/14/2013 Administrator]
		pMtr->GotoXY(x, y);
		if (bInner) {
			pVideo->LoadSimuDIB(GetInnerEdge2(angle));
			MsgStr.Format("Change exposure to: %.3f", pRcp->TWExposureInner + Global.BaseExpoEcho);
			Message(pList, MsgStr);
			pVideo->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho);	// pRcp->THExposure ??? [6/30/2014 C2C]
		}
		else {
			pVideo->LoadSimuDIB(GetOuterEdge2(angle));
			MsgStr.Format("Change exposure to: %.3f", pRcp->TWExposureOuter + Global.BaseExpoEcho);
			Message(pList, MsgStr);
			pVideo->SnapOneFrame(pRcp->TWExposureOuter + Global.BaseExpoEcho);	// pRcp->THExposure ??? [6/30/2014 C2C]
		}
		pImgWnd->DupStore1(g->StoredDIB);
		float rad1 = GetEdgeOffset(pRcp->TrimWidthLines, pRcp->TrimWidthSpacing, angle, bInner, pImgWnd);
		if (abs(rad1) < 0.01f) {
			radius += rad;
			g->Polar2Cartesian(radius, angle, pMP->Co.x, pMP->Co.y);
			return radius; // return radius + rad not rad1 [6/28/2014 Yuen]
		}
	}
	// Fail, return unmodified radius [6/13/2014 Yuen]
	return radius;
}

// Base on the input point, find inner trim edge and return the radius offset of the edge [6/13/2014 Yuen]
float C413AppStep::GetRadiusOfInnerEdge(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	return GetRadiusOfEdge(pRcp, TRUE, pMP, pVideo, pImgWnd, pList);
}

// Base on the input point, find inner trim edge and return the radius offset of the edge [6/13/2014 Yuen]
float C413AppStep::GetRadiusOfOuterEdge(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	return GetRadiusOfEdge(pRcp, FALSE, pMP, pVideo, pImgWnd, pList);
}