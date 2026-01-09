// FFTData.h: interface for the CFFTData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FFTDATA_H__418047E5_2E4C_4767_8E8F_F409857D45D0__INCLUDED_)
#define AFX_FFTDATA_H__418047E5_2E4C_4767_8E8F_F409857D45D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FFTPoint.h"

// NFFTSP over declare for safety reason [6/28/2011 XPMUser]
//#define NFFTSP	16384

class CFFTData  
{
public:
	void Clear();
	void Alloc(int n);
	void ShrinkY2(int n);

	int nPoint;
	int jPmax,jPlow,jPhi;
	float nlmin, nlmax;
	CFFTPoint *pPt/*[NFFTSP]*/;

	CFFTData();
	virtual ~CFFTData();

};

#endif // !defined(AFX_FFTDATA_H__418047E5_2E4C_4767_8E8F_F409857D45D0__INCLUDED_)
