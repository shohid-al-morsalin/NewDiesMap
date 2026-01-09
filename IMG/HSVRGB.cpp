#include "stdafx.h"
#include "HSVRGB.h"

using namespace IMGL;

CHSVRGB::CHSVRGB()
{
}

CHSVRGB::~CHSVRGB()
{
}

void CHSVRGB::SetHSV(float h, float s, float v)
{
	hsv.h = h;
	hsv.s = s;
	hsv.v = v;
}

void CHSVRGB::SetRGB(float r, float g, float b)
{
	rgb.r = r;
	rgb.g = g;
	rgb.b = b;
}

void CHSVRGB::SetBGR3(BYTE *pBGR)
{
	rgb.b = *(pBGR++) / 255.0;
	rgb.g = *(pBGR++) / 255.0;
	rgb.r = *(pBGR) / 255.0;
}

CHSV CHSVRGB::GetHSV()
{
	CHSV hs;
	CalcHSV(hs);
	return hs;
}

void CHSVRGB::GetHSV(double &H, double &S, double &V)
{
	CHSV hs;
	CalcHSV(hs);
	H = hs.h;
	S = hs.s;
	V = hs.v;
}

double CHSVRGB::GetH()
{
	CHSV hs;
	CalcHSV(hs);
	return hs.h;
}

double CHSVRGB::GetV()
{
	CHSV hs;
	CalcHSV(hs);
	return hs.v;
}

double CHSVRGB::GetS()
{
	CHSV hs;
	CalcHSV(hs);
	return hs.s;
}

CRGBf CHSVRGB::GetRGB()
{
	CRGBf rg;
	CalcRGB(rg);
	return rg;
}

void CHSVRGB::CalcRGB(CRGBf &rgb)
{
	double hh, p, q, t, ff;
	long i;

	if (hsv.s <= 0.0)
	{       // < is bogus, just shuts up warnings
		rgb.r = hsv.v;
		rgb.g = hsv.v;
		rgb.b = hsv.v;
		return;
	}
	hh = hsv.h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = hsv.v * (1.0 - hsv.s);
	q = hsv.v * (1.0 - (hsv.s * ff));
	t = hsv.v * (1.0 - (hsv.s * (1.0 - ff)));

	switch (i)
	{
	case 0:
		rgb.r = hsv.v;
		rgb.g = t;
		rgb.b = p;
		break;
	case 1:
		rgb.r = q;
		rgb.g = hsv.v;
		rgb.b = p;
		break;
	case 2:
		rgb.r = p;
		rgb.g = hsv.v;
		rgb.b = t;
		break;

	case 3:
		rgb.r = p;
		rgb.g = q;
		rgb.b = hsv.v;
		break;
	case 4:
		rgb.r = t;
		rgb.g = p;
		rgb.b = hsv.v;
		break;
	case 5:
	default:
		rgb.r = hsv.v;
		rgb.g = p;
		rgb.b = q;
		break;
	}
}

void CHSVRGB::CalcHSV(CHSV &hsv)
{
	double min, max, delta;

	min = rgb.r < rgb.g ? rgb.r : rgb.g;
	min = min < rgb.b ? min : rgb.b;

	max = rgb.r > rgb.g ? rgb.r : rgb.g;
	max = max > rgb.b ? max : rgb.b;

	hsv.v = max;                                // v
	delta = max - min;
	if (max > 0.0)
	{ // NOTE: if Max is == 0, this divide would cause a crash
		hsv.s = (delta / max);                  // s
	}
	else
	{
		// if max is 0, then r = g = b = 0
		// s = 0, v is undefined
		hsv.s = 0.0;
		hsv.h = 0/*NAN*/;                            // its now undefined
		return;
	}
	if (rgb.r >= max)                           // > is bogus, just keeps compilor happy
	{
		hsv.h = (rgb.g - rgb.b) / delta;        // between yellow & magenta
	}
	else
	{
		if (rgb.g >= max)
		{
			hsv.h = 2.0 + (rgb.b - rgb.r) / delta;  // between cyan & yellow
		}
		else
		{
			hsv.h = 4.0 + (rgb.r - rgb.g) / delta;  // between magenta & cyan
		}
	}

	hsv.h *= 60.0;                              // degrees

	if (hsv.h < 0.0)
	{
		hsv.h += 360.0;
	}
}