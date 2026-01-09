// PRJobMgr.h: interface for the CPRJobMgr class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CPRJob;

class CPRJobMgr {
	CCriticalSection CS;

public:
	int MAXPRJOB;
	CPtrList m_PRJobList;

	// Yuen: Add new error code to end of list, never insert new error code [2002/11/07  7:53]
	enum RCODE {
		OK,
		CANNOTPERFORMACTION,
		NOACTIVEPRJOB,
		PRJOBNOTFOUND,
		SETPROCESSINGFAIL,
		SETUPFAIL,
		QUEUEDFAIL,
		ABORTFAIL,
		STOPFAIL,
		STARTFAIL,
		CANCELFAIL,
		SETWAITINGFORSTARTFAIL,
		SETPROCESSINGCOMPLETEFAIL,
		POINTERERROR,
		DEQUEUEOFSOMEPRJOBFAIL,
		DEQUEUEFAIL,
		EQUIPMENTNOTIDLE
	};

	enum PRACTION {
		START,
		STOP,
		PAUSE,
		RESUME,
		ABORT,
		CANCEL
	};

private:
	CPRJob* ActivePJ;
public:
	CString LastErr;

public:
	int GetJobSpace();
	RCODE ClearAllJobs();

public:
	CPRJob* GetPRJob(const char* pPRJob);
	CPRJob* GetPRJob(const CString& PRJobName);

	BOOL IsPRJobExist(const char* pPRJob);
	BOOL IsPRJobExist(const CString& pPRJob);
	RCODE RemovePRJob(CPRJob* pPRJob);
	short GetPRJobCount();
	RCODE GetPRJobList(CStringArray& JList);
	RCODE GetActiveJobList(CStringArray& JList);
	RCODE SetStartMethod(char* pPRJob, BOOL bStart);

	RCODE DequeueAll();
	RCODE Stop(char* pPRJob);
	RCODE Abort(char* pPRJob);
	RCODE Dequeue(char* pPRJob);
	RCODE SetActive(char* pPRJob);
	RCODE Create(CPRJob* pPRJob);
	RCODE SetCompleted(char* pPRJob);
	RCODE Start(char* pPRJob);
	RCODE Resume(char* pPRJob);
	RCODE Pause(char* pPRJob);
	RCODE Cancel(char* pPRJob);

public:
	CPRJobMgr::RCODE  SetCompleted(CPRJob* pPJ);
	BOOL WFS2Start(char* pPrjID);
	CPRJob* GetEOQ();
	CPRJob* GetHOQ();
	BOOL GetMatlData(const char* PrJobID, CString& CarrierID, CString& SlotMap);
	BOOL Aborted(char* pPRJob);
	BOOL Stopped(char* pPRJob);
	RCODE CanCancel(char* pPRJob);
	RCODE CanAbort(char* pPRJob);
	RCODE CanResume(char* pPRJob);
	RCODE CanPause(char* pPRJob);
	RCODE CanStop(char* pPRJob);
	RCODE CanStart(char* pPRJob);
	CPRJobMgr();
	virtual ~CPRJobMgr();
};

extern CPRJobMgr PJobMgr;
