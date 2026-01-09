#pragma once

#include "ContentMap.h"
#include "IPC/OSSMemS.h"

class CCarrierObj {
public:
	char ObjID[OBJIDLEN + 1];
	char ObjType[OBJTYPELEN + 1];
	CSubstObj Substrate[MAXCAPACITYLEN + 1];
	char Usage[USAGELEN + 1];
	short Capacity;
	char LocationID[SUBSTLOCIDLEN + 1];
	short SubstrateCount;
	char SlotMaP[MAXCAPACITYLEN + 1];
	CContentMap CtMap[MAXCAPACITYLEN + 1];

	enum IDSTATUS {
		IDNOSTATE, IDNOTREAD, IDWAITINGFORHOST, IDVERIFICATIONOK, IDVERIFICATIONFAIL, igIDSTATUS
	};

	IDSTATUS IDStatus;

	enum SLOTMAPSTATUS {
		SMNOSTATE, SMNOTREAD, SMWAITINGFORHOST, SMVERIFICATIONOK, SMVERIFICATIONFAIL, igSMSTATUS
	};

	SLOTMAPSTATUS SlotmapStatus;

	enum ACCESSINGSTATUS {
		ASNOSTATE, NOTACCESSED, INACCESS, CARRIERCOMPLETE, CARRIERSTOPPED, igACCESSINGSTATUS
	};

	ACCESSINGSTATUS AccessingStatus;
public:
	void SetID(CString ID);
	BOOL IsSlotmapPresent();
	CString GetAccessingStatusStr();
	CString GetSlotmapStatusStr();
	static CString GetSlotmapStatusStr(SLOTMAPSTATUS state);
	CString GetIDStatusStr();
	void GetSlotmapStr(CString& Slotmap);
	BOOL UpdateAfterProceedWithCarrierID(CCarrierObj& Ca);
	void MakeSlotmapFromContentMap();
	BOOL SetAttr(char* attrname, char* attr);
	void ClearCt();
	void Preset();
	char* GetObjType();
	BOOL GetAttr(char* attrname, char* attr, short len);
};
