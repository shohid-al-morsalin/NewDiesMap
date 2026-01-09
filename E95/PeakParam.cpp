// PeakParam.cpp: implementation of the CPeakParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PeakParam.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPeakParam::CPeakParam() {
	Method = cmEQUAL;
	Clear();
}

CPeakParam::~CPeakParam() {}

void CPeakParam::Clear() {
	Method = cmEQUAL;
	nPeakT = nPeakB = 0;  // Number of peaks
	for (int i = 0; i < MAXPKCONSTRAIN; i++) {
		HeightB[i].Clear();
		HeightT[i].Clear();
		DistanceB[i].Clear();
		DistanceB[i].Clear();
	}
}