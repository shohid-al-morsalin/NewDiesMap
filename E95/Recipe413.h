// Recipe413.h: interface for the CRecipe413 class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "..\IPC\SMOL.h"

#include "CRdlPatt.h"
#include "CCcrPatt.h"
#include "CGridPatt.h"
#include "DataDesc413.h"
#include "custpatt.h"
#include "CrossPattern.h"
#include "MaskPattern.h"
#include "CBatchDoc.h"

#include "CWaferParam.h"
#include "MeasurementProperties.h"

#include "MParam.h"
#include "MeParam.h"
#include "PackedDIB1.h"	// Added by ClassView

#include "XYPair.h"
#include "CTaikoRingMeasProp.h" //08142024

// NOTE: Change MAXPATTERNCNT will void recipe file downward compatibility [7/9/2010 128 NT]

#define MAXXYPAIR			3
#define MAXXYPAIROFFSET		16
#define VARPARAMCNT			25

class CRecipe413 : public OSSMemS {
	short repeatdmy;
	// Following variable need to be verify [7/4/2010 Yuen]
public:
	short Version;
	char Creator[RCPNAMELEN + 1];
	char  ObjClass[OBJIDLEN + 1];
	SYSTEMTIME CreateTime;
	SYSTEMTIME ModifiedTime;

public:
	CTime startTime, endTime; // 20251014_Mohir
	char RcpeName[RCPNAMELEN + 1];  // Recipe name [6/24/2010 Yuen]

	short SlotID;		// Intermediate variable, not save to file [11/6/2011 Administrator]
	CString WaferID;	// Intermediate variable, not save to file [11/6/2011 Administrator]
	CString CarrierID;	// Intermediate variable, not save to file [11/6/2011 Administrator]
	short ScanNo;		// Intermediate variable, not save to file [11/6/2011 Administrator]

	short nCycle;		// 0-Angstrong or mils, 1-micron [8/21/2009 FSM]
	short nExpand;
	CDataDesc413 Desc;

	CPattern Patt;
	CCcrPatt ccrpatt;
	CRdlPatt rdlpatt;
	CCustPatt custpatt;
	CGridPatt gridpatt;
	CCrossPattern CrossPattern;
	CMaskPattern MaskPattern;
	CTaikoRingMeasProp TRWMeasProp; //08142024

	MParam Padmy[8];
	CWaferParam Wp;
	CMeasurementProperties Me;

	short nTrimMethod/*NMeasuredmy*/;	// 0 = ExtractLine, 1 = FindEdge [5/20/2014 FSMT]

	/** General information like operator name batch details etc...*/
	CBatchDoc BatchDoc;

	// Matching sub-sampling parameters [6/6/2010 Yuen]
	short NMeasureIn, NMeasureOut;
	short TargetLocX1Dmy, TargetLocY1Dmy;

	CString BaseLineFile;   // baseline file [6/24/2010 Yuen]
	CString m_strMaskFileName;

	CPackedDIB MatchDIBA1[MAXMATCHPATT];		// Pattern template images [6/24/2010 Yuen]
	CPackedDIB MatchDIBB1[MAXMATCHPATT];		// Pattern template images [6/24/2010 Yuen]
	CPackedDIB MatchDIBA2[MAXMATCHPATT];		// Pattern template images [6/24/2010 Yuen]
	CPackedDIB MatchDIBB2[MAXMATCHPATT];		// Pattern template images [6/24/2010 Yuen]
	CPackedDIB MatchDIBA3[MAXMATCHPATT];		// Pattern template images [6/24/2010 Yuen]
	CPackedDIB MatchDIBB3[MAXMATCHPATT];		// Pattern template images [6/24/2010 Yuen]
	CPackedDIB MatchDIBA4[MAXMATCHPATT];		// Pattern template images [6/24/2010 Yuen]
	CPackedDIB MatchDIBB4[MAXMATCHPATT];		// Pattern template images [6/24/2010 Yuen]

	int nMappedPoints;
	float EchoFocus;	// unit is micron [5/2/2013 user]

	int	nFilmMeasTimedmy;

	bool bIsRa_rcp; // 07102024 -> Mahedi Kamal
	BOOL bIsRaCWL_rcp; // 12120224
	BOOL bIsNmCWL;//20250306
	float RoughStepSizeCWL_rcp; // 12120224
	float RoughScanLineLenCWL_rcp; // 12120224
	BOOL bIsAutoFocus008; // if true autofocus will happen while doing roughness with 008cwl

	int thkSearchNum, warpSearchNum;//10032025_3
	float stepSrhSize;
	BOOL isCalibrationAutoFocusNeeded;

	// Obsolete but maintained for downward compatibility [6/24/2010 Yuen]
	double fTapeIndexDmy;
	double fPolyIndexDmy;
	double fSiIndexDmy;
	BOOL bEchoPeakMonitor;
	BOOL bAutoZTargetdmy;
	double fSiOffsetDmy;
	double fPatOffsetDmy;
	double fDieSizeDmy;
	double fPatternThkDmy;
	double fPolyThkDmy;
	double fTapeIndex2Dmy;
	double fSiThkDmy;
	double fSiRangeDmy;
	double fTapeThkDmy;
	double fTapeRangeDmy;
	double fTape2ThkDmy;
	double fTape2RangeDmy;
	double fFilmThkDmy;
	double fFilmRangeDmy;
	BOOL bSaveVideo;
	BOOL bAllOrNothing;
	//search around
	int nTrialPat;		// Number of trial for pattern matching [5/26/2014 FSMT]
	int	nTrialEcho;		// Number of trial to search around [4/6/2010 Valued Customer]
	int nTrialTrim;		// Number of trial to search around during trim height measurement [4/14/2013 Administrator]
	float fStepSizePat;	// Number of steps to search for pattern matching [5/26/2014 FSMT]
	float fStepSizeEcho;// Number of steps to search
	float fStepSizeTrim/*fGlobalStressdmy*/; // Number of steps to search during trim height measurement [4/14/2013 Administrator]
	//pattern recognition
	float TrimHeightDistanceInner1;
	float TrimHeightDistanceInner2;
	float TrimHeightDistanceOuter1;
	//float STIOffsetX[MAXMEPASET];		// Double up as Y offset in TFILMPRB [9/6/2012 Administrator]
	float POffsetX[MAXMEPASET][2];		// ODOffset = P0, Double up as X offset in TFILMPRB [9/6/2012 Administrator]
	//float STIOffsetY[MAXMEPASET];		// Double up as Y offset in TFILMPRB [9/6/2012 Administrator]
	float POffsetY[MAXMEPASET][2];		// Double up as X offset in TFILMPRB [9/6/2012 Administrator]
	double fZref1Dmy;
	double fZref2Dmy;
	BOOL bUseMotorFile;
	BOOL bFastPMatch;
	float TrimWidthOuterRadius;
	float TFZOffset;
	float TrimHeightDistanceOuter2;
	float TrimHeightZOffset;
	int ImgAverdmy;
	int EchoNormalExposure;
	int TFNormalExposure;
	int TFTakeSpecExposure;
	enum PATMAT  // Used by PatMat [6/1/2011 XPMUser]
	{
		EXPB4FOCUS = 0x01, FOCB4MATCH = 0x02, EXPAFOCUS = 0x04, FOCAMATCH = 0x08, EXPAMATCH = 0x10, FOCUS3 = 0x20, EXPOSE4 = 0x40, MATCHATOFFSET = 0x80
	};
	int PatMatDmy;
	int MatchResolutiondmy;
	BOOL bMeasureOuterdge/*bSiOnTopDmy*/;
	int WaferTimeOut;	// Unit: Seconds [11/11/2011 Administrator]
	int FailCountTimeOut;
	int PointTimeOut;
	float AlgnXPos;
	float AlgnYPos;
	float AlgnZPos;
	float BumpOffsetX;
	float BumpOffsetY;

	//extra Echo peak parameters
	int nPeakWidth1dmy;
	int nPeakWidth2dmy;
	double fThresholdFactor1dmy;
	float TrimWidthInnerRadius/*fThresholdFactor2dmy*/;
	short AutoExposeInner, AutoExposeOuter;
	//
	BOOL  bStrict1dmy, bStrict2dmy;
	BOOL  bSkipAlignerdmy;
	BOOL  bUseWaferAlign/*bStitchingdmy*/;
	float GOFdmy;		// Pattern matching quality indicator [2/10/2012 Administrator]
	float dmyCXOffset; // X Center offset [11/4/2011 C2C]
	float dmyCYOffset;	// Y Center offset [11/4/2011 C2C]

	// New version
	void* m_Imagedmy;
	UINT m_ImgSizedmy;

	BOOL bLocateEdgeStep;
	BOOL bLocateEdgeWidth;
	short TrimWidthLines;
	short TrimWidthSpacing;
	float TWExposureInner;	// Use in first trim and trim width inner [4/15/2013 Administrator]
	float TWExposureOuter;
	float TWExposureBevel;
	float THExposure;

	short AutoExposeEcho, AutoExposeTF, TrimSmooth, NSmooth;

	CMeParam MeParam[MAXMEPASET];

	CString MotorFile;
	int SamplingFreq;
	int NumberOfInterestedData;

	float fTgtGRnX[MAXMATCHPATT + 1][MAXTGTSET];	// Value ranges from 0 to 1 [9/13/2011 Administrator]
	float fTgtGRnY[MAXMATCHPATT + 1][MAXTGTSET];	// Value ranges from 0 to 1 [9/13/2011 Administrator]
	float fTGtRedXdmy;					// Moved to C413Global [9/13/2011 Administrator]
	float fTGtRedYdmy;					// Moved to C413Global [9/13/2011 Administrator]
	float fTgtBLuX[MAXMATCHPATT + 1][MAXTGTSET];	// Value ranges from 0 to 1 [9/13/2011 Administrator]
	float fTgtBLuY[MAXMATCHPATT + 1][MAXTGTSET];	// Value ranges from 0 to 1 [9/13/2011 Administrator]

	BOOL bEnableAutoZ;
	float fAutoZValue;
	float FindTrimThres;	// Find trim edge threshold [7/15/2013 Yuen]
	float FindBevelThres;	// Find bevel edge threshold [06/15/2022 Manjurul]
	float ContrastLevel;	//20210329
	short SamplePerPoint;

	enum BESTFITPLN {
		bfBESTFIT,
		bfZEROMIN,
		bf3POINTS
	};
	BESTFITPLN BFPlane;
	enum BESTFITPLNCALC {
		bfcSURFACE,
		bfcMEDIAN
	};
	BESTFITPLNCALC BFPlaneCalc;

	short RefPt1, RefPt2, RefPt3;
	short FindEdgeType;
	short FindEdgeMethod;  // 0,1,2, ... [7/8/2013 Yuen]
	float FindEdgeWaferSize;
	BOOL bTestFindEdgeResult;

	// Stress implementation [2/24/2012 Administrator]
	short NumLines, PointsPerLine, ScanLength;

	int PatMatOpt[MAXMEPASET];
	short MatchRes[MAXMEPASET];
	short MatchAreaDmy[MAXMEPASET]; // Specifies which cross hair is used on reduce area matching [11/28/2012 Yuen]
	short MatchAlgorithm[MAXMEPASET];	// Specifies which matching algorithm is used, 0=primary, 1=second, 3=alternate, 4=multi-template [11/28/2012 Yuen]
	float SearchDist[MAXMEPASET];
	short SearchDept[MAXMEPASET];
	float GoFScore[MAXMEPASET];
	short SubSamI[MAXMEPASET], SubSamS[MAXMEPASET];
	int FFTFilter[MAXMEPASET];

	short XYPairCount;
	CXYPair XYPair[MAXXYPAIR];
	CXYPair XYPairOffset[MAXXYPAIROFFSET];

	float EstNTrimWidth;
	float settlingTime;						//01312024/yukchiu
	BOOL ThicknessWithScan; // 09232025_Shamim

public:
	BOOL IsMarkers();
	BOOL ChangePaSet(short n);
	BOOL IsAutoFocus();
	BOOL IsAutoFocus(CMPoint* pMP);
	BOOL IsProbe(MParam::PRBNUM Prb);
	BOOL IsStressMeasurement(CMPoint* p);
	//[ 03202024 ----> Mahedi Kamal {for adding rough probe}
	BOOL IsRoughnessMeasurement(CMPoint* p);
	BOOL IsRoughnessMeasurement();
	//]
	//[ 08132024 ----> Mahedi Kamal {for adding Ring Width probe}
	BOOL IsRingWidthMeasurement(CMPoint* p);
	BOOL IsRingWidthMeasurement();
	//]
	BOOL IsProfileMeasurement(CMPoint* p);
	BOOL IsEchoBowWarpMeasurement(CMPoint* p);
	BOOL IsEchoMeasurement(CMPoint* p);
	BOOL IsMultiPointMeasurement(CMPoint* p);
	BOOL IsEdgeMeasurement(CMPoint* p);
	BOOL IsHeightMeasurement(CMPoint* p);
	BOOL IsBevelMeasurement(CMPoint* p);
	short IsEchoMarker(CMPoint* p);
	short IsThinFilmMarker(CMPoint* p);
	BOOL IsThinFilmMeasurement(CMPoint* p);
	BOOL IsTotalThicknessMeasurement(CMPoint* p);
	BOOL IsValidMeSet(CMPoint* p);
	BOOL IsBevelMeasurement();
	BOOL IsStressMeasurement();
	BOOL IsMultiPointMeasurement();
	BOOL IsEdgeMeasurement();
	BOOL IsHeightMeasurement();
	BOOL IsThinFilmMeasurement();
	BOOL IsEchoMeasurement();
	BOOL IsTotalThickness();
	BOOL IsProfileMeasurement();
	BOOL IsEchoBowWarpMeasurement();
	CString GetReportPointStr(int cnt, CMPoint* p);
	BOOL ExportData(CFile& fp, BOOL bIncBaseline);
	void RemoveBaseLineSelf();
	void BaselineCorrectSelf();
	void ClearBaseLine();
	BOOL IsStress(short j);
	void AddBaseline();
	void LoadBaseline();
	void BaselineCorrectionA(CRecipe413* pBsl, short idx);
	BOOL IsPatternMatching();
	void VerboseOut(FILE* fp);
	void Verbose(CString fname);
	void Renumber();
	BOOL SaveRecipe(CString filename, BOOL bSort);
	BOOL SaveBaseline();
	void UpdateControl(CListCtrl& Ctrl);
	BOOL LoadDefDir(CString defDir);
	BOOL Load(CString RcpName);
	long Binary(char** msg);
	void Conv(char** msg);
	long GetLength();

	short GetBinarySize();
	short RecipeToBinary(BYTE* RpcBody, UINT size);
	short BinaryToRecipe(BYTE* RcpBody, UINT size);

	void MakeBaseline();
	void MakeRelative();
	// 	BOOL IsProbeToSurface(short j);
	// 	BOOL IsThinFilm(short j);
	// 	BOOL IsRoughness(short j);
	BOOL IsEcho(short j);
	// 	BOOL IsTotalThickness(short j);
	void LoadDesc(short j);

	LPCTSTR GetMaskFile();
	BOOL SetMaskFile(LPCTSTR lpszMaskFileName);

	// 	CMPoint * FindMP(CCoor &point);
	//  CMPoint * FindNearestMP(CCoor &point, float &Distance);

	CWaferParam* GetWp();
	CCustPatt* GetCustPatt();

	void GenCross();
	BOOL SaveWindowImage(CWnd* pWnd);
	void ClearData(void);
	int NumPts();
	BOOL ClearReadings(void);
	BOOL EditLocMP(CCoor& Point, float XCord, float YCord);
	BOOL DelMP(CCoor& Point);
	CBatchDoc* GetBatchDoc();
	void SetCcrData(CPtrList& rings);
	void SetRdlData(CPtrList& lines);
	void FakeData();
	BOOL GetMPList(CPtrList& MPList);

	void SetRecipeName(char* name);
	char* GetRecipeName();

	CMPoint* GetActiveMP(short index);
	short GetActiveMPCount();
	void GetBatchInfo(CString& PrjName, CString& CID, CString& LID, CString& WID, CString& Op, CString& Nt);

	void SetPattType(short type);
	void SetName(LPCTSTR lpszFileName);

	void Clear();
	BOOL LoadRecipe();
	BOOL SaveRecipe(BOOL bSortPrompt = TRUE);
	BOOL LoadRecipeByPath();

	// [04222022 ZHIMING
	void ElliminateOutOfChuck();
	// ]

	CRecipe413& operator=(CRecipe413& co);
	void Serialize(CArchive& ar);

	// [6/14/2022 ZHIMING]
	BOOL IsBevelUsingTF();
	// [11222022 ZHIMING]
	BOOL IsConfocalWarpMeasurement();
	BOOL IsConfocalWarpMeasurement(CMPoint* p);
	//[ Tushar //20231227
	BOOL IsConfocalThicknessMeasurement();
	BOOL IsConfocalThicknessMeasurement(CMPoint* p);
	// [20231227 ZHIMING
	BOOL IsConfocalMeasurement(CMPoint* p);
	BOOL IsConfocalMeasurement();
	BOOL IsConfocalRoughnessMeasurement(CMPoint* p);
	BOOL IsConfocalRoughnessMeasurement();
	BOOL IsConfocalLocalMeasurement(CMPoint* p); //24102024/MORSALIN
	BOOL IsConfocalLocalMeasurement(); //24102024/MORSALIN
	BOOL IsConfocalDualWarpMeasurement(CMPoint* p);
	BOOL IsConfocalDualWarpMeasurement();
	//[ 04182024 Tushar
	float GetRadius();
	float CalculateWaferSize();
	BOOL IsEchoThicknessMeasurement();
	// [042252024 ZHIMING
	double fEchoTopZ, fThinFilmZ, fThinFilmZCaldmy, fCWLTopZ, fCWLTopZ2, fEchoBottomZ, fRoughZ; // fCWLTopZ2->cwl008, fCWLTopZ->cwl030,040
	//]

	bool isRangeType;
	BOOL m_bIncludeOn, m_LSFOn; //Morsalin
	BOOL r_LTVMin, r_LTVMax, r_LTVAvg, r_LTV, r_STIR;  // 10312025_1 MORSALIN
	float DieHeight, DieWidth, AnchorX = 0.0, AnchorY = 0.0; //Morsalin

	BOOL ExportMeasSumLog(CString fileName, short SlotID, CString LotID, CString opName, CString WaferID, CString WaferType = "WaferType",
		CString WaferSize = "WaferSize", CString Station = "Station", CTime startTime = CTime(2025, 1, 1, 0, 0, 0), CTime endTime = CTime(2025, 1, 1, 0, 0, 0));  //20251022_Mohir
 
 	BOOL ExportDataWithHeader(CFile& fp, BOOL bIncBaseline, CString recipe, CString lotId, CString waferID,
		CString station = "Station", CString waferType = "WaferType", CString waferSize = "WaferSize", CString opName = "FSM"); // 20251014_Mohir
	CTime StartT, EndT; //20251030_Mohir

	BOOL  IsFileLocked(const CString& filename);//20251219_Mohir
	BOOL  RemoveFileFromTask(const CString& filename); //20251219_Mohir

	CRecipe413();
	virtual ~CRecipe413();
};

extern string PPExecName;
extern string PPChangeName;
extern BYTE PPChangeStatus;

BOOL WriteDIB(LPCTSTR szFile, HANDLE hDIB);
BOOL WriteWindowToDIB(LPCTSTR szFile, CWnd* pWnd);
HANDLE DDBToDIB(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal);
