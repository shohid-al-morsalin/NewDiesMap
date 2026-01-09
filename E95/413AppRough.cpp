#include "stdafx.h"
#include "413AppRough.h"
#include "MPoint.h"
#include "RoughProbe.h"
#include "Data.h"
#include "413/MotorS2.h"

//[ Mahedi Kamal - 03202024
BOOL C413AppRough::GetSubstrateRoughness(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		//MsgStr = "Invalid MeSet";
		return FALSE;
	}

	int i;
	BOOL bIsRoughness = pRcp->IsRoughnessMeasurement();

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	BOOL bAllOK = TRUE;
	for (i = 0; i < MaXPA; i++) {
		if (*(code + i) >= 1) {
			// done measurement [11/5/2011 Administrator]
			continue;
		}
		CString str;
		MParam* p = &pMeP->MPa[i];
		MParam::PRBNUM Pb = p->Prb;
		if (Pb > 0) {
			float Rhf = -1.0f;
			int NAver = 64; //need to be changed Mahedi Kamal
			if (Pb == MParam::ROUGHPRB) {
				for (int i = 0; i < 10; i++) {
					pRough->Read(NAver);
				}
				Sleep(500);
				p->fRough = pRough->Read(NAver);
				if (p->Lo && p->Hi) {
					if (p->fRough < p->Lo) {
						MsgStr.Format(_T("%s[%.3f] at (%s) did not pass Lo limit %.3f"), p->Name.GetBuffer(), p->fRough, pMP->Co.GetXYStr().GetBuffer(), p->Lo);
						Message(pList, MsgStr);
					}
					else if (p->fRough > p->Hi) {
						MsgStr.Format(_T("%s[%.3f] at (%s) did not pass Hi limit %.3f"), p->Name.GetBuffer(), p->fRough, pMP->Co.GetXYStr().GetBuffer(), p->Lo);
						Message(pList, MsgStr);
					}
					else {
						*(code + i) = 1;
					}
				}
				else {
					if (p->fRough < 0) {
						p->fRough = -1;
					}
					else {
						*(code + i) = 1;
					}
				}
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
		for (i = 0; i < MaXPA; i++) {
			if ((pMeP->MPa[i].Prb > 0) && (*(code + i) == 1)) {
				d->Set(MeSetX + i, pMeP->MPa[i].fRough);
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
//]

//[ Mahedi Kamal - 03202024
BOOL C413AppRough::PerformRoughnessCalibration(CRecipe413* pRcp, CListCtrl* pList) {
	CString msg;
	msg.Format("Perform Roughness Calibration"); Log(msg);
	if (pList) Message(pList, msg);
	GotoXYS(Global.RoughCalibPos.X, Global.RoughCalibPos.Y, CCamBase::ECHOCAM);
	if (Global.bStop) { msg = ""; return FALSE; }
	pMtr->MoveRoughZ(pRcp->fRoughZ);

	if (Global.bStop) { pMtr->MoveRoughZ(0);  msg = ""; return FALSE; }

	if (!pRough) { msg = ""; ASSERT(0); return FALSE; }
	msg.Format(_T("Roughness calibration # of sampling %d"), 256); Log(msg);
	if (pList) Message(pList, msg);
	pRough->CalibrateRH(256, msg);
	if (pList) Message(pList, msg);
	return TRUE;
}
//]

//[ Mahedi Kamal - 03202024
BOOL C413AppRough::MeasureRoughMPList(CRecipe413* pRcp, CPtrList* pMPList/*, GEM::SGEMCtx* pCtx*/) {
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		if (Global.bStop) break;
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos); if (!p) continue;
		if (!pRcp->IsRoughnessMeasurement(p)) continue;
		if (Global.bStop) return FALSE;
		if (!MeasureOnePointRough(pRcp, p, 64, TRUE)) continue;
	}
	return TRUE;
}
//]


//[ Mahedi Kamal - 03202024
BOOL C413AppRough::MeasureOnePointRough(CRecipe413* pRcp, CMPoint* p, short NAver, BOOL bSettling) {
	if (!pMtr->GotoXY(p->Co.x, p->Co.y)) { AfxMessageBox("GotoXY error", 0); return FALSE; }
	if (Global.bStop) return FALSE;
	return DoOnePointRoughnessMeasurement(pRcp, p, NAver);
}
//]

//[ Mahedi Kamal - 03202024
BOOL C413AppRough::DoOnePointRoughnessMeasurement(CRecipe413* pRcp, CMPoint* p, short NAver) {
	if (NAver < 1) NAver = 1;

	if (Global.bStop) return FALSE;

	CData* d = p->GetData(0, TRUE); if (!d) return FALSE;

	if (pRcp->IsRoughnessMeasurement(p)) {
		AfxMessageBox("Roughness point");
		double fRh;
		for (int i = 0; i < 10; i++) {
			pRough->Read(NAver);
		}
		Sleep(500);
		fRh = pRough->Read(NAver);
		CString res;
		res.Format("rough val is %lf", fRh);
		AfxMessageBox(res);
	}
	return TRUE;
}
//]

//[ Mahedi Kamal - 03202024
void C413AppRough::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CSetupProp15Dlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}
//]

//[ Mahedi Kamal - 03202024
BOOL C413AppRough::GotoRoughnessCalibrationPos() {
	return pMtr->GotoXY(Global.RoughCalibPos.X, Global.RoughCalibPos.Y);
}
//]