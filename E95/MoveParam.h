// MoveParam.h: interface for the CMoveParam class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "..\413\Dmccom.h"	// Added by ClassView

class CMoveParam {
public:
	float X, Y;
	BOOL bRet;
	short Ret;
	HANDLE h;
	DWORD id, dw;
	LPVOID pObj;
	CMoveParam();
	virtual ~CMoveParam();
};