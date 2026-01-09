#include "stdafx.h"
#include "Var.h"

#include "..\SRC\DOSUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

CVar Var;

CVar::CVar(void) {
	WinID = 1000;
	// 	bSECS = FALSE;
	int i;
	for (i = 0; i < 5; i++) {
		bFFO[i] = FALSE;
	}
	bHostWD = TRUE;
	bEnabled = FALSE;
	bEnableBaseline = TRUE;
	bClampOnUnload = TRUE;

	bSortByRecipe = FALSE;

	// 0 = SEMI state model
	// 1 = TTI scheme
// 	LoadUnloadScheme = 1;
	MaxSlot = 25;
	MaxPlotLine = 16;
	nCIDR = 0;
	nLoadPort = 0;
	Repeat = 1;
	PrintPitch = 8;
	WorkDir = "C:\\DATA";
	bByPassCIDR = FALSE;
	bAutoClose = TRUE;
	bReCalc = FALSE;

	bBkgSubstract = TRUE;
	bSkipFirstScan = FALSE;
	bHideMapLine = FALSE;
	bShowAllWafer = TRUE;
	ShowWaferFilter = "";
	bCalcHeightUsingWaferSize = TRUE;

	for (i = 0; i < 4; i++) {
		bTransferInProgress[i] = FALSE;
	}

	bAMHS = FALSE;

	StressFactor = 1e-7f;
	StressUnit = "MPa";
	ismegapascal = TRUE;

	StressFactor = DosUtil.GetStressFactor(StressFactor);
	DosUtil.SetStressFactor(StressFactor);
	StressUnit = DosUtil.GetStressUnit(StressUnit);
	DosUtil.SetStressUnit(StressUnit);

	FilmThicknessFactor = 1e-1f;
	FilmThicknessUnit = "nm";

	FilmThicknessFactor = DosUtil.GetFilmThkFactor(FilmThicknessFactor);
	DosUtil.SetFilmThkFactor(FilmThicknessFactor);
	FilmThicknessUnit = DosUtil.GetFilmThkUnit(FilmThicknessUnit);
	DosUtil.SetFilmThkUnit(FilmThicknessUnit);

	// 	bAirCyl = DosUtil.IsLaserSwitchCyl();
	// 	DosUtil.SetLaserSwitchCyl(bAirCyl);

	bFlatten = DosUtil.GetFlattenMap();
	DosUtil.SetFlattenMap(bFlatten);

	SPCMethod = 0;

	bScanning = FALSE;

	Laser1 = 780;
	Laser2 = 810;

	// 	DosUtil.SetLaserName1(1,Laser1);
	// 	DosUtil.SetLaserName1(2,Laser2);
	// 	DosUtil.SetLaserName1(3,Laser1);
	// 	DosUtil.SetLaserName1(4,Laser2);

	LoadParam();
	SaveParam();
}

CVar::~CVar(void) {
	SaveParam();
}

void CVar::SaveParam(void) {
	CString str;
	CString sect;
	CString winini;

	DosUtil.GetLocalSysFile(winini);
	sect = "FSM";

	str.Format("%d", nLoadPort);
	WritePrivateProfileString(sect, "NoLoadPort", str, winini);

	str.Format("%d", nCIDR);
	WritePrivateProfileString(sect, "NoCIDR", str, winini);

	sect = "Variables";
	// 	str.Format("%d", PrintPitch);
	// 	WritePrivateProfileString(sect, "PrintPitch", str, winini);
	str.Format("%d", MaxSlot);
	WritePrivateProfileString(sect, "MaxSlot", str, winini);
	// 	str.Format("%d", LoadUnloadScheme);
	// 	WritePrivateProfileString(sect, "LoadUnloadScheme", str, winini);

	// 	sect = "Persist";
	//
	// 	str.Format("%d", MaxPlotLine);
	// 	WritePrivateProfileString(sect, "MaxPlotLine", str, winini);
	// 	str.Format("%d", bBkgSubstract);
	// 	WritePrivateProfileString(sect, "BackgroundSubstration", str, winini);
	//
	// 	str.Format("%d", bEnableBaseline);
	// 	WritePrivateProfileString(sect, "EnableBaseline", str, winini);
	//
	// 	DosUtil.GetLocalCfgFile(winini);

	// 	sect = "ProcessControl";
	// 	str.Format("%d", bAutoClose);
	// 	WritePrivateProfileString(sect, "AutoClose", str, winini);

	//	DosUtil.WriteUIMode(UIMode);

		// Save CTE parameters in local CFG file
	// 	sect = "CTE";
	// 	str.Format("%f", sc1);
	// 	WritePrivateProfileString(sect, "SC1", str, winini);

	// 	str.Format("%f", sc2);
	// 	WritePrivateProfileString(sect, "SC2", str, winini);

	// 	str.Format("%f", gd1);
	// 	WritePrivateProfileString(sect, "GD1", str, winini);

	// 	str.Format("%f", gd2);
	// 	WritePrivateProfileString(sect, "GD2", str, winini);

	// 	str.Format("%f", cte);
	// 	WritePrivateProfileString(sect, "CTE", str, winini);

	// 	str.Format("%f", bem);
	// 	WritePrivateProfileString(sect, "BEM", str, winini);
}

void CVar::LoadParam(void) {
	//	char str[256];
	CString winini;
	CString st, sect;

	DosUtil.GetLocalSysFile(winini);
	sect = "FSM";

	nLoadPort = GetPrivateProfileInt(sect, "NoLoadPort", nLoadPort, winini);
	nCIDR = GetPrivateProfileInt(sect, "NoCIDR", nLoadPort, winini);

	sect = "Variables";
	// 	PrintPitch = GetPrivateProfileInt(sect, "PrintPitch", PrintPitch, winini);
	MaxSlot = GetPrivateProfileInt(sect, "MaxSlot", MaxSlot, winini);
	// 	LoadUnloadScheme = GetPrivateProfileInt(sect, "LoadUnloadScheme", LoadUnloadScheme, winini);

	// 	sect = "Modules";
	// 	bSECS = FALSE;
	// 	bSECS = GetPrivateProfileInt(sect, "SECS", bSECS, winini);
	//
	// 	CString tmp;
	// 	tmp.Format("%d", bSECS);
	// 	WritePrivateProfileString(sect, "SECS", tmp, winini);

	// 	sect = "Persist";
	// 	MaxPlotLine = GetPrivateProfileInt(sect, "MaxPlotLine", MaxPlotLine, winini);
	// 	bBkgSubstract = GetPrivateProfileInt(sect, "BackgroundSubstration", bBkgSubstract, winini);
	// 	bEnableBaseline = GetPrivateProfileInt(sect, "EnableBaseline", bEnableBaseline, winini);

	// 	DosUtil.GetLocalCfgFile(winini);
	// 	sect = "ProcessControl";
	// 	bAutoClose = GetPrivateProfileInt(sect, "AutoClose", bAutoClose, winini);

	UIMode = DosUtil.GetUIMode();

	bClampOnUnload = DosUtil.GetClampState();
	DosUtil.SetClampState(bClampOnUnload);

	// 	DosUtil.GetLocalCfgFile(winini);
		// Load CTE parameters
	// 	sect = "CTE";
	// 	GetPrivateProfileString(sect, "SC1", "0.001", str, 255, winini);
	// 	sc1 = atof(str);

	// 	GetPrivateProfileString(sect, "SC2", "0.001", str, 255, winini);
	// 	sc2 = atof(str);

	// 	GetPrivateProfileString(sect, "GD1", "0.001", str, 255, winini);
	// 	gd1 = atof(str);

	// 	GetPrivateProfileString(sect, "GD2", "0.001", str, 255, winini);
	// 	gd2 = atof(str);

	// 	GetPrivateProfileString(sect, "CTE", "0.001", str, 255, winini);
	// 	cte = atof(str);

	// 	GetPrivateProfileString(sect, "BEM", "0.001", str, 255, winini);
	// 	bem = atof(str);
}

BOOL CVar::SetTransferInProgress(short port, BOOL state) {
	switch (port) {
	case 1:
		Var.bTransferInProgress[0] = state;
		break;
	case 2:
		Var.bTransferInProgress[1] = state;
		break;
	case 3:
		Var.bTransferInProgress[2] = state;
		break;
	case 4:
		Var.bTransferInProgress[3] = state;
		break;
	default:
		ASSERT(0);
		return FALSE;
	}
	return TRUE;
}
//BOOL CVar::SendHeartBeat()
//{
//	CSMHdr SM;
//	SM.cmd = CSMHdr::CTIAMACTIVE;
//	CSMClt ipcLog;
//	if (ipcLog.Start1(Var.LogSvr, sizeof(CSMHdr),FALSE)) {
//		if (ipcLog.Send(&SM,1000)) {
//			return TRUE;
//		}
//	}
//	// Log program is down, need to restart
//	return FALSE;
//}

//void CVar::Log(CString msg, short lvl)
//{
//
//}