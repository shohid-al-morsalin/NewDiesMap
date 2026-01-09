#include "stdafx.h"

#include "strSOp.h"
#include "CarrierObj.h"

void CCarrierObj::Preset() {
	IDStatus = CCarrierObj::IDNOSTATE;
	SlotmapStatus = CCarrierObj::SMNOSTATE;
	AccessingStatus = CCarrierObj::ASNOSTATE;
	ZeroMemory(Usage, USAGELEN + 1);
	strcpy(Usage, "Process");
	ZeroMemory(LocationID, LOCIDLEN + 1);
	strcpy(LocationID, "LoadPort");
	strcpy(ObjType, "Carrier");
	strcpy(ObjID, "");
	Capacity = MAXCAPACITYLEN;
	SubstrateCount = 0;
	for (int i = 0; i < Capacity; i++) {
		SlotMaP[i] = CSubstObj::SM_UNDEFINED;
		CtMap[i].Preset();
		Substrate[i].Preset();
	}
}

char* CCarrierObj::GetObjType() {
	return ObjType;
}

void CCarrierObj::ClearCt() {
	short Cap = Capacity;
	if (Cap < 1) {
		Cap = 1;
	}
	if (Cap > 26) {
		Cap = 26;
	}
	for (int i = 0; i < Cap; i++) {
		CtMap[i].Preset();
	}
}

BOOL CCarrierObj::GetAttr(char* attrname, char* attr, short len) {
	CString name;
	name = attrname;
	if (len > OBJIDLEN) {
		if (name == "ObjID") {
			strscpy(attr, len, ObjID);
			return TRUE;
		}
	}
	if (len > OBJTYPELEN) {
		if (name == "ObjType") {
			strscpy(attr, len, ObjType);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CCarrierObj::SetAttr(char* attrname, char* attr) {
	CString name;
	name = attrname;
	if (name == "ObjID") {
		strscpy(ObjID, OBJIDLEN, attr);
		return TRUE;
	}
	if (name == "ObjType") {
		strscpy(ObjType, OBJTYPELEN, attr);
		return TRUE;
	}
	return FALSE;
}

void CCarrierObj::MakeSlotmapFromContentMap() {
	if (Capacity < 1) {
		ASSERT(0);
	}
	if (Capacity > 26) {
		ASSERT(0);
		Capacity = 26;
	}
	for (int i = 0; i < Capacity; i++) {
		SlotMaP[i] = CSubstObj::SM_EMPTY;
		if (strlen(CtMap[i].LotID)) {
			if (strlen(CtMap[i].SubstID)) {
				SlotMaP[i] = CSubstObj::SM_CORRECTLY_OCCUPIED;
			}
		}
	}
}

BOOL CCarrierObj::UpdateAfterProceedWithCarrierID(CCarrierObj& Ca) {
	if (Capacity < 1) {
		ASSERT(0);
	}
	if (Capacity > 26) {
		ASSERT(0);
		Capacity = 26;
	}
	SubstrateCount = 0;
	for (int i = 0; i < Capacity; i++) {
		CtMap[i] = Ca.CtMap[i];
		SlotMaP[i] = CSubstObj::SM_EMPTY;
		if (strlen(CtMap[i].LotID)) {
			if (strlen(CtMap[i].SubstID)) {
				SlotMaP[i] = CSubstObj::SM_CORRECTLY_OCCUPIED;
				SubstrateCount++;
			}
		}
		Substrate[i] = Ca.Substrate[i];
		Substrate[i].SlotNo = i + 1;
	}
	return TRUE;
}

void CCarrierObj::GetSlotmapStr(CString& SMp) {
	char SMap[MAXCAPACITYLEN + 2];
	SMap[0] = '0';
	for (int i = 0; i < Capacity; i++) {
		SMap[i] = CSubstObj::GetSMChar(SlotMaP[i]);
	}
	SMap[Capacity] = 0;
	SMp = SMap;
}

CString CCarrierObj::GetIDStatusStr() {
	switch (IDStatus) {
	case IDNOTREAD:
		return "ID NOT READ";
		break;
	case IDWAITINGFORHOST:
		return "ID WAITING FOR HOST";
		break;
	case IDVERIFICATIONOK:
		return "ID VERIFICATION OK";
		break;
	case IDVERIFICATIONFAIL:
		return "ID VERIFICATION FAILED";
		break;
	case IDNOSTATE:
		return "ID NO STATE";
		break;
	};
	return "_";
}

CString CCarrierObj::GetSlotmapStatusStr() {
	return CCarrierObj::GetSlotmapStatusStr(SlotmapStatus);
}

CString CCarrierObj::GetSlotmapStatusStr(SLOTMAPSTATUS state) {
	switch (state) {
	case SMNOTREAD:
		return "SM NOT READ";
		break;
	case SMWAITINGFORHOST:
		return "SM WAITING FOR HOST";
		break;
	case SMVERIFICATIONOK:
		return "SM VERIFICATION OK";
		break;
	case SMVERIFICATIONFAIL:
		return "SM VERIFICATION FAILED";
		break;
	case SMNOSTATE:
		return "SM NO STATE";
		break;
	};
	return "_";
}

CString CCarrierObj::GetAccessingStatusStr() {
	switch (AccessingStatus) {
	case NOTACCESSED:
		return "NOT ACCESS";
		break;
	case INACCESS:
		return "IN ACCESS";
		break;
	case CARRIERCOMPLETE:
		return "CARRIER COMPLETE";
		break;
	case CARRIERSTOPPED:
		return "CARRIER STOPPED";
		break;
	case ASNOSTATE:
		return "NO STATE";
		break;
	};
	return "_";
}

BOOL CCarrierObj::IsSlotmapPresent() {
	int i;
	for (i = 0; i < Capacity; i++) {
		if (SlotMaP[i] == CSubstObj::SM_UNDEFINED) {
			return FALSE;
		}
	}
	for (i = 0; i < Capacity; i++) {
		if (SlotMaP[i] == CSubstObj::SM_CORRECTLY_OCCUPIED) {
			return TRUE;
		}
	}
	return FALSE;
}

void CCarrierObj::SetID(CString ID) {
	// Beware of overflow [8/30/2010 XPMUser]
	if (ID.GetLength() >= OBJIDLEN) {
		ASSERT(0);
		return;
	}
	strcpy(ObjID, ID.GetBuffer(0));
}