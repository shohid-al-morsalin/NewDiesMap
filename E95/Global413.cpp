// Global.cpp: implementation of the C413Global class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global413.h"
#include "..\SRC\DOSUtil.h"
#include "ChuckMask.h"

#include <math.h>

#define PIE180  0.0174532925199432957692369076849
#define VERSIONGLOBAL	7

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//[09092024
BOOL C413Global::CheckRecovery()
{

	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Measurement";

	GetPrivateProfileString(sec, "RecoveryNeed", "1", str, 255, inifile);
	isRecoverNeeded = atof(str);
	if (isRecoverNeeded == 1) {
		return TRUE;
	}
	
	return FALSE;
	
}

void C413Global::SetRecoveryStatus(int stat)
{

	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString str;
	CString sec = "Measurement";

	str.Format("%d", stat);
	WritePrivateProfileString(sec, "RecoveryNeed", str, inifile);
}
//]

C413Global::C413Global() {
	mode = 0;  // Engineering or User mode [6/24/2010 Yuen]

	ActiveCamera = CCamBase::SIMULCAM;

	//LocX = 0;
	//LocY = 0;
	LocEchoTopZ = 0;
	LocThinFilmZ = 0;
	LocP = 0;

	X_Axis = 1;
	Y_Axis = 2;
	EchoTop_Axis = 5;
	ThinFilm_Axis = 3;
	P_Axis = 4;

	//LoadPosX = LoadPosY = 0;
	//DPPosX = DPPosY = 0;
	//TFPosX = TFPosY = 0;
	//BGPosX = BGPosY = 0;

	ImageCount = 1;
	ImageTitle = "";

	MWdw1 = 109;
	MWdw2 = 109;

	ThnkTF = 0;
	// 	Index=0;

	m_pChuckMask = NULL;

	XPort = YPort = Z1Port = Z2Port = PPort = 0;

	Exposure[0] = 800;
	Exposure[1] = 800;

	DPZPos = 0;
	DPZOff = 0;
	DPDistance = 0;
	TFScalingFactor = 1;

	bResetX = FALSE;
	bStageReset = FALSE;

	DPGageThickness = 500.0f;
	SPGageThickness = 500.0f;

	WaferRepeat = 0;
	CassetteRepeat = 0;

	CurMeSet = 1; // Start from 1 [11/3/2012 Yuen]
	CurPaSet = 1; // Start from 1 [11/3/2012 Yuen]

	bTakePictureOnMeasure = FALSE;

	CalZPosEcho = 3.4f;
	CalZPosTF = 4.05f;

	//	bRevX = bRevY = FALSE;

	//	XOff = YOff = Z2Off = 0;

	bImageFlipYECHO = 0;
	bImageFlipYTFILM = 0;

	bStop = FALSE;  // Stop global measurement [7/15/2010 C2C]
	bAborting = FALSE;
	bSPCalib = FALSE;

	FFUPressure = 0;
	WaferOnStage = 0;

	bCanClose = FALSE;

	bCycleFirst = TRUE;
	bShowSpec = TRUE;

	// 	CXOffsetTF = CYOffsetTF = 0;
	// 	CXOffsetEcho = CYOffsetEcho = 0;

	BevelWidth = 0;
	AlgnExposure = 600;
	fAlignZEcho = 0;
	FindEdgeThres = 0.01f;

	//bConfirmStage=FALSE;
	ResetLapTime = 0;

	bSpectLight = FALSE;
	bEchoLight = FALSE;
	bChuckVac = FALSE;
	bSpecSource = FALSE;
	bSpecPower = FALSE;
	bSetFFU = FALSE; // 08142024

	WaferSize = 150.0f;

	fTgtRedTF.X = fTgtRedTF.Y = 0.5;
	fTgtRedECHO.X = fTgtRedECHO.Y = 0.5;

	memset(Histo, 0, 256 * sizeof(int));

	ScaleEcho = 0.1f;
	ScaleTF = 0.05f;

	//	ImgWd = ImgHt = 0;

	fLPosBLuX = fLPosBLuY = 0.5f;
	fLPosGRnX = fLPosGRnY = 0.5f;
	fLPosX = fLPosY = fLPosZ = 0;
	fLExposure = 0;
	BaseExpoEcho = BaseExpoTF = 0;

	PSWidthOff[0] = PSWidthOff[1] = 0;
	PSLevelOff[0] = PSLevelOff[1] = 0;

	ResetFreq = 36.0f;
	DMCProgram = 0;

	GainEchoCam = GainThinFCam = 1.0f;
	OffsetEchoCam = OffsetThinFCam = 0;

	// [8/3/2020]
	bEnableIntensity = FALSE;
	bEnablePosition = FALSE;
	bEnableMaths = FALSE;
	// [11302022 ZHIMING
	bEnableBow = FALSE;
	bEnableWarp = FALSE;
	bDoorInterlock = FALSE;
	nDoorInterlockInterval = 5000;
	// [01172024
	bDualConfocal = 0;
	fConfocalConstantK = DosUtil.GetConfocalK();
	// [03152024
	bCheckWaferPresence = FALSE;
	// [20240426 tushar
	LocRoughZ = 0;
	LocEchoBottomZ = 0;
	LocCWLTopZ = 0;
	EchoBottomZ_Axis = 0;
	// [05092024 ZHIMING
	RoughZ_Axis = 0;
	CalZPosRough = 10;
	CalZPosCWLTop = 10;
	// [06182024 ZHIMING
	bImageFlipXECHO = 0;
	bImageFlipXTFILM = 0;
	// [06262024
	umUnitInRoughness = FALSE;
	// ==========

	//[10122024
	setDiceDisplay = false;
	ltvHighLimit  = 0.00; ltvLowLimit =0.00;
	stirHighLimit =0.00; stirLowLimit=0.00;
	//ifWithVal = false;
	//]

	pinHeight = 0.0;////18122024
	isStart = false;

	//// 04092025
	isCombinedCWLMes = false; 
	combinedBotData = 0.0;
	combinedTopData = 0.0;

	saveDataWhileThkLineScan = TRUE; // 09242025

	waferLotID = "";
	diceMapSavePath = "";
	diceRcpName="";
	diceSlotID=0;
	diceWaferID="";
	diceLotID="";

	isDuringMes = false;

	isFirstCall = false;
	isAutoSave = false;
	isManual = false;
	hasColChanged = false;
	callFrom2DAutoSave = false;
	callFrom3DAutoSave = false;
	isCallFromViewContentDlg413 = false;
	callFromBtn = false;
	currentDataCol = 0;
	isFromT = false;
	is3DClosed = true;

	LoadINI();
	SaveINI();

	LoadChuckMask();

	Load();
}

C413Global::~C413Global() {
	Save();

	if (m_pChuckMask) {
		delete m_pChuckMask;
	}
}

void C413Global::Serialize(CArchive& ar) {
	short ver = VERSIONGLOBAL;
	if (ar.IsStoring()) {
		ar << ver;
		ar << fLPosGRnX;
		ar << fLPosGRnY;
		ar << fLPosBLuX;
		ar << fLPosBLuY;
		MatchDIBA.Serialize(ar);
		MatchDIBB.Serialize(ar);
		ar << fLPosX;
		ar << fLPosY;
		ar << fLPosZ;
		ar << PSWidthOff[0];
		ar << PSWidthOff[1];
		ar << fLExposure;
		ar << MWdw1;
		ar << MWdw2;
		ar << BaseExpoEcho;
		ar << BaseExpoTF;
		ar << PSLevelOff[0];
		ar << PSLevelOff[1];
		ar << GainEchoCam;
		ar << GainThinFCam;
		ar << OffsetEchoCam;
		ar << OffsetThinFCam;
	}
	else {
		ar >> ver;
		ar >> fLPosGRnX;
		ar >> fLPosGRnY;
		ar >> fLPosBLuX;
		ar >> fLPosBLuY;
		MatchDIBA.Serialize(ar);
		MatchDIBB.Serialize(ar);
		ar >> fLPosX;
		ar >> fLPosY;
		ar >> fLPosZ;
		if (ver > 1) {
			ar >> PSWidthOff[0];
			ar >> PSWidthOff[1];
		}
		if (ver > 2) {
			ar >> fLExposure;
		}
		if (ver > 3) {
			ar >> MWdw1;
			ar >> MWdw2;
		}
		if (ver > 4) {
			ar >> BaseExpoEcho;
			ar >> BaseExpoTF;
		}
		if (ver > 5) {
			ar >> PSLevelOff[0];
			ar >> PSLevelOff[1];
		}
		if (ver > 6) {
			ar >> GainEchoCam;
			ar >> GainThinFCam;
			ar >> OffsetEchoCam;
			ar >> OffsetThinFCam;
		}
	}
}

BOOL C413Global::IsEngineerMode() {
	return (mode == 0);
}

void C413Global::Sleep2(int nTime) {}

void C413Global::LoadCalFactor() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Calibration";

	// 	GetPrivateProfileString(sec, "EchoCalibrationFactor",  "1.0f", str, 255, inifile);
	// 	calFactor1 = atof(str);

	// 	GetPrivateProfileString(sec, "EchoCalibrationFactor2",  "1.0f", str, 255, inifile);
	// 	calFactor2 = atof(str);

	GetPrivateProfileString(sec, "DPCalibrationFactor", "1.0f", str, 255, inifile);
	DPDistance = atof(str);

	GetPrivateProfileString(sec, "DPCalibrationZPos", "1.0f", str, 255, inifile);
	DPZPos = atof(str);

	GetPrivateProfileString(sec, "SPCalibrationOnConfirmPos", "0", str, 255, inifile);
	bSPCalib = atoi(str);

	// 	//20131009 SCOTT ADD -----------------------------------------------------------------------
	// 	GetPrivateProfileString(sec, "SPCalRcpName", "DONOTCAL", str, 255, inifile);
	// 	strcpy(chSPCalRcpName, str);
	// 	//------------------------------------------------------------------------------------------
}

void C413Global::SaveCalFactor() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Calibration";

	// 	sprintf(str,"%.6f", calFactor1);
	// 	WritePrivateProfileString(sec, "EchoCalibrationFactor", str, inifile);
	//
	// 	sprintf(str,"%.6f", calFactor2);
	// 	WritePrivateProfileString(sec, "EchoCalibrationFactor2", str, inifile);

	sprintf(str, "%.6f", DPDistance);
	WritePrivateProfileString(sec, "DPCalibrationFactor", str, inifile);

	sprintf(str, "%.6f", DPZPos);
	WritePrivateProfileString(sec, "DPCalibrationZPos", str, inifile);

	sprintf(str, "%d", bSPCalib);
	WritePrivateProfileString(sec, "SPCalibrationOnConfirmPos", str, inifile);
}

void C413Global::LoadINI() {
	LoadModule();
	LoadCalFactor();
	LoadMotorOption();
	LoadVideoOption();
	LoadEchoOption();
	LoadThinFilmOption();
	LoadMeasurementOption();
	LoadScrnCalibINI();
	LoadLoadPortOptions();
	// [05142024 ZHIMING
	LoadRoughOption();
	// ]
	LoadMaxSlot(); //20240820/Arup
	LoadProbeSwitchingOptions(); // 02082024 / MORSALIN
	LoadLTV_STIR_Options();
}

void C413Global::SaveINI() {
	SaveModule();
	SaveCalFactor();
	SaveMotorOption();
	SaveVideoOption();
	SaveEchoOption();
	SaveThinFilmOption();
	SaveMeasurementOption();
	SaveScrnCalibINI();
	SaveLoadPortOptions();
	// [05142024 ZHIMING
	SaveRoughOption();
	// ]
	//SaveProbeSwitchingOptions(); // 02082024 / MORSALIN
}

void C413Global::LoadMotorOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Stage";

	GetPrivateProfileString(sec, "LoadPosX", "1", str, 255, inifile);
	LoadPos.PX = atof(str);

	GetPrivateProfileString(sec, "LoadPosY", "1", str, 255, inifile);
	LoadPos.PY = atof(str);

	GetPrivateProfileString(sec, "LoadVPosX", "1", str, 255, inifile);
	LoadPos.VX = atof(str);

	GetPrivateProfileString(sec, "LoadVPosY", "1", str, 255, inifile);
	LoadPos.VY = atof(str);

	GetPrivateProfileString(sec, "LoadVPosZ", "1", str, 255, inifile);
	LoadPos.VZ = atof(str);

	GetPrivateProfileString(sec, "LoadPosExposure", "1", str, 255, inifile);
	LoadPos.Exposure = atoi(str);

	GetPrivateProfileString(sec, "DPPosX", "1", str, 255, inifile);
	DPPos.X = atof(str);

	GetPrivateProfileString(sec, "DPPosY", "1", str, 255, inifile);
	DPPos.Y = atof(str);

	GetPrivateProfileString(sec, "TFPosX", "1", str, 255, inifile);
	TFPos.X = atof(str);

	GetPrivateProfileString(sec, "TFPosY", "1", str, 255, inifile);
	TFPos.Y = atof(str);

	GetPrivateProfileString(sec, "BGPosX", "1", str, 255, inifile);
	BGPos.X = atof(str);

	GetPrivateProfileString(sec, "BGPosY", "1", str, 255, inifile);
	BGPos.Y = atof(str);

	GetPrivateProfileString(sec, "CXOffsetTF", "0", str, 255, inifile);
	COffTF.X = atof(str);

	GetPrivateProfileString(sec, "CYOffsetTF", "0", str, 255, inifile);
	COffTF.Y = atof(str);

	GetPrivateProfileString(sec, "CXOffsetEcho", "0", str, 255, inifile);
	COffEcho.X = atof(str);

	GetPrivateProfileString(sec, "CYOffsetEcho", "0", str, 255, inifile);
	COffEcho.Y = atof(str);

	// [05072024 ZHIMING
	GetPrivateProfileString(sec, "CXOffsetCWL", "0", str, 255, inifile);
	COffCWL.X = atof(str);

	GetPrivateProfileString(sec, "CYOffsetCWL", "0", str, 255, inifile);
	COffCWL.Y = atof(str);

	GetPrivateProfileString(sec, "CXOffsetRough", "0", str, 255, inifile);
	COffRough.X = atof(str);

	GetPrivateProfileString(sec, "CYOffsetRough", "0", str, 255, inifile);
	COffRough.Y = atof(str);

	// [05242024
	GetPrivateProfileString(sec, "RoughCalibX", "1", str, 255, inifile);
	RoughCalibPos.X = atof(str);

	GetPrivateProfileString(sec, "RoughCalibY", "1", str, 255, inifile);
	RoughCalibPos.Y = atof(str);
	// ]

	sec = "MotorPort";

	GetPrivateProfileString(sec, "XMotor", "1", str, 255, inifile);
	X_Axis = atoi(str);

	GetPrivateProfileString(sec, "YMotor", "2", str, 255, inifile);
	Y_Axis = atoi(str);

	/*GetPrivateProfileString(sec, "EchoTopZMotor", "5", str, 255, inifile);
	EchoTop_Axis = atoi(str);*/
	
	GetPrivateProfileString(sec, "TopMicroscopeZMotor", "5", str, 255, inifile);
	EchoTop_Axis = atoi(str);

	GetPrivateProfileString(sec, "ThinFilmZMotor", "3", str, 255, inifile);
	ThinFilm_Axis = atoi(str);

	GetPrivateProfileString(sec, "PMotor", "4", str, 255, inifile);
	P_Axis = atoi(str);

	//[20240426 Tushar
	/*GetPrivateProfileString(sec, "EchoBottomZMotor", "0", str, 255, inifile);
	EchoBottomZ_Axis = atoi(str);*/
	// [05092024 ZHIMING
	GetPrivateProfileString(sec, "RoughZMotor", "0", str, 255, inifile);
	RoughZ_Axis = atoi(str);
	
	GetPrivateProfileString(sec, "TopCWLZMotor", "0", str, 255, inifile);
	TopCWLZ_Axis = atoi(str);
	//

	// 	sec = "MotionControl";
	//
	// 	GetPrivateProfileString(sec, "XMotor",  "0", str, 255, inifile);
	// 	bRevX = atoi(str);
	//
	// 	GetPrivateProfileString(sec, "YMotor",  "0", str, 255, inifile);
	// 	bRevY = atoi(str);
}

void C413Global::SaveMotorOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Stage";

	sprintf(str, "%.4f", LoadPos.PX);
	WritePrivateProfileString(sec, "LoadPosX", str, inifile);

	sprintf(str, "%.4f", LoadPos.PY);
	WritePrivateProfileString(sec, "LoadPosY", str, inifile);

	sprintf(str, "%.4f", LoadPos.VX);
	WritePrivateProfileString(sec, "LoadVPosX", str, inifile);

	sprintf(str, "%.4f", LoadPos.VY);
	WritePrivateProfileString(sec, "LoadVPosY", str, inifile);

	sprintf(str, "%.4f", LoadPos.VZ);
	WritePrivateProfileString(sec, "LoadPosZ", str, inifile);

	sprintf(str, "%d", LoadPos.Exposure);
	WritePrivateProfileString(sec, "LoadPosExposure", str, inifile);

	sprintf(str, "%.4f", DPPos.X);
	WritePrivateProfileString(sec, "DPPosX", str, inifile);

	sprintf(str, "%.4f", DPPos.Y);
	WritePrivateProfileString(sec, "DPPosY", str, inifile);

	sprintf(str, "%.4f", TFPos.X);
	WritePrivateProfileString(sec, "TFPosX", str, inifile);

	sprintf(str, "%.4f", TFPos.Y);
	WritePrivateProfileString(sec, "TFPosY", str, inifile);

	sprintf(str, "%.4f", BGPos.X);
	WritePrivateProfileString(sec, "BGPosX", str, inifile);

	sprintf(str, "%.4f", BGPos.Y);
	WritePrivateProfileString(sec, "BGPosY", str, inifile);

	sprintf(str, "%.4f", COffTF.X);
	WritePrivateProfileString(sec, "CXOffsetTF", str, inifile);

	sprintf(str, "%.4f", COffTF.Y);
	WritePrivateProfileString(sec, "CYOffsetTF", str, inifile);

	sprintf(str, "%.4f", COffEcho.X);
	WritePrivateProfileString(sec, "CXOffsetEcho", str, inifile);

	sprintf(str, "%.4f", COffEcho.Y);
	WritePrivateProfileString(sec, "CYOffsetEcho", str, inifile);

	// [05072024 ZHIMING
	sprintf(str, "%.4f", COffCWL.X);
	WritePrivateProfileString(sec, "CXOffsetCWL", str, inifile);

	sprintf(str, "%.4f", COffCWL.Y);
	WritePrivateProfileString(sec, "CYOffsetCWL", str, inifile);

	sprintf(str, "%.4f", COffRough.X);
	WritePrivateProfileString(sec, "CXOffsetRough", str, inifile);

	sprintf(str, "%.4f", COffRough.Y);
	WritePrivateProfileString(sec, "CYOffsetRough", str, inifile);

	// [05242024
	sprintf(str, "%.4f", RoughCalibPos.X);
	WritePrivateProfileString(sec, "RoughCalibX", str, inifile);

	sprintf(str, "%.4f", RoughCalibPos.Y);
	WritePrivateProfileString(sec, "RoughCalibY", str, inifile);
	// ]

	sec = "MotorPort";

	sprintf(str, "%d", X_Axis);
	WritePrivateProfileString(sec, "XMotor", str, inifile);

	sprintf(str, "%d", Y_Axis);
	WritePrivateProfileString(sec, "YMotor", str, inifile);

	/*sprintf(str, "%d", EchoTop_Axis);
	WritePrivateProfileString(sec, "EchoTopZMotor", str, inifile);*/
	
	sprintf(str, "%d", EchoTop_Axis);
	WritePrivateProfileString(sec, "TopMicroscopeZMotor", str, inifile);

	sprintf(str, "%d", ThinFilm_Axis);
	WritePrivateProfileString(sec, "ThinFilmZMotor", str, inifile);

	sprintf(str, "%d", P_Axis);
	WritePrivateProfileString(sec, "PMotor", str, inifile);

	//[20240426 Tushar
	/*sprintf(str, "%d", EchoBottomZ_Axis);
	WritePrivateProfileString(sec, "EchoBottomZMotor", str, inifile);*/
	// [05092024 ZHIMING
	sprintf(str, "%d", RoughZ_Axis);
	WritePrivateProfileString(sec, "RoughZMotor", str, inifile);
	
	sprintf(str, "%d", TopCWLZ_Axis);
	WritePrivateProfileString(sec, "TopCWLZ_Axis", str, inifile);
	//]

	// 	sec = "MotionControl";
	//
	// 	sprintf(str,"%d", bRevX);
	// 	WritePrivateProfileString(sec, "XMotor", str, inifile);
	//
	// 	sprintf(str,"%d", bRevY);
	// 	WritePrivateProfileString(sec, "YMotor", str, inifile);
}

void C413Global::LoadVideoOption() {
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);
	LoadVideoExposure(inifile);

	char str[256];
	CString sec = "Video";

	GetPrivateProfileString(sec, "FlipYECHO", "0", str, 255, inifile);
	bImageFlipYECHO = atoi(str);

	GetPrivateProfileString(sec, "FlipYTFILM", "0", str, 255, inifile);
	bImageFlipYTFILM = atoi(str);

	GetPrivateProfileString(sec, "EchoCameraID", "1", str, 255, inifile);
	CamraNum[CCamBase::ECHOCAM] = atoi(str);

	GetPrivateProfileString(sec, "ThinFilmCameraID", "2", str, 255, inifile);
	CamraNum[CCamBase::THINFCAM] = atoi(str);

	GetPrivateProfileString(sec, "AlignExposure", "600", str, 255, inifile);
	AlgnExposure = atoi(str);

	GetPrivateProfileString(sec, "AlignZPos", "0", str, 255, inifile);
	fAlignZEcho = atof(str);

	GetPrivateProfileString(sec, "FindEdgeThreshold", "0.01", str, 255, inifile);
	FindEdgeThres = atof(str);

	AutoExposeStepTF = DosUtil.GetAutoExposeStepTF(5);
	AutoExposeStepEcho = DosUtil.GetAutoExposeStepEcho(5);
	AutoFocusStepEcho = DosUtil.GetAutoFocusStepEcho(0.015f);
	AutoFocusStepTF = DosUtil.GetAutoFocusStepTF(0.015f);

	// [06182024 ZHIMING
	GetPrivateProfileString(sec, "FlipXECHO", "0", str, 255, inifile);
	bImageFlipXECHO = atoi(str);

	GetPrivateProfileString(sec, "FlipXTFILM", "0", str, 255, inifile);
	bImageFlipXTFILM = atoi(str);
	// ]
	isStart = false;//03192025

}

void C413Global::SaveVideoOption() {
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);
	SaveVideoExposure();

	CString str;
	CString sec = "Video";

	str.Format("%d", bImageFlipYECHO);
	WritePrivateProfileString(sec, "FlipYECHO", str, inifile);

	str.Format("%d", bImageFlipYTFILM);
	WritePrivateProfileString(sec, "FlipYTFILM", str, inifile);

	str.Format("%d", CamraNum[CCamBase::ECHOCAM]);
	WritePrivateProfileString(sec, "EchoCameraID", str, inifile);

	str.Format("%d", CamraNum[CCamBase::THINFCAM]);
	WritePrivateProfileString(sec, "ThinFilmCameraID", str, inifile);

	str.Format("%d", AlgnExposure);
	WritePrivateProfileString(sec, "AlignExposure", str, inifile);

	str.Format("%.4f", fAlignZEcho);
	WritePrivateProfileString(sec, "AlignZPos", str, inifile);

	str.Format("%.4f", FindEdgeThres);
	WritePrivateProfileString(sec, "FindEdgeThreshold", str, inifile);

	DosUtil.SetAutoExposeStepTF(AutoExposeStepTF);
	DosUtil.SetAutoExposeStepEcho(AutoExposeStepEcho);
	DosUtil.SetAutoFocusStepEcho(AutoFocusStepEcho);
	DosUtil.SetAutoFocusStepTF(AutoFocusStepTF);

	// [06182024 ZHIMING
	str.Format("%d", bImageFlipXECHO);
	WritePrivateProfileString(sec, "FlipXECHO", str, inifile);

	str.Format("%d", bImageFlipXTFILM);
	WritePrivateProfileString(sec, "FlipXTFILM", str, inifile);
	// ]
}

void C413Global::LoadVideoExposure(CString inifile) {
	char str[256];

	CString sec = "Video";

	GetPrivateProfileString(sec, "Exposure1", "800", str, 255, inifile);
	Exposure[0] = atoi(str);

	GetPrivateProfileString(sec, "Exposure2", "800", str, 255, inifile);
	Exposure[1] = atoi(str);
}

void C413Global::SaveVideoExposure() {
	SaveExposure(0, Exposure[0]);
	SaveExposure(1, Exposure[1]);
}

void C413Global::SaveExposure(short n, int Exp) {
	char str[256];
	CString inifile;

	DosUtil.GetLocalSysFile(inifile);

	CString str2;
	str2.Format("Exposure%d", n + 1);
	sprintf(str, "%d", Exposure[n]);
	WritePrivateProfileString("Video", str2, str, inifile);
}

void C413Global::LoadEchoOption() {
	char str[256];
	CString inifile, str2;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Echo";

	str2.Format("%.6f", CalZPosEcho);
	GetPrivateProfileString(sec, "CalibPos", str2, str, 255, inifile);
	CalZPosEcho = float(atof(str));

	str2.Format("%.3f", ScaleEcho);
	GetPrivateProfileString(sec, "ScaleEcho", str2, str, 255, inifile);
	ScaleEcho = float(atof(str));

	str2.Format("%d", DMCProgram);
	GetPrivateProfileString(sec, "DMCProgram", str2, str, 255, inifile);
	DMCProgram = atof(str);
}

void C413Global::SaveEchoOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Echo";

	sprintf(str, "%.6f", CalZPosEcho);
	WritePrivateProfileString(sec, "CalibPos", str, inifile);

	sprintf(str, "%.3f", ScaleEcho);
	WritePrivateProfileString(sec, "ScaleEcho", str, inifile);

	sprintf(str, "%d", DMCProgram);
	WritePrivateProfileString(sec, "DMCProgram", str, inifile);
}

void C413Global::LoadRoughOption() {
	char str[256];
	CString inifile, str2;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Rough";

	str2.Format("%.6f", CalZPosRough);
	GetPrivateProfileString(sec, "CalibPos", str2, str, 255, inifile);
	str2.Format("%d", umUnitInRoughness);
	GetPrivateProfileString(sec, "umUnit", str2, str, 255, inifile);
}

void C413Global::SaveRoughOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Rough";

	sprintf(str, "%.6f", CalZPosRough);
	WritePrivateProfileString(sec, "CalibPos", str, inifile);
	sprintf(str, "%d", umUnitInRoughness);
	WritePrivateProfileString(sec, "umUnit", str, inifile);
}

// [ 02082024/MORSALIN
void C413Global::SaveProbeSwitchingOptions() {
	
	char str[256];
	CString name = _T("ProbeSwitching"), inifile;
	DosUtil.GetLocalCfgFile(inifile);

	sprintf(str, "%d", isPSwitchingSimu);
	WritePrivateProfileString(name, "isPSwitchingSimu", str, inifile);
	sprintf(str, "%d", comPort);
	WritePrivateProfileString(name, "comPort", str, inifile);
	sprintf(str, "%d", transRate);
	WritePrivateProfileString(name, "transRate", str, inifile);
	sprintf(str, "%d", stopBit);
	WritePrivateProfileString(name, "stopBit", str, inifile);
	sprintf(str, "%c", parity);
	WritePrivateProfileString(name, "parity", str, inifile);
	sprintf(str, "%d", RoughnessPrbPos);
	WritePrivateProfileString(name, "RoughnessPrbPos", str, inifile);
	sprintf(str, "%d", Acceleration);
	WritePrivateProfileString(name, "Acceleration", str, inifile);
	sprintf(str, "%d", Deceleration);
	WritePrivateProfileString(name, "Deceleration", str, inifile);
	sprintf(str, "%d", Speed);
	WritePrivateProfileString(name, "Speed", str, inifile);
	sprintf(str, "%f", xStageOffset);
	WritePrivateProfileString(name, "xStageOffset", str, inifile);
	sprintf(str, "%f", yStageOffset);
	WritePrivateProfileString(name, "yStageOffset", str, inifile);
}
void C413Global::LoadProbeSwitchingOptions() {
	char str[256];
	CString name = _T("ProbeSwitching"), inifile;
	DosUtil.GetLocalCfgFile(inifile);

	GetPrivateProfileString(name, "isPSwitchingSimu", "1", str, 255, inifile);
	isPSwitchingSimu = (bool)atoi(str);
	GetPrivateProfileString(name, "comPort", "1", str, 255, inifile);
	comPort = atoi(str);
	GetPrivateProfileString(name, "transRate", "1", str, 255, inifile);
	transRate = atoi(str);
	GetPrivateProfileString(name, "stopBit", "1", str, 255, inifile);
	stopBit = atoi(str);
	GetPrivateProfileString(name, "parity", "1", str, 255, inifile);
	parity = (char)atoi(str);
	//[10172024 Morsalin
	GetPrivateProfileString(name, "MicroscopePrbPos", "1", str, 255, inifile);
	MicroscopePrbPos = atoi(str);
	GetPrivateProfileString(name, "CWL008prbPos", "1", str, 255, inifile);
	CWL008prbPos = atoi(str);
	GetPrivateProfileString(name, "CWL030prbPos", "1", str, 255, inifile);
	CWL030prbPos = atoi(str);
	//]
	GetPrivateProfileString(name, "RoughnessPrbPos", "1", str, 255, inifile);
	RoughnessPrbPos = atoi(str);
	GetPrivateProfileString(name, "PosSoftLimit", "1", str, 255, inifile);
	PosSoftLimit = atoi(str);
	GetPrivateProfileString(name, "NegSoftLimit", "1", str, 255, inifile);
	NegSoftLimit = atoi(str);
	GetPrivateProfileString(name, "Acceleration", "1", str, 255, inifile);
	Acceleration = atoi(str);
	GetPrivateProfileString(name, "Deceleration", "1", str, 255, inifile);
	Deceleration = atoi(str);
	GetPrivateProfileString(name, "Speed", "1", str, 255, inifile);
	Speed = atoi(str);
	GetPrivateProfileString(name, "xStageOffset", "1", str, 255, inifile);
	xStageOffset = atof(str);
	GetPrivateProfileString(name, "yStageOffset", "1", str, 255, inifile);
	yStageOffset = atof(str);
}
//]

// [ //24102024/MORSALIN
void C413Global::SaveLTV_STIR_Options() {
	
	char str[256];
	CString name = _T("LTV-STIR-Measurement"), inifile;
	DosUtil.GetLocalCfgFile(inifile);

	sprintf(str, "%d", DespikeWindowSize);
	WritePrivateProfileString(name, "DespikeWindowSize", str, inifile);
	sprintf(str, "%d", GaussianKernelSize);
	WritePrivateProfileString(name, "GaussianKernelSize", str, inifile);
	sprintf(str, "%lf", GaussianSigma);
	WritePrivateProfileString(name, "GaussianSigma", str, inifile);
	sprintf(str, "%lf", scanOffset);
	WritePrivateProfileString(name, "ScanOffset", str, inifile);
	sprintf(str, "%lf", scanSpeed);
	WritePrivateProfileString(name, "ScanSpeed", str, inifile);
	sprintf(str, "%lf", LinePerDie);
	WritePrivateProfileString(name, "LinePerDie", str, inifile);
	sprintf(str, "%lf", isSqDie);
	WritePrivateProfileString(name, "isSqDie", str, inifile);
	sprintf(str, "%lf", ScanPolarity);
	WritePrivateProfileString(name, "ScanPolarity", str, inifile);
}
void C413Global::LoadLTV_STIR_Options() {
	char str[256];
	CString name = _T("LTV-STIR-Measurement"), inifile;
	DosUtil.GetLocalCfgFile(inifile);

	GetPrivateProfileString(name, "DespikeWindowSize", "1", str, 255, inifile);
	DespikeWindowSize = (int)atoi(str);
	GetPrivateProfileString(name, "GaussianKernelSize", "1", str, 255, inifile);
	GaussianKernelSize = atoi(str);
	GetPrivateProfileString(name, "GaussianSigma", "1", str, 255, inifile);
	GaussianSigma = atof(str);
	GetPrivateProfileString(name, "ScanOffset", "1", str, 255, inifile);
	scanOffset = atof(str);
	GetPrivateProfileString(name, "ScanSpeed", "1", str, 255, inifile);
	scanSpeed = atof(str);
	GetPrivateProfileString(name, "LinePerDie", "1", str, 255, inifile);
	LinePerDie = atof(str);
	GetPrivateProfileString(name, "isSqDie", "1", str, 255, inifile);
	isSqDie = (BOOL)atoi(str);
	GetPrivateProfileString(name, "ScanPolarity", "1", str, 255, inifile);
	ScanPolarity = (BOOL)atoi(str);
}
//]

void C413Global::LoadMeasurementOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Measurement";

	GetPrivateProfileString(sec, "DPGageThickness", "500", str, 255, inifile);
	DPGageThickness = atof(str);

	GetPrivateProfileString(sec, "SPGageThickness", "500", str, 255, inifile);
	SPGageThickness = atof(str);

	bResetX = GetPrivateProfileInt(sec, "ResetX", bResetX, inifile);

	bEnableIntensity = GetPrivateProfileInt(sec, "Intensity", bEnableIntensity, inifile);

	bEnablePosition = GetPrivateProfileInt(sec, "Position", bEnablePosition, inifile);

	bEnableMaths = GetPrivateProfileInt(sec, "Maths", bEnableMaths, inifile);

	// [11302022 ZHIMING
	bEnableBow = GetPrivateProfileInt(sec, "Bow", bEnableBow, inifile);

	bEnableWarp = GetPrivateProfileInt(sec, "Warp", bEnableWarp, inifile);

	// [20231226
	bDualConfocal = GetPrivateProfileInt(sec, "DualConfocal", bDualConfocal, inifile);
	// ]

	bDoorInterlock = GetPrivateProfileInt(sec, "DoorInterlock", bDoorInterlock, inifile);

	nDoorInterlockInterval = GetPrivateProfileInt(sec, "DoorInterlockInterval", nDoorInterlockInterval, inifile);

	pinHeight = GetPrivateProfileInt(sec, "PinHeight", pinHeight, inifile); //18122024

	// [03052025
	thkTrialNum = GetPrivateProfileInt(sec, "ThicknessTrial", thkTrialNum, inifile);
	warpTrialNum = GetPrivateProfileInt(sec, "WarpTrial", warpTrialNum, inifile);
	// ]
	
	//
	// 	sec = "FindEdge";
	//
	// 	GetPrivateProfileString(sec, "Method",  "0", str, 255, inifile);
	// 	FindEdgeMethod = atoi(str);
}

void C413Global::SaveMeasurementOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Measurement";

	sprintf(str, "%.3f", DPGageThickness);
	WritePrivateProfileString(sec, "DPGageThickness", str, inifile);

	sprintf(str, "%.3f", SPGageThickness);
	WritePrivateProfileString(sec, "SPGageThickness", str, inifile);

	sprintf(str, "%d", bResetX);
	WritePrivateProfileString(sec, "bResetX", str, inifile);

	sprintf(str, "%d", bEnableIntensity);
	WritePrivateProfileString(sec, "Intensity", str, inifile);

	sprintf(str, "%d", bEnablePosition);
	WritePrivateProfileString(sec, "Position", str, inifile);

	sprintf(str, "%d", bEnableMaths);
	WritePrivateProfileString(sec, "Maths", str, inifile);

	sprintf(str, "%d", bEnableBow);
	WritePrivateProfileString(sec, "Bow", str, inifile);

	sprintf(str, "%d", bEnableWarp);
	WritePrivateProfileString(sec, "Warp", str, inifile);

	// [20231226
	sprintf(str, "%d", bDualConfocal);
	WritePrivateProfileString(sec, "DualConfocal", str, inifile);
	// ]

	sprintf(str, "%d", bDoorInterlock);
	WritePrivateProfileString(sec, "DoorInterlock", str, inifile);

	sprintf(str, "%d", nDoorInterlockInterval);
	WritePrivateProfileString(sec, "DoorInterlockInterval", str, inifile);
	//
	// 	sec = "FindEdge";
	//
	// 	sprintf(str,"%d", FindEdgeMethod);
	// 	WritePrivateProfileString(sec, "Method", str, inifile);

}

void C413Global::LoadThinFilmOption() {
	char str[256];
	CString inifile, str2;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Spectrometer";

	sprintf(str, "%.3f", TFScalingFactor);
	GetPrivateProfileString(sec, "ThkFactor", str, str, 255, inifile);
	TFScalingFactor = atof(str);

	sec = "ThinFilm";
	str2.Format("%.6f", CalZPosTF);
	GetPrivateProfileString(sec, "CalibPos", str2, str, 255, inifile);
	CalZPosTF = float(atof(str));

	str2.Format("%.3f", ScaleTF);
	GetPrivateProfileString(sec, "ScaleTF", str2, str, 255, inifile);
	ScaleTF = float(atof(str));
}

void C413Global::SaveThinFilmOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Spectrometer";

	sprintf(str, "%.3f", TFScalingFactor);
	WritePrivateProfileString(sec, "ThkFactor", str, inifile);

	sec = "ThinFilm";
	sprintf(str, "%.6f", CalZPosTF);
	WritePrivateProfileString(sec, "CalibPos", str, inifile);

	sprintf(str, "%.3f", ScaleTF);
	WritePrivateProfileString(sec, "ScaleTF", str, inifile);
}

void C413Global::Polar2Cartesian(float radius, float ang, float& x, float& y) {
	while (ang >= 360.0f) ang -= 360.0f;
	while (ang < 0) ang += 360.0f;

	ang = float(ang * PIE180);
	x = float(radius * cos(ang));
	y = float(radius * sin(ang));
}

void C413Global::Cartesian2Polar(float X, float Y, float& radius, float& angle) {
	radius = float(sqrt(X * X + Y * Y));
	angle = atan2(Y, X);
	angle /= PIE180;
	while (angle >= 360.0f) angle -= 360.0f;
	while (angle < 0) angle += 360.0f;
}

BOOL C413Global::LoadChuckMask() {
	if (m_pChuckMask) {
		delete m_pChuckMask;
	}
	m_pChuckMask = new CChuckMask;

	CString MaskFile;
	DosUtil.GetProgramDir(MaskFile);
	MaskFile += "300mmChuck.msk";
	if (m_pChuckMask->LoadFromFile(MaskFile)) {
		return TRUE;
	}
	AfxMessageBox("No Chuck Mask file found");
	return FALSE;
}

void C413Global::SaveScrnCalibINI() {
	ScrnCalibTF.SaveParam("ScreenCalibTF");
	ScrnCalibECH.SaveParam("ScreenCalibEcho");
}

void C413Global::LoadScrnCalibINI() {
	ScrnCalibTF.LoadParam("ScreenCalibTF");
	ScrnCalibECH.LoadParam("ScreenCalibEcho");
}

void C413Global::LoadModule() {
	LoadPosConfType = (C413Global::LOADPOSCONFTYPE)DosUtil.GetLoadPosConfType(USEECHOPROBE);
	FFUPressure = DosUtil.GetFFUPressure();
	WaferOnStage = DosUtil.GetWaferOnStage(WaferOnStage);
	WaferSize = DosUtil.GetWaferSize(WaferSize);
	DosUtil.GetTgtRed(fTgtRedTF, fTgtRedECHO);
	ResetFreq = DosUtil.GetResetFreq();
	bCheckWaferPresence = DosUtil.GetCheckWaferPresent(bCheckWaferPresence);
}

void C413Global::SaveModule() {
	DosUtil.SetFFUPressure(FFUPressure);
	DosUtil.SetWaferOnStage(WaferOnStage);
	DosUtil.SetWaferSize(WaferSize);
	DosUtil.SetTgtRed(fTgtRedTF, fTgtRedECHO);
	DosUtil.SetLoadPosConfType(LoadPosConfType);
	DosUtil.SetResetFreq(ResetFreq);
	DosUtil.SetCheckWaferPresent(bCheckWaferPresence);
}

CScrCalib::CScrCalib() {
	Clear();
}

void CScrCalib::Clear() {
	dX = 0.0025556f / 4.0f;
	dY = 0.0025546f / 4.0f;
	ppmX = ppmY = 1;
	ScrnX = ScrnY = 1;
}

void CScrCalib::SaveParam(CString name) {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	sprintf(str, "%f", dX);
	WritePrivateProfileString(name, "dX", str, inifile);

	sprintf(str, "%f", dY);
	WritePrivateProfileString(name, "dY", str, inifile);

	sprintf(str, "%f", ppmX);
	WritePrivateProfileString(name, "mppX", str, inifile);

	sprintf(str, "%f", ppmY);
	WritePrivateProfileString(name, "mppY", str, inifile);

	sprintf(str, "%d", ScrnX);
	WritePrivateProfileString(name, "ScrnX", str, inifile);

	sprintf(str, "%d", ScrnY);
	WritePrivateProfileString(name, "ScrnY", str, inifile);
}

void CScrCalib::LoadParam(CString name) {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	GetPrivateProfileString(name, "dX", "1", str, 255, inifile);
	dX = atof(str);

	GetPrivateProfileString(name, "dY", "1", str, 255, inifile);
	dY = atof(str);

	GetPrivateProfileString(name, "mppX", "1", str, 255, inifile);
	ppmX = atof(str);

	GetPrivateProfileString(name, "mppY", "1", str, 255, inifile);
	ppmY = atof(str);

	GetPrivateProfileString(name, "ScrnX", "1", str, 255, inifile);
	ScrnX = atoi(str);

	GetPrivateProfileString(name, "ScrnY", "1", str, 255, inifile);
	ScrnY = atoi(str);
}

void C413Global::LoadLoadPortOptions() {
	LPNo[0] = 4;
	LPNo[1] = 6;
	LPNo[2] = 8;
	LPNo[3] = 10;

	CString name = "LPUnit", inifile;

	DosUtil.GetLocalCfgFile(inifile);
	LPNo[0] = GetPrivateProfileInt(name, "InCasLoc1", LPNo[0], inifile);
	LPNo[1] = GetPrivateProfileInt(name, "InCasLoc2", LPNo[1], inifile);
	LPNo[2] = GetPrivateProfileInt(name, "InCasLoc3", LPNo[2], inifile);
	LPNo[3] = GetPrivateProfileInt(name, "InCasLoc4", LPNo[3], inifile);
}

//[[20240820/ Arup
void C413Global::LoadMaxSlot() {

	/*CString name = "Config", inifile;

	DosUtil.GetLocalCfgFile(inifile);*/
	CString name = "Variables", inifile;
	DosUtil.GetLocalSysFile(inifile);
	SlotsPerFOUP = GetPrivateProfileInt(name, "MaxSlot", SlotsPerFOUP, inifile);
	
}
//]]
void C413Global::SaveLoadPortOptions() {
	char str[256];
	CString name = "LPUnit", inifile;

	DosUtil.GetLocalCfgFile(inifile);

	sprintf(str, "%d", LPNo[0]);
	WritePrivateProfileString(name, "InCasLoc1", str, inifile);

	sprintf(str, "%d", LPNo[1]);
	WritePrivateProfileString(name, "InCasLoc2", str, inifile);

	sprintf(str, "%d", LPNo[2]);
	WritePrivateProfileString(name, "InCasLoc3", str, inifile);

	sprintf(str, "%d", LPNo[3]);
	WritePrivateProfileString(name, "InCasLoc4", str, inifile);
}

BOOL C413Global::Save() {
	CFile theFile;
	CString filename;

	DosUtil.GetProgramDir(filename);
	filename += "\\GLOBAL.BIN";

	if (theFile.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL C413Global::Load() {
	CFile theFile;
	CString filename;

	DosUtil.GetProgramDir(filename);
	filename += "\\GLOBAL.BIN";

	if (theFile.Open(filename, CFile::modeRead)) {
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}
