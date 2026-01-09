#include "stdafx.h"
#include "MemInt.h"

using namespace IMGL;

CMemInt::CMemInt()
{
	wd = 0;
	ht = 0;
	siz = 0;
	counts = 0;
	Max = Min = 0;
	pIntImg = NULL;
}

CMemInt::~CMemInt()
{
	Clear();
}

void CMemInt::Clear()
{
	if (pIntImg)
	{
		siz = 0;
		delete[] pIntImg; pIntImg = NULL;
	}
}

void CMemInt::ClearTopTen()
{
	TopTen.Clear();
}

void CMemInt::Dump(CString fname)
{
	FILE *fp = fopen(fname, "wb");
	if (fp)
	{
		for (int y = 0; y < ht; y++)
		{
			int *p = pIntImg + y * wd;
			for (int x = 0; x < wd; x++)
			{
				fprintf(fp, "%d,", *p);
				p++;
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
}

void CMemInt::DumpHisto(CString fname)
{
	FILE *fp = fopen(fname, "wb");
	if (fp)
	{
		for (int i = 0; i < 256; i++)
		{
			fprintf(fp, "%d\n", His[i]);
		}
		fclose(fp);
	}
}

// NOTE: No limit checking, caller make sure x and y are in range [8/5/2016 FSM]
void CMemInt::Increment(int x, int y)
{
	//if ((x < 0) || (y < 0) || (x >= wd) || (y >= ht)) return;
	pIntImg[y*wd + x]++;
}

void CMemInt::SetDimension(int w, int h)
{
	wd = w;
	ht = h;
	cx = wd / 2;
	cy = ht / 2;
}

// NOTE: thres > 1 and < max to be meaningfull [8/5/2016 FSM]
void CMemInt::FindBestPos(int r)
{
	MaxV();  // calculate probability density [9/29/2016 FSM]
	int thres = Max * 0.90f;
	TopTen.Clear();
	counts = 0;
	for (int y = 0; y < ht; y++)
	{
		int *p = pIntImg + y * wd;
		for (int x = 0; x < wd; x++)
		{
			if (*p >= thres)
			{
				counts++;
				TopTen.Insert(x, y, *p, r);
			}
			p++;
		}
	}
}

void CMemInt::Average3x3()
{ // 3x3 averaging
	// Duplicate [7/5/2016 Yuen]
	int *pInt2 = new int[siz]; if (!pInt2) return;

	memcpy(pInt2, pIntImg, siz * sizeof(int));
#pragma omp parallel for
	for (int y = 1; y < ht - 1; y++)
	{
		int *p1 = pInt2 + (y - 1)*wd; int *p2 = p1 + 1; int *p3 = p2 + 1;
		int *q1 = pInt2 + (y - 1)*wd; int *q2 = q1 + 1; int *q3 = q2 + 1;
		int *r1 = pInt2 + (y - 1)*wd; int *r2 = r1 + 1; int *r3 = r2 + 1;
		int *s = pIntImg + (y)*wd;
		for (int x = 1; x < wd - 1; x++)
		{
			*s = (*p1 + *p2 + *p3 + *q1 + *q2 + *q3 + *r1 + *r2 + *r3) / 9;
			s++; p1++; p2++; p3++; q1++; q2++; q3++; r1++; r2++; r3++;
		}
	}

	delete[] pInt2;
}

// Histogram performed on int buffer [7/8/2016 Yuen]
void CMemInt::Histo()
{
	memset(His, 0, 256 * sizeof(int));
	// Cannot parallelize due to His[]
	for (int y = 0; y < ht; y++)
	{
		int *p = pIntImg + y * wd;
		for (int x = 0; x < wd; x++)
		{
			His[*p]++;
			p++;
		}
	}
}

void CMemInt::Merge(short dist)
{
	TopTen.Merge(dist);
}

void CMemInt::Sort()
{
	TopTen.Sort();
}

CScore * CMemInt::GetCScore(int n)
{
	return TopTen.GetCScore(n);
}

int CMemInt::GetHistoSumMax()
{
	HisSum = 0;
	HisMaxIndex = 0;
	HisMax = INT_MIN;
	for (int i = 0; i < 256; i++)
	{
		if (His[i] > HisMax)
		{
			HisMaxIndex = i;
			HisMax = His[i];
		}
		HisSum += His[i];
	}
	return HisSum;
}

int CMemInt::GetCumMidIdex(int mid)
{
	int sum = 0;
	for (int i = 0; i < 256; i++)
	{
		sum += His[i];
		if (sum > mid)
		{
			HisMidIndex = i;
			return i;
		}
	}
	return 0;
}

int CMemInt::GetCumDistance()
{
	Histo();
	GetHistoSumMax();
	GetCumMidIdex(HisSum / 2);
	return abs(HisMaxIndex - HisMidIndex);
}

int CMemInt::GetArea(int x, int y, int r)
{
	int stx = x - r;
	int sty = y - r;
	int edx = x + r;
	int edy = y + r;
	if (stx < 0) stx = 0;
	if (sty < 0) sty = 0;
	if (edx >= wd - 1) edx = wd - 2;
	if (edy >= ht - 1) edy = ht - 2;
	int sum = 0;
	for (int y = sty; y <= edy; y++)
	{
		int *p = pIntImg + y * wd + stx;
		for (int x = stx; x <= edx; x++)
		{
			sum += *(p++);
		}
	}
	return sum;
}

int CMemInt::GetCumLevel(int cnt)
{
	int sum = 0;
	for (int i = 255; i >= 0; i--)
	{
		sum += His[i];
		if (sum > cnt)
		{
			return i;
		}
	}
	return 0;
}

// Contrast stretch input [7/5/2016 Yuen]
void CMemInt::Normalize256(float cf)
{// Normalizing pInt to 0 - 255
	MinMaxV();
	cf = 1.8f;
	int *p = pIntImg;
	float sf = 255 / float(Max - Min);
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		int *p = pIntImg + y * wd;
		for (int x = 0; x < wd; x++)
		{
			int v = (*p - Min) * sf * cf;
			*p = (v < 0) ? 0 : ((v > 255) ? 255 : v);
			p++;
		}
	}
}

void CMemInt::MaxV()
{
	Max = INT_MIN;  // Max shows best match probability [9/29/2016 FSM]
	int *p = pIntImg;
	for (int i = 0; i < siz; i++)
	{
		if (*p > Max) Max = *p;
		p++;
	}
}

void CMemInt::MinMaxV()
{
	Max = INT_MIN;
	Min = INT_MAX;
	int *p = pIntImg;
	for (int i = 0; i < siz; i++)
	{
		if (*p > Max) Max = *p;
		if (*p < Min) Min = *p;
		p++;
	}
}

int CMemInt::Percentile(float pc)
{
	MaxV();
	int thres = Max * pc;
	counts = 0;
	int *p = pIntImg;
	for (int i = 0; i < siz; i++)
	{
		if (*p >= thres) counts++;
		p++;
	}
	return counts;
}

void CMemInt::SetValue(int v, int x, int y)
{
	pIntImg[y*wd + x] = v;
}

BOOL CMemInt::AllocMem(int sz)
{
	if (siz == sz)
	{// Allocated, simple return
		memset(pIntImg, 0, siz * sizeof(int));
		return TRUE;
	}
	Clear();
	pIntImg = new int[sz];
	if (pIntImg)
	{
		siz = sz;
		memset(pIntImg, 0, siz * sizeof(int));
		return TRUE;
	}
	return FALSE;
}