#pragma once

#include "413AppMtr.h"

#include "PattMatch.h"

struct SDoMatchPattern {
	CPackedDIB* pSourceDIB;
	CPackedDIB* pMatchDIB, * pMatchDibA, * pMatchDibB;
	int startX, endX, startY, endY;
	int dI, dT;
	int sourceWd, sourceHt;	// source image dimension [5/25/2014 Yuen]
	int wdA, htA, wdB, htB;		// template image dimension [5/25/2014 Yuen]
	BOOL bFast;		// True will process matching in green band only [5/16/2013 user]
	float GoF;

	SDoMatchPattern();
	BOOL SetSourceDIB(CPackedDIB* pDIB);
	void SetMatchDIBA(CPackedDIB* pDIB, float X, float Y);
	void SetMatchDIBB(CPackedDIB* pDIB, float X, float Y);
	BOOL GetWidthHeight();
	BOOL IsValid();
	BOOL ValidCheckdTdI();
	BOOL SetMatchRegionA(BOOL bReduceArea);
	BOOL SetMatchRegionB();
};

struct SDoPattrnMatching {
	SDoMatchPattern Par;

	CMPoint* pMP;
	CSICam* pVideo;
	CImgWnd* pImgWnd;
	CListCtrl* pList;
	CRecipe413* pRcp;

	CString CarrierID, RcpeName;
	short SlotID;

	short MeSet;
	short MatchRes; // Param.pRcp->MatchRes[Param.PaSet-1] [6/8/2014 Yuen]
	short PaSet, PaSSet;
	short MatchAlgorithm; // Param.pRcp->MatchAlgorithm[Param.PaSet-1] [6/8/2014 Yuen]
	short SearchDepth;	// Param.pRcp->SearchDept[Param.PaSet-1] [6/8/2014 Yuen]
	float SearchDistance; // Param.pRcp->SearchDist[Param.PaSet-1] [6/8/2014 Yuen]
	int PatMatOpt; // Param.pRcp->PatMatOpt[Param.PaSet-1] [6/8/2014 Yuen]

	short nTrialPat;
	float fStepSizePat;

	BOOL bSaveVideo;
	BOOL bMatchOnceX;
	BOOL bApplyOffset, bMoveCenter;

	float POffsetX; // Param.pRcp->POffsetX[Param.PaSet-1][n] [6/8/2014 Yuen]
	float POffsetY; // Param.pRcp->POffsetY[Param.PaSet-1][n] [6/8/2014 Yuen]

	// Return values [6/13/2013 Yuen]
	float initX, initY;	// mm, Initial motor position at start of matching process [6/25/2013 FSM413]
	float finalX, finalY;	// mm, Final motor position at completion of matching process [6/25/2013 FSM413]

	float PatOffX, PatOffY;
	SLocMatch MatchPos;	// pix [6/27/2013 Yuen]
	SLocMatch ConfPos;	// pix [6/27/2013 Yuen]
	SLocMatch OffDist;	// pix, offset distance of match pattern from center of view [6/27/2013 Yuen]
	SLocMatch MoveDist;	// pix, Offset distance of match pattern from green XHair [6/25/2013 FSM413]

	// For use in thread communication [8/7/2013 Yuen]
	BOOL bRetCode;
	LPVOID pObj;	// Object where function can be executed [8/7/2013 Yuen]

	void Set(CSICam* pVid, CImgWnd* pImgW, CListCtrl* pLCtr) {
		pVideo = pVid;
		pImgWnd = pImgW;
		pList = pLCtr;
	}

	SDoPattrnMatching() {
		MeSet = -1;
		PaSet = -1;
		PaSSet = 0;
		MatchRes = -1;
		PatMatOpt = -1;
		MatchAlgorithm = -1;
		SearchDepth = -1;
		SearchDistance = -1;

		bSaveVideo = FALSE;
		bMatchOnceX = FALSE;
		bApplyOffset = TRUE;
		bMoveCenter = TRUE;

		initX = initY = 0;
		finalX = finalY = 0;
		PatOffX = PatOffY = 0;

		nTrialPat = 1;
		fStepSizePat = 0;

		POffsetX = POffsetY = -9999;

		SlotID = 0;
		CarrierID = "";
		RcpeName = "";

		pMP = NULL;
		pRcp = NULL;
		pList = NULL;
		pVideo = NULL;
		pImgWnd = NULL;
	};
};

class C413AppPatt : public C413AppMtr {
public:
	void SetupParam(CRecipe413* pRcp, SDoPattrnMatching& Param, short PaSSet);
	BOOL SetMatchDIBB(CRecipe413* pRcp, SDoPattrnMatching& Param);
	BOOL SetMatchDIBA(CRecipe413* pRcp, SDoPattrnMatching& Param);
	void MoveMatchPatToRedCross(SDoPattrnMatching& Param);
	enum PATMATCHRTN {
		pmOK, pmFAILED, pmMOVE, pmMOVES, pmABORT, pmTOOFAR, pmFAILLIMIT, pmNOMORE
	};

	PATMATCHRTN DoMatchingC(SDoPattrnMatching& Param);
	short DoPatternMatchingWithRetrialTF(SDoPattrnMatching& Param, CRecipe413* pRcp);
	void PreprocessAfterMatching(CRecipe413* pRcp, short CurPaSet);
	void PreprocessBeforeMatching(CRecipe413* pRcp, short CurPaSet);
	void MoveToOffsetMatchB(short n, SDoPattrnMatching& Param);
	void MoveToOffsetMatchA(short n, SDoPattrnMatching& Param);
	BOOL MoveMatchPatCenter(SDoPattrnMatching& Param);
	BOOL GetConfirmPosition(SDoPattrnMatching& Param, SLocMatch& SizeD, SLocMatch& SizeC);
	BOOL GetMatchPosition(SLocMatch& SizeC, SDoPattrnMatching& Param, BOOL bReduceArea);

	BOOL DoMatchPattern(SLocMatch& Loc, SDoPattrnMatching& Param);

	BOOL Match(SDoPattrnMatching& Param, SLocMatch& SizeC, BOOL bReduceArea);
	BOOL Match2(SDoPattrnMatching& Param, SLocMatch& SizeC);
	BOOL Match3(SDoPattrnMatching& Param, SLocMatch& SizeD, SLocMatch& SizeC);

	BOOL ShrinkStoredDIB(tagSIPackedDIB* pDIB, CImgWnd* pImgWnd);
	void ShrinkMatchDIB(int n, tagSIPackedDIB* pDIB, SLocMatch& SizeC, CImgWnd* pImgWnd);

	C413AppPatt::PATMATCHRTN DoPattrnMatching(SDoPattrnMatching& Param);
	C413AppPatt::PATMATCHRTN DoMatchingA(SDoPattrnMatching& Param);
	C413AppPatt::PATMATCHRTN DoMatchingB(SDoPattrnMatching& Param);

	double GetScore(tagSIPackedDIB* pDIBi, tagSIPackedDIB* pDIB);

	// 	BOOL SaveSpectrum(CString fname);

	void CreateHalfSizeDIB(tagSIPackedDIB* pDIB, BYTE* pBuf, long SizeImage);

	C413AppPatt();
	virtual ~C413AppPatt();
};
