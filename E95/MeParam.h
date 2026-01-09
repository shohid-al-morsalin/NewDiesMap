// MeParam.h: interface for the CMeParam class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "MParam.h"
#include "PeakShape.h"

#define MAXPEAKCOUNT	10
#define MAXPKCHAN		20
#define MAXTPAVLU		3
#define MAXTHKVLU		6

struct TParam {
	// Thin Film Section [7/15/2010 C2C]
	float xlmin, xlmax;			// wavelength [4/29/2011 C2C]
	float ThkMin[MAXTHKVLU];	// Minimum search film thickness, Original # MAXPAVLU=3 [7/15/2010 C2C]
	float ThkMax[MAXTHKVLU];	// Maximum search film thickness [7/15/2010 C2C]
	float RI[MAXTHKVLU];		// Thin film refractive index [7/15/2010 C2C]
	short IntegTime;			// Integration time
	short Boxcar;
	short Method;

	TParam() {
		Clear();
	}
	void Clear() {
		for (int i = 0; i < MAXTHKVLU; i++) {
			ThkMax[i] = 0;
			ThkMin[i] = 0;
			RI[i] = 0;
		}
		IntegTime = 500;
		Boxcar = 1;
		Method = 2;
	}
};

class CMeParam {
public:
	int magic;
	BOOL bEqual[2];
	short NAve, NPeak[2];
	CPeakShape PSG[2];  // Global peak shape tuning parameters [1/8/2011 FSMT]
	CPeakShape PS1[MAXPEAKCOUNT]; // Top probe local shape tuning parameters [1/8/2011 FSMT]
	CPeakShape PS2[MAXPEAKCOUNT]; // Bottom probe local shape tuning parameters [1/8/2011 FSMT]

	// Echo section [3/24/2012 Administrator]
	MParam MPa[MaXPA];
	// Thin Film Section [7/15/2010 C2C]
	TParam Ta;

	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	void Clear();
	BOOL IsBowWarp();
	void VerboseOut(FILE* fp);
	short GetDataCol(short idx, short& start);

	CMeParam& operator=(CMeParam& co);
	void Serialize(CArchive& ar);
	CMeParam();
	virtual ~CMeParam();
};
