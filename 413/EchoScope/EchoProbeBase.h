// EchoProbeBase.h: interface for the CEchoProbeBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "Peaks.h"
#include "EchoPeak.h"
#include "PosHt.h"
#include "ScanLine.h"
#include "..\..\E95\Recipe413.h"

#define MAXPROBE		2
#define MAXAVEBUFF		128

// #define DEFDATASET	355500L
// #define DEFFREQ		9600000L

//#define DEFDATASET	115000L
//#define DEFFREQ		4100000L

/* 20140108 SCOTT EDIT
#define DEFDATASET	260000L
#define DEFFREQ		7200000L
*/
#define DEFDATASET	125000L
#define DEFFREQ		3600000L

//#define DEFDATASET	300000L
//#define DEFFREQ		2500000L

//#define DEFDATASET	125500L
//#define DEFFREQ		4200000L

#define TOPPRB	0
#define BOTPRB	1

typedef enum { ERROR_STATUS = 0, IDLE_STATUS, READING_STATUS, CALCULATING_STATUS, BUSY_STATUS } STATUS;

typedef void (CALLBACK* lpfnReadingErrorProc)(int, LPVOID);
typedef void (CALLBACK* lpfnReadingEndProc)(LPVOID);
typedef void (CALLBACK* lpfnCalculatingEndProc)(int, LPVOID);

static HANDLE m_hReadingEndThread;

class CSICam;
class CMaxMin;
//class CScanLine;
class CPeakParam;

class CPeaksBuf {
public:
	int Total;
	CEchoPeak Peaks[MAXPKCHAN];
	CPeaksBuf() {
		ResetAll();
	}
	void ResetAll() {
		Total = 0;
		for (int j = 0; j < MAXPKCHAN; j++) {
			Peaks[j].Reset();
		}
	}
};

class CEchoProbeBase {
	STATUS nStatus = IDLE_STATUS; // IDLE, READING, ERROR, CALCULATING or BUSY

protected:
	// These memory must be global [3/15/2010 Valued Customer]
	unsigned short* pAChannelData = nullptr; // save A and B channel raw data, only used for output
	unsigned short* pBChannelData = nullptr; // to be removed soon
	unsigned short* pSignal1Data = nullptr;
	unsigned short* pSignal2Data = nullptr;

	unsigned short* pRawDataArraySpaceA = nullptr;	// raw data from the buffer
	unsigned short* pRawDataArraySpaceB = nullptr;	// raw data from the buffer

	int* pPosData = nullptr; // point to the same address of m_pnDataArraySpace for data handling
	int* pPosDataA = nullptr;

	float* pfThkTBuf = nullptr, * pfThkBBuf = nullptr;  // temporary buffer to collect peaks for averaging [10/9/2011 Administrator]

public:
	short TTLA;
	BOOL bSimu;
	short SimuPkNum;
	CStatic* pGenMessage = NULL;
	short Cnt1, Cnt2, Cnt3TB;		// Number of peaks collected. Transient variable no valid over long period of time [11/5/2011 Administrator]
	CPosHt PosT[16], PosB[16];
	BOOL bSignal;
	short PkNear, PkSmooth;
	float fPeak0[MAXPROBE][MAXAVEBUFF];	// unit in microns [5/2/2013 user]
	float fCalibProbeDistance[MAXPROBE];	// Sample probe distance at calibrate position [3/28/2013 Yuen]

	CScanLine* pSLineT = NULL;		// pSLnT contains reduced data set [5/3/2010 Valued Customer]
	CScanLine* pSLineB = NULL;		// pSLnB contains reduced data set [5/3/2010 Valued Customer]

	// callback function
	lpfnReadingErrorProc m_lpfnReadingErrorProc = NULL;
	lpfnReadingEndProc m_lpfnReadingEndProc = NULL;
	lpfnCalculatingEndProc m_lpfnCalculatingEndProc = NULL;

	enum DOFINDPEAKERR {
		DFPNONE, DFPSTROKEDIRC
	};
	DOFINDPEAKERR DoFindPeakErr;
	enum GETPEAKSERR {
		// Note: GPFOUND1 to GPFOUND3 are not error condition [9/30/2012 Yuen]
		// enum positions are used, value <= GPPKTOTALERR indicate error condition [9/30/2012 Yuen]
		GPNOTSTARTED, GPPARAMERR, GPCAPTUREERR, GPPKHEIGHTERR, GPPKTOTALERR, GPTOPHOK, GPBOTHOK, GPALLHOK
	};

	virtual BOOL SetupEvents(int& ErrStatus, short BoardNum);
	/*virtual*/ GETPEAKSERR GetPeaks(CRecipe413* pRcp, CPeaks& Pk, short nAve, CSICam* pVideo, CListCtrl* pList, MParam* pMPa, short MeSet, CString Name, CPeakParam* PParam = NULL);

	//	virtual void SetBoardNumber(int nBoardNum);

		// start to measure
	virtual BOOL StartMeasurement(BOOL bFilter = FALSE, float fGuessThick = 0, float fGuessRange = 0);

	virtual void ClearEcho(int nBoardNum);

	// set callback functions
	virtual void SetReadingEndProc(lpfnReadingEndProc pFunc = NULL, LPVOID lpParam = NULL);
	virtual void SetReadingErrorProc(lpfnReadingErrorProc pFunc = NULL, LPVOID lpParam = NULL);
	virtual void SetCalculatingEndProc(lpfnCalculatingEndProc pFunc = NULL, LPVOID lpParam = NULL);

	// get the calculated thickness
	float fGetThickness(int Chan, int nPeak1, int nPeak2);
	STATUS GetStatus() // Simulation only
	{
		return nStatus;
	}
	void SetStatus(STATUS sta) {
		nStatus = sta;
	}
	virtual int GetNumberOfData();
	int GetPositionData(int* pfPosition, int nPoint = 1, BOOL bRev = FALSE);
	virtual int GetEcho1Data(int* pfSignal, int* pfPos, int& Xmax, int& Xmin, int nPoint = 1, BOOL bRev = FALSE);
	virtual int GetEcho2Data(int* pfIndex, int* pfPos, int& Xmax, int& Xmin, int nPoint = 1, BOOL bRev = FALSE);
	virtual void GetAChannelData(int* pfChannel, int nPoint = 1, BOOL bRev = FALSE);
	virtual void GetBChannelData(int* pfChannel, int nPoint = 1, BOOL bRev = FALSE);
	virtual void SetThresholdFactor(float fFactor, float fFactor2);

	virtual void FindPeak2(CRecipe413* pRcp, int Chan, float Baseline, CPeaksBuf* pPeaksBuf, CScanLine* pL, int* pP, unsigned short* pS, short MeSet);
	virtual BOOL FindStrokeDirection(short& Directn);
	virtual BOOL DoFindPeak(CRecipe413* pRcp, short MeSet, CString& MsgStr);
	virtual BOOL GetAbsolutePosition(short direction);
	virtual void CopyData2(short MeSet, short direction, float& baseline1, float& baseline2, unsigned short* pA, unsigned short* pB, unsigned short* pS1, unsigned short* pS2);
	/*virtual*/ CEchoProbeBase::GETPEAKSERR ChkPeaks(CRecipe413* pRcp, CPeaks& Pk, short nAve, MParam* pMPa, short MeSet, CPeakParam* PParam, CSICam* pVideo, CListCtrl* pList, CString Name);
	virtual BOOL StartGetPeaks(CListCtrl* pList);

	CEchoProbeBase();
	virtual ~CEchoProbeBase();

public:
	// direction of motion
	short m_nDirection;
	BOOL bDualProbe;
	CPeaksBuf PeaksBuf[MAXPROBE];	// One for top probe and one for bottom probe [10/9/2011 Administrator]

protected:

	// the index when direction changes
	int m_nRangeLevel;

	float RangeT[MAXPEAKCOUNT], RangeB[MAXPEAKCOUNT];
	float RangeT1[MAXPEAKCOUNT], RangeB1[MAXPEAKCOUNT];

	float  m_fThresholdFactor; // adjustable empirical value
	float  m_fThresholdFactor2;

private:
	CRecipe413* pRecipe;

public:
	void FindPeaks(CRecipe413* pRcp, short MeSet);
	BOOL FindStrokeDirc(short& Directn);
	void GenerateFakeData();
	virtual BOOL Initialize(CListBox* pMessage);
	virtual void ReAllocMemory(int size);
	void GeneratePeakSine(unsigned short* pS, short n, short height);
	void GeneratePeakGaussian(unsigned short* pS, short n, short height);
	GETPEAKSERR FilterPeaks(CPeakParam* PParam);
	void CloseDump(FILE* fp);
	void OpenDump(FILE** fp, short id);
	void StoreDianosticInfo(CRecipe413* pRcp, CSICam* pVideo, CString Name, int idx);
	float CalcBaseline(unsigned short* pS);
	void FreeAllocMemory();
	BOOL AllocMemory(int Size);
	void DupRange();
	BOOL RangeVerify(short nP1, short nP2);
	BOOL RangeCheck(float Limit1, short nP1, float Limit2, short nP2);
	void SimuRemoveFlyer();
	float GetRange(float* thk, short cnt, short j);
	float GetAverage(float* thk, short cnt, short j);
	void RemoveFlyer(float* thk1, short* cnt1, short nA1, float* thk2, short* cnt2, short nA2);
	CRecipe413* GetRecipe();
	void SetRecipe(CRecipe413* rp);
	virtual void SaveSpec(CString Name, int counter);
};

extern void* m_lpReadingErrorParam;
extern void* m_lpReadingEndParam;
extern void* m_lpCalculatingEndParam;

extern HANDLE m_hMeasureEndEvent;
extern HANDLE m_hCalculatingEvent;

extern int* pfEcho1Data;
extern int* pfEcho2Data;
extern int* pfXData;

extern size_t m_pointsCnt;

extern int EPMAX_RATE;
extern int NUMBER_OF_INTEREST_DATASETS;

extern BOOL bSaveSpec1;
extern BOOL bSaveSpec2;
extern BOOL bSaveSpec3;
extern BOOL bSaveSpec31;
extern BOOL bSaveSpec4;
extern BOOL bSaveSpec5;

extern FILE* pDmpPeakT, * pDmpPeakB;

extern FILE* gpDump;
extern BOOL gpOn;

#define STDEV_NUMBER 30
