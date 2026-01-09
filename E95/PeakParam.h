// PeakParam.h: interface for the CPeakParam class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAXPKCONSTRAIN	8

class CPeakHeightItem {
public:
	short Index;
	short Height;

public:
	CPeakHeightItem() {
		Clear();
	}
	void Set(short i, short ht) {
		Index = i;
		Height = ht;
	}
	void Clear() {
		Index = Height = 0;
	}
};

class CPeakDistanceItem {
public:
	short Index;
	float Distance, Delta;

public:
	CPeakDistanceItem() {
		Clear();
	}
	void Set(short i, float dist, float dlt) {
		Index = i;
		Distance = dist;
		Delta = dlt;
	}
	void Clear() {
		Index = 0;
		Distance = Delta = 0;
	}
};

class CPeakParam {
public:
	void Clear();
	enum CMPMETHOD {
		cmEQUAL, cmGREATER
	};
	CMPMETHOD Method;
	short nPeakT, nPeakB;  // Number of peaks
	CPeakHeightItem HeightT[MAXPKCONSTRAIN];
	CPeakHeightItem HeightB[MAXPKCONSTRAIN];
	CPeakDistanceItem DistanceT[MAXPKCONSTRAIN];
	CPeakDistanceItem DistanceB[MAXPKCONSTRAIN];
	short rMethod; // Return value [6/29/2011 XPMUser]
	CPeakParam();
	virtual ~CPeakParam();
};
