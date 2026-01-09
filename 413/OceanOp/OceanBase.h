// OceanBase.h: interface for the COceanBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
//#include "OOPARS.h"

class CMeParam;

#define DLL_OOI_BUSY	1
#define DLL_OOI_READY	0
#define DLL_OOI_ERROR	-1

#define NUMBEROFMAPPEDPOINTS	65536

#include "Spec.h"
#include "PolyParams.h"

struct TParam;

class COceanBase {
public:
	COceanBase();
	virtual ~COceanBase();

public:
	BOOL BackgroundTreadment(CSpec* pOSpec, short nSmooth);
	void ClearPolyThk(CSpec* pSpec);
	void SetActivatePoly(CSpec* pSpec, short n, float fMin, float fMax, float fRI);
	void PreparefThk(CSpec* pSpec, short n);
	virtual BOOL Initialize(CListBox* pMessage);
	BOOL LoadSpectrum(CSpec* pSpec, CString name);

	short nMethod;
	int boxcar, nMeastime, nRepeat;
	float fCalCoeff, fThkFactor;
	float lambdamax;	// Starting value, will change when real spectrum is captured [1/6/2012 Administrator]
	float lambdamin;	// Same above [1/6/2012 Administrator]

	HWND hWnd2;
	HINSTANCE hInstance;

	int jPmax;
	CString FileDarkSpec, FileRefSpec;
	int StatusFlag;
	int ExposureTimes;
	BOOL bSimu;
	BOOL Registered;
	int nFFTSP, nFFTFac;

	short Counter;	// For simulation onlu [11/29/2012 Yuen]
	int numberOfSpectrometersAttached;

	CSpec CurrSpectrum;
	CSpec CalibrSpectrum;
	CSpec BackGdSpectrum;
	CSpec ResultSpectrum;

	SYSTEMTIME AuxilaryStartTime;
	SYSTEMTIME AuxilaryStopTime;

	float CosineInterpolate(float y1, float y2, float mu);
	void CalcMaxMin(float* val, int len, float& Vmax, float& Vmin);
	void ShowFreqSpec(CListBox& m_cList);
	void Close();
	int ModifySpectrum(CSpec* pSpec, CSpec::SPFILTER filter, int nCycle, /*short NSmooth,*/ short nExpand);
	long Getms(SYSTEMTIME systime);
	void CalibrationReset(short Nwave);

	virtual void Wait();
	virtual void SetParam(int Method, float fCoeff, int meastime, int Boxcar, float TFScalingFactor);
	BOOL GetReference(short nSmooth);
	BOOL GetBackground(short nSmooth);
	virtual int StartTF(CMeParam* p, CString dirc, short SpecNo);
	BOOL Result(CSpec* pISpec, CSpec* pOSpec, CSpec::SPFILTER filter, int nCycle, int NumMappedPoints, short NSmooth, short nExpand);
	BOOL ResultB(CSpec* pISpec, CSpec* pOSpec, CSpec::SPFILTER filter, int nCycle, int NumMappedPoints, short NSmooth, short nExpand);
	BOOL Result2(CSpec* pISpec, CSpec* pOSpec, CSpec::SPFILTER filter, int nCycle, int NumMappedPoints, short NSmooth, short nExpand);
	float GetPolyThickness(CMeParam* pMe, float fMin, float fMax, float fRI, CSpec::SPFILTER filter, int nCycle, int numMappedPoints, short NSmooth, short nExpand, BOOL bCapture = TRUE);
	CPolyParams* GetPolyThicknessB(TParam* pTa, CSpec::SPFILTER filter, int nCycle, int numMappedPoints, short NSmooth, short nExpand, float TFScalingFactor, BOOL bCapture = TRUE);
};

extern short NumDATAPT;
