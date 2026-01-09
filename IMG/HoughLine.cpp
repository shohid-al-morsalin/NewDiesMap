#include "stdafx.h"
#include <fstream>

#include "HoughLine.h"

#include "CSV\CSVRow.h"
#include "SrC\DOSUtil.h"

#define PIE  3.1415926535897932384626433832795
#define SQRE(x) ((x)*(x))

using namespace IMGL;

CHoughLine::CHoughLine()
{
	cx = cy = 0;
	na = nr = 0;
	ast = aed = 0; ainc = 1;
}

CHoughLine::~CHoughLine()
{
}

int IMGL::CHoughLine::GetWidth()
{
	return wd;
}

int IMGL::CHoughLine::GetHeight()
{
	return ht;
}

float IMGL::CHoughLine::GetAreaRatio(float prcs)
{
	return 0.05f;
}

BOOL IMGL::CHoughLine::ParseRadiusDB(const CString &fname)
{ //  [7/6/2016 Yuen]
	CCSVRow row;

	CRadiusItem Param;
	CString mdir = DosUtil.GetProgramDir();
	mdir += "RECIPE\\" + fname;
	std::ifstream file(mdir);
	if (!file.is_open()) return FALSE;
	file >> row;	// Version
	if (row[0] != "RadiusDB")
	{
		// Incorrect file
		return FALSE;
	}

	CRadiusItem Item;

	Radii.clear();
	file >> row; // Column header
	while (file >> row)	// Actual data
	{
		Item.radius = atoi(row[0].c_str());
		Item.intensity = atoi(row[1].c_str());
		Radii.push_back(Item);
	}
	return TRUE;
}

BOOL CHoughLine::AllocLine(int wd, int ht)
{
	// origin is at top left corner of image
	// angle sweep from 0 to 90 degree at increment of 0.5 degree
	// radius runs from 0 to full diagonal of image. resolution is 4 pixel
	redR = 1;
	redA = 1;
	nr = int((sqrt(2.0) * (ht > wd ? ht : wd)) / 2.0) / redR; // since this is an image. we will work with integer number [7/3/2016 Yuen]
	na = 180 / redA; // work with 180 degree angle [7/3/2016 Yuen]
	ast = 0; aed = na; ainc = 1;
	if ((na < 1) || (nr < 1)) return FALSE;
	if (!AllocMem(na*(2 * nr + 1))) return FALSE;

	SetDimension(na, nr);
	return TRUE;
}
BOOL CHoughLine::AllocCircle(int wd, int ht)
{
	// origin is at top left corner of image
	// angle sweep from 0 to 90 degree at increment of 0.5 degree
	// radius runs from 0 to full diagonal of image. resolution is 4 pixel
	redR = 1;
	nr = ht / redR; // since this is an image. we will work with integer number [7/3/2016 Yuen]
	redA = 1; ainc = 1;
	na = wd * redA; // work with 180 degree angle [7/3/2016 Yuen]
	ast = 0; aed = na;
	if ((na < 1) || (nr < 1)) return FALSE;
	if (!AllocMem(((na) / ainc)*(nr))) return FALSE;
	SetDimension(wd, ht);
	return TRUE;
}

BOOL CHoughLine::TransformLine(int x, int y)
{ // formular: r = x * cos(a) + y * sin(a)
	float ang;
	float cf = float(PIE / 180.0) / redA;
	//#pragma omp parallel for
	for (int a = ast; a < aed; a++)
	{
		ang = a * cf;
		int r = ((x - cx)*cos(ang) + (y - cy)*sin(ang)) / redR;
		if (abs(r) < nr)
		{
			int idx = int(r + nr) * na + a;
			if (idx < siz)
			{
				pIntImg[idx]++;
			}
		}
	}
	return TRUE;
}

// Parametric version [7/5/2016 Yuen]
BOOL CHoughLine::TransformCircle(int cx, int cy, int r)
{
	//+ x = cx + r * cos(t)
	//+ y = cy + r * sin(t)
	double sf = PIE / 180.0;
	// Do NOT parallelize [8/5/2016 FSM]
	//#pragma omp parallel for
	int xb = INT_MAX;
	int yb = INT_MAX;
	for (int a = 0; a <= 360; a++)
	{
		double v = a * sf;
		int x = cx + r * cos(v);
		int y = cy + r * sin(v);

		if ((x < 0) || (y < 0)) continue;
		if ((x >= iwd) || (y >= iht)) continue;

		// 		Increment(x, y);
		if ((x != xb) && (y != yb))
		{
			// avoid increment the same point twice [8/5/2016 FSM]
			xb = x; yb = y;
			Increment(x, y);
		}
	}
	return TRUE;
}

// Cartesian version [7/5/2016 Yuen]
BOOL CHoughLine::TransformCircleCartesian(int x, int y, int r)
{ //+ formula: (x - a)^2 + (y - b)^2 = r^2
	int r2 = r * r;
	for (int a = -r; a <= r; a++)
	{
		int b = r2 - SQRE(a);
		b = sqrt(b);
		Increment(x + a, y + b);
		Increment(x + b, y + a);
		Increment(x - a, y + b);
		Increment(x - b, y + a);
		Increment(x - a, y - b);
		Increment(x - b, y - a);
		Increment(x + a, y - b);
		Increment(x + b, y - a);
	}
	return TRUE;
}