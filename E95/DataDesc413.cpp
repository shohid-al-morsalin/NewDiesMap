#include "stdafx.h"
#include "DataDesc413.h"

#include "Data.h"
#include "SRC/BMake.h"

CDataDesc413& CDataDesc413::operator = (CDataDesc413& co) {
	magic = co.magic;
	Model = co.Model;
	for (int i = 0; i < LASTXCOL; i++) {
		unit[i] = co.unit[i];
		ID[i] = co.ID[i];
		bVis[i] = co.bVis[i];
		Col[i] = co.Col[i];
	}
	MapCol = co.MapCol;
	return *this;
}

void CDataDesc413::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << Model;
		switch (Model) {
		case 10:
			SaveM(ar);
			break;
		case 20:
			SaveTM(ar);
			break;
		}
	}
	else {
		ar >> Model;
		switch (Model) {
		case 10:
			LoadM(ar);
			break;
		case 20:
			LoadTM(ar);
			break;
		}
	}
}

CDataDesc413::CDataDesc413() {
	Clear();
}

CDataDesc413::~CDataDesc413() {}

void CDataDesc413::Clear() {
	for (int i = 2; i < LASTXCOL; i++) {
		unit[i] = 0;
		ID[i] = "";
		bVis[i] = FALSE;
		Col[i] = 0;
	}
	MapCol = 0;
	Model = 20;  // 20 is wafercal [3/13/2010 Valued Customer]
	InitTM();
}

short CDataDesc413::GetVisCount() {
	short cnt = 0;
	for (int i = 0; i < LASTXCOL; i++) {
		if (bVis[i]) cnt++;
	}
	return cnt;
}

void CDataDesc413::InitM() {
	int n;

	n = 0;
	ID[n] = " No."; unit[n] = 1; bVis[n] = TRUE;
	n = 1;
	ID[n] = " Position "; unit[n] = 1; bVis[n] = TRUE;
}

void CDataDesc413::InitTM() {
	int n;

	n = 0;
	ID[n] = "No.  "; unit[n] = 1; bVis[n] = TRUE;
	n = 1;
	ID[n] = "Position    "; unit[n] = 1; bVis[n] = TRUE;
}

void CDataDesc413::LoadM(CArchive& ar) {
	int i, n;

	ar >> magic;
	ar >> n;
	for (i = 0; i < n; i++) {
		ar >> unit[i];
		ar >> ID[i];
		ar >> bVis[i];
		ar >> Col[i];
	}
	for (i = n; i < LASTXCOL; i++) {
		ID[i] = "";
		unit[i] = 0;
		bVis[i] = FALSE;
		Col[i] = 0;
	}
	ar >> MapCol;
}

void CDataDesc413::SaveM(CArchive& ar) {
	// Version 2 and below has 33 column only [7/3/2012 user]
	magic = 3;
	ar << magic;
	ar << LASTXCOL;
	for (int i = 0; i < LASTXCOL; i++) {
		ar << unit[i];
		ar << ID[i];
		ar << bVis[i];
		ar << Col[i];
	}
	if (magic > 1) {
		ar << MapCol;
	}
}

void CDataDesc413::LoadTM(CArchive& ar) {
	short i, n;
	ar >> magic;
	ar >> n;
	for (i = 0; i < n; i++) {
		ar >> unit[i];
		ar >> ID[i];
		ar >> bVis[i];
		ar >> Col[i];
	}
	for (i = n; i < LASTXCOL; i++) {
		ID[i] = "";
		unit[i] = 0;
		bVis[i] = FALSE;
		Col[i] = 0;
	}
	if (magic > 1) {
		ar >> MapCol;
	}
}

void CDataDesc413::SaveTM(CArchive& ar) {
	// Version 2 and below has 33 column only [7/3/2012 user]
	magic = 3;
	ar << magic;
	ar << short(LASTXCOL);
	for (int i = 0; i < LASTXCOL; i++) {
		ar << unit[i];
		ar << ID[i];
		ar << bVis[i];
		ar << Col[i];
	}
	ar << MapCol;
}

void CDataDesc413::Conv(char** msg) {
	CMake::Conv(Model, msg);

	for (int i = 0; i < LASTXCOL; i++) {
		CMake::Conv(Col[i], msg);
		CMake::Conv(bVis[i], msg);
		CMake::Conv(unit[i], msg);
		CMake::Conv(ID[i], msg);
	}
	CMake::Conv(MapCol, msg);
}

long CDataDesc413::Binary(char** msg) {
	long sz = 0;
	sz += CMake::Binary(Model, msg);

	for (int i = 0; i < LASTXCOL; i++) {
		sz += CMake::Binary(Col[i], msg);
		sz += CMake::Binary(bVis[i], msg);
		sz += CMake::Binary(unit[i], msg);
		sz += CMake::Binary(ID[i], msg);
	}
	sz += CMake::Binary(MapCol, msg);

	return sz;
}

long CDataDesc413::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(Model);
	for (int i = 0; i < LASTXCOL; i++) {
		sz += CMake::GetLength(Col[i]);
		sz += CMake::GetLength(bVis[i]);
		sz += CMake::GetLength(unit[i]);
		sz += CMake::GetLength(ID[i]);
	}
	sz += CMake::GetLength(MapCol);

	return sz;
}

void CDataDesc413::VerboseOut(FILE* fp) {
	if (!fp) {
		return;
	}
	fprintf(fp, "Data description\n");
	fprintf(fp, "Version,%d\n", magic);
	fprintf(fp, "Model,%d\n", Model);
	fprintf(fp, "Column to display,%d\n", MapCol + 3);
	fprintf(fp, ",Column,Visible,ID,unit\n");
	int i;
	for (i = 0; i < 2; i++) {
		fprintf(fp, ",%d,%d,%s,%.4f\n", i + 1, bVis[i], ID[i].GetBuffer(0), unit[i]);
	}
	for (i = 2; i < LASTXCOL; i++) {
		if (ID[i].GetLength() > 0) {
			fprintf(fp, ",%d,%d,%s,%.4f\n", Col[i] + 3, bVis[i], ID[i].GetBuffer(0), unit[i]);
		}
	}
}