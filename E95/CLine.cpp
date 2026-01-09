#include "stdafx.h"

#include "CLine.h"

CLine& CLine::operator =(CLine& co) {
	magic = co.magic;
	npoint = co.npoint;
	angle = co.angle;
	return *this;
}
void CLine::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		ar << npoint;
		ar << angle;
	}
	else {
		ar >> magic;
		ar >> npoint;
		ar >> angle;
	}
}

CLine::~CLine() {}

CLine::CLine() {
	magic = 0;
	npoint = 9;
	angle = 10;
}