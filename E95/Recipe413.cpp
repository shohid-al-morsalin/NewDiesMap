// Recipe413.cpp: implementation of the CRecipe413 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "strSOp.h"
#include "Recipe413.h"

#include "413App.h"
#include "Data.h"
#include "MPoint.h"

#include "..\SRC\BMake.h"
#include "..\SRC\DOSUtil.h"
#include "..\IPC\OSSMemS.h"

#include "GlobalDeviceContainer.h"

#include <sstream>
#include <algorithm>

#include<fstream> // 09292025
#include<sstream> // 09292025
 

// 10312025_1 MORSALIN change to 110
#define CURRENTRECIPEVERSION	110	//20210329 96->97//20220615 97->98//01312024 99//05292024 //07102024 101->102 //08142024 102 -> 103// 103->104(added autofocus008) // 104->105(20250306) //105->106(10032025_3) // 106->107 (09232025_Shamim)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
string PPExecName = "";
string PPChangeName = "";
BYTE PPChangeStatus = 0;

CRecipe413::CRecipe413() {
	bIsRa_rcp = false; // must be assign false, else bug //  07102024 -> Mahedi Kamal
	bIsRaCWL_rcp = TRUE; // 12120224
	bIsNmCWL = TRUE; //20250306
	RoughStepSizeCWL_rcp = 0; // 12120224
	RoughScanLineLenCWL_rcp = 0;
	bIsAutoFocus008 = TRUE;
	m_Imagedmy = NULL;

	ccrpatt.pPatt = &Patt;
	rdlpatt.pPatt = &Patt;
	custpatt.pPatt = &Patt;
	gridpatt.pPatt = &Patt;
	CrossPattern.pPatt = &Patt;
	MaskPattern.pPatt = &Patt;
	isRangeType = false;//17122024
	ThicknessWithScan = TRUE; // 09232025_Shamim
	Clear();

	srand((unsigned)time(NULL));
}

CRecipe413::~CRecipe413() {
	if (m_Imagedmy) {
		delete m_Imagedmy;
		m_Imagedmy = NULL;
	}
}

BOOL CRecipe413::SaveRecipe(BOOL bSortPrompt) {
	BOOL bSort = FALSE;
	if (bSortPrompt && (!IsMarkers())) {
		if (AfxMessageBox("Sort data?", MB_YESNO) == IDYES) {
			bSort = TRUE;
		}
	}
	CFileDialog dlg(FALSE,
		"RCP",
		NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Recipe file|*.RCP|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetRecipeDir();
	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		BOOL ret = SaveRecipe(filename, bSort);
		// [02282024 ZHIMING
		CString str = _T("FSM 413C2C Control Module : ");
		if (pGDev->SystemModel == pGDev->NMODEL::FSM413SA) {
			str = _T("FSM 413SA Control Module : ");
		}
		else if (pGDev->SystemModel == pGDev->NMODEL::FSM413MOT) {
			str = _T("FSM 413MOT Control Module : ");
		}
		str.AppendFormat("[%s]", RcpeName);
		// ]
		AfxGetMainWnd()->SetWindowText(str);
		return ret;
	}
	return FALSE;
}

BOOL CRecipe413::LoadRecipe() {
	CFileDialog dlg(TRUE,
		"RCP",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT,
		"Recipe file|*.RCP|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetRecipeDir();

	if (dlg.DoModal() == IDOK) {
		filename = dlg.GetPathName();
		BOOL ret = Load(filename);
		// [02282024 ZHIMING
		CString str = _T("FSM 413C2C Control Module : ");
		if (pGDev->SystemModel == pGDev->NMODEL::FSM413SA) {
			str = _T("FSM 413SA Control Module : ");
		}
		else if (pGDev->SystemModel == pGDev->NMODEL::FSM413MOT) {
			str = _T("FSM 413MOT Control Module : ");
		}
		str.AppendFormat("[%s]", RcpeName);
		// ]
		AfxGetMainWnd()->SetWindowText(str);
		return ret;
	}
	return FALSE;
}

BOOL CRecipe413::LoadRecipeByPath() {
	CFileDialog dlg(TRUE,
		"RCP",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT,
		"Recipe file|*.RCP|");

	CString filename = "";
	dlg.m_ofn.lpstrInitialDir = DosUtil.GetRecipeDir();

	if (dlg.DoModal() == IDCANCEL) return FALSE;
	POSITION pos = dlg.GetStartPosition();

	while (pos) {
		filename = dlg.GetNextPathName(pos); if (!filename) break;
		BOOL ret = Load(filename); if (!ret) break;
		// [02282024 ZHIMING
		CString str = _T("FSM 413C2C Control Module : ");
		if (pGDev->SystemModel == pGDev->NMODEL::FSM413SA) {
			str = _T("FSM 413SA Control Module : ");
		}
		else if (pGDev->SystemModel == pGDev->NMODEL::FSM413MOT) {
			str = _T("FSM 413MOT Control Module : ");
		}
		str.AppendFormat("[%s]", RcpeName);
		// ]
		AfxGetMainWnd()->SetWindowText(str);
		ret = SaveRecipe(filename, FALSE); if (!ret) break;
	}

	return FALSE;
}

// [04222022 ZHIMING
void CRecipe413::ElliminateOutOfChuck() {
	Patt.EliminateOutOfChuck(this);
}
// ]

void CRecipe413::Clear() {
	int i, j;

	Version = 0;
	nTrimMethod = 0; // Default to ExtractLine [5/20/2014 FSMT]
	NMeasureIn = 1; // Must be one or greater [8/31/2011 XPMUser]
	NMeasureOut = 3; // Must be one or greater [8/31/2011 XPMUser]
	m_Imagedmy = NULL;
	m_ImgSizedmy = 0;;
	nMappedPoints = 65536;
	EchoFocus = 0;

	fTGtRedXdmy = fTGtRedYdmy = 0.5f;  // Set to middle of screen [9/12/2011 Administrator]
	for (i = 0; i < MAXMATCHPATT; i++) {
		for (j = 0; j < MAXTGTSET; j++) {
			fTgtGRnX[i][j] = fTgtGRnY[i][j] = 0.5f;  // Set to middle of screen [9/12/2011 Administrator]
			fTgtBLuX[i][j] = fTgtBLuY[i][j] = 0.5f;  // Set to middle of screen [9/12/2011 Administrator]
		}
	}
	bEnableAutoZ = FALSE;
	fAutoZValue = 0;
	FindTrimThres = 0.08f;
	ContrastLevel = 0.0f;	//20210329
	FindBevelThres = 3.5;	//20220615 ZHIMING
	SamplePerPoint = 1;

	Desc.Clear();
	Wp.Clear();
	ccrpatt.ClearList();
	rdlpatt.ClearList();
	gridpatt.ClearList();
	custpatt.ClearList();
	Patt.ClearData();
	Me.Clear();
	BatchDoc.Clear();

	TRWMeasProp.Clear(); //08142024

	for (i = 0; i < MAXMATCHPATT; i++) {
		MatchDIBA1[i].ClearDIB();
		MatchDIBB1[i].ClearDIB();
		MatchDIBA2[i].ClearDIB();
		MatchDIBB2[i].ClearDIB();
		MatchDIBA3[i].ClearDIB();
		MatchDIBB3[i].ClearDIB();
		MatchDIBA4[i].ClearDIB();
		MatchDIBB4[i].ClearDIB();
	}

	for (j = 0; j < MAXMEPASET; j++) {
		for (i = 0; i < MaXPA; i++) {
			MeParam[j].MPa[i].Clear();
		}
		MeParam[j].PSG[0].Set(0, 70, 180, 32, 6, 0, 0, FALSE, 0);
		MeParam[j].PSG[1].Set(0, 70, 180, 32, 6, 0, 0, FALSE, 0);
		MeParam[j].Clear();
		for (i = 0; i < MAXPEAKCOUNT; i++) {
			MeParam[j].PS1[i].Set(0, 70, 180, 32, 6, 0, 0, FALSE, 0);
			MeParam[j].PS2[i].Set(0, 70, 180, 32, 6, 0, 0, FALSE, 0);
		}
	}

	bAutoZTargetdmy = FALSE;
	bEchoPeakMonitor = TRUE;
	fEchoTopZ = 0.0;
	fThinFilmZ = 0.0;
	bSaveVideo = FALSE;
	bAllOrNothing = FALSE;
	nTrialPat = 1;
	nTrialEcho = 1;
	nTrialTrim = 1;
	fStepSizePat = 0.5;
	fStepSizeEcho = 0.5;
	fStepSizeTrim = 0.5;
	TrimHeightDistanceInner1 = TrimHeightDistanceOuter1 = 0.5f;
	TrimHeightDistanceInner2 = 0.8f;
	for (i = 0; i < MAXMEPASET; i++) {
		POffsetX[i][1] = 0;
		POffsetX[i][0] = 0;
		POffsetY[i][1] = 0;
		POffsetY[i][0] = 0;
		MatchRes[i] = 8;
		MatchAlgorithm[i] = 0;
		SearchDept[i] = 6;
		SearchDist[i] = 0;
		PatMatOpt[i] = 0;
		GoFScore[i] = 0;
		SubSamI[i] = 2;
		SubSamS[i] = 5;
		FFTFilter[i] = 0;
	}
	bUseMotorFile = FALSE;
	TrimWidthInnerRadius = 147.4f;
	TrimWidthOuterRadius = 150.0f;
	EstNTrimWidth = 2.60f;
	TFZOffset = 0.0f;
	TrimHeightDistanceOuter2 = 0.15f;
	TrimHeightZOffset = 0.0f;
	EchoNormalExposure = 200;
	TFNormalExposure = 100;
	TFTakeSpecExposure = 100;
	WaferTimeOut = 0;
	FailCountTimeOut = 0;
	PointTimeOut = 0;
	AlgnXPos = 0.0f;
	AlgnYPos = 0.0f;
	AlgnZPos = 0.0f;
	BumpOffsetX = 0.0f;
	BumpOffsetY = 0.0f;
	dmyCXOffset = 0.0f;
	dmyCYOffset = 0.0f;

	bLocateEdgeStep = FALSE;
	bLocateEdgeWidth = FALSE;
	TrimSmooth = 3;
	TrimWidthLines = 5;
	TrimWidthSpacing = 8;
	THExposure = 0.0f;
	TWExposureInner = 0.0f;
	TWExposureOuter = 0.0f;
	TWExposureBevel = 0.0f;	// zero exposure will hang the camera [6/16/2012 FSM 413 C2C]

	NSmooth = 3;
	AutoExposeTF = 90;
	AutoExposeEcho = 90;
	AutoExposeInner = AutoExposeOuter = 0;

	nCycle = 1;
	nExpand = 1;

	::GetLocalTime(&ModifiedTime); // Initial Default values
	::GetLocalTime(&CreateTime);

	strcpy(Creator, "FSM");
	strcpy(ObjClass, "Metrology");
	strcpy(RcpeName, "");

	CString name;
	name.Format("%s%s%d", ObjClass, RcpeName, Version);
	SetID(name.GetBuffer(0));
	SetType("Recipe");

	BaseLineFile = "";
	m_strMaskFileName = "";

	MotorFile = "MOTOR.TXT";
	SamplingFreq = 3600000;
	NumberOfInterestedData = 125000;

	NumLines = PointsPerLine = ScanLength = 0;

	BFPlane = bfBESTFIT;
	BFPlaneCalc = bfcSURFACE;
	RefPt1 = RefPt2 = RefPt3 = 0;
	FindEdgeType = CImgWnd::DARKFWD;
	FindEdgeMethod = 1;  // 1-slow, 2-fast [7/8/2013 Yuen]
	FindEdgeWaferSize = 150.0f;
	bTestFindEdgeResult = TRUE;

	bFastPMatch = TRUE;
	bMeasureOuterdge = TRUE;
	bUseWaferAlign = FALSE;

	XYPairCount = 0;
	XYPair[0].Set(32.24f, 88.965f);
	XYPair[1].Set(32.58f, 121.255f);
	XYPair[2].Set(56.629f, 88.699f);

	// [02022024 ZHIMING
	settlingTime = 0;
	// [05152024
	fRoughZ = 0.0;
	fEchoBottomZ = 0.0;
	fCWLTopZ = 0.0;
	fCWLTopZ2 = 0.0;
	// ]
	DieHeight = 0.0; // Morsalin
	DieWidth = 0.0; // Morsalin
	AnchorX = 0.0;
	AnchorX = 0.0;
	m_bIncludeOn = FALSE;
	m_LSFOn = TRUE; // 10202025_1
	r_LTVMax = TRUE; // 10312025_1 MORSALIN
	r_LTVMin = TRUE; // 10312025_1 MORSALIN
	r_LTVAvg = TRUE; // 10312025_1 MORSALIN
	r_LTV = TRUE; // 10312025_1 MORSALIN
	r_STIR = TRUE; // 10312025_1 MORSALIN

	thkSearchNum = 5;
	warpSearchNum = 5;
	isCalibrationAutoFocusNeeded = FALSE;
	ThicknessWithScan = TRUE; // 09232025_Shamim
}

CWaferParam* CRecipe413::GetWp() {
	return &Wp;
}

// CMPoint * CRecipe413::FindMP(CCoor &point)
// {
// 	return Patt.FindMP(point);
// }

// CMPoint * CRecipe413::FindNearestMP(CCoor &point, float &Distance)
// {
// 	return Patt.FindNearestMP(point, Distance);
// }

CRecipe413& CRecipe413::operator =(CRecipe413& co) {
	int i, j;

	// Members in OSSMemS [2/29/2012 Administrator]
	SetID(co.ObjID);
	SetType(co.ObjType);

	Version = co.Version;
	BaseLineFile = co.BaseLineFile;
	strscpy(RcpeName, RCPNAMELEN, co.RcpeName);
	NMeasureIn = co.NMeasureIn;
	NMeasureOut = co.NMeasureOut;
	nMappedPoints = co.nMappedPoints;   //fw 01/02 add here later
	//repeatdmy = co.repeatdmy;
	Desc = co.Desc;

	Wp = co.Wp;

	TRWMeasProp = co.TRWMeasProp; //08142024

	ccrpatt = co.ccrpatt;
	rdlpatt = co.rdlpatt;
	gridpatt = co.gridpatt;
	BatchDoc = co.BatchDoc;

	Me = co.Me;
	custpatt = co.custpatt;

	CrossPattern = co.CrossPattern;
	MaskPattern = co.MaskPattern;

	m_strMaskFileName = co.m_strMaskFileName;
	//fTapeIndexDmy = co.fTapeIndexDmy;
	//fSiIndexDmy = co.fSiIndexDmy;
	//fPatOffsetDmy = co.fPatOffsetDmy;
	//fDieSizeDmy = co.fDieSizeDmy;
	//fPatternThkDmy = co.fPatternThkDmy;
	//fPolyThkDmy = co.fPolyThkDmy;
	fEchoTopZ = co.fEchoTopZ;

	//fTapeIndex2Dmy = co.fTapeIndex2Dmy;
	//fSiThkDmy = co.fSiThkDmy;
	//fSiRangeDmy = co.fSiRangeDmy;
	//fTapeThkDmy = co.fTapeThkDmy;
	//fTapeRangeDmy = co.fTapeRangeDmy;
	//fTape2ThkDmy = co.fTape2ThkDmy;
	//fTape2RangeDmy = co.fTape2RangeDmy;
	//fFilmThkDmy = co.fFilmThkDmy;
	//fFilmRangeDmy = co.fFilmRangeDmy;
	//nFilmMeasTimedmy = co.nFilmMeasTimedmy;
	bSaveVideo = co.bSaveVideo;
	bAllOrNothing = co.bAllOrNothing;
	nTrialPat = co.nTrialPat;
	nTrialEcho = co.nTrialEcho;
	nTrialTrim = co.nTrialTrim;
	fStepSizePat = co.fStepSizePat;
	fStepSizeEcho = co.fStepSizeEcho;
	TrimHeightDistanceInner1 = co.TrimHeightDistanceInner1;
	TrimHeightDistanceInner2 = co.TrimHeightDistanceInner2;
	TrimHeightDistanceOuter1 = co.TrimHeightDistanceOuter1;
	for (i = 0; i < MAXMEPASET; i++) {
		POffsetX[i][1] = co.POffsetX[i][1];
		POffsetX[i][0] = co.POffsetX[i][0];
		POffsetY[i][1] = co.POffsetY[i][1];
		POffsetY[i][0] = co.POffsetY[i][0];
		GoFScore[i] = co.GoFScore[i];
		PatMatOpt[i] = co.PatMatOpt[i];
		MatchRes[i] = co.MatchRes[i];
		SearchDept[i] = co.SearchDept[i];
		SearchDist[i] = co.SearchDist[i];
		//MatchArea[i] = co.MatchArea[i];
		MatchAlgorithm[i] = co.MatchAlgorithm[i];
		SubSamI[i] = co.SubSamI[i];
		SubSamS[i] = co.SubSamS[i];
		FFTFilter[i] = co.FFTFilter[i];
	}
	//GOFdmy = co.GOFdmy;
	//PatMatDmy = co.PatMatDmy;
	//MatchResolutiondmy = co.MatchResolutiondmy;

	//fZref1Dmy = co.fZref1Dmy;
	//fZref2Dmy = co.fZref2Dmy;
	bUseMotorFile = co.bUseMotorFile;
	//bMark2SelectedDmy = co.bMark2SelectedDmy;
	TrimWidthOuterRadius = co.TrimWidthOuterRadius;
	EstNTrimWidth = co.EstNTrimWidth;
	TFZOffset = co.TFZOffset;
	TrimHeightDistanceOuter2 = co.TrimHeightDistanceOuter2;
	TrimHeightZOffset = co.TrimHeightZOffset;
	//ImgAverdmy = co.ImgAverdmy;
	EchoNormalExposure = co.EchoNormalExposure;
	TFNormalExposure = co.TFNormalExposure;
	TFTakeSpecExposure = co.TFTakeSpecExposure;
	bMeasureOuterdge = co.bMeasureOuterdge;
	WaferTimeOut = co.WaferTimeOut;
	FailCountTimeOut = co.FailCountTimeOut;
	AlgnXPos = co.AlgnXPos;
	AlgnYPos = co.AlgnYPos;
	AlgnZPos = co.AlgnZPos;
	BumpOffsetX = co.BumpOffsetX;
	BumpOffsetY = co.BumpOffsetY;
	PointTimeOut = co.PointTimeOut;
	//nPeakWidth1dmy = co.nPeakWidth1dmy;
	//fThresholdFactor2dmy = co.fThresholdFactor2dmy;
	//nPeakWidth2dmy = co.nPeakWidth2dmy;
	//bSkipAlignerdmy = co.bSkipAlignerdmy;
	bUseWaferAlign = co.bUseWaferAlign;
	nCycle = co.nCycle;
	//TargetLocX2Dmy = co.TargetLocX2Dmy;
	//TargetLocY2Dmy = co.TargetLocY2Dmy;
	//bStrict1dmy = co.bStrict1dmy;
	//bStrict2dmy = co.bStrict2dmy;

	//for (i=0; i<8; i++)
	//{
	//	Padmy[i] = co.Padmy[i];
	//}
	//SubSamplingXdmy = co.SubSamplingXdmy;
	//SubSamplingYdmy = co.SubSamplingYdmy;

	for (i = 0; i < MAXMATCHPATT; i++) {
		MatchDIBA1[i].Duplicate(co.MatchDIBA1[i]);
		MatchDIBB1[i].Duplicate(co.MatchDIBB1[i]);
		MatchDIBA2[i].Duplicate(co.MatchDIBA2[i]);
		MatchDIBB2[i].Duplicate(co.MatchDIBB2[i]);
		MatchDIBA3[i].Duplicate(co.MatchDIBA3[i]);
		MatchDIBB3[i].Duplicate(co.MatchDIBB3[i]);
		MatchDIBA4[i].Duplicate(co.MatchDIBA4[i]);
		MatchDIBB4[i].Duplicate(co.MatchDIBB4[i]);
	}

	bLocateEdgeStep = co.bLocateEdgeStep;
	bLocateEdgeWidth = co.bLocateEdgeWidth;
	TrimWidthLines = co.TrimWidthLines;
	TrimWidthSpacing = co.TrimWidthSpacing;
	THExposure = co.THExposure;
	TWExposureInner = co.TWExposureInner;
	TWExposureOuter = co.TWExposureOuter;
	TWExposureBevel = co.TWExposureBevel;

	strscpy(ObjID, OBJIDLEN, co.ObjID);
	strscpy(ObjType, OBJTYPELEN, co.ObjType);
	strscpy(ObjClass, OBJIDLEN, co.ObjClass);
	strscpy(Creator, RCPNAMELEN, co.Creator);

	ModifiedTime = co.ModifiedTime;
	CreateTime = co.CreateTime;

	for (j = 0; j < MAXMEPASET; j++) {
		MeParam[j] = co.MeParam[j];
	}

	fThinFilmZ = co.fThinFilmZ;

	Patt.MP = co.Patt.MP;

	//TargetLocX1Dmy = co.TargetLocX1Dmy;
	//TargetLocY1Dmy = co.TargetLocY1Dmy;

	///*PkClnDmy*/NSmooth = co.NSmooth;
	//PkHeightDmy = co.PkHeightDmy;

	AutoExposeTF = co.AutoExposeTF;
	AutoExposeEcho = co.AutoExposeEcho;
	AutoExposeInner = co.AutoExposeInner;
	AutoExposeOuter = co.AutoExposeOuter;
	//fThinFilmZCaldmy = co.fThinFilmZCaldmy;

	MotorFile = co.MotorFile;
	SamplingFreq = co.SamplingFreq;
	NumberOfInterestedData = co.NumberOfInterestedData;

	EchoFocus = co.EchoFocus;

	fTGtRedXdmy = co.fTGtRedXdmy;
	fTGtRedYdmy = co.fTGtRedYdmy;

	for (i = 0; i < MAXMATCHPATT; i++) {
		for (j = 0; j < MAXTGTSET; j++) {
			fTgtGRnX[i][j] = co.fTgtGRnX[i][j];
			fTgtGRnY[i][j] = co.fTgtGRnY[i][j];
			fTgtBLuX[i][j] = co.fTgtBLuX[i][j];
			fTgtBLuY[i][j] = co.fTgtBLuY[i][j];
		}
	}

	bEnableAutoZ = co.bEnableAutoZ;
	fAutoZValue = co.fAutoZValue;
	FindTrimThres = co.FindTrimThres;
	SamplePerPoint = co.SamplePerPoint;

	dmyCXOffset = co.dmyCXOffset;
	dmyCYOffset = co.dmyCYOffset;

	NumLines = co.NumLines;
	PointsPerLine = co.PointsPerLine;
	ScanLength = co.ScanLength;

	BFPlane = co.BFPlane;
	BFPlaneCalc = co.BFPlaneCalc;
	RefPt1 = co.RefPt1;
	RefPt2 = co.RefPt2;
	RefPt3 = co.RefPt3;
	FindEdgeType = co.FindEdgeType;
	FindEdgeMethod = co.FindEdgeMethod;
	FindEdgeWaferSize = co.FindEdgeWaferSize;
	bTestFindEdgeResult = co.bTestFindEdgeResult;

	XYPairCount = co.XYPairCount;
	for (i = 0; i < MAXXYPAIR; i++) {
		XYPair[i] = co.XYPair[i];
	}
	for (i = 0; i < MAXXYPAIROFFSET; i++) {
		XYPairOffset[i] = co.XYPairOffset[i];
	}
	// [20210329 ZHIMING
	ContrastLevel = co.ContrastLevel;
	//20220615 
	FindBevelThres = co.FindBevelThres;
	//02022024 
	settlingTime = co.settlingTime;
	// 05302024
	fRoughZ = co.fRoughZ;
	fEchoBottomZ = co.fEchoBottomZ;
	fCWLTopZ = co.fCWLTopZ;
	fCWLTopZ2 = co.fCWLTopZ2;
	// ]
	DieHeight = co.DieHeight;
	DieWidth = co.DieWidth;
	m_bIncludeOn = co.m_bIncludeOn; // Morsalin
	m_LSFOn = co.m_LSFOn; // Morsalin
	r_LTVMin = co.r_LTVMin; // 10312025_1 MORSALIN
	r_LTVMax = co.r_LTVMax; // 10312025_1 MORSALIN
	r_LTVAvg = co.r_LTVAvg; // 10312025_1 MORSALIN
	r_LTV = co.r_LTV; // 10312025_1 MORSALIN
	r_STIR = co.r_STIR; // 10312025_1 MORSALIN

	AnchorX = co.AnchorX; // Morsalin
	AnchorY = co.AnchorY; // Morsalin
	return *this;
}

void CRecipe413::Serialize(CArchive& ar) {
	short i, j, val;
	short dummy = 0;
	short maxPA = 8;

	if (ar.IsStoring()) {
		// Sanity check [11/4/2012 Yuen]
		// 20130828-2 SCOTT EDIT if (nTrial > 48)
		if (nTrialEcho > 168) {
			// There are only 49 search position [3/14/2012 Administrator]
			// 20130828 SCOTT EDIT nTrial = 48;
			nTrialEcho = 168;
		}
		if (nTrialEcho < 0) {
			nTrialEcho = 0;
		}

		::GetLocalTime(&ModifiedTime);//  [5/2/2008 Yuen]
		Version = CURRENTRECIPEVERSION;
		ar << Version;
		ar << BaseLineFile;
		ar << CString(RcpeName);
		ar << nTrimMethod;
		ar << nExpand;
		ar << nMappedPoints;
		ar << repeatdmy;
		Desc.Serialize(ar);
		ar << dummy;
		Wp.Serialize(ar);
		ccrpatt.Serialize(ar);
		rdlpatt.Serialize(ar);
		gridpatt.Serialize(ar);
		BatchDoc.Serialize(ar);

		// 0 Ends
		Me.Serialize(ar);
		// 1 Ends
		custpatt.Serialize(ar);
		// 2 Ends
		ar << m_ImgSizedmy;
		if (0 < m_ImgSizedmy) ar.Write(m_Imagedmy, m_ImgSizedmy);
		// 3 ends
		ar << 0;
		// 4 ends
		CrossPattern.Serialize(ar);
		MaskPattern.Serialize(ar);
		// 5 ends
		ar << m_strMaskFileName;
		// 7 ends
		ar << fThresholdFactor1dmy;
		ar << fTapeIndexDmy;
		// 8 ends
		ar << fSiIndexDmy;
		ar << fPolyIndexDmy;
		ar << bAutoZTargetdmy;
		ar << bEchoPeakMonitor;
		// 9 ends
		ar << fSiOffsetDmy;
		ar << fPatOffsetDmy;
		// 10 ends
		ar << fDieSizeDmy;
		// 11 ends
		ar << fPatternThkDmy;
		ar << fPolyThkDmy;
		ar << fEchoTopZ;
		//12 ends
		ar << fTapeIndex2Dmy;
		ar << fSiThkDmy;
		ar << fSiRangeDmy;
		ar << fTapeThkDmy;
		ar << fTapeRangeDmy;
		ar << fTape2ThkDmy;
		ar << fTape2RangeDmy;
		ar << fFilmThkDmy;
		ar << fFilmRangeDmy;
		ar << nFilmMeasTimedmy;
		//13 ends
		ar << bSaveVideo;
		ar << bAllOrNothing;
		ar << nTrialEcho;
		ar << nTrialTrim;
		ar << fStepSizeEcho;
		//14 ends
		ar << TrimHeightDistanceInner1;
		ar << TrimHeightDistanceOuter1;
		for (i = 0; i < MAXMEPASET; i++) {
			ar << POffsetX[i][1];
			ar << POffsetX[i][0];
		}
		ar << fZref1Dmy;
		ar << fZref2Dmy;
		ar << bUseMotorFile;
		ar << bFastPMatch;
		//15 ends
		ar << TrimWidthOuterRadius; //pattern recognition parameters
		ar << TFZOffset;
		ar << TrimHeightDistanceOuter2;
		ar << TrimHeightZOffset;
		ar << ImgAverdmy;
		ar << EchoNormalExposure;
		ar << TFNormalExposure;	//camera parameters
		ar << PatMatDmy;
		ar << TFTakeSpecExposure;
		ar << MatchResolutiondmy;
		ar << bMeasureOuterdge;
		ar << WaferTimeOut;
		ar << FailCountTimeOut;
		ar << AlgnXPos;
		ar << AlgnYPos;
		ar << AlgnZPos;
		ar << BumpOffsetX;
		ar << BumpOffsetY;
		//16 ends
		ar << PointTimeOut;
		// 17 ends
		ar << nPeakWidth1dmy;
		ar << TrimWidthInnerRadius;
		ar << nPeakWidth2dmy;
		//18 ends
		ar << bSkipAlignerdmy;
		//19 ends
		ar << bUseWaferAlign;
		ar << GOFdmy;
		//20 ends
		ar << nCycle;
		// 21 ends
		ar << AutoExposeInner;
		ar << AutoExposeOuter;
		ar << bStrict1dmy;
		ar << bStrict2dmy;
		// 24 ends
		ar << maxPA;
		for (i = 0; i < maxPA; i++) {
			Padmy[i].Serialize(ar);
		}
		// 25 ends
		ar << NMeasureIn;
		ar << NMeasureOut;

		MatchDIBA1[0].Serialize(ar);
		MatchDIBB1[0].Serialize(ar);

		ar << TrimWidthLines;
		ar << TWExposureInner;
		ar << TrimWidthSpacing;

		ar << CString(ObjID);
		ar << CString(ObjType);
		ar << CString(ObjClass);
		ar << CString(Creator);
		ar << ModifiedTime.wYear;
		ar << ModifiedTime.wMonth;
		ar << ModifiedTime.wDayOfWeek;
		ar << ModifiedTime.wDay;
		ar << ModifiedTime.wHour;
		ar << ModifiedTime.wMinute;
		ar << ModifiedTime.wSecond;
		ar << ModifiedTime.wMilliseconds;
		ar << CreateTime.wYear;
		ar << CreateTime.wMonth;
		ar << CreateTime.wDayOfWeek;
		ar << CreateTime.wDay;
		ar << CreateTime.wHour;
		ar << CreateTime.wMinute;
		ar << CreateTime.wSecond;
		ar << CreateTime.wMilliseconds;
		// 27 ends

		for (i = 1; i < MAXMATCHPATT; i++) {
			MatchDIBA1[i].Serialize(ar);
			MatchDIBB1[i].Serialize(ar);
		}
		for (i = 0; i < MAXMEPASET; i++) {
			MeParam[i].Serialize(ar);
		}

		ar << fThinFilmZ;

		Patt.MP.Serialize(ar);

		ar << TargetLocX1Dmy;
		ar << TargetLocY1Dmy;

		ar << NSmooth;
		ar << TrimSmooth;
		ar << AutoExposeTF;
		ar << fThinFilmZCaldmy;

		ar << MotorFile;
		ar << SamplingFreq;
		ar << NumberOfInterestedData;

		ar << EchoFocus;

		ar << fTgtGRnX[0][0];
		ar << fTgtGRnY[0][0];
		ar << fTGtRedXdmy;
		ar << fTGtRedYdmy;

		ar << bEnableAutoZ;
		ar << fAutoZValue;

		ar << fTgtBLuX[0][0];
		ar << fTgtBLuY[0][0];

		ar << MatchAreaDmy[0];

		ar << dmyCXOffset;
		ar << dmyCYOffset;

		ar << TWExposureOuter;

		ar << NumLines;
		ar << PointsPerLine;
		ar << ScanLength;

		ar << TWExposureBevel;

		for (i = 1; i < MAXMATCHPATT; i++) {
			ar << fTgtGRnX[i][0];
			ar << fTgtGRnY[i][0];
			ar << fTgtBLuX[i][0];
			ar << fTgtBLuY[i][0];
		}
		ar << SearchDept[0];
		ar << SearchDist[0];
		for (i = 0; i < MAXMEPASET; i++) {
			ar << POffsetY[i][1];
			ar << POffsetY[i][0];
		}
		for (i = 0; i < MAXMEPASET; i++) {
			ar << GoFScore[i];
			ar << PatMatOpt[i];
			ar << MatchRes[i];
			ar << MatchAreaDmy[i];
			ar << SearchDept[i];
			ar << SearchDist[i];
			ar << SubSamI[i];
			ar << SubSamS[i];
			ar << FFTFilter[i];
		}
		for (i = 0; i < MAXMEPASET; i++) {
			ar << MatchAlgorithm[i];
		}
		ar << fStepSizeTrim;
		val = BFPlane;
		ar << val;
		ar << RefPt1;
		ar << RefPt2;
		ar << RefPt3;
		val = BFPlaneCalc;
		ar << val;
		for (i = 0; i < MAXXYPAIR; i++) {
			XYPair[i].Serialize(ar);
		}
		ar << XYPairCount;
		for (i = 0; i < MAXXYPAIROFFSET; i++) {
			XYPairOffset[i].Serialize(ar);
		}
		MatchDIBA2[0].Serialize(ar);
		MatchDIBB2[0].Serialize(ar);
		MatchDIBA3[0].Serialize(ar);
		MatchDIBB3[0].Serialize(ar);
		ar << AutoExposeEcho;
		ar << FindTrimThres;
		ar << SamplePerPoint;

		for (i = 1; i < MAXMATCHPATT; i++) {
			MatchDIBA2[i].Serialize(ar);
			MatchDIBB2[i].Serialize(ar);
			MatchDIBA3[i].Serialize(ar);
			MatchDIBB3[i].Serialize(ar);
		}
		for (i = 0; i < MAXMATCHPATT; i++) {
			MatchDIBA4[i].Serialize(ar);
			MatchDIBB4[i].Serialize(ar);
		}
		for (i = 0; i < MAXMATCHPATT; i++) {
			for (j = 1; j < MAXTGTSET; j++) {
				ar << fTgtGRnX[i][j];
				ar << fTgtGRnY[i][j];
				ar << fTgtBLuX[i][j];
				ar << fTgtBLuY[i][j];
			}
		}
		ar << nTrialPat;
		ar << fStepSizePat;
		ar << FindEdgeType;
		ar << FindEdgeMethod;
		ar << FindEdgeWaferSize;
		ar << bTestFindEdgeResult;
		ar << THExposure;
		ar << bLocateEdgeStep;
		ar << TrimHeightDistanceInner2;
		ar << EstNTrimWidth;
		ar << bLocateEdgeWidth;
		// 96 ends
		ar << ContrastLevel;
		// 97 ends
		ar << FindBevelThres;
		// 98 ends
		//01312024/yukchiu
		ar << settlingTime;
		// 99 ends
		// [05152024 ZHIMING
		ar << fRoughZ;
		ar << fEchoBottomZ;
		ar << fCWLTopZ;
		
		// 100 ends
		ar << bIsRa_rcp; // 07102024 -> Mahedi Kamal
		// 101 ends
		TRWMeasProp.Serialize(ar); //08142024
		// 102 ends
		ar << bIsRaCWL_rcp; // 12120224
		ar << RoughStepSizeCWL_rcp; // 12120224
		ar << RoughScanLineLenCWL_rcp; // 12120224
		ar << DieHeight; //Morsalin
		ar << DieWidth; //Morsalin
		ar << m_bIncludeOn; //Morsalin
		ar << m_LSFOn; //Morsalin
		
		ar << fCWLTopZ2;

		ar << AnchorX; //Morsalin
		ar << AnchorY; //Morsalin
		ar << bIsAutoFocus008; //Morsalin
		//103 ends
		ar << bIsNmCWL; //20250306

		//104 ends

		ar << thkSearchNum; //10032025_3
		ar << warpSearchNum;//10032025_3
		ar << stepSrhSize;//10032025_3
		ar << isCalibrationAutoFocusNeeded;//03192025

		//106 ends
		ar << ThicknessWithScan; // 09232025_Shamim

		// 107 emds
		ar << r_LTVMin; // 10312025_1 MORSALIN
		ar << r_LTVMax; // 10312025_1 MORSALIN
		ar << r_LTVAvg; // 10312025_1 MORSALIN
		ar << r_LTV; // 10312025_1 MORSALIN
		ar << r_STIR; // 10312025_1 MORSALIN
	}
	else {
		for (i = 0; i < MAXMEPASET; i++) {
			MeParam[i].Clear();
		}
		ar >> Version;
		ar >> BaseLineFile;
		CString rp;
		ar >> rp;
		strscpy(RcpeName, RCPNAMELEN, rp.GetBuffer(0));
		ar >> nTrimMethod/*NMeasuredmy*/;
		if (Version < 88) {
			nTrimMethod = 0;
		}
		ar >> nExpand;
		if (Version < 71) {
			nExpand = 1;
		}
		ar >> nMappedPoints;
		if (Version < 45) {
			nMappedPoints = 65536;
		}
		ar >> repeatdmy;
		Desc.Serialize(ar);
		ar >> dummy;
		Wp.Serialize(ar);
		ccrpatt.Serialize(ar);
		rdlpatt.Serialize(ar);
		gridpatt.Serialize(ar);
		BatchDoc.Serialize(ar);
		Me.Serialize(ar);
		custpatt.Serialize(ar);
		m_Imagedmy = NULL;
		ar >> m_ImgSizedmy;
		if (0 < m_ImgSizedmy) {
			BYTE* bTemp = new BYTE[m_ImgSizedmy];
			ASSERT(bTemp != NULL);
			ar.Read(bTemp, m_ImgSizedmy);
			delete[] bTemp;  // Get rid of allocation, no longer use [11/19/2012 Yuen]
		}
		int nMaskType;	// for backward compatibility
		ar >> nMaskType;
		CrossPattern.Serialize(ar);
		MaskPattern.Serialize(ar);
		ar >> m_strMaskFileName;
		ar >> fThresholdFactor1dmy;
		ar >> fTapeIndexDmy;
		ar >> fSiIndexDmy;
		ar >> fPolyIndexDmy;
		ar >> bAutoZTargetdmy;
		ar >> bEchoPeakMonitor;
		if (Version < 39) {
			bEchoPeakMonitor = TRUE;
		}
		if (Version < 42) {
			bAutoZTargetdmy = FALSE;
		}
		ar >> fSiOffsetDmy;
		ar >> fPatOffsetDmy;
		ar >> fDieSizeDmy;
		ar >> fPatternThkDmy;
		ar >> fPolyThkDmy;
		ar >> fEchoTopZ;
		ar >> fTapeIndex2Dmy;
		ar >> fSiThkDmy;
		ar >> fSiRangeDmy;
		ar >> fTapeThkDmy;
		ar >> fTapeRangeDmy;
		ar >> fTape2ThkDmy;
		ar >> fTape2RangeDmy;
		ar >> fFilmThkDmy;
		ar >> fFilmRangeDmy;
		ar >> nFilmMeasTimedmy;
		ar >> bSaveVideo;
		ar >> bAllOrNothing;
		if (Version < 55) {
			bAllOrNothing = FALSE;
		}
		ar >> nTrialEcho;
		ar >> nTrialTrim;
		ar >> fStepSizeEcho;
		if (Version < 43) {
			bSaveVideo = FALSE;
		}
		ar >> TrimHeightDistanceInner1;
		ar >> TrimHeightDistanceOuter1;
		if (Version < 65) {
			ar >> POffsetX[0][1];
			ar >> POffsetX[0][0];
			for (i = 1; i < MAXMEPASET; i++) {
				POffsetX[i][1] = POffsetX[i][0] = 0;
			}
		}
		else {
			for (i = 0; i < MAXMEPASET; i++) {
				ar >> POffsetX[i][1];
				ar >> POffsetX[i][0];
			}
		}
		ar >> fZref1Dmy;
		ar >> fZref2Dmy;
		ar >> bUseMotorFile;
		if (Version < 56) {
			bUseMotorFile = FALSE;
		}
		ar >> bFastPMatch;
		if (Version < 75) {
			bFastPMatch = TRUE;
		}
		ar >> TrimWidthOuterRadius;
		if (Version < 34) {
			TrimWidthOuterRadius = 150.0f;
		}
		ar >> TFZOffset;
		ar >> TrimHeightDistanceOuter2;
		if (Version < 51) {
			TrimHeightDistanceOuter2 = 0;
		}
		ar >> TrimHeightZOffset;
		ar >> ImgAverdmy;
		ar >> EchoNormalExposure;
		if (Version < 52) {
			EchoNormalExposure = 200;
		}
		ar >> TFNormalExposure;
		if (Version < 36) {
			TFNormalExposure = 100;
		}
		ar >> PatMatOpt[0];
		if (Version < 42) {
			PatMatOpt[0] = 0;
		}
		ar >> TFTakeSpecExposure;
		ar >> MatchResolutiondmy;
		MatchRes[0] = MatchResolutiondmy;
		if (Version < 43) {
			TrimHeightZOffset = 0;
			TFTakeSpecExposure = 100;
			MatchRes[0] = 8;
		}
		ar >> bMeasureOuterdge/*bSiOnTopDmy*/;
		if (Version < 77) {
			bMeasureOuterdge = TRUE;
		}
		ar >> WaferTimeOut;
		ar >> FailCountTimeOut;
		if (Version < 59) {
			WaferTimeOut = 0;
			FailCountTimeOut = 0;
		}
		ar >> AlgnXPos;
		ar >> AlgnYPos;
		ar >> AlgnZPos;
		if (Version < 46) {
			AlgnXPos = AlgnYPos = AlgnZPos = 0.0f;
		}
		ar >> BumpOffsetX;
		ar >> BumpOffsetY;
		if (Version < 47) {
			BumpOffsetX = BumpOffsetY = 0;
		}
		if (Version < 37) {
			TFZOffset = 0.0f;
		}
		ar >> PointTimeOut;
		if (Version < 60) {
			PointTimeOut = 0;
		}
		ar >> nPeakWidth1dmy;
		ar >> TrimWidthInnerRadius /*fThresholdFactor2dmy*/;
		if (Version < 73) {
			TrimWidthInnerRadius = 150.0f;
		}
		ar >> nPeakWidth2dmy;
		ar >> bSkipAlignerdmy;
		ar >> bUseWaferAlign/*bStitchingdmy*/;
		if (Version < 80) {
			bUseWaferAlign = FALSE;
		}
		ar >> GoFScore[0];
		if (Version < 54) {
			GoFScore[0] = 0;
		}
		ar >> nCycle;
		if (Version < 45) {
			nCycle = 3;
		}
		else if (nCycle < 1) {
			nCycle = 1;
		}
		if (21 < Version) {
			ar >> AutoExposeInner;
			ar >> AutoExposeOuter;
		}
		else {
			if (Version < 80) {
				AutoExposeInner = 90;
				AutoExposeOuter = 90;
			}
		}
		if (22 < Version) {
			ar >> bStrict1dmy;
		}
		else {
			bStrict1dmy = FALSE;
		}
		if (23 < Version) {
			ar >> bStrict2dmy;
		}
		else {
			bStrict2dmy = FALSE;
		}
		if (24 < Version) {
			ar >> maxPA;
			for (i = 0; i < maxPA; i++) {
				Padmy[i].Serialize(ar);
			}
		}
		else {
			for (i = 0; i < 8; i++) {
				Padmy[i].Clear();
			}
		}
		if (25 < Version) {
			ar >> NMeasureIn;
			ar >> NMeasureOut;
			MatchDIBA1[0].Serialize(ar);
			MatchDIBB1[0].Serialize(ar);
			ar >> TrimWidthLines;
			ar >> TWExposureInner;
			ar >> TrimWidthSpacing;
		}
		else {
			NMeasureIn = 1;
			NMeasureOut = 5;
			MatchDIBA1[0].ClearDIB();
			MatchDIBB1[0].ClearDIB();
			TrimWidthLines = 5;
			TWExposureInner = 2000;
			TrimWidthSpacing = 8;
		}
		if (Version < 83) {
			NMeasureIn = 1;
			NMeasureOut = 3;
		}
		if (26 < Version) {
			CString str;
			ar >> str;
			SetID(str.GetBuffer(0));
			ar >> str;
			SetType(str.GetBuffer(0));
			ar >> str;
			strscpy(ObjClass, OBJIDLEN, str.GetBuffer(0));
			ar >> str;
			strscpy(Creator, RCPNAMELEN, str.GetBuffer(0));
			ar >> ModifiedTime.wYear;
			ar >> ModifiedTime.wMonth;
			ar >> ModifiedTime.wDayOfWeek;
			ar >> ModifiedTime.wDay;
			ar >> ModifiedTime.wHour;
			ar >> ModifiedTime.wMinute;
			ar >> ModifiedTime.wSecond;
			ar >> ModifiedTime.wMilliseconds;
			ar >> CreateTime.wYear;
			ar >> CreateTime.wMonth;
			ar >> CreateTime.wDayOfWeek;
			ar >> CreateTime.wDay;
			ar >> CreateTime.wHour;
			ar >> CreateTime.wMinute;
			ar >> CreateTime.wSecond;
			ar >> CreateTime.wMilliseconds;
		}
		else {
		}
		if (27 < Version) {
			for (i = 1; i < MAXMATCHPATT; i++) {
				MatchDIBA1[i].Serialize(ar);
				MatchDIBB1[i].Serialize(ar);
			}
		}
		else {
			for (i = 1; i < MAXMATCHPATT; i++) {
				MatchDIBA1[i].ClearDIB();
				MatchDIBB1[i].ClearDIB();
			}
		}
		if (28 < Version) {
			MeParam[0].Serialize(ar);
			if (Version < 62) {
				// flush extra entry in older version [3/24/2012 Administrator]
				for (int j = 1; j </*MAXPATTERNCNT*/9; j++) {
					CMeParam MeParamdmy;
					MeParamdmy.Serialize(ar);
				}
			}
			else if (Version > 62) {
				// Recovery after ver 62 [5/30/2012 Yuen]
				for (int j = 1; j < MAXMEPASET; j++) {
					MeParam[j].Serialize(ar);
				}
			}
			else {
				// Clear the rest for ver 62 [5/30/2012 Yuen]
				for (int j = 1; j < MAXMEPASET; j++) {
					MeParam[j].Clear();
				}
			}
		}
		if (29 < Version) {
			ar >> fThinFilmZ;
		}
		if (30 < Version) {
			Patt.MP.Serialize(ar);
		}
		if (31 < Version) {
			ar >> TargetLocX1Dmy;
			ar >> TargetLocY1Dmy;
		}
		LoadDesc(0);
		if (Version > 32) {
			ar >> NSmooth;
			if (Version < 69) {
				NSmooth = 3;
			}
			ar >> TrimSmooth;
			if (Version < 73) {
				TrimSmooth = 3;
			}
			ar >> AutoExposeTF;
			if (Version < 40) {
				AutoExposeTF = 90;
			}
		}
		else {
			NSmooth = 3;
			TrimSmooth = 16;
			AutoExposeTF = 90;
		}
		if (Version > 33) {
			ar >> fThinFilmZCaldmy;
		}
		else {
			fThinFilmZCaldmy = fThinFilmZ;
		}
		if (Version > 36) {
			ar >> MotorFile;
			ar >> SamplingFreq;
			ar >> NumberOfInterestedData;
		}
		if (Version > 45) {
			ar >> EchoFocus;
		}
		if (48 < Version) {
			ar >> fTgtGRnX[0][0];
			ar >> fTgtGRnY[0][0];
			ar >> fTGtRedXdmy;
			ar >> fTGtRedYdmy;
		}
		if (49 < Version) {
			ar >> bEnableAutoZ;
			ar >> fAutoZValue;
		}
		if (52 < Version) {
			ar >> fTgtBLuX[0][0];
			ar >> fTgtBLuY[0][0];
		}
		if (53 < Version) {
			ar >> MatchAreaDmy[0];
		}
		if (56 < Version) {
			ar >> dmyCXOffset;
			ar >> dmyCYOffset;
		}
		else {
			dmyCXOffset = dmyCYOffset = 0;
		}
		if (57 < Version) {
			ar >> TWExposureOuter;
		}
		else {
			TWExposureOuter = TWExposureInner;
		}

		if (60 < Version) {
			ar >> NumLines;
			ar >> PointsPerLine;
			ar >> ScanLength;
		}
		if (Version > 63) {
			ar >> TWExposureBevel;
		}
		if (Version > 65) {
			for (i = 1; i < MAXMATCHPATT; i++) {
				ar >> fTgtGRnX[i][0];
				ar >> fTgtGRnY[i][0];
				ar >> fTgtBLuX[i][0];
				ar >> fTgtBLuY[i][0];
			}
		}
		if (Version > 66) {
			ar >> SearchDept[0];
			ar >> SearchDist[0];
			for (i = 0; i < MAXMEPASET; i++) {
				ar >> POffsetY[i][1];
				ar >> POffsetY[i][0];
			}
		}
		if ((Version > 67) && (Version <= 69)) {
			for (i = 0; i < MAXMEPASET; i++) {
				ar >> GoFScore[i];
				ar >> PatMatOpt[i];
				ar >> MatchRes[i];
				ar >> MatchAreaDmy[i];
				ar >> SearchDept[i];
				ar >> SearchDist[i];
				ar >> SubSamI[i];
				ar >> SubSamS[i];
			}
		}
		if (Version > 69) {
			for (i = 0; i < MAXMEPASET; i++) {
				ar >> GoFScore[i];
				ar >> PatMatOpt[i];
				ar >> MatchRes[i];
				ar >> MatchAreaDmy[i];
				ar >> SearchDept[i];
				ar >> SearchDist[i];
				ar >> SubSamI[i];
				ar >> SubSamS[i];
				ar >> FFTFilter[i];
			}
		}
		if (Version > 71) {
			for (i = 0; i < MAXMEPASET; i++) {
				ar >> MatchAlgorithm[i];
			}
		}
		if (Version > 72) {
			ar >> fStepSizeTrim;
		}
		if (Version > 73) {
			ar >> val;
			BFPlane = (CRecipe413::BESTFITPLN)val;
			ar >> RefPt1;
			ar >> RefPt2;
			ar >> RefPt3;
		}
		if (Version > 75) {
			ar >> val;
			BFPlaneCalc = (CRecipe413::BESTFITPLNCALC)val;
		}
		if (Version > 77) {
			for (i = 0; i < MAXXYPAIR; i++) {
				XYPair[i].Serialize(ar);
			}
			ar >> XYPairCount;
			for (i = 0; i < MAXXYPAIROFFSET; i++) {
				XYPairOffset[i].Serialize(ar);
			}
		}
		if (Version > 78) {
			MatchDIBA2[0].Serialize(ar);
			MatchDIBB2[0].Serialize(ar);
			MatchDIBA3[0].Serialize(ar);
			MatchDIBB3[0].Serialize(ar);
		}
		if (Version > 81) {
			ar >> AutoExposeEcho;
		}
		if (Version > 83) {
			ar >> FindTrimThres;
		}
		if (Version > 84) {
			ar >> SamplePerPoint;
		}
		if (Version > 85) {
			for (i = 1; i < MAXMATCHPATT; i++) {
				MatchDIBA2[i].Serialize(ar);
				MatchDIBB2[i].Serialize(ar);
				MatchDIBA3[i].Serialize(ar);
				MatchDIBB3[i].Serialize(ar);
			}
		}
		if (Version > 86) {
			for (i = 0; i < MAXMATCHPATT; i++) {
				MatchDIBA4[i].Serialize(ar);
				MatchDIBB4[i].Serialize(ar);
			}
			for (i = 0; i < MAXMATCHPATT; i++) {
				for (j = 1; j < MAXTGTSET; j++) {
					ar >> fTgtGRnX[i][j];
					ar >> fTgtGRnY[i][j];
					ar >> fTgtBLuX[i][j];
					ar >> fTgtBLuY[i][j];
				}
			}
		}
		if (Version > 88) {
			ar >> nTrialPat;
			ar >> fStepSizePat;
		}
		if (Version > 89) {
			ar >> FindEdgeType;
			ar >> FindEdgeMethod;
		}
		if (Version > 90) {
			ar >> FindEdgeWaferSize;
		}
		if (Version > 91) {
			ar >> bTestFindEdgeResult;
		}
		if (Version > 92) {
			ar >> THExposure;
			ar >> bLocateEdgeStep;
		}
		if (Version > 93) {
			ar >> TrimHeightDistanceInner2;
		}
		if (Version > 94) {
			ar >> EstNTrimWidth;
		}
		if (Version > 95) {
			ar >> bLocateEdgeWidth;
		}
		if (Version > 96) ar >> ContrastLevel;
		// [20220615 ZHIMING
		if (Version > 97) ar >> FindBevelThres;
		// 01312024/yukchiu
		if (Version > 98) ar >> settlingTime;
		// [05152024 ZHIMING
		if (Version > 99) {
			ar >> fRoughZ;
			ar >> fEchoBottomZ;
			ar >> fCWLTopZ;
			
		}
		//[ 07102024 -> Mahedi Kamal
		if (Version > 100) {
			ar >> bIsRa_rcp;
		}
		//]
		if (Version > 101) { //08142024
			TRWMeasProp.Serialize(ar); 
		}
		if (Version > 102) {// 12120224
			ar >> bIsRaCWL_rcp; 
			ar >> RoughStepSizeCWL_rcp; 
			ar >> RoughScanLineLenCWL_rcp; 
			ar >> DieHeight; // Morsalin
			ar >> DieWidth; // Morsalin
			ar >> m_bIncludeOn; // Morsalin
			ar >> m_LSFOn; // Morsalin
			
			
			ar >> fCWLTopZ2;

			ar >> AnchorX; // Morsalin
			ar >> AnchorY; // Morsalin
		}
		if (Version > 103) {
			ar >> bIsAutoFocus008;
		}
		if (Version > 104) { //20250306
			ar >> bIsNmCWL;
		}

		if (Version > 105) {//10032025_3
			ar >> thkSearchNum;
			ar >> warpSearchNum;
			ar >> stepSrhSize;
			ar >> isCalibrationAutoFocusNeeded;//03192025
		}
		//[ 09232025_Shamim
		if (Version > 106) 
		{ 
			ar >> ThicknessWithScan; 
		}
		//]
		// [ 10312025_1 MORSALIN
		if (Version > 109)
		{
			ar >> r_LTVMin;
			ar >> r_LTVMax; 
			ar >> r_LTVAvg; 
			ar >> r_LTV; 
			ar >> r_STIR; 
		}
		// ]
	}
}

void CRecipe413::SetPattType(short typ) {}

CCustPatt* CRecipe413::GetCustPatt() {
	return &custpatt;
}

void CRecipe413::SetName(LPCTSTR lpszFileName) {
	strscpy(RcpeName, RCPNAMELEN, lpszFileName);
	CString str;
	str = RcpeName;
	DosUtil.ExtractFilename(str);
	strscpy(RcpeName, RCPNAMELEN, str.GetBuffer(0));
}

void CRecipe413::GetBatchInfo(CString& PrjName, CString& CID, CString& LID, CString& WID, CString& Op, CString& Nt) {
	BatchDoc.Get(PrjName, CID, LID, WID, Op, Nt);
}

short CRecipe413::GetActiveMPCount() {
	return Patt.MP.GetCount();
}

CMPoint* CRecipe413::GetActiveMP(short index) {
	return Patt.MP.Get(index);
}

void CRecipe413::FakeData() {
	Patt.FakeData();
}

BOOL CRecipe413::GetMPList(CPtrList& MPList) {
	return Patt.GetMPList(MPList);
}

void CRecipe413::SetRdlData(CPtrList& lines) {
	rdlpatt.SetData(lines);
}

void CRecipe413::SetCcrData(CPtrList& rings) {
	ccrpatt.SetData(rings);
}

CBatchDoc* CRecipe413::GetBatchDoc() {
	return &BatchDoc;
}

BOOL CRecipe413::DelMP(CCoor& Point) {
	return Patt.DelMP(Point);
}

BOOL CRecipe413::EditLocMP(CCoor& Point, float XCord, float YCord) {
	return Patt.EditMP(Point, XCord, YCord);
}

BOOL CRecipe413::ClearReadings() {
	return FALSE;
}

int CRecipe413::NumPts() {
	return Patt.MP.GetCount();
}

void CRecipe413::ClearData() {
	Patt.ClearData();
}

BOOL CRecipe413::SaveWindowImage(CWnd* pWnd) {
	WriteWindowToDIB("C:\\temp.BMP", pWnd);
	return TRUE;
}

void CRecipe413::LoadDesc(short j) {
	// first to column is reserved [3/13/2010 Valued Customer]
	short cnt = 2;
	int i;
	for (i = 0; i < MaXPA; i++) {
		if (MeParam[j].MPa[i].Prb) {
			Desc.bVis[cnt] = TRUE;
			Desc.ID[cnt] = MeParam[j].MPa[i].Name;
			Desc.Col[cnt] = i;
			cnt++;
		}
	}
	for (i = cnt; i < LASTXCOL; i++) {
		Desc.bVis[i] = FALSE;
		Desc.ID[i] = "";
		Desc.Col[i] = 0;
	}
}

// BOOL CRecipe413::IsTotalThickness(short j)
// {
// 	for (int i=0; i<MaXPA; i++)
// 	{
// 		if ((MeParam[j].MPa[i].Prb == MParam::ECHOTT1PRB) || (MeParam[j].MPa[i].Prb == MParam::ECHOTT2PRB))
// 		{
// 			return TRUE;
// 		}
// 	}
// 	return FALSE;
// }

BOOL CRecipe413::IsEcho(short j) {
	for (int i = 0; i < MaXPA; i++) {
		if ((MeParam[j].MPa[i].Prb > 0 && MeParam[j].MPa[i].Prb < MParam::ROUGHPRB)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsStress(short j) {
	for (int i = 0; i < MaXPA; i++) {
		if ((MeParam[j].MPa[i].Prb == MParam::STRESSPRB)) {
			return TRUE;
		}
	}
	return FALSE;
}

void CRecipe413::MakeRelative() {
	CPtrList* pO = &Patt.MP.MPLst;

	CCoor co;
	float Distance;

	co.x = co.y = 0;

	CMPoint* p = Patt.FindNearestMP(co, Distance);
	if (p) {
		CData* dd = p->GetData(0, FALSE);
		if (dd) {
			POSITION pos = pO->GetHeadPosition();
			while (pos) {
				CMPoint* q = (CMPoint*)pO->GetNext(pos);
				if (q) {
					CData* d = q->GetData(0, FALSE);
					if (d) {
						q->baseline = d->DVal[0] - dd->DVal[0];  // Baseline info is only valid in the very first DVal [11/8/2011 Administrator]
					}
				}
			}
		}
	}
}

void CRecipe413::MakeBaseline() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	float min = 9.9999e30f;

	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* q = (CMPoint*)pMPList->GetNext(pos);
		if (q) {
			CData* d = q->GetData(0, FALSE);
			if (d) {
				if (d->DVal[0] < min) {
					min = d->DVal[0];
				}
			}
		}
	}

	pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* q = (CMPoint*)pMPList->GetNext(pos);
		if (q) {
			CData* d = q->GetData(0, FALSE);
			if (d) {
				// Only apply to bottom probe [10/30/2011 FSMT]
				q->baseline = d->DVal[0] - min;
			}
		}
	}
}

// WriteDIB		- Writes a DIB to file
// Returns		- TRUE on success
// szFile		- Name of file to write to
// hDIB			- Handle of the DIB
BOOL WriteDIB(LPCTSTR szFile, HANDLE hDIB) {
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB) {
		return FALSE;
	}

	CFile file;
	if (!file.Open(szFile, CFile::modeWrite | CFile::modeCreate)) {
		return FALSE;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header
	hdr.bfType = ((WORD)('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize = GlobalSize(hDIB) + sizeof(hdr);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = (DWORD)(sizeof(hdr) + lpbi->biSize +
		nColors * sizeof(RGBQUAD));

	// Write the file header
	file.Write(&hdr, sizeof(hdr));

	// Write the DIB header and the bits
	file.Write(lpbi, GlobalSize(hDIB));

	return TRUE;
}

// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE DDBToDIB(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal) {
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;

	ASSERT(bitmap.GetSafeHandle());

	// The function has no arg for bitfields
	if (dwCompression == BI_BITFIELDS) {
		return NULL;
	}

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE)pPal->GetSafeHandle();
	if (hPal == NULL) {
		hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	}

	// Get bitmap information
	bitmap.GetObject(sizeof(bm), (LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = (USHORT)(bm.bmPlanes * bm.bmBitsPixel);
	bi.biCompression = dwCompression;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if (nColors > 256) {
		nColors = 0;
	}
	dwLen = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = GetDC(NULL);
	hPal = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	if (!hDIB) {
		SelectPalette(hDC, hPal, FALSE);
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver
	// will calculate the biSizeImage field
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0) {
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
			* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB) {
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
		}
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE);
	if (handle) {
		hDIB = handle;
	}
	else {
		//int nRes = GetLastError();
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC, hPal, FALSE);
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(),
		0L,				// Start scan line
		(DWORD)bi.biHeight,		// # of scan lines
		(LPBYTE)lpbi 			// address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,		// address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);	// Use RGB for color table

	if (!bGotBits) {
		GlobalFree(hDIB);

		SelectPalette(hDC, hPal, FALSE);
		ReleaseDC(NULL, hDC);
		return NULL;
	}

	SelectPalette(hDC, hPal, FALSE);
	ReleaseDC(NULL, hDC);
	return hDIB;
}

BOOL WriteWindowToDIB(LPCTSTR szFile, CWnd* pWnd) {
	CBitmap 	bitmap;
	CDC 		memDC;
	CRect		rect;

	CDC* pDC = pWnd->GetDC();
	memDC.CreateCompatibleDC(pDC);
	pWnd->GetClientRect(rect);
	bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());

	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, SRCCOPY);

	// Create logical palette if device support a palette
	CPalette pal;
	if (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) {
		UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256);
		LOGPALETTE* pLP = (LOGPALETTE*) new BYTE[nSize];
		pLP->palVersion = 0x300;
		pLP->palNumEntries = (USHORT)GetSystemPaletteEntries(pDC->m_hDC, 0, 255, pLP->palPalEntry);

		// Create the palette
		pal.CreatePalette(pLP);

		delete[] pLP;
	}

	pWnd->ReleaseDC(pDC);
	memDC.SelectObject(pOldBitmap);

	// Convert the bitmap to a DIB
	HANDLE hDIB = DDBToDIB(bitmap, BI_RGB, &pal);
	if (!hDIB) return FALSE;

	// Write it to file
	WriteDIB(szFile, hDIB);
	// Free the memory allocated by DDBToDIB for the DIB
	GlobalFree(hDIB);
	return TRUE;
}

// Binary to Recipe conversion [7/3/2012 user]
void CRecipe413::Conv(char** msg) {
	int i, j;
	short val;
	CString str;

	CMake::Conv(Version, msg);
	str = RcpeName;
	CMake::Conv(str, msg);
	CMake::Conv(NMeasureIn, msg);
	CMake::Conv(NMeasureOut, msg);
	CMake::Conv(nExpand, msg);
	CMake::Conv(nMappedPoints, msg);
	Desc.Conv(msg);
	Wp.Conv(msg);
	Me.Conv(msg);
	CMake::Conv(bAutoZTargetdmy, msg);
	CMake::Conv(bEchoPeakMonitor, msg);
	CMake::Conv(fEchoTopZ, msg);
	CMake::Conv(bSaveVideo, msg);
	CMake::Conv(bAllOrNothing, msg);
	CMake::Conv(nTrialPat, msg);
	CMake::Conv(nTrialEcho, msg);
	CMake::Conv(nTrialTrim, msg);
	CMake::Conv(fStepSizePat, msg);
	CMake::Conv(fStepSizeEcho, msg);
	CMake::Conv(TrimHeightDistanceInner1, msg);
	CMake::Conv(TrimHeightDistanceInner2, msg);
	CMake::Conv(TrimHeightDistanceOuter1, msg);
	for (i = 0; i < MAXMEPASET; i++) {
		CMake::Conv(POffsetX[i][1], msg);
		CMake::Conv(POffsetX[i][0], msg);
		CMake::Conv(POffsetY[i][1], msg);
		CMake::Conv(POffsetY[i][0], msg);
		CMake::Conv(PatMatOpt[i], msg);
		CMake::Conv(MatchRes[i], msg);
		CMake::Conv(SearchDept[i], msg);
		CMake::Conv(SearchDist[i], msg);
		CMake::Conv(MatchAreaDmy[i], msg);
		CMake::Conv(MatchAlgorithm[i], msg);
		CMake::Conv(GoFScore[i], msg);
		CMake::Conv(SubSamI[i], msg);
		CMake::Conv(SubSamS[i], msg);
		CMake::Conv(FFTFilter[i], msg);
	}
	CMake::Conv(bUseMotorFile, msg);
	CMake::Conv(TrimWidthOuterRadius, msg);
	CMake::Conv(EstNTrimWidth, msg);
	CMake::Conv(TFZOffset, msg);
	CMake::Conv(TrimHeightDistanceOuter2, msg);
	CMake::Conv(TrimHeightZOffset, msg);
	CMake::Conv(EchoNormalExposure, msg);
	CMake::Conv(TFNormalExposure, msg);
	CMake::Conv(TFTakeSpecExposure, msg);
	CMake::Conv(WaferTimeOut, msg);
	CMake::Conv(FailCountTimeOut, msg);
	CMake::Conv(AlgnXPos, msg);
	CMake::Conv(AlgnYPos, msg);
	CMake::Conv(AlgnZPos, msg);
	CMake::Conv(BumpOffsetX, msg);
	CMake::Conv(BumpOffsetY, msg);
	CMake::Conv(PointTimeOut, msg);
	CMake::Conv(nCycle, msg);
	CMake::Conv(TrimWidthLines, msg);
	CMake::Conv(THExposure, msg);
	CMake::Conv(bLocateEdgeStep, msg);
	CMake::Conv(bLocateEdgeWidth, msg);
	CMake::Conv(TWExposureInner, msg);
	CMake::Conv(TrimWidthSpacing, msg);
	for (i = 0; i < MAXMATCHPATT; i++) {
		MeParam[i].Conv(msg);
	}
	CMake::Conv(fThinFilmZ, msg);
	Patt.MP.Conv(msg);
	CMake::Conv(AutoExposeTF, msg);
	CMake::Conv(AutoExposeEcho, msg);
	CMake::Conv(FindTrimThres, msg);
	CMake::Conv(SamplePerPoint, msg);
	CMake::Conv(MotorFile, msg);
	CMake::Conv(SamplingFreq, msg);
	CMake::Conv(NumberOfInterestedData, msg);
	CMake::Conv(EchoFocus, msg);
	CMake::Conv(fTGtRedXdmy, msg);
	CMake::Conv(fTGtRedYdmy, msg);
	for (i = 0; i < MAXMATCHPATT; i++) {
		for (j = 0; j < MAXTGTSET; j++) {
			CMake::Conv(fTgtGRnX[i][j], msg);
			CMake::Conv(fTgtGRnY[i][j], msg);
			CMake::Conv(fTgtBLuX[i][j], msg);
			CMake::Conv(fTgtBLuY[i][j], msg);
		}
	}
	CMake::Conv(bEnableAutoZ, msg);
	CMake::Conv(fAutoZValue, msg);
	CMake::Conv(dmyCXOffset, msg);
	CMake::Conv(dmyCYOffset, msg);
	CMake::Conv(TWExposureOuter, msg);
	CMake::Conv(NumLines, msg);
	CMake::Conv(PointsPerLine, msg);
	CMake::Conv(ScanLength, msg);
	CMake::Conv(TWExposureBevel, msg);
	CMake::Conv(val, msg);
	BFPlane = (CRecipe413::BESTFITPLN)val;
	CMake::Conv(RefPt1, msg);
	CMake::Conv(RefPt2, msg);
	CMake::Conv(RefPt3, msg);
	CMake::Conv(val, msg);
	BFPlaneCalc = (CRecipe413::BESTFITPLNCALC)val;
}

// Recipe to binary conversion [7/3/2012 user]
long CRecipe413::Binary(char** msg) {
	int i, j;
	short val;
	long sz = 0;

	sz += CMake::Binary(Version, msg);
	CString str = RcpeName;
	sz += CMake::Binary(str, msg);
	sz += CMake::Binary(NMeasureIn, msg);
	sz += CMake::Binary(NMeasureOut, msg);
	sz += CMake::Binary(nExpand, msg);
	sz += CMake::Binary(nMappedPoints, msg);
	sz += Desc.Binary(msg);
	sz += Wp.Binary(msg);
	sz += Me.Binary(msg);
	sz += CMake::Binary(bAutoZTargetdmy, msg);
	sz += CMake::Binary(bEchoPeakMonitor, msg);
	sz += CMake::Binary(fEchoTopZ, msg);
	sz += CMake::Binary(bSaveVideo, msg);
	sz += CMake::Binary(bAllOrNothing, msg);
	sz += CMake::Binary(nTrialPat, msg);
	sz += CMake::Binary(nTrialEcho, msg);
	sz += CMake::Binary(nTrialTrim, msg);
	sz += CMake::Binary(fStepSizePat, msg);
	sz += CMake::Binary(fStepSizeEcho, msg);
	sz += CMake::Binary(TrimHeightDistanceInner1, msg);
	sz += CMake::Binary(TrimHeightDistanceInner2, msg);
	sz += CMake::Binary(TrimHeightDistanceOuter1, msg);
	for (i = 0; i < MAXMEPASET; i++) {
		sz += CMake::Binary(POffsetX[i][1], msg);
		sz += CMake::Binary(POffsetX[i][0], msg);
		sz += CMake::Binary(POffsetY[i][1], msg);
		sz += CMake::Binary(POffsetY[i][0], msg);
		sz += CMake::Binary(PatMatOpt[i], msg);
		sz += CMake::Binary(MatchRes[i], msg);
		sz += CMake::Binary(SearchDept[i], msg);
		sz += CMake::Binary(SearchDist[i], msg);
		sz += CMake::Binary(MatchAreaDmy[i], msg);
		sz += CMake::Binary(MatchAlgorithm[i], msg);
		sz += CMake::Binary(GoFScore[i], msg);
		sz += CMake::Binary(SubSamI[i], msg);
		sz += CMake::Binary(SubSamS[i], msg);
		sz += CMake::Binary(FFTFilter[i], msg);
	}
	sz += CMake::Binary(bUseMotorFile, msg);
	sz += CMake::Binary(TrimWidthOuterRadius, msg);
	sz += CMake::Binary(EstNTrimWidth, msg);
	sz += CMake::Binary(TFZOffset, msg);
	sz += CMake::Binary(TrimHeightDistanceOuter2, msg);
	sz += CMake::Binary(TrimHeightZOffset, msg);
	sz += CMake::Binary(EchoNormalExposure, msg);
	sz += CMake::Binary(TFNormalExposure, msg);
	sz += CMake::Binary(TFTakeSpecExposure, msg);
	sz += CMake::Binary(WaferTimeOut, msg);
	sz += CMake::Binary(FailCountTimeOut, msg);
	sz += CMake::Binary(AlgnXPos, msg);
	sz += CMake::Binary(AlgnYPos, msg);
	sz += CMake::Binary(AlgnZPos, msg);
	sz += CMake::Binary(BumpOffsetX, msg);
	sz += CMake::Binary(BumpOffsetY, msg);
	sz += CMake::Binary(PointTimeOut, msg);
	sz += CMake::Binary(nCycle, msg);
	sz += CMake::Binary(TrimWidthLines, msg);
	sz += CMake::Binary(THExposure, msg);
	sz += CMake::Binary(bLocateEdgeStep, msg);
	sz += CMake::Binary(bLocateEdgeWidth, msg);
	sz += CMake::Binary(TWExposureInner, msg);
	sz += CMake::Binary(TrimWidthSpacing, msg);
	for (i = 0; i < MAXMATCHPATT; i++) {
		sz += MeParam[i].Binary(msg);
	}
	sz += CMake::Binary(fThinFilmZ, msg);
	sz += Patt.MP.Binary(msg);
	sz += CMake::Binary(AutoExposeTF, msg);
	sz += CMake::Binary(AutoExposeEcho, msg);
	sz += CMake::Binary(FindTrimThres, msg);
	sz += CMake::Binary(SamplePerPoint, msg);
	sz += CMake::Binary(MotorFile, msg);
	sz += CMake::Binary(SamplingFreq, msg);
	sz += CMake::Binary(NumberOfInterestedData, msg);
	sz += CMake::Binary(EchoFocus, msg);
	sz += CMake::Binary(fTGtRedXdmy, msg);
	sz += CMake::Binary(fTGtRedYdmy, msg);
	for (i = 0; i < MAXMATCHPATT; i++) {
		for (j = 0; i < MAXTGTSET; j++) {
			sz += CMake::Binary(fTgtGRnX[i][j], msg);
			sz += CMake::Binary(fTgtGRnY[i][j], msg);
			sz += CMake::Binary(fTgtBLuX[i][j], msg);
			sz += CMake::Binary(fTgtBLuY[i][j], msg);
		}
	}
	sz += CMake::Binary(bEnableAutoZ, msg);
	sz += CMake::Binary(fAutoZValue, msg);
	sz += CMake::Binary(dmyCXOffset, msg);
	sz += CMake::Binary(dmyCYOffset, msg);
	sz += CMake::Binary(TWExposureOuter, msg);
	sz += CMake::Binary(NumLines, msg);
	sz += CMake::Binary(PointsPerLine, msg);
	sz += CMake::Binary(ScanLength, msg);
	sz += CMake::Binary(TWExposureBevel, msg);
	val = BFPlane;
	sz += CMake::Binary(val, msg);
	sz += CMake::Binary(RefPt1, msg);
	sz += CMake::Binary(RefPt2, msg);
	sz += CMake::Binary(RefPt3, msg);
	val = BFPlaneCalc;
	sz += CMake::Binary(val, msg);

	return sz;
}

// Note: return structure size instead of usage size [5/2/2008 Yuen]
long CRecipe413::GetLength() {
	int i, j;
	short val = 0;
	long sz = 0;

	sz += CMake::GetLength(Version);
	CString str = RcpeName;
	sz += CMake::GetLength(str);
	sz += CMake::GetLength(NMeasureIn);
	sz += CMake::GetLength(NMeasureOut);
	sz += CMake::GetLength(nExpand);
	sz += CMake::GetLength(nMappedPoints);
	sz += Desc.GetLength();
	sz += Wp.GetLength();
	sz += Me.GetLength();
	sz += CMake::GetLength(bAutoZTargetdmy);
	sz += CMake::GetLength(bEchoPeakMonitor);
	sz += CMake::GetLength(fEchoTopZ);
	sz += CMake::GetLength(bSaveVideo);
	sz += CMake::GetLength(bAllOrNothing);
	sz += CMake::GetLength(nTrialPat);
	sz += CMake::GetLength(nTrialEcho);
	sz += CMake::GetLength(nTrialTrim);
	sz += CMake::GetLength(fStepSizePat);
	sz += CMake::GetLength(fStepSizeEcho);
	sz += CMake::GetLength(TrimHeightDistanceInner1);
	sz += CMake::GetLength(TrimHeightDistanceInner2);
	sz += CMake::GetLength(TrimHeightDistanceOuter1);
	for (i = 0; i < MAXMEPASET; i++) {
		sz += CMake::GetLength(POffsetX[i][1]);
		sz += CMake::GetLength(POffsetX[i][0]);
		sz += CMake::GetLength(POffsetY[i][1]);
		sz += CMake::GetLength(POffsetY[i][0]);
		sz += CMake::GetLength(PatMatOpt[i]);
		sz += CMake::GetLength(MatchRes[i]);
		sz += CMake::GetLength(SearchDept[i]);
		sz += CMake::GetLength(SearchDist[i]);
		sz += CMake::GetLength(MatchAreaDmy[i]);
		sz += CMake::GetLength(MatchAlgorithm[i]);
		sz += CMake::GetLength(GoFScore[i]);
		sz += CMake::GetLength(SubSamI[i]);
		sz += CMake::GetLength(SubSamS[i]);
		sz += CMake::GetLength(FFTFilter[i]);
	}
	sz += CMake::GetLength(bUseMotorFile);
	sz += CMake::GetLength(TrimWidthOuterRadius);
	sz += CMake::GetLength(EstNTrimWidth);
	sz += CMake::GetLength(TFZOffset);
	sz += CMake::GetLength(TrimHeightDistanceOuter2);
	sz += CMake::GetLength(TrimHeightZOffset);
	sz += CMake::GetLength(EchoNormalExposure);
	sz += CMake::GetLength(TFNormalExposure);
	sz += CMake::GetLength(TFTakeSpecExposure);
	sz += CMake::GetLength(WaferTimeOut);
	sz += CMake::GetLength(FailCountTimeOut);
	sz += CMake::GetLength(AlgnXPos);
	sz += CMake::GetLength(AlgnYPos);
	sz += CMake::GetLength(AlgnZPos);
	sz += CMake::GetLength(BumpOffsetX);
	sz += CMake::GetLength(BumpOffsetY);
	sz += CMake::GetLength(PointTimeOut);
	sz += CMake::GetLength(nCycle);
	sz += CMake::GetLength(TrimWidthLines);
	sz += CMake::GetLength(THExposure);
	sz += CMake::GetLength(bLocateEdgeStep);
	sz += CMake::GetLength(bLocateEdgeWidth);
	sz += CMake::GetLength(TWExposureInner);
	sz += CMake::GetLength(TrimWidthSpacing);
	for (i = 0; i < MAXMATCHPATT; i++) {
		sz += MeParam[i].GetLength();
	}
	sz += CMake::GetLength(fThinFilmZ);
	sz += Patt.MP.GetLength();
	sz += CMake::GetLength(AutoExposeTF);
	sz += CMake::GetLength(AutoExposeEcho);
	sz += CMake::GetLength(FindTrimThres);
	sz += CMake::GetLength(SamplePerPoint);
	sz += CMake::GetLength(MotorFile);
	sz += CMake::GetLength(SamplingFreq);
	sz += CMake::GetLength(NumberOfInterestedData);
	sz += CMake::GetLength(EchoFocus);
	sz += CMake::GetLength(fTGtRedXdmy);
	sz += CMake::GetLength(fTGtRedYdmy);
	for (i = 0; i < MAXMATCHPATT; i++) {
		for (j = 0; j < MAXTGTSET; j++) {
			sz += CMake::GetLength(fTgtGRnX[i][j]);
			sz += CMake::GetLength(fTgtGRnY[i][j]);
			sz += CMake::GetLength(fTgtBLuX[i][j]);
			sz += CMake::GetLength(fTgtBLuY[i][j]);
		}
	}
	sz += CMake::GetLength(bEnableAutoZ);
	sz += CMake::GetLength(fAutoZValue);
	sz += CMake::GetLength(dmyCXOffset);
	sz += CMake::GetLength(dmyCYOffset);
	sz += CMake::GetLength(TWExposureOuter);
	sz += CMake::GetLength(NumLines);
	sz += CMake::GetLength(PointsPerLine);
	sz += CMake::GetLength(ScanLength);
	sz += CMake::GetLength(TWExposureBevel);
	sz += CMake::GetLength(val/*BFPlane*/);
	sz += CMake::GetLength(RefPt1);
	sz += CMake::GetLength(RefPt2);
	sz += CMake::GetLength(RefPt3);
	sz += CMake::GetLength(val/*BFPlaneCalc*/);

	return sz;
}

char* CRecipe413::GetRecipeName() {
	return RcpeName;
}

void CRecipe413::SetRecipeName(char* name) {
	strscpy(RcpeName, RCPNAMELEN, name);
}

BOOL CRecipe413::Load(CString RcpName) {
	CFile theFile;
	if (theFile.Open(RcpName, CFile::modeRead)) {
		Clear();
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		DosUtil.ExtractFilename(RcpName);
		strscpy(RcpeName, RCPNAMELEN, RcpName.GetBuffer(0));
		LoadBaseline();  // Load data from baseline file and save in CMPoint::baseline [10/31/2011 Administrator]
		return TRUE;
	}
	return FALSE;
}

BOOL CRecipe413::LoadDefDir(CString defDir) {
	CString filename;
	filename = DosUtil.GetRecipeDir();
	filename += "\\";
	filename += defDir;
	//	filename += ".RCP";
	return Load(filename);
}

short CRecipe413::GetBinarySize() {
	return GetLength();
}

short CRecipe413::RecipeToBinary(BYTE* RpcBody, UINT size) {
	Binary((char**)&RpcBody);
	return 0;
}

short CRecipe413::BinaryToRecipe(BYTE* RcpBody, UINT size) {
	Conv((char**)&RcpBody);
	return 0;
}

void CRecipe413::UpdateControl(CListCtrl& Ctrl) {
	Patt.UpdateControl(Ctrl);
}

BOOL CRecipe413::SaveBaseline() {
	CFileDialog dlg(FALSE,
		"RCP",
		NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Baseline file|*.BLN|");

	CString filename = "";
	DosUtil.GetProgramDir(filename);
	CString bl = filename + "Baseline";
	dlg.m_ofn.lpstrInitialDir = bl;

	if (dlg.DoModal() == IDOK) {
		MakeBaseline();
		filename = dlg.GetPathName();
		return SaveRecipe(filename, FALSE);
	}
	return FALSE;
}

BOOL CRecipe413::SaveRecipe(CString filename, BOOL bSort) {
	CFile theFile;
	if (theFile.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		if (bSort) {
			Patt.MP.Sort();
			Renumber();
		}
		Patt.MP.EnsureData();
		//}
		CString name;
		name = filename;
		DosUtil.ExtractFilename(name);
		strscpy(RcpeName, RCPNAMELEN, name.GetBuffer(0));
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

void CRecipe413::Renumber() {
	Patt.Renumber();
}

void CRecipe413::Verbose(CString fname) {
	FILE* fp;

	fp = fopen(fname, "wb");
	if (fp) {
		VerboseOut(fp);
		fclose(fp);
	}
}

void CRecipe413::VerboseOut(FILE* fp) {
	if (!fp) {
		return;
	}

	int i, j;

	fprintf(fp, "Recipe\n");
	fprintf(fp, "Recipe name,%s\n", RcpeName);
	fprintf(fp, "Version,%d\n", Version);
	fprintf(fp, "File type,%s\n", ObjClass);
	fprintf(fp, "Creator,%s\n", Creator);
	fprintf(fp, "Create time, %d/%d/%d, %d:%d:%d\n", CreateTime.wYear, CreateTime.wMonth, CreateTime.wDay, CreateTime.wHour, CreateTime.wMinute, CreateTime.wSecond);
	fprintf(fp, "Modify time, %d/%d/%d, %d:%d:%d\n", ModifiedTime.wYear, ModifiedTime.wMonth, ModifiedTime.wDay, ModifiedTime.wHour, ModifiedTime.wMinute, ModifiedTime.wSecond);
	fprintf(fp, "Baseline filename, %s\n", BaseLineFile.GetBuffer(0));

	fprintf(fp, "\n");
	Desc.VerboseOut(fp);

	fprintf(fp, "\n");
	Wp.VerboseOut(fp);

	fprintf(fp, "\n");
	BatchDoc.VerboseOut(fp);

	fprintf(fp, "\n");
	Me.VerboseOut(fp);

	fprintf(fp, "\n");
	fprintf(fp, "Mask file name, %s\n", m_strMaskFileName.GetBuffer(0));
	// 	fprintf(fp,"Auto Z target, %d\n", bAutoZTarget);
	fprintf(fp, "Echo peak monitor, %d\n", bEchoPeakMonitor);
	fprintf(fp, "Z Positions, Echo=%.4f, TF=%.4f\n", fEchoTopZ, fThinFilmZ);
	fprintf(fp, "Save video, %d\n", bSaveVideo);
	fprintf(fp, "Search around for echo, #Trial=%d, Step size=%.3f\n", nTrialEcho, fStepSizeEcho);
	fprintf(fp, "Search around for pattern matching, #Trial=%d, Step size=%.3f\n", nTrialPat, fStepSizePat);
	fprintf(fp, "Trim height, Inner distance1=%.3f, Inner distance2=%.3f, Outer distance=(%.3f.%.3f), Z Offset=%.3f\n", TrimHeightDistanceInner1, TrimHeightDistanceInner2, TrimHeightDistanceOuter1, TrimHeightDistanceOuter2, TrimHeightZOffset);
	for (i = 0; i < MAXMEPASET; i++) {
		fprintf(fp, "\n");
		fprintf(fp, "MeSet %d\n", i);
		fprintf(fp, "Offset, P1=%.3f.%.3f, P2=%.3f.%.3f\n", POffsetX[i][0], POffsetY[i][0], POffsetX[i][1], POffsetY[i][1]);
		fprintf(fp, "Pattern matching bits, %x\n", PatMatOpt[i]);
		fprintf(fp, "Pattern matching resolution, %d\n", MatchRes[i]);
		fprintf(fp, "Pattern search depth, %d\n", SearchDept[i]);
		fprintf(fp, "Pattern search distance, %.3f\n", SearchDist[i]);
		fprintf(fp, "Matching Area: %d, Algorithm, %d\n", MatchAreaDmy[i], MatchAlgorithm[i]);
		fprintf(fp, "Matching GOF, %.4f\n", GoFScore[i]);
		fprintf(fp, "Pattern matching sub-sampling, I=%d, S=%d\n", SubSamI[i], SubSamS[i]);
		fprintf(fp, "FFT Filter, %d\n", FFTFilter[i]);
	}
	fprintf(fp, "Fast Match: %d\n", bFastPMatch);
	fprintf(fp, "Use Wafer Centering: %d\n", bUseWaferAlign);
	fprintf(fp, "Trim width, Probe %d, outer radius=%.3f  estimated NTrim width=%.3f\n", MParam::WTRIMPRB1, TrimWidthOuterRadius, EstNTrimWidth);
	fprintf(fp, "Thin film, Z Offset from focus=%.3f\n", TFZOffset);
	fprintf(fp, "Thin film, Exposure(Normal)=%d Exposure2=%d\n", TFNormalExposure, TFTakeSpecExposure);
	fprintf(fp, "Pattern matching, Auto exposure preset=%d(TF) %d(Echo)\n", AutoExposeTF, AutoExposeEcho);
	fprintf(fp, "Trim width, # Lines=%d, Spacing=%d\n", TrimWidthLines, TrimWidthSpacing);
	fprintf(fp, "Trim width, Threshold=%.2f\n", FindTrimThres);
	fprintf(fp, "Trim width, Sample per point=%d\n", SamplePerPoint);
	fprintf(fp, "Trim width, ExposureInner=%.2f,  ExposureOuter=%.2f,  ExposureBevel=%.2f\n", TWExposureInner, TWExposureOuter, TWExposureBevel);
	fprintf(fp, "Trim width:, AutoExposuInner=%hd,  AutoExposeOuter=%hd\n", AutoExposeInner, AutoExposeOuter);
	fprintf(fp, "Trim height:, Expose=%.2f  LocateInnerEdge=%d,%d\n", THExposure, bLocateEdgeStep, bLocateEdgeWidth);
	for (i = 0; i < MAXMEPASET; i++) {
		MeParam[i].VerboseOut(fp);
	}
	fprintf(fp, "Measure Outer Edge: %d\n", bMeasureOuterdge);
	fprintf(fp, "Motor file: %s\n", MotorFile.GetBuffer(0));
	fprintf(fp, "Sampling frequency: %d\n", SamplingFreq);
	fprintf(fp, "Number of data points: %d\n", NumberOfInterestedData);

	fprintf(fp, "Cross hair 2(Red): %.2f, %.2f\n", fTGtRedXdmy, fTGtRedYdmy);
	for (i = 0; i < MAXMATCHPATT; i++) {
		for (j = 0; j < MAXTGTSET; j++) {
			fprintf(fp, "Cross hair 1.%d.%d (Green): %.2f, %.2f\n", i + 1, j + 1, fTgtGRnX[i][j], fTgtGRnY[i][j]);
			fprintf(fp, "Cross hair 3.%d.%d (Blue): %.2f, %.2f\n", i + 1, j + 1, fTgtBLuX[i][j], fTgtBLuY[i][j]);
		}
	}

	fprintf(fp, "Enable Auto Z: %d, value: %.3f\n", bEnableAutoZ, fAutoZValue);

	fprintf(fp, "Number of lines: %d\n", NumLines);
	fprintf(fp, "Points per line: %d\n", PointsPerLine);
	fprintf(fp, "Scan length: %d\n", ScanLength);

	fprintf(fp, "Best Fit Plane: %d\n", BFPlane);
	fprintf(fp, "Best Fit Reference Points: %d %d %d\n", RefPt1, RefPt2, RefPt3);
	fprintf(fp, "Find Edge: Type: %d, Method: %d, Wafer Size: %.3f, Test Result: %d\n", FindEdgeType, FindEdgeMethod, FindEdgeWaferSize, bTestFindEdgeResult);
	fprintf(fp, "Best Fit Plane Calculation: %d\n", BFPlaneCalc);

	for (i = 0; i < MAXXYPAIR; i++) {
		fprintf(fp, "Dice Point %d: %.3f %.3f\n", i + 1, XYPair[i].x, XYPair[i].y);
	}
	for (i = 0; i < MAXXYPAIROFFSET; i++) {
		fprintf(fp, "Dice Point %d offset: %.3f %.3f\n", i + 1, XYPairOffset[i].x, XYPairOffset[i].y);
	}
}

BOOL CRecipe413::IsPatternMatching() {
	CPtrList* pMPList = (CPtrList*)&Patt.MP.MPLst;

	if (!pMPList) {
		return FALSE;
	}

	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p && p->PatSet) {
			return TRUE;
		}
	}
	return FALSE;
}

// idx ranges from 0 to MAXPATTERN * MAXPA [11/8/2011 Administrator]
void CRecipe413::BaselineCorrectionA(CRecipe413* pBsl, short idx) {
	CCoor co;
	float dist;
	CPtrList* pO = &Patt.MP.MPLst;
	POSITION pos = pO->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pO->GetNext(pos);
		if (p) {
			CData* d = p->GetData(0, FALSE);
			if (d) {
				CMPoint* q = pBsl->Patt.FindNearestMP(p->Co, dist);
				if (q) {
					CData* dd = q->GetData(0, FALSE);
					if (dd) {
						d->DVal[idx] -= q->baseline;
					}
				}
			}
		}
	}
}

void CRecipe413::LoadBaseline() {
	return;	// temporary disable [10/31/2011 Administrator]
}

// Baseline file must be located in BASELINE directory [9/10/2012 Administrator]
void CRecipe413::AddBaseline() {
	if (BaseLineFile.GetLength() < 1) {
		ClearBaseLine();
		return;
	}
	CRecipe413* pBL = new CRecipe413;
	if (!pBL) {
		return;
	}
	CString Pathname = "";
	DosUtil.GetProgramDir(Pathname);
	Pathname += "BASELINE\\";
	Pathname += BaseLineFile;
	if (pBL->Load(Pathname)) {
		CCoor co;
		float dist;
		CPtrList* pO = &Patt.MP.MPLst;
		POSITION pos = pO->GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)pO->GetNext(pos);
			if (p) {
				CMPoint* q = pBL->Patt.FindNearestMP(p->Co, dist);
				if (q) {
					p->baseline = q->baseline;
				}
			}
		}
		CString msg;
		msg.Format("Baseline %s loaded", Pathname);
		p413App->Log(msg);
	}
	delete pBL;
}

void CRecipe413::ClearBaseLine() {
	CPtrList* pO = &Patt.MP.MPLst;
	POSITION pos = pO->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pO->GetNext(pos);
		if (p) {
			p->baseline = 0.0f;
		}
	}
}

void CRecipe413::BaselineCorrectSelf() {
	CFileDialog dlg(TRUE,
		"BLN",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		"Baseline file|*.BLN|");

	CString filename = "";
	DosUtil.GetProgramDir(filename);
	CString bl = filename + "BASELINE";
	dlg.m_ofn.lpstrInitialDir = bl;

	if (dlg.DoModal() != IDOK) {
		return;
	}

	filename = dlg.GetPathName();

	CRecipe413* pBL = new CRecipe413;
	if (pBL && pBL->Load(filename)) {
		float dist;
		CPtrList* pO = &Patt.MP.MPLst;
		POSITION pos = pO->GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)pO->GetNext(pos);
			if (p) {
				CMPoint* q = pBL->Patt.FindNearestMP(p->Co, dist);
				if (q) {
					int j = p->MeSet - 1;
					CMeParam* pMeP = &MeParam[j];
					for (int i = 0; i < MaXPA; i++) {
						if (pMeP->MPa[i].Prb == MParam::ECHO1PRB) {
							short ix = j * MaXPA + i;
							CData* d = p->GetData(0, FALSE);
							if (d) {
								p->baseline = q->baseline;
								d->DVal[ix] += q->baseline;
							}
						}
						else if (pMeP->MPa[i].Prb == MParam::ECHO2PRB) {
							short ix = j * MaXPA + i;
							CData* d = p->GetData(0, FALSE);
							if (d) {
								p->baseline = q->baseline;
								d->DVal[ix] -= q->baseline;
							}
						}
					}
				}
			}
		}
		delete pBL;
	}
}

void CRecipe413::RemoveBaseLineSelf() {
	CPtrList* pO = &Patt.MP.MPLst;
	POSITION pos = pO->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pO->GetNext(pos);
		if (p) {
			int j = p->MeSet - 1;
			CMeParam* pMeP = &MeParam[j];
			for (int i = 0; i < MaXPA; i++) {
				if (pMeP->MPa[i].Prb == MParam::ECHO1PRB) {
					short ix = j * MaXPA + i;
					CData* d = p->GetData(0, FALSE);
					if (d) {
						d->DVal[ix] -= p->baseline;
						p->baseline = 0;
					}
				}
				else if (pMeP->MPa[i].Prb == MParam::ECHO2PRB) {
					short ix = j * MaXPA + i;
					CData* d = p->GetData(0, FALSE);
					if (d) {
						d->DVal[ix] += p->baseline;
						p->baseline = 0;
					}
				}
			}
		}
	}
}


BOOL CRecipe413::ExportData(CFile& fp, BOOL bIncBaseline) {
	CString str;
	////17122024
	if (IsConfocalThicknessMeasurement() || IsEchoThicknessMeasurement()) {
		isRangeType = true;
	}
	//
	str = "No,X(mm), Y(mm)";

	int i, j;
	for (i = 0; i < MAXMEPASET; i++) {
		CMeParam* Me = &MeParam[i];
		for (j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				str += "," + Me->MPa[j].Name;
			}
		}
	}
	if (bIncBaseline) {
		str += ", Baseline";
	}
	str += "\n";
	fp.Write(str, str.GetLength());

	CPtrList* pMPList = &Patt.MP.MPLst;

	// Getting ready to measure [7/12/2010 128 NT]
	int cnt2 = 0;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if ((p->MeSet < 1) || (p->MeSet > MaXPA)) {
				continue;
			}
			str = GetReportPointStr(cnt2++, p);
			if (bIncBaseline) {
				CString str1;
				str1.Format(",%.2f", p->baseline);
				str += str1;
			}
			str += "\n";
			fp.Write(str, str.GetLength());
		}
	}

	str = "\n\nStatistics\nName";
	for (i = 0; i < MAXMEPASET; i++) {
		CMeParam* Me = &MeParam[i];
		for (j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				str = str + "," + Me->MPa[j].Name;
			}
		}
	}
	str += "\n";
	fp.Write(str, str.GetLength());

	CString StatName[8] = { "Max", "Min", "Ave", "StDev", "TTV", "Bow", "Warp" };
	//[12172024
	if (isRangeType == true) {
		StatName[4] = "Range";
	}//]

	for (int k = 0; k < 7; k++)  // 7 Named items [3/9/2011 FSMT]
	{
		//int cnt1=1;
		str = StatName[k];
		for (j = 0; j < MAXMEPASET; j++) {
			short jj = j * MaXPA;
			CMeParam* pMeP = &MeParam[j];
			for (i = 0; i < MaXPA; i++) {
				if (pMeP->MPa[i].Prb > 0) {
					CStat Stats(Patt.MP.Stats[jj + i]);
					// [06262024 ZHIMING
					if (pMeP->MPa[i].Prb == MParam::ROUGHPRB) {
						Stats.Divide(p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f);
					}
					// ]
					CString str1;
					switch (k) {
					case 0:	// Max [9/8/2012 Administrator]
						str1.Format("%.3f", Stats.m_fMax);
						break;
					case 1: // Min [9/8/2012 Administrator]
						str1.Format("%.3f", Stats.m_fMin);
						break;
					case 2: // Ave [9/8/2012 Administrator]
						str1.Format("%.3f", Stats.m_fAverage);
						break;
					case 3: // StDev [9/8/2012 Administrator]
						str1.Format("%.3f", Stats.m_fSTD);
						break;
					case 4: // TTV [9/8/2012 Administrator]
						if (pMeP->MPa[i].Fr == 1) {
							str1 = "";
							if (pMeP->MPa[i].IsTTV()) {
								str1.Format("%.3f", Stats.m_fTTV);
							}
						}
						else {
							str1.Format("%.3f", Stats.m_fTTV);
						}
						break;
					case 5: // Bow [9/8/2012 Administrator]
					case 6: // Warp [9/8/2012 Administrator]
						switch (pMeP->MPa[i].Prb) {
						case MParam::BOWWARP1:
						case MParam::BOWWARP2:
						case MParam::CONFOCAL_WARP: //20221108
							switch (k) {
							case 5:
								str1.Format("%.3f", Stats.m_fBow);
								break;
							case 6:
								str1.Format("%.3f", Stats.m_fWarp);
								break;
							}
							break;
						case MParam::CONFOCAL_DUALWARP: //23102024
							switch (k) {
							case 5:
								str1.Format("%.3f", Stats.m_fBow);
								break;
							case 6:
								str1.Format("%.3f", Stats.m_fWarp);
								break;
							}
							break;
						default:
							str1 = "";
							break;
						}
						break;
					}
					str = str + "," + str1;
					//[ 09292025/ save average thickness of each mes
					if (k == 2 && pMeP->MPa[i].Prb == MParam::CONFOCAL_TOTAL) { // k=2 meaqns this is average
						SYSTEMTIME st;
						GetSystemTime(&st);
						CString Date;
						Date.Format(_T("DATE_%04d.%02d.%02d  TIME_%02d_%02d_%02d"),st.wYear, st.wMonth, st.wDay,st.wHour, st.wMinute, st.wSecond);
						std::ofstream file("C:\\DATA\\TTK_LOG.csv", std::ios::app);
						if (file.is_open()) {
							file << Date << "," << Stats.m_fAverage << "\n";
							file.close();
						}

					}
						
					//]
				}
			}
		}
		str += "\n";
		fp.Write(str, str.GetLength());
	}

	return TRUE;
}

// Will not include new line character [10/4/2012 Yuen]
CString CRecipe413::GetReportPointStr(int cnt, CMPoint* p) {
	int i, j;
	CString str;
	CString total;
	str.Format("%d,", cnt + 1);
	total += str;
	total += p->GetXCoorStr() + ",";
	total += p->GetYCoorStr();
	CData* d = p->GetData(0, FALSE);

	CMeParam* Me;
	// Skip prior MeSet by replacing it with comma  [9/5/2012 Administrator]
	for (j = 0; j < p->MeSet - 1; j++) {
		Me = &MeParam[j];
		for (i = 0; i < MaXPA; i++) {
			if (Me->MPa[i].Prb > 0) {
				total += ",";
			}
		}
	}
	short MeSetX = MaXPA * (p->MeSet - 1);
	Me = &MeParam[p->MeSet - 1];
	for (i = 0; i < MaXPA; i++) {
		if (Me->MPa[i].Prb > 0) {
			float fData = d->Get(MeSetX + i);
			// [ 10222025 MORSALIN
			if (Me->MPa[i].Prb == MParam::CONFOCAL_LTV_MIN ||
				Me->MPa[i].Prb == MParam::CONFOCAL_LTV_MAX ||
				Me->MPa[i].Prb == MParam::CONFOCAL_LTV_AVG)
				continue;
			// ]
			// [06262024 ZHIMING
			if (Me->MPa[j].Prb == MParam::ROUGHPRB) {
				fData /= p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f;
			}
			// ]
			if (fData != BADDATANEG) {
				CString dec1, inifile;
				DosUtil.GetLocalCfgFile(inifile);
				int prec = GetPrivateProfileInt("ThinFilm", "Precision", 0, inifile);
				if (prec != 0)
					Me->MPa[i].D = prec;
				dec1.Format(",%%.%df", Me->MPa[i].D);
				str.Format(dec1, fData);
			}
			else {
				str = ",N/A";
			}
			total += str;
		}
	}
	return total;
}

BOOL CRecipe413::IsEchoBowWarpMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsEchoBowWarpMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsProfileMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsProbeToSurface()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsTotalThickness() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsTotalThicknessMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsEchoMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsEchoMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsConfocalMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsConfocalMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CRecipe413::IsConfocalMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsConfocalMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

//[12102024
BOOL CRecipe413::IsConfocalRoughnessMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsConfocalRoughnessMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsConfocalRoughnessMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsConfocalRoughnessMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

//]


// [ 24102024/MORSALIN
BOOL CRecipe413::IsConfocalLocalMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsConfocalLocalMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CRecipe413::IsConfocalLocalMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsConfocalLocalMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
// ]

//[23102024
BOOL CRecipe413::IsConfocalDualWarpMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsConfocalDualWarpMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsConfocalDualWarpMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsConfocalDualWarpMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

//]

BOOL CRecipe413::IsConfocalWarpMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsConfocalWarpMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}
BOOL CRecipe413::IsConfocalWarpMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsConfocalWarpMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

//[ Tushar //20231227

BOOL CRecipe413::IsConfocalThicknessMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsConfocalTotalThicknessMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsConfocalThicknessMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsConfocalTotalThicknessMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
//]

BOOL CRecipe413::IsThinFilmMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsThinFilmMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsEdgeMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsEdgeMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsHeightMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsHeightMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsMultiPointMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsMultiPointMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsStressMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsStressMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsBevelUsingTF() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsBevelUsingTF()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsBevelMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsBevelMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsProbe(MParam::PRBNUM Prb) {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->Prb == Prb) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsValidMeSet(CMPoint* p) {
	if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
		return TRUE;
	}
	return FALSE;
}

BOOL CRecipe413::IsTotalThicknessMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsTotalThicknessMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsThinFilmMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsThinFilmMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

short CRecipe413::IsThinFilmMarker(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return 0;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		switch (pM->IsThinFilmMarker()) {
		case 1:
			return 1;
		case 2:
			return 2;
		}
	}
	return 0;
}

short CRecipe413::IsEchoMarker(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return 0;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		switch (pM->IsEchoMarker()) {
		case 1:
			return 1;
		case 2:
			return 2;
		}
	}
	return 0;
}

BOOL CRecipe413::IsBevelMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsBevelMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsEdgeMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsEdgeMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsHeightMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsHeightMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsMultiPointMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsMultiPointMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsEchoMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsEchoMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsEchoBowWarpMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsEchoBowWarpMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsProfileMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsProbeToSurface()) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsStressMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsStressMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}
//[ 03202024 ----> Mahedi Kamal {for adding rough probe}
/*returns true for specific point of the recipe if has roughness*/
BOOL CRecipe413::IsRoughnessMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsRoughnessMeasurement()) {
			return TRUE;
		}
	}
	return FALSE;
}

/*returns true if any point of the recipe has roughness*/
BOOL CRecipe413::IsRoughnessMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsRoughnessMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
//]

//[ 08132024 ----> Mahedi Kamal {for adding Ring Width probe}
/*returns true for specific point of the recipe if has Ring Width*/
BOOL CRecipe413::IsRingWidthMeasurement(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		if (pM->IsRingWidthMeasurement()) {
			return TRUE;
		}
	}
	return FALSE; 
}

/*returns true if any point of the recipe has Ring Width*/
BOOL CRecipe413::IsRingWidthMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsRingWidthMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
//]

BOOL CRecipe413::IsAutoFocus(CMPoint* p) {
	if (!IsValidMeSet(p)) {
		return FALSE;
	}
	for (int i = 0; i < MaXPA; i++) {
		MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
		switch (pM->Prb) {
		case MParam::ECHO1AFPRB:
		case MParam::ECHO2AFPRB:
		case MParam::ECHOTTAFPRB:
			return TRUE;
			break;
		}
	}
	return FALSE;
}

BOOL CRecipe413::IsAutoFocus() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsAutoFocus()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL CRecipe413::ChangePaSet(short n) {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->PatSet > 0) {
				p->PatSet = n;
			}
		}
	}
	return TRUE;
}

BOOL CRecipe413::IsMarkers() {
	short cnt = 0;
	CPtrList* pMPList = &Patt.MP.MPLst;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->Co.p > 0) {
				cnt++;
			}
		}
	}
	if (cnt >= 3) {
		return TRUE;
	}
	return FALSE;
}

//[ 04182024 Tushar
float CRecipe413::GetRadius() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return -1;
	}
	float mxPoint = -FLT_MAX;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			float X, Y;
			p->GetCoor(X, Y);
			float tempX = (X >= 0) ? X : -X;
			float tempY = (Y >= 0) ? Y : -Y;
			mxPoint = max(mxPoint, max(tempX, tempY));
		}
	}
	return mxPoint;
}

float CRecipe413::CalculateWaferSize() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Measurement";
	GetPrivateProfileString(sec, "VacuumLine", "300", str, 255, inifile);

	std::vector<int> numbers;
	std::stringstream ss(str);
	std::string numberStr;

	while (std::getline(ss, numberStr, ',')) {
		int number = std::stoi(numberStr);
		number /= 2;
		numbers.push_back(number);
	}

	std::sort(numbers.begin(), numbers.end());

	float fRadius = GetRadius();

	for (auto a : numbers) {
		if (fRadius >= a) continue;
		return a;
	}
	return *max_element(numbers.begin(), numbers.end());
}
//]


//[12172024
/*returns true if any point of the recipe has roughness*/
BOOL CRecipe413::IsEchoThicknessMeasurement() {
	CPtrList* pMPList = &Patt.MP.MPLst;
	if (!pMPList) {
		ASSERT(0);
		return FALSE;
	}
	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if (p->MeSet > 0 && p->MeSet <= MAXMEPASET) {
				for (int i = 0; i < MaXPA; i++) {
					MParam* pM = &MeParam[p->MeSet - 1].MPa[i];
					if (pM->IsEchoThicknessMeasurement()) {
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
//]

//BOOL CRecipe413::ExportMeasSumLog(CString fileName, short SlotID, CString LotID, CString opName, CString WaferID, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime) { //20251022_Mohir
//	//[20251030_Mohir
//	StartT = startTime;
//	EndT = endTime;
//	//]
//	int i, j;
//	CString tmpFileName = "";
//	for (i = 0; i < MAXMEPASET; i++) {
//		short ii = i * MaXPA;
//		CMeParam* Me = &MeParam[i];
//		for (j = 0; j < MaXPA; j++) {
//			if (Me->MPa[j].Prb > 0) {
// 				if (Me->MPa[j].Name.IsEmpty())continue;//777_Mohir
//				tmpFileName = fileName + CTime::GetCurrentTime().Format("\\%Y_") + Me->MPa[j].Name + ".CSV";
//
//				int nTemp = ::GetFileAttributes(tmpFileName);
//				FILE* fp = fopen(tmpFileName, "a");
//				if (!fp) return FALSE;
//				BOOL ret = FALSE;
//
//				int nRow = Patt.MP.MPLst.GetCount();
//				if (nTemp == -1)
//				{
//					CString str, str2 = "";
//					for (int i = 1; i <= nRow; i++)
//					{
//						str.Format("%d,", i); str2 += str;
//					}
//					//20251022_Mohir.. added column Wafer Type, wafer size, station
//					fprintf(fp, "Date,StartTime,EndTime,LotID,WaferID,WaferType,WaferSize,Station,Operator,%sMax,Min,Ave,StDev,TTV,Bow,Warp\n", str2.GetBuffer());
//				}
//
//				CPtrList* pMPList = &Patt.MP.MPLst;
//				int cnt2 = 0;
//				CString Res = "", str = "";
//				POSITION pos = pMPList->GetHeadPosition();
//				while (pos) {
//					CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
//					if (p) {
//						if ((p->MeSet < 1) || (p->MeSet > MaXPA)) {
//							continue;
//						}
//
//						CData* d = p->GetData(0, FALSE);
//						short MeSetX = MaXPA * (p->MeSet - 1);
//						CMeParam* pCurrentMe = &MeParam[p->MeSet - 1];  // Use different variable name
//						if (pCurrentMe->MPa[j].Prb > 0 && pCurrentMe->MPa[j].Prb == pCurrentMe->MPa[j].Prb) {
//							float fData = d->Get(MeSetX + j);
//							if (fData != BADDATANEG) {
//								CString dec1, inifile;
//								DosUtil.GetLocalCfgFile(inifile);
//								int prec = GetPrivateProfileInt("ThinFilm", "Precision", 0, inifile);
//								if (prec != 0)
//									pCurrentMe->MPa[j].D = prec;  // Fixed: use j instead of i
//								dec1.Format("%%.%df", pCurrentMe->MPa[j].D);  // Fixed: use j instead of i
//								str.Format(dec1, fData);
//							}
//							else {
//								str = "N/A";
//							}
//						}
//					}
//					Res += (str + ",");
//				}
//
//				CString str1 = "";
//				str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fMax);
//				Res += (str1 + ",");
//				str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fMin);
//				Res += (str1 + ",");
//				str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fAverage);
//				Res += (str1 + ",");
//				str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fSTD);
//				Res += (str1 + ",");
//
//				if (Me->MPa[j].Fr == 1) {
//					str1 = "";
//					if (Me->MPa[j].IsTTV()) {
//						str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fTTV);
//						Res += (str1 + ",");
//					}
//					else {
//						Res += "N/A,";  // Add empty column for TTV when not applicable
//					}
//				}
//				else {
//					str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fTTV);
//					Res += (str1 + ",");
//				}
//
//				// Fixed Bow/Warp section
//				BOOL bIsBowWarpType = FALSE;
//				switch (Me->MPa[j].Prb) {
//				case MParam::BOWWARP1:
//				case MParam::BOWWARP2:
//				case MParam::CONFOCAL_WARP:      // Add missing cases
//				case MParam::CONFOCAL_DUALWARP:  // Add missing cases
//					bIsBowWarpType = TRUE;
//					str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fBow);
//					Res += (str1 + ",");
//					str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fWarp);
//					Res += str1;
//					break;
//
//				default:
//					// Only set N/A for non-BowWarp types
//					if (!bIsBowWarpType) {
//						str1 = "N/A,N/A";
//						Res += str1;
//					}
//					break;
//				}
//
//				//[20251022_Mohir
//				// Assign default values if variables are empty
//				if (WaferType.IsEmpty())
//					WaferType = "N/A";
//				if (WaferSize.IsEmpty())
//					WaferSize = "N/A";
//				if (Station.IsEmpty())
//					Station = "N/A";
//				if (LotID.IsEmpty())
//					LotID = "N/A";
//				if (WaferID.IsEmpty())
//					WaferID = "N/A";
//				if (opName.IsEmpty())
//					opName = "N/A";
//				if (Res.IsEmpty())
//					Res = "N/A";
//
//				CTime endTime = CTime::GetCurrentTime();
//				fprintf(fp, "%d/%d/%d,%02d:%02d:%02d,%02d:%02d:%02d,%s,%s,%s,%s,%s,%s,%s\n",
//					startTime.GetYear(), startTime.GetMonth(), startTime.GetDay(),
//					startTime.GetHour(), startTime.GetMinute(), startTime.GetSecond(),
//					endTime.GetHour(), endTime.GetMinute(), endTime.GetSecond(),
//					LotID.GetBuffer(), WaferID.GetBuffer(), WaferType.GetBuffer(),
//					WaferSize.GetBuffer(), Station.GetBuffer(), opName.GetBuffer(), Res.GetBuffer());
//				fclose(fp);
//			}
//		}
//	}
//
//	return TRUE;  // Added return statement
//}
/////////////////////////////////////////////////////////////////////////////////////CLOSING ISSUES ///////////////////////////////////////////////////////////

//[20251219_Mohir close file implementation
BOOL CRecipe413::IsFileLocked(const CString& filename)
{
	HANDLE hFile = CreateFile(
		filename,
		GENERIC_READ | GENERIC_WRITE,        // ask for read+write
		0,                                   // do NOT allow sharing
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dw = GetLastError();
		if (dw == ERROR_SHARING_VIOLATION || dw == ERROR_ACCESS_DENIED)
			return TRUE;   // File is opened by another process (e.g., Excel)

		return FALSE;      // Other error (file missing etc.)
	}

	CloseHandle(hFile);
	return FALSE;          // Successfully opened ¡ú NOT locked
}

BOOL CRecipe413::RemoveFileFromTask(const CString& filename)
{
	// ---- Get EXE folder path (ANSI/MBCS) ----
	char exePathA[MAX_PATH];
	GetModuleFileNameA(NULL, exePathA, MAX_PATH);

	CStringA exeDirA = exePathA;
	PathRemoveFileSpecA(exeDirA.GetBuffer());
	exeDirA.ReleaseBuffer();

	// ---- Build ps1 path ----
	CStringA ps1PathA = exeDirA + "\\CommanCloseCSV.ps1";

	// ---- Extract just filename (MBCS safe) ----
	CStringA justFileNameA = CT2A(PathFindFileName(filename));

	// ---- Build PowerShell command (ANSI) ----
	std::string command =
		"powershell.exe -ExecutionPolicy Bypass -File \"" +
		std::string(ps1PathA) +
		"\" -FileName \"" +
		std::string(justFileNameA) +
		"\"";

	// ---- Execute ----
	int result = system(command.c_str());

	if (result == 0)
	{
		Sleep(2000); // allow Excel to close
		return TRUE;
	}

	return FALSE;
}
//]
//[20251022_Mohir
BOOL CRecipe413::ExportMeasSumLog(CString fileName, short SlotID, CString LotID, CString opName, CString WaferID, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime) { //20251022_Mohir
	//[20251030_Mohir
	StartT = startTime;
	EndT = endTime;
	//]
	int i, j;
	CString tmpFileName = "";
	for (i = 0; i < MAXMEPASET; i++) {
		short ii = i * MaXPA;
		CMeParam* Me = &MeParam[i];
		for (j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				if (Me->MPa[j].Name.IsEmpty())continue;//777_Mohir
				CString FileN = "";
				FileN = CTime::GetCurrentTime().Format("%Y_") + Me->MPa[j].Name + ".CSV";
				tmpFileName = fileName + "\\" + FileN;
				// ====== SIMPLE FILE LOCK HANDLING ======
				if (IsFileLocked(tmpFileName)) RemoveFileFromTask(FileN); //20251212_Mohir
				// ====== END FILE LOCK HANDLING ======

				int nTemp = ::GetFileAttributes(tmpFileName);
				FILE* fp = fopen(tmpFileName, "a");
				if (!fp) return FALSE;
				BOOL ret = FALSE;

				int nRow = Patt.MP.MPLst.GetCount();
				if (nTemp == -1)
				{
					CString str, str2 = "";
					for (int i = 1; i <= nRow; i++)
					{
						str.Format("%d,", i); str2 += str;
					}
					//20251022_Mohir.. added column Wafer Type, wafer size, station
					fprintf(fp, "Date,StartTime,EndTime,LotID,WaferID,WaferType,WaferSize,Station,Operator,%sMax,Min,Ave,StDev,TTV,Bow,Warp\n", str2.GetBuffer());
				}

				CPtrList* pMPList = &Patt.MP.MPLst;
				int cnt2 = 0;
				CString Res = "", str = "";
				POSITION pos = pMPList->GetHeadPosition();
				while (pos) {
					CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
					if (p) {
						if ((p->MeSet < 1) || (p->MeSet > MaXPA)) {
							continue;
						}

						CData* d = p->GetData(0, FALSE);
						short MeSetX = MaXPA * (p->MeSet - 1);
						CMeParam* pCurrentMe = &MeParam[p->MeSet - 1];  // Use different variable name
						if (pCurrentMe->MPa[j].Prb > 0 && pCurrentMe->MPa[j].Prb == pCurrentMe->MPa[j].Prb) {
							float fData = d->Get(MeSetX + j);
							if (fData != BADDATANEG) {
								CString dec1, inifile;
								DosUtil.GetLocalCfgFile(inifile);
								int prec = GetPrivateProfileInt("ThinFilm", "Precision", 0, inifile);
								if (prec != 0)
									pCurrentMe->MPa[j].D = prec;  // Fixed: use j instead of i
								dec1.Format("%%.%df", pCurrentMe->MPa[j].D);  // Fixed: use j instead of i
								str.Format(dec1, fData);
							}
							else {
								str = "N/A";
							}
						}
					}
					Res += (str + ",");
				}

				CString str1 = "";
				str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fMax);
				Res += (str1 + ",");
				str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fMin);
				Res += (str1 + ",");
				str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fAverage);
				Res += (str1 + ",");
				str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fSTD);
				Res += (str1 + ",");

				if (Me->MPa[j].Fr == 1) {
					str1 = "";
					if (Me->MPa[j].IsTTV()) {
						str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fTTV);
						Res += (str1 + ",");
					}
					else {
						Res += "N/A,";  // Add empty column for TTV when not applicable
					}
				}
				else {
					str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fTTV);
					Res += (str1 + ",");
				}

				// Fixed Bow/Warp section
				BOOL bIsBowWarpType = FALSE;
				switch (Me->MPa[j].Prb) {
				case MParam::BOWWARP1:
				case MParam::BOWWARP2:
				case MParam::CONFOCAL_WARP:      // Add missing cases
				case MParam::CONFOCAL_DUALWARP:  // Add missing cases
					bIsBowWarpType = TRUE;
					str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fBow);
					Res += (str1 + ",");
					str1.Format("%.3f", Patt.MP.Stats[ii + j].m_fWarp);
					Res += str1;
					break;

				default:
					// Only set N/A for non-BowWarp types
					if (!bIsBowWarpType) {
						str1 = "N/A,N/A";
						Res += str1;
					}
					break;
				}

				//[20251022_Mohir
				// Assign default values if variables are empty
				if (WaferType.IsEmpty())
					WaferType = "N/A";
				if (WaferSize.IsEmpty())
					WaferSize = "N/A";
				if (Station.IsEmpty())
					Station = "N/A";
				if (LotID.IsEmpty())
					LotID = "N/A";
				if (WaferID.IsEmpty())
					WaferID = "N/A";
				if (opName.IsEmpty())
					opName = "N/A";
				if (Res.IsEmpty())
					Res = "N/A";

				CTime endTime = CTime::GetCurrentTime();
				fprintf(fp, "%d/%d/%d,%02d:%02d:%02d,%02d:%02d:%02d,%s,%s,%s,%s,%s,%s,%s\n",
					startTime.GetYear(), startTime.GetMonth(), startTime.GetDay(),
					startTime.GetHour(), startTime.GetMinute(), startTime.GetSecond(),
					endTime.GetHour(), endTime.GetMinute(), endTime.GetSecond(),
					LotID.GetBuffer(), WaferID.GetBuffer(), WaferType.GetBuffer(),
					WaferSize.GetBuffer(), Station.GetBuffer(), opName.GetBuffer(), Res.GetBuffer());
				fclose(fp);
			}
		}
	}
	return TRUE;  // Added return statement
}
//]




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//[[20251014_Mohir
BOOL CRecipe413::ExportDataWithHeader(CFile& fp, BOOL bIncBaseline, CString recipe, CString lotId, CString waferID, CString station, CString waferType, CString waferSize, CString opName) {
	CString str;
	////17122024
	if (IsConfocalThicknessMeasurement() || IsEchoThicknessMeasurement()) {
		isRangeType = true;
	}
	//[20251022_Mohir
					// Assign default values if variables are empty
	if (waferType.IsEmpty())
		waferType = "N/A";
	if (waferSize.IsEmpty())
		waferSize = "N/A";
	if (station.IsEmpty())
		station = "N/A";
	if (lotId.IsEmpty())
		lotId = "N/A";
	if (waferID.IsEmpty())
		waferID = "N/A";
	if (opName.IsEmpty())
		opName = "N/A";
	//]
	
	// [ 20250818
	str = "Recipe," + recipe + "\n";
	str += "Lot ID," + lotId + "\n";
	str += "Wafer ID," + waferID + "\n";
	str += "Wafer Type," + waferType + "\n";
	str += "Wafer Size," + waferSize + "\n";
	str += "Station," + station + "\n";
	str += "Operator," + opName + "\n";
	CTime now = CTime::GetCurrentTime();
	str += _T("Date/Time,") + now.Format(_T("%Y-%m-%d %H:%M:%S")) + _T("\n");
	str += _T("Time Start,") + StartT.Format(_T("%H:%M:%S")) + _T("\n"); //20251030_Mohir
	str += _T("Time End,") + EndT.Format(_T("%H:%M:%S")) + _T("\n"); //2021030_Mohir
	str += "No,X(mm), Y(mm)";
	// ]
	int i, j;
	for (i = 0; i < MAXMEPASET; i++) {
		CMeParam* Me = &MeParam[i];
		for (j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				str += "," + Me->MPa[j].Name;
			}
		}
	}
	if (bIncBaseline) {
		str += ", Baseline";
	}
	str += "\n";
	fp.Write(str, str.GetLength());

	CPtrList* pMPList = &Patt.MP.MPLst;

	// Getting ready to measure [7/12/2010 128 NT]
	int cnt2 = 0;
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			if ((p->MeSet < 1) || (p->MeSet > MaXPA)) {
				continue;
			}
			str = GetReportPointStr(cnt2++, p);
			if (bIncBaseline) {
				CString str1;
				str1.Format(",%.2f", p->baseline);
				str += str1;
			}
			str += "\n";
			fp.Write(str, str.GetLength());
		}
	}

	str = "\n\nStatistics\nName";
	for (i = 0; i < MAXMEPASET; i++) {
		CMeParam* Me = &MeParam[i];
		for (j = 0; j < MaXPA; j++) {
			if (Me->MPa[j].Prb > 0) {
				str = str + "," + Me->MPa[j].Name;
			}
		}
	}
	str += "\n";
	fp.Write(str, str.GetLength());

	CString StatName[8] = { "Max", "Min", "Ave", "StDev", "TTV", "Bow", "Warp" };
	//[12172024
	if (isRangeType == true) {
		StatName[4] = "Range";
	}//]

	for (int k = 0; k < 7; k++)  // 7 Named items [3/9/2011 FSMT]
	{
		//int cnt1=1;
		str = StatName[k];
		for (j = 0; j < MAXMEPASET; j++) {
			short jj = j * MaXPA;
			CMeParam* pMeP = &MeParam[j];
			for (i = 0; i < MaXPA; i++) {
				if (pMeP->MPa[i].Prb > 0) {
					CStat Stats(Patt.MP.Stats[jj + i]);
					// [ 10222025 MORSALIN
					if (pMeP->MPa[i].Prb == MParam::CONFOCAL_LTV_MIN ||
						pMeP->MPa[i].Prb == MParam::CONFOCAL_LTV_MAX ||
						pMeP->MPa[i].Prb == MParam::CONFOCAL_LTV_AVG)
						continue;
					// ]
					// [06262024 ZHIMING
					if (pMeP->MPa[i].Prb == MParam::ROUGHPRB) {
						Stats.Divide(p413App->Global.umUnitInRoughness ? 10000.0f : 1.0f);
					}
					// ]
					CString str1;
					switch (k) {
					case 0:	// Max [9/8/2012 Administrator]
						str1.Format("%.3f", Stats.m_fMax);
						break;
					case 1: // Min [9/8/2012 Administrator]
						str1.Format("%.3f", Stats.m_fMin);
						break;
					case 2: // Ave [9/8/2012 Administrator]
						str1.Format("%.3f", Stats.m_fAverage);
						break;
					case 3: // StDev [9/8/2012 Administrator]
						str1.Format("%.3f", Stats.m_fSTD);
						break;
					case 4: // TTV [9/8/2012 Administrator]
						if (pMeP->MPa[i].Fr == 1) {
							str1 = "";
							if (pMeP->MPa[i].IsTTV()) {
								str1.Format("%.3f", Stats.m_fTTV);
							}
						}
						else {
							str1.Format("%.3f", Stats.m_fTTV);
						}
						break;
					case 5: // Bow [9/8/2012 Administrator]
					case 6: // Warp [9/8/2012 Administrator]
						switch (pMeP->MPa[i].Prb) {
						case MParam::BOWWARP1:
						case MParam::BOWWARP2:
						case MParam::CONFOCAL_WARP: //20221108
							switch (k) {
							case 5:
								str1.Format("%.3f", Stats.m_fBow);
								break;
							case 6:
								str1.Format("%.3f", Stats.m_fWarp);
								break;
							}
							break;
						case MParam::CONFOCAL_DUALWARP: //23102024
							switch (k) {
							case 5:
								str1.Format("%.3f", Stats.m_fBow);
								break;
							case 6:
								str1.Format("%.3f", Stats.m_fWarp);
								break;
							}
							break;
						default:
							str1 = "";
							break;
						}
						break;
					}
					str = str + "," + str1;
				}
			}
		}
		str += "\n";
		fp.Write(str, str.GetLength());
	}
	return TRUE;
}

//]]