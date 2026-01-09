// LoadPos.cpp: implementation of the CLoadPos class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "LoadPos.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoadPos::CLoadPos() {
	Clear();
}

CLoadPos::~CLoadPos() {}

void CLoadPos::Clear() {
	PX = PY = 0;
	VX = VY = VZ = 0;
	Exposure = 0;
}

void CLoadPos::Load() {}

void CLoadPos::Save() {}