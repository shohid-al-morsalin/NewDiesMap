// DMCMotor.h: interface for the CDMCMotor class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "Dmccom.h"
#include "DMCMotorBase.h"

class CDMCMotor : public CDMCMotorBase {
	CString FailStopPrgm;
	CString FailExePrgm;
	CString FailDnldPrgm;

public:
	BOOL Send(char* cmd);
	BOOL LoadProgramA();

	HANDLEDMC   hDmc;

	BOOL Deinitialize();
	BOOL Initialize(CString Filename, CListBox* pMessage);
	BOOL Reload(CString Filename);
	BOOL Stop();
	BOOL Run();
	CDMCMotor();
	virtual ~CDMCMotor();
};
