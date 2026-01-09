#include "stdafx.h"
#include "CRing.h"

CRing& CRing::operator =(CRing& co) {
	this->magic = co.magic;
	npoint = co.npoint;
	angle = co.angle;
	radius = co.radius;
	radius2 = co.radius2;
	return *this;
}

void CRing::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		ar << npoint;
		ar << angle;
		ar << radius;
	}
	else {
		ar >> magic;
		ar >> npoint;
		ar >> angle;
		ar >> radius;
	}
}

CRing::~CRing() {}

CRing::CRing() {
	magic = 0;
	radius = 45;
	radius2 = 45;
	npoint = 7;
	angle = 70;
}