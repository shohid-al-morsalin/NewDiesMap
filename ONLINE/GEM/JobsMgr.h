// JobsMgr.h: interface for the CJobsMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JOBSMGR_H__145CF80A_5CDC_4B5E_BB78_E78860B80E68__INCLUDED_)
#define AFX_JOBSMGR_H__145CF80A_5CDC_4B5E_BB78_E78860B80E68__INCLUDED_
#include "CtrlJobMgr.h"
#include "PRJobMgr.h"
#include "JobsMgrBase.h"
class CCtrlJob;
class CPRJob;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CJobsMgr : public CJobsMgrBase  
{
public:
    enum FSMERRCODE { 
		NO_FSM_ERROR, 
			MEM_ALLOC 
	};

	
	CJobsMgr();
	virtual ~CJobsMgr();	

	/**
	 * Add a P Job to the system.
	 * 
	 * Check 1. 1availability of recipe, 2 Duplication of P Job 
	 */
	void AddPJ(CPRJob * pPJ);

	/**
	 * Add a C Job to the system.
	 * No checks. 
	 */
	void AddCJ(CCtrlJob * pCJ);



	/**
	 * Caller Tries to start a CJ that is in WAITINGFORSTART state. If there are more then one CJ in WAITINGFORSTART state, FIFO will applied.
	 * 
	 * Checks 1. Referenced PJ available. 2. Items referenced by PJ available.
	 */
	FSMERRCODE StartCJ(char * pCJ);

	/**
	 * Caller inform CJobsMgr that the STOPPING CJ has stopped. 
	 */
	void CJStopped(char * pCJ);

	/**
	 * Caller tries to stop the active CJ.
	 * CJobsMgr will return the CJ that it transit to STOPPING state. 
	 */
	FSMERRCODE StopCJ(char *pCJ);

	FSMERRCODE CancelCJ(char * pCJ);

	void CJAborted(char * pCJ);

	FSMERRCODE AbortCJ(char *pCJ);

	FSMERRCODE PauseCJ();

	FSMERRCODE ResumeCJ();

	/**
	 * Caller tries to start a CJ that has fulfilled the necessary condition to transit to SELECTED state. 
	 */
	FSMERRCODE SelectCJ();

	FSMERRCODE DeselectCJ();

	/**
	 * Caller retreive active PJ in order to gain access to its contents. 
	 */
	CPRJob * GetActivePJ();

	/**
	 * Caller retreive current active CJ to gain access to its content. 
	 */
	CCtrlJob * GetActiveCJ();

	/**
	 * Caller indicates the completion of active PJ.
	 * Returns:
	 * TRUE if no more PJ to execute, ie. CJCompleted.
	 * FALSE if more PJ is pending 
	 */
	BOOL PJCompleted();

	BOOL GetCJList(CStringArray &JList);

	BOOL GetPJList(CStringArray & JList);	
};

#endif // !defined(AFX_JOBSMGR_H__145CF80A_5CDC_4B5E_BB78_E78860B80E68__INCLUDED_)
