#pragma once
#include "413AppRingWidth.h"
#include <thread>
#include <vector>
#include <unordered_map>
#include "MPoint.h"
#include <fstream>
// 10152024 / Class added by Mahedi Kamal
class C413AppCWL : public C413AppRingWidth
{
private:
	static double CalculateMean(const std::vector<double>& values);
	static double CalculateStandardDeviation(const std::vector<double>& values, double mean);
	static double C413AppCWL::GetAverageValueWithStdDevCheck(LPVOID pParam);
	//double ReadINI(char* section, char* name, double defvalue, CString& inifile);
protected:
	C413Global* g;
	CRecipe413* pRcp;
public:
	C413AppCWL();
	~C413AppCWL();
	static CString GetResultFile(LPVOID pParam, float X, float Y); //20250313_A saving roughness data
	BOOL isFastScanning; // If true then XY stage will move fast without minimal 
	BOOL runningWaferScanCWLThread;
	BOOL monitor008;
	BOOL monitor030;
	BOOL monitor040; // 12262024 Mahedi Kamal
	//BOOL IsFocusScanInMeasurementRoutine;
	
	static UINT C413AppCWL::FocusScanCWL(LPVOID pParam); // 09042024 Mahedi Kamal
	static UINT C413AppCWL::QuickFocusScanCWL008(LPVOID pParam); // 09042024 Mahedi Kamal
	BOOL QuickFocusScanCWL008_(double curZpos, double deviationRange);
	static UINT C413AppCWL::FocusScanCWL030(LPVOID pParam); // 09042024 Mahedi Kamal
	static UINT C413AppCWL::QuickFocusScanCWL030(LPVOID pParam); // 09042024 Mahedi Kamal
	static UINT C413AppCWL::QuickFocusScanCWL030_(LPVOID pParam); // 09042024 Mahedi Kamal
	static UINT C413AppCWL::AutoFocusToCallibrate(LPVOID pParam); // 09042024 Mahedi Kamal
	static UINT C413AppCWL::MoveXYAndCollectCWLData(LPVOID pParam); // 09042024 Mahedi Kamal
	void FullWaferScan(CRecipe413* pRcp); //24102024/MORSALIN
	BOOL LineScan(double fiX, double fiY, std::vector<double> &tmp, bool frwrd); //24102024/MORSALIN
	void FilterScanData(int ind); //24102024/MORSALIN
	thread AreaScanThread;

	//Filtering //12102024/REZA
	//Despike filtering
	void DespikeFilter(vector<double>&);
	int DespikeWindowSize; //1D kernel, must be odd 
	std::vector<double>DespikeSignal; //contains signal after removal of spikes
	std::vector<double> copyData;
	vector<pair<int, double>> copyResult;

	//Gussian filtering 
	int GaussianKernelSize;  // Kernel size (should be odd)
	double GaussianSigma;  // Standard deviation of Gaussian, Smoothness
	std::vector<double> GaussianKernel(int, double);
	void GaussianFilter(std::vector<double>&);
	std::vector<double> GaussianSignal; //contains signal after guassian smoothing
	
	void SaveToCSV();
	void SaveToCSV2();
	void CollectDieInfo(CRecipe413* pRcp);
	void Cal_LTV_STIR(CRecipe413* pRcp);
	void ShowDieValue(CRecipe413* pRcp);
	void GenerateMap(CRecipe413* pRcp);
	void leastSquaresFit(const std::vector<pair<int, double>>& confocalVal, double& slope, double& intercept);
	double medianVal; // 10202025
	vector<double> medianAll;
	double minVal = 100.0;
	//void GetDieCoordinate();
	unordered_map<int, vector<float>> corners;
	//CDiceView m_cDiceWafer_V2;
	CXYPair dX, dY;
	POINT coord;
	float m_Radius;

	static UINT C413AppCWL::QuickFocusScanCWL030_MatchConstantK(LPVOID pParam);

	void LinearFit(vector<double> pdataf, double& slope, double& intercept); // 09242025
	float LSQFit(vector<double>& data); // 09242025
	BOOL GetCWL_030_040_AvgVal(const float& X, const float& Y, double& Val030, double& Val040); // 09232025 
	void removeAbnormalPoint(vector<double>& data); // 09232025
	BOOL IsMinimumRangeForNormalData(vector<double>& data, const double minRange); // 09232025
	BOOL IsMaximumRangeForNormalData(vector<double>& data, const double maxRange); // 09232025
	BOOL IsThicknessWarpMes(CRecipe413* pRcp, short* code); // check if recipe if for bow or thickness measurement
	BOOL CollectConfocalMeasurentData(double& confocalData030, double& confocalData040, CMPoint* pMP, BOOL bIsLineScan); // Gives point average data or line scan data depending on the parameter
	void Trimdata(vector<double>& Data); // Mahedi
};

