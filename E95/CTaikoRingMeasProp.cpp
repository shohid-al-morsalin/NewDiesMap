#include "stdafx.h"
#include "CTaikoRingMeasProp.h"

CTaikoRingMeasProp::CTaikoRingMeasProp() {
	Clear();
}

CTaikoRingMeasProp::~CTaikoRingMeasProp() {}

CTaikoRingMeasProp& CTaikoRingMeasProp::operator =(CTaikoRingMeasProp& co) {
	radius = co.radius;
	angle = co.angle;
	ZPos = co.ZPos;
	fromAngle = co.fromAngle;
	toAngle = co.toAngle;
	distanceFromCenter = co.distanceFromCenter;
	offsetAngle = co.offsetAngle;
	magic = co.magic;
	waferRadius = co.waferRadius;
	npoints = co.npoints;
	fMinWidth = co.fMinWidth;
	fMaxWidth = co.fMaxWidth;
	bSearchAround = co.bSearchAround;
	nRepeat = co.nRepeat;
	fStepDistance = co.fStepDistance;
	eSearchType = co.eSearchType;
	RI = co.RI;
	Exposure = co.Exposure;
	return *this;
}

void CTaikoRingMeasProp::Clear()
{
	radius = 150.0;
	angle = 90.0;
	ZPos = 0.0;
	fromAngle = 268.5;
	toAngle = 271.5;
	distanceFromCenter = 150.3;
	offsetAngle = -5;
	magic = 1;
	waferRadius = 150;
	ringWidth = 0;
	bevelWidth = 0;
	RingThk = 0;
	RingStepHeight = 0;
	npoints = 4;
	fMinWidth = 0.0f;
	fMaxWidth = 0.0f;
	bSearchAround = FALSE;
	nRepeat = 0;
	fStepDistance = 0.0f;
	eSearchType = SearchType::LINEAR;
	RI = 1.0;
	Exposure = 40;
	RingSTD = 0.0;
	RingTTV = 0.0;
	RingStepWidth = 0.0;
	RingStepWidthSTD = 0.0;
	RingStepWidthTTV = 0.0;
}

void CTaikoRingMeasProp::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		ar << magic;
		ar << radius;
		ar << angle;
		ar << ZPos;
		ar << fromAngle;
		ar << toAngle;
		ar << distanceFromCenter;
		ar << offsetAngle;
		ar << waferRadius;
		ar << npoints;
		ar << fMinWidth;
		ar << fMaxWidth;
		ar << bSearchAround;
		ar << nRepeat;
		ar << fStepDistance;
		ar << RI;
		ar << Exposure;
		short searchType = eSearchType;
		ar << searchType;
	}
	else {
		ar >> magic;
		ar >> radius;
		ar >> angle;
		ar >> ZPos;
		ar >> fromAngle;
		ar >> toAngle;
		ar >> distanceFromCenter;
		ar >> offsetAngle;
		ar >> waferRadius;
		ar >> npoints;
		ar >> fMinWidth;
		ar >> fMaxWidth;
		ar >> bSearchAround;
		ar >> nRepeat;
		ar >> fStepDistance;
		ar >> RI;
		ar >> Exposure;
		short st;
		ar >> st;
		switch (st) {
		case SearchType::LINEAR:
			eSearchType = SearchType::LINEAR;
			break;
		case SearchType::CIRCULAR:
			eSearchType = SearchType::CIRCULAR;
			break;
		default:
			eSearchType = SearchType::LINEAR;
			break;
		}
	}
}