// OceanOpNIRQuest.cpp: implementation of the COceanOpNIRQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Dos.h"
#include "OceanOpNIRQuest.h"
#include "OOIDrv32.h"
#include "math.h"
#include <complex>

//#include "FFT.h"

using namespace std;

#define PI 3.1415926535897932384626433832795

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COceanOpNIRQuest::COceanOpNIRQuest() {
	bSimu = FALSE;

	wavelengths = NULL;
	spectrum = NULL;
	numberOfSpectrometersAttached = wrapper.openAllSpectrometers();	// Gets an array of spectrometer objects
	if (numberOfSpectrometersAttached > 0) {
		wrapper.setStrobeEnable(0, 1);							// Enables the strobe on the first spectrometer
		wrapper.setAutoToggleStrobeLampEnable(0, 1);				// Enables the Auto Strobe lamp on the first spectrometer
	}

	lambdamax = 951.429f;	// Starting value, will change when real spectrum is captured [1/6/2012 Administrator]
	lambdamin = 1649.99f;	// Same above [1/6/2012 Administrator]
}

COceanOpNIRQuest::~COceanOpNIRQuest() {
	wrapper.setStrobeEnable(0, 0);							// Disables the strobe on the first spectrometer
	wrapper.setAutoToggleStrobeLampEnable(0, 0);				// Disables the Auto Strobe lamp on the first spectrometer
}

int COceanOpNIRQuest::StartTF(CMeParam* p, CString dirc, short SpecNo) {
	if (numberOfSpectrometersAttached < 1) {
		return 0;
	}

	wrapper.setIntegrationTime(0, nMeastime * 1000);	// Sets the integration time of the first spectrometer to 100ms

	spectrumArray = wrapper.getSpectrum(0);					// Retrieves the spectrum from the first spectrometer
	wavelengthArray = wrapper.getWavelengths(0);			// Retrieves the wavelengths of the first spectrometer

	int Nwave = spectrumArray.getLength();					// Sets numberOfPixels to the length of the spectrumArray
	if ((Nwave == 512) || (Nwave == 2048)) {
		NumDATAPT = Nwave;
		spectrum = spectrumArray.getDoubleValues();			// Sets a pointer to the values of the Spectrum array
		wavelengths = wavelengthArray.getDoubleValues();	// Sets a pointer to the values of the wavelength array
		//wrapper.setAutoToggleStrobeLampEnable(0, 0);		// Disables the Auto Strobe lamp on the first spectrometer
		GetSpectrum(&CurrSpectrum, Nwave);
	}
	else {
		ASSERT(0);
	}

	return Nwave;
}

int COceanOpNIRQuest::GetSpectrum(CSpec* Spec, short Nwave) {
	int i;
	Spec->Alloc(Nwave, 0);
	CSpecPoint* pSp = Spec->pPt;
	double* pS = spectrum;
	double* pW = wavelengths;
	for (i = 0; i < Nwave; i++) {
		pSp->Set1(*pW, *pS);
		pSp++;
		pS++;
		pW++;
	}
	return Nwave;
}

void COceanOpNIRQuest::SetParam(int Method, float fCoeff, int meastime, int Boxcar, float TFScalingFactor) {
	sp.dsf = meastime;	// Additional hardware specific [11/20/2012 Yuen]
	COceanBase::SetParam(Method, fCoeff, meastime, Boxcar, TFScalingFactor);
}

BOOL COceanOpNIRQuest::Initialize(CListBox* pMessage) {
	pMessage->InsertString(pMessage->GetCount(), "Thin Film system (NIR512) started OK");
	return TRUE;
}