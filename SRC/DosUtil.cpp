// DOS.cpp: implementation of the CDOS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <direct.h>
#include "DOSUtil.h"
#include "FPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

CDOS DosUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDOS::CDOS() {
	GetProgramDir(ProgramDir);
	GetFSMAppFile(FSMAppFile);
	GetLocalCfgFile(LocalCfgFile);
	GetLocalSysFile(LocalSysFile);

	SetHelpType(GetHelpType());

	// Preset
	CString RootDir;
	RootDir = GetRootDir();
	WriteRootDir(RootDir);
	DirSys.SetRootDir(RootDir);

	WriteFanFilterUnit(GetFanFilterUnit());
	SetFFUType(GetFFUType());
	SetFFUPressure(GetFFUPressure());
	SetResetFreq(GetResetFreq());

	WriteUIMode(GetUIMode());
	// 	WriteIPAddr(GetIPAddr(3),3);	// 413 [5/29/2011 XPMUser]

		// LYF Must be set here because other modules in C2C uses it [4/28/2006]
	SetInCasLoc(1, GetInCasLoc(1, 4));
	SetInCasLoc(2, GetInCasLoc(2, 6));
	SetInCasLoc(3, GetInCasLoc(3, 8));
	SetInCasLoc(4, GetInCasLoc(4, 10));

	SetCCDPos(1, GetCCDPos(1));
	// 	SetCCDPos(2,GetCCDPos(2));
	SetCCDNum(1, GetCCDNum(1));
	// 	SetCCDNum(2,GetCCDNum(2));

	SetNumLP(GetNumLP());
	SetNumCIDR(GetNumCIDR());

	// 	SetEnaAlignerCtrl(GetEnaAlignerCtrl());
	SetAlignerPostPos(1, GetAlignerPostPos(1));
	SetAlignerPostPos(2, GetAlignerPostPos(2));
	SetAlignerOffsetPos(1, GetAlignerOffsetPos(1));
	SetAlignerOffsetPos(2, GetAlignerOffsetPos(2));

	// 	SetRbtRecoveryType(GetRbtRecoveryType());

	SetCasRepeat(GetCasRepeat());
	SetScanRepeat(GetScanRepeat());

	//	SetPLCSimu(GetPLCSimu());
	//	SetDIOSimu(GetDIOSimu());
	//	SetDaqSimu(GetDaqSimu());
	//	SetWDTSimu(GetWDTSimu());
	SetRbtUnitSimu(GetRbtUnitSimu());
	SetLPUnitSimu(GetLPUnitSimu(1), 1);
	SetLPUnitSimu(GetLPUnitSimu(2), 2);
	// 	SetCassetteSwitchSimu(GetCassetteSwitchSimu());
	// 	SetSenTorrUnitSimu(GetSenTorrUnitSimu());

	//	SetWDTPort(GetWDTPort());
	SetLPPort(1, GetLPPort(1));
	SetLPPort(2, GetLPPort(2));
	SetCIDRPort(1, GetCIDRPort(1));
	SetCIDRPort(2, GetCIDRPort(2));
	SetRobotPort(GetRobotPort());
	// 	SetAlignerCtrlPort(GetAlignerCtrlPort());
	// 	SetDIOMotor(GetDIOMotor());
	// 	SetCommSenTorr(GetCommSenTorr());
	SetSignalTowerPort(GetSignalTowerPort());

	//	GetRoughingPressure();
	//	GetHoldingPressure();
	//	GetIGSensitivity();
	//	GetIGGasCorrection();
	//	GetAutoOnPressure();

	// 	SetVacOffTempr(GetVacOffTempr());
	// 	SetEnableModeSwitch(GetEnableModeSwitch());

	// 	SetBakeTempr(GetBakeTempr());
	// 	SetAtmVac(GetAtmVac());
	// 	SetTargetVac(GetTargetVac());
	// 	SetColdTempr(GetColdTempr());

	SetWaferPresent(GetWaferPresent());
	// 	SetSPCMethod(128,GetSPCMethod(128));
	// 	SetSPCMethod(900,GetSPCMethod(900));
	// 	SetWaferPresentABLimit(GetWaferPresentABLimit());
	// 	SetEnable810Laser(GetEnable810Laser());

	GetSetLPLight("NoFoup");
	GetSetLPLight("FoupPresent");
	GetSetLPLight("ButtonPressed1");
	GetSetLPLight("ButtonPressed2");
	GetSetLPLight("Clamping");
	GetSetLPLight("Clampped");
	GetSetLPLight("Docking");
	GetSetLPLight("Docked");
	GetSetLPLight("Opened");
	GetSetLPLight("Undocking");
	GetSetLPLight("Undocked");
	GetSetLPLight("Unclamping");
	GetSetLPLight("Unclamped");
	GetSetLPLight("CloseNoRelease");
}

CDOS::~CDOS() {}

void* CDOS::StartModule(CString name, BOOL bWait2Start) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	CString filename;

	MakeFileNameFromModule(filename, name, "exe");

	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (CreateProcess(filename, NULL, NULL, NULL, FALSE, NULL, NULL, ".", &si, &pi)) {
		if (bWait2Start) {
			::WaitForInputIdle(pi.hProcess, 1000);
		}
		return pi.hProcess;
	}
	return NULL;
}

// rootdir equivalent to Var.WorkDir
BOOL CDOS::SetWorkDirectory(CString& rootdir, CString& workdir) {
	MakeFileNameFromModule(rootdir, workdir, "");
	if (rootdir != "") {
		if (_chdir(rootdir) == -1) {
			if (_mkdir(rootdir) == -1) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CDOS::MakeFileNameFromModule(CString& filename, CString& name, CString extn) {
	char fullname[_MAX_PATH];
	char dir[_MAX_DIR];
	char drive[_MAX_DRIVE];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetModuleFileName(NULL, fullname, MAX_PATH);
	_splitpath(fullname, drive, dir, fname, ext);
	_makepath(fullname, drive, dir, name, extn);
	filename = fullname;
	return TRUE;
}

void CDOS::AddExtension(CString& filename, CString ex) {
	char dirname[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(filename, drive, dir, fname, ext);
	_makepath(dirname, drive, dir, fname, ex);
	filename = dirname;
}

CString CDOS::MakeFullPathName(CString& path, CString& name, CString ext) {
	return path + "\\" + name + ext;
}

void CDOS::ExtractPath(CString& filename) {
	char dirname[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	strcpy(dirname, filename.GetBuffer(filename.GetLength()));
	_splitpath(dirname, drive, dir, fname, ext);
	filename = drive;
	filename += dir;
}

void CDOS::ExtractFilename(CString& filename) {
	char dirname[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	strcpy(dirname, filename.GetBuffer(filename.GetLength()));
	_splitpath(dirname, drive, dir, fname, ext);
	filename = fname;
}

void CDOS::ExtractFilenameExt(CString& filename) {
	char dirname[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	strcpy(dirname, filename.GetBuffer(filename.GetLength()));
	_splitpath(dirname, drive, dir, fname, ext);
	filename = fname;
	filename += ext;
}

void CDOS::RemoveExtension(CString& filename) {
	char dirname[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	strcpy(dirname, filename.GetBuffer(filename.GetLength()));
	_splitpath(dirname, drive, dir, fname, ext);
	_makepath(dirname, drive, dir, fname, "");
	filename = dirname;
}

BOOL CDOS::GetProgramDir(CString& mdir) {
	char dirname[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetModuleFileName(NULL, dirname, _MAX_PATH);
	_splitpath(dirname, drive, dir, fname, ext);
	_makepath(dirname, drive, dir, "", "");
	mdir = dirname;
	return TRUE;
}

CString CDOS::GetResultDir() {
	CString str;
	str = _T("C:\\DATA\\");
	if (GetFileAttributes(str) == -1) {
		if (!CreateDirectory(str, NULL)) return _T("");
	}
	return str;
}

void CDOS::GetLocalCfgFile(CString& filename) {
	char inifile[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetModuleFileName(NULL, inifile, _MAX_PATH);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "FSMLocalCfg", "ini");
	filename = inifile;
}

void CDOS::GetLocalSysFile(CString& filename) {
	char inifile[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetModuleFileName(NULL, inifile, _MAX_PATH);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "FSMLocalSys", "ini");
	filename = inifile;
}

void CDOS::GetFSMAppFile(CString& filename) {
	char winini[256];

	GetWindowsDirectory(winini, 255);
	lstrcat(winini, "\\fsmapp.ini");

	filename = winini;
}

short CDOS::GetUIMode() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("FSM", "UIMode", "1", str, 255, LocalSysFile);
	return short(atoi(str));
}

void CDOS::WriteUIMode(short mode) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", mode);
	WritePrivateProfileString("FSM", "UIMode", str, LocalSysFile);
}

// CString CDOS::GetIPAddr(short which)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	switch(which)
// 	{
// 	case 1:	// 128
// 		GetPrivateProfileString("FSM","IPAddr128","",str,255,LocalSysFile);
// 		break;
// 	case 2:	// 900
// 		GetPrivateProfileString("FSM","IPAddr900","",str,255,LocalSysFile);
// 		break;
// 	case 3:	// 413
// 		GetPrivateProfileString("FSM","IPAddr413","",str,255,LocalSysFile);
// 		break;
// 	default:
// 		memset(str,0,sizeof(str));
// 		ASSERT(0);
// 		break;
// 	}
// 	return str;
// }
//
// void CDOS::WriteIPAddr(CString Addr, short which)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	switch(which)
// 	{
// 	case 1:	// 128
// 		WritePrivateProfileString("FSM","IPAddr128",Addr,LocalSysFile);
// 		break;
// 	case 2:	// 900
// 		WritePrivateProfileString("FSM","IPAddr900",Addr,LocalSysFile);
// 		break;
// 	case 3:	// 4130
// 		WritePrivateProfileString("FSM","IPAddr413",Addr,LocalSysFile);
// 		break;
// 	default:
// 		ASSERT(0);
// 		break;
// 	}
// }

CString CDOS::GetCIDRType() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("CIDRUnit", "Type", "Asyst", str, 255, LocalCfgFile);
	return str;
}

void CDOS::SetCIDRType(CString& type) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("CIDRUnit", "Type", type, LocalCfgFile);
}

CString CDOS::GetHelpType() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("FSM", "HelpType", "PDF", str, 255, LocalSysFile);
	return str;
}

void CDOS::SetHelpType(CString& type) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("FSM", "HelpType", type, LocalSysFile);
}

CString CDOS::GetRbtType() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("RobotUnit", "Type", "Brooks", str, 255, LocalCfgFile);
	return str;
}

void CDOS::SetRbtType(CString type) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("RobotUnit", "Type", type, LocalCfgFile);
}

// CString CDOS::GetRbtRecoveryType()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("RobotUnit","RecoveryType","Sensor",str,255,LocalCfgFile);
// 	return str;
// }

// void CDOS::SetRbtRecoveryType(CString type)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	WritePrivateProfileString("RobotUnit","RecoveryType",type,LocalCfgFile);
// }

BOOL CDOS::GetLPUnit(CString Name, BOOL bDef) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("LPUnit", Name, bDef, LocalCfgFile);
}

void CDOS::SetLPUnit(CString Name, BOOL bNV) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", bNV);
	WritePrivateProfileString("LPUnit", Name, str, LocalCfgFile);
}

short CDOS::GetLPUnit(CString Name, short port) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("LPUnit", Name, port, LocalCfgFile);
}

void CDOS::SetLPUnit(CString Name, short port) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", port);
	WritePrivateProfileString("LPUnit", Name, str, LocalCfgFile);
}

CString CDOS::GetLPUnit(CString Name, CString str) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char rstr[256];
	GetPrivateProfileString("LPUnit", Name, str, rstr, sizeof(rstr), LocalCfgFile);
	return rstr;
}

void CDOS::SetLPUnit(CString Name, CString str) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("LPUnit", Name, str, LocalCfgFile);
}

CString CDOS::GetSlotmap() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("TestSlotmap", "SM", "00000000000000000000000000", str, 255, LocalCfgFile);
	Lock.Unlock();
	WriteSlotmap(str);
	return str;
}

void CDOS::WriteSlotmap(CString SM) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("TestSlotmap", "SM", SM, LocalCfgFile);
}

CString CDOS::GetLPLight(CString Name, CString def) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("LPLight", Name, def, str, 255, LocalCfgFile);
	if (strlen(str) != 3) {
		AfxMessageBox("Incorrect field in LPLight");
		return "";
	}
	return str;
}

void CDOS::SetLPLight(CString Name, CString val) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("LPLight", Name, val, LocalCfgFile);
}

CString CDOS::GetLPLightAuto(CString Name, CString def) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("LPLightAuto", Name, def, str, 255, LocalCfgFile);
	if (strlen(str) != 3) {
		AfxMessageBox("Incorrect field in LPLight");
		return "";
	}
	return str;
}

void CDOS::SetLPLightAuto(CString Name, CString val) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("LPLightAuto", Name, val, LocalCfgFile);
}

CString CDOS::GetLTower(CString Name, CString def) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("LightTower", Name, def, str, 255, LocalCfgFile);
	if (strlen(str) != 3) {
		AfxMessageBox("Incorrect field in LightTower");
		return "";
	}
	return str;
}

void CDOS::SetLTower(CString Name, CString val) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("LightTower", Name, val, LocalCfgFile);
}

short CDOS::GetInCasLoc(short loc, short def) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("InCasLoc%d", loc);
	return GetPrivateProfileInt("LPUnit", str, def, LocalCfgFile);
}

void CDOS::SetInCasLoc(short loc, short val) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString sLoc;
	str.Format("%d", val);
	sLoc.Format("InCasLoc%d", loc);
	WritePrivateProfileString("LPUnit", sLoc, str, LocalCfgFile);
}

short CDOS::GetRobotPort() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("Ports", "RobotPort", 0, LocalCfgFile);
}

void CDOS::SetRobotPort(short port) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", port);
	WritePrivateProfileString("Ports", "RobotPort", str, LocalCfgFile);
}

// short CDOS::GetStorageType()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("FSM", "StorageType", 0, LocalCfgFile);
// }

// void CDOS::SetStorageType(short type)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", type);
// 	WritePrivateProfileString("FSM", "StorageType", str, LocalCfgFile);
// }

float CDOS::GetEncoderFactor(short Probe) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	CString name;
	name.Format("Encoder%d", Probe);
	GetPrivateProfileString("Calibration", name, "1.00", str, 255, LocalCfgFile);
	return float(atof(str));
}

void CDOS::SetEncoderFactor(short Probe, float factor) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString name;
	name.Format("Encoder%d", Probe);
	str.Format("%.6f", factor);
	WritePrivateProfileString("Calibration", name, str, LocalCfgFile);
}

float CDOS::GetTRiCorr() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Calibration", "TopRICorr", "1", str, 255, LocalCfgFile);
	return float(atof(str));
}

void CDOS::SetTRiCorr(float factor) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%.6f", factor);
	WritePrivateProfileString("Calibration", "TopRICorr", str, LocalCfgFile);
}

float CDOS::GetBRiCorr() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Calibration", "BottomRICorr", "1", str, 255, LocalCfgFile);
	return float(atof(str));
}

void CDOS::SetBRiCorr(float factor) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%.6f", factor);
	WritePrivateProfileString("Calibration", "BottomRICorr", str, LocalCfgFile);
}

BOOL CDOS::GetOS(CString& OS) {
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	CString WindowsPlatform;
	if (GetVersionEx(&osv)) {
		// note: szCSDVersion =  service pack  release
		CString ServiceRelease = osv.szCSDVersion;
		switch (osv.dwPlatformId) {
		case VER_PLATFORM_WIN32s: //Win32s on Windows 3.1.
			WindowsPlatform = "3.1";
			break;

		case VER_PLATFORM_WIN32_WINDOWS: //WIN32 on 95 or 98
			//determine if Win95 or Win98
			if (osv.dwMinorVersion == 0) {
				WindowsPlatform = "95 " + ServiceRelease;
			}
			else {
				WindowsPlatform = "98 " + ServiceRelease;
			}
			break;

		case VER_PLATFORM_WIN32_NT: //Win32 on Windows NT.
			WindowsPlatform = "NT " + ServiceRelease;
			break;

		default:
			AfxMessageBox("Failed to get correct Operating System.", MB_OK);
			return FALSE;
			break;
		} //end switch
	} //end if
	OS = WindowsPlatform;
	return TRUE;
}

// BOOL CDOS::GetCassetteSwitchSimu()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return (BOOL)GetPrivateProfileInt("OpenCassette","Switch",FALSE,LocalCfgFile);
// }

// void CDOS::SetCassetteSwitchSimu(BOOL bSimu)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d",bSimu);
// 	WritePrivateProfileString("OpenCassette","Switch",str,LocalCfgFile);
// }

// CDOS::OPENCASSETTESWITCHTYPE CDOS::GetOpenCassetteSwitchType()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return (CDOS::OPENCASSETTESWITCHTYPE)GetPrivateProfileInt("OpenCassette","Type",(short)OCTTI,LocalCfgFile);
// }

// void CDOS::SetOpenCassetteSwitchType(CDOS::OPENCASSETTESWITCHTYPE type)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d",type);
// 	WritePrivateProfileString("OpenCassette","Type",str,LocalCfgFile);
// }

// BOOL CDOS::GetBCRMode(BOOL mode)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	CString dft;
// 	char str[256];
// 	dft.Format("%d",mode);
// 	GetPrivateProfileString("LPUnit","BCRMode",dft,str,255,LocalSysFile);
// 	return BOOL(atoi(str));
// }

// void CDOS::WriteBCRMode(BOOL mode)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	sprintf(str,"%d",mode);
// 	WritePrivateProfileString("LPUnit","BCRMode",str,LocalSysFile);
// }

BOOL CDOS::GetHostVerSM() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("LPUnit", "HostVerSM", "0", str, 255, LocalSysFile);

	return BOOL(atoi(str));
}

void CDOS::WriteHostVerSM(BOOL mode) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	sprintf(str, "%d", mode);
	WritePrivateProfileString("LPUnit", "HostVerSM", str, LocalSysFile);
}

void CDOS::WriteRootDir(CString str) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("FSM", "RootDir", str, LocalSysFile);
}

CString CDOS::GetRootDir() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[255];
	GetPrivateProfileString("FSM", "RootDir", "C:\\DATA", str, 255, LocalSysFile);
	return str;
}

short CDOS::GetFanFilterUnit() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("FFU", "Installed", "0", str, 255, LocalCfgFile);
	return short(atoi(str));
}

void CDOS::WriteFanFilterUnit(short mode) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", mode);
	WritePrivateProfileString("FFU", "Installed", str, LocalCfgFile);
}

float CDOS::GetFFUPressure() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("FFU", "FFUPressure", "0.05", str, 255, LocalCfgFile);
	return float(atof(str));
}

void CDOS::SetFFUPressure(float Press) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%.4f", Press);
	WritePrivateProfileString("FFU", "FFUPressure", str, LocalCfgFile);
}

void CDOS::SetResetFreq(float Freq) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%.4f", Freq);
	WritePrivateProfileString("FSM", "ResetFreq", str, LocalCfgFile);
}

float CDOS::GetResetFreq() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("FSM", "ResetFreq", "36.0", str, 255, LocalCfgFile);
	return float(atof(str));
}

CString CDOS::GetFFUType() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("FFU", "Type", "GALIL", str, 255, LocalCfgFile);
	return str;
}

void CDOS::SetFFUType(CString type) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("FFU", "Type", type, LocalCfgFile);
}

BOOL CDOS::GetAutoConnect(BOOL bAuto) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("GEM", "AutoConnect", bAuto, LocalSysFile);
}

void CDOS::WriteAutoConnect(BOOL option) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", option);
	WritePrivateProfileString("GEM", "AutoConnect", str, LocalSysFile);
}

BOOL CDOS::GetAutoOnline(BOOL bAuto) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("GEM", "AutoOnline", bAuto, LocalSysFile);
}

void CDOS::WriteAutoOnline(BOOL option) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", option);
	WritePrivateProfileString("GEM", "AutoOnline", str, LocalSysFile);
}

short CDOS::GetJobOption() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("GEM", "JobControlledByCJ", "1", str, 255, LocalSysFile);
	return short(atoi(str));
}

void CDOS::WriteJobOption(short option) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", option);
	WritePrivateProfileString("GEM", "JobControlledByCJ", str, LocalSysFile);
}

// BOOL CDOS::GetTimeButton(BOOL dwfval)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	int ret = GetPrivateProfileInt("GEM", "TimeButton", dwfval, LocalSysFile);
// 	str.Format("%d", ret);
// 	WritePrivateProfileString("GEM", "TimeButton", str, LocalSysFile);
// 	return ret;
// }

BOOL CDOS::GetClampState() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	BOOL def = TRUE;
	return GetPrivateProfileInt("LPUnit", "ClampOnUnload", def, LocalCfgFile);
}

void CDOS::SetClampState(BOOL state) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", state);
	WritePrivateProfileString("LPUnit", "ClampOnUnload", str, LocalCfgFile);
}

float CDOS::GetStressFactor(float stressfactor) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	CString factor;
	factor.Format("%e", stressfactor);
	GetPrivateProfileString("Scanner", "StressFactor", factor, str, 255, LocalSysFile);
	return (float)atof(str);
}

void CDOS::SetStressFactor(float factor) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%e", factor);
	WritePrivateProfileString("Scanner", "StressFactor", str, LocalSysFile);
}

CString CDOS::GetStressUnit(CString unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Scanner", "StressUnit", unit, str, 255, LocalSysFile);
	return str;
}

void CDOS::SetStressUnit(CString unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("Scanner", "StressUnit", unit, LocalSysFile);
}

void CDOS::SetFilmThkFactor(float factor) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%e", factor);
	WritePrivateProfileString("Scanner", "FilmThkFactor", str, LocalSysFile);
}

float CDOS::GetFilmThkFactor(float stressfactor) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	CString factor;
	factor.Format("%e", stressfactor);
	GetPrivateProfileString("Scanner", "FilmThkFactor", factor, str, 255, LocalSysFile);
	return float(atof(str));
}

CString CDOS::GetFilmThkUnit(CString unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Scanner", "FilmThkUnit", unit, str, 255, LocalSysFile);
	return str;
}

void CDOS::SetFilmThkUnit(CString unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("Scanner", "FilmThkUnit", unit, LocalSysFile);
}

BOOL CDOS::GetEnterDoc(BOOL bDef) {
	CSingleLock Lock(&CS);
	Lock.Lock();

	char str[256];
	CString def;

	def.Format("%d", bDef);
	GetPrivateProfileString("Documentation", "AskForDoc", def, str, 255, LocalSysFile);
	return BOOL(atoi(str));
}

void CDOS::SetEnterDoc(BOOL bDef) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", bDef);
	WritePrivateProfileString("Documentation", "AskForDoc", str, LocalSysFile);
}

CString CDOS::GetLTMethod(CString def) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("LightTower", "Method", def, str, 255, LocalCfgFile);
	return str;
}

void CDOS::SetLTMethod(CString method) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("LightTower", "Method", method, LocalCfgFile);
}

void CDOS::SetStressConst(CString Konst) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	WritePrivateProfileString("Scanner", "StressConstant", Konst, LocalSysFile);
}

float CDOS::GetStressConst(CString def) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Scanner", "StressConstant", def, str, 255, LocalSysFile);
	return float(atof(str));
}

// float CDOS::GetBeampath(short lsr, float def)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	CString key = "BeamPath750";
// 	if (lsr == 2)
// 	{
// 		key = "BeamPath810";
// 	}
// 	char val[64];
// 	str.Format("%.2f",def);
// 	GetPrivateProfileString("Scanner",key,str,val,sizeof(val),LocalSysFile);
//
// 	return float(atof(val));
// }

// void CDOS::SetBeampath(short lsr, float val)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	CString key = "BeamPath750";
// 	if (lsr == 2)
// 	{
// 		key = "BeamPath810";
// 	}
// 	str.Format("%.2f",val);
// 	WritePrivateProfileString("Scanner",key,str,LocalSysFile);
// }

char* CDOS::GetRecipeDir() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	static CString mdir;
	GetProgramDir(mdir);
	mdir += "RECIPE";
	return mdir.GetBuffer(0);
}

// DAQ settings for Specific unit
float CDOS::GetEncoderRes(int UnitNo) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	CString Section;
	if (UnitNo) {
		Section.Format("%s%d", "DAQ", UnitNo);
	}
	else {
		Section = "DAQ";
	}
	GetPrivateProfileString(Section, "EncoderRes", "1.0", str, 255, FSMAppFile);
	return float(atof(str));
}

// float CDOS::GetDaqGain(int UnitNo)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	CString Section;
// 	if(UnitNo)
// 	{
// 		Section.Format("%s%d","DAQ",UnitNo);
// 	}
// 	else
// 	{
// 		Section = "DAQ";
// 	}
// 	GetPrivateProfileString(Section,"DaqGain","1.0",str,255,FSMAppFile);
// 	return float(atof(str));
// }

// unsigned char CDOS::BeamChannel(int UnitNo)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	CString Section;
// 	if(UnitNo)
// 	{
// 		Section.Format("%s%d","DAQ",UnitNo);
// 	}
// 	else
// 	{
// 		Section = "DAQ";
// 	}
// 	GetPrivateProfileString(Section,"BeamChannel","1",str,255,FSMAppFile);
// 	return unsigned char(atoi(str));
// }

// int CDOS::GetDaqDeviceID(int UnitNo)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	CString Section;
// 	if(UnitNo)
// 	{
// 		Section.Format("%s%d","DAQ",UnitNo);
// 	}
// 	else
// 	{
// 		Section = "DAQ";
// 	}
// 	GetPrivateProfileString(Section,"DeviceID","1",str,255,FSMAppFile);
// 	return (atoi(str));
// }

// unsigned char CDOS::GetAO750Chan(int UnitNo)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	CString Section;
// 	if(UnitNo)
// 	{
// 		Section.Format("%s%d","DAQ",UnitNo);
// 	}
// 	else
// 	{
// 		Section = "DAQ";
// 	}
// 	GetPrivateProfileString(Section,"AO750Channel","0",str,255,FSMAppFile);
// 	return (unsigned char)(atoi(str));
// }
//
// unsigned char CDOS::GetAO810Chan(int UnitNo)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	CString Section;
// 	if(UnitNo)
// 	{
// 		Section.Format("%s%d","DAQ",UnitNo);
// 	}
// 	else
// 	{
// 		Section = "DAQ";
// 	}
// 	DWORD Val =GetPrivateProfileString(Section,"AO810Channel","1",str,255,FSMAppFile);
// 	return (unsigned char)(atoi(str));
// }

// float CDOS::GetLaserMaxVolt(int UnitNo)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	CString Section;
// 	if(UnitNo)
// 	{
// 		Section.Format("%s%d","DAQ",UnitNo);
// 	}
// 	else
// 	{
// 		Section = "DAQ";
// 	}
// 	DWORD Val =GetPrivateProfileString(Section,"LaserMaxVolt","4.0",str,255,FSMAppFile);
// 	return (float)(atof(str));
// }
//
// float CDOS::GetPSDMinVolt(int UnitNo)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	CString Section;
// 	if(UnitNo)
// 	{
// 		Section.Format("%s%d","DAQ",UnitNo);
// 	}
// 	else
// 	{
// 		Section = "DAQ";
// 	}
// 	DWORD Val =GetPrivateProfileString(Section,"PSDMinVolt","1.0",str,255,FSMAppFile);
// 	return (float)(atof(str));
// }
//
// BOOL CDOS::IsLaserSwitchCyl()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	BOOL ret = GetPrivateProfileInt("Scanner", "LaserSwitchCyl", 0, LocalCfgFile);
// 	return ret;
// }
//
// void CDOS::SetLaserSwitchCyl(BOOL bCylinder)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", bCylinder);
// 	WritePrivateProfileString("Scanner", "LaserSwitchCyl", str, LocalCfgFile);
// }
//
// int CDOS::GetDIOMotor()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	int ret = GetPrivateProfileInt("Ports","SiMotor",0,LocalCfgFile);
// 	return ret;
// }
//
// void CDOS::SetDIOMotor(short Port)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", Port);
// 	WritePrivateProfileString("Ports", "SiMotor", str, LocalCfgFile);
// }

// short CDOS::GetSiMOtorPort()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	short ret = GetPrivateProfileInt("Ports", "SiDIO", 0, LocalCfgFile);
// 	SetSiMOtorPort(ret);
// 	return ret;
// }
//
// void CDOS::SetSiMOtorPort(short Port)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", Port);
// 	WritePrivateProfileString("Ports", "SiDIO", str, LocalCfgFile);
// }

// int CDOS::GetCommSenTorr()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("Ports","SenTorr",0,LocalCfgFile);
// }

// void CDOS::SetCommSenTorr(short Port)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", Port);
// 	WritePrivateProfileString("Ports", "SenTorr", str, LocalCfgFile);
// }

int CDOS::GetSignalTowerPort() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("Ports", "SignalTowerPort", 0, LocalCfgFile);
}

BOOL CDOS::SetSignalTowerPort(unsigned char Port) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", Port);
	return WritePrivateProfileString("Ports", "SignalTowerPort", str, LocalCfgFile);
}

int CDOS::GetCasRepeat() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("Scanner", "CasRepeat", 0, FSMAppFile);
}

BOOL CDOS::SetCasRepeat(short cnt) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", cnt);
	return WritePrivateProfileString("Scanner", "CasRepeat", str, FSMAppFile);
}

int CDOS::GetScanRepeat() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("Scanner", "ScanRepeat", 0, FSMAppFile);
}

BOOL CDOS::SetScanRepeat(short cnt) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", cnt);
	return WritePrivateProfileString("Scanner", "ScanRepeat", str, FSMAppFile);
}

// BOOL CDOS::SetPLCSimuI0(DWORD Flag)
// {
// 	str.Format("%u",Flag);
// 	return 	(TRUE == WritePrivateProfileString("PLC","SimuI0",str,LocalSysFile));
// }

// BOOL CDOS::SetPLCSimuI1(DWORD Flag)
// {
// 	str.Format("%u",Flag);
// 	return 	(TRUE == WritePrivateProfileString("PLC","SimuI1",str,LocalSysFile));
// }

// BOOL CDOS::SetPLCSimuO0(DWORD Flag)
// {
// 	str.Format("%u",Flag);
// 	return 	(TRUE == WritePrivateProfileString("PLC","SimuO0",str,LocalSysFile));
// }

// BOOL CDOS::SetPLCSimuB3_0(DWORD Flag)
// {
// 	str.Format("%u",Flag);
// 	return 	(TRUE == WritePrivateProfileString("PLC","SimuB3_0",str,LocalSysFile));
// }

// BOOL CDOS::SetPLCSimuB3_1(DWORD Flag)
// {
// 	str.Format("%u",Flag);
// 	return 	(TRUE == WritePrivateProfileString("PLC","SimuB3_1",str,LocalSysFile));
// }

// BOOL CDOS::SetPLCSimuB3_2(DWORD Flag)
// {
// 	str.Format("%u",Flag);
// 	return 	(TRUE == WritePrivateProfileString("PLC","SimuB3_2",str,LocalSysFile));
// }

// BOOL CDOS::SetPLCSimuB3_3(DWORD Flag)
// {
// 	str.Format("%u",Flag);
// 	return 	(TRUE == WritePrivateProfileString("PLC","SimuB3_3",str,LocalSysFile));
// }

// BOOL CDOS::ReadPLCSimuDefaluts(DWORD &I0,DWORD &I1,DWORD &O0,DWORD &B3_0,
// 		 DWORD &B3_1,DWORD &B3_2,DWORD &B3_3)
// {
// 	char Ret[52];
// 	GetPrivateProfileString("PLC","SimuI0_def","0",Ret,50,LocalSysFile);
// 	I0 = atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuI1_def","0",Ret,50,LocalSysFile);
// 	I1 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuO0_def","0",Ret,50,LocalSysFile);
// 	O0 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuB3_0_def","0",Ret,50,LocalSysFile);
// 	B3_0 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuB3_1_def","0",Ret,50,LocalSysFile);
// 	B3_1 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuB3_2_def","0",Ret,50,LocalSysFile);
// 	B3_2 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuB3_3_def","0",Ret,50,LocalSysFile);
// 	B3_3 =atoi(Ret);
//
// 	return TRUE;
// }

// BOOL CDOS::ReadPLCSimu(DWORD &I0,DWORD &I1,DWORD &O0,DWORD &B3_0,
// 							   DWORD &B3_1,DWORD &B3_2,DWORD &B3_3)
// {
// 	char Ret[52];
// 	GetPrivateProfileString("PLC","SimuI0","0",Ret,50,LocalSysFile);
// 	WritePrivateProfileString("PLC","SimuI0",Ret,LocalSysFile);
// 	I0 = atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuI1","0",Ret,50,LocalSysFile);
// 	WritePrivateProfileString("PLC","SimuI1",Ret,LocalSysFile);
// 	I1 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuO0","0",Ret,50,LocalSysFile);
// 	WritePrivateProfileString("PLC","SimuO0",Ret,LocalSysFile);
// 	O0 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuB3_0","0",Ret,50,LocalSysFile);
// 	WritePrivateProfileString("PLC","SimuB3_0",Ret,LocalSysFile);
// 	B3_0 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuB3_1","0",Ret,50,LocalSysFile);
// 	WritePrivateProfileString("PLC","SimuB3_1",Ret,LocalSysFile);
// 	B3_1 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuB3_2","0",Ret,50,LocalSysFile);
// 	WritePrivateProfileString("PLC","SimuB3_2",Ret,LocalSysFile);
// 	B3_2 =atoi(Ret);
//
// 	GetPrivateProfileString("PLC","SimuB3_3","0",Ret,50,LocalSysFile);
// 	WritePrivateProfileString("PLC","SimuB3_3",Ret,LocalSysFile);
// 	B3_3 =atoi(Ret);
//
// 	return TRUE;
// }

// BOOL CDOS::GetPLCSimu()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("PLC", "bSimu", TRUE, LocalCfgFile);
// }

// void CDOS::SetPLCSimu(BOOL bSimu)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d",bSimu);
// 	WritePrivateProfileString("PLC","bSimu",str,LocalCfgFile);
// }

BOOL CDOS::GetCIDRSimu(short unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	sec.Format("CIDRUnit%d", unit);
	return GetPrivateProfileInt(sec, "bSimu", TRUE, LocalCfgFile);
}

void CDOS::SetCIDRSimu(BOOL bSimu, short unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	sec.Format("CIDRUnit%d", unit);
	str.Format("%d", bSimu);
	WritePrivateProfileString(sec, "bSimu", str, LocalCfgFile);
}

// BOOL CDOS::SetPLCSimuDefaluts(DWORD I0,DWORD I1,DWORD O0,DWORD B3_0,
// 						DWORD B3_1,DWORD B3_2,DWORD B3_3)
// {
// 	str.Format("%u",I0);
// 	WritePrivateProfileString("PLC","SimuI0_def",str,LocalSysFile);
//
// 	str.Format("%u",I1);
// 	WritePrivateProfileString("PLC","SimuI1_def",str,LocalSysFile);
//
// 	str.Format("%u",O0);
// 	WritePrivateProfileString("PLC","SimuO0_def",str,LocalSysFile);
//
// 	str.Format("%u",B3_0);
// 	WritePrivateProfileString("PLC","SimuB3_0_def",str,LocalSysFile);
//
// 	str.Format("%u",B3_1);
// 	WritePrivateProfileString("PLC","SimuB3_1_def",str,LocalSysFile);
//
// 	str.Format("%u",B3_2);
// 	WritePrivateProfileString("PLC","SimuB3_2_def",str,LocalSysFile);
//
// 	str.Format("%u",B3_3);
// 	WritePrivateProfileString("PLC","SimuB3_3_def",str,LocalSysFile);
// 	return true;
// }

// BOOL CDOS::GetSenTorrUnitSimu()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("SenTorrUnit", "bSimu", TRUE, LocalCfgFile);
// }

// void CDOS::SetSenTorrUnitSimu(BOOL bSimu)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", bSimu);
// 	WritePrivateProfileString("SenTorrUnit", "bSimu", str, LocalCfgFile);
// }

// BOOL CDOS::GetDIOSimu()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("DIOUnit", "bSimu", TRUE, LocalCfgFile);
// }
//
// void CDOS::SetDIOSimu(BOOL bSimu)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", bSimu);
// 	WritePrivateProfileString("DIOUnit", "bSimu", str, LocalCfgFile);
// }

// BOOL CDOS::GetDaqSimu()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt( "DaqUnit", "bSimu", TRUE, LocalCfgFile);
// }

// void CDOS::SetDaqSimu(BOOL bSimu)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", bSimu);
// 	WritePrivateProfileString("DaqUnit", "bSimu", str, LocalCfgFile);
// }

// float CDOS::GetRoughingPressure()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char Str[256] = {0};
// 	GetPrivateProfileString("SenTorrUnit","RoughingPressure","5",Str,255,LocalCfgFile);
// 	WritePrivateProfileString("SenTorrUnit","RoughingPressure",Str,LocalCfgFile);
// 	return (float)::atof(Str);
// }

// float CDOS::GetHoldingPressure()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char Str[256] = {0};
// 	GetPrivateProfileString("SenTorrUnit","HoldingPressure",".05",Str,255,LocalCfgFile);
// 	WritePrivateProfileString("SenTorrUnit","HoldingPressure",Str,LocalCfgFile);
// 	return (float)::atof(Str);
// }

// float CDOS::GetIGSensitivity()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char Str[256] = {0};
// 	GetPrivateProfileString("SenTorrUnit","IGSensitivity","1.00",Str,255,LocalCfgFile);
// 	WritePrivateProfileString("SenTorrUnit","IGSensitivity",Str,LocalCfgFile);
// 	return (float)::atof(Str);
// }

// float CDOS::GetIGGasCorrection()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char Str[256] = {0};
// 	GetPrivateProfileString("SenTorrUnit","IGGasCorrection","1.00",Str,255,LocalCfgFile);
// 	WritePrivateProfileString("SenTorrUnit","IGGasCorrection",Str,LocalCfgFile);
// 	return (float)::atof(Str);
// }

// float CDOS::GetAutoOnPressure()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char Str[256] = {0};
// 	GetPrivateProfileString("SenTorrUnit","AutoOnPressure",".018",Str,255,LocalCfgFile);
// 	WritePrivateProfileString("SenTorrUnit","AutoOnPressure",Str,LocalCfgFile);
// 	return (float)::atof(Str);
// }

// short CDOS::GetWDTPort()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("Ports","WDT",0,LocalCfgFile);
// }

// void CDOS::SetWDTPort(short loc)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d",loc);
// 	WritePrivateProfileString("Ports","WDT",str,LocalCfgFile);
// }

long CDOS::GetAlignerPostPos(short pos) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("POSTPOS%d", pos);
	return GetPrivateProfileInt("BrkUnit", str, 0, LocalCfgFile);
}

//[21082024/Tushar
short CDOS::GetLogosolAlignerPort() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str = "Port";
	return GetPrivateProfileInt("LogosolUnit", str, 0, LocalCfgFile);
	//return GetPrivateProfileInt("BrkUnit", str, 0, LocalCfgFile);
}
//]
void CDOS::SetAlignerPostPos(short pos, long post) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString pstr;
	str.Format("%ld", post);
	pstr.Format("POSTPOS%d", pos);
	WritePrivateProfileString("BrkUnit", pstr, str, LocalCfgFile);
}

long CDOS::GetAlignerOffsetPos(short pos) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("OFFSETPOS%d", pos);
	return GetPrivateProfileInt("BrkUnit", str, 0, LocalCfgFile);
}
//[21082024/Tushar
long CDOS::GetLogosolAlignerPostPosAngle() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("LogosolUnit", "POSTPOSANG", 0, LocalCfgFile);
}

short CDOS::GetLogosolCCDNum(short arm) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	short Num = 1;
	switch (arm) {
	case 1:
		str = "CCDNUMA";
		break;
	case 2:
		str = "CCDNUMB";
		break;
	default:
		ASSERT(0);
		return 0;
		break;
	}
	return GetPrivateProfileInt("LogosolUnit", str, Num, LocalCfgFile);
}
//]
void CDOS::SetAlignerOffsetPos(short pos, long offset) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString pstr;
	str.Format("%ld", offset);
	pstr.Format("OFFSETPOS%d", pos);
	WritePrivateProfileString("BrkUnit", pstr, str, LocalCfgFile);
}

long CDOS::GetCCDPos(short arm) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	long postpos;
	switch (arm) {
	case 1:
		str = "CCDPOSA";
		postpos = GetPrivateProfileInt("BrkUnit", str, 340000, LocalCfgFile);
		break;
	case 2:
		str = "CCDPOSB";
		postpos = GetPrivateProfileInt("BrkUnit", str, 62000, LocalCfgFile);
		break;
	default:
		ASSERT(0);
		return 0;
		break;
	}
	return postpos;
}

void CDOS::SetCCDPos(short arm, long offset) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString pstr;
	str.Format("%ld", offset);
	switch (arm) {
	case 1:
		pstr = "CCDPOSA";
		break;
	case 2:
		pstr = "CCDPOSB";
		break;
	default:
		ASSERT(0);
		return;
	}
	WritePrivateProfileString("BrkUnit", pstr, str, LocalCfgFile);
}

short CDOS::GetCCDNum(short arm) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	short Num = 1;
	switch (arm) {
	case 1:
		str = "CCDNUMA";
		break;
	case 2:
		str = "CCDNUMB";
		break;
	default:
		ASSERT(0);
		return 0;
		break;
	}
	return GetPrivateProfileInt("BrkUnit", str, Num, LocalCfgFile);
}

void CDOS::SetCCDNum(short arm, short Num) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString pstr;
	str.Format("%d", Num);
	switch (arm) {
	case 1:
		pstr = "CCDNUMA";
		break;
	case 2:
		pstr = "CCDNUMB";
		break;
	default:
		ASSERT(0);
		return;
	}
	WritePrivateProfileString("BrkUnit", pstr, str, LocalCfgFile);
}

// BOOL CDOS::GetWDTSimu()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("WDTUnit", "bSimu", TRUE, LocalCfgFile);
// }

// void CDOS::SetWDTSimu(BOOL bSimu)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", bSimu);
// 	WritePrivateProfileString("WDTUnit", "bSimu", str, LocalCfgFile);
// }

BOOL CDOS::GetRbtUnitSimu() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("BrkUnit", "bSimu", TRUE, LocalCfgFile);
}

void CDOS::SetRbtUnitSimu(BOOL bSimu) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", bSimu);
	WritePrivateProfileString("BrkUnit", "bSimu", str, LocalCfgFile);
}

BOOL CDOS::GetLPUnitSimu(short unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	sec.Format("LPUnit%d", unit);
	return GetPrivateProfileInt(sec, "bSimu", TRUE, LocalCfgFile);
}

void CDOS::SetLPUnitSimu(BOOL bSimu, short unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	sec.Format("LPUnit%d", unit);
	str.Format("%d", bSimu);
	WritePrivateProfileString(sec, "bSimu", str, LocalCfgFile);
}

// short CDOS::GetVacOffTempr()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("Process", "VentTemperature", 200, LocalSysFile);
// }

// void CDOS::SetVacOffTempr(short tempr)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", tempr);
// 	WritePrivateProfileString("Process", "VentTemperature", str, LocalSysFile);
// }

// BOOL CDOS::GetEnableModeSwitch()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("Process", "EnableModeSwitch", FALSE, LocalSysFile);
// }

// void CDOS::SetEnableModeSwitch(BOOL bEna)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", bEna);
// 	WritePrivateProfileString("Process", "EnableModeSwitch", str, LocalSysFile);
// }

short CDOS::GetLogLevel(void) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("FSM", "LogLevel", "10", str, 255, LocalSysFile);
	return short(atoi(str));
}

void CDOS::WriteLogLevel(short level) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", level);
	WritePrivateProfileString("FSM", "LogLevel", str, LocalSysFile);
}

void CDOS::SetNumLP(short num) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", num);
	WritePrivateProfileString("FSM", "NoLoadPort", str, LocalSysFile);
}

short CDOS::GetNumLP() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("FSM", "NoLoadPort", 0, LocalSysFile);
}

short CDOS::GetNumCIDR() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("FSM", "NoCIDR", 0, LocalSysFile);
}

void CDOS::SetNumCIDR(short num) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", num);
	WritePrivateProfileString("FSM", "NoCIDR", str, LocalSysFile);
}

short CDOS::GetLPPort(short which) {
	short port = 0;
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("LPPort%d", which);
	port = GetPrivateProfileInt("Ports", str, port, LocalCfgFile);
	return port;
}

void CDOS::SetLPPort(short which, short port) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString str2;
	str.Format("%d", port);
	str2.Format("LPPort%d", which);
	WritePrivateProfileString("Ports", str2, str, LocalCfgFile);
}

short CDOS::GetSiNetPort() {
	short port = 0;
	CSingleLock Lock(&CS);
	Lock.Lock();
	port = GetPrivateProfileInt("Ports", "SiNetHubPort", port, LocalCfgFile);
	return port;
}

void CDOS::SetSiNetPort(short port) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", port);
	WritePrivateProfileString("Ports", "SiNetHubPort", str, LocalCfgFile);
}

short CDOS::GetSiDrvPort() {
	short port = 0;
	CSingleLock Lock(&CS);
	Lock.Lock();
	port = GetPrivateProfileInt("4PP", "ProbePort", port, "FP.INI");
	return port;
}

void CDOS::SetSiDrvPort(short port) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", port);
	WritePrivateProfileString("4PP", "ProbePort", str, "FP.INI");
}

short CDOS::GetCIDRPort(short which) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("CIDRPort%d", which);
	return GetPrivateProfileInt("Ports", str, 0, LocalCfgFile);
}

void CDOS::SetCIDRPort(short which, short port) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString str2;
	str.Format("%d", port);
	str2.Format("CIDRPort%d", which);
	WritePrivateProfileString("Ports", str2, str, LocalCfgFile);
}

short CDOS::GetCIDRUnit(short which) {
	str.Format("CIDR%dID", which);
	return GetPrivateProfileInt("CIDRUnit", str, 0, LocalCfgFile);
}

void CDOS::SetCIDRUnit(short which, short id) {
	CString str2;
	str.Format("%d", id);
	str2.Format("CIDR%dID", which);
	WritePrivateProfileString("CIDRUnit", str2, str, LocalCfgFile);
}

// short CDOS::GetAlignerCtrlPort()
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("Ports", "AlignerCtrl", 0, LocalCfgFile);
// }
//
// void CDOS::SetAlignerCtrlPort(short port)
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	str.Format("%d", port);
// 	WritePrivateProfileString("Ports", "AlignerCtrl", str, LocalCfgFile);
// }
//
// BOOL CDOS::GetEnaAlignerCtrl()
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("RobotUnit", "EnaAlignerCtrl", TRUE, LocalCfgFile);
// }
//
// void CDOS::SetEnaAlignerCtrl(BOOL bEna)
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	str.Format("%d", bEna);
// 	WritePrivateProfileString("RobotUnit", "EnaAlignerCtrl", str, LocalCfgFile);
// }

float CDOS::GetXSPMM() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Scanner", "SPMM", "41.99475", str, 255, FSMAppFile);
	float ret = float(atof(str));
	Lock.Unlock();
	SetXSPMM(ret);
	return ret;
}

float CDOS::GetZSPMM() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Detector", "SPMM", "1259.84252", str, 255, FSMAppFile);
	float ret = float(atof(str));
	Lock.Unlock();
	SetZSPMM(ret);
	return ret;
}

void CDOS::SetXSPMM(float val) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%.4f", val);
	WritePrivateProfileString("Scanner", "SPMM", str, FSMAppFile);
}

void CDOS::SetZSPMM(float val) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%.4f", val);
	WritePrivateProfileString("Detector", "SPMM", str, FSMAppFile);
}

// Carrier ID is used as the indicator that some process is on
void CDOS::RememberPrJob(short unit, CString& CarrierId, CString& Recipe) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString sect, str;

	if ((unit >= 1) && (unit <= 4)) {
		sect.Format("ActiveJob%d", unit);
		WritePrivateProfileString(sect, "CarrierID", CarrierId, LocalSysFile);
		WritePrivateProfileString(sect, "Recipe", Recipe, LocalSysFile);
		str.Format("%d", 1);
		WritePrivateProfileString(sect, "Module", str, LocalSysFile);
	}
}

void CDOS::ForgetPrJob(short unit) {
	CString sect, str;
	if ((unit >= 1) && (unit <= 4)) {
		sect.Format("ActiveJob%d", unit);
		WritePrivateProfileString(sect, "CarrierID", "", LocalSysFile);
		WritePrivateProfileString(sect, "Recipe", "", LocalSysFile);
		str = "0";
		WritePrivateProfileString(sect, "Module", str, LocalSysFile);
	}
}

void CDOS::GetRememberedPrJob(short unit, CString& CarrierId, CString& Recipe) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString sect;

	if ((unit >= 1) && (unit <= 4)) {
		sect.Format("ActiveJob%d", unit);
		char str[256];
		GetPrivateProfileString(sect, "CarrierID", "", str, 255, LocalSysFile);
		CarrierId = str;
		GetPrivateProfileString(sect, "Recipe", "", str, 255, LocalSysFile);
		Recipe = str;
		short Val = 0;
		Val = GetPrivateProfileInt(sect, "Module", Val, LocalSysFile);
	}
}

void CDOS::RememberSM(short unit, CString SM) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString sect, str;

	if ((unit >= 1) && (unit <= 4)) {
		sect.Format("ActiveJob%d", unit);
		WritePrivateProfileString(sect, "SlotMap", SM, LocalSysFile);
	}
}

void CDOS::ForgetSM(short unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString sect, str;

	if ((unit >= 1) && (unit <= 4)) {
		sect.Format("ActiveJob%d", unit);
		WritePrivateProfileString(sect, "SlotMap", "", LocalSysFile);
	}
}

CString CDOS::GetRememberedSM(short unit) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	CString sect;

	if ((unit >= 1) && (unit <= 4)) {
		sect.Format("ActiveJob%d", unit);
		char str[256];
		GetPrivateProfileString(sect, "SlotMap", "", str, 255, LocalSysFile);
		return str;
	}
	return "";
}

// short CDOS::GetBakeTempr()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("Process", "BakeTemperature", 200, LocalSysFile);
// }

// void CDOS::SetBakeTempr(short tempr)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", tempr);
// 	WritePrivateProfileString("Process", "BakeTemperature", str, LocalSysFile);
// }

// float CDOS::GetTargetVac()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Process","TargetVac","1e-4",str,255,LocalSysFile);
// 	return float(atof(str));
// }

// void CDOS::SetTargetVac(float fac)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%0.2e",fac);
// 	WritePrivateProfileString("Process","TargetVac",str,LocalSysFile);
// }

// float CDOS::GetAtmVac()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Process","AtmVac","1e5",str,255,LocalSysFile);
// 	return float(atof(str));
// }

// void CDOS::SetAtmVac(float fac)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%0.2e",fac);
// 	WritePrivateProfileString("Process","AtmVac",str,LocalSysFile);
// }

// short CDOS::GetColdTempr()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("Process", "ColdTemperature", 80, LocalSysFile);
// }

// void CDOS::SetColdTempr(short tempr)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", tempr);
// 	WritePrivateProfileString("Process", "ColdTemperature", str, LocalSysFile);
// }

// BOOL CDOS::GetDIOEcho()
// {
// 	BOOL bEcho = TRUE;
// 	CString sect;
// 	sect = "DIOUnit";
// 	bEcho = GetPrivateProfileInt(sect, "Echo", bEcho, LocalCfgFile);
// 	SetDIOEcho(bEcho);
// 	return bEcho;
// }

// void CDOS::SetDIOEcho(BOOL bEcho)
// {
// 	CString str;
// 	CString sect;
// 	sect = "DIOUnit";
// 	str.Format("%d", bEcho);
// 	WritePrivateProfileString(sect, "Echo", str, LocalCfgFile);
// }

CString CDOS::GetLogDirectory() {
	CString dir;
	DosUtil.GetProgramDir(dir);

	dir += "Log\\";

	int nTemp = ::GetFileAttributes(dir);
	if (!(FILE_ATTRIBUTE_DIRECTORY & nTemp) || (nTemp == -1)) {
		if (!::CreateDirectory(dir, NULL)) {
			return "";
		}
	}
	dir += CTime::GetCurrentTime().Format("Y%y%B%d\\");
	nTemp = ::GetFileAttributes(dir);
	if (!(FILE_ATTRIBUTE_DIRECTORY & nTemp) || (nTemp == -1)) {
		if (!::CreateDirectory(dir, NULL)) {
			return "";
		}
	}
	return dir;
}

// short CDOS::GetTCPort()
// {
// 	short port = 0;
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	port = GetPrivateProfileInt("TemperatureControl", "CommPort", port, LocalCfgFile);
// 	return port;
// }

// void CDOS::SetTCPort(short port)
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	CString str2;
// 	str.Format("%d", port);
// 	WritePrivateProfileString("TemperatureControl", "CommPort", str, LocalCfgFile);
// }

int CDOS::GetIgnorRecipeParameter() {
	return GetPrivateProfileInt("GEM", "IgnorRecipeParameter", 0, LocalSysFile);
}

BOOL CDOS::SetIgnorRecipeParameter(short cnt) {
	str.Format("%d", cnt);
	return WritePrivateProfileString("GEM", "IgnorRecipeParameter", str, LocalSysFile);
}

int CDOS::GetJobAlertSupport() {
	return GetPrivateProfileInt("GEM", "JobAlertSupport", 1, LocalSysFile);
}

BOOL CDOS::SetJobAlertSupport(short cnt) {
	str.Format("%d", cnt);
	return WritePrivateProfileString("GEM", "JobAlertSupport", str, LocalSysFile);
}

int CDOS::GetJobEventSupport() {
	return GetPrivateProfileInt("GEM", "JobEventSupport", 1, LocalSysFile);
}

BOOL CDOS::SetJobEventSupport(short cnt) {
	str.Format("%d", cnt);
	return WritePrivateProfileString("GEM", "JobEventSupport", str, LocalSysFile);
}

short CDOS::GetFlipMapX() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Map2D", "FlipMapX", "0", str, 255, LocalSysFile);
	return atoi(str);
}

void CDOS::SetFlipMapX(short pos) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", pos);
	WritePrivateProfileString("Map2D", "FlipMapX", str, LocalSysFile);
}

short CDOS::GetNotchAngle() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Map2D", "NotchAngle", "0", str, 255, LocalSysFile);
	return atoi(str);
}

void CDOS::SetNotchAngle(short pos) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", pos);
	WritePrivateProfileString("Map2D", "NotchAngle", str, LocalSysFile);
}

// short CDOS::GetSPL()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Map2D","SPL","24",str,255,LocalSysFile);
// 	return atoi(str);
// }

// void CDOS::SetSPL(short pos)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", pos);
// 	WritePrivateProfileString("Map2D", "SPL", str, LocalSysFile);
// }

// short CDOS::GetPixelRes()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Map2D","PixelRes","64",str,255,LocalSysFile);
// 	return atoi(str);
// }

// void CDOS::SetPixelRes(short pos)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", pos);
// 	WritePrivateProfileString("Map2D", "PixelRes", str, LocalSysFile);
// }

// short CDOS::GetOrder()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Map2D","Order","8",str,255,LocalSysFile);
// 	return atoi(str);
// }

// void CDOS::SetOrder(short pos)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", pos);
// 	WritePrivateProfileString("Map2D", "Order", str, LocalSysFile);
// }

// short CDOS::GetShowDataPoint()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Map2D","ShowDataPoint","0",str,255,LocalSysFile);
// 	return atoi(str);
// }

// void CDOS::SetShowDataPoint(short pos)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", pos);
// 	WritePrivateProfileString("Map2D", "ShowDataPoint", str, LocalSysFile);
// }

BOOL CDOS::GetFlattenMap() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	char str[256];
	GetPrivateProfileString("Map2D", "Flatten", "0", str, 255, LocalSysFile);
	return BOOL(atoi(str));
}

void CDOS::SetFlattenMap(BOOL pos) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", pos);
	WritePrivateProfileString("Map2D", "Flatten", str, LocalSysFile);
}

// short CDOS::GetDensity()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Map2D","Density","24",str,255,LocalSysFile);
// 	return atoi(str);
// }
//
// void CDOS::SetDensity(short pos)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", pos);
// 	WritePrivateProfileString("Map2D", "Density", str, LocalSysFile);
// }

// float CDOS::GetMapWeight()
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Map2D","Weight","16.0",str,255,LocalSysFile);
// 	float ret = float(atof(str));
// 	return ret;
// }
//
// void CDOS::SetMapWeight(float val)
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	str.Format("%.2f", val);
// 	WritePrivateProfileString("Map2D", "Weight", str, LocalSysFile);
// }

BOOL CDOS::GetWaferPresent() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	BOOL def = FALSE;
	return GetPrivateProfileInt("Persist", "WaferPresent", def, LocalSysFile);
}

void CDOS::SetWaferPresent(BOOL state) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", state);
	WritePrivateProfileString("Persist", "WaferPresent", str, LocalSysFile);
}

short CDOS::GetDumpCounter() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	return GetPrivateProfileInt("Persist", "DumpCounter", 1, LocalSysFile);
}

void CDOS::SetDumpCounter(short state) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	str.Format("%d", state);
	WritePrivateProfileString("Persist", "DumpCounter", str, LocalSysFile);
}

// short CDOS::GetSPCMethod(short module)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	BOOL def = FALSE;
// 	CString method;
// 	method.Format("SPCMethod%d",module);
// 	return GetPrivateProfileInt("Variables",method, def, LocalSysFile);
// }
//
// void CDOS::SetSPCMethod(short module, short state)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", state);
// 	CString method;
// 	method.Format("SPCMethod%d",module);
// 	WritePrivateProfileString("Variables", method, str, LocalSysFile);
// }

// short CDOS::GetWaferPresentABLimit()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	return GetPrivateProfileInt("ScanUnit", "WaferPresentThreshold", 80, LocalSysFile);
// }

// void CDOS::SetWaferPresentABLimit(short limit)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", limit);
// 	WritePrivateProfileString("ScanUnit", "WaferPresentThreshold", str, LocalSysFile);
// }

// void CDOS::SetLaserName1(short num, short Freq)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%d", Freq);
// 	CString str2;
// 	str2.Format("Laser%d",num);
// 	WritePrivateProfileString("Variables", str2, str, LocalSysFile);
// }

// short CDOS::GetLaserName1(short num, short deft)
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	CString str2;
// 	str2.Format("Laser%d",num);
// 	return GetPrivateProfileInt("Variables", str2, deft, LocalSysFile);
// }

void CDOS::SetWaferOnStage(short No) {
	str.Format("%d", No);
	WritePrivateProfileString("ScanUnit", "WaferOnStage", str, LocalSysFile);
}

short CDOS::GetWaferOnStage(short No) {
	return GetPrivateProfileInt("ScanUnit", "WaferOnStage", No, LocalSysFile);
}

void CDOS::SetLoadPosConfType(short No) {
	str.Format("%d", No);
	WritePrivateProfileString("FSM", "LoadPosConfMethod", str, LocalCfgFile);
}

short CDOS::GetLoadPosConfType(short No) {
	return GetPrivateProfileInt("FSM", "LoadPosConfMethod", No, LocalCfgFile);
}

void CDOS::SetCheckWaferPresent(BOOL bCheck) {
	str.Format("%d", bCheck);
	WritePrivateProfileString("FSM", "CheckWaferPresent", str, LocalCfgFile);
}

short CDOS::GetCheckWaferPresent(BOOL bCheck) {
	return GetPrivateProfileInt("FSM", "CheckWaferPresent", bCheck, LocalCfgFile);
}

void CDOS::GetSetLPLight(CString str) {
	SetLPLight(str, GetLPLight(str, "000"));
	SetLPLightAuto(str, GetLPLightAuto(str, "000"));
}

void CDOS::SetWaferSize(float No) {
	str.Format("%.4f", No);
	WritePrivateProfileString("ScanUnit", "WaferSize", str, LocalSysFile);
}

float CDOS::GetWaferSize(float No) {
	char str[256];
	CString val;
	val.Format("%.4f", No);
	GetPrivateProfileString("ScanUnit", "WaferSize", val, str, 255, LocalSysFile);
	return atof(str);
	//return GetPrivateProfileInt("ScanUnit", "WaferSize", No, LocalSysFile);
}

void CDOS::SetTgtRed(CFPoint& PtTF, CFPoint& PtEC) {
	str.Format("%.6f", PtTF.X);
	WritePrivateProfileString("ScanUnit", "TgtRedTFX", str, LocalSysFile);
	str.Format("%.6f", PtTF.Y);
	WritePrivateProfileString("ScanUnit", "TgtRedTFY", str, LocalSysFile);
	str.Format("%.6f", PtEC.X);
	WritePrivateProfileString("ScanUnit", "TgtRedEchoX", str, LocalSysFile);
	str.Format("%.6f", PtEC.Y);
	WritePrivateProfileString("ScanUnit", "TgtRedEchoY", str, LocalSysFile);
}

void CDOS::GetTgtRed(CFPoint& PtTF, CFPoint& PtEC) {
	char str[256];
	GetPrivateProfileString("ScanUnit", "TgtRedTFX", "", str, 255, LocalSysFile);
	PtTF.X = atof(str);
	GetPrivateProfileString("ScanUnit", "TgtRedTFY", "", str, 255, LocalSysFile);
	PtTF.Y = atof(str);
	GetPrivateProfileString("ScanUnit", "TgtRedEchoX", "", str, 255, LocalSysFile);
	PtEC.X = atof(str);
	GetPrivateProfileString("ScanUnit", "TgtRedEchoY", "", str, 255, LocalSysFile);
	PtEC.Y = atof(str);
}

CString CDOS::GetRecipeButtonStr(int RcpNum) {
	char str[256];
	CString strNum;
	strNum.Format("Button%d", RcpNum);
	GetPrivateProfileString("RecipeButton", strNum, "", str, 255, LocalSysFile);
	return str;
}

void CDOS::SetRecipeButtonStr(int RcpNum, CString RcpName) {
	CString strNum;
	strNum.Format("Button%d", RcpNum);
	WritePrivateProfileString("RecipeButton", strNum, RcpName, LocalSysFile);
}

// short CDOS::GetPkSpc()
// {
// 	short Spc = 8;
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	Spc = GetPrivateProfileInt("Echo", "PkSpc", Spc, LocalCfgFile);
// 	return Spc;
// }

// void CDOS::SetPkSpc(short Spc)
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	CString str2;
// 	str.Format("%d", Spc);
// 	WritePrivateProfileString("Echo", "PkSpc", str, LocalCfgFile);
// }

// short CDOS::GetPkHeight()
// {
// 	short Spc = 16;
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	Spc = GetPrivateProfileInt("Echo", "PkHeight", Spc, LocalCfgFile);
// 	return Spc;
// }

// void CDOS::SetPkHeight(short Spc)
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	CString str2;
// 	str.Format("%d", Spc);
// 	WritePrivateProfileString("Echo", "PkHeight", str, LocalCfgFile);
// }

// short CDOS::GetPkCln()
// {
// 	short Spc = 16;
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	Spc = GetPrivateProfileInt("Echo", "PkClean", Spc, LocalCfgFile);
// 	return Spc;
// }

// void CDOS::SetPkCln(short Spc)
// {
// 	CSingleLock Lock(& CS);
// 	Lock.Lock();
// 	CString str2;
// 	str.Format("%d", Spc);
// 	WritePrivateProfileString("Echo", "PkClean", str, LocalCfgFile);
// }

//added AP 20111119
// float CDOS::GetZgapEM()
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	char str[256];
// 	GetPrivateProfileString("Z1Motor","ZgapEM","0",str,255,LocalCfgFile);
// 	return float(atof(str));
// }

// void CDOS::SetZgapEM(float val)
// {
// 	CSingleLock Lock(&CS);
// 	Lock.Lock();
// 	str.Format("%.3f",val);
// 	WritePrivateProfileString("Z1Motor", "ZgapEM", str, LocalCfgFile);
// }

short CDOS::GetAutoExposeStepTF(short stp) {
	return GetPrivateProfileInt("Video", "AutoExposeStepTF", stp, LocalCfgFile);
}

void CDOS::SetAutoExposeStepTF(short stp) {
	CString pstr;
	str.Format("%d", stp);
	WritePrivateProfileString("Video", "AutoExposeStepTF", str, LocalCfgFile);
}

short CDOS::GetAutoExposeStepEcho(short stp) {
	return GetPrivateProfileInt("Video", "AutoExposeStep", stp, LocalCfgFile);
}

void CDOS::SetAutoExposeStepEcho(short stp) {
	CString pstr;
	str.Format("%d", stp);
	WritePrivateProfileString("Video", "AutoExposeStep", str, LocalCfgFile);
}

void CDOS::SetAutoFocusStepEcho(float factor) {
	str.Format("%e", factor);
	WritePrivateProfileString("Video", "AutoFocusStepEcho", str, LocalSysFile);
}

float CDOS::GetAutoFocusStepEcho(float factor) {
	char str[256];
	CString str1;
	str1.Format("%e", factor);
	GetPrivateProfileString("Video", "AutoFocusStepEcho", str1, str, 255, LocalSysFile);
	return float(atof(str));
}

void CDOS::SetAutoFocusStepTF(float factor) {
	str.Format("%e", factor);
	WritePrivateProfileString("Video", "AutoFocusStepTF", str, LocalSysFile);
}

float CDOS::GetAutoFocusStepTF(float factor) {
	char str[256];
	CString str1;
	str1.Format("%e", factor);
	GetPrivateProfileString("Video", "AutoFocusStepTF", str1, str, 255, LocalSysFile);
	return float(atof(str));
}

//20130903-1 SCOTT ADD--------------------------------------------------------
int CDOS::GetSendAllPoint() {
	return GetPrivateProfileInt("GEM", "SendAllPoint", 0, LocalSysFile);
}
//-------------------------------------------------------------------------

short CDOS::GetCameraType(CCamBase::CAMERAID iCam) {
	switch (iCam) {
	case CCamBase::ECHOCAM:	// Echo camera [6/1/2014 FSMT]
		return GetPrivateProfileInt("Video", "CameraTypeEcho", CCamBase::TYSIMUCAM, LocalCfgFile);
		break;
	default:
		return GetPrivateProfileInt("Video", "CameraTypeTF", CCamBase::TYSIMUCAM, LocalCfgFile);
		break;
	}
}

void CDOS::SetCameraType(short type, CCamBase::CAMERAID iCam) {
	CString str;
	str.Format("%d", type);
	switch (iCam) {
	case CCamBase::ECHOCAM: // Echo camera [6/1/2014 FSMT]
		WritePrivateProfileString("Video", "CameraTypeEcho", str, LocalCfgFile);
		break;
	default:
		WritePrivateProfileString("Video", "CameraTypeTF", str, LocalCfgFile);
		break;
	}
}

double CDOS::GetConfocalK() {
	char kVal[256];
	double CalibraionConstant;
	GetPrivateProfileString("Confocal", "ConstantK", "0.0", kVal, 255, LocalCfgFile);
	CalibraionConstant = atof(kVal);
	return CalibraionConstant;
}

void CDOS::SetConfocalK(double kVal) {
	CString k = "0.0";
	k.Format(_T("%.2lf"), kVal);
	WritePrivateProfileString("Confocal", "ConstantK", k, LocalCfgFile);
}
// 06202022 / yukchiu
BOOL CDOS::TurnConfigTab() {
	CSingleLock Lock(&CS);
	Lock.Lock();
	BOOL def = TRUE; // MASUM SEP
	return GetPrivateProfileInt("Persist", "ConfigTabOn", def, LocalSysFile);
}
// ==================

//08142024 - START

int CDOS::ReadINI(char* section, char* name, int defvalue, CString& inifile) {
	return GetPrivateProfileInt(section, name, defvalue, inifile);
}

ULONG CDOS::ReadINI(char* section, char* name, ULONG defvalue, CString& inifile) {
	char str[256];
	sprintf(str, "%ld", defvalue);
	GetPrivateProfileString(section, name, str, str, sizeof(str), inifile);
	return ULONG(atol(str));
}

float CDOS::ReadINI(char* section, char* name, float defvalue, CString& inifile) {
	char str[256];
	sprintf(str, "%f", defvalue);
	GetPrivateProfileString(section, name, str, str, sizeof(str), inifile);
	return float(atof(str));
}

double CDOS::ReadINI(char* section, char* name, double defvalue, CString& inifile) {
	char str[256];
	sprintf(str, "%f", defvalue);
	GetPrivateProfileString(section, name, str, str, sizeof(str), inifile);
	return atof(str);
}

CString CDOS::ReadINI(char* section, char* name, CString& str, CString& inifile) {
	char cstr[256];
	GetPrivateProfileString(section, name, str, cstr, sizeof(cstr), inifile);
	return cstr;
}

void CDOS::WriteINI(char* section, char* key, int val, CString& inifile) {
	CString str;
	str.Format("%d", val);
	WritePrivateProfileString(section, key, str, inifile);
}

void CDOS::WriteINI(char* section, char* key, ULONG val, CString& inifile) {
	CString str;
	str.Format("%ld", val);
	WritePrivateProfileString(section, key, str, inifile);
}

void CDOS::WriteINI(char* section, char* key, UINT val, CString& inifile) {
	CString str;
	str.Format("%d", val);
	WritePrivateProfileString(section, key, str, inifile);
}

void CDOS::WriteINI(char* section, char* key, char* str, CString& inifile) {
	WritePrivateProfileString(section, key, str, inifile);
}

void CDOS::WriteINI(char* section, char* key, CString& str, CString& inifile) {
	WritePrivateProfileString(section, key, str, inifile);
}

void CDOS::WriteINI(char* section, char* key, double val, CString& inifile) {
	CString str;
	str.Format("%f", val);
	WritePrivateProfileString(section, key, str, inifile);
}

void CDOS::WriteINI(char* section, char* key, float val, CString& inifile) {
	CString str;
	str.Format("%f", val);
	WritePrivateProfileString(section, key, str, inifile);
}

//08142024 - END