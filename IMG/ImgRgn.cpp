#include "stdafx.h"
#include "ImgRgn.h"

using namespace IMGL;

CImgRgn::CImgRgn()
{
	dI = 1;
	bMono = TRUE;
	scale = 1.0f;
	sx = sy = ex = ey = 0;
}

short CImgRgn::GetSkip(short bpp)
{
	return dI * bpp / 8;
}

// Set image search region [4/5/2015 VIT#1]
// Scale range from 0 to 1, centered at image center [4/5/2015 VIT#1]

void CImgRgn::SetSearchRgn(int wd, int ht)
{
	if (scale > 1) scale = 1;
	int dx = wd * scale;
	int dy = ht * scale;
	sx = (wd - dx) / 2;
	sy = (ht - dy) / 2;
	ex = sx + dx;
	ey = sy + dy;
}