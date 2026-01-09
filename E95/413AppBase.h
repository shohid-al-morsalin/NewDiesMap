#pragma once

#define PI 3.1415926535897932384626433832795
#define PIE180 0.0174532925199432957692369076849

#include "Global413.h"
#include "413/EchoScope/Peaks.h"

class CEchoProbeBase;
class CMotorS2;
class CDMCMotorBase;
class COceanBase;
class CMotionControl;
class CFFUFailDlg;
class CRecipe413;
class CMPoint;
class CImgWnd;
class CRoughProbeBase; // Mahedi Kamal - 03202024
class CRingWidthProbeBase; // Mahedi Kamal - 08132024
class CProbeSwitchingBase; //02082024/MORSALIN
class CCWLBase;

class C413AppBase {
public:
	short rnd1, rnd2, maxrnd1;
	//	float PatOffX, PatOffY;
	float fCurEchoTopZPos;	// Intermediate [10/12/2012 FSM413]
	BOOL bSimuEcho, bSimuDMC, bReverseAB, bReverseSignal;
	BOOL bResetYFirst;
	BOOL bSimuStage, bSimuX, bSimuY, bSimuEchoTopZ, bSimuThinFilmZ, bSimuP;
	int nSpectroType;
	//BOOL bSimuVideo, bEchoCam, bThinFCam;
	CString MsgStr;
	CMotorS2* pMtr;
	CProbeSwitchingBase* pPSwitch; //02082024/MORSALIN

public:
	CString GetInnerEdge1(float angle);
	CString GetInnerEdge2(/*CRecipe413 *pRcp, */float angle);
	CString GetOuterEdge1(float angle);
	CString GetOuterEdge2(float angle);
	void LoadSimuDIB(CSICam* pVideo);

	void PerpendicularLine(sABC& PL1, sABC& L1, float X1, float Y1, float X2, float Y2);
	void Intersec(sABC& L1, sABC& L2, float& Cx, float& Cy);
	void FindABC(sABC& P, float X1, float Y1, float X2, float Y2);
	CString GetTimeStamp();
	CPtrList* GetCurMPList(CRecipe413* pRcp);
	float fRemoveFlyer(int n, float* pQ, int in, short MaxPkCnt);
	void Log(CString msg, short lvl = 5);
	void RemoveFlyerX(float* thk, short& cnt);
	float GetAverage(float* thk, short cnt);
	float GetRange(float* thk, short cnt);
	void Message(CListCtrl* pList, CString& msg, short lvl = 5);
	void Message(CListBox* pList, CString& str, short lvl = 5);
	short sign(long value);
	short sign(double value);

	void DefinePeakPosition0(short n);
	void DefinePeakPosition6(short n);
	void DefinePeakPosition5(CMPoint* p, short Delta);
	void DefinePeakPosition4(CMPoint* p, short Delta);
	void DefinePeakPosition3(CMPoint* p, short Delta);
	void DefinePeakPosition2(CMPoint* p, short delta);
	void DefinePeakPosition1(CMPoint* p, short delta);
	void Yld(ULONGLONG dly);
	BOOL ValidCheckRcp(CRecipe413& Rcp);
	BOOL IsOperating();
	void SaveThinFilmOption();
	void LoadThinFilmOption();
	void SaveEchoOption();
	void LoadEchoOption();
	void SaveVideoOption();
	void LoadVideoOption();
	void SaveMotorOption();
	void LoadMotorOption();
	void SaveINI();
	void LoadINI();
	void CloseDmpPeak();
	void OpenDmpPeak();
	void SleepEX(ULONGLONG dly);

	CDMCMotorBase* pDMC;
	// 	COceanBase * pOceanOp;
	C413Global Global;
	CMotionControl* pMCtrl;
	CEchoProbeBase* pEcho;
	//	CFFUFailDlg * pFFU;

	CPeaks Pk;
	float SRIT, SRIB;

	float cfTop; // Top probe correction factor [11/5/2011 Administrator]
	float cfBot; // Bottom probe correction factor [11/5/2011 Administrator]
	float TRiCorr; // Top probe RI correction [1/7/2011 FSMT]
	float BRiCorr; // Bottom probe RI correction [1/7/2011 FSMT]
	int Exposure; // Temp value [12/14/2010 C2C]

	enum MOTORCTRLER {
		SICONTRLR, GALILCONTRLR
	};

	MOTORCTRLER MotorController;
	char IPAddress[32];

	float backLashX, backLashY;
	float backLashZ;

	short SiHubPort;
	short EchoPort;
	short MCBoardNo;

	BOOL bLoadDIB;	// For simulation use only [5/20/2013 user]

public:
	void LoadSimuDIB3(CSICam* pVideo, float angle);
	void SaveVideo(CSICam* pVideo, CRecipe413* pRcp);
	void LoadSimuDIB2(CSICam* pVideo);
	void MouseNormal(HWND hWndPC);
	void MouseWait(HWND hWndPC);
	void StageMoveError();
	void RecordWaferCenterEvent(CString& strEvent);
	void RecordSPCalibEvent(CString& strEvent);

	// [11212022 ZHIMING
	CCWLBase* pCWL;
	CCWLBase* pCWL2; // for UTAC it is for 008 / //[ Mahedi Kamal 10182024
	BOOL bSimuConfocal;
	float ZFocusPos008; // Mahedi Kamal 10152024
 	float ThicknessScanLength; // 20250926_Mohir
 	float ThicknessScanSpeed; // 10242025_M
	float ZFocusPos030; // Mahedi Kamal 10152024
	float ZFocusDevRenge008; // Mahedi Kamal 10152024
	float ZFocusDevRenge030; // Mahedi Kamal 10152024
	BOOL IsFocusScanInMeasurementRoutine;//Mahedi ARIF 12120224
	BOOL bConfocalRS232NotEthernet; // Mahedi Kamal 10172024
	CString sConfocalAddress;
	CString sConfocalAddress2; // //[ Mahedi Kamal 10182024
	int ConfocalPort1; // Mahedi Kamal 10182024
	int ConfocalPort2; // Mahedi Kamal 10182024
	void SaveConfocalOption();
	void LoadConfocalOption();
	// [03202024 Mahedi Kamal -- only one entity will exist through out the code 
	CRoughProbeBase* pRough; 
	//[ // Mahedi Kamal 08132024
	CRingWidthProbeBase* pRingWidth;
	BOOL bSimuRingWidth; 
	//]
	// [04082024 ZHIMING
	BOOL bSimuRough;
	int nRoughPort;
	void SaveRoughOption() {
		char str[256];
		CString inifile;

		DosUtil.GetLocalCfgFile(inifile);

		CString sec = "Ports";
		sprintf(str, "%d", nRoughPort);
		WritePrivateProfileString(sec, "Rough", str, inifile);

		sec = "Rough";
		sprintf(str, "%d", bSimuRough);
		WritePrivateProfileString(sec, "bSimu", str, inifile);
	}
	void LoadRoughOption() {
		char str[256];
		CString inifile;

		DosUtil.GetLocalCfgFile(inifile);

		CString sec = "Ports";
		GetPrivateProfileString(sec, "Rough", "0", str, 255, inifile);
		nRoughPort = atoi(str);

		sec = "Rough";
		GetPrivateProfileString(sec, "bSimu", "1", str, 255, inifile);
		bSimuRough = atoi(str);
	}

	// [Mahedi Kamal 08132024
	void SaveRingWidthOption() {
		char str[256];
		CString inifile;

		DosUtil.GetLocalCfgFile(inifile);

		CString sec = "RingWidth";
		sprintf(str, "%d", bSimuRingWidth);
		WritePrivateProfileString(sec, "bSimu", str, inifile);
	}
	void LoadRingWidthOption() {
		char str[256];
		CString inifile;

		DosUtil.GetLocalCfgFile(inifile);

		CString sec = "RingWidth";
		GetPrivateProfileString(sec, "bSimu", "1", str, 255, inifile);
		bSimuRingWidth = atoi(str);
	}
	//]
	
	// [20240426 Tushar
	BOOL bSimuEchoBottomZ;
	short SiHubPort2;
	// [05092024 ZHIMING
	BOOL bSimuRoughZ;
	// [05142024
	float fCurRoughZPos;
	float fCurEchoBottomZPos;
	float fCurCWLTopZPos;
	// ]
	BOOL bSimuTopCWLZ; // 06112024/MORSALIN
	BOOL bSimuTopMicroscopeZ; // 06112024/MORSALIN


	C413AppBase();
	virtual ~C413AppBase();
};

extern float gPX[];
extern float gPY[];
