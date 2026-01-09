#include "stdafx.h"
#include <math.h>
#include <omp.h>

#include "ImageL.h"
#include "HSVRGB.h"
#include "ColorTable.h"
#include "TimeCounterEx.h"
#include "ImgPixel.h"

#define SQR(x) ((x)*(x))

using namespace IMGL;

BYTE Kernel3[] =
{
	0, 1, 0,
	1, 1, 1,
	0, 1, 0,
};
BYTE Kernel5[] =
{
	0, 0, 1, 0, 0,
	0, 1, 1, 1, 0,
	1, 1, 1, 1, 1,
	0, 1, 1, 1, 0,
	0, 0, 1, 0, 0,
};
BYTE Kernel7[] =
{
	0, 0, 1, 1, 1, 0, 0,
	0, 1, 1, 1, 1, 1, 0,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1,
	0, 1, 1, 1, 1, 1, 0,
	0, 0, 1, 1, 1, 0, 0
};

CImageL Rslt;  // Intermediate usage [2/12/2015 128]

CImageL::CImageL(void)
{
}

CImageL::~CImageL(void)
{
	Destroy();
}

template<typename T>
void CImageL::Convert2Grey()
{
	int wd = GetWidth();
	int ht = GetHeight();
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			p->MakeGrey();
			p++;
		}
	}
}

template<typename T>
BOOL CImageL::Threshold(BYTE lvl)
{
	int wd = GetWidth();
	int ht = GetHeight();
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			p->Threshold(lvl);
			p++;
		}
	}
	return TRUE;
}

// Obtain average intensity [7/14/2016 Yuen]
template<typename T>
int IMGL::CImageL::AreaIntensity(float scale, int xst, int xed, int yst, int yed, BOOL bShow)
{
	int ht = GetHeight();

	long *plsum = new long[ht];
	long *plcnt = new long[ht];

	if (!plsum || !plcnt)
	{
		if (plsum) delete[] plsum;
		if (plcnt) delete[] plcnt;
		return 0;
	}
	memset(plsum, 0, ht * sizeof(long));
	memset(plcnt, 0, ht * sizeof(long));

	// do not use omp here
	for (int y = yst; y < yed; y++)
	{
		T *p = (T *)GetPixelAddress(xst, y);
		for (int x = xst; x < xed; x++)
		{
			plsum[y] += p->GetIntensity();
			plcnt[y]++;
			if (bShow)
			{
				// Diagnostic information [3/25/2016 Yuen]
				if (((x - xst) < 3) || ((xed - x) < 3) || ((y - yst) < 3) || ((yed - y) < 3))
				{
					p->SetBlue();
				}
			}
			p++;
		}
	}
	t_sum = 0;
	t_cnt = 0;
	for (int i = 0; i < ht; i++)
	{
		t_sum += plsum[i];
		t_cnt += plcnt[i];
	}

	if (plsum) delete[] plsum;
	if (plcnt) delete[] plcnt;

	if (t_cnt > 0)
	{
		return t_sum / t_cnt;
	}
	return 0;
}

template<typename T>
int IMGL::CImageL::Histogram(CLine1D &Line, int xst, int xed, int yst, int yed)
{
	short nBin = 256;
	float sf = nBin / 256.0f;

	int ht = GetHeight();
	int *psum = new int[ht];	// intensity sum per line [7/13/2016 Yuen]
	int *phis = new int[3 * ht*nBin];	// histogram per line per channel [7/13/2016 Yuen]

	if (!psum || !phis)
	{
		if (psum) delete[] psum;
		if (phis) delete[] phis;
	}

	memset(psum, 0, ht * sizeof(int));
	memset(phis, 0, 3 * ht*nBin * sizeof(int));

	t_sum = 0; // image property [7/11/2016 Yuen]
	t_bright = 0; // image property [7/11/2016 Yuen]

#pragma omp parallel for
	for (int y = yst; y < yed; y++)
	{
		BYTE r, g, b;
		T *p = (T *)GetPixelAddress(xst, y);
		int n = nBin * y;
		int *pR = phis + n;
		int *pG = phis + n + nBin;
		int *pB = phis + n + 2 * nBin;
		for (int x = xst; x < xed; x++)
		{
			int v = p->Get(r, g, b);
			psum[y] += v;
			*(pR + r) += 1;
			*(pG + g) += 1;
			*(pB + b) += 1;
			p++;
		}
	}
	// Perform reduction here [6/30/2016 Yuen]
	// and find Max histogram [3/25/2016 Yuen]
	t_sum = 0; // image property [7/11/2016 Yuen]
	Line.Clear();
	for (int y = yst; y < yed; y++)
	{
		int n = nBin * y;
		int *pR = phis + n;
		int *pG = phis + n + nBin;
		int *pB = phis + n + 2 * nBin;
		for (int i = 0; i < nBin; i++)
		{
			Line.histg[0][i] += *pR; pR++;
			Line.histg[1][i] += *pG; pG++;
			Line.histg[2][i] += *pB; pB++;
		}
		t_sum += psum[y];
	}

	// #pragma omp parallel for
	for (int i = 0; i < 3; i++)
	{
		Line.MaxMin(i);
	}

	t_bright = t_sum / ((yed - yst) * (xed - xst));  // image property [7/11/2016 Yuen]

	delete[] psum;
	delete[] phis;

	return Line.histg_maxI[1];
}

template<typename T>
BOOL IMGL::CImageL::ReduceSize(CImageL & Src, short reduction)
{
	int wd = Src.GetWidth() / reduction;
	int ht = Src.GetHeight() / reduction;
	int bpp = Src.GetBPP();

	if (!CreateIf(wd, ht, bpp)) return FALSE;

	wd = GetWidth();
	ht = GetHeight();

#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T * pSrc = (T *)Src.GetPixelAddress(0, y*reduction);
		T * pDes = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			pDes->Set(pSrc);
			pDes++;
			pSrc += reduction;
		}
	}
	return TRUE;
}

template<typename T>
long IMGL::CImageL::AreaCount(BYTE thres)
{
	int wd = GetWidth();
	int ht = GetHeight();

	int *pcnt = new int[ht];
	if (!pcnt) return 0;

	memset(pcnt, 0, ht * sizeof(T));
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; y < wd; x++)
		{
			if (p->GetIntensity() > thres) pcnt[y]++;
		}
	}
	long count = 0;
	for (int y = 0; y < ht; y++)
	{
		count += pcnt[y];
	}
	delete[] pcnt;
	return count;
}

template<typename T>
void IMGL::CImageL::SubOffset(short amount)
{
	int wd = GetWidth();
	int ht = GetHeight();
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		T *p = (T *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			p->SubOff(amount); p++;
		}
	}
}

template<typename T>
void IMGL::CImageL::Erosion(short nK)
{   // all points on kernel is copied to image if POI is foreground
	if ((nK < 3) || (nK > 7))
	{
		// input error [8/5/2016 FSM]
		ASSERT(0);
		return;
	}

	int wd = GetWidth();
	int ht = GetHeight();

	// Make duplicate [7/10/2016 Yuen]
	CImageL DiB;
	if (!DiB.Copy(*this)) return;

	BYTE *pK;
	switch (nK)
	{
	case 5:
		pK = Kernel5;
		break;
	case 7:
		pK = Kernel7;
		break;
	default:
		nK = 3;
		pK = Kernel3;
		break;
	}
	BYTE *k[7]; // Allocate max amount [8/5/2016 FSM]
	for (int i = 0; i < nK; i++)
	{
		k[i] = pK + i * nK;
	}

	int count = 0;
	for (int i = 0; i < nK*nK; i++)
	{
		if (pK[i]) count++;
	}

	int n = nK / 2;
#pragma omp parallel for
	for (int y = n; y < ht - n; y++)
	{
		// Set buffer pointers [7/9/2016 Yuen]
		T *p[7]; // Allocate max amount [8/5/2016 FSM]
		for (int j = 0; j < nK; j++)
		{
			p[j] = (T *)DiB.GetPixelAddress(0, y - (j - n));
		}

		T *S = (T *)DiB.GetPixelAddress(n, y);
		T *D = (T *)GetPixelAddress(n, y);

		for (int x = n; x < wd - n; x++)
		{
			if (S->G == FOREGRND)
			{ // if POI is foreground, set all position with kernel value of 1 to FOREGRND [7/9/2016 Yuen]
				int cnt = 0;
				for (int i = 0; i < nK; i++)
				{
					for (int j = 0; j < nK; j++)
					{
						if (*(k[j] + i))
						{
							cnt += ((p[j]->G == FOREGRND) ? 1 : 0);
						}
					}
					for (int j = 0; j < nK; j++) p[j]++;
				}
				for (int j = 0; j < nK; j++) p[j] -= nK;

				if (cnt < count) D->SetGrey(BACKGRND);
			}
			S++; D++;
			for (int j = 0; j < nK; j++) p[j]++;
		}
	}

	// clear top and bottom 3 lines [7/17/2016 Yuen]
	T *D;
	for (int y = 0; y < n; y++)
	{
		D = (T *)GetPixelAddress(0, y);
		memset(D, BACKGRND, wd * sizeof(T));
		D = (T *)GetPixelAddress(0, ht - y - 1);
		memset(D, BACKGRND, wd * sizeof(T));
	}
}

template<typename T>
void IMGL::CImageL::Dilation(short nK)
{   // all points on kernel is copied to image if POI is foreground
	if ((nK < 3) || (nK > 7))
	{
		// input error [8/5/2016 FSM]
		ASSERT(0);
		return;
	}

	int wd = GetWidth();
	int ht = GetHeight();

	// Make duplicate [7/10/2016 Yuen]
	CImageL DiB;
	if (!DiB.Copy(*this)) return;

	BYTE *pK;
	switch (nK)
	{
	case 5:
		pK = Kernel5;
		break;
	case 7:
		pK = Kernel7;
		break;
	default:
		nK = 3;
		pK = Kernel3;
		break;
	}

	BYTE *k[7];
	for (int i = 0; i < nK; i++)
	{
		k[i] = pK + i * nK;
	}

	int n = nK / 2;
#pragma omp parallel for
	for (int y = n; y < ht - n; y++)
	{
		// Set buffer pointers [7/9/2016 Yuen]
		T *q[7]; // Allocate max amount [8/5/2016 FSM]
		for (int j = 0; j < nK; j++)
		{
			q[j] = (T *)GetPixelAddress(0, y - (j - n));
		}
		T *S = (T *)DiB.GetPixelAddress(n, y);

		for (int x = n; x < wd - n; x++)
		{
			if (S->G == FOREGRND)
			{ // if POI is foreground, set all position with kernel value of 1 to FOREGRND [7/9/2016 Yuen]
				for (int i = 0; i < nK; i++)
				{
					for (int j = 0; j < nK; j++)
					{
						if (*(k[j] + i) && (q[j]->G == BACKGRND)) q[j]->SetGrey(FOREGRND);
						q[j]++;
					}
				}
				for (int j = 0; j < nK; j++) q[j] -= nK;
			}
			S++;
			for (int j = 0; j < nK; j++) q[j]++;
		}
	}
}

BOOL CImageL::ReduceSize(CImageL &Src, short reduction)
{
	int wd = Src.GetWidth() / reduction;
	int ht = Src.GetHeight() / reduction;

	if (IsNull())
	{
		Create(wd, ht, Src.GetBPP());
		ptype = Src.ptype;
	}
	else
	{
		if ((GetWidth() != wd) || (GetHeight() != ht))
		{
			Destroy();
			Create(wd, ht, Src.GetBPP());
			ptype = Src.ptype;
		}
	}

	if (IsNull()) return FALSE;

	switch (GetBYPP())
	{
	case 1:
		return ReduceSize<sBINR>(Src, reduction);
		break;
	case 3:
		return ReduceSize<sBGR>(Src, reduction);
		break;
	case 4:
		return ReduceSize<sBGRA>(Src, reduction);
		break;
	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}

BOOL CImageL::GetCG(float &Cx, float &Cy)
{
	if (IsNull()) return FALSE;

	int ex = GetWidth();
	int ey = GetHeight();

	int bypp = GetBYPP();
	double sumXY = 0, sumPx = 0, sumPy = 0;
#pragma omp parallel for reduction (+:sumXY) reduction (+:sumPx) reduction (+:sumPy)
	for (int y = 0; y < ey; y++)
	{
		PBYTE pS = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < ex; x++)
		{
			BYTE p = SQR(255 - *pS);
			sumXY += p;
			sumPx += p * x;
			sumPy += p * y;
			pS += bypp;
		}
	}
	Cx = sumPx / sumXY;
	Cy = sumPy / sumXY;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Processing [3/11/2015 Yuen]
// This function performs a 3 x 3 convolution on the active image.The
// kernel array is passed externally.Edges are added(doubly weighted)
// for the computation.

void IMGL::CImageL::ConvlLowPass()
{
	Convl(0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f);
}

void IMGL::CImageL::ConvlHighPass()
{
	Convl(-1.0, -1.0, -1.0, -1.0, 9.0, -1.0, -1.0, -1.0, -1.0);
}

void IMGL::CImageL::ConvlVSobel()
{
	Convl(-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0);
}

void IMGL::CImageL::ConvlVSobelI()
{
	Convl(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);
}

void IMGL::CImageL::ConvlHSobel()
{
	Convl(-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0);
}

void IMGL::CImageL::ConvlHSobelI()
{
	Convl(1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
}

void IMGL::CImageL::ConvlFreiChen()
{
	Convl(1.0, 0.0, -1.0, 1.41421f, 0.0, -1.41421f, 1.0, 0.0, -1.0);
}

void IMGL::CImageL::ConvlVPrewitt()
{
	Convl(-1.0, 0.0, 1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 1.0);
}

void IMGL::CImageL::ConvlHPrewitt()
{
	Convl(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, -1.0, -1.0, -1.0);
}

void IMGL::CImageL::Convl45Grad()
{
	Convl(1.0, 1.0, 0.0, 1.0, 1.0, -1.0, 0.0, -1.0, -1.0);
}

void IMGL::CImageL::ConvlNGrad()
{
	Convl(1.0, 1.0, 1.0, 1.0, -2.0, 1.0, -1.0, -1.0, -1.0);
}

void IMGL::CImageL::ConvlVShiftDiff()
{
	Convl(0.0, 0.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0, 0.0);
}

void IMGL::CImageL::ConvlLaplacian()
{
	Convl(0.0, -1.0, 0.0, -1.0, 4.0, -1.0, 0.0, -1.0, 0.0);
}

void IMGL::CImageL::ConvlLaplacianSharp()
{
	Convl(-1.0, -1.0, -1.0, -1.0, 8.0, -1.0, -1.0, -1.0, -1.0);
}

void IMGL::CImageL::ConvlDiagLaplacian()
{
	Convl(1.0, 0.0, 1.0, 0.0, -4.0, 0.0, 1.0, 0.0, 1.0);
}

void IMGL::CImageL::ConvlAvgDLaplacian()
{
	Convl(1.0, -2.0, 1.0, -2.0, 4.0, -2.0, 1.0, -2.0, 1.0);
}

void IMGL::CImageL::ConvlCompLaplacian()
{
	Convl(0.0, -1.0, 0.0, -1.0, 5.0, -1.0, 0.0, -1.0, 0.0);
}

void IMGL::CImageL::ConvlLineEnh()
{
	Convl(0.0, 1.0, 0.0, -1.0, 1.0, -1.0, 0.0, 1.0, 0.0);
}

void IMGL::CImageL::ConvlEdgeDet()
{
	Convl(-0.125f, -0.125f, -0.125f, -0.125f, 1.0, -0.125f, -0.125f, -0.125f, -0.125f);
}

void IMGL::CImageL::ConvlEdgeEnh()
{
	Convl(-0.15f, -0.15f, -0.15f, -0.15f, 1.0, -0.15f, -0.15f, -0.15f, -0.15f);
}

void IMGL::CImageL::ConvlEdgeKirsh()
{
	Convl(5.0, 5.0, 5.0, -3.0, 0.0, -3.0, -3.0, -3.0, -3.0);
}

void IMGL::CImageL::ConvlEmboss()
{
	Convl(-2.0, -1.0, 0.0, -1.0, 1.0, 1.0, 0.0, 1.0, 2.0);
}

void IMGL::CImageL::ConvlHEmboss()
{
	Convl(-1.0, -1.0, -1.0, 0.2f, 0.2f, 0.2f, 1.0, 1.0, 1.0);
}

void IMGL::CImageL::ConvlVEmboss()
{
	Convl(-1.0, 0.2f, 1.0, -1.0, 0.2f, 1.0, -1.0, 0.2f, 1.0);
}

void CImageL::Show(CDC *pDC, CRect &rc)
{
	GetBestRect(rc);
	Draw(pDC->GetSafeHdc(), rc);
}

float CImageL::GetRelPosX(int pos)
{
	return pos / float(GetWidth());
}

float CImageL::GetRelPosY(int pos)
{
	return pos / float(GetHeight());
}

SRect_type CImageL::GetRelRect(CRect& rc)
{
	SRect_type r;
	float wd = float(GetWidth());
	r.left = rc.left / wd;
	r.right = rc.right / wd;

	float ht = float(GetHeight());
	r.top = rc.top / ht;
	r.bottom = rc.bottom / ht;

	return r;
}

void CImageL::GetBestRect(CRect& rc1)
{
	int nwd, nht;
	float aspImg = this->GetWidth() / float(this->GetHeight());
	float aspScr = rc1.Width() / float(rc1.Height());
	if (aspImg > aspScr)
	{
		nwd = rc1.Width();
		nht = int(nwd / aspImg);
	}
	else
	{
		nht = rc1.Height();
		nwd = int(nht * aspImg);
	}
	int x = (rc1.Width() - nwd) / 2;

	rc1.SetRect(x, 0, x + nwd, nht);  // shift y up to 0, ie y is always start from 0 [3/9/2015 Yuen]
}

template<class T>
BOOL CImageL::Convert(CImage16L<T> &Image)
{
	if (Image.IsNull())
	{
		return FALSE;
	}
	if (IsNull())
	{
		Create(Image.GetWidth(), Image.GetHeight(), 24);
		ptype = Image.ptype;
	}
	if (!IsSameSize(Image))
	{
		Destroy();
		Create(Image.GetWidth(), Image.GetHeight(), 24);
		ptype = Image.ptype;
	}
	return Scale(Image);
}

//************************************
// Method:    Scale, scale Image (16 bits mono) to this
// FullName:  IMGL::CImageL::Scale
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: CImage16L & Image
// Assumption: this and Image are qualified
//************************************
template<class T>
BOOL CImageL::Scale(CImage16L<T> &Image)
{
	int wd = GetWidth();
	int ht = GetHeight();

	CColorTable LUT;
	int n = 256;
	LUT.Create0(n, 1, 1, 1);
	LUT.pLUT[255].b = 0; LUT.pLUT[255].g = 0;
	LUT.pLUT[254].b = 0; LUT.pLUT[254].g = 0;

	float fmin, fmax;
	Image.GetMaxMin(fmin, fmax);
	float sf = n / (fmax - fmin);
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		BYTE * p = (BYTE *)GetPixelAddress(0, y);
		float *q = (float *)Image.GetPixelAddress(0, y);
		// assumed 24 bits  [3/8/2015 Yuen]
		for (int x = 0; x < wd; x++)
		{
			short v = short((*(q++) - fmin) *sf);
			if (v < 0) v = 0;
			if (v > 255) v = 255;
			*(p++) = BYTE(LUT.pLUT[v].b * 255);
			*(p++) = BYTE(LUT.pLUT[v].g * 255);
			*(p++) = BYTE(LUT.pLUT[v].r * 255);
		}
	}
	return TRUE;
}

// Convert to 24 bits pixel format [1/28/2015 FSM]
// Convert bottom up image to top down
// Rearrange pixel from BGR to RGB [1/28/2015 FSM]
// If image is already top down, convert BGR to RGB [1/28/2015 FSM]
BOOL CImageL::Convert24BitTopDown(BOOL bBGR)
{
	if (this->GetBPP() < 8) return FALSE;  // Not supported [1/28/2015 FSM]
	if ((this->GetPitch() > 0) && !bBGR) return TRUE; // Already in proper format [1/28/2015 FSM]

	byte *buf;
	int i, j;
	int nx, ny;
	RGBQUAD *pRGB = new RGBQUAD[256];	// For GetDIBColorTable()
	if (!pRGB) return FALSE;

	nx = this->GetWidth();
	ny = this->GetHeight();
	unsigned long n = nx * ny;	// No. of pixels

	// Allocate n sized buffer for temp storage
	buf = new byte[n * 3];
	if (!buf)
	{
		delete[] pRGB;
		return FALSE;
	}

	switch (this->GetBPP())
	{
	case 8:
		ptype = ptGREY;
		i = GetDIBColorTable(this->GetDC(), 0, 256, pRGB);
		if (!i)
		{
			this->ReleaseDC();
			delete[] pRGB;
			delete[] buf;
			return FALSE;
		}
		this->ReleaseDC();
#pragma omp parallel for private(i)
		for (j = 0; j < ny; j++)
		{
			BYTE *q = buf + j * nx;
			BYTE *t = (byte *)this->GetPixelAddress(0, j);
			for (i = 0; i < nx; i++)
			{
				*(q++) = pRGB[*t].rgbRed;
				*(q++) = pRGB[*t].rgbGreen;
				*(q++) = pRGB[*t].rgbBlue;
				t++;
			}
		}
		break;
	case 24:
	{
		ptype = ptBGR;
		BYTE *q = buf;
		for (j = 0; j < ny; j++)
		{
			// Addr. next row (avoids 24 bit offset and bottom-up calc.)
			BYTE *t = (byte *)this->GetPixelAddress(0, j);
			if (bBGR)
			{
				for (i = 0; i < nx; i++)
				{
					*(q + 2) = *(t++);			// CImage is BGR
					*(q + 1) = *(t++);
					*(q) = *(t++);
					q += 3;
				}
			}
			else
			{
				memcpy(q, t, nx * 3);			// CImage is RGB
				q += nx * 3;
			}
		}
	}
	break;
	case 32:	// Just need to make top-down
	{
		ptype = ptBGRA;
		BYTE *q = buf;
		for (j = 0; j < ny; j++)
		{
			// Addr. next row (avoids bottom-up calc.)
			// Image is already RGB ??? [1/28/2015 FSM]
			BYTE *t = (byte *)this->GetPixelAddress(0, j);
			if (bBGR)
			{
				for (i = 0; i < nx; i++, q += 3)
				{
					t++;
					*(q + 2) = *(t++);			// CImage is BGR
					*(q + 1) = *(t++);
					*(q) = *(t++);
				}
			}
			else
			{
				for (i = 0; i < nx; i++, q += 3)
				{
					t++;
					*(q++) = *(t++);			// CImage is RGB
					*(q++) = *(t++);
					*(q++) = *(t++);
				}
			}
		}
	}
	break;
	}

	// Start a new CImage
	this->Destroy();
	if (!this->Create(nx, -(int)ny, 24, 0))
	{
		delete[] pRGB;
		delete[] buf;
		return FALSE;
	}
	//this->GetDim();
	BYTE *t = (byte *)this->GetBits();	// Ptr to new bitmap (top-down DIB)
	memcpy_s(t, n * 3, buf, n * 3); // Copy buf to bitmap
	this->ptype = ptBGR;			// Update pixel type

	delete[] pRGB;
	delete[] buf;
	return TRUE;
}

// Convert to 32 bits pixel format [1/28/2015 FSM]
// Convert bottom up image to top down
// Rearrange pixel from BGR to RGB [1/28/2015 FSM]
// If image is already top down, convert BGR to RGB [1/28/2015 FSM]
BOOL CImageL::Convert32BitTopDown(BOOL bBGR)
{
	if (this->GetBPP() < 8) return FALSE;  // Not supported [1/28/2015 FSM]

	byte *t, r, g, b;
	int *p, *q, *buf;
	unsigned long i, j, nx, ny;
	RGBQUAD *pRGB = new RGBQUAD[256];	// For GetDIBColorTable()
	if (!pRGB) return FALSE;

	nx = this->GetWidth();
	ny = this->GetHeight();
	unsigned long n = nx * ny;	// No. of pixels

	// Allocate n sized buffer for temp storage
	buf = (int *)new int[n];
	if (!buf)
	{
		delete[] pRGB;
		return FALSE;
	}

	switch (this->GetBPP())
	{
	case 8:
		ptype = ptGREY;
		i = GetDIBColorTable(this->GetDC(), 0, 256, pRGB);
		if (!i)
		{
			this->ReleaseDC();
			delete[] pRGB;
			delete[] buf;
			return FALSE;
		}
		this->ReleaseDC();

		for (j = 0, q = buf; j < ny; j++)
		{
			t = (byte *)this->GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, t++, q++)
			{
				r = pRGB[*t].rgbRed;
				g = pRGB[*t].rgbGreen;
				b = pRGB[*t].rgbBlue;
				*q = RGB(b, g, r);	// CImage is BGR
			}
		}
		break;
	case 24:
		ptype = ptBGR;
		for (j = 0, q = buf; j < ny; j++)
		{
			// Addr. next row (avoids 24 bit offset and bottom-up calc.)
			t = (byte *)this->GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, t++, q++)
			{
				b = *t;			// CImage is BGR
				g = *(++t);
				r = *(++t);
				*q = RGB(b, g, r);
			}
		}
		break;
	case 32:	// Just need to make top-down
		ptype = ptBGRA;
		for (j = 0, q = buf; j < ny; j++)
		{
			// Addr. next row (avoids bottom-up calc.)
			// Image is already RGB ??? [1/28/2015 FSM]
			p = (int *)this->GetPixelAddress(0, j);
			for (i = 0; i < nx; i++, p++, q++)
			{
				*q = *p;
			}
		}
		break;
	}

	// Start a new CImage
	this->Destroy();
	if (!this->Create(nx, -(int)ny, 32, 0))
	{
		delete[] pRGB;
		delete[] buf;
		return FALSE;
	}
	//this->GetDim();
	p = (int *)this->GetBits();	// Ptr to new bitmap (top-down DIB)
	memcpy_s(p, n * sizeof(int), buf, n * sizeof(int)); // Copy buf to bitmap
	this->ptype = ptBGR;			// Update pixel type

	delete[] pRGB;
	delete[] buf;
	return TRUE;
}

BOOL CImageL::AutoBrightness()
{
	if (IsNull()) return FALSE;

	if (GetMinMax())
	{
		NrmlzRange(ptBGR);
		return TRUE;
	}
	return FALSE;
}
BOOL CImageL::GetMinMax()
{
	if (this->IsNull())
	{
		minmax.found = FALSE;
		return FALSE;
	}
	minmax.max = 0;
	minmax.min = 255;

	int *pmax = &minmax.max;
	int *pmin = &minmax.min;

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pSrc = (BYTE *)GetBufferStart();

	int i, ImgSize;
	switch (GetBPP())
	{
	case 8:
		ImgSize = GetBufferSize();
		for (i = 0; i < ImgSize; i += 8)
		{
			if (*pSrc < *pmin) *pmin = *pSrc;
			if (*pSrc > *pmax) *pmax = *pSrc;
			pSrc++;
		}
		minmax.found = TRUE;
		break;
	case 24:
		ImgSize = GetBufferSize();
		for (i = 0; i < ImgSize; i++)
		{
			if (*pSrc < *pmin) *pmin = *pSrc;
			if (*pSrc > *pmax) *pmax = *pSrc;
			pSrc++;
		}
		minmax.found = TRUE;
		break;
	case 32:
		ImgSize = GetBufferSize();
		for (i = 0; i < ImgSize; i += 4)
		{
			pSrc++;
			for (int j = 0; j < 3; j++)
			{
				if (*pSrc < *pmin) *pmin = *pSrc;
				if (*pSrc > *pmax) *pmax = *pSrc;
				pSrc++;
			}
		}
		minmax.found = TRUE;
		break;
	default:
		minmax.found = FALSE;
		return FALSE;
		break;
	}
	return TRUE;
}

/*----------------------------------------------------------------------
  This function normalizes the histogram from the passed range to 0..255.
  (For calling interactively from the slider dialog.)
  ----------------------------------------------------------------------*/
void CImageL::NrmlzRange(int ptype)
{
	float factor;
	byte r = 0, g = 0, b = 0;

	if (!minmax.found) return;

	if (minmax.max - minmax.min == 0)
	{
		factor = 32767.;	// Avoid div. by 0
	}
	else
	{
		factor = (float)((float)255 / (float)(minmax.max - minmax.min));
	}

	int *pmax = &minmax.max;
	int *pmin = &minmax.min;

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *p = (BYTE *)GetBufferStart();	// Ptr to bitmap
	unsigned long n = GetImageSize();

	switch (ptype)
	{
	case ptGREY:
		for (; n > 0; n--, p++)
		{
			*p = (BYTE)((*p - *pmin) * factor);
		}
		break;
	case ptBGR:
	case ptBGRA:
		switch (GetBPP())
		{
		case 24:
			for (; n > 0; n--)
			{
				*p = (BYTE)((*p - *pmin) * factor); p++;
				*p = (BYTE)((*p - *pmin) * factor); p++;
				*p = (BYTE)((*p - *pmin) * factor); p++;
			}
			break;
		case 32:
			for (; n > 0; n--)
			{
				p++;
				*p = (BYTE)((*p - *pmin) * factor); p++;
				*p = (BYTE)((*p - *pmin) * factor); p++;
				*p = (BYTE)((*p - *pmin) * factor); p++;
			}
			break;
		}
		break;
	default:	// INTG
		// Not tested yet [1/28/2015 FSM]
		int *q = (int *)p;
		for (; n > 0; n--)
		{
			r = (int)((float)(*p - *pmin) * factor);
			*p = (r << 16) + (r << 8) + r;
		}
		ptype = ptGREY;	// Changed type
		break;
	}
}

/*----------------------------------------------------------------------
  This function computes a 256 level (and out of bounds <0 and >255)
  histogram of the active image. RGB bitmaps have ea. color component
  counted as well.

  Showing the histogram dialog window is handled by HistDlg. If the user
  hasn't selected to display the hist. then the hist calc. can proceed
  quietly, so some functions (like Eqliz()) can call Hist() w/o
  displaying it.

  Returns true if successful.
  ----------------------------------------------------------------------*/
BOOL CImageL::Hist()
{
	if (!minmax.found) return FALSE;

	histg_ovr = histg_neg = 0;

	int a1 = 0;
	int a2 = 1;
	int a3 = 2;
	if (ptype == ptBGR)
	{
		a1 = 2;
		a3 = 0;
	}
	unsigned long *hisA1 = histg[a1];
	unsigned long *hisA2 = histg[a2];
	unsigned long *hisA3 = histg[a3];

	int byPP = GetBPP();

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *p = (BYTE *)GetBufferStart();	// Ptr to bitmap
	unsigned long n = GetImageSize();

	/* Hash *p to a h level index & accumulate count */
	switch (ptype)
	{
	case ptGREY:
		ZeroMemory(histg, 256 * sizeof(unsigned long));
		for (; n > 0; n--, p++)
		{
			histg[0][*p]++;
		}
		break;
	case ptBGR:
	case ptBGRA:
		ZeroMemory(histg, 3 * 256 * sizeof(unsigned long));
		for (; n > 0; n--)
		{
			if (byPP == 32) p++;
			hisA1[*(p++)]++;
			hisA2[*(p++)]++;
			hisA3[*(p++)]++;
		}
		break;
	default:	// INTG
		// Not tested yet [1/28/2015 FSM]
		ZeroMemory(histg, 256 * sizeof(int));
		for (; n > 0; n--, p++)
		{
			if (*p < 0)
			{
				histg_neg++;
			}
			else if (*p > 255)
			{
				histg_ovr++;
			}
			else
			{
				histg[0][*p]++;
			}
		}
		break;
	}
	return(true);
}

/*----------------------------------------------------------------------
  This function redistributes the image data so an equalized histogram
  is obtained. This results in an image with similar contrast over
  the full range of intensities. Note that the hist must be computed 1st.
  (Thanks to Frank Hoogterp and Steve Caito for the original FORTRAN code).
  ----------------------------------------------------------------------*/
void CImageL::Eqliz()
{
	if (!Hist()) return;	// Need histogram

	int a1 = 0;
	int a2 = 1;
	int a3 = 2;
	if (ptype == ptBGR)
	{
		a1 = 2;
		a3 = 0;
	}
	unsigned long *hisA1 = histg[a1];
	unsigned long *hisA2 = histg[a2];
	unsigned long *hisA3 = histg[a3];

	int bPP = GetBPP();

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *p = (BYTE *)GetBufferStart();	// Ptr to bitmap

	unsigned long n = GetImageSize();
	unsigned long i;
	unsigned ncol = 0;
	unsigned long sum = 0;
	double xnext, step;
	const unsigned Histg_steps = 256; //_countof(histg);

	step = (float)n / Histg_steps;
	xnext = step;

	switch (ptype)
	{
	case ptGREY:
		/* Remap histg[] */
		for (i = 0; i < Histg_steps; i++)
		{
			sum += histg[0][i];
			while (sum >= xnext)
			{
				ncol++;
				xnext += step;
			}
			histg[0][i] = ncol;
		}

		for (; n > 0; n--, p++)
		{
			*p = (BYTE)histg[0][*p];
			//if (*p > 255)
			//{
			//	*p = 255;
			//}
		}
		break;
	case ptBGR:
	case ptBGRA:
		/* Remap histg[] */
		for (i = 0; i < Histg_steps; i++)
		{
			// Avg. of RGB's
			sum += (hisA1[i] + hisA2[i] + hisA3[i]) / 3;
			while (sum >= xnext)
			{
				ncol++;
				xnext += step;
			}
			hisA1[i] = ncol;
			hisA2[i] = ncol;
			hisA3[i] = ncol;
		}

		for (; n > 0; n--)
		{
			if (bPP == 32) p++;
			*p = (BYTE)hisA1[*p];
			//if (*p > 255)
			//{
			//	*p = 255;
			//}
			p++;

			*p = (BYTE)hisA2[*p];
			//if (*p > 255)
			//{
			//	*p = 255;
			//}
			p++;

			*p = (BYTE)hisA3[*p];
			//if (*p > 255)
			//{
			//	*p = 255;
			//}
			p++;
		}
		break;
	default:	// INTG
	{
		short *q = (short *)p;
		// Not tested [1/28/2015 FSM]
		for (; n > 0; n--, q++)
		{
			*q = (short)histg[0][*q];
			if (*q > 255) *q = 255;
		}
	}
	break;
	}
}

CImageL &CImageL::operator=(CImageL &Image)
{
	if (this == &Image)
	{
		return *this;
	}
	if (IsNull())
	{
		Create(Image.GetWidth(), Image.GetHeight(), Image.GetBPP());
		ptype = Image.ptype;
	}
	else
	{
		if (!IsSameSize(Image))
		{
			Destroy();
			Create(Image.GetWidth(), Image.GetHeight(), Image.GetBPP());
			ptype = Image.ptype;
		}
	}
	if (IsNull())
	{
		return *this;
	}

	CSingleLock Lock(&CS1); Lock.Lock();
	memcpy(GetBufferStart(), Image.GetBufferStart(), Image.GetBufferSize());
	return *this;
}

// Only works with 24 bits image [3/14/2015 VIT]
BOOL CImageL::SubtractAdd(CImageL &Image1, BYTE val)
{
	if (IsNull() || Image1.IsNull()) return FALSE;
	if (!IsSameSize(Image1)) return FALSE;

	int wd = GetWidth();
	int ht = GetHeight();
#pragma omp parallel for
	for (int y = 0; y < ht; y++)
	{
		short v;
		BYTE *p = (BYTE *)GetPixelAddress(0, y);
		BYTE *q = (BYTE *)Image1.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			v = (*p - *q) + val;
			*p = v;
			if (v < 0) *p = 0;
			if (v > 255) * p = 255;
			p++; q++;
		}
	}
	return TRUE;
}

CImageL &CImageL::operator-(CImageL &Image)
{
	Rslt = *this;
	Rslt -= Image;
	return Rslt;
}

CImageL &CImageL::operator+(CImageL &Image)
{
	Rslt = *this;
	Rslt += Image;
	return Rslt;
}

CImageL &CImageL::operator-=(CImageL &Image)
{
	if (Image.IsNull() || this->IsNull()) return *this;
	if ((Image.GetWidth() != GetWidth()) || (Image.GetHeight() != GetHeight()) || (Image.GetBPP() != GetBPP())) return *this;

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pSrc = (BYTE *)Image.GetBufferStart();
	BYTE *pDes = (BYTE *)GetBufferStart();
	int iCount = (int)GetBufferSize();

	switch (ptype)
	{
	case ptGREY:
	case ptBGR:
		ASSERT(0);
		// _asm
		// {
		// 	// Assign pointers to register
		// 	mov			esi, [pSrc]; put src addr to esi reg
		// 	mov			edi, [pDes]; put dest addr to edi reg
		// 	mov			ecx, [iCount]; put count to ecx reg
		// 	shr			ecx, 3; divide count with 8 by shifting 3 bits to right
		//
		// 	codeloop :
		// 	movq		mm0, [esi]; mov 8 bytes of src data to mmx reg 0
		// 		movq		mm1, [edi]; mov 8 bytes of dest data to mmx reg 1
		// 		PSUBUSB		mm0, mm1; Subtract unsigned 8bit saturated to mmx reg 0
		// 		movq[edi], mm0; dump back the added 8 bytes of data to dest memory
		// 		add			esi, 8; add src pointer by 8 bytes
		// 		add			edi, 8; add dest pointer by 8 bytes
		//
		// 		dec			ecx; decrement count by 1
		// 		jnz			codeloop; jump to codeloop if not Zero
		// 		emms; Restore FPU state to normal
		// }
		break;
	default:
		// TODO: 16 bit image pixel [1/28/2015 FSM]
		break;
	}
	return *this;
}

CImageL &CImageL::operator+=(CImageL &Image)
{
	if (Image.IsNull() || this->IsNull()) return *this;
	if ((Image.GetWidth() != GetWidth()) || (Image.GetHeight() != GetHeight()) || Image.GetBPP() != GetBPP())
	{
		return *this;
	}

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pSrc = (BYTE *)Image.GetBufferStart();
	BYTE *pDes = (BYTE *)GetBufferStart();
	int iCount = (int)GetBufferSize();
	switch (ptype)
	{
	case ptGREY:
	case ptBGR:
		ASSERT(0);
		// _asm
		// {
		// 	// Assign pointers to register
		// 	mov			esi, [pSrc]; put src addr to esi reg
		// 	mov			edi, [pDes]; put dest addr to edi reg
		// 	mov			ecx, [iCount]; put count to ecx reg
		// 	shr			ecx, 3; divide count with 8 by shifting 3 bits to right
		//
		// 	codeloop :
		// 	movq		mm0, [esi]; mov 8 bytes of src data to mmx reg 0
		// 		movq		mm1, [edi]; mov 8 bytes of dest data to mmx reg 1
		// 		PADDUSB 	mm0, mm1; Subtract unsigned 8bit saturated to mmx reg 0
		// 		movq[edi], mm0; dump back the added 8 bytes of data to dest memory
		// 		add			esi, 8; add src pointer by 8 bytes
		// 		add			edi, 8; add dest pointer by 8 bytes
		//
		// 		dec			ecx; decrement count by 1
		// 		jnz			codeloop; jump to codeloop if not Zero
		// 		emms; Restore FPU state to normal
		// }
		break;
	default:
		// TODO: 16 bit image pixel [1/28/2015 FSM]
		break;
	}
	return *this;
}

BOOL CImageL::AbsSubtract(CImageL &Image, BYTE Thresh, BOOL bRetain)
{
	if (IsNull() || Image.IsNull()) return FALSE;
	if ((Image.GetWidth() != GetWidth()) || (Image.GetHeight() != GetHeight()) || (Image.GetBPP() != GetBPP()))
	{
		return FALSE;
	}

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pSrc = (BYTE *)Image.GetBufferStart();
	BYTE *pDes = (BYTE *)GetBufferStart();

	int i, ImgSize;
	short v1, v2, v3, bPP = GetBPP();
	switch (ptype)
	{
	case ptGREY:
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++, pDes++, pSrc++)
		{
			*pDes = abs(*pDes - *pSrc);
		}
		break;
	case ptBGR:
	{
		ImgSize = GetImageSize();
		CHSVRGB Des, Src;
		for (i = 0; i < ImgSize; i++)
		{
			if (bPP == 32) { pSrc++; pDes++; }
			//////////////////////////////////////////////////////////////////////////
			// Detect intensity change [1/29/2015 FSM]
			//v1 = (abs(*pDes - *pSrc) + abs(*(pDes+1) - *(pSrc+1)) + abs(*(pDes+2)-*(pSrc+2))) / 3;
			//if (v1 < Thresh)
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// Detect color change [1/29/2015 FSM]
			v1 = abs(*pDes - *pSrc);
			v2 = abs(*(pDes + 1) - *(pSrc + 1));
			v3 = abs(*(pDes + 2) - *(pSrc + 2));
			if ((v1 > Thresh) || (v2 > Thresh) || (v3 > Thresh))
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//Src.SetBGR3(pSrc); Des.SetBGR3(pDes);
				//double h1,h2;
				//h1 = Src.GetH();
				//h2 = Des.GetH();
				//v1 = short(fabs(h1 - h2) * 0.7111);  // for hue [1/29/2015 FSM]
				//v1 = short(fabs(h1 - h2) * 255);  // for S & V [1/29/2015 FSM]
				//if (v1 < Thresh)
				//////////////////////////////////////////////////////////////////////////
			{
				*(pDes++) = 255;
				*(pDes++) = 255;
				*(pDes++) = 255;
			}
			else
			{
				// retain image if no unnoticeable change occurs [1/29/2015 FSM]
				if (bRetain)
				{
					pDes += 3;
				}
				else
				{
					*(pDes++) = 0;
					*(pDes++) = 0;
					*(pDes++) = 0;
				}
			}
			pSrc += 3;
		}
	}
	break;
	default:
		// To be implemented [1/28/2015 FSM]
		break;
	}
	return TRUE;
}

// resulting image is mono chrome [2/12/2015 128]
BOOL CImageL::AbsSubtract(CImageL &ImgRef, CImageL &ImgCur, BYTE Thresh, BOOL bRetain)
{
	if (ImgRef.IsNull() || (ImgCur.IsNull())) return FALSE;
	if ((ImgRef.GetWidth() != ImgCur.GetWidth()) || (ImgRef.GetHeight() != ImgCur.GetHeight()) || (ImgRef.GetBPP() != ImgCur.GetBPP())) return FALSE;
	if (!IsNull())
	{
		if ((GetWidth() != ImgCur.GetWidth()) || (GetHeight() != ImgCur.GetHeight()) || (GetBPP() != ImgCur.GetBPP()))
		{
			Destroy();
			this->Create(ImgRef.GetWidth(), ImgRef.GetHeight(), ImgRef.GetBPP());
			//this->GetDim();
		}
	}
	else
	{
		this->Create(ImgRef.GetWidth(), ImgRef.GetHeight(), ImgRef.GetBPP());
		//this->GetDim();
	}

	ptype = ImgRef.ptype;

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pA = (BYTE *)ImgRef.GetBufferStart();
	BYTE *pB = (BYTE *)ImgCur.GetBufferStart();
	BYTE *pC = (BYTE *)this->GetBufferStart();

	int i, ImgSize;
	short v1 = 0, v2 = 0, v3 = 0, bPP = GetBPP();
	switch (ptype)
	{
	case ptGREY:
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++, pA++, pB++, pC++)
		{
			*pC = abs(*pA - *pB);
		}
		break;
	case ptBGR:
	{
		CHSVRGB Des, Src;
		ImgSize = GetImageSize();
		//CTimeCounter Tc;
		//Tc.Start( 0 );
		for (i = 0; i < ImgSize; i++)
		{
			if (bPP == 32) { pA++; pB++; pC++; }
			//////////////////////////////////////////////////////////////////////////
			// Detect intensity change [1/29/2015 FSM]
			//v1 = (abs(*pDes - *pSrc) + abs(*(pDes+1) - *(pSrc+1)) + abs(*(pDes+2)-*(pSrc+2))) / 3;
			//if (v1 < Thresh)
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// Detect color change [1/29/2015 FSM]
#if 0
			v1 = abs(*(pA++) - *(pB++));
			v2 = abs(*(pA++) - *(pB++));
			v3 = abs(*(pA++) - *(pB++));
#endif
#if 1
			ASSERT(0);
			// _asm mov		eax, dword ptr[pA];
			// _asm movzx		ecx, byte ptr[eax];
			// _asm mov		edx, dword ptr[pB];
			// _asm movzx		eax, byte ptr[edx];
			// _asm sub		ecx, eax;
			// _asm mov		dword ptr[v1], ecx;	// v1 = *pA - *pB
			// _asm inc		ecx;						// pA++;
			// _asm inc		eax;						// pB++;
			// _asm sub		ecx, eax;
			// _asm mov		dword ptr[v2], ecx;	// v2 = *pA - *pB
			// _asm inc		ecx;						// pA++;
			// _asm inc		eax;						// pB++;
			// _asm sub		ecx, eax;
			// _asm mov		dword ptr[v3], ecx;	// v3 = *pA - *pB
			// _asm mov		ecx, dword ptr[pB];
			// _asm add		ecx, 3;					// pB += 3
			// _asm mov		dword ptr[pB], ecx;
			// _asm mov		edx, dword ptr[pA];
			// _asm add		edx, 3;					// pA += 3
			// _asm mov		dword ptr[pA], edx;
#endif

			if ((abs(v1) > Thresh) && (abs(v2) > Thresh) && (abs(v3) > Thresh))
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				//Src.SetBGR3(pSrc); Des.SetBGR3(pDes);
				//double h1,h2;
				//h1 = Src.GetH();
				//h2 = Des.GetH();
				//v1 = short(fabs(h1 - h2) * 0.7111);  // for hue [1/29/2015 FSM]
				//v1 = short(fabs(h1 - h2) * 255);  // for S & V [1/29/2015 FSM]
				//if (v1 < Thresh)
				//////////////////////////////////////////////////////////////////////////
			{
				if (bRetain)
				{
					pB -= 3;
					memcpy(pC, pB, 3);
					pC += 3; pB += 3;
				}
				else
				{
#if 0
					memset(pC, 255, 3);
					pC += 3;
#endif
#if 1
					// memset(pC,255,3) [2/12/2015 128]
					ASSERT(0);
					// _asm mov	eax, dword ptr[pC];
					// _asm mov	byte ptr[eax], 255;
					// _asm inc	eax;
					// _asm mov	byte ptr[eax], 255;
					// _asm inc	eax;
					// _asm mov	byte ptr[eax], 255;
					// // pC += 3; [2/12/2015 128]
					// _asm mov	ecx, dword ptr[pC];
					// _asm add	ecx, 3;
					// _asm mov	dword ptr[pC], ecx;
#endif
				}
			}
			else
			{
				// retain image if no unnoticeable change occurs [1/29/2015 FSM]
				//if (bRetain)
				//{
				//	pA-=3;
				//	*(pC++) = *(pA++);
				//	*(pC++) = *(pA++);
				//	*(pC++) = *(pA++);
				//}
				//else
				//{
#if 0
				memset(pC, 0, 3);
				pC += 3;
#endif
#if 1
				// memset (pC,0,3) [2/12/2015 128]
				ASSERT(0);
				// _asm mov	eax, dword ptr[pC];
				// _asm mov	byte ptr[eax], 0;
				// _asm inc	eax;
				// _asm mov	byte ptr[eax], 0;
				// _asm inc	eax;
				// _asm mov	byte ptr[eax], 0;
				// // pC += 3; [2/12/2015 128]
				// _asm mov	ecx, dword ptr[pC];
				// _asm add	ecx, 3;
				// _asm mov	dword ptr[pC], ecx;
#endif
			}
		}
		//Tc.GetExecutionTime( 0, 1000 );
	}
	break;
	default:
		// To be implemented [1/28/2015 FSM]
		break;
	}
	return TRUE;
}

// Byte version [2/3/2015 Yuen]
BOOL CImageL::PixelDivision(CImageL &Image)
{
	if (IsNull() || Image.IsNull()) return FALSE;
	if ((Image.GetWidth() != GetWidth()) || (Image.GetHeight() != GetHeight()) || (Image.GetBPP() != GetBPP()))
	{
		return FALSE;
	}

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pSrc = (BYTE *)Image.GetBufferStart();
	BYTE *pDes = (BYTE *)GetBufferStart();

	int i, ImgSize;
	short bPP = GetBPP();
	switch (ptype)
	{
	case ptGREY:
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++, pDes++, pSrc++)
		{
			if (*pSrc > 0) *(pDes) /= *(pSrc); else *pDes = 0; pSrc++; pDes++;
		}
		break;
	case ptBGR:
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++)
		{
			if (bPP == 32) { pSrc++; pDes++; }
			// TODO: floating point image buffer is needed to improve quality [1/28/2015 FSM]
			if (*pSrc > 0) *(pDes) /= *(pSrc); else *pDes = 0; pSrc++; pDes++;
			if (*pSrc > 0) *(pDes) /= *(pSrc); else *pDes = 0; pSrc++; pDes++;
			if (*pSrc > 0) *(pDes) /= *(pSrc); else *pDes = 0; pSrc++; pDes++;
		}
		break;
	default:
		// To be implemented [1/28/2015 FSM]
		break;
	}
	return TRUE;
}

// float version [2/3/2015 Yuen]
BOOL CImageL::fPixelDivision(CImageL &Image)
{
	if (IsNull() || Image.IsNull()) return FALSE;
	if ((Image.GetWidth() != GetWidth()) || (Image.GetHeight() != GetHeight()) || (Image.GetBPP() != GetBPP()))
	{
		return FALSE;
	}

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pSrc = (BYTE *)Image.GetBufferStart();
	BYTE *pDes = (BYTE *)GetBufferStart();

	int i, ImgSize;
	short bPP = GetBPP();
	switch (ptype)
	{
	case ptGREY:
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++, pDes++, pSrc++)
		{
			if (*pSrc > 0) *(pDes) /= *(pSrc); else *pDes = 0; pSrc++; pDes++;
		}
		break;
	case ptBGR:
	{
		ImgSize = GetImageSize();
		float *pBuf = new float[GetBufferSize()];
		if (pBuf)
		{
			float *p = pBuf;
			float max = -1, min = FLT_MAX;
			for (i = 0; i < ImgSize; i++)
			{
				if (bPP == 32) { pSrc++; pDes++; }
				if (*pSrc > 0) *(p) = (*pDes) / float(*pSrc); else *(p) = 1; pSrc++; pDes++;
				if (*p > max) max = *p; if (*p < min) min = *p; p++;
				if (*pSrc > 0) *(p) = (*pDes) / float(*pSrc); else *(p) = 1; pSrc++; pDes++;
				if (*p > max) max = *p; if (*p < min) min = *p; p++;
				if (*pSrc > 0) *(p) = (*pDes) / float(*pSrc); else *(p) = 1; pSrc++; pDes++;
				if (*p > max) max = *p; if (*p < min) min = *p; p++;
			}
			float sf = 255 / (max - min);
			p = pBuf;

			CSingleLock Lock(&CS1); Lock.Lock();

			pDes = GetBufferStart();
			for (i = 0; i < ImgSize; i++)
			{
				if (bPP == 32) { pDes++; }
				*(pDes++) = BYTE((*p - min) *sf); p++;
				*(pDes++) = BYTE((*p - min) *sf); p++;
				*(pDes++) = BYTE((*p - min) *sf); p++;
			}
			delete[] pBuf;
		}
	}
	break;
	default:
		// To be implemented [1/28/2015 FSM]
		break;
	}
	return TRUE;
}

void CImageL::Convl(float k1, float k2, float k3,
	float k4, float k5, float k6,
	float k7, float k8, float k9)
{
	if (IsNull()) return;

	BYTE *p;						// Image ptr
	int nxx;
	unsigned long i, j, nx, ny;
	BYTE *m1, *m2, *m3;				// Pointers to buffers to free()
	BYTE *old_r1, *r1, *r2, *r3;	// Cycling pointers to rows
	float s, fsum;
	short byPP;

	nx = GetWidth();
	ny = GetHeight();
	byPP = GetBPP() / 8;
	nxx = (nx)* byPP;

	CSingleLock Lock(&CS1); Lock.Lock();

	p = (BYTE *)GetBufferStart();	// Ptr to bitmap
	// Allocate row buffers
	m1 = new BYTE[(nx + 2) * byPP];
	m2 = new BYTE[(nx + 2) * byPP];
	m3 = new BYTE[(nx + 2) * byPP];
	if (!m1 || !m2 || !m3)
	{
		if (m1) delete[] m1;
		if (m2) delete[] m2;
		if (m3) delete[] m3;
		return;
	}
	r1 = m1;
	r2 = m2;
	r3 = m3;

	int width = nx * byPP;
	// Initialize rows
	switch (ptype)
	{
	case ptBGR:
		p = (BYTE *)GetPixelAddress(0, 0);
		memcpy_s(&r1[3], width, p, width);
		p = (BYTE *)GetPixelAddress(0, 1);
		memcpy_s(&r2[3], width, p, width);
		r1[0] = r1[3];                      // Doubly weight edges
		r1[1] = r1[4];                      // Doubly weight edges
		r1[2] = r1[5];                      // Doubly weight edges
		r1[width + 0] = r1[width - 3];
		r1[width + 1] = r1[width - 2];
		r1[width + 2] = r1[width - 1];
		r2[0] = r2[3];                      // Doubly weight edges
		r2[1] = r2[4];                      // Doubly weight edges
		r2[2] = r2[5];                      // Doubly weight edges
		r2[width + 0] = r2[width - 3];
		r2[width + 1] = r2[width - 2];
		r2[width + 2] = r2[width - 1];
		break;
	default:
		p = (BYTE *)GetPixelAddress(0, 0);
		memcpy_s(&r1[1], width, p, width);
		p = (BYTE *)GetPixelAddress(0, 1);
		memcpy_s(&r2[1], width, p, width);
		r1[0] = r1[1];                      // Doubly weight edges
		r1[width] = r1[width - 1];
		r2[0] = r2[1];                      // Doubly weight edges
		r2[width] = r2[width - 1];
		break;
	}

	// Calc. sum of kernel
	fsum = k1 + k2 + k3 + k4 + k5 + k6 + k7 + k8 + k9;
	if (fsum == 0)
	{
		fsum = 1;			// Avoid div. by 0
	}
	else
	{
		fsum = 1 / fsum;		// Invert so can mult.
	}
	switch (ptype)
	{
	case ptGREY:
		for (j = 1; j < ny - 1; j++)
		{
			if (j == ny)				/* Last row */
			{
				r3 = r2;				/* Last row doubly weighted */
			}
			else /* Read next row (into the 3rd row) */
			{
				p = (BYTE *)GetPixelAddress(0, j + 1);
				memcpy_s(&r3[1], width, p, width);
				r3[0] = r3[1];			/* Doubly weight edges */
				r3[width] = r3[width - 1];
			}

			p = (BYTE *)GetPixelAddress(0, j);
			//#pragma omp parallel for private(s,r11,r22,r33,p0)
			for (int i = 0; i < int(nx); i++)
			{
				BYTE *p0 = p + i;
				BYTE *r11 = r1 + i;
				BYTE *r22 = r2 + i;
				BYTE *r33 = r3 + i;
				s = k1 * (float)r11[0] + k2 * (float)r11[1] + k3 * (float)r11[2]
					+ k4 * (float)r22[0] + k5 * (float)r22[1] + k6 * (float)r22[2]
					+ k7 * (float)r33[0] + k8 * (float)r33[1] + k9 * (float)r33[2];
				*p0 = (BYTE)THRESH(NINT(s * fsum));
			}

			/* Cycle row pointers */
			old_r1 = r1;	// To save addr. for r3
			r1 = r2;
			r2 = r3;
			r3 = old_r1;
		}
		break;
	case ptBGR:
		for (int j = 1; j < int(ny - 1); j++)
		{
			if (j == ny) r3 = r2;
			else  /* Read next row (into the 3rd row) */
			{
				p = (BYTE *)GetPixelAddress(0, j + 1);
				memcpy_s(&r3[3], width, p, width);
				r3[0] = r3[3];                      // Doubly weight edges
				r3[1] = r3[4];                      // Doubly weight edges
				r3[2] = r3[5];                      // Doubly weight edges
				r3[width + 0] = r3[width - 3];
				r3[width + 1] = r3[width - 2];
				r3[width + 2] = r3[width - 1];
			}

			BYTE *pp = (BYTE *)GetPixelAddress(0, j);
#pragma omp parallel for
			for (int i = 0; i < int(nx); i++)
			{
				short nByte = i * byPP;
				BYTE *p0 = pp + nByte;
				BYTE *r11 = r1 + nByte;
				BYTE *r22 = r2 + nByte;
				BYTE *r33 = r3 + nByte;
				if (byPP > 3) { r11++; r22++; r33++; p0++; }
				// Red component
				float s;
				s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
					+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
					+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];

				*p0 = (BYTE)THRESH(NINT(s * fsum));
				r11++; r22++; r33++; p0++;

				// Green component
				s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
					+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
					+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];

				*p0 = (BYTE)THRESH(NINT(s * fsum));
				r11++; r22++; r33++; p0++;

				// Blue component
				s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
					+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
					+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];

				*p = (BYTE)THRESH(NINT(s * fsum));
			}

			/* Cycle row pointers */
			old_r1 = r1;
			r1 = r2;
			r2 = r3;
			r3 = old_r1;
		}
		break;
	default:	// INTG
		// Not tested [1/29/2015 Yuen]
		for (j = 1; j <= ny; j++, p += nx)
		{
			if (j == ny) 				/* Last row */
			{
				r3 = r2;				/* Last row doubly weighted */
			}
			else  /* Read next row (into the 3rd row) */
			{
				memcpy_s(&r3[1], nx * byPP, p + nx, nx * byPP);
				r3[0] = r3[1];			/* Doubly weight edges */
				r3[nx + 1] = r3[nx];
			}

			for (i = 0; i < nx; i++)
			{
				s = k1 * r1[i] + k2 * r1[i + 1] + k3 * r1[i + 2]
					+ k4 * r2[i] + k5 * r2[i + 1] + k6 * r2[i + 2]
					+ k7 * r3[i] + k8 * r3[i + 1] + k9 * r3[i + 2];
				p[i] = NINT(s * fsum);
			}

			/* Cycle row pointers */
			old_r1 = r1;
			r1 = r2;
			r2 = r3;
			r3 = old_r1;
		}
		break;
	}

	delete[] m1;
	delete[] m2;
	delete[] m3;
}

void IMGL::CImageL::Opening(short n, short nK)
{
	int i;
	for (i = 0; i < n; i++) Erosions(nK);
	for (i = 0; i < n; i++) Dilations(nK);
}

void IMGL::CImageL::Closing(short n, short nK)
{
	int i;
	for (i = 0; i < n; i++) Dilations(nK);
	for (i = 0; i < n; i++) Erosions(nK);
}

// Kernel is in RGB format [1/30/2015 FSM]
void CImageL::DilateErode(BOOL bDilate)
{
	if (IsNull())
	{
		// Cannot perform [1/30/2015 FSM]
		ASSERT(0);
		return;
	}

	short s;
	short k1 = 0; short k2 = 1; short k3 = 0;
	short k4 = 1; short k5 = 1; short k6 = 1;
	short k7 = 0; short k8 = 1; short k9 = 0;
	short sum = k1 + k2 + k3 + k4 + k5 + k6 + k7 + k8 + k9;
	if (bDilate)
	{
		sum = 1;
	}
	//else sum = 6;

	int nxx;
	unsigned long i, j, nx, ny;
	BYTE *m1, *m2, *m3;				// Pointers to buffers to free()
	BYTE *old_r1, *r1, *r2, *r3;	// Cycling pointers to rows
	BYTE *r11a, *r11b, *r11c;
	BYTE *r22a, *r22b, *r22c;
	BYTE *r33a, *r33b, *r33c;			// Working pointers to buffers

	short byPP;

	nx = GetWidth();
	ny = GetHeight();
	byPP = GetBYPP();

	nxx = (nx)* byPP;

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *p = (BYTE *)GetBufferStart();	// Ptr to bitmap
	// Allocate row buffers
	m1 = new BYTE[(nx + 2) * byPP];
	m2 = new BYTE[(nx + 2) * byPP];
	m3 = new BYTE[(nx + 2) * byPP];
	if (!m1 || !m2 || !m3)
	{
		if (m1) delete[] m1;
		if (m2) delete[] m2;
		if (m3) delete[] m3;
		return;
	}
	r1 = m1;
	r2 = m2;
	r3 = m3;

	// Initialize rows
	switch (ptype)
	{
	case ptGREY:
		memcpy_s(&r1[1], nx * byPP, p, nx * byPP);
		r1[0] = r1[3];                      // Doubly weight edges
		r1[nxx + 1] = r1[nx];
		break;
	case ptBGR:
		memcpy_s(&r1[3], nx * byPP, p, nx * byPP);
		r1[0] = r1[3];                      // Doubly weight edges
		r1[1] = r1[4];                      // Doubly weight edges
		r1[2] = r1[5];                      // Doubly weight edges
		r1[nxx + 3] = r1[nx];
		r1[nxx + 4] = r1[nx + 1];
		r1[nxx + 5] = r1[nx + 2];
		break;
	default:
		memcpy_s(&r1[1], nx * byPP, p, nx * byPP);
		break;
	}

	// Start r2 = r1 (doubly weight 1st row)
	memcpy_s(r2, (nx + 2) * byPP, r1, (nx + 2) * byPP);

	CTimeCounter TC;
	switch (ptype)
	{
	case ptGREY:
		for (j = 1; j <= ny; j++)
		{
			if (j == ny)				/* Last row */
			{
				r3 = r2;				/* Last row doubly weighted */
			}
			else /* Read next row (into the 3rd row) */
			{
				memcpy_s(&r3[1], nx * byPP, p, nx * byPP);
				r3[0] = r3[1];			/* Doubly weight edges */
				r3[nx + 1] = r3[nx];
			}

			r11a = r1; r11b = r1 + 1; r11c = r1 + 2;
			r22a = r2; r22b = r2 + 1; r22c = r2 + 2;
			r33a = r3; r33b = r3 + 1; r33c = r3 + 2;
			for (i = 0; i < nx; i++)
			{
				s = (((k1 * (*r11a)) > 0) ? 1 : 0)
					+ (((k2 * (*r11b)) > 0) ? 1 : 0)
					+ (((k3 * (*r11c)) > 0) ? 1 : 0)
					+ (((k4 * (*r22a)) > 0) ? 1 : 0)
					+ (((k5 * (*r22b)) > 0) ? 1 : 0)
					+ (((k6 * (*r22c)) > 0) ? 1 : 0)
					+ (((k7 * (*r33a)) > 0) ? 1 : 0)
					+ (((k8 * (*r33b)) > 0) ? 1 : 0)
					+ (((k9 * (*r33c)) > 0) ? 1 : 0);
				r11a++; r11b++; r11c++;
				r22a++; r22b++; r22c++;
				r33a++; r33b++; r33c++;
				if (s >= sum) *p = 255;
				p++;
			}

			/* Cycle row pointers */
			old_r1 = r1;	// To save addr. for r3
			r1 = r2;
			r2 = r3;
			r3 = old_r1;
		}
		break;
	case ptBGR:
		for (j = 1; j <= ny; j++)
		{
			if (j == ny) { r3 = r2; } /* Last row */
			else /* Read next row (into the 3rd row) */
			{
				memcpy_s(&r3[3], nx*byPP, p + nx * byPP, nx*byPP);
				r3[0] = r3[3];                      // Doubly weight edges
				r3[1] = r3[4];                      // Doubly weight edges
				r3[2] = r3[5];                      // Doubly weight edges
				r3[nxx + 3] = r3[nx];
				r3[nxx + 4] = r3[nx + 1];
				r3[nxx + 5] = r3[nx + 2];
			}
			r11a = r1; r11b = r1 + 1; r11c = r1 + 2;
			r22a = r2; r22b = r2 + 1; r22c = r2 + 2;
			r33a = r3; r33b = r3 + 1; r33c = r3 + 2;
			for (i = 0; i < nx; i++)
			{
				if (byPP == 4)
				{
					r11a++; r11b++; r11c++;
					r22a++; r22b++; r22c++;
					r33a++; r33b++; r33c++;
					p++;
				}
				// Red component
				s = (((k1 * (*r11a)) > 0) ? 1 : 0)
					+ (((k2 * (*r11b)) > 0) ? 1 : 0)
					+ (((k3 * (*r11c)) > 0) ? 1 : 0)
					+ (((k4 * (*r22a)) > 0) ? 1 : 0)
					+ (((k5 * (*r22b)) > 0) ? 1 : 0)
					+ (((k6 * (*r22c)) > 0) ? 1 : 0)
					+ (((k7 * (*r33a)) > 0) ? 1 : 0)
					+ (((k8 * (*r33b)) > 0) ? 1 : 0)
					+ (((k9 * (*r33c)) > 0) ? 1 : 0);
				*p = 0;
				if (s >= sum) *p = 255;
				p++;
				r11a++; r11b++; r11c++;
				r22a++; r22b++; r22c++;
				r33a++; r33b++; r33c++;

				// Green component
				s = (((k1 * (*r11a)) > 0) ? 1 : 0)
					+ (((k2 * (*r11b)) > 0) ? 1 : 0)
					+ (((k3 * (*r11c)) > 0) ? 1 : 0)
					+ (((k4 * (*r22a)) > 0) ? 1 : 0)
					+ (((k5 * (*r22b)) > 0) ? 1 : 0)
					+ (((k6 * (*r22c)) > 0) ? 1 : 0)
					+ (((k7 * (*r33a)) > 0) ? 1 : 0)
					+ (((k8 * (*r33b)) > 0) ? 1 : 0)
					+ (((k9 * (*r33c)) > 0) ? 1 : 0);

				*p = 0;
				if (s >= sum) *p = 255;
				p++;
				r11a++; r11b++; r11c++;
				r22a++; r22b++; r22c++;
				r33a++; r33b++; r33c++;

				// Blue component
				s = (((k1 * (*r11a)) > 0) ? 1 : 0)
					+ (((k2 * (*r11b)) > 0) ? 1 : 0)
					+ (((k3 * (*r11c)) > 0) ? 1 : 0)
					+ (((k4 * (*r22a)) > 0) ? 1 : 0)
					+ (((k5 * (*r22b)) > 0) ? 1 : 0)
					+ (((k6 * (*r22c)) > 0) ? 1 : 0)
					+ (((k7 * (*r33a)) > 0) ? 1 : 0)
					+ (((k8 * (*r33b)) > 0) ? 1 : 0)
					+ (((k9 * (*r33c)) > 0) ? 1 : 0);

				*p = 0;
				if (s >= sum) *p = 255;
				p++;
				r11a++; r11b++; r11c++;
				r22a++; r22b++; r22c++;
				r33a++; r33b++; r33c++;
			}
			/* Cycle row pointers */
			old_r1 = r1;
			r1 = r2;
			r2 = r3;
			r3 = old_r1;
		}
		break;
	default:	// INTG
		// Not tested [1/29/2015 Yuen]
		break;
	}
	delete[] m1;
	delete[] m2;
	delete[] m3;
}

BOOL CImageL::Accumulate(CImageL &Image)
{
	if (IsNull())
	{
		Create(Image.GetWidth(), Image.GetHeight(), Image.GetBPP());
		if (IsNull())
		{
			return FALSE;
		}
		//this->GetDim();
		ptype = Image.ptype;
	}
	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pDes = (BYTE *)GetBufferStart();
	BYTE *pSrc = (BYTE *)Image.GetBufferStart();

	int i, ImgSize;
	short bPP = GetBPP();
	switch (ptype)
	{
	case ptGREY:
		// No relevant [1/30/2015 FSM]
		return TRUE;
		break;
	case ptBGR:
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++)
		{
			if (bPP == 32) { pDes++; }
			// TODO: floating point image buffer is needed to improve quality [1/28/2015 FSM]
			*(pDes++) = BYTE((*pDes + *(pSrc++)) / 2.0);
			*(pDes++) = BYTE((*pDes + *(pSrc++)) / 2.0);
			*(pDes++) = BYTE((*pDes + *(pSrc++)) / 2.0);
		}
		break;
	default:
		// To be implemented [1/28/2015 FSM]
		break;
	}
	return TRUE;
}

void CImageL::Sobel()
{
	ConvlVSobel();
	ConvlHSobel();
}

//////////////////////////////////////////////////////////////////////////
// Morphological operation [7/5/2016 Yuen]
// Will convert to either 8, 24 or 32 bits Grey image
// Input image must be either 8, 24 or 32 bits Grey image
void IMGL::CImageL::Erosions(short nK)
{
	switch (GetBYPP())
	{
	case 1:
		Erosion<sBINR>(nK);
		break;
	case 3:
		Erosion<sBGR>(nK);
		break;
	case 4:
		Erosion<sBGRA>(nK);
		break;
	default:
		ASSERT(0);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
// Morphological operation [7/5/2016 Yuen]
// Will convert to either 8, 24 or 32 bits grey image
// Input image must be either 8, 24 or 32 bits grey image
void IMGL::CImageL::Dilations(short nK)
{
	switch (GetBYPP())
	{
	case 1:
		Dilation<sBINR>(nK);
		break;
	case 3:
		Dilation<sBGR>(nK);
		break;
	case 4:
		Dilation<sBGRA>(nK);
		break;
	default:
		ASSERT(0);
		break;
	}
}

// this = Image1 - Image2 [2/13/2015 128]
BOOL CImageL::Subtract(CImageL &Image1, CImageL &Image2, short Thres, BOOL bRetain)
{
	if (Image1.IsNull() || Image2.IsNull()) return FALSE;
	if ((Image1.GetWidth() != Image2.GetWidth()) || (Image1.GetHeight() != Image2.GetHeight()) || (Image1.GetBPP() != Image2.GetBPP()))
	{
		return FALSE;
	}
	if (!IsNull()) Destroy();
	if (!Create(Image1.GetWidth(), Image1.GetHeight(), Image1.GetBPP())) return FALSE;
	//this->GetDim();
	ptype = Image1.ptype;

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pRef = (BYTE *)Image1.GetBufferStart();
	BYTE *pCur = (BYTE *)Image2.GetBufferStart();

	int i, ImgSize;
	int v, max = -999, min = 999;
	short bPP = GetBPP();
	switch (ptype)
	{
	case ptGREY:
	{
		int *pBuf = new int[GetBufferSize()];
		if (pBuf)
		{
			int *p = pBuf;
			ImgSize = GetImageSize();
			for (i = 0; i < ImgSize; i++)
			{
				v = (*(pCur)-*(pRef)+255); pCur++; pRef++;
				if (v < min) min = v; if (v > max) max = v;
				*(p++) = v;
			}
			float sf = 255 / float(max - min);
			p = pBuf;

			BYTE *pMask = (BYTE *)this->GetBufferStart();
			for (i = 0; i < ImgSize; i++)
			{
				*(pMask++) = BYTE(*p * sf);
			}
			delete[]pBuf;
		}
	}
	break;
	case ptBGR:
	{
		int *pBuf = new int[GetBufferSize()];
		if (pBuf)
		{
			int *p = pBuf;
			int max = -999, min = 999;
			ImgSize = GetImageSize();
			for (i = 0; i < ImgSize; i++)
			{
				if (bPP == 32) { pCur++; pRef++; }
				v = int(*(pCur)-*(pRef)); pCur++; pRef++;
				if (v < min) min = v; if (v > max) max = v;
				*(p++) = v;
				v = int(*(pCur)-*(pRef)); pCur++; pRef++;
				if (v < min) min = v; if (v > max) max = v;
				*(p++) = v;
				v = int(*(pCur)-*(pRef)); pCur++; pRef++;
				if (v < min) min = v; if (v > max) max = v;
				*(p++) = v;
			}
			float sf = 255 / float(max - min);
			p = pBuf;

			pRef = (BYTE *)Image1.GetBufferStart();
			pCur = (BYTE *)Image2.GetBufferStart();
			BYTE *pMask = (BYTE *)this->GetBufferStart();
			for (i = 0; i < ImgSize; i++)
			{
				if (bPP == 32) { pMask++; pCur++; }
				if (int((*p - min) * sf) < Thres)
				{
					if (bRetain)*pMask = *pCur;
					else *pMask = 0;
				}
				else *pMask = 255;
				p++; pMask++; pCur++;
				if (int((*p - min) * sf) < Thres)
				{
					if (bRetain) *pMask = *pCur;
					else *pMask = 0;
				}
				else *pMask = 255;
				p++; pMask++; pCur++;
				if (int((*p - min) * sf) < Thres)
				{
					if (bRetain) *pMask = *pCur;
					else *pMask = 0;
				}
				else *pMask = 255;
				p++; pMask++; pCur++;
			}
			delete[] pBuf;
		}
	}
	break;
	default:
		// To be implemented [1/28/2015 FSM]
		break;
	}
	return TRUE;
}

BOOL CImageL::Overlay(CImageL &Image, CImageL &Mask)
{
	if (IsNull() || Image.IsNull() || Mask.IsNull()) return FALSE;
	if ((Image.GetWidth() != Mask.GetWidth()) || (Image.GetHeight() != Mask.GetHeight()) || (Image.GetBPP() != Mask.GetBPP()))
	{
		return FALSE;
	}
	if ((Image.GetWidth() != this->GetWidth()) || (Image.GetHeight() != this->GetHeight()) || (Image.GetBPP() != this->GetBPP()))
	{
		return FALSE;
	}

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pSrc = (BYTE *)Image.GetBufferStart();
	BYTE *pMsk = (BYTE *)Mask.GetBufferStart();
	BYTE *pDes = (BYTE *)this->GetBufferStart();

	int i, ImgSize;
	ImgSize = GetBufferSize();
	for (i = 0; i < ImgSize; i++)
	{
		if (*(pMsk) > 0) *(pDes) = *(pSrc);
		pSrc++; pDes++; pMsk++;
	}
	return TRUE;
}

// Overlay Src pixel to this if this pixel is 0 [2/13/2015 128]
BOOL CImageL::Overlay(CImageL &Src)
{
	if (Src.IsNull() || IsNull()) return FALSE;
	if ((GetWidth() != Src.GetWidth()) || (GetHeight() != Src.GetHeight()) || (GetBPP() != Src.GetBPP())) return FALSE;

	CSingleLock Lock(&CS1); Lock.Lock();

	BYTE *pSrc = (BYTE *)Src.GetBufferStart();
	BYTE *pMsk = (BYTE *)GetBufferStart();
	short bPP = GetBPP();
	int i, ImgSize;
	CTimeCounter TC;
	switch (bPP)
	{
	case 8:
		break;
	case 32:
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++)
		{
			pSrc++; pMsk++;
			if (*(pMsk) == 0) *(pMsk) = *pSrc; else *pMsk = 0;
			pSrc++; pMsk++;
			if (*(pMsk) == 0) *(pMsk) = *pSrc; else *pMsk = 0;
			pSrc++; pMsk++;
			if (*(pMsk) == 0) *(pMsk) = *pSrc; else *pMsk = 255;
			pSrc++; pMsk++;
		}
		break;
	case 24:
#if 0
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++)
		{
			TC.Start(0);
			if (*(pMsk) == 0) *(pMsk) = *pSrc; else *pMsk = 0;
			*pSrc++; *pMsk++;
			if (*(pMsk) == 0) *(pMsk) = *pSrc; else *pMsk = 0;
			*pSrc++; *pMsk++;
			if (*(pMsk) == 0) *(pMsk) = *pSrc; else *pMsk = 255;
			*pSrc++; *pMsk++;
			TC.GetExecutionTime(0);
		}
#endif

#if 1
		//TC.Start( 0 );
		ImgSize = GetImageSize();
		for (i = 0; i < ImgSize; i++)
		{
			ASSERT(0);
			// 	_asm mov		eax, dword ptr[pMsk];		// eax is pMsk
			// 	_asm mov		edx, dword ptr[pSrc];		// edx is pDes
			// 	_asm movzx		ecx, byte ptr[eax];			// ecx = *pMsk
			// 	_asm mov		byte ptr[eax], 0;
			// 	_asm test     ecx, ecx;						// if (*pMsk == 0)
			// 	_asm jnz      Next1
			// 	_asm mov		cl, byte ptr[edx];			// else
			// 	_asm mov		byte ptr[eax], cl;			// *pMsk = *pSrc
			// Next1:
			// 	_asm inc		eax; pMsk++;
			// 	_asm inc		edx; pSrc++;
			//
			// 	_asm movzx		ecx, byte ptr[eax];
			// 	_asm mov		byte ptr[eax], 0;
			// 	_asm test     ecx, ecx;
			// 	_asm jnz      Next2
			// 	_asm mov		cl, byte ptr[edx];
			// 	_asm mov		byte ptr[eax], cl;
			// Next2:
			// 	_asm inc		eax;
			// 	_asm inc		edx;
			//
			// 	_asm movzx		ecx, byte ptr[eax];
			// 	_asm mov		byte ptr[eax], 255;
			// 	_asm test     ecx, ecx;
			// 	_asm jnz      Next3
			// 	_asm mov		cl, byte ptr[edx];
			// 	_asm mov		byte ptr[eax], cl;
			// Next3:
			// 	_asm inc		eax;
			// 	_asm inc		edx;
			// 	_asm mov      dword ptr[pMsk], eax;
			// 	_asm mov      dword ptr[pSrc], edx;
		}
		//TC.GetExecutionTime( 0, 1000 );
#endif
		break;
	}
	return TRUE;
}

void CImageL::Dump()
{
	if (IsNull()) return;
	short bPP = GetBPP();
	FILE *fp;
	CString fname;
	fname.Format("C:\\TEMP\\%s.csv", CTime::GetCurrentTime().Format("%m%d%H%M%S"));
	fp = fopen(fname.GetBuffer(0), "wb");
	int i, j;
	if (fp)
	{
		for (i = 0; i < GetHeight(); i++)
		{
			BYTE *pDes = (BYTE *)GetPixelAddress(0, i);
			for (j = 0; j < GetWidth(); j++)
			{
				switch (bPP)
				{
				case 8:
					fprintf(fp, "%d,", *(pDes++));
					break;
				case 32:
					pDes++;
				case 24:
					fprintf(fp, "%d,", *(pDes++));
					fprintf(fp, "%d,", *(pDes++));
					fprintf(fp, "%d,", *(pDes++));
					break;
				}
			}
			fprintf(fp, "\n");
		}
		fclose(fp);
	}
	//Sleep(1100);
}

BOOL CImageL::GetArea(CImageL &Thumb, int sx, int sy, int wd, int ht)
{
	if (Thumb.IsNull())
	{
		if (!Thumb.Create(wd, ht, GetBPP())) return FALSE;
	}
	if ((Thumb.GetWidth() != wd) || (Thumb.GetHeight() != ht) || GetBPP() != Thumb.GetBPP())
	{
		Thumb.Destroy();
		if (!Thumb.Create(wd, ht, GetBPP())) return FALSE;
	}
	//Thumb.GetDim();
	int bypr = Thumb.GetBYPR();
	for (int y = sy; y < sy + ht; y++)
	{
		memcpy(Thumb.GetPixelAddress(0, y - sy), GetPixelAddress(sx, y), bypr);
	}
	return TRUE;
}

// Return a reduce image Thumb of same bit depth [3/6/2015 Yuen]
BOOL CImageL::GetArea(CImageL &Thumb, CRect &rc)
{
	return GetArea(Thumb, rc.left, rc.top, rc.Width(), rc.Height());
}

long CImageL::GetAreaCount(CRect &rc, short thres)
{
	if (IsNull()) return 0;
	switch (GetBYPR())
	{
	case 1:
		return AreaCount<sBINR>(thres);
		break;
	case 3:
		return AreaCount<sBGR>(thres);
		break;
	case 4:
		return AreaCount<sBGRA>(thres);
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

BOOL CImageL::ConvertToGrey()
{
	if (IsNull()) return FALSE;
	switch (GetBYPP())
	{
	case 1:
		//Convert2Grey<sBINR>();
		break;
	case 3:
		Convert2Grey<sBGR>();
		break;
	case 4:
		Convert2Grey<sBGRA>();
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

// Alternative function is Binarize() [7/7/2016 Yuen]
BOOL CImageL::Threshold(BYTE level)
{
	if (IsNull()) return FALSE;
	switch (GetBYPP())
	{
	case 1:
		return Threshold<sBINR>(level);
		break;
	case 3:
		return Threshold<sBGR>(level);
		break;
	case 4:
		return Threshold<sBGRA>(level);
	default:
		ASSERT(0);
		break;
	}
	return FALSE;
}

BOOL IMGL::CImageL::ZeroMean()
{
	CSingleLock Lock(&CS1); Lock.Lock();
	short mean = CalcMean();
	switch (GetBYPP())
	{
	case 1:
		SubOffset<sBINR>(mean);
		break;
	case 3:
		SubOffset<sBGR>(mean);
		break;
	case 4:
		SubOffset<sBGRA>(mean);
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

// Assuming 8 bits per channel pixel [3/25/2016 Yuen]
// scale: percentage area to find histo centered at image center [3/25/2016 Yuen]
// Return index of max bin [3/25/2016 Yuen]
int CImageL::Histo(float His[], int nBin, float scale, float sx, float sy, BOOL bShow)
{
	if (IsNull()) return 0;
	CSingleLock Lock(&CS1); Lock.Lock();

	int wd = GetWidth();
	int ht = GetHeight();
	float sf = nBin / 256.0f;
	memset(His, 0, nBin * sizeof(float));

	int iwd = wd * scale;
	int xst = wd * sx - iwd / 2; // Start X [3/25/2016 Yuen]
	if (xst < 0) xst = 0;
	int xed = xst + iwd;
	if (xed >= wd) { xed = wd - 1; xst = xed - wd; }

	int iht = ht * scale;
	int yst = ht * sy - iht / 2; // Start Y [3/25/2016 Yuen]
	if (yst < 0) yst = 0;
	int yed = yst + iht;
	if (yed >= ht) { yed = ht - 1; yst = yed - ht; }

	short bpp = GetBYPP();
#pragma omp parallel for
	for (int y = yst; y < yed; y++)
	{
		BYTE *p = (BYTE *)GetPixelAddress(xst, y);
		for (int x = xst; x < xed; x++)
		{
			BYTE v = BYTE(GetPixelInt24(p));
			if (bShow)
			{
				// Diagnostic information [3/25/2016 Yuen]
				if (((x - xst) < 3) || ((xed - x) < 3) || ((y - yst) < 3) || ((yed - y) < 3))
				{
					SetPixelInt24(p, 0, 0, 255);
				}
			}
			His[BYTE(v*sf)]++;
			p += bpp;
		}
	}

	// Normalizing histogram [3/25/2016 Yuen]
	int i, idx = -1;
	float max = -1;
	for (i = 0; i < nBin; i++)
	{
		float v = His[i];
		if (max < v) { max = v; idx = i; }
	}
	for (i = 0; i < nBin; i++)
	{
		His[i] /= max;
	}
	return idx;
}

// Note: Each area must have the same number of pixels or the ratio will be uneven [8/2/2016 Yuen]
int CImageL::AreaIntensity(float scale, float sx, float sy, BOOL bShow)
{
	if (IsNull()) return 0;
	CSingleLock Lock(&CS1); Lock.Lock();

	int wd = GetWidth();
	int ht = GetHeight();

	int iwd = wd * scale;
	int xst = wd * sx - iwd / 2; // Start X [3/25/2016 Yuen]
	if (xst < 0) xst = 0;
	int xed = xst + iwd;
	if (xed >= wd) { xed = wd - 1; xst = xed - wd; }

	int iht = ht * scale;
	int yst = ht * sy - iht / 2; // Start Y [3/25/2016 Yuen]
	if (yst < 0) yst = 0;
	int yed = yst + iht;
	if (yed >= ht) { yed = ht - 1; yst = yed - ht; }

	switch (GetBYPP())
	{
	case 1:
		return AreaIntensity<sBINR>(scale, xst, xed, yst, yed, bShow);
		break;
	case 3:
		return AreaIntensity<sBGR>(scale, xst, xed, yst, yed, bShow);
		break;
	case 4:
		return AreaIntensity<sBGRA>(scale, xst, xed, yst, yed, bShow);
		break;
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

float CImageL::GetAveIntensity()
{
	long sum = 0;
	int wd = GetWidth();
	int ht = GetHeight();
#pragma omp parallel for reduction (+:sum)
	for (int y = 0; y < ht; y++)
	{
		BYTE *p = (BYTE *)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++)
		{
			sum += SQR(*p); p += 3;
		}
	}
	return sum / float(wd * ht);
}

int CImageL::GetPixelInt24(BYTE *p)
{
	BYTE B = *(p++);
	BYTE G = *(p++);
	BYTE R = *p;
	return (R + 2 * G + B) / 4;
}

void CImageL::SetPixelIntGrey24(BYTE * p, BYTE n)
{
	*(p++) = n;
	*(p++) = n;
	*(p) = n;
}

void CImageL::SetPixelInt24(BYTE *p, BYTE B, BYTE G, BYTE R)
{
	*(p++) = R < 256 ? (R < 0 ? 0 : R) : 255;
	*(p++) = G < 256 ? (G < 0 ? 0 : G) : 255;
	*(p) = B < 256 ? (B < 0 ? 0 : B) : 255;
}

int IMGL::CImageL::CalcHistogram(CLine1D &Line)
{
	if (IsNull()) return 0;
	CSingleLock Lock(&CS1); Lock.Lock();

	BOOL bShow = TRUE;
	int wd = GetWidth();
	int ht = GetHeight();

	int iwd = wd;
	int xst = 0; // Start X [3/25/2016 Yuen]
	if (xst < 0) xst = 0;
	int xed = xst + iwd;
	if (xed > wd) { xed = wd; xst = xed - wd; }

	int iht = ht;
	int yst = 0; // Start Y [3/25/2016 Yuen]
	if (yst < 0) yst = 0;
	int yed = yst + iht;
	if (yed > ht) { yed = ht; yst = yed - ht; }

	switch (GetBYPP())
	{
	case 1:
		return Histogram<sBINR>(Line, xst, xed, yst, yed);
		break;
	case 3:
		return Histogram<sBGR>(Line, xst, xed, yst, yed);
		break;
	case 4:
		return Histogram<sBGRA>(Line, xst, xed, yst, yed);
		break;
	}
	return 0;
}