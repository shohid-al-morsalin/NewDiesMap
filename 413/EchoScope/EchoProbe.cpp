// EchoProbe.cpp: implementation of the CEchoProbe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "EchoProbe.h"

#include "GlobalDeviceContainer.h"

#include "Peaks.h"
#include "EchoPeak.h"
#include "ScanLine.h"
#include "ScanPoint.h"

#include "..\..\E95\PeakParam.h"
#include "..\..\E95\413App.h"

#include "cbw.h"

#define TTLTHRESHOLD 3072

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern float px[], py[];

CEchoProbe::CEchoProbe() {
	bSimu = FALSE;
	bEventSet = FALSE;

	pRawDataArraySpaceA = NULL;
	pRawDataArraySpaceB = NULL;

	// Additional hardware related memory [5/15/2011 C2C]
	CEchoProbeBase::AllocMemory(NUMBER_OF_INTEREST_DATASETS);
	AllocMemory(NUMBER_OF_INTEREST_DATASETS);
}

CEchoProbe::~CEchoProbe() {
	ClearEcho(p413App->MCBoardNo);

	if (gpDump) {
		fclose(gpDump);
		gpDump = NULL;
	}
	// free memory
	FreeAllocMemory();
}

// only used when hardware no response
void CEchoProbe::ClearEcho(int nBoardNum) {
	cbStopBackground(nBoardNum, AIFUNCTION);
	cbDisableEvent(nBoardNum, ALL_EVENT_TYPES); // absolute necessary
	bEventSet = FALSE;
	Sleep(100); // Give MC card some time to react [10/16/2012 FSM413]
}

// start to measure
BOOL CEchoProbe::StartMeasurement(/*short MeSet, */BOOL bFilter, float fGuessThick, float fGuessRange) {
	SetStatus(BUSY_STATUS);
	short nCurScanStatus;
	long CurCount, CurIndex;

	short MCBoardNo = p413App->MCBoardNo;

	// Grab a set of data from measurement computing card [9/29/2012 Yuen]
	SetEvent(m_hCalculatingEvent);

	cbGetStatus(MCBoardNo, &nCurScanStatus, &CurCount, &CurIndex, AIFUNCTION);
	if (nCurScanStatus == RUNNING) {
		return FALSE;
	}

	int ErrStatus;
	if (!bEventSet) {
		if (!SetupEvents(ErrStatus, MCBoardNo)) {
			return FALSE;
		}
	}

	long lRate = EPMAX_RATE;
	int AInStatus = cbAInScan(MCBoardNo, 0, 3, NUMBER_OF_INTEREST_DATASETS * 4, &lRate, m_nRangeLevel, (HGLOBAL)pRawDataArraySpaceA, BACKGROUND | EXTTRIGGER | CONVERTDATA);

	if (AInStatus != NOERRORS) {
		cbMemReset(MCBoardNo);
		ClearEcho(MCBoardNo);
		SetStatus(ERROR_STATUS);
		if (m_lpfnReadingErrorProc) {
			m_lpfnReadingErrorProc(AInStatus, m_lpReadingErrorParam);
		}
		if (pGenMessage) {
			CString str;
			str.Format("AIStatus: %d", AInStatus);
			pGenMessage->SetWindowText(str);
		}
		return FALSE;
	}
	return TRUE;
}

// Callback after issuing cbAIScan [9/29/2012 Yuen]
void CALLBACK CEchoProbe::OnReadingEnd(int nBoardNum, unsigned int nEventType, unsigned int nEventData, void* pUserData) {
	CEchoProbe* pEcho = (CEchoProbe*)pUserData;
	if (!pEcho) {
		ASSERT(pEcho);
		return;
	}

	cbStopBackground(nBoardNum, AIFUNCTION);

	if (WaitForSingleObject(m_hCalculatingEvent, INFINITE) == WAIT_OBJECT_0) // This should have no effect and return immediately [9/23/2011 Administrator]
	{
		// Wait until main tread to signal OK to change data [3/22/2010 Valued Customer]
		// Swap buffer [4/6/2010 Valued Customer]
		unsigned short* p = pEcho->pRawDataArraySpaceA;
		pEcho->pRawDataArraySpaceA = pEcho->pRawDataArraySpaceB;
		pEcho->pRawDataArraySpaceB = p;
		pEcho->SetStatus(IDLE_STATUS);
		//signal user
		if (pEcho->m_lpfnCalculatingEndProc) {
			pEcho->m_lpfnCalculatingEndProc(10, m_lpCalculatingEndParam);
		}
	}
}

// when error occurs during data collecting
void CALLBACK CEchoProbe::OnReadingError(int nBoardNum, unsigned int nEventType,
	unsigned int nEventData, void* pUserData) {
	CEchoProbe* pEcho = (CEchoProbe*)pUserData;
	pEcho->ClearEcho(nBoardNum);
	pEcho->SetStatus(ERROR_STATUS);

	//signal user
	if (pEcho->m_lpfnReadingErrorProc) {
		pEcho->m_lpfnReadingErrorProc(nEventData, m_lpReadingErrorParam);
	}
}

// save A B channel data
// direction -1 is forward [3/20/2010 Valued Customer]
// void CEchoProbe::CopyData(short direction)
// {
// 	unsigned short * pA = pAChannelData;
// 	unsigned short * pB = pBChannelData;
// 	unsigned short * pS1 = pSignal1Data;
// 	unsigned short * pS2 = pSignal2Data;
//
// 	if (p413App->bReverseAB)
// 	{
// 		pA = pBChannelData;
// 		pB = pAChannelData;
// 	}
// 	if (p413App->bReverseSignal)
// 	{
// 		pS1 = pSignal2Data;
// 		pS2 = pSignal1Data;
// 	}
//
// 	int i;
// 	unsigned short *p;
//
// 	if (direction == -1)
// 	{
// 		p = pRawDataArraySpaceB;
// 		for (i = 0; i < NUMBER_OF_INTEREST_DATASETS; i++)
// 		{
// 			*(pA++) = (*(p++) > TTLTHRESHOLD)?1:0;
// 			*(pB++) = (*(p++) > TTLTHRESHOLD)?1:0;
// 			*(pS1++) = *(p++);
// 			*(pS2++) = *(p++);
// 		}
// 	}
// 	else
// 	{
// 		p = pRawDataArraySpaceB + 4 * (NUMBER_OF_INTEREST_DATASETS-1);
// 		for (i = (NUMBER_OF_INTEREST_DATASETS-1); i >= 0; i--)
// 		{
// 			*(pA++) = (*(p++) > TTLTHRESHOLD)?1:0;
// 			*(pB++) = (*(p++) > TTLTHRESHOLD)?1:0;
// 			*(pS1++) = *(p++);
// 			*(pS2++) = *(p++);
// 			p-=8;
// 		}
// 	}
//
// 	GetAbsolutePosition(direction);  // always -1 [9/26/2011 C2C]
// }

// Copy raw data into pSLineT and pSLineB [5/6/2013 user]
void CEchoProbe::CopyData2(short MeSet, short direction, float& blin1, float& blin2, unsigned short* pA, unsigned short* pB, unsigned short* pS1, unsigned short* pS2) {
	CRecipe413* rp = GetRecipe();
	if (!rp) return;

	int i;
	int* pP;
	unsigned short* p, v;
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

	if (direction == -1) {
		p = pRawDataArraySpaceB;
		// Extract and binaries encoder information  [9/22/2011 Administrator]
		for (i = 0; i < NUMBER_OF_INTEREST_DATASETS; i++) {
			*(pA++) = (*(p++) > TTLA) ? 1 : 0;
			*(pB++) = (*(p++) > TTLA) ? 1 : 0;
			p += 2;
		}
		GetAbsolutePosition(direction);

		pSLineT->MaxMinX.iMin = pSLineB->MaxMinX.iMin = 0;
		pSLineT->MaxMinX.iMax = pSLineB->MaxMinX.iMax = *(pPosData + NUMBER_OF_INTEREST_DATASETS - 1);

		pP = pPosData;
		p = pRawDataArraySpaceB;

		for (i = 0; i < NUMBER_OF_INTEREST_DATASETS; i++) {
			p += 2;
			*(pS1) = *(p++);
			*(pS2) = *(p++);
			v = *(pS1++);
			if (v < blin1L) {
				v = blin1 - v;
			}
			if (v > blin1H) {
				pSLineT->Set(*(pP), (v < ThresT) ? v : ThresT, i);
			}
			v = *(pS2++);
			if (v < blin2L) {
				v = blin2 - v;
			}
			if (v > blin2H) {
				pSLineB->Set(*(pP), (v < ThresT) ? v : ThresT, i);
			}
			pP++;
		}
	}
	else {
		pA += NUMBER_OF_INTEREST_DATASETS - 1;
		pB += NUMBER_OF_INTEREST_DATASETS - 1;
		p = pRawDataArraySpaceB + 4 * ((NUMBER_OF_INTEREST_DATASETS - 1));
		// Extract and binaries encoder information  [9/22/2011 Administrator]
		for (i = (NUMBER_OF_INTEREST_DATASETS - 1); i >= 0; i--) {
			*(pA--) = (*(p++) > TTLA) ? 1 : 0;
			*(pB--) = (*(p++) > TTLA) ? 1 : 0;
			p -= 6;
		}
		GetAbsolutePosition(direction);
		pS1 += NUMBER_OF_INTEREST_DATASETS - 1;
		pS2 += NUMBER_OF_INTEREST_DATASETS - 1;
		pSLineT->MaxMinX.iMin = pSLineB->MaxMinX.iMin = 0;
		pSLineT->MaxMinX.iMax = pSLineB->MaxMinX.iMax = *(pPosData);
		pP = pPosData + (NUMBER_OF_INTEREST_DATASETS - 1);
		p = pRawDataArraySpaceB + 4 * (NUMBER_OF_INTEREST_DATASETS - 1);
		for (i = NUMBER_OF_INTEREST_DATASETS - 1; i >= 0; i--) {
			p += 2;
			*(pS1) = *(p++);
			*(pS2) = *(p++);
			v = *(pS1--);
			if (v < blin1L) {
				v = blin1 - v;
			}
			if (v > blin1H) {
				pSLineT->Set(*(pP), (v < ThresT) ? v : ThresT, i);
			}
			v = *(pS2--);
			if (v < blin2L) {
				v = blin2 - v;
			}
			if (v > blin2H) {
				pSLineB->Set(*(pP), (v < ThresT) ? v : ThresT, i);
			}
			pP--;
			p -= 8;
		}
	}
}

// find the direction of motion, we hope the direction is the same during the whole
// measurement procedure, but this is not true if the trigger signal is not correct.
BOOL CEchoProbe::FindStrokeDirection(short& Directn) {
	if (!pRawDataArraySpaceB) {
		ASSERT(pRawDataArraySpaceB);
		return FALSE;
	}
	short a = 0;
	short b = 1;

	if (p413App->bReverseAB) {
		a = 1; b = 0;
	}

	m_nDirection = 0;
	int i, dirc1 = 0, dirc2 = 0, dirc3 = 0;
	int nAChannelLevel = 0, nBChannelLevel = 0;
	int nOldAChannelLevel = 0;

	// First region [5/17/2011 C2C]
	int nMidData = NUMBER_OF_INTEREST_DATASETS / 3;
	int idx = 4 * (nMidData + 1);
	nOldAChannelLevel = pRawDataArraySpaceB[4 * nMidData] > TTLA;
	for (i = nMidData + 1; i < nMidData + 10000; i++) {
		nAChannelLevel = pRawDataArraySpaceB[idx + a] > TTLA;
		if (nOldAChannelLevel > nAChannelLevel) {
			nBChannelLevel = pRawDataArraySpaceB[idx + b] > TTLA;
			switch (p413App->Global.DMCProgram) {
			case 1:
				if (nBChannelLevel > 0) {
					dirc1 = -1;		// TSMC2, UMC [7/19/2012 C2C]
				}
				else {
					dirc1 = 1;		// TSMC2, UMC [7/19/2012 C2C]
				}
				break;
			default:
				if (nBChannelLevel > 0) {
					dirc1 = 1;	// TSMC1, ASE [7/19/2012 C2C]
				}
				else {
					dirc1 = -1;	// TSMC1, ASE [7/19/2012 C2C]
				}
				break;
			}
			break;
		}
		idx += 4;
		nOldAChannelLevel = nAChannelLevel;
	}

	// Second region [5/17/2011 C2C]
	nMidData = NUMBER_OF_INTEREST_DATASETS / 2;
	idx = 4 * (nMidData + 1);
	nOldAChannelLevel = pRawDataArraySpaceB[4 * nMidData] > TTLA;
	for (i = nMidData + 1; i < nMidData + 10000; i++) {
		nAChannelLevel = pRawDataArraySpaceB[idx + a] > TTLA;
		if (nOldAChannelLevel > nAChannelLevel) {
			nBChannelLevel = pRawDataArraySpaceB[idx + b] > TTLA;
			switch (p413App->Global.DMCProgram) {
			case 1:
				if (nBChannelLevel > 0) {
					dirc2 = -1;		// TSMC2, UMC [7/19/2012 C2C]
				}
				else {
					dirc2 = 1;		// TSMC2, UMC [7/19/2012 C2C]
				}
				break;
			default:
				if (nBChannelLevel > 0) {
					dirc2 = 1;	// TSMC1, ASE [7/19/2012 C2C]
				}
				else {
					dirc2 = -1;	// TSMC1, ASE [7/19/2012 C2C]
				}
				break;
			}
			break;
		}
		idx += 4;
		nOldAChannelLevel = nAChannelLevel;
	}

	// Third region [5/17/2011 C2C]
	nMidData = 2 * NUMBER_OF_INTEREST_DATASETS / 3;
	idx = 4 * (nMidData + 1);
	nOldAChannelLevel = pRawDataArraySpaceB[4 * nMidData] > TTLA;
	for (i = nMidData + 1; i < nMidData + 10000; i++) {
		nAChannelLevel = pRawDataArraySpaceB[idx + a] > TTLA;
		if (nOldAChannelLevel > nAChannelLevel) {
			nBChannelLevel = pRawDataArraySpaceB[idx + b] > TTLA;
			switch (p413App->Global.DMCProgram) {
			case 1:
				if (nBChannelLevel > 0) {
					dirc3 = -1;		// TSMC2, UMC [7/19/2012 C2C]
				}
				else {
					dirc3 = 1;		// TSMC2, UMC [7/19/2012 C2C]
				}
				break;
			default:
				if (nBChannelLevel > 0) {
					dirc3 = 1;	// TSMC1, ASE [7/19/2012 C2C]
				}
				else {
					dirc3 = -1;	// TSMC1, ASE [7/19/2012 C2C]
				}
				break;
			}
			break;
		}
		idx += 4;
		nOldAChannelLevel = nAChannelLevel;
	}

	if (dirc1 != dirc2) {
		return FALSE;
	}
	if (dirc1 != dirc3) {
		return FALSE;
	}

	Directn = dirc1;

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

// BOOL CEchoProbe::FindCenterOfMassValues(int Chan, double nCurPeakWidth, double fThreshold, CEchoPeak *pEp, int nPeakNumber)
// {
// 	double fCMTempSum0 = 0.0;
// 	double fCMTempSum1 = 0.0;
// 	double fTemp = 0.0;
//
// 	CRecipe413 *rp = GetRecipe();
//
// 	unsigned short *pS;
// 	switch(Chan)
// 	{
// 	case 0:
// 		pS = pSignal1Data;
// 		break;
// 	case 1:
// 		pS = pSignal2Data;
// 		break;
// 	default:
// 		ASSERT(0);
// 		return FALSE;
// 		break;
// 	}
//
// 	int nX0, nX1;
//
// 	nX0 = pEp->GetPosition() - nCurPeakWidth;  // reduced peak width [8/31/2009 FSM]
// 	if (nX0 < 0)
// 	{
// 		nX0 = 0;
// 	}
// 	nX1 = pEp->GetPosition() + nCurPeakWidth;  // reduced peak width [8/31/2009 FSM]
//
// 	int st = pEp->GetIndex() - nCurPeakWidth/2;
// 	if (st < 0)
// 	{
// 		st = 0;
// 	}
// 	int ed = pEp->GetIndex() + nCurPeakWidth/2;
// 	if (ed >= NUMBER_OF_INTEREST_DATASETS)
// 	{
// 		ed = NUMBER_OF_INTEREST_DATASETS - 1;
// 	}
//
// 	int *pP = pPosData+st;
// 	float fBline = CalcBaseline(pS);
// 	for (int i = st; i <= ed; i++)
// 	{
// 		if(((*pP) > nX0) && ((*pP) < nX1))
// 		{
// 			fTemp = abs(*(pS+i) - fBline);
// 			if(fTemp > fThreshold)
// 			{
// 				fCMTempSum1 += (*pP) * fTemp * fTemp;
// 				fCMTempSum0 += fTemp * fTemp;
// 				//  [3/22/2010 Valued Customer]
// 				if (*(pS+i) < fBline)
// 				{
// 					*(pS+i) = fBline-((nPeakNumber%2)?300:700);
// 				}
// 			}
// 		}
// 		pP++;
// 	}
//
// 	if(fCMTempSum0 != 0)
// 	{
// 		int idx = 0;
// 		float CM = fCMTempSum1 / fCMTempSum0;
// 		pP = pPosData+st;
// 		for (int i=st; i<=ed; i++)
// 		{
// 			if (*pP>CM)
// 			{
// 				idx = i;
// 				break;
// 			}
// 			pP++;
// 		}
// 		pEp->SetCM(idx,CM);
// 	}
// 	else
// 	{
// 		return FALSE; // divided by zero
// 	}
//
// 	return TRUE;
// }

void CEchoProbe::SetBoardNumber(int nBoardNum) {
	ClearEcho(nBoardNum);
	// 	CEchoProbeBase::SetBoardNumber(nBoardNum);
}

// First half of collecting data, the other half is a callback OnReadingEnd [9/30/2012 Yuen]
// Wait 30 seconds if previous cbAInScan still running [10/11/2012 Yuen]
BOOL CEchoProbe::StartGetPeaks(CListCtrl* pList) {
	SetStatus(BUSY_STATUS);

	CString MsgStr;
	short nCurScanStatus;
	long CurCount, CurIndex;

	short BoardNum = p413App->MCBoardNo;

	ULONGLONG tick = GetTickCount64();
	BOOL bGood = FALSE;
	while ((GetTickCount64() - tick) < 10000) {
		cbGetStatus(BoardNum, &nCurScanStatus, &CurCount, &CurIndex, AIFUNCTION);
		if (nCurScanStatus == RUNNING) {
			Sleep(50);
			//SetStatus(READING_STATUS);
			continue;
		}
		else {
			bGood = TRUE;
			break;
		}
	}

	if (!bGood) {
		// Timeout detected [10/11/2012 Yuen]
		if (pList) {
			MsgStr = "MC GetStatus timeout";
			p413App->Message(pList, MsgStr);
		}
		if (m_lpfnReadingErrorProc) {
			m_lpfnReadingErrorProc(READING_STATUS, m_lpReadingErrorParam);
		}
		cbMemReset(BoardNum);
		ClearEcho(BoardNum);
		return FALSE;
	}

	int ErrStatus;
	if (!bEventSet) {
		if (!SetupEvents(ErrStatus, BoardNum)) {
			cbMemReset(BoardNum);
			ClearEcho(BoardNum);
			SetStatus(ERROR_STATUS);
			if (m_lpfnReadingErrorProc) {
				m_lpfnReadingErrorProc(ErrStatus, m_lpReadingErrorParam);
			}
			if (pList) {
				MsgStr = "MC SetupEvents error";
				p413App->Message(pList, MsgStr);
			}
			return FALSE;
		}
	}

	long lRate = EPMAX_RATE;
	int AInStatus = cbAInScan(BoardNum, 0, 3, NUMBER_OF_INTEREST_DATASETS * 4, &lRate, m_nRangeLevel, (HGLOBAL)pRawDataArraySpaceA, BACKGROUND | EXTTRIGGER | CONVERTDATA);
	if (AInStatus != NOERRORS) {
		cbMemReset(BoardNum);
		ClearEcho(BoardNum);
		SetStatus(ERROR_STATUS);
		if (m_lpfnReadingErrorProc) {
			m_lpfnReadingErrorProc(AInStatus, m_lpReadingErrorParam);
		}
		if (pList) {
			MsgStr = "MC AInScan error";
			p413App->Message(pList, MsgStr);
		}
		return FALSE;
	}
	return TRUE;
}

// Calling function must take care of error handling [3/24/2010 Valued Customer]
BOOL CEchoProbe::SetupEvents(int& ErrStatus, short BoardNum) {
	cbMemReset(BoardNum);
	ClearEcho(BoardNum);

	ErrStatus = cbEnableEvent(BoardNum, ON_END_OF_AI_SCAN, 0, OnReadingEnd, this);
	if (ErrStatus == NOERRORS) {
		ErrStatus = cbEnableEvent(BoardNum, ON_SCAN_ERROR, 0, OnReadingError, this);
		if (ErrStatus == NOERRORS) {
			cbSetTrigger(BoardNum, TRIG_HIGH, 1000, 4095);  //  [8/5/2009 Valued Customer]
			bEventSet = TRUE;
			return TRUE;
		}
	}

	cbMemReset(BoardNum);
	ClearEcho(BoardNum);
	SetStatus(ERROR_STATUS);
	if (m_lpfnReadingErrorProc) {
		m_lpfnReadingErrorProc(ErrStatus, m_lpReadingErrorParam);
	}
	bEventSet = FALSE;
	return FALSE;
}

// DoFindPeak: Extract peaks positions from raw data. Per sweep of motor motion [5/8/2011 FSMT]
// m_nTotalPeaks[] [5/8/2011 FSMT]
// m_EchoPeaks.XMaxima [5/8/2011 FSMT]
// m_EchoPeaks.Index [5/8/2011 FSMT]
// m_EchoPeaks.YMaxima [5/8/2011 FSMT]
// m_EchoPeaks.Mid [5/8/2011 FSMT]
// m_EchoPeaks.CM [5/8/2011 FSMT]

BOOL CEchoProbe::DoFindPeak(CRecipe413* pRcp, short MeSet, CString& MsgStr) {
	DoFindPeakErr = DFPNONE;
	SetStatus(CALCULATING_STATUS);
	unsigned short* p = pRawDataArraySpaceB;
	short MaxA, MinA;
	MaxA = MinA = *p;
	p += 4;

	// Find encoder voltage level [3/28/2012 Yuen]
	for (int i = 1; i < 256; i++) {
		if (*p < 2048) {
			*p = 2048;	// Value should not get below this unless it is an error [9/30/2012 Yuen]
		}
		if (*p > MaxA) {
			MaxA = *p;
		}
		if (*p < MinA) {
			MinA = *p;
		}
		p++;
		if (*p > MaxA) {
			MaxA = *p;
		}
		if (*p < MinA) {
			MinA = *p;
		}
		p += 3;
	}
	TTLA = (MaxA + MinA) / 2;	// encoder mid-voltage level, not sensitive to intermediate voltage [3/28/2012 Yuen]

	if (!FindStrokeDirc(m_nDirection)) {
		switch (DoFindPeakErr) {
		case DFPSTROKEDIRC:
			MsgStr = "# Check encoder reading";
			break;
		default:
			MsgStr = "# Unknown error";
			break;
		}
		return FALSE;
	}

	FindPeaks(pRcp, MeSet);

	SetStatus(IDLE_STATUS);
	return TRUE;
}

BOOL CEchoProbe::AllocMemory(int Size) {
	FreeAllocMemory();

	BOOL bErr = FALSE;
	pRawDataArraySpaceA = (unsigned short*)cbWinBufAlloc(Size * 4);
	if (!pRawDataArraySpaceA) {
		bErr = TRUE;
		ASSERT(0);
	}
	pRawDataArraySpaceB = (unsigned short*)cbWinBufAlloc(Size * 4);
	if (!pRawDataArraySpaceB) {
		bErr = TRUE;
		ASSERT(0);
	}
	return !bErr;
}

void CEchoProbe::FreeAllocMemory() {
	if (pRawDataArraySpaceA) {
		cbWinBufFree((HGLOBAL)pRawDataArraySpaceA);
		pRawDataArraySpaceA = NULL;
	}

	if (pRawDataArraySpaceB) {
		cbWinBufFree((HGLOBAL)pRawDataArraySpaceB);
		pRawDataArraySpaceB = NULL;
	}
}

void CEchoProbe::ReAllocMemory(int size) {
	CEchoProbeBase::AllocMemory(size);
	AllocMemory(size);
}

BOOL CEchoProbe::Initialize(CListBox* pMessage) {
	pMessage->InsertString(pMessage->GetCount(), "Echo system started OK");
	return TRUE;
}

void CEchoProbe::SaveSpec(CString Name, int counter) {
	if (!pRawDataArraySpaceB) {
		return;
	}
	FILE* fp;

	Name.Format("C:\\TEMP\\%s%d-1.CSV", Name, counter);
	fp = fopen(Name, "wb");
	if (fp) {
		unsigned short* pS1, * pS2, * pA, * pB, * p;
		int* pP;
		pS1 = pSignal1Data;
		pS2 = pSignal2Data;
		pA = pAChannelData;
		pB = pBChannelData;
		pP = pPosData;
		p = pRawDataArraySpaceB;
		fprintf(fp, "Pos,Top,Bottom,A,B,A,B\r\n");
		for (int i = 0; i < NUMBER_OF_INTEREST_DATASETS; i++) {
			//			fprintf(fp,"%d,%d,%d\r\n",*pP,*pS1,*pS2);
			//			fprintf(fp,"%d,%d,%d,%d,%d\r\n",*pP,*pS1,*pS2,*pA,*pB);
			fprintf(fp, "%d,%d,%d,%d,%d,%d,%d\r\n", *pP, *pS1, *pS2, *pA, *pB, *(p), *(p + 1));
			pP++;
			pS1++;
			pS2++;
			pA++;
			pB++;
			p += 4;
		}
		fclose(fp);
	}
}