// DOS.h: interface for the CDOS class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <direct.h>
#include <afxmt.h>

#include "DirSys.h"
#include "CamBase.h"

class CFPoint;

class CDOS {
	CCriticalSection CS;
	CString sec, str;
public:
	CDirSys DirSys;
	CString FSMAppFile;
	CString ProgramDir;
	CString LocalSysFile;
	CString LocalCfgFile;

public:
	void GetTgtRed(CFPoint& PtTF, CFPoint& PtEC);
	void SetTgtRed(CFPoint& PtTF, CFPoint& PtEC);
	float GetAutoFocusStepTF(float factor);
	void SetAutoFocusStepTF(float factor);
	float GetAutoFocusStepEcho(float factor);
	void SetAutoFocusStepEcho(float factor);
	short GetAutoExposeStepTF(short stp);
	void SetAutoExposeStepTF(short stp);
	short GetAutoExposeStepEcho(short stp);
	void SetAutoExposeStepEcho(short stp);

	void SetRecipeButtonStr(int RcpNum, CString RcpName);
	CString GetRecipeButtonStr(int RcpNum);

	short GetCIDRUnit(short which);
	void SetCIDRUnit(short which, short id);

	void GetSetLPLight(CString str);
	void SetWaferOnStage(short No);
	short GetWaferOnStage(short No);
	void SetLoadPosConfType(short No);
	short GetLoadPosConfType(short No);
	void SetWaferSize(float No);
	float GetWaferSize(float No);
	// 	short GetLaserName1(short num, short deft);
	// 	void SetLaserName1(short num, short freq);
	// 	short GetWaferPresentABLimit();
	// 	void SetWaferPresentABLimit(short limit);
	// 	void SetSPCMethod(short module, short state);
	// 	short GetSPCMethod(short module);
	void SetWaferPresent(BOOL state);
	BOOL GetWaferPresent();
	void SetDumpCounter(short state);
	short GetDumpCounter();
	// 	short GetSPL();
	// 	void SetSPL(short pos);
	// 	short GetOrder();
	// 	void SetOrder(short pos);
	// 	short GetPixelRes();
	// 	void SetPixelRes(short pos);
	// 	short GetDensity();
	// 	void SetDensity(short pos);
	// 	short GetShowDataPoint();
	// 	void SetShowDataPoint(short pos);
	BOOL GetFlattenMap();
	void SetFlattenMap(BOOL pos);
	// 	float GetMapWeight();
	// 	void SetMapWeight(float val);
	void SetFlipMapX(short pos);
	short GetFlipMapX();
	void SetNotchAngle(short pos);
	short GetNotchAngle();
	BOOL SetJobEventSupport(short cnt);
	int GetJobEventSupport();
	BOOL SetJobAlertSupport(short cnt);
	int GetJobAlertSupport();
	BOOL SetIgnorRecipeParameter(short cnt);
	int GetIgnorRecipeParameter();
	// 	void SetTCPort(short port);
	// 	short GetTCPort();
	CString GetLogDirectory();
	// 	void SetDIOEcho(BOOL bEcho);
	// 	BOOL GetDIOEcho();
	// 	short GetColdTempr();
	// 	void SetColdTempr(short tempr);
	// 	short GetBakeTempr();
	// 	void SetBakeTempr(short tempr);
	// 	float GetTargetVac();
	// 	void SetTargetVac(float fac);
	// 	float GetAtmVac();
	// 	void SetAtmVac(float fac);

	CString GetRememberedSM(short unit);
	void ForgetSM(short unit);
	void RememberSM(short unit, CString SM);

	void GetRememberedPrJob(short unit, CString& CarrierId, CString& Recipe);
	void ForgetPrJob(short unit);
	void RememberPrJob(short unit, CString& CarrierId, CString& Recipe);

	void SetZSPMM(float val);
	void SetXSPMM(float val);
	float GetZSPMM();
	float GetXSPMM();
	void WriteRootDir(CString dir);
	CString GetRootDir();
	// 	void SetEnaAlignerCtrl(BOOL bEna);
	// 	BOOL GetEnaAlignerCtrl();
	// 	void SetAlignerCtrlPort(short port);
	// 	short GetAlignerCtrlPort();
	// 	void SetEnableModeSwitch(BOOL bEna);
	// 	BOOL GetEnableModeSwitch();
	// 	void SetVacOffTempr(short tempr);
	// 	short GetVacOffTempr();
	// 	void SetWDTSimu(BOOL bSimu);
	// 	BOOL GetWDTSimu();
	void SetLPUnitSimu(BOOL bSimu, short unit);
	BOOL GetLPUnitSimu(short unit);
	void SetRbtUnitSimu(BOOL bSimu);
	BOOL GetRbtUnitSimu();
	void SetAlignerOffsetPos(short pos, long offset);
	long GetAlignerOffsetPos(short pos);
	void SetCCDPos(short arm, long offset);
	long GetCCDPos(short arm);
	void SetCCDNum(short arm, short num);
	short GetCCDNum(short arm);
	void SetAlignerPostPos(short pos, long post);
	long GetAlignerPostPos(short pos);
	short GetLogosolAlignerPort(); //Tushar 21082024
	long GetLogosolAlignerPostPosAngle();//Tushar 21082024
	short GetLogosolCCDNum(short arm);//tushar 21082024
	// 	short GetWDTPort();
	// 	void SetWDTPort(short Port);

	// 	float GetRoughingPressure();
	// 	float GetHoldingPressure();
	// 	float GetAutoOnPressure();
	// 	float GetIGSensitivity();
	// 	float GetIGGasCorrection();

	// 	BOOL GetSenTorrUnitSimu();
	// 	void SetSenTorrUnitSimu(BOOL bSimu);
	//	BOOL GetDIOSimu();
	//	void SetDIOSimu(BOOL bSimu);
	// 	BOOL GetDaqSimu();
	// 	void SetDaqSimu(BOOL bSimu);
	// 	BOOL GetPLCSimu();
	// 	void SetPLCSimu(BOOL bSimu);
	BOOL GetCIDRSimu(short unit);
	void SetCIDRSimu(BOOL bSimu, short unit);
	// 	BOOL ReadPLCSimuDefaluts(DWORD &I0,DWORD &I1,DWORD &O0,DWORD &B3_0,
	// 		 DWORD &B3_1,DWORD &B3_2,DWORD &B3_3);
	// 	BOOL ReadPLCSimu(DWORD &I0,DWORD &I1,DWORD &O0,DWORD &B3_0,
	// 		DWORD &B3_1,DWORD &B3_2,DWORD &B3_3);
	// 	BOOL SetPLCSimuDefaluts(DWORD I0,DWORD I1,DWORD O0,DWORD B3_0,
	// 		DWORD B3_1,DWORD B3_2,DWORD B3_3);
	// 	BOOL SetPLCSimuB3_3(DWORD Flag);
	// 	BOOL SetPLCSimuB3_2(DWORD Flag);
	// 	BOOL SetPLCSimuB3_1(DWORD Flag);
	// 	BOOL SetPLCSimuB3_0(DWORD Flag);
	// 	BOOL SetPLCSimuO0(DWORD Flag);
	// 	BOOL SetPLCSimuI1(DWORD Flag);
	// 	BOOL SetPLCSimuI0(DWORD Flag);
	int GetCasRepeat();
	BOOL SetCasRepeat(short cnt);
	int GetScanRepeat();
	BOOL SetScanRepeat(short cnt);
	// 	int GetDIOMotor();
	// 	void SetDIOMotor(short Port);
	// 	int GetCommSenTorr();
	// 	void SetCommSenTorr(short Port);
	// 	float GetPSDMinVolt(int UnitNo=0);
	// 	float GetLaserMaxVolt(int UnitNo=0);
	// 	unsigned char GetAO810Chan(int UnitNo=0);
	// 	unsigned char GetAO750Chan(int UnitNo=0);
	char* GetRecipeDir();
	// 	void SetBeampath(short lsr, float val);
	// 	float GetBeampath(short lsr, float def = 340.0f);
	float GetStressConst(CString def);
	void SetStressConst(CString Konst);
	void SetStressFactor(float factor);
	float GetStressFactor(float stressfactor);
	void SetStressUnit(CString unit);
	CString GetStressUnit(CString unit);

	void SetLTMethod(CString method);
	CString GetLTMethod(CString def);
	void SetLBUnclampped(CString val);
	CString GetLBUnclampped(CString val);
	void SetLBButtonPressed2(CString def);
	void SetLBButtonPressed1(CString def);
	CString GetLBButtonPressed2(CString val);
	CString GetLBButtonPressed1(CString val);
	void SetEnterDoc(BOOL bDef);
	BOOL GetEnterDoc(BOOL bDef);
	void SetFilmThkFactor(float factor);
	void SetFilmThkUnit(CString unit);
	CString GetFilmThkUnit(CString unit);
	float GetFilmThkFactor(float stressfactor);
	void SetClampState(BOOL state);
	BOOL GetClampState();
	//	BOOL GetTimeButton(BOOL dwfval);

	enum OPENCASSETTESWITCHTYPE {
		OCNONE, OCSANDIA, OCTTI, OCSMIF
	};

	void WriteJobOption(short option);
	short GetJobOption();
	void WriteAutoOnline(BOOL option);
	BOOL GetAutoOnline(BOOL bConnect);
	void WriteAutoConnect(BOOL option);
	BOOL GetAutoConnect(BOOL bConnect);
	short GetFanFilterUnit();
	void WriteFanFilterUnit(short mode);
	float GetFFUPressure();
	void SetFFUPressure(float press);
	float GetResetFreq();
	void SetResetFreq(float freq);
	void SetFFUType(CString type);
	CString GetFFUType();
	BOOL GetHostVerSM();
	void WriteHostVerSM(BOOL mode);
	// 	BOOL GetBCRMode(BOOL mode);
	// 	void WriteBCRMode(BOOL mode);
	// 	void SetOpenCassetteSwitchType(CDOS::OPENCASSETTESWITCHTYPE type);
	// 	CDOS::OPENCASSETTESWITCHTYPE GetOpenCassetteSwitchType();
	BOOL GetOS(CString& OS);
	float GetEncoderFactor(short Probe);
	void SetEncoderFactor(short Probe, float factor);
	float GetTRiCorr();
	void SetTRiCorr(float factor);
	float GetBRiCorr();
	void SetBRiCorr(float factor);

	// 	short GetPkSpc();
	// 	void SetPkSpc(short spc);

	// 	short GetPkHeight();
	// 	void SetPkHeight(short height);

	// 	short GetPkCln();
	// 	void SetPkCln(short Cln);

	void SetRobotPort(short port);
	short GetRobotPort();

	void SetInCasLoc(short loc, short val);
	short GetInCasLoc(short loc, short def);
	// 	void SetCassetteSwitchSimu(BOOL bSimu);
	// 	BOOL GetCassetteSwitchSimu();
	void SetLPLight(CString Name, CString val);
	CString GetLPLight(CString Name, CString val = "");
	void SetLPLightAuto(CString Name, CString val);
	CString GetLPLightAuto(CString Name, CString val = "");
	void SetLTower(CString Name, CString val);
	CString GetLTower(CString Name, CString val = "");
	CString GetSlotmap();
	void WriteSlotmap(CString SM);
	void SetLPUnit(CString Name, short port);
	short GetLPUnit(CString Name, short port);
	void SetLPUnit(CString Name, CString str);
	CString GetLPUnit(CString Name, CString str);
	void SetLPUnit(CString Name, BOOL bNV);
	BOOL GetLPUnit(CString Name, BOOL bDef = 0);
	void SetCIDRType(CString& type);
	CString GetCIDRType();
	void SetHelpType(CString& type);
	CString GetHelpType();
	// 	void SetStorageType(short type);
	// 	short GetStorageType();
	void SetRbtType(CString type);
	CString GetRbtType();
	// 	void SetRbtRecoveryType(CString type);
	// 	CString GetRbtRecoveryType();
	short GetUIMode();
	void WriteUIMode(short mode);
	// 	CString GetIPAddr(short which);
	// 	void WriteIPAddr(CString IPAddr, short which);
	void* StartModule(CString name, BOOL bWait2Start = TRUE);
	void GetFSMAppFile(CString& filename);
	void GetLocalSysFile(CString& filename);
	void GetLocalCfgFile(CString& filename);
	BOOL GetProgramDir(CString& mdir);
	void RemoveExtension(CString& filename);
	void ExtractPath(CString& filename);
	void ExtractFilename(CString& filename);
	void ExtractFilenameExt(CString& filename);
	CString MakeFullPathName(CString& path, CString& name, CString ext);
	void AddExtension(CString& filename, CString ex);
	BOOL MakeFileNameFromModule(CString& filename, CString& name, CString extn);
	BOOL SetWorkDirectory(CString& rootdir, CString& workdir);
	float GetEncoderRes(int UnitNo);
	// 	BOOL IsLaserSwitchCyl();
	// 	void SetLaserSwitchCyl(BOOL bCylinder);
	// 	float GetDaqGain(int UnitNo);
	// 	unsigned char BeamChannel(int UnitNo);
	// 	int GetDaqDeviceID(int UnitNo);
	// 	short GetSiMOtorPort();
	// 	void SetSiMOtorPort(short Port);

		// FSM Inhouse signal tower fuctions
	BOOL SetSignalTowerPort(unsigned char Port);
	int GetSignalTowerPort();
	CDOS();
	virtual ~CDOS();

	CString GetResultDir(); //08142024

	short GetLogLevel(void);
	void WriteLogLevel(short level);

	void SetNumLP(short loc);
	void SetNumCIDR(short loc);

	short GetNumLP();
	short GetNumCIDR();

	void SetLPPort(short which, short port);
	short GetLPPort(short which);

	void SetSiNetPort(short port);
	short GetSiNetPort();
	short GetSiDrvPort();
	void SetSiDrvPort(short port);

	void SetCIDRPort(short which, short port);
	short GetCIDRPort(short which);

	// 	float GetZgapEM();				//added AP 20111119
	// 	void SetZgapEM(float val);

	int GetSendAllPoint(); //20130903-1 SCOTT ADD

	short GetCameraType(CCamBase::CAMERAID iCam);
	void SetCameraType(short type, CCamBase::CAMERAID iCam);

	BOOL TurnConfigTab();				// 06202022 / yukchiu
	void Yld(DWORD dly) {
		ULONGLONG tick = GetTickCount64();
		while ((GetTickCount64() - tick) < dly) {
			MSG msg;
			while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	// [01182024 ZHIMING
	double GetConfocalK();
	void SetConfocalK(double kVal);
	// [03152024
	void SetCheckWaferPresent(BOOL bCheck);
	short GetCheckWaferPresent(BOOL bCheck);
	// ]

	//08142024 - START
	int ReadINI(char* section, char* name, int defvalue, CString& inifile);
	ULONG ReadINI(char* section, char* name, ULONG defvalue, CString& inifile);
	float ReadINI(char* section, char* name, float defvalue, CString& inifile);
	double ReadINI(char* section, char* name, double defvalue, CString& inifile);
	CString ReadINI(char* section, char* name, CString& str, CString& inifile);

	void WriteINI(char* section, char* key, int val, CString& inifile);
	void WriteINI(char* section, char* key, ULONG val, CString& inifile);
	void WriteINI(char* section, char* key, UINT val, CString& inifile);
	void WriteINI(char* section, char* key, char* str, CString& inifile);
	void WriteINI(char* section, char* key, CString& str, CString& inifile);
	void WriteINI(char* section, char* key, double val, CString& inifile);
	void WriteINI(char* section, char* key, float val, CString& inifile);
	//08142024 - END
};

extern CDOS DosUtil;
