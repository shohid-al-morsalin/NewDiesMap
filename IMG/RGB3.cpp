// RGB3.cpp: implementation of the CRGB3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RGB3.h"

using namespace IMGLx;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BYTE CRGB3::I()
{
	return (R + 2*G + B) / 4;
}

void CRGB3::Grey(BYTE v)
{
	R = G = B = v;
}

void CRGB3::Threshold(BYTE lvl)
{
	if (G < lvl)
	{ 
		R = G = B = 0;
	}
	else
	{
		R = G = B = 255;
	}
}

void CRGB3::Set(BYTE r, BYTE g, BYTE b)
{
	R = r;
	G = g;
	B = b;
}

void CRGB3::Dup(CRGB3 &rgb1)
{
	R = rgb1.R;
	G = rgb1.G;
	B = rgb1.B;
}

void CRGB3::Ave(CRGB3 *rgb1, short BG)
{
	short v;
	float fac = 1.0f;
	v = (rgb1->R-BG)*fac;
	if (v<0)
	{
		v=0;
	}
	if (v>255)
	{
		v=255;
	}
	R = (R + v) / 2;
	v = (rgb1->G-BG)*fac;
	if (v<0)
	{
		v=0;
	}
	if (v>255)
	{
		v=255;
	}
	G = (G + v) / 2;
	v = (rgb1->B-BG)*fac;
	if (v<0)
	{
		v=0;
	}
	if (v>255)
	{
		v=255;
	}
	B = (B + v) / 2;
}

void CRGB3::Add(int * histoB)
{
	histoB[B]++;
}

void CRGB3::Add(int *histoR, int *histoG, int *histoB)
{
	histoR[R]++;
	histoG[G]++;
	histoB[B]++;
}

void CRGB3::HistoEqualize(int *histoB)
{
	B = histoB[B];
}

void CRGB3::HistoEqualize(int *histoR, int *histoG, int *histoB)
{
	R = histoR[R];
	G = histoG[G];
	B = histoB[B];
}

BYTE CRGB3::MaxRGB()
{
	if (R >= B)
	{
		if (R >= G)
		{
			return R;
		}
	}
	else
	{
		if (B > G)
		{
			return B;
		}
	}
	return G;
}

BYTE CRGB3::MinRGB()
{
	if (R <= B)
	{
		if (R <= G)
		{
			return R;
		}
	}
	else
	{
		if (B <=G)
		{
			return B;
		}
	}
	return G;
}
