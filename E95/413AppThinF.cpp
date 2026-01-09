#include "stdafx.h"

#include "413/MotorS2.h"
#include "413/OceanOp/OceanBase.h"
#include "413App.h"
#include "Data.h"
#include "GlobalDeviceContainer.h"
#include "MPoint.h"

#include "413AppThinF.h"

C413AppThinF::C413AppThinF() {
	pOceanOp = NULL;
}

C413AppThinF::~C413AppThinF() {}

DWORD WINAPI C413AppThinF::GetThinFilmThread(LPVOID Param) {
	SDoPattrnMatching* p = (SDoPattrnMatching*)Param;
	p->bRetCode = p413App->GetThinFilmThicknesses(*p);
	return 0;
}

// pVideo->bBlt is always false in this function [6/16/2013 Yuen]
BOOL C413AppThinF::GetThinFilmThicknesses(SDoPattrnMatching& Param) {
	// By now, the measurement location is under the probe, red cross hair [2/21/2013 Administrator]
	CMPoint* pMP = Param.pMP;
	CSICam* pVideo = Param.pVideo;
	short ActiveCamera = Param.pVideo->iCID;
	CRecipe413* pRcp = Param.pRcp;

	if (!pRcp) {
		ASSERT(0);
		return FALSE;
	}
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}
	BOOL bAllOK = FALSE;
	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	short ExpBck = 80;
	if (pVideo) {
		ExpBck = pVideo->GetExposure();
		pVideo->SetExposure(pRcp->TFTakeSpecExposure + Global.BaseExpoTF, FALSE);
		pVideo->FileDirc.Format("C:\\TEMP\\%s\\%s-%s-%d\\", CTime::GetCurrentTime().Format("%y%m%d"), Param.CarrierID, Param.RcpeName, Param.SlotID);
	}

	if (pMtr) {
		if (pRcp->TFZOffset) {
			pMtr->MoveRThinFilmZ(pRcp->TFZOffset * 1e-3, FALSE);	// Non blocking move [6/16/2013 Yuen]
		}
		pVideo->bBlt = FALSE;
		pMtr->SetSpectroIllumLight(FALSE);  // Just in case [5/7/2011 FSMT]
		pMtr->SetSpectroSourceLight(TRUE);
		Sleep(750); // wait a while for above changes to take effects [6/16/2013 Yuen]
		pVideo->bBlt = TRUE;
	}

	if (pRcp->TFZOffset) {
		pMtr->WaitThinFilmZ();
	}

	short St = 0;
	BOOL bFirst = TRUE;
	MParam::PRBNUM Pb = MParam::NOPRB;
	// Look at all the MaXPA entry [10/25/2012 FSM413]
	for (int i = 0; i < MaXPA - 2; i++) {
		Pb = pMeP->MPa[i].Prb;
		if ((Pb == MParam::TFILMPRB) || (Pb == MParam::TFILMFASTPRB)) {
			// Single point measurement [9/6/2012 Administrator]
			St = i;
			bAllOK = ExecuteTFILM(pRcp, pVideo, pMeP, i, pMP, ExpBck, bFirst); bFirst = FALSE;
			bAllOK = UpdateResult(bAllOK, St, pMP, MeSetX, pMeP, Pb);
		}
		else if (Pb == MParam::ODSTIPRB) {
			// Specialized OD/STI measurement [9/6/2012 Administrator]
			St = i;
			bAllOK = ExecuteODSTI(pRcp, pVideo, pMeP, i, pMP, ExpBck);
			bAllOK = UpdateResult(bAllOK, St, pMP, MeSetX, pMeP, Pb);
		}
		else if (Pb == MParam::TFILMSPMV) {
			// Single point measurement [6/14/2013 Yuen]
			St = i;
			bAllOK = ExecuteTFUMC(pRcp, pVideo, pMeP, i, pMP, ExpBck, bFirst); bFirst = FALSE;
			bAllOK = UpdateResult(bAllOK, St, pMP, MeSetX, pMeP, Pb);
		}
	}
	if (pMtr) {
		pVideo->bBlt = FALSE;
		pMtr->SetSpectroIllumLight(TRUE);  // Just in case [5/7/2011 FSMT]
		pMtr->SetSpectroSourceLight(FALSE);
		pVideo->bBlt = TRUE;
	}
	if (pVideo) {
		pVideo->SetExposure(ExpBck);
	}

	return bAllOK;
}

BOOL C413AppThinF::TakeReference(CRecipe413* pRcp, CSICam* pVideo, int Exposure1, int Exposure2) {
	BOOL ret = FALSE;
	if (pOceanOp && (pOceanOp->numberOfSpectrometersAttached > 0)) {
		if (pMtr) {
			pMtr->SetSpectroIllumLight(FALSE);
			if (!p413App->GotoXYS(Global.TFPos.X, Global.TFPos.Y, CCamBase::THINFCAM)) {
				return FALSE;
			}
		}
		if (pVideo && (Exposure1 > 0)) {
			pVideo->SetExposure(Exposure1);
		}
		if (pOceanOp->GetReference(pRcp->NSmooth)) {
			ret = TRUE;
		}
		else {
			pGDev->SetAlarm(CAlrmList::TAKEREFERENCEFAIL, "Take reference failed");
		}
		if (pVideo && (Exposure2 > 0)) {
			pVideo->SetExposure(Exposure2);
		}
		if (pMtr) pMtr->SetSpectroIllumLight(TRUE);
	}
	return ret;
}

BOOL C413AppThinF::TakeBackground(CRecipe413* pRcp, CSICam* pVideo, int Exposure1, int Exposure2) {
	BOOL ret = FALSE;
	if (pOceanOp && (pOceanOp->numberOfSpectrometersAttached > 0)) {
		if (pMtr) {
			pMtr->SetSpectroIllumLight(FALSE);
			if (!p413App->GotoXYS(Global.BGPos.X, Global.BGPos.Y, CCamBase::THINFCAM)) {
				return FALSE;
			}
		}
		if (pVideo && (Exposure1 > 0)) {
			pVideo->SetExposure(Exposure1);
		}
		if (pOceanOp->GetBackground(pRcp->NSmooth)) {
			ret = TRUE;
		}
		else {
			pGDev->SetAlarm(CAlrmList::TAKEBACKGROUNDFAIL, "Take background failed");
		}
		if (pVideo && (Exposure2 > 0)) {
			pVideo->SetExposure(Exposure2);
		}
		if (pMtr) pMtr->SetSpectroIllumLight(TRUE);
	}
	return ret;
}

// Good for bckground and reference spectrum only [11/24/2012 Yuen]
BOOL C413AppThinF::OpenSpecFile(CString SpecName, CSpec* pSpec) {
	int cnt = 512;
	//	char str[128];
	float wlen, intn;

	switch (cnt) {
	case 512:
	case 2048:
		pSpec->Alloc(cnt, 0);
		break;
	default:
		return FALSE;
	}

	// Actual loading of spectrum [1/11/2012 Administrator]
	FILE* fp = fopen(SpecName, "rb");
	if (fp) {
		CSpecPoint* pSp = pSpec->pPt;
		for (int i = 0; i < cnt; i++) {
			if (fscanf(fp, "%f\t%f\n", &wlen, &intn) == EOF) break;
			pSp->Set1(wlen, intn); pSp++;
		}
		fclose(fp);
		pSpec->CalcMaxMin();
		return TRUE;
	}
	return FALSE;
}

BOOL C413AppThinF::OpenSpecCSV(CString SpecName, CSpec* pSpec) {
	int cnt = 0;
	char str[256];
	CString str2;
	float wlen, intn;

	// Calculate number of points [1/11/2012 Administrator]
	FILE* fp = fopen(SpecName, "rt");
	if (fp) {
		if (fscanf(fp, "%s\n", str) == 0) {
			fclose(fp);
			return FALSE;
		}
		char* pch;
		pch = strtok(str, ",");
		if (pch) {
			str2 = str;
			pch = strtok(NULL, ",");
			if (pch) {
				cnt = atol(pch);
			}
		}

		switch (cnt) {
		case 512:
		case 1024:
		case 2048:
		case 4096:
		case 8192:
		case 16384:
			pSpec->Alloc(cnt, 0);
			break;
		default:
			fclose(fp);
			return FALSE;
		}
		CSpecPoint* pSp = pSpec->pPt;
		for (int i = 0; i < cnt; i++) {
			if (fscanf(fp, "%f,%f\n", &wlen, &intn) == EOF) break;
			pSp->Set1(wlen, intn); pSp++;
		}
		fclose(fp);
		pSpec->CalcMaxMin();
		return TRUE;
	}
	return FALSE;
}

BOOL C413AppThinF::ExecuteODSTI(CRecipe413* pRcp, CSICam* pVideo, CMeParam* pMeP, short St, CMPoint* pMP, short ExpBck)
// St: Start of ODSTIPRBPRB entry [5/31/2012 Yuen]
{
	int i;
	BOOL bAllOK = TRUE;
	float thkn1, thkn2, GOF1, GOF2, WHH1, WHH2;
	if (!pOceanOp) {
		return FALSE;
	}

	// Start measure first point [9/6/2012 Administrator]
	if (pMtr) {
		pMtr->MoveRX(pRcp->POffsetX[0][0]);
	}

	// First point [11/20/2012 Yuen]
	TParam* pTa = &pMeP->Ta;
	pOceanOp->Counter = pMP->Co.n;
	pOceanOp->SetParam(pTa->Method, 1, pTa->IntegTime, pTa->Boxcar, Global.TFScalingFactor);

	if (pVideo) pVideo->bBlt = TRUE;
	thkn1 = pOceanOp->GetPolyThickness(pMeP, pMeP->MPa[St].Lo, pMeP->MPa[St].Hi, pMeP->MPa[St].RI, (CSpec::SPFILTER)pRcp->FFTFilter[pMP->MeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand);
	GOF1 = pOceanOp->ResultSpectrum.Poly[0].GOF;	// assuming item 1 [11/20/2012 Yuen]
	WHH1 = pOceanOp->ResultSpectrum.Poly[0].WHH;
	//if (pVideo) pVideo->bBlt = FALSE;
	if (pRcp->bEchoPeakMonitor && pVideo) {
		CSingleLock Lock(&pVideo->RsltSpec.CS);
		Lock.Lock();
		pVideo->RsltSpec.Dup(pOceanOp->ResultSpectrum);
		Lock.Unlock();
		pVideo->SetExposure(ExpBck);
		if (pRcp->TFZOffset) {
			if (pMtr) {
				pMtr->MoveRThinFilmZ(-pRcp->TFZOffset * 1e-3);
			}
		}
		pVideo->Index = pMP->Co.n;
		if (pRcp->bSaveVideo) {
			if (pMtr) {
				pMtr->SetSpectroIllumLight(TRUE);  // Just in case [5/7/2011 FSMT]
			}
			pVideo->FileDirc.Format("C:\\TEMP\\%s\\%s-%s-%d\\", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
			pVideo->FileBase.Format("%03d-1.BMP", pMP->Co.n);
			pVideo->bStoreComposite = TRUE;
			while (pVideo->bStoreComposite) {
				Sleep(80);
			}
			DosUtil.RemoveExtension(pVideo->FileName);
			// Call SaveSpectrum without file extension [2/12/2012 Administrator]
			SaveSpectrum(pVideo->FileName);
		}
		else if (pVideo->IsSimu()) {
			// Simulate save delay [7/31/2013 Yuen]
			Sleep(700);
		}
		if (pMtr) {
			if (pRcp->TFZOffset) {
				pMtr->MoveRThinFilmZ(pRcp->TFZOffset * 1e-3);
			}
			pMtr->SetSpectroIllumLight(FALSE);  // Just in case [5/7/2011 FSMT]
		}
	}

	// Start second point [9/6/2012 Administrator]
	if (pMtr) {
		pMtr->MoveRX(pRcp->POffsetX[0][1]);
	}
	if (pRcp->bEchoPeakMonitor && pVideo) {
		pVideo->SetExposure(pRcp->TFTakeSpecExposure + Global.BaseExpoTF);
	}

	// Second point [11/20/2012 Yuen]
	pTa = &pMeP->Ta;
	pOceanOp->SetParam(pTa->Method, 1.0f, pTa->IntegTime, pTa->Boxcar, Global.TFScalingFactor);
	if (pVideo) pVideo->bBlt = TRUE;
	pOceanOp->Counter = pMP->Co.n;
	thkn2 = pOceanOp->GetPolyThickness(pMeP, pMeP->MPa[St].Lo, pMeP->MPa[St].Hi, pMeP->MPa[St].RI, (CSpec::SPFILTER)pRcp->FFTFilter[pMP->MeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand); // should use different thickness range [11/20/2012 Yuen]
	GOF2 = pOceanOp->ResultSpectrum.Poly[0].GOF;
	WHH2 = pOceanOp->ResultSpectrum.Poly[0].WHH;
	//if (pVideo) pVideo->bBlt = FALSE;
	if (pRcp->bEchoPeakMonitor && pVideo) {
		CSingleLock Lock(&pVideo->RsltSpec.CS);
		Lock.Lock();
		pVideo->RsltSpec.Dup(pOceanOp->ResultSpectrum);
		Lock.Unlock();
		pVideo->SetExposure(ExpBck);
		if (pRcp->TFZOffset) {
			if (pMtr) {
				pMtr->MoveRThinFilmZ(-pRcp->TFZOffset * 1e-3);
			}
		}
		//pVideo->TFThick = thkn2;
		pVideo->Index = pMP->Co.n;
		if (pRcp->bSaveVideo) {
			if (pMtr) {
				pMtr->SetSpectroIllumLight(TRUE);  // Just in case [5/7/2011 FSMT]
			}
			pVideo->FileDirc.Format("C:\\TEMP\\%s\\%s-%s-%d\\", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
			pVideo->FileBase.Format("%03d-2.BMP", pMP->Co.n);
			pVideo->bStoreComposite = TRUE;
			while (pVideo->bStoreComposite) {
				Sleep(80);
			}
			DosUtil.RemoveExtension(pVideo->FileName);
			// Call SaveSpectru without file extension [2/12/2012 Administrator]
			SaveSpectrum(pVideo->FileName + ".TXT");
			if (pMtr) {
				pMtr->SetSpectroIllumLight(TRUE);  // Just in case [5/7/2011 FSMT]
			}
		}
		else if (pVideo->IsSimu()) {
			// Simulate save delay [7/31/2013 Yuen]
			Sleep(700);
		}
		if (pVideo) pVideo->bBlt = TRUE;
	}

	i = St;
	if (pMeP->MPa[i].Lo && pMeP->MPa[i].Hi) {
		if (thkn1 < pMeP->MPa[i].Lo) {
			pMeP->MPa[i].fThkn = BADDATANEG;
			bAllOK = FALSE;
		}
		else if (thkn1 > pMeP->MPa[i].Hi) {
			pMeP->MPa[i].fThkn = BADDATANEG;
			bAllOK = FALSE;
		}
		else {
			pMeP->MPa[i].fThkn = thkn1 + pMeP->MPa[i].Off;
		}
		if (thkn2 < pMeP->MPa[i + 1].Lo) {
			pMeP->MPa[i + 1].fThkn = BADDATANEG;
			bAllOK = FALSE;
		}
		else if (thkn2 > pMeP->MPa[i + 1].Hi) {
			pMeP->MPa[i + 1].fThkn = BADDATANEG;
			bAllOK = FALSE;
		}
		else {
			pMeP->MPa[i + 1].fThkn = thkn2 + pMeP->MPa[i].Off;
		}
		pMeP->MPa[i + 2].fThkn = GOF1;
		pMeP->MPa[i + 3].fThkn = WHH1;
		pMeP->MPa[i + 4].fThkn = GOF2;
		pMeP->MPa[i + 5].fThkn = WHH2;
	}
	else {
		pMeP->MPa[i].fThkn = thkn1 + pMeP->MPa[i].Off;
		pMeP->MPa[i + 1].fThkn = thkn2 + pMeP->MPa[i].Off;
		pMeP->MPa[i + 2].fThkn = GOF1;
		pMeP->MPa[i + 3].fThkn = WHH1;
		pMeP->MPa[i + 4].fThkn = GOF2;
		pMeP->MPa[i + 5].fThkn = WHH2;
	}

	return bAllOK;
}

BOOL C413AppThinF::ExecuteTFILM(CRecipe413* pRcp, CSICam* pVideo, CMeParam* pMeP, short i, CMPoint* pMP, short ExpBck, BOOL bFirst) {
	BOOL bAllOK = TRUE;
	float thkn1, GOF1, WHH1;
	if (!pOceanOp) {
		return FALSE;
	}

	TParam* pTa = &pMeP->Ta;
	MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[i];
	if ((pM->Prb == MParam::TFILMPRB) || (pM->Prb == MParam::TFILMFASTPRB)) {
		pOceanOp->SetParam(pTa->Method, 1.0f, pTa->IntegTime, pTa->Boxcar, Global.TFScalingFactor);
		pOceanOp->Counter = pMP->Co.n;
		thkn1 = pOceanOp->GetPolyThickness(pMeP, pMeP->MPa[i].Lo, pMeP->MPa[i].Hi, pMeP->MPa[i].RI, (CSpec::SPFILTER)pRcp->FFTFilter[pMP->MeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand, TRUE);
		thkn1 += pMeP->MPa[i].Off;
		GOF1 = pOceanOp->ResultSpectrum.Poly[0].GOF;
		WHH1 = pOceanOp->ResultSpectrum.Poly[0].WHH;
		if (pRcp->bEchoPeakMonitor && pVideo) {
			pVideo->bBlt = TRUE;	// Must turn it on becase need to save video [8/7/2013 Yuen]
			CSingleLock Lock(&pVideo->RsltSpec.CS);
			pVideo->RsltSpec.Dup(pOceanOp->ResultSpectrum);
			Lock.Unlock();
			pVideo->Index = pMP->Co.n;
			if (pRcp->bSaveVideo) {
				pVideo->SetExposure(ExpBck);
				if (pMtr) {
					pMtr->SetSpectroIllumLight(TRUE);  // Just in case [5/7/2011 FSMT]
					if (pRcp->TFZOffset) {
						// Move Z to picture taking position [11/8/2012 Yuen]
						pMtr->MoveRThinFilmZ(-pRcp->TFZOffset * 1e-3, FALSE);
					}
					Sleep(750);	// Wait till illumination is fully on [8/6/2013 Yuen]
					if (pRcp->TFZOffset) {
						pMtr->WaitThinFilmZ();
					}
				}
				pVideo->FileBase.Format("%03d-%03d.BMP", pMP->Co.n, i + 1);
				pVideo->bStoreComposite = TRUE;
				while (pVideo->bStoreComposite) {
					Sleep(80);
				}
				DosUtil.RemoveExtension(pVideo->FileName);
				// Call SaveSpectru without file extension [2/12/2012 Administrator]
				SaveSpectrum(pVideo->FileName);
				if (pMtr) {
					// Return Z to measuring position [11/8/2012 Yuen]
					if (pRcp->TFZOffset) {
						pMtr->MoveRThinFilmZ(pRcp->TFZOffset * 1e-3, FALSE);
					}
				}
			}
		}
		pMeP->MPa[i].fThkn = thkn1;
		pMeP->MPa[i + 1].fThkn = GOF1;
		pMeP->MPa[i + 2].fThkn = WHH1;
	}
	return bAllOK;
}

BOOL C413AppThinF::UpdateResult(BOOL bAllOK, short St, CMPoint* pMP, short MeSetX, CMeParam* pMeP, MParam::PRBNUM Pb) {
	int i;
	if (bAllOK) {
		CData* d = pMP->GetData(0, TRUE);
		if (!d) {
			return FALSE;
		}

		// All together 6 entries for ODSTIPRB and 3 entries for TFILMPRB [5/31/2012 Yuen]
		if (Pb == MParam::ODSTIPRB) {
			for (i = St; i < St + 6; i++) {
				if (pMeP->MPa[i].fThkn != BADDATANEG) {
					d->Set(MeSetX + i, pMeP->MPa[i].fThkn);
				}
				else {
					d->Set(MeSetX + i, BADDATANEG);  // NA [7/16/2010 C2C]
				}
			}
		}
		else if (Pb == MParam::TFILMPRB) {
			for (i = St; i < St + 3; i++) {
				if (pMeP->MPa[i].fThkn != BADDATANEG) {
					d->Set(MeSetX + i, pMeP->MPa[i].fThkn);
				}
				else {
					d->Set(MeSetX + i, BADDATANEG);  // NA [7/16/2010 C2C]
				}
			}
		}
		else if (Pb == MParam::TFILMFASTPRB)	// To be confirm [2/6/2013 Administrator]
		{
			for (i = St; i < St + 3; i++) {
				if (pMeP->MPa[i].fThkn != BADDATANEG) {
					d->Set(MeSetX + i, pMeP->MPa[i].fThkn);
				}
				else {
					d->Set(MeSetX + i, BADDATANEG);  // NA [7/16/2010 C2C]
				}
			}
		}
		else if (Pb == MParam::TFILMSPMV) {
			for (i = St; i < MaXPA; i++) {
				if (pMeP->MPa[i].fThkn != BADDATANEG) {
					d->Set(MeSetX + i, pMeP->MPa[i].fThkn);
				}
				else {
					d->Set(MeSetX + i, BADDATANEG);  // NA [7/16/2010 C2C]
				}
			}
		}
		pMP->ProgressSM = CMPoint::COMPLETED;
		return TRUE;
	}
	return FALSE;
}

BOOL C413AppThinF::ExecuteTFUMC(CRecipe413* pRcp, CSICam* pVideo, CMeParam* pMeP, short i, CMPoint* pMP, short ExpBck, BOOL bFirst) {
	BOOL bAllOK = TRUE;

	if (!pOceanOp) {
		return FALSE;
	}

	TParam* pTa = &pMeP->Ta;
	MParam* pM = &pMeP->MPa[i];
	if (pM->Prb == MParam::TFILMSPMV) {
		if (pVideo) pVideo->bBlt = TRUE;
		pOceanOp->Counter = pMP->Co.n;
		CPolyParams* pRsl = pOceanOp->GetPolyThicknessB(pTa, (CSpec::SPFILTER)pRcp->FFTFilter[pMP->MeSet - 1], pRcp->nCycle, pRcp->nMappedPoints, pRcp->NSmooth, pRcp->nExpand, p413App->Global.TFScalingFactor, TRUE);
		short n = i;
		for (int k = 0; k < MAXTHKVLU; k++) {
			if ((n + 2) < MaXPA) {
				if (pTa->RI[k] > 0) {
					pMeP->MPa[n++].fThkn = pRsl[k].fThk;
					pMeP->MPa[n++].fThkn = pRsl[k].GOF;
					pMeP->MPa[n++].fThkn = pRsl[k].WHH;
				}
			}
		}
		//if (pVideo) pVideo->bBlt = FALSE;
		if (pRcp->bEchoPeakMonitor && pVideo) {
			CSingleLock Lock(&pVideo->RsltSpec.CS);
			Lock.Lock();
			pVideo->RsltSpec.Dup(pOceanOp->ResultSpectrum);
			Lock.Unlock();
			pVideo->SetExposure(ExpBck);
			if (pRcp->TFZOffset) {
				if (pMtr) {
					// Move Z to picture taking position [11/8/2012 Yuen]
					pMtr->MoveRThinFilmZ(-pRcp->TFZOffset * 1e-3);
				}
			}
			//pVideo->TFThick = thkn1;
			pVideo->Index = pMP->Co.n;
			if (pRcp->bSaveVideo) {
				if (pMtr) {
					pMtr->SetSpectroIllumLight(TRUE);  // Just in case [5/7/2011 FSMT]
					Sleep(750);	// Wait till illumination is fully on [8/6/2013 Yuen]
				}
				pVideo->FileDirc.Format("C:\\TEMP\\%s\\%s-%s-%d\\", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
				pVideo->FileBase.Format("%03d-%d.BMP", pMP->Co.n, i);
				pVideo->bStoreComposite = TRUE;
				while (pVideo->bStoreComposite) {
					Sleep(80);
				}
				DosUtil.RemoveExtension(pVideo->FileName);
				// Call SaveSpectru without file extension [2/12/2012 Administrator]
				SaveSpectrum(pVideo->FileName);
				if (pMtr) {
					// Return Z to measuring position [11/8/2012 Yuen]
					if (pRcp->TFZOffset) {
						pMtr->MoveRThinFilmZ(pRcp->TFZOffset * 1e-3);
					}
				}
			}
		}
	}
	return bAllOK;
}

BOOL C413AppThinF::SaveSpectrum(CString fname) {
	CString finame;
	FILE* f;

	if (pOceanOp->ResultSpectrum.N) {
		finame = fname + "_1.CSV";
		f = fopen(finame, "w");
		if (f) {
			CSpecPoint* pSp = pOceanOp->ResultSpectrum.pPt;
			fprintf(f, "Spectrum,%d\n", pOceanOp->ResultSpectrum.N);
			for (int i = 0; i < pOceanOp->ResultSpectrum.N; i++) {
				fprintf(f, "%.3f,%.3f\n", 1 / pSp->X1, pSp->Y1);
				pSp++;
			}
			fclose(f);
		}
	}

	if (pOceanOp->CurrSpectrum.N) {
		finame = fname + "_2.CSV";
		f = fopen(finame, "w");
		if (f) {
			fprintf(f, "Raw,%d\n", pOceanOp->CurrSpectrum.N);
			CSpecPoint* pSp = pOceanOp->CurrSpectrum.pPt;
			int j;
			for (j = 0; j < pOceanOp->CurrSpectrum.N; j++) {
				fprintf(f, "%.3f,%.3f\n", pSp->X1, pSp->Y1);
				pSp++;
			}
			fclose(f);
		}
	}

	if (pOceanOp->ResultSpectrum.S) {
		finame = fname + "_3.CSV";
		f = fopen(finame, "w");
		if (f) {
			fprintf(f, "FFT,%d\n", pOceanOp->ResultSpectrum.S);
			CFFTPoint* pSf = pOceanOp->ResultSpectrum.pV;
			for (int j = 0; j < pOceanOp->ResultSpectrum.S; j++) {
				fprintf(f, "%d,%.3f\n", j, pSf->V);
				pSf++;
			}
			fclose(f);
		}
	}
	return TRUE;
}