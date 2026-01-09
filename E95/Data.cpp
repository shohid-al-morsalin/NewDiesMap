#include "stdafx.h"

#include "Data.h"

CData& CData::operator =(CData& co) {
	int i;
	model = co.model;
	magic = co.magic;

	// 	if (magic < 12)
	// 	{
	// 		for (i=0; i<24; i++)
	// 		{
	// 			DVal[i] = co.DVal[i];
	// 		}
	// 	}
	// 	else
	// 	{
	for (i = 0; i < MAXMEPASET * MaXPA; i++) {
		DVal[i] = co.DVal[i];
	}
	//	}
	return *this;
}

void CData::Serialize(CArchive& ar) {
	int i;
	if (ar.IsStoring()) {
		ar << model;
		magic = 12;
		ar << magic;
		for (i = 0; i < MAXMEPASET * MaXPA; i++) {
			ar << DVal[i];
		}
	}
	else {
		ar >> model;
		ar >> magic;
		for (i = 0; i < 24; i++) {
			ar >> DVal[i];
			// Patch for loading old recipe [9/5/2012 Administrator]
			if (magic < 12) {
				if (DVal[i] == 0) {
					DVal[i] = BADDATANEG;
				}
			}
		}
		if ((magic > 9) && (magic < 11)) {
			for (i = 24; i </*MaXDATAVALUE*/72; i++) {
				float dmy;
				ar >> dmy;
			}
		}
		if (magic > 11) {
			for (i = 24; i < MAXMEPASET * MaXPA; i++) {
				ar >> DVal[i];
				// Patch for loading old recipe [9/5/2012 Administrator]
// 				if (DVal[i] == 0)
// 				{
// 					DVal[i] = BADDATANEG;
// 				}
			}
		}
	}
}

CData::CData() {
	int i;
	magic = 12;
	model = SYSTEM_TM413;
	for (i = 0; i < MAXMEPASET * MaXPA; i++) {
		DVal[i] = BADDATANEG;
	}
}

CData::~CData() {}

float CData::Get(short idx) {
	if ((idx >= 0) && (idx < MAXMEPASET * MaXPA)) {
		return DVal[idx];
	}
	ASSERT(0);
	return -1;
}

void CData::Set(short idx, float val) {
	if ((idx >= 0) && (idx < MAXMEPASET * MaXPA)) {
		DVal[idx] = val;
		return;
	}
	ASSERT(0);
}

void CData::Conv(char** msg) {
	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		CMake::Conv(DVal[i], msg);
	}
}

long CData::Binary(char** msg) {
	long sz = 0;
	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		sz += CMake::Binary(DVal[i], msg);
	}
	return sz;
}

long CData::GetLength() {
	long sz = 0;
	for (int i = 0; i < MAXMEPASET * MaXPA; i++) {
		sz += CMake::GetLength(DVal[i]);
	}
	return sz;
}