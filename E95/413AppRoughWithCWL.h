#pragma once
#include "413AppCWL.h"
#include <thread>
#include <vector>
/// Class added by Mahedi Kamal
class C413AppRoughWithCWL : public C413AppCWL
{
private:
	double ReadINI(char* section, char* name, double defvalue, CString& inifile);
	void filterVector( vector<double>& data, float avgData);
	void LinearFit(const vector<double>& data, const float startX, double& slope, double& intercept);
	void DespikeFilter( vector<double>& data);
	void removeAbnormalSignal(vector<double>& data); // 12262024 Mahedi Kamal
	BOOL IsMinimumRangeForNormalData(vector<double>& data, const double minRange); // 12262024 Mahedi Kamal
	BOOL IsMaximumRangeForNormalData(vector<double>& data, const double minRange); // 12262024 Mahedi Kamal
	BOOL filterOn;
	
public:
	BOOL IsNmCWL; //20250306 ARIF
	BOOL IsRaCWL; // Mahedi Kamal 12120224
	float RoughStepSizeCWL; // Mahedi Kamal 12120224
	float RoughScanLineLenCWL; // Mahedi Kamal 12120224
	BOOL SaveScanTestValue;// ARIF
	C413AppRoughWithCWL();
	~C413AppRoughWithCWL();
	BOOL ReadCWLData(float X, float Y, float& roughRa, float& roughRms);
	int thkSrchNum =5, warpSrchNum=5; //10032025_3
	float stepSrchNum = 0.5;
	BOOL isCaliBAutoFoucs = FALSE;
};

