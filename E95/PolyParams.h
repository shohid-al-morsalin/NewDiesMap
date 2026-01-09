// PolyParams.h: interface for the CPolyParams class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CPolyParams {
public:
	float fThk;	// Calculated thickness returned here, set to -1 to indicate value required during calculation [11/20/2012 Yuen]
	float fMin, fMax, fRI;
	float GOF, WHH, PolyL, PolyM, PolyH, PolyR;
	int jPmax, jPlow, jPhi, jLo, jHi;

public:
	void MarkfThk();
	void ClearfThk();
	void Clear();
	CPolyParams();
	virtual ~CPolyParams();
};