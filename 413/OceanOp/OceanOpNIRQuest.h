// OceanOpNIRQuest.h: interface for the COceanOpNIRQuest class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#define HIGHRESTIMESTAMP_H
#define SPECTROMETERCHANNEL_H

#include "OceanBase.h"

extern "C"
{
#include "OOIDRV32.H"
}

#include <ArrayTypes.h> // located in %OMNIDRIVER_HOME%\include
#include <Wrapper.h>

#define BEGSPECTPIX		0//667
#define NUMBEROFPIX		512 //263

class COceanOpNIRQuest : public COceanBase {
public:

	COceanOpNIRQuest();
	virtual ~COceanOpNIRQuest();

public:
	BOOL Initialize(CListBox* pMessage);
	SCANPARM sp;
	OOI_PARAM ooip;

	DoubleArray spectrumArray;	// pixel values from the CCD elements
	DoubleArray wavelengthArray;// wavelengths (in nanometers) corresponding to each CCD element
	Wrapper wrapper;			// this is the object through which we will access all of Omnidriver's capabilities
	double* wavelengths;		// Sets a pointer to the values of the wavelength array
	double* spectrum;			// Sets a pointer to the values of the Spectrum array

	void SetParam(int Method, float fCoeff, int meastime, int Boxcar, float TFScalingFactor);
	int GetSpectrum(CSpec* Spec, short Nwave);
	int StartTF(CMeParam* p, CString dirc, short SpecNo);
};
