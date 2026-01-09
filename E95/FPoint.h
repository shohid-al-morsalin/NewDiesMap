// FPoint.h: interface for the CFPoint class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CFPoint {
public:
	union {
		float X;
		float Z1;
	};
	union {
		float Y;
		float Z2;
	};
public:
	void Clear();
	void Get(float& Cx, float& Cy);
	void Set(float Cx, float Cy);

	CFPoint();
	virtual ~CFPoint();
};
