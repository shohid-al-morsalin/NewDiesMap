// OSSBASE.h: interface for the COSSBASE class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "IPC/OSSMemS.h"

class COSSBASE : public CObject {
public:
	CStringArray	m_AttrIDName;
	char			m_ObjID[OBJIDLEN + 1];
	char			m_ObjType[OBJTYPELEN + 1];

public:
	//======================================================
	// Pure Virtual Function
	virtual BOOL  GetAttr(CString service, void* buf);
	virtual BOOL  SetAttr(CString service, void* val);
	virtual void  GetAttrName(CStringArray& nArray) = 0;
	virtual void  ClearAll() = 0;

	//======================================================
	// Basic function Function
	char* GetType();

public:
	//virtual char* test( CString test ) = 0;
	COSSBASE();
	virtual ~COSSBASE();
};