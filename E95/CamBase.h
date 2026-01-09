#pragma once

#include "sicap.h"

class CCamBase {
public:
	BOOL IsSimu();
	virtual BOOL Close(short iCNum);
	virtual BOOL Open(short iCNum, SIU_Config* pCfg);
	enum CAMTYPE {
		TYSIMUCAM, TYSICAM, TYLUMICAM
	};
	CAMTYPE CamType;

	enum CAMERAID {
		ECHOCAM, THINFCAM, CWL, ROUGH, SIMULCAM, LASTCAM
	};
	CAMERAID ID;

	virtual BOOL EndCapture(SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID);
	virtual BOOL BeginCapture(const SIU_Config* pCfg, SIU_CaptureContext* pCxt, CCamBase::CAMERAID ID);
	virtual BOOL ReadConfig(const char* lpszFileName, SIU_Config* pCfg, CCamBase::CAMERAID ID, short iCNum);
	virtual BOOL SetGlobalGain(short iCNum, int Gain);
	virtual BOOL Capture(SIU_CaptureContext* pCxt);
	virtual BOOL SetAutoMode(short CamNum, short Prop);
	virtual BOOL SetExposure(short CamNum, short Exposure);

	CCamBase();
	virtual ~CCamBase();
};
