// FPDAQ.h: interface for the CDAQBoard class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _FPDAQBASE_H_
#define _FPDAQBASE_H_

#include "olmem.h"         
#include "olerrors.h"         
#include "oldaapi.h"

#define STRLEN 80        /* string size for general text manipulation   */
#define MAX_DIGIO 4
#define MAX_COUNTER 4
#define MAX_ADC_SUBSYSTEM 4
#define MAX_BUF_SIZE 4096
//digital outputs
//#define LED1 0x0100
#define LED1 0x04000
#define LED2 0x0200
#define LED3 0x0400
#define HAL_LAMP 0x0800
#define VACUUM_SOL 0x1000
#define DOOR_SOL 0x2000
#define DOOR_INTERLOCK_DISABLE 0x4000
#define HAL_LAMP2 0x0100
#define HAL_LAMP3 0x8000
//digital inputs
// Bit 1
#define AIRFLOW_SENSOR 0x0001 
// Bit 5
#define ROTARY_SENSOR 0x0020 
// Bit 6
#define VACUUM_SWITCH 0x0040
// Bit 7
#define DOOR_SWITCH 0x0080

#define SAMPLESMAX 4000

typedef struct AdcData
{
	int Nfreq;
	double SampFreq;
	int Ndac;
	double Light;
	int Nchan;
	int Chan[5];
	double Signal[5];	
	float fData[SAMPLESMAX];
	int Nsamples;
} ADC;

typedef ADC* LPADC;

typedef struct tag_board {
   HDEV hdrvr;         /* device handle            */
   ECODE status;       /* board error status       */
   HBUF  hbuf;         /* sub system buffer handle */
   PWORD lpbuf;        /* buffer pointer           */
   char name[STRLEN];  /* string for board name    */
   char entry[STRLEN]; /* string for board name    */
} BOARD;

typedef BOARD* LPBOARD;

class CDAQBoardBase //: public CDAQContain
{
public:
	virtual long GetDigIn();
	BOARD board;
	int m_BaseAddAccess;
	virtual void vSetDACOut(int iChan, float fValue);
	int m_BufferSize;
	virtual void vReassignBuffer();
	float m_Wave[120];
	virtual BOOL bIsVirtual();
	virtual DWORD iGetMirrorPort32();
	virtual	void vSetDigOut32(DWORD iVal);
	BOOL bSingleEnded;
	virtual BOOL bInit();
	virtual BOOL bResetADC();
	BOOL m_bDAC_Cont;
	BOOL m_bADC_Cont;
	virtual UINT iGetADCBuffer(float *fData);
	virtual void vAssociateDlg(HWND hDlg);
	int m_Cycles;
	virtual BOOL bStopADCContinous();
	virtual BOOL bStartADCContinous();
	HBUF m_hbuf;
	PWORD m_lpbuf;
	UINT m_dma;
	DBL  m_DAC_Freq;
	virtual float fReadADCSingle(UINT iChan, DBL Gain);
	CDAQBoardBase();
	virtual ~CDAQBoardBase();
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
	ADC adc;
	HDEV m_hdrvr;
	HDASS m_hADC;
	UINT Trig;
protected:
	DBL  m_ADC_max;
	DBL  m_ADC_min;
    UINT m_ADC_resolution, m_ADC_encoding;
	char str[STRLEN];   

//private:
	UINT  m_resolution;
	UINT  m_channel, m_ADC_Channel;
	DBL   m_gain;
	int   m_nCounter;
	int   m_nDigitIO;
	BOOL  m_CTRunning[MAX_COUNTER];
	BOOL  m_CTCounting[MAX_COUNTER];
	int	  m_digitalIOMirror[MAX_DIGIO]; 
	HDASS m_hCT[MAX_COUNTER];
	HDASS m_hDAC;
	HDASS m_hDigIO[MAX_DIGIO];
    ECODE m_status;       /* board error status       */
	char  m_name[STRLEN];
	char  m_entry[STRLEN];
	float m_BufferSave[MAX_BUF_SIZE];
};


#endif