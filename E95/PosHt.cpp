// PosHt.cpp: implementation of the CPosHt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "PosHt.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPosHt::CPosHt() {
	n = 0;
}

CPosHt::~CPosHt() {}

void CPosHt::Clear() {
	n = 0;
}

BOOL CPosHt::Add(float P, short dP, int H, short dH) {
	if (n > 10) {
		return FALSE;
	}
	Pos[n].Set(P, dP, H, dH);
	n++;
	return TRUE;
}