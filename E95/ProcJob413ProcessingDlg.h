#pragma once

#include "CamBase.h"
#include "ChartWnd.h"
#include "ImgWnd.h"
#include "SICam.h"
#include "WaferMap.h"
#include "MParam.h"
#include "ProcJobProcessingB.h"
#include "SRC/ResizableDialog.h"
#include "DiceMapTemp.h"

/////////////////////////////////////////////////////////////////////////////
// CProcJob413ProcessingDlg dialog

class CPeaks;
class C413Global;
class CEchoData;
class CInfoPanelMain;
class CMeParam;
class CData;
class CMPoint;
struct MParam;
class CPattern;

class CProcJob413ProcessingDlg : public CResizableDialog,
	public CProcJobProcessingB {
	BOOL bMonitor, bPause;
	BOOL bMeasureOn;
	BOOL bEchoMotorStarted;
	HANDLE hProcMonThread;

	short nCjName;
	int maxpointtime;
	clock_t lottime, wafertime, pointtime, dpTime;
	short code[MAXMEPASET * MaXPA];
	CString dwCjName[MAXMEPASET * MaXPA];

	C413Global* g;
	CRecipe413* pRcp;

	//short markerCount = 0;					//20220601/yukchiu

	//20220603/yukchiu
	CString MsgStr;

	//20220603/yukchiu
	float fX1Old;
	float fY1Old;
	float fX2Old;
	float fY2Old;
	float fX1New;
	float fY1New;
	float fX2New;
	float fY2New;

	//20220603/yukchiu /recalculate location
	float PrC;
	float PrS;
	float PrSx;
	float PrSy;

	// Construction
public:
	enum MEASURE1RET {
		erUNKNOWN,
		erMEMORY,
		erECHOMOTOR,
		erSTAGE,
		erROBOTOUT,
		erROBOTIN,
		erABORTING,
		erOK = 0
	};

	enum MEASURERET {
		meUNKNOWNERR,
		meMEMORYERR,
		meROBOTERR,
		meABORTED,
		meSTAGEERR,
		meCOMPLETED
	};

	short Show, FailCount;
	HANDLE m_hLiveThread1;
	HANDLE m_hLiveThread2;

public:
	CTime startTime; //20251030_Mohir
	void Abort();
	void SendWaferDataToHost(CRecipe413* pRcp, short SlotNo);
	void SendPointToHost(CRecipe413* pRcp, CMPoint* p, short SlotNo);
	// 	void SwitchCameraSetExposure(CCamBase::CAMERAID view, CRecipe413 *pRcp, int Exposure);
	BOOL ReloadRecipe();
 	BOOL SaveData(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID, BOOL bCompleted, CString WaferType, CString WaferSize, CString Station, CTime startTime, CTime endTime); //20251030_Mohir
	void UpdateReport(CMPoint* p);
	void SetupReportHeader();
	void UpdateUI(CMPoint* p);
	void UpdateUILocal(CMPoint* p);	//12012025/HAQUE/LTV STIR Measurement
	CRecipe413* GetCurRecipe();
	BOOL StopEchoMotor();
	BOOL StartEchoMotor();
	// 	BOOL CanTransfer2Cas(short Lp, short slot);
	// 	BOOL CanTransfer2Stage(short Lp, short slot);
	void Log(CString msg, short lvl = 5);

	void ClearList();
	void SetupColumn();
	void Message(CString msg, int lvl = 5);
	BOOL TransferStage2Cas(short slot);
	BOOL TransferCas2Stage(short slot, BOOL bVacOn);
	short Measure();
	MEASURE1RET MeasureOne();
	CString Generate2DMapBasePath(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID);
	void CopyMeasuredDataToSlot0(CRecipe413* pRcp);
	BOOL MeasureOneWafer(CRecipe413* pRcp, CString WaferID, short SlotNo);
	void StoreData();
	CString GetDiceMapFolderPath(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID);
	CString Get2DMapFolderPath(CRecipe413* pRcp, short SlotID, CString WaferID, CString LotID);
	int GetNextAvailableBmpNumber(const CString& folderPath);
	int GetNextAvailableBmpNumberForProbe(const CString& folderPath, const CString& probeName);
	CString GetNextAvailableBmpNameForProbe(const CString& folderPath, const CString& probeName);
	void FillWaferMapStats(CWaferMap& map, CRecipe413* pRcp);
	void StopDoMeasure();
	void StartDoMeasure();
	static DWORD WINAPI DoMeasurement(LPVOID pObj);
	void StopCamera();
	void StartCamera(CCamBase::CAMERAID No, CRecipe413* pRcp);

	void ClearAllMemory();
	void StoppingTool(BOOL bIsAbort); //  [9/12/2007 LYF]

	void ScanDataArrived(CEchoData* pScanData);
	void CarrierObjNotFoundEvent(CString& CarrierID, short port);
	BOOL MakeProcJobInfo(short ID, CSMOL* pSM);
	BOOL StartControlJob();
	void SetProcessingHalt(CMsgItem* pMsg);
	void SetProcessingFail(CMsgItem* pMsg);
	BOOL SetProcessingComplete(CMsgItem* pMsg);
	LRESULT OnMsgInQue(WPARAM wP, LPARAM lP);
	CWnd* GetThis();
	void ScrnToClnt(LPRECT lpRect);
	CWnd* GetDialogItem(UINT ID);
	LRESULT OnPJStart(WPARAM wP, LPARAM lP);	//  [9/5/2007 LYF]
	LRESULT OnCJAbort(WPARAM wP, LPARAM lP);	//  [9/5/2007 LYF]
	LRESULT OnCJStop(WPARAM wP, LPARAM lP);		//  [9/5/2007 LYF]

	CProcJob413ProcessingDlg(int IDD, CWnd* pParent = NULL);   // standard constructor
	~CProcJob413ProcessingDlg();

	// Dialog Data
		//{{AFX_DATA(CProcJob413ProcessingDlg)
	enum { IDD = IDD_PROCJOB413PROCESSING };
	CButton	m_cRecover;
	CStatic	m_cHeartBeat;
	CButton	m_cPauseVideo;
	CButton	m_cMonitor;
	CChartWnd m_cProfile;
	CImgWnd	m_cImgWnd;
	CListCtrl m_cList;
	CListCtrl m_cReport;
	CWaferMap m_cWaferMap;
	CSICam	m_cVideo;
	CButton	m_cClose;
	CButton m_cAbort;
	//}}AFX_DATA
	CXYPair dX, dY;
	POINT coord;
	CStatic m_cCoor;

	DiceMapTemp dlgDice;//20251219_Mahmudul
	BOOL bIsDiceType = FALSE;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcJob413ProcessingDlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProcJob413ProcessingDlg)
	afx_msg void OnAbort();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnIpcHelloack();
	afx_msg void OnIpcHellonak();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LRESULT OnPrcsProcessingstart(WPARAM wP, LPARAM lP);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPausevideo();
	afx_msg void OnMonitor();
	afx_msg void OnSwitchview();
	afx_msg void OnColumnclickReport(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void ReloadCassette();
	afx_msg void OnRecover();
	//}}AFX_MSG
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	LRESULT OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	BOOL CenteringWafer(float& Cx, float& Cy, float& Cr);
	void SwitchOnChuckVacuum(CRecipe413* pRcp);
	// 	void UpdateTargets();
	float DoBestValue(CData* d, short off, MParam* p, CMeParam* pMeP);
	BOOL DoEchoProbePositionMarkers(CRecipe413* pRcp, CPtrList* pMPList, CPattern* pPatt, short SlotNo);
	BOOL DoThinFilmPositionMarkers(CRecipe413* pRcp, CPtrList* pMPList, CPattern* pPatt, short SlotNo);
	void OnToErrorRecovery();
	short GetItemIndex(CString* pstr, short n);
	float DoMaths(CData* d, short off, MParam* p, CMeParam* pMeP);
	void DoMathCalculation(CRecipe413* pRcp, CPtrList* pMPList);
	void ConvertBadDataNegToBadData(CRecipe413* pRcp, CPtrList* pMPList);
	void DoMultiPointMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	BOOL DoThicknessBowWarpMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	void DoThinFilmMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	void DoTrimMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	void DoBevelMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	BOOL InitializeEchoMeasurement(CRecipe413* pRcp, BOOL& bDoNotMoveZProbe, CMPoint* p, MParam::PRBNUM& measurement);
	// [06022022/yukchiu / new version
	BOOL DoEchoProbePositionMarkers_v2(CRecipe413* pRcp, CPtrList* pMPList, CPattern* pPatt, short SlotNo);
	// 06072022/yukchiu / new version us echo probe for marker PR
	BOOL DoThinFilmPositionMarkers_v2(CRecipe413* pRcp, CPtrList* pMPList, CPattern* pPatt, short SlotNo);
	BOOL DoThicknessBowWarpMeasurement_v2(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	// 06132022/yukchiu / thinfilm measurement with marker
	void DoThinFilmMeasurement_v2(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	// 11222022 ZHIMING
	BOOL InitializeConfocalMeasurement(CRecipe413* pRcp, CMPoint* p, MParam::PRBNUM& measurement);
	// 20231227 ZHIMING
	BOOL DoConfocalMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	BOOL DoConfocalLocalMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo); //24102024/MORSALIN
	// Mahedi Kamal 03202024
	BOOL DoRoughnessMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo);
	// ]
	BOOL DoRingWidthMeasurement(CRecipe413* pRcp, CPtrList* pMPList, short SlotNo); // 08142024

	// Record an event in event log
	//void RecordEvent(CString strEvent);
	//20220603/yukchiu
	void RecalculateLocations(float fRefX1Old, float fRefY1Old, float fRefX2Old, float fRefY2Old,
		float fRefX1New, float fRefY1New, float fRefX2New, float fRefY2New);
	void Message(CListCtrl* pList, CString& msg, short lvl = 5);
	CString Get2DFolderFromC2CPath(const CString& c2cPath);
	int repeatCount; // 08152025
	CString FindNextAvailableBmpFile(const CString& folderPath);
	// 10162025 ZHIMING
	void Export3D();
	//std::vector<CImage> waferMapImages2D; //202511210_Mohir
//	std::vector<CImage> waferMapImages3D;//20251210_Mohir
	CString sAutoSavePath;
	BOOL IsConfocalSignalOK();
	CString m_strLastSavedC2CPath; // Store real C2C path
	
};
