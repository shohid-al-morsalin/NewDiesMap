#include "stdafx.h"
#include "ColorTable.h"
#include "HSLRGB.h"

#include <math.h>

#define PI 3.141592654

using namespace IMGL;

CColorTable::CColorTable()
{
	nL = 0;
	tL = 0;
	pLUT = NULL;
}

CColorTable::~CColorTable()
{
	Clear();
}

void CColorTable::Clear()
{
	if (pLUT)
	{
		delete[] pLUT;
		pLUT = NULL;
	}
	nL = 0;
}

// Monochrome color [11/23/2014 FSM]
// Input fR,fG,fB range from 0.0 to 1.0
void CColorTable::Create0(int n, double fR, double fG, double fB)
{
	if (!Alloc(n)) return;

	double v;
	double sf = 1 / double(n - 1);
	for (int i = 0; i < n; i++)
	{
		v = i * sf;
		pLUT[i].Set(v * fR, v * fG, v * fB);
	}
}

void CColorTable::Create1(int n)
{
	if (!Alloc(n)) return;

	double v;
	double sf = (PI) / double(n);
	for (int i = 0; i < n; i++)
	{
		v = i * sf;
		pLUT[i].Set((sin(v + 4.71238898) + 1) / 2.0f, (sin(v) + 1) / 2.0f, (sin(v + 1.570796327) + 1) / 2.0f);
	}
}

void CColorTable::Create2(int n)
{
	if (!Alloc(n)) return;

	double v;
	double sf = (2 * PI) / double(n);
	for (int i = 0; i < n; i++)
	{
		v = i * sf;
		pLUT[i].Set(sin(v + 4.71238898), sin(v), sin(v + 1.570796327));
	}
}

void CColorTable::Create3(int n)
{
	if (!Alloc(n)) return;

	CRGBf rgb;
	CHSLRGB C;

	double stAng = 250.0;
	double stSpand = 280.0;

	double r = stAng / 360.0;
	double sf = stSpand / double(n) / 360.0;

	for (int i = 0; i < n; i++)
	{
		double h;
		h = -i * sf + r;
		if (h > 1) h -= 1;
		if (h < 0) h += 1;

		C.SetHSL(h, 1.f, .45f);
		C.GetRGB(rgb);
		pLUT[i].Set(rgb.r, rgb.g, rgb.b);
	}
}

// HSL version [10/31/2014 Yuen]
void CColorTable::Create4(int n)
{
	if (!Alloc(n)) return;
	CRGBf rgb;
	CHSLRGB C;

	double stAng = 270.0;
	double stSpand = 280.0;

	double r = stAng / 360.0;
	double sf = stSpand / double(n) / 360.0;

	for (int i = 0; i < n; i++)
	{
		double h;
		h = -i * sf + r;
		if (h > 1) h -= 1;
		if (h < 0) h += 1;

		C.SetHSL(h, 0.9f, .65f);
		C.GetRGB(rgb);
		pLUT[i].Set(rgb.r, rgb.g, rgb.b);
	}
}

void CColorTable::CycleLUT()
{
	tL++; if (tL > 7) tL = 0;
	if (nL == 0) nL = 256;

	switch (tL)
	{
	case 0:
		Create0(nL, 1, 1, 1);
		break;
	case 1:
		Create0(nL, 0, 0, 1);
		break;
	case 2:
		Create0(nL, 1, 0, 1);
		break;
	case 3:
		Create0(nL, 1, 1, 0);
		break;
	case 4:
		Create1(nL);
		break;
	case 5:
		Create2(nL);
		break;
	case 6:
		Create3(nL);
		break;
	case 7:
		Create4(nL);
		break;
	}
}