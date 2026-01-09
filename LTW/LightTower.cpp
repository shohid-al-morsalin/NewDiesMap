// LightTower.cpp: implementation of the CLightTower class.
//
//////////////////////////////////////////////////////////////////////

#include "..\..\FPU\stdafx.h"
#include "LightTower.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
;//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLightTower::CLightTower()
{

}

CLightTower::~CLightTower()
{

}


BOOL CLightTower::StateString(LIGHTSTATE State,CString &StateStr)
{
	switch(State) 
	{
	case OFF:
		StateStr = "Off";
		break;
	case ON:
		StateStr = "On";
		break;
	case BLINK:
		StateStr = "Blink";
		break;
	default:
		StateStr = "<Unknown State>";
		break;
	}
	return TRUE;
}

BOOL CLightTower::LightString(LIGHT Light,CString &Color)
{
	switch(Light) 
	{
	case RED:
		Color = "Red";
		break;
	case GREEN:
		Color = "Green";
		break;
	case AMBER:
		Color = "Amber";
		break;
	default:
		Color = "<Unknown Color>";
		break;
	}	
	return TRUE;
}
