#pragma once

#include "OSSBASE.h"

using namespace std;

class CPRJob;
class SecsItem;
class SecsItemList;

class CCtrlJob : public COSSBASE {
	CWnd* pPWnd; //  [9/5/2007 LYF]

public:
	CString ErrorMsg;
	CCtrlJob& operator = (CCtrlJob& Rhs);

	enum STATE {
		NO_STATE = 0, QUEUED, SELECTED, WATINGFORSTART, EXECUTING, PAUSED, COMPLETED, CJACTIVE, ABORTED, STOPPED,
	};
	STATE State; // Use to indicated that CJ is stopping or aborting, so that processing algorithm will known what to do when CARRIERCOMPLETE arrive

	enum STATEEX {
		NORMAL, ABORTING, STOPPING
	};
	STATEEX StateEx;

	BYTE MatlOrderMgmt;
	vector < UINT > PauseEvents;
	BYTE ProcessOrderMgmt;
	CStringArray ProcessingCtrlSpec; // Holds the identifiers for any currently running processing jobs, event if job is
	CStringArray CarrierInputSpec; // A list of carrier ID for material that will be
	BOOL StartMethod; // A logical flag that determines if the ControlJob can start automatically . A user start may come
	// through either the host connection ro the operator console.

public:
	STATE GetState();
	void SetState(CCtrlJob::STATE state);
	BOOL Dump(string& Data);
	CString GetStateStr();
	CString GetStateStr(CCtrlJob::STATE state);
	CString GetStartMethodStr();
	void ClearAll();
	void SetStartMethod(BOOL method);
	void SetCarrierInput(char* mid);
	void SetAddPRjob(char* prjob);
	void SetObjectID(char* objid);
	void StateChange(CCtrlJob::STATE CjState);

	BOOL GetAttr(string& nSer, SecsItemList* pItemList);
	//=================================================/
	// Virtual function Override
	//BOOL GetAttr(CString & service, char * buf);
	//	BOOL GetAttr(string & service, SecsItemList * pList);
	void GetAttrName(CStringArray& nArray);
	BOOL SetAttr(CString& nSer, void* val);
	//=================================================/
	BOOL GetAttr(const string& nSer, string& Data);
	static void GetAttrIDs(vector < string >& IdList);
	BOOL SetAttr(CString& service, vector < UINT > vec);
	BOOL SetAttr(CString& service, CString FromCarrier, CString ToCarrier);
	BOOL SetAttr(CString& service, BYTE POMgmt);
	BOOL SetAttr(CString& service, BOOL bAuto);
	BOOL SetAttr(CString& service, char* val);
	int SetAttr(const string& AttrID, SecsItem* pItem, BOOL bCreating);

	BOOL IsActive();
	BOOL IsQueued();
	BOOL IsWaitingForStart();
	BOOL IsExecuting();
	BOOL IsPaused();

	void Deselect();
	BOOL Abort();
	BOOL Stop();
	BOOL Dequeue();
	void Queued();
	void Select();
	BOOL WaitingForStart();
	BOOL Executing(CWnd* pWnd);
	void Paused();
	void Completed();
	void Resume();

public:
	void Log(CString msg, short lvl = 5);
	BOOL IsCarrierPresent();
	short IsPRJobValid();
	BOOL Aborted();
	BOOL Stopped();
	BOOL SelectPRJob();
	BOOL Aborting();
	BOOL Stopping();
	BOOL CanStart();
	BOOL CanResume();
	BOOL CanCancel();
	BOOL CanDeselect();
	BOOL CanSelect();
	BOOL CanStop();
	BOOL CanAbort();
	BOOL CanPause();
	BOOL CanDequeue();
	BOOL IsAutoStart();
	BOOL CanPRJobStart();
	CPRJob* GetActivePRJob();
	void RemoveAllPRJob();
	CCtrlJob();
	virtual ~CCtrlJob();
};