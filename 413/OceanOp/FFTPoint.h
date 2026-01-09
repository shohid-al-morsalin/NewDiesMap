// FFTPoint.h: interface for the CFFTPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FFTPOINT_H__94DD5E7F_66E9_49BF_96E1_7DCEB3B01823__INCLUDED_)
#define AFX_FFTPOINT_H__94DD5E7F_66E9_49BF_96E1_7DCEB3B01823__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFFTPoint  
{
public:
	double V;		// FFT vector [12/11/2011 Administrator]
	double P;		// Misc use [12/11/2011 Administrator]
	CFFTPoint();
	virtual ~CFFTPoint();
};

#endif // !defined(AFX_FFTPOINT_H__94DD5E7F_66E9_49BF_96E1_7DCEB3B01823__INCLUDED_)
