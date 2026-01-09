// OceanOp.h: interface for the COceanOp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCEANOP_H__DBA4D610_EDC8_4C79_8454_23F4ED788E2C__INCLUDED_)
#define AFX_OCEANOP_H__DBA4D610_EDC8_4C79_8454_23F4ED788E2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OceanBase.h"

#define BEGSPECTPIX		0//667
#define NUMBEROFPIX		512 //263


#define OVER_EXPOSED_TRYAGAIN	-8
#define OVER_EXPOSED_CRITICAL	-9

#define UNDER_EXPOSED_TRYAGAIN	-16
#define UNDER_EXPOSED_CRITICAL	-17

#define OOIFAILED				-32

#define POLYTOOTHIN				-64

class COceanOp : public COceanBase  
{
public:
	
	COceanOp();
	virtual ~COceanOp();

public:
	void SetThkRange2(float fThkMin, float fThkMax, int Method, float fCoeff, float RI,float lmin, float lmax, int meastime, int Boxcar, float TFScalingFactor);
	int GetSpectrum(float* Inten, float*Freq);
	int StartTF(CMeParam *p);
	int Result(double &PolyThickness, float **FFTArray, int &len);
	void Register(HWND wnd, long MessageID);
	void DoSingleScan();
	SCANPARM sp;
	OOI_PARAM ooip;


	int GetDataNumber(BOOL bFFT);
};


#endif // !defined(AFX_OCEANOP_H__DBA4D610_EDC8_4C79_8454_23F4ED788E2C__INCLUDED_)
