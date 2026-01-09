#pragma once

#include "ONLINE/INC/E5-ErrCodes.h"

#include <vector>
#include "PRJobMgr.h"

//using namespace std;

class CPRJob;
class CCtrlJob;

class CCtrlJobMgr : public CPRJobMgr {
	CWnd* pPWnd;
	CCriticalSection CS;

public:
	CString ErrorMsg;
	BOOL bKeepCompletedCJob;
	CPtrList m_CJobList;
	CPtrList m_CmplJobList;
	int MAXCONTROLJOB;
private:
	CCtrlJob* ActiveCJ;
public:

public:
	BOOL RemoveCJob(CCtrlJob* q);
	CCtrlJob* GetHOQ();
	BOOL AddToCompletedList(CCtrlJob* q);
	BOOL CleanupCancelCarrier(const char* CarrierID);
	BOOL ActivateNextCJob();
	BOOL GetActiveCJobList(CStringArray& JList);
	CCtrlJob* GetCJob(const CString& cjobname);
	BOOL GetCJobList(CStringArray& JList);
	E5_ERRORS CanStop(char* pObjid);
	E5_ERRORS CanAbort(char* pObjid);
	E5_ERRORS CanPause(char* pObjid);
	E5_ERRORS CanResume(char* pObjid);
	E5_ERRORS CanCancel(char* pObjid);
	E5_ERRORS CanStart(char* objid);
	E5_ERRORS CanSelect(char* pObjid);
	E5_ERRORS CanDeselect(char* objid);
	int CanHOQ(char* objid);
	void Completed(char* objid);
	E5_ERRORS Start(char* objid);
	E5_ERRORS Abort(char* pObjid);
	E5_ERRORS Stop(char* pObjid);
	E5_ERRORS Cancel(char* pObjid);
	E5_ERRORS Resume(char* pObjid);
	E5_ERRORS Pause(char* pObjid);
	E5_ERRORS Dequeue(char* pObjid);
	E5_ERRORS DequeueAll();
	E5_ERRORS Deselect(char* objid);
	BOOL Create(CCtrlJob* cjob);
	// Helpers for Events
	BOOL GetCJPJIDFromWaferID(CString& WaferID, CString& CJID, CString& PJID);
	// Helpers
public:
	CCtrlJobMgr();
	virtual ~CCtrlJobMgr();

	// Yuen: May not be used section [2002/11/07  8:12]
public:
	void Register(CWnd* pWnd);
	BOOL ActivateNextPRJob(CString CJobName);
	BOOL StartCJob(CString CJobName, CWnd* pWnd);
	CCtrlJob* GetCJobHOQ();
	// 	void StartCJob();
	void Stop(short unit);
	void Abort(short unit);
	BOOL CtrlJobIDFromPj(const char* pPrjobID, CString& CjobID);
	void Log(CString Msg, short Lvl = 0);
	UINT ProcessJobReferanceCount(const char* PrJobID);
	BOOL IsCarrierInProcess(const char* CarrierID);
	BOOL CJobIdsFromCarrier(const char* CarrierID, vector<string>& CjobIds);

	BOOL Aborted(char* prjob);
	BOOL ActivateNextPRJob();
	BOOL Stopped(char* cjid);
	CCtrlJob* GetActiveCJob();
	CPRJob* GetActivePRJob();
	CCtrlJob* GetCmplCJob(char* pObjid);
	void ClearAllJobs();
	int GetJobSpace();
	void GetQueuedCJobList(vector <string>& JList);
	BOOL IsCarrierPresent(const CString& CarrierID);
	CCtrlJob* GetCJob(char* pObjid);
	BOOL ProcessingCompleted(CCtrlJob* p);

	E5_ERRORS CanDequeue(char* pObjid);
	E5_ERRORS Select(char* objid);
};

extern CCtrlJobMgr CJobMgr;
