// OceanOpOmni.cpp: implementation of the COceanOpOmni class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OceanOpOmni.h"

#include "Wrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COceanOpOmni::COceanOpOmni()
{
	pWrapper = new Wrapper;

	numberOfSpectrometersAttached = pWrapper->openAllSpectrometers();	// Gets an array of spectrometer objects
	if (numberOfSpectrometersAttached < 0)
	{
		// error occurs [11/4/2010 XPMUser]
		pWrapper->getLastException();
	}
	lambdamax = 951.429f;	// Starting value, will change when real spectrum is captured [1/6/2012 Administrator]
	lambdamin = 1649.99f;	// Same above [1/6/2012 Administrator]
}

COceanOpOmni::~COceanOpOmni()
{
}

int COceanOpOmni::Start()
{
	if (numberOfSpectrometersAttached == 0)
	{
		return 0;
	}

	pWrapper->setExternalTriggerMode(0,0);
	pWrapper->setIntegrationTime(0,nMeastime);
	pWrapper->setBoxcarWidth(0,boxcar);
	pWrapper->setScansToAverage(0,nRepeat);
	pWrapper->setCorrectForDetectorNonlinearity(0,TRUE);
	pWrapper->setCorrectForElectricalDark(0,TRUE);

	spectrumArray = pWrapper->getSpectrum(0);					// Retrieves the spectrum from the first spectrometer
	wavelengthArray = pWrapper->getWavelengths(0);			    // Retrieves the wavelengths of the first spectrometer 
	numberOfPixels = spectrumArray.getLength();					// Sets numberOfPixels to the length of the spectrumArray 

	return 1;
}

int COceanOpOmni::GetSpectrum(float *DataArray)
{
	double *wavelengths = wavelengthArray.getDoubleValues();	// Sets a pointer to the values of the wavelength array 
	double *spectrum = spectrumArray.getDoubleValues();			// Sets a pointer to the values of the Spectrum array 
	numberOfPixels = spectrumArray.getLength();					// Sets numberOfPixels to the length of the spectrumArray 
	
	for (int i=0; i<numberOfPixels;i++)
	{
		DataArray[i]=spectrum[i];
	}
	return 1;
}

BOOL COceanOpOmni::Measure()
{
	int n=0;
tryagain:	
	if (!Start())
	{
		goto tryagain;
	}
	int ret;
	
	//ret = Result(PolyThickness);

	switch(ret)
	{
	case -8:
		if(n<=10)
		{
			n+=1;
			goto tryagain;
		}
	//	SetThickness(-8);
		break;
	case -16:
		if(n<=10)
		{
			n+=1;
			goto tryagain;
		}
	//	SetThickness(-16);
		break;
	case 0:
	//	SetThickness(PolyThickness);
		break;
	case -9:
	//	SetThickness(-9);
		break;
	case -17:
	//	SetThickness(-17);
		break;
	case -64:
	//	SetThickness(-64);
		break;
	default:
	//	SetThickness(0);
		break;
	}

	//SetThickness(PolyThickness);
	return TRUE;
}

void COceanOpOmni::GetSpectrumData(BOOL bFFT, float *DataArray1, float *DataArray2)
{
	float lambdatemp;
	int j=0;
	
	double *wavelengths = wavelengthArray.getDoubleValues();	// Sets a pointer to the values of the wavelength array 
	double *spectrum = spectrumArray.getDoubleValues();			// Sets a pointer to the values of the Spectrum array 

	if(!bFFT)
	{
		Coefficients Coeff;
		Coeff = pWrapper->getCalibrationCoefficientsFromEEProm(0);
		
		for(int i=numberOfPixels-1;i>=0;i--)
		{
			int k = (i+BEGSPECTPIX);
			lambdatemp = Coeff.getWlIntercept();
			lambdatemp += Coeff.getWlFirst() * k;
			lambdatemp += Coeff.getWlSecond() * k * k;
			lambdatemp += Coeff.getWlThird() * k * k * k;
			
			if(lambdatemp <= lambdamax && lambdatemp >= lambdamin)
			{
				DataArray1[j] = lambdatemp;
				switch(nMethod%4)
				{
				case 0:	
					DataArray2[j] = spectrum[i]; 
					break;
				case 1:	
					DataArray2[j] = spectrum[i] - DarkSpectrum[i]; 
					break;
				case 2:	
					DataArray2[j] = (fCalCoeff*spectrum[i]/RefSpectrum[i]); 
					break;
				case 3:	
					if (RefSpectrum[i] - DarkSpectrum[i] ==0)
					{
						DataArray2[j] = 0;
					}
					else
					{
						DataArray2[j] = (fCalCoeff*(spectrum[i] - DarkSpectrum[i])/(RefSpectrum[i] - DarkSpectrum[i])); 
					}
					break;
				default: 
					DataArray2[j] = spectrum[i]; 
					break;
				}	
				j++;
			}
		}
	}
	else
	{
		for (j=oopars.nFFTBeg;j<=oopars.nFFTEnd;j++)
		{
			DataArray1[j - oopars.nFFTBeg] = j / fThkFactor / oopars.m_RefrIndex / 2 
				/ 1e3 / (1.0/lambdamin - 1.0/lambdamax);
			DataArray2[j - oopars.nFFTBeg] = FFTData[j];
		}
	}
}

int COceanOpOmni::GenerateLamdaCalib()
{
	double lambdatemp;
	Coefficients Coeff;
	Coeff = pWrapper->getCalibrationCoefficientsFromEEProm(0);

	for (int i=0; i<numberOfPixels;i++)
	{
		int k = (i+BEGSPECTPIX);
		lambdatemp = Coeff.getWlIntercept();
		lambdatemp += Coeff.getWlFirst() * k;
		lambdatemp += Coeff.getWlSecond() * k * k;
		lambdatemp += Coeff.getWlThird() * k * k * k;
		LamdaCalib[i] = lambdatemp;
	}

	return 0;  // -32 - Failed, 0 is passed [11/4/2010 XPMUser]
}

void COceanOpOmni::Close()
{
	if (pWrapper)
	{
		pWrapper->closeAllSpectrometers();
		//delete pWrapper;
		pWrapper = NULL;
	}
}
