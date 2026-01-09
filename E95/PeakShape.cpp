// PeakShape.cpp: implementation of the CPeakShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "PeakShape.h"
#include "..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPeakShape::CPeakShape() {
	Clear();
}

CPeakShape::~CPeakShape() {}

void CPeakShape::Set(short Thr, short Lev, float Wd, short Nr, short Sm, short Mthod, short Min, BOOL bUs, short Ht) {
	Thres = Thr;
	Level = Lev;
	Width = Wd;
	Near = Nr;
	Smooth = Sm;
	Method = Mthod;
	bUse = bUs;
	Minimum = Min;
	Height = Ht;
}

void CPeakShape::SetDistance(float dist, float Delta) {
	Distance = dist;
	DeltaDistance = Delta;
}

void CPeakShape::Serialize(CArchive& ar) {
	int magic;
	if (ar.IsStoring()) {
		magic = 8;
		ar << magic;
		ar << Thres;
		ar << Level;
		ar << Width;
		ar << Near;
		ar << Smooth;
		ar << Method;
		ar << bUse;
		ar << Minimum;
		ar << Height;
		ar << Distance;
		ar << DeltaDistance;
	}
	else {
		ar >> magic;
		ar >> Thres;
		ar >> Level;
		ar >> Width;
		if (magic > 1) {
			ar >> Near;
		}
		else {
			Near = 32;
		}
		if (magic > 2) {
			ar >> Smooth;
		}
		else {
			Smooth = 6;
		}
		if (magic > 3) {
			ar >> Method;
		}
		else {
			Method = 0;
		}
		if (magic > 4) {
			ar >> bUse;
		}
		else {
			bUse = FALSE;
		}
		if (magic > 5) {
			ar >> Minimum;
		}
		else {
			Minimum = 0;
		}
		if (magic > 6) {
			ar >> Height;
		}
		else {
			Height = 0;
		}
		if (magic > 7) {
			ar >> Distance;
			ar >> DeltaDistance;
		}
		else {
			Distance = 0;
			DeltaDistance = 0;
		}
	}
}

void CPeakShape::Conv(char** msg) {
	CMake::Conv(Thres, msg);
	CMake::Conv(Level, msg);
	CMake::Conv(Width, msg);
	CMake::Conv(Near, msg);
	CMake::Conv(Smooth, msg);
	CMake::Conv(Method, msg);
	CMake::Conv(Minimum, msg);
	CMake::Conv(bUse, msg);
	CMake::Conv(Height, msg);
	CMake::Conv(Distance, msg);
	CMake::Conv(DeltaDistance, msg);
}

long CPeakShape::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(Thres, msg);
	sz += CMake::Binary(Level, msg);
	sz += CMake::Binary(Width, msg);
	sz += CMake::Binary(Near, msg);
	sz += CMake::Binary(Smooth, msg);
	sz += CMake::Binary(Method, msg);
	sz += CMake::Binary(Minimum, msg);
	sz += CMake::Binary(bUse, msg);
	sz += CMake::Binary(Height, msg);
	sz += CMake::Binary(Distance, msg);
	sz += CMake::Binary(DeltaDistance, msg);

	return  sz;
}

long CPeakShape::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(Thres);
	sz += CMake::GetLength(Level);
	sz += CMake::GetLength(Width);
	sz += CMake::GetLength(Near);
	sz += CMake::GetLength(Smooth);
	sz += CMake::GetLength(Method);
	sz += CMake::GetLength(Minimum);
	sz += CMake::GetLength(bUse);
	sz += CMake::GetLength(Height);
	sz += CMake::GetLength(Distance);
	sz += CMake::GetLength(DeltaDistance);

	return  sz;
}

void CPeakShape::Clear() {
	Thres = 0;
	Level = 70;
	Width = 180;
	Near = 32;
	Smooth = 6;
	Method = 0;
	bUse = FALSE;
	Minimum = 0;
	Height = 0;
	Distance = 0;
	DeltaDistance = 0;
}

void CPeakShape::VerboseHeaderOut(FILE* fp) {
	if (!fp) {
		return;
	}
	fprintf(fp, "Use\tThreshold\tLevel\tMethod\tWidth\tNear\tSmooth\tMinimum\nHeight\tDistance\tDelta\n");
}

void CPeakShape::VerboseOut(FILE* fp, int n) {
	if (!fp) {
		return;
	}
	fprintf(fp, "Peak %d: %d\t%d\t%d\t%d\t%.2f\t%d\t%d\t%d\t%d\t%.3f\t%.3f\n",
		n, bUse, Thres, Level, Method, Width, Near, Smooth, Minimum, Height, Distance, DeltaDistance);
}