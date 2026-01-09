// SpecPoint.h: interface for the CSpecPoint class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CSpecPoint {
public:
	void Set1(float x, float y);
	float X1;
	float Y1;
	float Y2;	// Indicate peak locations [12/13/2011 Administrator]
	CSpecPoint();
	virtual ~CSpecPoint();
};