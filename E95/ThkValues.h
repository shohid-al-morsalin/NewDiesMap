// ThkValues.h: interface for the CThkValues class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAXTHKVLU2	48

class CThkValues {
public:
	float Thk;
	float Dist;
	float TThk;
public:
	void Clear();
	CThkValues();
	virtual ~CThkValues();
};
