// MaxMin.h: interface for the CMaxMin class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CMaxMin {
public:
	void SetMM(int m);
	void SetMM(float m);
	void SetMin(int& m);
	void SetMax(int& m);
	void SetMin(float& m);
	void SetMax(float& m);
	void Set(int m);
	void Set(float m);
	int iMax;
	int iMin;
	float fMax;
	float fMin;
	BOOL bFirst;
	CMaxMin();
	virtual ~CMaxMin();
};