#include "stdafx.h"
#include <list>
#include <math.h>

#include "PackedDIB.h"
#include "IMG\tRGB.h"
#include "Line1D.h"

using namespace IMGL;

#define SQRE(x) ((x)*(x))

CPackedDIB::CPackedDIB()
{
	Ocx = Ocy = -1;
}

CPackedDIB::~CPackedDIB()
{
	ClearDIB();
}

void CPackedDIB::ClearDIB()
{
	Destroy();
}

template<typename T>
T * CPackedDIB::FillBackGrnd(T * p, int & n)
{
	if (n < 1)
	{
		return NULL;
	}
	T *q = p; // backup [7/7/2016 Yuen]

	// count number of pixel to fill [7/7/2016 Yuen]
	int i, cnt = 0;
	for (i = 0; i < n; i++)
	{
		if (p->G == FOREGRND)
		{
			break;
		}
		cnt++; p++;
	}
	n -= cnt;
	if (n > 0)
	{
		for (i = 0; i < cnt; i++)
		{
			q->SetGrey(FOREGRND); q++;
		}
	}
	return p;
}

template<typename T>
T * CPackedDIB::SkipBackGrnd(T * p, int & n)
{
	if (n < 1)
	{
		return NULL;
	}
	int i;
	for (i = 0; i < n; i++)
	{
		if (p->G == BACKGRND)
		{
			p++; continue;
		}
		n -= i;
		return p;
	}
	n -= i;
	return NULL;
}

template<typename T>
T * CPackedDIB::SkipForeGrnd(T * p, int & n)
{
	if (n < 1)
	{
		return NULL;
	}
	int i;
	for (i = 0; i < n; i++)
	{
		if (p->G == FOREGRND)
		{
			p++; continue;
		}
		n -= i;
		return p;
	}
	n -= i;
	return NULL;
}

template<typename T>
void IMGL::CPackedDIB::FillHoleLoop()
{
	int wd = GetWidth();
	int ht = GetHeight();

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		// Search for first valid transition [7/6/2016 Yuen]
		int x = 0;
		int n = wd - x;
		if (p->G == BACKGRND)
		{
			p = SkipBackGrnd(p, n);
		}
		if (n > 0) p = SkipForeGrnd(p, n);
		BOOL bIn = TRUE;
		while (n > 0)
		{
			if (bIn)
			{
				p = FillBackGrnd(p, n);
				bIn = FALSE;
			}
			else
			{
				p = SkipBackGrnd(p, n);
				bIn = TRUE;
			}
			if (n > 0) p = SkipForeGrnd(p, n); //if (n <= 0) break; // Next line [7/7/2016 Yuen]
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::FindBlob()
{
	// find connected pixel [7/16/2016 Yuen]
	// record count in DiB16
	int wd = GetWidth();
	int ht = GetHeight();

	CImage16L<int>DiB16;
	if (!DiB16.Create(wd, ht)) return;

	// Count consecutive lines [7/4/2016 Yuen]
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		BOOL bIn = FALSE;
		int counter = (y + 1) * 256; // expect each line at more have wd counter [7/16/2016 Yuen]
		T *p = (T *)GetPixelAddress(0, y); // image line [7/16/2016 Yuen]
		int *q = DiB16.GetPixelAddress(0, y); // buffer line [7/16/2016 Yuen]
		for (int x = 0; x < wd; x++)
		{
			if (p->G == FOREGRND)
			{ // Marked point [7/4/2016 Yuen]
				if (bIn)
				{
					*q = counter;
				}
				else
				{
					*q = counter++;
					bIn = TRUE;
				}
			}
			else bIn = FALSE;
			p++; q++;
		}
	}
	// find touching lines [7/16/2016 Yuen]
	std::vector<int> v;
	std::vector<std::vector<int>>vlist;
	for (int y = 1; y < ht; y++)
	{
		v.clear();
		int *p = DiB16.GetPixelAddress(0, y - 1); // buffer line [7/16/2016 Yuen]
		int *q = DiB16.GetPixelAddress(0, y); // buffer line [7/16/2016 Yuen]
		T *r = (T *)GetPixelAddress(0, y); // image line [7/16/2016 Yuen]
		int prevq = -1, prevp = -1;
		for (int x = 0; x < ht; x++)
		{
			if (*q)
			{
				if (*p && (*p != prevp) && (*p != *q))
				{
					if (prevq != *q)
					{ // push back first entry [7/16/2016 Yuen]
						if (v.size() > 0)
						{
							// prepare for next object on the same line [7/17/2016 Yuen]
							vlist.push_back(v);
							v.clear();
						}
						v.push_back(x);  v.push_back(y); v.push_back(*q); prevq = *q;
						//r->SetRed();
					}
					prevp = *p;
					v.push_back(*p);
					r->SetRed();
				}
				else
				{
					r->SetBlue();
				}
			}
			p++; q++; r++;
		}
		if (v.size() > 0) vlist.push_back(v);
	}

	// walk the list to find connected item [7/17/2016 Yuen]

	// dump result [7/16/2016 Yuen]
	FILE *fp = fopen("C:\\TEMP\\BLOB.csv", "wb");
	if (fp)
	{
		fprintf(fp, "X,Y,Q1,P1,P2,...\n");
		size_t n = vlist.size();
		for (size_t i = 0; i < n; i++)
		{
			std::vector<int> v = vlist[i];
			size_t m = v.size();
			if (m > 0)
			{
				for (UINT j = 0; j < m; j++)
				{
					if (j < m - 1)
					{
						fprintf(fp, "%d,", v[j]);
					}
					else
					{
						fprintf(fp, "%d", v[j]);
					}
				}
				fprintf(fp, "\n");
			}
		}
		fclose(fp);
	}
}

void IMGL::CPackedDIB::TestVector()
{
	std::list<int> v;
	std::list<std::list<int>>vlist;
	v.push_back(11);
	v.push_back(12);
	vlist.push_back(v);
	v.clear();
	v.push_back(21);
	v.push_back(22);
	v.push_back(23);
	vlist.push_back(v);

	std::list<std::list<int>>::iterator u = vlist.begin();
}

template<typename T>
void IMGL::CPackedDIB::FindLines(CHoughLine & H)
{
	int wd = GetWidth();
	int ht = GetHeight();
	//#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			if (p->IsFGrnd())
			{
				H.TransformLine(x, y);
			}
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::FindCircles(CHoughLine & H, int radius)
{
	// Built Hough space [8/5/2016 FSM]
	int wd = GetWidth();
	int ht = GetHeight();

	// Not possible to parallelize [8/5/2016 FSM]
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			if (p->IsFGrnd())
			{
				H.TransformCircle(x, y, radius);
			}
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::AddAve(CPackedDIB &DiB, BYTE BG)
{
	int wd = GetWidth();
	int ht = GetHeight();

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		T *q = (T *)DiB.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			p->Ave(q, BG);
			p++; q++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::HisAdd(int HisR[], int HisG[], int HisB[])
{
	int wd = GetWidth();
	int ht = GetHeight();

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			p->HisAdd(HisR, HisG, HisB);
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::HisEqualize(int HisR[], int HisG[], int HisB[])
{
	int wd = GetWidth();
	int ht = GetHeight();

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			p->HisEqualize(HisR, HisG, HisB);
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::HisCompute(int HisR[], int HisG[], int HisB[])
{
	int *pR = HisR, *pR1 = HisR + 1;
	int *pG = HisG, *pG1 = HisG + 1;
	int *pB = HisB, *pB1 = HisB + 1;
	// Cumulative histogram [2/14/2011 XPMUser]
	for (int x = 1; x < 256; x++)
	{
		*(pR1++) += *(pR++);
		*(pG1++) += *(pG++);
		*(pB1++) += *(pB++);
	}
	int cdfRmin = -1;
	int cdfGmin = -1;
	int cdfBmin = -1;
	pR = HisR;
	pG = HisG;
	pB = HisB;
	for (int x = 0; x < 256; x++)
	{
		if ((cdfRmin == -1) && (*pR > 0))
		{
			cdfRmin = *pR;
		}
		if ((cdfGmin == -1) && (*pG > 0))
		{
			cdfGmin = *pG;
		}
		if ((cdfBmin == -1) && (*pB > 0))
		{
			cdfBmin = *pB;
		}
		pR++; pG++; pB++;
	}

	int ImgSize = GetImageSize();
	float sfR = 255.0f / (ImgSize - cdfRmin);
	float sfG = 255.0f / (ImgSize - cdfGmin);
	float sfB = 255.0f / (ImgSize - cdfBmin);
	// Actual equalization [2/14/2011 XPMUser]
	pR = HisR;
	pG = HisG;
	pB = HisB;
	for (int x = 0; x < 256; x++)
	{
		*(pR++) = int((*pR - cdfRmin) * sfR);
		*(pG++) = int((*pG - cdfGmin) * sfG);
		*(pB++) = int((*pB - cdfBmin) * sfB);
	}
}

template<typename T>
void IMGL::CPackedDIB::MaxMinI(int & Max, int & Min)
{
	int wd = GetWidth();
	int ht = GetHeight();

	T *p = (T *)GetPixelAddress(0, 0);
	Max = Min = p->GetIntensity();
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			BYTE v = p->GetIntensity();
			if (v > Max) Max = v;
			if (v < Min) Min = v;
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::ContrastStretch(int Max, int Min, float TMax, float TMin)
{
	int wd = GetWidth();
	int ht = GetHeight();

	float df = (Max - Min);
	Min = Min + df * TMin;
	Max = Max - df * TMax;
	df = Max - Min;

	if (df) df = 255.0f / df; else df = 0;

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			short v = (p->GetIntensity() - Min) * df;
			p->SetGrey((v < 256) ? ((v >= 0) ? v : 0) : 255);
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::MaxMinL(CHslRgb & H, float & Max, float & Min, float * pL, float * pH, float * pS)
{
	int wd = GetWidth();
	int ht = GetHeight();

	Max = Min = H.GetL((T *)GetPixelAddress(0, 0));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		int off = y * wd;
		float *ppL = pL + off;
		float *ppH = pH + off;
		float *ppS = pS + off;

		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			H.HsL(p->R, p->G, p->G);
			float v = H.L;
			if (v > Max) Max = v;
			if (v < Min) Min = v;
			*(ppL++) = H.L;
			*(ppH++) = H.H;
			*(ppS++) = H.S;
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::ContrastColor(CHslRgb & H, float & Max, float & Min, float * pL, float * pH, float * pS)
{
	int ht = GetHeight();
	int wd = GetWidth();
	float sf = 1.0f / (Max - Min);
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		float *ppL = pL + y * wd;
		float *ppH = pH + y * wd;
		float *ppS = pS + y * wd;
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			H.RgB(*(ppH++), *(ppS++) + 0.15, (*(ppL++) - Min) * sf);
			p->Set(H.R, H.G, H.B);
			p++;
		}
	}
}

template<typename T>
float IMGL::CPackedDIB::AverageHue()
{
	CHslRgb H;
	int *pcnt = NULL;
	float *psum = NULL;

	int wd = GetWidth();
	int ht = GetHeight();

	pcnt = new int[ht];
	psum = new float[ht];
	if (!pcnt || !psum)
	{
		if (pcnt) delete[] pcnt;
		if (psum) delete[] psum;
		return -3;
	}
	memset(pcnt, 0, ht * sizeof(int));
	memset(psum, 0, ht * sizeof(int));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			pcnt[y]++;
			psum[y] += H.GetH(p);
			p++;
		}
	}
	// reduction [7/8/2016 Yuen]
	int cnt = 0;
	int sum = 0;
	for (int i = 0; i < ht; i++)
	{
		cnt += pcnt[i];
		sum += psum[i];
	}
	if (cnt)
	{
		return sum / (float)cnt;
	}
	return -2;
}

template<typename T>
float IMGL::CPackedDIB::AverageSat()
{
	CHslRgb H;
	int *pcnt = NULL;
	float *psum = NULL;
	int wd = GetWidth();
	int ht = GetHeight();

	pcnt = new int[ht];
	psum = new float[ht];
	if (!pcnt || !psum)
	{
		if (pcnt) delete[] pcnt;
		if (psum) delete[] psum;
		return -3;
	}
	memset(pcnt, 0, ht * sizeof(int));
	memset(psum, 0, ht * sizeof(int));

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			pcnt[y]++;
			psum[y] += H.GetS(p);
			p++;
		}
	}
	// reduction [7/8/2016 Yuen]
	int cnt = 0;
	int sum = 0;
	for (int i = 0; i < ht; i++)
	{
		cnt += pcnt[i];
		sum += psum[i];
	}
	if (cnt)
	{
		return sum / (float)cnt;
	}
	return -2;
}

template<typename T>
void IMGL::CPackedDIB::EdgeDetect()
{
	// tSrc is the source image [8/5/2016 FSM]
	// *this is the destination image [8/5/2016 FSM]
	if (IsNull())
	{
		ASSERT(0);
		return;
	}
	int wd = GetWidth();
	int ht = GetHeight();
	int sz = wd * sizeof(T);

	IMGL::CPackedDIB SrcDiB;  // Make a copy [8/5/2016 FSM]
	SrcDiB.Copy(*this);

#pragma omp parallel for
	for (int y = 0; y < ht - 1; y++)
	{
		T *q = (T *)GetPixelAddress(1, y);
		T *p1 = (T *)SrcDiB.GetPixelAddress(0, y) + 1;
		T *p2 = p1 - 1;
		T *p3 = (T *)SrcDiB.GetPixelAddress(0, y + 1) + 1;
		for (int x = 0; x < wd - 1; x++)
		{
			if ((p1->G == p2->G) && (p1->G == p3->G))
			{	// If both pixel the same
				q->SetBGrnd();
			}
			else if (p1->G == BACKGRND)
			{
				q->SetFGrnd();
			}
			p1++; p2++; p3++; q++;
		}
		memset(p1 - 2, BACKGRND, sizeof(T));  // last 4 points [7/10/2016 Yuen]
		p1 = (T *)GetPixelAddress(0, y);
		memset(p1, BACKGRND, sizeof(T));  // first 4 points [7/10/2016 Yuen]
	}
	// clear top and bottom line [8/5/2016 FSM]
	memset(GetPixelAddress(0, 0), BACKGRND, wd * sizeof(T));
	memset(GetPixelAddress(0, ht - 1), BACKGRND, wd * sizeof(T));
}

template<typename T>
void IMGL::CPackedDIB::ContrastHistoBW()
{
	int histo[256];
	memset(histo, 0, 256 * sizeof(int));

	int wd = GetWidth();
	int ht = GetHeight();

	// Generate histogram [2/14/2011 XPMUser]
#pragma omp parallel
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			histo[p->GetIntensity()]++;
			p++;
		}
	}
	int *pR = histo, *pR1 = histo + 1;
	// Cumulative histogram [2/14/2011 XPMUser]
	for (int x = 1; x < 256; x++)
	{
		*(pR1++) += *(pR++);
	}
	int cdfRmin = -1;
	pR = histo;
	for (int x = 0; x < 256; x++)
	{
		if ((cdfRmin == -1) && (*pR > 0))
		{
			cdfRmin = *pR;
		}
		pR++;
	}
	int ImgSize = wd * ht;
	short dd = 0;	// Control parameter [4/15/2013 Administrator]
	cdfRmin += dd;
	float sfR = (255 - dd) / float(ImgSize - cdfRmin);
	// Actual equalization [2/14/2011 XPMUser]
	pR = histo;
	int a;
	for (int x = 0; x < 256; x++)
	{
		a = int((*pR - cdfRmin) * sfR);
		if (a < 0) a = 0;
		if (a > 255) a = 255;
		*(pR++) = a;
	}

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			p->SetGrey(histo[p->GetIntensity()]);
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::ContrastHisto3LevelSet(int histo[])
{
	int ht = GetHeight();
	int wd = GetWidth();
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			p->SetGrey(histo[p->GetIntensity()]);
			p++;
		}
	}
}

template<typename T>
void IMGL::CPackedDIB::ContrastHisto3LevelCount(int histo[])
{
	// Generate histogram [2/14/2011 XPMUser]
	int ht = GetHeight();
	int wd = GetWidth();
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			histo[p->GetIntensity()]++;
			p++;
		}
	}
}

void CPackedDIB::AddAve(CPackedDIB &DiB, short BG)
{
	if (IsNull() || DiB.IsNull()) return;
	if (!IsSameSize(DiB)) return;
	if (GetBYPP() != DiB.GetBYPP()) return;

	switch (GetBYPP())
	{
	case 1:
		AddAve<sBINR>(DiB, BG);
		break;
	case 3:
		AddAve<sBGR>(DiB, BG);
		break;
	case 4:
		AddAve<sBGRA>(DiB, BG);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void CPackedDIB::HistogramEqualize()
{
	int histoR[256], histoG[256], histoB[256];

	memset(histoR, 0, 256 * sizeof(int));
	memset(histoG, 0, 256 * sizeof(int));
	memset(histoB, 0, 256 * sizeof(int));

	// Generate histogram [2/14/2011 XPMUser]
	switch (GetBYPP())
	{
	case 1:
		HisAdd<sBINR>(histoR, histoG, histoB);
		HisCompute<sBINR>(histoR, histoG, histoB);
		HisEqualize<sBINR>(histoR, histoG, histoB);
		break;
	case 3:
		HisAdd<sBGR>(histoR, histoG, histoB);
		HisCompute<sBGR>(histoR, histoG, histoB);
		HisEqualize<sBGR>(histoR, histoG, histoB);
		break;
	case 4:
		HisAdd<sBGRA>(histoR, histoG, histoB);
		HisCompute<sBGRA>(histoR, histoG, histoB);
		HisEqualize<sBGRA>(histoR, histoG, histoB);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void CPackedDIB::ContrastHistoBW()
{
	switch (GetBYPP())
	{
	case 1:
		ContrastHistoBW<sBINR>();
		break;
	case 3:
		ContrastHistoBW<sBGR>();
		break;
	case 4:
		ContrastHistoBW<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
}

// Evolution of ContrastBW [4/15/2013 Administrator]
// work with 24 bits pixel image only
void CPackedDIB::ContrastHisto3Level()
{
	int histo[256];
	memset(histo, 0, 256 * sizeof(int));

	// Generate histogram [2/14/2011 XPMUser]
	switch (GetBYPP())
	{
	case 1:
		ContrastHisto3LevelCount<sBINR>(histo);
		break;
	case 3:
		ContrastHisto3LevelCount<sBGR>(histo);
		break;
	case 4:
		ContrastHisto3LevelCount<sBGRA>(histo);
		break;
	default:
		ASSERT(0);
		break;
	}

	int *pR = histo, *pR1 = histo + 1;
	// Cumulative histogram [2/14/2011 XPMUser]
	for (int x = 1; x < 256; x++)
	{
		*(pR1++) += *(pR++);
	}
	int cdfRmin = -1;
	pR = histo;
	for (int x = 0; x < 256; x++)
	{
		if ((cdfRmin == -1) && (*pR > 0))
		{
			cdfRmin = *pR;
		}
		pR++;
	}
	int ImgSize = GetWidth()*GetHeight();
	short dd = 0;	// Control parameter [4/15/2013 Administrator]
	cdfRmin += dd;
	float sfR = (255 - dd) / float(ImgSize - cdfRmin);
	// Actual equalization [2/14/2011 XPMUser]
	pR = histo;
	int a;
	for (int x = 0; x < 256; x++)
	{
		a = int((*pR - cdfRmin) * sfR);
		if (a < 8) a = 0;
		else if (a < 253) a = 128;
		else a = 255;
		*(pR++) = a;
	}

	switch (GetBYPP())
	{
	case 1:
		ContrastHisto3LevelSet<sBINR>(histo);
		break;
	case 3:
		ContrastHisto3LevelSet<sBGR>(histo);
		break;
	case 4:
		ContrastHisto3LevelSet<sBGRA>(histo);
		break;
	default:
		ASSERT(0);
		break;
	}
}

void IMGL::CPackedDIB::FillHole()
{
	// look for foreground to background transition, mark location and
	// continue to find background to foreground transition and mark in
	// between as foreground [7/6/2016 Yuen]
	switch (GetBYPP())
	{
	case 1:
		FillHoleLoop<sBINR>();
		break;
	case 3:
		FillHoleLoop<sBGR>();
		break;
	case 4:
		FillHoleLoop<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
}

// Adaptive thresholding [3/10/2016 Yuen]
// Pixel below 90% of area average intensity is black otherwise it will be white[3/10/2016 Yuen]
BOOL CPackedDIB::Binarize()
{
	CLine1D His, His1;
	short ch = 1;

	CalcHistogram(His);
	//His.Dump(ch, "C:\\TEMP\\HISTO.CSV");
	for (int i = 0; i < 5; i++)
	{
		His.MovingAverage(ch, 9);
	}
	His.MaxMin(ch);
	//His.Dump(ch, "C:\\TEMP\\HISTO_AVE.CSV");
	His1 = His;
	//His1.Dump(ch, "C:\\TEMP\\HISTO_AVE.CSV");
	His1.Reduce(ch);
	His1.RemoveNearest(16);
	His1.MaxMin(ch);
	//His1.Dump(ch, "C:\\TEMP\\HISTO1.CSV");
	int Thres;
	if (His.ThresholdFirstMaxAndMin(ch, His1.GetFirstPeakIndex(ch), Thres))
	{
		Threshold(Thres);
		return TRUE;
	}
	return FALSE;
}

void CPackedDIB::PreProcess()
{
	//ContrastBW();
}

// Not applicable to grey image [7/8/2016 Yuen]
void CPackedDIB::ContrastColor()
{
	int sz = GetWidth()*GetHeight();

	float *pL = new float[sz];
	float *pH = new float[sz];
	float *pS = new float[sz];

	// Step 1: find Max and Min lightness L [5/16/2013 user]
	// Step 2: Show lightness map [5/16/2013 user]
	CHslRgb H;
	float Max, Min;
	switch (GetBYPP())
	{
	case 3:
		MaxMinL<sBGR>(H, Max, Min, pL, pH, pS);
		ContrastColor<sBGR>(H, Max, Min, pL, pH, pS);
		break;
	case 4:
		MaxMinL<sBGRA>(H, Max, Min, pL, pH, pS);
		ContrastColor<sBGRA>(H, Max, Min, pL, pH, pS);
		break;
	default:
		ASSERT(0);
		break;
	}

	delete[] pL;
	delete[] pH;
	delete[] pS;
}

void CPackedDIB::ContrastStretchColor(float TMax, float TMin)
{
	// Find Max Min [5/16/2013 user]
	int Max, Min;
	switch (GetBYPP())
	{
	case 1:
		MaxMinI<sBINR>(Max, Min);
		ContrastStretch<sBINR>(Max, Min, TMax, TMin);
		break;
	case 3:
		MaxMinI<sBGR>(Max, Min);
		ContrastStretch<sBGR>(Max, Min, TMax, TMin);
		break;
	case 4:
		MaxMinI<sBGRA>(Max, Min);
		ContrastStretch<sBGRA>(Max, Min, TMax, TMin);
		break;
	default:
		ASSERT(0);
		break;
	}
}

float CPackedDIB::AverageHue()
{
	switch (GetBYPP())
	{
	case 3:
		AverageHue<sBGR>();
		break;
	case 4:
		AverageHue<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

// Image transition detector [7/2/2016 Yuen]
// Only detect edges in horizontal direction only [7/2/2016 Yuen]
BOOL IMGL::CPackedDIB::EdgeDetect()
{
	// Taking a binarized image and mark at first black pixel at transition in red [7/2/2016 Yuen]

	// Assume BGR pixel format for the time being [7/2/2016 Yuen]
	// Assume binary image with 0 or 255 [7/2/2016 Yuen]
	switch (GetBYPP())
	{
	case 1:
		EdgeDetect<sBINR>();
		break;
	case 3:
		EdgeDetect<sBGR>();
		break;
	case 4:
		EdgeDetect<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

float CPackedDIB::AverageSat()
{
	switch (GetBYPP())
	{
	case 3:
		return AverageSat<sBGR>();
		break;
	case 4:
		return AverageSat<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

// Image analysis functions [7/3/2016 Yuen]
//////////////////////////////////////////////////////////////////////////
// Hough transformation on straight line [7/3/2016 Yuen]

void IMGL::CPackedDIB::FindLines(CHoughLine &H)
{
	if (!H.AllocLine(GetWidth(), GetHeight())) return;

	switch (GetBYPP())
	{
	case 1:
		FindLines<sBINR>(H);
		break;
	case 3:
		FindLines<sBGR>(H);
		break;
	case 4:
		FindLines<sBGRA>(H);
		break;
	default:
		ASSERT(0);
		return;
		break;
	}
}

// Hough transformation on a circle [7/3/2016 Yuen]

void IMGL::CPackedDIB::FindCircles(CHoughLine &H, int radius)
{
	if (!H.AllocCircle(GetWidth(), GetHeight())) return;

	switch (GetBYPP())
	{
	case 1:
		FindCircles<sBINR>(H, radius);
		break;
	case 3:
		FindCircles<sBGR>(H, radius);
		break;
	case 4:
		FindCircles<sBGRA>(H, radius);
		break;
	default:
		ASSERT(0);
		return;
		break;
	}
}

void IMGL::CPackedDIB::FindBlob()
{   // Require binarized image [7/4/2016 Yuen]
	// G channel is the binary image [7/16/2016 Yuen]
	// Count buffer storage type is CImage16L [7/16/2016 Yuen]
	//short Counter = 0;
	switch (GetBYPP())
	{
	case 1:
		FindBlob<sBINR>();
		break;
	case 3:
		FindBlob<sBGR>();
		break;
	case 4:
		FindBlob<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
}

// Check for bump given its radius [11/26/2016 Yuen]
BOOL IMGL::CPackedDIB::IsBumpArea(short radius, PAR::CCamImgPar *pPar)
{
	memset(H, 0, 10 * sizeof(float));

	float sizCenter = radius / float(GetWidth());
	H[0] = AreaIntensity(sizCenter, 0.5f, 0.5f, TRUE);
	if (pPar->bExcludeCenter)
	{
		int cnt = t_cnt;
		int sum = t_sum;
		float SizExclude = sizCenter / 2.0f;
		H[0] -= AreaIntensity(SizExclude, 0.5f, 0.5f, TRUE);
		cnt -= t_cnt;
		sum -= t_sum;
		if (cnt) H[0] = sum / float(cnt); else H[0] = 0;
	}
	int wd = GetWidth();
	float scale = sizCenter; if (pPar->bExcludeCenter) scale *= 0.705f;
	float pos = 0.5f - sizCenter * 1.20f;
	H[1] = AreaIntensity(scale, 1 - pos, pos, TRUE) / H[0];
	H[2] = AreaIntensity(scale, pos, pos, TRUE) / H[0];
	H[3] = AreaIntensity(scale, 1 - pos, 1 - pos, TRUE) / H[0];
	H[4] = AreaIntensity(scale, pos, 1 - pos, TRUE) / H[0];

	short cnt = 0;
	for (int i = 1; i <= 4; i++)
	{
		if (H[i] >= pPar->BrightLimit) cnt++;
	}
	if ((H[0] < pPar->DarkLimit) && cnt >= 3) return TRUE;
	return FALSE;
}

BOOL IMGL::CPackedDIB::IsBackground2(float lim)
{
	//float pos = 0.25f;
	float scale = 0.1f;
	//float x[9] = { 0.5f, 1 - pos, pos, 1 - pos, pos, 0.5f, 0.5f, pos, 1 - pos };
	//float y[9] = { 0.5f, pos, pos, 1 - pos, 1 - pos, pos, 1 - pos, 0.5f, 0.5f };

	float H[32];

#pragma omp parallel for
	for (int y = 0; y <= 4; y++)
	{
		for (int x = 0; x <= 4; x++)
		{
			H[y * 5 + x] = AreaIntensity(scale, 0.15f + 0.55f*x / 4.0f, 0.3f + 0.6f*y / 4.0f, TRUE);
		}
	}
	//for (int i = 0; i < 9; i++)
	//{
	//	H[i] = AreaIntensity(scale, x[i], y[i], TRUE);
	//}

	float max, min;
	max = min = H[0];
	dI = 0;
	for (int j = 0; j <= 4; j++)
	{
		for (int i = 0; i <= 4; i++)
		{
			int n = j * 5 + i;
			if (max < H[n]) max = H[n];
			if (min > H[n]) min = H[n];
		}
	}
	dI = max - min;
	if ((dI) < lim)
	{
		//float score = GetVarianceScore();
		//if (score > 250.0f)
		//{
		//	dI = score;
		//	return FALSE;  // Limit should set low [11/28/2016 FSM]
		//}
		return TRUE;
	}
	return FALSE;
}

BOOL IMGL::CPackedDIB::IsBackground(float lim)
{
	//CPackedDIB DiB;
	//DiB = (*this);
	//if (DiB.Binarize()) // This operation destroy original image [12/11/2016 Yuen]
	{
		if (!IsBackground2(lim))
		{
			float v = GetVarianceScore();
			if (v > 600)
			{
				return FALSE;
			}
		}
		float v = GetVarianceScore();
		if (v > 1000)
		{
			return FALSE;
		}
	}
	return TRUE;

	//////////////////////////////////////////////////////////////////////////
	ZeroMean();
	dI = GetContrastScore();
	if (dI > 500.0f)
	{
		return FALSE;  // Limit should set low [11/28/2016 FSM]
	}
	return TRUE;

	//////////////////////////////////////////////////////////////////////////
}