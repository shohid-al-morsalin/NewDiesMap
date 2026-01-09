#pragma once

#include "ONLINE/GEM/OSSBASE.H"
#include "IPC/Constant.h"
#include "IPC/OSSMemS.h"

#include <string>
#include <vector>

#define PRJOBSTATELEN		64

class SecsItemList;
struct ParamNew;

struct WaferStartInfo {
	short ScanNo; //  [9/10/2007 LYF]
	short SlotNo; // slot no
	float Waferthk; // wafer thickness;
	float Filmthk; // film thickness;
	char LotID[LOTIDLEN + 1];
	char Waferid[OBJIDLEN + 1]; // wafer id

	WaferStartInfo() {
		ClearAll();
	};

	void ClearAll() {
		ScanNo = 0;
		SlotNo = 0;
		Waferthk = 0;
		Filmthk = 0;
		ZeroMemory(LotID, sizeof(LotID));
		ZeroMemory(Waferid, sizeof(Waferid));
	};
};

class CPRJob : public COSSBASE {
public:
	// Pure Virtual function Override
	void GetAttrName(CStringArray& nArray);
	BOOL SetAttr(CString service, void* data);
	BOOL GetAttr(std::string service, SecsItemList* pList);
	BOOL GetAttr(CString service, void* buf);
	BOOL GetAttr(const std::string& nSer, std::string& Data);

public:
	enum PRMTLTYPE {
		WAFER = 1, CASSETTE, DIEORCHIPS, BOATS, INGOTS, LEADFRAMES, LOTS, MAGAZINES, PACKAGES, PLATES, TUBES, WAFERFRAMES,
		CARRIERS, SUBSTRATES
	};

	// YFL: SEMI-0701 [1/11/2005]
	enum PRJOBSTATE {
		NO_STATE = -1, QUEUED_POOLED, SETTING_UP, WAITING_FOR_START, PROCESSING, PROCESS_COMPLETE, EXECUTING,
		PAUSING, /*PAUSE,*/
		PAUSED, STOPPING, ABORTING
	};

	BYTE MF;
	PRJOBSTATE JobState;
	PRJOBSTATE ResumeState;

	enum PRJOBMILESTONES {
		alSETTING_UP, alPROCESSING, alCOMPLETED_PROCESSING, alJOB_COMPLETE, alJOB_WAITITNG_FOR_START, alJOB_WAITITNG_FOR_MATL
	};

public:
	std::vector < UINT > m_PauseEvents;
	std::vector < BYTE > m_Slots;
	char m_PRJobState[PRJOBSTATELEN + 1]; // A unique sub-state of the job according to the process job statemodel
	char m_PRMtlName[OBJIDLEN + 1]; // Textual identifier of the material being processed.
	std::vector < std::string > MatlNameList; // Material name list
	PRMTLTYPE m_PRMtlType; // Identifies the type of material being processed.
	BOOL m_PRProcessStart; // TRUE is a AutoStart, FALSE is a manual
	WaferStartInfo m_Param[26];
	short m_PortNo;
	short m_Paramcnt;
	std::string m_Recipe; // Recipe to be used for this Job
	CString ErrorMsg;

public:
	void SetObjectID(char* pObjID);
	BOOL CanActive();
	CString GetJobParamStr(int i);
	void SetWaferStartInfo(WaferStartInfo& Param, int arryno);
	BOOL Stopped();
	BOOL Setup();
	void SetWaferParams(ParamNew& Param, int arryno);
	BOOL Start();
	void SetState(CPRJob::PRJOBSTATE State);
	CPRJob::PRJOBSTATE GetState();
	BOOL Dump(std::string& Data);
	CString GetJobStateStr();
	static CString GetJobStateStr(CPRJob::PRJOBSTATE state);
	CString GetStartMethodStr();
	BOOL AbortProcessing();
	BOOL ResumeProcessing();
	BOOL PauseProcessing();
	BOOL StopProcessing();
	void AlertNotify(CPRJob::PRJOBMILESTONES MileStone);
	BOOL SetMF(BYTE MF);
	void SetParamWaferThickness(float val, int arryno);
	void SetParamFilmThickness(float val, int arryno);
	void SetParamLotID(char* lotid, int arryno);
	void SetParamWaferID(char* waferid, int arryno);
	void SetParamSlotNo(short slotno, int arryno);
	void SetParamPtnID(short ptn);
	void SetPRStart(BOOL start);
	void SetMtlType(short type);
	void SetMtlName(const char* name);
	void WriteStartData413();
	void GetCurrTime(std::string& TimeStr);
	void StateChange(PRJOBSTATE NewState);
	void ClearAll();

public:
	BOOL IsActive();
	BOOL IsPaused();
	BOOL IsExecuting();
	BOOL IsProcessing();
	BOOL IsAutostart();
	BOOL IsWaitingForStart(void);
	BOOL IsQueued();
	BOOL IsNoState();

	BOOL CanCancel();
	BOOL CanSetup();
	BOOL CanStop();
	BOOL CanAbort();
	BOOL CanDequeue();
	BOOL CanPause();
	BOOL Cancel();
	BOOL CanResume();

	//  [10/26/2002]
public:
	BOOL SettingUp();
	BOOL Queued();
	BOOL Abort();
	BOOL Stop();
	BOOL Dequeue();
	BOOL Paused();
	BOOL Pausing();
	BOOL Resume();
	BOOL Processing();
	BOOL ProcessingComplete();
	BOOL WaitingStart();

public:
	void Log(CString msg, short lvl = 5);
	BOOL WFS2Start();
	BOOL GetCarrierAndSM(CString& CarrierID, CString& Slotmap);
	BOOL Aborted();
	BOOL IsStopping();
	BOOL CanStart();
	BOOL IsAborting();
	BOOL IsPausing();
	BOOL CanProcess();
	BOOL IsSettingUp();
	CPRJob();
	virtual ~CPRJob();
	static void GetAttrIDs(std::vector < std::string >& IdList);
private:
};