// Toolvar.cpp: implementation of the CToolVar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolVar.h"
#include "..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolVar::CToolVar() {
	OpMode = 0;
}

CToolVar::~CToolVar() {}

void CToolVar::Conv(char** msg) {
	CMake::Conv(OpMode, msg);
}

long CToolVar::GetLength() {
	long sz = 0;
	sz = CMake::GetLength(OpMode);

	return sz;
}

long CToolVar::Binary(char** msg) {
	long sz = 0;
	sz += CMake::Binary(OpMode, msg);
	return sz;
}