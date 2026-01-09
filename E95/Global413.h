// Global.h: interface for the C413Global class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "Recipe413.h"
#include "413Confg.h"
#include "LoadPos.h"
#include "..\SRC\DOSUtil.h"
#include "LogE.h"
#include "SICam.h"
#include "FPoint.h"
#include "DiceLoc.h"
#include "ProcJobInfo.h"
#include <unordered_map>
#include <vector>

#define BMPSIZEX	512
#define BMPSIZEY	384

class CScrCalib {
public:
	float dX, dY;		// Screen to motor distance, unit: mm of motor step per pixel [6/2/2011 XPMUser]
	float ppmX, ppmY;	// Screen to micron, pixel per micron [6/2/2011 XPMUser]
	short ScrnX, ScrnY;	// Size of the field of view at calibration [1/15/2013 Administrator]

	CScrCalib();
	void Clear();
	void SaveParam(CString name);
	void LoadParam(CString name);
};

struct sABC {
	float A, B, C;

	sABC() {
		A = B = C = 0.0f;
	}
};

class CChuckMask;

class C413Global {
public:
	short CamraNum[2];
	CCamBase::CAMERAID ActiveCamera;

	C413Confg Confg;
	CRecipe413 RcpSetup;	// Recipe use for Setup purposes only [7/8/2010 128 NT]

	short X_Axis, Y_Axis;
	short EchoTop_Axis, ThinFilm_Axis;
	short P_Axis;
	BOOL bStop, bAborting;  // Use to stop entire measurement process [7/15/2010 C2C]
	BOOL bSPCalib;
	BOOL bStageReset, bResetX;

	short mode;  // user or engineering mode [8/5/2010 XPMUser]
	short XPort, YPort, Z1Port, Z2Port, PPort;

	CChuckMask* m_pChuckMask;

	// [ Morsalin for LTV wafer scan
	struct Point
	{
		double x, y;
	};

	struct Die
	{
		Point DL, UR;
	};

	Point P;
	Die DieData[5000];

	//vector<double> scanData[700];
	vector<vector<double> > scanData, storeData;
	//vector<double> storeData[700];
	vector<pair<double, double> > LineCoordinate;

	//double radius, scanOffset, scanSpeed;
	double scanOffset, scanSpeed;
	bool stopScan;
	double scanStep;
	int noOfDie;
	bool isLoad = false; // 01012025 MORSALIN open DieMap on load
	bool isNew = false; // 01012025 MORSALIN open DieMap on load
	vector<double> finalLTV, storefinalLTV, finalSTIR, storefinalSTIR, finalLTVMax, finalLTVMin, finalLTVAvg; // 102220252
	vector<vector<double> > previousLTV;
	vector<int> notMeasured; // 10312025 MORSALIN
	map<int, int> LessDataDie;
	// ]

	CFPoint LocXY;

	//float LocX;		// Internal usage only [7/20/2011 XPMUser]
	//float LocY;		// Internal usage only [7/20/2011 XPMUser]
	float LocEchoTopZ;	// Internal usage only [7/20/2011 XPMUser]
	float LocThinFilmZ;	// Internal usage only [7/20/2011 XPMUser]
	float LocP;

	CLoadPos LoadPos;	// Load position [8/2/2013 Yuen]
	enum LOADPOSCONFTYPE {
		USEECHOPROBE, USEPATTMATCHING, USENONE
	};
	LOADPOSCONFTYPE LoadPosConfType;	// Load position confirmation method [6/7/2014 FSMT]
	CFPoint DPPos/*X, DPPosY*/;	// Dual probe calibration location [8/2/2013 Yuen]
	CFPoint BGPos/*X, BGPosY*/;	// TF measurement background location [8/2/2013 Yuen]
	CFPoint TFPos/*X, TFPosY*/;	// TF calibration position;
	float FFUPressure;

	int ImageCount;
	CString ImageTitle;

	int MWdw1, MWdw2;	// Pattern sizes, temporary variables [7/8/2010 128 NT]

	// Working copy saved by SnapOneFrame only [3/21/2012 Yuen]
	CPackedDIB StoredDIB;	// First copy of BMP saved using bStore [7/8/2010 C2C]
	CPackedDIB StoredDIB2;  // Second copy of BMP saved using bStore [7/8/2010 C2C]

	float BevelWidth;	// Temporary variable [5/31/2012 Yuen]
	int AlgnExposure;	// Exposure used during wafer centering [6/10/2013 Yuen]
	float fAlignZEcho;
	float FindEdgeThres;// Threshold used in FindWaferEdge [7/15/2013 Yuen]

	float ThnkTF;  // Thin film thickness [7/8/2010 128 NT]
	int Histo[256];

	float DPZPos, DPZOff;// Unit: micron [3/28/2013 Yuen]
	float DPDistance;
	float TFScalingFactor;

	float CalZPosEcho;
	float CalZPosTF;

	int Exposure[CCamBase::LASTCAM];

	CScrCalib ScrnCalibTF, ScrnCalibECH;

	float DPGageThickness;
	float SPGageThickness;

	short WaferRepeat, CassetteRepeat;

	short CurMeSet;
	short CurPaSet; // always start from 1
	BOOL bTakePictureOnMeasure;

	//BOOL bRevX, bRevY;
	BOOL bImageFlipYECHO;		// Specify whether the Echo video image should be flip in Y direction [9/12/2011 Administrator]
	BOOL bImageFlipYTFILM;		// Specify whether the TF video image should be flip in Y direction [9/12/2011 Administrator]
	//BOOL bImageFlipXTFILM; //Added 12182024
	//BOOL bImageFlipXECHO; //Added 12182024

	//float XOff, YOff, Z2Off;	// Intermediate variable [12/3/2011 XPMUser]
	CDiceLoc DLoc;				// Intermediate variable [7/23/2013 Yuen]
	BOOL bHasMarker;

	short LPNo[6];	// Support 4 load port [9/20/2012 Administrator]
	short WaferOnStage;	// Value indicates the load port number where the wafer came from [10/10/2012 Yuen]

	BOOL bCanClose;

	// temporary globals, maybe removed anytime [11/24/2012 Yuen]
	BOOL bCycleFirst;
	BOOL bShowSpec;

	CFPoint COffTF;	// X Center offset, for tool to tool matching [11/4/2011 C2C]
	//float CYOffsetTF;	// Y Center offset [11/4/2011 C2C]
	CFPoint COffEcho; // Center offset, for trim measurement [11/4/2011 C2C]
	//float CYOffsetEcho;	// Y Center offset [11/4/2011 C2C]

	//BOOL bConfirmStage;
	DWORD ResetLapTime;

	BOOL bSpectLight, bEchoLight, bChuckVac, bSpecSource, bSpecPower, bSetFFU = FALSE; // 08142024;

	float WaferSize;

	CFPoint fTgtRedTF;
	CFPoint fTgtRedECHO;

	short AutoExposeStepTF;
	short AutoExposeStepEcho;
	float AutoFocusStepEcho;
	float AutoFocusStepTF;

	// 	short FindEdgeMethod;  // 0,1,2, ... [7/8/2013 Yuen]

	//	float fGPatOrgX, fGPatOrgY; //20130827 SCOTT ADD TO REMEMBER the original point of Point 1.
	// 	char  chSPCalRcpName[255]; //20131009

	float ScaleEcho, ScaleTF;
	//	int ImgWd, ImgHt;	// temporary variable, refering to video image size [5/27/2014 FSMT]

	float fLPosBLuX, fLPosBLuY;
	float fLPosGRnX, fLPosGRnY;
	float fLPosX, fLPosY, fLPosZ;
	short fLExposure;
	short BaseExpoEcho, BaseExpoTF;

	short PSLevelOff[2];	// Peak level global offset [6/10/2014 Yuen]
	short PSWidthOff[2];	// Peak width global offset [6/10/2014 Yuen]

	float ResetFreq;
	short DMCProgram;

	float GainEchoCam, GainThinFCam;
	short OffsetEchoCam, OffsetThinFCam;

	CPackedDIB MatchDIBA;		// Pattern template images [6/24/2010 Yuen]
	CPackedDIB MatchDIBB;		// Pattern template images [6/24/2010 Yuen]

	// [8/3/2020]
	// Measurement
	BOOL bEnableIntensity;
	BOOL bEnablePosition;
	BOOL bEnableMaths;
	BOOL bEnableBow;
	BOOL bEnableWarp;
	BOOL bDoorInterlock;
	int nDoorInterlockInterval;
	// [20231226 ZHIMING
	BOOL bDualConfocal;
	float fConfocalConstantK; // same usage compare with echo calibration gauge thickness
	// [03152024
	BOOL bCheckWaferPresence;
	// [20240426
	short EchoBottomZ_Axis;
	float LocEchoBottomZ;
	// [05072024 ZHIMING
	CFPoint COffCWL;
	CFPoint COffRough;
	CFPoint fTgtRedCWL;
	CFPoint fTgtRedRough;
	float LocCWLTopZ, LocRoughZ;
	short RoughZ_Axis;
	short TopCWLZ_Axis; // 06112024/MORSALIN
	// [05132024
	CFPoint RoughCalibPos;	// roughness calibration position
	float CalZPosRough;
	float CalZPosCWLTop;
	// [06182024 ZHIMING
	BOOL bImageFlipXECHO;
	BOOL bImageFlipXTFILM;
	// ]
	// [06262024
	BOOL umUnitInRoughness;
	//08142024 - START
	short TaikoPointCount = 0;
	//08142024 - END
	// ==========
	int SlotsPerFOUP = 25; //Arup
	void LoadMaxSlot(); //20240820/Arup

	// [ 02082024/MORSALIN
	void LoadProbeSwitchingOptions();
	void SaveProbeSwitchingOptions();
	BOOL isPSwitchingSimu;
	int comPort, transRate, stopBit, MicroscopePrbPos, CWL008prbPos, CWL030prbPos, RoughnessPrbPos, ProbeSwitchCurPos = 0, RMovingStep = 1, //10172024 Morsalin
		PosSoftLimit, NegSoftLimit, Acceleration = 1500, Deceleration = 1500, Speed = 2000;
	char parity;
	float xStageOffset, yStageOffset;
	// ]
	// [ //24102024/MORSALIN
	void LoadLTV_STIR_Options();
	void SaveLTV_STIR_Options();
	int DespikeWindowSize, GaussianKernelSize;
	double GaussianSigma, DieHeight, DieWidth, LinePerDie;
	bool isContinue, forWaferScan = false;
	BOOL m_bIncludeOn, m_LSFOn, isSqDie, ScanPolarity;
	float AnchorX, AnchorY;
	// ]
	BOOL r_LTVMin, r_LTVMax, r_LTVAvg, r_LTV, r_STIR;


	BOOL isRecoverNeeded; //Tushar//09092024
	int thkTrialNum, warpTrialNum; //03052025

public:
	BOOL Load();
	BOOL Save();
	void SaveLoadPortOptions();
	void LoadLoadPortOptions();
	void SaveModule();
	void LoadModule();
	void LoadScrnCalibINI();
	void SaveScrnCalibINI();
	BOOL LoadChuckMask();
	void Polar2Cartesian(float radius, float ang, float& x, float& y);
	void Cartesian2Polar(float X, float Y, float& radius, float& ang);
	void SaveVideoExposure();
	void LoadVideoExposure(CString inifile);
	void SaveThinFilmOption();
	void LoadThinFilmOption();
	void SaveExposure(short n, int Exp);
	void SaveMeasurementOption();
	void LoadMeasurementOption();
	void SaveEchoOption();
	void LoadEchoOption();
	void SaveVideoOption();
	void LoadVideoOption();
	void SaveMotorOption();
	void LoadMotorOption();
	void SaveINI();
	void LoadINI();
	void SaveCalFactor();
	void LoadCalFactor();
	void Sleep2(int nTime);
	BOOL IsEngineerMode();
	CChuckMask* GetChuckMask() { return m_pChuckMask; }

	// [05142024 ZHIMING
	void LoadRoughOption();
	void SaveRoughOption();
	// ]

	BOOL CheckRecovery();//09092024
	void SetRecoveryStatus(int stat);//09092024
	// [10122024
	bool setDiceDisplay;
	vector<vector<string>> stt;
	float ltvHighLimit; float ltvLowLimit;
	float stirHighLimit; float stirLowLimit;
	//]
	float pinHeight;
	C413Global();
	virtual ~C413Global();
	void Serialize(CArchive& ar);
	float testData = 0.00;
	int cnt = 0;
	bool isStart;
	map<int, vector<float>> rectangleMap;//13012025

	bool isCombinedCWLMes;
	float combinedBotData;
	float combinedTopData;
	BOOL saveDataWhileThkLineScan; // 09242025

	bool isDuringMes;

	CString waferLotID;
	CString diceMapSavePath;
	CString diceRcpName;
	short diceSlotID;
	CString diceWaferID;
	CString diceLotID;
	short diceJobID=0;
	CNCas diceCas;
	CProcJobInfo* pDiceJobInfo;

	// [12112025_Tushar
	bool isFirstCall;
	bool isAutoSave;
	bool isManual;
	bool hasColChanged;
	std::vector<std::vector<double>> GeneralInterpolatedGrid;
	bool callFrom2DAutoSave;
	bool callFrom3DAutoSave;
	bool isCallFromViewContentDlg413;
	bool callFromBtn;
	unordered_map<int, vector<vector<double>>> interpolationData_map;
	int currentDataCol;
	//]

	bool isFromT;
	bool is3Ddone;
	bool is3DClosed;
};