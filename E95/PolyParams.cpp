// PolyParams.cpp: implementation of the CPolyParams class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "PolyParams.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolyParams::CPolyParams() {
	Clear();
}

CPolyParams::~CPolyParams() {}

void CPolyParams::Clear() {
	fThk = -1;
	fMin = fMax = 0;
	fRI = 0;
	GOF = WHH = PolyL = PolyM = PolyH = PolyR = 0;
	jPmax = jPlow = jPhi = jLo = jHi = 0;
}

void CPolyParams::ClearfThk() {
	fThk = 0;
}

void CPolyParams::MarkfThk() {
	fThk = -1;
}