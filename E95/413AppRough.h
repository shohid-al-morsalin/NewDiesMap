#pragma once
#include "413AppThinF.h"

// C413AppRough -- Added By Mahedi Kamal - 03202024
// Add C413AppRough in the class architecture with its module function like calibration, measurement, calculation -- comment from Zhiming


class C413AppRough : public C413AppThinF
{
public:
	bool bIsRa;
	BOOL GetSubstrateRoughness(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList);
	C413AppRough() {
		bIsRa = false;
	}
	virtual C413AppRough::~C413AppRough() {}
	BOOL PerformRoughnessCalibration(CRecipe413* pRcp, CListCtrl* pList = NULL);
	BOOL MeasureRoughMPList(CRecipe413* pRcp, CPtrList* pMPList/*, GEM::SGEMCtx* pCtx*/);
	BOOL MeasureOnePointRough(CRecipe413* pRcp, CMPoint* p, short NAver, BOOL bSettling);
	BOOL DoOnePointRoughnessMeasurement(CRecipe413* pRcp, CMPoint* p, short NAver);
	void Log(CString msg, short lvl = 5);
	BOOL GotoRoughnessCalibrationPos();
};
