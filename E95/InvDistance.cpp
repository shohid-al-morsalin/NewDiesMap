// InvDistance.cpp: implementation of the CInvDistance class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "InvDistance.h"
#include "Coor.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInvDistance::CInvDistance() {
	pCo = NULL;
	wt = NULL;

	pw = 2.0f;

	Clear();
}

CInvDistance::~CInvDistance() {
	Clear();
}

// X,Y = coordinate [7/17/2013 Yuen]
// Z = data [7/17/2013 Yuen]
float CInvDistance::GetZ(float x, float y) {
	int i;
	float X, Y;
	CCoor** pP;
	float* pwt;
	double dist;

	// Calculate weight [7/17/2013 Yuen]
	double sum = 0;
	pP = pCo;
	pwt = wt;
	pw = 2.0f;
	float Shortest = 9999.0f;
	CCoor* pShortest = NULL;
	for (i = 0; i < Last; i++) {
		X = (x - (*pP)->x);
		Y = (y - (*pP)->y);
		dist = sqrt(X * X + Y * Y);
		if (dist < Shortest) {
			Shortest = dist;
			pShortest = *pP;
		}
		sum += (*pwt = 1 / (dist * dist));
		pP++; pwt++;
	}
	if (pShortest) {
		// Return the desire value [7/19/2013 Yuen]
		if (Shortest < 5.0f)  // less than 5mm [7/19/2013 Yuen]
		{
			return pShortest->z;
		}
	}

	// Calculate interpolated value [7/17/2013 Yuen]
	double sum2 = 0;
	pP = pCo;
	pwt = wt;
	for (i = 0; i < Last; i++) {
		sum2 += (((*pwt) * (*pP)->z) / sum);
		pP++; pwt++;
	}
	return sum2;
}

BOOL CInvDistance::Add(CCoor* p) {
	if (Last < Total) {
		*(pCo + Last) = p;
		Last++;
		return TRUE;
	}
	return FALSE;
}

void CInvDistance::Alloc(short n) {
	Clear();
	pCo = new CCoor * [n + 1];
	wt = new float[n + 1];
	if (pCo && wt) {
		Total = n;
		return;
	}
	Clear();
}

void CInvDistance::Clear() {
	if (pCo) {
		delete[] pCo;
		pCo = NULL;
	}
	if (wt) {
		delete[] wt;
		wt = NULL;
	}
	Total = 0; Last = 0;
}