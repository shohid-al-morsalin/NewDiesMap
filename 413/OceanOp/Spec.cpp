// Spec.cpp: implementation of the CSpec class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "Spec.h"
#include "FFTINC.h"
#include "FFreq.h"
#include "PolyParams.h"

#include "spline.h"
#include <complex>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

double PI = 3.14159265f;

CSpec::CSpec() {
	N = 0;
	S = 0;

	MaxV = 0;
	Cmax = Cmin = 0;
	// 	jPmax = jPlow = jPhi = jLo = jHi = 0;

	pV = NULL;
	pPt = NULL;

	for (int i = 0; i < MAXPOLYVLU; i++) {
		Poly[i].Clear();
	}
}

CSpec::~CSpec() {
	Clear();
}

void CSpec::Alloc(int n, int s) {
	if (n > 0) {
		if (N != n) {
			if (pPt) {
				delete[] pPt;
			}
			N = n;
			pPt = new CSpecPoint[N];
		}
	}
	else {
		if (pPt) {
			delete[] pPt;
			pPt = NULL;
		}
		N = 0;
	}
	if (s > 0) {
		if (S != s) {
			if (pV) {
				delete[] pV;
			}
			S = s;
			pV = new CFFTPoint[S];
		}
	}
	else {
		if (pV) {
			delete[] pV;
			pV = NULL;
		}
		S = 0;
	}
}

void CSpec::ReAllocS(int s) {
	if (s > 0) {
		if (S != s) {
			if (pV) {
				delete[] pV;
			}
			S = s;
			pV = new CFFTPoint[S];
		}
	}
	else {
		if (pV) {
			delete[] pV;
		}
		S = 0;
	}
}

void CSpec::Clear() {
	if (pPt) {
		N = 0;
		delete[] pPt;
		pPt = NULL;
	}
	if (pV) {
		S = 0;
		delete[] pV;
		pV = NULL;
	}
	for (int i = 0; i < MAXPOLYVLU; i++) {
		Poly[i].Clear();
	}
}

void CSpec::Dup(CSpec& p) {
	if ((!p.N) || (!p.pPt)) {
		return;
	}
	CSingleLock Lock(&CS);
	Lock.Lock();

	Alloc(p.N, p.S);

	if (p.pV) {
		int sz = p.S * sizeof(CFFTPoint);
		memcpy(pV, p.pV, sz);
	}

	int i;
	CSpecPoint* pPd;
	CSpecPoint* pPs;

	pPd = pPt;
	pPs = p.pPt;

	for (i = 0; i < p.N; i++) {
		pPd->X1 = pPs->X1;
		pPd->Y1 = pPs->Y1;
		pPd->Y2 = pPs->Y2;
		pPd++;
		pPs++;
	}
	Cmin = p.Cmin;
	Cmax = p.Cmax;
	// 	jPhi = p.jPhi;
	// 	jPlow = p.jPlow;
	// 	jPmax = p.jPmax;
	// 	jLo = p.jLo;
	// 	jHi = p.jHi;
	MaxV = p.MaxV;

	for (i = 0; i < MAXPOLYVLU; i++) {
		Poly[i] = p.Poly[i];
	}

	//	Lock.Unlock();
}

void CSpec::Dump(CString fname) {
	if (!N || !pPt) {
		return;
	}
	FILE* fp = fopen(fname, "wb");
	if (!fp) {
		return;
	}

	CSpecPoint* pSp = pPt;
	for (int i = 0; i < N; i++) {
		fprintf(fp, "%d,%.8f,%f,%f\n", i, pSp->X1, pSp->Y1, pSp->Y2);
		pSp++;
	}

	fclose(fp);
}

void CSpec::DumpFFT(CString fname) {
	if (!S || !pV) {
		return;
	}
	FILE* fp = fopen(fname, "wb");
	if (!fp) {
		return;
	}

	CFFTPoint* p = pV;
	for (int i = 0; i < S / 2; i++) {
		fprintf(fp, "%d,%f,%f\n", i, p->V, p->P);
		p++;
	}

	fclose(fp);
}

void CSpec::FakeData(int n, int s, CFFreq* pFF) {
	Alloc(n, s);

	float PI2N = float(2 * PI / float(n));
	float PI2 = float(2 * PI / 360.0f);

	CSpecPoint* pSp = pPt;
	float wn1 = 1 / 951.429f;
	float wn2 = 1 / 1649.990f;
	double f1 = (wn1 - wn2) / double(n - 1);
	double p1 = PI / 5.0f;
	double p2 = PI / 12.0f;
	for (int i = 0; i < n; i++) {
		float v = 0;
		for (int j = 0; j < 10; j++) {
			if (pFF[j].Freq) {
				v += float(pFF[j].Ampl * sin(pFF[j].Freq * i * PI2N + pFF[j].Phase * PI2));
			}
		}
		pSp->Set1(wn2 + float(i * f1), v);
		pSp++;
	}
}

BOOL CSpec::IsAlloc() {
	if (!N || !pPt) {
		return FALSE;
	}
	return TRUE;
}

BOOL CSpec::IsAlloc2() {
	if (!S || !pV) {
		return FALSE;
	}
	return TRUE;
}

// Only affects pPt [12/9/2011 Administrator]
void CSpec::ZeroAve() {
	if (!N || !pPt) {
		//ASSERT(0);
		return;
	}

	double Sum = 0;
	CSpecPoint* pSp = pPt;

	int i;
	for (i = 0; i < N; i++) {
		Sum += pSp->Y1;
		pSp++;
	}
	Sum /= N;

	pSp = pPt;
	for (i = 0; i < N; i++) {
		pSp->Y1 -= float(Sum);
		pSp++;
	}
}

void CSpec::HannWindow() {
	if (!N || !pPt) {
		//ASSERT(0);
		return;
	}

	int i;
	CSpecPoint* pSp = pPt;

	float v = float(2 * PI / float(N - 1));
	for (i = 0; i < N; i++) {
		pSp->Y1 = float(pSp->Y1 * (1 - cos(v * i)) / 2.0f);
		pSp++;
	}
}

void CSpec::GaussianWindow() {
	if (!N || !pPt) {
		//ASSERT(0);
		return;
	}

	int i;
	CSpecPoint* pSp = pPt;
	float a = 0.45f;
	float v = float(float(N - 1) / 2.0f);
	for (i = 0; i < N; i++) {
		float v1 = (i - v) / (a * v);
		pSp->Y1 = float(pSp->Y1 * exp(-v1 * v1 / 2.0f));
		pSp++;
	}
}

void CSpec::ExponentWindow() {
	if (!N || !pPt) {
		//ASSERT(0);
		return;
	}

	int i;
	CSpecPoint* pSp = pPt;
	float D = 60;
	float t = (2.0f * D) / (N * 8.69f);

	float v1 = float(float(N - 1) / 2.0f);
	for (i = 0; i < N; i++) {
		pSp->Y1 = float(pSp->Y1 * (exp(-fabs(i - v1) * t)));
		pSp++;
	}
}

void CSpec::HammingWindow() {
	if (!N || !pPt) {
		//ASSERT(0);
		return;
	}

	int i;
	CSpecPoint* pSp = pPt;

	float v = float(2 * PI / float(N - 1));
	for (i = 0; i < N; i++) {
		pSp->Y1 = float(pSp->Y1 * (0.54 - 0.46 * cos(v * i)));
		pSp++;
	}
}

// Reject is in percentage [12/12/2011 Administrator]
void CSpec::FindPeaksFFT(float Reject) {
	if (!S || !pV) {
		return;
	}

	int i;

	CFFTPoint* p = pV;
	for (i = 0; i < S; i++) {
		p->P = p->V;
		p++;
	}

	p = pV;

	for (i = 0; i < S; i++) {
		if ((p + 1)->P > p->P) {
			p->P = 0;
		}
		if (p->P < 1) {
			p->P = 0;
		}
		p++;
	}
	p = pV + S - 1;
	for (i = 0; i < S; i++) {
		if ((p->P > 0) && ((p - 1)->P > p->P)) {
			p->P = 0;
		}
		p--;
	}

	p = pV;
	double max = 0;
	BOOL bFirst = TRUE;
	for (i = 0; i < S; i++) {
		if (p->P > 0) {
			if (bFirst) {
				max = p->P;
				bFirst = FALSE;
			}
			else {
				if (p->P > max) {
					max = p->P;
				}
			}
		}
		p++;
	}

	max = max * Reject / 100.0f;

	p = pV;
	for (i = 0; i < S; i++) {
		if (p->P > 0) {
			if (p->P < max) {
				p->P = 0;
			}
		}
		p++;
	}
}

void CSpec::FindPeakFreq(float Reject) {
	if (!N || !pPt) {
		return;
	}

	//Dump("A1.CSV");
	SmoothY2(1);
	//Dump("A2.CSV");

	int i;
	CSpecPoint* p;
	p = pPt;
	for (i = 1; i < N; i++) {
		if (p->Y2 < (p + 1)->Y2) {
			p->Y2 = 0;
		}
		p++;
	}

	p = pPt + N - 1;
	for (i = 1; i < N; i++) {
		if ((p->Y2 > 0) && (p->Y2 < (p - 1)->Y2)) {
			p->Y2 = 0;
		}
		else if (p->Y2 < 0) {
			p->Y2 = 0;
		}
		p--;
	}
	//Dump("A3.CSV");
	CleanUpPeaks(Reject);
	//Dump("A4.CSV");
}

void CSpec::CleanUpPeaks(float Reject) {
	if (!N || !pPt) {
		return;
	}

	int i, dx = 10;
	float max = 0;
	CSpecPoint* p = pPt;

	for (i = 0; i < N; i++) {
		if (p->Y2 > max) {
			max = p->Y2;
		}
		p++;
	}

	max *= Reject / 100.0f;

	p = pPt;
	for (i = 0; i < N; i++) {
		if (p->Y2 > 0) {
			if (p->Y2 < max) {
				p->Y2 = 0;
			}
			// 			else if ((p->Y1-(p-dx)->Y1) < 16)
			// 			{
			// 				p->Y2 = 0;
			// 			}
			else {
				if ((i > dx) && (i < N - dx)) {
					for (int j = -dx; j <= dx; j++) {
						if ((j != 0) && (p + j)->Y2 > 0) {
							if ((p + j)->Y1 > p->Y1) {
								p->Y2 = 0;
								break;
							}
						}
					}
				}
				else {
					p->Y2 = 0;
				}
			}
		}
		p++;
	}
}

// Load Y2 with smoothed value of Y1 [12/13/2011 Administrator]
void CSpec::SmoothY2(short n) {
	if ((!N) || (!pPt)) {
		return;
	}
	int i;
	CSpecPoint* p;

	float* pV1 = new float[N];
	float* pV2 = new float[N];
	float* pVV1 = pV1;
	float* pVV2 = pV2;

	p = pPt;
	for (i = 0; i < N; i++) {
		*(pVV1) = p->Y1;
		pVV1++;
		p++;
	}

	for (i = 0; i < n; i++) {
		pVV1 = pV1;
		pVV2 = pV2;
		*(pVV2) = *(pVV1);
		pVV2++; pVV1++;
		*(pVV2) = *(pVV1);
		pVV2++; pVV1++;
		*(pVV2) = *(pVV1);
		pVV2++; pVV1++;

		for (int j = 3; j < N - 3; j++) {
			*pVV2 = (*(pVV1 - 3) + *(pVV1 - 2) + *(pVV1 - 1) + *pVV1 + (*pVV1 + 1) + *(pVV1 + 2) + *(pVV1 + 3)) / 7.0f;
			pVV1++;
			pVV2++;
		}
		*(pVV2) = *(pVV1);
		pVV2++; pVV1++;
		*(pVV2) = *(pVV1);
		pVV2++; pVV1++;
		*(pVV2) = *(pVV1);

		float* pV3 = pV1;
		pV1 = pV2;
		pV2 = pV3;
	}

	p = pPt;
	pVV1 = pV1;
	for (i = 0; i < N; i++) {
		p->Y2 = *(pVV1);
		pVV1++;
		p++;
	}

	if (pV1) {
		delete[] pV1;
	}
	if (pV2) {
		delete[] pV2;
	}
}

void CSpec::Smooth(short n) {
	if ((!N) || (!pPt)) {
		return;
	}
	if (n < 1) {
		return;
	}
	int i;
	CSpecPoint* p;

	float* pV1 = new float[N];
	float* pV2 = new float[N];
	float* pVV1 = pV1;
	float* pVV2 = pV2;

	p = pPt;
	for (i = 0; i < N; i++) {
		*(pVV1) = p->Y1;
		pVV1++;
		p++;
	}

	for (i = 0; i < n; i++) {
		pVV1 = pV1;
		pVV2 = pV2;
		*(pVV2) = *(pVV1);
		pVV2++; pVV1++;

		for (int j = 1; j < N - 1; j++) {
			*pVV2 = (*(pVV1 - 1) + *pVV1 + (*pVV1 + 1)) / 3.0f;
			pVV1++;
			pVV2++;
		}
		pVV2 = pV2;
		*(pV2) = *(pV2 + 1);
		*(pV2 + N - 1) = *(pV2 + N - 2);
	}

	p = pPt;
	pVV1 = pV1;
	for (i = 0; i < N; i++) {
		p->Y1 = *(pVV1);
		pVV1++;
		p++;
	}

	if (pV1) {
		delete[] pV1;
	}
	if (pV2) {
		delete[] pV2;
	}
}

void CSpec::GetWnum(float& L1, float& L2, short& n1, short& n2) {
	if (!N || !pPt) {
		return;
	}
	int i;
	short C = 0;
	CSpecPoint* p = pPt;
	for (i = 0; i < N; i++) {
		if (p->Y2 > 0) {
			C++;
		}
		p++;
	}
	short D = 0;
	p = pPt;
	if (p->X1 < 1) {
		for (i = 0; i < N; i++) {
			if (p->Y2 > 0) {
				D++;
				if (D == 2) {
					L1 = p->X1;
					n1 = D;
				}
				if (D == C - 1) {
					L2 = p->X1;
					n2 = D;
				}
			}
			p++;
		}
	}
	else {
		for (i = 0; i < N; i++) {
			if (p->Y2 > 0) {
				D++;
				if (D == 2) {
					L1 = 1 / p->X1;
					n1 = D;
				}
				if (D == C - 1) {
					L2 = 1 / p->X1;
					n2 = D;
				}
			}
			p++;
		}
	}
}

// Expand the data point to a bigger array through interpolation [11/19/2012 Yuen]
// Will set Y2 to 0 in new array [12/14/2011 Administrator]
// Also convert wavelength to wave number [12/15/2011 Administrator]
void CSpec::Resample(short n) {
	if ((n < 1) || (!pPt)) {
		return;
	}
	if ((n * N) > 65536) {
		// limit to 65536 points only [11/25/2012 Yuen]
		return;
	}

	short dir = 1;
	int i, j, c = 0;
	CSpecPoint* pPn = new CSpecPoint[n * N];
	if (!pPn) {
		return;
	}
	CSpecPoint* pS = pPt;

	if (pPt[0].X1 < pPt[2].X1) {
		dir = 0;
	}

	CSpecPoint* pD = pPn;
	float sf = (pPt[N - 1].X1 - pPt[0].X1) / float(n * N - 1);

	float Start = pS->X1;
	for (i = 0; i < n * N; i++) {
		pD->X1 = Start + i * sf;
		pD->Y2 = 0;
		pD++;
	}

	int St = 0;
	pD = pPn;
	pD->Y1 = pPt[0].Y1;
	pD++;
	for (i = 1; i < n * N; i++) {
		pS = pPt + St;
		for (j = St; j < St + 9; j++) {
			if (dir) {
				if (pD->X1 >= pS->X1) {
					pD->Y1 = ((pS)->Y1 - (pS - 1)->Y1) * (pD->X1 - (pS - 1)->X1) / ((pS)->X1 - (pS - 1)->X1) + (pS - 1)->Y1;
					St = j - 1;
					break;
				}
			}
			else {
				if (pD->X1 <= pS->X1) {
					pD->Y1 = ((pS)->Y1 - (pS - 1)->Y1) * (pD->X1 - (pS - 1)->X1) / ((pS)->X1 - (pS - 1)->X1) + (pS - 1)->Y1;
					St = j - 1;
					break;
				}
			}
			pS++;
		}
		pD++;
	}

	N = n * N;

	delete[] pPt;
	pPt = pPn;
}

void CSpec::ToFreq() {
	if (!N || !pPt) {
		return;
	}
	int i;
	//CSpecPoint* p = pPt;
	//for (i = 0; i < N; i++) {
	//	p->X1 = 1 / p->X1;
	//	p++;
	//}
	CSpecPoint* p = &pPt[N - 1];
	double* mX = new double[N]; double* pX = mX;
	double* mY = new double[N]; double* pY = mY;
	//double xx = 1.f;// / p[N - 1].X1;
	for (i = N - 1; i >= 0; i--) {
		*pX = /*xx - */1.f / p->X1;
		*pY = (p->Y1);
		//p->X = 1 / p->X;
		p--;
		pX++;
		pY++;
	}
	//FILE* fp = fopen("C:\\TEMP\\DUMP_TF_MXMY.csv", "wb");
	//if (!fp) {
	//	return;
	//}
	//for (int i = 0; i < N; i++) {
	//	fprintf(fp, "%f,%f\n", mX[i], mY[i]);
	//}
	//fclose(fp);
	//Dump("C:\\TEMP\\DUMP_TF_1.csv");
	tk::spline S;
	S.set_points(mX, mY, N);
	double st = mX[0], ed = mX[N - 1], inc = (ed - st) / double(N);

	p = &pPt[N - 1];
	for (int i = 0; i < N; i++) {
		p->X1 = st + i * inc;
		p->Y1 = S(p->X1);
		p--;
	}
	//Dump("C:\\TEMP\\DUMP_TF_2.csv");
	delete[] mX; mX = NULL;
	delete[] mY; mY = NULL;
}

void CSpec::BartlettHannWindow() {
	if (!N || !pPt) {
		return;
	}

	int i;
	CSpecPoint* pSp = pPt;
	float a0 = 0.62f;
	float a1 = 0.48f;
	float a2 = 0.38f;
	float v = float(2 * PI / (N - 1));
	for (i = 0; i < N; i++) {
		float v1 = float(fabs(i / (N - 1) - 0.5f));
		pSp->Y1 = float(pSp->Y1 * (a0 - a1 * v1 + a2 * cos(i * v)));
		pSp++;
	}
}

void CSpec::CopyY2(CSpec& Spec) {
	if ((!Spec.N) || (!Spec.pPt)) {
		return;
	}

	if (N != Spec.N) {
		return;
	}

	CSpecPoint* p1 = pPt;
	CSpecPoint* p2 = Spec.pPt;

	for (int i = 0; i < N; i++) {
		p1->Y2 = p2->Y2;
		p1++; p2++;
	}
}

// Calculate Max and Min intensity for scaling purposes [5/13/2013 user]
void CSpec::CalcMaxMin() {
	if ((!N) || (!pPt)) {
		return;
	}
	CSpecPoint* pSp = pPt;
	Cmax = Cmin = pSp->Y1;
	pSp++;
	for (int i = 1; i < N; i++) {
		if (pSp->Y1 < Cmin) {
			Cmin = pSp->Y1;
		}
		if (pSp->Y1 > Cmax) {
			Cmax = pSp->Y1;
		}
		pSp++;
	}
}

// Remember to multiply RI with fThkFactor [11/20/2012 Yuen]
void CSpec::GetThicknessInRange(float fThkFactor, short nCycle) {
	if ((!N) || (!pPt)) {
		return;
	}

	// assume X is in KSpace
	float deltawnum = float(fabs(pPt[N - 1].X1 - pPt[0].X1) / N * nCycle);
	//if (pPt[0].X1 < 1) {
	//	deltawnum = float(fabs(1 / pPt[N - 1].X1 - 1 / pPt[0].X1) / N * nCycle);
	//}
	float delta = float((0.5 / deltawnum) / float(2 * S) / 1000);

	for (int j = 0; j < MAXPOLYVLU; j++) {
		CPolyParams* pPl = &Poly[j];
		if (pPl->fThk == -1)	// -1 indicates that calculation needed [11/29/2012 Yuen]
		{
			float sf = (pPl->fRI * fThkFactor) / delta;
			pPl->jPlow = pPl->fMin * sf;
			pPl->jPhi = pPl->fMax * sf;

			float max = -1;
			pPl->jPmax = -1;
			CFFTPoint* p = &pV[pPl->jPlow];
			int ll = pPl->jPlow - (pPl->jPhi - pPl->jPlow);
			if (ll < 0) {
				ll = 0;
			}
			int ul = pPl->jPhi + (pPl->jPhi - pPl->jPlow);
			if (ul >= S) {
				ul = S - 1;
			}
			int i;
			for (i = pPl->jPlow; i < pPl->jPhi; i++) {
				if (p->P > 0) {
					if (max < p->P) {
						max = p->P;
						pPl->jPmax = i;
					}
				}
				p++;
			}

			p = &pV[0];
			float max2 = -1;
			for (i = 0; i < S / 8; i++) {
				if (p->P > 0) {
					if (max2 < p->P) {
						max2 = p->P;
					}
				}
				p++;
			}
			if (pPl->jPmax > -1) {
				pPl->fThk = pPl->jPmax * delta / (pPl->fRI * fThkFactor);	// this is located thickness [11/20/2012 Yuen]
				pPl->GOF = max / max2;
				// Calculate peak with at half height [1/12/2012 Administrator]
				max /= 2.0f;
				pPl->jLo = pPl->jPlow;
				pPl->jHi = pPl->jPhi;
				p = &pV[pPl->jPmax - 1];
				for (i = pPl->jPmax - 1; i >= ll; i--) {
					if (p->V < max) {
						pPl->jLo = i;
						break;
					}
					p--;
				}
				p = &pV[pPl->jPmax + 1];
				for (i = pPl->jPmax + 1; i <= ul; i++) {
					if (p->V < max) {
						pPl->jHi = i;
						break;
					}
					p++;
				}
				pPl->WHH = GetThickness(pPl->jHi, pPl->fRI * fThkFactor) - GetThickness(pPl->jLo, pPl->fRI * fThkFactor);
			}
			else {
				pPl->fThk = -1;
				pPl->GOF = 0;
				pPl->WHH = -1;
			}
		}
		else {
			break;
		}
	}
}

// Remember to multiply RI with fThkFactor [11/20/2012 Yuen]
void CSpec::GetThicknessInRangeB(float fThkFactor, short nCycle) {
	if ((!N) || (!pPt)) {
		return;
	}

	float deltawnum = float(fabs(pPt[N - 1].X1 - pPt[0].X1) / N * nCycle);
	if (pPt[0].X1 < 1) {
		deltawnum = float(fabs(1 / pPt[N - 1].X1 - 1 / pPt[0].X1) / N * nCycle);
	}
	float delta = float((0.5 / deltawnum) / float(2 * S) / 1000);

	float jPMax1 = 0;	// Max position of first calculation that appear in MeSet [11/29/2012 Yuen]
	for (int j = 0; j < MAXPOLYVLU; j++) {
		CPolyParams* pPl = &Poly[j];
		if (pPl->fThk == -1)	// -1 indicates that calculation needed [11/29/2012 Yuen]
		{
			float sf = (pPl->fRI * fThkFactor) / delta;
			pPl->jPlow = pPl->fMin * sf;
			pPl->jPhi = pPl->fMax * sf;

			float max = -1;
			pPl->jPmax = -1;
			CFFTPoint* p = &pV[pPl->jPlow];
			int ll = pPl->jPlow - (pPl->jPhi - pPl->jPlow);
			if (ll < 0) {
				ll = 0;
			}
			int ul = pPl->jPhi + (pPl->jPhi - pPl->jPlow);
			if (ul >= S) {
				ul = S - 1;
			}
			int i;
			for (i = pPl->jPlow; i < pPl->jPhi; i++) {
				if (p->P > 0) {
					if (max < p->P) {
						max = p->P;
						pPl->jPmax = i;
					}
				}
				p++;
			}

			p = &pV[0];
			float max2 = -1;
			for (i = 0; i < S / 8; i++) {
				if (p->P > 0) {
					if (max2 < p->P) {
						max2 = p->P;
					}
				}
				p++;
			}
			// Calculate thickness after found max peak intensity [11/29/2012 Yuen]
			if (pPl->jPmax > -1) {
				if (j == 0) {
					jPMax1 = pPl->jPmax;
				}
				else {
					pPl->jPmax -= jPMax1;	// Make offset adjustment [11/29/2012 Yuen]
				}
				pPl->fThk = pPl->jPmax * delta / (pPl->fRI * fThkFactor);	// this is located thickness [11/20/2012 Yuen]
				pPl->GOF = max / max2;
				// Calculate peak with at half height [1/12/2012 Administrator]
				max /= 2.0f;
				pPl->jLo = pPl->jPlow;
				pPl->jHi = pPl->jPhi;
				p = &pV[pPl->jPmax - 1];
				for (i = pPl->jPmax - 1; i >= ll; i--) {
					if (p->V < max) {
						pPl->jLo = i;
						break;
					}
					p--;
				}
				p = &pV[pPl->jPmax + 1];
				for (i = pPl->jPmax + 1; i <= ul; i++) {
					if (p->V < max) {
						pPl->jHi = i;
						break;
					}
					p++;
				}
				pPl->WHH = GetThickness(pPl->jHi, pPl->fRI * fThkFactor) - GetThickness(pPl->jLo, pPl->fRI * fThkFactor);
			}
			else {
				pPl->fThk = -1;
				pPl->GOF = 0;
				pPl->WHH = -1;
			}
		}
		else {
			break;
		}
	}
}

float CSpec::GetDeltaWN() {
	if ((!N) || (!pPt)) {
		return 1;
	}
	float deltawnum = float(fabs(pPt[N - 1].X1 - pPt[0].X1) / N);
	if (pPt[0].X1 < 1) {
		deltawnum = float(fabs(1 / pPt[N - 1].X1 - 1 / pPt[0].X1) / N);
	}

	float delta = float((0.5 / deltawnum) / float(2 * S) / 1000);
	return delta;
}

// int CSpec::GetIndex(float thk, float RI)
// {
// 	return thk * RI / GetDeltaWN();
// }

float CSpec::GetThickness(int index, float RI) {
	return index * GetDeltaWN() / RI;
}

void CSpec::BlackManWindow() {
	if (!N || !pPt) {
		return;
	}

	int i;
	CSpecPoint* pSp = pPt;
	for (i = 0; i < N; i++) {
		float v = 0.42f - float(0.5 * cos((2 * PI * i) / float(N) + 0.08 * cos(4 * PI * i / float(N))));
		pSp->Y1 = v * pSp->Y1;
		pSp++;
	}
}

// n is the number of element in pCycle and pPhase [11/17/2012 Yuen]
BOOL CSpec::SimuSpec(short n, float* pCycle, float* pAmp, float* pPhase) {
	Alloc(512, 0);
	int i;
	CSpecPoint* pSp = pPt;
	for (i = 0; i < N; i++) {
		float v = 0.42f - float(0.5 * cos((2 * PI * i) / float(N) + 0.08 * cos(4 * PI * i / float(N))));
		pSp->Y1 = v * pSp->Y1;
		pSp++;
	}
	return TRUE;
}

// Padding the spectrum at the end n times [11/19/2012 Yuen]
void CSpec::Repeat(short n) {
	if (n <= 1) {
		return;
	}
	if ((n < 1) || (!pPt)) {
		return;
	}
	if ((n * N) > 66636) {
		return;
	}
	int i, j, c = 0;
	CSpecPoint* pPn = new CSpecPoint[n * N];
	if (!pPn) {
		return;
	}
	CSpecPoint* pS = pPt;
	CSpecPoint* pD = pPn;
	float sf = (pPt[N - 1].X1 - pPt[0].X1) / float(n * N - 1);
	float Start = pS->X1;
	if (pPt[0].X1 < 1) {
		sf = (1 / pPt[N - 1].X1 - 1 / pPt[0].X1) / float(n * N - 1);
		Start = 1 / pS->X1;
	}
	int ix = 0;
	for (i = 0; i < n; i++) {
		pS = pPt;
		for (j = 0; j < N; j++) {
			pD->X1 = Start + ix * sf;
			pD->Y1 = pS->Y1;
			pD->Y2 = 0;
			pD++; pS++; ix++;
		}
	}
	N = n * N;
	delete[] pPt;
	pPt = pPn;
}