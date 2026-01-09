#include "stdafx.h"
#include "CWaferParam.h"
#include "SRC/BMake.h"

#include "SRC/DOSUtil.h"

CWaferParam& CWaferParam::operator =(CWaferParam& co) {
	magic = co.magic;
	size = co.size;
	mapsize = co.mapsize;
	return *this;
}

CWaferParam::~CWaferParam() {}

CWaferParam::CWaferParam() {
	Clear();
}

void CWaferParam::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << magic;
		ar << size;
		ar << mapsize;
	}
	else {
		ar >> magic;
		ar >> size;
		ar >> mapsize;
	}
}

void CWaferParam::Clear() {
	magic = 0;
	size = DosUtil.GetWaferSize(300);
	percentage = 95;
	mapsize = size * percentage / 100;
}

void CWaferParam::Conv(char** msg) {
	CMake::Conv(size, msg);
	CMake::Conv(mapsize, msg);
}

long CWaferParam::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(size, msg);
	sz += CMake::Binary(mapsize, msg);

	return sz;
}

long CWaferParam::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(size);
	sz += CMake::GetLength(mapsize);

	return sz;
}

void CWaferParam::VerboseOut(FILE* fp) {
	if (!fp) {
		return;
	}
	fprintf(fp, "Wafer parameters\n");
	fprintf(fp, "Version, %d\n", magic);
	fprintf(fp, "Size, %d\n", size);
	fprintf(fp, "Mapsize, %d\n", mapsize);
}