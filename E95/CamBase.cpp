#include "stdafx.h"

#include "CamBase.h"

CCamBase::CCamBase() {
	ID = SIMULCAM;
	CamType = TYSIMUCAM;
}

CCamBase::~CCamBase() {}

BOOL CCamBase::SetExposure(short CamNum, short Exposure) {
	return TRUE;
}

BOOL CCamBase::SetAutoMode(short CamNum, short Prop) {
	return TRUE;
}

BOOL CCamBase::Capture(SIU_CaptureContext* pCxt) {
	return TRUE;
}

BOOL CCamBase::SetGlobalGain(short iCNum, int Gain) {
	return TRUE;
}

BOOL CCamBase::ReadConfig(const char* lpszFileName, SIU_Config* pCfg, CCamBase::CAMERAID ID, short iCNum) {
	return TRUE;
}

BOOL CCamBase::BeginCapture(const SIU_Config* pCfg, SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID) {
	return TRUE;
}

BOOL CCamBase::EndCapture(SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID) {
	return TRUE;
}

BOOL CCamBase::Open(short iCNum, SIU_Config* pCfg) {
	return TRUE;
}

BOOL CCamBase::Close(short iCNum) {
	return TRUE;
}

BOOL CCamBase::IsSimu() {
	if (CamType == TYSIMUCAM) {
		return TRUE;
	}
	return FALSE;
}