// DMCMotorBase.cpp: implementation of the CDMCMotorBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DMCMotorBase.h"
#include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDMCMotorBase::CDMCMotorBase() {
	MotorFile = "";
	bRun = FALSE;
}

CDMCMotorBase::~CDMCMotorBase() {}

BOOL CDMCMotorBase::Run() {
	bRun = TRUE;
	return TRUE;
}

BOOL CDMCMotorBase::Stop() {
	bRun = FALSE;
	return TRUE;
}

BOOL CDMCMotorBase::Reload(CString Filename) {
	return TRUE;
}

BOOL CDMCMotorBase::Initialize(CString Filename, CListBox* pMessage) {
	pMessage->InsertString(pMessage->GetCount(), "Echo Motion Controller (simu) started OK");
	return TRUE;
}

BOOL CDMCMotorBase::Deinitialize() {
	return TRUE;
}

void CDMCMotorBase::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CDMCMotorBase");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CDMCMotorBase::LoadProgramA() {
	return TRUE;
}

BOOL CDMCMotorBase::LoadProgram(CString file) {
	MotorFile = file;
	return LoadProgramA();
}

BOOL CDMCMotorBase::Send(char* cmd) {
	return TRUE;
}