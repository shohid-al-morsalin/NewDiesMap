// SecsParamItem.h: interface for the CSecsParamItem class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#define MAXNAMELENGTH	64
#define MAXDESCRIPLEN	128
#define MAXVALUELENGTH	24

class CSecsParamItem {
	char str[128];
	CString inifile;
public:
	short type;
	char* GetRMinChar();
	char* GetRMaxChar();
	void LoadParam();
	void SaveParam();
	char* GetVStr();
	char Name[MAXNAMELENGTH];
	union {
		int iValue;
		char cValue[MAXVALUELENGTH];
	};
	int RMax, RMin;
	char Description[MAXDESCRIPLEN + 1];

	char* GetiValueChar();
	void Set(char* name, char* desc, int value, int min, int max);
	void Set(char* name, char* desc, DWORD value, int min, int max);
	void Set(char* name, char* desc, char* value, int min, int max);
	void Set(CString& pName, CString& pVal, CString& Min, CString& Max);

	CSecsParamItem();
	virtual ~CSecsParamItem();
};