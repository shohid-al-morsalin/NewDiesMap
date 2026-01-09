// MeasurementProperties.cpp: implementation of the CMeasurementProperties class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include "wafercal.h"
#include "MeasurementProperties.h"
#include "..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMeasurementProperties& CMeasurementProperties::operator = (CMeasurementProperties& co) {
	magic = co.magic;
	cfactorindex = co.cfactorindex;
	measurement = co.measurement;
	ave = co.ave;
	matrixsize = co.matrixsize;
	matrixspacing = co.matrixspacing;
	calpos = co.calpos;
	xdicesize = co.xdicesize;
	ydicesize = co.ydicesize;
	return *this;
}

void CMeasurementProperties::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 4;
		ar << magic;
		ar << measurement;
		ar << ave;
		ar << matrixsize;
		ar << matrixspacing;
		ar << calpos;
		ar << xdicesize;
		ar << ydicesize;
		ar << cfactorindex; //fw 01/02 added
	}
	else {
		ar >> magic;
		ar >> measurement;
		if (magic < 2) return;
		ar >> ave;
		if (magic < 3) return;
		ar >> matrixsize;
		ar >> matrixspacing;
		ar >> calpos;
		if (magic < 4) {
			xdicesize = 1.f;
			ydicesize = 1.f;
		}
		else {
			ar >> xdicesize;
			ar >> ydicesize;
		}
		if (matrixsize != 0) matrixspacing = 1;

		ar >> cfactorindex; //fw 01/02 added
	}
}

CMeasurementProperties::CMeasurementProperties() {
	magic = 4;
	Clear();
}

CMeasurementProperties::~CMeasurementProperties() {}

void CMeasurementProperties::Clear() {
	ave = 8;
	calpos = 1;
	measurement = 11;
	matrixsize = 0;
	matrixspacing = 1;
	xdicesize = 1.f;
	ydicesize = 1.f;
	cfactorindex = 0; //fw
}

void CMeasurementProperties::Conv(char** msg) {
	CMake::Conv(matrixsize, msg);
	CMake::Conv(matrixspacing, msg);
	CMake::Conv(calpos, msg);
	CMake::Conv(ave, msg);
	CMake::Conv(xdicesize, msg);
	CMake::Conv(ydicesize, msg);
	CMake::Conv(cfactorindex, msg);
	CMake::Conv(measurement, msg);
}

long CMeasurementProperties::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(matrixsize, msg);
	sz += CMake::Binary(matrixspacing, msg);
	sz += CMake::Binary(calpos, msg);
	sz += CMake::Binary(ave, msg);
	sz += CMake::Binary(xdicesize, msg);
	sz += CMake::Binary(ydicesize, msg);
	sz += CMake::Binary(cfactorindex, msg);
	sz += CMake::Binary(measurement, msg);

	return sz;
}

long CMeasurementProperties::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(matrixsize);
	sz += CMake::GetLength(matrixspacing);
	sz += CMake::GetLength(calpos);
	sz += CMake::GetLength(ave);
	sz += CMake::GetLength(xdicesize);
	sz += CMake::GetLength(ydicesize);
	sz += CMake::GetLength(cfactorindex);
	sz += CMake::GetLength(measurement);

	return sz;
}

void CMeasurementProperties::VerboseOut(FILE* fp) {
	if (!fp) {
		return;
	}
	fprintf(fp, "Measurement Properties\n");
	fprintf(fp, "Version, %d\n", magic);
	fprintf(fp, "Matrix size, %d\n", matrixsize);
	fprintf(fp, "Matrix spacing, %d\n", matrixspacing);
	fprintf(fp, "Calibration gauge position, %d\n", calpos);
	fprintf(fp, "Sample average, %d\n", ave);
	fprintf(fp, "Dice size, X=%.3fmm, Y=%.3fmm\n", xdicesize, ydicesize);
	fprintf(fp, "Correction factor(index), %d\n", cfactorindex);
	fprintf(fp, "Measurement type, %ld\n", measurement);
}