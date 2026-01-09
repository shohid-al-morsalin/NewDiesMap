// FFTPoint.cpp: implementation of the CFFTPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FFTPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFFTPoint::CFFTPoint() {
	// 	X = Y1 = Y2 = Y3 = 0;
}

CFFTPoint::~CFFTPoint() {}

CFFTPoint& CFFTPoint::operator = (CFFTPoint& co) {
	// 	X = co.X;
	// 	Y1 = co.Y1;
	// 	Y2 = co.Y2;
	// 	Y3 = co.Y3;

	P = co.P;
	V = co.V;

	return *this;
}