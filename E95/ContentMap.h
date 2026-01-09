#pragma once

#include "IPC/Constant.h"
#include "IPC/SubstObj.h"

class CContentMap {
public:
	char LotID[LOTIDLEN + 1];
	char SubstID[SUBSTIDLEN + 1];
public:
	void Preset();
	BOOL GetAttr(char* attrname, char* attr, short len);
	BOOL SetAttr(char* attrname, char* attr);
public:
	void Set(CContentMap* pCt);
};