#include "stdafx.h"

#include "CamSi.h"
#include "siusb.h"

CCamSi::CCamSi() {
	CamType = TYSICAM;
}

CCamSi::~CCamSi() {}

BOOL CCamSi::SetExposure(short iCNum, short iExpose) {
	SI_SetExposureValue(iCNum, iExpose);
	return TRUE;
}

BOOL CCamSi::SetAutoMode(short iCNum, short Prop) {
	SI_SetAutoModes(iCNum, Prop);
	return TRUE;
}

BOOL CCamSi::Capture(SIU_CaptureContext* pCapCxt) {
	if (SI_CaptureDIB(pCapCxt) == SR_OK) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCamSi::SetGlobalGain(short iCNum, int nGain) {
	SI_SetGlobalGainB(iCNum, nGain);
	return TRUE;
}

BOOL CCamSi::ReadConfig(const char* lpszFileName, SIU_Config* pCfg, CCamBase::CAMERAID ID, short iCNum) {
	if (SI_ReadConfig(lpszFileName, pCfg) == SR_OK) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCamSi::BeginCapture(const SIU_Config* pCfg, SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID) {
	if (SI_BeginCapture(pCfg, pCxt) == SR_OK) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCamSi::EndCapture(SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID) {
	if (SI_EndCapture(pCxt) == SR_OK) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCamSi::Open(short iCNum, SIU_Config* pCfg) {
	return TRUE;
}

BOOL CCamSi::Close(short iCNum) {
	return TRUE;
}