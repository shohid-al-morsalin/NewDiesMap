// EchoProbeBase.cpp: implementation of the CEchoProbeBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EchoProbeBase.h"

#include <math.h>

#include "GlobalDeviceContainer.h"
#include "..\..\SRC\DOSUtil.h"
#include "..\..\E95\413App.h"
#include "..\..\E95\SICam.h"

#include "EchoPeak.h"
#include "ScanLine.h"

#include "..\..\E95\PeakParam.h"

#include "cbw.h"
#include "..\413\DMCMotorBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int* pfXData = NULL;
int* pfEcho1Data = NULL;
int* pfEcho2Data = NULL;

size_t	m_pointsCnt;

FILE* gpDump = NULL;
BOOL gpOn = FALSE;

void* m_lpReadingErrorParam = NULL;
void* m_lpReadingEndParam = NULL;
void* m_lpCalculatingEndParam = NULL;

HANDLE m_hMeasureEndEvent = NULL;
HANDLE m_hCalculatingEvent = NULL;

short REF_PEAK = 1;
short MAX_LAYER_NUMBERS = 10; // normally less than 8

//int NUMBER_OF_INTEREST_DATASETS = 140625;
//int EPMAX_RATE = 4050000L;

//int NUMBER_OF_INTEREST_DATASETS = 190000;
//int EPMAX_RATE = 5400000L;

//int NUMBER_OF_INTEREST_DATASETS = 170000;
//int EPMAX_RATE = 4600000L;

int NUMBER_OF_INTEREST_DATASETS = DEFDATASET;
int EPMAX_RATE = DEFFREQ;

//int EPMAX_RATE = 10000000L;
//int EPMAX_RATE = 5700000L;

BOOL bSaveSpec1 = FALSE;
BOOL bSaveSpec2 = FALSE;
BOOL bSaveSpec3 = FALSE;
BOOL bSaveSpec31 = FALSE;
BOOL bSaveSpec4 = FALSE;
BOOL bSaveSpec5 = FALSE;

FILE* pDmpPeakT = NULL;
FILE* pDmpPeakB = NULL;

CEchoProbeBase::CEchoProbeBase() {
	bSimu = TRUE;
	SimuPkNum = 0;
	//pGenMessage = NULL;

	//m_hMeasureEndEvent = NULL;
	//m_hCalculatingEvent = NULL;

	//nStatus = IDLE_STATUS;

	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);

	bDualProbe = GetPrivateProfileInt("Config", "DualProbe", 1, inifile);

	CRecipe413* rp = GetRecipe();
	if (!rp) {
		// Recipe must exist [8/22/2009 FSM]
		ASSERT(0);
	}

	m_nDirection = 0;
	m_nRangeLevel = BIP5VOLTS;

	//m_lpfnReadingErrorProc = NULL;
	//m_lpfnReadingEndProc = NULL;
	//m_lpfnCalculatingEndProc = NULL;

	m_fThresholdFactor = 16; //adjustable empirical value
	m_fThresholdFactor2 = 16; //adjustable empirical value

	//pPosData = NULL;
	//pPosDataA = NULL;
	//pAChannelData = NULL;
	//pBChannelData = NULL;
	//pSignal1Data = NULL;
	//pSignal2Data = NULL;
	//pfEcho1Data = NULL;
	//pfEcho2Data = NULL;
	//pfXData = NULL;

	//pRawDataArraySpaceA=NULL;	// raw data from the buffer
	//pRawDataArraySpaceB=NULL;	// raw data from the buffer

	//pSLineT = NULL;
	//pSLineB = NULL;
	bSignal = FALSE;

	int i;
	for (i = 0; i < MAXAVEBUFF; i++) {
		fPeak0[TOPPRB][i] = fPeak0[BOTPRB][i] = 0;
	}
	fCalibProbeDistance[0] = fCalibProbeDistance[1] = 0;

	AllocMemory(NUMBER_OF_INTEREST_DATASETS);

	pfThkTBuf = new float[MAXAVEBUFF * MAXPEAKCOUNT];
	pfThkBBuf = new float[MAXAVEBUFF * MAXPEAKCOUNT];

	// Begin: For simulation use [6/1/2012 Yuen]
	for (i = 0; i < 16; i++) {
		PosT[i].Add(0.05f, 0, 1800, 0);
		PosT[i].Add(0.3f, 0, 1800, 0);
		PosT[i].Add(0.8f, 0, 1800, 0);
		PosB[i].Add(0.05f, 0, 1800, 0);
		PosB[i].Add(0.3f, 0, 1800, 0);
		PosB[i].Add(0.8f, 0, 1800, 0);
	}

	// Dual probe  [11/4/2011 Administrator]
// 	PosT[0].Clear();
// 	PosT[0].Add(0.05f,0,1800,0);
// 	PosT[0].Add(0.25f,0,1800,0);
// 	PosT[0].Add(0.7f,0,1800,0);
// 	PosB[0].Clear();
// 	PosB[0].Add(0.05f,0,1800,0);
// 	PosB[0].Add(0.25f,0,1800,0);
// 	PosB[0].Add(0.7f,0,1800,0);

	// Genera[11/4/2011 Administrator]
	float t1 = 0.03f;
	// 	PosT[1].Clear();
	// 	PosT[1].Add(0.05f,0,1800,0);
	// 	PosT[1].Add(0.25f+t1,0,1800,0);
	// 	PosT[1].Add(0.7f-p413App->SRIT*t1+t1,0,1800,0);
	// 	PosB[1].Clear();
	// 	PosB[1].Add(0.05f,0,1800,0);
	// 	PosB[1].Add(0.25f,0,1800,0);
	// 	PosB[1].Add(0.7f-p413App->SRIB*t1,0,1800,0);

	PosT[1].Clear();
	PosT[1].Add(0.05f, 0, 1800, 0);
	PosT[1].Add(0.20f, 150, 1800, 0);
	PosT[1].Add(0.57f, 150, 1800, 0);
	PosT[1].Add(0.7f, 150, 1800, 0);
	PosB[1].Clear();
	PosB[1].Add(0.05f, 0, 1800, 0);
	PosB[1].Add(0.15f, 150, 1800, 0);
	PosB[1].Add(0.47f, 150, 1800, 0);
	PosB[1].Add(0.7f, 150, 1800, 0);

	// GetStepHeightOuter [11/4/2011 Administrator]
	PosT[2].Clear();
	PosT[2].Add(0.05f, 0, 1800, 0);
	PosT[2].Add(0.10f + t1, 0, 1800, 0);
	PosT[2].Add(0.70f, 0, 1000, 0);
	PosB[2].Clear();
	PosB[2].Add(0.05f, 0, 1800, 0);
	PosB[2].Add(0.10f, 0, 1800, 0);
	PosB[2].Add(0.70f, 0, 1000, 0);

	// GetStepHeightInner [11/4/2011 Administrator]
	PosT[3].Clear();
	PosT[3].Add(0.05f, 0, 1800, 0);
	PosT[3].Add(0.10f, 0, 1800, 0);
	PosT[3].Add(0.70f, 0, 1000, 0);
	PosB[3].Clear();
	PosB[3].Add(0.05f, 0, 1800, 0);
	PosB[3].Add(0.10f, 0, 1800, 0);
	PosB[3].Add(0.70f, 0, 1000, 0);

	PosT[4].Clear();
	PosT[4].Add(0.05f, 0, 600, 700);
	PosT[4].Add(0.3f, 50, 600, -700);
	PosT[4].Add(0.8f, 50, 600, -800);
	PosB[4].Clear();
	PosB[4].Add(0.05f, 0, 700, 700);
	PosB[4].Add(0.3f, 50, 700, -900);
	PosB[4].Add(0.9f, 50, 700, -950);

	PosT[5].Clear();
	PosT[5].Add(0.05f, 0, 600, 700);
	PosT[5].Add(0.3f, 50, 600, -700);
	PosT[5].Add(0.4f, 50, 200, -800);
	PosT[5].Add(0.8f, 50, 600, -800);
	PosB[5].Clear();
	PosB[5].Add(0.05f, 0, 700, 700);
	PosB[5].Add(0.3f, 50, 700, -900);
	PosB[5].Add(0.9f, 50, 700, -950);

	// GetStepHeightOuter with errors [11/4/2011 Administrator]
	PosT[6].Clear();
	PosT[6].Add(0.05f, 0, 1000, 0);
	PosT[6].Add(0.25f + t1, 100, 800, -500);
	PosT[6].Add(0.7f - p413App->SRIT * t1 + t1, 800, 500, -300);
	PosB[6].Clear();
	PosB[6].Add(0.05f, 0, 1000, 0);
	PosB[6].Add(0.25f, 0, 800, -500);
	PosB[6].Add(0.7f - p413App->SRIB * t1, 0, 500, -500);

	// GetStepHeightOuter with errors [11/4/2011 Administrator]
	PosT[7].Clear();
	PosT[7].Add(0.05f, 0, 1000, 0);
	PosT[7].Add(0.25f + t1, 0, 800, -850);
	PosT[7].Add(0.7f - p413App->SRIT * t1 + t1, 0, 500, 0);
	PosB[7].Clear();
	PosB[7].Add(0.05f, 0, 1000, 0);
	PosB[7].Add(0.25f, 0, 800, 0);
	PosB[7].Add(0.7f - p413App->SRIB * t1, 0, 500, 0);
	// End: For simulation use [6/1/2012 Yuen]
}

CEchoProbeBase::~CEchoProbeBase() {
	// free memory
	if (pfThkTBuf) {
		delete[] pfThkTBuf;
	}
	if (pfThkBBuf) {
		delete[] pfThkBBuf;
	}
	FreeAllocMemory();
}

// only used when hardware no response
void CEchoProbeBase::ClearEcho(int nBoardNum) {}

// set threshold level, ie. fThreshold * m_fThresholdFactor
void CEchoProbeBase::SetThresholdFactor(float fFactor, float fFactor2) {
	m_fThresholdFactor = fFactor;
	m_fThresholdFactor2 = fFactor2;
}

// start to measure
BOOL CEchoProbeBase::StartMeasurement(BOOL bFilter, float fGuessThick, float fGuessRange) {
	SetStatus(BUSY_STATUS);

	// simulate OnReadingEnd [9/30/2012 Yuen]
	unsigned short* p = pRawDataArraySpaceA;
	pRawDataArraySpaceA = pRawDataArraySpaceB;
	pRawDataArraySpaceB = p;

	GenerateFakeData();

	SetStatus(IDLE_STATUS);

	//signal user
	if (m_lpfnCalculatingEndProc) {
		m_lpfnCalculatingEndProc(10, m_lpCalculatingEndParam);
	}

	return TRUE;
}

// set callback function
void CEchoProbeBase::SetReadingEndProc(lpfnReadingEndProc pFunc, LPVOID lpParam) {
	m_lpfnReadingEndProc = pFunc;
	m_lpReadingEndParam = lpParam;
}

// set callback func
void CEchoProbeBase::SetReadingErrorProc(lpfnReadingErrorProc pFunc, LPVOID lpParam) {
	m_lpfnReadingErrorProc = pFunc;
	m_lpReadingErrorParam = lpParam;
}

// set callback func
void CEchoProbeBase::SetCalculatingEndProc(lpfnCalculatingEndProc pFunc, LPVOID lpParam) {
	m_lpfnCalculatingEndProc = pFunc;
	m_lpCalculatingEndParam = lpParam;
}

// calculate thickness
// Channel should be between 0 and MAXPROBE [10/9/2011 Administrator]
// Do not perform RI correction [11/5/2011 Administrator]
// Unit is micron [5/2/2013 user]
float CEchoProbeBase::fGetThickness(int Chan, int nPeak1, int nPeak2) {
	if (Chan >= MAXPROBE) {
		ASSERT(0);
		return 0;
	}
	if ((nPeak1 >= MAXPEAKCOUNT) || (nPeak2 >= MAXPEAKCOUNT)) {
		return 0;
	}
	if (nPeak1 > nPeak2) {
		int tmp = nPeak1;
		nPeak1 = nPeak2;
		nPeak2 = tmp;
	}

	float v1 = PeaksBuf[Chan].Peaks[nPeak2].fGetCM();
	float v2 = PeaksBuf[Chan].Peaks[nPeak1].fGetCM();

	if ((v1 >= 0) && (v2 >= 0)) {
		return (v1 - v2) / 1.0e4;
	}
	return 0;
}

int CEchoProbeBase::GetNumberOfData() {
	return NUMBER_OF_INTEREST_DATASETS;
}

int CEchoProbeBase::GetPositionData(int* pfPosition, int nPoint, BOOL bRev) {
	if (!pfPosition) {
		ASSERT(pfPosition);
		return 0;
	}
	int nReduced = NUMBER_OF_INTEREST_DATASETS / nPoint;
	if (!bRev) {
		for (int i = 0; i < nReduced; i++) {
			pfPosition[i] = (float)pPosData[i * nPoint];
		}
	}
	else {
		int nn = nReduced - 1;
		for (int i = 0; i < nReduced; i++) {
			pfPosition[i] = (float)pPosData[(nn - i) * nPoint];
		}
	}
	return nReduced;
}

int CEchoProbeBase::GetEcho1Data(int* pfSignal, int* pfPos, int& Xmax, int& Xmin, int nPoint, BOOL bRev) {
	if (!pfSignal) {
		ASSERT(pfSignal);
		return 0;
	}
	int v;
	Xmax = Xmin = -1;

	int nReduced = NUMBER_OF_INTEREST_DATASETS / nPoint;
	if (!bRev) {
		for (int i = 0; i < nReduced; i++) {
			*(pfSignal++) = pSignal1Data[i * nPoint];
			v = pPosData[i * nPoint];
			if (v < Xmin) {
				Xmin = v;
			}
			if (v > Xmax) {
				Xmax = v;
			}
			*(pfPos++) = v;
		}
	}
	else {
		int nn = nReduced - 1;
		for (int i = 0; i < nReduced; i++) {
			*(pfSignal++) = pSignal1Data[(nn - i) * nPoint];
			v = pPosData[(nn - i) * nPoint];
			if (v < Xmin) {
				Xmin = v;
			}
			if (v > Xmax) {
				Xmax = v;
			}
			*(pfPos++) = v;
		}
	}
	return nReduced;
}

int CEchoProbeBase::GetEcho2Data(int* pfIndex, int* pfPos, int& Xmax, int& Xmin, int nPoint, BOOL bRev) {
	if (!pfIndex) {
		ASSERT(pfIndex);
		return 0;
	}

	int v;
	Xmax = Xmin = -1;
	int nReduced = NUMBER_OF_INTEREST_DATASETS / nPoint;
	if (!bRev) {
		for (int i = 0; i < nReduced; i++) {
			*(pfIndex++) = pSignal2Data[i * nPoint];
			v = pPosData[i * nPoint];
			if (v < Xmin) {
				Xmin = v;
			}
			if (v > Xmax) {
				Xmax = v;
			}
			*(pfPos++) = v;
		}
	}
	else {
		int nn = nReduced - 1;
		for (int i = 0; i < nReduced; i++) {
			*(pfIndex++) = pSignal2Data[(nn - i) * nPoint];
			v = pPosData[(nn - i) * nPoint];
			if (v < Xmin) {
				Xmin = v;
			}
			if (v > Xmax) {
				Xmax = v;
			}
			*(pfPos++) = v;
		}
	}
	return nReduced;
}

void CEchoProbeBase::GetAChannelData(int* pfChannel, int nPoint, BOOL bRev) {
	for (int i = 0; i < nPoint; i++) {
		pfChannel[i] = pAChannelData[i];
	}
}

void CEchoProbeBase::GetBChannelData(int* pfChannel, int nPoint, BOOL bRev) {
	for (int i = 0; i < nPoint; i++) {
		pfChannel[i] = pBChannelData[i];
	}
}

//void CEchoProbeBase::SetBoardNumber(int nBoardNum)
//{
//	// No longer necessary [7/18/2012 Administrator]
//	p413App->MCBoardNo/*m_nBoardNum*/ = nBoardNum;
//}

void CEchoProbeBase::FindPeak2(CRecipe413* pRcp, int Chan, float Baseline, CPeaksBuf* pPeaksBuf, CScanLine* pL, int* pP, unsigned short* pS, short MeSet) {
	if (!pRcp || !pL) {
		ASSERT(0);
		return;
	}
	if ((Chan < 0) || (Chan > 1)) {
		ASSERT(0);
		return;
	}

	int i;
	int Width;
	short Thres, Level;
	pPeaksBuf->ResetAll();

	C413Global* g = &p413App->Global;
	CMeParam* Me;
	Me = &pRcp->MeParam[MeSet - 1];

	Thres = Me->PSG[Chan].Thres;
	Width = Me->PSG[Chan].Width + g->PSWidthOff[Chan];  // Unit in 0.1 micron [5/3/2010 Valued Customer]
	Level = Me->PSG[Chan].Level + g->PSLevelOff[Chan];
	PkNear = Me->PSG[Chan].Near;
	PkSmooth = Me->PSG[Chan].Smooth;

	pL->Shrink(0);	// Find all +ve peaks of interference [3/22/2013 Yuen]
	// left peaks in y2

	if (bSaveSpec2) {
		CString str;
		str.Format("C:\\TEMP\\%s-%d-SHRINK1-%d.CSV", p413App->Global.ImageTitle.GetBuffer(), p413App->Global.ImageCount, Chan);
		pL->Dump(str);
		//		SLShrink1[Chan].Dup(pL);
	}
	pL->Reduce(Baseline);	// Reduce data set by removing all zero values points, resulting in envelope of signal [3/22/2013 Yuen]
	if (bSaveSpec2) {
		CString str;
		str.Format("C:\\TEMP\\%s-%d-REDUCE-%d.CSV", p413App->Global.ImageTitle.GetBuffer(), p413App->Global.ImageCount, Chan);
		pL->Dump(str);
		//		SLReduce[Chan].Dup(pL);
	}

	// Smooth out the envelope signal [3/22/2013 Yuen]
	for (i = 0; i < PkSmooth; i++) {
		pL->Smooth(Baseline);
	}

	pL->ZeroY2();	// Make sure Y2 is clean [3/22/2013 Yuen]

	if (bSaveSpec2) {
		CString str;
		str.Format("C:\\TEMP\\%s-%d-SMOOTH-%d.CSV", p413App->Global.ImageTitle.GetBuffer(), p413App->Global.ImageCount, Chan);
		pL->Dump(str);
		//		SLSmooth[Chan].Dup(pL);
		if (Chan == 1) {
			bSaveSpec2 = FALSE;
		}
	}

	pL->Shrink(6);	// Obtain the peak position of envelope and put in Y2 [3/22/2013 Yuen]

	if (bSaveSpec3) {
		CString str;
		str.Format("C:\\TEMP\\%s-%d-SHRINK2-%d.CSV", p413App->Global.ImageTitle.GetBuffer(), p413App->Global.ImageCount, Chan);
		pL->Dump(str);
		//		SLShrink2[Chan].Dup(pL);
		if (Chan == 1) {
			bSaveSpec3 = FALSE;
		}
	}

	pL->Clean(Chan, &pRcp->MeParam[MeSet - 1]);

	if (bSaveSpec31) {
		CString str;
		str.Format("C:\\TEMP\\%s-%d-CLEAN-%d.CSV", p413App->Global.ImageTitle.GetBuffer(), p413App->Global.ImageCount, Chan);
		pL->Dump(str);
		//		SLClean[Chan].Dup(pL);
		if (Chan == 1) {
			bSaveSpec31 = FALSE;
		}
	}

	int Tp = 0;
	CScanPoint* pD = pL->pData;
	short ThresB = Baseline + Thres;

	CEchoPeak* pEp = &pPeaksBuf->Peaks[0];

	for (i = 0; i < pL->Idx; i++) {
		//            |
		//           ||||
		//         ||||||||
		//-------|||||||||||||--------
		//            !
		//            !
		//         Position/Index         : Position=encoder position, index=array position of data set
		//             !
		//             CM
		// Range of y1 and y2 start from 0 to 2048 [3/22/2013 Yuen]
		if ((Tp < MAXPEAKCOUNT) && (pD->y2 > Baseline) && (pD->y1 > ThresB))  // pD->y2 is either 2047 or 2600 [6/13/2011 XPMUser]
		{
			pEp->SetPosition(pD->x);	// Position of peak maximal (fXMaxima) [11/30/2010 XPMUser]
			pEp->SetIndex(pD->n);		// index of peak maximal (iXMaxima)
			pEp->SetHeight(pD->y1 - Baseline);  // Height at fXMaxima (YMaxima) pD->y1 can range from 0 to 2048 value [6/13/2011 XPMUser]
			switch (Chan) {
			case 0:
				if (Me->PS1[Tp].bUse) {
					//pL->GetCM(i,Me->PS1[Tp].Width*1e3,Me->PS1[Tp].Thres+Level+Baseline,pP,pS);
					pL->GetCMv2(i, (Me->PS1[Tp].Width + p413App->Global.PSWidthOff[Chan]) * 1e3, Me->PS1[Tp].Thres + Level + Baseline);
				}
				else {
					//pL->GetCM(i,Width*1e3,Thres+Level+Baseline,pP,pS);
					pL->GetCMv2(i, Width * 1e3, Thres + Level + Baseline);
				}
				//pEp->SetMid(pL->Mid);
				//pEp->SetCM(pL->iCM,pL->CM);
				//pEp->iLL = pL->iX1;
				//pEp->iRL = pL->iX2;
				break;
			case 1:
				if (Me->PS2[Tp].bUse) {
					//pL->GetCM(i,Me->PS2[Tp].Width*1e3,Me->PS2[Tp].Thres+Level+Baseline,pP,pS);
					pL->GetCMv2(i, (Me->PS2[Tp].Width * 1e3 + p413App->Global.PSWidthOff[Chan]), Me->PS2[Tp].Thres + Level + Baseline);
				}
				else {
					//pL->GetCM(i,Width*1e3,Thres+Level+Baseline,pP,pS);
					pL->GetCMv2(i, Width * 1e3, Thres + Level + Baseline);
				}
				//pEp->SetMid(pL->Mid);
				//pEp->SetCM(pL->iCM,pL->CM);
				//pEp->iLL = pL->iX1;
				//pEp->iRL = pL->iX2;
				break;
			default:
				ASSERT(0);
				break;
			}
			pEp->SetMid(pL->Mid);
			pEp->SetCM(pL->iCM, pL->CM);
			pEp->iLL = pL->iX1;
			pEp->iRL = pL->iX2;

			switch (Chan) {
			case 0:
				if (pDmpPeakT) {
					fprintf(pDmpPeakT, "%d,%.3f,%d,%.0f,%.0f\n", pEp->iXMaxima, pEp->fXMaxima, pEp->YMaxima, pEp->fCMs, pEp->Mid);
				}
				break;
			case 1:
				if (pDmpPeakB) {
					fprintf(pDmpPeakB, "%d,%.3f,%d,%.0f,%.0f\n", pEp->iXMaxima, pEp->fXMaxima, pEp->YMaxima, pEp->fCMs, pEp->Mid);
				}
				break;
			}
			Tp++;
			pEp++;
		}
		pD++;
	}

	// 	float v1 = PeaksBuf[0].Peaks[2].fGetCM();
	// 	float v2 = PeaksBuf[0].Peaks[1].fGetCM();
	// 	if ((v1-v2) == 0)
	// 	{
	// 		CString str;
	// 		str.Format("C:\\TEMP\\%s-%d-CLEAN-%d.CSV", p413App->Global.ImageTitle, p413App->Global.ImageCount,0);
	// 		pL->Dump(str);
	// 		ASSERT(0);
	// 	}

	switch (Chan) {
	case 0:
		if (pDmpPeakT) {
			fprintf(pDmpPeakT, "\n");
		}
		break;
	case 1:
		if (pDmpPeakB) {
			fprintf(pDmpPeakB, "\n");
		}
		break;
	}

	pL->DupY3Y2(Baseline);
	pPeaksBuf->Total = Tp;

	if (bSaveSpec5) {
		CString str;
		str.Format("C:\\TEMP\\%s-%d-FINAL-%d.CSV", p413App->Global.ImageTitle.GetBuffer(), p413App->Global.ImageCount, Chan);
		pL->Dump(str);
		if (Chan == 1) {
			bSaveSpec5 = FALSE;
		}
	}
}

// Entry point to perform a scan from Jobs[9/29/2012 Yuen]
CEchoProbeBase::GETPEAKSERR CEchoProbeBase::GetPeaks(CRecipe413* pRcp, CPeaks& Pk, short nAve, CSICam* pVideo, CListCtrl* pList, MParam* pMPa, short MeSet, CString Name, CPeakParam* PParam) {
	int ii, j, x;
	short ret = 0;	// Only the last value is returned [9/30/2012 Yuen]
	CString MsgStr;

	if (p413App->pDMC) {
		if (!p413App->pDMC->bRun) {
			return GETPEAKSERR::GPNOTSTARTED;
		}
	}

	// nAve should be smaller than MAXAVEBUF [3/28/2012 Yuen]
	if (nAve > MAXAVEBUFF) {
		ASSERT(0);
		nAve = MAXAVEBUFF;
	}

	Pk.Clear();	// Must clear peak even on error return [11/5/2011 Administrator]

	if (!m_hCalculatingEvent) {
		m_hCalculatingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	if (!m_hMeasureEndEvent) {
		m_hMeasureEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	// Buffers to collect all thicknesses for nAve of all peaks per channel [3/28/2012 Yuen]
	memset(pfThkTBuf, 0, nAve * sizeof(float) * MAXPEAKCOUNT);
	memset(pfThkBBuf, 0, nAve * sizeof(float) * MAXPEAKCOUNT);

	BOOL bFound = FALSE;

	if (gpOn) {
		if (gpDump) fprintf(gpDump, "Name,%s\n", Name.GetBuffer(0));
	}

	short nAve1 = 1;
	if ((!PParam) && (!pRcp->bAllOrNothing)) {
		nAve1 = 2 * nAve;
	}
	else {
		nAve1 = nAve;
	}
	if (nAve1 > MAXAVEBUFF) {
		nAve1 = MAXAVEBUFF;
	}

	for (int i = 0; i < MAXAVEBUFF; i++) {
		fPeak0[TOPPRB][i] = fPeak0[BOTPRB][i] = 0;
	}

	Cnt1 = 0;
	Cnt2 = 0;
	Cnt3TB = 0;
	short rtcnt1 = 0, rtcnt2 = 0;
	for (ii = 0; ii < nAve1; ii++) {
		if (p413App->Global.bAborting) {
			return GPPKTOTALERR;
		}
		SetEvent(m_hMeasureEndEvent);
		SetEvent(m_hCalculatingEvent);
		if (!StartGetPeaks(pList)) {
			ClearEcho(p413App->MCBoardNo);
			//p413App->pDMC->Run();
			if (pList) {
				MsgStr = "MC StartGetPeaks, retry";
				p413App->Message(pList, MsgStr);
			}
			rtcnt1++;
			if (rtcnt1 < 12) {
				ii--;
			}
			else {
				// Break out [10/11/2012 Yuen]
				ClearEcho(p413App->MCBoardNo);
				//p413App->pDMC->Run();
				return GPCAPTUREERR;
			}
			continue;  // collect one sweep [5/8/2011 FSMT]
		}
		Sleep(15);  // Initial wait must be shorter than StartGetPeaks() time [3/12/2010 Valued Customer]
		ULONGLONG tick = GetTickCount64();
		BOOL bGood = FALSE;
		while ((GetTickCount64() - tick) < 3000) {
			// Simulation class will return immediately [9/30/2012 Yuen]
			STATUS st = GetStatus();
			if (st == ERROR_STATUS) {
				p413App->Global.bAborting = TRUE;
				if (pList) {
					MsgStr = "MC error, suggest reboot PC(1)";
					p413App->Message(pList, MsgStr);
				}
				ClearEcho(p413App->MCBoardNo);
				//p413App->pDMC->Run();
				return GETPEAKSERR::GPCAPTUREERR;
			}
			if (st == IDLE_STATUS) {
				bGood = TRUE;
				break;
			}
			Sleep(5);
		}
		if (!bGood) {
			// Time out indication bad MC card, do not know whether it can be recover other than reboot [10/10/2012 Yuen]
			if (pList) {
				MsgStr = "MC error, suggest reboot PC(2)";
				p413App->Message(pList, MsgStr);
			}
			ClearEcho(p413App->MCBoardNo);
			//p413App->pDMC->Run();
			continue;
		}

		// DoFindPeak: Extract peaks positions from raw data. Per sweep of motor motion [5/8/2011 FSMT]
		// m_nTotalPeaks[] [5/8/2011 FSMT]
		// m_EchoPeaks.XMaxima [5/8/2011 FSMT]
		// m_EchoPeaks.Index [5/8/2011 FSMT]
		// m_EchoPeaks.YMaxima [5/8/2011 FSMT]
		// m_EchoPeaks.Mid [5/8/2011 FSMT]
		// m_EchoPeaks.CM [5/8/2011 FSMT]
		if (!DoFindPeak(pRcp, MeSet, MsgStr)) {
			rtcnt2++;
			if (rtcnt2 < 16) {
				ii--;
			}
			if (pList) p413App->Message(pList, MsgStr);
			// Retrieve error code from DoFindPeakErr [9/30/2012 Yuen]
			continue;	// AB Encoder error recapture signal, retry [9/27/2012 Yuen]
		}

		if (pVideo) {
			StoreDianosticInfo(pRcp, pVideo, "", ii);
		}

		// Initial data rejection [5/7/2011 FSMT]
		if (PParam) {
			GETPEAKSERR Err = FilterPeaks(PParam);
			if (Err <= GETPEAKSERR::GPPKTOTALERR) {
				// Nothing match, return immediately [10/23/2011 Administrator]
				if (pList) {
					MsgStr = "# Filter peaks not satisfied";
					p413App->Message(pList, MsgStr);
				}
				return Err;
			}
			switch (Err) {
			case GETPEAKSERR::GPTOPHOK:
				ret = 1;
				break;
			case GETPEAKSERR::GPBOTHOK:
				ret = 2;
				break;
			case GETPEAKSERR::GPALLHOK:
				ret = 3;
				break;
			default:
				ret = 0;
				break;
			}
		}
		else {
			// Peak count rejection, if PParam is not present [6/29/2011 XPMUser]
			if (pMPa) {
				for (j = 0; j < MaXPA; j++) {
					short nPeak = pMPa[j].NPeak;
					if (nPeak > 0) {
						if (pMPa[j].Prb == MParam::ECHO1PRB) {
							if (PeaksBuf[TOPPRB].Total < nPeak) {
								// Must return because Total is corrupted [10/25/2011 fsm]
								if (pList) {
									MsgStr = "# Peaks total not satisfied";
									p413App->Message(pList, MsgStr);
								}
								return GETPEAKSERR::GPPKTOTALERR;
							}
						}
						else if (pMPa[j].Prb == MParam::ECHO2PRB) {
							if (PeaksBuf[BOTPRB].Total < nPeak) {
								// Must return because Total is corrupted [10/25/2011 fsm]
								if (pList) {
									MsgStr = "# Peaks total not satisfied";
									p413App->Message(pList, MsgStr);
								}
								return GETPEAKSERR::GPPKTOTALERR;
							}
						}
					}
				}
			}

			// Number of Peaks are guaranteed from this point onward [10/29/2011 FSMT]
			CMeParam* pMeP = &pRcp->MeParam[MeSet - 1];
			// Peak count constrain [10/25/2011 fsm]
			for (int a1 = 0; a1 < 2; a1++) {
				if ((pMeP->NPeak[a1] > 0)) {
					if (pMeP->bEqual[a1]) {
						if (PeaksBuf[a1].Total != pMeP->NPeak[a1]) {
							if (pList) {
								MsgStr = "# Peaks total not satisfied";
								p413App->Message(pList, MsgStr);
							}
							return GETPEAKSERR::GPPKTOTALERR;
						}
					}
					else {
						if (PeaksBuf[a1].Total < pMeP->NPeak[a1]) {
							if (pList) {
								MsgStr = "# Peaks total not satisfied";
								p413App->Message(pList, MsgStr);
							}
							return GETPEAKSERR::GPPKTOTALERR;
						}
					}
				}
			}

			// Check top probe, peak height constrain [9/18/2012 Administrator]
			BOOL bOK = TRUE;
			short NPeak = MAXPEAKCOUNT;
			for (j = 0; j < NPeak; j++) {
				CPeakShape* pS = &pMeP->PS1[j];
				if (pS->bUse) {
					if (pS->Height > 0) {
						if (PeaksBuf[TOPPRB].Peaks[j].YMaxima < pS->Height) {
							if (pRcp->bAllOrNothing) {
								if (pList) {
									MsgStr = "# Peaks height not satisfied";
									p413App->Message(pList, MsgStr);
								}
								return GETPEAKSERR::GPPKHEIGHTERR;
							}
							bOK = FALSE;
							break;
						}
					}
				}
			}
			if (bOK) {
				ret = 1;
			}
			// Check bottom peak [9/18/2012 Administrator]
			bOK = TRUE;
			NPeak = MAXPEAKCOUNT;
			for (j = 0; j < NPeak; j++) {
				CPeakShape* pS = &pMeP->PS2[j];
				if (pS->bUse) {
					if (pS->Height > 0) {
						if (PeaksBuf[BOTPRB].Peaks[j].YMaxima < pS->Height) {
							if (pRcp->bAllOrNothing) {
								if (pList) {
									MsgStr = "# Peaks height not satisfied";
									p413App->Message(pList, MsgStr);
								}
								return GETPEAKSERR::GPPKHEIGHTERR;
							}
							bOK = FALSE;
							break;
						}
					}
				}
			}
			if (bOK) {
				ret += 2;
			}
		}

		// all peak position is relative to peak 1 [3/11/2010 Valued Customer]
		//        |                |               |              |
		//        |                |               |              |
		// Peak#  1                2               3              n
		//     reference
		// reference is peak # 1
		// Echo[x][0] = alway zero, starting point
		// Echo[x][1] = reference to peak 2
		// Echo[x][2] = reference to peak 3
		// Echo[x][n] = reference to peak(n+1)

		if (PeaksBuf[TOPPRB].Total > MAXPEAKCOUNT) PeaksBuf[TOPPRB].Total = MAXPEAKCOUNT;
		if (PeaksBuf[BOTPRB].Total > MAXPEAKCOUNT) PeaksBuf[BOTPRB].Total = MAXPEAKCOUNT;
		Pk.Total[TOPPRB] = PeaksBuf[TOPPRB].Total;
		Pk.Total[BOTPRB] = PeaksBuf[BOTPRB].Total;
		float dPeaks01 = PeaksBuf[TOPPRB].Peaks[0].fGetCM();
		float dPeaks02 = PeaksBuf[BOTPRB].Peaks[0].fGetCM();
		float df0 = (PeaksBuf[TOPPRB].Peaks[1].fGetCM() - dPeaks01) * 1e-4;
		float df1 = (PeaksBuf[BOTPRB].Peaks[1].fGetCM() - dPeaks02) * 1e-4;

		if ((PeaksBuf[TOPPRB].Total > 1) && (PeaksBuf[BOTPRB].Total > 1)) {
			if (fCalibProbeDistance[TOPPRB] == 0) {
				// Used in dual probe calibration [10/29/2011 FSMT]
				fPeak0[TOPPRB][Cnt3TB] = df0;	// distance of probe to surface in micron [5/3/2013 user]
			}
			else {
				// For use in dual probe measurement only [10/29/2011 FSMT]
				fPeak0[TOPPRB][Cnt3TB] = fCalibProbeDistance[0] - df0;	// first peak position relative to first peak position of calibration wafer [11/5/2011 Administrator]
			}
			if (fCalibProbeDistance[BOTPRB] == 0) {
				// Used in dual probe calibration [10/29/2011 FSMT]
				fPeak0[BOTPRB][Cnt3TB] = df1;
			}
			else {
				// For use in dual probe measurement only [10/29/2011 FSMT]
				fPeak0[BOTPRB][Cnt3TB] = fCalibProbeDistance[1] - df1;
			}
			Cnt3TB++;
		}

		// Collect individual Pk(CPeaks) information [9/28/2011 Administrator]
		float* tfT = pfThkTBuf + Cnt1 * MAXPEAKCOUNT;
		*(tfT++) = dPeaks01; // Peak position only [9/28/2011 Administrator]
		// pthkTBuf will be incorrect if peaks in middle disappear and there are more than specified total peaks [10/29/2011 FSMT]
		for (x = 1; x < Pk.Total[TOPPRB]; x++) {
			*(tfT++) = (PeaksBuf[TOPPRB].Peaks[x].fGetCM() - dPeaks01);  // Make the rest of CM relative to first CM [9/28/2011 Administrator]
		}
		// Collect other CPeaks information at first average iteration [9/28/2011 Administrator]
		if (Cnt1 == 0) {
			int kk;
			for (kk = 0; kk < Pk.Total[TOPPRB]; kk++) {
				Pk.Inten[0][kk] = PeaksBuf[TOPPRB].Peaks[kk].YMaxima;
				Pk.fXMaxima[0][kk] = PeaksBuf[TOPPRB].Peaks[kk].fXMaxima * 1e-4f;
				Pk.fMiddle[0][kk] = PeaksBuf[TOPPRB].Peaks[kk].Mid * 1e-4f;
			}
		}
		else {
			// Calculate running average only [10/29/2011 FSMT]
			int kk;
			for (kk = 0; kk < Pk.Total[TOPPRB]; kk++) {
				Pk.Inten[0][kk] += PeaksBuf[TOPPRB].Peaks[kk].YMaxima;
				Pk.Inten[0][kk] /= 2;
				Pk.fXMaxima[0][kk] += PeaksBuf[TOPPRB].Peaks[kk].fXMaxima * 1e-4f;
				Pk.fXMaxima[0][kk] /= 2.0f;
				Pk.fMiddle[0][kk] += PeaksBuf[TOPPRB].Peaks[kk].Mid * 1e-4f;
				Pk.fMiddle[0][kk] /= 2.0f;
			}
		}
		Cnt1++;

		// Collect individual Pk(CPeaks) information [9/28/2011 Administrator]
		float* tfB = pfThkBBuf + Cnt2 * MAXPEAKCOUNT;
		*(tfB++) = dPeaks02; // Peak position only [9/28/2011 Administrator]
		// pthkTBuf will be incorrect if peaks in middle disappear and there are more than specified total peaks [10/29/2011 FSMT]
		for (x = 1; x < Pk.Total[BOTPRB]; x++) {
			*(tfB++) = (PeaksBuf[BOTPRB].Peaks[x].fGetCM() - dPeaks02);  // Make the rest of CM relative to first CM [9/28/2011 Administrator]
		}
		// Collect other CPeaks information at first average iteration [9/28/2011 Administrator]
		if (Cnt2 == 0) {
			int kk;
			for (kk = 0; kk < Pk.Total[BOTPRB]; kk++) {
				Pk.Inten[1][kk] = PeaksBuf[BOTPRB].Peaks[kk].YMaxima;
				Pk.fXMaxima[1][kk] = PeaksBuf[BOTPRB].Peaks[kk].fXMaxima * 1e-4f;
				Pk.fMiddle[1][kk] = PeaksBuf[BOTPRB].Peaks[kk].Mid * 1e-4f;
			}
		}
		else {
			// Calculate running average only [10/29/2011 FSMT]
			int kk;
			for (kk = 0; kk < Pk.Total[BOTPRB]; kk++) {
				Pk.Inten[1][kk] += PeaksBuf[BOTPRB].Peaks[kk].YMaxima;
				Pk.Inten[1][kk] /= 2;
				Pk.fXMaxima[1][kk] += PeaksBuf[BOTPRB].Peaks[kk].fXMaxima * 1e-4f;
				Pk.fXMaxima[1][kk] /= 2.0f;
				Pk.fMiddle[1][kk] += PeaksBuf[BOTPRB].Peaks[kk].Mid * 1e-4f;
				Pk.fMiddle[1][kk] /= 2.0f;
			}
		}
		Cnt2++;

		if (gpOn) {
			if (gpDump) Pk.Dump(gpDump, ii);
		}

		if (!PParam && !pRcp->bAllOrNothing) {
			if ((Cnt1 >= nAve) && (Cnt2 >= nAve)) {
				// Has reached the desired average count
				break;
			}
		}
	}

	// Data conditioning [8/26/2010 XPMUser]
	// Pk.Peaks[?][0] = always 0 [11/30/2010 XPMUser]
	// Pk.Peaks[?][1] = Peak 2 - Peak 1 (reference [11/30/2010 XPMUser]
	// Pk.Peaks[?][2] = Peals 3 - Peak 1 (layer 1) [11/30/2010 XPMUser]
	// ... etc [11/30/2010 XPMUser]
	for (x = 0; x < MAXPEAKCOUNT; x++) {
		Pk.fCM[0][x] = p413App->fRemoveFlyer(Cnt1, pfThkTBuf, x, MAXPEAKCOUNT) * 1e-4;
	}
	for (x = 0; x < MAXPEAKCOUNT; x++) {
		Pk.fCM[1][x] = p413App->fRemoveFlyer(Cnt2, pfThkBBuf, x, MAXPEAKCOUNT) * 1e-4;
	}
	Pk.fCM[0][0] = 0;
	Pk.fCM[1][0] = 0;

	switch (ret) {
	case 1:
		return GETPEAKSERR::GPTOPHOK;
		break;
	case 2:
		return GETPEAKSERR::GPBOTHOK;
		break;
	case 3:
		return GETPEAKSERR::GPALLHOK;
		break;
	}

	return GETPEAKSERR::GPPKTOTALERR;
}

BOOL CEchoProbeBase::SetupEvents(int& ErrStatus, short BoardNum) {
	ErrStatus = NOERRORS;
	return TRUE;
}

// The first two channels save the data of position(A and B channel), the 3rd saves the index signal,
// the 4th saves the Echo signal, the 5th is the trigger signal
//			 ____    ____
//			|   |   |   |
//   A   ___|   |___|   |____
//		       ___     ____
//	          |   |   |   |
//   B     ___|   |___|   |____
//
//			| | | | | | |      here are the marks of the incremental points
//
// if one of the channel level changes, increment the position counter

// DataArray [ChnA(2 bytes), ChnB (2 bytes), Trigger (2 bytes), Signal A(2 bytes), Signal B(2 bytes) [3/20/2010 Valued Customer]

BOOL CEchoProbeBase::GetAbsolutePosition(short direction) {
	int i, j;
	int* pP = pPosDataA;
	unsigned short* pA = pAChannelData;
	unsigned short* pB = pBChannelData;

	int OldPos = 0;
	int nStep = 1000;
	if (direction > 0) {
		pP += NUMBER_OF_INTEREST_DATASETS - 1;
		pA += NUMBER_OF_INTEREST_DATASETS - 1;
		pB += NUMBER_OF_INTEREST_DATASETS - 1;
		*(pP--) = OldPos;
	}
	else {
		*(pP++) = OldPos;
	}
	int nAChannelLevel = *pA, nBChannelLevel = *pB;
	if (direction > 0) {
		pA--; pB--;
	}
	else {
		pA++; pB++;
	}

	int cnt = 1;
	for (i = 1; i < NUMBER_OF_INTEREST_DATASETS; i++) {
		if ((*(pA) == nAChannelLevel) && (*(pB) == nBChannelLevel)) {
			cnt++;
		}
		else {
			nAChannelLevel = *pA;
			nBChannelLevel = *pB;
			float sf = nStep / float(cnt);
			if (direction > 0) {
				for (j = 1; j < cnt; j++) {
					*(pP--) = OldPos + j * sf;
				}
				cnt = 1;
				OldPos += nStep;
				*(pP--) = OldPos;
			}
			else {
				for (j = 1; j < cnt; j++) {
					*(pP++) = OldPos + j * sf;
				}
				cnt = 1;
				OldPos += nStep;
				*(pP++) = OldPos;
			}
		}
		if (direction > 0) {
			pA--; pB--;
		}
		else {
			pA++; pB++;
		}
	}

	// Last mile [3/23/2010 Valued Customer]
	if (cnt > 1) {
		float sf = nStep / float(cnt);
		if (direction > 0) {
			for (j = 1; j < cnt; j++) {
				*(pP--) = OldPos + j * sf;
			}
		}
		else {
			for (j = 1; j < cnt; j++) {
				*(pP++) = OldPos + j * sf;
			}
		}
	}

	// Smooth position data with 5X window [11/16/2011 Administrator]
	float sum = 0;
	int* p1 = pPosDataA;
	int* p2 = p1 + 2;
	int* p3 = p1 + 5;
	int* p4 = pPosData;

	sum = *(p1)+*(p1 + 1) + *(p1 + 2) + *(p1 + 3) + *(p1 + 4);
	*(p4++) = sum / 5;
	*(p4++) = sum / 5;
	*(p4++) = sum / 5;

	for (i = 3; i < NUMBER_OF_INTEREST_DATASETS - 2; i++) {
		sum = sum - *(p1++) + *(p3++);
		*(p4++) = sum / 5;
	}
	*(p4++) = sum / 5;
	*(p4++) = sum / 5;

	if (bSaveSpec4) {
		bSaveSpec4 = FALSE;
		pA = pAChannelData;
		pB = pBChannelData;
		pP = pPosData;
		CString name;
		name.Format("C:\\TEMP\\%s-%d-7.CSV", p413App->Global.ImageTitle.GetBuffer(), p413App->Global.ImageCount);
		FILE* fp = fopen(name, "wb");
		if (fp) {
			fprintf(fp, "CHNA,CHNB,POS\r\n");
			for (i = 0; i < NUMBER_OF_INTEREST_DATASETS; i++) {
				fprintf(fp, "%d,%d,%d\r\n", *(pA++), *(pB++), *(pP++));
			}
			fclose(fp);
		}
	}
	return TRUE;
}

// find the direction of motion, we hope the direction is the same during the whole
// measurement procedure, but this is not true if the trigger signal is not correct.
BOOL CEchoProbeBase::FindStrokeDirection(short& Directn) {
	// Simulation alway return -1 [9/29/2012 Yuen]
	Directn = -1;
	return TRUE;
}

BOOL CEchoProbeBase::DoFindPeak(CRecipe413* pRcp, short MeSet, CString& MsgStr) {
	DoFindPeakErr = DOFINDPEAKERR::DFPNONE;
	SetStatus(CALCULATING_STATUS);
	TTLA = 3072;

	if (!FindStrokeDirc(m_nDirection)) {
		switch (DoFindPeakErr) {
		case DOFINDPEAKERR::DFPSTROKEDIRC:
			MsgStr = "# Check encoder reading";
			break;
		default:
			MsgStr = "# Unknown error";
			break;
		}
		return FALSE;
	}

	FindPeaks(pRcp, MeSet);

	SetStatus(STATUS::IDLE_STATUS);
	return TRUE;
}

void CEchoProbeBase::SaveSpec(CString Name, int counter) {
	Name.Format("C:\\TEMP\\%s%d-1.CSV", Name.GetBuffer(), counter);
	FILE* fp = fopen(Name.GetBuffer(), "wb");
	if (fp) {
		unsigned short* pS1, * pS2, * pA, * pB/*, *p*/;
		int* pP;
		pS1 = pSignal1Data;
		pS2 = pSignal2Data;
		pA = pAChannelData;
		pB = pBChannelData;
		pP = pPosData;
		fprintf(fp, "Pos,Top,Bottom,A,B\r\n");
		for (int i = 0; i < NUMBER_OF_INTEREST_DATASETS; i++) {
			fprintf(fp, "%d,%d,%d,%d,%d\r\n", *pP, *pS1, *pS2, *pA, *pB);
			pP++;
			pS1++;
			pS2++;
			pA++;
			pB++;
		}
		fclose(fp);
	}
}

CRecipe413* CEchoProbeBase::GetRecipe() {
	if (!pRecipe) {
		ASSERT(0);
		return NULL;
	}
	return pRecipe;
}

void CEchoProbeBase::SetRecipe(CRecipe413* rp) {
	pRecipe = rp;
}

void CEchoProbeBase::RemoveFlyer(float* thk1, short* cnt1, short nA1, float* thk2, short* cnt2, short nA2) {
	int n1, n2, j, jj;
	float Max = 0, Min = 0, Ave, t, Max1 = 0, Min1 = 0;
	for (j = 0; j <= nA1; j++) {
		n1 = 0;
		n2 = 0;
		for (int i = 0; i < cnt1[j]; i++) {
			jj = i * MAXPEAKCOUNT + j;
			if (i == 0) {
				t = *(thk1 + jj);
				Ave = Max = Min = Max1 = Min1 = t;
			}
			else {
				t = *(thk1 + jj);
				Ave += t;
				if (Max < t) {
					Max1 = Max;
					Max = t;
					n1 = i;
				}
				if (Min > t) {
					Min1 = Min;
					Min = t;
					n2 = i;
				}
			}
		}
		Ave = Max - Min;
		if (fabs(Max - Ave) > fabs(Min - Ave)) {
			*(thk1 + n1 * MAXPEAKCOUNT + j) = *(thk1 + (cnt1[j] - 1) * MAXPEAKCOUNT + j);
			//Max = Max1;
			Ave = Max1 - Min;
		}
		else {
			*(thk1 + n2 * MAXPEAKCOUNT + j) = *(thk1 + (cnt1[j] - 1) * MAXPEAKCOUNT + j);
			//Min = Min1;
			Ave = Max - Min1;
		}
		RangeT[j] = Ave;
		cnt1[j]--;
	}
	Max = Min = 0;
	Max1 = Min1 = 0;
	for (j = 0; j <= nA2; j++) {
		n1 = 0;
		n2 = 0;
		for (int i = 0; i < cnt2[j]; i++) {
			jj = i * MAXPEAKCOUNT + j;
			if (i == 0) {
				t = *(thk2 + jj);
				Ave = Max = Min = Max1 = Min1 = t;
			}
			else {
				t = *(thk2 + jj);
				Ave += t;
				if (Max < t) {
					Max1 = Max;
					Max = t;
					n1 = i;
				}
				if (Min > t) {
					Min1 = Min;
					Min = t;
					n2 = i;
				}
			}
		}
		Ave = Max - Min;
		if (fabs(Max - Ave) > fabs(Min - Ave)) {
			*(thk2 + n1 * MAXPEAKCOUNT + j) = *(thk2 + (cnt2[j] - 1) * MAXPEAKCOUNT + j);
			Ave = Max1 - Min;
		}
		else {
			*(thk2 + n2 * MAXPEAKCOUNT + j) = *(thk2 + (cnt2[j] - 1) * MAXPEAKCOUNT + j);
			Ave = Max - Min1;
		}
		RangeB[j] = Ave;
		cnt2[j]--;
	}
}

float CEchoProbeBase::GetAverage(float* thk, short cnt, short j) {
	float Ave = 0;
	for (int i = 0; i < cnt; i++) {
		Ave += *(thk + i * MAXPEAKCOUNT + j);
	}
	return Ave / float(cnt);
}

float CEchoProbeBase::GetRange(float* thk, short cnt, short j) {
	float Max, Min;
	Max = Min = *thk;
	for (int i = 1; i < cnt; i++) {
		float v = *(thk + i * MAXPEAKCOUNT + j);
		if (v > Max) {
			Max = v;
		}
		if (v < Min) {
			Min = v;
		}
	}
	return Max - Min;
}

void CEchoProbeBase::SimuRemoveFlyer() {
	int i, j, nAve = 8;
	short cnt1[MAXPEAKCOUNT];
	short cnt2[MAXPEAKCOUNT];
	for (j = 0; j < MAXPEAKCOUNT; j++) {
		cnt1[j] = cnt2[j] = nAve;
	}
	float* thk1, * thk2;
	thk1 = new float[nAve * MAXPEAKCOUNT];
	thk2 = new float[nAve * MAXPEAKCOUNT];

	for (i = 0; i < nAve; i++) {
		for (j = 0; j < MAXPEAKCOUNT; j++) {
			*(thk1 + i * MAXPEAKCOUNT + j) = 100.0f * rand() / float(RAND_MAX);
			*(thk2 + i * MAXPEAKCOUNT + j) = 100.0f * rand() / float(RAND_MAX);
		}
	}

	FILE* fp = fopen("C:\\TEMP\\THK1.CSV", "wt");
	if (fp) {
		for (i = 0; i < nAve; i++) {
			for (j = 0; j < MAXPEAKCOUNT; j++) {
				fprintf(fp, "%.2f,", *(thk1 + i * MAXPEAKCOUNT + j));
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}

	float val11[MAXPEAKCOUNT];
	float val12[MAXPEAKCOUNT];
	float val21[MAXPEAKCOUNT];
	float val22[MAXPEAKCOUNT];

	for (j = 0; j < MAXPEAKCOUNT; j++) {
		val11[j] = GetAverage(thk1, cnt1[j], j);
		val12[j] = GetAverage(thk2, cnt2[j], j);
	}
	RemoveFlyer(thk1, cnt1, MAXPEAKCOUNT, thk2, cnt2, MAXPEAKCOUNT);
	fp = fopen("C:\\TEMP\\THK2.CSV", "wt");
	if (fp) {
		for (i = 0; i < cnt1[0]; i++) {
			for (j = 0; j < MAXPEAKCOUNT; j++) {
				fprintf(fp, "%.2f,", *(thk1 + i * MAXPEAKCOUNT + j));
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}

	RemoveFlyer(thk1, cnt1, MAXPEAKCOUNT, thk2, cnt2, MAXPEAKCOUNT);
	fp = fopen("C:\\TEMP\\THK3.CSV", "wt");
	if (fp) {
		for (i = 0; i < cnt1[0]; i++) {
			for (j = 0; j < MAXPEAKCOUNT; j++) {
				fprintf(fp, "%.2f,", *(thk1 + i * MAXPEAKCOUNT + j));
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
	for (j = 0; j < MAXPEAKCOUNT; j++) {
		val21[j] = GetAverage(thk1, cnt1[j], j);
		val22[j] = GetAverage(thk2, cnt2[j], j);
	}

	delete[] thk1;
	delete[] thk2;
}

BOOL CEchoProbeBase::RangeCheck(float Limit1, short nP1, float Limit2, short nP2) {
	int i;
	for (i = 1; i < nP1 + 1; i++) {
		if (RangeT[i] > Limit1) {
			return FALSE;
		}
	}
	for (i = 1; i < nP2 + 1; i++) {
		if (RangeB[i] > Limit2) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CEchoProbeBase::RangeVerify(short nP1, short nP2) {
	INT i;
	for (i = 1; i < nP1 + 1; i++) {
		if (RangeT1[i] < RangeT[i]) {
			return FALSE;
		}
	}
	for (i = 1; i < nP2 + 1; i++) {
		if (RangeB1[i] < RangeB[i]) {
			return FALSE;
		}
	}
	return TRUE;
}

void CEchoProbeBase::DupRange() {
	for (int i = 0; i < MAXPEAKCOUNT; i++) {
		RangeT1[i] = RangeT[i];
		RangeB1[i] = RangeB[i];
	}
}

BOOL CEchoProbeBase::StartGetPeaks(CListCtrl* pList) {
	SetStatus(BUSY_STATUS);

	// Instate of collecting data from card, we manufacture scan data here [9/30/2012 Yuen]
	unsigned short* p = pRawDataArraySpaceA;
	// Swap buffer [4/6/2010 Valued Customer]
	pRawDataArraySpaceA = pRawDataArraySpaceB;
	pRawDataArraySpaceB = p;

	GenerateFakeData();

	if (!pList) {
		SetStatus(IDLE_STATUS);
		return TRUE;
	}
	SetStatus(IDLE_STATUS);
	return TRUE;

	float v = (rand() / float(RAND_MAX));
	if (v > 0.6) {
		SetStatus(IDLE_STATUS);
		return TRUE;
	}

	CString MsgStr;
	if (v < 0.25) {
		SetStatus(ERROR_STATUS);
		MsgStr = "MC StartGetPeaks, #1";
		p413App->Message(pList, MsgStr);
	}
	else if (v < 0.5) {
		SetStatus(READING_STATUS);
		MsgStr = "MC StartGetPeaks, #2";
		p413App->Message(pList, MsgStr);
	}
	else {
		MsgStr = "MC StartGetPeaks, #3";
		p413App->Message(pList, MsgStr);
	}
	return FALSE;
}

BOOL CEchoProbeBase::AllocMemory(int Size) {
	FreeAllocMemory();

	BOOL bErr = FALSE;
	pPosData = new int[Size + 256];
	if (!pPosData) {
		bErr = TRUE;
		ASSERT(0);
	}
	if (pPosDataA) {
		delete[] pPosDataA;
	}
	pPosDataA = new int[Size + 256];
	if (!pPosDataA) {
		bErr = TRUE;
		ASSERT(0);
	}
	pAChannelData = new unsigned short[Size + 256];
	if (!pAChannelData) {
		bErr = TRUE;
		ASSERT(0);
	}
	pBChannelData = new unsigned short[Size + 256];
	if (!pBChannelData) {
		bErr = TRUE;
		ASSERT(0);
	}
	pSignal1Data = new unsigned short[Size + 256];
	if (!pSignal1Data) {
		bErr = TRUE;
		ASSERT(0);
	}
	pSignal2Data = new unsigned short[Size + 256];
	if (!pSignal2Data) {
		bErr = TRUE;
		ASSERT(0);
	}
	pfEcho1Data = new int[Size];
	if (!pfEcho1Data) {
		bErr = TRUE;
		ASSERT(0);
	}
	pfEcho2Data = new int[Size];
	if (!pfEcho2Data) {
		bErr = TRUE;
		ASSERT(0);
	}
	pfXData = new int[Size];
	if (!pfXData) {
		bErr = TRUE;
		ASSERT(0);
	}

	pSLineT = new CScanLine;
	if (!pSLineT) {
		bErr = TRUE;
		ASSERT(0);
	}
	else if (!pSLineT->Alloc(Size)) {
		delete pSLineT;
		pSLineT = NULL;
		bErr = TRUE;
	}

	pSLineB = new CScanLine;
	if (!pSLineB) {
		bErr = TRUE;
		ASSERT(0);
	}
	else if (!pSLineB->Alloc(Size)) {
		delete pSLineB;
		pSLineB = NULL;
		bErr = TRUE;
	}

	return !bErr;
}

void CEchoProbeBase::FreeAllocMemory() {
	if (pPosData) {
		delete[] pPosData;
		pPosData = NULL;
	}

	if (pPosDataA) {
		delete[] pPosDataA;
		pPosDataA = NULL;
	}

	if (pAChannelData) {
		delete[] pAChannelData;
		pAChannelData = NULL;
	}
	if (pBChannelData) {
		delete[] pBChannelData;
		pBChannelData = NULL;
	}

	if (pSignal1Data) {
		delete[] pSignal1Data;
		pSignal1Data = NULL;
	}

	if (pSignal2Data) {
		delete[] pSignal2Data;
		pSignal2Data = NULL;
	}

	if (pfEcho1Data) {
		delete[] pfEcho1Data;
		pfEcho1Data = NULL;
	}
	if (pfEcho2Data) {
		delete[] pfEcho2Data;
		pfEcho2Data = NULL;
	}

	if (pfXData) {
		delete[] pfXData;
		pfXData = NULL;
	}

	if (pSLineT) {
		delete pSLineT;
		pSLineT = NULL;
	}
	if (pSLineB) {
		delete pSLineB;
		pSLineB = NULL;
	}
}

float CEchoProbeBase::CalcBaseline(unsigned short* pS) {
	float sum = 0;

	pS += 256;
	for (int i = 0; i < 256; i++) {
		sum += *(pS++);
	}
	return sum / 256.0f;
}

void CEchoProbeBase::StoreDianosticInfo(CRecipe413* pRcp, CSICam* pVideo, CString Name, int idx) {
	if (pVideo && (!pVideo->bPause)) {
		CSingleLock Lock(&pVideo->CS1);
		Lock.Lock();
		// Only passing pointers, not guarantee to be valid elsewhere [5/8/2011 FSMT]
		ASSERT(pVideo->AllocSLine());
		pVideo->pSLineT->Dup(pSLineT);
		pVideo->pSLineT->CalcMaxMin();
		pVideo->pSLineB->Dup(pSLineB);
		pVideo->pSLineB->CalcMaxMin();
		pVideo->pPeakBuf = &PeaksBuf[TOPPRB];
		Lock.Unlock();
		if (pRcp && pRcp->bSaveVideo && Name.GetLength()) {
			pVideo->bSaveVideo = TRUE;
			pVideo->FileDirc.Format("C:\\TEMP\\%s\\%s-%d-%s\\", CTime::GetCurrentTime().Format("%y%m%d").GetBuffer(), pRcp->CarrierID.GetBuffer(), pRcp->SlotID, pRcp->RcpeName);
			pVideo->FileBase.Format("%s-%d.BMP", Name.GetBuffer(), idx);
		}
		pVideo->bShow = TRUE;
		/* 20120922 SCOTT REF
		   Sometimes, measurement was stopped at below loop.
		   At that time camera doesn't work.
		   To recover, turn off the pc, disconnect camera usb cable,
		   connect usb cable, turn on the pc, execute E95
		*/
		ULONGLONG tick = GetTickCount64();
		while (pVideo->bVideoOK && pVideo->bShow && ((GetTickCount64() - tick) < 3000))  // 3 second time out
		{
			Sleep(1);
		}
	}
}

// Copy raw data into pSLineT and pSLineB [5/6/2013 user]
void CEchoProbeBase::CopyData2(short MeSet, short direction, float& blin1, float& blin2, unsigned short* pA, unsigned short* pB, unsigned short* pS1, unsigned short* pS2) {
	CRecipe413* rp = GetRecipe();
	if (!rp) return;

	int i;
	int* pP = pPosData;
	unsigned short v;
	C413Global* g = &p413App->Global;

	CMeParam* pMeP = &rp->MeParam[MeSet - 1];
	short Level1 = pMeP->PSG[0].Level + g->PSLevelOff[0];
	short Level2 = pMeP->PSG[1].Level + g->PSLevelOff[1];

	pSLineT->Idx = 0;
	pSLineB->Idx = 0;

	int ThresT = 3950;

	blin1 = CalcBaseline(pS1);
	short blin1H = blin1 + Level1;
	short blin1L = blin1 - Level1;
	blin2 = CalcBaseline(pS2);
	short blin2H = blin2 + Level2;
	short blin2L = blin2 - Level2;

	GetAbsolutePosition(direction);

	pSLineT->MaxMinX.iMin = pSLineB->MaxMinX.iMin = 0;
	pSLineT->MaxMinX.iMax = pSLineB->MaxMinX.iMax = *(pPosData + NUMBER_OF_INTEREST_DATASETS - 1);

	for (i = 0; i < NUMBER_OF_INTEREST_DATASETS; i++) {
		v = *(pS1);
		if (v < blin1L) {
			v = blin1 - v;
		}
		if (v > blin1H) {
			pSLineT->Set(*(pP), (v < ThresT) ? v : ThresT, i);
		}

		v = *(pS2);
		if (v < blin2L) {
			v = blin2 - v;
		}
		if (v > blin2H) {
			pSLineB->Set(*(pP), (v < ThresT) ? v : ThresT, i);
		}
		pS1++;
		pS2++;
		pP++;
	}
}

void CEchoProbeBase::OpenDump(FILE** fp, short id) {
	CString name;
	name.Format("C:\\TEMP\\GP%s-%d.CSV", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer(), id);
	*fp = fopen(name, "wb");
	gpOn = TRUE;
}

void CEchoProbeBase::CloseDump(FILE* fp) {
	gpOn = FALSE;
	fclose(fp);
}

// Checks number of peaks and peak height of top and bottom probe [10/23/2011 Administrator]
// return GPPKTOTALERR if number of peaks not satisfied for either top or bottom probe, peak height will not be check in this case [10/23/2011 Administrator]
// return GPFOUND1 if top probe peak height condition is met [10/23/2011 Administrator]
// return GPFOUND2 if bottom probe peak height condition is met [10/23/2011 Administrator]
// return GPFOUND3 if both peak height condition is met [10/23/2011 Administrator]
// PParam must point to valid object, test of PParam for NULL should be done by caller [9/30/2012 Yuen]
CEchoProbeBase::GETPEAKSERR CEchoProbeBase::FilterPeaks(CPeakParam* PParam) {
	short ret = 0;
	if (PParam->nPeakT) {
		// return 0 if number of peaks not satisfied [10/22/2011 Administrator]
		switch (PParam->Method) {
		case CPeakParam::cmEQUAL:
			if (PeaksBuf[TOPPRB].Total != PParam->nPeakT) {
				return GETPEAKSERR::GPPKTOTALERR;
			}
			break;
		case CPeakParam::cmGREATER:
			if (PeaksBuf[TOPPRB].Total < PParam->nPeakT) {
				return GETPEAKSERR::GPPKTOTALERR;
			}
			break;
		default:
			ASSERT(0);
			return GETPEAKSERR::GPPARAMERR;
			break;
		}
	}
	if (PParam->nPeakB) {
		// return 0 if number of peaks not satisfied [10/22/2011 Administrator]
		switch (PParam->Method) {
		case CPeakParam::cmEQUAL:
			if (PeaksBuf[BOTPRB].Total != PParam->nPeakB) {
				return GETPEAKSERR::GPPKTOTALERR;
			}
			break;
		case CPeakParam::cmGREATER:
			if (PeaksBuf[BOTPRB].Total < PParam->nPeakB) {
				return GETPEAKSERR::GPPKTOTALERR;
			}
			break;
		default:
			ASSERT(0);
			return GETPEAKSERR::GPPARAMERR;
			break;
		}
	}
	// will not constrain within total number of peaks  [10/22/2011 Administrator]
	int b;
	BOOL bOK = TRUE;
	CPeaksBuf* pBB = &PeaksBuf[TOPPRB];
	CPeakHeightItem* pPH = &PParam->HeightT[0];
	CPeakDistanceItem* pPD = &PParam->DistanceT[0];
	for (b = 0; b < MAXPKCONSTRAIN; b++) {
		// Top probe peak height check [9/30/2012 Yuen]
		if (pPH->Height && (pPH->Index < MAXPEAKCOUNT)) {
			if (pBB->Peaks[pPH->Index].YMaxima < pPH->Height) {
				bOK = FALSE;
			}
		}
		// Top probe peak distance check [9/30/2012 Yuen]
		if (pPD->Distance && (pPH->Index < MAXPEAKCOUNT)) {
			if (fabs(pBB->Peaks[pPD->Index].fXMaxima - pPD->Distance) > pPD->Delta) {
				bOK = FALSE;
			}
		}
		pPH++; pPD++;
	}
	if (bOK) {
		ret += 1;	// Top probe peak height condition satisfied [10/22/2011 Administrator]
	}
	bOK = TRUE;
	pBB = &PeaksBuf[BOTPRB];
	pPH = &PParam->HeightB[0];
	pPD = &PParam->DistanceB[0];
	for (b = 0; b < MAXPKCONSTRAIN; b++) {
		// Bottom probe peak height check [9/30/2012 Yuen]
		if (pPH->Height && (pPH->Index < MAXPEAKCOUNT)) {
			if (pBB->Peaks[pPH->Index].YMaxima < pPH->Height) {
				bOK = FALSE;
			}
		}
		// Bottom probe peak distance check [9/30/2012 Yuen]
		if (pPD->Distance && (pPH->Index < MAXPEAKCOUNT)) {
			if (fabs(pBB->Peaks[pPD->Index].fXMaxima - pPD->Distance) > pPD->Delta) {
				bOK = FALSE;
			}
		}
		pPH++; pPD++;
	}
	if (bOK) {
		ret += 2;  // Bottom probe peak height condition satisfied [10/22/2011 Administrator]
	}

	switch (ret) {
	case 1:
		return GETPEAKSERR::GPTOPHOK;	// Top probe peak height OK [9/30/2012 Yuen]
		break;
	case 2:
		return GETPEAKSERR::GPBOTHOK;	// Bottom probe peak height OK [9/30/2012 Yuen]
		break;
	case 3:
		return GETPEAKSERR::GPALLHOK;	// Both probe peak height OK [9/30/2012 Yuen]
		break;
	}

	return GETPEAKSERR::GPPKTOTALERR;
}

CEchoProbeBase::GETPEAKSERR CEchoProbeBase::ChkPeaks(CRecipe413* pRcp, CPeaks& Pk, short nAve, MParam* pMPa, short MeSet, CPeakParam* PParam, CSICam* pVideo, CListCtrl* pList, CString Name) {
	int i, j;
	short ret = 0;

	if (!m_hCalculatingEvent) {
		m_hCalculatingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	if (!m_hMeasureEndEvent) {
		m_hMeasureEndEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	CString MsgStr;
	short rtcnt1 = 0, rtcnt2 = 0;
	for (i = 0; i < nAve; i++) {
		if (p413App->Global.bAborting) {
			return GETPEAKSERR::GPPKTOTALERR;
		}
		SetEvent(m_hMeasureEndEvent);
		SetEvent(m_hCalculatingEvent);
		if (!StartGetPeaks(pList)) {
			rtcnt1++;
			if (rtcnt1 < 32) {
				i--;
			}
			else {
				// Break out [10/11/2012 Yuen]
				ClearEcho(p413App->MCBoardNo);
				return GETPEAKSERR::GPCAPTUREERR;
			}
			continue;  // collect one sweep [5/8/2011 FSMT]
		}
		Sleep(15);  // Initial wait must be shorter than StartGetPeaks() time [3/12/2010 Valued Customer]
		ULONGLONG tick = GetTickCount64();
		BOOL bGood = FALSE;
		while ((GetTickCount64() - tick) < 10000) {
			STATUS st = GetStatus();
			if (st == STATUS::ERROR_STATUS) {
				ClearEcho(p413App->MCBoardNo);
				return GETPEAKSERR::GPCAPTUREERR;
			}
			if (st == STATUS::IDLE_STATUS) {
				bGood = TRUE;
				break;
			}
			Sleep(5);
		}
		if (!bGood) {
			// Time out indication bad MC card, do not know wherether it can be recover other than reboot [10/10/2012 Yuen]
			if (pList) {
				MsgStr = "MC error, suggest reboot PC(3)";
				p413App->Message(pList, MsgStr);
			}
			ClearEcho(p413App->MCBoardNo);
			continue;
		}
		if (!DoFindPeak(pRcp, MeSet, MsgStr)) {
			rtcnt2++;
			if (rtcnt2 < 16) {
				i--;
			}
			p413App->Message(pList, MsgStr);
			continue;	// AB Encoder error recapture signal, retry [9/29/2012 Yuen]
		}

		if (pVideo) {
			StoreDianosticInfo(pRcp, pVideo, "", i);
		}

		// Initial data rejection [5/7/2011 FSMT]
		if (PParam) {
			GETPEAKSERR Err = FilterPeaks(PParam);
			if (Err <= GETPEAKSERR::GPPKTOTALERR) {
				// Nothing match, return immediately [10/23/2011 Administrator]
				return Err;
			}
			switch (Err) {
			case GETPEAKSERR::GPTOPHOK:
				ret = 1;
				break;
			case GETPEAKSERR::GPBOTHOK:
				ret = 2;
				break;
			case GETPEAKSERR::GPALLHOK:
				ret = 3;
				break;
			default:
				ret = 0;
				break;
			}
		}
		else {
			// Peak count rejection, if PParam is not present [6/29/2011 XPMUser]
			if (pMPa) {
				for (j = 0; j < MaXPA; j++) {
					short nPeak = pMPa[j].NPeak;
					if (nPeak > 0) {
						if (pMPa[j].Prb == MParam::ECHO1PRB) {
							if (PeaksBuf[TOPPRB].Total < nPeak) {
								return GETPEAKSERR::GPPKTOTALERR;
							}
						}
						else if (pMPa[j].Prb == MParam::ECHO2PRB) {
							if (PeaksBuf[BOTPRB].Total < nPeak) {
								return GETPEAKSERR::GPPKTOTALERR;
							}
						}
					}
				}
			}
			CMeParam* pMeP = &pRcp->MeParam[MeSet - 1];
			for (j = 0; j < PeaksBuf[TOPPRB].Total; j++) {
				CPeakShape* pS = &pMeP->PS1[j];
				if (pS->bUse) {
					if (pS->Height > 0) {
						if (PeaksBuf[TOPPRB].Peaks[j].YMaxima < pS->Height) {
							return GETPEAKSERR::GPPKTOTALERR;
						}
					}
				}
			}
			for (j = 0; j < PeaksBuf[BOTPRB].Total; j++) {
				CPeakShape* pS = &pMeP->PS2[j];
				if (pS->bUse) {
					if (pS->Height > 0) {
						if (PeaksBuf[BOTPRB].Peaks[j].YMaxima < pS->Height) {
							return GETPEAKSERR::GPPKTOTALERR;
						}
					}
				}
			}
			ret = 3;  // All condition checks out OK [10/23/2011 Administrator]
		}
	}
	switch (ret) {
	case 1:
		return GETPEAKSERR::GPTOPHOK;
		break;
	case 2:
		return GETPEAKSERR::GPBOTHOK;
		break;
	case 3:
		return GETPEAKSERR::GPALLHOK;
		break;
	}
	return GETPEAKSERR::GPPKTOTALERR;
}

void CEchoProbeBase::GeneratePeakSine(unsigned short* pS, short n, short height) {
	float n2 = n / 8.0f;
	float sf = PI / float(n);

	for (int i = 0; i < n; i++) {
		float v = i * sf;
		*(pS++) = height * (sin(v) * sin(v * n2)) + 2048;
	}
}

void CEchoProbeBase::GeneratePeakGaussian(unsigned short* pS, short n, short height) {
	float n2 = n / 12.0f;
	float sf = PI / float(n);
	float sigma = 0.60f;
	float k = 1 / (sigma * sqrt(2 * PI));
	float sigma2 = 2 * sigma * sigma;
	float sf2 = 2.0f / float(n - 1);

	for (int i = 0; i < n; i++) {
		float v1 = i * sf;
		float v2 = i * sf2 - 1.0f;
		*(pS++) = height * (k * exp(-v2 * v2 / sigma2) * sin(v1 * n2)) + 2048;
	}
}

void CEchoProbeBase::ReAllocMemory(int size) {
	AllocMemory(size);
}

BOOL CEchoProbeBase::Initialize(CListBox* pMessage) {
	pMessage->InsertString(pMessage->GetCount(), "Echo system (simu) started OK");
	return TRUE;
}

void CEchoProbeBase::GenerateFakeData() {
	unsigned short* pA = pAChannelData;
	unsigned short* pB = pBChannelData;
	unsigned short* pS1 = pSignal1Data;
	unsigned short* pS2 = pSignal2Data;

	int i;
	short A = 0, B = 0;

	// Simulation, instead of copy the captured data, it is generating simulated data here [9/29/2012 Yuen]
	for (i = 0; i < NUMBER_OF_INTEREST_DATASETS; i++) {
		*(pA++) = A;
		*(pB++) = B;
		*(pS1++) = 2048;
		*(pS2++) = 2048;
		switch (i % 20) {
		case 0:
			B = 0;
			break;
		case 2:
			A = 5;
			break;
		case 4:
			B = 10;
			break;
		case 6:
			A = 15;
			break;
		}
	}

	short n = 1200;
	int pos, ht;
	for (i = 0; i < PosT[SimuPkNum].n; i++) {
		CPosHtItem* p = &PosT[SimuPkNum].Pos[i];
		if (p->dP) {
			pos = p->Position * NUMBER_OF_INTEREST_DATASETS + p->dP * rand() / RAND_MAX;
		}
		else {
			pos = p->Position * NUMBER_OF_INTEREST_DATASETS;
		}
		if (p->dH) {
			ht = p->Height + p->dH * rand() / RAND_MAX;
			if (ht < 0) {
				ht = 0;
			}
		}
		else {
			ht = p->Height;
		}
		if ((pos > n) && (pos < NUMBER_OF_INTEREST_DATASETS - n)) {
			GeneratePeakGaussian(pSignal1Data + pos, n, ht);
		}
	}

	for (i = 0; i < PosB[SimuPkNum].n; i++) {
		CPosHtItem* p = &PosB[SimuPkNum].Pos[i];
		if (p->dP) {
			pos = p->Position * NUMBER_OF_INTEREST_DATASETS + p->dP * rand() / RAND_MAX;
		}
		else {
			pos = p->Position * NUMBER_OF_INTEREST_DATASETS;
		}
		if (p->dH) {
			ht = p->Height + p->dH * rand() / RAND_MAX;
			if (ht < 0) {
				ht = 0;
			}
		}
		else {
			ht = p->Height;
		}
		if ((pos > n) && (pos < NUMBER_OF_INTEREST_DATASETS - n)) {
			GeneratePeakGaussian(pSignal2Data + pos, n, ht);
		}
	}
	Sleep(10);
}

BOOL CEchoProbeBase::FindStrokeDirc(short& Directn) {
	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		if (!FindStrokeDirection(Directn)) // Must be the first function to call [3/20/2010 Valued Customer]
		{
			DoFindPeakErr = DFPSTROKEDIRC;
			SetStatus(IDLE_STATUS);
			return FALSE;
		}
	}
	else {
		if (pGDev->SubModel != CGlobalDeviceContainer::ASE) {
			if (!FindStrokeDirection(Directn)) // Must be the first function to call [3/20/2010 Valued Customer]
			{
				DoFindPeakErr = DFPSTROKEDIRC;
				SetStatus(IDLE_STATUS);
				return FALSE;
			}
		}
		else {
			Directn = -1; // Fixed for ASE [9/26/2012 fsm]
			if (bSimu) {
				// Simulation only [9/30/2012 Yuen]
				float v = rand() / float(RAND_MAX);
				if (v < 0.1) {
					DoFindPeakErr = DFPSTROKEDIRC;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

void CEchoProbeBase::FindPeaks(CRecipe413* pRcp, short MeSet) {
	float baseline1 = 2047.5f, baseline2 = 2047.5f;
	unsigned short* pA = pAChannelData;
	unsigned short* pB = pBChannelData;
	unsigned short* pS1 = pSignal1Data;
	unsigned short* pS2 = pSignal2Data;

	if (p413App->bReverseAB) {
		pA = pBChannelData;
		pB = pAChannelData;
	}
	if (p413App->bReverseSignal) {
		pS1 = pSignal2Data;
		pS2 = pSignal1Data;
	}
	CopyData2(MeSet, m_nDirection, baseline1, baseline2, pA, pB, pS1, pS2);
	if (bSaveSpec1) {
		bSaveSpec1 = FALSE;
		SaveSpec(p413App->Global.ImageTitle, p413App->Global.ImageCount);
	}
	// First probe [8/19/2009 FSM]
	FindPeak2(pRcp, 0, baseline1, &PeaksBuf[TOPPRB], pSLineT, pPosData, pS1, MeSet);
	if (bDualProbe) {
		// Second probe [8/19/2009 FSM]
		FindPeak2(pRcp, 1, baseline2, &PeaksBuf[BOTPRB], pSLineB, pPosData, pS2, MeSet);
	}
}