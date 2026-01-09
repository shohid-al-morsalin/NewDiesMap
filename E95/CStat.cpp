#include "stdafx.h"

#include "CStat.h"

#include "SRC/BMake.h"

CStat::CStat() {
	Clear();
}

CStat::~CStat() {}

void CStat::Clear() {
	m_fTTV = m_fSTD = m_fAverage = m_fMin = m_fMax = m_fWarp = m_fBow = m_fMaxDieTTV = m_fStress = 0;
}

void CStat::Serialize(CArchive& ar) {
	short magic;

	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		ar << m_fTTV;
		ar << m_fSTD;
		ar << m_fAverage;
		ar << m_fMin;
		ar << m_fMax;
		ar << m_fWarp;
		ar << m_fBow;
		ar << m_fMaxDieTTV;
		ar << m_fStress;
	}
	else {
		ar >> magic;
		ar >> m_fTTV;
		ar >> m_fSTD;
		ar >> m_fAverage;
		ar >> m_fMin;
		ar >> m_fMax;
		ar >> m_fWarp;
		ar >> m_fBow;
		ar >> m_fMaxDieTTV;
		ar >> m_fStress;
	}
}

void CStat::Conv(char** msg) {
	CMake::Conv(m_fTTV, msg);
	CMake::Conv(m_fSTD, msg);
	CMake::Conv(m_fAverage, msg);
	CMake::Conv(m_fMin, msg);
	CMake::Conv(m_fMax, msg);
	CMake::Conv(m_fWarp, msg);
	CMake::Conv(m_fBow, msg);
	CMake::Conv(m_fMaxDieTTV, msg);
	CMake::Conv(m_fStress, msg);
}

long CStat::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(m_fTTV, msg);
	sz += CMake::Binary(m_fSTD, msg);
	sz += CMake::Binary(m_fAverage, msg);
	sz += CMake::Binary(m_fMin, msg);
	sz += CMake::Binary(m_fMax, msg);
	sz += CMake::Binary(m_fWarp, msg);
	sz += CMake::Binary(m_fBow, msg);
	sz += CMake::Binary(m_fMaxDieTTV, msg);
	sz += CMake::Binary(m_fStress, msg);

	return sz;
}

long CStat::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(m_fTTV);
	sz += CMake::GetLength(m_fSTD);
	sz += CMake::GetLength(m_fAverage);
	sz += CMake::GetLength(m_fMin);
	sz += CMake::GetLength(m_fMax);
	sz += CMake::GetLength(m_fWarp);
	sz += CMake::GetLength(m_fBow);
	sz += CMake::GetLength(m_fMaxDieTTV);
	sz += CMake::GetLength(m_fStress);

	return sz;
}