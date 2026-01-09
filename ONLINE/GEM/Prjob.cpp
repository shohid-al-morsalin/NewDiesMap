#include "stdafx.h"
#include "strSOp.h"

#include "128/GEM/ObjectsContainer.h"
#include "CtrlJobMgr.h"
#include "GemFactory.h"
#include "GlobalDeviceContainer.h"
#include "IFC/SecsSvrF.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/ShareMemory/SharedMemQue.h"
#include "SecsItem.h"

#include "PRJob.h"

using namespace std;
extern UINT TimeFormat;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPRJob::CPRJob() {
	ClearAll();
}

CPRJob::~CPRJob() {}

void CPRJob::SetObjectID(char* pObjID) {
	if (strlen(pObjID) > sizeof(m_ObjID) - 1) {
		ASSERT(0);
		return;
	}
	strscpy(m_ObjID, OBJIDLEN, pObjID);
}

BOOL CPRJob::GetAttr(std::string service, SecsItemList* pList) {
	std::string Temp;
	if (service == "ObjID") {
		Temp = m_ObjID;
		pList->AddItem(new SecsItemASCII(Temp));
		return TRUE;
	}
	else if (service == "ObjType") {
		Temp = m_ObjType;
		pList->AddItem(new SecsItemASCII(Temp));
		return TRUE;
	}
	else if ((service == "PRJobState") || (service == "State")) {
		Temp = "-";
		switch (JobState) {
		case CPRJob::NO_STATE:
			Temp = "NO STATE";
			break;
		case CPRJob::QUEUED_POOLED:
			Temp = "QUEUED";
			break;
		case CPRJob::SETTING_UP:
			Temp = "SETTING UP";
			break;
		case CPRJob::WAITING_FOR_START:
			Temp = "WAITING FOR START";
			break;
		case CPRJob::PROCESSING:
			Temp = "PROCESSING";
			break;
		case CPRJob::PROCESS_COMPLETE:
			Temp = "PROCESS COMPLETE";
			break;
		case CPRJob::EXECUTING:
			Temp = "EXECUTING";
			break;
		case CPRJob::PAUSING:
			Temp = "PAUSING";
			break;
		case CPRJob::PAUSED:
			Temp = "PAUSED";
			break;
		case CPRJob::ABORTING:
			Temp = "ABORTING";
			break;
		case CPRJob::STOPPING:
			Temp = "STOPPING";
			break;
		}
		pList->AddItem(new SecsItemASCII(Temp));
		return TRUE;
	}
	else if (service == "PRMtlName") {
		Temp = m_PRMtlName;
		pList->AddItem(new SecsItemASCII(Temp));
		return TRUE;
	}
	else if (service == "PRMtlType") {
		BYTE MF_Wafer = 14;
		pList->AddItem(new SecsItemBinary(&MF_Wafer, 1));
		return TRUE;
	}
	else if (service == "PRProcessStart") {
		pList->AddItem(new SecsItemBoolean(m_PRProcessStart == TRUE));
		return TRUE;
	}
	else if (service == "RecipeVariableList") {
		for (int i = 0; i < 26; i++) {
			if (m_Param[i].SlotNo) {
				CString str;
				SecsItemList* pNew;
				pNew = new SecsItemList;
				pNew->AddItem(new SecsItemASCII("Slot No"));
				pNew->AddItem(new SecsItemUInt2(m_Param[i].SlotNo));
				pList->AddItem(pNew);
				pNew = new SecsItemList;
				pNew->AddItem(new SecsItemASCII("LotID"));
				pNew->AddItem(new SecsItemASCII(m_Param[i].LotID));
				pList->AddItem(pNew);
				pNew = new SecsItemList;
				pNew->AddItem(new SecsItemASCII("WaferID"));
				pNew->AddItem(new SecsItemASCII(m_Param[i].Waferid));
				pList->AddItem(pNew);
				pNew = new SecsItemList;
				pNew->AddItem(new SecsItemASCII("WAFERTHICKNESS"));
				pNew->AddItem(new SecsItemASCII(m_Param[i].Filmthk));
				pList->AddItem(pNew);
			}
		}

		return TRUE;
	}
	else if (service == "PortNo") {
		pList->AddItem(new SecsItemUInt2(m_PortNo));
		return TRUE;
	}
	else if (service == "PRMtlNameList") {
		SecsItemList* pNew = new SecsItemList;
		pNew->AddItem(new SecsItemASCII(m_PRMtlName));
		pList->AddItem(pNew);
		return TRUE;
	}
	else if (service == "PRMtlOrder") {
		pList->AddItem(new SecsItemUInt1(1));
		return TRUE;
	}
	else if ((service == "PRRecipe") || ((service == "RecID"))) {
		pList->AddItem(new SecsItemASCII(m_Recipe));
		return TRUE;
	}
	else if (service == "PRRecipeMethod") {
		pList->AddItem(new SecsItemUInt1(2));
		return TRUE;
	}
	else if (service == "Timestamp") {
		this->GetCurrTime(Temp);
		pList->AddItem(new SecsItemASCII(Temp));
		return TRUE;
	}
	else if (service == "PauseEvent") {
		SecsItemList* EventsList = new SecsItemList;
		for (int nIndex = 0; nIndex < int(m_PauseEvents.size()); nIndex++) {
			EventsList->AddItem(new SecsItemUInt2(m_PauseEvents[nIndex]));
		}
		pList->AddItem(EventsList);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::GetAttr(const std::string& service, std::string& Data) {
	char Buf[126] =
	{
	0
	};
	if (service == "ObjID") {
		Data = m_ObjID;
		return TRUE;
	}
	else if (service == "ObjType") {
		Data = m_ObjType;
		return TRUE;
	}
	else if ((service == "PRJobState") || (service == "State")) {
		Data = m_PRJobState;
		return TRUE;
	}
	else if (service == "PRMtlName") {
		Data = m_PRMtlName;
		return TRUE;
	}
	else if (service == "PRMtlType") {
		Data = "WAFER";
		return TRUE;
	}
	else if (service == "PRProcessStart") {
		Data = (m_PRProcessStart == TRUE) ? "Auto" : "Manual";
		return TRUE;
	}
	else if (service == "PortNo") {
		Data = _itoa(m_PortNo, Buf, 10);
		return TRUE;
	}
	else if (service == "PRMtlNameList") {
		Data = m_PRMtlName;
		return TRUE;
	}
	else if (service == "PRMtlOrder") {
		Data = "Arrival";
		return TRUE;
	}
	else if (service == "PRRecipe") {
		Data = m_Recipe;
		return TRUE;
	}
	else if (service == "PRRecipeMethod") {
		Data = "2";
		return TRUE;
	}
	else if (service == "Timestamp") {
		GetCurrTime(Data);
		return TRUE;
	}
	else if (service == "PauseEvent") {
		for (int nIndex = 0; nIndex < int(m_PauseEvents.size()); nIndex++) {
			_itoa(m_PauseEvents[nIndex], Buf, 10);
			strcat(Buf, "\r\n");
			Data += '\t';
			Data += Buf;
		}
		return TRUE;
	}
	return FALSE;
}

void CPRJob::GetCurrTime(std::string& TimeStr) {
	char dtime[25];
	SYSTEMTIME CurTime;
	::GetLocalTime(&CurTime);
	UINT nYear = CurTime.wYear;
	if (0 == ::TimeFormat) {
		nYear %= 1000;
	}
	memset(dtime, ' ', sizeof dtime);
	if (0 == TimeFormat) {
		// 12 digit format
		sprintf(dtime, "%0.2d%0.2d%0.2d%0.2d%0.2d%0.2d", nYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute,
			CurTime.wSecond);
	}
	else {
		// 16 digit format
		sprintf(dtime, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d%0.2d", nYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour,
			CurTime.wMinute, CurTime.wSecond, CurTime.wMilliseconds / 10);
	}
	TimeStr = dtime;
}

// Yuen: Dropped, not secure [2002/10/18  20:49]
BOOL CPRJob::GetAttr(CString service, void* buf) {
	ASSERT(0);
	return FALSE;
}

// Yuen: Not supported [2002/10/18  22:25]
BOOL CPRJob::SetAttr(CString service, void* data) {
	return FALSE;
}

void CPRJob::GetAttrName(CStringArray& nArray) {
	nArray.Copy(m_AttrIDName);
}

void CPRJob::StateChange(PRJOBSTATE NewState) {
	UINT CEID = 0;
	std::string Start = "", Finish = "";
	switch (JobState) {
	case CPRJob::NO_STATE:
		Start = "NO STATE";
		switch (NewState) {
		case CPRJob::QUEUED_POOLED:
			Finish = "QUEUED";
			CEID = CEID_LIST::PJSM01NOSTATETOQUEUED;
			break;
		}
		break;
	case CPRJob::QUEUED_POOLED:
		Start = "QUEUED";
		switch (NewState) {
		case CPRJob::SETTING_UP:
			Finish = "SETTING UP";
			CEID = CEID_LIST::PJSM02QUEUEDTOSETTINGUP;
			break;
		case CPRJob::NO_STATE:
			Finish = "NO STATE";
			CEID = CEID_LIST::PJSM18QUEUEDTONOSTATE;
			break;
		}
		break;
	case CPRJob::SETTING_UP:
		Start = "SETTING UP";
		switch (NewState) {
		case CPRJob::WAITING_FOR_START:
			Finish = "WAITING FOR START";
			CEID = CEID_LIST::PJSM03SETTINGUPTOWAITINGFORSTART;
			break;
		case CPRJob::PROCESSING:
			Finish = "PROCESSING";
			CEID = CEID_LIST::PJSM04SETTINGUPTOPROCESSING;
			break;
		case CPRJob::PAUSING:
			ASSERT(0);
			Finish = "PAUSING";
			CEID = CEID_LIST::PJSM08EXECUTINGTOPAUSING;
			break;
		case CPRJob::STOPPING:
			ASSERT(0);
			Finish = "STOPPING";
			CEID = CEID_LIST::PJSM11EXECUTINGTOSTOPPING;
			break;
		case CPRJob::ABORTING:
			ASSERT(0);
			Finish = "ABORTING";
			CEID = CEID_LIST::PJSM13EXECUTINGTOABORTING;
			break;
		}
		break;
	case CPRJob::WAITING_FOR_START:
		Start = "WAITING FOR START";
		switch (NewState) {
		case CPRJob::PROCESSING:
			Finish = "PROCESSING";
			CEID = CEID_LIST::PJSM05WAITINGFORSTARTTOPROCESSING;
			break;
		case CPRJob::PAUSING:
			ASSERT(0);
			Finish = "PAUSING";
			CEID = CEID_LIST::PJSM08EXECUTINGTOPAUSING;
			break;
		case CPRJob::STOPPING:
			ASSERT(0);
			Finish = "STOPPING";
			CEID = CEID_LIST::PJSM11EXECUTINGTOSTOPPING;
			break;
		case CPRJob::ABORTING:
			ASSERT(0);
			Finish = "ABORTING";
			CEID = CEID_LIST::PJSM13EXECUTINGTOABORTING;
			break;
		}
		break;
	case CPRJob::PROCESSING:
		Start = "PROCESSING";
		switch (NewState) {
		case CPRJob::PROCESS_COMPLETE:
			Finish = "PROCESS COMPLETE";
			CEID = CEID_LIST::PJSM06PROCESSINGTOPROCESSCOMPLETE;
			break;
		case CPRJob::PAUSING:
			Finish = "PAUSING";
			CEID = CEID_LIST::PJSM08EXECUTINGTOPAUSING;
			break;
		case CPRJob::STOPPING:
			Finish = "STOPPING";
			CEID = CEID_LIST::PJSM11EXECUTINGTOSTOPPING;
			break;
		case CPRJob::ABORTING:
			Finish = "ABORTING";
			CEID = CEID_LIST::PJSM13EXECUTINGTOABORTING;
			break;
		}
		break;
	case CPRJob::PROCESS_COMPLETE:
		Start = "PROCESS COMPLETE";
		switch (NewState) {
		case CPRJob::NO_STATE:
			Finish = "NO STATE";
			CEID = CEID_LIST::PJSM07PROCESSCOMPLETETONOSTATE;
			break;
		}
		break;
	case CPRJob::EXECUTING:
		Start = "EXECUTING";
		switch (NewState) {
		case CPRJob::PAUSING:
			Finish = "PAUSING";
			CEID = CEID_LIST::PJSM08EXECUTINGTOPAUSING;
			break;
		case CPRJob::STOPPING:
			Finish = "STOPPING";
			CEID = CEID_LIST::PJSM11EXECUTINGTOSTOPPING;
			break;
		case CPRJob::ABORTING:
			Finish = "ABORTING";
			CEID = CEID_LIST::PJSM13EXECUTINGTOABORTING;
			break;
		}
		break;
	case CPRJob::PAUSING:
		Start = "PAUSING";
		switch (NewState) {
		case CPRJob::PAUSED:
			Finish = "PAUSED";
			CEID = CEID_LIST::PJSM09PAUSINGTOPAUSED;
			break;
		}
		break;
	case CPRJob::PAUSED: // Concurrent Substate
		Start = "PAUSED";
		switch (NewState) {
		case CPRJob::EXECUTING:
			Finish = "EXECUTING";
			CEID = CEID_LIST::PJSM10PAUSETOEXECUTING;
			break;
		case CPRJob::STOPPING:
			Finish = "STOPPING";
			CEID = CEID_LIST::PJSM12PAUSETOSTOPPING;
			break;
		case CPRJob::ABORTING:
			Finish = "ABORTING";
			CEID = CEID_LIST::PJSM15PAUSETOABORTING;
			break;
		}
		break;
	case CPRJob::ABORTING:
		Start = "ABORTING";
		switch (NewState) {
		case CPRJob::NO_STATE:
			Finish = "NO STATE";
			CEID = CEID_LIST::PJSM16ABORTINGTONOSTATE;
			break;
		}
		break;
	case CPRJob::STOPPING:
		Start = "STOPPING";
		switch (NewState) {
		case CPRJob::NO_STATE:
			Finish = "NO STATE";
			CEID = CEID_LIST::PJSM17STOPPINGTONOSTATE;
			break;
		}
		break;
	}

	JobState = NewState;

	if (CEID != 0) {
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		strscpy(m_PRJobState, PRJOBSTATELEN, Finish.c_str());
		pEvt->PRJobState = GetState();
		strscpy(pEvt->ObjID, OBJIDLEN, m_ObjID);
		CString CjID;
		if (CJobMgr.CtrlJobIDFromPj(m_ObjID, CjID)) {
			strscpy(pEvt->CJobID, CRJIDLEN, CjID.GetBuffer(0));
		}
		CSharedMemQue pQue;
		pQue.OpenMemory("EVTMSG");
		pEvt->Set(EVT_CMS, CEID);
		pGDev->SetActivePort(m_PortNo);
		pGDev->SetActiveCJPJ(CjID.GetBuffer(0), m_ObjID);
		pGDev->SetActivePrJob(m_ObjID);
		pQue.WriteQue(pEvt);
		pGDev->SetActivePrJob("");
		pGDev->SetActiveCtrlJob("");

		pEvt = new CEvt;
		ASSERT(pEvt);
		strscpy(m_PRJobState, PRJOBSTATELEN, Finish.c_str());
		strscpy(pEvt->ObjID, OBJIDLEN, m_ObjID);
		pEvt->PRJobState = GetState();
		strscpy(pEvt->ObjID, OBJIDLEN, m_ObjID);
		if (CJobMgr.CtrlJobIDFromPj(m_ObjID, CjID)) {
			strscpy(pEvt->CJobID, CRJIDLEN, CjID.GetBuffer(0));
		}
		pEvt->Set(EVT_PJM_PRJEN, CEID);
		pGDev->SetActivePort(m_PortNo);
		pGDev->SetActivePrJob(m_ObjID);
		pGDev->SetActiveCtrlJob(pEvt->CJobID);
		pQue.WriteQue(pEvt);
		pGDev->SetActivePrJob("");
		pGDev->SetActiveCtrlJob("");

		// Send to Log server
		std::string Msg;
		Msg = m_ObjID;
		Msg += ": " + Start + " -> " + Finish;

		Log(Msg.c_str());
	}
}

BOOL CPRJob::Queued() {
	if (IsNoState()) {
		StateChange(CPRJob::QUEUED_POOLED);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::SettingUp() {
	if (IsQueued()) {
		StateChange(CPRJob::SETTING_UP);
		AlertNotify(CPRJob::alSETTING_UP);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Processing() {
	if (CanProcess()) {
		StateChange(CPRJob::PROCESSING);
		AlertNotify(CPRJob::alPROCESSING);
		WriteStartData413();
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::ProcessingComplete() {
	if (IsProcessing()) {
		StateChange(CPRJob::PROCESS_COMPLETE);
		AlertNotify(CPRJob::alCOMPLETED_PROCESSING);
		return TRUE;
	}
	return FALSE;
	// PRJob will be removed from queue at the end of this function by the caller
}

BOOL CPRJob::WaitingStart() {
	if (m_PRProcessStart) {
		return FALSE;
	}
	if (IsSettingUp()) {
		StateChange(CPRJob::WAITING_FOR_START);
		AlertNotify(CPRJob::alJOB_WAITITNG_FOR_START);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Pausing() {
	if (IsExecuting()) {
		ResumeState = JobState;
		StateChange(CPRJob::PAUSING);
		// Yuen: Inform C2C to pause, C2C will signal completion of pausing to caller of this function [2002/10/18  22:59]
		return PauseProcessing();
	}
	return FALSE;
}

BOOL CPRJob::Paused() {
	if (IsPausing()) {
		StateChange(CPRJob::PAUSED);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Resume() {
	if (IsPaused()) {
		StateChange(ResumeState);
		if (IsActive()) {
			ResumeProcessing();
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Abort() {
	if (!IsAborting()) {
		if (IsQueued()) {
			// behave like PRJobCancel
			StateChange(CPRJob::NO_STATE);
			AlertNotify(CPRJob::alJOB_COMPLETE);
			return TRUE;
		}
		else if (IsActive()) {
			StateChange(CPRJob::ABORTING);
			// Yuen: Inform C2C to abort, C2C will in turn signal abort completion [2002/10/18  22:04]
			AbortProcessing();
			return TRUE; // Yuen: Keep object until C2C signal completion [2002/10/18  22:13]
		}
	}
	return FALSE;
}

BOOL CPRJob::Stop() {
	if (IsExecuting()) {
		StateChange(CPRJob::STOPPING);
		// Yuen: Inform C2C to stop, C2C will in turn signal stop complete to caller of this function [2002/10/18  23:18]
		StopProcessing();
		return TRUE;
	}
	return FALSE;
}

void CPRJob::SetMtlName(const char* name) {
	strscpy(m_PRMtlName, OBJIDLEN, name);
}

void CPRJob::SetMtlType(short type) {
	m_PRMtlType = (PRMTLTYPE)type;
}

void CPRJob::SetPRStart(BOOL start) {
	m_PRProcessStart = start;
}

void CPRJob::SetParamPtnID(short ptn) {
	m_PortNo = ptn;
}

void CPRJob::SetParamSlotNo(short slotno, int arryno) {
	m_Param[arryno].SlotNo = slotno;
}

void CPRJob::SetParamWaferID(char* waferid, int arryno) {
	strscpy(m_Param[arryno].Waferid, OBJIDLEN, waferid);
}

void CPRJob::SetParamLotID(char* lotid, int arryno) {
	strscpy(m_Param[arryno].LotID, LOTIDLEN, lotid);
}

void CPRJob::SetParamFilmThickness(float val, int arryno) {
	m_Param[arryno].Filmthk = val;
}

void CPRJob::SetParamWaferThickness(float val, int arryno) {
	m_Param[arryno].Waferthk = val;
}

void CPRJob::ClearAll() {
	ZeroMemory(m_ObjType, sizeof(m_ObjType));
	strscpy(m_ObjType, OBJTYPELEN, "ProcessJob");
	ZeroMemory(m_ObjID, sizeof(m_ObjID));
	m_Paramcnt = 0;
	m_PortNo = 0;
	JobState = CPRJob::NO_STATE;
	ResumeState = JobState;
	m_PRMtlType = (PRMTLTYPE)1;
	for (int i = 0; i < 25; i++) {
		m_Param[i].ClearAll();
	}
	m_AttrIDName.RemoveAll();
	m_AttrIDName.Add("ObjID");
	m_AttrIDName.Add("ObjType");
	m_AttrIDName.Add("PRJobState");
	m_AttrIDName.Add("PRJobStateList");
	m_AttrIDName.Add("PRMtlName");
	m_AttrIDName.Add("PRMtlType");
	m_AttrIDName.Add("PRProcessStart");
	m_AttrIDName.Add("Param");
	m_AttrIDName.Add("PortNo");

	memset(m_PRMtlName, 0, sizeof m_PRMtlName);
}

void CPRJob::AlertNotify(CPRJob::PRJOBMILESTONES MileStone) {
	char Data[52];
	int MLs = 0;
	switch (MileStone) {
	case alSETTING_UP:
		strscpy(Data, sizeof(Data), "SETTING UP");
		MLs = CEID_LIST::PJALERTSETUP;
		break;
	case alPROCESSING:
		strscpy(Data, sizeof(Data), "PROCESSING");
		MLs = CEID_LIST::PJALERTPROCESSING;
		break;
	case alCOMPLETED_PROCESSING:
		strscpy(Data, sizeof(Data), "PROCESSING COMPLETE");
		MLs = CEID_LIST::PJALERTPROCESINGCOMPLETE;
		break;
	case alJOB_COMPLETE:
		strscpy(Data, sizeof(Data), "JOB COMPLETE");
		MLs = CEID_LIST::PJALERTCOMPLETE;
		break;
	case alJOB_WAITITNG_FOR_START:
		strscpy(Data, sizeof(Data), "WAITING FOR START");
		MLs = CEID_LIST::PJALERTWAITINGFORSTART;
		break;
	case alJOB_WAITITNG_FOR_MATL:
		strscpy(Data, sizeof(Data), "WAITING FOR MATL");
		MLs = CEID_LIST::PJALERTWAITINGFORMATL;
		break;
	};
	if (MLs) {
		CEvt* pEvt;

		pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->PRJobState = GetState();
		strscpy(pEvt->ObjID, OBJIDLEN, m_ObjID);
		CString CjID;
		if (CJobMgr.CtrlJobIDFromPj(m_ObjID, CjID)) {
			strscpy(pEvt->CJobID, CRJIDLEN, CjID.GetBuffer(0));
		}

		CSharedMemQue pQue;
		pEvt->Set(PRJOB_ALERT, MLs);
		pQue.OpenMemory("EVTMSG");
		pGDev->SetActiveCtrlJob(pEvt->CJobID);
		pGDev->SetActivePrJob(m_ObjID);
		pGDev->SetActivePort(m_PortNo);
		pQue.WriteQue(pEvt);
		pGDev->SetActiveCtrlJob("");
		pGDev->SetActivePrJob("");
		pQue.CloseMemory();
		Log(Data);
	}
}

void CPRJob::WriteStartData413() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdPCJOBEXEC;
	SM.Pj.act = SPcJobExec::ADD;
	strscpy(SM.Pj.PrJID, PRJIDLEN, m_ObjID);
	strscpy(SM.Pj.CarrierID, OBJIDLEN, m_PRMtlName); // PRJOBID
	strscpy(SM.Pj.RecipeName, RECIPEIDLEN, m_Recipe.c_str());
	int i;
	for (i = 0; i < 25; i++) {
		SM.Pj.slot[i].slotno = 0;
		SM.Pj.CtMap[i].Preset();
	}
	// Added size check [1/24/2013 FSM413]
	short sz = short(m_Slots.size());
	if (sz > 25) {
		sz = 25;
	}
	for (i = 0; i < /*25*/sz; i++) {
		short sn = m_Slots[i];	// m_Slots[] may not have 25 elements [1/24/2013 FSM413]
		if ((sn >= 0) && (sn < 26)) {
			//20120920 SCOTT  EDIT SM.Pj.slot[sn].slotno = sn; // 1 offset [8/30/2010 XPMUser]  //20120920 SLOT NO OK
			SM.Pj.slot[sn].slotno = sn; // 1 offset [8/30/2010 XPMUser]  //20120920 SLOT NO OK
		}
		// 		else
		// 		{
		// 			SM.Pj.slot[i].slotno = 0; // Invalid slot number specified
		// 		}
	}

	CString txt;
	char CarId[70] =
	{
		0
	};
	SM.cmd = CSMOL::cmdCARRIERACTION;
	SM.Ca.act = CarrierAction::STARTPROCESSING;
	strscpy(SM.Ca.CarrierID, OBJIDLEN, m_PRMtlName);
	strscpy(SM.Ca.PrJobID, PRJIDLEN, m_ObjID);
	SM.Ca.PortNo = m_PortNo;
	if (pGDev->pGEMObjs->pSecsSvr) {
		pGDev->pGEMObjs->pSecsSvr->CarrierActionsExec(SM);
	}
}

BOOL CPRJob::SetMF(BYTE MF) {
	this->MF = MF;
	return TRUE;
}

BOOL CPRJob::IsExecuting() {
	switch (JobState) {
	case CPRJob::SETTING_UP:
	case CPRJob::WAITING_FOR_START:
	case CPRJob::PROCESSING:
	case CPRJob::PROCESS_COMPLETE:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CPRJob::IsPaused() {
	switch (JobState) {
	case CPRJob::PAUSED:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CPRJob::StopProcessing() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdCARRIERACTION;
	SM.Ca.act = CarrierAction::STOPPROCESSING;
	strscpy(SM.Ca.CarrierID, OBJIDLEN, m_PRMtlName);
	SM.Ca.PortNo = m_PortNo;
	if (pGDev->pGEMObjs->pSecsSvr) {
		if (pGDev->pGEMObjs->pSecsSvr->CarrierActionsExec(SM)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CPRJob::PauseProcessing() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdCARRIERACTION;
	SM.Ca.act = CarrierAction::PAUSEPROCESSING;
	strscpy(SM.Ca.CarrierID, OBJIDLEN, m_PRMtlName);
	SM.Ca.PortNo = m_PortNo;
	if (pGDev->pGEMObjs->pSecsSvr) {
		if (pGDev->pGEMObjs->pSecsSvr->CarrierActionsExec(SM)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CPRJob::ResumeProcessing() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdCARRIERACTION;
	SM.Ca.act = CarrierAction::RESUMEPROCESSING;
	strscpy(SM.Ca.CarrierID, OBJIDLEN, m_PRMtlName);
	SM.Ca.PortNo = m_PortNo;
	if (pGDev->pGEMObjs->pSecsSvr) {
		if (pGDev->pGEMObjs->pSecsSvr->CarrierActionsExec(SM)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CPRJob::IsWaitingForStart() {
	return JobState == CPRJob::WAITING_FOR_START;
}

BOOL CPRJob::IsAutostart() {
	return m_PRProcessStart;
}

BOOL CPRJob::AbortProcessing() {
	CSMOL SM;
	SM.cmd = CSMOL::cmdCARRIERACTION;
	SM.Ca.act = CarrierAction::ABORTPROCESSING;
	strscpy(SM.Ca.CarrierID, OBJIDLEN, m_PRMtlName);
	strscpy(SM.Ca.PrJobID, OBJIDLEN, m_ObjID);
	SM.Ca.PortNo = m_PortNo;
	if (pGDev->pGEMObjs->pSecsSvr) {
		if (pGDev->pGEMObjs->pSecsSvr->CarrierActionsExec(SM)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CPRJob::Dump(std::string& Data) {
	std::vector < std::string > IdList;
	std::string Temp;
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

void CPRJob::GetAttrIDs(std::vector < std::string >& IdList) {
	IdList.clear();
	IdList.push_back("ObjType");
	IdList.push_back("ObjID");
	IdList.push_back("PRJobState");
	IdList.push_back("PRJobStateList");
	IdList.push_back("PRMtlName");
	IdList.push_back("PRMtlType");
	IdList.push_back("PRProcessStart");
	IdList.push_back("ScanLine");
	IdList.push_back("ScanNo");
	IdList.push_back("PortNo");
	IdList.push_back("PRMtlNameList");
	IdList.push_back("PRMtlOrder");
	IdList.push_back("PRRecipe");
	IdList.push_back("PRRecipeMethod");
	IdList.push_back("RecID");
	IdList.push_back("Timestamp");
	IdList.push_back("PauseEvent");
	IdList.push_back("RecVariableList");
}

CString CPRJob::GetStartMethodStr() {
	if (m_PRProcessStart) {
		return "Auto Start";
	}
	return "Manual Start";
}

CString CPRJob::GetJobStateStr() {
	return GetJobStateStr(JobState);
}

CString CPRJob::GetJobStateStr(CPRJob::PRJOBSTATE state) {
	switch (state) {
	case QUEUED_POOLED:
		return "QUEUED";
		break;
	case SETTING_UP:
		return "SETTING UP";
		break;
	case WAITING_FOR_START:
		return "WAITING FOR START";
		break;
	case PROCESSING:
		return "PROCESSING";
		break;
	case PROCESS_COMPLETE:
		return "PROCESSING COMPLETE";
		break;
	case EXECUTING:
		return "EXECUTING";
		break;
	case PAUSING:
		return "PAUSING";
		break;
	case PAUSED:
		return "PAUSED";
		break;
	case ABORTING:
		return "ABORTING";
		break;
	case STOPPING:
		return "STOPPING";
		break;
	case NO_STATE:
		return "NO STATE";
		break;
	}
	return "";
}

BOOL CPRJob::IsActive() {
	if (JobState == CPRJob::QUEUED_POOLED) {
		return FALSE;
	}
	if (JobState == CPRJob::NO_STATE) {
		return FALSE;
	}
	return TRUE;
}

BOOL CPRJob::Dequeue() {
	if (CanDequeue()) {
		StateChange(CPRJob::NO_STATE);
		AlertNotify(CPRJob::alJOB_COMPLETE);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::CanDequeue() {
	if (JobState == CPRJob::QUEUED_POOLED) {
		return TRUE;
	}
	return FALSE;
}

void CPRJob::SetWaferParams(ParamNew& Param, int arryno) {
	if (arryno > 25) {
		ASSERT(0);
		return;
	}
	m_Param[arryno].Waferthk = Param.WThick;
	m_Param[arryno].Filmthk = Param.FThick;
	m_Param[arryno].SlotNo = Param.SlotNo;
	m_Param[arryno].ScanNo = Param.ScanNo; //  [9/10/2007 LYF]
}

BOOL CPRJob::CanAbort() {
	if (IsActive()) {
		return TRUE;
	}
	// for compatibility with CCS [9/6/2007 LYF]
	if (JobState == CPRJob::QUEUED_POOLED) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::CanStop() {
	if (IsExecuting()) {
		return TRUE;
	}
	return FALSE;
}

CPRJob::PRJOBSTATE CPRJob::GetState() {
	return JobState;
}

void CPRJob::SetState(CPRJob::PRJOBSTATE State) {
	JobState = State;
}

BOOL CPRJob::CanCancel() {
	switch (JobState) {
	case CPRJob::QUEUED_POOLED:
	case CPRJob::NO_STATE:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CPRJob::CanResume() {
	if (IsPaused()) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Cancel() {
	if (CanCancel()) {
		StateChange(CPRJob::NO_STATE);
		AlertNotify(CPRJob::alCOMPLETED_PROCESSING);
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::CanPause() {
	if (IsPausing()) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Start() {
	ErrorMsg = "";
	if (CanSetup()) {
		if (Setup()) {
			return TRUE;
		}
		ErrorMsg = "PJob cannot transit to PROCESSING or WAITINGFORSTART";
		return FALSE;
	}
	ErrorMsg = "PJob cannot start";
	return FALSE;
}

BOOL CPRJob::CanSetup() {
	if (IsQueued()) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::IsQueued() {
	if (GetState() == CPRJob::QUEUED_POOLED) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Setup() {
	StateChange(CPRJob::SETTING_UP);
	AlertNotify(CPRJob::alSETTING_UP);
	if (!IsAutostart()) {
		StateChange(CPRJob::WAITING_FOR_START);
		AlertNotify(CPRJob::alJOB_WAITITNG_FOR_START); //200619
		return TRUE;
	}
	else {
		StateChange(CPRJob::PROCESSING);
		AlertNotify(CPRJob::alPROCESSING);
		WriteStartData413();
		return TRUE;
	}
	return TRUE;
}

BOOL CPRJob::IsNoState() {
	if (GetState() == CPRJob::NO_STATE) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Stopped() {
	StateChange(CPRJob::NO_STATE);
	AlertNotify(CPRJob::alJOB_COMPLETE);
	return TRUE;
}

void CPRJob::SetWaferStartInfo(WaferStartInfo& Param, int arryno) {
	if (arryno > 25) {
		ASSERT(0);
		return;
	}
	m_Param[arryno].Waferthk = Param.Waferthk;
	m_Param[arryno].Filmthk = Param.Filmthk;
	m_Param[arryno].SlotNo = Param.SlotNo;
	strscpy(m_Param[arryno].LotID, LOTIDLEN, Param.LotID);
	strscpy(m_Param[arryno].Waferid, OBJIDLEN, Param.Waferid);
}

CString CPRJob::GetJobParamStr(int i) {
	if ((i < 0) || (i > 25)) {
		return "";
	}
	if (!m_Param[i].SlotNo) {
		return "";
	}
	CString str;

	str.Format("SN=%d  FT=%.4f", m_Param[i].SlotNo, m_Param[i].Filmthk);
	return str;
}

BOOL CPRJob::CanActive() {
	if (JobState == CPRJob::QUEUED_POOLED) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::IsProcessing() {
	if (JobState == CPRJob::PROCESSING) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::IsSettingUp() {
	if (JobState == CPRJob::SETTING_UP) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::CanProcess() {
	switch (JobState) {
	case CPRJob::SETTING_UP:
	case CPRJob::WAITING_FOR_START:
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::IsPausing() {
	if (JobState == CPRJob::PAUSING) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::IsAborting() {
	if (JobState == CPRJob::ABORTING) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::CanStart() {
	switch (JobState) {
	case CPRJob::QUEUED_POOLED:
	case CPRJob::WAITING_FOR_START:
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CPRJob::IsStopping() {
	if (JobState == CPRJob::STOPPING) {
		return TRUE;
	}
	return FALSE;
}

BOOL CPRJob::Aborted() {
	StateChange(CPRJob::NO_STATE);
	AlertNotify(CPRJob::alJOB_COMPLETE);
	return TRUE;
}

BOOL CPRJob::GetCarrierAndSM(CString& CarrierID, CString& Slotmap) {
	BOOL bSMOk, bCIDOk;
	bSMOk = bCIDOk = FALSE;
	if ((MAXCAPACITYLEN <= Slotmap.GetLength())) {
		int nIndex;
		for (nIndex = 0; nIndex < int(m_Slots.size()); nIndex++) {
			if (m_Slots[nIndex] < MAXCAPACITYLEN) {
				Slotmap.SetAt(nIndex, CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED));
				bSMOk = TRUE;
			}
		}
		for (nIndex = 0; nIndex < 25; nIndex++) {
			if (m_Param[nIndex].SlotNo < MAXCAPACITYLEN) {
				Slotmap.SetAt(nIndex, CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED));
				bSMOk = TRUE;
			}
		}
	};
	CarrierID = m_PRMtlName;
	if (CarrierID.GetLength()) {
		bCIDOk = TRUE;
	}
	return ((bSMOk) && (bCIDOk));
}

BOOL CPRJob::WFS2Start() {
	BOOL bRes = FALSE;
	if (IsWaitingForStart()) {
		StateChange(CPRJob::PROCESSING);
		AlertNotify(CPRJob::alPROCESSING);
		WriteStartData413();
		bRes = TRUE;
	}
	return bRes;
}

void CPRJob::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "PRJob");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}