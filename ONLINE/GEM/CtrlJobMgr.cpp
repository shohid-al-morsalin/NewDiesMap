#include "stdafx.h"

#include "CtrlJob.h"
#include "GlobalDeviceContainer.h"
#include "IPC/SMCHdr.h"
#include "ONLINE/INC/E5-ErrCodes.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/DataBank.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"
#include "SYS/CUMMsg.h"

#include "CtrlJobMgr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtrlJobMgr CJobMgr;

CCtrlJobMgr::CCtrlJobMgr() {
	pPWnd = NULL;
	ActiveCJ = NULL;
	MAXCONTROLJOB = 30;
	bKeepCompletedCJob = FALSE;
}

CCtrlJobMgr::~CCtrlJobMgr() {
	ClearAllJobs();
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CmplJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CmplJobList.GetNext(pos);
		if (cJob) {
			delete cJob;
		}
	}
	m_CmplJobList.RemoveAll();
}

int CCtrlJobMgr::CanHOQ(char* pObjid) {
	CStringArray JList;
	char Buf[126] = { 0 };

	GetCJobList(JList);
	if (JList.GetSize() <= 1) {
		return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
	}
	if (JList.GetAt(0) == pObjid) {
		// Already at head of queue, do nothing
		return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
	}
	for (int i = 1; i < JList.GetSize(); i++) {
		if (JList.GetAt(i) == pObjid) {
			// Found valid CJob
			return NO_E5_ERROR;
		}
	}
	return UNKNOWN_OBJECT_INSTANCE;
}

void CCtrlJobMgr::Completed(char* pObjid) {
	CStringArray JList;

	CCtrlJob* cJob = GetCJob(pObjid);
	if (cJob) {
		cJob->RemoveAllPRJob();
		cJob->Completed();
		RemoveCJob(cJob);
	}

	return;
}

E5_ERRORS CCtrlJobMgr::CanStart(char* pObjid) {
	switch (pGDev->GetARAMSState()) {
	case SArams::NONSCHEDULED:
	case SArams::SCHEDULEDDOWN:
	case SArams::UNSCHEDULEDDOWN:
		return EQUIPMENT_OUT_OF_SERVICE;
		break;
	}
	CStringArray JList;
	GetActiveCJobList(JList);
	for (int i = 0; i < JList.GetSize(); i++) {
		if (JList.GetAt(i) != pObjid) {
			return BUSY;
		}
	}

	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}

	if (cJob->CanStart()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::CanCancel(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanCancel()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::CanResume(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanResume()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::CanPause(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanPause()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::CanAbort(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}

	if (cJob->CanAbort()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::CanStop(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}

	if (cJob->CanStop()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::CanSelect(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanSelect()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::CanDeselect(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanDeselect()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::CanDequeue(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);

	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanDequeue()) {
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::Select(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanSelect()) {
		cJob->Select();
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::Pause(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanPause()) {
		// Yuen: cJob will cycle through its PJ and stop all PJ from running [2002/10/19  10:52]
		cJob->Paused();
		// Yuen: C2C will inform status of pausing through other channel [2002/10/19  10:38]
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::Resume(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanResume()) {
		// Yuen: cJob will cycle through its PJ and resume all PJ [2002/10/19  10:52]
		cJob->Resume();
		// Yuen: C2C will inform status of resumeing through other channel [2002/10/19  10:38]
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::Cancel(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->Dequeue()) {
		cJob->RemoveAllPRJob();
		if (RemoveCJob(cJob)) return NO_E5_ERROR;
	}
	return UNKNOWN_E5_ERROR;
}

E5_ERRORS CCtrlJobMgr::Stop(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->Stop()) {
		cJob->RemoveAllPRJob();
		RemoveCJob(cJob);
	}
	else {
		// Stopping [9/14/2007 LYF]
		// CJ will be removed at later stage [9/14/2007 LYF]
		cJob->RemoveAllPRJob(); // simply remove all other pending PJ [9/14/2007 LYF]
	}
	if (pPWnd) {
		static CString CJObj = pObjid;
		pPWnd->PostMessage(UM_CJSTOP, 0, (LPARAM)CJObj.GetBuffer(0));
	}
	// C2C will signal completion of current process so that this can delete the CJob
	// return RemoveCJob(cJob);
	return NO_E5_ERROR;
}

E5_ERRORS CCtrlJobMgr::Abort(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->Abort()) {
		cJob->RemoveAllPRJob();
		RemoveCJob(cJob);
	}
	else {
		// Aborting [9/14/2007 LYF]
		// CJ will be removed at later stage [9/14/2007 LYF]
		cJob->RemoveAllPRJob(); // simply remove all other pending PJ [9/14/2007 LYF]
	}
	if (pPWnd) {
		static CString CJObj = pObjid;
		pPWnd->PostMessage(UM_CJABORT, 0, (LPARAM)CJObj.GetBuffer(0));
	}
	// C2C will signal completion of current process so that this can delete the CJob
	// return RemoveCJob(cJob);
	return NO_E5_ERROR;
}

E5_ERRORS CCtrlJobMgr::Deselect(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanDeselect()) {
		cJob->Deselect();	// Deselect will send the state back to queued
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::Dequeue(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanDequeue()) {
		if (cJob->Dequeue()) {
			cJob->RemoveAllPRJob();
			if (RemoveCJob(cJob)) return NO_E5_ERROR;
		}
		return UNKNOWN_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

E5_ERRORS CCtrlJobMgr::DequeueAll() {
	E5_ERRORS ret = NO_E5_ERROR;
	CStringArray JList;
	CString TempStr;
	GetCJobList(JList);
	for (int i = 0; i < JList.GetSize(); i++) {
		E5_ERRORS retc;
		TempStr = JList.GetAt(i);
		if ((retc = Dequeue(TempStr.GetBuffer(0))) != NO_E5_ERROR) {
			ret = retc;
		}
	}
	return ret;
}

BOOL CCtrlJobMgr::Create(CCtrlJob* cjob) {
	if (!cjob) {
		ASSERT(0);
		return FALSE;
	}
	CStringArray JList;
	GetCJobList(JList);
	for (int i = 0; i < JList.GetSize(); i++) {
		if (JList.GetAt(i) == cjob->m_ObjID) {
			return FALSE;
		}
	}
	m_CJobList.AddTail(cjob);
	cjob->Queued();

	return TRUE;
}

// Yuen: Test JLIst for CJob found [2002/11/07  8:16]
BOOL CCtrlJobMgr::GetCJobList(CStringArray& JList) {
	JList.RemoveAll();

	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			JList.Add(cJob->m_ObjID);
		}
	}
	if (JList.GetSize()) {
		return TRUE;
	}
	return FALSE;
}

CCtrlJob* CCtrlJobMgr::GetCJob(const CString& cjobname) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			if (!cjobname.Compare(cJob->m_ObjID)) {
				return cJob;
			}
		}
	}
	return NULL;
}

BOOL CCtrlJobMgr::GetActiveCJobList(CStringArray& JList) {
	JList.RemoveAll();
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			if (cJob->IsActive()) {
				JList.Add(cJob->m_ObjID);
			}
		}
	}
	if (JList.GetSize()) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJobMgr::ActivateNextCJob() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			if (cJob->GetState() == CCtrlJob::QUEUED) {
				CString CarrierID;
				if (IsCarrierPresent(cJob->CarrierInputSpec.GetAt(0))) {
					cJob->Select();
					if (cJob->StartMethod) {
						cJob->Executing(pPWnd);
					}
					else {
						cJob->WaitingForStart();
					}
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CCtrlJobMgr::AddToCompletedList(CCtrlJob* q) {
	if (!bKeepCompletedCJob) {
		delete q;
		return TRUE;
	}
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		POSITION opos = pos;
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob == q) {
			m_CJobList.RemoveAt(opos);
			m_CmplJobList.AddHead(cJob);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCtrlJobMgr::ProcessingCompleted(CCtrlJob* cJob) {
	CSharedMemQue pQue;

	// No more control job in control job list , send a event to carrier complete and
	// also , send a event  to Close the carrier to the C2C.
//     CEvt *pEvt = new CEvt;
// 	ASSERT(pEvt);
// 	pEvt->Set(EVT_CMS, CEID_LIST::PROCESSINGCOMPLETE);   // ********************************* check
// 	pQue.OpenMemory("EVTMSG");
// 	pQue.WriteQue(pEvt);
// 	pQue.CloseMemory();

	// Yuen: Only one CarrierInput spec per control job is supported [2002/10/20  22:06]
	CString CarrierID;
	for (int nIndex = 0; nIndex < cJob->CarrierInputSpec.GetSize(); nIndex++) {
		CarrierID = cJob->CarrierInputSpec.GetAt(nIndex);
		if (0 < CarrierID.GetLength()) {
			CSMOL SM;
			SM.Ca.act = CarrierAction::PROCESSINGCOMPLETE;
			strcpy(SM.Ca.CarrierID, CarrierID.GetBuffer(0));
			SM.cmd = CSMOL::cmdCARRIERACTION;
			CSMClt ipc;
			if (ipc.Start1("SecsSvr", sizeof(CSMOL))) {
				if (ipc.Send(&SM)) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

CCtrlJob* CCtrlJobMgr::GetHOQ() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (!m_CJobList.GetCount()) {
		return NULL;
	}
	CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetHead();
	if (cJob) {
		return cJob;
	}
	return NULL;
}

// Yuen: Remove offer no protection against state model corruption [2002/10/20  17:17]
BOOL CCtrlJobMgr::RemoveCJob(CCtrlJob* q) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		POSITION opos = pos;
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			if (cJob == q) {
				cJob->RemoveAllPRJob();
				cJob->StateChange(CCtrlJob::NO_STATE);
				m_CJobList.RemoveAt(opos);
				AddToCompletedList(cJob);
				return TRUE;
			}
		}
	}
	return FALSE;
}

CCtrlJob* CCtrlJobMgr::GetCJob(char* pObjid) {
	CString name;
	name = pObjid;
	return GetCJob(name);
}

BOOL CCtrlJobMgr::IsCarrierPresent(const CString& CarrierID) {
	return pGDev->IsCarrierPresent(CarrierID);
}

void CCtrlJobMgr::GetQueuedCJobList(vector <string>& JList) {
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			if (cJob->GetState() == CCtrlJob::QUEUED) {
				JList.push_back(cJob->m_ObjID);
			}
		}
	}
}

int CCtrlJobMgr::GetJobSpace() {
	return MAXCONTROLJOB - m_CJobList.GetCount();
}

void CCtrlJobMgr::ClearAllJobs() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			delete cJob;
		}
	}
	m_CJobList.RemoveAll();
}

CCtrlJob* CCtrlJobMgr::GetCmplCJob(char* pObjid) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CmplJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CmplJobList.GetNext(pos);
		if (cJob) {
			if (strcmp(cJob->m_ObjID, pObjid) == 0) {
				return cJob;
			};
		}
	}
	return NULL;
}

CPRJob* CCtrlJobMgr::GetActivePRJob() {
	CCtrlJob* cJob = GetActiveCJob();
	if (cJob) {
		return cJob->GetActivePRJob();
	}
	return NULL;
}

CCtrlJob* CCtrlJobMgr::GetActiveCJob() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			if (cJob->IsActive()) {
				return cJob;
			};
		}
	}
	return NULL;
}

BOOL CCtrlJobMgr::Stopped(char* prjob) {
	CCtrlJob* c = GetActiveCJob();
	if (c) {
		if (c->Stopped()) {
			ProcessingCompleted(c);
			c->RemoveAllPRJob();
			RemoveCJob(c);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCtrlJobMgr::ActivateNextPRJob() {
	CCtrlJob* cJob = GetActiveCJob();
	if (cJob) {
		if (!cJob->SelectPRJob()) {
			Completed(cJob->m_ObjID);
			return FALSE;
		};
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJobMgr::ActivateNextPRJob(CString CJobName) {
	CCtrlJob* pC = GetCJob(CJobName);
	if (pC && pC->IsActive()) {
		CPRJob* pPJ = pC->GetActivePRJob();
		if (pPJ) {
			PJobMgr.SetCompleted(pPJ);
		}
		if (!pC->SelectPRJob()) {
			Completed(pC->m_ObjID);	// CJCOMPLETE
			return FALSE;
		}
		return TRUE;
	}
	// protect against accidental call of this function

	return FALSE;
}

BOOL CCtrlJobMgr::Aborted(char* prjob) {
	CCtrlJob* c = GetActiveCJob();
	if (c) {
		if (c->Aborted()) {
			//ProcessingCompleted(c);
			//c->RemoveAllPRJob();
			RemoveCJob(c);
			return TRUE;
		}
	}
	return FALSE;
}

E5_ERRORS CCtrlJobMgr::Start(char* pObjid) {
	CCtrlJob* cJob = GetCJob(pObjid);
	if (!cJob) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	if (cJob->CanStart()) {
		cJob->Executing(pPWnd);
		return NO_E5_ERROR;
	}
	return COMMAND_NOT_VALID_FOR_CURRENT_STATE;
}

BOOL CCtrlJobMgr::CJobIdsFromCarrier(const char* CarrierID, vector<string>& CjobIds) {
	BOOL bRes = FALSE;
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			for (int i = 0; i<int(cJob->CarrierInputSpec.GetSize()); i++) {
				CString& SRef = cJob->CarrierInputSpec[i];
				if (SRef == CarrierID) {
					CjobIds.push_back(cJob->m_ObjID);
				}
			}
		}
	}
	if (CjobIds.size()) {
		bRes = TRUE;
	}
	return bRes;
}

BOOL CCtrlJobMgr::CleanupCancelCarrier(const char* CarrierID) {
	BOOL bRes = TRUE;
	vector<string> CJobList;
	if (CJobMgr.CJobIdsFromCarrier(CarrierID, CJobList)) {
		//CCtrlJob *pCJob = NULL;
		for (UINT i = 0; i < CJobList.size(); i++) {
			CCtrlJob* pCJob = GetCJob((char*)CJobList[i].c_str());
			if (pCJob) {
				if (pCJob->IsExecuting()) {
					// Should not come here, indicated failure to block command by function before this call
					ASSERT(0);
				}
				else if (pCJob->CanCancel()) {
					Cancel((char*)CJobList[i].c_str());
				}
				else if (pCJob->CanStop()) {
					Stop((char*)CJobList[i].c_str());
				}
				else if (pCJob->CanAbort()) {
					Abort((char*)CJobList[i].c_str());
				}
				else {
					// Should not come here
					ASSERT(0);
				}
			}
		}
	}
	return bRes;
}

BOOL CCtrlJobMgr::IsCarrierInProcess(const char* CarrierID) {
	BOOL bRes = FALSE;
	vector<string> CJobList;
	CCtrlJob* pCJob = NULL;
	if (CJobIdsFromCarrier(CarrierID, CJobList)) {
		//bRes = TRUE;
		for (UINT i = 0; i < CJobList.size(); i++) {
			pCJob = GetCJob((char*)CJobList[i].c_str());
			if (pCJob) {
				if (pCJob->IsExecuting()) {
					bRes = TRUE;
					break;
				}
			}
		}
	}
	return bRes;
}

/*
 *	To enforce the rule that one process jobs belongs to atmost one Control Job
 */
UINT CCtrlJobMgr::ProcessJobReferanceCount(const char* PrJobID) {
	UINT RefCnt = 0;
	CStringArray CjList, PjList;
	GetCJobList(CjList);
	CCtrlJob* pCJob = NULL;
	for (int nIdx = 0; nIdx<int(CjList.GetSize()); nIdx++) {
		pCJob = GetCJob(CjList[nIdx]);
		if (pCJob) {
			PjList.Append(pCJob->ProcessingCtrlSpec);
			for (int nPrjs = 0; nPrjs<int(PjList.GetSize()); PjList) {
				if (PjList[nPrjs] == PrJobID) {
					RefCnt++;
				}
			}
			PjList.RemoveAll();
		}
	}
	return RefCnt;
}

void CCtrlJobMgr::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CCtrlJobMgr");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CCtrlJobMgr::CtrlJobIDFromPj(const char* pPrjobID, CString& CjID) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			CStringArray& PrRef = cJob->ProcessingCtrlSpec;
			for (int nIdx = 0; nIdx<int(PrRef.GetSize()); nIdx++) {
				if (PrRef[nIdx] == pPrjobID) {
					CjID = cJob->m_ObjID;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void CCtrlJobMgr::Abort(short unit) {
	CCtrlJob* cJ = GetActiveCJob();
	if (cJ) {
		CPRJob* pJ = cJ->GetActivePRJob();
		if (pJ) {
			if (pJ->m_PortNo == unit) {
				Abort(cJ->m_ObjID);
			}
		}
	}
}

void CCtrlJobMgr::Stop(short unit) {
	CCtrlJob* cJ = GetActiveCJob();
	if (cJ) {
		CPRJob* pJ = cJ->GetActivePRJob();
		if (pJ) {
			if (pJ->m_PortNo == unit) {
				Stop(cJ->m_ObjID);
			}
		}
	}
}

BOOL CCtrlJobMgr::StartCJob(CString CJobName, CWnd* pWnd) {
	pPWnd = pWnd;

	ErrorMsg = "";
	CCtrlJob* cjob = GetCJob(CJobName);
	if (!cjob) {
		// Control Job not found [2/9/2012 Administrator]
		ErrorMsg = "CJ not found";
		return FALSE;
	}
	if (cjob->IsQueued()) {
		if (IsCarrierPresent(cjob->CarrierInputSpec.GetAt(0))) {
			if (cjob->CanPRJobStart()) {
				cjob->Select();
				if (cjob->IsAutoStart()) {
					if (!cjob->Executing(pPWnd)) {
						ErrorMsg = "Cannot execute CJob: " + cjob->ErrorMsg;
						return FALSE;
					}
				}
				else {
					if (!cjob->WaitingForStart()) {
						ErrorMsg = "Cannot execute WaitingForStart";
						return FALSE;
					}
				}
			}
			else {
				ErrorMsg = "PJ cannot start: " + cjob->ErrorMsg;
				return FALSE;
			}
		}
		else {
			ErrorMsg = "Carrier not present";
			return FALSE;
		}
	}
	else {
		ErrorMsg = "CJ not queued";
		return FALSE;
	}
	return TRUE;
}

// void CCtrlJobMgr::StartCJob()
// {
// 	CCtrlJob *cjob = GetCJobHOQ();
//
// 	if (!cjob)
// 	{
// 		return ;
// 	}
//
// 	if (cjob->IsQueued())
// 	{
// 		if (IsCarrierPresent(cjob->CarrierInputSpec.GetAt(0)))
// 		{
// 			if (cjob->CanPRJobStart())
// 			{
// 				cjob->Select();
// 				if (cjob->IsAutoStart())
// 				{
// 					cjob->Executing(pPWnd);
// 				}
// 				else
// 				{
// 					cjob->WaitingForStart();
// 				}
// 			}
// 		}
// 	}
// }

CCtrlJob* CCtrlJobMgr::GetCJobHOQ() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	if (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if (cJob) {
			return cJob;
		}
	}
	return NULL;
}

BOOL CCtrlJobMgr::GetCJPJIDFromWaferID(CString& WaferID, CString& CJID, CString& PJID) {
	CString CarrierID;
	if (!pGDev->GetCarrierID(WaferID.GetBuffer(0), CarrierID)) {
		return FALSE;
	};
	CCtrlJob* pCJob = GetActiveCJob();
	if (pCJob && (0 == pCJob->CarrierInputSpec[0].CompareNoCase(CarrierID))) {
		CJID = pCJob->m_ObjID;
		CPRJob* pPJ = GetActivePRJob();
		if (pPJ) {
			PJID = pPJ->m_ObjID;
		}
		return TRUE;
	}
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_CJobList.GetHeadPosition();
	while (pos) {
		CCtrlJob* cJob = (CCtrlJob*)m_CJobList.GetNext(pos);
		if ((cJob) && (cJob->CarrierInputSpec[0].CompareNoCase(CarrierID))) {
			CJID = cJob->m_ObjID;
			CPRJob* pPJ = GetActivePRJob();
			if (pPJ) {
				PJID = pPJ->m_ObjID;
			}
			return TRUE;
		}
	}
	return FALSE;
}

void CCtrlJobMgr::Register(CWnd* pWnd) {
	pPWnd = pWnd;
}