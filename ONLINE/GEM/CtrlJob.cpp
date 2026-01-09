#include "stdafx.h"
#include "strSOp.h"

#include "128/GEM/ObjectsContainer.h"
#include "GemFactory.h"
#include "GlobalDeviceContainer.h"
#include "ONLINE/INC/E5-ErrCodes.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/DataBank.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"
#include "PRJOB.H"
#include "PRJobMgr.h"
#include "SecsItem.h"
#include "SYS/CUMMsg.h"
#include "SYS/Var.h"
#include "CtrlJob.h"

#include <map>
#include <string>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCtrlJob::CCtrlJob() {
	pPWnd = NULL;
	ClearAll();
	StateEx = NORMAL;
	SetState(NO_STATE);
}

CCtrlJob::~CCtrlJob() {}

CCtrlJob& CCtrlJob::operator = (CCtrlJob& Rhs) {
	ProcessOrderMgmt = Rhs.ProcessOrderMgmt;
	ProcessingCtrlSpec.Copy(Rhs.ProcessingCtrlSpec);
	CarrierInputSpec.Copy(Rhs.CarrierInputSpec);
	StartMethod = Rhs.StartMethod;
	State = Rhs.State;
	MatlOrderMgmt = Rhs.MatlOrderMgmt;
	PauseEvents = Rhs.PauseEvents;
	memcpy(m_ObjType, Rhs.m_ObjType, sizeof(m_ObjType));
	memcpy(m_ObjID, Rhs.m_ObjID, sizeof(m_ObjID));
	return *this;
}

void CCtrlJob::ClearAll() // There is no "ProcessJobs" 130831.
{
	ZeroMemory(m_ObjType, sizeof(m_ObjType));
	ZeroMemory(m_ObjID, sizeof(m_ObjID));
	strscpy(m_ObjType, sizeof(m_ObjType), "ControlJob");
	State = (STATE)0;
	ProcessingCtrlSpec.RemoveAll();
	CarrierInputSpec.RemoveAll();
	m_AttrIDName.RemoveAll();
	m_AttrIDName.Add("ObjType");
	m_AttrIDName.Add("ObjID");
	m_AttrIDName.Add("CurrentPRJob");
	m_AttrIDName.Add("CarrierInputSpec");
	m_AttrIDName.Add("StartMethod");
	m_AttrIDName.Add("State");
	m_AttrIDName.Add("ProcessJobs"); //20130831 SCOTT ADD
}

void CCtrlJob::SetObjectID(char* objid) {
	strscpy(m_ObjID, sizeof(m_ObjID), objid);
}

void CCtrlJob::SetAddPRjob(char* prjob) {
	ProcessingCtrlSpec.Add(prjob);
}

void CCtrlJob::SetCarrierInput(char* mid) {
	CarrierInputSpec.Add(mid);
}

void CCtrlJob::SetStartMethod(BOOL method) {
	StartMethod = method;
}

CCtrlJob::STATE CCtrlJob::GetState() {
	return State;
}

void CCtrlJob::SetState(STATE state) {
	State = state;
}

void CCtrlJob::StateChange(CCtrlJob::STATE NewState) {
	UINT CEID = 0;
	string Start = "", Finish = "";
	CCtrlJob::STATE OldState = GetState();
	switch (OldState) {
	case CCtrlJob::NO_STATE:
		Start = "NO STATE";
		switch (NewState) {
		case CCtrlJob::QUEUED:
			CEID = CEID_LIST::CJSM01NOSTATETOQUEUED;
			Finish = "QUEUED";
			break;
		case CCtrlJob::NO_STATE:
			CEID = 9999;
			break;
		}
		break;
	case CCtrlJob::QUEUED:
		Start = "QUEUED";
		switch (NewState) {
		case CCtrlJob::NO_STATE:
			Finish = "NO STATE";
			CEID = CEID_LIST::CJSM02QUEUEDTONOSTATE;
			break;
		case CCtrlJob::SELECTED:
			Finish = "SELECTED";
			CEID = CEID_LIST::CJSM03QUEUEDTOSELECTED;
			break;
		}
		break;
	case CCtrlJob::SELECTED:
		Start = "SELECTED";
		switch (NewState) {
		case CCtrlJob::QUEUED:
			Finish = "QUEUED";
			CEID = CEID_LIST::CJSM04SELECTEDTOQUEUED;
			break;
		case CCtrlJob::EXECUTING:
			Finish = "EXECUTING";
			CEID = CEID_LIST::CJSM05SELECTEDTOEXECUTING;
			break;
		case CCtrlJob::WATINGFORSTART:
			Finish = "WAITING FOR START";
			CEID = CEID_LIST::CJSM06SELECTEDTOWAITINGFORSTART;
			break;
		case CCtrlJob::ABORTED:
			CEID = CEID_LIST::CJSM12ACTIVETOCOMPLETEDABORT;
			Finish = "ABORTED";
			break;
		case CCtrlJob::STOPPED:
			CEID = CEID_LIST::CJSM11ACTIVETOCOMPLETEDSTOP;
			Finish = "STOPPED";
			break;
		}
		break;
	case CCtrlJob::WATINGFORSTART:
		Start = "WAITING FOR START";
		switch (NewState) {
		case CCtrlJob::EXECUTING:
			Finish = "EXECUTING";
			CEID = CEID_LIST::CJSM07WAITINGFORSTARTTOEXECUTING;
			break;
		case CCtrlJob::ABORTED:
			CEID = CEID_LIST::CJSM12ACTIVETOCOMPLETEDABORT;
			Finish = "ABORTED";
			break;
		case CCtrlJob::STOPPED:
			CEID = CEID_LIST::CJSM11ACTIVETOCOMPLETEDSTOP;
			Finish = "STOPPED";
			break;
		case CCtrlJob::CJACTIVE:
			CEID = 9999;
			Finish = "ACTIVE";
			break;
		}
		break;
	case CCtrlJob::EXECUTING:
		Start = "EXECUTING";
		switch (NewState) {
		case CCtrlJob::PAUSED:
			Finish = "PAUSED";
			CEID = CEID_LIST::CJSM08EXECUTINGTOPAUSED;
			break;
		case CCtrlJob::COMPLETED:
			Finish = "COMPLETED";
			CEID = CEID_LIST::CJSM10EXECUTINGTOCOMPLETED;
			break;
		case CCtrlJob::ABORTED:
			CEID = CEID_LIST::CJSM12ACTIVETOCOMPLETEDABORT;
			Finish = "ABORTED";
			break;
		case CCtrlJob::STOPPED:
			CEID = CEID_LIST::CJSM11ACTIVETOCOMPLETEDSTOP;
			Finish = "STOPPED";
			break;
		}
		break;
	case CCtrlJob::PAUSED:
		Start = "PAUSED";
		switch (NewState) {
		case CCtrlJob::EXECUTING:
			CEID = CEID_LIST::CJSM09PAUSEDTOEXECUTING;
			Finish = "EXECUTING";
			break;
		case CCtrlJob::ABORTED:
			CEID = CEID_LIST::CJSM12ACTIVETOCOMPLETEDABORT;
			Finish = "ABORTED";
			break;
		case CCtrlJob::STOPPED:
			CEID = CEID_LIST::CJSM11ACTIVETOCOMPLETEDSTOP;
			Finish = "STOPPED";
			break;
		}
		break;
	case CCtrlJob::CJACTIVE:
		Start = "ACTIVE";
		switch (NewState) {
		case CCtrlJob::ABORTED:
			CEID = CEID_LIST::CJSM12ACTIVETOCOMPLETEDABORT;
			Finish = "ABORTED";
			break;
		case CCtrlJob::STOPPED:
			CEID = CEID_LIST::CJSM11ACTIVETOCOMPLETEDSTOP;
			Finish = "STOPPED";
			break;
		case CCtrlJob::COMPLETED:
			CEID = CEID_LIST::CJSM10EXECUTINGTOCOMPLETED;
			Finish = "COMPLETED";
			break;
		}
		break;
		// Yuen: STOP,ABORT is concurent state of COMPLETED [2002/10/20  16:34]
	case CCtrlJob::COMPLETED:
	case CCtrlJob::STOPPED:
	case CCtrlJob::ABORTED:
		Start = "COMPLETED";
		switch (NewState) {
		case CCtrlJob::NO_STATE:
			CEID = CEID_LIST::CJSM13COMPLETEDTONOSTATE;
			Finish = "NO STATE";
			break;
		}
		break;
		break;
	}
	if (!CEID) {
		//ASSERT(0);
		// No change, simple exit [3/1/2012 C2C]
		return;
	}
	if (NewState != CCtrlJob::NO_STATE) {
		SetState(NewState);
	}

	if (CEID != 9999) {
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		strscpy(pEvt->ObjID, OBJIDLEN, m_ObjID);
		if (ProcessingCtrlSpec.GetSize()) {
			strscpy(pEvt->PRJobID, PRJIDLEN, ProcessingCtrlSpec.GetAt(0));
		}
		else {
			pEvt->PRJobID[0] = 0;
		}
		CSharedMemQue pQue;
		pEvt->Set(EVT_CMS, CEID); // CEID = index into CEID_LIST [8/5/2010 XPMUser]
		pQue.OpenMemory("EVTMSG");
		pGDev->SetActiveCJPJ(m_ObjID, pEvt->PRJobID);
		pQue.WriteQue(pEvt);
		pGDev->SetActiveCJPJ("", "");
		pQue.CloseMemory();

		string Msg;
		Msg = m_ObjID;
		Msg += ": " + Start + " -> " + Finish;

		CString msg;
		msg = Msg.c_str();
		pGDev->LogFile.Log(msg, 5, "CCtrlJob");
	}
	if (NewState == CCtrlJob::NO_STATE) {
		SetState(NewState);
	}
}

void CCtrlJob::Queued() {
	StateChange(CCtrlJob::QUEUED);
}

void CCtrlJob::Select() {
	StateChange(CCtrlJob::SELECTED);
}

BOOL CCtrlJob::WaitingForStart() {
	if (StartMethod) {
		// Auto start [2/9/2012 Administrator]
		// Yuen: Should not reach here if calling function has tested the state [2002/10/18  22:54]
		ASSERT(0);
		return FALSE;
	}
	StateChange(CCtrlJob::WATINGFORSTART);
	return TRUE;
}

BOOL CCtrlJob::Executing(CWnd* pWnd) {
	pPWnd = pWnd;
	StateChange(CCtrlJob::EXECUTING);
	if (SelectPRJob()) {
		return TRUE;
	}
	return FALSE;
}

void CCtrlJob::Paused() {
	StateChange(CCtrlJob::PAUSED);
}

void CCtrlJob::Completed() {
	switch (StateEx) {
	case NORMAL:
		StateChange(CCtrlJob::COMPLETED);
		break;
	case STOPPING:
		Stopped();
		break;
	case ABORTING:
		Aborted();
		break;
	}
	StateEx = NORMAL;
}

BOOL CCtrlJob::IsQueued() {
	if (GetState() == CCtrlJob::QUEUED) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJob::IsPaused() {
	if (GetState() == CCtrlJob::PAUSED) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJob::IsExecuting() {
	if (GetState() == CCtrlJob::EXECUTING) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJob::IsWaitingForStart() {
	if (GetState() == CCtrlJob::WATINGFORSTART) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJob::IsActive() {
	switch (GetState()) {
	case CCtrlJob::SELECTED:
	case CCtrlJob::WATINGFORSTART:
	case CCtrlJob::EXECUTING:
	case CCtrlJob::PAUSED:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CCtrlJob::Dump(string& Data) {
	vector < string > IdList;
	string Temp;
	BOOL bRes = TRUE;
	GetAttrIDs(IdList);
	for (int nIndex = 0; nIndex < int(IdList.size()); nIndex++) {
		if (!GetAttr(IdList[nIndex], Temp)) {
			bRes = FALSE;
		}
		Data += "<< ";
		Data += IdList[nIndex];
		Data += " >>";
		Data += "\r\n\t";
		Data += Temp;
		Data += "\r\n";
		Temp.erase();
	}
	return bRes;
}

void CCtrlJob::GetAttrIDs(vector < string >& IdList) {
	IdList.clear();
	IdList.push_back("ObjType");
	IdList.push_back("ObjID");
	IdList.push_back("CurrentPRJob");
	IdList.push_back("CarrierInputSpec");
	IdList.push_back("StartMethod");
	IdList.push_back("State");
	IdList.push_back("ProcessOrderMgmt");
	IdList.push_back("ProcessJobs");
}

BOOL CCtrlJob::GetAttr(const string& nSer, string& Data) {
	string Temp;
	if (nSer == "ObjType") {
		Data = m_ObjType;
		return TRUE;
	}
	else if (nSer == "ObjID") {
		Data = m_ObjID;
		return TRUE;
	}
	else if (nSer == "CurrentPRJob") {
		ASSERT(0);
		return TRUE;
	}
	else if (nSer == "ProcessJobs") {
		for (int nIndex = 0; nIndex < ProcessingCtrlSpec.GetSize(); nIndex++) {
			Data += '\t';
			Data += ProcessingCtrlSpec[nIndex].GetBuffer(0);
			Data += "\r\n";
		}
		return TRUE;
	}
	else if (nSer == "CarrierInputSpec") {
		for (int nIndex = 0; nIndex < CarrierInputSpec.GetSize(); nIndex++) {
			Data += '\t';
			Data += CarrierInputSpec.GetAt(nIndex);
			Data += "\r\n";
		}
		return TRUE;
	}
	else if (nSer == "StartMethod") {
		Data = (StartMethod == TRUE) ? "Auto" : "Manual";
		return TRUE;
	}
	else if ((nSer == "State") || (nSer == "CJobState")) {
		switch (GetState()) {
		case CCtrlJob::QUEUED:
			Data = "QUEUED";
			break;
		case CCtrlJob::SELECTED:
			Data = "SELECTED";
			break;
		case CCtrlJob::WATINGFORSTART:
			Data = "WATINGFORSTART";
			break;
		case CCtrlJob::EXECUTING:
			Data = "EXECUTING";
			break;
		case CCtrlJob::PAUSED:
			Data = "PAUSED";
			break;
		case CCtrlJob::COMPLETED:
			Data = "COMPLETED";
			break;
		default:
			Data = "";
			break;
		}
		return TRUE;
	}
	else if (nSer == "ProcessOrderMgmt") {
		switch (ProcessOrderMgmt) {
		case 0:
			Data = "LIST";
			break;
		case 1:
			Data = "ARRIVAL";
			break;
		case 2:
			Data = "OPTIMIZE";
			break;
		}
		return TRUE;
	}
	else {
		return FALSE;
	}
}

CString CCtrlJob::GetStartMethodStr() {
	if (StartMethod) {
		return "Auto Start";
	}
	return "Manual Start";
}

CString CCtrlJob::GetStateStr() {
	return GetStateStr(GetState());
	// 	{
	// 		case QUEUED:
	// 			return "QUEUED";
	// 			break;
	// 		case SELECTED:
	// 			return "SELECTED";
	// 			break;
	// 		case WATINGFORSTART:
	// 			return "WAITING FOR START";
	// 			break;
	// 		case EXECUTING:
	// 			return "EXECUTING";
	// 			break;
	// 		case PAUSED:
	// 			return "PAUSED";
	// 			break;
	// 		case COMPLETED:
	// 			return "COMPLETED";
	// 			break;
	// 		case CJACTIVE:
	// 			return "ACTIVE";
	// 			break;
	// 		case ABORTED:
	// 			return "ABORTED";
	// 			break;
	// 		case STOPPED:
	// 			return "STOPPED";
	// 			break;
	// 		case NO_STATE:
	// 			return "NO STATE";
	// 			break;
	// 		default:
	// 			ASSERT(0);
	// 			break;
	// 	}
	// 	return "";
}

CString CCtrlJob::GetStateStr(CCtrlJob::STATE state) {
	switch (state) {
	case QUEUED:
		return "QUEUED";
		break;
	case SELECTED:
		return "SELECTED";
		break;
	case WATINGFORSTART:
		return "WAITING FOR START";
		break;
	case EXECUTING:
		return "EXECUTING";
		break;
	case PAUSED:
		return "PAUSED";
		break;
	case COMPLETED:
		return "COMPLETED";
		break;
	case CJACTIVE:
		return "ACTIVE";
		break;
	case ABORTED:
		return "ABORTED";
		break;
	case STOPPED:
		return "STOPPED";
		break;
	case NO_STATE:
		return "NO STATE";
		break;
	default:
		ASSERT(0);
		break;
	}
	return "";
}

BOOL CCtrlJob::SetAttr(CString& service, char* val) {
	if (service == "ObjID") {
		strscpy(m_ObjID, OBJIDLEN, val);
		return TRUE;
	}
	if (service == "CarierInputSpec") {
		CarrierInputSpec.Add(val);
		return TRUE;
	}
	if (service == "ProcessingCtrlSpec") {
		ProcessingCtrlSpec.Add(val);
	}
	if (service == "CurrentPRJob") {
		// Yuen: Not supported [2002/10/18  20:08]
		ASSERT(0);
	}
	return FALSE;
}

BOOL CCtrlJob::SetAttr(CString& service, BOOL bAuto) {
	StartMethod = (bAuto) ? 1 : 0;
	return TRUE;
}

BOOL CCtrlJob::SetAttr(CString& service, BYTE POMgmt) {
	ProcessOrderMgmt = POMgmt;
	return TRUE;
}

BOOL CCtrlJob::SetAttr(CString& service, CString FromCarrier, CString ToCarrier) {
	// Yuen: Not supported [2002/10/18  20:08]
	ASSERT(0);
	return FALSE;
}

BOOL CCtrlJob::SetAttr(CString& service, vector < UINT > vec) {
	PauseEvents = vec;
	return TRUE;
}

//***************************************************************
// Yuen: Obsolete, functionality provided by CCtrlJobMgr [2002/10/18  17:42]
// BOOL CCtrlJob::GetAttr(CString & nSer, char * buf)
// {
// 	ASSERT(0);
// 	return FALSE;
// }
//
// BOOL CCtrlJob::SetAttr(CString & service, void * val)
// {
// 	ASSERT(0);
// 	return FALSE;
// }

// Yuen: Obsolete : capability provided by CCtrlJobMgr [2002/10/18  17:45]
void CCtrlJob::GetAttrName(CStringArray& nArray) {
	ASSERT(0);
}

BOOL CCtrlJob::Dequeue() {
	for (int i = 0; i < ProcessingCtrlSpec.GetSize(); i++) {
		CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
		if (p) {
			if (p->Dequeue()) {
				PJobMgr.RemovePRJob(p);
			}
		}
	}
	return TRUE;
}

// Yuen: TRUE means can remove object from memory [2002/10/19  23:53]
BOOL CCtrlJob::Stop() {
	switch (GetState()) {
	case CCtrlJob::QUEUED:
	case CCtrlJob::COMPLETED:
		return TRUE;
	case CCtrlJob::SELECTED:
	case CCtrlJob::WATINGFORSTART:
		StateChange(CCtrlJob::STOPPED);
		return TRUE;
		break;
	case CCtrlJob::EXECUTING:
		Stopping();
		break;
	case CCtrlJob::PAUSED:
		// Yuen: Do not known how to abort just yet [2002/10/20  0:00]
		break;
	}
	return FALSE;
}

BOOL CCtrlJob::Abort() {
	switch (GetState()) {
	case CCtrlJob::QUEUED:
	case CCtrlJob::COMPLETED:
		return TRUE;
	case CCtrlJob::SELECTED:
	case CCtrlJob::WATINGFORSTART:
		StateChange(CCtrlJob::ABORTED);
		return TRUE;
		break;
	case CCtrlJob::EXECUTING:
		Aborting();
		break;
	case CCtrlJob::PAUSED:
		// Yuen: Do not known how to abort just yet [2002/10/20  0:00]
		break;
	}
	return FALSE;
}

void CCtrlJob::Deselect() {}

void CCtrlJob::Resume() {}

void CCtrlJob::RemoveAllPRJob() {
	for (int i = ProcessingCtrlSpec.GetSize() - 1; i >= 0; i--) {
		CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
		if (p) {
			PJobMgr.RemovePRJob(p);
			// Yuen: Do not remove name just in case the CJ is kept [2002/10/20  16:25]
			//ProcessingCtrlSpec.RemoveAt(i);
		}
	}
}

int CCtrlJob::SetAttr(const string& AttrID, SecsItem* pItem, BOOL bCreating) {
	if (!pItem) {
		return UNKNOWN_E5_ERROR;
	}
	string TempVal;

	CString strAttrID = AttrID.c_str();

	if (0 == strAttrID.CompareNoCase("ObjID")) {
		if ((!pItem->GetASCII(TempVal))) {
			Log("Illegal ObjID", 2);
			return INVALID_ATTRIBUTE_VALUE;
		}
		strscpy(m_ObjID, OBJIDLEN, TempVal.c_str());
		return NO_E5_ERROR;
	}
	// ReadOnly Attribute ??
	else if (0 == strAttrID.CompareNoCase("CurrentPRJob")) {
		Log("Reject: CurrentPRJob is read only", 2);
		return READ_ONLY_ATTRIBUTE_ACCESS_DENIED;
	}
	else if (0 == strAttrID.CompareNoCase("CarrierInputSpec")) {
		int nNumCarriers = pItem->size();
		nNumCarriers = 1;
		if (!nNumCarriers) {
			Log("No carrier ID specified", 2);
			return INVALID_ATTRIBUTE_VALUE;
		}
		if (nNumCarriers > 1) {
			Log("More then one carrier ID specified", 2);
			return INVALID_ATTRIBUTE_VALUE;
		}
		for (int nIndex = 0; nIndex < nNumCarriers; nIndex++) {
			if (!(*pItem)[0][nIndex].GetASCII(TempVal)) {
				Log("Failed to retrieve carrier ID", 2);
				return INVALID_ATTRIBUTE_VALUE;
			}
			if (TempVal.empty()) {
				Log("Invalid carrier ID", 2);
				return INVALID_ATTRIBUTE_VALUE;
			}
			CarrierInputSpec.Add(TempVal.c_str());
		}
		return NO_E5_ERROR;
	}
	else if (0 == strAttrID.CompareNoCase("StartMethod")) {
		bool bAuto;
		if (!pItem->GetBoolean(bAuto)) {
			Log("Invalid StartMethod", 2);
			return INVALID_ATTRIBUTE_VALUE;
		}
		StartMethod = (bAuto) ? 1 : 0;
		return NO_E5_ERROR;
	}
	else if (0 == strAttrID.CompareNoCase("ProcessOrderMgmt")) {
		BYTE POMgmt;
		string strPOMgmt;
		if (!pItem->GetUInt1(POMgmt)) {
			if (!pItem->GetASCII(strPOMgmt)) {
				Log("Invalid ProcessOrderMgmt format", 2);
				return INVALID_ATTRIBUTE_VALUE;
			}
			POMgmt = strPOMgmt[0] - '0';
		}
		if ((POMgmt > 2) || (POMgmt < 1)) {
			Log("Invalid ProcessOrderMgmt", 2);
			return INVALID_ATTRIBUTE_VALUE;
		}
		ProcessOrderMgmt = POMgmt;
		return NO_E5_ERROR;
	}
	else if (0 == strAttrID.CompareNoCase("MtrlOutSpec")) {
		map < string, string > OutSpec;
		string FromCarrier, ToCarrier;
		int nNumEntries = pItem->size();
		int NNumSlots = 0;
		BYTE FromSlot, ToSlot, nNumSlots;
		char Buf[25];
		for (int nIndex = 0; nIndex < nNumEntries; nIndex++) {
			if ((2 != (*pItem)[nIndex].size()) || (2 != (*pItem)[nIndex][0].size()) ||
				(2 != (*pItem)[nIndex][1].size()) || (0 == (*pItem)[nIndex][0][1].size()) ||
				((*pItem)[nIndex][0][1].size() != (*pItem)[nIndex][1][1].size()) ||
				(!(*pItem)[nIndex][0][0].GetASCII(FromCarrier)) || (!(*pItem)[nIndex][1][0].GetASCII(ToCarrier))) {
				return INVALID_ATTRIBUTE_VALUE;
			}
			nNumSlots = (*pItem)[nIndex][0][1].size();
			for (int nNum = 0; nNum < nNumSlots; nNum++) {
				if ((!(*pItem)[nIndex][0][1][nNum].GetUInt1(FromSlot)) ||
					(!(*pItem)[nIndex][1][1][nNum].GetUInt1(ToSlot))) {
					return INVALID_ATTRIBUTE_VALUE;
				}
				FromCarrier += ":";
				FromCarrier += _itoa(FromSlot, Buf, 10);
				ToCarrier += ":";
				ToCarrier += _itoa(ToSlot, Buf, 10);
				OutSpec.insert(map < string, string >::value_type(FromCarrier, ToCarrier));
			}
		}
		return NO_E5_ERROR;
	}
	else if (0 == strAttrID.CompareNoCase("ProcessingCtrlSpec")) {
		string PrJobID;
		vector < string > TempPVec;
		int nNumPrJobs = pItem->size();
		if (0 == nNumPrJobs) {
			Log("No PrJob specified", 2);
			return INVALID_ATTRIBUTE_VALUE;
		}
		int nIndex;
		for (nIndex = 0; nIndex < nNumPrJobs; nIndex++) {
			if ((3 != (*pItem)[nIndex].size()) || (!(*pItem)[nIndex][0].GetASCII(PrJobID))) {
				Log("Invalid ProcessingCtrlSpec attribute format", 2);
				return INVALID_ATTRIBUTE_VALUE;
			}
			if (PrJobID.empty()) {
				Log("Invalid PrJob ID specified", 2);
				return INVALID_ATTRIBUTE_VALUE;
			}
			TempPVec.push_back(PrJobID.c_str());
		}
		for (nIndex = 0; nIndex < int(TempPVec.size()); nIndex++) {
			ProcessingCtrlSpec.Add(TempPVec[nIndex].c_str());
		}

		return NO_E5_ERROR;
	}
	else if (0 == strAttrID.CompareNoCase("PauseEvent")) {
		vector < UINT > TempVec;
		int nNumPE = pItem->size();
		UINT EvtID;
		for (int nIndex = 0; nIndex < nNumPE; nIndex++) {
			if (!(*pItem)[nIndex].GetAInt(EvtID)) {
				return INVALID_ATTRIBUTE_VALUE;
			}
			TempVec.push_back(EvtID);
		}
		PauseEvents = TempVec;
		return NO_E5_ERROR;
	}
	else {
		return UNKNOWN_E5_ERROR;
	}
}

CPRJob* CCtrlJob::GetActivePRJob() {
	for (int i = 0; i < ProcessingCtrlSpec.GetSize(); i++) {
		CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
		if (p) {
			if (p->IsActive()) {
				return p;
			}
		}
	}
	return NULL;
}

BOOL CCtrlJob::GetAttr(string& nSer, SecsItemList* pItemList) {
	string Temp;
	if (nSer == "ObjType") {
		Temp = m_ObjType;
		pItemList->AddItem(new SecsItemASCII(Temp));
		return TRUE;
	}
	else if (nSer == "ObjID") {
		Temp = m_ObjID;
		pItemList->AddItem(new SecsItemASCII(Temp));
		return TRUE;
	}

	//20130831-1 SCOTT EDIT 	else if (nSer == "CurrentPRJob")
	else if (nSer == "CurrentPRJob" || nSer == "ProcessJobs") {
		SecsItemList* pNew = new SecsItemList;
		for (int nIndex = 0; nIndex < ProcessingCtrlSpec.GetSize(); nIndex++) {
			//CString Temp = ProcessingCtrlSpec.GetAt(nIndex);
			pNew->AddItem(new SecsItemASCII(ProcessingCtrlSpec[nIndex].GetBuffer(0)));
		}
		pItemList->AddItem(pNew);
		return TRUE;
		//CopyMemory( buf , &ProcessingCtrlSpec, sizeof(ProcessingCtrlSpec) );
		// Do not copy into the Buffer beacuase of ,  this value is a arry.
		// So , you can directly access the variable
	}
	else if (nSer == "CarrierInputSpec") {
		SecsItemList* pNew = new SecsItemList;
		for (int nIndex = 0; nIndex < CarrierInputSpec.GetSize(); nIndex++) {
			//CString Temp = CarrierInputSpec.GetAt(nIndex);
			pNew->AddItem(new SecsItemASCII(CarrierInputSpec[nIndex].GetBuffer(0)));
		}
		pItemList->AddItem(pNew);
		return TRUE;
		//CopyMemory( buf , &CarrierInputSpec , sizeof ( CarrierInputSpec) );
		// Do not copy into the Buffer beacuase of ,  this value is a arry.
		// So , you can directly access the variable
	}
	else if (nSer == "StartMethod") {
		pItemList->AddItem(new SecsItemBoolean(StartMethod == TRUE));
		return TRUE;
	}
	else if ((nSer == "State") || (nSer == "CJobState")) {
		int State;
		switch (GetState()) {
		case CCtrlJob::NO_STATE:
			State = 1;
			break;
		case CCtrlJob::QUEUED:
			State = 2;
			break;
		case CCtrlJob::SELECTED:
			State = 3;
			break;
		case CCtrlJob::WATINGFORSTART:
			State = 4;
			break;
		case CCtrlJob::EXECUTING:
			State = 10;
			break;
		case CCtrlJob::PAUSED:
			State = 6;
			break;
		case CCtrlJob::COMPLETED:
			State = 5;
			break;
		case CCtrlJob::CJACTIVE:
			State = 8;
			break;
		case CCtrlJob::ABORTED:
			State = 5;
			break;
		case CCtrlJob::STOPPED:
			State = 5;
			break;
		default:
			State = 0;
		}
		pItemList->AddItem(new SecsItemUInt1(State));
		return TRUE;
	}
	else if (nSer == "ProcessOrderMgmt") {
		pItemList->AddItem(new SecsItemUInt1(this->ProcessOrderMgmt));
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL CCtrlJob::CanPRJobStart() {
	ErrorMsg = "";
	for (int i = 0; i < ProcessingCtrlSpec.GetSize(); i++) {
		CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
		if (!p) {
			ErrorMsg = "PRJob not found";
			return FALSE;
		}
		if (!p->CanSetup()) {
			ErrorMsg = "PRJob cannot setup";
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CCtrlJob::IsAutoStart() {
	return StartMethod;
}

BOOL CCtrlJob::CanDequeue() {
	switch (GetState()) {
	case CCtrlJob::QUEUED:
		for (int i = 0; i < ProcessingCtrlSpec.GetSize(); i++) {
			CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
			if (p) {
				if (p->IsActive()) {
					return FALSE;
				}
			}
		}
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CCtrlJob::CanPause() {
	if (GetState() == CCtrlJob::EXECUTING) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJob::CanAbort() {
	switch (GetState()) {
	case CCtrlJob::NO_STATE:
	case CCtrlJob::COMPLETED:
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL CCtrlJob::CanStop() {
	switch (GetState()) {
	case CCtrlJob::NO_STATE:
	case CCtrlJob::COMPLETED:
		return FALSE;
		break;
	}
	return TRUE;
}

BOOL CCtrlJob::CanSelect() {
	if (GetState() == CCtrlJob::QUEUED) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJob::CanDeselect() {
	if (GetState() == CCtrlJob::SELECTED) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJob::CanCancel() {
	switch (GetState()) {
	case CCtrlJob::QUEUED:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CCtrlJob::CanResume() {
	if (GetState() == CCtrlJob::PAUSED) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCtrlJob::CanStart() {
	switch (GetState()) {
	case CCtrlJob::WATINGFORSTART:
	case CCtrlJob::SELECTED:
	case CCtrlJob::PAUSED:
		return CanPRJobStart();
		break;
	}
	return FALSE;
}

BOOL CCtrlJob::Stopping() {
	StateEx = STOPPING;
	CPRJob* p = GetActivePRJob();
	if (p) {
		return p->Stop();
	}
	return FALSE;
}

BOOL CCtrlJob::Aborting() {
	StateEx = ABORTING;
	CPRJob* p = GetActivePRJob();
	if (p) {
		return p->Abort();
	}
	return FALSE;
}

BOOL CCtrlJob::SelectPRJob() {
	ErrorMsg = "";
	if (ProcessingCtrlSpec.GetSize()) {
		for (int i = 0; i < ProcessingCtrlSpec.GetSize(); i++) {
			CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
			if (p) {
				if (p->GetState() == CPRJob::QUEUED_POOLED) {
					if (p->Start()) {
						// 						StateChange(CCtrlJob::EXECUTING);
						if (pPWnd) {
							// Inform ProcJob413ProcessingDlg to load new cassette file [9/5/2007 LYF]
							::PostMessage(pPWnd->m_hWnd, UM_PJSTART, 0, (LPARAM)p);
						}
						return TRUE;
					}
					ErrorMsg = "PJob start error: " + p->ErrorMsg;
					return FALSE;
				}
			}
		}
		ErrorMsg = "No PRJob in QUEUED_POOLED state";
		return FALSE;
	}
	ErrorMsg = "ProcessingCtrSpec is EMPTY";
	return FALSE;
}

BOOL CCtrlJob::Stopped() {
	if (ProcessingCtrlSpec.GetSize()) {
		for (int i = 0; i < ProcessingCtrlSpec.GetSize(); i++) {
			CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
			if (p) {
				if (p->Stopped()) {
					PJobMgr.RemovePRJob(p);
				}
			}
		}
	}
	StateChange(CCtrlJob::STOPPED);
	return TRUE;
}

BOOL CCtrlJob::Aborted() {
	if (ProcessingCtrlSpec.GetSize()) {
		for (int i = 0; i < ProcessingCtrlSpec.GetSize(); i++) {
			CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
			if (p) {
				switch (p->GetState()) {
				case CPRJob::ABORTING:
				case CPRJob::QUEUED_POOLED:
					PJobMgr.RemovePRJob(p);
					break;
				default:
					ASSERT(0);
					break;
				}
			}
		}
	}
	StateChange(CCtrlJob::ABORTED);
	return TRUE;
}

// 0 = all checks out correctly
// -1 = At least one PRJob not found
// -2 = At least one carrier id not match
short CCtrlJob::IsPRJobValid() {
	for (int i = 0; i < ProcessingCtrlSpec.GetSize(); i++) {
		CPRJob* p = PJobMgr.GetPRJob(ProcessingCtrlSpec.GetAt(i));
		if (p) {
			if (p->m_PRMtlName != CarrierInputSpec[0]) {
				return -2;
			}
		}
		else {
			return -1;
		}
	}
	return 0;
}

BOOL CCtrlJob::IsCarrierPresent() {
	for (int i = 0; i < Var.nLoadPort; i++) {
		for (int j = 0; j < CarrierInputSpec.GetSize(); j++) {
			if (pGDev && pGDev->pGEMObjs->Cms[i].CaObj.ObjID == CarrierInputSpec[j]) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CCtrlJob::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CCtrlJob");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}