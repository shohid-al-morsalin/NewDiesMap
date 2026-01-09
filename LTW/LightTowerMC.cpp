// LightTowerMC.cpp: implementation of the CLightTowerMC class.
//
//////////////////////////////////////////////////////////////////////

#include "..\..\FPU\stdafx.h"

#include "LightTowerMC.h"
#include "..\..\IPC\SMCHdr.h"
#include "..\..\SYS\var.h"
#include "..\..\GEM\ObjectsContainer.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
CLightTowerMC McTower;
CLightTower &LightTower = McTower;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// @@Vijay#31Jan$03 Instead of throwing in a constructor Implement a good
// .... global singleton scheme (Done)

CLightTowerMC::CLightTowerMC()
{
	Red.Light = RED;
	Red.State = OFF;
	Green.Light = GREEN;
	Green.State = OFF;
	Amber.Light = AMBER;
	Amber.State = OFF;
//	if (LightTower) {
//		throw "Duplicate Instance of LightTower ";
//	}
//	LightTower = this;
}

CLightTowerMC::~CLightTowerMC()
{
	
}


BOOL CLightTowerMC::GreenLight(CLightTower::LIGHTSTATE NewState, BOOL bExclusive)
{
	if (!CanDoIt()) 
	{
		return FALSE;
	}
	CString MsgStr,State;
	StateString(Green.State,State);
	if (Green.State != NewState)
	{
		// Operate on Green Light
		MsgStr.Format("Green Light Change to %s",State);
	}
	else
	{
		MsgStr.Format("Green Light Already %s",State);
	}
	if (bExclusive)
	{
		// Switch off Other Lights
		Red.State = OFF;
		Amber.State = OFF;
		Log("Switch off  Red & Amber Light");
	}
	return TRUE;
}

BOOL CLightTowerMC::RedLight(LIGHTSTATE NewState, BOOL bExclusive)
{
	if (!CanDoIt()) 
	{
		return FALSE;
	}
	CString MsgStr,State;
	StateString(Red.State,State);
	if (Red.State != NewState)
	{
		// Operate on Green Light
		MsgStr.Format("Red Light Change to %s",State);
	}
	else
	{
		MsgStr.Format("Red Light Already %s",State);
	}
	if (bExclusive)
	{
		// Switch off Other Lights
		Green.State = OFF;
		Amber.State = OFF;
		Log("Switch off  Green & Amber Light");
	}
	return TRUE;
}

BOOL CLightTowerMC::AmberLight(LIGHTSTATE NewState, BOOL bExclusive)
{
	if (!CanDoIt()) 
	{
		return FALSE;
	}
	CString MsgStr,State;
	StateString(Amber.State,State);
	if (Amber.State != NewState)
	{
		// Operate on Green Light
		MsgStr.Format("Amber Light Change to %s",State);
	}
	else
	{
		MsgStr.Format("Amber Light Already %s",State);
	}
	if (bExclusive)
	{
		// Switch off Other Lights
		Red.State = OFF;
		Green.State = OFF;
		Log("Switch off  Red & AmGreen Light");
	}
	return TRUE;
}

BOOL CLightTowerMC::AllLightsOff() 
{
	// Set All Lights Off
	Log("Switch off  All Lights ");
	return FALSE;
}

CLightTower::LIGHTSTATE CLightTowerMC::GreenState()
{
	return Green.State;
}

CLightTower::LIGHTSTATE CLightTowerMC::RedState()
{
	return Red.State;
}

CLightTower::LIGHTSTATE CLightTowerMC::AmberState()
{
	return Amber.State;
}

BOOL CLightTowerMC::AllLights(std::vector<LightData> &Lights)
{
	Lights.push_back(Red);
	Lights.push_back(Amber);
	Lights.push_back(Green);
	return TRUE;
}

void CLightTowerMC::Log(const CString &Msg)
{
	CSMClt ipcLog;
	if (ipcLog.Start1(V.LogSvr,sizeof(CSMHdr),FALSE)) {
		ipcLog.LogMsg("LightTower", "", Msg, 10);
	}
}

BOOL CLightTowerMC::CanDoIt()
{
	if ((GEMObjs.GetLineMode() != EqpStatus::NOSECS)&&
		(GEMObjs.GetOpMode() != EqpStatus::MAINT))
	{
		return TRUE;
	}
	return FALSE;
}
