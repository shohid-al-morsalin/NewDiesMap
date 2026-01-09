#include "stdafx.h"
#include "RGB.h"

using namespace IMGL;

CRGBf::CRGBf()
{
	r = g = b = 0.0f;
}

CRGBf::~CRGBf()
{
}

void CRGBf::Set(double _r, double _g, double _b)
{
	r = (_r < 0) ? 0 : ((_r > 1.0) ? 1.0 : _r);
	g = (_g < 0) ? 0 : ((_g > 1.0) ? 1.0 : _g);
	b = (_b < 0) ? 0 : ((_b > 1.0) ? 1.0 : _b);
}

BYTE CRGBf::Intn()
{
	return BYTE(r * RCPNT + g * GCPNT + b * BCPNT);
}

double CRGBf::fIntn()
{
	return r * RCPNT + g * GCPNT + b * BCPNT;
}

void CRGBf::Grey(BYTE v)
{
	r = g = b = v / 256.0;
}