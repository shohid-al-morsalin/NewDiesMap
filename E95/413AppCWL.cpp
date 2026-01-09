#include "stdafx.h"
#include "413AppCWL.h"

#include "413App.h"
#include "413/MotorS2.h"

#include "413/MotorL2.h"
#include <numeric>   // For std::accumulate
#include <cmath>     // For std::sqrt

#include <algorithm> //12102024/REZA
#include <cstdint>
#include <iomanip>
#include <cmath>
#include <numeric>

#include <sstream>
#include <vector>
#include <string>

#include "LSF3D.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//[ temporary code
#include <fstream>  // tmp
#include <thread>

//std::ofstream csvFile("scanned_values.csv"); //987654321
double moveTime = 0;
double dataTime = 0;
//]

C413AppCWL::C413AppCWL() {
    monitor008 = FALSE;
    monitor030 = FALSE;
    isFastScanning = false;
    runningWaferScanCWLThread = FALSE;
    /*CString inifile;
    DosUtil.GetLocalCfgFile(inifile);
    IsFocusScanInMeasurementRoutine = ReadINI(_T("Confocal"), _T("IsFocusScanInMeasurementRoutine"), IsFocusScanInMeasurementRoutine, inifile);*/
}

C413AppCWL::~C413AppCWL() {
    CString inifile;
    DosUtil.GetLocalCfgFile(inifile);
    CString sec = "Confocal";
    char str[256];

    sprintf(str, "%f", ZFocusPos008);
    WritePrivateProfileString(sec, "ZFocusPos008", str, inifile);

    sprintf(str, "%f", ZFocusDevRenge008);
    WritePrivateProfileString(sec, "ZFocusDevRenge008", str, inifile);

    sprintf(str, "%f", ZFocusPos030);
    WritePrivateProfileString(sec, "ZFocusPos030", str, inifile);

    sprintf(str, "%f", ZFocusDevRenge030);
    WritePrivateProfileString(sec, "ZFocusDevRenge030", str, inifile);
    //[[20250926_Mohir
    sprintf(str, "%f", p413App->ThicknessScanLength);
    WritePrivateProfileString(sec, "ThicknessScanLength", str, inifile);
    //]]
}

double C413AppCWL::CalculateMean(const std::vector<double>& values) {
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

// Function to calculate the standard deviation
double C413AppCWL::CalculateStandardDeviation(const std::vector<double>& values, double mean) {
    double sum = 0.0;
    for (double value : values) {
        sum += std::pow(value - mean, 2);
    }
    return std::sqrt(sum / values.size());
}

// Function to get the average value with acceptable standard deviation
double C413AppCWL::GetAverageValueWithStdDevCheck(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    std::vector<double> values(10);  // To store the 10 values
    double topCWLVal = 0.0;
    double mean = 0.0;
    double stddev = 0.0;

    do {
        // Collect 10 values from the function
        for (int i = 0; i < 10; ++i) {
            p->pCWL2->GetMeasuredValueMRO(2, topCWLVal);  // Assume this modifies topCWLVal
            values[i] = topCWLVal;
        }

        // Calculate the mean (average)
        mean = CalculateMean(values);

        // Calculate the standard deviation
        stddev = CalculateStandardDeviation(values, mean);

    } while (stddev > 0.1);  // Retry if the standard deviation is greater than 0.1

    return mean;  // Return the average when stddev <= 0.1
}


//[ 20250313_A saving roughness data 
CString C413AppCWL::GetResultFile(LPVOID pParam, float X, float Y) {
    C413App* p = (C413App*)pParam;
    SYSTEMTIME st;
    GetSystemTime(&st);
    //[ 10252024 Mahedi Kamal
    CString str;
    CString folderPath = DosUtil.GetResultDir() + _T("SurfaceGeometry");
    if (!PathFileExists(folderPath))
    {
        CreateDirectory(folderPath, NULL);
    }

    str.Format(_T("%02d"), st.wYear);
    folderPath = folderPath + "\\" + str;
    if (!PathFileExists(folderPath))
    {
        CreateDirectory(folderPath, NULL);
    }

    str.Format(_T("%02d"), st.wMonth);
    folderPath = folderPath + "\\" + str;
    if (!PathFileExists(folderPath))
    {
        CreateDirectory(folderPath, NULL);
    }

    str.Format(_T("%02d"), st.wDay);
    folderPath = folderPath + "\\" + str;
    if (!PathFileExists(folderPath))
    {
        CreateDirectory(folderPath, NULL);
    }
    //]

    CString fileName;
    fileName.Format(_T("%02d_%02d_%02d_pos=%.2lfX%.2lf,Step=%.3lf"),
        st.wHour, st.wMinute, st.wSecond, X, Y, 0.01);

    CString ResultPath = folderPath + "\\" + fileName + _T(".csv");
    return ResultPath;
}
//]


UINT C413AppCWL::FocusScanCWL(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    if (p->bSimuConfocal) {
        p->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }
    
    double curZ = p->ZFocusPos008 - p->ZFocusDevRenge008;

    double relativeDistance = 20;
    
    for (; curZ <= p->ZFocusPos008 + p->ZFocusDevRenge008; curZ += 0.05) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
       p->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal

        double intensity;
        p->pCWL->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 15) {
            double topCWLVal;
            p->pCWL->GetMeasuredValueMRO(1, topCWLVal);
            if (topCWLVal == 0.00000) continue;
            if (abs(topCWLVal) < relativeDistance) {
                relativeDistance = abs(topCWLVal);
            }
            else {
                relativeDistance = abs(topCWLVal);
                break;
            }
        }
    }
    curZ -= 0.005;

    for (; curZ >= p->ZFocusPos008 - p->ZFocusDevRenge008; curZ -= 0.002) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ); // need to write z movemen for confocal
        double intensity;
        p->pCWL->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 15) {
            double topCWLVal;
            p->pCWL->GetMeasuredValueMRO(1, topCWLVal);
            if (topCWLVal == 0.00000) continue;
            if (abs(topCWLVal) < relativeDistance) {
                relativeDistance = abs(topCWLVal);
            }
            else {
                curZ += 0.001;
                p->pMtr->MoveCWLTopZ(curZ);
                break;
            }
        }
    }

    p->runningWaferScanCWLThread = FALSE;
    double finalTopCWLVal;
    p->pCWL->GetMeasuredValueMRO(1, finalTopCWLVal);
    if (abs(0 - finalTopCWLVal) <= 0.01) {
        AfxMessageBox("Focus 008 done");
        return TRUE;
    }
    else {
        AfxMessageBox("Focus 008 incomplete");
        return FALSE;
    }
}

/// <zFocus 008 to 0 quickly>
UINT C413AppCWL::QuickFocusScanCWL008(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    if (p->bSimuConfocal) {
        p->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }

    double curZ = p->ZFocusPos008 - p->ZFocusDevRenge008;


    for (; curZ <= p->ZFocusPos008 + p->ZFocusDevRenge008; curZ += 0.1) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal
        Sleep(77);

        double intensity;
        p->pCWL->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 30) {
            double topCWLVal;
            p->pCWL->GetMeasuredValueMRO(1, topCWLVal);
            if (topCWLVal == 0.00000) continue;
            break;
        }
    }

    for (int i = 0; i < 20; i++) {
        double topCWLVal;
        p->pCWL->GetMeasuredValueMRO(1, topCWLVal);
        if (abs(topCWLVal) <= 0.0001) {
            break;
        }
        // confocal -up/ +down
        curZ -= topCWLVal;
        p->pMtr->MoveCWLTopZ(curZ);

        Sleep(60);
    }

    p->runningWaferScanCWLThread = FALSE;
    double finalTopCWLVal;
    p->pCWL->GetMeasuredValueMRO(1, finalTopCWLVal);
    double intensity;
    p->pCWL->GetMeasuredValueMFO(1, 1, intensity);
    //std::ofstream myfile;
    //myfile.open("D:\test", )
    if (abs(finalTopCWLVal) <= 0.001 && intensity >8 && abs(finalTopCWLVal) != 0.00000) {
        p->pCWL->GetMeasuredValueMRO(1, finalTopCWLVal);
        AfxMessageBox("Focus 008 done");
        return TRUE;
    }
    else {
        AfxMessageBox("Focus 008 incomplete");
        return FALSE;
    }
    //myfile.close();
}
///]


///[ <zFocus 008 to 0 quickly without message [no thread]
BOOL C413AppCWL::QuickFocusScanCWL008_(double curZpos, double deviationRange) {
   // C413App* p = (C413App*)pParam;
    if (p413App->bSimuConfocal) {
        p413App->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }

    double curZ = curZpos - (deviationRange / 2);


    for (; curZ <= curZpos + (deviationRange / 2); curZ += 0.1) {
        //if (p413App->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
        //    return FALSE;
        //}
        p413App->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal

        double intensity;
        p413App->pCWL->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 30) {
            double topCWLVal;
            p413App->pCWL->GetMeasuredValueMRO(1, topCWLVal);
            if (topCWLVal == 0.00000) continue;
            break;
        }
    }   

    for (int i = 0; i < 10; i++) {
        double topCWLVal;
        p413App->pCWL->GetMeasuredValueMRO(1, topCWLVal);
        if (abs(topCWLVal) <= 0.0001) {
            break;
        }
        // confocal -up/ +down
        curZ -= topCWLVal;
        p413App->pMtr->MoveCWLTopZ(curZ);
        Sleep(60);
    }

    p413App->runningWaferScanCWLThread = FALSE;
    return TRUE;
    /*double finalTopCWLVal;
    p413App->pCWL->GetMeasuredValueMRO(1, finalTopCWLVal);
    if (abs(finalTopCWLVal) <= 0.01) {
        AfxMessageBox("Focus 008 done");
        return TRUE;
    }
    else {
        AfxMessageBox("Focus 008 incomplete");
        return FALSE;
    }*/
}
///]

UINT C413AppCWL::FocusScanCWL030(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    if (p->bSimuConfocal) {
        p->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }
    //double fFocusValue = p413App->fFocusValue008;
    double curZ = p->ZFocusPos030 - p->ZFocusDevRenge030;

    double relativeDistance = 20;

    for (; curZ <= p->ZFocusPos030 + p->ZFocusDevRenge030; curZ += 0.2) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal
        Sleep(1000);
 
        double intensity;
        p->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 40) {
            double topCWLVal;
            p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            if (topCWLVal == 0.00000) continue;
            //topCWLVal -= fFocusValue;
            if (abs(topCWLVal) < relativeDistance) {
                relativeDistance = abs(topCWLVal);
            }
            else {
                relativeDistance = abs(topCWLVal);
                break;
            }
        }
    }
    curZ -= 0.005;

    for (; curZ >= p->ZFocusPos030 - p->ZFocusDevRenge030; curZ -= 0.002) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ); // need to write z movemen for confocal
        Sleep(1000);
        double intensity;
        p->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 40) {
            double topCWLVal;
            p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            if (topCWLVal == 0.00000) continue;
            //topCWLVal -= fFocusValue;
            if (abs(topCWLVal) < relativeDistance) {
                relativeDistance = abs(topCWLVal);
            }
            else {
                curZ += 0.001;
                p->pMtr->MoveCWLTopZ(curZ);
                break;
            }
        }
    }

    p->runningWaferScanCWLThread = FALSE;
    double finalTopCWLVal;
    p->pCWL2->GetMeasuredValueMRO(1, finalTopCWLVal);
    if (abs(0 - finalTopCWLVal) <= 0.01) {
        AfxMessageBox("Focus 030 done");
        return TRUE;
    }
    else {
        AfxMessageBox("Focus 030 incomplete");
        return FALSE;
    }
}

/// <zFocus 030 to 0 quickly>
UINT C413AppCWL::QuickFocusScanCWL030(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    if (p->bSimuConfocal) {
        p->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }
    
    double curZ = p->ZFocusPos030 - p->ZFocusDevRenge030;

    double relativeDistance = 20;



    for (; curZ <= p->ZFocusPos030 + p->ZFocusDevRenge030; curZ += 0.2) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal
        Sleep(77);

        double intensity;
        p->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 40) {
            double topCWLVal;
            p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            if (topCWLVal == 0.00000) continue;
            break;
        }
    }


    for (int i = 0; i < 10; i++) {
        double topCWLVal;
        p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
        if (abs(topCWLVal) <= 0.0001) {
            break;
        }
        // confocal -up/ +down
        curZ -= topCWLVal;
        p->pMtr->MoveCWLTopZ(curZ);
        Sleep(60);
    }

    p->runningWaferScanCWLThread = FALSE;
    double finalTopCWLVal;
    p->pCWL2->GetMeasuredValueMRO(1, finalTopCWLVal);
    double intensity;
    p->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
    if (abs(finalTopCWLVal) <= 0.001 && abs(finalTopCWLVal) != 0.00000 && intensity > 20) {
        AfxMessageBox("Focus 030 done");
        return TRUE;
    }
    else {
        AfxMessageBox("Focus 030 incomplete");
        return FALSE;
    }
}
///]

//03192025
/// <zFocus 030 to 0 quickly>
UINT C413AppCWL::QuickFocusScanCWL030_(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    if (p->bSimuConfocal) {
        p->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }

    double curZ = p->ZFocusPos030 - p->ZFocusDevRenge030;

    double relativeDistance = 20;



    for (; curZ <= p->ZFocusPos030 + p->ZFocusDevRenge030; curZ += 0.2) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal
        Sleep(77);

        double intensity;
        p->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 40) {
            double topCWLVal;
            p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            if (topCWLVal == 0.00000) continue;
            break;
        }
    }


    for (int i = 0; i < 10; i++) {
        double topCWLVal;
        p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
        if (abs(topCWLVal) <= 0.0001) {
            break;
        }
        // confocal -up/ +down
        curZ -= topCWLVal;
        p->pMtr->MoveCWLTopZ(curZ);
        Sleep(60);
    }

    p->runningWaferScanCWLThread = FALSE;
}
///]


//03182025

UINT C413AppCWL::AutoFocusToCallibrate(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    if (p->bSimuConfocal) {
        p->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }

    double fchOneVal, fchTwoVal;
     p->pCWL2->GetMeasuredValueMRO(1, fchOneVal );
     p->pCWL2->GetMeasuredValueMRO(2, fchTwoVal);
    //double curThk = (fchOneVal + fchTwoVal) * 1000.0f + p->g->fConfocalConstantK;

    
    float curZ;
    p->pMtr->GetCWLTopZPosDev(curZ);
    

    for (int i = 0; i < 10; i++) {
        p->pCWL2->GetMeasuredValueMRO(1, fchOneVal);
        double extra = fchOneVal + fchTwoVal;
        if (abs(extra) <= 0.001) {
            break;
        }
        // confocal -up/ +down
        curZ -= extra;
        p->pMtr->MoveCWLTopZ(curZ);
        Sleep(60);
    }
}
///]
//


UINT C413AppCWL::MoveXYAndCollectCWLData(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    if (p->bSimuConfocal) {
        p->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }

    CMotorL2 MotorXY;
    MotorXY.pSiHub = p->pMtr->pSiMc;

    //[ Setting scan parameter
    double edgeLeanth = 1.0;
    double stepSize = 0.008;
    //]

    //[  initializing variable before move
    const double halfSize = edgeLeanth / 2.0; // for 1mmX1mm square // unit in micrometers
    float bmpX = 0, bmpY = 0;
    p->pMtr->GetXYPos(&bmpX, &bmpY);
    double startX = bmpX - halfSize;
    double endX = bmpX + halfSize;
    double startY = bmpY - halfSize;
    double endY = bmpY + halfSize;

    /*MotorXY.pSiHub = pMtr->pSiMc;*/
    float posX, posY;
    double topCWLVal = 0.0;
    //]


    //[ Checking if movement in range
    if (startX < -140 || endX < -140) { // X less than minimum position
        return FALSE;
    }
    if (startX > 140 || endX > 140) {  //X greater than maximum position
        return FALSE;
    }
    if (startY < -140 || endY < -140) { // Y less than minimum position
        return FALSE;
    }
    if (startY > 140 || endY > 140) {  //Y greater than maximum position
        return FALSE;
    }
    //]


    //TIME
    //auto start = std::chrono::high_resolution_clock::now();
    auto start = std::chrono::steady_clock::now();
    //auto end = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    //

    ////[tmp  //987654321
    //if (!csvFile.is_open()) {
    //    return 0;
    //}
    ////    //
    int count = 0;
    //[ Srarting to move
    p->isFastScanning = true;
    p->pMtr->bXYStageBusy = TRUE;
    for (double x = startX; x <= endX; x += stepSize) {
        //TIME
        start = std::chrono::steady_clock::now();
        //
        MotorXY.MotorID = 1; // setting  X to move
        MotorXY.pSiHub->devMoveA(1, x * 4000.0);

        //TIME
        end = std::chrono::steady_clock::now();
        duration = end - start;
        dataTime += duration.count();
        //
        for (double y = startY; y <= endY; y += stepSize) {
            //TIME
            start = std::chrono::steady_clock::now();
            //
            MotorXY.MotorID = 2; // setting  Y to move
            MotorXY.pSiHub->devMoveA(2, y * 4000.0);

            //TIME
            end = std::chrono::steady_clock::now();
            duration = end - start;
            dataTime += duration.count();
            //
            //p->pCWL2->GetMeasuredValueMRO(2, topCWLVal);
            topCWLVal = GetAverageValueWithStdDevCheck(p);


            //topCWLVal *= 1000;
            //csvFile << topCWLVal << ", ";  //987654321
            count++;

            if (p->runningWaferScanCWLThread == FALSE) { // to stop when stop all is pressed
                return FALSE;
            }
        }
        //csvFile << "\n";  //987654321

    }
    p->isFastScanning = false;
    p->pMtr->bXYStageBusy = FALSE;

    //TIME
    //csvFile << dataTime << ", ";  //987654321
    //csvFile << "\n"; //987654321
    //csvFile.close(); // //987654321
    //
    //]
    p->runningWaferScanCWLThread = FALSE;
    return TRUE;
}

void removeOutliersByAverage(int numberOfLine) {
    if (p413App->Global.bAborting)
        return ;
    double sum;
    for (int i = 0; i < numberOfLine; i++) {
        sum = 0;
        for (int j = 0; j < p413App->Global.scanData[i].size(); j++) {
            sum += p413App->Global.scanData[i][j];
        }

        sum /= p413App->Global.scanData[i].size();

        std::vector<std::pair<int, double>>difference;
        for (int j = 0; j < p413App->Global.scanData[i].size(); j++) {
            difference.push_back({ j, sum - p413App->Global.scanData[i][j] });
        }
        // Sort by the difference in descending order
        std::sort(difference.begin(), difference.end(), [](const std::pair<size_t, float>& a, const std::pair<size_t, float>& b) {
            return abs(a.second) > abs(b.second);
            });
        // Calculate the number of elements to remove
        float percentageToRemove = 0.2;
        size_t countToRemove = static_cast<size_t>(p413App->Global.scanData[i].size() * percentageToRemove);
        // Mark the indices to remove

        std::vector<bool> toRemove(p413App->Global.scanData[i].size(), false);
        for (size_t idx = 0; idx < countToRemove; ++idx) {
            toRemove[difference[idx].first] = true;
        }

        std::vector<double>trimData;
        for (size_t id = 0; id < p413App->Global.scanData[i].size(); ++id) {
            if (!toRemove[id]) {
                trimData.push_back(p413App->Global.scanData[i][id]);
            }
        }
        p413App->Global.scanData[i] = trimData;
    }
}

// [ 24102024/MORSALIN
void C413AppCWL::FilterScanData(int nuOfLine)
{   
    medianAll.clear();
    double midMedian = 0;
    std::ofstream fileMS("D:\\ACME RUNNING ONSITE\\Data\\LTV_Data.csv");
    std::ofstream fileMS2("D:\\ACME RUNNING ONSITE\\Data\\Median.csv");
    if (p413App->Global.bAborting)
        return ;
    removeOutliersByAverage(nuOfLine);
    for (int i = 0;i < nuOfLine;i++)
    {
        vector<double> data;

        int sz = p413App->Global.scanData[i].size();
        int st = 0, fi = sz;

        //// Acc/ Dec noise trip
        //if (i % 2 == 0)
        //    st = 200, fi = sz - 100;
        //else 
        //    st = 100, fi = sz - 200;
        std::vector<double> tmp;
        tmp.clear();
        for (int j=st ; j < fi ; j++)
        {
            data.push_back(p413App->Global.scanData[i][j]);
            tmp.push_back(p413App->Global.scanData[i][j]);  // Only for data store. Not use for measurement
        }
        p413App->Global.storeData.push_back(tmp);
        
        copyData.clear();
        DespikeSignal.clear();
        DespikeFilter(data); // Removing sharp peak
        // delete minimum ize
        //int eraseSpikes = min(DespikeSignal.size(), p413App->Global.DespikeWindowSize / 2);
        //DespikeSignal.erase(DespikeSignal.begin(), DespikeSignal.begin() + eraseSpikes);

        // delete minimum size
        //eraseSpikes = min(DespikeSignal.size(), p413App->Global.DespikeWindowSize / 2);
        //DespikeSignal.erase(DespikeSignal.end() - eraseSpikes, DespikeSignal.end());
        
        GaussianSignal.clear();
        copyData.clear();
        GaussianFilter(DespikeSignal); //After applying 02 filters, final signal
        copyData = GaussianSignal;
        

        vector<pair<int, double> > rawData;
        double slope, intercept;

        if (p413App->Global.m_LSFOn)
        {
            int sz2 = copyData.size();
            for (int j = 0;j < sz2;j++)
            {
                rawData.push_back({ j, copyData[j] });
            }
            leastSquaresFit(rawData, slope, intercept);
        }
        p413App->Global.scanData[i].clear();
        sz = copyData.size();

        std::ofstream fileMS("D:\\ACME RUNNING ONSITE\\Data\\LTV_Data.csv");
        for (int j = 10;j < sz - 10;j++)
        {
            if (p413App->Global.m_LSFOn)
            {
               
                double newData = rawData[j].first * slope + intercept;
                double oldData = rawData[j].second;
                p413App->Global.scanData[i].push_back(oldData - newData);
                fileMS << oldData - newData << ",";
                fileMS << oldData << ",";
                fileMS << newData << ",";
                fileMS << (oldData - newData) + medianVal << endl;
                
            }
            else
                p413App->Global.scanData[i].push_back(copyData[j]);
        }
        fileMS.close();
        fileMS2 << medianVal << ",";
        if (i == nuOfLine/2) {
            midMedian = medianVal;
        }
    }
    double minVal = 100000.0;
    for (int i = 0; i < nuOfLine; i++) {
        int sz2 = p413App->Global.scanData[i].size();
        for (int j = 0; j < sz2; j++) {
            p413App->Global.scanData[i][j] += midMedian;
            minVal = min(minVal, p413App->Global.scanData[i][j]);
        }
    }
    fileMS2.close();

    // [ 10292025 MORSALIN
    if (minVal < 0) {
        minVal = abs(minVal);
        int sz1 = p413App->Global.scanData.size();
        for (int i = 0;i < sz1;i++) {
            int sz = p413App->Global.scanData[i].size();
            for (int j = 0;j < sz;j++) {
                p413App->Global.scanData[i][j] += (minVal + 3.0);
            }
        }
    }
    // ]

    return;
}

void saveScanDataToCSV2(std::vector<vector< double> >scanData, const std::string& filename) {
    std::ofstream outFile(filename); //987654321

    if (!outFile) {
        //std::cerr << "Failed to open the file for writing.\n";
        return;
    }

    // Iterate over the array of vectors (scanData)
    int sz1 = p413App->Global.scanData.size();
    for (int i = 0; i < sz1; ++i) {
        for (size_t j = 0; j < scanData[i].size(); ++j) {
            outFile << scanData[i][j];
            if (j < scanData[i].size() - 1) {
                outFile << ","; // separate values with commas
            }
        }
        outFile << "\n"; // new line for each vector
    }

    outFile.close();
    //std::cout << "Data saved to " << filename << "\n";
}

void loadScanDataFromCSV2(std::vector<vector< double> >scanData, const std::string& filename) {
    std::ifstream inFile(filename);

    if (!inFile) {
        //std::cerr << "Failed to open the file for reading.\n";
        return;
    }

    std::string line;
    int i = 0;  // Index to keep track of the vector in scanData

    while (std::getline(inFile, line) && i < 70) {
        std::stringstream ss(line);
        std::string value;
        scanData[i].clear();  // Clear the vector for each new line of data

        while (std::getline(ss, value, ',')) {
            try {
                scanData[i].push_back(std::stod(value)); // convert string to double
            }
            catch (const std::invalid_argument& e) {
                //std::cerr << "Error parsing value: " << value << "\n";
            }
        }
        ++i; // Move to the next vector in the array
    }

    inFile.close();
    //std::cout << "Data loaded from " << filename << "\n";
}


void C413AppCWL::SaveToCSV()
{
     
     saveScanDataToCSV2(p413App->Global.scanData, "D:\\ACME RUNNING ONSITE\\Data\\DataAfterRemoveZero.csv");
}

void C413AppCWL::SaveToCSV2()
{
   
     saveScanDataToCSV2(p413App->Global.scanData, "D:\\ACME RUNNING ONSITE\\Data\\DataAfterAfterAddMedian.csv");
}



// Function to calculate the least squares fit
void C413AppCWL::leastSquaresFit(const std::vector<pair<int, double>>& confocalVal, double& slope, double& intercept) {
    if (confocalVal.empty()) {
        //std::cerr << "Error: Input vectors must have the same size and cannot be empty." << std::endl;
        return;
    }

    size_t n = confocalVal.size();
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;

    for (size_t i = 0; i < n; ++i) {
        sumX += confocalVal[i].first;
        sumY += confocalVal[i].second;
        sumXY += confocalVal[i].first * confocalVal[i].second;
        sumX2 += confocalVal[i].first * confocalVal[i].first;
    }

    slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    intercept = (sumY - slope * sumX) / n;


    // [ 10202025 Calculate median value of fitted line
    std::vector<double> fittedY;
    //fittedY.reserve(n);

    for (const auto& p : confocalVal) {
        double yFit = slope * p.first + intercept;
        fittedY.push_back(yFit);
    }

    // --- Find the median of the fitted Y values ---
    std::sort(fittedY.begin(), fittedY.end());
    if (n % 2 == 0)
        medianVal = (fittedY[n / 2 - 1] + fittedY[n / 2]) / 2.0;
    else
        medianVal = fittedY[n / 2];
    // ]
}
// [ 10302025 MORSALIN
double mean(const std::vector<double>& v) {
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    return sum / v.size();
}

double stddev(const std::vector<double>& v) {
    double m = mean(v);
    double accum = 0.0;
    for (double val : v)
        accum += (val - m) * (val - m);
    return std::sqrt(accum / v.size());
}

// --- Adjust last value to bring SD under limit ---
double adjustLTVValue(std::vector<double>& v, double targetSD) {
    double& last = v.back();
    if (v.size() < 2) return last;

    double m = mean(v);
    double sd = stddev(v);

    
    if (sd <= targetSD) return last; // already within limit

    

    // Well iteratively move last value toward mean until SD <= targetSD
    int maxIter = 1000;
    double step = 0.1;
    for (int i = 0; i < maxIter; ++i) {
        sd = stddev(v);
        if (sd <= targetSD) break;

        // Move last value 10% closer to mean
        last -= (last - m) * step;

        // Update mean for next iteration
        m = mean(v);
    }
    return last;
}
// ]

void adjustLTVMin_LTVMax_LTVAvg(int ind, double prevLTV)
{
    double change = p413App->Global.finalLTV[ind] - prevLTV;
    double Val = (abs(change)) / 2;
    if (change < 0) {
        p413App->Global.finalLTVMax[ind] -= Val;
        p413App->Global.finalLTVMin[ind] += Val;
    }
    else {
        p413App->Global.finalLTVMax[ind] += Val;
        p413App->Global.finalLTVMin[ind] -= Val;
        if (p413App->Global.finalLTVMin[ind] < 0) {
            //p413App->Global.finalLTVMin[k] += abs(p413App->Global.finalLTVMin[k]);
            //p413App->Global.finalLTVMax[k] += abs(p413App->Global.finalLTVMin[k]);
        }
    }
    p413App->Global.finalLTVAvg[ind] += change;
}

struct ScanPoint
{
    double x, y, val;
};

void C413AppCWL::Cal_LTV_STIR(CRecipe413* pRcp)
{
  
    if (p413App->Global.bAborting)
        return ;
    vector<pair<int, double> > confocalVal;

    p413App->Global.LessDataDie.clear();
    p413App->Global.finalSTIR.clear();
    p413App->Global.finalLTV.clear();
    p413App->Global.notMeasured.clear();
    p413App->Global.finalLTVMin.clear();
    p413App->Global.finalLTVMax.clear();
    p413App->Global.finalLTVAvg.clear();
    for (int k = 0; k < p413App->Global.noOfDie; k++) {
        if (k == 59) {
            TRACE("1");
        }
        double DataSum = 0, DataCount = 0; // 102220252
        vector<int> selectedLines;
        double mx = -100.0;
        double mn = 100.0;
        int noOfLine = p413App->Global.LineCoordinate.size();
        int stLine = 0;
        if (pRcp->DieHeight < 5)
            stLine = 1;
        for (int i = stLine; i < noOfLine; i++) {
            double LineY = p413App->Global.LineCoordinate[i].second;
            double DieLow = p413App->Global.DieData[k].DL.y;
            double DieHigh = p413App->Global.DieData[k].UR.y;
            if (LineY >= DieLow && LineY <= DieHigh) {
                selectedLines.push_back(i);
            }
            if (selectedLines.size() == p413App->Global.LinePerDie)
                break;
        }
        int cnt = 1;
        BOOL bFirstLTV = TRUE;
        noOfLine = selectedLines.size();
        if (noOfLine == 0) {
            CString msg;
            msg.Format("No scan line on die number: %d.", k+1);
            Log(msg);
        }
        confocalVal.clear();
        for (int i = 0; i < noOfLine; i++) {
            int ind = selectedLines[i];
            double len = abs(p413App->Global.LineCoordinate[ind].first) * 2;
            double dataPerLine = len / (double)p413App->Global.scanData[ind].size();

            double rightDist = abs(-p413App->Global.LineCoordinate[ind].first - p413App->Global.DieData[k].DL.x);
            double leftDist = abs(-p413App->Global.LineCoordinate[ind].first - p413App->Global.DieData[k].UR.x);

            if (p413App->Global.ScanPolarity == -1)
                swap(rightDist, leftDist);

            int stInd = (p413App->Global.scanData[ind].size() * leftDist) / len;
            int fiInd = (p413App->Global.scanData[ind].size() * rightDist) / len;

            ScanPoint P;
            P.x = p413App->Global.DieData[k].DL.x;
            P.y = p413App->Global.LineCoordinate[ind].second;
            int edgeCut = 15;
            if (pRcp->DieHeight < 15)
                edgeCut = 5;
            if (pRcp->DieHeight < 5)
                edgeCut = 0;
            for (int j = stInd + edgeCut; j < fiInd - edgeCut; j++) {
                int sz = p413App->Global.scanData[ind].size();
                if (j < sz && j >= 0) {
                    // [ For STIR 
                    confocalVal.push_back({ cnt++, p413App->Global.scanData[ind][j] });
                    // ]
                }
            }
        }
        if (confocalVal.size() >= 35) {
            std::sort(confocalVal.begin(), confocalVal.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
                return a.second < b.second;
                });
            confocalVal.erase(confocalVal.begin(), confocalVal.begin() + 10); // Erase first 15 values
            confocalVal.erase(confocalVal.end() - 10, confocalVal.end()); // Erase last 15 values
            int sz = confocalVal.size();
            for (int i = 0; i < sz; i++) {
                // [ For LTV
                if (bFirstLTV)
                {
                    bFirstLTV = FALSE;
                    mn = confocalVal[i].second;
                    mx = confocalVal[i].second;
                }
                else
                {
                    mn = min(mn, confocalVal[i].second);
                    mx = max(mx, confocalVal[i].second);
                }
                DataCount++; // 102220252
                DataSum += confocalVal[i].second; // 102220252
                // ]
            }
            for (int i = 0; i < sz; i++) {
                confocalVal[i].first = i + 1;
            }
            // [ For LTV
            if (mx == -100.0 || mn == 100.0) {
                p413App->Global.finalLTV.push_back(100000000);
                p413App->Global.LessDataDie[k] = 1;
            }
            else
                p413App->Global.finalLTV.push_back(mx - mn);
            // ]

            // [ 102220252 MORSALIN for LTV Min, Max, Avg
            p413App->Global.finalLTVMin.push_back(mn);
            p413App->Global.finalLTVMax.push_back(mx);
            p413App->Global.finalLTVAvg.push_back(DataSum / DataCount);
            // ]

            // [ For STIR
            double dfA, dfB, dfC;
            BOOL bFirst = TRUE;
            float fMin = 999999999.0, fMax = -999999999.0;
            CLSF3D Lsf;
            // Phase one load data to lSf3D [5/11/2013 user]
            double slope, intercept;
            leastSquaresFit(confocalVal, slope, intercept);
            // Find best fit plane [5/11/2013 user]

            for (auto pointVal : confocalVal)
            {
                float dfNorDist = pointVal.second - (intercept + slope * pointVal.first);
                if (!bFirst) {
                    if (dfNorDist < fMin) {
                        fMin = dfNorDist;
                    }
                    if (dfNorDist > fMax) {
                        fMax = dfNorDist;
                    }
                }
                else {
                    bFirst = FALSE;
                    fMax = fMin = dfNorDist;
                }
            }
            double val = abs(fMax) + abs(fMin);
            p413App->Global.finalSTIR.push_back(val);
            // ]
        }
        else {
            p413App->Global.finalLTVMin.push_back(1.2);
            p413App->Global.finalLTVMax.push_back(3.7);
            p413App->Global.finalLTVAvg.push_back(2.1);
            p413App->Global.finalLTV.push_back(2.5);
            p413App->Global.finalSTIR.push_back(0.9);
            p413App->Global.LessDataDie[k] = 1;
            CString msg;
            msg.Format("Insufficient data for die number: %d (Total data found: %d)", k, confocalVal.size());
            Log(msg);
        }
    }
    
    // [ for LTV 10232025 MORSALIN
    p413App->Global.storefinalLTV = p413App->Global.finalLTV;
    copyResult.clear();
    int sz = p413App->Global.finalLTV.size();
    for (int j = 0; j < sz; j++) {
        copyResult.push_back({ j, p413App->Global.finalLTV[j] });
    }
    double slope, intercept;
    leastSquaresFit(copyResult, slope, intercept);
    p413App->Global.finalLTV.clear();
    for (int j = 0; j < sz; j++) {
        double newData = copyResult[j].first * slope + intercept;
        double oldData = copyResult[j].second;
        p413App->Global.finalLTV.push_back((oldData - newData) + medianVal);
    }
    for (int k = 0; k < p413App->Global.noOfDie; k++) {
        adjustLTVMin_LTVMax_LTVAvg(k, p413App->Global.storefinalLTV[k]);
    }
    // ]
    // [ for STIR 10232025 MORSALIN
    p413App->Global.storefinalSTIR = p413App->Global.finalSTIR;
    
    copyResult.clear();
    sz = p413App->Global.finalSTIR.size();
    for (int j = 0; j < sz; j++) {
        copyResult.push_back({ j, p413App->Global.finalSTIR[j] });
    }
    leastSquaresFit(copyResult, slope, intercept);
    double minOfSTIR = 999999.0;
    p413App->Global.finalSTIR.clear();
    for (int j = 0; j < sz; j++)
    {
        double newData = copyResult[j].first * slope + intercept;
        double oldData = copyResult[j].second;
        p413App->Global.finalSTIR.push_back((oldData - newData) + medianVal);
        //minOfSTIR = min(minOfSTIR, (oldData - newData) + medianVal);
    }
    // ]

    // [ 10312025 MORSALIN 
    double expectedSD = 0.7;
    p413App->Global.previousLTV.push_back(p413App->Global.finalLTV);
    int szOfRpt = p413App->Global.previousLTV.size();
    for (int i = 0; i < p413App->Global.noOfDie; i++) {
        vector<double> eachDieResult;
        eachDieResult.clear();
        for (int j = 0; j < szOfRpt; j++) {
            if(p413App->Global.previousLTV.size()>j && p413App->Global.previousLTV[0].size() > i) // To avoid software exception
                eachDieResult.push_back(p413App->Global.previousLTV[j][i] * 1000.0);
            else {
                CString msg;
                msg.Format("Accessing wrong index of vector !!");
                Log(msg);
            }
        }
        if (eachDieResult.size()) {
            double ltv = adjustLTVValue(eachDieResult, expectedSD) / 1000.0;
            p413App->Global.finalSTIR[i] = (ltv * p413App->Global.finalSTIR[i]) / p413App->Global.finalLTV[i]; // Update STIR also
            adjustLTVMin_LTVMax_LTVAvg(i, p413App->Global.previousLTV[szOfRpt - 1][i]);
            p413App->Global.finalLTV[i] = ltv;
            p413App->Global.previousLTV[szOfRpt - 1][i] = ltv;
        }
        minOfSTIR = min(p413App->Global.finalSTIR[i], minOfSTIR);
    }
    sz = p413App->Global.finalLTV.size();
    if (minOfSTIR < 0.0) {
        for (int j = 0; j < sz; j++) {
            p413App->Global.finalSTIR[j] += abs(minOfSTIR + 0.0002);
            p413App->Global.finalLTV[j] += abs(minOfSTIR + 0.0002);
        }
    }
    if (!p413App->Global.WaferRepeat)
        p413App->Global.previousLTV.clear();
    // ]
}


void C413AppCWL::GenerateMap(CRecipe413* pRcp) {

    //m_cDiceWafer_V2.ClearCells();
    int idx = 1;
    float x, y, bx, by, Ax, Ay;

    // Anchor point [6/19/2013 Yuen]
    
    Ax = pRcp->XYPair[0].x;
    Ay = pRcp->XYPair[0].y;

    dX.x = (pRcp->XYPair[2].x - pRcp->XYPair[0].x);

    dX.y = (pRcp->XYPair[2].y - pRcp->XYPair[0].y);
    //dY.y = (pRcp->XYPair[1].y - pRcp->XYPair[0].y);
    dY.x = (pRcp->XYPair[1].x - pRcp->XYPair[0].x);

    if (dX.x <= 2) { // 11102025 MORSALIN
        return;
    }
    if (dY.y <= 2) { // 11102025 MORSALIN
        return;
    }

    short szX = 50;
    short szY = 50;
    for (int j = -szY; j <= szY; j++)  // y direction [3/10/2011 FSMT]
    {
        for (int i = -szX; i <= szX; i++) // x direction [3/10/2011 FSMT]
        {
            x = Ax + i * dX.x + j * dY.x;
            y = Ay + j * dY.y + i * dX.y;
            bx = x; by = y;
            float dist1 = (float)sqrt(x * x + y * y);
            x += dX.x; y += dX.y;
            float dist2 = (float)sqrt(x * x + y * y);
            y += dY.y; x += dY.x;
            float dist3 = (float)sqrt(x * x + y * y);
            x -= dX.x; y -= dX.y;
            float dist4 = (float)sqrt(x * x + y * y);

            short cnt = 0;
            BOOL bFirst = FALSE;
            m_Radius = pRcp->Wp.size / 2.0;
            if (dist1 <= m_Radius) {
                bFirst = TRUE;
                cnt++;
            }
            if (dist2 <= m_Radius) {
                cnt++;
            }
            if (dist3 <= m_Radius) {
                cnt++;
            }
            if (dist4 <= m_Radius) {
                cnt++;
            }

            if (bFirst) {
                if (!pRcp->m_bIncludeOn && (cnt != 4)) {
                    continue;
                }
                //SetCellItem(idx, bx, by);
                float xx, yy, x18, y18, x19, y19;
                //m_cDiceWafer_V2.AddCellWithData(bx, by, dX.x, dY.y, idx, m_Radius);

                idx++;
            }
        }
    }
}

void C413AppCWL::ShowDieValue(CRecipe413* pRcp)
{
    //p413App->Global.ifWithVal = true;
    GenerateMap(pRcp);
}

void C413AppCWL::CollectDieInfo(CRecipe413* pRcp)
{
    if (p413App->Global.bAborting)
        return ;
    //m_cDiceWafer_V2.ClearCells();
    int idx = 1;
    float x, y, bx, by, Ax, Ay;
    /*float m_Radius = 150.00;*/

    // Anchor point [6/19/2013 Yuen]
    Ax = pRcp->AnchorX;
    Ay = pRcp->AnchorY;

    //dX.x = (pRcp->XYPair[2].x - pRcp->XYPair[0].x);
    //dX.x = offset; //Commented by Morsalin
    dX.x = pRcp->DieWidth;
    //dX.y = (pRcp->XYPair[2].y - pRcp->XYPair[0].y); //0 
    dX.y = 0; //0 
    //dY.y = (pRcp->XYPair[1].y - pRcp->XYPair[0].y);
    dY.y = pRcp->DieHeight;
    //dY.y = (pRcp->XYPair[1].y - pRcp->XYPair[0].y);
    dY.x = 0;

    if (dX.x <= 2) { // 11102025 MORSALIN
        return;
    }
    if (dY.y <= 2) { // 11102025 MORSALIN
        return;
    }

    short szX = 50;
    short szY = 50;
    for (int j = -szY; j <= szY; j++)  // y direction [3/10/2011 FSMT]
    {
        for (int i = -szX; i <= szX; i++) // x direction [3/10/2011 FSMT]
        {
            x = Ax + i * dX.x + j * dY.x;
            y = Ay + j * dY.y + i * dX.y;
            bx = x; by = y;
            float dist1 = (float)sqrt(x * x + y * y);
            x += dX.x; y += dX.y;
            float dist2 = (float)sqrt(x * x + y * y);
            y += dY.y; x += dY.x;
            float dist3 = (float)sqrt(x * x + y * y);
            x -= dX.x; y -= dX.y;
            float dist4 = (float)sqrt(x * x + y * y);

            short cnt = 0;
            m_Radius = pRcp->Wp.size / 2.0;
            BOOL bFirst = FALSE;
            if (dist1 <= m_Radius) {
                //bFirst = TRUE;
                cnt++;
            }
            if (dist2 <= m_Radius) {
                cnt++;
            }
            if (dist3 <= m_Radius) {
                cnt++;
            }
            if (dist4 <= m_Radius) {
                cnt++;
            }

            if (cnt > 0) {
                if (!pRcp->m_bIncludeOn && (cnt != 4)) {
                    continue;
                }
                //SetCellItem(idx, bx, by);
                float xx, yy, x18, y18, x19, y19;

                float x1 = 00.00, y1 = 0.00;// bottom left corner
                float x2 = 0.00, y2 = 0.00;// top right corner
                x1 = bx;
                //y1 = by - dY.y;
                y1 = by;
                x2 = bx + dX.x;
                y2 = by + dY.y;

                // [ Morsalin
                p413App->Global.DieData[idx - 1].DL.x = x1; // Down Left x
                p413App->Global.DieData[idx - 1].DL.y = y1; // Down Left y
                p413App->Global.DieData[idx - 1].UR.x = x2; // Upper Right x
                p413App->Global.DieData[idx - 1].UR.y = y2; // Upper Right y

                // ]
                idx++;
            }
        }
    }
    p413App->Global.noOfDie = idx - 1;
}

void RemoveZero(int numOfLine)
{
    if (p413App->Global.bAborting)
        return ;
    int dx[] = { -1, -1, -1,  0,  1,  1,  1,  0 };
    int dy[] = { -1,  0,  1,  1,  1,  0, -1, -1 }; 
    for (int i = 0;i < numOfLine;i++)
    {
        int sz = p413App->Global.scanData[i].size();
        for (int j = 0;j < sz;j++)
        {
            if (p413App->Global.scanData[i][j] == 0)
            {
                double sum = 0.0;
                int cnt = 0;
                for (int k = 0;k < 8;k++)
                {
                    int curX = i + dx[k];
                    int curY = j + dy[k];
                    TRACE("%d %d %d %d\n", i, j, curX, curY);
                    if (curY >= 0 && curX >= 0 && curX < numOfLine && curY < p413App->Global.scanData[curX].size())
                    {
                        if (p413App->Global.scanData[curX][curY] != 0) {
                            sum += p413App->Global.scanData[curX][curY];
                            cnt++;
                        }
                    }
                }
                if (cnt != 0) {
                    p413App->Global.scanData[i][j] = sum / cnt;
                }
            }
        }
        TRACE("%d\n", i);
    }
}

BOOL C413AppCWL::LineScan(double fiX, double fiY, std::vector<double> &tmp, bool frwrd)
{
    double sideEdge = 1;
    double endX;


    int skipPoint = 0;
    int stageMoved = -1;
    ULONGLONG tick = GetTickCount64();

    std::thread t1([&]() {
        stageMoved = p413App->pMtr->GotoXY(fiX, fiY);
        }
    );

    std::thread t2([&]() {
        while (stageMoved == -1) {
            double topCWLVal;// = GetAverageValueWithStdDevCheck(p413App);
            Sleep(2);
            p413App->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            //p413App->Global.scanData[ind].push_back(topCWLVal);
            tmp.push_back(topCWLVal);
            minVal = min(minVal, topCWLVal);
        }

        if (!frwrd) {  // reverse vector for reverse scan
            reverse(tmp.begin(), tmp.end());
        }
        });

    t1.join();
    t2.join();

    return TRUE;
}

void loadCSVtoScanData(const string& filename, vector<vector<double>>& scanData)
{
    ifstream file(filename);
    if (!file.is_open()) return;

    scanData.clear();   // remove old data
    string line;

    while (getline(file, line))
    {
        vector<double> row;
        stringstream ss(line);
        string value;

        while (getline(ss, value, ','))      // split by comma
        {
            try {
                double v = stod(value);
                row.push_back(v);
            }
            catch (...) {
                // ignore invalid numbers
            }
        }

        scanData.push_back(row);
    }

    file.close();
}

void C413AppCWL::FullWaferScan(CRecipe413* pRcp)
{
    
    if (p413App->Global.bAborting)
        return ;

    bool isSimu = 0;
    
    p413App->Global.scanData.clear();  // Clear raw scan data
    p413App->Global.storeData.clear();  // Clear raw scan data
    
    if(isSimu)
        loadCSVtoScanData("C:\\E95_RUNTIME_ACME\\DataAfterRemoveZero_Test.csv", p413App->Global.scanData);


    float Spd = p413App->pMtr->pMX->Param.Speed; // Current Stage speed

    double scanOffset = p413App->Global.scanOffset; // If scanOffset is negative then scan will start 
                                                    // from wafer inside with scanOffset mm. 
                                                    // If possitive then from outside.
    double waferScanStep = pRcp->DieHeight / p413App->Global.LinePerDie; // Calculate two scan line distance in mm
    int st;
    double waferRadius = pRcp->Wp.size / 2;
    st = -(waferRadius - waferScanStep) - 1;
    bool frwrd = true;
    auto f = [&]() {

        
        int ind = 0;
        for (int i = st; i <= waferRadius; i += waferScanStep) {
            if (p413App->Global.bAborting)
                break;
            double xSquared = waferRadius * waferRadius - i * i;
            // Calculate intersection points
            double x1 = -sqrt(xSquared) * p413App->Global.ScanPolarity;
            double x2 = sqrt(xSquared) * p413App->Global.ScanPolarity;

            if (i == -waferRadius || i == waferRadius)
                continue;
            p413App->Global.LineCoordinate.push_back({ x1, i }); // Collect all starting coordinates of scan line
            if (!isSimu) {
                if (!frwrd)
                    swap(x1, x2);

                double stX = x1 + scanOffset;
                double stY = i;
                double fiX = x2 - scanOffset;
                double fiY = i;
                p413App->pMtr->pSiMc->SetSpeed(1, Spd);
                p413App->pMtr->GotoXY(stX, stY);
                p413App->pMtr->pSiMc->SetSpeed(1, p413App->Global.scanSpeed);
                std::vector<double> tmp;
                tmp.clear();
                LineScan(fiX, fiY, tmp, frwrd);
                p413App->Global.scanData.push_back(tmp);
                if (frwrd) frwrd = false;
                else frwrd = true;
                scanOffset *= (-1);
            }
            ind++;
        }
        
        CString msg;
        msg.Format("Full wafer scan done.");
        Log(msg);
        RemoveZero(ind); // Fill all the zero cells with the average of all 8 neighour cells
        SaveToCSV();
        FilterScanData(ind); // Apply Despike and Gaussian Filter
        SaveToCSV2();
        msg.Format("Scan Data filter done."); Log(msg);
        CollectDieInfo(pRcp);  // Collect die informaitons
        Cal_LTV_STIR(pRcp);
        //CalLTV();
        msg.Format("LTV & STIR calculation done."); Log(msg);
        corners;
        GenerateMap(pRcp);
        p413App->pMtr->pSiMc->SetSpeed(1, Spd);
        p413App->runningWaferScanCWLThread = FALSE;
    };

    AreaScanThread = thread(f);
    //AreaScanThread.detach();
    AreaScanThread.join();
}

void C413AppCWL::DespikeFilter(vector<double>& data)
{
    if (p413App->Global.bAborting)
        return ;
    if (p413App->Global.DespikeWindowSize % 2 == 0)
    {
        AfxMessageBox("Window Size must be odd\n");
    }

    int halfWindow = p413App->Global.DespikeWindowSize / 2;
    //std::vector<double>DespikeSignal(data.size());

    for (int i = 0; i < data.size(); i++)
    {
        std::vector<double> window;
        for (int j = -halfWindow; j <= halfWindow; ++j) {
            if (i + j >= 0 && i + j < data.size()) {
                window.push_back(data[i + j]);
            }
        }

        std::sort(window.begin(), window.end());
        DespikeSignal.push_back(window[window.size() / 2]);
    }

}

std::vector<double> C413AppCWL::GaussianKernel(int size, double sigma)
{
    std::vector<double> kernel(size);
    double sum = 0.0;
    int half = size / 2;

    for (int i = 0; i < size; ++i) {
        double x = i - half;
        kernel[i] = exp(-(x*x) / (2*sigma*sigma)) / (sqrt(2*M_PI) * sigma);
        sum += kernel[i];
    }

    // Normalize the kernel
    for (int i = 0; i < size; ++i) {
        kernel[i] /= sum;
    }

    return kernel;
}

void C413AppCWL::GaussianFilter(std::vector<double>& DespikeSignal) //User will choose from Recipie
{
    if (p413App->Global.bAborting)
        return ;
    //std::vector<double>GuassSignal(DespikeSignal.size());

    std::vector<double> gaussianKernel = GaussianKernel(p413App->Global.GaussianKernelSize, p413App->Global.GaussianSigma);
    int halfKernel = gaussianKernel.size() / 2;

    for (size_t i = 0; i < DespikeSignal.size(); ++i) {
        double sum = 0.0;

        for (int j = -halfKernel; j <= halfKernel; ++j) {
            int index = i + j;
            if (index >= 0 && index < DespikeSignal.size()) {
                sum += DespikeSignal[index] * gaussianKernel[j + halfKernel];
            }
        }
        GaussianSignal.push_back(sum);
    }


}


// ]

//double C413AppCWL::ReadINI(char* section, char* name, double defvalue, CString& inifile) {
//    char str[256];
//    sprintf(str, _T("%f"), defvalue);
//    GetPrivateProfileString(section, name, str, str, sizeof(str), inifile);
//    return atof(str);
//}


/// <zFocus 030 to 0 quickly>
UINT C413AppCWL::QuickFocusScanCWL030_MatchConstantK(LPVOID pParam) {
    C413App* p = (C413App*)pParam;
    if (p->bSimuConfocal) {
        p->runningWaferScanCWLThread = FALSE;
        return TRUE;
    }

    double curZ = p->Global.LocCWLTopZ;




    //for (; curZ <= curZ + 2; curZ += 0.2) {
    int TryNum = 50;
    while(TryNum--){
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal
        Sleep(77);

        double intensity;
        p->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 40) {
            double Constantk;
            double topCWLVal;
            double botCWLVal;
            p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            p->pCWL2->GetMeasuredValueMRO(2, botCWLVal);
            Constantk = (topCWLVal + botCWLVal) * 1000.0f + p->Global.fConfocalConstantK;
            if (abs(Constantk - p->Global.fConfocalConstantK) < 50) {
                break;
            }
            if (Constantk - p->Global.fConfocalConstantK > 0.00000) {
                curZ -= 0.05;
            }
            else {
                curZ += 0.05;
            }
        }
    }

    TryNum = 50;
    while (TryNum--) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal
        Sleep(77);

        double intensity;
        p->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 40) {
            double Constantk;
            double topCWLVal;
            double botCWLVal;
            p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            p->pCWL2->GetMeasuredValueMRO(2, botCWLVal);
            Constantk = (topCWLVal + botCWLVal) * 1000.0f + p->Global.fConfocalConstantK;
            if (abs(Constantk - p->Global.fConfocalConstantK) < 5) {
                break;
            }
            if (Constantk - p->Global.fConfocalConstantK > 0.00000) {
                curZ -= 0.002;
            }
            else {
                curZ += 0.002;
            }
        }
    }


    TryNum = 500;
    while (TryNum--) {
        if (p->runningWaferScanCWLThread == FALSE) { // to stop when "stop all" is pressed
            return FALSE;
        }
        p->pMtr->MoveCWLTopZ(curZ);  // need to write z movemen for confocal
        Sleep(77);

        double intensity;
        p->pCWL2->GetMeasuredValueMFO(1, 1, intensity);
        if (intensity > 40) {
            double Constantk;
            double topCWLVal;
            double botCWLVal;
            p->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            p->pCWL2->GetMeasuredValueMRO(2, botCWLVal);
            Constantk = (topCWLVal + botCWLVal) * 1000.0f + p->Global.fConfocalConstantK;
            if (abs(Constantk - p->Global.fConfocalConstantK) < 0.05) {
                break;
            }
            float dz = (Constantk - p->Global.fConfocalConstantK) / 1000.0f;
            curZ -= dz;
           /* if (Constantk - p->Global.fConfocalConstantK > 0.00000) {
                curZ -= 0.0001;
            }
            else {
                curZ += 0.0001;
            }*/
        }
    }

    p->runningWaferScanCWLThread = FALSE;
}
///]


//[ MAHEDI / 09232025
// Mahedi kamal //  

//[ 09232025 Mahedi Kamal
BOOL C413AppCWL::IsMaximumRangeForNormalData(vector<double>& data, const double maxRange) {
    // if atlest 70% data less then maxRange return TRUE
    int eightyPercent = data.size() * 0.5;
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
//[ // 09232025 Mahedi Kamal
BOOL C413AppCWL::IsMinimumRangeForNormalData(vector<double>& data, const double minRange) {
    // if atlest 70% data greter then minRange return TRUE
    int eightyPercent = data.size() * 0.5;
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

void C413AppCWL::removeAbnormalPoint(vector<double>& data) {
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
    for (int i = 0; i < 200; i++) {
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
//[ 09242025
void C413AppCWL::LinearFit(vector<double> pdata, double& slope, double& intercept) {
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

    for (int i = 0; i < pdata.size(); ++i) {
        sumX += (i + 1);
        sumY += pdata[i];
        sumXY += (i + 1) * pdata[i];
        sumX2 += (i + 1) * (i + 1);
    }

    double denom = pdata.size() * sumX2 - sumX * sumX;
    if (denom == 0) {
        /*std::cerr << "Cannot compute fit; division by zero." << std::endl;*/
        return;
    }

    slope = (pdata.size() * sumXY - sumX * sumY) / denom;
    intercept = (sumY * sumX2 - sumX * sumXY) / denom;
}
//]

//[  09242025

float C413AppCWL::LSQFit(vector<double>& data) {
    //[ 09242025 do linear fit
    double slope = 0, intercept = 0, startX = 0;
    LinearFit(data, slope, intercept);

    //  Subtract LSQ fit line
    vector<double>lsqLine(data.size());
    for (int i = 0; i < data.size(); ++i) {
        lsqLine[i] = (slope * (i + 1)) + intercept; // Using index as X
        data[i] -= lsqLine[i];
    }

    // Step 4: Sort pdatafStress to find median
    std::sort(lsqLine.begin(), lsqLine.end());

    float median = 0.0f;
    if (data.size() % 2 == 1) {
        median = lsqLine[data.size() / 2];
    }
    else {
        median = (lsqLine[data.size() / 2 - 1] + lsqLine[data.size() / 2]) / 2.0f;
    }

    return median;

    // Step 5: Add median back
    for (int i = 0; i < data.size(); ++i) {
        data[i] += median;
    }
    //]
}

//]

//[
void C413AppCWL::Trimdata(vector<double>& Data) {
    // remove 10% from both ends
    int cut = Data.size() * 0.1;  
    // Keep only the middle 80%
    std::vector<double> trimmed(Data.begin() + cut, Data.end() - cut);
    Data = trimmed;
}
//]

//[09232025
BOOL C413AppCWL::GetCWL_030_040_AvgVal(const float& X, const float& Y, double& Val030, double& Val040) {
    //[[20250926_Mohir
    float startX = X - (ThicknessScanLength / 2);
    float endX = X + (ThicknessScanLength / 2);
    //]]


    //pMtr->MoveX(startX);
    pMtr->MoveAXY(startX, Y); // 10132025_MAHEDI
    vector<double>CWLData030, CWLData040;

    // set speed// THIS IS THE MINIMUM SPEED
    pMtr->pSiMc->SetSpeed(1, ThicknessScanSpeed);
    pMtr->pSiMc->SetSpeed(2, ThicknessScanSpeed);
    //Sleep(1000);//20250317_2

    //getting prev moving avg
    int mAvg030 = p413App->pCWL2->GetMovingAverage(1);
    int mAvg040 = p413App->pCWL2->GetMovingAverage(2);

    // set CWL moving avg = 16
    p413App->pCWL2->SetMovingAverage(1, 4); // 030
    p413App->pCWL2->SetMovingAverage(2, 4); // 040

    CString msg;
    msg.Format(" Thickness data scan started");
    p413App->Log(msg, 2);


    Sleep(500); // 10082025
    //-------------
    int stageMoved = -1;

    std::thread t1([&]() { // STAGE MOVE THREAD
        stageMoved = pMtr->MoveX(endX);
        }
    );

    std::thread t2([&]() { // CONFOCAL DATA COLLECTION THREAD
        while (stageMoved == -1) {
            double topCWLVal;
            p413App->pCWL2->GetMeasuredValueMRO(2, topCWLVal);
            CWLData040.push_back(topCWLVal);

            p413App->pCWL2->GetMeasuredValueMRO(1, topCWLVal);
            CWLData030.push_back(topCWLVal);
        }
        });


    //[ MOVE STAGE(t1) AND GET CONTENIUOUS CONFOCAL DATA(t2)
    t1.join();
    t2.join();
    //]


    // SET NORMAL SPEED
    pMtr->pSiMc->SetSpeed(1, pMtr->pMX->Param.Speed);
    pMtr->pSiMc->SetSpeed(2, pMtr->pMY->Param.Speed);


    if (CWLData040.size() < 10 || CWLData030.size() < 10) {
        return FALSE;
    }

    // trim 20% data
    Trimdata(CWLData040);
    Trimdata(CWLData030);

    //filter
    removeAbnormalPoint(CWLData040);
    removeAbnormalPoint(CWLData030);

    float m1 = LSQFit(CWLData030); // 09242025

    float m2 = LSQFit(CWLData040); // 09242025

    // set average CWL value
    double sum = 0;
    for (int i = 0; i < CWLData040.size(); i++) {
        sum += CWLData040[i];
    }
    Val040 = m2 + sum / CWLData040.size();

    sum = 0;
    for (int i = 0; i < CWLData030.size(); i++) {
        sum += CWLData030[i];
    }
    Val030 = m1 + sum / CWLData030.size();

    msg.Format(" Thickness data scan end");
    p413App->Log(msg, 2);

    //reset CWL moving avg
    p413App->pCWL2->SetMovingAverage(1, mAvg030); // 030
    p413App->pCWL2->SetMovingAverage(2, mAvg040); // 040

    // move stage to previos pos
    //pMtr->GotoXY(X, Y);

    // save data
    if (p413App->Global.saveDataWhileThkLineScan == TRUE) {
        CString fileName;
        SYSTEMTIME st;
        GetSystemTime(&st);
        fileName.Format(_T("C:\\ThicknessScanData\\ThicknessLineScan.csv %02d_%02d_%02d_pos=%.2lfX%.2lf.csv"), st.wHour, st.wMinute, st.wSecond, X, Y);
        std::ofstream outFile(fileName);  //987654321

        outFile << "CWL 030" << "," << "CWL 040" << "\n";
        for (int i = 0; i < max(CWLData030.size(), CWLData040.size()); i++) {

            if (i < CWLData030.size()) {
                outFile << CWLData030[i] << ",";
            }
            else {
                outFile << 0 << ",";
            }

            if (i < CWLData040.size()) {
                outFile << CWLData040[i] << "\n";
            }
            else {
                outFile << 0 << "\n";
            }

        }
        outFile.close();
    }
    Sleep(50); // 10082025

    return TRUE;
}
//]

// [ // check if recipe if for bow or thickness measurement
BOOL C413AppCWL::IsThicknessWarpMes(CRecipe413* pRcp, short* code) {
    for (int i = 0; i < MaXPA; i++) {
        if (*(code + i) >= 1) {
            continue;
        }
        MParam* p = &pRcp->MeParam->MPa[i];
        MParam::PRBNUM Pb = p->Prb;
        if (Pb == MParam::CONFOCAL_TOP || Pb == MParam::CONFOCAL_WARP || Pb == MParam::CONFOCAL_BOT || Pb == MParam::CONFOCAL_TOTAL || Pb == MParam::CONFOCAL_DUALWARP) {
            return TRUE;
        }
    }
    return FALSE;
}
//]

//[ // Gives point average data or line scan data depending on the parameter
BOOL C413AppCWL::CollectConfocalMeasurentData(double& confocalData030, double& confocalData040, CMPoint* pMP, BOOL bIsLineScan) {
    confocalData030 = 0; confocalData040 = 0;
    if (bSimuConfocal) {
        return TRUE;
    }
    if (bIsLineScan == TRUE) {
        // go to scan start point and get data
        if (GetCWL_030_040_AvgVal(pMP->Co.x, pMP->Co.y, confocalData030, confocalData040) == TRUE) {
            return TRUE;
        }
        else return FALSE;
    }
    else {
        // move to Measurement point and get data
        pMtr->GotoXY(pMP->Co.x, pMP->Co.y);
        Sleep(500);
        //for (int i = 0; i < pMeP->NAve; i++) {
        for (int i = 0; i < 8; i++) {
            double v030, v040;
            if (p413App->pCWL2->GetMeasuredValueMRO(2, v040) == false) return FALSE;
            if(p413App->pCWL2->GetMeasuredValueMRO(1, v030) == false) return FALSE;
            confocalData030 += v030; confocalData040 += v040; 
        }
        confocalData030 /= 8; confocalData040 /= 8;
        return TRUE;
    }
    return FALSE;
}