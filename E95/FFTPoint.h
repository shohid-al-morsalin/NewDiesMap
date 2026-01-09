// FFTPoint.h: interface for the CFFTPoint class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CFFTPoint {
public:
	// 	float X;		// wavelength or wavenumber depend on situation [1/2/2012 Administrator]
	// 	float Y1,Y2,Y3;	// Spectrum power [1/2/2012 Administrator]
	// 	float P;		// Intermediate usage [1/2/2012 Administrator]
	double V;		// FFT vector [12/11/2011 Administrator]
	double P;		// Misc use [12/11/2011 Administrator]
	CFFTPoint();
	virtual ~CFFTPoint();
	CFFTPoint& operator = (CFFTPoint& co);
};
