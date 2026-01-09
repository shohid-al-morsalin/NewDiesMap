#pragma once

#include "AlrmList.h"
#include "EqpState.h"
#include "LogFEM.h"
#include "SRC/Arams.h"

class COLnClt;
class CLPMgr;
class CIDRMgr;
class CLogSvr;
class CRbtMgr;
class CLTWMgr;
class CDrLockMgr;
class COnlineDev;
class CLPXChDat;
class COnlineMgr;
class SecsItem;
class SecsItemList;
class CObjectsContainer;
class CSystemStatusDlg;
class CDataBank;
class CCtrlJob;
class CPRJob;

struct PortGroup;

class CMgrStatus {
public:
	enum STATUSFLAG {
		NOTINITIALIZE, NOTINSTANTIATED, ALREADYINSTANTIATED, OBJECTCREATEFAIL, STARTTHREADFAIL,
		AUXOBJECTSTATED, AUXOBJECTCREATEFAIL, STARTMGRFAIL, STARTUNITFAIL, FUNCTIONLEAK, STOPTHREADFAIL,
		SENDMSGFAIL,
		STARTOK, STOPOK,
	};
	STATUSFLAG LogSvr;
	STATUSFLAG RbtMgr;
	STATUSFLAG OLnMgr;
	STATUSFLAG LTWMgr;
	STATUSFLAG LpMgr;
	STATUSFLAG IDRMgr;
	//STATUSFLAG E95BemSvr;
	STATUSFLAG DrLockMgr;

	HWND hRpyWnd;

	CMgrStatus() {
		hRpyWnd = NULL;

		LogSvr = STATUSFLAG::NOTINITIALIZE;
		RbtMgr = STATUSFLAG::NOTINITIALIZE;
		OLnMgr = STATUSFLAG::NOTINITIALIZE;
		LTWMgr = STATUSFLAG::NOTINITIALIZE;
		LpMgr = STATUSFLAG::NOTINITIALIZE;
		IDRMgr = STATUSFLAG::NOTINITIALIZE;
		//E95BemSvr = NOTINITIALIZE;
		DrLockMgr = STATUSFLAG::NOTINITIALIZE;
	}
	BOOL IsAllStartOK() {
		if (LogSvr != STATUSFLAG::STARTOK) {
			return FALSE;
		}
		if (RbtMgr != STATUSFLAG::STARTOK) {
			return FALSE;
		}
		if (OLnMgr != STATUSFLAG::STARTOK) {
			return FALSE;
		}
		if (LTWMgr != STATUSFLAG::STARTOK) {
			return FALSE;
		}
		if (LpMgr != STATUSFLAG::STARTOK) {
			return FALSE;
		}
		if (IDRMgr != STATUSFLAG::STARTOK) {
			return FALSE;
		}
		//if (E95BemSvr != STARTOK)
		//{
		//	return FALSE;
		//}
		if (DrLockMgr != STATUSFLAG::STARTOK) {
			return FALSE;
		}
		return TRUE;
	}
	BOOL IsAllStopOK() {
		if (LogSvr != STATUSFLAG::STOPOK) {
			return FALSE;
		}
		if (RbtMgr != STATUSFLAG::STOPOK) {
			return FALSE;
		}
		if (OLnMgr != STATUSFLAG::STOPOK) {
			return FALSE;
		}
		if (LTWMgr != STATUSFLAG::STOPOK) {
			return FALSE;
		}
		if (LpMgr != STATUSFLAG::STOPOK) {
			return FALSE;
		}
		if (IDRMgr != STATUSFLAG::STOPOK) {
			return FALSE;
		}
		//if (E95BemSvr != STOPOK)
		//{
		//	return FALSE;
		//}
		if (DrLockMgr != STATUSFLAG::STOPOK) {
			return FALSE;
		}
		return TRUE;
	}
};

class CGlobalDeviceContainer {
public:
	CEqpState EqpState;

	~CGlobalDeviceContainer();
	CGlobalDeviceContainer();

public:
	CLogFEM LogFile;
	CMgrStatus MgrStatus;

	COLnClt* pOLnClt = NULL;
	CObjectsContainer* pGEMObjs = NULL;
	CDataBank* pDataBnk = NULL;
	// 	COnlineDev * pOLnDev;
	CRbtMgr* pRbtMgr = NULL;
	CLogSvr* pLogSvr = NULL;
	CIDRMgr* pIDRMgr = NULL;
	CLPMgr* pLPMgr = NULL;
	CLTWMgr* pLTMgr = NULL;
	CDrLockMgr* pDrLockMgr = NULL;
	COnlineMgr* pOLnMgr = NULL;

	// 	BOOL bSimuEcho,bSimuDMC,bReverseAB,bReverseSignal;
	// 	BOOL bResetYFirst;
	// 	BOOL bSimuStage, bSimuX, bSimuY, bSimuZ1, bSimuZ2, bSimuP;
	// 	int nSpectroType; //added AP 20110113
	// 	BOOL bSimuVideo, bEchoCam, bThinFCam;

	// 	enum MOTORCTRLER
	// 	{
	// 		SICONTRLR, GALILCONTRLR
	// 	};
	//
	// 	MOTORCTRLER MotorController;
	// 	char IPAddress[32];
	//
	// 	BOOL bXYStage;
	// 	float backLash;
	// 	float backLashZ;
	//
	// 	short SiHubPort;
	// 	enum CAMERAID
	// 	{
	// 		ECHOCAM, THINFCAM, LASTCAM
	// 	};
	// 	short CamrID[LASTCAM];

	// 	short EchoPort;
	// 	short MCBoardNo;

	CString LogSrc;
	CString InterPaneMessage;
	// 	CMgrStatus MgrStatus;
	//COnlineMgr *m_OnlineMgr;

	// [04262022 ZHIMING
	BOOL bAutoStartLatching = FALSE;
	// ]

private:
	CSystemStatusDlg* m_SystemStatusDlg;
public:
	CString username;
	enum NMODEL		// Main model number [5/14/2012 Yuen]
	{
		MGENERIC, FSM413MOT, FSM413SA, FSM413C2C
	};
	NMODEL SystemModel;
	enum SMODEL	// Sub-model number [5/14/2012 Yuen]
	{
		SGENERIC, TSMC, ASE, SMIC, UMC
	};
	SMODEL SubModel;

	CArams AramsE95;

public:
	BOOL SetPostPos(short AlgnNum, long PostPos, long OffsetPos);
	BOOL ExecuteXFer(CString cmd);
	CString GetSubModelStr();
	CString GetModelStr();
	// 	void StartHardwareSystem(CListBox *pMessage);
	void ClearActiveSubstrate();
	void SetActiveSubstrate(int SubstNo);
	void SetActiveCtrlJob(const std::string& CtlJobID);
	void SetActivePrJob(const std::string& PrjobID);
	void ClearJobs();
	BOOL GetCurrentCJobID(string& Id);
	int GetActivePort();
	short GetActiveSubstrateNo();
	CSubstObj* GetActiveSubstrateObj();
	CCtrlJob* GetActiveCtrlJob();
	CPRJob* GetActivePrJob();
	BOOL GetCurrentPRJobID(string& Id);
	BOOL GetSubstLocList(vector<string>& Locs);
	BOOL GetAllSubstIDS(vector<string>& SubstIds);
	void GetCarrierIds(vector<string>& CIds);
	void GetAllCJobIDs(vector<string>& IdList);
	void GetAllPRJobIDs(vector<string>& IdList);
	CPRJob* GetPrjobObj(CString& PrJobID);
	int GetSubstLocAttr(const string& SubstLocID, const string& AttrID, SecsItemList* pList);
	int GetSubstAttr(const string& SubstID, const string& AttrID, SecsItemList* pList);
	BOOL IsSubstrateLoc(const string& SubstLocID);
	BOOL IsSubstrate(const string& SubstID);
	int CreateObject(const string& ObjSpec, const string& ObjType, map<string, SecsItem*>& Map, vector<int>& ErrCodes, string& ObCreated);
	BOOL IsCarrierPresent(const string& Carrier);
	void AddPortGroup(PortGroup& PGrp);
	int DeleteObject(string& ObjID);
	int GetPortID(const string& CarrierID);
	int GetAssociatedPorts(const string& PGrpName, vector<BYTE>& PortList);
	int PerformE87Service(const char* szCarrierID, const char* szAction, vector<string>& AttrIDs, vector<SecsItem*>& AttrValues, SecsItemList* pErrorList, BYTE PotID);
	int CanPerformService(string& Service, string& CarrierID, BYTE PortID, vector<string>& Attributes);
	BOOL GetCarrierID(const string& SubstID, CString& CarrierID);
	BOOL IsCarrierPresent(const char* Carrier);
	void SetActiveCJPJ(const std::string& CtlJobID, const std::string& PrjobID);
	void ClearActivePortSubstrate();
	void SetActivePortSubstrate(short PortID, short Slot);
	int CanPerformPortAction(int PortID, const string& actn);
	void ClearActivePort();
	void SetActivePort(short PortNo);
	void SaveSystemOption();
	void LoadSystemOption();
	// 	void SaveThinFilmOption();
	// 	void LoadThinFilmOption();
	// 	void SaveEchoOption();
	// 	void LoadEchoOption();
	// 	void SaveVideoOption();
	// 	void LoadVideoOption();
	// 	void SaveMotorOption();
	// 	void LoadMotorOption();
	// 	void SaveINI();
	// 	void LoadINI();
	BOOL TransferWafer(short FrStn, short FrSlot, short ToStn, short ToSlot, BOOL bAlgn);
	BOOL TransferWafer1(short FrStn, short FrSlot, short ToStn, short ToSlot, BOOL bAlgn);
	BOOL IsSA413();
	// 	void StopMgrs();
	// 	void StartMgrs(HWND hWnd, CListBox *pMessage);
	// 	CMgrStatus::STATUSFLAG StartRbtMgr();
	// 	CMgrStatus::STATUSFLAG StopRbtMgr();
	// 	CMgrStatus::STATUSFLAG StartRbtUnit();
	CMgrStatus::STATUSFLAG StartLogSvr();
	CMgrStatus::STATUSFLAG StopLogSvr();
	// 	CMgrStatus::STATUSFLAG StartLpMgr(HWND hWnd);
	// 	CMgrStatus::STATUSFLAG StopLpMgr();
	// 	CMgrStatus::STATUSFLAG StartLpUnits(HWND hWnd);
	// 	CMgrStatus::STATUSFLAG StopOnlineMgr();
	// 	CMgrStatus::STATUSFLAG StartOnlineMgr(CListBox *pMessage);
	// 	CMgrStatus::STATUSFLAG StopLTWMgr();
	// 	CMgrStatus::STATUSFLAG StartLTWMgr();
	// 	CMgrStatus::STATUSFLAG StartIDRMgr();
	// 	CMgrStatus::STATUSFLAG StopIDRMgr();
	// 	CMgrStatus::STATUSFLAG StartIDRUnits();
	CMgrStatus::STATUSFLAG StopE95BemSvr();
	CMgrStatus::STATUSFLAG StartE95BemSvr();

	CString GetTimeStamp();

	void SECSContent(BYTE* data, short len, BOOL bLcl);
	void SECSMessage(CString msg);
	BOOL WaferPresentCheck();
	BOOL AuthenticateUser(short& AuthenLevel, short UILevel = 1);
	void RegisterSystemStatus(CSystemStatusDlg* pDlg);
	int GGetARAMSState();
	CString GetARAMSStateStr();
	SArams::ARAMSSTATE GetARAMSState();
	void SetARAMSState(SArams::ARAMSSTATE state);
	void SetAlarm(CAlrmList::ALIDX  alidx, CString msg = "");
	void ClearAlarm(CAlrmList::ALIDX alidx);
	CString GetIDRBypassStr(short unit);
	CString GetIDRStatusStr(short unit);
	PrcsMgrServices::LPBSTATE GetLPState(short unit);
	void SetLPState(short unit, PrcsMgrServices::LPBSTATE state);
	void Initialize();
	void TriggerAlarm(short ALID);
	BOOL RegisterWithLogSvr(HWND hWnd);
	CString GetMessage();
	void SetMessage(CString msg);
	BOOL RegisterWinWithLPMgr(HWND hWnd, short ID);
	BOOL RegisterWinWithLPMgrEx(HWND hWnd, short ID);
	void UnRegisterWinWithLPMgrEx(HWND hWnd, short ID);
	BOOL RegisterWinWithIDRMgr(HWND hWnd, short ID);
	BOOL GetLPState(CLPXChDat& Xch);

	void ContinueDoorMonitor();
	void PauseDoorMonitor();
	void justTest();
	BOOL TransferTest(short FrStn, short FrSlot, short ToStn, short ToSlot, BOOL bAlgn);
	BOOL TransferTest2(short FrStn, short FrSlot, short ToStn, short ToSlot, BOOL bAlgn);
	// only for testing perpose

private:
	void Log(CString Msg, short Lvl = 5);
};

extern CGlobalDeviceContainer* pGDev;
