#pragma once

#include "413AppPatt.h"

class CPeakParam;

class C413AppEcho : public C413AppPatt {
public:
	BOOL GetDMCStatus();
	void AutoFocusEcho(CRecipe413* pRcp, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, float fReff, CImgWnd* pImgWnd = NULL);
	BOOL DoDualProbeCalibration(float CalibThickness, float DPZPos, CRecipe413* pRcp, short MeSet, CListCtrl* pList, CSICam* pVideo, CPeakParam* pParam, BOOL bResetX, short SimuPk);
	BOOL GetSubstrateThicknesses(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList, CSICam* pVideo);
	void ResetEchoSamplingProperty(CRecipe413* pRcp, CListCtrl* pList);
	float GetBowWarpRef(CPeaks& Pk, MParam* p, CMPoint* pMP, CListCtrl* pList);
	float GetTopProbeRef(CPeaks& Pk, MParam* p, CMPoint* pMP, CListCtrl* pList);

	// [09042024 ZHIMING
	BOOL WaferOnStageCheck(CRecipe413* pRcp, bool* waferOnStage, CSICam* pVideo = NULL, CListCtrl* pList = NULL);
	// ]

	C413AppEcho();
	virtual ~C413AppEcho();
};
