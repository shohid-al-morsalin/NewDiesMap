// 128SVar.cpp: implementation of the C128SVar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "128SVar.h"
#include "..\SRC\BMake.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C128SVar::C128SVar()
{
	bScaleOnline = FALSE;
	bScannerOnline = FALSE;
	OpMode = 0;
	ServiceState = 0;
	WaferPresent = 0;  // Bitmap: from LSB : Alinger|Scale|Scanner
}

C128SVar::~C128SVar()
{

}

long C128SVar::GetLength()
{
	long sz=0;

	sz += CMake::GetLength(bScaleOnline);
	sz += CMake::GetLength(bScannerOnline);
	sz += CMake::GetLength(OpMode);
	sz += CMake::GetLength(ServiceState);
	sz += CMake::GetLength(WaferPresent);
	sz += CMake::GetLength(bp780);
	sz += CMake::GetLength(bp810);

	return sz;
}

long C128SVar::Binary(char **msg)
{
	long sz = 0;

	sz += CMake::Binary(bScaleOnline,msg);
	sz += CMake::Binary(bScannerOnline,msg);
	sz += CMake::Binary(OpMode,msg);
	sz += CMake::Binary(ServiceState,msg);
	sz += CMake::Binary(WaferPresent,msg);
	sz += CMake::Binary(bp780,msg);
	sz += CMake::Binary(bp810,msg);

	return sz;
}

void C128SVar::Conv(char **msg)
{
	CMake::Conv(bScaleOnline,msg);
	CMake::Conv(bScannerOnline,msg);
	CMake::Conv(OpMode,msg);
	CMake::Conv(ServiceState,msg);
	CMake::Conv(WaferPresent,msg);
	CMake::Conv(bp780,msg);
	CMake::Conv(bp810,msg);
}
