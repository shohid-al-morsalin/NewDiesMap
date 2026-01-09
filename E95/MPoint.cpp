// MPoint.cpp: implementation of the CMPoint class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MPoint.h"
#include "Data.h"

#include "..\SRC\BMake.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CMPoint::ClearData() {
	Dat.ClearList();
}

CMPoint& CMPoint::operator =(CMPoint& RHs) {
	magic = RHs.magic;
	PaN = RHs.PaN;
	Co = RHs.Co;
	Dat = RHs.Dat;
	MeSet = RHs.MeSet;
	PatSet = RHs.PatSet;
	ProgressSM = RHs.ProgressSM;
	GOFdmy = RHs.GOFdmy;
	baseline = RHs.baseline;
	m_fvalOffset = RHs.m_fvalOffset;	// added value offset [10/6/2020]

	return *this;
}

CData* CMPoint::GetData(short index, BOOL bCreate) {
	if (index < 0) {
		// illegal call [5/14/2013 user]
		return NULL;
	}
	if (index == 0) {
		if (Dat.DatLst.GetCount() < 1) {
			if (bCreate) {
				CData* pDat = new CData;
				if (pDat) {
					Dat.SetData(pDat);
					return pDat;
				}
			}
			return NULL;
		}
		return (CData*)Dat.DatLst.GetTail();
	}
	return Dat.GetData((short)(index - 1));
}

void CMPoint::SetCoorXYZ(short n, float x, float y, float z) {
	Co.SetXYZ(n, x, y, z);
}

void CMPoint::Serialize(CArchive& ar) {
	short a;
	if (ar.IsStoring()) {
		magic = 11;//20211229 ZHIMING 10->11
		ar << magic;
		ar << PaN;
		ar << progressNotUsedmy;
		Co.Serialize(ar);
		Dat.Serialize(ar);
		ar << m_nNumSamplesdmy;
		ar << m_fOffsetXdmy;
		ar << m_fOffsetYdmy;
		ar << zN;
		ar << MeSet;
		ar << PatSet;
		a = GOFdmy;
		ar << a;
		ar << baseline;
		ar << m_fvalOffset;	// added value offset [10/6/2020]
	}
	else {
		ar >> magic;
		ar >> PaN;
		if (magic < 11) {
			PaN = 0;
		}
		ar >> progressNotUsedmy;
		Co.Serialize(ar);
		Dat.Serialize(ar);
		ar >> m_nNumSamplesdmy;
		ar >> m_fOffsetXdmy;
		ar >> m_fOffsetYdmy;
		if (5 < magic) {
			ar >> zN;
		}
		if (magic > 6) {
			ar >> MeSet;
			ar >> PatSet;
		}
		else {
			MeSet = 0;
			PatSet = 0;
		}
		if (magic > 7) {
			ar >> a;
			GOFdmy = (ENGOF)a;
		}
		else {
			GOFdmy = NOTKNOWN;
		}
		if (magic > 8) {
			ar >> baseline;
		}
		else {
			baseline = 0;
		}
		if (magic > 10)//20211229 ZHIMING
			ar >> m_fvalOffset;	// added value offset [10/6/2020]
		else
			m_fvalOffset = 0.0f;
	}
}

void CMPoint::SetCoor(CCoor* p) {
	Co = *p;
}

CMPoint::CMPoint() {
	PaN = 0;
	magic = 2;
	progressNotUsedmy = 0;
	m_nNumSamplesdmy = 1;

	m_fOffsetXdmy = 0;
	m_fOffsetYdmy = 0;
	zN = 0;

	baseline = 0;

	MeSet = 0;
	PatSet = 0;
	ProgressSM = NOTMEASURE;

	GOFdmy = NOTKNOWN;

	m_fvalOffset = 0.0f;	// added value offset [10/6/2020]
}

CMPoint::~CMPoint() {
	Dat.ClearList();
}

CCoor* CMPoint::GetCoor() {
	return &Co;
}

void CMPoint::GetCoor(float& x, float& y) {
	x = Co.x;
	y = Co.y;
}

void CMPoint::GetCoor(POINT& pt) {
	pt.x = (int)Co.x;
	pt.y = (int)Co.y;
}

void CMPoint::SetData(CData* pDat) {
	Dat.SetData(pDat);
}

void CMPoint::AddData(CData* pDat) {
	Dat.AddTail(pDat);
}

// pDat will not be freed [7/15/2009 Valued Customer]
// Only works with TM413 [7/15/2009 Valued Customer]
void CMPoint::AddHead(CData* pDat) {
	CData* p = new CData();
	if (p) {
		*p = *pDat;
		Dat.AddHead(p);
	}
}

BOOL CMPoint::GetSFPD(float& fSFPD) {
	fSFPD = zN;
	return TRUE;
}

void CMPoint::Conv(char** msg) {
	Dat.Conv(msg);
	CMake::Conv(m_nNumSamplesdmy, msg);
	CMake::Conv(baseline, msg);
	Co.Conv(msg);
	CMake::Conv(m_fOffsetXdmy, msg);
	CMake::Conv(m_fOffsetYdmy, msg);
	CMake::Conv(zN, msg);
	CMake::Conv(MeSet, msg);
	CMake::Conv(PatSet, msg);
	short a;
	CMake::Conv(a, msg);
	GOFdmy = (ENGOF)a;
	CMake::Conv(m_fvalOffset, msg);	// added value offset [10/6/2020]
}

long CMPoint::Binary(char** msg) {
	long sz = 0;

	sz += Dat.Binary(msg);
	sz += CMake::Binary(m_nNumSamplesdmy, msg);
	sz += CMake::Binary(baseline, msg);
	sz += Co.Binary(msg);
	sz += CMake::Binary(m_fOffsetXdmy, msg);
	sz += CMake::Binary(m_fOffsetYdmy, msg);
	sz += CMake::Binary(zN, msg);
	sz += CMake::Binary(MeSet, msg);
	sz += CMake::Binary(PatSet, msg);
	short a = GOFdmy;
	sz += CMake::Binary(a, msg);

	sz += CMake::Binary(m_fvalOffset, msg);	// added value offset [10/6/2020]
	return sz;
}

long CMPoint::GetLength() {
	long sz = 0;

	sz += Dat.GetLength();
	sz += CMake::GetLength(m_nNumSamplesdmy);
	sz += CMake::GetLength(baseline);
	sz += Co.GetLength();
	sz += CMake::GetLength(m_fOffsetXdmy);
	sz += CMake::GetLength(m_fOffsetYdmy);
	sz += CMake::GetLength(zN);
	sz += CMake::GetLength(MeSet);
	sz += CMake::GetLength(PatSet);
	sz += sizeof(short);  // GOF [7/15/2010 C2C]

	sz += CMake::GetLength(m_fvalOffset);	// added value offset [10/6/2020]
	return sz;
}

CString CMPoint::GetValueOffset() {
	CString str;
	str.Format("%d", m_fvalOffset);	// added value offset [10/6/2020]
	return str;
}

CString CMPoint::GetPropStr() {
	CString str;
	CString mkr[] = { "", "M1", "M2", "M3" };
	//str.Format("%.3f  %.3f  %.3f  %s", Co.x, Co.y, Co.z, mkr[Co.p]); //10032025_2
	str.Format("%.3f  %.3f    %s", Co.x, Co.y, mkr[Co.p]); //10032025_2
	return str;
}

CString CMPoint::GetMeSetStr() {
	CString str;
	str.Format("%d", MeSet);
	return str;
}

CString CMPoint::GetPatSetStr() {
	CString str;
	str.Format("%d", PatSet);
	return str;
}

CString CMPoint::GetXCoorStr() {
	CString str;
	str.Format("%.3f", Co.x);
	return str;
}

CString CMPoint::GetYCoorStr() {
	CString str;
	str.Format("%.3f", Co.y);
	return str;
}

void CMPoint::Dump(FILE* fp) {
	fprintf(fp, "%d,%.3f,%5.3f,%.3f,%d,%d\n", Co.n, Co.x, Co.y, Co.z, MeSet, PatSet);
}

void CMPoint::SetMarker(short n) {
	Co.p = n;
}

void CMPoint::ClearMarker(short n) {
	Co.p = 0;
}