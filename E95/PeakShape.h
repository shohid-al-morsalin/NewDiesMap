// PeakShape.h: interface for the CPeakShape class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CPeakShape {
public:
	BOOL bUse;
	short Thres;
	short Level;
	short Method;  // Peak location method, YMaxima, CM, Middle, ... [1/8/2011 FSMT]
	float Width;
	short Near, Smooth;
	short Minimum;
	short Height;	// Peak height
	float Distance;	// Peak distance from reference [11/6/2011 Administrator]
	float DeltaDistance; // Distance += DeltaDistance [11/6/2011 Administrator]

public:
	void SetDistance(float dist, float Delta);
	void VerboseHeaderOut(FILE* fp);
	void VerboseOut(FILE* fp, int n);
	void Clear();
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();
	void Set(short Thr, short Lev, float Wd, short Nr, short Sm, short Mthod, short Min, BOOL bUse, short Height);

	void Serialize(CArchive& ar);
	CPeakShape();
	virtual ~CPeakShape();
};
