#pragma once

#include "413AppTrimStep.h"

class CThkValues;

// Structure to transfer trim width and height data [4/14/2013 Administrator]
struct STrimWH {
	union {
		float Rout;
		float TW1;
	};
	union {
		float Oout;
		float TW2;
	};
	short n;
	short nPt; // Number of sample point [7/15/2013 Yuen]
	float Rin, Oin;
	float TThk, Hgt;
	float TTin, TTout;	// Total thickness [4/14/2013 Administrator]
	float HTin, HTout;	// Step height [4/14/2013 Administrator]

	STrimWH() {
		n = 0;
		HTin = HTout = TTin = TTout = 0;
		Rout = Rin = Oout = Oin = 0;
		TThk = Hgt = 0;
	}
};

class C413AppStep : public C413AppTrimStep {
public:
	float GetRadiusOfEdge(CRecipe413* pRcp, BOOL bInner, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	float GetRadiusOfInnerEdge(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	float GetRadiusOfOuterEdge(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL GetStepHeightThickness(STrimWH& S, CRecipe413* pRcp, CMPoint* pMP, float radius, float angle, float GageThickness, CSICam* pVideo, CListCtrl* pList);
	BOOL GetStepHeightThicknessA(STrimWH& S, CRecipe413* pRcp, CMPoint* pMP, float radius, float angle, float GageThickness, CSICam* pVideo, CListCtrl* pList);
	BOOL GetStepHeightOuter(CThkValues& Thk, float* pGageThickness, CRecipe413* pRcp, MParam* pM, CMPoint* pMP, CMeParam* pMeP, float radius, float angle, CPeaks& Pk, CSICam* pVideo, CListCtrl* pList);
	BOOL GetStepHeightInner(CThkValues& Thk, float GageThickness, CRecipe413* pRcp, MParam* pM, CMPoint* pMP, CMeParam* pMeP, float radius, float angle, CPeaks& Pk, CSICam* pVideo, CListCtrl* pList);
	BOOL MeasureStepHeight(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, short kI, CImgWnd* pImgWnd, CListCtrl* pList);
	BOOL MeasureStepHeightG(CRecipe413* pRcp, CMPoint* pMP, CSICam* pVideo, int kI, CListCtrl* pList);
	BOOL GetStepHeightG(float& Thk, CRecipe413* pRcp, MParam* pM, CMPoint* pMP, CMeParam* pMeP, float x, float y, CPeaks& Pk, CSICam* pVideo, CListCtrl* pList);

	C413AppStep();
	virtual ~C413AppStep();
};
