#include "stdafx.h"
#include "strSOp.h"

#include "128/GEM/ObjectsContainer.h"
#include "Evt.h"
#include "GlobalDeviceContainer.h"
#include "ONLINE/GEM/CtrlJobMgr.h"
#include "ONLINE/GEM/PRJobMgr.h"
#include "ONLINE/general_utils.h"
#include "ONLINE/INC/E5-ErrCodes.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/TCPIP/HsmsParaMeter.h"
#include "SharedMemQue.h"
#include "SRC/DOSUtil.h"

#include "DataBank.h"

BOOL bCjValid = FALSE;

//
// CDataBank m_DataBank;

bool operator ==(const CCarrierObj& LHS, const CCarrierObj& RHS) {
	return (0 == strcmp(LHS.ObjID, RHS.ObjID));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataBank::CDataBank() {
	hMutex_Abort = NULL;
	hMutex_Stop = NULL;

	ClearAll();

	bAutoConnect = FALSE;
	bAutoConnect = DosUtil.GetAutoConnect(bAutoConnect);
	DosUtil.WriteAutoConnect(bAutoConnect);

	bAutoOnline = FALSE;
	bAutoOnline = DosUtil.GetAutoOnline(bAutoOnline);
	DosUtil.WriteAutoOnline(bAutoOnline);

	bCJJobControl = DosUtil.GetJobOption();
	DosUtil.WriteJobOption(bCJJobControl);

	pGDev->pGEMObjs->Cms[0].PortID = 1;
	pGDev->pGEMObjs->Cms[0].SetLPObjID(1);
	pGDev->pGEMObjs->Cms[1].PortID = 2;
	pGDev->pGEMObjs->Cms[1].SetLPObjID(2);
	pGDev->pGEMObjs->Cms[2].PortID = 3;
	pGDev->pGEMObjs->Cms[2].SetLPObjID(3);
	pGDev->pGEMObjs->Cms[3].PortID = 4;
	pGDev->pGEMObjs->Cms[3].SetLPObjID(4);

	//	m_EStatus.LineMode = EqpStatus::OFFLINEEQP;
	//	m_EStatus.OpMode = EqpStatus::IDLE;
}

CDataBank::~CDataBank() {}

void CDataBank::RemovePRJobInCtrJob(char* pObjid) {
	CCtrlJob* p = CJobMgr.GetCJob(pObjid);
	if (!p) {
		ASSERT(0);
		return;
	}
	p->RemoveAllPRJob();
}

void CDataBank::ClearAll() {
	ActivePort = 0;
	ActiveSubst = 0;
	pGDev->pGEMObjs->Cms[0].ClearAll();
	pGDev->pGEMObjs->Cms[1].ClearAll();
	pGDev->pGEMObjs->Cms[2].ClearAll();
	pGDev->pGEMObjs->Cms[3].ClearAll();

	memset(LastSM1, 0, SLOTMAPLEN + 1);
	memset(LastSM2, 0, SLOTMAPLEN + 1);
}

void CDataBank::GetQueuedCJobsList(vector<string>& CjList) {
	CJobMgr.GetQueuedCJobList(CjList);
}

// Must be called after Stop or Abort
void CDataBank::CleanUp() {
	if (hMutex_Abort != NULL) {
		::ReleaseMutex(hMutex_Abort);
		hMutex_Abort = NULL;
	}
	if (hMutex_Stop != NULL) {
		::ReleaseMutex(hMutex_Stop);
		hMutex_Stop = NULL;
	}
}

//  [9/12/2007 LYF]
//int CDataBank::GetAvailableQSpace()
//{
//	return CJobMgr.GetJobSpace();
//}

void CDataBank::GetCarrierIds(vector<string>& CIds) {
	//CString ObjID = "ObjID";
	char CarrierID[126] = { 0 };
	string Temp;
	//  Yuen [7/4/2002/23:45]
	//	BYTE NumPorts = sizeof(m_Carrier)/sizeof(Cms[0]);
//	BYTE NumPorts = m_EStatus.MaxLP;
	for (int nIndex = 0; nIndex < Var.nLoadPort; nIndex++) {
		if (pGDev->pGEMObjs->Cms[nIndex].CaObj.IDStatus == CCarrierObj::IDVERIFICATIONOK) {
			pGDev->pGEMObjs->Cms[nIndex].CaObj.GetAttr("ObjID", CarrierID, sizeof(CarrierID));
			if (0 < strlen(CarrierID)) {
				Temp = CarrierID;
				CIds.push_back(Temp);
			}
		}
		memset(CarrierID, 0, sizeof(CarrierID));
	}
}

BOOL CDataBank::IsCarrierPresent(const string& Carrier) {
	vector<string> CaIds;
	GetCarrierIds(CaIds);
	for (int nIndex = 0; nIndex<int(CaIds.size()); nIndex++) {
		if (Carrier == CaIds[nIndex]) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDataBank::IsCarrierPresent(const char* Carrier) {
	if (!Carrier) {
		ASSERT(0);
		return FALSE;
	}
	string TempID = Carrier;
	return(IsCarrierPresent(TempID));
}

CPRJob* CDataBank::GetPrjobObj(CString& PrJobID) {
	return PJobMgr.GetPRJob(PrJobID);
}

CPRJob* CDataBank::GetPrjobObj(string& PrJobID) {
	CString Temp = PrJobID.c_str();
	return PJobMgr.GetPRJob(Temp);
}

CCtrlJob* CDataBank::GetCtrlJobObj(const char* CtrlJobID) {
	CString Temp = CtrlJobID;
	return CJobMgr.GetCJob(Temp);
}

BOOL CDataBank::CarrierPresent(string& ProcessJobName) {
	CString Temp = ProcessJobName.c_str();
	vector<string > CarrierIdNames;
	string CarrierName;

	CPRJob* pPrjob = GetPrjobObj(Temp);
	if (NULL != pPrjob) {
		CarrierName = pPrjob->m_PRMtlName;
		if (IsCarrierPresent(CarrierName)) {
			return TRUE;
		}
	}
	return FALSE;
}

short CDataBank::GetPortNumber(string& CarrierName) {
	ASSERT(0 < CarrierName.size());
	char CarrierID[126] = { 0 };
	string Temp;
	//  Yuen [7/4/2002/23:45]
	for (int nIndex = 0; nIndex < Var.nLoadPort; nIndex++) {
		if (pGDev->pGEMObjs->Cms[nIndex].CaObj.IDStatus == CCarrierObj::IDVERIFICATIONOK) {
			pGDev->pGEMObjs->Cms[nIndex].CaObj.GetAttr("ObjID", CarrierID, sizeof(CarrierID));
			Temp = CarrierID;
			if (Temp == CarrierName) {
				return nIndex + 1;
			}
		}
		memset(CarrierID, 0, sizeof(CarrierID));
	}
	return -1;
}

void CDataBank::AddPortGroup(PortGroup& PGrp) {
	// Check if the port group is already exists
	for (int nIndex = 0; nIndex<int(PGrpList.size()); nIndex++) {
		if (PGrpList[nIndex].PGPNAME == PGrp.PGPNAME) {
			// If so replace the old with new
			PGrpList[nIndex].PortList = PGrp.PortList;
			return;
		}
	}
	// else Add at end of list
	PGrpList.push_back(PGrp);
}

bool CDataBank::GetAssociatedPorts(const string& PGrpName, vector<BYTE>& PortList) {
	for (int nIndex = 0; nIndex<int(PGrpList.size()); nIndex++) {
		if (PGrpList[nIndex].PGPNAME == PGrpName) {
			// Found the group
			PortList = PGrpList[nIndex].PortList;
			return true;
		}
	}
	// Group was not defined
	return false;
}

void CDataBank::ClearJobs() {
	// Clear Process Jobs
	CJobMgr.ClearAllJobs();
	PJobMgr.ClearAllJobs();
	Log("All Process Jobs & control Jobs cleared");
}

void CDataBank::GetCurPrjobDetails(short PortNo, char* pjob, CPRJob::PRJOBSTATE* State) {
	if (!State || !pjob) {
		ASSERT(0);
		return;
	}
	CStringArray JList;
	PJobMgr.GetActiveJobList(JList);
	if (!JList.GetSize()) {
		return;
	}
	for (int i = 0; i < JList.GetSize(); i++) {
		CPRJob* p = PJobMgr.GetPRJob(pjob);
		if (!p) {
			ASSERT(0);
			return;
		}
		if (p->m_PortNo == PortNo) {
			strcpy(pjob, p->m_ObjID);
			*State = p->JobState;
		}
	}
}

BOOL CDataBank::RemoveCtrlJob(CCtrlJob* pCJob, BOOL bSave) {
	return CJobMgr.RemoveCJob(pCJob);
}

CCtrlJob* CDataBank::GetCompletedCjobObj(char* pObjid) {
	return CJobMgr.GetCmplCJob(pObjid);
}

int CDataBank::CanPerformService(string& Service, string& CarrierID, BYTE PortID, vector<string>& Attributes) {
	CString strService;

	strService = Service.c_str();

	if (0 == strService.CompareNoCase("Bind")) {
		return CanBindCarrier(CarrierID, PortID, Attributes);
	}
	else if (0 == strService.CompareNoCase("CancelAllCarrierOut")) {
		return CanCancelAllCarrierOut(Attributes);
	}
	else if (0 == strService.CompareNoCase("CancelBind")) {
		if (CarrierID.empty()) {
			return CanCancelBindPort(PortID, Attributes);
		}
		else {
			return CanCancelBindCarrier(CarrierID, Attributes);
		}
	}
	else if (0 == strService.CompareNoCase("CancelCarrier")) {
		return CanCancelCarrier(CarrierID, Attributes);
	}
	else if (0 == strService.CompareNoCase("CancelCarrierAtPort")) {
		return CanCancelCarrierAtPort(PortID, Attributes);
	}
	else if (0 == strService.CompareNoCase("CancelCarrierNotification")) {
		return CanCancelCarrierNotification(CarrierID, Attributes);
	}
	else if (0 == strService.CompareNoCase("CancelCarrierOut")) {
		return CanCancelCarrierOut(CarrierID, Attributes);
	}
	else if (0 == strService.CompareNoCase("CarrierIn")) {
		return CanCarrierIn(CarrierID, Attributes);
	}
	else if (0 == strService.CompareNoCase("CarrierNotification")) {
		return CanCarrierNotification(CarrierID, Attributes);
	}
	else if (0 == strService.CompareNoCase("CarrierOut")) {
		return CanCarrierOut(CarrierID, Attributes);
	}
	else if (0 == strService.CompareNoCase("ProceedWithCarrier")) {
		return CanProceedWithCarrierID(PortID, CarrierID, Attributes);
	}
	else {
		return INVALID_CMD;
	}
}

int CDataBank::CanPerformPortAction(int PortID, const string& Action) {
	CString strAction = Action.c_str();
	if (0 == strAction.CompareNoCase("ReserveAtPort")) {
		return CanReserveAtPort(PortID);
	}
	else if (0 == strAction.CompareNoCase("CancelReservationAtPort")) {
		return CanCancelReservationAtPort(PortID);
	}
	else {
		return ACK_CMD_OK;
	}
}

int CDataBank::CanBindCarrier(string& CarrierID, BYTE PortID, vector<string>& Attributes) {
	if ((CarrierID.empty()) || (PortID > Var.nLoadPort) || (0 <= PortID)) {
		return INVALID_DATA_OR_ARG;
	}
	CCms* pCms = &pGDev->pGEMObjs->Cms[PortID - 1];
	if ((pCms->LPObj.GetAccessMode() != CLPObj::MANUAL) && (pCms->LPObj.GetAccessMode() != CLPObj::AUTO)) {
		return REJECT_INVALID_STATE;
	}
	if (!pCms->IsReadyToLoad()) {
		return REJECT_INVALID_STATE;
	}
	if (pCms->LPObj.Association != CLPObj::NOTASSOCIATED) {
		return REJECT_INVALID_STATE;
	}
	return ACK_CMD_OK;
}

int CDataBank::CanCancelAllCarrierOut(vector<string>& Attributes) {
	return RESERVED_CMD_NOT_SUPPORTED;
}

int CDataBank::CanCancelBindPort(BYTE PotID, vector<string>& Attributes) {
	short PortID = PotID;
	if ((PortID > Var.nLoadPort) || (PortID < 1)) {
		return INVALID_DATA_OR_ARG;
	}
	CCms* pCms = &pGDev->pGEMObjs->Cms[PortID - 1];
	CLPObj::TRANSFERSTATUS LP_TS = pCms->LPObj.GetTransferStatus();
	CLPObj::ACCESSMODE     LP_AC = pCms->LPObj.GetAccessMode();
	CLPObj::ASSOCIATION    LP_AS = pCms->LPObj.Association;
	CLPObj::CARRIERLOC     LP_CL = pCms->LPObj.CarrierLoc;

	CCarrierObj::ACCESSINGSTATUS CO_AC = pCms->CaObj.AccessingStatus;
	CCarrierObj::IDSTATUS        CO_IS = pCms->CaObj.IDStatus;
	CCarrierObj::SLOTMAPSTATUS   CO_SS = pCms->CaObj.SlotmapStatus;

	if (LP_TS != CLPObj::READYTOLOAD) {
		return REJECT_INVALID_STATE;
	}
	if (CO_IS != CCarrierObj::IDNOTREAD) {
		return REJECT_INVALID_STATE;
	}
	if (CO_SS != CCarrierObj::SMNOTREAD) {
		return REJECT_INVALID_STATE;
	}
	if (LP_AS != CLPObj::ASSOCIATED) {
		return REJECT_INVALID_STATE;
	}
	if ((LP_AC != CLPObj::MANUAL) && (LP_AC != CLPObj::AUTO)) {
		return REJECT_INVALID_STATE;
	}
	return ACK_CMD_OK;
}

int CDataBank::CanCancelBindCarrier(string& CarrierID, vector<string>& Attributes) {
	return CanCancelBindPort(GetPortID(CarrierID), Attributes);
}

int CDataBank::CanCancelCarrier(string& CarrierID, vector<string>& Attributes) {
	return CanCancelCarrierAtPort(GetPortID(CarrierID), Attributes);
}

int CDataBank::CanCancelCarrierAtPort(BYTE PotID, vector<string>& Attributes) {
	short PortID = PotID;
	if ((PortID > Var.nLoadPort) || (PortID < 1)) {
		return INVALID_DATA_OR_ARG;
	}
	CLPObj* pLPO = &pGDev->pGEMObjs->Cms[PortID - 1].LPObj;
	if (!pLPO) {
		return INVALID_DATA_OR_ARG;
	}
	CLPObj::TRANSFERSTATUS LP_TS = pLPO->GetTransferStatus();
	CLPObj::ACCESSMODE     LP_AC = pLPO->GetAccessMode();
	CLPObj::ASSOCIATION    LP_AS = pLPO->Association;
	CLPObj::CARRIERLOC     LP_CL = pLPO->CarrierLoc;

	CCarrierObj* pCO = &pGDev->pGEMObjs->Cms[PortID - 1].CaObj;
	if (!pCO) {
		return INVALID_DATA_OR_ARG;
	}
	CCarrierObj::ACCESSINGSTATUS CO_AC = pCO->AccessingStatus;
	CCarrierObj::IDSTATUS        CO_IS = pCO->IDStatus;
	CCarrierObj::SLOTMAPSTATUS   CO_SS = pCO->SlotmapStatus;

	if (CJobMgr.IsCarrierInProcess(pCO->ObjID)) {
		return CANNOT_PERFORM_NOW;
	}

	if (LP_TS != CLPObj::TRANSFERBLOCKED) {
		return REJECT_INVALID_STATE;
	}
	if ((CO_IS == CCarrierObj::IDNOSTATE) || (CO_IS == CCarrierObj::igIDSTATUS)) {
		return REJECT_INVALID_STATE;
	}

	if ((CO_SS == CCarrierObj::SMNOSTATE) || (CO_SS == CCarrierObj::igSMSTATUS)) {
		return REJECT_INVALID_STATE;
	}

	if ((LP_AC != CLPObj::MANUAL) && (LP_AC != CLPObj::AUTO)) {
		return REJECT_INVALID_STATE;
	}
	return ACK_CMD_OK;
}

int CDataBank::CanCancelCarrierNotification(string& CarrierID, vector<string>& Attributes) {
	int PortID = GetPortID(CarrierID);
	if (-1 == PortID) {
		for (list<CCarrierObj>::iterator Iter = Notified.begin(); Iter != Notified.end(); Iter++) {
			if (0 == CarrierID.compare(Iter->ObjID)) {
				Notified.remove(*Iter);
				return ACK_CMD_OK;
			}
		}
		return INVALID_DATA_OR_ARG;
	}
	CLPObj::TRANSFERSTATUS LP_TS = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.GetTransferStatus();
	CLPObj::ACCESSMODE     LP_AC = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.GetAccessMode();
	CLPObj::ASSOCIATION    LP_AS = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.Association;
	CLPObj::CARRIERLOC     LP_CL = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.CarrierLoc;

	CCarrierObj::ACCESSINGSTATUS CO_AC = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.AccessingStatus;
	CCarrierObj::IDSTATUS        CO_IS = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.IDStatus;
	CCarrierObj::SLOTMAPSTATUS   CO_SS = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.SlotmapStatus;

	if ((LP_TS == CLPObj::igTRANSFERSTATUS) ||
		(LP_TS == CLPObj::tsNOSTATE)) {
		return REJECT_INVALID_STATE;
	}
	if ((CO_IS == CCarrierObj::IDVERIFICATIONFAIL) ||
		(CO_IS == CCarrierObj::IDVERIFICATIONOK) ||
		(CO_IS == CCarrierObj::IDNOSTATE) ||
		(CO_IS == CCarrierObj::igIDSTATUS)) {
		return REJECT_INVALID_STATE;
	}
	if ((CO_SS == CCarrierObj::SMNOSTATE) || (CO_SS == CCarrierObj::igSMSTATUS)) {
		return REJECT_INVALID_STATE;
	}
	if ((LP_AC != CLPObj::MANUAL) && (LP_AC != CLPObj::AUTO)) {
		return REJECT_INVALID_STATE;
	}
	if (LP_AS != CLPObj::NOTASSOCIATED) {
		return REJECT_INVALID_STATE;
	}
	return ACK_CMD_OK;
}

int CDataBank::CanCancelCarrierOut(string& CarrierID, vector<string>& Attributes) {
	int PortID = GetPortID(CarrierID);
	if (-1 == PortID) {
		return INVALID_DATA_OR_ARG;
	}
	CLPObj::TRANSFERSTATUS LP_TS = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.GetTransferStatus();
	CLPObj::ACCESSMODE     LP_AC = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.GetAccessMode();
	CLPObj::ASSOCIATION    LP_AS = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.Association;
	CLPObj::CARRIERLOC     LP_CL = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.CarrierLoc;

	CCarrierObj::ACCESSINGSTATUS CO_AC = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.AccessingStatus;
	CCarrierObj::IDSTATUS        CO_IS = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.IDStatus;
	CCarrierObj::SLOTMAPSTATUS   CO_SS = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.SlotmapStatus;

	if ((LP_TS != CLPObj::TRANSFERBLOCKED) && (LP_TS != CLPObj::READYTOUNLOAD)) {
		return REJECT_INVALID_STATE;
	}
	if ((CO_IS == CCarrierObj::IDNOSTATE) || (CO_IS == CCarrierObj::igIDSTATUS)) {
		return REJECT_INVALID_STATE;
	}
	if ((CO_SS == CCarrierObj::SMNOSTATE) || (CO_SS == CCarrierObj::igSMSTATUS)) {
		return REJECT_INVALID_STATE;
	}
	if ((LP_AC != CLPObj::MANUAL) && (LP_AC != CLPObj::AUTO)) {
		return REJECT_INVALID_STATE;
	}
	if (LP_AS != CLPObj::ASSOCIATED) {
		return REJECT_INVALID_STATE;
	}
	return ACK_CMD_OK;
}

int CDataBank::CanCancelReservationAtPort(BYTE PotID) {
	short PortID = PotID;

	if ((PortID > Var.nLoadPort) || (PortID < 1)) {
		return INVALID_DATA_OR_ARG;
	}
	CCms* pCms = &pGDev->pGEMObjs->Cms[PortID - 1];
	CLPObj::TRANSFERSTATUS LP_TS = pCms->LPObj.GetTransferStatus();
	CLPObj::ACCESSMODE     LP_AC = pCms->LPObj.GetAccessMode();
	CLPObj::ASSOCIATION    LP_AS = pCms->LPObj.Association;
	CLPObj::CARRIERLOC     LP_CL = pCms->LPObj.CarrierLoc;

	CCarrierObj::ACCESSINGSTATUS CO_AC = pCms->CaObj.AccessingStatus;
	CCarrierObj::IDSTATUS        CO_IS = pCms->CaObj.IDStatus;
	CCarrierObj::SLOTMAPSTATUS   CO_SS = pCms->CaObj.SlotmapStatus;

	if (LP_TS != CLPObj::READYTOLOAD) {
		return REJECT_INVALID_STATE;
	}
	if ((LP_AC != CLPObj::MANUAL) && (LP_AC != CLPObj::AUTO)) {
		return REJECT_INVALID_STATE;
	}
	if (LP_AS != CLPObj::NOTASSOCIATED) {
		return REJECT_INVALID_STATE;
	}
	return ACK_CMD_OK;
}

int CDataBank::CanCarrierIn(string& CarrierID, vector<string>& Attributes) {
	return RESERVED_CMD_NOT_SUPPORTED;
}

int CDataBank::CanCarrierNotification(string& CarrierID, vector<string>& Attributes) {
	if (IsCarrierPresent(CarrierID)) {
		return INVALID_DATA_OR_ARG;
	}
	return ACK_CMD_OK;
}

int CDataBank::CanCarrierOut(string& CarrierID, vector<string>& Attributes) {
	return RESERVED_CMD_NOT_SUPPORTED;
}

int CDataBank::CanProceedWithCarrierID(BYTE PotID, string& CarrierID, vector<string>& Attributes) {
	int PortID = GetPortID(CarrierID);
	if (PortID == -1) {
		switch (PotID) {
		case 1:
			PortID = 1;
			break;
		case 2:
			PortID = 2;
			break;
		}
	}

	if (PortID == -1) {
		int i;
		for (i = 1; i <= Var.nLoadPort; i++) {
			CString tmp;
			tmp.Format("XUNKNOWNX%d", i);
			string str = tmp;
			PortID = GetPortID(str);
			if (PortID != -1) {
				strcpy(pGDev->pGEMObjs->Cms[PortID - 1].CaObj.ObjID, CarrierID.c_str());
				break;
			}
		}
		if (i > Var.nLoadPort) return INVALID_DATA_OR_ARG;
	}
	if (pGDev->pGEMObjs->Cms[PortID - 1].CaObj.IDStatus == CCarrierObj::IDVERIFICATIONOK) {
		if (pGDev->pGEMObjs->Cms[PortID - 1].CaObj.SlotmapStatus != CCarrierObj::SMWAITINGFORHOST) {
			return REJECT_INVALID_STATE;
		}
	}
	CLPObj::TRANSFERSTATUS LP_TS = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.GetTransferStatus();
	CLPObj::ACCESSMODE     LP_AC = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.GetAccessMode();
	CLPObj::ASSOCIATION    LP_AS = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.Association;
	CLPObj::CARRIERLOC     LP_CL = pGDev->pGEMObjs->Cms[PortID - 1].LPObj.CarrierLoc;

	CCarrierObj::ACCESSINGSTATUS CO_AC = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.AccessingStatus;
	CCarrierObj::IDSTATUS        CO_IS = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.IDStatus;
	CCarrierObj::SLOTMAPSTATUS   CO_SS = pGDev->pGEMObjs->Cms[PortID - 1].CaObj.SlotmapStatus;

	if (LP_TS != CLPObj::TRANSFERBLOCKED) {
		return REJECT_INVALID_STATE;
	}
	if ((CO_IS == CCarrierObj::IDNOSTATE) || (CO_IS == CCarrierObj::igIDSTATUS)) {
		return REJECT_INVALID_STATE;
	}
	if ((CO_SS == CCarrierObj::SMNOSTATE) || (CO_SS == CCarrierObj::igSMSTATUS)) {
		return REJECT_INVALID_STATE;
	}
	if (LP_AC == CLPObj::amNOSTATE) {
		return REJECT_INVALID_STATE;
	}
	if (LP_AS != CLPObj::ASSOCIATED) {
		return REJECT_INVALID_STATE;
	}
	return ACK_CMD_OK;
}

int CDataBank::CanReserveAtPort(BYTE PortID) {
	return CanCancelReservationAtPort(PortID);
}

int CDataBank::GetPortID(const string& CarrierID) {
	for (int nIndex = 0; nIndex < Var.nLoadPort; nIndex++) {
		if (0 == CarrierID.compare(pGDev->pGEMObjs->Cms[nIndex].CaObj.ObjID)) {
			return nIndex + 1;
		}
	}
	return -1;
}

string CDataBank::GetCarrierID(BYTE PotID) {
	short PortID = PotID;
	if ((PortID > Var.nLoadPort) || (PortID < 1)) {
		return "";
	}
	CCms* pCms = &pGDev->pGEMObjs->Cms[PortID - 1];
	if (!pCms) {
		return "";
	}
	return pCms->CaObj.ObjID;
}

int CDataBank::PerformE87Service(const char* szCarrierID, const char* szAction,
	vector<string>& AttrIDs, vector<SecsItem*>& AttrValues, SecsItemList* pErrorList, BYTE PotID) {
	short PortID = PotID;

	if (!(PortID > Var.nLoadPort) || (PortID < 1)) {
		if (0 == strcmp(szAction, "CarrierNotification")) {
			CCarrierObj CarrierNotif;
			CarrierNotif.Preset();
			strscpy(CarrierNotif.ObjID, OBJIDLEN, szCarrierID);
			Notified.push_back(CarrierNotif);
			return ACK_CMD_OK;
		}
	}

	if ((PortID < 1) || (PortID > Var.nLoadPort)) {
		string CarrierID;
		CarrierID = szCarrierID;
		PortID = GetPortID(CarrierID);
		if ((PortID > 0) && (PortID <= Var.nLoadPort)) {
			return pGDev->pGEMObjs->Cms[PortID - 1].CarryOutCarrierAction(szCarrierID, szAction, AttrIDs, AttrValues, pErrorList, PortID);
		}
		return -1;
	}

	CCms* pCms = &pGDev->pGEMObjs->Cms[PortID - 1];
	return (pCms->CarryOutCarrierAction(szCarrierID, szAction, AttrIDs, AttrValues, pErrorList, PortID));
}

BOOL CDataBank::CheckAttribute(string  AttrName, vector<string>& Set) {
	for (int nIndex = 0; nIndex<int(Set.size()); nIndex++) {
		if (0 == Set[nIndex].compare(AttrName)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDataBank::IsSubstrate(const string& SubstID) {
	CSubstObj* pSubST = GetSubstrateObj(SubstID);
	if (pSubST) {
		return TRUE;
	}
	return FALSE;
}

BOOL CDataBank::IsSubstrateLoc(const string& SubstLocID) {
	if (IsEqSubstrateLoc(SubstLocID)) {
		return TRUE;
	}
	else {
		int Pos;
		if (string::npos == (Pos = SubstLocID.find(':'))) {
			return FALSE;
		}
		string PortID = SubstLocID.substr(0, Pos);
		string Slot = SubstLocID.substr(Pos + 1, SubstLocID.size() - 1);
		int SlotNo = atoi(Slot.c_str());
		int PortNo = atoi(PortID.c_str());
		if ((PortNo > 0 && PortNo <= int(Var.nLoadPort)) && (SlotNo < 26 && SlotNo>0)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDataBank::IsEqSubstrateLoc(const string& SubstLocID) {
	CString strSubstLocID = SubstLocID.c_str();

	if (0 == strSubstLocID.CompareNoCase("Scale")) {
		return TRUE;
	}
	else if (0 == strSubstLocID.CompareNoCase("Scanner128")) {
		return TRUE;
	}
	else if (0 == strSubstLocID.CompareNoCase("Scanner900")) {
		return TRUE;
	}
	else if (0 == strSubstLocID.CompareNoCase("Aligner")) {
		return TRUE;
	}
	return FALSE;
}
int CDataBank::GetSubstAttr(const string& SubstID, const string& AttrID, SecsItemList* pList) {
	CSubstObj* pSubST = GetSubstrateObj(SubstID);
	if ((!pSubST) || (!pList)) {
		return UNKNOWN_OBJECT_INSTANCE;
	}

	CString strAttrID = AttrID.c_str();

	if (0 == strAttrID.CompareNoCase("ObjID")) {
		pList->AddItem(new SecsItemASCII(pSubST->ObjID));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("ObjType")) {
		pList->AddItem(new SecsItemASCII(pSubST->ObjType));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("LotID")) {
		pList->AddItem(new SecsItemASCII(pSubST->LotID));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstLocID")) {
		pList->AddItem(new SecsItemASCII(pSubST->SubstLocID));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstSource")) {
		pList->AddItem(new SecsItemASCII(pSubST->SubstSource));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstDestination")) {
		pList->AddItem(new SecsItemASCII(pSubST->SubstDestination));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstType")) {
		pList->AddItem(new SecsItemUInt1(pSubST->SubstType));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstState")) {
		pList->AddItem(new SecsItemUInt1(pSubST->SubstState));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstUsage")) {
		pList->AddItem(new SecsItemUInt1(pSubST->SubstUsage));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstProcState")) {
		pList->AddItem(new SecsItemUInt1(pSubST->SubstProcState));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstHistory")) {
		SecsItemList* pHistoryList = new SecsItemList;
		for (int nIndex = 0; nIndex < SUBSTHISTITEMLEN; nIndex++) {
			if (0 == strlen(pSubST->History[nIndex].SubstLocID)) {
				break;
			}
			SecsItemList* pHistory = new SecsItemList;
			pHistory->AddItem(new SecsItemASCII(pSubST->History[nIndex].SubstLocID));
			CTime In(pSubST->History[nIndex].TimeIn);
			CTime Out(pSubST->History[nIndex].TimeOut);
			pHistory->AddItem(new SecsItemASCII(In.Format("%m:%d:%Y:%H:%M:%S:").GetBuffer(0)));
			pHistory->AddItem(new SecsItemASCII(Out.Format("%m:%d:%Y:%H:%M:%S:").GetBuffer(0)));
			pHistoryList->AddItem(pHistory);
		}
		pList->AddItem(pHistoryList);
		return NO_E5_ERROR;
	}

	return UNKNOWN_ATTRIBUTE_NAME;
}

int CDataBank::GetSubstLocAttr(const string& SubstLocID, const string& AttrID, SecsItemList* pList) {
	if (!IsSubstrateLoc(SubstLocID)) {
		return UNKNOWN_OBJECT_INSTANCE;
	}
	string SubStID;
	BYTE   Occupied = 0;
	for (int nPort = 0; nPort < Var.nLoadPort; nPort++) {
		for (int nIndex = 0; nIndex < MAXCAPACITYLEN; nIndex++) {
			if (0 == SubstLocID.compare(pGDev->pGEMObjs->Cms[nPort].CaObj.Substrate[nIndex].SubstLocID)) {
				SubStID = pGDev->pGEMObjs->Cms[nPort].CaObj.Substrate[nIndex].ObjID;
				Occupied = 1;
			}
		}
	}

	CString strAttrID = AttrID.c_str();

	if (0 == strAttrID.CompareNoCase("ObjID")) {
		pList->AddItem(new SecsItemASCII(SubstLocID));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("ObjType")) {
		pList->AddItem(new SecsItemASCII("SubstLoc"));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstID")) {
		pList->AddItem(new SecsItemASCII(SubStID));
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstLocState")) {
		pList->AddItem(new SecsItemUInt1(Occupied));
		return NO_E5_ERROR;
	}
	return UNKNOWN_ATTRIBUTE_NAME;
}

CSubstObj* CDataBank::GetSubstrateObj(const string& SubstID) {
	for (int i = 0; i < Var.nLoadPort; i++) {
		for (int j = 0; j < MAXCAPACITYLEN; j++) {
			CSubstObj* pSubST = &(pGDev->pGEMObjs->Cms[i].CaObj.Substrate[j]);
			if (SubstID.compare(pSubST->ObjID) == 0) {
				return pSubST;
			}
		}
	}
	return NULL;
}

BOOL CDataBank::GetCarrierID(const string& SubstID, CString& CarrierID) {
	for (int i = 0; i < Var.nLoadPort; i++) {
		for (int j = 0; j < MAXCAPACITYLEN; j++) {
			CString Temp = (pGDev->pGEMObjs->Cms[i].CaObj.CtMap[j].SubstID);
			if (SubstID.compare(Temp) == 0) {
				CarrierID = pGDev->pGEMObjs->Cms[i].CaObj.ObjID;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CDataBank::GetRefObj(long MagicRef, BOOL bRemove, CSubstObj& Ref) {
	for (int nIndex = 0; nIndex < Var.nLoadPort; nIndex++) {
		if (pGDev->pGEMObjs->Cms[nIndex].GetRefObj(MagicRef, bRemove, Ref)) {
			return TRUE;
		}
	}
	return FALSE;
}

int CDataBank::CreateObject(const string& ObjSpec, const string& ObjType, map<string, SecsItem*>& Map, vector<int>& ErrCodes, string& ObCreated) {
	if (ObjSpec.empty()) {
		ErrCodes.push_back(INVALID_DATA_OR_ARG);
		return INVALID_DATA_OR_ARG;
	}
	if (!IsObjectSupported(ObjType.c_str())) {
		ErrCodes.push_back(UNSUPPORTED_OPTION_REQUESTED);
		return UNSUPPORTED_OPTION_REQUESTED;
	}
	CString strObjType = ObjType.c_str();
	if (0 == strObjType.CompareNoCase("ControlJob")) {
		if (CJobMgr.GetCJob((char*)ObjSpec.c_str())) {
			// Yuen: Duplication detected [2002/10/19  22:04]
			ErrCodes.push_back(JOB_INQUEUE);
			return JOB_INQUEUE;
		}
		if (!CheckCJobAttr(Map)) {
			ErrCodes.push_back(INSUFFICIENT_PARAMETERS_SPECIFIED);
			return INSUFFICIENT_PARAMETERS_SPECIFIED;
		}
		CCtrlJob* Cjob = CreateCJob(Map, ErrCodes);
		if (!Cjob) {
			return OBJECT_CREATION_ERROR;
		}
		if (!CJobMgr.Create(Cjob)) {
			ErrCodes.push_back(INVALID_DATA_OR_ARG);
			return INVALID_DATA_OR_ARG;
		}
		ObCreated = Cjob->m_ObjID;
		return NO_E5_ERROR;
	}
	else if (0 == strObjType.CompareNoCase("Substrate")) {
		CSubstObj* pSub = CreateSubst(Map, ErrCodes);
		if (!pSub) {
			return FALSE;
		}
		string Loc = pSub->SubstLocID;
		if (IsSubstrateLoc(Loc)) {
			if (!IsEqSubstrateLoc(Loc)) {
				int Pos;
				if (string::npos == (Pos = Loc.find(':'))) {
					delete pSub;
					return UNKNOWN_E5_ERROR;
				}
				int Lp = atoi(Loc.substr(0, Pos).c_str());
				UINT Slot = atoi(Loc.substr(Pos, Loc.size() - 1).c_str());
				if (!(Lp > 0 && Lp <= int(Var.nLoadPort)) && (Slot > 0 && Slot < MAXCAPACITYLEN)) {
					delete pSub;
					return UNKNOWN_E5_ERROR;
				}
				CCms* pCms = &pGDev->pGEMObjs->Cms[Lp - 1];
				if (pCms && pSub) {
					pCms->CaObj.Substrate[Slot - 1] = *(pSub);
				}
			}
			else {
			}
		}
		pGDev->pGEMObjs->Cms[0].CaObj.Substrate[0] = *(pSub);
		ObCreated = pSub->ObjID;
		delete pSub;
		return TRUE;
	}
	return FALSE;
}

// Yuen: Create CCtrlJob object and set attributes [2002/10/19  22:01]
// Yuen: Will not check for the existance of Carrier Obj [2002/11/07  8:57]
CCtrlJob* CDataBank::CreateCJob(map<string, SecsItem*>& AttMap, vector<int>& ErrCodes) {
	CCtrlJob* pObj = new CCtrlJob;
	int Err = 0;
	map<string, SecsItem*>::iterator it = AttMap.begin();
	for (; it != AttMap.end(); it++) {
		if ((Err = pObj->SetAttr(it->first, it->second, TRUE))) {
			ErrCodes.push_back(Err);
		}
		delete it->second;
	}
	AttMap.clear();
	if (!ErrCodes.size()) {
		switch (pObj->IsPRJobValid()) {
		case  0:
			if (pObj->IsCarrierPresent()) {
				return pObj;
			}
			else {
				ErrCodes.push_back(INVALID_CARRIER_OBJECT_SPECIFIED);
			}
			break;
		case -1:
			ErrCodes.push_back(SPECIFIED_PRJOB_NOT_IN_QUEUE);
			break;
		case -2:
			ErrCodes.push_back(INVALID_CARRIER_OBJECT_SPECIFIED);
			break;
		}
	}
	delete pObj;
	return NULL;
}

CSubstObj* CDataBank::CreateSubst(map<string, SecsItem*>& AttMap, vector<int>& ErrCodes) {
	CSubstObj* pObj = new CSubstObj;
	pObj->Preset();
	BOOL bFail = FALSE;
	int Err = 0;

	map<string, SecsItem*>::iterator it = AttMap.begin();
	for (; it != AttMap.end(); it++) {
		if ((Err = SetSubstAttr(pObj, it->first, it->second, TRUE))) {
			ErrCodes.push_back(Err);
			bFail = TRUE;
		}
		delete it->second;
	}
	AttMap.clear();
	if (!bFail) {
		return pObj;
	}
	delete pObj;
	return NULL;
}

int CDataBank::SetSubstAttr(CSubstObj* pSubst, const string& AttrID, SecsItem* pItem, BOOL bCreating) {
	if (!pSubst || !pItem) {
		return UNKNOWN_E5_ERROR;
	}
	string TempVal;
	CString strAttrID = AttrID.c_str();

	if (0 == strAttrID.CompareNoCase("ObjID")) {
		if ((!pItem->GetASCII(TempVal)) || (TempVal.size() >= OBJIDLEN)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		strcpy(pSubst->ObjID, TempVal.c_str());
		return NO_E5_ERROR;
	}
	// ReadOnly Parameter ???
	if (0 == strAttrID.CompareNoCase("ObjType")) {
		if ((!pItem->GetASCII(TempVal)) || (TempVal.size() >= OBJTYPELEN)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		strcpy(pSubst->ObjType, TempVal.c_str());
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("LotID")) {
		if ((!pItem->GetASCII(TempVal)) || (TempVal.size() >= LOTIDLEN)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		strcpy(pSubst->LotID, TempVal.c_str());
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstLocID")) {
		if ((!pItem->GetASCII(TempVal)) || (TempVal.size() >= SUBSTLOCIDLEN)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		strcpy(pSubst->SubstLocID, TempVal.c_str());
		int nIndex;
		for (nIndex = 0; nIndex < SUBSTHISTITEMLEN; nIndex++) {
			if (0 == strlen(pSubst->History[nIndex].SubstLocID)) {
				break;
			}
		}
		if (nIndex < SUBSTHISTITEMLEN) {
			strcpy(pSubst->History[nIndex].SubstLocID, TempVal.c_str());
			time_t Time = CTime::GetCurrentTime().GetTime();
			if (nIndex != 0) {
				pSubst->History[nIndex - 1].TimeOut = Time;
			}
			pSubst->History[nIndex].TimeIn = Time;
		}
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstSource")) {
		if ((!pItem->GetASCII(TempVal)) || (TempVal.size() >= SUBSTLOCLEN)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		strcpy(pSubst->SubstSource, TempVal.c_str());
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstDestination")) {
		if ((!pItem->GetASCII(TempVal)) || (TempVal.size() >= SUBSTLOCLEN)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		strcpy(pSubst->SubstDestination, TempVal.c_str());
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstType")) {
		BYTE SubstType = 0;
		if ((!pItem->GetUInt1(SubstType)) || (SubstType != CSubstObj::WAFER)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		pSubst->SubstType = (CSubstObj::SUBSTTYPE)SubstType;
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstState")) {
		BYTE State = 0;
		if ((!pItem->GetUInt1(State)) || (State > CSubstObj::ATDESTINATION)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		pSubst->SubstState = (CSubstObj::SUBSTSTATE)State;
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstUsage")) {
		BYTE Usage = 0;
		if ((!pItem->GetUInt1(Usage)) || (Usage > CSubstObj::sCLEANING)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		pSubst->SubstUsage = (CSubstObj::SUBSTUSAGE)Usage;
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstProcState")) {
		BYTE ProcState = 0;
		if ((!pItem->GetUInt1(ProcState)) || (ProcState > CSubstObj::REJECTED)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		pSubst->SubstProcState = (CSubstObj::SUBSTPROCSTATE)ProcState;
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstMtrlStatus")) {
		BYTE MStat = 0;
		if ((!pItem->GetUInt1(MStat)) || (MStat > 5)) {
			return INVALID_ATTRIBUTE_VALUE;
		}
		return NO_E5_ERROR;
	}
	if (0 == strAttrID.CompareNoCase("SubstHistory")) {
		UINT nHistory = (pItem->size() > SUBSTHISTITEMLEN) ? (SUBSTHISTITEMLEN) : (pItem->size());
		string TimeIn, TimeOut, SubstLoc;
		for (UINT nIndex = 0; nIndex < nHistory; nIndex++) {
			if (
				(!(*pItem)[nIndex][0].GetASCII(SubstLoc)) ||
				(!(*pItem)[nIndex][1].GetASCII(TimeIn)) ||
				(!(*pItem)[nIndex][2].GetASCII(TimeOut))
				) {
				return INVALID_ATTRIBUTE_VALUE;
			}
			strscpy(pSubst->History[nIndex].SubstLocID, SUBSTLOCIDLEN, SubstLoc.c_str());
			pSubst->History[nIndex].TimeIn = Convert2Time(TimeIn);
			pSubst->History[nIndex].TimeOut = Convert2Time(TimeOut);
		}
		return NO_E5_ERROR;
	}
	return UNKNOWN_ATTRIBUTE_NAME;
}

int CDataBank::DeleteObject(string& ObjID) {
	CCtrlJob* pCJ = NULL;
	CPRJob* pPJ = NULL;
	CString OID = ObjID.c_str();

	// Yuen: Standard cases [2002/10/09  16:17]
	if (IsSubstrate(ObjID)) {
		GetSubstrateObj(ObjID)->Preset();
		return 0;
	}
	else if (pCJ = CJobMgr.GetCJob((char*)ObjID.c_str())) {
		if (CJobMgr.CanCancel((char*)ObjID.c_str()) == NO_E5_ERROR) {
			if (CJobMgr.RemoveCJob(pCJ)) {
				return 0;
			}
		}
		return 1;
	}
	else if (pPJ = PJobMgr.GetPRJob(OID)) {
		if (PJobMgr.RemovePRJob(pPJ) == CPRJobMgr::OK) {
			return 0;
		}
		return 1;
	}
	// Yuen: Special non-standard cases [2002/10/09  16:16]
	else if (OID == "CONTROLLJOBALL") {
		return CJobMgr.DequeueAll();
	}
	else if (OID == "PROCESSJOBALL") {
		if (PJobMgr.DequeueAll() == CPRJobMgr::OK) {
			return 0;
		}
	}
	return 1;
}

BOOL CDataBank::GetCurrentPRJobID(short PortNo, string& Id) {
	CStringArray JList;
	CString TempStr;
	PJobMgr.GetActiveJobList(JList);
	for (int i = 0; i < JList.GetSize(); i++) {
		TempStr = JList.GetAt(i);
		CPRJob* p = PJobMgr.GetPRJob(TempStr);
		if (p) {
			if (p->m_PortNo == PortNo) {
				Id = p->m_ObjID;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CDataBank::GetCurrentCJobID(string& Id) {
	CStringArray JList;
	CJobMgr.GetActiveCJobList(JList);
	for (int i = 0; i < JList.GetSize(); i++) {
		CCtrlJob* p = CJobMgr.GetCJob(JList.GetAt(i));
		if (p) {
			Id = p->m_ObjID;
			return TRUE;
		}
	}
	return FALSE;
}

void CDataBank::GetAllPRJobIDs(vector<string>& IdList) {
	CStringArray JList;
	PJobMgr.GetPRJobList(JList);
	for (int i = 0; i < JList.GetSize(); i++) {
		string str;
		str = JList.GetAt(i);
		IdList.push_back(str);
	}
}

void CDataBank::GetAllCJobIDs(vector<string>& IdList) {
	CStringArray JList;
	CJobMgr.GetCJobList(JList);
	for (int i = 0; i < JList.GetSize(); i++) {
		string str;
		str = JList.GetAt(i);
		IdList.push_back(str);
	}
}

void CDataBank::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CDataBank");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CDataBank::GetAllSubstIDS(vector<string>& SubstIds) {
	for (int i = 0; i < Var.nLoadPort; i++) {
		for (int j = 0; j < MAXCAPACITYLEN; j++) {
			CSubstObj* pSubST = &(pGDev->pGEMObjs->Cms[i].CaObj.Substrate[j]);
			if (pSubST->SubstState == CSubstObj::SM_CORRECTLY_OCCUPIED) {
				SubstIds.push_back(pSubST->ObjID);
			}
		}
	}
	return TRUE;
}

BOOL CDataBank::GetSubstLocList(vector<string>& Locs) {
	Locs.push_back("Scale");
	Locs.push_back("Scanner");
	Locs.push_back("Aligner");
	Locs.push_back("Scale");
	char Buf[25] = { 0 };
	for (int nLp = 0; nLp< int(Var.nLoadPort); nLp++) {
		for (UINT nSlot = 0; nSlot < MAXCAPACITYLEN; nSlot++) {
			sprintf(Buf, "%d:%d", nLp + 1, nSlot + 1);
		}
	}
	return TRUE;
}

BOOL CDataBank::IsObjectSupported(const char* ObjType) {
	CString Type = ObjType;
	if (Type.CompareNoCase("ControlJob") == 0) {
		return TRUE;
	}
	else if (Type.CollateNoCase("Substrate") == 0) {
		return TRUE;
	}
	return FALSE;
}

BOOL CDataBank::IsEqpIdle() {
	return (pGDev->GetARAMSState() == SArams::STANDBY);
}

BOOL CDataBank::IsLPInService(UCHAR PortNo) {
	BOOL  bRes = FALSE;
	if ((PortNo > 0) && (PortNo < Var.nLoadPort)) {
		CCms* pCms = &pGDev->pGEMObjs->Cms[PortNo - 1];
		CLPObj::TRANSFERSTATUS Ts = pCms->LPObj.GetTransferStatus();
		bRes = ((Ts != CLPObj::OUTOFSERVICE) &&
			(Ts != CLPObj::tsNOSTATE) &&
			(Ts != CLPObj::igTRANSFERSTATUS));
	}
	return bRes;
}

BOOL CDataBank::CjobValidityCheck(const char* CJobID) {
	return ((IsCjobCarrierPresent(CJobID)) &&
		(IsCJobPjobsPresent(CJobID)) &&
		(ValidatePrjobMatlInCJ(CJobID))
		);
}

// Following methods are exclusive for Control job validation
// ... before it can be executed
BOOL CDataBank::IsCjobCarrierPresent(const char* CJobID) {
	BOOL bFound = FALSE;
	CCtrlJob* pCjob = CJobMgr.GetCJob((char*)CJobID);
	if (pCjob) {
		CStringArray& CarrierRef = pCjob->CarrierInputSpec;
		if (CarrierRef.GetSize()) {
			bFound = TRUE;
			for (int nCarriers = 0; nCarriers<int(CarrierRef.GetSize()); nCarriers++) {
				if (!IsCarrierPresent(CarrierRef[nCarriers])) {
					bFound = FALSE;
					CString Msg;
					Msg.Format("Control Job <%s> Error : Carrier <%s> Not on equipment", CJobID, CarrierRef[nCarriers].GetBuffer(0));
					Log(Msg, 1);
					break;
				}
			}
		}
	}
	return bFound;
}

BOOL CDataBank::IsCJobPjobsPresent(const char* CJobID) {
	CStringArray PjList;
	PJobMgr.GetPRJobList(PjList);
	BOOL bFound = FALSE;
	CCtrlJob* pCjob = CJobMgr.GetCJob((char*)CJobID);
	if (pCjob) {
		CStringArray& ArRef = pCjob->ProcessingCtrlSpec;
		for (int nPjInCj = 0; nPjInCj<int(ArRef.GetSize()); nPjInCj++) {
			bFound = FALSE;
			for (int nPjInList = 0; nPjInList<int(PjList.GetSize()); nPjInList++) {
				if (PjList[nPjInList] == ArRef[nPjInCj]) {
					bFound = TRUE;
					break;
				}
			}
			if (!bFound) {
				CString Msg;
				Msg.Format("Control Job <%s> Error : PrJob<%s> Not in Process job Queue", CJobID, ArRef[nPjInCj].GetBuffer(0));
				Log(Msg, 1);
				break;
			}
		}
	}
	return bFound;
}

BOOL CDataBank::ValidatePrjobMatlInCJ(const char* CJobID) {
	BOOL bOk = FALSE;
	CString ErrMsg;
	CCtrlJob* pCjob = CJobMgr.GetCJob((char*)CJobID);
	if (pCjob) {
		CStringArray& ArRef = pCjob->ProcessingCtrlSpec;
		CString CarrierID, SlotMap;
		for (UINT nSlot = 0; nSlot < MAXCAPACITYLEN; nSlot++) {
			SlotMap += CSubstObj::GetSMChar(CSubstObj::SM_EMPTY);
		}
		bOk = TRUE;
		for (int nPjInCj = 0; nPjInCj<int(ArRef.GetSize()); nPjInCj++) {
			if (!PJobMgr.GetMatlData(ArRef[nPjInCj].GetBuffer(0), CarrierID, SlotMap)) {
				bOk = FALSE;
				ErrMsg.Format("Control Job <%s> Error :Cannot get matl info for PrJob<%s>", CJobID, ArRef[nPjInCj].GetBuffer(0));
				Log(ErrMsg, 1);
				break;
			}
			if (!CanProcessCarrier(CarrierID, SlotMap)) {
				bOk = FALSE;
				ErrMsg.Format("Control Job <%s> Error : Matl Validation Fail PrJob<%s> CarrierID<%s> Slotmap<%s>", CJobID,
					ArRef[nPjInCj].GetBuffer(0),
					CarrierID.GetBuffer(0),
					SlotMap.GetBuffer(0));
				Log(ErrMsg, 1);
				break;
			}
		}
	}
	return bOk;
}

BOOL CDataBank::CanProcessCarrier(CString& CarrierID, CString& SlotMap) {
	// Carrier is Docked
	BOOL bRes = FALSE;
	string CID = CarrierID.GetBuffer(0);
	int PortID = GetPortID(CID);
	CString ErrMsg;
	if ((PortID != -1) && (PortID < Var.nLoadPort)) {
		CCms& CmRef = pGDev->pGEMObjs->Cms[PortID - 1];
		if (CmRef.IsDocked() &&
			CmRef.IsIdVerificationOk() &&
			CmRef.IsSMVerificationOk() &&
			CmRef.IsSMMatched(SlotMap)
			) {
			bRes = TRUE;
		}
		else {
			ErrMsg.Format("Carrier <%s> cannot process(!Docked || !ID/SM VerificationOK || SM does not match )", CarrierID.GetBuffer(0));
			Log(ErrMsg, 1);
		}
	}
	else {
		ErrMsg.Format("Carrier <%s> not on Equipment", CarrierID.GetBuffer(0));
		Log(ErrMsg, 1);
	}
	return bRes;
}

BOOL CDataBank::CheckCJobAttr(map<string, SecsItem*>& Map) {
	vector<string> ReqdAttr;
	ReqdAttr.push_back("StartMethod");
	ReqdAttr.push_back("CarrierInputSpec");
	ReqdAttr.push_back("ProcessingCtrlSpec");
	ReqdAttr.push_back("ObjID");
	for (UINT iAttr = 0; iAttr < ReqdAttr.size(); iAttr++) {
		if (Map.find(ReqdAttr[iAttr]) == Map.end()) {
			CString msg;
			msg.Format("Attribute %s not found", ReqdAttr[iAttr]);
			Log(msg, 2);
			return FALSE;
		}
	}

	return TRUE;
}

int CDataBank::IsCarrierLocation(const string& LocID) {
	if ((string::npos == LocID.find("LP")) ||
		((string::npos == LocID.find("PLACEMENT")) || (string::npos == LocID.find("DOCKED")))
		) {
		return 0;
	}
	int Port = atoi(LocID.substr(2, 4).c_str());
	if ((Port > int(Var.nLoadPort)) || (Port == 0)) {
		return 0;
	}
	return Port;
}

short CDataBank::GetActiveSubstrateNo() {
	return ActiveSubst;
}

CSubstObj* CDataBank::GetActiveSubstrateObj() {
	if (ActivePort) {
		if (ActiveSubst) {
			CCms* pCms = &pGDev->pGEMObjs->Cms[ActivePort - 1];
			return &pCms->CaObj.Substrate[ActiveSubst - 1];
		}
	}
	return NULL;
}

int CDataBank::GetActivePort() {
	return ActivePort;
}

// VJTD Remove hard coded number
void CDataBank::SetActivePort(int PortNo) {
	if (((PortNo <= 2) && (PortNo > 0))) {
		ActivePort = PortNo;
	}
}

int CDataBank::GetActiveSubstrate() {
	return ActiveSubst;
}

BOOL CDataBank::GetCurrentPRJobID(string& Id) {
	BOOL bRes = FALSE;
	CPRJob* Pj = CJobMgr.GetActivePRJob();
	if (Pj) {
		Id = Pj->m_ObjID;
		bRes = TRUE;
	}
	return bRes;
}

CPRJob* CDataBank::GetActivePrJob() {
	string ID;
	if (Prjob.size()) {
		ID = Prjob;
	}
	else {
		GetCurrentPRJobID(ID);
	}
	return GetPrjobObj(ID);
}

CCtrlJob* CDataBank::GetActiveCtrlJob() {
	string ID;
	if (CJob.size()) {
		ID = CJob;
	}
	else {
		GetCurrentCJobID(ID);
	}
	return GetCtrlJobObj(ID.c_str());
}

void CDataBank::SetActivePrJob(const std::string& PrjobID) {
	this->Prjob = PrjobID;
}

void CDataBank::SetActiveCtrlJob(const std::string& CtlJobID) {
	this->CJob = CtlJobID;
}

void CDataBank::SetActiveSubstrate(int SubstNo) {
	if ((SubstNo > 0) && (SubstNo < 26)) {
		ActiveSubst = SubstNo;
	}
}

void CDataBank::ClearActiveSubstrate() {
	ActiveSubst = 0;
}

void CDataBank::ClearActivePort() {
	ActivePort = 0;
}

BOOL CDataBank::IsCarrierOnLp(CString& CarrierId) {
	for (int iCar = 0; iCar < int(Var.nLoadPort); iCar++) {
		if (pGDev->pGEMObjs->Cms[iCar].CaObj.ObjID == CarrierId) {
			CCarrierObj& Ref = pGDev->pGEMObjs->Cms[iCar].CaObj;
			if ((Ref.IDStatus > CCarrierObj::IDNOTREAD) && (Ref.IDStatus < CCarrierObj::igIDSTATUS)) {
				return TRUE;
			}
			else {
				return FALSE;
			}
		}
	}
	return FALSE;
}

void CDataBank::SetActivePortSubstrate(short PortID, short Slot) {
	SetActivePort(PortID);
	SetActiveSubstrate(Slot);
}

void CDataBank::ClearActivePortSubstrate() {
	ClearActiveSubstrate();
	ClearActivePort();
}

void CDataBank::SetActiveCJPJ(const std::string& CtlJobID, const std::string& PrjobID) {
	SetActiveCtrlJob(CtlJobID);
	SetActivePrJob(PrjobID);
}