#include "stdafx.h"
#include "strSOp.h"
#include "ContentMap.h"

void CContentMap::Preset() {
	ZeroMemory(LotID, LOTIDLEN + 1);
	ZeroMemory(SubstID, SUBSTIDLEN + 1);
}

BOOL CContentMap::GetAttr(char* attrname, char* attr, short len) {
	CString name;
	name = attrname;
	if (len < LOTIDLEN) {
		if (name == "LotID") {
			strscpy(attr, len, LotID);
			return TRUE;
		}
	}
	if (len < SUBSTIDLEN) {
		if (name == "SubstID") {
			strscpy(attr, len, SubstID);
			return TRUE;
		}
	}
	*attr = 0;
	return FALSE;
}

BOOL CContentMap::SetAttr(char* attrname, char* attr) {
	CString name;
	name = attrname;
	if (name == "LotID") {
		strscpy(LotID, LOTIDLEN, attr);
		return TRUE;
	}
	if (name == "SubstID") {
		strscpy(SubstID, SUBSTIDLEN, attr);
		return TRUE;
	}
	return FALSE;
}

void CContentMap::Set(CContentMap* pCt) {
	if (pCt) {
		strcpy(LotID, pCt->LotID);
		strcpy(SubstID, pCt->SubstID);
	}
}