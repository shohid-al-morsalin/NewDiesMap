// Spec.h: interface for the CSpec class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SpecPoint.h"
#include "..\..\E95\FFTPoint.h"
#include "PolyParams.h"
//#include "SpecPar.h"

#define MAXPOLYVLU		8

class CFFreq;
//class CPolyParams;

class CSpec {
public:
	CCriticalSection CS;

	enum SPFILTER {
		NONE, HANN, HAMMING, GAUSIAN, EXPONENT, BARTLETT, BLACKMAN
	};

public:
	void Repeat(short n);
	BOOL SimuSpec(short n, float* pCycle, float* pAmp, float* pPhase);
	void BlackManWindow();
	float GetDeltaWN();
	float GetThickness(int index, float RI);
	void GetThicknessInRange(float fThkFactor, short nCycle);
	void GetThicknessInRangeB(float fThkFactor, short nCycle);
	void CalcMaxMin();
	void CopyY2(CSpec& Spec);
	void BartlettHannWindow();
	void GaussianWindow();
	void ToFreq();
	void Smooth(short n);
	void Resample(short n);
	void GetWnum(float& L1, float& L2, short& n1, short& n2);
	void SmoothY2(short n);
	void CleanUpPeaks(float Reject);
	void FindPeakFreq(float Reject);
	void FindPeaksFFT(float Reject);
	void ReAllocS(int S);
	void ExponentWindow();
	void HammingWindow();
	void HannWindow();
	void ZeroAve();
	BOOL IsAlloc2();
	BOOL IsAlloc();
	void FakeData(int n, int s, CFFreq* pFF);
	void Dump(CString fname);
	void DumpFFT(CString fname);
	void Dup(CSpec& Spec);
	void Clear();
	void Alloc(int n, int s);

	int N, S;

	// 	CSpecPar Par[MAXPOLYVLU];
	CPolyParams Poly[MAXPOLYVLU];

	//float GOF, WHH;	// Goodness of fitness, width at half height [1/12/2012 Administrator]
	float Cmax, Cmin;
	CSpecPoint* pPt;
	CFFTPoint* pV;

	float MaxV;	// FFT peak position [11/25/2012 Yuen]
	//float fThk;
	//int jPmax,jPlow,jPhi,jLo,jHi;

	CSpec();
	virtual ~CSpec();
};