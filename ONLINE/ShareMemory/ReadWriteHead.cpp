// ReadWriteHead.cpp: implementation of the CReadWriteHead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReadWriteHead.h"
#include "..\..\Src\DosUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRWHead::CRWHead(int HeadNum) {
	// Common Initialization for all subtypes
	this->m_HeadID = HeadNum;
	m_HeadStatus = NO_CIDRW;
	m_NumCycles = 0;					// Read from Cfg file
	m_HeadDateInstalled;				// Read from Cfg file
	m_HeadCondition = NO_POWER;			// Read from Cfg file
	m_MaintData = "";					// Read from Cfg file
	m_OperatingStatus = NO_STATE;		// Read from Cfg file
}

CRWHead::~CRWHead() {}
// Common Attributes for RWHead derived Attributes
// .. derived classes may have additional parameters
int CRWHead::InitAttributes() {
	// Data should be from configuration file
	AttribMap["HeadStatus"] = 1;
	AttribMap["HeadID"] = 2;
	AttribMap["Cycles"] = 3;
	AttribMap["HeadCondition"] = 4;
	AttribMap["HeadDateInstalled"] = 5;
	AttribMap["HeadMaintenanceData"] = 6;
	return AttribMap.size();
}
void CRWHead::GetAllAttribIDs(vector<string>& AttribIDs) {
	AttribIDs.clear();
	map<string, int>::iterator Pos;
	for (Pos = AttribMap.begin(); Pos != AttribMap.end(); ++Pos) {
		AttribIDs.push_back(Pos->first);
	}
}
bool CRWHead::GetAttribute(const string& Attrib, vector<SecsItem*>& Res) {
	Res.clear();
	char Data[52] = { 0 };
	if (Attrib.compare("HeadStatus")) {
		switch (m_HeadStatus) {
		case NOT_OPERATING:
			Res.push_back(new SecsItemUInt1(2));
			break;

		case OPERATING:
			switch (m_OperatingStatus) {
			case bsIDLE: Res.push_back(new SecsItemUInt1(0)); break;
			case bsBUSY: Res.push_back(new SecsItemUInt1(1)); break;
			default:ASSERT(0);
			}
			break;
		default:ASSERT(0);
		}
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	else if (Attrib.compare("HeadID")) {
		sprintf(Data, "%02d", atoi(this->m_HeadID.c_str())); // HeadID is not int [12/5/2007 YUEN]
		Res.push_back(new SecsItemASCII(Data));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	else if (Attrib.compare("HeadCondition")) {
		Res.push_back(new SecsItemUInt1(m_HeadCondition));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	else {
		// All other attributes values are fixed so are stored in
		// ... configuration file
		CString LocalSys;
		char Str[256];
		char Sect[256];
		sprintf(Sect, "RWHEAD%d", atoi(this->m_HeadID.c_str()));  // HeadID is not int [12/5/2007 YUEN]
		DosUtil.GetLocalSysFile(LocalSys);
		::GetPrivateProfileString(Sect, Attrib.c_str(), "UnKnown", Str, sizeof(Str), LocalSys);
		Res.push_back(new SecsItemASCII(Str));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	return true;
}
/*
	Can change to busy only from Idle state CIDRW SCT 13
*/
bool CRWHead::SetBusy() {
	bool bRet = false;
	if (m_OpSubStates == bsIDLE) {
		bRet = true;
	}
	m_HeadStatus = CIDRW_HEAD;
	m_OperatingStatus = OPERATING;
	m_OpSubStates = bsBUSY;
	return bRet;
}

/*
	Can change to idle state from 2 states
	1) Default entry into Operating state. CIDRW SCT 12
	2) From busy substrate. CIDRW SCT 14
*/
bool CRWHead::SetIdle() {
	bool bRet = false;
	if ((m_OpSubStates == bsBUSY) || (m_OperatingStatus == NO_STATE)) {
		bRet = true;
	}
	m_HeadStatus = CIDRW_HEAD;
	m_OperatingStatus = OPERATING;
	m_OpSubStates = bsBUSY;
	return bRet;
}

/*
	Alarms present puts the head in NOT operating state. CIDRW SCT 15
	(Called from CIDRW)
*/
bool CRWHead::AlarmPresent() {
	bool bRet = false;
	if (OPERATING == m_OperatingStatus) {
		bRet = true;
	}
	//  HeadStatus Do not change
	m_OperatingStatus = NOT_OPERATING;
	m_OpSubStates = bsNO_SUBSTATE;
	return bRet;
}

/*
	Alarms clearing puts the head in operating state. CIDRW SCT 16
	(Called from CIDRW)
*/
bool CRWHead::AlarmCleared() {
	bool bRet = false;
	if (NOT_OPERATING == m_OperatingStatus) {
		bRet = true;
	}
	//  HeadStatus Do not change
	m_OperatingStatus = OPERATING;
	m_OpSubStates = bsIDLE;
	return bRet;
}

bool CRWHead::IsValidAtrib(const string& Key) {
	return (AttribMap.find(Key) != AttribMap.end());
}
void CRWHead::GetPMInfo(SecsItemList* pStat) {
	pStat->AddItem(new SecsItemASCII((NEEDS_MAINT == m_HeadCondition) ? "MR" : "NO"));
}
void CRWHead::GetAlarmStat(SecsItemList* pStat) {
	pStat->AddItem(new SecsItemUInt1((NO_ALARMS == m_HeadCondition) ? 0 : 1));
}
void CRWHead::GetOpStat(SecsItemList* pStat) {
	pStat->AddItem(new SecsItemUInt1(m_OperatingStatus));
}
void CRWHead::GetHeadStat(SecsItemList* pStat) {
	string HS = "NOOP";
	switch (m_HeadStatus) {
	case bsIDLE:HS = "IDLE"; break;
	case bsBUSY:HS = "BUSY"; break;
	}
	pStat->AddItem(new SecsItemASCII(HS));
}
void CRWHead::GetStatus(SecsItemList* pStat) {
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
}

// Bar code reader type Head IMPL
RWHeadBcr::RWHeadBcr(int HeadNum) :CRWHead(HeadNum) {
	// Initialize
}
RWHeadBcr::~RWHeadBcr() {}
BOOL  RWHeadBcr::GetAttr(CString service, void* buf) {
	return TRUE;
}
int RWHeadBcr::InitBcrAttribs() {
	// Data should be from configuration file
	int nSize = AttribMap.size();
	AttribMap["TestBCR"] = nSize + 1;
	return AttribMap.size();
}
bool RWHeadBcr::GetAttributes(vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pResStat) {
	static int nNumAttribs = InitAttributes() + InitBcrAttribs();;
	enum { INDEX_ATTRIB = 0, INDEX_RES };

	if ((!pAttrib) || (!pResStat)) {
		// Adding anything to Item that is not a list is NOP so..
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

	return true;
}

bool RWHeadBcr::GetAttribute(const string& Attrib, vector<SecsItem*>& Res) {
	Res.clear();
	if (!IsValidAtrib(Attrib)) {
		// Parameters not found
		return false;
	}
	else if (CRWHead::GetAttribute(Attrib, Res)) {
		// Check common attributes
		return true;
	}
	else if (Attrib.compare("TestBCR")) {
		// Check class specific attributes
		Res.push_back(new SecsItemASCII("TestBCR_OK"));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	return true;
}

BOOL  RWHeadBcr::SetAttr(CString service, void* val) {
	return FALSE;
}
void  RWHeadBcr::GetAttrName(CStringArray& nArray) {};
void  RWHeadBcr::ClearAll() {};
bool RWHeadBcr::ReadData(string& Data, int DataSeg, UINT DataSize) {
	// Forward the request to actual reader
	return true;
};
bool RWHeadBcr::WriteData(const string& Data, int DataSeg, UINT DataSize) {
	// Forward the request to actual Writer
	return true;
};
bool RWHeadBcr::ReadID(string& CarrierID) {
	// Forward the request to actual reader
	return true;
};
bool RWHeadBcr::WriteID(const string& Data) {
	// Forward the request to actual Writer
	return true;
};
bool RWHeadBcr::PerformDiag(const string& Action,/*CRWHead::HEAD_STATE &Stat,*/ SecsItemList* pStat) {
	return true;
}

bool RWHeadBcr::WriteDataSeg(const string&, int, int, SecsItemList* pStat) {
	return true;
}
bool RWHeadBcr::WriteID(const string& CarrierID, SecsItemList* pStat) {
	return true;
}

bool RWHeadBcr::ReadDataSeg(string& Data, int DataSize, int DataSeg) {
	return true;
}
bool RWHeadBcr::ReadID(string& CarrierID, SecsItemList* pStat) {
	return true;
}

// RFID reader type Head
RWHeadRfid::RWHeadRfid(int HeadNum) :CRWHead(HeadNum) {}
RWHeadRfid::~RWHeadRfid() {}
BOOL  RWHeadRfid::GetAttr(CString service, void* buf) {
	return TRUE;
}

bool RWHeadRfid::GetAttributes(vector<string>& Attribs, SecsItemList* pAttrib, SecsItemList* pResStat) {
	static int nNumAttribs = InitAttributes() + InitRfidAttribs();;
	enum { INDEX_ATTRIB = 0, INDEX_RES };

	if ((!pAttrib) || (!pResStat)) {
		// Adding anything to Item that is not a list is NOP so..
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

	return true;
}

bool RWHeadRfid::GetAttribute(const string& Attrib, vector<SecsItem*>& Res) {
	Res.clear();
	if (!IsValidAtrib(Attrib)) {
		// Parameters not found
		return false;
	}
	else if (CRWHead::GetAttribute(Attrib, Res)) {
		// Check common attributes
		return true;
	}
	else if (Attrib.compare("TestRFID")) {
		// Check class specific attributes
		Res.push_back(new SecsItemASCII("TestRFID_OK"));
		Res.push_back(new SecsItemUInt1(NORMAL_OPERATION));
	}
	return true;
}
int RWHeadRfid::InitRfidAttribs() {
	// Data should be from configuration file
	int nSize = AttribMap.size();
	AttribMap["TestRFID"] = nSize + 1;
	return AttribMap.size();
}

BOOL  RWHeadRfid::SetAttr(CString service, void* val) {
	return FALSE;
}
void  RWHeadRfid::GetAttrName(CStringArray& nArray) {};
void  RWHeadRfid::ClearAll() {};
bool RWHeadRfid::ReadData(string& Data, int DataSeg, UINT DataSize) {
	// Forward the request to actual reader
	return true;
};
bool RWHeadRfid::WriteData(const string& Data, int DataSeg, UINT DataSize) {
	// Forward the request to actual Writer
	return true;
};
bool RWHeadRfid::ReadID(string& CarrierID) {
	// Forward the request to actual reader
	return true;
};
bool RWHeadRfid::WriteID(const string& Data) {
	// Forward the request to actual Writer
	return true;
};
bool RWHeadRfid::PerformDiag(const string& Action,/*CRWHead::HEAD_STATE &Stat,*/ SecsItemList* pStat) {
	return true;
}

bool RWHeadRfid::WriteDataSeg(const string&, int, int, SecsItemList* pStat) {
	return true;
}
bool RWHeadRfid::WriteID(const string& CarrierID, SecsItemList* pStat) {
	return true;
}

bool RWHeadRfid::ReadDataSeg(string& Data, int DataSize, int DataSeg) {
	return true;
}
bool RWHeadRfid::ReadID(string& CarrierID, SecsItemList* pStat) {
	return true;
}