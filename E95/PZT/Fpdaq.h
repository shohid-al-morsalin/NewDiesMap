// FPDAQ.h: interface for the CDAQBoard class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _FPDAQ_H_
#define _FPDAQ_H_

#include "FPDaqBase.h"

class CDAQBoard : public CDAQBoardBase
{
public:
	long GetDigIn();
	virtual void vSetDACOut(int iChan, float fValue);
	virtual void vReassignBuffer();
	virtual BOOL bIsVirtual();
	DWORD iGetMirrorPort32();
	virtual	void vSetDigOut32(DWORD iVal);
	virtual BOOL bInit();
	virtual BOOL bResetADC();
	virtual UINT iGetADCBuffer(float *fData);
	virtual void vAssociateDlg(HWND hDlg);
	virtual BOOL bStopADCContinous();
	virtual BOOL bStartADCContinous();
	virtual float fReadADCSingle(UINT iChan, DBL Gain);
	CDAQBoard();
	virtual ~CDAQBoard();
	virtual int iGetDigOutMirror(int chan);
	virtual void SetDigOut(int chan, int iVal);
	virtual void vSetClockFreq(int iChan, float fFreq);
	virtual void vStartClock(int iChan);
	virtual void vStopClock(int iChan);
	virtual void vStartCount(int iChan);
	virtual void vStopCount(int iChan);
	virtual int  iGetCounter(int iChan);
	virtual BOOL bSetContinousConvert(int iSize, /*float *fWave,*/ int Nchan, int* Chan, float fSampFreq);
	virtual void vGetADCdata(int N, double Freq, double Light, BOOL type);	
	virtual void vGetAmplitude(BOOL type);
	float m_fPulseWidth[MAX_COUNTER];
	//digital outputs
	virtual void vTurnDoorOnOff(BOOL bOnOff);
	virtual void vTurnVacuumOnOff(BOOL bOnOff);
	virtual void vTurnDoorInterlockOnOff(BOOL bOnOff);
	virtual void vTurnLEDOnOff(int Number, BOOL bOnOff);
	virtual void vTurnHLOnOff(int N, BOOL bOnOff);
	//digital inputs
	virtual BOOL bIsDoorOn();
	virtual BOOL bIsVacuumOn();
	virtual BOOL bIsRotationSwitchOn();
	virtual BOOL bIsAirflowFailureOn();
};


#endif