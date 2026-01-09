#include "stdafx.h"
//#include <math.h>

#include "413/EchoScope/EchoProbeBase.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "MPoint.h"
#include "PosHt.h"
#include "ProcJob413ProcessingDlg.h"

#include "413AppBase.h"

// Search strategy, 49 positions [3/14/2012 Administrator]
/*
-7	-6	-5	-4	-3	-2	-1	0	1	2	3	4	5	6	7

7
6			*	*	*	*	*	*	*	*	*	*	*	*	*
5			*	*	*	*	*	*	*	*	*	*	*	*	*
4			*	*	*	*	*	*	*	*	*	*	*	*	*
3			*	*	*	*	*	*	*	*	*	*	*	*	*
2			*	*	*	*	*	*	*	*	*	*	*	*	*
1			*	*	*	*	*	*	*	*	*	*	*	*	*
0			*	*	*	*	*	*	*	*	*	*	*	*	*
-1			*	*	*	*	*	*	*	*	*	*	*	*	*
-2			*	*	*	*	*	*	*	*	*	*	*	*	*
-3			*	*	*	*	*	*	*	*	*	*	*	*	*
-4			*	*	*	*	*	*	*	*	*	*	*	*	*
-5			*	*	*	*	*	*	*	*	*	*	*	*	*
-6			*	*	*	*	*	*	*	*	*	*	*	*	*
-7
*/

/*
float gPX[] =
{
	0,  0,  1,  1,  1,
	0, 	-1, -1, -1, -1,
	0,  1,  2,  2,  2,
	2,  2,  1,  0, -1,
   -2, -2, -2, -2, -2,
   -2, -1,  0,  1,  2,
	3,  3,  3,  3,  3,
	3,  3,  2,  1,  0,
   -1, -2, -3, -3, -3,
   -3, -3, -3, -3,
};

// Search strategy, 49 positions [3/14/2012 Administrator]
float gPY[] =
{
	0,  1,  1,  0, -1,
	-1, -1,   0,  1,  2,
	2,  2,   2,  1,  0,
   -1, -2,  -2, -2, -2,
   -2, -1,   0,  1,  2,
	3,  3,   3,  3,  3,
	3,  2,   1,  0, -1,
   -2, -3,  -3, -3, -3,
   -3, -3,  -3, -2, -1,
	0,  1,   2,  3
};
*/
//20130828-2 SCOTT EDIT
float gPX[] =
{
	0,0,1,1,1,0,-1,-1,-1,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2,-2,-2,-1,0,
	1,2,3,3,3,3,3,3,3,2,1,0,-1,-2,-3,-3,-3,-3,-3,-3,-3,	-3,-2,-1,0,1,2,3,
	4,4,4,4,4,4,4,4,4,3,2,1,0,-1,-2,-3,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,
	-2,-1,0,1,2,3,4,5,5,5,5,5,5,5,5,5,5,5,4,3,2,1,0,-1,-2,-3,-4,-5,-5,-5,
	-5,-5,-5,-5,-5,-5,-5,-5,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,6,6,6,6,6,6,6,6,
	6,6,6,6,5,4,3,2,1,0,-1,-2,-3,-4,-5,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6
};

float gPY[] =
{
	0,1,1,0,-1,-1,-1,0,1,2,2,2,2,1,0,-1,-2,-2,-2,-2,-2,-1,0,1,2,3,3,3,3,
	3,3,2,1,0,-1,-2,-3,-3,-3,-3,-3,-3,-3,-2,-1,0,1,2,3,4,4,4,4,4,4,4,4,3,
	2,1,0,-1,-2,-3,-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,-2,-1,0,1,2,3,4,5,5,5,5,
	5,5,5,5,5,5,4,3,2,1,0,-1,-2,-3,-4,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,-5,
	-4,-3,-2,-1,0,1,2,3,4,5,6,6,6,6,6,6,6,6,6,6,6,6,5,4,3,2,1,0,-1,-2,-3,
	-4,-5,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6
};
//-----------
C413AppBase::C413AppBase() {
	pDMC = NULL;
	pMtr = NULL;
	pPSwitch = NULL; //02082024/MORSALIN
	// 	pOceanOp = NULL;
	pMCtrl = NULL;
	pEcho = NULL;
	//	pFFU = NULL;

	Exposure = 450;
	// 	PatOffX = PatOffY = 0;

	SRIT = 3.6982f;
	SRIB = 3.6982f;
	TRiCorr = SRIT / 3.5f;  // Constant [11/5/2011 Administrator]
	BRiCorr = SRIB / 3.5f;	// Constant [11/5/2011 Administrator]

	cfTop = DosUtil.GetEncoderFactor(1);
	cfBot = DosUtil.GetEncoderFactor(2);

	bSimuEcho = TRUE;
	bSimuDMC = TRUE;
	bReverseAB = FALSE;
	bReverseSignal = FALSE;
	bResetYFirst = TRUE;
	bSimuStage = bSimuX = bSimuY = bSimuEchoTopZ = bSimuThinFilmZ = bSimuP = TRUE;
	//bSimuVideo = FALSE; bEchoCam = FALSE; bThinFCam = FALSE;
	nSpectroType = 0;

	MotorController = GALILCONTRLR;
	strcpy(IPAddress, "192.168.0.1");

	EchoPort = 0;
	SiHubPort = 0;
	MCBoardNo = 0;

	backLashX = 0.03f;
	backLashY = 0.03f;
	backLashZ = 0.01f;

	rnd1 = 1;
	maxrnd1 = 96;
	rnd2 = 15;
	bLoadDIB = FALSE;

	// [04152024 ZHIMING
	pCWL = NULL;
	pRough = NULL;
	// [20240426 tushar
	//SiHubPort2 = 0;
	//bSimuEchoBottomZ = TRUE;
	// [05092024 ZHIMING
	bSimuRoughZ = TRUE;
	// ]
	
	pRingWidth = NULL; // 08132024 Mahedi Kamal
	ThicknessScanSpeed = 0.065; // 10242025_M
	LoadINI();
	SaveINI();
}

C413AppBase::~C413AppBase() {}

CString C413AppBase::GetTimeStamp() {
	CString str;
	str.Format("%s  ", CTime::GetCurrentTime().Format("%y/%m/%d %H:%M:%S"));
	return str;
}

CPtrList* C413AppBase::GetCurMPList(CRecipe413* pRcp) {
	if (!pRcp) {
		ASSERT(0);
		return NULL;
	}
	CPattern* pPatt = &pRcp->Patt;
	if (pPatt) {
		return &pPatt->MP.MPLst;
	}
	return NULL;
}

float C413AppBase::fRemoveFlyer(int n, float* pQ, int in, short MaxPkCnt) {
	double fSum = 0;
	double fAve = 0;
	float* p;

	// Calculate stdev [9/28/2011 Administrator]
	int i;

	// find mean [7/19/2013 Yuen]
	p = pQ + in;
	for (i = 0; i < n; i++) {
		fAve += *p;
		p += MaxPkCnt;
	}
	fAve /= double(n);

	// Find variance [7/19/2013 Yuen]
	p = pQ + in;
	for (i = 0; i < n; i++) {
		float v = (*p - fAve);
		fSum += v * v;
		p += MaxPkCnt;
	}
	double fStd = sqrt(fSum / double(n - 1));

	// Calculate average of values within 0.95 stdev [9/28/2011 Administrator]
	double mn = fAve - fStd * 0.95f;
	double mx = fAve + fStd * 0.95f;
	int m = 0;
	double fAve1 = 0;
	p = pQ + in;
	for (i = 0; i < n; i++) {
		if (((*p) >= mn) && ((*p) <= mx)) {
			fAve1 += *p;
			m++;
		}
		p += MaxPkCnt;
	}
	if (m > 1) {
		return fAve1 / double(m);
	}

	// Calculate average of values within 1.25 stdev [9/28/2011 Administrator]
	mn = fAve - fStd * 1.25f;
	mx = fAve + fStd * 1.25f;
	m = 0;
	fAve1 = 0;
	p = pQ + in;
	for (i = 0; i < n; i++) {
		if (((*p) >= mn) && ((*p) <= mx)) {
			fAve1 += *p;
			m++;
		}
		p += MaxPkCnt;
	}
	if (m > 1) {
		return fAve1 / double(m);
	}

	// Calculate average of values within 1.95X stdev [9/28/2011 Administrator]
	mn = fAve - fStd * 1.95f;
	mx = fAve + fStd * 1.95f;
	m = 0;
	fAve1 = 0;
	p = pQ + in;
	for (i = 0; i < n; i++) {
		if (((*p) >= mn) && ((*p) <= mx)) {
			fAve1 += *p;
			m++;
		}
		p += MaxPkCnt;
	}
	if (m) {
		return fAve1 / double(m);
	}
	return 0;
}

void C413AppBase::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "413App");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void C413AppBase::RemoveFlyerX(float* thk, short& cnt) {
	int n1, n2, i;
	float Max, Min, Ave, t;

	n1 = n2 = 0;
	Max = Min = Ave = *(thk);
	for (i = 1; i < cnt; i++) {
		t = *(thk + i);
		Ave += i;
		if (Max < t) {
			Max = t;
			n1 = i;
		}
		if (Min > t) {
			Min = t;
			n2 = i;
		}
	}
	if (fabs(Max - Ave) > fabs(Min - Ave)) {
		*(thk + n1) = *(thk + (cnt - 1));
	}
	else {
		*(thk + n2) = *(thk + (cnt - 1));
	}
	cnt--;
}

float C413AppBase::GetAverage(float* thk, short cnt) {
	float Ave = 0;
	for (int i = 0; i < cnt; i++) {
		Ave += *(thk + i);
	}
	return Ave / float(cnt);
}

float C413AppBase::GetRange(float* thk, short cnt) {
	float Max, Min;
	Max = Min = *thk;
	for (int i = 1; i < cnt; i++) {
		float v = *(thk + i);
		if (v > Max) {
			Max = v;
		}
		if (v < Min) {
			Min = v;
		}
	}
	return Max - Min;
}

void C413AppBase::Message(CListCtrl* pList, CString& str, short lvl) {
	Log(str, lvl);
	if (!pList) {
		return;
	}

	short idx = pList->InsertItem(0, CTime::GetCurrentTime().Format("%y:%m:%d %H:%M:%S"));
	pList->SetItemText(idx, 1, str);
	short len = pList->GetItemCount();
	if (len > 1024) {
		for (int i = 0; i < 1024 / 5; i++) {
			pList->DeleteItem(len - i);
		}
	}
	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void C413AppBase::Message(CListBox* pList, CString& str, short lvl) {
	Log(str, lvl);
	if (!pList) {
		return;
	}
	pList->InsertString(0, str);
	if (pList->GetCount() > 256) {
		for (int i = 0; i < 64; i++) {
			pList->DeleteString(pList->GetCount());
		}
	}
	MSG msg;
	while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

short C413AppBase::sign(long value) {
	if (value < 0) {
		return -1;
	}
	return 1;
}

short C413AppBase::sign(double value) {
	if (value < 0) {
		return -1;
	}
	return 1;
}

BOOL C413AppBase::ValidCheckRcp(CRecipe413& Rcp) {
	return TRUE;
}

BOOL C413AppBase::IsOperating() {
	for (int i = 0; i < 2; i++) {
		if (pUDev->pProcJob413ProcessingDlg[i]) {
			if (pUDev->pProcJob413ProcessingDlg[i]->bProcessing) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

void C413AppBase::LoadINI() {
	LoadEchoOption();
	LoadMotorOption();
	LoadVideoOption();
	LoadThinFilmOption();
	// [11212022 ZHIMING
	LoadConfocalOption();
	// [04242024
	LoadRoughOption();
	// ]
	LoadRingWidthOption(); // Mahedi Kamal 08132024
}

void C413AppBase::SaveINI() {
	SaveEchoOption();
	SaveMotorOption();
	SaveVideoOption();
	SaveThinFilmOption();
	// [11212022 ZHIMING
	SaveConfocalOption();
	// [04242024
	SaveRoughOption();
	// ]
	SaveRingWidthOption(); // Mahedi Kamal 08132024
}

void C413AppBase::LoadMotorOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Stage";

	GetPrivateProfileString(sec, "bSimu", "1", str, 255, inifile);
	bSimuStage = atoi(str);

	GetPrivateProfileString(sec, "ResetYFirst", "1", str, 255, inifile);
	bResetYFirst = atoi(str);

	GetPrivateProfileString(sec, "bSimuX", "1", str, 255, inifile);
	bSimuX = atoi(str);

	GetPrivateProfileString(sec, "bSimuY", "1", str, 255, inifile);
	bSimuY = atoi(str);

	/*GetPrivateProfileString(sec, "bSimuEchoTopZ", "1", str, 255, inifile);
	bSimuEchoTopZ = atoi(str);*/

	GetPrivateProfileString(sec, "bSimuThinFilmZ", "1", str, 255, inifile);
	bSimuThinFilmZ = atoi(str);

	GetPrivateProfileString(sec, "bSimuP", "1", str, 255, inifile);
	bSimuP = atoi(str);

	////[20240426 Tushar
	//GetPrivateProfileString(sec, "bSimuEchoBottomZ", "1", str, 255, inifile);
	//bSimuEchoBottomZ = atoi(str);
	// [05092024 ZHIMING
	GetPrivateProfileString(sec, "bSimuRoughZ", "1", str, 255, inifile);
	bSimuRoughZ = atoi(str);
	//]

	// [ 06112024/MORSALIN
	GetPrivateProfileString(sec, "bSimuTopCWLZ", "1", str, 255, inifile);
	bSimuTopCWLZ = atoi(str);
	GetPrivateProfileString(sec, "bSimuTopMicroscopeZ", "1", str, 255, inifile);
	bSimuEchoTopZ = atoi(str);
	// ]

	// 	GetPrivateProfileString(sec, "XYStage",  "1", str, 255, inifile);
	// 	bXYStage = atoi(str);

	GetPrivateProfileString(sec, "BacklashX", "0", str, 255, inifile);
	backLashX = atof(str);

	GetPrivateProfileString(sec, "BacklashY", "0", str, 255, inifile);
	backLashY = atof(str);

	GetPrivateProfileString(sec, "BacklashZ", "0", str, 255, inifile);
	backLashZ = atof(str);

	GetPrivateProfileString(sec, "MotorController", "0", str, 255, inifile);
	MotorController = (MOTORCTRLER)atoi(str);

	sec = "Ports";
	GetPrivateProfileString(sec, "SiHubPort", "0", str, 255, inifile);
	SiHubPort = atoi(str);

	GetPrivateProfileString(sec, "IPAddress", "192.168.0.1", str, 255, inifile);
	strncpy(IPAddress, str, 31);

	//[ 20240426 Tushar
	GetPrivateProfileString(sec, "SiHubPort2", "0", str, 255, inifile);
	SiHubPort2 = atoi(str);
	//]
}

void C413AppBase::SaveMotorOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Stage";

	sprintf(str, "%d", bSimuStage);
	WritePrivateProfileString(sec, "bSimu", str, inifile);

	sprintf(str, "%d", bResetYFirst);
	WritePrivateProfileString(sec, "ResetYFirst", str, inifile);

	sprintf(str, "%d", bSimuX);
	WritePrivateProfileString(sec, "bSimuX", str, inifile);

	sprintf(str, "%d", bSimuY);
	WritePrivateProfileString(sec, "bSimuY", str, inifile);

	sprintf(str, "%d", bSimuEchoTopZ);
	WritePrivateProfileString(sec, "bSimuEchoTopZ", str, inifile);

	sprintf(str, "%d", bSimuThinFilmZ);
	WritePrivateProfileString(sec, "bSimuThinFilmZ", str, inifile);

	sprintf(str, "%d", bSimuP);
	WritePrivateProfileString(sec, "bSimuP", str, inifile);
	
	// [20240426 Tushar
	/*sprintf(str, "%d", bSimuEchoBottomZ);
	WritePrivateProfileString(sec, "bSimuEchoBottomZ", str, inifile);*/
	sprintf(str, "%d", bSimuRoughZ);
	WritePrivateProfileString(sec, "bSimuRoughZ", str, inifile);
	// ]

	// 	sprintf(str,"%d", bXYStage);
	// 	WritePrivateProfileString(sec, "XYStage", str, inifile);

	sprintf(str, "%.4f", backLashX);
	WritePrivateProfileString(sec, "BacklashX", str, inifile);

	sprintf(str, "%.4f", backLashY);
	WritePrivateProfileString(sec, "BacklashY", str, inifile);

	sprintf(str, "%.4f", backLashZ);
	WritePrivateProfileString(sec, "BacklashZ", str, inifile);

	sprintf(str, "%d", MotorController);
	WritePrivateProfileString(sec, "MotorController", str, inifile);

	sec = "Ports";

	sprintf(str, "%d", SiHubPort);
	WritePrivateProfileString(sec, "SiHubPort", str, inifile);

	sprintf(str, "%s", IPAddress);
	WritePrivateProfileString(sec, "IPAddress", str, inifile);

	//[ 20240426 Tushar
	sprintf(str, "%d", SiHubPort2);
	WritePrivateProfileString(sec, "SiHubPort2", str, inifile);
	// ]

	// [ 06112024/MORSALIN
	sprintf(str, "%d", bSimuTopCWLZ);
	WritePrivateProfileString(sec, "bSimuTopCWLZ", str, inifile);
	sprintf(str, "%d", bSimuEchoTopZ);
	WritePrivateProfileString(sec, "bSimuTopMicroscopeZ", str, inifile);
	// ]

}

void C413AppBase::LoadVideoOption() {
	// 	char str[256];
	// 	CString inifile;
	//
	// 	DosUtil.GetLocalCfgFile(inifile);
	//
	// 	CString sec = "Video";
	//
	// 	GetPrivateProfileString(sec, "bSimu",  "1", str, 255, inifile);
	// 	bSimuVideo = atoi(str);
	//
	// 	GetPrivateProfileString(sec, "EchoCamera",  "1", str, 255, inifile);
	// 	bEchoCam = atoi(str);
	//
	// 	GetPrivateProfileString(sec, "ThinFilmCamera",  "1", str, 255, inifile);
	// 	bThinFCam = atoi(str);
}

void C413AppBase::SaveVideoOption() {
	// 	char str[256];
	// 	CString inifile;
	//
	// 	DosUtil.GetLocalCfgFile(inifile);
	//
	// 	CString sec = "Video";
	//
	// 	sprintf(str,"%d", bSimuVideo);
	// 	WritePrivateProfileString(sec, "bSimu", str, inifile);
	//
	// 	sprintf(str,"%d", bEchoCam);
	// 	WritePrivateProfileString(sec, "EchoCamera", str, inifile);
	//
	// 	sprintf(str,"%d", bThinFCam);
	// 	WritePrivateProfileString(sec, "ThinFilmCamera", str, inifile);
}

void C413AppBase::LoadEchoOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Ports";

	GetPrivateProfileString(sec, "Echo", "0", str, 255, inifile);
	EchoPort = atoi(str);

	sec = "Echo";

	GetPrivateProfileString(sec, "bSimu", "1", str, 255, inifile);
	bSimuEcho = atoi(str);

	GetPrivateProfileString(sec, "bSimuDMC", "1", str, 255, inifile);
	bSimuDMC = atoi(str);

	sec = "Echo";

	GetPrivateProfileString(sec, "ReverseAB", "0", str, 255, inifile);
	bReverseAB = atoi(str);

	GetPrivateProfileString(sec, "ReverseSignal", "0", str, 255, inifile);
	bReverseSignal = atoi(str);

	GetPrivateProfileString(sec, "MCBoardNo", "0", str, 255, inifile);
	MCBoardNo = atoi(str);
}

void C413AppBase::SaveEchoOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Ports";

	sprintf(str, "%d", EchoPort);
	WritePrivateProfileString(sec, "Echo", str, inifile);

	sec = "Echo";

	sprintf(str, "%d", bSimuEcho);
	WritePrivateProfileString(sec, "bSimu", str, inifile);

	sprintf(str, "%d", bSimuDMC);
	WritePrivateProfileString(sec, "bSimuDMC", str, inifile);

	sec = "Echo";

	sprintf(str, "%d", bReverseAB);
	WritePrivateProfileString(sec, "ReverseAB", str, inifile);

	sprintf(str, "%d", bReverseSignal);
	WritePrivateProfileString(sec, "ReverseSignal", str, inifile);

	sprintf(str, "%d", MCBoardNo);
	WritePrivateProfileString(sec, "MCBoardNo", str, inifile);
}

void C413AppBase::LoadThinFilmOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Spectrometer";

	GetPrivateProfileString(sec, "Type", "1", str, 255, inifile); //added AP 20110113
	nSpectroType = atoi(str);
}

void C413AppBase::SaveThinFilmOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Spectrometer";
	sprintf(str, "%d", nSpectroType);  //added AP 20110113
	WritePrivateProfileString(sec, "Type", str, inifile);
}

void C413AppBase::Yld(ULONGLONG dly) {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < dly) {
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void C413AppBase::DefinePeakPosition0(short n) {
	pEcho->SimuPkNum = n;

	CPosHt* pH;
	pH = &pEcho->PosT[n];
	pH->Clear();
	pH->Add(0.05f, 0, 1500, 0);
	pH->Add(0.1f, 0, 1800, 0);
	//pH->Add(0.895f, 0, 1500, 0);

	pH = &pEcho->PosB[n];
	pH->Clear();
	pH->Add(0.05f, 0, 1500, 0);
	pH->Add(0.1f, 0, 1800, 0);
	//pH->Add(0.895f, 0, 1500, 0);
}

// Constant 3 peaks each [5/2/2013 user]
void C413AppBase::DefinePeakPosition6(short n) {
	pEcho->SimuPkNum = n;

	CPosHt* pH;
	pH = &pEcho->PosT[n];
	pH->Clear();
	pH->Add(0.05f, 0, 1000, 100);
	pH->Add(0.1f, 0, 1200, 100);
	pH->Add(0.31f, 0, 1000, 100);

	pH = &pEcho->PosB[n];
	pH->Clear();
	pH->Add(0.05f, 0, 1000, 100);
	pH->Add(0.1f, 0, 1200, 100);
	pH->Add(0.31f, 0, 1000, 100);
}

// Bow shape perfectly horizontal [10/18/2012 Yuen]
void C413AppBase::DefinePeakPosition1(CMPoint* p, short delta) {
	float rds = 68500;  // 50 meter [10/27/2011 Administrator]
	float radius = float(sqrt(p->Co.x * p->Co.x + p->Co.y * p->Co.y));
	float bh = rds - float(sqrt(rds * rds + radius * radius));
	float wedge = 0.0125f * (p->Co.x + p->Co.y) / 150.0f;
	//wedge = 0;		// Wedge not effective [5/11/2013 user]
	float bow = 0.35f * (bh / 10.0f);
	//bow = 0;		// Bow not effective [5/11/2013 user]
	float pos1;
	pos1 = -bow + wedge;	// Bow and wedge [5/22/2013 user]
	//pos1=0;		// flat [11/2/2011 Administrator]
	//pos1=wedge;	// wedge only
	//pos1=0.00018f * rand() / float(RAND_MAX); // flat random [5/22/2013 user]
	if (p->Co.n == 31) {
		pEcho->SimuPkNum = 15; // For simulation only [10/27/2011 Administrator]
	}
	pEcho->SimuPkNum = 15; // For simulation only [10/27/2011 Administrator]
	CPosHt* pPH = &pEcho->PosT[15];
	CPosHt* pPH1 = &pEcho->PosT[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 0, 1800, 0);
	pPH->Add(pPH1->Pos[1].Position - pos1, delta, 1800, 0);
	pPH->Add(pPH1->Pos[2].Position - pos1, delta, 1800, 0);
	pPH = &pEcho->PosB[15];
	pPH1 = &pEcho->PosB[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 0, 1800, 0);
	pPH->Add(pPH1->Pos[1].Position + pos1, delta, 1800, 0);
	pPH->Add(pPH1->Pos[2].Position + pos1, delta, 1800, 0);
}
//---------------------------------------------------

// Bow shape with a tilt [10/18/2012 Yuen]
void C413AppBase::DefinePeakPosition2(CMPoint* p, short delta) {
	float rds = 50000; // 50 meter [10/27/2011 Administrator]
	float radius = float(sqrt(p->Co.x * p->Co.x + p->Co.y * p->Co.y));
	float bh = rds - float(sqrt(rds * rds + radius * radius));
	float wedge = 0.1f * (p->Co.x / 150.0f);
	float bow = 0.5f * (bh / 10.0f);
	float pos = -(bow + wedge);

	pos = 0.018 * sin(((radius) / 150.0f) * PI / 2.0f) + (0.01 * rand() / float(RAND_MAX)) + wedge;
	//pos=0;  // flat [11/2/2011 Administrator]
	pEcho->SimuPkNum = 15; // For simulation only [10/27/2011 Administrator]
	CPosHt* pPH = &pEcho->PosT[15];
	CPosHt* pPH1 = &pEcho->PosT[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 0, pPH1->Pos[0].Height, 0);
	pPH->Add(pPH1->Pos[1].Position - pos, delta, pPH1->Pos[1].Height, 0);
	pPH->Add(pPH1->Pos[2].Position - pos, delta, pPH1->Pos[2].Height, 0);
	pPH = &pEcho->PosB[15];
	pPH1 = &pEcho->PosB[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 0, pPH1->Pos[0].Height, 0);
	pPH->Add(pPH1->Pos[1].Position + pos, delta, pPH1->Pos[1].Height, 0);
	pPH->Add(pPH1->Pos[2].Position + pos, delta, pPH1->Pos[2].Height, 0);
}

// Bun shape with a tilt [10/18/2012 Yuen]
void C413AppBase::DefinePeakPosition4(CMPoint* p, short delta) {
	float rds = 50000; // 50 meter [10/27/2011 Administrator]
	float radius = float(sqrt(p->Co.x * p->Co.x + p->Co.y * p->Co.y));
	float bh = rds - float(sqrt(rds * rds + radius * radius));
	float wedge = 0.1f * (p->Co.x / 150.0f);
	float bow = 0.5f * (bh / 10.0f);
	float pos = (bow + wedge);

	pos = 0.018 * sin(((radius) / 150.0f) * PI / 2.0f) + (0.01 * rand() / float(RAND_MAX)) + wedge;
	//pos=0;  // flat [11/2/2011 Administrator]
	pEcho->SimuPkNum = 15; // For simulation only [10/27/2011 Administrator]
	CPosHt* pPH = &pEcho->PosT[15];
	CPosHt* pPH1 = &pEcho->PosT[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 0, pPH1->Pos[0].Height, 0);
	pPH->Add(pPH1->Pos[1].Position - pos, delta, pPH1->Pos[1].Height, 0);
	pPH->Add(pPH1->Pos[2].Position - pos, delta, pPH1->Pos[2].Height, 0);
	pPH = &pEcho->PosB[15];
	pPH1 = &pEcho->PosB[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 0, pPH1->Pos[0].Height, 0);
	pPH->Add(pPH1->Pos[1].Position + pos, delta, pPH1->Pos[1].Height, 0);
	pPH->Add(pPH1->Pos[2].Position + pos, delta, pPH1->Pos[2].Height, 0);
}

void C413AppBase::DefinePeakPosition3(CMPoint* p, short Delta) {
	float rds = 50000; // 50 meter [10/27/2011 Administrator]
	float radius = float(sqrt(p->Co.x * p->Co.x + p->Co.y * p->Co.y));
	float bh = rds - float(sqrt(rds * rds + radius * radius));
	float wedge = 0.1f * (p->Co.x / 150.0f);
	float bow = 0.5f * (bh / 10.0f);
	float pos1 = -(bow + wedge);
	pos1 = 0.018 * sin(((radius) / 150.0f) * PI / 2.0f) + /*(0.01 * rand() / float(RAND_MAX))*/ +wedge;
	pEcho->SimuPkNum = 15; // For simulation only [10/27/2011 Administrator]
	CPosHt* pPH = &pEcho->PosT[15];
	CPosHt* pPH1 = &pEcho->PosT[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 50, pPH1->Pos[0].Height, 0);
	pPH->Add(pPH1->Pos[1].Position - pos1, 500, pPH1->Pos[1].Height, 0);
	pPH->Add(pPH1->Pos[2].Position - pos1, 500, pPH1->Pos[2].Height, 0);
	pPH = &pEcho->PosB[15];
	pPH1 = &pEcho->PosB[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 50, pPH1->Pos[0].Height, 0);
	pPH->Add(pPH1->Pos[1].Position + pos1, 500, pPH1->Pos[1].Height, 0);
	pPH->Add(pPH1->Pos[2].Position + pos1, 500, pPH1->Pos[2].Height, 0);
}

void C413AppBase::DefinePeakPosition5(CMPoint* p, short Delta) {
	float rds = 50000; // 50 meter [10/27/2011 Administrator]
	float radius = float(sqrt(p->Co.x * p->Co.x + p->Co.y * p->Co.y));
	float bh = rds - float(sqrt(rds * rds + radius * radius));
	float wedge = 0.1f * (p->Co.x / 150.0f);
	float bow = 0.5f * (bh / 10.0f);
	float pos1 = -(bow + wedge);
	pos1 = 0.018 * sin(((radius) / 150.0f) * PI / 2.0f) + (0.01 * rand() / float(RAND_MAX)) + wedge;
	//pos=0;  // flat [11/2/2011 Administrator]
	pEcho->SimuPkNum = 15; // For simulation only [10/27/2011 Administrator]
	CPosHt* pPH = &pEcho->PosT[15];
	CPosHt* pPH1 = &pEcho->PosT[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 50, pPH1->Pos[0].Height, 0);
	pPH->Add(pPH1->Pos[1].Position - pos1, 500, pPH1->Pos[1].Height, 0);
	pPH->Add(pPH1->Pos[2].Position - pos1, 500, pPH1->Pos[2].Height, 0);
	pPH = &pEcho->PosB[15];
	pPH1 = &pEcho->PosB[0];
	pPH->Clear();
	pPH->Add(pPH1->Pos[0].Position, 50, pPH1->Pos[0].Height, 0);
	pPH->Add(pPH1->Pos[1].Position + pos1, 500, pPH1->Pos[1].Height, 0);
	pPH->Add(pPH1->Pos[2].Position + pos1, 500, pPH1->Pos[2].Height, 0);
}

void C413AppBase::OpenDmpPeak() {
	if (pDmpPeakT) {
		CloseDmpPeak();
	}
	short DumpCounter = DosUtil.GetDumpCounter();
	CString str;
	str.Format("C:\\TEMP\\DPEAKS-0-%d.CSV", DumpCounter);
	pDmpPeakT = fopen(str, "wb");
	fprintf(pDmpPeakT, "Start,%.3f\n", GetTickCount64() / 1000.0f);
	fprintf(pDmpPeakT, "%s,", CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(0));
	fprintf(pDmpPeakT, "\n");
	fprintf(pDmpPeakT, "iXMaxima,XMaxima,YMaxima,CM,Mid\n");

	str.Format("C:\\TEMP\\DPEAKS-1-%d.CSV", DumpCounter);
	pDmpPeakB = fopen(str, "wb");
	fprintf(pDmpPeakB, "Start,%.3f\n", GetTickCount64() / 1000.0f);
	fprintf(pDmpPeakB, "%s,", CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(0));
	fprintf(pDmpPeakB, "\n");
	fprintf(pDmpPeakB, "iXMaxima,XMaxima,YMaxima,CM,Mid\n");

	DumpCounter++;
	DosUtil.SetDumpCounter(DumpCounter);
}

void C413AppBase::CloseDmpPeak() {
	if (pDmpPeakT) {
		fprintf(pDmpPeakT, "\nEnd,%.3f", GetTickCount64() / 1000.0f);
		fclose(pDmpPeakT);
		pDmpPeakT = NULL;
	}
	if (pDmpPeakB) {
		fprintf(pDmpPeakB, "\nEnd,%.3f", GetTickCount64() / 1000.0f);
		fclose(pDmpPeakB);
		pDmpPeakB = NULL;
	}
}

void C413AppBase::PerpendicularLine(sABC& PL1, sABC& L1, float X1, float Y1, float X2, float Y2) {
	PL1.A = -L1.B;
	PL1.B = L1.A;
	PL1.C = PL1.A * ((X1 + X2) / 2) + PL1.B * ((Y1 + Y2) / 2);
}

void C413AppBase::Intersec(sABC& L1, sABC& L2, float& Cx, float& Cy) {
	float det = L1.A * L2.B - L2.A * L1.B;

	if (det) {
		Cx = (L2.B * L1.C - L1.B * L2.C) / det;
		Cy = (L1.A * L2.C - L2.A * L1.C) / det;
	}
	else {
		Cx = Cy = 0;
	}
}

void C413AppBase::FindABC(sABC& P, float X1, float Y1, float X2, float Y2) {
	P.A = Y2 - Y1;
	P.B = X1 - X2;
	P.C = P.A * X1 + P.B * Y1;
}

void C413AppBase::RecordWaferCenterEvent(CString& strEvent) {
	CString mdir;
	DosUtil.GetProgramDir(mdir);
	mdir += "WAFERCENTER.LOG";
	FILE* fp;
	fp = fopen(mdir, "ab");
	if (fp) {
		fprintf(fp, "%s,%s\n", GetTimeStamp().GetBuffer(0), strEvent.GetBuffer(0));
		fclose(fp);
	}
}

void C413AppBase::RecordSPCalibEvent(CString& strEvent) {
	CString mdir;
	DosUtil.GetProgramDir(mdir);
	mdir += "SPCALIB.LOG";
	FILE* fp;
	fp = fopen(mdir, "ab");
	if (fp) {
		fprintf(fp, "%s,%s\n", GetTimeStamp().GetBuffer(0), strEvent.GetBuffer(0));
		fclose(fp);
	}
}

// [11212022 ZHIMING
void C413AppBase::SaveConfocalOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Ports";

	sprintf(str, "%s", sConfocalAddress.GetBuffer(0));
	WritePrivateProfileString(sec, "Confocal", str, inifile);

	//[ Mahedi Kamal 10182024
	sprintf(str, "%s", sConfocalAddress2.GetBuffer(0));
	WritePrivateProfileString(sec, "Confocal2", str, inifile);
	
	sprintf(str, "%d", ConfocalPort1);
	WritePrivateProfileString(sec, "ConfocalPort1", str, inifile);
	
	sprintf(str, "%d", ConfocalPort2);
	WritePrivateProfileString(sec, "ConfocalPort2", str, inifile);
	//]
	sec = "Confocal";

	sprintf(str, "%d", bSimuConfocal);
	WritePrivateProfileString(sec, "bSimu", str, inifile);

	//[ 10152024 Mahedi Kamal
	sprintf(str, "%f", ZFocusPos008);
	WritePrivateProfileString(sec, "ZFocusPos008", str, inifile);

	sprintf(str, "%f", ZFocusDevRenge008);
	WritePrivateProfileString(sec, "ZFocusDevRenge008", str, inifile);

	sprintf(str, "%f", ZFocusPos030);
	WritePrivateProfileString(sec, "ZFocusPos030", str, inifile);

	sprintf(str, "%f", ZFocusDevRenge030);
	WritePrivateProfileString(sec, "ZFocusDevRenge030", str, inifile);
	//]
	//[[20250926_Mohir
	sprintf(str, "%f", ThicknessScanLength);
	WritePrivateProfileString(sec, "ThicknessScanLength", str, inifile);
	//]]
	
	//[ 10242025_M
	sprintf(str, "%f", ThicknessScanSpeed);
	WritePrivateProfileString(sec, "ThicknessScanSpeed", str, inifile);
	//]
}

void C413AppBase::LoadConfocalOption() {
	char str[256];
	CString inifile;

	DosUtil.GetLocalCfgFile(inifile);

	CString sec = "Ports";

	GetPrivateProfileString(sec, "Confocal", "192.168.1.1", str, 255, inifile);
	sConfocalAddress = str;
	//[ Mahedi Kamal 10182024
	GetPrivateProfileString(sec, "Confocal2", "192.168.1.1", str, 255, inifile);
	sConfocalAddress2 = str;

	GetPrivateProfileString(sec, "ConfocalPort1", "0", str, 255, inifile);
	ConfocalPort1 = atoi(str);

	GetPrivateProfileString(sec, "ConfocalPort2", "0", str, 255, inifile);
	ConfocalPort2 = atoi(str);
	//]
	sec = "Confocal";

	GetPrivateProfileString(sec, "bSimu", "1", str, 255, inifile);
	bSimuConfocal = atoi(str);

	//[ // Mahedi Kamal 10152024
	GetPrivateProfileString(sec, "ZFocusPos008", "1.00", str, 255, inifile);
	ZFocusPos008 = atof(str);

	GetPrivateProfileString(sec, "ZFocusDevRenge008", "1.00", str, 255, inifile);
	ZFocusDevRenge008 = atof(str);

	GetPrivateProfileString(sec, "ZFocusPos030", "1.00", str, 255, inifile);
	ZFocusPos030 = atof(str);

	GetPrivateProfileString(sec, "ZFocusDevRenge030", "1.00", str, 255, inifile);
	ZFocusDevRenge030 = atof(str);

	GetPrivateProfileString(sec, "IsFocusScanInMeasurementRoutine", "0", str, 255, inifile); // 12120224
	IsFocusScanInMeasurementRoutine = atoi(str);

	GetPrivateProfileString(sec, "bConfocalRS232NotEthernet", "1", str, 255, inifile); // 10172024
	bConfocalRS232NotEthernet = atoi(str);
	//]
	//[[20250926_Mohir
	GetPrivateProfileString(sec, "ThicknessScanLength", "2.00", str, 255, inifile);
	ThicknessScanLength = atof(str);
	//]]

	//[ 10242025_M
	GetPrivateProfileString(sec, "ThicknessScanSpeed", "0.065", str, 255, inifile);
	ThicknessScanSpeed = atof(str);
	//]
}
// ]

void C413AppBase::SleepEX(ULONGLONG dly) {
	ULONGLONG now = GetTickCount64();
	while ((GetTickCount64() - now) < dly) {
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			AfxGetThread()->PumpMessage();
		}
	}
}

void C413AppBase::StageMoveError() {
	AfxMessageBox("Stage move error");
}

void C413AppBase::MouseWait(HWND hWndPC) {
	if (hWndPC) {
		::SetCapture(hWndPC);
		SetCursor(LoadCursor(0, IDC_WAIT));
	}
}

void C413AppBase::MouseNormal(HWND hWndPC) {
	if (hWndPC) {
		SetCursor(LoadCursor(0, IDC_ARROW));
		ReleaseCapture();
	}
}

void C413AppBase::LoadSimuDIB(CSICam* pVideo) {
	// Do not clear overlay inside SearchDepth loop so user can see them longer [4/20/2013 Administrator]
	if (pVideo && pVideo->IsSimu() /*&& bLoadDIB*/) {
		CString dir, name;
		DosUtil.GetProgramDir(dir);
		dir += "IMAGES\\BSI\\BSI-%d.BMP";
		name.Format(dir, rnd1); rnd1++; if (rnd1 > maxrnd1) rnd1 = 1;
		pVideo->LoadSimuDIB(name);
	}
}

void C413AppBase::LoadSimuDIB2(CSICam* pVideo) {
	if (pVideo && pVideo->IsSimu() && bLoadDIB) {
		CString dir, name;
		DosUtil.GetProgramDir(dir);
		dir += "IMAGE\\BSI\\BSI-%d.BMP";
		name.Format(dir, rnd2); rnd2++; if (rnd2 > 48) rnd2 = 1;
		pVideo->LoadSimuDIB(name);
	}
}

void C413AppBase::SaveVideo(CSICam* pVideo, CRecipe413* pRcp) {
	if (pVideo && pRcp->bSaveVideo) {
		pVideo->SetRecipe(pRcp);
		pVideo->bSaveVideo = TRUE;
		pVideo->FileDirc.Format("C:\\TEMP\\%s\\%s-%s-%d\\", CTime::GetCurrentTime().Format("%y%m%d"), pRcp->CarrierID, pRcp->RcpeName, pRcp->SlotID);
		//pVideo->FileBase.Format("TH%03d-%d.BMP",pMP->Co.n,S.n);  // To be supplied externally [6/9/2014 Yuen]
		pVideo->bShow = TRUE;
		ULONGLONG tick = GetTickCount64();
		while (pVideo->bShow && ((GetTickCount64() - tick) < 1000)) {
			Sleep(10);
		}
	}
}

CString C413AppBase::GetOuterEdge1(float angle) {
	CString name;
	DosUtil.GetProgramDir(name);
	name += "IMAGES\\";

	if ((angle >= 315 + 22.5) || (angle < 45 - 22.5)) {
		name = "IMG0.BMP";
	}
	else if ((angle >= 45 - 22.5) && (angle < 45 + 22.5)) {
		name = "IMG45.BMP";
	}
	else if ((angle >= 90 - 22.5) && (angle < 90 + 22.5)) {
		name = "IMG90.BMP";
	}
	else if ((angle >= 135 - 22.5) && (angle < 135 + 22.5)) {
		name = "IMG135.BMP";
	}
	else if ((angle >= 180 - 22.5) && (angle < 180 + 22.5)) {
		name = "IMG180.BMP";
	}
	else if ((angle >= 225 - 22.5) && (angle < 225 + 22.5)) {
		name = "IMG225.BMP";
	}
	else if ((angle >= 270 - 22.5) && (angle < 270 + 22.5)) {
		name = "IMG270.BMP";
	}
	else if ((angle >= 315 - 22.5) && (angle < 315 + 22.5)) {
		name = "IMG315.BMP";
	}
	return name;
}

// Simulation only
CString C413AppBase::GetOuterEdge2(float angle) {
	float a1 = 22.5f;
	float a2 = a1 / 2.0f;

	CString name;
	DosUtil.GetProgramDir(name);
	name += "IMAGES\\TRIM-OUT\\";

	if ((angle >= 360.0 - a2) || (angle < 0.0 + a2)) {
		name += "OE0.BMP";
	}
	else if ((angle >= 22.5 - a2) && (angle < 22.5 + a2)) {
		name += "OE22.5.BMP";
	}
	else if ((angle >= 45 - a2) && (angle < 45 + a2)) {
		name += "OE45.BMP";
	}
	else if ((angle >= 67.5 - a2) && (angle < 67.5 + a2)) {
		name += "OE67.5.BMP";
	}
	else if ((angle >= 90 - a2) && (angle < 90 + a2)) {
		name += "OE90.BMP";
	}
	else if ((angle >= 112.5 - a2) && (angle < 112.5 + a2)) {
		name += "OE112.5.BMP";
	}
	else if ((angle >= 135 - a2) && (angle < 135 + a2)) {
		name += "OE135.BMP";
	}
	else if ((angle >= 157.5 - a2) && (angle < 157.5 + a2)) {
		name += "OE157.5.BMP";
	}
	else if ((angle >= 180 - a2) && (angle < 180 + a2)) {
		name += "OE180.BMP";
	}
	else if ((angle >= 202.5 - a2) && (angle < 202.5 + a2)) {
		name += "OE202.5.BMP";
	}
	else if ((angle >= 225 - a2) && (angle < 225 + a2)) {
		name += "OE225.BMP";
	}
	else if ((angle >= 247.5 - a2) && (angle < 247.5 + a2)) {
		name += "OE247.5.BMP";
	}
	else if ((angle >= 270 - a2) && (angle < 270 + a2)) {
		name += "OE270.BMP";
	}
	else if ((angle >= 292.5 - a2) && (angle < 292.5 + a2)) {
		name += "OE292.5.BMP";
	}
	else if ((angle >= 315 - a2) && (angle < 315 + a2)) {
		name += "OE315.BMP";
	}
	else if ((angle >= 337.5 - a2) && (angle < 337.5 + a2)) {
		name += "OE337.5.BMP";
	}
	return name;
}

// Simulation only
CString C413AppBase::GetInnerEdge1(float angle) {
	CString name;
	DosUtil.GetProgramDir(name);
	name += "IMAGES\\";

	if ((angle >= 315 + 22.5) || (angle < 45 - 22.5)) {
		name += "IE0.BMP";
	}
	else if ((angle >= 45 - 22.5) && (angle < 45 + 22.5)) {
		name += "IE45.BMP";
	}
	else if ((angle >= 90 - 22.5) && (angle < 90 + 22.5)) {
		name += "IE90.BMP";
	}
	else if ((angle >= 135 - 22.5) && (angle < 135 + 22.5)) {
		name += "IE135.BMP";
	}
	else if ((angle >= 180 - 22.5) && (angle < 180 + 22.5)) {
		name += "IE180.BMP";
	}
	else if ((angle >= 225 - 22.5) && (angle < 225 + 22.5)) {
		name += "IE225.BMP";
	}
	else if ((angle >= 270 - 22.5) && (angle < 270 + 22.5)) {
		name += "IE270.BMP";
	}
	else if ((angle >= 315 - 22.5) && (angle < 315 + 22.5)) {
		name += "IE315.BMP";
	}
	return name;
}

// Simulation only
CString C413AppBase::GetInnerEdge2(float angle) {
	float a1 = 22.5f;
	float a2 = a1 / 2.0f;

	CString name;
	DosUtil.GetProgramDir(name);
	name += "IMAGES\\TRIM-IN\\";

	if ((angle >= 360.0 - a2) || (angle < 0.0 + a2)) {
		name += "IE0.BMP";
	}
	else if ((angle >= 22.5 - a2) && (angle < 22.5 + a2)) {
		name += "IE22.5.BMP";
	}
	else if ((angle >= 45 - a2) && (angle < 45 + a2)) {
		name += "IE45.BMP";
	}
	else if ((angle >= 67.5 - a2) && (angle < 67.5 + a2)) {
		name += "IE67.5.BMP";
	}
	else if ((angle >= 90 - a2) && (angle < 90 + a2)) {
		name += "IE90.BMP";
	}
	else if ((angle >= 112.5 - a2) && (angle < 112.5 + a2)) {
		name += "IE112.5.BMP";
	}
	else if ((angle >= 135 - a2) && (angle < 135 + a2)) {
		name += "IE135.BMP";
	}
	else if ((angle >= 157.5 - a2) && (angle < 157.5 + a2)) {
		name += "IE157.5.BMP";
	}
	else if ((angle >= 180 - a2) && (angle < 180 + a2)) {
		name += "IE180.BMP";
	}
	else if ((angle >= 202.5 - a2) && (angle < 202.5 + a2)) {
		name += "IE202.5.BMP";
	}
	else if ((angle >= 225 - a2) && (angle < 225 + a2)) {
		name += "IE225.BMP";
	}
	else if ((angle >= 247.5 - a2) && (angle < 247.5 + a2)) {
		name += "IE247.5.BMP";
	}
	else if ((angle >= 270 - a2) && (angle < 270 + a2)) {
		name += "IE270.BMP";
	}
	else if ((angle >= 292.5 - a2) && (angle < 292.5 + a2)) {
		name += "IE292.5.BMP";
	}
	else if ((angle >= 315 - a2) && (angle < 315 + a2)) {
		name += "IE315.BMP";
	}
	else if ((angle >= 337.5 - a2) && (angle < 337.5 + a2)) {
		name += "IE337.5.BMP";
	}
	return name;
}

void C413AppBase::LoadSimuDIB3(CSICam* pVideo, float angle) {
	if (pVideo && pVideo->IsSimu()) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "IMAGES\\";
		if ((angle >= 0) && (angle < 45 - 22.5)) {
			dir = dir + "IMG0.BMP";
		}
		else if ((angle >= 45 - 22.5) && (angle < 45 + 22.5)) {
			dir = dir + "IMG45.BMP";
		}
		else if ((angle >= 90 - 22.5) && (angle < 90 + 22.5)) {
			dir = dir + "IMG90.BMP";
		}
		else if ((angle >= 135 - 22.5) && (angle < 135 + 22.5)) {
			dir = dir + "IMG135.BMP";
		}
		else if ((angle >= 180 - 22.5) && (angle < 180 + 22.5)) {
			dir = dir + "IMG180.BMP";
		}
		else if ((angle >= 225 - 22.5) && (angle < 225 + 22.5)) {
			dir = dir + "IMG225.BMP";
		}
		else if ((angle >= 270 - 22.5) && (angle < 270 + 22.5)) {
			dir = dir + "IMG270.BMP";
		}
		else if ((angle >= 315 - 22.5) && (angle < 315 + 22.5)) {
			dir = dir + "IMG315.BMP";
		}
		pVideo->LoadSimuDIB(dir);
	}
}