// OceanOpOmni.h: interface for the COceanOpOmni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCEANOPOMNI_H__CBB430D9_E1FF_49BB_B24B_242B6F74CEAB__INCLUDED_)
#define AFX_OCEANOPOMNI_H__CBB430D9_E1FF_49BB_B24B_242B6F74CEAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OceanBase.h"
#include "ArrayTypes.h"										// located in %OMNIDRIVER_HOME%\include

class Wrapper;

class COceanOpOmni : public COceanBase  
{
	Wrapper *pWrapper;

public:
	void Close();
	int numberOfSpectrometersAttached;						// actually attached and talking to us
	int	numberOfPixels;										// number of CCD elements/pixels provided by the spectrometer
	DoubleArray spectrumArray;								// pixel values from the CCD elements
	DoubleArray wavelengthArray;							// wavelengths (in nanometers) corresponding to each CCD element

	int GenerateLamdaCalib();
	void GetSpectrumData(BOOL bFFT, float *DataArray1, float *DataArray2);
	BOOL Measure();
	int GetSpectrum(float *DataArray);
	int Start();
	COceanOpOmni();
	virtual ~COceanOpOmni();

};

#endif // !defined(AFX_OCEANOPOMNI_H__CBB430D9_E1FF_49BB_B24B_242B6F74CEAB__INCLUDED_)
