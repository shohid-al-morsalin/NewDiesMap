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

BOOL CALLBACK
GetDriver(LPSTR lpszName, LPSTR lpszEntry, LPARAM lParam)   
/*
this is a callback function of olDaEnumBoards, it gets the 
strings of the Open Layers board and attempts to initialize
the board.  If successful, enumeration is halted.
*/

{
	LPBOARD lpboard = (LPBOARD)(LPVOID)lParam;
	
	/* fill in board strings */
	
#ifdef WIN32
	strncpy(lpboard->name, lpszName, STRLEN);
	strncpy(lpboard->entry, lpszEntry, STRLEN);
#else
	lstrcpyn(lpboard->name, lpszName, STRLEN);
	lstrcpyn(lpboard->entry, lpszEntry, STRLEN);
#endif
	
	/* try to open board */
	
	lpboard->status = olDaInitialize(lpszName, &lpboard->hdrvr);
	if (lpboard->hdrvr != NULL)
		return FALSE;          /* false to stop enumerating */
	else                      
		return TRUE;           /* true to continue          */
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDAQBoard::CDAQBoard()
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


CDAQBoard::~CDAQBoard()
{
	int i;
	for (i = 0; i < m_nCounter; i++) 
		if (m_hCT[i])
			olDaReleaseDASS(m_hCT[i]);
		for (i = 0; i < m_nDigitIO; i++)
			if (m_hDigIO[i])
				olDaReleaseDASS(m_hDigIO[i]);
			if (m_hADC)
				olDaReleaseDASS(m_hADC);
			if (m_hDAC)
				olDaReleaseDASS(m_hDAC);
			if (m_hdrvr)
				olDaTerminate(m_hdrvr);
}


void CDAQBoard::SetDigOut(int chan, int iVal)
{
	ASSERT(chan < MAX_DIGIO);
	if (chan < m_nDigitIO) 
	{
		m_digitalIOMirror[chan] = iVal;
		CHECKERROR(olDaPutSingleValue(m_hDigIO[chan], iVal, 0, m_gain));
	}
}


int CDAQBoard::iGetDigOutMirror(int chan)
{
	ASSERT(chan < MAX_DIGIO);
	return m_digitalIOMirror[chan];
}


void CDAQBoard::vSetClockFreq(int iChan, float fFreq)
{
	BOOL bRestart = FALSE;
	float fMarkSpace;
	ULNG  freq = (LONG)fFreq;
	
	if (iChan >= m_nCounter)
		return;
	
	if (m_CTRunning[iChan])
	{
		vStopClock(iChan);
		bRestart = TRUE;
	}
	olDaSetClockFrequency(m_hCT[iChan], freq);
	fMarkSpace = m_fPulseWidth[iChan] * fFreq * (float)100.0;
	if (fMarkSpace < 1.0)
		fMarkSpace = 1.0;
    olDaSetPulseWidth(m_hCT[iChan], fMarkSpace);
	if (bRestart)
		vStartClock(iChan);
}


void CDAQBoard::vStartClock(int iChan)
{
    ECODE status;
	
	if (iChan >= m_nCounter)
		return;
	if (m_CTRunning[iChan])
		return;
	
	olDaSetCTMode(m_hCT[iChan], OL_CTMODE_RATE);
	olDaSetTrigger(m_hCT[iChan], OL_TRG_SOFT);
    olDaSetGateType(m_hCT[iChan], OL_GATE_NONE);
    olDaSetClockSource(m_hCT[iChan], OL_CLK_INTERNAL);
    // go for it    
    status = olDaConfig(m_hCT[iChan]);
	
	if (status == OLNOERROR)
        status = olDaStart(m_hCT[iChan]);                                      
	m_CTRunning[iChan] = TRUE;
}

void CDAQBoard::vStopClock(int iChan)
{
	if (iChan >= m_nCounter)
		return;
	if (!m_CTRunning[iChan])
		return;
	olDaStop(m_hCT[iChan]);
	m_CTRunning[iChan] = FALSE;
}

void CDAQBoard::vStartCount(int iChan)
{
    ECODE status;
	
	if (iChan >= m_nCounter)
		return;
	if (m_CTCounting[iChan])
		return;
	olDaSetCTMode(m_hCT[iChan], OL_CTMODE_COUNT);
	olDaSetTrigger(m_hCT[iChan], OL_TRG_SOFT);
    olDaSetGateType(m_hCT[iChan], OL_GATE_NONE);
    olDaSetClockSource(m_hCT[iChan], OL_CLK_EXTERNAL);
    // go for it    
    status = olDaConfig(m_hCT[iChan]);
	
	if (status == OLNOERROR)
        status = olDaStart(m_hCT[iChan]);                                      
	m_CTCounting[iChan] = TRUE;
}

void CDAQBoard::vStopCount(int iChan)
{
	if (iChan >= m_nCounter)
		return;
	if (!m_CTCounting[iChan])
		return;
	olDaStop(m_hCT[iChan]);
	m_CTCounting[iChan] = FALSE;
}

int CDAQBoard::iGetCounter(int iChan)
{
    ULNG events;
	if (iChan >= m_nCounter)
		return -1;
    ECODE status = olDaReadEvents(m_hCT[iChan], &events);
	return (int)events;
}

float CDAQBoard::fReadADCSingle(UINT iChan, DBL Gain)
{
	long  value;
	ECODE status;
	BOOL  bRestart = m_bADC_Cont;
	if (iChan >= m_ADC_Channel)
		return 0.0;
		/*
		if (m_bDAC_Cont)
		return 0.0;
	*/
	if (bRestart)
	{
		bStopADCContinous();
	}
	status = olDaSetDataFlow(m_hADC, OL_DF_SINGLEVALUE);
	if (status == OLNOERROR)
		status = olDaConfig(m_hADC);
	if (status == OLNOERROR)
		status = olDaGetSingleValue(m_hADC, &value, iChan, Gain);
   	
	if (m_ADC_encoding != OL_ENC_BINARY)
	{
		/* convert to offset binary by inverting the sign bit */
		
		value ^= 1L <<(m_ADC_resolution - 1);
		value &=(1L << m_ADC_resolution) - 1;     /* zero upper bits */
	}
	if (bRestart)
	{
		vReassignBuffer();
		bStartADCContinous();
	}
	return (float)((float)m_ADC_max- (float)m_ADC_min)/ (1L << m_ADC_resolution) * value + (float)m_ADC_min;
}


// convert DAC and ADC continously, wave form is supplied by fWave
//  assuming running two channels of DAC and ADC
BOOL CDAQBoard::bSetContinousConvert(int iSize, /*float *fWave, */ int Nchan, int* Chan, float fSampFreq)
{
	int i;
	HBUF hBuffer = NULL;
	UINT gainsup;
	
	if (iSize > SAMPLESMAX/Nchan)
		return FALSE;
	m_BufferSize = iSize;
	//  DAC stuff
	/*    CHECKERROR2(olDaGetRange(m_hDAC, &max, &min));
    CHECKERROR2(olDaGetEncoding(m_hDAC, &encoding));
    CHECKERROR2(olDaGetResolution(m_hDAC, &resolution));
	
	  
		CHECKERROR2(olDaGetSSCapsEx(m_hDAC, OLSSCE_MAXTHROUGHPUT, &m_DAC_Freq));
		CHECKERROR2(olDaGetSSCaps(m_hDAC, OLSSC_NUMDMACHANS, &m_dma));
	*/   
	m_dma  = min(1, m_dma);                    /* try for one dma channel          */ 
	m_DAC_Freq = min(fSampFreq, m_DAC_Freq); 
	
	/*	if (!m_bDAC_Cont)
	{
	  CHECKERROR2(olDaSetDataFlow(m_hDAC, OL_DF_CONTINUOUS));
	  CHECKERROR2(olDaSetWrapMode(m_hDAC, OL_WRP_SINGLE));
	  CHECKERROR2(olDaSetClockFrequency(m_hDAC, m_DAC_Freq));
	  CHECKERROR2(olDaSetDmaUsage(m_hDAC, m_dma));
	  CHECKERROR2(olDaSetChannelListEntry(m_hDAC, 0, 0));
	  CHECKERROR2(olDaSetChannelListEntry(m_hDAC, 1, 1));
	  CHECKERROR2(olDaSetChannelListSize(m_hDAC, 2));
	  CHECKERROR2(olDaConfig(m_hDAC));
	  }
	  
		// Put waveform into buffer
	   /* allocate the output buffer */
	
	   /*    CHECKERROR2(olDmCallocBuffer(0, 0, (ULNG) iSize * 2 , 2, &m_hbuf));
	   CHECKERROR2(olDmGetBufferPtr(m_hbuf, (LPVOID*)&m_lpbuf));
	   for (i = 0; i < iSize * 2; i++)
	   {
	   long iDAC;
	   DBL  fVolt = m_BufferSave[i] = (DBL)*fWave++;
	   olDaVoltsToCode(min, max, 1.0, resolution, encoding, fVolt, &iDAC);
	   m_lpbuf[i] = (UINT)iDAC;
	   }
	   CHECKERROR2(olDmSetValidSamples(m_hbuf, iSize*2));
    /* Put the buffer to the DAC */
	//   CHECKERROR2 (olDaPutBuffer(m_hDAC, m_hbuf));
	
	
	// ADC stuff
	CHECKERROR2(olDaGetSSCapsEx(m_hADC, OLSSCE_MAXTHROUGHPUT, &m_DAC_Freq));
	CHECKERROR2(olDaGetSSCaps(m_hADC, OLSSC_NUMDMACHANS, &m_dma));
	CHECKERROR2(olDaGetSSCaps(m_hADC, OLSSC_SUP_PROGRAMGAIN, &gainsup));
	
	m_dma  = min(1, m_dma);                       /* try for one dma channel   */ 
	m_DAC_Freq = min(fSampFreq, m_DAC_Freq);      /* try for 1000hz throughput */
	
	if (!m_bADC_Cont)
	{
		CHECKERROR2(olDaSetDataFlow(m_hADC, OL_DF_CONTINUOUS));
		CHECKERROR2(olDaSetWrapMode(m_hADC, OL_WRP_MULTIPLE));
		CHECKERROR2(olDaSetClockFrequency(m_hADC, m_DAC_Freq*2));
		CHECKERROR2(olDaSetDmaUsage(m_hADC, m_dma));
		CHECKERROR2(olDaSetChannelListSize(m_hADC, Nchan));
		for (i = 0; i < Nchan; i++)
			CHECKERROR2(olDaSetChannelListEntry(m_hADC, i, Chan[i]));
		CHECKERROR2(olDaConfig(m_hADC));
	}
	
	for (i = 0; i < NUM_BUFFERS; i++)
    {
		CHECKERROR2(olDmCallocBuffer(0, 0, (ULNG) iSize * 2, 2, &hBuffer));
		CHECKERROR2(olDaPutBuffer(m_hADC, hBuffer));
    }
	return TRUE;
}

BOOL CDAQBoard::bStartADCContinous()
{
    HBUF  hBuffer = NULL;
	/*	if (!m_bDAC_Cont)
	{
	olDaConfig(m_hDAC);
	olDaStart(m_hDAC);
	}
	*/
	if (!m_bADC_Cont)
	{
		olDaConfig(m_hADC);
		olDaStart(m_hADC);
	}
	m_bADC_Cont = TRUE;
	m_bDAC_Cont = TRUE;
	return TRUE;
}

BOOL CDAQBoard::bStopADCContinous()
{
	int i;
    HBUF  hBuffer = NULL;
	// olDaAbort(m_hDAC);
	olDaAbort(m_hADC);
    olDaGetBuffer(m_hDAC, &m_hbuf);
    olDmFreeBuffer(m_hbuf);
	for (i = 0; i < NUM_BUFFERS; i++)
    {
		olDaGetBuffer(m_hADC, &hBuffer);
		olDmFreeBuffer(hBuffer);
    }
	m_bADC_Cont = FALSE;
	m_bDAC_Cont = FALSE;
	return TRUE;
}

void CDAQBoard::vAssociateDlg(HWND hDlg)
{
	olDaSetWndHandle(m_hADC, hDlg, (UINT)NULL);
}

UINT CDAQBoard::iGetADCBuffer(float *fData)
{
	HBUF hBuffer = NULL, hBuf2 = NULL;
	ULNG samples = 0;
	PDWORD  pBuffer32 = NULL;
	PWORD  pBuffer = NULL;
	DBL  min = 0, max = 0;
	UINT i, encoding = 0, resolution = 0;
	HLIST hList;
	
	/* get buffer off done list */
	i = 0;
	while ((!hBuffer)| (i < 10)) 
	{
		olDaGetBuffer(m_hADC, &hBuffer);
		i++;
	}
	olDmGetListHandle(hBuffer, &hList);
	
	/* if there is a buffer */
	if (hBuffer)
	{
		/* get sub system information for code/volts conversion */
		
		olDaGetRange(m_hADC, &max, &min);
		olDaGetEncoding(m_hADC, &encoding);
		olDaGetResolution(m_hADC, &resolution);
		
		/* get max samples in input buffer */
		
		olDmGetValidSamples(hBuffer, &samples);
		
		/* get pointer to the buffer */
		if (resolution > 16)
		{
			olDmGetBufferPtr(hBuffer, (LPVOID*)&pBuffer32);
			/* get last sample in buffer */
		}
		else
		{
			olDmGetBufferPtr(hBuffer, (LPVOID*)&pBuffer);
		}
		/* put buffer back to ready list */
		
		olDaPutBuffer(m_hADC, hBuffer);
		
		/*  convert value to volts */
		
		if ((samples > 0)& (samples <= SAMPLESMAX))
		{
			for (i = 0; i < samples; i++)
			{					
				ULNG value = pBuffer[i];	
				if (encoding != OL_ENC_BINARY) 
				{
					/* convert to offset binary by inverting the sign bit */
					value ^= 1L <<(resolution - 1);
					value &=(1L << resolution) - 1;     /* zero upper bits */
				}
				* (fData + i) = (float)(((float)max- (float)min)/ (1L << resolution)*value + (float)min);
            }
		}
	}
	return samples;
}

BOOL CDAQBoard::bInit()
{
	BOARD board;
	UINT result;
	int i;
	float fWave;
	
	//	f1 = fopen("wave.txt", "rt");
	for (i = 0; i < 120; i++)
	{
		//		fscanf(f1, "%f\n", &fWave);
		fWave = 0;
		m_Wave[i] = fWave;
	}
	//	fclose(f1);
	m_status  = olDaEnumBoards(GetDriver, (LPARAM)(LPBOARD)&board);
	CHECKERROR2(m_status);
	
	m_hdrvr  = board.hdrvr;
	m_status = board.status;
	strcpy(m_name, board.name);
	strcpy(m_entry, board.entry);
	
	/* check for error within callback function */
	
	CHECKERROR2(m_status);
	
	/* check for NULL driver handle - means no boards */
	
	if (m_hdrvr == NULL)
	{
		MessageBox(NULL, " No Open Layer boards!!!", "Error",
			MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}
	
	
	/* set subsystem for single value operation */
	
	for (i = 0; i < MAX_DIGIO; i++)
		m_digitalIOMirror[i] = 0;
	for (i = 0; i < MAX_DIGIO; i++)
	{
		if (i == 0)
			m_status = olDaGetDASS(m_hdrvr, OLSS_DIN, i, &m_hDigIO[i]);
		else
			m_status = olDaGetDASS(m_hdrvr, OLSS_DOUT, i, &m_hDigIO[i]);
		if (m_status != OLNOERROR)
			break;
		CHECKERROR2(olDaSetDataFlow(m_hDigIO[i], OL_DF_SINGLEVALUE));
		CHECKERROR2(olDaConfig(m_hDigIO[i]));
	}
	m_nDigitIO = i;
	
	for (i = 0; i < MAX_COUNTER; i++)
	{
		m_status = olDaGetDASS(m_hdrvr, OLSS_CT, i, &m_hCT[i]); 
		if (m_status != OLNOERROR)
			break;
	}
	m_nCounter = i;
	
	olDaGetDASS(m_hdrvr, OLSS_AD, 0, &m_hADC); 
	olDaGetDASS(m_hdrvr, OLSS_DA, 0, &m_hDAC);
	// single ended for now
	olDaGetSSCaps(m_hADC, OLSSC_SUP_DIFFERENTIAL, &result);
	if (result)
	{
		olDaSetChannelType(m_hADC, OLSSC_SUP_DIFFERENTIAL);
		olDaGetSSCaps(m_hADC, OLSSC_MAXDICHANS, &m_ADC_Channel);
		bSingleEnded = FALSE;
	}
	else 
	{
		olDaSetChannelType(m_hADC, OLSSC_SUP_SINGLEENDED);
		olDaGetSSCaps(m_hADC, OLSSC_MAXSECHANS, &m_ADC_Channel);
		bSingleEnded = TRUE;
	}
	olDaGetSSCaps(m_hADC, OLSSC_SUP_SINGLEVALUE, &result);
    CHECKERROR2(olDaGetRange(m_hADC, &m_ADC_max, &m_ADC_min));
    CHECKERROR2(olDaGetEncoding(m_hADC, &m_ADC_encoding));
    CHECKERROR2(olDaGetResolution(m_hADC, &m_ADC_resolution));
	//	SetDigOut(0, 0);
	//  SetDigOut(1, 4);
	CHECKERROR2(olDaGetSSCaps(m_hADC, OLSSC_SUP_EXTERNTRIG, &Trig));
	return TRUE;
}

BOOL CDAQBoard::bResetADC()
{
	UINT result;
	
	olDaGetDASS(m_hdrvr, OLSS_AD, 0, &m_hADC); 
	// single ended for now
	olDaGetSSCaps(m_hADC, OLSSC_SUP_DIFFERENTIAL, &result);
	if (result)
	{
		olDaSetChannelType(m_hADC, OLSSC_SUP_DIFFERENTIAL);
		olDaGetSSCaps(m_hADC, OLSSC_MAXDICHANS, &m_ADC_Channel);
		bSingleEnded = FALSE;
	}
	else 
	{
		olDaSetChannelType(m_hADC, OLSSC_SUP_SINGLEENDED);
		olDaGetSSCaps(m_hADC, OLSSC_MAXSECHANS, &m_ADC_Channel);
		bSingleEnded = TRUE;
	}
	olDaGetSSCaps(m_hADC, OLSSC_SUP_SINGLEVALUE, &result);
    CHECKERROR2(olDaGetRange(m_hADC, &m_ADC_max, &m_ADC_min));
    CHECKERROR2(olDaGetEncoding(m_hADC, &m_ADC_encoding));
    CHECKERROR2(olDaGetResolution(m_hADC, &m_ADC_resolution));
	CHECKERROR2(olDaGetSSCaps(m_hADC, OLSSC_SUP_EXTERNTRIG, &Trig));
	bSetContinousConvert(4000/adc.Nchan, adc.Nchan, &adc.Chan[0], float(adc.SampFreq));
	return (TRUE);
}

// each digital channel is 8 bits wide, use 32 bits word to write to four channels at once
//   so don't need two variables to store the data and the channel
void CDAQBoard::vSetDigOut32(DWORD iVal)
{
	int iLowByte, iHighByte;
	iLowByte = iVal & 0xff;			// lower 8 Bytes
	iHighByte = (iVal >> 8) & 0xff; // Higher 8 bytes
	//	SetDigOut(0, iLowByte);
	SetDigOut(1, iHighByte);
/*	iLowByte  = (iVal >> 16) & 0xff;
	iHighByte = (iVal >> 24) & 0xff;
	SetDigOut(2, iLowByte);
	SetDigOut(3, iHighByte);
*/}

DWORD CDAQBoard::iGetMirrorPort32()
{
	int i;
	DWORD iResult = 0;
	for (i = 0; i < 4; i++)
	{
		iResult |=(m_digitalIOMirror[i] & 0xff) <<(8 * i);
	}
	return iResult;
}

BOOL CDAQBoard::bIsVirtual()
{
	return FALSE;
}


void CDAQBoard::vReassignBuffer()
{
	// if continous conversion is stopped for any reason. 
	// This procedure reassign the buffer without the need for the calling
	//  procedure to supply the buffer
	int N = 1, Ch;
	bSetContinousConvert(m_BufferSize, N, &Ch, (float) m_DAC_Freq);
}

void CDAQBoard::vSetDACOut(int iChan, float fValue)
{
	DBL max, min;
	UINT encoding, resolution;
	olDaGetDASS(m_hdrvr, OLSS_DA, 0, &m_hDAC);
	/* set subsystem for single value operation */
	olDaSetDataFlow(m_hDAC, OL_DF_SINGLEVALUE);
	olDaConfig(m_hDAC);
	/* get sub system information for code/volts conversion */
	olDaGetRange(m_hDAC, &max, &min);
	olDaGetEncoding(m_hDAC, &encoding);
	olDaGetResolution(m_hDAC, &resolution);
	int N0 = 2047;
	if (resolution == 16)
		N0 = 32767;
	long Value = long(/*32767*/N0 * (1 + 1.0 * fValue/max));
	olDaPutSingleValue(m_hDAC, Value, iChan, m_gain);
}

void CDAQBoard::vTurnDoorOnOff(BOOL bOnOff)
{
	DWORD iVal = iGetMirrorPort32();
	if (bOnOff)
		iVal |= DOOR_SOL;
	else
		iVal &= ~DOOR_SOL;
	vSetDigOut32(iVal);
}

BOOL CDAQBoard::bIsDoorOn()
{
	long iVal; //= iGetMirrorPort32();
	CHECKERROR2(olDaGetSingleValue(m_hDigIO[0], &iVal, 0, 1.0)); 
	return ((iVal & (DOOR_SOL)) != 0);
}

BOOL CDAQBoard::bIsAirflowFailureOn()
{
	long iVal; //= iGetMirrorPort32();
	CHECKERROR2(olDaGetSingleValue(m_hDigIO[0], &iVal, 0, 1.0)); 
	return ((iVal & (AIRFLOW_SENSOR)) != 0);
}

void CDAQBoard::vTurnVacuumOnOff(BOOL bOnOff)
{
	DWORD iVal = iGetMirrorPort32();
	if (bOnOff)
		iVal |=(VACUUM_SOL);		
	else 
		iVal &= ~(VACUUM_SOL);	
	vSetDigOut32(iVal);
	Sleep(500);
}

void CDAQBoard::vTurnDoorInterlockOnOff(BOOL bOnOff)
{
	DWORD iVal = iGetMirrorPort32();
	if (bOnOff)
		iVal |=(DOOR_INTERLOCK_DISABLE);		
	else 
		iVal &= ~(DOOR_INTERLOCK_DISABLE);	
	vSetDigOut32(iVal);
	Sleep(500);
}

void CDAQBoard::vTurnLEDOnOff(int Number, BOOL bOnOff)
{	DWORD Mask;
	DWORD iVal = iGetMirrorPort32();
	switch (Number) 
	{
		case 0: Mask = LED1; break;
		case 1: Mask = LED2; break;
		case 2: Mask = LED3; break;
	}
	if (bOnOff)
		iVal |=(Mask);		
	else 
		iVal &= ~(Mask);	
	vSetDigOut32(iVal);
	Sleep(100);
}

void CDAQBoard::vTurnHLOnOff(int N, BOOL bOnOff)
{
	DWORD iVal = iGetMirrorPort32();
	switch (N)
	{
		case 0:
			if (bOnOff)
				iVal |=(HAL_LAMP);		
			else 
				iVal &= ~(HAL_LAMP);	
			break;
		case 1:
			if (bOnOff)
				iVal |=(HAL_LAMP2);		
			else 
				iVal &= ~(HAL_LAMP2);	
			break;
		case 2:
			if (bOnOff)
				iVal |=(HAL_LAMP3);		
			else 
				iVal &= ~(HAL_LAMP3);	
			break;
	}
	vSetDigOut32(iVal);
	Sleep(100);
}

BOOL CDAQBoard::bIsVacuumOn()
{
	long iVal; //= iGetMirrorPort32();
	CHECKERROR2(olDaGetSingleValue(m_hDigIO[0], &iVal, 0, 1.0)); 
	return ((iVal & (VACUUM_SWITCH)) != 0);
}

BOOL CDAQBoard::bIsRotationSwitchOn()
{
	long iVal, iVal2;
	int i;
	iVal2 = 0;
	for (i = 0; i < 10; i++)
	{
		CHECKERROR2(olDaGetSingleValue(m_hDigIO[0], &iVal, 0, 1.0)); 
		iVal2 +=(iVal & (ROTARY_SENSOR));
	}
	if (iVal2 >= 5)
		return (true); 
	else 
		return (false);
}

void CDAQBoard::vGetAmplitude(BOOL type)
{
	int i, j, Nstrob = 20, N2 = adc.Nsamples/4/Nstrob;
	double sum;
	if (type)
	{
		// channel without lock-in amplifier	
		sum = 0;
		for (i = 0; i < N2; i++)
			for (j = 0; j < Nstrob; j++)  
				sum +=(adc.fData[2* (Nstrob*2*i + j)] - adc.fData[2* (Nstrob* (2*i + 1) + j)])
				* (adc.fData[2* (Nstrob*2*i + j)] - adc.fData[2* (Nstrob* (2*i + 1) + j)]);
			adc.Signal[0] = sqrt(sum/N2/Nstrob); 
	} 
	else 
	{
		// channel with lock-in amplifier
		sum = 0;
		for (i = 0; i < adc.Nsamples/2; i++)
			sum += adc.fData[2*i];
		adc.Signal[0] = sum/adc.Nsamples*2; 
	}
	sum = 0;
	for (i = 0; i < adc.Nsamples/2; i++)
		sum += adc.fData[2*i + 1];
	adc.Signal[1] = sum/adc.Nsamples*2; 
}

long CDAQBoard::GetDigIn()
{
	long iVal =0; 
	olDaGetSingleValue(m_hDigIO[0], &iVal, 0, 1.0); 
	return iVal;
}


void CDAQBoard::vGetADCdata(int N, double Freq, double Light, BOOL type)
{
/*	int i, NN = 10, count; 
	double sum1, sum2;
	adc.Nfreq = adc.Ndac = N; 
	adc.Freq = Freq;
	adc.Light = Light;
	adc.Nchan = 2; 
	if (type)
		adc.Chan[0] = 0;
	else 
		adc.Chan[0] = 2; 
	adc.Chan[1] = 5;
	m_fPulseWidth[adc.Nfreq] = float(0.99/adc.Freq);
	sum1 = sum2 = 0;
	count = 0;
	for (i = 0; i < SAMPLESMAX; i++)
		adc.fData[i] = 0;
	for (i = 0; i < NN; i++)
	{
		bSetContinousConvert(SAMPLESMAX/2, adc.Nchan, & (adc.Chan[0]), adc.SampFreq);
		bStartADCContinous();	
		adc.Nsamples = iGetADCBuffer(adc.fData);
		bStopADCContinous();
		if (adc.Nsamples > SAMPLESMAX)
			adc.Nsamples = SAMPLESMAX;
		if (adc.Nsamples < 0)
			adc.Nsamples = 0;
		if (adc.Nsamples == SAMPLESMAX)
		{
			vGetAmplitude(type);
			sum1 += adc.Signal[0];
			sum2 += adc.Signal[1];
			count++;
		}
	}
	if (count > 0)
	{
		adc.Signal[0] = sum1/count;
		adc.Signal[1] = sum2/count;
	} 
	else 
	{
		adc.Signal[0] = 0;
		adc.Signal[1] = 0;
	}
*/}
#endif

