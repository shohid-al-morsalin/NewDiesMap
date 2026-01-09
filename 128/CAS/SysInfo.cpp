// SysInfo.cpp: implementation of the CSysInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SysInfo.h"
#include "..\..\SRC\DosUtil.h"
#include "..\..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSysInfo SysInf;

CSysInfo::CSysInfo() {
	Clear();
	SaveParam();
}

CSysInfo::~CSysInfo() {
	//	SaveParam();
}

CSysInfo& CSysInfo::operator =(CSysInfo& dat) {
	BP750 = dat.BP750;
	BP810 = dat.BP810;
	stressconst = dat.stressconst;
	RefScheme = dat.RefScheme;
	ShowLines = dat.ShowLines;
	NotchAngle = dat.NotchAngle;
	bFlipX = dat.bFlipX;

	return *this;
}

void CSysInfo::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 2;
		ar << magic;
		ar << BP750;
		ar << BP810;
		ar << stressconst;
		ar << RefScheme;
		ar << ShowLines;
		ar << NotchAngle;
		ar << bFlipX;
	}
	else {
		ar >> magic;
		ar >> BP750;
		ar >> BP810;
		ar >> stressconst;
		ar >> RefScheme;
		ar >> ShowLines;
		if (magic > 1) {
			ar >> NotchAngle;
			ar >> bFlipX;
		}
		else {
			NotchAngle = 0;
			bFlipX = TRUE;
		}
	}
}

void CSysInfo::LoadParam() {
	//	char str[256];
	//	CString sect;
	//	CString winini;

	//	DosUtil.GetLocalSysFile(winini);

	//	sect = "Scanner";
	//	GetPrivateProfileString(sect,"BeamPath750","340.0",str,255,winini);
	//	BP750 = float(atof(str));
	//	GetPrivateProfileString(sect,"BeamPath810","340.0",str,255,winini);
	//	BP810 = float(atof(str));
	//	GetPrivateProfileString(sect,"StressConstant","1.805e12",str,255,winini);
	//	stressconst = float(atof(str));

	// 	BP750 = DosUtil.GetBeampath(1);
	// 	BP810 = DosUtil.GetBeampath(2);
	stressconst = DosUtil.GetStressConst("1.805e+012");
	NotchAngle = DosUtil.GetNotchAngle();
	bFlipX = DosUtil.GetFlipMapX();

	//	sect = "Persist";
	//	RefScheme = (REFSCHEME)GetPrivateProfileInt(sect,"RefScheme",rfFIRST,winini);
	//	ShowLines = GetPrivateProfileInt(sect,"ShowLines",lnREF|lnCUR|lnDIF|lnPRO|lnLSQ,winini);
}

void CSysInfo::SaveParam() {
	//	CString str;
	//	CString sect;
	//	CString winini;

	//	DosUtil.GetLocalSysFile(winini);

	//	sect = "Scanner";
	//	str.Format("%.2f",BP750);
	//	WritePrivateProfileString(sect,"BeamPath750",str,winini);
	//	str.Format("%.2f",BP810);
	//	WritePrivateProfileString(sect,"BeamPath810",str,winini);
	//	str.Format("%.3e",stressconst);
	//	WritePrivateProfileString(sect,"StressConstant",str,winini);

	// 	DosUtil.SetBeampath(1,BP750);
	// 	DosUtil.SetBeampath(2,BP810);
	CString str;
	str.Format("%.4e", stressconst);
	DosUtil.SetStressConst(str);
	DosUtil.SetNotchAngle(NotchAngle);
	DosUtil.SetFlipMapX(bFlipX);

	//	sect = "Persist";
	//	str.Format("%d",RefScheme);
	//	WritePrivateProfileString(sect,"RefScheme",str,winini);
	//	str.Format("%d",ShowLines);
	//	WritePrivateProfileString(sect,"ShowLines",str,winini);
}

void CSysInfo::Clear() {
	BP750 = 270;
	BP810 = 270;
	stressconst = 1.81e12f;
	RefScheme = rfFIRST;
	ShowLines = lnCUR | lnDIF | lnLSQ | /*lnPRO |*/ lnREF;
	NotchAngle = 0;
	bFlipX = TRUE;
	LoadParam();
}

long CSysInfo::GetLength() {
	long sz;

	sz = CMake::GetLength(BP750);
	sz += CMake::GetLength(BP810);
	sz += CMake::GetLength(stressconst);
	sz += CMake::GetLength(RefScheme);
	sz += CMake::GetLength(ShowLines);
	sz += CMake::GetLength(NotchAngle);
	sz += CMake::GetLength(bFlipX);

	return sz;
}

long CSysInfo::Binary(char** msg) {
	long sz;
	sz = CMake::Binary(BP750, msg);
	sz += CMake::Binary(BP810, msg);
	sz += CMake::Binary(stressconst, msg);
	sz += CMake::Binary(RefScheme, msg);
	sz += CMake::Binary(ShowLines, msg);
	sz += CMake::Binary(NotchAngle, msg);
	sz += CMake::Binary(bFlipX, msg);

	return sz;
}

void CSysInfo::Conv(char** msg) {
	CMake::Conv(BP750, msg);
	CMake::Conv(BP810, msg);
	CMake::Conv(stressconst, msg);
	CMake::Conv(RefScheme, msg);
	CMake::Conv(ShowLines, msg);
	CMake::Conv(NotchAngle, msg);
	CMake::Conv(bFlipX, msg);
}