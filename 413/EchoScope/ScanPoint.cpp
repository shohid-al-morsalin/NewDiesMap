// ScanPoint.cpp: implementation of the CScanPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScanPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScanPoint::CScanPoint() {
	x = 0;
	n = 0;
	y1 = y2 = y3 = 0;
}

CScanPoint::~CScanPoint() {}

void CScanPoint::Set(int xx, int yy, int nn) {
	x = xx;
	n = nn;  //  nLastInterestDataSet
	y1 = yy;
	y2 = y3 = 0;
}

CScanPoint& CScanPoint::operator =(CScanPoint& co) {
	x = co.x;
	n = co.n;
	y1 = co.y1;
	y2 = co.y2;
	y3 = co.y3;
	return *this;
}