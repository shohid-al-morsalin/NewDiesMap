#pragma once

#include "CamBase.h"

class CCamSi : public CCamBase {
public:
	BOOL Close(short iCNum);
	BOOL Open(short iCNum, SIU_Config* pCfg);
	BOOL EndCapture(SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID);
	BOOL BeginCapture(const SIU_Config* pCfg, SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID);
	BOOL ReadConfig(const char* lpszFileName, SIU_Config* pCfg, CCamBase::CAMERAID ID, short iCNum);
	BOOL SetGlobalGain(short iCNum, int Gain);
	BOOL Capture(SIU_CaptureContext* pCxt);
	BOOL SetAutoMode(short CamNum, short Prop);
	BOOL SetExposure(short CamNum, short Exposure);

	CCamSi();
	virtual ~CCamSi();
};