// SecsParamItem.cpp: implementation of the CSecsParamItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "strSOp.h"
#include "SecsParamItem.h"

#include "..\SRC\DOSUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSecsParamItem::CSecsParamItem() {
	type = 0;  // 0 = not define [7/27/2010 Yuen]
	DosUtil.GetLocalCfgFile(inifile);
}

CSecsParamItem::~CSecsParamItem() {}

char* CSecsParamItem::GetVStr() {
	if (type == 1) {
		sprintf(str, "%d", iValue);
		return str;
	}
	return cValue;
}

void CSecsParamItem::SaveParam() {
	char str[128];

	WritePrivateProfileString(Name, "Description", Description, inifile);
	switch (type) {
	case 1:
	case 3:
		sprintf(str, "%d", iValue);
		WritePrivateProfileString(Name, "Value", str, inifile);
		break;
	default:
		WritePrivateProfileString(Name, "Value", cValue, inifile);
		break;
	}
	sprintf(str, "%d", RMin);
	WritePrivateProfileString(Name, "Minimum", str, inifile);
	sprintf(str, "%d", RMax);
	WritePrivateProfileString(Name, "Maximum", str, inifile);
}

void CSecsParamItem::LoadParam() {
	char str[128], str2[128];

	GetPrivateProfileString(Name, "Description", Description, Description, sizeof(Description), inifile);
	if (type == 1) {
		sprintf(str2, "%d", iValue);
		GetPrivateProfileString(Name, "Value", str2, str, sizeof(str2), inifile);
		iValue = atoi(str);
	}
	else {
		GetPrivateProfileString(Name, "Value", cValue, cValue, sizeof(cValue), inifile);
	}

	sprintf(str2, "%d", RMin);
	GetPrivateProfileString(Name, "Minimum", str2, str, sizeof(str2), inifile);
	RMin = atof(str);

	sprintf(str2, "%d", RMax);
	GetPrivateProfileString(Name, "Maximum", str2, str, sizeof(str2), inifile);
	RMax = atof(str);
}

char* CSecsParamItem::GetRMaxChar() {
	sprintf(str, "%d", RMax);
	return str;
}

char* CSecsParamItem::GetRMinChar() {
	sprintf(str, "%d", RMin);
	return str;
}

char* CSecsParamItem::GetiValueChar() {
	sprintf(str, "%d", iValue);
	return str;
}

void CSecsParamItem::Set(char* name, char* desc, int value, int min, int max) {
	type = 1;  // Integer [7/27/2010 Yuen]
	if (Name && (strlen(name) < MAXNAMELENGTH)) {
		strscpy(Name, MAXNAMELENGTH, name);
	}

	if (desc && (strlen(desc) < MAXDESCRIPLEN)) {
		strscpy(Description, MAXDESCRIPLEN, desc);
	}

	iValue = value;
	RMax = max;
	RMin = min;
}

void CSecsParamItem::Set(char* name, char* desc, DWORD value, int min, int max) {
	type = 3;  // DWORD [7/27/2010 Yuen]
	if (name && (strlen(name) < MAXNAMELENGTH)) {
		strscpy(Name, MAXNAMELENGTH, name);
	}

	if (desc && (strlen(desc) < MAXDESCRIPLEN)) {
		strscpy(Description, MAXDESCRIPLEN, desc);
	}

	iValue = value;
	RMax = max;
	RMin = min;
}

void CSecsParamItem::Set(char* name, char* desc, char* value, int min, int max) {
	type = 2;  // char [7/27/2010 Yuen]
	if (name && (strlen(name) < MAXNAMELENGTH)) {
		strscpy(Name, MAXNAMELENGTH, name);
	}

	if (desc && (strlen(desc) < MAXDESCRIPLEN)) {
		strscpy(Description, MAXDESCRIPLEN, desc);
	}

	if (value && (strlen(value) < MAXVALUELENGTH)) {
		strscpy(cValue, MAXVALUELENGTH, value);
	}
	RMax = max;
	RMin = min;
}

void CSecsParamItem::Set(CString& pName, CString& pVal, CString& Min, CString& Max) {
	Set(pName.GetBuffer(0), Description, pVal.GetBuffer(0), atoi(Min), atoi(Max));
}