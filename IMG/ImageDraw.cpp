#include "stdafx.h"
#include "ImageDraw.h"
#include "ImgPixel.h"

#define PIE  3.1415926535897932384626433832795

template<typename T>
void IMGL::CImageDraw::DrawCircle(int cx, int cy, int r)
{
	//////////////////////////////////////////////////////////////////////////
	//+ x = cx + r * cos (t)
	//+ y = cy + r * sin (t)
	//////////////////////////////////////////////////////////////////////////
	int wd = GetWidth() - 1;
	int ht = GetHeight() - 1;
	double sf = PIE / 180.0;

	short dr = 360 / (2 * 3.1415*r);
	if (dr < 1) dr = 1;

#pragma omp parallel for
	for (int t = 0; t <= 360; t += dr)
	{
		int x = cx + r * cos(t*sf);
		int y = cy + r * sin(t*sf);
		if ((x < 0) || (y < 0)) continue;
		if ((x > wd) || (y > ht)) continue;

		T *p = (T *)GetPixelAddress(x, y);
		p->Set(255, 125, 255);
	}
}

template<typename T>
void IMGL::CImageDraw::DrawCircle(int cx, int cy, int r, int &marked, int &total, BOOL bWrite)
{
	int wd = GetWidth() - 1;
	int ht = GetHeight() - 1;
	double sf = PIE / 180.0;

	int *ptotal = new int[361];
	int *pmarked = new int[361];
	if (!ptotal || !pmarked)
	{
		if (ptotal) delete[] ptotal;
		if (pmarked) delete[] pmarked;
	}
	memset(ptotal, 0, 361 * sizeof(int));
	memset(pmarked, 0, 361 * sizeof(int));

	short dr = 360 / (2 * 3.1415*r);
	if (dr < 1) dr = 1;

#pragma omp parallel for
	for (int t = 0; t <= 360; t += dr)
	{
		BYTE cr, cg, cb;
		int x = cx + r * cos(t*sf);
		int y = cy + r * sin(t*sf);
		if ((x < 0) || (y < 0)) continue;
		if ((x > wd) || (y > ht)) continue;
		ptotal[t]++;
		T *p = (T *)GetPixelAddress(x, y);
		p->Get(cr, cg, cb);
		if (cr == 0) pmarked[t]++;
		if (bWrite) p->Set(255, 0, 0);
	}

	total = 0; marked = 0;
	for (int i = 0; i <= 360; i++)
	{
		total += ptotal[i];
		marked += pmarked[i];
	}
	if (ptotal) delete[] ptotal;
	if (pmarked) delete[] pmarked;
}

void IMGL::CImageDraw::DrawCircle(int cx, int cy, int r)
{
	switch (GetBYPP())
	{
	case 1:
		DrawCircle<sBINR>(cx, cy, r);
		break;
	case 3:
		DrawCircle<sBGR>(cx, cy, r);
		break;
	case 4:
		DrawCircle<sBGRA>(cx, cy, r);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void IMGL::CImageDraw::DrawCircle(int cx, int cy, int r, int &marked, int &total, BOOL bWrite)
{
	switch (GetBYPP())
	{
	case 1:
		DrawCircle<sBINR>(cx, cy, r, marked, total, bWrite);
		break;
	case 3:
		DrawCircle<sBGR>(cx, cy, r, marked, total, bWrite);
		break;
	case 4:
		DrawCircle<sBGRA>(cx, cy, r, marked, total, bWrite);
		break;
	default:
		ASSERT(0);
		break;
	}
}