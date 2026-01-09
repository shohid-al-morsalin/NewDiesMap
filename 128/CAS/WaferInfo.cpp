// WaferInfo.cpp: implementation of the CWaferInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WaferInfo.h"
#include "..\..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWaferInfo::CWaferInfo() {
	WaferID = "";
	LotID = "";
	RecipeName = "";
	wafernum = 0;
	WaferType = "Wf";
	WaferSize = "WsZ";
	Station = "St";
	fthick = 0;
	wthick = 0;
	ScanNo = 0; // 0 - use scan number in recipe file [9/10/2007 LYF]
	bSelected = FALSE;
}

CWaferInfo::~CWaferInfo() {}

CWaferInfo& CWaferInfo::operator =(CWaferInfo& dat) {
	WaferID = dat.WaferID;
	LotID = dat.LotID;
	RecipeName = dat.RecipeName;
	wafernum = dat.wafernum;
	fthick = dat.fthick;
	wthick = dat.wthick;
	ScanNo = dat.ScanNo;
	bSelected = dat.bSelected;
	return *this;
}

void CWaferInfo::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 5; // 20251022_Mohir 4 -> 5
		ar << magic;
		ar << wafernum;
		ar << fthick;
		ar << wthick;
		ar << WaferID;
		ar << bSelected;
		ar << LotID;
		ar << RecipeName;
		ar << ScanNo;
		//[[20251022_Mohir
		ar << WaferType;
		ar << WaferSize;
		ar << Station;
		//]]
	}
	else {
		ar >> magic;
		ar >> wafernum;
		ar >> fthick;
		ar >> wthick;
		ar >> WaferID;
		if (magic > 1) {
			ar >> bSelected;
		}
		if (magic > 2) {
			ar >> LotID;
			ar >> RecipeName;
		}
		if (magic > 3) {
			ar >> ScanNo;
		}
		if (magic > 4) {
			//[[20251022_Mohir
			ar >> WaferType;
			ar >> WaferSize;
			ar >> Station;
			//]]
		}
	}
}

void CWaferInfo::Set(CString wid, short wnum, float fthk, float wthk, short sNo) {
	WaferID = wid;
	wafernum = wnum;
	fthick = fthk;
	wthick = wthk;
	ScanNo = sNo;

}

long CWaferInfo::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(fthick);
	sz += CMake::GetLength(WaferID);
	sz += CMake::GetLength(wafernum);
	sz += CMake::GetLength(wthick);
	sz += CMake::GetLength(bSelected);
	sz += CMake::GetLength(LotID);
	sz += CMake::GetLength(RecipeName);
	sz += CMake::GetLength(ScanNo);
	//[20251022_Mohir
	sz += CMake::GetLength(WaferType);
	sz += CMake::GetLength(WaferSize);
	sz += CMake::GetLength(Station);
	//]
	return sz;
}

long CWaferInfo::Binary(char** msg) {
	long sz = 0;
	sz += CMake::Binary(fthick, msg);
	sz += CMake::Binary(WaferID, msg);
	sz += CMake::Binary(wafernum, msg);
	sz += CMake::Binary(wthick, msg);
	sz += CMake::Binary(bSelected, msg);
	sz += CMake::Binary(LotID, msg);
	sz += CMake::Binary(RecipeName, msg);
	sz += CMake::Binary(ScanNo, msg);
	//[20251022_Mohir
	sz += CMake::Binary(WaferType, msg);
	sz += CMake::Binary(WaferSize, msg);
	sz += CMake::Binary(Station, msg);
	//]

	return sz;
}

void CWaferInfo::Conv(char** msg) {
	CMake::Conv(fthick, msg);
	CMake::Conv(WaferID, msg);
	CMake::Conv(wafernum, msg);
	CMake::Conv(wthick, msg);
	CMake::Conv(bSelected, msg);
	CMake::Conv(LotID, msg);
	CMake::Conv(RecipeName, msg);
	CMake::Conv(ScanNo, msg);
	//[20251022_Mohir
	CMake::Conv(WaferType, msg);
	CMake::Conv(WaferSize, msg);
	CMake::Conv(Station, msg);
	//]
}

void CWaferInfo::SetWaferID(CString ID) {
	WaferID = ID;
}