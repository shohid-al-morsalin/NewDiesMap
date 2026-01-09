// 900SVar.cpp: implementation of the C900SVar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "900SVar.h"
#include "..\SRC\BMake.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C900SVar::C900SVar()
{
	bWaferPresent = FALSE;
	bChamberHot = FALSE;
	bGasFlow = FALSE;
	bGasPresent = FALSE;
	bHeatOK = FALSE;
	bOverTemp1 = FALSE;
	bOverTemp2 = FALSE;
	bRoughPump = FALSE;
	bTurboPump = FALSE;
	bVacE3 = FALSE;
	bVacE5 = FALSE;
	bWaterFlow = FALSE;
	bWaterLeak = FALSE;
	Environ = 0;
	GasSetting = 0;
	int i;
	for (i=0; i<12; i++)
	{
		Lamp[i] = 0;
	}
	Tempr = 0;
	VacIG = 0;
	VacTC = 0;
	OpMode = 0;
	ServiceState = 0;
}

C900SVar::~C900SVar()
{

}

long C900SVar::GetLength()
{
	long sz=0;

	sz += CMake::GetLength(bChamberHot);
	sz += CMake::GetLength(bGasFlow);
	sz += CMake::GetLength(bGasPresent);
	sz += CMake::GetLength(bHeatOK);
	sz += CMake::GetLength(bOverTemp1);
	sz += CMake::GetLength(bOverTemp2);
	sz += CMake::GetLength(bRoughPump);
	sz += CMake::GetLength(bTurboPump);
	sz += CMake::GetLength(bVacE3);
	sz += CMake::GetLength(bVacE5);
	sz += CMake::GetLength(bWaterFlow);
	sz += CMake::GetLength(bWaterLeak);
	sz += CMake::GetLength(Environ);
	sz += CMake::GetLength(GasSetting);
	sz += CMake::GetLength(Lamp[0]) * 12;
	sz += CMake::GetLength(Tempr);
	sz += CMake::GetLength(VacIG);
	sz += CMake::GetLength(VacTC);
	sz += CMake::GetLength(OpMode);
	sz += CMake::GetLength(ServiceState);
	sz += CMake::GetLength(bp780);
	sz += CMake::GetLength(bp810);
	sz += CMake::GetLength(bWaferPresent);
	return sz;
}

long C900SVar::Binary(char **msg)
{
	int i;
	long sz = 0;
	sz += CMake::Binary(bChamberHot,msg);
	sz += CMake::Binary(bGasFlow,msg);
	sz += CMake::Binary(bGasPresent,msg);
	sz += CMake::Binary(bHeatOK,msg);
	sz += CMake::Binary(bOverTemp1,msg);
	sz += CMake::Binary(bOverTemp2,msg);
	sz += CMake::Binary(bRoughPump,msg);
	sz += CMake::Binary(bTurboPump,msg);
	sz += CMake::Binary(bVacE3,msg);
	sz += CMake::Binary(bVacE5,msg);
	sz += CMake::Binary(bWaterFlow,msg);
	sz += CMake::Binary(bWaterLeak,msg);
	sz += CMake::Binary(Environ,msg);
	sz += CMake::Binary(GasSetting,msg);
	for (i=0; i<12; i++)
	{
		sz += CMake::Binary(Lamp[i],msg);
	}
	sz += CMake::Binary(Tempr,msg);
	sz += CMake::Binary(VacIG,msg);
	sz += CMake::Binary(VacTC,msg);
	sz += CMake::Binary(OpMode,msg);
	sz += CMake::Binary(ServiceState,msg);
	sz += CMake::Binary(bp780,msg);
	sz += CMake::Binary(bp810,msg);
	sz += CMake::Binary(bWaferPresent,msg);
	return sz;
}

void C900SVar::Conv(char **msg)
{
	CMake::Conv(bChamberHot,msg);
	CMake::Conv(bGasFlow,msg);
	CMake::Conv(bGasPresent,msg);
	CMake::Conv(bHeatOK,msg);
	CMake::Conv(bOverTemp1,msg);
	CMake::Conv(bOverTemp2,msg);
	CMake::Conv(bRoughPump,msg);
	CMake::Conv(bTurboPump,msg);
	CMake::Conv(bVacE3,msg);
	CMake::Conv(bVacE5,msg);
	CMake::Conv(bWaterFlow,msg);
	CMake::Conv(bWaterLeak,msg);
	CMake::Conv(Environ,msg);
	CMake::Conv(GasSetting,msg);
	int i;
	for (i=0; i<12; i++) {
		CMake::Conv(Lamp[i],msg);
	}
	CMake::Conv(Tempr,msg);
	CMake::Conv(VacIG,msg);
	CMake::Conv(VacTC,msg);
	CMake::Conv(OpMode,msg);
	CMake::Conv(ServiceState,msg);
	CMake::Conv(bp780,msg);
	CMake::Conv(bp810,msg);
	CMake::Conv(bWaferPresent,msg);
}
