#include "stdafx.h"

#include "413/EchoScope/EchoProbeBase.h"
#include "413/MotorS2.h"
#include "413App.h"
#include "ChartWnd.h"
#include "Data.h"
#include "MPoint.h"
#include "PeakParam.h"

#include "413AppTrim.h"

float AveST[MaXPA];

C413AppTrim::C413AppTrim() {
	rnd = 0;
}

C413AppTrim::~C413AppTrim() {}

BOOL C413AppTrim::MeasureBevelWidth(CRecipe413* pRcp, CMPoint* pMP, short kI, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo) {
	int i;
	BOOL bX;

	if (!pImgWnd) {
		ASSERT(0);
		return FALSE;
	}

	//20220616/Haque - New alogrithm for bevel width measurement
	C413Global* g = &Global;	

	float angle = GetAngle(pMP->Co.x, pMP->Co.y);

	pVideo->LoadStockDIB("BEVEL\\BEVEL.BMP");
	pVideo->SnapOneFrame(pRcp->TWExposureBevel + g->BaseExpoEcho);
	pImgWnd->DupStore1(g->StoredDIB);

	short n = pRcp->TrimWidthLines;
	short Offset = pRcp->TrimWidthSpacing;
	float* pfTrimWidth = new float[2 * n + 1];

	CPackedDIB* pDIB = &g->StoredDIB;
	if (!pDIB->pDIB) {
		return FALSE;
	}
	int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * (pDIB->pDIB->BmpInfo.biBitCount / 8);

	short cnt = 0;
	for (i = -n; i <= n; i++) {
		int idx1;
		short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
		short len = pImgWnd->ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX/*,0,255,0*/);
		if (len >= siz) {
			ASSERT(0);
		}
		if (len > 0) {
			CString EV;
			EV.Format("%d", i);
			PreprocessBevelData(pBuf, len, 6, TRUE, EV);
			short left = 0, right = 0;
			float TW = GetBevelWidth(pBuf, len, angle, bX, left, right);
			if ((right > 0) && (right < len) && (left > 0) && (left < len) && right > left) {
				*(pfTrimWidth + (cnt++)) = TW;
				if ((i == 0) && (pProfile)) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					pProfile->Set(pDspBuf, len);
					pProfile->Invalidate();
				}
				short* p = pBuf;
				int j;
				for (j = 0; j < left; j++) {
					*(p++) = 255;
				}
				for (j = left; j <= right; j++) {
					*(p++) = 0;
				}
				for (j = right + 1; j < len; j++) {
					*(p++) = 255;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128;
					*(pBuf + idx1) = 128;
					*(pBuf + idx1 + 1) = 128;
				}
				pImgWnd->ReplaceLine(pBuf, angle, i * Offset, bX);
			}
		}
		if (pBuf) {
			delete[] pBuf;
		}
	}

	float sfx = g->ScrnCalibECH.ScrnX / float(pDIB->pDIB->BmpInfo.biWidth);
	float Ave = fRemoveFlyer(cnt, pfTrimWidth, 0, 1) * sfx / g->ScrnCalibECH.ppmX; // p413App->Global.ScrnCalibECH.mppY;
	delete[] pfTrimWidth;

	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	MParam* p = &pMeP->MPa[kI];
	MParam::PRBNUM Pb = p->Prb;
	if (Pb == MParam::BEVELPRB) {
		//Ave = 1.99f;	// For simulation only [8/15/2012 Administrator]
		p->fThkn = (Ave + pMeP->MPa[kI].Off) * 1e-3;  // Convert unit to mm [6/11/2014 C2C]
		g->BevelWidth = p->fThkn;
	}

	pImgWnd->ClearTextInfo();
	pImgWnd->TWBevel = p->fThkn;
	if (pRcp->bSaveVideo) {
		pImgWnd->bStoreComposite = TRUE;
		pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\Bevel.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
	}

	pImgWnd->Refresh(); // Save image [7/3/2014 FSM 413 C2C]

	CData* d = pMP->GetData(0, TRUE);
	if (!d) {
		return -3;
	}

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	if (pMeP->MPa[kI].Prb == MParam::BEVELPRB) {
		if (pMeP->MPa[kI].fThkn != BADDATANEG) {
			d->Set(MeSetX + kI, pMeP->MPa[kI].fThkn);
		}
		else {
			d->Set(MeSetX + kI, BADDATANEG);  // NA [7/16/2010 C2C]
		}
	}
	return TRUE;
}

BOOL C413AppTrim::MeasureBevelWidth_v1(CRecipe413* pRcp, CMPoint* pMP, short kI, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo) {
	BOOL bX;

	if (!pImgWnd) {
		ASSERT(0);
		return FALSE;
	}

	C413Global* g = &Global;

	float angle = GetAngle(pMP->Co.x, pMP->Co.y);

	pVideo->LoadStockDIB("BEVEL\\BEVEL.BMP");
	pVideo->SnapOneFrame(pRcp->TWExposureBevel + g->BaseExpoEcho);
	pImgWnd->DupStore1(g->StoredDIB);

	CPackedDIB* pDIB = &g->StoredDIB;
	if (!pDIB->pDIB) {
		return FALSE;
	}
	short minLen = short(-1 * pDIB->pDIB->BmpInfo.biHeight * 0.25f);	//biHeight negative
	int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * (pDIB->pDIB->BmpInfo.biBitCount / 8);
	short* pBuf = new short[siz];
	//short* pCrossLine = new short[siz];
	float* pScore = new float[siz];

	short n = pRcp->TrimWidthLines;
	short offset = pRcp->TrimWidthSpacing;
	short cnt = 0;
	float* pfTrimWidth = new float[2 * n + 1];
	if (pBuf)
	{
		short nL = sqrt(pDIB->pDIB->BmpInfo.biHeight * pDIB->pDIB->BmpInfo.biHeight + pDIB->pDIB->BmpInfo.biWidth * pDIB->pDIB->BmpInfo.biWidth) * 0.75;
		//short* lImg = new short[-1 * pDIB->pDIB->BmpInfo.biHeight * pDIB->pDIB->BmpInfo.biWidth];

		//SLnX* pL = new SLnX[nL * 2 + 1];
		float* pS1, * pS = pScore;
		float Offset = 1.0f;	//pixel offsets
		angle += 90.0f;
		if (angle >= 360.0f)
		{
			angle -= 360.0f;
		}
		short i, m = 0;
		//SLnX* pLL;
		int idx1;
		float Smin = 0;
		pS1 = pScore;
		map<int, int>offsetIndex;
		for (i = -nL; i <= nL; i++)
		{
			//pLL = pL + m;
			//pLL->angle = angle;
			//pLL->offset = i * Offset;
			short len = pImgWnd->ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX);
			if (len > minLen)
			{
				offsetIndex[m] = i * Offset;
				*pS = /*pLL->score =*/ pImgWnd->ScoreLine1(pBuf, len);
				if (m)
				{
					if (*pS < Smin)
					{
						Smin = *pS;
					}
				}
				else
				{
					Smin = *pS;
				}
				m++;
				pS++;
			}
		}
		Smin = pRcp->FindBevelThres * Smin;
		pS = pScore;
		for (int i = 0; i < m; i++)
		{
			if (*pS > Smin)
			{
				*pS++ = 255;
			}
			else
			{
				*pS++ = 0;
			}
		}
		pS = pScore;
		short firstOffset = -5000, secondOffset = -5000;
		short skip = 1;
		for (i = 1; i < (m - 1); i++)
		{
			if (pS[i] == 255.0f && pS[i + 1] == 0.0f)
			{
				firstOffset = offsetIndex[i] - 2;
			}
			else if (pS[i] == 0.0f && pS[i + 1] == 255.0f)
			{
				secondOffset = offsetIndex[i];
				break;
			}
		}

		if (firstOffset != -50000 && secondOffset != -5000)
		{
			pImgWnd->SetLineColor(angle, firstOffset, 0, 255, 0);
			pImgWnd->SetLineColor(angle, secondOffset, 0, 255, 0); //rgb

			angle -= 90;
			if (angle < 0.0f)
			{
				angle += 360.0f;
			}
			int deviation[5] = { 0, 1, -1, 2, -2 };
			for (i = -n; i <= n; i++)
			{
				short left, right;
				for (int j = 0; j < 5; j++)
				{
					if (pImgWnd->FindColor(idx1, siz, angle, i * offset + deviation[j], bX, left, right))
					{
						short len = pImgWnd->DetectBevel(idx1, pBuf, siz, angle, i * offset + deviation[j], bX, left, right);
						pImgWnd->ReplaceLine(pBuf, angle, i * offset + deviation[j], bX);
						short d = right - left;
						short v = d;
						if (bX) {
							d /= cos(angle * 0.01745329);
						}
						else {
							d /= sin(angle * 0.01745329);
						}
						float TW = abs(d) * sign(long(v));
						*(pfTrimWidth + (cnt++)) = TW;
						break;
					}
				}
			}
		}
	}
	pImgWnd->Refresh();
	//Back to the previous tpe of camera after measurement bevel width

	float sfx = g->ScrnCalibECH.ScrnX / float(pDIB->pDIB->BmpInfo.biWidth);
	float Ave = p413App->fRemoveFlyer(cnt, pfTrimWidth, 0, 1) * sfx / g->ScrnCalibECH.ppmX;
	delete[] pfTrimWidth;
	delete[] pBuf;
	delete[] pScore;

	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	MParam* p = &pMeP->MPa[kI];
	MParam::PRBNUM Pb = p->Prb;
	if (Pb == MParam::BEVELPRB) {
		//Ave = 1.99f;	// For simulation only [8/15/2012 Administrator]
		p->fThkn = (Ave + pMeP->MPa[kI].Off);  // Convert unit to mm [6/11/2014 C2C]
		g->BevelWidth = p->fThkn;
	}

	pImgWnd->ClearTextInfo();
	pImgWnd->TWBevel = p->fThkn;
	if (pRcp->bSaveVideo) {
		pImgWnd->bStoreComposite = TRUE;
		pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\Bevel.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
	}

	pImgWnd->Refresh(); // Save image [7/3/2014 FSM 413 C2C]

	CData* d = pMP->GetData(0, TRUE);
	if (!d) {
		return -3;
	}

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	if (pMeP->MPa[kI].Prb == MParam::BEVELPRB) {
		if (pMeP->MPa[kI].fThkn != BADDATANEG) {
			d->Set(MeSetX + kI, pMeP->MPa[kI].fThkn);
		}
		else {
			d->Set(MeSetX + kI, BADDATANEG);  // NA [7/16/2010 C2C]
		}
	}
	return TRUE;
}

BOOL C413AppTrim::MeasureBevelWidthUsingTF(CRecipe413* pRcp, CMPoint* pMP, short kI, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo) {
	int i;
	BOOL bX;

	if (!pImgWnd) {
		ASSERT(0);
		return FALSE;
	}

	C413Global* g = &Global;

	float angle = GetAngle(pMP->Co.x, pMP->Co.y);

	pVideo->LoadStockDIB("BEVEL\\BEVEL.BMP");
	pVideo->SnapOneFrame(pRcp->TWExposureBevel + g->BaseExpoTF);
	pImgWnd->DupStore1(g->StoredDIB);

	short n = pRcp->TrimWidthLines;
	short Offset = pRcp->TrimWidthSpacing;
	float* pfTrimWidth = new float[2 * n + 1];

	CPackedDIB* pDIB = &g->StoredDIB;
	if (!pDIB->pDIB) {
		return FALSE;
	}
	int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * (pDIB->pDIB->BmpInfo.biBitCount / 8);

	short cnt = 0;
	for (i = -n; i <= n; i++) {
		int idx1;
		short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
		short len = pImgWnd->ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX/*,0,255,0*/);
		if (len >= siz) {
			ASSERT(0);
		}
		if (len > 0) {
			CString EV;
			EV.Format("%d", i);
			PreprocessBevelData(pBuf, len, 6, TRUE, EV);
			short left = 0, right = 0;
			float TW = GetBevelWidth(pBuf, len, angle, bX, left, right);
			if ((right > 0) && (right < len) && (left > 0) && (left < len) && right > left) {
				*(pfTrimWidth + (cnt++)) = TW;
				if ((i == 0) && (pProfile)) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					pProfile->Set(pDspBuf, len);
					pProfile->Invalidate();
				}
				short* p = pBuf;
				int j;
				for (j = 0; j < left; j++) {
					*(p++) = 255;
				}
				for (j = left; j <= right; j++) {
					*(p++) = 0;
				}
				for (j = right + 1; j < len; j++) {
					*(p++) = 255;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128;
					*(pBuf + idx1) = 128;
					*(pBuf + idx1 + 1) = 128;
				}
				pImgWnd->ReplaceLine(pBuf, angle, i * Offset, bX);
			}
		}
		if (pBuf) {
			delete[] pBuf;
		}
	}

	float sfx = g->ScrnCalibTF.ScrnX / float(pDIB->pDIB->BmpInfo.biWidth);
	float Ave = fRemoveFlyer(cnt, pfTrimWidth, 0, 1) * sfx / g->ScrnCalibTF.ppmX; // p413App->Global.ScrnCalibECH.mppY;
	delete[] pfTrimWidth;

	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	MParam* p = &pMeP->MPa[kI];
	MParam::PRBNUM Pb = p->Prb;
	if (Pb == MParam::BEVEL_TF_PRB) {
		//Ave = 1.99f;	// For simulation only [8/15/2012 Administrator]
		p->fThkn = (Ave + pMeP->MPa[kI].Off) * 1e-3;  // Convert unit to mm [6/11/2014 C2C]
		g->BevelWidth = p->fThkn;
	}

	pImgWnd->ClearTextInfo();
	pImgWnd->TWBevel = p->fThkn;
	if (pRcp->bSaveVideo) {
		pImgWnd->bStoreComposite = TRUE;
		pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\Bevel.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
	}

	pImgWnd->Refresh(); // Save image [7/3/2014 FSM 413 C2C]

	CData* d = pMP->GetData(0, TRUE);
	if (!d) {
		return -3;
	}

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	if (pMeP->MPa[kI].Prb == MParam::BEVEL_TF_PRB) {
		if (pMeP->MPa[kI].fThkn != BADDATANEG) {
			d->Set(MeSetX + kI, pMeP->MPa[kI].fThkn);
		}
		else {
			d->Set(MeSetX + kI, BADDATANEG);  // NA [7/16/2010 C2C]
		}
	}
	return TRUE;
}

BOOL C413AppTrim::GetBevelWidth(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}

	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);
	int BevelAlgo = GetPrivateProfileInt("Bevel", "UpdateAlgo", 1, inifile);

	for (int k = 0; k < MaXPA; k++) {
		MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[k];
		switch (pM->Prb) {
		case MParam::BEVELPRB:
		{
			//if (MeasureBevelWidth_v1(pRcp, pMP, k, pImgWnd, pProfile, pVideo)) { //12182024
			if (BevelAlgo == 0) {
				if (MeasureBevelWidth(pRcp, pMP, k, pImgWnd, pProfile, pVideo)) {
					return TRUE;
				}
			}
			else {
				if (MeasureBevelWidth_v1(pRcp, pMP, k, pImgWnd, pProfile, pVideo)) {
					return TRUE;
				}
			}
		}
		// [6/14/2022 ZHIMING]
		case MParam::BEVEL_TF_PRB:
		{
			if (MeasureBevelWidthUsingTF(pRcp, pMP, k, pImgWnd, pProfile, pVideo)) {
				return TRUE;
			}
		}
		// ]
		break;
		}
	}
	return FALSE;
}

BOOL C413AppTrim::MeasureBevel(CRecipe413* pRcp, CMPoint* pMP, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return FALSE;
	}
	pMP->ProgressSM = CMPoint::MEASURING;

	if (GetBevelWidth(pRcp, pMP, pVideo, pImgWnd, pProfile, pList)) {
		pMP->ProgressSM = CMPoint::COMPLETED;
		return TRUE;
	}

	pMP->ProgressSM = CMPoint::FAILED;
	return FALSE;
}

float C413AppTrim::GetBevelWidth(short* pBuf, short len, float ang, BOOL bX, short& left, short& right) {
	int i;
	BOOL bFound1 = FALSE;
	BOOL bFound2 = FALSE;
	left = 0;
	right = 0;
	short n = 2;
	short* p;

	// Look for highest peak [6/29/2012 user]
	p = pBuf;
	int idx = 0;
	short max = 0;
	for (i = 0; i < len - 1; i++) {
		if (*p >= max) {
			max = *p;
			idx = i;
		}
		p++;
	}

	short max1 = max * 0.5;
	// Look for first peak which is bigger or equal to max1 in REVERSE order [7/8/2014 FSM 413 C2C]

	p = pBuf + len - 1;
	for (i = len - 1; i >= 0; i--) {
		if (*p >= max1) {
			idx = i;
			break;
		}
		p--;
	}

	max *= 0.25;
	if (idx > len - 5) {
		idx = len - 5;
	}

	// Looking from the first and second peak from right to left [6/29/2012 user]
	p = pBuf + idx + 4;
	for (i = idx + 4; i > 0; i--) {
		if (*p > max) {
			if (!bFound2) {
				bFound2 = TRUE;
				right = i;
			}
			else if (!bFound1) {
				bFound1 = TRUE;
				left = i;
			}
		}
		p--;
	}

	if (bFound1 && bFound2) {
		short d = right - left;
		short v = d;
		if (bX) {
			d /= cos(ang * 0.01745329);
		}
		else {
			d /= sin(ang * 0.01745329);
		}
		return abs(d) * sign(long(v));
	}
	return 0;
}

float C413AppTrim::GetBevelWidth2(short* pBuf, short len, float ang, BOOL bX, short& left, short& right) {
	int i;
	short* p;
	short Max, Min, Imax = 0, Imin = 0;

	p = pBuf;
	Max = Min = *p;
	for (i = 0; i < len; i++) {
		if (*p < Min) {
			Min = *p;
			Imin = i;
		}
		if (*p > Max) {
			Max = *p;
			Imax = i;
		}
		p++;
	}
	Max = (Max - Min) / 20;

	if ((Imax < Imin) || (Imax == Imin)) {
		// Problem with image [5/28/2012 Yuen]
		return 0;
	}

	BOOL bFound1 = FALSE;
	BOOL bFound2 = FALSE;
	left = 0;
	right = 0;
	short n = 2;
	p = pBuf + Imin;
	// Looking for -ve to +ve zero crossing [11/6/2011 Administrator]
	// It is assumed that it lies between Imin and Imax [5/28/2012 Yuen]
	for (i = Imin; i <= Imax; i++) {
		if ((*p < 0) && (*(p + n) >= 0)) {
			short* q = p;
			for (int j = 0; j < n; j++) {
				if ((*q < 0) && *(q + 1) >= 0) {
					left = i + j;
					*(p + j) = Min;
					bFound1 = TRUE;
					break;
				}
				q++;
			}
		}
		if (bFound1) {
			break;
		}
		p++;
	}

	// Find first rising edge after Imax [5/28/2012 Yuen]
	p = pBuf + Imax;
	for (i = Imax; i < len - n; i++) {
		if ((*(p + n) - *(p)) > 6) {
			break;
		}
		p++;
	}
	// Find the next +ve peak [5/28/2012 Yuen]
	int start = i;
	p = pBuf + start;
	int Max2 = *p;
	int Imax2 = start;
	for (i = start; i < len - n; i++) {
		if (*p > Max2) {
			Max2 = *p;
			right = i;
			*(p + i) = Min;
			bFound2 = TRUE;
		}
		p++;
	}

	if (bFound1 && bFound2) {
		short d = right - left;
		short v = d;
		if (bX) {
			d /= cos(ang * 0.01745329);
		}
		else {
			d /= sin(ang * 0.01745329);
		}
		return abs(d) * sign(long(v));
	}
	return 0;
}

// return relative distance of edge from center in real unit [5/31/2013 Yuen]
float C413AppTrim::fMeasureSecondTrimWidthInner(float radius, float angle, int pN, CRecipe413* pRcp, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo, CListCtrl* pList) {
	BOOL bX;
	float x, y;

	if (!pImgWnd) {
		ASSERT(0);
		return 0;
	}

	pVideo->LoadSimuDIB(GetInnerEdge2(angle));

	if (!pRcp->bLocateEdgeWidth) {
		radius = pRcp->TrimWidthInnerRadius;
	}
	Global.Polar2Cartesian(radius, angle, x, y);
	if (pMtr) {
		MsgStr.Format("GotoXYZ (fEchoTopZ): %.3f,%.3f,%.3f", x, y, pRcp->fEchoTopZ);
		Message(pList, MsgStr);
		if (!pMtr->GotoXYZ1(x, y, pRcp->fEchoTopZ)) {
			return 0;
		}
	}

	if (!pVideo->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho)) {
		return 0;
	}

	CSingleLock Lock(&Global.StoredDIB.CS);
	Lock.Lock();

	if (pImgWnd) {
		pImgWnd->DupStore1(Global.StoredDIB);
	}

	short n = pRcp->TrimWidthLines;
	short Offset = pRcp->TrimWidthSpacing;

	// Inner Edge [10/6/2010 C2C]
	float* pfTrimWidth = new float[2 * pRcp->TrimWidthLines + 1];
	if (!pfTrimWidth) {
		return 0;
	}
	int cnt = 0;
	CPackedDIB* pDIB = &Global.StoredDIB;
	if (!pDIB->pDIB) {
		return 0;
	}
	for (int i = -n; i <= n; i++) {
		short left;
		short BPP = pDIB->pDIB->BmpInfo.biBitCount / 8;
		int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * BPP;
		if (pImgWnd) {
			int idx1;
			short* pBuf = new short[siz];  // Must obtain new allocation here, allocation will be reduced to len after calling PreporcessTrimData [8/21/2010 C2C]
			short len = pImgWnd->ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX/*,0,255,0*/);
			if (len >= siz) {
				ASSERT(0);
			}
			if (len > 0) {
				// 20210401 Add contrast level for inner trim contrast stretch
				CString str;
				str.Format("%d", i);
				PreprocessTrimData(&pBuf, len, TRUE, pRcp->TrimSmooth, TRUE, str, pRcp->ContrastLevel);  // After calling PreprocessTrimData, pBuf is only len long and smaller than siz [2/18/2012 Administrator]
				float r = GetTrimWidthIn(pBuf, len, angle, bX, left, idx1);
				*(pfTrimWidth + (cnt++)) = r;
				if ((i == 0) && (pProfile)) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					pProfile->Set(pDspBuf, len);
					pProfile->Invalidate();
				}
				int j;
				short* p = pBuf;
				for (j = 0; j <= left; j++) {
					*(p++) = 255;
				}
				for (j = left + 1; j < len; j++) {
					*(p++) = 0;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128;
					*(pBuf + idx1) = 128;
					*(pBuf + idx1 + 1) = 128;
				}
				p = pBuf + 24;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				pImgWnd->ReplaceLine(pBuf, angle, i * Offset, bX);
			}
			if (pBuf) {
				delete[] pBuf;
			}
		}
	}
	// Calculate edge distance [5/31/2013 Yuen]
	float r2 = p413App->fRemoveFlyer(cnt, pfTrimWidth, 0, 1);
	short r = r2;	// edge offset in pixel [5/31/2013 Yuen]
	Global.Polar2Cartesian(r2, angle, x, y);
	float sfx = Global.ScrnCalibECH.ScrnX / float(pImgWnd->pPackedDIB1->IWidth);
	float sfy = Global.ScrnCalibECH.ScrnY / float(pImgWnd->pPackedDIB1->IHeight);
	x = x * sfx / Global.ScrnCalibECH.ppmX;
	y = y * sfy / Global.ScrnCalibECH.ppmY;
	r2 = sqrt(x * x + y * y) * 1e-3 * p413App->sign(r2) + radius;

	if (pfTrimWidth) {
		delete[] pfTrimWidth;
	}
	Lock.Unlock();

	if (pImgWnd) {
		pImgWnd->ClearTextInfo();
		pImgWnd->nTWEdge1 = r;
		pImgWnd->fTWEdge1 = r2;
		if (pRcp->bSaveVideo) {
			pImgWnd->bStoreComposite = TRUE;
			pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW-%d-1.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pN);
		}
		pImgWnd->Refresh();
	}

	return r2;
}

float C413AppTrim::fMeasureSecondTrimWidthOuter(float angle, int pN, CRecipe413* pRcp, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo, CListCtrl* pList) {
	BOOL bX;
	float x, y;

	if (!pImgWnd) {
		ASSERT(0);
		return 0;
	}

	if (pVideo && pVideo->IsSimu()) {
		CString dir;
		dir = GetOuterEdge2(angle);
		pVideo->LoadSimuDIB(dir);
	}

	float radius = pRcp->TrimWidthOuterRadius;
	Global.Polar2Cartesian(radius, angle, x, y);
	if (pMtr) {
		MsgStr.Format("GotoXYZ (fEchoTopZ): %.3f,%.3f,%.3f", x, y, pRcp->fEchoTopZ);
		Message(pList, MsgStr);
		if (!pMtr->GotoXYZ1(x, y, pRcp->fEchoTopZ)) {
			return 0;
		}
	}

	if (!pVideo->SnapOneFrame(pRcp->TWExposureOuter + Global.BaseExpoEcho)) {
		return 0;
	}

	CSingleLock Lock(&Global.StoredDIB.CS);
	Lock.Lock();
	if (pImgWnd) {
		pImgWnd->DupStore1(Global.StoredDIB);
	}
	short n = pRcp->TrimWidthLines;
	short Offset = pRcp->TrimWidthSpacing;

	float* pfTrimWidth = new float[2 * pRcp->TrimWidthLines + 1];
	if (!pfTrimWidth) {
		return 0;
	}

	// Outer edge [5/31/2013 Yuen]
	int cnt = 0;
	CPackedDIB* pDIB = &Global.StoredDIB;
	if (!pDIB->pDIB) {
		return 0;
	}
	for (int i = -n; i <= n; i++) {
		short right;
		short BPP = pDIB->pDIB->BmpInfo.biBitCount / 8;
		int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * BPP;
		if (pImgWnd) {
			int idx1;
			short* pBuf = new short[siz];  // Must obtain new allocation here, allocation will be reduced to len after calling PreporcessTrimData [8/21/2010 C2C]
			short len = pImgWnd->ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX/*,0,255,0*/);
			if (len >= siz) {
				ASSERT(0);
			}
			if (len > 0) {
				PreprocessTrimData(&pBuf, len, TRUE, 6, TRUE, ""); // After calling PreprocessTrimData, pBuf is only len long and smaller than siz [2/18/2012 Administrator]
				float r = GetTrimWidthOut(pBuf, len, angle, bX, right, idx1);
				*(pfTrimWidth + (cnt++)) = r;  // Assuming pixels are square [6/29/2012 user]
				if ((i == 0) && (pProfile)) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					pProfile->Set(pDspBuf, len);
					pProfile->Invalidate();
				}
				int j;
				short* p = pBuf;
				for (j = 0; j < right; j++) {
					*(p++) = 0;
				}
				for (j = right; j < len; j++) {
					*(p++) = 255;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128;
					*(pBuf + idx1) = 128;
					*(pBuf + idx1 + 1) = 128;
				}
				p = pBuf + 24;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				pImgWnd->ReplaceLine(pBuf, angle, i * Offset, bX);
			}
			if (pBuf) {
				delete[] pBuf;
			}
		}
	}

	// Calculate edge distance [5/31/2013 Yuen]
	float r2 = p413App->fRemoveFlyer(cnt, pfTrimWidth, 0, 1);
	short r = r2; // edge offset in pixel [5/31/2013 Yuen]
	Global.Polar2Cartesian(r2, angle, x, y);
	float sfx = Global.ScrnCalibECH.ScrnX / float(pImgWnd->pPackedDIB1->IWidth);
	float sfy = Global.ScrnCalibECH.ScrnY / float(pImgWnd->pPackedDIB1->IHeight);
	x = x * sfx / Global.ScrnCalibECH.ppmX;
	y = y * sfy / Global.ScrnCalibECH.ppmY;
	r2 = sqrt(x * x + y * y) * 1e-3 * p413App->sign(r2) + radius;

	if (pfTrimWidth) {
		delete[] pfTrimWidth;
	}

	Lock.Unlock();

	if (pImgWnd) {
		pImgWnd->ClearTextInfo();
		pImgWnd->nTWEdge1 = r;
		pImgWnd->fTWEdge1 = r2;
		if (pRcp->bSaveVideo) {
			pImgWnd->bStoreComposite = TRUE;
			pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW-%d-2.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pN);
		}
		pImgWnd->Refresh();
	}

	return r2;
}

// Entry point [5/20/2014 FSMT]
// Use by TSMC1 [6/13/2014 Yuen]
// User TrimWidthMethod 0 [5/20/2014 FSMT]
short C413AppTrim::MeasureSecondTrimWidth(CRecipe413* pRcp, CMPoint* pMP, short kI, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		return -1;
	}
	if ((pMP->Co.x == 0) && (pMP->Co.y == 0)) {
		return -2;
	}

	CString msg;
	msg = "MeasureSecondTrimWidth";
	Message(pList, msg);

	float angle, radius;

	BOOL bAllOK = TRUE;
	CString name;

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	// Radius suppose to be at inner trim edge [4/18/2013 Administrator]
	Global.Cartesian2Polar(pMP->Co.x, pMP->Co.y, radius, angle);
	msg.Format("C2P: %.3f,%.3f  R%.3f A%.3f", pMP->Co.x, pMP->Co.y, radius, angle);
	Message(pList, msg);

	if (pMtr) {
		if (pRcp->bLocateEdgeWidth) {
			//if (!pMtr->GotoXYZ1(pMP->Co.x,pMP->Co.y,pRcp->fEchoTopZ))
			//{
			//	return -3;
			//}
			float radius1 = GetRadiusOfInnerEdge(pRcp, pMP, pVideo, pImgWnd, pList);
			if (radius1 != radius) {
				radius = radius1; // Edge found, adjust radius [6/30/2014 Yuen]
				Global.Polar2Cartesian(radius, angle, pMP->Co.x, pMP->Co.y);	// Make correction to x,y value due to new radius value [5/20/2014 FSMT]
				//pMP->Co.x = x; pMP->Co.y = y;	// Update measurement point [6/30/2014 Yuen]
				//if (!pMtr->GotoXY(x,y))
				//{
				//	return -3;
				//}
			}
		}
	}
	Global.Polar2Cartesian(radius, angle, pMP->Co.x, pMP->Co.y);	// Make correction to x,y value due to new radius value [5/20/2014 FSMT]
	if (pMtr) {
		if (!pMtr->GotoXYZ1(pMP->Co.x, pMP->Co.y, pRcp->fEchoTopZ)) {
			return -3;
		}
	}

	// Snap first frame [10/6/2010 C2C]
	msg = "MeasureSecondTrimWidthInner";
	Message(pList, msg);
	float Ave2 = fMeasureSecondTrimWidthInner(radius, angle, pMP->Co.n, pRcp, pImgWnd, pProfile, pVideo, pList);

	if (Global.bAborting) {
		return FALSE;
	}
	// Snap second frame [10/6/2010 C2C]
	msg = "MeasureSecondTrimWidthOuter";
	Message(pList, msg);
	float Ave1 = fMeasureSecondTrimWidthOuter(angle, pMP->Co.n, pRcp, pImgWnd, pProfile, pVideo, pList);

	MParam* p = &pMeP->MPa[kI];
	AveST[kI] = Ave1 - Ave2;
	p->fThkn = AveST[kI] + pMeP->MPa[kI].Off + Global.BevelWidth;

	if (pImgWnd) {
		pImgWnd->ClearTextInfo();
		pImgWnd->nTWEdge1 = 0;
		pImgWnd->fTWEdge1 = 0;
		pImgWnd->SetAve(AveST);
		pImgWnd->SetRecipe(pRcp);
		if (pRcp->bSaveVideo) {
			pImgWnd->bStoreComposite = TRUE;
			pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW-%d-3.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
		}
		pImgWnd->Refresh();
		pImgWnd->SetRecipe(NULL);
	}

	if (pVideo) {
		pVideo->SetAve(AveST);
		pVideo->bShow = TRUE;
		//		ULONGLONG tick = GetTickCount64();
		//		while(pVideo->bShow && ((GetTickCount64()-tick) < 1000))
		//		{
		Sleep(1000);
		//		}
	}

	if (bAllOK) {
		CData* d = pMP->GetData(0, TRUE);
		if (!d) {
			return -3;
		}

		if (pMeP->MPa[kI].Prb == MParam::WTRIMPRB1) {
			if (pMeP->MPa[kI].fThkn != BADDATANEG) {
				d->Set(MeSetX + kI, pMeP->MPa[kI].fThkn + pMP->m_fvalOffset);	// added value offset [10/6/2020]
			}
			else {
				d->Set(MeSetX + kI, BADDATANEG);  // NA [7/16/2010 C2C]
			}
		}
		pMP->ProgressSM = CMPoint::COMPLETED;
		return 0;
	}
	return 4;
}

// Entry point [5/20/2014 FSMT]
// Uses TrimWidthMethod 0 [5/20/2014 FSMT]
// Measure one point only [4/17/2013 Administrator]
// Returns 0 - OK, -ve number indicated error [5/20/2014 FSMT]
short C413AppTrim::MeasureFirstTrimWidth(STrimPARAM& Pa) {
	if (!Pa.pRcp || !Pa.pMP) { ASSERT(0); return -7; }
	CRecipe413* pRcp = Pa.pRcp;
	CMPoint* pMP = Pa.pMP;
	if (!pRcp->IsValidMeSet(pMP)) { return -1; }
	//if ((pMP->Co.x == 0) && (pMP->Co.y == 0)) {
	//	// Not a valid point because trim are always at the edge [5/20/2014 FSMT]
	//	return -2;
	//}
	MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[Pa.kI];
	if (pM->Prb != MParam::NTRIMPROBE1) { return -9; }

	int i;
	float x, y, angle, radius;

	CString name;
	BOOL bX, bAllOK = TRUE;

	x = pMP->Co.x; y = pMP->Co.y;
	Global.Cartesian2Polar(x, y, radius, angle);	//-
//	radius = pRcp->TrimWidthInnerRadius;		// Set radius to outer radius [4/18/2013 Administrator]
//	Global.Polar2Cartesian(radius,angle,x,y);	// Make correction to x,y value due to new radius value [5/20/2014 FSMT]

	if (pMtr) {
		if (pRcp->bLocateEdgeWidth) {
			//if (!pMtr->GotoXYZ1(x,y,pRcp->fEchoTopZ))
			//{
			//	return -3;
			//}
			float radius1 = GetRadiusOfInnerEdge(pRcp, pMP, Pa.pVideo, Pa.pImgWnd, Pa.pList);
			if (radius1 != radius) {
				radius = radius1 - pRcp->EstNTrimWidth / 2.0f; // Edge found, adjust radius [6/30/2014 Yuen]
				Global.Polar2Cartesian(radius, angle, x, y);	// Make correction to x,y value due to new radius value [5/20/2014 FSMT]
				//pMP->Co.x = x; pMP->Co.y = y;	// Update measurement point [6/30/2014 Yuen]
				//if (!pMtr->GotoXY(x,y))
				//{
				//	return -3;
				//}
			}
		}
		else {
			radius += (pRcp->EstNTrimWidth / 2.0f);
			Global.Polar2Cartesian(radius, angle, pMP->Co.x, pMP->Co.y);	// Make correction to x,y value due to new radius value [5/20/2014 FSMT]
		}
		if (!pMtr->GotoXYZ1(pMP->Co.x, pMP->Co.y, pRcp->fEchoTopZ)) {
			return -3;
		}
	}

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	LoadSimuDIB3(Pa.pVideo, angle);

	if (!Pa.pVideo->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho)) {
		return -4;
	}

	CSingleLock Lock(&Global.StoredDIB.CS);
	Lock.Lock();
	CPackedDIB* pDIB = &Global.StoredDIB;
	if (!pDIB->pDIB) { return -8; }
	if (Pa.pImgWnd) { Pa.pImgWnd->DupStore1(Global.StoredDIB); }

	short left, right;
	short n = pRcp->TrimWidthLines;
	short Offset = pRcp->TrimWidthSpacing;
	float* pfTrimWidth = new float[2 * n + 1];
	short BPP = pDIB->pDIB->BmpInfo.biBitCount / 8;

	short cnt = 0;
	for (i = -n; i <= n; i++) {
		int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * BPP;
		if (Pa.pImgWnd) {
			CString s; FILE* fp;
			short* pBuf = new short[siz];
			int idx1 = 0;
			short len = Pa.pImgWnd->ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX/*,0,255,0*/);
			if (len >= siz) { ASSERT(0); return -13; }
			PreprocessTrimData(&pBuf, len, FALSE, 6, TRUE, "");
			s.Format("C:\\TEMP\\F%d.CSV", i);
			fp = fopen(s, "wb");
			if (fp) {
				short* p = pBuf;
				for (int j = 0; j < len; j++, p++) {
					fprintf(fp, "%d,%d\n", j + 1, *p);
				}
				fclose(fp);
			}
			//Reduce(pBuf, len);
			//s.Format("C:\\TEMP\\G%d.CSV", i);
			//fp = fopen(s, "wb");
			//if (fp) {
			//	short* p = pBuf;
			//	for (int j = 0; j < len; j++, p++) {
			//		fprintf(fp, "%d,%d\n", j + 1, *p);
			//	}
			//	fclose(fp);
			//}
			//CString s; s.Format("C:\\TEMP\\D0.CSV");
			//FILE* fp = fopen(s, "r");
			//if (fp) {
			//	short* p = pBuf; int abc = 0;
			//	for (int j = 0; j < len; j++, p++) {
			//		fscanf(fp, "%d,%hd\n", &abc, p);
			//	}
			//	fclose(fp);
			//}
			//FindPeaks(pBuf, len);
			//s.Format("C:\\TEMP\\FP%d.CSV", i);
			//fp = fopen(s, "w");
			//if (fp) {
			//	for (int j = 0; j < 100; j++) {
			//		fprintf(fp, "%d,%f,,%d,%f\n", Max.val[j].idx, Max.val[j].value, Min.val[j].idx, Min.val[j].value);
			//	}
			//	fclose(fp);
			//}
			* (pfTrimWidth + (cnt++)) = GetTrimWidth1(pBuf, len, angle, bX, left, right);
			if (len > 0) {
				if ((i == 0) && (Pa.pProfile)) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					Pa.pProfile->Set(pDspBuf, len);
					Pa.pProfile->Invalidate();
				}
				int j;
				short* p = pBuf;
				for (j = 0; j < left; j++) {
					*(p++) = 255;
				}
				for (j = left; j <= right; j++) {
					*(p++) = 0;
				}
				for (j = right + 1; j < len; j++) {
					*(p++) = 255;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128; *(pBuf + idx1) = 128; *(pBuf + idx1 + 1) = 128;
				}
				Pa.pImgWnd->ReplaceLine(pBuf, angle, i * Offset, bX);
			}
			if (pBuf) { delete[] pBuf; }
		}
	}
	Lock.Unlock();

	C413Global* g = &Global;
	float Ave = fRemoveFlyer(cnt, pfTrimWidth, 0, 1);
	g->Polar2Cartesian(Ave, angle, x, y);
	float sfx = g->ScrnCalibECH.ScrnX / float(Pa.pImgWnd->pPackedDIB1->IWidth);
	x = x * sfx / g->ScrnCalibECH.ppmX;
	float sfy = g->ScrnCalibECH.ScrnY / float(Pa.pImgWnd->pPackedDIB1->IHeight);
	y = y * sfy / g->ScrnCalibECH.ppmY;
	Ave = sqrt(x * x + y * y) * 1e-3 * sign(Ave);

	delete[] pfTrimWidth;

	MParam* p = &pMeP->MPa[Pa.kI];
	p->fThkn = Ave + pMeP->MPa[Pa.kI].Off + g->BevelWidth + pMP->m_fvalOffset;	// added value offset [10/6/2020]

	CString str;
	str.Format("Trim width = %.3fmm", p->fThkn);
	p413App->Message(Pa.pList, str);

	if (bAllOK) {
		CData* d = pMP->GetData(0, TRUE);
		if (!d) {
			return -5;
		}

		if (p->fThkn != BADDATANEG) {
			d->Set(MeSetX + Pa.kI, p->fThkn);
		}
		else {
			d->Set(MeSetX + Pa.kI, BADDATANEG);  // NA [7/16/2010 C2C]
		}

		if (Pa.pImgWnd) {
			Pa.pImgWnd->ClearTextInfo();
			Pa.pImgWnd->SetAve(d->DVal);
			Pa.pImgWnd->SetRecipe(pRcp);
			if (pRcp->bSaveVideo) {
				Pa.pImgWnd->bStoreComposite = TRUE;
				Pa.pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW-%d-1.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
			}
			Pa.pImgWnd->Refresh();
			Pa.pImgWnd->SetRecipe(NULL);
		}
		if (Pa.pVideo) {
			Pa.pVideo->SetAve(d->DVal);
			Pa.pVideo->bShow = TRUE;
			ULONGLONG tick = GetTickCount64();
			while (Pa.pVideo->bShow && ((GetTickCount64() - tick) < 1000)) {
				Sleep(1);
			}
		}
		pMP->ProgressSM = CMPoint::COMPLETED;
		return 0;
	}
	return -6;
}

short C413AppTrim::MeasureFirstTrimWidth_v2(STrimPARAM& Pa) {
	if (!Pa.pRcp || !Pa.pMP) { ASSERT(0); return -7; }
	CRecipe413* pRcp = Pa.pRcp;
	CMPoint* pMP = Pa.pMP;
	if (!pRcp->IsValidMeSet(pMP)) { return -1; }

	MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[Pa.kI];
	if (pM->Prb != MParam::NTRIMPROBE1) { return -9; }

	int i;
	float x, y, angle, radius;

	CString name;
	BOOL bX, bAllOK = TRUE;

	x = pMP->Co.x; y = pMP->Co.y;
	Global.Cartesian2Polar(x, y, radius, angle);
	if (pMtr) {
		if (pRcp->bLocateEdgeWidth) {
			float radius1 = GetRadiusOfInnerEdge(pRcp, pMP, Pa.pVideo, Pa.pImgWnd, Pa.pList);
			if (radius1 != radius) {
				radius = radius1 - pRcp->EstNTrimWidth / 2.0f; // Edge found, adjust radius [6/30/2014 Yuen]
				Global.Polar2Cartesian(radius, angle, x, y);	// Make correction to x,y value due to new radius value [5/20/2014 FSMT]
			}
		}
		else {
			radius += (pRcp->EstNTrimWidth / 2.0f);
			Global.Polar2Cartesian(radius, angle, pMP->Co.x, pMP->Co.y);	// Make correction to x,y value due to new radius value [5/20/2014 FSMT]
		}
		if (!pMtr->GotoXYZ1(pMP->Co.x, pMP->Co.y, pRcp->fEchoTopZ)) {
			return -3;
		}
	}

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	LoadSimuDIB3(Pa.pVideo, angle);

	if (!Pa.pVideo->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho)) {
		return -4;
	}

	CSingleLock Lock(&Global.StoredDIB.CS);
	Lock.Lock();
	CPackedDIB* pDIB = &Global.StoredDIB;
	if (!pDIB->pDIB) { return -8; }
	if (Pa.pImgWnd) { Pa.pImgWnd->DupStore1(Global.StoredDIB); }

	short left, right;
	short n = pRcp->TrimWidthLines;
	short Offset = pRcp->TrimWidthSpacing;
	float* pfTrimWidth = new float[2 * n + 1];
	short BPP = pDIB->pDIB->BmpInfo.biBitCount / 8;

	SLine L;
	vector<SPont> L2;
	vector<SPont>& Ln = L.Ln;
	Ln.reserve(1000);

	short cnt = 0;
	for (i = -n; i <= n; i++) {
		int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * BPP;
		if (Pa.pImgWnd) {
			CString s;
			short* pBuf = new short[siz];
			int idx1 = 0;
			short len = Pa.pImgWnd->ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX/*,0,255,0*/);
			if (len >= siz) { ASSERT(0); return -13; }

			pDIB->Polar2Cartesian(float(i * Offset), float(angle), x, y);
			pDIB->GetLine(Ln, angle, x + 320, y + 240);
			float angle90 = angle + 90.f;
			CString filename;
			for (int j = 0; j < int(Ln.size()); j++) {
				SPont& Lx = Ln[j];
				L2.clear();
				if ((i == 0) && (j == int(Ln.size() / 2))) {
					pDIB->GetLine(L2, angle90, Lx.x, Lx.y);
					filename.Format("C:\\TEMP\\L2-%d.CSV", j);
					pDIB->Dump(filename, L2);
				}
				else {
					pDIB->GetLine(L2, angle90, Lx.x, Lx.y);
				}
				Lx.s = L.Score(j, L2);
			}

			pDIB->Filter(Ln);
			filename.Format("C:\\TEMP\\FILTERED%d.CSV", i);
			pDIB->Dump(filename, Ln);

			right = 0;
			for (int i = 0; i < int(Ln.size()); i++) {
				if (Ln[i].t == 0) {
					right = i;
					break;
				}
			}
			left = 0;
			for (int i = Ln.size() - 1; i >= 0; i--) {
				if (Ln[i].b == 255) {
					left = i;
					break;
				}
			}
			short d = right - left;
			if (bX) { d /= cos(angle * 0.01745329); }
			else { d /= sin(angle * 0.01745329); }
			*(pfTrimWidth + (cnt++)) = abs(d) * sign(long(right - left));

			if (len > 0) {
				if ((i == 0) && (Pa.pProfile)) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					Pa.pProfile->Set(pDspBuf, len);
					Pa.pProfile->Invalidate();
				}
				int j;
				short* p = pBuf;
				for (j = 0; j < left; j++) {
					*(p++) = 255;
				}
				for (j = left; j <= right; j++) {
					*(p++) = 0;
				}
				for (j = right + 1; j < len; j++) {
					*(p++) = 255;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128; *(pBuf + idx1) = 128; *(pBuf + idx1 + 1) = 128;
				}
				Pa.pImgWnd->ReplaceLine(pBuf, angle, i * Offset, bX);
			}
			if (pBuf) { delete[] pBuf; }
		}
	}
	Lock.Unlock();

	C413Global* g = &Global;
	float Ave = fRemoveFlyer(cnt, pfTrimWidth, 0, 1);
	g->Polar2Cartesian(Ave, angle, x, y);
	float sfx = g->ScrnCalibECH.ScrnX / float(Pa.pImgWnd->pPackedDIB1->IWidth);
	x = x * sfx / g->ScrnCalibECH.ppmX;
	float sfy = g->ScrnCalibECH.ScrnY / float(Pa.pImgWnd->pPackedDIB1->IHeight);
	y = y * sfy / g->ScrnCalibECH.ppmY;
	Ave = sqrt(x * x + y * y) * 1e-3 * sign(Ave);

	delete[] pfTrimWidth;

	MParam* p = &pMeP->MPa[Pa.kI];
	p->fThkn = Ave + pMeP->MPa[Pa.kI].Off + g->BevelWidth + pMP->m_fvalOffset;	// added value offset [10/6/2020]

	CString str;
	str.Format("Trim width = %.3fmm", p->fThkn);
	p413App->Message(Pa.pList, str);

	if (bAllOK) {
		CData* d = pMP->GetData(0, TRUE);
		if (!d) {
			return -5;
		}

		if (p->fThkn != BADDATANEG) {
			d->Set(MeSetX + Pa.kI, p->fThkn);
		}
		else {
			d->Set(MeSetX + Pa.kI, BADDATANEG);  // NA [7/16/2010 C2C]
		}

		if (Pa.pImgWnd) {
			Pa.pImgWnd->ClearTextInfo();
			Pa.pImgWnd->SetAve(d->DVal);
			Pa.pImgWnd->SetRecipe(pRcp);
			if (pRcp->bSaveVideo) {
				Pa.pImgWnd->bStoreComposite = TRUE;
				Pa.pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW-%d-1.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
			}
			Pa.pImgWnd->Refresh();
			Pa.pImgWnd->SetRecipe(NULL);
		}
		if (Pa.pVideo) {
			Pa.pVideo->SetAve(d->DVal);
			Pa.pVideo->bShow = TRUE;
			ULONGLONG tick = GetTickCount64();
			while (Pa.pVideo->bShow && ((GetTickCount64() - tick) < 1000)) {
				Sleep(1);
			}
		}
		pMP->ProgressSM = CMPoint::COMPLETED;
		return 0;
	}
	return -6;
}

// Entry point [5/20/2014 FSMT]
// Measure trim width and height [5/20/2014 FSMT]
// Uses TrimWidthMethod 0 [5/20/2014 FSMT]
// Measure one point [4/18/2013 Administrator]
BOOL C413AppTrim::MeasureTrim(CRecipe413* pRcp, CMPoint* pMP, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile, CListCtrl* pList) {
	pMP->ProgressSM = CMPoint::MEASURING;
	//////////////////////////////////////////////////////////////////////////
	// Pattern matching [6/3/2014 FSMT]
	float X, Y;
	X = pMP->Co.x;
	Y = pMP->Co.y;

	bLoadDIB = TRUE;

	CString MsgStr;
	BOOL bMatched = FALSE;
	if (pMP->PatSet > 0) {
		// Pattern matching is not use universally but by step height function only [6/30/2014 C2C]
		LoadSimuDIB(pVideo);
		MsgStr = "Pattern matching initiated";
		Message(pList, MsgStr);
		pMtr->GotoXYZ1(X, Y, pRcp->fEchoTopZ);

		if (pRcp->PatMatOpt[pMP->PatSet - 1] & CRecipe413::EXPOSE4) {
			pVideo->Level = pRcp->AutoExposeEcho;
			pVideo->AutoAdjust();
		}
		if (pRcp->PatMatOpt[pMP->PatSet - 1] & CRecipe413::FOCAMATCH) {
			AutoFocusVideo(CCamBase::ECHOCAM, pRcp->fTgtGRnX[pMP->PatSet - 1][0], pRcp->fTgtGRnY[pMP->PatSet - 1][0], pVideo);
		}
		// Do pattern matching [3/28/2013 Yuen]
		SDoPattrnMatching Param;
		Param.Set(pVideo, pImgWnd, pList);
		p413App->SetupParam(pRcp, Param, 0);
		Param.bApplyOffset = FALSE;
		Param.bMoveCenter = FALSE;

		Param.bApplyOffset = TRUE;
		Param.SearchDepth = 12;
		if (pRcp->MatchAlgorithm[pMP->PatSet - 1] == 4) {
			Param.bMoveCenter = TRUE;
			Param.bApplyOffset = FALSE;
		}
		else if (pRcp->MatchAlgorithm[pMP->PatSet - 1] == 0) {
			Param.bMoveCenter = TRUE;
			Param.bApplyOffset = FALSE;
		}
		else if (pRcp->MatchAlgorithm[pMP->PatSet - 1] == 1) {
			Param.bMoveCenter = TRUE;
			Param.bApplyOffset = TRUE;
		}
		else {
			Param.bMoveCenter = FALSE;
		}

		for (int k = 1; k < pRcp->nTrialPat; k++) {
			if (Global.bAborting) {
				return FALSE;
			}
			MsgStr.Format("Trial %d, move to green location", k);
			short PatMatchRsl = DoPattrnMatching(Param); // Echo Match [6/28/2013 Yuen]
			switch (PatMatchRsl) {
			case C413AppPatt::pmOK:
				float X1, Y1;
				if (pMtr) {
					MsgStr.Format("Modify point coor from %.3f,%.3f", pMP->Co.x, pMP->Co.y);
					Message(pList, MsgStr);
					pMtr->GetXYPos(&pMP->Co.x, &pMP->Co.y); // Modify measurement coordinate [6/3/2014 Yuen]
					MsgStr.Format("to %.3f,%.3f", pMP->Co.x, pMP->Co.y);
					X = pMP->Co.x; Y = pMP->Co.y;
					if (pMP->Co.p > 0) {
						X1 = Global.LocXY.X;
						Y1 = Global.LocXY.Y;
						//pMtr->GetXYPos(&X1,&Y1);
						Global.DLoc.Pt[pMP->Co.p].Set(X1, Y1);
						Global.DLoc.Calc();
					}
					bMatched = TRUE;
				}
				break;
				// Many other cases need to consider [2/25/2013 Administrator]
			default:
			{
				float X1, Y1;
				X1 = X + gPX[k] * pRcp->fStepSizePat;
				Y1 = Y + gPY[k] * pRcp->fStepSizePat;
				// Move to new search location [5/26/2014 FSMT]
				MsgStr.Format("Trial %d: X=%.4f Y=%.4f", k + 1, X1, Y1);
				Message(pList, MsgStr);
				LoadSimuDIB(pVideo);
				if (!pMtr->GotoXY(X1, Y1)) {
					pMP->ProgressSM = CMPoint::FAILED;
					return FALSE;
				}
				continue;
			}
			break;
			}
			if (bMatched) {
				break;
			}
		}

		if (bMatched) {
			if (pRcp->PatMatOpt[pMP->PatSet - 1] & CRecipe413::FOCUS3) {
				AutoFocusVideo(CCamBase::ECHOCAM, pRcp->fTgtGRnX[pMP->PatSet - 1][0], pRcp->fTgtGRnY[pMP->PatSet - 1][0], pVideo);
			}
			else {
				pMtr->MoveEchoTopZ(pRcp->fEchoTopZ + pRcp->TFZOffset);  // reuse of mutually exclusive variable [10/26/2011 Administrator]
			}
		}
		else {
			MsgStr = "Fail pattern matching";
			Message(pList, MsgStr);
			pMP->ProgressSM = CMPoint::FAILED;
			return FALSE;
		}

		// pMP.Co modified to matched location on exit here [6/30/2014 Yuen]
	}

	//////////////////////////////////////////////////////////////////////////
	// Note: X,Y should not be moved after pattern matching [6/30/2014 C2C]
	// X,Y needed to be move in inner function if pattern matching is not specified [6/30/2014 C2C]
	if (GetTrimWidthHeight(pRcp, pMP, pVideo, pImgWnd, pProfile, pList)) {
		pMP->ProgressSM = CMPoint::COMPLETED;
		return TRUE;
	}
	pMP->ProgressSM = CMPoint::FAILED;
	return FALSE;
}

float C413AppTrim::CalcTrimWidth(short** pBuf, short len, float& ang, BOOL& bX, short& left, short& right) {
	float ret = GetTrimWidth(*pBuf, len, ang, bX, left, right);
	return ret;
}

// Input: expect a second derivative of raw data [4/22/2013 Administrator]
// Return radius of the line [4/22/2013 Administrator]
float C413AppTrim::GetTrimWidth(short* pBuf, short len, float ang, BOOL bX, short& left, short& right) {
	int i;
	short* p;
	short Max, Min, iMin = 0, iMax = 0;

	p = pBuf;
	Max = Min = *p;
	for (i = 0; i < len; i++, p++) {
		if (*p < Min) { Min = *p; iMin = i; }
		if (*p > Max) { Max = *p; iMax = i; }
	}

	Max = (Max - Min) / 25;

	BOOL bFound1 = FALSE;
	BOOL bFound2 = FALSE;
	left = 0;
	right = 0;
	short n = 2;
	p = pBuf + n;
	// Looking for -ve to +ve zero crossing [11/6/2011 Administrator]
	for (i = n; i < len - n; i++) {
		if ((*(p - n) < 0) && (*(p + n) >= 0)) {
			if ((*(p + n) - *(p - n)) >= Max) {
				short* q = p - n;
				for (int j = -n; j < n; j++) {
					if ((*q < 0) && *(q + 1) >= 0) {
						left = i + j;
						*(p + j) = Min;
						bFound1 = TRUE;
						break;
					}
					q++;
				}
			}
		}
		if (bFound1) {
			break;
		}
		p++;
	}
	p = pBuf + len - 1 - n;
	for (i = len - n; i >= n; i--) {
		if ((*(p - n) < 0) && (*(p + n) >= 0)) {
			if ((*(p + n) - *(p - n)) >= Max) {
				short* q = p - n;
				for (int j = -n; j < n; j++) {
					if ((*q < 0) && *(q + 1) >= 0) {
						right = i + j;
						*(p + j + 1) = Min;
						bFound2 = TRUE;
						break;
					}
					q++;
				}
			}
		}
		if (bFound2) {
			break;
		}
		p--;
	}

	short d = 0;
	if (bFound1 && bFound2) {
		// Readjust to find peak maxima [4/22/2013 Administrator]
		// left of left should contain a peak minima [4/22/2013 Administrator]
		d = right - left;
		short v = d;
		if (d <= 0) {
			Log("GetTrimWidth: right < left", 1);
		}
		if (bX) {
			d /= cos(ang * 0.01745329);
		}
		else {
			d /= sin(ang * 0.01745329);
		}
		return abs(d) * sign(long(v));
	}
	return d;
}

float C413AppTrim::GetTrimWidth1(short* pBuf, short len, float ang, BOOL bX, short& left, short& right) {
	short* p = pBuf;
	short Max, Min;
	Max = Min = *p;
	left = right = 0;
	for (int i = 0; i < len; i++, p++) {
		if (*p < Min) { Min = *p; left = i; }
		if (*p > Max) { Max = *p; right = i; }
	}
	if (left > right) {
		// Swap values
		int tmp = left; left = right; right = tmp;
		tmp = Min; Min = Max; Max = tmp;
	}

	// Readjust to find peak maxima [4/22/2013 Administrator]
	// left of left should contain a peak minima [4/22/2013 Administrator]
	short d = right - left;
	if (bX) { d /= cos(ang * 0.01745329); }
	else { d /= sin(ang * 0.01745329); }
	return abs(d) * sign(long(right - left));
}

// Main entry point [5/20/2014 FSMT]
// Measure trim width and height of different probes [5/20/2014 FSMT]
// Measure one point only [4/18/2013 Administrator]
BOOL C413AppTrim::GetTrimWidthHeight(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) { return FALSE; }

	if (pVideo) { pVideo->SetAve(NULL); }
	if (pImgWnd) { pImgWnd->SetAve(NULL); }

	CString msg;
	short Method = 0;
	BOOL bAllOK = TRUE;
	float angle, radius;
	float x, y;
	STrimWH ST;

	for (int k = 0; k < MaXPA; k++) {
		if (Global.bAborting || Global.bStop) { return FALSE; }
		MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[k];
		switch (pM->Prb) {
		case MParam::NTRIMPROBE1:
		{
			// Measure one point only [4/17/2013 Administrator]
			msg.Format("Narrow trim width 1: %d %.4f,%.4f", pMP->Co.n, pMP->Co.x, pMP->Co.y);
			Message(pList, msg);

			STrimPARAM Pa;
			Pa.pRcp = pRcp;
			Pa.pMP = pMP;
			Pa.kI = k;
			Pa.pImgWnd = pImgWnd;
			Pa.pProfile = pProfile;
			Pa.pVideo = pVideo;
			Pa.pList = pList;
			if (MeasureFirstTrimWidth_v2(Pa) != 0) { return FALSE; }
		}
		break;
		case MParam::NTRIMPROBE2:	// ##### [4/16/2013 Administrator]
		case MParam::NTRIMPROBE3:	// ##### [4/16/2013 Administrator]
		{
			// Measure one point only [4/17/2013 Administrator]
			msg.Format("Narrow trim width 2/3: %d %.3f,%.3f,%.3f", pMP->Co.n, pMP->Co.x, pMP->Co.y, pMP->Co.z);
			Message(pList, msg);

			if (MeasureNarrowTrimWidth(ST, pMP, k, pRcp, pImgWnd, pImgWnd, pVideo, pProfile, pList) != 0) {
				return FALSE;
			}
		}
		break;
		case MParam::WTRIMPRB1:
		{
			msg.Format("Wide trim width 1: %d %.3f,%.3f", pMP->Co.n, pMP->Co.x, pMP->Co.y, pMP->Co.z);
			Message(pList, msg);

			if (MeasureSecondTrimWidth(pRcp, pMP, k, pImgWnd, pProfile, pVideo, pList) != 0) {
				return FALSE;
			}
		}
		break;
		case MParam::WTRIMPRB2:		// ##### [4/16/2013 Administrator]
		case MParam::WTRIMPRB3:		// ##### [4/16/2013 Administrator]
		{
			// Measure one point only [4/17/2013 Administrator]
			msg.Format("Wide trim width 2/3: %d %.3f,%.3f,%.3f", pMP->Co.n, pMP->Co.x, pMP->Co.y, pMP->Co.z);
			Message(pList, msg);

			if (MeasureWideTrimWidth(ST, pMP, k, pRcp, pImgWnd, pVideo, pImgWnd, pVideo, pProfile, pList) != 0) {
				return FALSE;
			}
		}
		break;
		case MParam::TRIMWHPROB:
		{
			// Implementation 2, UMC [4/16/2013 Administrator]
			msg.Format("Trim width and height: %d %.3f,%.3f %.3f", pMP->Co.n, pMP->Co.x, pMP->Co.y, pMP->Co.z);
			Message(pList, msg);
			if (MeasureTrimWidthAndHeight(ST, pMP, k, pRcp, pImgWnd, pImgWnd, pVideo, pVideo, pList) != 0) {
				return FALSE;
			}
		}
		break;
		case MParam::STEPHEIGHTCOMPRB:
		{
			// Trim height measurement, TSMC1 [6/7/2014 FSMT]
			msg = "Step height combo";
			Message(pList, msg);
			MeasureStepHeight(pRcp, pMP, pVideo, k, pImgWnd, pList);
		}
		break;
		case MParam::BUMPHEIGHTTPRB:
		case MParam::BUMPHEIGHTBPRB:
		case MParam::TRENCHTOPPRB:
		case MParam::TRENCHBOTPRB:
		{
			msg = "Step heightG";
			Message(pList, msg);
			MeasureStepHeightG(pRcp, pMP, pVideo, k, pList);
		}
		break;
		case MParam::STEPHEIGHTDISTPRB:
			Method++;
		case MParam::STEPHEIGHTTHKBPRB:
			// fall through intended [12/22/2010 XPMUser]
		case MParam::STEPHEIGHTTHKPRB:
		{
			msg = "Step height distance/thickness";
			Message(pList, msg);
			short Prb = 0;  // for STEPHEIGHTTHKPRB and STEPHEIGHTDISTPRB probe [8/21/2011 FSMT]
			if (pM->Prb == MParam::STEPHEIGHTTHKBPRB) {
				Prb = 1;
			}
		REPEAT:
			int i/*,j*/;
			CPeaks Pk;
			float refA[MaXPA], refB[MaXPA], cntA[MaXPA], cntB[MaXPA];
			float Max[MaXPA], Min[MaXPA], AveA[MaXPA], AveB[MaXPA];

			Pk.Clear();
			if (pMtr) pMtr->MoveEchoTopZ(pRcp->fEchoTopZ + pRcp->TrimHeightZOffset);
			pEcho->SetRecipe(pRcp);

			for (i = 0; i < MaXPA; i++) {
				refA[i] = 0;
				refB[i] = 0;
				cntA[i] = 0;
				cntB[i] = 0;
				Max[i] = 0;
				Min[i] = 0;
				AveA[i] = 0;
				AveB[i] = 0;
			}
			x = pMP->Co.x;
			y = pMP->Co.y;
			// Inner point [8/26/2010 XPMUser]
			float v;
			short MeSetX = MaXPA * (pMP->MeSet - 1);
			CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];
			angle = GetAngle(x, y);
			radius = sqrt(x * x + y * y);
			BOOL bFound1 = FALSE;
			BOOL bFound2 = FALSE;
			float startRadius = radius - pRcp->TrimHeightDistanceInner1;
			float dR;
			dR = (pRcp->TrimHeightDistanceInner1 - pRcp->TrimHeightDistanceInner2) / float(pRcp->nTrialTrim + 3);
			if (dR < 0) {
				dR = 0;
			}

			Global.Polar2Cartesian(startRadius, angle, x, y);
			CPeakParam PParam;
			switch (Method) {
			case 0:
				if (Prb == 0) {
					PParam.nPeakT = 3;
					PParam.HeightT[0].Set(2, 32);
				}
				else {
					PParam.nPeakB = 3;
					PParam.HeightB[0].Set(2, 32);
				}
				PParam.Method = CPeakParam::cmEQUAL;
				break;
			case 1:
				if (Prb == 0) {
					PParam.nPeakT = 2;
				}
				else {
					PParam.nPeakB = 2;
				}
				PParam.Method = CPeakParam::cmGREATER;
				break;
			default:
				ASSERT(0);
				break;
			}
			short nRetry = pRcp->nTrialTrim;
			if (nRetry < 1) {
				nRetry = 1;
			}
			if (pMtr) {
				if (!pMtr->GotoXY(x, y)) {
					return FALSE;
				}
			}
			if (pMtr) pMtr->WaitEchoTopZ();
			short nRetryCnt = 1;
			while (nRetry--) {
				if (pRcp->bEchoPeakMonitor && pVideo) {
					CString Name;
					Name.Format("1-%d-%d", pMP->Co.n, nRetryCnt);
					if (pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, pVideo, pList, NULL, pMP->MeSet, Name, &PParam) > CEchoProbeBase::GPPKTOTALERR) {
						// found [12/17/2010 C2C]
						bFound1 = TRUE;
						break;
					}
					else {
						Message(pList, MsgStr); // Show message from GetPeaks [9/30/2012 Yuen]
						startRadius += dR;
						Global.Polar2Cartesian(startRadius, angle, x, y);
						if (pMtr) {
							if (!pMtr->GotoXY(x, y)) {
								return FALSE;
							}
						}
						nRetryCnt++;
					}
				}
				else {
					if (pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, NULL, pList, NULL, pMP->MeSet, "", &PParam) > CEchoProbeBase::GPPKTOTALERR) {
						// found [12/17/2010 C2C]
						bFound1 = TRUE;
						break;
					}
					else {
						Message(pList, MsgStr); // Show message from GetPeaks [9/30/2012 Yuen]
						startRadius += dR;
						Global.Polar2Cartesian(startRadius, angle, x, y);
						if (pMtr) {
							if (!pMtr->GotoXY(x, y)) {
								return FALSE;
							}
						}
						nRetryCnt++;
					}
				}
			}

			for (i = 0; i < MaXPA; i++) {
				MParam* p = &pMeP->MPa[i];
				MParam::PRBNUM Pb = p->Prb;
				if ((Pb == MParam::STEPHEIGHTTHKPRB) || (Pb == MParam::STEPHEIGHTTHKBPRB) ||
					(Pb == MParam::STEPHEIGHTDISTPRB)) {
					float v1, v2;
					if (Method > 0) {
						// for STEPHEIGHTDISTPRB [8/21/2011 FSMT]
						v1 = Pk.fCM[Prb][1];
						v2 = Pk.fCM[Prb][0];  // This is always zero [11/30/2010 XPMUser]
					}
					else {
						v1 = Pk.fCM[Prb][2];
						v2 = Pk.fCM[Prb][1];
					}
					if ((v1 > -1) && (v2 > -1) && (v1 - v2) > 0) {
						if (Method == 0) {
							if (p->RI != 1) {
								v = (v1 - v2) * cfTop / (p->RI * TRiCorr); //changed AP
							}
							else {
								v = (v1 - v2) * cfTop;
							}
						}
						else {
							v = (v1 - v2);
						}
						refA[i] += v;
						cntA[i]++;
						AveA[i] = Max[i] = Min[i] = v;
					}
				}
			}
			// Massage data [8/26/2010 XPMUser]
			for (i = 0; i < MaXPA; i++) {
				if (cntA[i] > 2) {
					if (fabs(AveA[i] - Max[i]) > fabs(AveA[i] - Min[i])) {
						AveA[i] = (AveA[i] - Max[i]) / (cntA[i] - 1);
					}
					else {
						AveA[i] = (AveA[i] - Min[i]) / (cntA[i] - 1);
					}
				}
				else if (cntA[i] > 1) {
					AveA[i] /= cntA[i];
				}
			}

			// Outer point [8/26/2010 XPMUser]
			for (i = 0; i < MaXPA; i++) {
				refB[i] = 0;
				cntB[i] = 0;
				Max[i] = 0;
				Min[i] = 0;
				AveB[i] = 0;
			}
			startRadius = radius + pRcp->TrimHeightDistanceOuter1;
			dR = (pRcp->TrimHeightDistanceOuter1 - 0.1) / float(pRcp->nTrialTrim + 3);
			if (dR < 0) {
				dR = 0;
			}

			Global.Polar2Cartesian(startRadius, angle, x, y);
			switch (Method) {
			case 0:
				if (Prb == 0) {
					PParam.nPeakT = 3;
					PParam.HeightT[0].Set(2, 32);
					//PParam.hPeakT[0] = 32;
					//PParam.IndexT[0] = 2;
				}
				else {
					PParam.nPeakB = 3;
					PParam.HeightB[0].Set(2, 32);
					//PParam.hPeakB[0] = 32;
					//PParam.IndexB[0] = 2;
				}
				PParam.Method = CPeakParam::cmEQUAL;
				break;
			case 1:
				if (Prb == 0) {
					PParam.nPeakT = 2;
				}
				else {
					PParam.nPeakB = 2;
				}
				PParam.Method = CPeakParam::cmGREATER;
				break;
			default:
				ASSERT(0);
				break;
			}
			nRetry = pRcp->nTrialTrim;
			if (nRetry < 1) {
				nRetry = 1;
			}
			if (pMtr) {
				if (!pMtr->GotoXY(x, y)) {
					return FALSE;
				}
			}
			nRetryCnt = 1;
			while (nRetry--) {
				CString Name;
				Name.Format("2-%d-%d", pMP->Co.n, nRetryCnt);
				if (pRcp->bEchoPeakMonitor && pVideo) {
					if (pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, pVideo, pList, NULL, pMP->MeSet, Name, &PParam) > CEchoProbeBase::GPPKTOTALERR) {
						// found [12/17/2010 C2C]
						bFound2 = TRUE;
						break;
					}
					else {
						Message(pList, MsgStr); // Show message from GetPeaks [9/30/2012 Yuen]
						startRadius -= dR;
						Global.Polar2Cartesian(startRadius, angle, x, y);
						if (pMtr) {
							if (!pMtr->GotoXY(x, y)) {
								return FALSE;
							}
						}
						nRetryCnt++;
					}
				}
				else {
					if (pEcho->GetPeaks(pRcp, Pk, pMeP->NAve, NULL, pList, NULL, pMP->MeSet, "", &PParam) > CEchoProbeBase::GPPKTOTALERR) {
						// found [12/17/2010 C2C]
						bFound2 = TRUE;
						break;
					}
					else {
						Message(pList, MsgStr); // Show message from GetPeaks [9/30/2012 Yuen]
						startRadius -= dR;
						Global.Polar2Cartesian(startRadius, angle, x, y);
						if (pMtr) {
							if (!pMtr->GotoXY(x, y)) {
								return FALSE;
							}
						}
						nRetryCnt++;
					}
				}
			}
			for (i = 0; i < MaXPA; i++) {
				MParam* p = &pMeP->MPa[i];
				MParam::PRBNUM Pb = p->Prb;
				if ((Pb == MParam::STEPHEIGHTTHKPRB) || (Pb == MParam::STEPHEIGHTTHKBPRB) ||
					(Pb == MParam::STEPHEIGHTDISTPRB)) {
					float v1, v2;
					if (Method > 0) {
						// for STEPHEIGHTDISTPRB [8/21/2011 FSMT]
						v1 = Pk.fCM[Prb][1];
						v2 = Pk.fCM[Prb][0];  // This is always zero [11/30/2010 XPMUser]
					}
					else {
						v1 = Pk.fCM[Prb][2];
						v2 = Pk.fCM[Prb][1];
					}
					if ((v1 > -1) && (v2 > -1)) {
						if (Method == 0) {
							if (p->RI != 1) {
								v = (v1 - v2) * cfTop / (p->RI * TRiCorr);
							}
							else {
								v = (v1 - v2) * cfTop;
							}
						}
						else {
							v = (v1 - v2);
						}
						refB[i] += v;
						cntB[i]++;
						AveB[i] = Max[i] = Min[i] = v;
					}
				}
			}

			// Massage data [8/26/2010 XPMUser]
			for (i = 0; i < MaXPA; i++) {
				if (cntB[i] > 2) {
					if (fabs(AveB[i] - Max[i]) > fabs(AveB[i] - Min[i])) {
						AveB[i] = (AveB[i] - Max[i]) / (cntB[i] - 1);
					}
					else {
						AveB[i] = (AveB[i] - Min[i]) / (cntB[i] - 1);
					}
				}
				else if (cntB[i] > 1) {
					AveB[i] /= cntB[i];
				}
			}

			for (i = 0; i < MaXPA; i++) {
				MParam* p = &pMeP->MPa[i];
				MParam::PRBNUM Pb = p->Prb;
				if (bFound1 && bFound2) {
					if (Pb == MParam::STEPHEIGHTTHKPRB) {
						p->fThkn = fabs(AveA[i] - AveB[i]) + pMeP->MPa[i].Off;
					}
					else if (Pb == MParam::STEPHEIGHTTHKBPRB) {
						p->fThkn = fabs(AveA[i] - AveB[i]) + pMeP->MPa[i].Off;
					}
					else if (Pb == MParam::STEPHEIGHTDISTPRB) {
						p->fThkn = fabs(AveB[i] - AveA[i]) + pMeP->MPa[i].Off;
					}
					else if (Pb == MParam::NOPRB) {
					}
					else {
						ASSERT(0);
					}
				}
				else {
					if (Method == 0) {
						Method++;
						goto REPEAT;
					}
					p->fThkn = 0;
				}
			}
			if (bAllOK) {
				CData* d = pMP->GetData(0, TRUE);
				if (!d) {
					return FALSE;
				}

				for (i = 0; i < MaXPA; i++) {
					if (pMeP->MPa[i].fThkn != BADDATANEG) {
						d->Set(MeSetX + i, pMeP->MPa[i].fThkn);
					}
					else {
						d->Set(MeSetX + i, BADDATANEG);  // NA [7/16/2010 C2C]
					}
				}
				pMP->ProgressSM = CMPoint::COMPLETED;
				continue;
			}
			else {
				return FALSE;
			}
			continue;
		}
		break;
		}
	}
	return TRUE;
}

void C413AppTrim::PreprocessBevelData(short* pBuf, short len, short nSmooth, BOOL thresh, CString tm) {
	int i;
	BOOL bSave = FALSE;

	if (tm.GetLength() > 0) {
		bSave = TRUE;
	}
	bSave = FALSE;	// disable saving data [6/28/2012 user]
	if (bSave) {
		SaveExtractedLine(-1, pBuf, len, tm + "-R");
	}

	for (i = 0; i < nSmooth; i++) {
		Smooth(pBuf, len);
	}
	if (bSave) {
		SaveExtractedLine(-1, pBuf, len, tm + "-S");
	}

	if (thresh) {
		ContrastStretch(pBuf, len);
		for (i = 0; i < 3; i++) {
			Smooth(pBuf, len);
		}
		if (bSave) {
			SaveExtractedLine(-1, pBuf, len, tm + "-C");
		}
	}

	IntegrateAndReverseNegative(pBuf, len);
	if (bSave) {
		SaveExtractedLine(-1, pBuf, len, tm + "-I1");
	}

	Shrink(pBuf, len);
	if (bSave) {
		SaveExtractedLine(-1, pBuf, len, tm + "-K");
	}

	// 	Integrate(pBuf,len);
	// 	if (bSave)
	// 	{
	// 		SaveExtractedLine(-1,pBuf,len,tm+"-I2");
	// 	}
}

void C413AppTrim::ThresholdTrimData(short* pBuf, short len, short val, CString tm) {
	int i;
	short* p = pBuf;
	for (i = 0; i < len; i++) {
		if (*p > 0) {
			*p -= val;
			if (*p < 0) {
				*p = 0;
			}
		}
		else if (*p < 0) {
			*p += val;
			if (*p > 0) {
				*p = 0;
			}
		}
		p++;
	}
}

void C413AppTrim::IntegrateAndReverseNegative(short* pBuf, short len) {
	short* pNew = new short[len + 16];
	short* p = pNew;
	short* q = pBuf;

	int i;
	short n = 5;
	for (i = 0; i < n; i++) {
		*(p++) = 0;
		q++;
	}
	for (i = n; i < len - n; i++) {
		*(p++) = abs(*(q - n) - *(q + n));
		q++;
	}
	for (int j = i; j < len; j++) {
		*(p++) = 0;
	}
	memcpy(pBuf, pNew, len * sizeof(short));
	delete[] pNew;
}

void C413AppTrim::Shrink(short* pBuf, short len) {
	int i;
	short* p1, * p2;

	// Shrink left to right [6/29/2012 user]
	p1 = pBuf; p2 = p1 + 1;
	for (i = 0; i < len - 1; i++) {
		if (*p1 < *p2) {
			*p1 = 0;
		}
		p1++; p2++;
	}
	p1 = pBuf + len - 1; p2 = p1 - 1;
	for (i = len - 1; i > 0; i--) {
		if (*p1 <= *p2) {
			*p1 = 0;
		}
		p1--; p2--;
	}
	*pBuf = 0; *(pBuf + len - 1) = 0;
}

// Entry point [5/20/2014 FSMT]
// Uses TrimWidthMethod 1 [5/20/2014 FSMT]
// Dual probe calibration needed before calling this function [4/18/2013 Administrator]
// Output format [4/15/2013 Administrator]
// TRMWD, TRMHTT, TRMHD, RIN, ROUT, REFIN, REFOUT, TTIN, TTOUT per MPa [4/15/2013 Administrator]
short C413AppTrim::MeasureTrimWidthAndHeight(STrimWH& S, CMPoint* pMP, short kI, CRecipe413* pRcp, CImgWnd* pImgWnd1, CImgWnd* pImgWnd2, CSICam* pVideo1, CSICam* pVideo2, CListCtrl* pList) {
	CString msg;

	// Sanity check [4/15/2013 Administrator]
	if (!pVideo1 || !pVideo2 || !pImgWnd1 || !pImgWnd2 || !pRcp || !pMP) {
		// Not possible to proceed [4/14/2013 Administrator]
		return -1;
	}
	if (!pRcp->IsValidMeSet(pMP)) {
		msg = "Invalid MeSet";
		Message(pList, msg);
		return -3;
	}

	MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	if (pM->Prb != MParam::TRIMWHPROB) {
		if (!((pM->Prb == MParam::NOPRB) || (pM->Prb == MParam::DISPPRB))) {
			// Only valid for probe 38 [6/9/2014 Yuen]
			msg.Format("Probe = %d not 38", pM->Prb);
			Message(pList, msg);
		}
		return -4;
	}

	if ((pMP->Co.x == 0) && (pMP->Co.y == 0)) {
		msg = "Position error";
		Message(pList, msg);
		return -5;
	}

	pMP->ProgressSM = CMPoint::MEASURING;

	// Perform dual probe calibration here [4/15/2013 Administrator]
	float radius = 0, angle = 0;
	C413Global* g = &Global;
	g->Cartesian2Polar(pMP->Co.x, pMP->Co.y, radius, angle);
	SFINDEDGES Par;

	pVideo1->ClearEchoInfo();

	// Output format [4/15/2013 Administrator]
	// TRMWD, TRMHTT, TRMHD, RIN, ROUT, REFIN, REFOUT, TTIN, TTOUT per MPa [4/15/2013 Administrator]

	int i;

	Par.pRcp = pRcp;
	Par.pList = pList;
	Par.pVideo = pVideo2;
	Par.pImgWnd = pImgWnd2;
	Par.Cmax = 0; Par.Cmin = 0;

	// Measures trim width [4/15/2013 Administrator]
	// Goto outer edge of wafer [4/14/2013 Administrator]
	Par.radius = pRcp->TrimWidthOuterRadius;	// Start the ball rolling [4/14/2013 Administrator]
	Par.angle = angle;
	Par.Thres = pRcp->FindTrimThres;
	Par.bFindBestAngle = FALSE;
	Par.bContrastStrecth = FALSE;
	Par.method = CImgWnd::DARKFWD;
	Par.Exposure = pRcp->TWExposureOuter;
	Par.AutoExpose = pRcp->AutoExposeOuter;
	Par.ZPosition = pRcp->fEchoTopZ;
	Par.name = GetOuterEdge2(angle);

	msg.Format("Measure point #%d", pMP->Co.n);
	Message(pList, msg);

	msg.Format("Measure outer edge at R=%.3f, A=%.3f", Par.radius, Par.angle);
	Message(pList, msg);

	FindEdges(Par);

	// Find Edge and record Rout [4/14/2013 Administrator]
	float nTWEdge = 0;

	S.Rout = Par.radius - Par.dist1;

	pImgWnd1->ClearTextInfo();
	pImgWnd1->PointNum = pMP->Co.n;
	pImgWnd1->TWWidth = BADDATANEG;
	if (pRcp->bSaveVideo) {
		pImgWnd1->bStoreComposite = TRUE;
		pImgWnd1->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW-%d-1.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
	}
	pImgWnd1->Refresh();

	// Base on Rout, move inward to expected Rin [4/14/2013 Administrator]
	//radius = S.Rout - (pRcp->TrimWidthOuterRadius - pRcp->TrimWidthInnerRadius);
	Par.radius = pRcp->TrimWidthInnerRadius;
	Par.angle = angle;
	Par.Thres = pRcp->FindTrimThres;
	Par.bFindBestAngle = FALSE;
	Par.bContrastStrecth = FALSE;
	Par.method = CImgWnd::DARKFWD;
	Par.Exposure = pRcp->TWExposureInner;
	Par.AutoExpose = pRcp->AutoExposeInner;
	Par.name = GetInnerEdge2(angle);

	msg.Format("Measure inner edge at R=%.3f, A=%.3f", Par.radius, Par.angle);
	Message(pList, msg);

	//20130828-1 SCOTT EDIT---------------------------------------
	BOOL bFind = TRUE;
	BOOL bResult = TRUE;
	float fDist;
	FindEdges(Par);
	if ((Par.dist2 == 0) || fabs(Par.dist1 - Par.dist2) > 0.045) {
		if (fabs(Par.dist1) > 0.125f) {
			// Edge line too large [7/3/2013 Yuen]
			Par.Thres = pRcp->FindTrimThres * 2;
			Par.bFindBestAngle = FALSE;
			bFind = FALSE;

			msg = "Retry #1";
			Message(pList, msg);
			FindEdges(Par);
		}
	}
	if (fabs(Par.dist1) > 0.50f) {
		// Edge line too large [7/3/2013 Yuen]
		Par.Thres = pRcp->FindTrimThres * 2;
		Par.bFindBestAngle = FALSE;
		bFind = FALSE;

		msg = "Retry #2";
		Message(pList, msg);
		FindEdges(Par);
	}
	if (!bFind) {
		if ((Par.dist2 == 0) || fabs(Par.dist1 - Par.dist2) > 0.045) {
			if (fabs(Par.dist1) > 0.125f) {
				//OutputDebugString("??? refindEdges() 1");
				bResult = refindEdges(Par, fDist);
			}
		}
		if (fabs(Par.dist1) > 0.50f) {
			//OutputDebugString("??? refindEdges() 2");
			bResult = refindEdges(Par, fDist);
		}
	}
	if (!bResult) {
		Par.dist1 = fDist;
	}
	/*------------------------------------------------------------

	FindEdges(Par);
	if ((Par.dist2 == 0) || fabs(Par.dist1-Par.dist2) > 0.045)
	{
		if (fabs(Par.dist1) > 0.125f)
		{
			// Edge line too large [7/3/2013 Yuen]
			Par.Thres = pRcp->FindTrimThres * 2;
			Par.bFindBestAngle = FALSE;
			FindEdges(Par);
		}
	}
	if (fabs(Par.dist1) > 0.50f)
	{
		// Edge line too large [7/3/2013 Yuen]
		Par.Thres = pRcp->FindTrimThres * 2;
		Par.bFindBestAngle = FALSE;
		FindEdges(Par);
	}
	*/

	S.Rin = Par.radius - Par.dist1;

	MParam* pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	pImgWnd2->ClearTextInfo();
	pImgWnd2->PointNum = pMP->Co.n;
	pImgWnd2->TWBevel = g->BevelWidth;
	pImgWnd2->TWWidth = (S.Rout - S.Rin) + pM1->Off + g->BevelWidth;
	if (pRcp->bSaveVideo) {
		pImgWnd2->bStoreComposite = TRUE;
		pImgWnd2->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTWH-%d-2.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
	}
	pImgWnd2->Refresh();

	// Trim height measurement [4/15/2013 Administrator]
	// TWB.Rin now represent the boundary between trim and non-trim area [4/14/2013 Administrator]
	// Par.radius = S.Rin - pRcp->TrimHeightDistanceInner;
	Par.radius = pRcp->TrimWidthInnerRadius - pRcp->TrimHeightDistanceInner1;
	msg.Format("Measure inner step at R=%.3f", Par.radius);
	Message(pList, msg);

	pEcho->SetRecipe(pRcp);
	short SimuPkBak = pEcho->SimuPkNum;
	pEcho->SimuPkNum = 0;

	// Measure inner step [4/14/2013 Administrator]
	// Output: REFIN, TTIN [4/15/2013 Administrator]
	S.n = 1;
	S.nPt = Par.pRcp->SamplePerPoint;
	S.TTin = BADDATANEG;
	S.HTin = BADDATANEG;
	if (GetStepHeightThickness(S, pRcp, pMP, Par.radius, angle, g->DPGageThickness, pVideo1, pList)) // Inner step [6/29/2011 XPMUser]
	{
		S.TTin = S.TThk;
		S.HTin = S.Hgt;
	}

	// Measure outer step [7/6/2013 Yuen]
	if (pRcp->NMeasureOut < 2) {
		pRcp->NMeasureOut = 1;
	}
	float* pTThk = new float[pRcp->NMeasureOut + 6];	// Need extra [6/5/2013 FSM413]
	float* pHgt = new float[pRcp->NMeasureOut + 6];		// Need extra [6/5/2013 FSM413]
	float Out1 = pRcp->TrimWidthInnerRadius + pRcp->TrimHeightDistanceOuter1;
	float Out2 = pRcp->TrimWidthOuterRadius - pRcp->TrimHeightDistanceOuter2;
	for (i = 0; i < pRcp->NMeasureOut; i++) {
		if (g->bAborting || g->bStop) {
			break;
		}
		switch (pRcp->NMeasureOut) {
		case 1:
			Par.radius = pRcp->TrimWidthInnerRadius + pRcp->TrimHeightDistanceOuter1 + fabs(Out2 - Out1) / 2.0f;
			break;
		case 2:
			Par.radius = pRcp->TrimWidthInnerRadius + pRcp->TrimHeightDistanceOuter1 + (i) * (fabs(Out2 - Out1));
			break;
		default:
			float dR = fabs(Out2 - Out1) / float(pRcp->NMeasureOut - 1);
			Par.radius = pRcp->TrimWidthInnerRadius + pRcp->TrimHeightDistanceOuter1 + i * dR;
			break;
		}
		if (Par.radius > pRcp->TrimWidthOuterRadius) {
			Par.radius -= 0.5f;
		}
		msg.Format("Measure outer step at R=%.3f Pos=#%d", Par.radius, i + 1);
		Message(pList, msg);

		pEcho->SimuPkNum = 0;
		pEcho->PosT[0].Pos[1].Position += 0.005f;
		// Measure outer step [4/14/2013 Administrator]
		// Output: REFOUT, TTOUT [4/15/2013 Administrator]
		S.n++;
		if (GetStepHeightThickness(S, pRcp, pMP, Par.radius, angle, g->DPGageThickness, pVideo1, pList)) // Inner step [6/29/2011 XPMUser]
		{
			// Save single point results [4/15/2013 Administrator]
			pTThk[i] = S.TThk;
			pHgt[i] = S.Hgt;
		}
		else {
			// Error recovery [4/15/2013 Administrator]
			pTThk[i] = BADDATANEG;
			pHgt[i] = BADDATANEG;
		}
		pEcho->PosT[0].Pos[1].Position -= 0.005f;
	}
	short cnt1 = 0, cnt2 = 0;
	float TTout = 0, HTout = 0;
	S.TTout = BADDATANEG;
	S.HTout = BADDATANEG;
	// Save result here [4/15/2013 Administrator]
	for (i = 0; i < pRcp->NMeasureOut; i++) {
		if (pTThk[i] != BADDATANEG) {
			cnt1++;
			TTout += pTThk[i];
		}
		if (pHgt[i] != BADDATANEG) {
			cnt2++;
			HTout += pHgt[i];
		}
	}
	if (cnt1 > 0) {
		S.TTout = TTout / float(cnt1);
	}
	if (cnt2 > 0) {
		S.HTout = HTout / float(cnt2);
	}

	// Save results [8/26/2011 FSMT]
	CData* d = pMP->GetData(0, TRUE);
	if (!d) {
		msg = "CData error";
		Message(pList, msg);
		return FALSE;
	}

	// Output format [4/15/2013 Administrator]
	// TRMWD, TRMHTT, TRMHD, RIN, ROUT,  REFIN, REFOUT, TTIN, TTOUT per MPa [4/15/2013 Administrator]
	short v;
	short MeSetX = MaXPA * (pMP->MeSet - 1);
	// Note: kI get increment in value from this point onward [4/17/2013 Administrator]
	if ((S.Rin != BADDATANEG) && (S.Rout != BADDATANEG)) {
		v = kI;
		if (v >= MaXPA) { v = MaXPA - 1; }
		pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[v];
		d->Set(MeSetX + kI, fabs(S.Rout - S.Rin) + pM1->Off + g->BevelWidth + pMP->m_fvalOffset);	// Trim width [8/30/2011 C2C] added value offset [10/6/2020]
	}
	else {
		d->Set(MeSetX + kI, BADDATANEG);	// Trim width [8/30/2011 C2C]
	}
	kI++;
	if ((S.HTin != BADDATANEG) && (S.HTout != BADDATANEG)) {
		v = kI;
		if (v >= MaXPA) { v = MaXPA - 1; }
		pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[v];
		d->Set(MeSetX + kI, fabs(S.HTout - S.HTin) + pM1->Off);  // Trim height using HT
	}
	else {
		d->Set(MeSetX + kI, BADDATANEG);  // Trim height using HT
	}
	kI++;
	if ((S.TTin != BADDATANEG) && (S.TTout != BADDATANEG)) {
		v = kI;
		if (v >= MaXPA) { v = MaXPA - 1; }
		pM1 = &pRcp->MeParam[pMP->MeSet - 1].MPa[v];
		d->Set(MeSetX + kI, fabs(S.TTout - S.TTin) + pM1->Off);		// Trim height using TT [8/30/2011 C2C]
	}
	else {
		d->Set(MeSetX + kI, BADDATANEG);		// Trim height using TT [8/30/2011 C2C]
	}

	// Optional basic data that can be override [4/15/2013 Administrator]
	kI++;
	d->Set(MeSetX + kI, S.Rin);
	kI++;
	d->Set(MeSetX + kI, S.Rout);
	kI++;
	d->Set(MeSetX + kI, S.HTin);
	kI++;
	d->Set(MeSetX + kI, S.HTout);
	kI++;
	d->Set(MeSetX + kI, S.TTin);
	kI++;
	d->Set(MeSetX + kI, S.TTout);

	// Do not check index limit [7/8/2013 Yuen]
	for (i = 0; i < pRcp->NMeasureOut; i++) {
		if (kI >= MAXMEPASET * MaXPA - 2) {
			break;
		}
		kI++;
		d->Set(MeSetX + kI, fabs(pHgt[i] - S.HTin) + pM1->Off);	// HT out [7/27/2013 Yuen]
		kI++;
		d->Set(MeSetX + kI, fabs(S.TTin - pTThk[i]) + pM1->Off);	// TT out [7/27/2013 Yuen]
	}

	// Clean up [4/15/2013 Administrator]
	delete[] pTThk;
	delete[] pHgt;
	pEcho->SimuPkNum = SimuPkBak;

	pMP->ProgressSM = CMPoint::COMPLETED;

	return 0;
}

// Entry point [5/20/2014 FSMT]
// Uses TrimWidthMethod 1 [5/20/2014 FSMT]
// Use in setup section only. Not use in actual Job, no need to save  [4/18/2013 Administrator]
BOOL C413AppTrim::MeasureTrimWidthOnly(STrimWH& S, float angle, CRecipe413* pRcp, CImgWnd* pImgWnd1, CImgWnd* pImgWnd2, CSICam* pVideo1, CSICam* pVideo2, CListCtrl* pList) {
	// Sanity check [4/15/2013 Administrator]
	if (!pVideo1 || !pVideo2 || !pImgWnd1 || !pImgWnd2 || !pRcp) {
		// Not possible to proceed [4/14/2013 Administrator]
		return FALSE;
	}
	CSICam* pVideo = pVideo1;
	CImgWnd* pImgWnd = pImgWnd1;

	SFINDEDGES Par;
	C413Global* g = &Global;

	// Measures trim width only [4/15/2013 Administrator]
	Par.radius = pRcp->TrimWidthOuterRadius;	// Start the ball rolling [4/14/2013 Administrator]

	Par.pRcp = pRcp;
	Par.pVideo = pVideo;
	Par.pImgWnd = pImgWnd;
	Par.Cmax = 0; Par.Cmin = 0;

	Par.angle = angle;
	Par.Thres = pRcp->FindTrimThres;
	Par.bFindBestAngle = FALSE;
	Par.bContrastStrecth = FALSE;
	Par.method = CImgWnd::DARKFWD;
	Par.Exposure = pRcp->TWExposureOuter;
	Par.AutoExpose = pRcp->AutoExposeOuter;
	Par.ZPosition = pRcp->fEchoTopZ;
	Par.name = "";

	FindEdges(Par);

	S.Rout = Par.radius - Par.dist1;

	pImgWnd->ClearTextInfo();
	if (pRcp->bSaveVideo) {
		pImgWnd->bStoreComposite = TRUE;
		pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW-1.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
	}
	pImgWnd->Refresh();

	if (pVideo2->IsSimu()) {
		pImgWnd = pImgWnd2;
		pVideo = pVideo2;
	}
	Par.pImgWnd = pImgWnd;
	Par.pVideo = pVideo;

	// Base on Rout, move inward to expected Rin [4/14/2013 Administrator]
	Par.radius = pRcp->TrimWidthInnerRadius;

	Par.angle = angle;
	Par.Thres = pRcp->FindTrimThres;
	Par.bFindBestAngle = FALSE;
	Par.bContrastStrecth = FALSE;
	Par.method = CImgWnd::DARKFWD;
	Par.Exposure = pRcp->TWExposureInner;
	Par.AutoExpose = pRcp->AutoExposeInner;
	Par.name = "";

	FindEdges(Par);
	if ((Par.dist2 == 0) || ((Par.dist1 - Par.dist2) > 0.5f)) {
		Par.Thres = pRcp->FindTrimThres * 2;
		Par.bFindBestAngle = FALSE;
		FindEdges(Par);
	}
	if (abs(Par.dist1 > 0.5)) {
		Par.Thres = pRcp->FindTrimThres * 2;
		Par.bFindBestAngle = FALSE;
		FindEdges(Par);
	}
	S.Rin = Par.radius - Par.dist1;

	pImgWnd->ClearTextInfo();
	if (pRcp->bSaveVideo) {
		pImgWnd->bStoreComposite = TRUE;
		pImgWnd->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW-2.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
	}
	pImgWnd->Refresh();
	return TRUE;
}

// Entry point [5/20/2014 FSMT]
// Uses TrimWidthMethod 1 [5/20/2014 FSMT]
// Measure one point only [4/18/2013 Administrator]
// Return 0 - OK, -ve number indicates error [5/20/2014 FSMT]
short C413AppTrim::MeasureNarrowTrimWidth(STrimWH& S, CMPoint* pMP, short kI, CRecipe413* pRcp, CImgWnd* pImgWnd1, CImgWnd* pImgWnd2, CSICam* pVideo1, CChartWnd* pProfile, CListCtrl* pList) {
	// Sanity checks [4/15/2013 Administrator]
	if (!pMP || !pRcp || !pImgWnd1 || !pVideo1) {
		return -1;
	}
	if (!pRcp->IsValidMeSet(pMP)) {
		return -2;
	}
	MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	switch (pM->Prb) {
	case  MParam::NTRIMPROBE2:
	case  MParam::NTRIMPROBE3:
		break;
	default:
		return -3;
		break;
	}

	pMP->ProgressSM = CMPoint::MEASURING;

	SLnX L1, L2;
	int i, In, Out;
	float x, y;
	float radius, angle;
	float TrimW = 0;
	C413Global* g = &Global;

	g->Cartesian2Polar(pMP->Co.x, pMP->Co.y, radius, angle);	// Retrieve angle information [4/14/2013 Administrator]
	radius = pRcp->TrimWidthOuterRadius;	// Start the ball rolling [4/14/2013 Administrator]

	// Goto edge of wafer [4/14/2013 Administrator]
	g->Polar2Cartesian(radius, angle, x, y);
	if (pMtr) {
		if (!pMtr->GotoXYZ1(x, y, pRcp->fEchoTopZ)) {
			return -4;
		}
	}

	if (pVideo1 && pVideo1->IsSimu()) {
		CString dir;
		// 		DosUtil.GetProgramDir(dir);
		// 		dir += "IMAGES\\";
		dir = GetOuterEdge1(angle);
		pVideo1->LoadSimuDIB(dir);
	}
	if (pM->Prb == MParam::NTRIMPROBE2)	// Narrow Trim Width Probe [4/15/2013 Administrator]
	{
		//////////////////////////////////////////////////////////////////////////
		// Method 1 [4/15/2013 Administrator]
		//////////////////////////////////////////////////////////////////////////

		pVideo1->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho);

		pImgWnd1->DupStore1(g->StoredDIB);
		pImgWnd1->Invalidate(TRUE);

		angle = FindBestAngle(angle, pImgWnd1);

		pImgWnd1->FindEdge(&L1, &L2, In, Out, angle, CImgWnd::DARKBWD, 0.08f);
		if (pVideo1->IsSimu()) {
			p413App->Yld(500);
		}
		float Rout = L1.offset;

		// Find inside edge of wafer [4/15/2013 Administrator]
		pImgWnd1->FindEdge(&L1, &L2, In, Out, angle, CImgWnd::DARKFWD, 0.08f);
		if (pVideo1->IsSimu()) {
			p413App->Yld(500);
		}
		float Rin = L1.offset;

		float ra = Rout - Rin;	// First result [4/15/2013 Administrator]
		g->Polar2Cartesian(ra, angle, x, y);
		float sfx = g->ScrnCalibECH.ScrnX / float(pImgWnd1->pPackedDIB1->IWidth);
		x = x * sfx / g->ScrnCalibECH.ppmX;
		float sfy = g->ScrnCalibECH.ScrnY / float(pImgWnd1->pPackedDIB1->IHeight);
		y = y * sfy / g->ScrnCalibECH.ppmY;
		S.TW1 = TrimW = sqrt(x * x + y * y) * 1e-3 * sign(ra);  // Number is counted from center [4/22/2013 Administrator]

		if (pImgWnd1) {
			pImgWnd1->ClearTextInfo();
			pImgWnd1->nTWEdge1 = ra;
			pImgWnd1->fTWEdge1 = S.TW1;
			if (pRcp->bSaveVideo) {
				pImgWnd1->bStoreComposite = TRUE;
				pImgWnd1->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\NTW-%d.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
			}
			pImgWnd1->Refresh();
		}
	}
	if (pM->Prb == MParam::NTRIMPROBE3) {
		//////////////////////////////////////////////////////////////////////////
		// Method 2 [4/15/2013 Administrator]
		//////////////////////////////////////////////////////////////////////////
		pVideo1->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho);

		CImgWnd* pImage = pImgWnd2;
		if (!pImage) {
			pImage = pImgWnd1;
		}
		//g->StoredDIB.Contrast3Level();
		pImage->DupStore1(g->StoredDIB);
		pImage->Invalidate(TRUE);

		BOOL bX;
		short cnt = 0;
		short left, right;
		short n = pRcp->TrimWidthLines;
		short Offset = pRcp->TrimWidthSpacing;

		float* pfTrimWidth = new float[2 * n + 1];
		int siz = 3 * pImage->pPackedDIB1->IWidth * pImage->pPackedDIB1->BytesPerPixel;

		for (i = -n; i <= n; i++) {
			int idx1;
			short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
			short len = pImage->ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX/*,0,255,0*/);
			if (len >= siz) {
				ASSERT(0);
			}
			if (len > 0) {
				PreprocessTrimData(&pBuf, len, TRUE, 6, TRUE, "");
				S.TW2 = GetTrimWidth(pBuf, len, angle, bX, left, right);  // Directly return the number of pixel between left and right [4/22/2013 Administrator]
				if ((S.TW2 > 0) && (right > left)) {
					*(pfTrimWidth + (cnt++)) = S.TW2;
					if (i == 0 && pProfile) {
						short* pDspBuf = new short[len + 1];
						memcpy(pDspBuf, pBuf, len * sizeof(short));
						pProfile->Set(pDspBuf, len);
						pProfile->Invalidate();
					}
					short* p = pBuf;
					int j;
					for (j = 0; j < left; j++) {
						*(p++) = 255;
					}
					for (j = left; j <= right; j++) {
						*(p++) = 0;
					}
					for (j = right + 1; j < len; j++) {
						*(p++) = 255;
					}
					if ((idx1 < len - 2) && (idx1 > 1)) {
						*(pBuf + idx1 - 1) = 128;
						*(pBuf + idx1) = 128;
						*(pBuf + idx1 + 1) = 128;
					}
					pImage->ReplaceLine(pBuf, angle, i * pRcp->TrimWidthSpacing, bX);
				}
			}
			if (pBuf) {
				delete[] pBuf;
			}
		}

		float sfx = g->ScrnCalibECH.ScrnX / float(pImage->pPackedDIB1->IWidth);
		float sfy = g->ScrnCalibECH.ScrnY / float(pImage->pPackedDIB1->IHeight);
		float r = fRemoveFlyer(cnt, pfTrimWidth, 0, 1);
		g->Polar2Cartesian(r, angle, x, y);
		x = x * sfx / g->ScrnCalibECH.ppmX;
		y = y * sfy / g->ScrnCalibECH.ppmY;
		S.TW2 = TrimW = sqrt(x * x + y * y) * sign(r) * 1e-3;
		delete[] pfTrimWidth;

		if (pImage) {
			pImage->ClearTextInfo();
			pImage->nTWEdge1 = r;
			pImage->fTWEdge1 = S.TW2;
			if (pRcp->bSaveVideo) {
				pImage->bStoreComposite = TRUE;
				pImage->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\NTW-%d.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
			}
			pImage->Refresh();
		}
	}
	// ##### Store result here [4/16/2013 Administrator]
	CData* d = pMP->GetData(0, TRUE);
	if (!d) {
		return -5;
	}
	short MeSetX = MaXPA * (pMP->MeSet - 1);
	//		MParam *pM1 = &pRcp->MeParam[pMP->MeSet-1].MPa[k];
	if (pM->Prb == MParam::NTRIMPROBE2) {
		d->Set(MeSetX + kI, TrimW + pM->Off + Global.BevelWidth + pMP->m_fvalOffset);	// Trim width [8/30/2011 C2C] added value offset [10/6/2020]
	}
	if (pM->Prb == MParam::NTRIMPROBE3) {
		d->Set(MeSetX + kI, TrimW + pM->Off + Global.BevelWidth + pMP->m_fvalOffset);	// Trim width [8/30/2011 C2C] added value offset [10/6/2020]
	}
	return 0;
}

BOOL C413AppTrim::FindPeaks(short* pBuf, int len) {
	for (int i = 0; i < len; i++, pBuf++) {
		Max.AddMax(i, *pBuf);
		Min.AddMin(i, *pBuf);
	}
	return TRUE;
}

BOOL C413AppTrim::Reduce(short* pBuf, int len) {
	short* pA = new short[len];
	memcpy(pA, pBuf, len * sizeof(short));
	pBuf++;
	for (int i = 1; i < len - 1; i++) {
		if (pA[i] > 0) {
			if (((pA[i] >= pA[i - 1]) && (pA[i] >= pA[i + 1]))) {}
			else pBuf[i] = 0;
		}
		else if (pA[i] < 0) {
			if (((pA[i] <= pA[i - 1]) && (pA[i] <= pA[i + 1]))) {}
			else pBuf[i] = 0;
		}
	}
	delete[]pA;
	return TRUE;
}

// Entry point [5/20/2014 FSMT]
// Uses TrimWidthMethod 1 [5/20/2014 FSMT]
// Wide trim width measurement [4/15/2013 Administrator]
short C413AppTrim::MeasureWideTrimWidth(STrimWH& S, CMPoint* pMP, short kI, CRecipe413* pRcp, CImgWnd* pImgWnd1, CSICam* pVideo1, CImgWnd* pImgWnd2, CSICam* pVideo2, CChartWnd* pProfile, CListCtrl* pList) {
	CString msg;
	// Sanity checks [4/15/2013 Administrator]
	if (!pMP || !pRcp || !pImgWnd1 || !pVideo1) {
		msg = "Fatal error";
		Message(pList, msg);
		return -1;
	}
	if (!pRcp->IsValidMeSet(pMP)) {
		msg = "Invalid MeSet";
		Message(pList, msg);
		return -2;
	}
	MParam* pM = &pRcp->MeParam[pMP->MeSet - 1].MPa[kI];
	switch (pM->Prb) {
	case MParam::WTRIMPRB2:
	case MParam::WTRIMPRB3:
		msg = "Wide trim 2/3 found";
		Message(pList, msg);
		break;
	default:
		msg = "Invalid probe number";
		Message(pList, msg);
		return -3;
		break;
	}

	// Do not know the purpose, comment off [5/21/2014 FSMT]
	//if ((pMP->Co.n == 5) && (pM->Prb == MParam::WTRIMPRB3))
	//{
	//	if (!pVideo1)
	//	{
	//		return -9;
	//	}
	//}

	pMP->ProgressSM = CMPoint::MEASURING;

	CString name;

	SLnX L1, L2;
	int In, Out;
	float x, y;
	float Rin, Rout;
	float radius, angle;
	float TrimW = 0;
	C413Global* g = &Global;

	g->Cartesian2Polar(pMP->Co.x, pMP->Co.y, radius, angle);	// Retrieve angle information [4/14/2013 Administrator]

	// Goto edge of wafer [4/14/2013 Administrator]
	radius = pRcp->TrimWidthOuterRadius;	// Start the ball rolling [4/14/2013 Administrator]

	g->Polar2Cartesian(radius, angle, x, y);
	msg.Format("R=%.3f, A=%.3f, X=%.3f, Y=%.3f", radius, angle, x, y);
	Message(pList, msg);

	if (pMtr) {
		if (!pMtr->GotoXYZ1(x, y, pRcp->fEchoTopZ)) {
			msg = "GotoXYZ1 error";
			Message(pList, msg);
			return FALSE;
		}
	}

	if (pM->Prb == MParam::WTRIMPRB2)	// Wide Trim Width Probe [4/15/2013 Administrator]
	{
		msg = "Wide trim probe 2";
		Message(pList, msg);

		//////////////////////////////////////////////////////////////////////////
		// Method 1 [4/15/2013 Administrator]
		//////////////////////////////////////////////////////////////////////////
		if (pVideo1 && pVideo1->IsSimu()) {
			CString dir;
			//DosUtil.GetProgramDir(dir);
			//dir += "IMAGES\\";
			dir = GetOuterEdge2(angle);
			pVideo1->LoadSimuDIB(dir);
		}

		pVideo1->SnapOneFrame(pRcp->TWExposureOuter + Global.BaseExpoEcho);

		pImgWnd1->DupStore1(g->StoredDIB);
		pImgWnd1->Invalidate(TRUE);
		p413App->Yld(500); // For UI refresh [5/21/2014 FSMT]

		angle = FindBestAngle(angle, pImgWnd1);
		msg.Format("Best angle = %.3f", angle);
		Message(pList, msg);

		// Find outer edge of wafer [4/15/2013 Administrator]
		pImgWnd1->FindEdge(&L1, &L2, In, Out, angle, CImgWnd::DARKBWD, 0.08f);
		p413App->Yld(500); // For UI refresh [5/21/2014 FSMT]

		g->Polar2Cartesian(L1.offset, angle, x, y);
		float sfx = g->ScrnCalibECH.ScrnX / float(pImgWnd1->pPackedDIB1->IWidth);
		x = x * sfx / g->ScrnCalibECH.ppmX;
		float sfy = g->ScrnCalibECH.ScrnY / float(pImgWnd1->pPackedDIB1->IHeight);
		y = y * sfy / g->ScrnCalibECH.ppmY;
		Rout = sqrt(x * x + y * y) * 1e-3 * sign(L1.offset) + radius;

		if (pImgWnd1) {
			pImgWnd1->ClearTextInfo();
			if (pRcp->bSaveVideo) {
				pImgWnd1->bStoreComposite = TRUE;
				pImgWnd1->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW1-%d-1.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
			}
			pImgWnd1->Refresh();
		}

		// Base on Rout, goto expected inner trim edge [4/21/2013 Administrator]
		radius = pRcp->TrimWidthInnerRadius;
		g->Polar2Cartesian(radius, angle, x, y);
		if (pMtr) {
			if (!pMtr->GotoXY(x, y)) {
				msg = "GotoXY error";
				Message(pList, msg);
				return -11;
			}
		}

		pVideo2->LoadSimuDIB(GetInnerEdge2(angle));
		pVideo2->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho);

		pImgWnd2->DupStore1(g->StoredDIB);
		pImgWnd2->Invalidate(TRUE);
		p413App->Yld(500);	// For UI refresh [5/21/2014 FSMT]

		// Find inside edge of wafer [4/15/2013 Administrator]
		pImgWnd2->FindEdge(&L1, &L2, In, Out, angle, CImgWnd::DARKFWD, 0.08f);
		g->Polar2Cartesian(L1.offset, angle, x, y);

		sfx = g->ScrnCalibECH.ScrnX / float(pImgWnd2->pPackedDIB1->IWidth);
		sfy = g->ScrnCalibECH.ScrnY / float(pImgWnd2->pPackedDIB1->IHeight);
		x = x * sfx / g->ScrnCalibECH.ppmX;
		y = y * sfy / g->ScrnCalibECH.ppmY;
		Rin = sqrt(x * x + y * y) * 1e-3 * sign(L1.offset) + radius;

		if (pImgWnd2) {
			pImgWnd2->ClearTextInfo();
			if (pRcp->bSaveVideo) {
				pImgWnd2->bStoreComposite = TRUE;
				pImgWnd2->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW1-%d-2.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
			}
			pImgWnd2->Refresh();
		}

		//		name = "C:\\TEMP\\TW2I.BMP";
		//		pImgWnd1->Save(name);

		S.TW1 = TrimW = Rout - Rin;
	}
	if (pM->Prb == MParam::WTRIMPRB3)	// Narrow Trim Width Probe [4/15/2013 Administrator]
	{
		msg = "Wide trim probe 3";
		Message(pList, msg);
		//////////////////////////////////////////////////////////////////////////
		// Method 2 [4/15/2013 Administrator]
		//////////////////////////////////////////////////////////////////////////
		if (pVideo1 && pVideo1->IsSimu()) {
			CString dir;
			// 			DosUtil.GetProgramDir(dir);
			// 			dir += "IMAGES\\";
			dir = GetOuterEdge1(angle);
			pVideo1->LoadSimuDIB(dir);
		}

		pVideo1->SnapOneFrame(pRcp->TWExposureOuter + Global.BaseExpoEcho);

		pImgWnd1->DupStore1(g->StoredDIB);
		pImgWnd1->Invalidate(TRUE);

		int i, j;
		BOOL bX;
		short cnt = 0;
		short left, right;
		short n = pRcp->TrimWidthLines;
		short Offset = pRcp->TrimWidthSpacing;
		float r;
		float* pfTrimWidth = new float[2 * n + 3];

		int siz = 3 * pImgWnd1->pPackedDIB1->IWidth * pImgWnd1->pPackedDIB1->BytesPerPixel;
		float sfx = g->ScrnCalibECH.ScrnX / float(pImgWnd1->pPackedDIB1->IWidth);
		float sfy = g->ScrnCalibECH.ScrnY / float(pImgWnd1->pPackedDIB1->IHeight);
		for (i = -n; i <= n; i++) {
			int idx1;
			short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
			short len = pImgWnd1->ExtractLine(idx1, pBuf, siz, angle, i * pRcp->TrimWidthSpacing, bX/*,0,255,0*/);
			if (len >= siz) {
				ASSERT(0);
			}
			if (len > 0) {
				PreprocessTrimData(&pBuf, len, TRUE, 6, TRUE, "");
				r = GetTrimWidthOut(pBuf, len, angle, bX, right, idx1);
				*(pfTrimWidth + (cnt++)) = r/*sqrt(x*x + y*y) * sign(r)*/;	// unit in um [4/21/2013 Administrator]
				if (i == 0 && pProfile) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					pProfile->Set(pDspBuf, len);
					pProfile->Invalidate();
				}
				short* p = pBuf;
				if (right < len - 1) {
					for (j = 0; j < right; j++) {
						*(p++) = 0;
					}
					for (j = right; j < len; j++) {
						*(p++) = 255;
					}
					if ((idx1 < len - 2) && (idx1 > 1)) {
						*(pBuf + idx1 - 1) = 128;
						*(pBuf + idx1) = 128;
						*(pBuf + idx1 + 1) = 128;
					}
					p = pBuf + 24;
					*(p++) = 128;
					*(p++) = 128;
					*(p++) = 128;
					*(p++) = 128;
					*(p++) = 128;
				}
				pImgWnd1->ReplaceLine(pBuf, angle, i * pRcp->TrimWidthSpacing, bX);
			}
			if (pBuf) {
				delete[] pBuf;
			}
		}
		r = fRemoveFlyer(cnt, pfTrimWidth, 0, 1);	// unit in mm [4/21/2013 Administrator]
		g->Polar2Cartesian(r, angle, x, y);
		x = x * sfx / g->ScrnCalibECH.ppmX;
		y = y * sfy / g->ScrnCalibECH.ppmY;
		Rout = sqrt(x * x + y * y) * sign(r) * 1e-3 + radius;

		if (pImgWnd1) {
			pImgWnd1->ClearTextInfo();
			if (pRcp->bSaveVideo) {
				pImgWnd1->bStoreComposite = TRUE;
				pImgWnd1->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW2-%d-1.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
			}
			pImgWnd1->Refresh();
		}

		// Goto inner radius [4/20/2013 Administrator]

		if (pVideo2 && pVideo2->IsSimu()) {
			CString dir;
			// 			DosUtil.GetProgramDir(dir);
			// 			dir += "IMAGES\\";
			dir = GetOuterEdge1(angle);
			pVideo2->LoadSimuDIB(dir);
		}

		pVideo2->SnapOneFrame(pRcp->TWExposureInner + Global.BaseExpoEcho);

		pImgWnd2->DupStore1(g->StoredDIB);
		pImgWnd2->Invalidate(TRUE);

		cnt = 0;
		for (i = -n; i <= n; i++) {
			int idx1;
			short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
			short len = pImgWnd2->ExtractLine(idx1, pBuf, siz, angle, i * pRcp->TrimWidthSpacing, bX/*,0,255,0*/);
			if (len >= siz) {
				ASSERT(0);
			}
			if (len > 0) {
				PreprocessTrimData(&pBuf, len, TRUE, 6, TRUE, "");
				r = GetTrimWidthIn(pBuf, len, angle, bX, left, idx1);
				*(pfTrimWidth + (cnt++)) = r/*sqrt(x*x + y*y) * sign(r)*/;	// unit in um [4/21/2013 Administrator]
				if (i == 0 && pProfile) {
					short* pDspBuf = new short[len + 1];
					memcpy(pDspBuf, pBuf, len * sizeof(short));
					pProfile->Set(pDspBuf, len);
					pProfile->Invalidate();
				}
				short* p = pBuf;
				for (j = 0; j <= left; j++) {
					*(p++) = 0;
				}
				for (j = left + 1; j < len; j++) {
					*(p++) = 255;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128;
					*(pBuf + idx1) = 128;
					*(pBuf + idx1 + 1) = 128;
				}
				p = pBuf + 24;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				*(p++) = 128;
				pImgWnd2->ReplaceLine(pBuf, angle, i * pRcp->TrimWidthSpacing, bX);
			}
			if (pBuf) {
				delete[] pBuf;
			}
		}
		r = fRemoveFlyer(cnt, pfTrimWidth, 0, 1);	// unit in mm [4/21/2013 Administrator]
		Global.Polar2Cartesian(r, angle, x, y);
		sfx = g->ScrnCalibECH.ScrnX / float(pImgWnd2->pPackedDIB1->IWidth);
		sfy = g->ScrnCalibECH.ScrnY / float(pImgWnd2->pPackedDIB1->IHeight);
		x = x * sfx / g->ScrnCalibECH.ppmX;
		y = y * sfy / g->ScrnCalibECH.ppmY;
		Rin = sqrt(x * x + y * y) * sign(r) * 1e-3 + radius;
		delete[] pfTrimWidth;
		S.TW2 = TrimW = Rout - Rin;

		if (pImgWnd2) {
			pImgWnd2->ClearTextInfo();
			if (pRcp->bSaveVideo) {
				pImgWnd2->bStoreComposite = TRUE;
				pImgWnd2->SaveName.Format("C:\\TEMP\\%s\\%s-%s-%d\\WTW2-%d-2.BMP", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID, pMP->Co.n);
			}
			pImgWnd2->Refresh();
		}
	}

	// ##### Store result here [4/16/2013 Administrator]
	CData* d = pMP->GetData(0, TRUE);
	if (!d) {
		msg = "CData error";
		Message(pList, msg);
		return FALSE;
	}

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	float val = TrimW + pM->Off + Global.BevelWidth + pMP->m_fvalOffset;	// added value offset [10/6/2020]
	if (pM->Prb == MParam::WTRIMPRB2) {
		d->Set(MeSetX + kI, val);	// Trim width [8/30/2011 C2C]
	}
	if (pM->Prb == MParam::WTRIMPRB3) {
		d->Set(MeSetX + kI, val);	// Trim width [8/30/2011 C2C]
	}
	msg.Format("TW2: %.3f", val);;
	Message(pList, msg);
	return 0;
}