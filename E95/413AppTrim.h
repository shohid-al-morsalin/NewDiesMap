#pragma once

// #include "ImgWnd.h"
#include "413AppStep.h"

class CImgWnd;
class CMPoint;
class CChartWnd;
class CRecipe413;

struct SEnt {
	int idx = -1;
	float value = 0;
	SEnt() {};
	SEnt(int i, float val) {
		idx = i;
		value = val;
	}
};

struct STop10 {
	SEnt val[100];

	STop10() {};
	virtual ~STop10() {}
	void PushDown(int n) {
		for (int i = 99; i > n; i--) {
			val[i] = val[i - 1];
		}
	}
	void PushUp(int n) {
		if (n == 0) return;
		for (int i = n; i < 100; i++) {
			val[i - 1] = val[i];
		}
	}
	void AddMax(short index, short value) {
		for (int i = 0; i < 100; i++) {
			if (val[i].idx == -1) {
				val[i] = SEnt(index, value);
				return;
			}
			if (val[i].value < value) {
				PushDown(i);
				val[i] = SEnt(index, value);
				return;
			}
		}
	}
	void AddMin(short index, short value) {
		for (int i = 0; i < 100; i++) {
			if (val[i].idx == -1) {
				val[i] = SEnt(index, value);
				return;
			}
			if (val[i].value > value) {
				PushDown(i);
				val[i] = SEnt(index, value);
				return;
			}
		}
	}
};

struct STrimPARAM {
	short kI;
	CMPoint* pMP;
	CRecipe413* pRcp;

	CSICam* pVideo;
	CImgWnd* pImgWnd;
	CListCtrl* pList;
	CChartWnd* pProfile;

	STrimPARAM() {
		kI = 0;
		pMP = NULL;
		pRcp = NULL;
		pList = NULL;
		pVideo = NULL;
		pImgWnd = NULL;
		pProfile = NULL;
	}
};

class C413AppTrim : public C413AppStep {
	STop10 Max, Min;

	BOOL FindPeaks(short* pBuf, int len);
	BOOL Reduce(short* pBuf, int len);

	int rnd; // Simulation only [4/15/2013 Administrator]
public:
	short MeasureWideTrimWidth(STrimWH& S, CMPoint* pMP, short kI, CRecipe413* pRcp, CImgWnd* pImgWnd1, CSICam* pVideo1, CImgWnd* pImgWnd2, CSICam* pVideo2, CChartWnd* pProfile, CListCtrl* pList);
	short MeasureNarrowTrimWidth(STrimWH& S, CMPoint* pMP, short kI, CRecipe413* pRcp, CImgWnd* pImgWnd1, CImgWnd* pImgWnd2, CSICam* pVideo1, CChartWnd* pProfile, CListCtrl* pList);
	BOOL MeasureTrimWidthOnly(STrimWH& S, float angle, CRecipe413* pRcp, CImgWnd* pImgWnd1, CImgWnd* pImgWnd2, CSICam* pVideo1, CSICam* pVideo2, CListCtrl* pList);
	short MeasureTrimWidthAndHeight(STrimWH& S, CMPoint* pMP, short kI, CRecipe413* pRcp, CImgWnd* pImgWnd1, CImgWnd* pImgWnd2, CSICam* pVideo1, CSICam* pVideo2, CListCtrl* pList);
	void Shrink(short* pBuf, short len);
	void IntegrateAndReverseNegative(short* pBuf, short len);
	short MeasureFirstTrimWidth(STrimPARAM& Param/*, CRecipe413 *pRcp, CMPoint *pMP, short kI, CImgWnd *pImgWnd, CChartWnd *pProfile, CSICam *pVideo*/);
	short MeasureFirstTrimWidth_v2(STrimPARAM& Pa);
	short MeasureSecondTrimWidth(CRecipe413* pRcp, CMPoint* pMP, short kI, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo, CListCtrl* pList);
	BOOL MeasureBevelWidth(CRecipe413* pRcp, CMPoint* pMP, short kI, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo);
	float fMeasureSecondTrimWidthInner(/*float *pfTrimWidth, float radius, */float radius, float angle, int pN, CRecipe413* pRcp, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo, CListCtrl* pList);
	float fMeasureSecondTrimWidthOuter(/*float *pfTrimWidth, float radius2,*/ float angle, int pN, CRecipe413* pRcp, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo, CListCtrl* pList);
	BOOL MeasureBevel(CRecipe413* pRcp, CMPoint* p, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile, CListCtrl* pList);
	float GetBevelWidth(short* pBuf, short len, float ang, BOOL bX, short& left, short& right);
	float GetBevelWidth2(short* pBuf, short len, float ang, BOOL bX, short& left, short& right);
	BOOL GetBevelWidth(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile, CListCtrl* pList);
	BOOL MeasureTrim(CRecipe413* pRcp, CMPoint* p, CCamBase::CAMERAID ActiveCamera, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile, CListCtrl* pList);
	float CalcTrimWidth(short** pBuf, short len, float& ang, BOOL& bX, short& left, short& right);
	float GetTrimWidth(short* pBuf, short len, float ang, BOOL bX, short& left, short& right);
	float GetTrimWidth1(short* pBuf, short len, float ang, BOOL bX, short& left, short& right);
	BOOL GetTrimWidthHeight(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CChartWnd* pProfile, CListCtrl* pList);
	void PreprocessBevelData(short* pBuf, short len, short nSmooth, BOOL thresh, CString tm);
	void ThresholdTrimData(short* pBuf, short len, short val, CString tm);
	// [6/14/2022 ZHIMING]
	BOOL MeasureBevelWidthUsingTF(CRecipe413* pRcp, CMPoint* pMP, short kI, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo);
	// 6/15/2022 Manjurul
	BOOL MeasureBevelWidth_v1(CRecipe413* pRcp, CMPoint* pMP, short kI, CImgWnd* pImgWnd, CChartWnd* pProfile, CSICam* pVideo);
	// ]

	C413AppTrim();
	virtual ~C413AppTrim();
};
