// OOPARS.cpp: implementation of the OOPARS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OOPARS.h"

#include "..\SRC\DOSUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OOPARS::OOPARS()
{
 	fThkFactor = 1.0;
	fPeakMin = 1.0;
	fThkMin = 1.0;
	fThkMax = 3.0;


	nMethod = 0;
	fCalCoeff = 1.0;
	fRefrIndex = 3.69f;

	boxcar = 0;
	nMeastime = 80;
	nPort = 4;
	nRepeat = 4;
}

OOPARS::~OOPARS()
{
}

void OOPARS::LoadParam()
{
	char str[256];
	CString inifile;
	
	DosUtil.GetLocalCfgFile(inifile);
	
	CString sec = "Spectrometer";
	
	boxcar = GetPrivateProfileInt(sec, "boxcar", boxcar, inifile);;
	nMeastime = GetPrivateProfileInt(sec, "meastime", nMeastime, inifile);
	nMethod = GetPrivateProfileInt(sec, "Method", nMethod, inifile);;
	nPort = 4;
	
	sprintf(str, "%.3f", fPeakMin);
	GetPrivateProfileString(sec, "PeakMin", str, str, 255, inifile);
	fPeakMin = atof(str); 

	sprintf(str, "%.3f", fThkMin);
	GetPrivateProfileString(sec, "MinFilmThk", str, str, 255, inifile);
	fThkMin = atof(str);

	sprintf(str, "%.3f", fThkMax);
	GetPrivateProfileString(sec, "MaxFilmThk", str, str, 255, inifile);
	fThkMax = atof(str);

	sprintf(str, "%.3f", fCalCoeff);
	GetPrivateProfileString(sec, "CalCoeff", str, str, 255, inifile);
	fCalCoeff = atof(str);
}

void OOPARS::SaveParam()
{
	char str[256];
	CString inifile;
	
	DosUtil.GetLocalCfgFile(inifile);
	
	CString sec = "Spectrometer";
	
	sprintf(str,"%d", boxcar);
	WritePrivateProfileString(sec, "boxcar", str, inifile);
	
	sprintf(str,"%d", nMeastime);
	WritePrivateProfileString(sec, "meastime", str, inifile);
	
	sprintf(str,"%d", nMethod);
	WritePrivateProfileString(sec, "Method", str, inifile);
	
	sprintf(str,"%.3f", fPeakMin);
	WritePrivateProfileString(sec, "PeakMin", str, inifile);
	
	sprintf(str,"%.3f", fThkMin);
	WritePrivateProfileString(sec, "MinFilmThk", str, inifile);
	
	sprintf(str,"%.3f", fThkMax);
	WritePrivateProfileString(sec, "MaxFilmThk", str, inifile);
	
	sprintf(str,"%.3f", fCalCoeff);
	WritePrivateProfileString(sec, "CalCoeff", str, inifile);
}
