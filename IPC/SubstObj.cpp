#include "stdafx.h"
#include "strSOp.h"
#include "SubstObj.h"

CString CSubstObj::GetSubstStateStr(SUBSTSTATE state) {
	switch (state) {
	case NO_TR_STATE:
		return "NO STATE";
	case ATSOURCE:
		return "AT SOURCE";
	case ATWORK:
		return "AT WORK";
	case ATDESTINATION:
		return "AT DESTINATION";
	}
	return "UNKNOWN";
}

CString CSubstObj::GetSubstProcStateStr(SUBSTPROCSTATE state) {
	switch (state) {
	case NO_PROCSTATE:
		return "NO STATE";
		break;
	case NEEDS_PROCESSING:
		return "NEEDS PROCESSING";
		break;
	case IN_PROCESS:
		return "IN PROCESS";
		break;
	case PROCESSED:
		return "PROCESSED";
		break;
	case ABORTED:
		return "ABORTED";
		break;
	case STOPPED:
		return "STOPPED";
		break;
	case REJECTED:
		return "REJECTED";
		break;
	}
	return "UNKNOWN";
}

char CSubstObj::GetSMChar(char sm) {
	switch (sm) {
	case SM_UNDEFINED:
		return '0';
		break;
	case SM_EMPTY:
		return '1';
		break;
	case SM_NOT_EMPTY:
		return '2';
		break;
	case SM_CORRECTLY_OCCUPIED:
		return '3';
		break;
	case SM_DOUBLE_SLOTTED:
		return '4';
		break;
	default:	// Cross slotted
		return '5';
		break;
	}
	return '?';
}

CSubstObj::SLOTMAP CSubstObj::GetSMEnum(char ch) {
	if (GetSMChar(SM_EMPTY) == ch) {
		return SM_EMPTY;
	}
	if (GetSMChar(SM_CORRECTLY_OCCUPIED) == ch) {
		return SM_CORRECTLY_OCCUPIED;
	}
	if (GetSMChar(SM_CROSS_SLOTTED) == ch) {
		return SM_CROSS_SLOTTED;
	}
	if (GetSMChar(SM_DOUBLE_SLOTTED) == ch) {
		return SM_DOUBLE_SLOTTED;
	}
	if (GetSMChar(SM_NOT_EMPTY) == ch) {
		return SM_NOT_EMPTY;
	}
	return SM_UNDEFINED;
}

void CSubstObj::Preset() {
	strscpy(ObjType, OBJTYPELEN, "Substrate");
	ZeroMemory(ObjID, OBJIDLEN + 1);
	ZeroMemory(LotID, LOTIDLEN + 1);
	ZeroMemory(SubstLocID, SUBSTLOCIDLEN + 1);
	ZeroMemory(SubstSource, SUBSTLOCLEN + 1);
	ZeroMemory(SubstDestination, SUBSTLOCLEN + 1);
	SlotNo = 0;
	SubstProcState = NO_PROCSTATE;
	SubstState = NO_TR_STATE;
	SubstType = WAFER;
	SubstUsage = sPRODUCT;
	for (int i = 0; i < SUBSTHISTITEMLEN; i++) {
		History[i].Preset();
	}
}

void CSubstObj::SetState(CSubstObj::SUBSTSTATE state) {
	SubstState = state;
}

void CSubstObj::SetProcState(CSubstObj::SUBSTPROCSTATE state) {
	SubstProcState = state;
}

BOOL CSubstObj::GetAttr(char* attrname, std::string& attr) {
	char str[32];
	CString name;
	name = attrname;
	if (name == "ObjID") {
		attr = ObjID;
		//strcpy_s(attr,len,ObjID);
		return TRUE;
	}
	if (name == "ObjType") {
		attr = ObjType;
		//strcpy_s(attr,len,ObjType);
		return TRUE;
	}
	if (name == "LotID") {
		attr = LotID;
		//strcpy_s(attr,len,LotID);
		return TRUE;
	}
	if (name == "SubstLocID") {
		attr = SubstLocID;
		//strcpy_s(attr,len,SubstLocID);
		return TRUE;
	}
	if (name == "SubstSource") {
		attr = SubstSource;
		//strcpy_s(attr,len,SubstSource);
		return TRUE;
	}
	if (name == "SubstDestination") {
		attr = SubstDestination;
		//strcpy_s(attr,len,SubstDestination);
		return TRUE;
	}
	if (name == "SubstType") {
		sprintf(str, "%d", SubstType);
		attr = str;
		return TRUE;
	}
	if (name == "SubstState") {
		sprintf(str, "%d", SubstState);
		attr = str;
		return TRUE;
	}
	if (name == "SubstUsage") {
		sprintf(str, "%d", SubstUsage);
		attr = str;
		return TRUE;
	}
	if (name == "SubstProcState") {
		sprintf(str, "%d", SubstProcState);
		attr = str;
		return TRUE;
	}
	return FALSE;
}

BOOL CSubstObj::SetAttr(char* attrname, std::string attr) {
	CString name;
	name = attrname;
	if (name == "ObjID") {
		strscpy(ObjID, OBJIDLEN, attr.c_str());
		return TRUE;
	}
	if (name == "ObjType") {
		strscpy(ObjType, OBJTYPELEN, attr.c_str());
		return TRUE;
	}
	if (name == "LotID") {
		strscpy(LotID, LOTIDLEN, attr.c_str());
		return TRUE;
	}
	if (name == "SubstLocID") {
		strscpy(SubstLocID, SUBSTLOCIDLEN, attr.c_str());
		return TRUE;
	}
	if (name == "SubstSource") {
		strscpy(SubstSource, SUBSTLOCLEN, attr.c_str());
		return TRUE;
	}
	if (name == "SubstDestination") {
		strscpy(SubstDestination, SUBSTLOCLEN, attr.c_str());
		return TRUE;
	}
	if (name == "SubstType") {
		SubstType = (CSubstObj::SUBSTTYPE)atoi(attr.c_str());
		return TRUE;
	}
	if (name == "SubstState") {
		SubstState = (CSubstObj::SUBSTSTATE)atoi(attr.c_str());
		return TRUE;
	}
	if (name == "SubstUsage") {
		SubstUsage = (CSubstObj::SUBSTUSAGE)atoi(attr.c_str());
		return TRUE;
	}
	if (name == "SubstProcState") {
		SubstProcState = (CSubstObj::SUBSTPROCSTATE)atoi(attr.c_str());
		return TRUE;
	}
	return FALSE;
}