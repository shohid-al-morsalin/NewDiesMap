// FPoint.cpp: implementation of the CFPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "FPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFPoint::CFPoint() {
	Clear();
}

CFPoint::~CFPoint() {}

void CFPoint::Clear() {
	X = Y = 0;
}

void CFPoint::Set(float Cx, float Cy) {
	X = Cx; Y = Cy;
}

void CFPoint::Get(float& Cx, float& Cy) {
	Cx = X; Cy = Y;
}