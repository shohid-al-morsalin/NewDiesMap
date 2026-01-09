#pragma once

#include "CamBase.h"
#include "413/Lucam/lucamapi.h"
#include "413/Lucam/lucamerr.h"

class CCamLumi : public CCamBase {
	BOOL bIsMono;
	HANDLE hCamera;
	ULONG ulCameraId;
	LUCAM_SNAPSHOT lsSnapshot;
	LUCAM_FRAME_FORMAT lffFormat;
	FLOAT fFrameRate;
	BYTE* pFrame;
	long bpr;

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

	CCamLumi();
	virtual ~CCamLumi();
};