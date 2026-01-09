#include "stdafx.h"
#include "ImageMeasure.h"
#include "ImgPixel.h"

#define ABSL(x) (abs(x))
#define SQRE(x) ((x)*(x))

using namespace IMGL;

CImageMeasure::~CImageMeasure()
{
}

template<typename T>
double CImageMeasure::StdevScore()
{
	int wd = GetWidth();
	int ht = GetHeight();

	int *pn = new int[ht];
	long *psum = new long[ht];
	long *psum_sqr = new long[ht];
	if (!pn || !psum || !psum_sqr)
	{
		if (pn) delete[] pn;
		if (psum) delete[] psum;
		if (psum_sqr) delete[] psum_sqr;
		return 0;
	}
	memset(pn, 0, ht * sizeof(int));
	memset(psum, 0, ht * sizeof(long));
	memset(psum_sqr, 0, ht * sizeof(long));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			int v = p->GetIntensity();
			p++;
			psum[y] += v;
			psum_sqr[y] += SQRE(v);
			pn[y]++;
		}
	}
	// perform reduction here [7/8/2016 Yuen]
	int n = 0;
	long sum = 0;
	long sum_sqr = 0;
	for (int i = 0; i < ht; i++)
	{
		n += pn[i];
		sum += psum[i];
		sum_sqr += psum_sqr[i];
	}
	if (pn) delete[] pn;
	if (psum) delete[] psum;
	if (psum_sqr) delete[] psum_sqr;
	return sqrt(double(sum_sqr - SQRE(sum) / double(n)) / double(n - 1));
}

template<typename T>
float CImageMeasure::EntropyScore1()
{
	int wd = GetWidth();
	int ht = GetHeight();

	float probArray[256];
	memset(probArray, 0, 256 * sizeof(float));

	float entropy = 0;
	for (int y = 0; y < ht; y++)
	{
		T *a = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd - 1; x++)
		{
			short v = a->GetIntensity();
			probArray[v] += 1;
			a++;
		}
	}
	float max = 0;
	for (int i = 0; i < 256; i++)
	{
		if (probArray[i] > max) max = probArray[i];
	}
	//for (int i = 0; i < 256; i++)
	//{
	//	max += probArray[i];
	//}
	for (int i = 0; i < 256; i++)
	{
		float v = probArray[i] / max;
		if (v != 0)
		{
			entropy -= v * log(v);
		}
	}
	return entropy;
}

template<typename T>
float IMGL::CImageMeasure::VarianceScore1(float mean)
{
	int wd = GetWidth();
	int ht = GetHeight();

	long *psum = new long[ht]; if (!psum)return 0;
	memset(psum, 0, ht * sizeof(long));
	long *psum2 = new long[ht]; if (!psum2)return 0;
	memset(psum2, 0, ht * sizeof(long));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			float v = p->GetIntensity() - mean;
			p++;
			psum[y] += v;
			psum2[y] += v * v;
		}
	}

	long sum = 0;
	long sum2 = 0;
	for (int y = 0; y < ht; y++)
	{
		sum += psum[y];
		sum2 += psum2[y];
	}
	delete[] psum;
	delete[] psum2;
	return (sum2 - sum * sum) / (wd * ht);
}

template<typename T>
float IMGL::CImageMeasure::VarianceScore(float mean)
{
	int wd = GetWidth();
	int ht = GetHeight();

	long *psum = new long[ht]; if (!psum)return 0;
	memset(psum, 0, ht * sizeof(long));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			float v = p->GetIntensity() - mean;
			psum[y] += v * v;
			p++;
		}
	}

	long sum = 0;
	for (int y = 0; y < ht; y++)
	{
		sum += psum[y];
	}
	delete[] psum;
	return sum / float(wd*ht - 1);
}

template<typename T>
float CImageMeasure::EntropyScore()
{
	int wd = GetWidth();
	int ht = GetHeight();

	float entropy = 0;
	float probArray[512 + 1];
	memset(probArray, 0, 513 * sizeof(float));
	for (int y = 0; y < ht; y++)
	{
		T *a = (T *)GetPixelAddress(0, y);
		T *p1 = a;
		T *p2 = a + 1;
		for (int x = 0; x < wd - 1; x++)
		{
			probArray[p2->GetIntensity() - p1->GetIntensity() + 256] += 1;
			p1++; p2++;
		}
	}
	float max = 0;
	for (int i = 0; i < 512; i++)
	{
		if (probArray[i] > max) max = probArray[i];
	}
	//for (int i = 0; i < 512; i++)
	//{
	//	max += probArray[i];
	//}
	for (int i = 0; i < 512; i++)
	{
		float v = probArray[i] / max;
		if (v != 0)
		{
			entropy -= v * log(v);
		}
	}
	return entropy;
}

template<typename T>
double IMGL::CImageMeasure::ContrastScore()
{
	int wd = GetWidth();
	int ht = GetHeight();

	int *pn = new int[ht];
	long *psum = new long[ht];
	if (!pn || !psum)
	{
		if (pn) delete[] pn;
		if (psum) delete[] psum;
		return 0;
	}
	memset(pn, 0, ht * sizeof(int));
	memset(psum, 0, ht * sizeof(long));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *a = (T *)GetPixelAddress(1, y);
		T *p = a - 1;
		T *q = a;
		T *r = a + 1;
		for (int x = 1; x < wd - 1; x++)
		{
			pn[y]++;
			psum[y] += SQRE(p->GetIntensity() - q->GetIntensity()) + SQRE(q->GetIntensity() - r->GetIntensity());
			p++; q++; r++;
		}
	}

	// perform reduction here [7/8/2016 Yuen]
	int n = 0;
	long sum = 0;
	for (int i = 0; i < ht; i++)
	{
		n += pn[i];
		sum += psum[i];
	}

	if (pn) delete[] pn;
	if (psum) delete[] psum;

	return sum / double(2 * n);
}

template<typename T>
int IMGL::CImageMeasure::ThresScore(BYTE Thres)
{
	int ht = GetHeight();
	int wd = GetWidth();

	int *psum;
	psum = new int[ht];
	if (!psum) return 0;
	memset(psum, 0, ht * sizeof(int));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			if (p->GetIntensity() >= Thres) psum[y]++;
			p++;
		}
	}
	long sum = 0;
	for (int i = 0; i < ht; i++)
	{
		sum += psum[i];
	}
	delete[] psum;
	return sum;
}

template<typename T>
int IMGL::CImageMeasure::PercentileScore(float lim)
{
	int ht = GetHeight();
	int wd = GetWidth();

	int *psum; psum = new int[ht]; if (!psum) return 0;
	memset(psum, 0, ht * sizeof(int));
	int *pmax; pmax = new int[ht]; if (!pmax) return 0;
	memset(pmax, 0, ht * sizeof(int));

	// find max value [11/25/2016 Yuen]
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			short v = p->GetIntensity();
			if (v > pmax[y]) pmax[y] = v;
			p++;
		}
	}

	for (int y = 0; y < ht; y++)
	{
		pmax[y] *= lim;
	}

	// count intensity over lim * max value [11/25/2016 Yuen]
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			short v = p->GetIntensity();
			if (v > pmax[y]) psum[y]++;
			p++;
		}
	}

	int sum = 0;
	for (int i = 0; i < ht; i++)
	{
		sum += psum[i];
	}
	delete[] psum;
	delete[] pmax;
	return sum;
}

template<typename T>
short IMGL::CImageMeasure::AverageScore()
{
	int wd = GetWidth();
	int ht = GetHeight();

	long *psum = new long[ht];
	if (!psum) return 0;
	memset(psum, 0, ht * sizeof(long));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			psum[y] += p->GetIntensity();
			p++;
		}
	}
	long sum = 0;
	for (int i = 0; i < ht; i++)
	{
		sum += psum[i];
	}
	delete[] psum;
	return short(sum / (wd*ht));
}

// template<typename T>
// int IMGL::CImageMeasure::IntensityScore()
// {
// 	int ht = GetHeight();
// 	int wd = GetWidth();
//
// 	int *pcnt = new int[ht];
// 	int *psum = new int[ht];
// 	if (!pcnt || !psum)
// 	{
// 		if (pcnt) delete[] pcnt;
// 		if (psum) delete[] psum;
// 		return 0;
// 	}
// 	memset(pcnt, 0, ht * sizeof(int));
// 	memset(psum, 0, ht * sizeof(long));
//
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++)
// 	{
// 		T *p = (T *)GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++)
// 		{
// 			pcnt[y]++;
// 			psum[y] += p->GetIntensity();
// 			p++;
// 		}
// 	}
//
// 	int cnt = 0;
// 	int sum = 0;
// 	for (int i = 0; i < ht; i++)
// 	{
// 		cnt += pcnt[i];
// 		sum += psum[i];
// 	}
// 	if (pcnt) delete[] pcnt;
// 	if (psum) delete[] psum;
//
// 	if (cnt > 0) return sum / cnt;
// 	return 0;
// }

template<typename T>
float IMGL::CImageMeasure::CalcMean()
{
	int wd = GetWidth();
	int ht = GetHeight();

	long *psum = new long[ht]; if (!psum)return 0;
	memset(psum, 0, ht * sizeof(long));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			psum[y] += p->GetIntensity();
			p++;
		}
	}

	long sum = 0;
	for (int y = 0; y < ht; y++)
	{
		sum += psum[y];
	}
	delete[] psum;
	return sum / float(wd*ht);
}

// Count the number of pixel that where the intensity is > Thres [4/3/2015 VIT#1]
// No sanity checks [4/5/2015 VIT#1]
int CImageMeasure::GetThreshScore(BYTE Thres)
{
	if (IsNull()) return 0;
	CImgRgn Rgn;
	Rgn.SetSearchRgn(GetWidth(), GetHeight());

	switch (GetBYPP())
	{
	case 1:
		return ThresScore<sBINR>(Thres);
		break;
	case 3:
		return ThresScore<sBGR>(Thres);
		break;
	case 4:
		return ThresScore<sBGRA>(Thres);
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

int CImageMeasure::GetPercentileScore(float upper)
{
	if (IsNull()) return 0;

	switch (GetBYPP())
	{
	case 1:
		return PercentileScore<sBINR>(upper);
		break;
	case 3:
		return PercentileScore<sBGR>(upper);
		break;
	case 4:
		return PercentileScore<sBGRA>(upper);
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

//return sum of all pixel within rc area [4/3/2015 VIT#1]
//No sanity checks [4/5/2015 VIT#1]
// int CImageMeasure::GetIntensityScore()
// {
// 	if (IsNull()) return 0;
//
// 	switch (GetBYPP())
// 	{
// 	case 1:
// 		return IntensityScore<sBINR>();
// 		break;
// 	case 3:
// 		return IntensityScore<sBGR>();
// 		break;
// 	case 4:
// 		return IntensityScore<sBGRA>();
// 		break;
// 	default:
// 		ASSERT(0);
// 		break;
// 	}
// 	return 0;
// }

// Return average of pixel value in buffer [4/4/2015 VIT#1]
// Whole image operation
// bMono = monochrome image on RGB buffer [4/4/2015 VIT#1]
short CImageMeasure::GetAverageScore()
{
	if (IsNull()) return 0;

	switch (GetBYPP())
	{
	case 1:
		return AverageScore<sBINR>();
		break;
	case 3:
		return AverageScore<sBGR>();
		break;
	case 4:
		return AverageScore<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

// Normalize by size of image [4/4/2015 VIT#1]
double CImageMeasure::GetContrastScore()
{
	if (IsNull()) return 0;

	switch (GetBYPP())
	{
	case 1:
		return ContrastScore<sBINR>();
		break;
	case 3:
		return ContrastScore<sBGR>();
		break;
	case 4:
		return ContrastScore<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

// Var{X} = E(X-u}^2, where u = mean [4/5/2015 VIT#1]
double CImageMeasure::GetStdevScore()
{
	if (IsNull()) return 0;

	switch (GetBYPP())
	{
	case 1:
		return StdevScore<sBINR>();
		break;
	case 3:
		return StdevScore<sBGR>();
		break;
	case 4:
		return StdevScore<sBGRA>();
		break;
	default:
		ASSERT(0);
		return 0;
		break;
	}
	return 0;
}

float CImageMeasure::GetEntropyScore()
{
	if (IsNull()) return 0;

	switch (GetBYPP())
	{
	case 1:
		return EntropyScore<sBINR>();
		break;
	case 3:
		return EntropyScore<sBGR>();
		break;
	case 4:
		return EntropyScore<sBGRA>();
		break;
	default:
		ASSERT(0);
		return 0;
		break;
	}
	return 0;
}

float IMGL::CImageMeasure::GetVarianceScore()
{
	switch (GetBYPP())
	{
	case 1:
		return VarianceScore<sBINR>(CalcMean());
		break;
	case 3:
		return VarianceScore<sBGR>(CalcMean());
		break;
	case 4:
		return VarianceScore<sBGRA>(CalcMean());
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

float CImageMeasure::CalcMean()
{
	long sum = 0;
	switch (GetBYPP())
	{
	case 1:
		return CalcMean<sBINR>();
		break;
	case 3:
		return CalcMean<sBGR>();
		break;
	case 4:
		return CalcMean<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}