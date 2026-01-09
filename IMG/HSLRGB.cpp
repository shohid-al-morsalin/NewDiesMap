#include "stdafx.h"
#include "HSLRGB.h"

using namespace IMGL;

CHSLRGB::CHSLRGB()
{
}

CHSLRGB::~CHSLRGB()
{
}

void CHSLRGB::SetHSL(double h, double s, double l)
{
	hsl.h = h;
	hsl.s = s;
	hsl.l = l;
}

void CHSLRGB::SetRGB(double r, double g, double b)
{
	rgb.r = r;
	rgb.g = g;
	rgb.b = b;
}

CRGBf CHSLRGB::GetRGB()
{
	GetRGB(rgb);
	return rgb;
}

CHSL CHSLRGB::GetHSL()
{
	GetHSL(hsl);
	return hsl;
}

void CHSLRGB::GetRGB(CRGBf &rgb)
{
	double v;

	rgb.r = hsl.l;   // default to gray
	rgb.g = hsl.l;
	rgb.b = hsl.l;
	v = (hsl.l <= 0.5) ? (hsl.l * (1.0 + hsl.l)) : (hsl.l + hsl.l - hsl.l * hsl.s);
	if (v > 0)
	{
		double m;
		double sv;
		int sextant;
		double fract, vsf, mid1, mid2;

		m = hsl.l + hsl.l - v;
		sv = (v - m) / v;
		hsl.h *= 6.0;
		sextant = (int)hsl.h;
		fract = hsl.h - sextant;
		vsf = v * sv * fract;
		mid1 = m + vsf;
		mid2 = v - vsf;
		switch (sextant)
		{
		case 0:
			rgb.r = v;
			rgb.g = mid1;
			rgb.b = m;
			break;
		case 1:
			rgb.r = mid2;
			rgb.g = v;
			rgb.b = m;
			break;
		case 2:
			rgb.r = m;
			rgb.g = v;
			rgb.b = mid1;
			break;
		case 3:
			rgb.r = m;
			rgb.g = mid2;
			rgb.b = v;
			break;
		case 4:
			rgb.r = mid1;
			rgb.g = m;
			rgb.b = v;
			break;
		case 5:
			rgb.r = v;
			rgb.g = m;
			rgb.b = mid2;
			break;
		}
	}
}

void CHSLRGB::GetHSL(CHSL &hsl)
{
	double v;
	double m;
	double vm;
	double r2, g2, b2;

	hsl.h = 0; // default to black
	hsl.s = 0;
	hsl.l = 0;
	v = (rgb.r > rgb.g) ? rgb.r : rgb.g;
	v = (v > rgb.b) ? v : rgb.b;
	m = (rgb.r > rgb.g) ? rgb.g : rgb.r;
	m = (m > rgb.b) ? rgb.b : m;
	hsl.l = (m + v) / 2.0;
	if (hsl.l <= 0.0)
	{
		return;
	}
	vm = v - m;
	hsl.s = vm;
	if (hsl.s > 0.0)
	{
		hsl.s /= (hsl.l <= 0.5) ? (v + m) : (2.0 - v - m);
	}
	else
	{
		return;
	}
	r2 = (v - rgb.r) / vm;
	g2 = (v - rgb.g) / vm;
	b2 = (v - rgb.b) / vm;
	if (rgb.r == v)
	{
		hsl.h = (rgb.g == m ? 5.0 + b2 : 1.0 - g2);
	}
	else if (rgb.g == v)
	{
		hsl.h = (rgb.b == m ? 1.0 + r2 : 3.0 - b2);
	}
	else
	{
		hsl.h = (rgb.r == m ? 3.0 + g2 : 5.0 - r2);
	}
	hsl.h /= 6.0;
}