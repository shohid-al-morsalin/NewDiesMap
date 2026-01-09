// SiMotr.cpp: implementation of the CSiMotr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SiMotr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSiMotr *pDIO = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSiMotr::CSiMotr()
{
	bSimu = TRUE;
}

CSiMotr::~CSiMotr()
{

}

BOOL CSiMotr::Open(short port)
{
	return TRUE;
}

BOOL CSiMotr::CheckBit(short Bit)
{
	return TRUE;
}

BOOL CSiMotr::SetDigitalOut(unsigned char Byte, BOOL bHigh)
{
	return TRUE;
}

BOOL CSiMotr::DigOutM2(unsigned char Bit, BOOL bHigh)
{
	return TRUE;
}

BOOL CSiMotr::DigOutM1(unsigned char Bit, BOOL bHigh)
{
	return TRUE;
}

BOOL CSiMotr::DigInM2(short Bit)
{
	return FALSE;
}

BOOL CSiMotr::DigInM1(short Bit)
{
	return FALSE;
}

BOOL CSiMotr::IsLightCurtainTriggered()
{
	return FALSE;
}

BOOL CSiMotr::TestClose()
{
	return TRUE;
}

BOOL CSiMotr::TestOpen()
{
	return TRUE;
}

CString CSiMotr::GetDigitalIn()
{
	return "????????";
}

void CSiMotr::SelectLaser(short wavelength)
{

}

CString CSiMotr::GetDigitalInChamber()
{
	return "SI=00000000";
}

short CSiMotr::GetWaferPresent()
{
	return 0;
}

BOOL CSiMotr::OpenChamber()
{
	return TRUE;
}

BOOL CSiMotr::CloseChamber()
{
	return TRUE;
}
