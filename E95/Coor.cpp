#include "stdafx.h"
// #include <math.h>

#include "Coor.h"
#include "SRC/BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL CCoor::IsPoint(CCoor& point) {
	if ((fabs(point.x - x) < 2.0f) && (fabs(point.y - y) < 2.0f)) {
		return TRUE;
	}
	return FALSE;
}

CCoor& CCoor::operator=(CCoor& co) {
	magic = co.magic;
	x = co.x;
	y = co.y;
	z = co.z;
	p = co.p;
	n = co.n;
	return *this;
}

void CCoor::Serialize(CArchive& ar) {
	if (ar.IsStoring()) {
		magic = 2;
		ar << magic;
		ar << x;
		ar << y;
		ar << n;
		ar << p;
		ar << z;
	}
	else {
		ar >> magic;
		ar >> x;
		ar >> y;
		ar >> n;
		ar >> p;
		if (magic > 1) {
			ar >> z;
		}
		else {
			z = 0;
		}
	}
}

CCoor::CCoor() {
	magic = 0;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	n = 0;
	p = 0;	// 0 means no marker set [7/23/2013 Yuen]
}

CCoor::~CCoor() {}

void CCoor::SetXYZ(short nn, float xx, float yy, float zz) {
	n = nn;
	z = zz;
	x = xx;
	y = yy;
	z = zz;
}

float CCoor::dia() {
	return float(sqrt(x * x + y * y));
}

float CCoor::dia2() {
	return x * x + y * y;
}

bool CCoor::operator ==(const CCoor& Rhs) {
	if (n == Rhs.n && p == Rhs.p && x == Rhs.x && y == Rhs.y && z == Rhs.z) {
		return true;
	}
	return false;
}

void CCoor::Conv(char** msg) {
	CMake::Conv(x, msg);
	CMake::Conv(y, msg);
	CMake::Conv(p, msg);
	CMake::Conv(n, msg);
	CMake::Conv(z, msg);
}

long CCoor::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(x, msg);
	sz += CMake::Binary(y, msg);
	sz += CMake::Binary(p, msg);
	sz += CMake::Binary(n, msg);
	sz += CMake::Binary(z, msg);

	return sz;
}

long CCoor::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(x);
	sz += CMake::GetLength(y);
	sz += CMake::GetLength(p);
	sz += CMake::GetLength(n);
	sz += CMake::GetLength(z);

	return sz;
}

CString CCoor::GetNStr() {
	CString str;
	str.Format("%d", n);
	return str;
}

CString CCoor::GetXYStr() {
	CString str;
	str.Format("%.3f,%.3f", x, y);
	return str;
}