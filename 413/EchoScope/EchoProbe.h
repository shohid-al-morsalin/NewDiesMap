// EchoProbe.h: interface for the CEchoProbe class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "EchoProbeBase.h"

class CEchoProbe : public CEchoProbeBase {
public:
	void SaveSpec(CString Name, int counter);
	BOOL Initialize(CListBox* pMessage);
	void ReAllocMemory(int size);
	void FreeAllocMemory();
	BOOL AllocMemory(int Size);
	BOOL bEventSet;

	void CopyData2(short MeSet, short direction, float& bline1, float& bline2, unsigned short* pA, unsigned short* pB, unsigned short* pS1, unsigned short* pS2);
	BOOL DoFindPeak(CRecipe413* pRcp, short MeSet, CString& MsgStr);
	BOOL SetupEvents(int& ErrStatus, short BoardNum);
	BOOL StartGetPeaks(CListCtrl* pList);

	void SetBoardNumber(int nBoardNum);
	BOOL StartMeasurement(/*short MeSet,*/ BOOL bFilter = FALSE, float fGuessThick = 0, float fGuessRange = 0);

	void ClearEcho(int nBoardNum);

	CEchoProbe();
	virtual ~CEchoProbe();

protected:
	// find the direction of motion
	BOOL FindStrokeDirection(short& Directn);
	// save the data of A B channels
	// void CopyData(short direction);
	//callback function when scan end
	void static CALLBACK OnReadingEnd(int nBoardNum, unsigned int nEventType, unsigned int nEventData, void* pUserData);
	// callback function when error occurs
	void static CALLBACK OnReadingError(int nBoardNum, unsigned int nEventType, unsigned int nEventData, void* UserData);
	// change the pulses of position data to incremental data as a counter
	//BOOL FindCenterOfMassValues(int Chan, double CurPeakWidth, double fThreshold, CEchoPeak *pEp, int nLayerNumber = 0);
};
