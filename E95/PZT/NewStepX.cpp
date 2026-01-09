// NewStep.cpp: implementation of the CNewStep class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dos.h"
#include "NewStep.h"
#include <complex>
#include "../FFT.h"

#include "AppGlobal.h"

//using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewStep::CNewStep()
{
	nPort = 3;
	nRepeat = 1;
	nWaittime = 500;
	nController = 1;

	char inifile[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetModuleFileName(NULL, inifile, _MAX_PATH);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "FSMLocalCfg", "ini");

	nPort = GetPrivateProfileInt("Tilt", "PortNo", 3, inifile); 
	nWaittime = GetPrivateProfileInt("Tilt", "WaitTime", 500, inifile); 
	nRepeat = GetPrivateProfileInt("Tilt", "Repeats", 10, inifile); 

	bRegistered = FALSE;
	bBusy = FALSE;
//	Initialize();
}

CNewStep::~CNewStep()
{
}

BOOL CNewStep::SetAxis(int nAxis)
{
	int /*i,*/ j, k;
	char str[255];

	if (bBusy)
		return FALSE;
	bBusy = TRUE;

	sprintf(str, "%dMX%d%c%c", nController, nAxis, 13, 10);
	Port.SendData(str,6);
	Global.Sleep2(nWaittime);
	sprintf(str, "%dMX?%c%c", nController, 13, 10);
	Port.SendData(str,6);
	Global.Sleep2(nWaittime);
	Port.ReadData(str,255);
	for (k=0;k<255;k++)
		if (str[k] == '?') break;		
	j = k;
	for (k=0;k<255-j-2;k++)
	{
		str[k] = str[j+2+k];		
		if (str[k] == 10) break;
	}
	j = atoi(str);
	if (j != nAxis)
	{
		sprintf(str, "Failed to switch to axis %d\n", nAxis);
		TRACE(str);
		bBusy = FALSE;
		return FALSE;
	}
	bBusy = FALSE;
	return TRUE;
}

BOOL CNewStep::HomeActuator(int nAxis)
{
	int i, j, k;
	char str[255];

	if (!SetAxis(nAxis))
		return FALSE;

	bBusy = TRUE;

	sprintf(str, "%dOR%c%c", nController, 13, 10);
	Port.SendData(str,5);
	Global.Sleep2(5*nWaittime);
	j = 0;
	i=0; while ((j != 1 )&&(i<10*nRepeat))
	{
		sprintf(str, "%dOR?%c%c", nController, 13, 10);
		Port.SendData(str,6);
		Global.Sleep2(5*nWaittime);
		Port.ReadData(str,255);
		for (k=0;k<255;k++)
			if (str[k] == '?') break;		
		j = k;
		for (k=0;k<255-j-2;k++)
		{
			str[k] = str[j+2+k];		
			if (str[k] == 10) break;
		}
		j = atoi(str);
			i++;
	}

	sprintf(str, "%dTP%d?%c%c", nController, nAxis, 13, 10);
	Port.SendData(str,7);
	Global.Sleep2(nWaittime);
	Port.ReadData(str,255);
	for (k=0;k<255;k++)
		if (str[k] == '?') break;		
	i = k;
	for (k=0;k<255-i-2;k++)
	{
		str[k] = str[i+2+k];		
		if (str[k] == 10) break;
	}
	i = atoi(str);

	Global.Sleep2(nWaittime);
	if (j != 1)
	{
		sprintf(str, "Failed to home axis %d. Pos = %d\n", nAxis, i);
		TRACE(str);
		bBusy = FALSE;
		return FALSE;
	}

	sprintf(str, "Axis %d homed OK. Pos = %d\n", nAxis, i);
	TRACE(str);
	bBusy = FALSE;
	return TRUE;
}


BOOL CNewStep::MoveActuator(int nAxis, int nPosition)
{
	int i, j, k;
	char str[255];

	if (!SetAxis(nAxis))
		return FALSE;

	bBusy = TRUE;

	sprintf(str, "%dPA%d%c%c", nController, nPosition, 13, 10);
	j = 6 + (nPosition>10) + (nPosition>100) + (nPosition>1000) + (nPosition>10000);
	Port.SendData(str,j);
	Global.Sleep2(nWaittime);
	i=0;
	j = 0;
	while ((j!=nPosition) && (i < nRepeat))
	{
		sprintf(str, "%dTP%d?%c%c", nController, nAxis, 13, 10);
		Port.SendData(str,7);
		Global.Sleep2(nWaittime);
		Port.ReadData(str,255);
		for (k=0;k<255;k++)
			if (str[k] == '?') break;		
		j = k;
		for (k=0;k<255-j-2;k++)
		{
			str[k] = str[j+2+k];		
			if (str[k] == 10) break;
		}
		j = atoi(str);
//		if (j == nPosition) 
//			i=nRepeat;
//		else
			i++;
	}
	Global.Sleep2(nWaittime);
	if (j != nPosition)
	{
		sprintf(str, "Axis %d has failed to reach pos %d. Pos= %d\n", nAxis, nPosition, j);
		TRACE(str);

		bBusy = FALSE;
		return FALSE;
	}
	sprintf(str, "Axis %d has moved to pos %d\n", nAxis, nPosition);
	TRACE(str);

	bBusy = FALSE;
	return TRUE;
}

int CNewStep::GetPosition(int nAxis)
{
	int j, k;
	char str[255];

	if (!SetAxis(nAxis))
		return -9999;
	
	bBusy = TRUE;

	sprintf(str, "%dTP%d?%c%c", nController, nAxis, 13, 10);
	Port.SendData(str,7);
	Global.Sleep2(nWaittime);
	Port.ReadData(str,255);
	for (k=0;k<255;k++)
		if (str[k] == '?') break;		
	j = k;
	for (k=0;k<255-j-2;k++)
	{
		str[k] = str[j+2+k];		
		if (str[k] == 10) break;
	}
	j = atoi(str);
	sprintf(str, "Axis %d position = %d\n", nAxis, j);
	TRACE(str);
	bBusy = FALSE;
	return j;
}

BOOL CNewStep::IsHomed(int nAxis)
{
	char str[255]; 
	int j=0, k;

	if (!SetAxis(nAxis))
		return FALSE;	

	bBusy = TRUE;
	sprintf(str, "%dOR?%c%c", nController, 13, 10);
	Port.SendData(str,6);
	Global.Sleep2(5*nWaittime);
	Port.ReadData(str,255);
	for (k=0;k<255;k++)
		if (str[k] == '?') break;		
	j = k;
	for (k=0;k<255-j-2;k++)
	{
		str[k] = str[j+2+k];		
		if (str[k] == 10) break;
	}
	j = atoi(str);
	sprintf(str, "Axis %d home = %d\n", nAxis, j);
	TRACE(str);
	bBusy = FALSE;
	return j;	
}

BOOL CNewStep::Initialize()
{
	char str[255]; 
// 	int /*i,*/ j, k;

	if (bBusy)
		return FALSE;

	if (!bRegistered)
	{
		bBusy = TRUE;
		Port.Open(nPort, 19200, 'n', 8, 1);
		sprintf(str, "%dRS%c%c", nController, 13, 10);
		Port.SendData(str,5);
		Global.Sleep2(10*nWaittime);
		sprintf(str, "%dDH0%c%c", nController, 13, 10);
		Port.SendData(str,6);
		Global.Sleep2(nWaittime);
		bBusy = FALSE;
	}
/*	for (i=0;i<3;i++)
	{
		if (!IsHomed(i+1))
			HomeActuator(i+1);
		Global.Sleep2(nWaittime);
	}*/
	bRegistered = TRUE;
	return TRUE;
}



/*-----------------------------------------------------------------------------------------
AC	Acceleration
AU	Set MAX acceleration (0-500)
BA	Set backlash compensation (0-512)
BX	Scan  switchbox
BZ	Restore EEPROM content to default
DH	Define home posititon
ID	Set positioner identification	
JA	Start jog motion
JS	Set jog scale-up factor
KM	Knob mode	
MF	Motor OFF
MO	Motor ON
MX	Select switchbox channel
OA	Homing Acceleration
OH	Homing speed
OR	Search for home	
PA	Move to absolute position
PD	Position delay in knob control
PH	Get hardware status
PR	Move to relative position	
RS	Reset controller 
SA	Set controller address
SH	Set hardware configuration	
SL	Set left travel limit
SM	Save settings to non-volatile memory
SP	Set position for knob control
SR	Set right travel limit
ST	Stop motion
SV	Set velocities for knob control
TE	Read error code
TP	Read position
TS	Controller status
VA	Set velocity
VE	Read controller firmware version
VU	Set maximum velocity (0-156.2)
---------------------------------------------------------------------------------------*/