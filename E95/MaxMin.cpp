// MaxMin.cpp: implementation of the CMaxMin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MaxMin.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMaxMin::CMaxMin() {
	iMax = iMin = 0;
	fMax = fMin = 0;
	bFirst = TRUE;
}

CMaxMin::~CMaxMin() {}

void CMaxMin::SetMax(float& m) {
	if (m > fMax) {
		fMax = m;
	}
}

void CMaxMin::SetMin(float& m) {
	if (m < fMin) {
		fMin = m;
	}
}

void CMaxMin::SetMax(int& m) {
	if (m > iMax) {
		iMax = m;
	}
}

void CMaxMin::SetMin(int& m) {
	if (m < iMin) {
		iMin = m;
	}
}

void CMaxMin::Set(float m) {
	fMax = fMin = m;
}

void CMaxMin::Set(int m) {
	iMax = iMin = m;
}

void CMaxMin::SetMM(float m) {
	if (!bFirst) {
		if (m < fMin) {
			fMin = m;
		}
		if (m > fMax) {
			fMax = m;
		}
	}
	else {
		bFirst = FALSE;
		fMin = fMax = m;
	}
}

void CMaxMin::SetMM(int m) {
	if (!bFirst) {
		if (m < fMin) {
			fMin = m;
		}
		if (m > fMax) {
			fMax = m;
		}
	}
	else {
		bFirst = FALSE;
		fMin = fMax = m;
	}
}