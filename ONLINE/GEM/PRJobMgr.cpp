// PRJobMgr.cpp: implementation of the CPRJobMgr class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "PRJobMgr.h"
#include "PRJob.h"
//#include "..\..\IPC\SMOL.h"
#include "..\ShareMemory\DataBank.h"

#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

CPRJobMgr PJobMgr;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPRJobMgr::CPRJobMgr() {
	MAXPRJOB = 30;
	ActivePJ = NULL;
}

CPRJobMgr::~CPRJobMgr() {
	ClearAllJobs();
}

// Entry Point
// Yuen: Only one PRJob per load port can be active at one time [2002/10/19  13:30]
CPRJobMgr::RCODE CPRJobMgr::SetActive(char* pPRJob) {
	if (!(pGDev->GetARAMSState() == SArams::STANDBY)) {
		return EQUIPMENTNOTIDLE;
	}

	CPRJob* p = GetPRJob(pPRJob);
	if (!p) {
		return PRJOBNOTFOUND;
	}

	if (p->SettingUp()) {
		if (p->IsAutostart()) {
			if (p->Processing()) {
				return OK;
			}
			return SETPROCESSINGFAIL;
		}
		else {
			if (p->WaitingStart()) {
				return OK;
			}
			return SETWAITINGFORSTARTFAIL;
		}
	}
	return SETUPFAIL;
}

// Caller of this function will decide whether there is pending PRJob to start
// Caller will call CJComplete when there is no more pending PJ
CPRJobMgr::RCODE CPRJobMgr::SetCompleted(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (!p) {
		return PRJOBNOTFOUND;
	}
	if (p->ProcessingComplete()) {
		RemovePRJob(p);
		return OK;
	}

	return SETPROCESSINGCOMPLETEFAIL;
}

CPRJobMgr::RCODE CPRJobMgr::SetCompleted(CPRJob* pPJ) {
	if (pPJ->ProcessingComplete()) {
		RemovePRJob(pPJ);
		return OK;
	}

	return SETPROCESSINGCOMPLETEFAIL;
}

CPRJobMgr::RCODE CPRJobMgr::DequeueAll() {
	CStringArray JList;

	GetPRJobList(JList);

	for (int i = 0; i < JList.GetSize(); i++) {
		CString str = JList.GetAt(i)/*.GetBuffer((int)0)*/;
		if (Dequeue(str.GetBuffer(0)) != OK) {
			return DEQUEUEOFSOMEPRJOBFAIL;
		}
	}
	return OK;
}

CPRJobMgr::RCODE CPRJobMgr::Create(CPRJob* pPRJob) {
	LastErr = "";
	if (NULL == pPRJob) {
		LastErr = "NULL pointer";
		ASSERT(0);	// Yuen: OK to assert [2002/11/07  7:54]
		return POINTERERROR;
	}
	m_PRJobList.AddTail(pPRJob);
	if (pPRJob->Queued()) {
		if (!pGDev->pDataBnk->bCJJobControl) {
			if (pPRJob->IsAutostart()) {
				if (!pPRJob->Start()) {
					LastErr = "Cannot start";
					return STARTFAIL;
				}
			}
		}
		return OK;
	}
	LastErr = "Cannot queue";
	return QUEUEDFAIL;
}

BOOL CPRJobMgr::IsPRJobExist(const CString& pPRJob) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_PRJobList.GetHeadPosition();
	while (pos) {
		CPRJob* p = (CPRJob*)m_PRJobList.GetNext(pos);
		if (!p) {
			ASSERT(0);	// Yuen: OK to assert [2002/11/07  7:55]
		}
		else if (pPRJob == p->m_ObjID) {
			return TRUE;
		}
	}
	return FALSE;
}

CPRJobMgr::RCODE CPRJobMgr::GetActiveJobList(CStringArray& JList) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_PRJobList.GetHeadPosition();
	while (pos) {
		CPRJob* p = (CPRJob*)m_PRJobList.GetNext(pos);
		if (!p) {
			ASSERT(0);	// Yuen: OK to assert [2002/11/07  7:55]
			return POINTERERROR;
		}
		if (p->IsActive()) {
			JList.Add(p->m_ObjID);
		}
	}
	if (JList.GetSize()) {
		return OK;
	}
	return NOACTIVEPRJOB;
}

// Yuen: Test JList for presence of PRJob in list, do not rely on RCODE [2002/11/07  7:56]
CPRJobMgr::RCODE CPRJobMgr::GetPRJobList(CStringArray& JList) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	JList.RemoveAll();
	POSITION pos = m_PRJobList.GetHeadPosition();
	while (pos) {
		CPRJob* p = (CPRJob*)m_PRJobList.GetNext(pos);
		if (p) {
			JList.Add(p->m_ObjID);
		}
	}
	return OK;
}

// Yuen: Remove offer no protection against state model corruption [2002/10/20  17:17]
CPRJobMgr::RCODE CPRJobMgr::RemovePRJob(CPRJob* pPRJob) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_PRJobList.GetHeadPosition();
	while (pos) {
		POSITION opos = pos;
		CPRJob* p = (CPRJob*)m_PRJobList.GetNext(pos);
		if (p == pPRJob) {
			if (p->GetState() != CPRJob::NO_STATE) {
				p->StateChange(CPRJob::NO_STATE);
			}
			p->AlertNotify(CPRJob::alJOB_COMPLETE);
			m_PRJobList.RemoveAt(opos);
			delete p;
			return OK;
		}
	}
	return PRJOBNOTFOUND;
}

CPRJob* CPRJobMgr::GetPRJob(const char* pPRJob) {
	CString job;
	job = pPRJob;
	return GetPRJob(job);
}

CPRJob* CPRJobMgr::GetPRJob(const CString& PRJobName) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_PRJobList.GetHeadPosition();
	while (pos) {
		CPRJob* p = (CPRJob*)m_PRJobList.GetNext(pos);
		if (!p) {
			ASSERT(0);	// Yuen: OK to assert [2002/11/07  7:58]
			return NULL;
		}
		if (!PRJobName.Compare(p->m_ObjID)) {
			return p;
		}
	}
	return NULL;
}

int CPRJobMgr::GetJobSpace() {
	int count = m_PRJobList.GetCount();
	return (MAXPRJOB - count);
}

CPRJobMgr::RCODE CPRJobMgr::SetStartMethod(char* pPRJob, BOOL bStart) {
	CPRJob* p = GetPRJob(pPRJob);
	if (!p) {
		return PRJOBNOTFOUND;
	}
	p->SetPRStart(bStart);
	return OK;
}

CPRJobMgr::RCODE CPRJobMgr::ClearAllJobs() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_PRJobList.GetHeadPosition();
	while (pos) {
		CPRJob* p = (CPRJob*)m_PRJobList.GetNext(pos);
		if (!p) {
			ASSERT(0);	// Yuen: OK to assert [2002/11/07  7:58]
			return POINTERERROR;
		}
		delete p;
	}
	m_PRJobList.RemoveAll();
	return OK;
}

BOOL CPRJobMgr::IsPRJobExist(const char* pPRJob) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	POSITION pos = m_PRJobList.GetHeadPosition();
	while (pos) {
		CPRJob* p = (CPRJob*)m_PRJobList.GetNext(pos);
		if (!p) {
			ASSERT(0);	// Yuen: OK to assert [2002/11/07  8:00]
			return FALSE;
		}
		{
			if (strcmp(p->m_ObjID, pPRJob) == 0) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

short CPRJobMgr::GetPRJobCount() {
	return m_PRJobList.GetCount();
}

CPRJobMgr::RCODE CPRJobMgr::CanStart(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->CanStart()) {
			return OK;
		}
	}
	return CANNOTPERFORMACTION;
}

CPRJobMgr::RCODE CPRJobMgr::CanStop(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->CanStop()) {
			return OK;
		}
	}
	return CANNOTPERFORMACTION;
}

CPRJobMgr::RCODE CPRJobMgr::CanPause(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->CanPause()) {
			return OK;
		}
	}
	return CANNOTPERFORMACTION;
}

CPRJobMgr::RCODE CPRJobMgr::CanResume(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->CanResume()) {
			return OK;
		}
	}
	return CANNOTPERFORMACTION;
}

CPRJobMgr::RCODE CPRJobMgr::CanAbort(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->CanAbort()) {
			return OK;
		}
	}
	return CANNOTPERFORMACTION;
}

CPRJobMgr::RCODE CPRJobMgr::CanCancel(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->CanCancel()) {
			return OK;
		}
	}
	return CANNOTPERFORMACTION;
}

CPRJobMgr::RCODE CPRJobMgr::Dequeue(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->Dequeue()) {
			RemovePRJob(p);
			return OK;
		}
	}
	return DEQUEUEFAIL;
}

CPRJobMgr::RCODE CPRJobMgr::Abort(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		BOOL bIsQueued = p->IsQueued();
		if (p->Abort()) {
			if (bIsQueued) {
				RemovePRJob(p);
			}
			else {
				// Removable of allocated memory will happen after the PRJob has aborted
			}
			return OK;
		}
	}
	return ABORTFAIL;
}

CPRJobMgr::RCODE CPRJobMgr::Stop(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->Stop()) {
			// Removable of allocated memory will happen after the PRJob has aborted
			return OK;
		}
	}
	return STOPFAIL;
}

CPRJobMgr::RCODE CPRJobMgr::Start(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->Start()) {
			return OK;
		}
	}

	//return STOPFAIL;
	return STARTFAIL;
}

CPRJobMgr::RCODE CPRJobMgr::Pause(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->Pausing()) {
			return OK;
		}
	}
	return STOPFAIL;
}

CPRJobMgr::RCODE CPRJobMgr::Resume(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->Resume()) {
			return OK;
		}
	}
	return STOPFAIL;
}

CPRJobMgr::RCODE CPRJobMgr::Cancel(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		if (p->Cancel()) {
			return RemovePRJob(p);
		}
	}
	return CANCELFAIL;
}

BOOL CPRJobMgr::Stopped(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		p->Stopped();
		RemovePRJob(p);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJobMgr::Aborted(char* pPRJob) {
	CPRJob* p = GetPRJob(pPRJob);
	if (p) {
		p->Aborted();
		RemovePRJob(p);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJobMgr::GetMatlData(/*In*/const char* PrJobID,/*Out*/CString& CarrierID,/*Out*/CString& SlotMap) {
	BOOL bOk = FALSE;
	CPRJob* pJob = GetPRJob((char*)PrJobID);
	if (pJob) {
		if (!pJob->GetCarrierAndSM(CarrierID, SlotMap)) {
			bOk = FALSE;
		}
	}
	return bOk;
}

CPRJob* CPRJobMgr::GetHOQ() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (m_PRJobList.GetCount()) {
		CPRJob* p = (CPRJob*)m_PRJobList.GetHead();
		if (p) {
			return p;
		}
	}
	return NULL;
}

CPRJob* CPRJobMgr::GetEOQ() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	if (m_PRJobList.GetCount()) {
		CPRJob* p = (CPRJob*)m_PRJobList.GetTail();
		if (p) {
			return p;
		}
	}
	return NULL;
}

BOOL CPRJobMgr::WFS2Start(char* pPrjID) {
	CPRJob* p = GetPRJob(pPrjID);
	if (p) {
		if (p->WFS2Start()) {
			return OK;
		}
	}
	return STARTFAIL;
}