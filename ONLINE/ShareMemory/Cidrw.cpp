#include "stdafx.h"
#include "Cidrw.h"
#include "SRC/DOSUtil.h"

CCidrw::CCidrw() {
	init();
}

CCidrw::~CCidrw() {
	for (int nIndex = 0; nIndex<int(this->m_Heads.size()); nIndex++) {
		delete(this->m_Heads[nIndex]);
	}
	this->m_Heads.clear();
}

bool CCidrw::GetStatus(UINT TargetID, SecsItemList* pStat) {
	if (IsInit()) {
		return false;
	}
	else if (TargetID > this->m_Heads.size()) {
		// Error Condition
		return false;
	}
	if (0 == TargetID) {
		// Apply to the CIDRW
		GetStatus(pStat);
	}
	else {
		CRWHead* pHead = this->m_Heads[TargetID - 1];
		pHead->GetPMInfo(pStat);
		pHead->GetAlarmStat(pStat);
		pHead->GetOpStat(pStat);
		pHead->GetHeadStat(pStat);
	}
	return true;
}

bool CCidrw::GetStatus(SecsItemList* pStat) {
	/*$$$$$$$$$
		 Item 1 is PM information
		 Item 2 is AlarmStatus
		 Item 3 is Operational Status
		 Item 4 is HeadStatus (List of HEadStatus of alll Heads)
	$$$$$$$$$*/
	GetPMInfo(pStat);
	GetAlarmStat(pStat);
	GetOpStat(pStat);
	GetHeadStat(pStat);
	return true;
}

void CCidrw::GetPMInfo(SecsItemList* pStat) {
	pStat->AddItem(new SecsItemASCII((m_bMaintReqd) ? "MR" : "NO"));
}

void CCidrw::GetAlarmStat(SecsItemList* pStat) {
	string AS = "0";
	if (ALARMS == m_AlarmStatus) {
		AS = "1";
	}
	pStat->AddItem(new SecsItemASCII(AS));
}

void CCidrw::GetOpStat(SecsItemList* pStat) {
	string OpS = "NOOP";
	switch (m_OperationStat) {
	case brIDLE: OpS = "IDLE"; break;
	case brBUSY: OpS = "BUSY"; break;
	case brMAINTENANCE: OpS = "MANT"; break;
	}
	pStat->AddItem(new SecsItemASCII(OpS));
}

void CCidrw::GetHeadStat(SecsItemList* pStat) {
	SecsItemList* pHS = new SecsItemList;
	for (int nIndex = 0; nIndex<int(m_Heads.size()); nIndex++) {
		m_Heads[nIndex]->GetHeadStat(pHS);
	}
	pStat->AddItem(pHS);
}

/*
	Return value will indicate Success/Failure of operation & pStat will always have
	valid data.

*/
bool CCidrw::ChangeState(OPER_STATE NewState, SecsItemList* pStat) {
	// Change State
	if (IsInit()) {
		// Invalid starting state cannot change at this time
		m_Err = EX_ERR;
		GetStatus(pStat);
		return false;
	}
	else if (this->m_OperationStat != NewState) {
		//change state
		this->m_OperationStat = NewState;
		// Send Event
	}
	// Send To Equipment
	//
	GetStatus(pStat);
	return true;
}

bool CCidrw::GetAttributes(int HeadID, vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pRes) {
	if (IsInit()) {
		m_Err = EX_ERR;
		return false;
	}
	else if (0 == HeadID) {
		// This applies to CIDRW head itself
		return GetAttributes(Attribs, pAttrib, pRes);
	}
	else if (HeadID <int(this->m_Heads.size())) {
		// Pass to appropraite Head
		return this->m_Heads[HeadID - 1]->GetAttributes(Attribs, pAttrib, pRes);
	}
	else {
		// Invalid TargetID
		m_Err = COMM_ERR;
		return false;
	}
	return true;
}

bool CCidrw::PerformDiagnostics(int HeadID, const string& Action,/*CRWHead::HEAD_STATE &Stat,*/SecsItemList* pStat) {
	if (IsInit()) {
		return false;
	}
	else if (0 == HeadID) {
		// Apply to CIDW
		return PerformDiag(Action/*,Stat*/, pStat);
	}
	else if (HeadID<int(this->m_Heads.size())) {
		return this->m_Heads[HeadID - 1]->PerformDiag(Action/*,Stat*/, pStat);
	}
	else {
		return false;
	}
}

bool CCidrw::PerformDiag(const string& Action/*,CRWHead::HEAD_STATE &Stat*/, SecsItemList* pStat) {
	//	Stat = (CRWHead::HEAD_STATE)0;
	GetStatus(pStat);
	return true;
}

bool CCidrw::ReadDataSeg(UINT HeadID, string& CarrierID, int DataSize, int DataSeg) {
	if (IsInit() || IsMaint()) {
		return false;
	}
	else if (0 == HeadID) {
		return false;
	}
	else if (HeadID < m_Heads.size()) {
		return m_Heads[HeadID - 1]->ReadDataSeg(CarrierID, DataSize, DataSeg);
	}
	return false;
}

bool CCidrw::ReadID(UINT HeadID, string& CarrierID, SecsItemList* pStat) {
	if (IsInit()) {
		return false;
	}
	else if (0 == HeadID) {
		return false;
	}
	else if (HeadID < m_Heads.size()) {
		return m_Heads[HeadID - 1]->ReadID(CarrierID, pStat);
	}
	return false;
}
bool CCidrw::Reset(SecsItemList* pStat) {
	if (IsInit()) {
		return false;
	}
	// Send Reset Command to Equipment
	GetStatus(pStat);
	return true;
}
/**
	Set Attributes Always returns false because all Attributes for CIDRW & RWhead are Read Only
**/
bool CCidrw::SetAttributes(int HeadID, const vector<string>& AttribIDs, const vector<string>& AttrVal, SecsItemList* pStat) {
	if (IsInit()) {
		m_Err = EX_ERR;
	}
	else if (0 == HeadID) {
		m_Err = EX_ERR;
	}
	else if (HeadID<int(m_Heads.size())) {
		m_Err = EX_ERR;
	}
	else {
		m_Err = COMM_ERR;
	}
	GetStatus(HeadID, pStat);
	return false;
}

bool CCidrw::WriteDataSeg(UINT HeadID, const string& CarrierID, int DataSize, int DataSeg, SecsItemList* pStat) {
	if (IsInit() || IsMaint()) {
		return false;
	}
	else if (0 == HeadID) {
		return false;
	}
	else if (HeadID < m_Heads.size()) {
		return m_Heads[HeadID - 1]->WriteDataSeg(CarrierID, DataSize, DataSeg, pStat);
	}
	return false;
}

bool CCidrw::WriteID(UINT HeadID, const string& CarrierID, SecsItemList* pStat) {
	if (!IsMaint()) {
		return false;
	}
	else if (0 == HeadID) {
		return false;
	}
	else if (HeadID < m_Heads.size()) {
		return m_Heads[HeadID - 1]->WriteID(CarrierID, pStat);
	}
	return false;
}

void CCidrw::init() {
	CString LocalSys;
	// Read number of heads form Config file
	DosUtil.GetLocalSysFile(LocalSys);

	int CidrType = ::GetPrivateProfileInt("CIDR", "CidrType", 0, LocalSys);
	int HeadType = ::GetPrivateProfileInt("CIDR", "HeadType", 0, LocalSys);
	int NumHeads = ::GetPrivateProfileInt("CIDR", "NumHeads", 0, LocalSys);
	CRWHead* pHead = NULL;
	// Create the Heads
	// TBD : Vijay Use Abstract factory
	for (int nIndex = 0; nIndex < NumHeads; nIndex++) {
		switch (HeadType) {
		case CRWHead::BCR:
			pHead = new RWHeadBcr(nIndex + 1);
			break;
		case CRWHead::RFID:
			pHead = new RWHeadRfid(nIndex + 1);
		}
		this->m_Heads.push_back(pHead);
	}
	switch (CidrType) {
	case READ_ONLY:
	case READ_WRITE:this->m_Type = (RW_TYPE)CidrType; break;
	default:ASSERT(0);
	}
	m_bMaintReqd = false;
	// Update State information
	SetInit();
	m_Err = NORMAL;
}

bool CCidrw::GetAttributes(vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pResStat) {
	static int nNumAttribs = InitAttributes();
	enum { INDEX_ATTRIB = 0, INDEX_RES };

	if ((!pAttrib) || (!pResStat)) {
		m_Err = COMM_ERR;
		return false;
	}
	vector<SecsItem*> Res;
	if (0 == Attribs.size()) {
		// All Attributes are requested
		GetAllAttribIDs(Attribs);
	}
	for (int nIndex = 0; nIndex<int(Attribs.size()); nIndex++) {
		if (GetAttribute(Attribs[nIndex], Res)) {
			pAttrib->AddItem(Res[INDEX_ATTRIB]);
			pResStat->AddItem(Res[INDEX_RES]);
		}
	}
	m_Err = NORMAL;
	return true;
}

bool CCidrw::GetAttribute(const string& Attrib, vector<SecsItem*>& Res) {
	Res.clear();
	char Data[52] = { 0 };
	if (!IsValidAtrib(Attrib)) {
		Res.push_back(new SecsItemASCII(""));
		Res.push_back(new SecsItemUInt1(COMM_ERR));
	}
	else if (Attrib.compare("Configuration")) {
		sprintf(Data, "%02d", this->m_Heads.size());
		Res.push_back(new SecsItemASCII(Data));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	else if (Attrib.compare("AlarmStatus")) {
		Res.push_back(new SecsItemUInt1(m_AlarmStatus));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	else if (Attrib.compare("OperationStatus")) {
		Res.push_back(new SecsItemUInt1(m_OperationStat));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	else {
		// All other attributes values are fixed so are stored in
		// ... configuration file
		CString LocalSys;
		char Str[256];
		DosUtil.GetLocalSysFile(LocalSys);
		::GetPrivateProfileString("CIDR", Attrib.c_str(), "UnKnown", Str, sizeof(Str), LocalSys);
		Res.push_back(new SecsItemASCII(Str));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	return true;
}

void CCidrw::GetAllAttribIDs(vector<string>& AttribIDs) {
	AttribIDs.clear();
	map<string, int>::iterator Pos;
	for (Pos = AttribMap.begin(); Pos != AttribMap.end(); ++Pos) {
		AttribIDs.push_back(Pos->first);
	}
}

bool CCidrw::IsValidAtrib(const string& Key) {
	return (AttribMap.find(Key) != AttribMap.end());
}

int CCidrw::InitAttributes() {
	// Data should be from configuration file
	AttribMap["Configuration"] = 1;
	AttribMap["AlarmStatus"] = 2;
	AttribMap["OperationStatus"] = 3;
	AttribMap["SoftwareRevisionLevel"] = 4;
	AttribMap["DateInstalled"] = 5;
	AttribMap["DeviceType"] = 6;
	AttribMap["HardwareRevisionLevel"] = 7;
	AttribMap["MaintenanceData"] = 8;
	AttribMap["Manufacturer"] = 9;
	AttribMap["ModelNumber"] = 10;
	AttribMap["SerialNumber"] = 11;
	return AttribMap.size();
}

bool CCidrw::SetAlarmStatP(bool bNewStat) {
	bool bRet = false;
	if (IsRunning()) {
		CString LocalSys;
		CString strVal = (bNewStat) ? "1" : "0";
		DosUtil.GetLocalSysFile(LocalSys);
		::WritePrivateProfileString("CIDR", "ALARMS", strVal, LocalSys);
		bRet = true;
	}
	return bRet;
}

bool GetAlarmStatP() {
	bool bRet = false;
	CString LocalSys;
	DosUtil.GetLocalSysFile(LocalSys);
	bRet = (1 == ::GetPrivateProfileInt("CIDR", "ALARMS", 0, LocalSys)) ? true : false;
	return bRet;
}

/***Iterface with Equipment */

/*** General Utility **/
bool CCidrw::SetRunning() {
	bool bRet = false;
	if (IsInit() || IsBusy()) {
		m_CidrwState = cidRUNNING;
		m_RunSubState = cidOPERATING;
		m_AlarmStatus = NO_ALARMS;
		m_OperationStat = brIDLE;
		bRet = true;
	}
	// apply to all heads
	return bRet;
}

bool CCidrw::SetOperating() {
	SetRunning();
	return true;
}

bool CCidrw::SetMaint() {
	bool bRet = false;
	if (IsIdle()) {
		m_OperationStat = brNOT_OPERATING;
		m_RunSubState = MAINT;
		bRet = true;
	}
	return bRet;;
}

bool CCidrw::SetIdle() {
	bool bRet = false;
	if (IsBusy()) {
		m_OperationStat = brIDLE;
		bRet = true;
	}
	else if (IsMaint()) {
		m_OperationStat = brIDLE;
		m_RunSubState = cidOPERATING;
		bRet = true;
	}
	return bRet;
}

bool CCidrw::SetBusy() {
	bool bRet = false;
	if (IsIdle()) {
		m_OperationStat = brBUSY;
		bRet = true;
	}
	return bRet;
}

bool CCidrw::SetInit() {
	m_CidrwState = cidINITIALIZING;
	m_RunSubState = NOT_RUNNING;
	m_AlarmStatus = NOT_ALARM_STATE;
	m_OperationStat = brNOT_OPERATING;
	return true;
}
/*** End General Utility **/

void CCidrw::GetErrCode(string& Err) {
	switch (m_Err) {
	case NORMAL: Err = "NO"; break;
	case EX_ERR: Err = "EE"; break;
	case COMM_ERR: Err = "CE"; break;
	case HW_ERR: Err = "HE"; break;
	case TAG_ERR: Err = "TE"; break;
	}
	m_Err = NORMAL;
}

bool CCidrw::ProcessCmd(int HeadID, const string& Cmd, const vector<string>& CmdPars, SecsItemList* pStat) {
	if (Cmd.compare("ChangeState")) {
		OPER_STATE NewState;
		if (CmdPars[0].compare("OP")) {
			NewState = brIDLE;
		}
		else if (CmdPars[0].compare("MO")) {
			NewState = brMAINTENANCE;
		}
		else {
			// Error ! Error ! Error
			return false;
		}
		return ChangeState(NewState, pStat);
	}
	else if (Cmd.compare("GetStatus")) {
		ASSERT(CmdPars.empty());
		GetStatus(HeadID, pStat);
		return GetStatus(HeadID, pStat);
	}
	else if (Cmd.compare("PerformDiagnostics")) {
		return PerformDiagnostics(HeadID, CmdPars[0], pStat);
	}
	else if (Cmd.compare("Reset")) {
		ASSERT(CmdPars.empty());
		return Reset(pStat);
	}
	else {
		return false;
		m_Err = COMM_ERR;
	}
	return true;
}