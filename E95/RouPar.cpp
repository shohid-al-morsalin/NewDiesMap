#include "stdafx.h"
#include "RouPar.h"
#include "..\SRC\DOSUtil.h"
CRouPar::CRouPar() {
}

CRouPar::~CRouPar() {
}

void CRouPar::LoadParam() {
	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);

	bCalibType =	ReadINI(_T("Rough"), _T("CalibType"), bCalibType, inifile);
	fReflMin =		ReadINI(_T("Rough"), _T("ReflMin"), fReflMin, inifile);
	fRdOffset =		ReadINI(_T("Rough"), _T("ReflOffset"), fRdOffset, inifile);
	fScatMin =		ReadINI(_T("Rough"), _T("ScatMin"), fScatMin, inifile);
	fRsOffset =		ReadINI(_T("Rough"), _T("ScatOffset"), fRsOffset, inifile);
	kRefl1 =		ReadINI(_T("Rough"), _T("kRefl"), kRefl1, inifile);
	kRefl2 =		ReadINI(_T("Rough"), _T("kRefl2"), kRefl2, inifile);
	Nmeas =			ReadINI(_T("Rough"), _T("Nmeas"), Nmeas, inifile);
	nRefreshTime =	ReadINI(_T("Rough"), _T("RefreshTime"), nRefreshTime, inifile);
	Phase =			ReadINI(_T("Rough"), _T("Phase"), Phase, inifile);
	ProbeXPos =		ReadINI(_T("Rough"), _T("ProbeXPos"), ProbeXPos, inifile);
	ProbeYPos =		ReadINI(_T("Rough"), _T("ProbeYPos"), ProbeYPos, inifile);
	SigmaCalibr1 =	ReadINI(_T("Rough"), _T("SigmaCalibr1"), SigmaCalibr1, inifile);
	SigmaCalibr2 =	ReadINI(_T("Rough"), _T("SigmaCalibr2"), SigmaCalibr2, inifile);
	RaFactor =	    ReadINI(_T("Rough"), _T("RaFactor"), RaFactor, inifile);
}

void CRouPar::SaveParam() {
	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);

	WriteINI(_T("Rough"), _T("CalibType"), bCalibType, inifile);
	WriteINI(_T("Rough"), _T("ReflMin"), fReflMin, inifile);
	WriteINI(_T("Rough"), _T("ReflOffset"), fRdOffset, inifile);
	WriteINI(_T("Rough"), _T("ScatMin"), fScatMin, inifile);
	WriteINI(_T("Rough"), _T("ScatOffset"), fRsOffset, inifile);
	WriteINI(_T("Rough"), _T("kRefl"), kRefl1, inifile);
	WriteINI(_T("Rough"), _T("kRefl2"), kRefl2, inifile);
	WriteINI(_T("Rough"), _T("Nmeas"), Nmeas, inifile);
	WriteINI(_T("Rough"), _T("RefreshTime"), nRefreshTime, inifile);
	WriteINI(_T("Rough"), _T("Phase"), Phase, inifile);
	WriteINI(_T("Rough"), _T("ProbeXPos"), ProbeXPos, inifile);
	WriteINI(_T("Rough"), _T("ProbeYPos"), ProbeYPos, inifile);
	WriteINI(_T("Rough"), _T("SigmaCalibr1"), SigmaCalibr1, inifile);
	WriteINI(_T("Rough"), _T("SigmaCalibr2"), SigmaCalibr2, inifile);
	WriteINI(_T("Rough"), _T("RaFactor"), RaFactor, inifile);
}

CRouPar::CRouPar(CRouPar& p) {
	sfc = p.sfc;
	bCalibType = p.bCalibType;
	fRdOffset = p.fRdOffset;
	fRsOffset = p.fRsOffset;
	SigmaCalibr1 = p.SigmaCalibr1;
	SigmaCalibr2 = p.SigmaCalibr2;
	kRefl1 = p.kRefl1;
	kRefl2 = p.kRefl2;
	fReflMin = p.fReflMin;
	fScatMin = p.fScatMin;
	Vf1 = p.Vf1;
	Vf2 = p.Vf2;
	Phase = p.Phase;
	Lockin = p.Lockin;
	Nmeas = p.Nmeas;
	ProbeXPos = p.ProbeXPos;
	ProbeYPos = p.ProbeYPos;
	nRefreshTime = p.nRefreshTime;
	RaFactor = p.RaFactor;
}

void CRouPar::WriteINI(char* section, char* key, int val, CString& inifile) {
	CString str;
	str.Format(_T("%d"), val);
	WritePrivateProfileString(section, key, str, inifile);
}
void CRouPar::WriteINI(char* section, char* key, double val, CString& inifile) {
	CString str;
	str.Format(_T("%f"), val);
	WritePrivateProfileString(section, key, str, inifile);
}

double CRouPar::ReadINI(char* section, char* name, double defvalue, CString& inifile) {
	char str[256];
	sprintf(str, _T("%f"), defvalue);
	GetPrivateProfileString(section, name, str, str, sizeof(str), inifile);
	return atof(str);
}
