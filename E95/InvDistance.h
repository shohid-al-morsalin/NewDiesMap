// InvDistance.h: interface for the CInvDistance class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CCoor;

class CInvDistance {
	short Last;		// last data point [7/17/2013 Yuen]
	short Total;	// Total allocated space [7/17/2013 Yuen]
	float* wt;
	float pw;
	CCoor** pCo;	// known data point [7/17/2013 Yuen]
public:
	void Clear();
	void Alloc(short n);
	BOOL Add(CCoor* pCo);
	float GetZ(float x, float y);
	CInvDistance();
	virtual ~CInvDistance();
};
