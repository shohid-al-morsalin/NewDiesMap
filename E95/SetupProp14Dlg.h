#pragma once

#include "SetupPropComm.h"
#include "ChartWnd.h"
#include "CWL.h"
#include "..\SRC\ResizableDialog.h"
#include <thread>
#include "DiceView.h"
#include "XYPair.h"
#include <unordered_map>
#include "WaferMap.h"
/////////////////////////////////////////////////////////////////////////////
// CSetupProp81Dlg dialog

class CCoor;
class CMPoint;
class CInfoPanelMain;

class CSetupProp14Dlg : public CResizableDialog, public CSetupPropComm {
	HWND m_hPE3; //  [ 10152024 3D View]
public:
	CSetupProp14Dlg(CWnd* pParent = NULL);   // standard constructor
	~CSetupProp14Dlg();

	enum { IDD = IDD_SETUPPROP14 };
	CListCtrl	m_cList;
	CListCtrl	m_cPoint;
	CStatic	m_cMessage;
	CChartWnd	m_cProfile;
	CXYPair dX, dY;
	POINT coord;
	CStatic m_cCoor;
	CSICam m_cVideo;
	CStatic m_cMouseMessage;
	float	Ave[MaXPA];
	//}}AFX_DATA
public:
	BOOL iRun;
	BOOL bRun;
	thread displacementIntensityPlot;
	void LocalToRecipe();
	void RecipeToLocal();

	void SetupColumnPos();
	void SetupColumnMessage();
	void Renumber();
	void Addzposition(CMPoint* pMP);
	void UpdatePoint(CCoor* p);
	void RelocateToTop(CCoor* p);
	void RelocateToBack(CCoor* p);
	short sign(long value);

	void plotIntensity(CChartWnd* pProfile);//Tushar
	CCWLBase* pCWL;//Tushar
	CWaferMap	m_cWaferMap;//Tushar
	float m_ZPos;
	float m_ZPos2; // for 008 in this softwae

public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void PostNcDestroy();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnSaverecipe();
	afx_msg void OnMotioncontrol();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedMeasureCwl();
	afx_msg void OnBnClickedCalibrateCwl();

	void GetConfocalTotalThickness();
	afx_msg void ChOneDisplamentModeBtn();
	afx_msg void ChTwoDisplacementMode();
	afx_msg void ChOneIntensityBtn();
	afx_msg void ChTwoIntensityBtn();
	afx_msg void OnBnClickedConfocalZHeight();
	afx_msg void OnKillfocusConfocalZPosEdit();
	afx_msg void SetChannelOneZeroBtn();
	afx_msg void SetChannelTwoZeroBtn();
	afx_msg void OnBnClickedResponse();
	afx_msg void OnBnClickedPresence();
	//[03092024
	CEdit m_DieHeight, m_DieWidth;
	CEdit m_Sz;
	//CEdit m_Offset;
	afx_msg void OnBnClickedInsert();
	CString DieHeight;
	CString DieWidth;
	float mDieHeight = 25;
	float mDieWidth = 25;
	CSiteItem* m_MarkerItems[3];
	CStatic	m_cPos[3];
	//]
	//[ //  Mahedi Kamal 10152024
	thread updateCWLVal;
	void updateConfocalValue();
	void updateConfocalValue030();
	void updateConfocalValue040(); // 12262024 Mahedi Kamal
	void offAllMonitor(); // 12262024 Mahedi Kamal
	afx_msg void OnEnChangeThicknessScanLengthDlg14(); //20250926_Mohir
	afx_msg void OnEnChangeApproxZPosConf008Dlg14();
	afx_msg void OnEnChangeApproxzposconf030dlg14();
	afx_msg void OnEnChangeFocusDevRange008Dlg14();
	afx_msg void OnEnChangeFocusdevrange030dlg14();
	afx_msg void OnBnClickedScanwaferdlg14();
	afx_msg void OnBnClickedFocus008dlg14();
	afx_msg void OnBnClickedFocus030dlg14();
	afx_msg void OnBnClickedStopalldlg14();
	afx_msg void OnBnClickedMonitor008();
	afx_msg void OnBnClickedMonitor030();
	//]

	//[ARIF 3D VIEW STARTS [ 10152024 3D View]
	float mnVal, mxVal;
	std::vector<std::vector<float>>data;
	std::vector<std::vector<float>>filterData;
	void Create3D();
	afx_msg void OnBnClickedPlotData();
	//]
	void CalLTV();
	void OnBnClickedBtnmeasure2();
	afx_msg void OnBnClickedSaveScanData();
	afx_msg void OnBnClickedScan();
	afx_msg void OnBnClickedRun();
	void GenerateMap();
	CStatic MROdata;
	CString MRO_data;

	BOOL LineScan(double fiX, double fiY, int ind, bool frwrd);
	thread AreaScanThread; // 10152024 surface line scan
	CDiceView m_cDiceWafer_V2;
	afx_msg void OnStnClickedDiceMap();
	float m_Radius;
	unordered_map<int, vector<float>> corners;
	/*afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();*/
	void ExecuteScan();
	//[[[ Mahedi Kamal 12120224
	afx_msg void OnChangeRoughStep();
	afx_msg void OnChangeRoughLineLen();
	afx_msg void OnBnClickedShowroughcwl();
	afx_msg void OnClickedRms();
	afx_msg void OnClickedRa();
	//]]]
	void CollectDieInfo();
	
	void SaveToCSV();
	void ShowDieValue();
	CEdit m_ScanStep;
	CEdit m_ScanOffset;
	CString ScanStep;
	CString ScanOffset;
	CEdit m_ScanSpeed;
	CString ScanSpeed;
	afx_msg void OnBnClickedUpdateIni();
	CMotorS2* pMtr;
	afx_msg void OnBnClickedStopScan();
	afx_msg void OnBnClickedCreatepoints();
	afx_msg void OnBnClickedIncludeCheck();
	afx_msg void OnBnClickedScanFilter();
	afx_msg void OnBnClickedLTVMin(); // 10312025_1 MORSALIN
	afx_msg void OnBnClickedLTVMax(); // 10312025_1 MORSALIN
	afx_msg void OnBnClickedLTVAvg(); // 10312025_1 MORSALIN
	afx_msg void OnBnClickedLTV(); // 10312025_1 MORSALIN
	afx_msg void OnBnClickedSTIR(); // 10312025_1 MORSALIN
	afx_msg void OnBnClickedCamerasettingdlg14();

	long OnMoMove(WPARAM wP, LPARAM lP);
	CEdit mBufferData;
	CEdit mOffsetData;
	afx_msg void OnBnClickedAdd();
	CString sBuffData;
	CString sOffsetData;
	float offsetData , buffData;
	long OnButClick(WPARAM wP, LPARAM lP);
	afx_msg void OnBnClickedLocations2dlg14();
	afx_msg void OnRefreshui();
	afx_msg void OnBnClickedTakepicture2dlg14();
	CButton m_ScanFilter, m_Include;
	CButton m_LTVMin, m_LTVMax, m_LTVAvg, m_LTV, m_STIR; // 10312025_1 MORSALIN
	float AnchorX, AnchorY;
//	void leastSquaresFit(const std::vector<pair<int, double>>& confocalVal, double& slope, double& intercept);
//	void SaveToCSVLocal();
//	//void SaveToCSV2LOcal();
//	void CollectDieInfo(CRecipe413* pRcp);
//	void Cal_LTV_STIR();
//
//	void FullWaferScan(CRecipe413* pRcp); //24102024/MORSALIN
//	BOOL LineScan(double fiX, double fiY, int ind, bool frwrd); //24102024/MORSALIN
//	void FilterScanData(int ind); //24102024/MORSALIN
//	thread AreaScanThread;
//
//	//Filtering //12102024/REZA
//	//Despike filtering
//	void DespikeFilter(vector<double>&);
//	int DespikeWindowSize; //1D kernel, must be odd 
//	std::vector<double>DespikeSignal; //contains signal after removal of spikes
//	std::vector<double> copyData;
//
//	//Gussian filtering 
//	int GaussianKernelSize;  // Kernel size (should be odd)
//	double GaussianSigma;  // Standard deviation of Gaussian, Smoothness
//	std::vector<double> GaussianKernel(int, double);
//	void GaussianFilter(std::vector<double>&);
//	std::vector<double> GaussianSignal; //contains signal after guassian smoothing
	afx_msg void OnBnClickedRoughScan();
	long OnAddpoint(WPARAM wP, LPARAM lP);
	long OnAddalgnpoint(WPARAM wP, LPARAM lP);
	afx_msg void OnBnClickedConfocalTopMotor();
	CEdit m_zPos030;
	CEdit m_zPos008;
	
	afx_msg void OnBnClickedMonitor040();
	
	afx_msg void OnBnClickedZposrough();
	void OnMark();
	int calculatePowerOf2(int value);
	int calculateValueofPow2(int pow);
	afx_msg void SetCWLMovingAvg008();
	afx_msg void SetCWLMovingAvg030();
	afx_msg void SetCWLMovingAvg040();
	afx_msg void OnBnClickedRecipeAutofocus();
	afx_msg void OnBnClickedRoughcwlnm();
	afx_msg void OnBnClickedRoughcwla();

	CString sThkNum;
	CString sWarpNum;
	int iThkNum, iWarpNum;
	float searchStepSize;
	
	
	CEdit thkNum;
	CEdit warpNum;
	//CEdit m_thk_scan_length; //20250926_Mohir
	
	afx_msg void OnEnChangeSearchStep();
	afx_msg void OnEnChangeThkSearch();
	afx_msg void OnEnChangeWarpSearch();
	CEdit srhStepSize;
	CEdit m_Thk030;
	afx_msg void OnBnClickedCalibAutofocus();
	afx_msg void OnBnClickedThicknessFirstScan();
	CButton m_calibAutoFocus;
	CButton m_thkFirstScan; // 09232025_Shamim
	afx_msg void OnBnClickedThkScanTest(); // 09242025
	afx_msg void OnBnClickedGetCwlMovingavg008();
	afx_msg void OnBnClickedGetCwlMovingavg030();
	afx_msg void OnBnClickedGetCwlMovingavg040();
};
