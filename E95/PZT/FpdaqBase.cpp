// FPDAQ.cpp

#include "stdafx.h"
#include "FPDAQ.h"
#include "mmsystem.h"
#include <math.h>

#define NUM_BUFFERS 16

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
#ifndef DEBUG_EMU

#define SHOW_ERROR(ecode) MessageBox(NULL, olDaGetErrorString(ecode,\
                  str, STRLEN), "Error", MB_ICONEXCLAMATION | MB_OK);
#define CHECKERROR(ecode) if ((m_status = (ecode)) != OLNOERROR)\
{\
	SHOW_ERROR(m_status);\
	olDaReleaseDASS(m_hADC);\
	olDaTerminate(m_hdrvr);\
	m_hdrvr = 0;\
	return;\
}
#define CHECKERROR2(ecode) if ((m_status = (ecode)) != OLNOERROR)\
{\
	return FALSE;\
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDAQBoardBase::CDAQBoardBase()
{
	int i;
	m_bDAC_Cont = FALSE;
	m_bADC_Cont = FALSE;
	m_channel = 0;
	m_gain    = 1.0;
	m_Cycles  = 16;
	
	for (i = 0; i < MAX_COUNTER; i++)
	{
		m_hCT[i] = NULL;
		m_CTRunning[i]   = FALSE;
		m_CTCounting[i]  = FALSE;
		m_fPulseWidth[i] = (float)2.0e-5;
	}
	for (i = 0; i < MAX_DIGIO; i++)
		m_hDigIO[i] = NULL;
	m_hADC	 = NULL;
	m_hDAC	 = NULL;
	m_hdrvr  = NULL;
}


CDAQBoardBase::~CDAQBoardBase()
{
}


void CDAQBoardBase::SetDigOut(int chan, int iVal)
{
}


int CDAQBoardBase::iGetDigOutMirror(int chan)
{
	return 1;
}


void CDAQBoardBase::vSetClockFreq(int iChan, float fFreq)
{
}


void CDAQBoardBase::vStartClock(int iChan)
{
}

void CDAQBoardBase::vStopClock(int iChan)
{
}

void CDAQBoardBase::vStartCount(int iChan)
{
}

void CDAQBoardBase::vStopCount(int iChan)
{
}

int CDAQBoardBase::iGetCounter(int iChan)
{
	return 0;
}

float CDAQBoardBase::fReadADCSingle(UINT iChan, DBL Gain)
{
	return (float)(1.2345);
}


// convert DAC and ADC continously, wave form is supplied by fWave
//  assuming running two channels of DAC and ADC
BOOL CDAQBoardBase::bSetContinousConvert(int iSize, /*float *fWave, */ int Nchan, int* Chan, float fSampFreq)
{
	return TRUE;
}

BOOL CDAQBoardBase::bStartADCContinous()
{
	return TRUE;
}

BOOL CDAQBoardBase::bStopADCContinous()
{
	return TRUE;
}

void CDAQBoardBase::vAssociateDlg(HWND hDlg)
{
}

UINT CDAQBoardBase::iGetADCBuffer(float *fData)
{
	for (int i = 0; i < SAMPLESMAX; i++)
	{					
		* (fData + i) = (float) 1.2345;
	}
	return SAMPLESMAX;
}

BOOL CDAQBoardBase::bInit()
{
	return TRUE;
}

BOOL CDAQBoardBase::bResetADC()
{
	return TRUE;
}

// each digital channel is 8 bits wide, use 32 bits word to write to four channels at once
//   so don't need two variables to store the data and the channel
void CDAQBoardBase::vSetDigOut32(DWORD iVal)
{
}

DWORD CDAQBoardBase::iGetMirrorPort32()
{
	return 0x5555;
}

BOOL CDAQBoardBase::bIsVirtual()
{
	return FALSE;
}


void CDAQBoardBase::vReassignBuffer()
{
}

void CDAQBoardBase::vSetDACOut(int iChan, float fValue)
{
}

void CDAQBoardBase::vTurnDoorOnOff(BOOL bOnOff)
{
}

BOOL CDAQBoardBase::bIsDoorOn()
{
	return (true);
}

BOOL CDAQBoardBase::bIsAirflowFailureOn()
{
	return (false);
}

void CDAQBoardBase::vTurnVacuumOnOff(BOOL bOnOff)
{
}

void CDAQBoardBase::vTurnDoorInterlockOnOff(BOOL bOnOff)
{
}

void CDAQBoardBase::vTurnLEDOnOff(int Number, BOOL bOnOff)
{
}

void CDAQBoardBase::vTurnHLOnOff(int N, BOOL bOnOff)
{
}

BOOL CDAQBoardBase::bIsVacuumOn()
{
	return (true);
}

BOOL CDAQBoardBase::bIsRotationSwitchOn()
{
	return (true); 
}

void CDAQBoardBase::vGetAmplitude(BOOL type)
{
	adc.Signal[0] = 1; 
	adc.Signal[1] = 2; 
}

long CDAQBoardBase::GetDigIn()
{
	long iVal =0; 
	for (int i = 0;i<7;i++)
	{
		m_hDigIO[0] = (HDASS) 1;
	}
	return iVal;
}


void CDAQBoardBase::vGetADCdata(int N, double Freq, double Light, BOOL type)
{
}
#endif

