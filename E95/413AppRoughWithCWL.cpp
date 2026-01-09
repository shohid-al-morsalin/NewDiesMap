#include "stdafx.h"
#include "413AppRoughWithCWL.h"
#include "413/MotorS2.h"
#include "413/MotorL2.h"
#include "413App.h"
#include <algorithm>
#include<fstream>
#include<sstream>
#include<string>


C413AppRoughWithCWL::C413AppRoughWithCWL() {
    RoughStepSizeCWL = 0;
    RoughScanLineLenCWL = 0;
    IsRaCWL = FALSE; // Mahedi Kamal 12120224
    IsNmCWL = TRUE;//20250306 
    CString inifile;
    DosUtil.GetLocalCfgFile(inifile);
    RoughStepSizeCWL = ReadINI(_T("CWLROUGH"), _T("Step"), RoughStepSizeCWL, inifile);
    RoughScanLineLenCWL = ReadINI(_T("CWLROUGH"), _T("ScanLength"), RoughScanLineLenCWL, inifile);
    filterOn = ReadINI(_T("CWLROUGH"), _T("filterOn"), filterOn, inifile);
    SaveScanTestValue = ReadINI(_T("CWLROUGH"), _T("SaveScanTestValue"), SaveScanTestValue, inifile);
}


C413AppRoughWithCWL::~C413AppRoughWithCWL() {

}


BOOL C413AppRoughWithCWL::ReadCWLData(float X, float Y, float &roughRa, float &roughRms) {
	float startX = X - (RoughScanLineLenCWL / 2);
	float endX = X + (RoughScanLineLenCWL / 2);
    Sleep(1000);//20250317_2
	pMtr->MoveX(startX);
    vector<double>CWLData;

    // set speed// THIS IS THE MINIMUM SPEED
    pMtr->pSiMc->SetSpeed(1, 0.015);
    pMtr->pSiMc->SetSpeed(2, 0.015);
    //20250317_2-----
    //Sleep(1000);
    Sleep(3000);
    //-------------
    int stageMoved = -1;
    //auto end = std::chrono::high_resolution_clock::now();
    //auto start = std::chrono::high_resolution_clock::now();
    
    std::thread t1([&]() { // STAGE MOVE THREAD
        stageMoved = pMtr->MoveX(endX);
        }
    );

    std::thread t2([&]() { // CONFOCAL DATA COLLECTION THREAD
        while (stageMoved == -1) {
            double topCWLVal;// = GetAverageValueWithStdDevCheck(p413App);
            Sleep(1);
            p413App->pCWL->GetMeasuredValueMRO(1, topCWLVal);
            CWLData.push_back(topCWLVal * 1000000);
        }
    });


    //[ MOVE STAGE(t1) AND GET CONTENIUOUS CONFOCAL DATA(t2)
    t1.join();
    t2.join();
    //]
    //end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> duration = end - start;
    //double durationInus = duration.count() * 1000;

    // SET NORMAL SPEED
    pMtr->pSiMc->SetSpeed(1, pMtr->pMX->Param.Speed);
    pMtr->pSiMc->SetSpeed(2, pMtr->pMY->Param.Speed);

    std::ofstream file(GetResultFile(p413App, X, Y));
    if (!file.is_open()) {
     }


    // div data into um step
    double dataPerUM = CWLData.size() / (RoughScanLineLenCWL * 1000); // data per um
    int dataPerStep = ceil(dataPerUM * (RoughStepSizeCWL* 1000));
    vector<double>data;
    for (int i = 0; i+ dataPerStep < CWLData.size(); ) {
        int loop = dataPerStep;
        double avgCWL = 0.0;
        while (loop) {
            if(SaveScanTestValue) file << CWLData[i] << ",";
            avgCWL += CWLData[i];
            loop--; i++;
        }
        avgCWL /= dataPerStep;
        data.push_back(avgCWL);
    }
    if (SaveScanTestValue) file << "\n";

    CString msg; 
    msg.Format(" Roughness data read completed");
    p413App->Log(msg, 2);
 
    //trim first and last 50% of data...
    size_t numToRemove = static_cast<size_t>(data.size() * 0.25);
    data.erase(data.begin(), data.begin() + numToRemove);
    data.erase(data.end() - numToRemove, data.end());

    //despike signal 
    //
    if (filterOn) {
        DespikeFilter(data);

        //Gausian filter

        GaussianFilter(data);

        msg.Format(" Filter applied");
        p413App->Log(msg, 2);
    }

    // do linear fit
    double slope, intercept;
    LinearFit(data, startX, slope, intercept);
    double xpos;
    double avgData = 0.0;
    for (int i = 0; i < data.size(); i++) {
       // xpos = startX + i * 0.001;
        xpos = startX + i * p413App->RoughStepSizeCWL;
        data[i] = data[i] - intercept - (xpos * slope);
        avgData += data[i];
    }
    avgData /= data.size();
    // remove 10% data as outlier
    filterVector(data, avgData);

    // do linear fit
    LinearFit(data, startX, slope, intercept);
    avgData = 0;
    for (int i = 0; i < data.size(); i++) {
        //xpos = startX + i * 0.001;
        xpos = startX + i * p413App->RoughStepSizeCWL;
        data[i] = data[i] - intercept - (xpos * slope);
        avgData += data[i];
    }
    avgData /= data.size();

    //avg subtract
    for (int i = 0; i < data.size(); i++) {
        data[i] -= avgData;
    }

    //[ TO REMOVE UNUSUAL SIGNAL THAT I HAVE NOTICED FROM DATA VISUALZATION
    removeAbnormalSignal(data); // 12262024 Mahedi Kamal
    //]
    
    // calculate roughness
    msg.Format(" Roughness Calculation Started");
    p413App->Log(msg, 2);

    /*CString fileName;
    fileName.Format("Roughness calc data(25 trim)[wafer 3] at X=%f, Y=%f.csv", X, Y);
    std::ofstream file(fileName);
    if (!file.is_open()) {

    }*/

    float sa=0.0, sq=0.0;
    for (int i = 0; i < data.size(); i++) {
        sa += abs(data[i]);
        sq += (data[i] * data[i]);
        if(SaveScanTestValue) file << data[i] << ",";
    }
    if(SaveScanTestValue) file.close();

    roughRa = sa /= data.size();
    sq /= data.size();
    roughRms = sq = sqrt(sq);
    //20250306 ARIF =============
    if (p413App->IsNmCWL == FALSE) {
        roughRa *= 10;
        roughRms *= 10;
    }
    //===========

    msg.Format(" Roughness Calculation Completed");
    p413App->Log(msg, 2);
    return TRUE;
}

//[ 12262024 Mahedi Kamal
BOOL C413AppRoughWithCWL::IsMaximumRangeForNormalData(vector<double>& data, const double maxRange) {
    // if atlest 70% data less then maxRange return TRUE
    int eightyPercent = data.size() * 0.7;
    int count = 0;
    for (int i = 0; i < data.size(); i++) {
        if (data[i] < maxRange) {
            count++;
        }
    }
    if (count < eightyPercent) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}
//]
//[ // 12262024 Mahedi Kamal
BOOL C413AppRoughWithCWL::IsMinimumRangeForNormalData(vector<double>& data, const double minRange) {
    // if atlest 70% data greter then minRange return TRUE
    int eightyPercent = data.size() * 0.7;
    int count = 0;
    for (int i = 0; i < data.size(); i++) {
        if (data[i] > minRange) {
            count++;
        }
    }
    if (count < eightyPercent) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}
//]
// [// 12262024 Mahedi Kamal
// Mahedi kamal //  rough signal filter only for roughness
void C413AppRoughWithCWL::removeAbnormalSignal(vector<double>& data) {
    if (data.empty()) {
        return;
    }
    // do average, max, min
    double avg_ = 0, max_ = -999999999, min_ = 999999999;
    for (int i = 0; i < data.size(); i++) {
        avg_ += data[i];
        max_ = max(max_, data[i]);
        min_ = min(min_, data[i]);
    }avg_ /= data.size();

    // finding what should be the maximum value for normal signal
    double high = max_, low = avg_;
    double mid = high;
    for(int i=0; i<200; i++){
        mid = (high + low) / 2;
        if (IsMaximumRangeForNormalData(data, mid)) {
            high = mid;
        }
        else {
            low = mid;
        }
    }
    double normalMaximulValue = mid;


    // finding what should be the minimum value for normal signal
    high = avg_, low = min_;
    mid = high;

    for (int i = 0; i < 200; i++) {
        mid = (high + low) / 2;
        if (IsMinimumRangeForNormalData(data, mid)) {
            low = mid;
        }
        else {
            high = mid;
        }
    }
    double normalMinimulValue = mid;
    

    // remove the abnornal pick
    for (int i = 0; i < data.size(); i++) {
        if (data[i] > normalMaximulValue) {
            data[i] = normalMaximulValue;
        }
        else if (data[i] < normalMinimulValue) {
            data[i] = normalMinimulValue;
        }
    }

}

//]
void C413AppRoughWithCWL::filterVector( vector<double>& data,float avgData) {
    double percentageToRemove = 0.2;

     // Calculate the number of elements to remove
    size_t countToRemove = static_cast<size_t>(data.size() * percentageToRemove);

    // Create a vector of indices and their corresponding differences from avgData
    vector<pair<size_t, double>> differences;
    for (size_t i = 0; i < data.size(); ++i) {
        differences.emplace_back(i, fabs(data[i]-avgData));
    }

    // Sort by the difference in descending order
    sort(differences.begin(), differences.end(), [](const pair<size_t, float>& a, const pair<size_t, float>& b) {
        return a.second > b.second;
        });

    // Mark the indices to remove
    vector<bool> toRemove(data.size(), false);
    for (size_t i = 0; i < countToRemove; ++i) {
        toRemove[differences[i].first] = true;
    }

    // Create a new vector with the remaining elements while preserving order
    vector<double> filteredData;
    for (size_t i = 0; i < data.size(); ++i) {
        if (!toRemove[i]) {
            filteredData.push_back(data[i]);
        }
    }

    // Replace the original data with the filtered data
    data = filteredData;
}

void C413AppRoughWithCWL::LinearFit(const vector<double>& data, const float startX, double &slope, double &intercept) {
    int  n = data.size();
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

    // Calculate sums
    for (size_t i = 0; i < n ; ++i) {
        //double xpos = startX + i * 0.001;
        double xpos = startX + i * p413App->RoughStepSizeCWL;
        sumX += xpos;
        sumY += data[i];
        sumXY += xpos * data[i];
        sumX2 += xpos * xpos;
    }

    // Calculate slope (m) and intercept (c)
    double denominator = n * sumX2 - sumX * sumX;
   
    slope = (n * sumXY - sumX * sumY) / denominator;
    intercept = (sumY * sumX2 - sumX * sumXY) / denominator;
}

double C413AppRoughWithCWL::ReadINI(char* section, char* name, double defvalue, CString& inifile) {
    char str[256];
    sprintf(str, _T("%f"), defvalue);
    GetPrivateProfileString(section, name, str, str, sizeof(str), inifile);
    return atof(str);
}

void C413AppRoughWithCWL::DespikeFilter( vector<double>& data) {
    std::vector<double>DespikeSignal(data.size());

    int DespikeWindowSize = 3;
    int halfWindow = DespikeWindowSize / 2;

    for (int i = 0; i < data.size(); i++)
    {
        std::vector<double> window;
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            if (i + j >= 0 && i + j < data.size()) {
                window.push_back(data[i + j]);
            }
        }

        std::sort(window.begin(), window.end());
        DespikeSignal[i] = window[window.size() / 2];
    }

    data = DespikeSignal;
}

//std::vector<double> C413AppRoughWithCWL::GaussianKernel(int size, double sigma)
//{
//    std::vector<double> kernel(size);
//    double sum = 0.0;
//    int half = size / 2;
//
//    for (int i = 0; i < size; ++i) {
//        double x = i - half;
//        kernel[i] = exp(-(x  x) / (2  sigma  sigma)) / (sqrt(2  3.1416) * sigma);
//        sum += kernel[i];
//    }
//
//    // Normalize the kernel
//    for (int i = 0; i < size; ++i) {
//        kernel[i] /= sum;
//    }
//
//    return kernel;
//}
//
//std::vector<double>GaussianSignal;
//void GaussianFilter(std::vector<double>& DespikeSignal, int GaussianKernelSize, int GaussianSigma) //User will choose from Recipie
//{
//    //std::vector<double>GuassSignal(DespikeSignal.size());
//
//    std::vector<double> gaussianKernel = GaussianKernel(GaussianKernelSize, GaussianSigma);
//    int halfKernel = gaussianKernel.size() / 2;
//
//    for (size_t i = 0; i < DespikeSignal.size(); ++i) {
//        double sum = 0.0;
//
//        for (int j = -halfKernel; j <= halfKernel; ++j) {
//            int index = i + j;
//            if (index >= 0 && index < DespikeSignal.size()) {
//                sum += DespikeSignal[index] * gaussianKernel[j + halfKernel];
//            }
//        }
//        GaussianSignal.push_back(sum);
//    }
//
//}
