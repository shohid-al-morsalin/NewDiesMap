// ScanLine.cpp: implementation of the CScanLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "ScanLine.h"
#include "ScanPoint.h"
#include "..\..\E95\413App.h"
#include "..\..\E95\Global413.h"

#include "MeParam.h"

#define PI 3.1415926535897932384626433832795

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScanLine::CScanLine() {
	pData = NULL;
	Clear();
}

CScanLine::~CScanLine() {
	Clear();
}

void CScanLine::Clear() {
	Count = 0;
	Idx = 0;
	iCM = yCM = 0;
	iMid = yMid = 0;
	CM = 0;
	Mid = 0;
	if (pData) {
		delete[] pData;
		pData = NULL;
	}
}

BOOL CScanLine::Alloc(int size) {
	Clear();
	pData = new CScanPoint[size + 1];
	if (pData) {
		Count = size;
		return TRUE;
	}
	return FALSE;
}

BOOL CScanLine::Set(int x, int y, int n) {
	if (Idx < Count) {
		pData[Idx++].Set(x, y, n);
		return TRUE;
	}
	return FALSE;
}

BOOL CScanLine::Dup(CScanLine* pL) {
	if (!pL) {
		return FALSE;
	}
	BOOL bAlloc = TRUE;
	if (Count != pL->Count) {
		Clear();
		bAlloc = Alloc(pL->Count);
	}

	if (bAlloc) {
		Idx = pL->Idx;
		Count = pL->Count;
		CScanPoint* p = pData;
		CScanPoint* q = pL->pData;
		memcpy(p, q, sizeof(CScanPoint) * Count);
		MaxMinX.iMax = pL->MaxMinX.iMax;
		MaxMinX.iMin = pL->MaxMinX.iMin;
		MaxMinY1.iMax = pL->MaxMinY1.iMax;
		MaxMinY1.iMin = pL->MaxMinY1.iMin;
		MaxMinY2.iMax = pL->MaxMinY2.iMax;
		MaxMinY2.iMin = pL->MaxMinY2.iMin;
		return TRUE;
	}
	return FALSE;
}

// Find tallest peaks from y1 and put it in y2 [11/30/2010 XPMUser]
void CScanLine::Shrink(short skip) {
	int i;
	CScanPoint* p = pData + skip;
	CScanPoint* p1 = pData + skip + 1;

	// forward [8/14/2011 FSMT]
	for (i = skip; i < Idx - 1 - skip; i++) {
		if (p->y1 > p1->y1) {
			p->y2 = p->y1;
		}
		else if (p->y1 == p1->y1) {
			p->y2 = p->y1;
		}
		else {
			p->y2 = 0;
		}
		p++; p1++;
	}
	p->y2 = 0;  // last value [8/14/2011 FSMT]

	// Reverse search [8/14/2011 FSMT]
	p = pData + Idx - 1;
	p1 = pData + Idx - 2;
	for (i = Idx - 1 - skip; i >= skip; i--) {
		if ((p->y2 != 0) && (p->y1 < p1->y1)) {
			p->y2 = 0;
		}
		p--; p1--;
	}
}

// y2 -> y1 [5/4/2010 Valued Customer]
// Data reduction [11/30/2010 XPMUser]
void CScanLine::Reduce(float baseline) {
	int i;
	CScanPoint* p = pData;
	CScanPoint* q = pData;

	// 	DupY1();

	int Idx2 = 0;
	for (i = 0; i < Idx; i++) {
		if (p->y2 > baseline) {
			q->x = p->x;
			q->n = p->n;
			q->y1 = p->y2;
			q->y2 = 0;
			q++;
			Idx2++;
		}
		p++;
	}
	Idx = Idx2;
}

void CScanLine::RejectPeak(short spc, short PkHeight, float baseline) {
	int i;
	CScanPoint* p0 = pData;
	CScanPoint* p1 = pData + spc;
	CScanPoint* p2 = pData + 2 * spc;

	CScanPoint* pY = pData;
	for (i = 0; i < spc; i++) {
		pY->y1 = baseline;
		pY++;
	}

	for (i = spc; i < Idx - spc; i++) {
		if (p1->y1 > baseline) {
			if (((p0->y2 - PkHeight) < p1->y2) && ((p2->y2 - PkHeight) < p1->y2)) {
			}
			else {
				p1->y1 = baseline;
			}
		}
		p0++; p1++; p2++;
	}
	for (i = Idx - spc; i < Idx; i++) {
		p1->y1 = baseline;
		p1++;
	}
}

void CScanLine::Smooth(float baseline) {
	int i;
	CScanPoint* p0 = pData;
	CScanPoint* p1 = pData + 1;
	CScanPoint* p2 = pData + 2;
	CScanPoint* p3 = pData + 3;
	CScanPoint* p4 = pData + 4;

	DupY1();

	p0->y1 = p0->y2 = p2->y2;
	p1->y1 = p1->y2 = p2->y2;
	for (i = 2; i < Idx - 2; i++) {
		p2->y1 = (p0->y2 + p1->y2 + p2->y2 + p3->y2 + p4->y2) / 5;
		p0++; p1++; p2++; p3++; p4++;
	}
	p2--;
	p3->y1 = p2->y2; p3->y2 = baseline;
	p4->y1 = p2->y2; p4->y2 = baseline;
}

void CScanLine::DupY1() {
	CScanPoint* p = pData;
	for (int i = 0; i < Idx; i++) {
		p->y2 = p->y1;
		p++;
	}
}

// Find the best peak by removing peaks that are close together [11/18/2011 Administrator]
void CScanLine::Clean(short Chan, CMeParam* Me) {
	int i, j, c1 = 0, c2 = 0;
	short PkCount = 0;
	short Near;

	// Phase 1: reduce duplicates [6/21/2013 Yuen]
	CScanPoint* p0 = pData;
	CScanPoint* p = pData + 1;
	for (i = 1; i < Idx; i++) {
		if (p->y2 > 0) {
			if (p->y2 == p0->y2) {
				// Look backward for same value [6/21/2013 Yuen]
				c1 = i;
				CScanPoint* q = p - 1;
				for (j = i - 1; j >= 1; j--) {
					if (p->y2 == q->y2) {
						c1 = j;
					}
					else {
						break;
					}
					q--;
				}
				// Look forward for same value [6/21/2013 Yuen]
				c2 = i;
				q = p + 1;
				for (j = i + 1; j < Idx; j++) {
					if (p->y2 == q->y2) {
						c2 = j;
					}
					else {
						break;
					}
					q++;
				}
				q = pData + c1;// c1 is smaller [6/21/2013 Yuen]
				short v = p->y2;
				for (j = c1; j <= c2; j++) {
					q->y2 = 0;
					q++;
				}
				(p - c1 + (c1 + c2) / 2)->y2 = v;
			}
		}
		p0++; p++;
	}

	// Phase 2: Clear neighboring peaks [6/21/2013 Yuen]

	C413Global* g = &p413App->Global;
	for (i = 0; i < Idx; i++) {
		CScanPoint* p = pData + i;
		if (p->y2 > 0) {
			switch (Chan) {
			case 0:
				if (Me->PS1[PkCount].bUse) {
					Near = Me->PS1[PkCount].Near;
				}
				else {
					Near = Me->PSG[0].Near;
				}
				break;
			default:
				if (Me->PS2[PkCount].bUse) {
					Near = Me->PS2[PkCount].Near;
				}
				else {
					Near = Me->PSG[1].Near;
				}
				break;
			}
			if (PkCount < MAXPEAKCOUNT) {
				EliminateNeighbour(i, 3.2 * Near);
			}
			else {
				p->y2 = 0;
			}
			PkCount++;
			if (PkCount >= MAXPEAKCOUNT) {
				// Too many peaks [10/19/2012 FSM413]
				break;
			}
		}
	}

	// 	// Reject smaller peaks based on the second peak [1/11/2011 XPMUser]
	// 	CPeakShape *PS;
	// 	switch (Chan)
	// 	{
	// 	case 0:
	// 		PS = &Me->PSG[0];
	// 		break;
	// 	default:
	// 		PS = &Me->PSG[1];
	// 		break;
	// 	}
	// 	if (PS->Minimum == 0)
	// 	{
	// 		// default value, nothing to do [11/18/2011 Administrator]
	// 		return ;
	// 	}
	// 	short height = baseline;
	// 	PkCount = 0;
	// 	for (i=0; i<Idx; i++)
	// 	{
	// 		CScanPoint *p = pData+i;
	// 		if (p->y2 > 0)
	// 		{
	// 			PkCount++;
	// 			if (PkCount >= MAXPEAKCOUNT)
	// 			{
	// 				break;
	// 			}
	// 			if (PkCount == 2)
	// 			{
	// 				height = baseline + (p->y2-baseline-PS->Thres) * PS->Minimum / 100;
	// 				p->y2 = 2601;// Adjust peak height for display [1/11/2011 C2C]
	// 			}
	// 			else if (PkCount > 2)
	// 			{
	// 				if (p->y2 <= height)
	// 				{
	// 					p->y2 = baseline;
	// 				}
	// 				else
	// 				{
	// 					p->y2 = 2600;// Adjust peak height for display [1/11/2011 C2C]
	// 				}
	// 			}
	// 			else
	// 			{
	// 				p->y2 = 2600;// Adjust peak height for display [1/11/2011 C2C]
	// 			}
	// 		}
	// 	}
}

// Eliminate the neighboring smaller peak. [11/18/2011 Administrator]
void CScanLine::EliminateNeighbour(int n, short dx) {
	int n1 = n - dx;
	int n2 = n + dx;
	int i;

	CScanPoint* q = pData + n;
	if (n1 < 0) {
		n1 = 0;
		n2 = n1 + 2 * dx;
	}
	for (i = n - 1; i >= n1; i--) {
		CScanPoint* p = pData + i;
		if (p->y2 > 0) {
			if ((i != n) && (abs((p->x) - (q->x)) < 1e6)) {
				if (p->y2 > q->y2) {
					q->y2 = 0;
				}
				else {
					p->y2 = 0;
				}
			}
			return;
		}
	}
}

void CScanLine::Reset() {
	Idx = 0;
}

void CScanLine::Merge(CScanLine* pL, short Threshold, float baseline) {
	CScanPoint* p = pData;
	CScanPoint* q = pL->pData;

	Threshold += baseline;

	for (int i = 0; i < Idx; i++) {
		if (q->y2 > Threshold) {
			p->y2 = 2800;
		}
		else {
			p->y2 = baseline;
		}
		p++;
		q++;
	}
}

int CScanLine::GetRangeX() {
	return MaxMinX.iMax - MaxMinX.iMin;
}

void CScanLine::Dump(CString name) {
	FILE* fp = fopen(name, "wb");
	if (fp) {
		fprintf(fp, "Index,Pos,Y1,Y2,Y3\n");
		Dump(fp);
		fclose(fp);
	}
}

void CScanLine::Dump(FILE* fp) {
	CScanPoint* p = pData;
	for (int i = 0; i < Idx; i++) {
		fprintf(fp, "%d,%d,%d,%d,%d\r\n", p->n, p->x, p->y1, p->y2, p->y3);
		p++;
	}
}

// Finding Center of Mass [11/18/2011 Administrator]
// iH = baseline + Level + Thres [11/18/2011 Administrator]
// pP = position information [9/18/2012 Administrator]
// pS = Peak intensity information [9/18/2012 Administrator]
void CScanLine::GetCM(int index, int width, short iH, int* pP, unsigned short* pS) {
	CScanPoint* p1, * p2, * p3;

	double fTemp = 0;
	int n2, n3;

	CM = 0;
	Mid = 0;
	X1 = X2 = 0;

	p1 = pData + index;
	int y = p1->y1 - iH;
	double fCMSum0 = y * y;
	double fCMSum1 = fCMSum0 * p1->x; // Center point [9/25/2011 Administrator]

	int nn = index - 1;
	p1 = pData + nn;
	int iX = p1->x;
	// Left side from max [9/18/2012 Administrator]
	while (((iX - p1->x) <= width) && (nn > 0) && (p1->y1 > iH))  // restored (p->y1 > iH) [11/30/2010 XPMUser]
	{
		y = p1->y1 - iH;
		double y2 = double(y * y);
		fCMSum1 += p1->x * double(y2);
		fCMSum0 += double(y2);
		nn--; p1--;
	}

	(p1 + 1)->y3 = LEFTPKEDGE;	// Left limit [9/22/2011 Administrator]
	X1 = (p1 + 1)->x;
	iX1 = (p1 + 1)->n;

	short vv = (p1 + 1)->y1;	// lower intensity limit [9/22/2011 Administrator]
	p2 = p3 = p1 + 1;
	n2 = n3 = nn + 1;
	nn = index + 1;
	p1 = pData + nn;
	iX = p1->x;

	// Right side from max [9/18/2012 Administrator]
	while (((p1->x - iX) <= width) && (nn < Idx) && (p1->y1 > iH) && p1->y1 >= vv)  // restored (p->y1 > iH) [11/30/2010 XPMUser]
	{
		int yy = p1->y1 - iH;
		double y2 = double(yy * yy);
		fCMSum1 += p1->x * double(y2);
		fCMSum0 += double(y2);
		nn++; p1++;
	}
	(p1 - 1)->y3 = RIGHTPKEDGE;  // right limit [9/22/2011 Administrator]
	X2 = (p1 - 1)->x;
	iX2 = (p1 - 1)->n;

	if (fCMSum0 != 0) {
		CM = (fCMSum1 / fCMSum0);
		while (n2 < Idx) {
			if (p2->x > CM) {
				iCM = p2->n;
				yCM = p2->y1;
				p2->y3 = CTRMASSPK;	// CM [9/22/2011 Administrator]
				break;
			}
			p2++;
			n2++;
		}
		Mid = (X1 + X2) / 2.0f;
		while (n3 < Idx) {
			if (p3->x > Mid) {
				if (p3 != p2) {
					iMid = p3->n;
					yMid = p3->y1;
					p3->y3 = MIDPKEDGE;  // Mid [9/22/2011 Administrator]
				}
				else {
					iMid = (p3 - 1)->n;
					yMid = (p3 - 1)->y1;
					(p3 - 1)->y3 = MIDPKEDGE;	// Mid [9/22/2011 Administrator]
				}
				break;
			}
			p3++;
			n3++;
		}
	}
	else {
		p1 = pData + index;
		CM = p1->x;
	}

	// Calculation of CM from raw data [9/18/2012 Administrator]
	if (pP && pS) {
		// Calculate CM in different way [9/24/2011 Administrator]
		if (iX1 > iX2) {
			int swap = iX1;
			iX1 = iX2;
			iX2 = swap;
		}
		int* ppP = pP + iX1;
		unsigned short* ppS = pS + iX1;
		fCMSum0 = fCMSum1 = 0;
		int i;
		for (i = iX1; i <= iX2; i++) {
			double yy = (*ppS) * (*ppS);
			fCMSum1 += (*ppP) * yy;
			fCMSum0 += yy;
			ppP++; ppS++;
		}
		if (fCMSum0 > 0) {
			CM = (fCMSum1 / fCMSum0);
		}
		else {
			// set to peak maxima [9/28/2011 C2C]
			p1 = pData + index;
			CM = p1->x;
		}
		ppP = pP + iX1;
		// Find rough index of CM [9/18/2012 Administrator]
		for (i = iX1; i <= iX2; i++) {
			if (*ppP > CM) {
				iCM = i;
				break;
			}
			ppP++;
		}
	}
}

void CScanLine::Fake(int nPoints) {
	Alloc(nPoints);
	FakPoint();
}

void CScanLine::FakPoint() {
	float freq = 4;
	CScanPoint* p = pData;
	for (int i = 0; i < Count; i++) {
		if (i) {
			p->x = i;
			float v = float(cos(2 * PI * freq * i / Count));
			if (v < 0) {
				v = 0;
			}
			p->y1 = 2047 + v;
			//p->y2 = 2047 + rand() * 1000 / float(RAND_MAX);
			//if (MaxMinY1.iMin > p->y1)
			//{
			//	MaxMinY1.iMin = p->y1;
			//}
			//if (MaxMinY1.iMax < p->y1)
			//{
			//	MaxMinY1.iMax = p->y1;
			//}
			//if (MaxMinY2.iMin > p->y2)
			//{
			//	MaxMinY2.iMin = p->y2;
			//}
			//if (MaxMinY2.iMax < p->y2)
			//{
			//	MaxMinY2.iMax = p->y2;
			//}
		}
		else {
			p->x = i;
			float v = float(cos(2 * PI * freq * i / Count));
			if (v < 0) {
				v = 0;
			}
			p->y1 = 2047 + v;
			//p->y2 = 2047 + rand() * 1000 / float(RAND_MAX);
			//MaxMinY1.iMax = MaxMinY1.iMin = p->y1;
			//MaxMinY2.iMax = MaxMinY2.iMin = p->y1;
		}
		p++;
	}
	Idx = Count;
	MaxMinX.iMin = 0;
	MaxMinX.iMax = Count;
}

void CScanLine::ZeroY2() {
	CScanPoint* p = pData;
	for (int i = 0; i < Idx; i++) {
		p->y2 = 0;
		p++;
	}
}

void CScanLine::DupY3Y2(float baseline) {
	CScanPoint* p = pData;
	for (int i = 0; i < Idx; i++) {
		if (p->y3 > 0) {
			p->y2 = p->y3;
		}
		// 		else
		// 		{
		// 			p->y2 = baseline;
		// 		}
		p++;
	}
}

void CScanLine::CalcMaxMin() {
	if (Count < 1) {
		return;
	}
	CScanPoint* p = pData;

	MaxMinX.iMax = MaxMinX.iMin = p->x;
	MaxMinY1.iMax = MaxMinY1.iMin = p->y1;
	MaxMinY2.iMax = MaxMinY2.iMin = p->y2;

	p++;
	for (int i = 1; i < Idx; i++) {
		if (p->y1 < MaxMinY1.iMin) {
			MaxMinY1.iMin = p->y1;
		}
		if (p->y1 > MaxMinY1.iMax) {
			MaxMinY1.iMax = p->y1;
		}
		if (p->y2 < MaxMinY2.iMin) {
			MaxMinY2.iMin = p->y2;
		}
		if (p->y2 > MaxMinY2.iMax) {
			MaxMinY2.iMax = p->y2;
		}
		p++;
	}
	p--;
	MaxMinX.iMax = p->x;
	if (MaxMinX.iMax < MaxMinX.iMin) {
		int swap = MaxMinX.iMax;
		MaxMinX.iMax = MaxMinX.iMin;
		MaxMinX.iMin = swap;
	}
}

short CScanLine::GetLimits(int* Pos) {
	short n = 0;
	CScanPoint* p = pData;
	for (int i = 0; i < Idx; i++) {
		if (p->y2 != 2048) {
			*Pos = p->n;
			Pos++;
			n++;
		}
		p++;
	}
	return n;
}

// ix = index of scan line array [3/22/2013 Yuen]
// wd = peak width to find CM [3/22/2013 Yuen]
// thres = min peak width to find [3/22/2013 Yuen]
void CScanLine::GetCMv2(short ix, float wd, float thres) {
	int i;
	CScanPoint* p1;

	p1 = pData + ix;	// peak position, ie y2 [3/22/2013 Yuen]
	p1->y3 = p1->y2;

	int y0 = p1->y1;
	int x0 = p1->x;
	double fCMSum0 = y0 * y0;
	double fCMSum1 = fCMSum0 * x0; // Center point [9/25/2011 Administrator]

	short lft = 0, rgt = 0;

	// backward search [3/22/2013 Yuen]

	for (i = ix - 1; i >= 0; i--) {
		p1--;
		int x = p1->x;
		int y = p1->y1;

		if (abs(x - x0) < wd) {
			double y2 = y * y;
			fCMSum0 += y2;
			fCMSum1 += x * y2;
		}
		else {
			// Reached lowest point of peak left [3/22/2013 Yuen]
			p1++;
			p1->y3 = LEFTPKEDGE;
			lft = i + 1;
			break;
		}
	}
	if (i < 0) {
		// reach beginning of pData [3/22/2013 Yuen]
		p1 = pData;
		p1->y3 = LEFTPKEDGE;
		lft = 0;
	}

	// forward search [3/22/2013 Yuen]
	p1 = pData + ix;

	for (i = ix + 1; i < Idx; i++) {
		p1++;
		int x = p1->x;
		int y = p1->y1;

		if (abs(x - x0) < wd) {
			double y2 = y * y;
			fCMSum0 += y2;
			fCMSum1 += x * y2;
		}
		else {
			// Reached lowest point of peak left [3/22/2013 Yuen]
			p1--;
			p1->y3 = RIGHTPKEDGE;
			rgt = i - 1;
			break;
		}
	}
	if (i >= Idx) {
		// Reaching end of data [3/22/2013 Yuen]
		p1 = pData + Idx - 1;
		p1->y3 = RIGHTPKEDGE;
		rgt = Idx - 1;
	}

	// Find mid point [3/22/2013 Yuen]

	p1 = pData + lft;
	iX1 = p1->x;
	p1 = pData + rgt;
	iX2 = p1->x;

	p1 = pData + (rgt + lft) / 2;
	iMid = p1->n;
	yMid = p1->y1;
	Mid = p1->x;
	p1->y3 = MIDPKEDGE;

	// Find CM here [3/22/2013 Yuen]
	CM = 0;
	if (fCMSum0 != 0) {
		CM = fCMSum1 / fCMSum0;
	}

	//	CM = Mid;	// @@@@ Test only [3/22/2013 Yuen]

	if (CM != 0) {
		// scan for the position of CM [3/22/2013 Yuen]
		short a = ix - 64;
		short b = ix + 64;
		if (a < 0) {
			a = 0;
			b = 128;
		}
		if (b > Idx) {
			b = Idx;
			if (a > 0) {
				a = b - 128;
				if (a < 0) {
					a = 0;
				}
			}
		}

		p1 = pData + a;
		for (i = a; i < b; i++) {
			if (p1->x > CM) {
				p1--;
				iCM = p1->n;
				yCM = p1->y1;
				p1->x = CM;
				p1->y3 = CTRMASSPK; // CM [3/22/2013 Yuen]
				break;
			}

			p1++;
		}
		if (i == b) {
			// Fail to locate CM, use max peak position [3/22/2013  Yuen]
			p1 = pData + ix;
			iCM = p1->n;
			yCM = p1->y1;
			p1->y3 = CTRMASSPK; // CM [3/22/2013 Yuen]
		}
	}
	else {
		p1 = pData + ix;
		iCM = p1->n;
		yCM = p1->y1;
		p1->x = CM;
		p1->y3 = CTRMASSPK; // CM [3/22/2013 Yuen]
	}
}