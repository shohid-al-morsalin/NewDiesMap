// JobInfo.cpp: implementation of the CJobInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JobInfo.h"
#include "..\..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CJobInfo::CJobInfo() {
	magic = 4;
	Clear();
}

CJobInfo::~CJobInfo() {}

void CJobInfo::Clear() {
	LPUnit = 0;
	comment = "";
	batchname = "";
	CarrierID = "";
	projectname = "";
	operatorname = "";
	cassettename = "N/A";
	LotID = "";
	time = CTime::GetCurrentTime().GetTime();
}

CJobInfo& CJobInfo::operator =(CJobInfo& dat) {
	time = dat.time;
	comment = dat.comment;
	batchname = dat.batchname;
	CarrierID = dat.CarrierID;
	projectname = dat.projectname;
	cassettename = dat.cassettename;
	operatorname = dat.operatorname;
	LPUnit = dat.LPUnit;
	LotID = dat.LotID;
	return *this;
}

void CJobInfo::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 4;
		ar << magic;
		ar << batchname;
		ar << cassettename;
		ar << comment;
		ar << operatorname;
		ar << projectname;
		ar << time;
		ar << CarrierID;
		ar << LPUnit;
		ar << LotID;
	}
	else {
		ar >> magic;
		ar >> batchname;
		ar >> cassettename;
		ar >> comment;
		ar >> operatorname;
		ar >> projectname;
		ar >> time;
		if (magic > 1) {
			ar >> CarrierID;
		}
		if (magic > 2) {
			ar >> LPUnit;
		}
		if (magic > 3) {
			ar >> LotID;
		}
	}
}

long CJobInfo::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(LPUnit);
	sz += CMake::GetLength(comment);
	sz += CMake::GetLength(batchname);
	sz += CMake::GetLength(CarrierID);
	sz += CMake::GetLength(operatorname);
	sz += CMake::GetLength(cassettename);
	int tt = time;
	sz += CMake::GetLength(tt);
	sz += CMake::GetLength(LotID);

	return sz;
}

long CJobInfo::Binary(char** msg) {
	long sz = 0;
	sz += CMake::Binary(LPUnit, msg);
	sz += CMake::Binary(comment, msg);
	sz += CMake::Binary(batchname, msg);
	sz += CMake::Binary(CarrierID, msg);
	sz += CMake::Binary(operatorname, msg);
	sz += CMake::Binary(cassettename, msg);
	int tt = time;
	sz += CMake::Binary(tt, msg);
	sz += CMake::Binary(LotID, msg);

	return sz;
}

void CJobInfo::Conv(char** msg) {
	CMake::Conv(LPUnit, msg);
	CMake::Conv(comment, msg);
	CMake::Conv(batchname, msg);
	CMake::Conv(CarrierID, msg);
	CMake::Conv(operatorname, msg);
	CMake::Conv(cassettename, msg);
	int tt;
	CMake::Conv(tt, msg);
	time = tt;
	CMake::Conv(LotID, msg);
}