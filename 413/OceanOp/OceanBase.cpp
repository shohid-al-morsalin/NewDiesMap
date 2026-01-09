// OceanBase.cpp: implementation of the COceanBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dos.h"
#include "OceanBase.h"
#include "math.h"
#include <complex>

#include "Fourier.h"

#include "..\..\SRC\DOSUtil.h"

#include "MeParam.h"
#include "413App.h"

using namespace std;

#define NPIX 2048

#define PI 3.1415926535897932384626433832795
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

short NumDATAPT = 512;

COceanBase::COceanBase() {
	bSimu = TRUE;
	FileDarkSpec = "DarkSpec.TXT";
	FileRefSpec = "RefSpec.TXT";

	hInstance = NULL;
	ExposureTimes = 100;
	StatusFlag = 0;

	nMethod = 0;
	boxcar = 1;
	nRepeat = 1;
	nMeastime = 1;

	lambdamax = 951.429f;	// Starting value, will change when real spectrum is captured [1/6/2012 Administrator]
	lambdamin = 1649.99f;	// Same above [1/6/2012 Administrator]

	fCalCoeff = fThkFactor = 0.0f;

	nFFTFac = 16;
	nFFTSP = NUMBEROFMAPPEDPOINTS / nFFTFac;

	numberOfSpectrometersAttached = 1;  // Fake one spectrometer [2/20/2012 C2C]

	CalibrationReset(NumDATAPT);

	CString mdir, fname;

	DosUtil.GetProgramDir(mdir);
	fname = mdir + FileRefSpec;
	LoadSpectrum(&CalibrSpectrum, fname);

	fname = mdir + FileDarkSpec;
	LoadSpectrum(&BackGdSpectrum, fname);
}

COceanBase::~COceanBase() {}

BOOL COceanBase::Result(CSpec* pISpec, CSpec* pOSpec, CSpec::SPFILTER filter, int nCycle, int NumMappedPoints, short NSmooth, short nExpand) {
	if (Result2(pISpec, pOSpec, filter, nCycle, NumMappedPoints, NSmooth, nExpand)) {
		pOSpec->GetThicknessInRange(fThkFactor, nCycle);
		return TRUE;
	}
	return FALSE;
}

BOOL COceanBase::ResultB(CSpec* pISpec, CSpec* pOSpec, CSpec::SPFILTER filter, int nCycle, int NumMappedPoints, short NSmooth, short nExpand) {
	if (Result2(pISpec, pOSpec, filter, nCycle, NumMappedPoints, NSmooth, nExpand)) {
		pOSpec->GetThicknessInRange(fThkFactor, nCycle);
		return TRUE;
	}
	return FALSE;
}

// Similar to Result but without GetThicknessInRange [11/20/2012 Yuen]
BOOL COceanBase::Result2(CSpec* pISpec, CSpec* pOSpec, CSpec::SPFILTER filter, int nCycle, int NumMappedPoints, short NSmooth, short nExpand) {
	pOSpec->Dup(*pISpec);

	if (BackgroundTreadment(pOSpec, NSmooth)) {
		ModifySpectrum(pOSpec, filter, nCycle,/*NSmooth,*/nExpand);
		CFourier FFT;
		FFT.ComplexFFT(pOSpec, pOSpec->N, NumMappedPoints, 1);
		pOSpec->FindPeaksFFT(2.5f);
		pOSpec->CalcMaxMin();
		return TRUE;
	}
	return FALSE;
}

// n = index of search thickness, starts from 0 [11/20/2012 Yuen]
int COceanBase::StartTF(CMeParam* p, CString dirc, short SpecNo) {
	CString fname = "";
	CSpec* pIn = NULL;

	if (SpecNo > 100) {
		if (SpecNo == 101) {
			// Background [11/25/2012 Yuen]
			fname.Format("C:\\TEMP\\SPEC\\%s", FileDarkSpec);
			pIn = &BackGdSpectrum;
		}
		else if (SpecNo == 102) {
			// Reference [11/25/2012 Yuen]
			fname.Format("C:\\TEMP\\SPEC\\%s", FileRefSpec);
			pIn = &CalibrSpectrum;
		}
		if (pIn) {
			if (p413App->OpenSpecFile(fname, &CurrSpectrum)) {
				pIn->CalcMaxMin();
			}
			return pIn->N;
		}
	}
	else {
		fname.Format(dirc, SpecNo % 50);
		if (p413App->OpenSpecCSV(fname, &CurrSpectrum)) {
			CurrSpectrum.CalcMaxMin();
			if (p) {
				SetParam(p->Ta.Method, 1.0f, p->Ta.IntegTime, p->Ta.Boxcar, p413App->Global.TFScalingFactor);
			}
		}
		return CurrSpectrum.N;
	}
	return 0;
}

long COceanBase::Getms(SYSTEMTIME systime) {
	long temp;
	temp = systime.wMilliseconds;
	temp += systime.wSecond * 1000;
	temp += systime.wMinute * 60 * 1000;
	temp += systime.wHour * 60 * 60 * 1000;
	temp += systime.wDay * 24 * 60 * 60 * 1000;
	return temp;
}

// Return FALSE if the operation cannot be perform [2/15/2012 Administrator]
BOOL COceanBase::GetBackground(short nSmooth) {
	int temp = 0;

	temp = StartTF(NULL, "", 101);

	if ((temp == 512) || (temp == 2048)) {
		// Successfully collected a spectrum [6/7/2011 XPMUser]
		BackGdSpectrum.Dup(CurrSpectrum);
		BackGdSpectrum.Smooth(nSmooth);
		BackGdSpectrum.CalcMaxMin();

		CString mdir, fname;
		DosUtil.GetProgramDir(mdir);
		fname = mdir + FileDarkSpec;
		FILE* f = fopen(fname, "w");
		if (f) {
			CSpecPoint* pSb = BackGdSpectrum.pPt;
			for (int i = 0; i < BackGdSpectrum.N; i++) {
				fprintf(f, "%f\t%f\n", pSb->X1, pSb->Y1);
				pSb++;
			}
			fclose(f);
		}
		return TRUE;
	}
	return FALSE;
}

BOOL COceanBase::GetReference(short nSmooth) {
	int temp = 0;

	temp = StartTF(NULL, "", 102);

	if ((temp == 512) || (temp == 2048)) {
		// Successfully collected a spectrum [6/7/2011 XPMUser]
		CalibrSpectrum.Dup(CurrSpectrum);
		CalibrSpectrum.Smooth(nSmooth);
		CalibrSpectrum.CalcMaxMin();

		CString mdir, fname;

		DosUtil.GetProgramDir(mdir);
		fname = mdir + FileRefSpec;
		FILE* f = fopen(fname, "w");
		if (f) {
			CSpecPoint* pSc = CalibrSpectrum.pPt;
			for (int i = 0; i < CalibrSpectrum.N; i++) {
				fprintf(f, "%f\t%f\n", pSc->X1, pSc->Y1);
				pSc++;
			}
			fclose(f);
		}
		return TRUE;
	}
	return FALSE;
}

// One of the entry point [11/20/2012 Yuen]
float COceanBase::GetPolyThickness(CMeParam* pMe, float fMin, float fMax, float fRI, CSpec::SPFILTER filter, int nCycle, int numMappedPoints, short NSmooth, short nExpand, BOOL bCapture) {
	if (bCapture) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "IMAGES\\SEM06\\%03d-0_1.CSV";
		switch (StartTF(pMe, dir, Counter)) {
		case 512:
		case 1024:
		case 2048:
		case 4096:
			// No error [11/20/2012 Yuen]
			break;
		default:
			// error if not in simulation mode [11/20/2012 Yuen]
			if (!bSimu) {
				return 0;
			}
			break;
		}
	}
	// @@@ [11/20/2012 Yuen]
	ClearPolyThk(&CurrSpectrum);
	BOOL bMultiPeaks = FALSE;
	if (pMe) {
		for (int i = 0; i < MAXTHKVLU; i++) {
			if (pMe->Ta.RI[i]) {
				bMultiPeaks = TRUE;
				SetActivatePoly(&CurrSpectrum, i, pMe->Ta.ThkMin[i], pMe->Ta.ThkMax[i], pMe->Ta.RI[i]);
			}
		}
	}
	if (!bMultiPeaks) {
		SetActivatePoly(&CurrSpectrum, 0, fMin, fMax, fRI);
	}
	//SetActivatePoly(&CurrSpectrum,0,fMin,fMax,fRI);
	Result(&CurrSpectrum, &ResultSpectrum, filter, nCycle, numMappedPoints, NSmooth, nExpand);
	return ResultSpectrum.Poly[0].fThk;
}

// One of the entry point [11/20/2012 Yuen]
CPolyParams* COceanBase::GetPolyThicknessB(TParam* pTa, CSpec::SPFILTER filter, int nCycle, int numMappedPoints, short NSmooth, short nExpand, float TFScalingFactor, BOOL bCapture) {
	if (bCapture) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "IMAGES\\SEM06\\%03d-0_2.CSV";
		switch (StartTF(NULL, dir, Counter)) {
		case 512:
		case 1024:
		case 2048:
		case 4096:
			// No error [11/20/2012 Yuen]
			break;
		default:
			// error if not in simulation mode [11/20/2012 Yuen]
			if (!bSimu) {
				return NULL;
			}
			break;
		}
	}
	// @@@ [11/20/2012 Yuen]
	int i;

	SetParam(pTa->Method, 1.0f, pTa->IntegTime, pTa->Boxcar, TFScalingFactor);
	ClearPolyThk(&CurrSpectrum);
	int n = 0;
	for (i = 0; i < MAXTHKVLU; i++) {
		if (pTa->RI[i] > 0) SetActivatePoly(&CurrSpectrum, n++, pTa->ThkMin[i], pTa->ThkMax[i], pTa->RI[i]);
	}
	if (n > 0) {
		Result(&CurrSpectrum, &ResultSpectrum, filter, nCycle, numMappedPoints, NSmooth, nExpand);
	}

	return ResultSpectrum.Poly;
}

void COceanBase::CalibrationReset(short Nwave) {
	// 	float lambdamax = 951.429f;	// Starting value, will change when real spectrum is captured [1/6/2012 Administrator]
	// 	float lambdamin = 1649.99f;	// Same above [1/6/2012 Administrator]
	float deltal = (lambdamax - lambdamin);
	float np = float(Nwave - 1);

	CalibrSpectrum.Alloc(Nwave, 0);
	BackGdSpectrum.Alloc(Nwave, 0);
	CurrSpectrum.Alloc(Nwave, 0);

	CSpecPoint* pSc = CalibrSpectrum.pPt;
	CSpecPoint* pSb = BackGdSpectrum.pPt;
	CSpecPoint* pSp = CurrSpectrum.pPt;
	for (int i = 0; i < Nwave; i++) {
		float w = lambdamin + deltal * i / np;
		pSc->Set1(w, 1.0); pSc++;
		pSb->Set1(w, 0.0); pSb++;
		pSp->Set1(w, 1.0); pSp++;
	}
}

void COceanBase::Close() {}

void COceanBase::SetParam(int Method, float Coeff, int meastime, int Boxcar, float TFScalingFactor) {
	nMethod = Method;
	fCalCoeff = Coeff;
	boxcar = Boxcar;
	nMeastime = meastime;
	fThkFactor = TFScalingFactor;
}

void COceanBase::Wait() {
	Sleep(300);
}

void COceanBase::ShowFreqSpec(CListBox& m_cList) {
	// 	CString str;
	// 	m_cList.ResetContent();
	// 	if (oopars.nFFTEnd >= nFFTData.nPoint)
	// 	{
	// 		return ;
	// 	}
	// 	for(int j=oopars.nFFTBeg;j<=oopars.nFFTEnd;j++)
	// 	{
	// 		str.Format("%d, %.2f", j+1, nFFTData.pPt[j].Y1);
	// 		m_cList.AddString(str);
	// 	}
}

void COceanBase::CalcMaxMin(float* val, int len, float& Vmax, float& Vmin) {
	Vmax = Vmin = *(val++);

	for (int i = 1; i < len; i++) {
		if (*val > Vmax) {
			Vmax = *val;
		}
		if (*val < Vmin) {
			Vmin = *val;
		}
		val++;
	}
}

float COceanBase::CosineInterpolate(float y1, float y2, float mu) {
	float mu2;

	mu2 = float((1 - cos(mu * PI)) / 2.0f);
	return (y1 * (1 - mu2) + y2 * mu2);
}

BOOL COceanBase::ModifySpectrum(CSpec* pRSpec, CSpec::SPFILTER filter, int nCycle, /*short NSmooth, */short nExpand) {
	CSpec Spec2;

	pRSpec->ToFreq();
	// 	pRSpec->Smooth(NSmooth);
	if (nExpand > 1) {
		pRSpec->Resample(nExpand);
	}
	pRSpec->ZeroAve();
	if (p413App->Global.bCycleFirst) {
		pRSpec->Repeat(nCycle);
	}

	switch (filter) {
	case CSpec::HANN:
		pRSpec->HannWindow();
		pRSpec->ZeroAve();
		break;
	case CSpec::HAMMING:
		pRSpec->HammingWindow();
		pRSpec->ZeroAve();
		break;
	case CSpec::GAUSIAN:
		pRSpec->GaussianWindow();
		pRSpec->ZeroAve();
		break;
	case CSpec::EXPONENT:
		pRSpec->ExponentWindow();
		pRSpec->ZeroAve();
		break;
	case CSpec::BARTLETT:
		pRSpec->BartlettHannWindow();
		pRSpec->ZeroAve();
		break;
	case CSpec::BLACKMAN:
		pRSpec->BlackManWindow();
		pRSpec->ZeroAve();
		break;
	}

	if (!p413App->Global.bCycleFirst) {
		pRSpec->Repeat(nCycle);
	}

	return TRUE;
}

BOOL COceanBase::LoadSpectrum(CSpec* pSpec, CString fname) {
	if (!pSpec) {
		return FALSE;
	}

	short cnt = 0;
	FILE* f = fopen(fname, "r");
	if (f) {
		float freq, Intn;
		while (1) {
			if (fscanf(f, "%f\t%f\n", &freq, &Intn) == EOF) {
				break;
			}
			cnt++;
		}
		fclose(f);
	}

	switch (cnt) {
	case 512:
	case 2048:
		break;
	default:
		return 0;
	}

	pSpec->Alloc(cnt, 0);

	f = fopen(fname, "r");
	if (f) {
		CSpecPoint* pSc = pSpec->pPt;
		float freq, Intn;
		for (int i = 0; i < cnt; i++) {
			if (fscanf(f, "%f\t%f\n", &freq, &Intn) == EOF) {
				break;
			}
			pSc->Set1(freq, Intn);
			pSc++;
		}
		fclose(f);
	}
	pSpec->CalcMaxMin();

	return TRUE;
}

BOOL COceanBase::Initialize(CListBox* pMessage) {
	pMessage->InsertString(pMessage->GetCount(), "Thin Film system (simu) started OK");
	return TRUE;
}

void COceanBase::PreparefThk(CSpec* pSpec, short n) {
	int i;
	for (i = 0; i < n; i++) {
		pSpec->Poly[i].MarkfThk();
	}
	for (i = n; i < MAXPOLYVLU; i++) {
		pSpec->Poly[i].ClearfThk();
	}
}

void COceanBase::SetActivatePoly(CSpec* pSpec, short n, float fMin, float fMax, float fRI) {
	if (fMin < 0) {
		fMin = 0;
	}
	if (fMax < 0) {
		fMax = 0;
	}
	pSpec->Poly[n].MarkfThk();
	pSpec->Poly[n].fMax = fMax;
	pSpec->Poly[n].fMin = fMin;
	pSpec->Poly[n].fRI = fRI;
}

void COceanBase::ClearPolyThk(CSpec* pSpec) {
	for (int i = 0; i < MAXPOLYVLU; i++) {
		pSpec->Poly[i].ClearfThk();
	}
}

BOOL COceanBase::BackgroundTreadment(CSpec* pOSpec, short nSmooth) {
	int i;
	float dDataSpectrum;
	float dCalibrationSpectrum;
	CSpecPoint* pSp = NULL;
	CSpecPoint* pSb = NULL;
	CSpecPoint* pSc = NULL;

	switch (nMethod % 4) {
	case 0:
		break;
	case 1:
		if (pOSpec->N == BackGdSpectrum.N) {
			pOSpec->Smooth(nSmooth);
			pSp = pOSpec->pPt;
			pSb = BackGdSpectrum.pPt;
			for (i = 0; i < pOSpec->N; i++) {
				pSp->Y1 -= pSb->Y1;
				if (pSp->Y1 < 0) {
					pSp->Y1 = 0;
				}
				pSp++;
				pSb++;
			}
		}
		break;
	case 2:
		if (pOSpec->N == CalibrSpectrum.N) {
			pOSpec->Smooth(nSmooth);
			pSp = pOSpec->pPt;
			pSc = CalibrSpectrum.pPt;
			for (i = 0; i < pOSpec->N; i++) {
				if (pSc->Y1 > 0) {
					pSp->Y1 = fCalCoeff * (pSp->Y1 / pSc->Y1);
				}
				else {
					pSp->Y1 = 0;
				}
				pSp++;
				pSc++;
			}
		}
		break;
	case 3:
		if (pOSpec->N == BackGdSpectrum.N) {
			if (pOSpec->N == CalibrSpectrum.N) {
				pSp = pOSpec->pPt;
				pSb = BackGdSpectrum.pPt;
				pSc = CalibrSpectrum.pPt;
				for (i = 0; i < pOSpec->N; i++) {
					dDataSpectrum = pSp->Y1 - pSb->Y1;
					if (dDataSpectrum < 0) {
						dDataSpectrum = 0;
					}
					dCalibrationSpectrum = pSc->Y1 - pSb->Y1;   // This value can be pre-calculated [7/20/2011 XPMUser]
					if (dCalibrationSpectrum < 0) {
						dCalibrationSpectrum = 0;
					}
					if (dCalibrationSpectrum == 0) {
						pSp->Y1 = 0;
					}
					else {
						pSp->Y1 = fCalCoeff * dDataSpectrum / dCalibrationSpectrum;
					}
					pSp++;
					pSb++;
					pSc++;
				}
			}
		}
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	return TRUE;
}