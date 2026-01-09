#pragma once

class XYLoc {
public:
	float X;
	float Y;
	BOOL p;	// true if X,Y are valid [7/23/2013 Yuen]

public:

	XYLoc() {
		X = Y = 0;
		p = FALSE;
	};
	float GetX() {
		return X;
	}
	float GetY() {
		return Y;
	}
	void Set(float pX, float pY) {
		X = pX; Y = pY;
		p = TRUE;
	};
	BOOL Get(float& pX, float& pY) {
		if (p) {
			pX = X; pY = Y;
		}
		return p;
	}
	void Clear() {
		p = FALSE;
		X = Y = 0;
	};
	BOOL IsSet() {
		return p;
	}
};

class CDiceLoc {
public:
	BOOL NeedOffset();
	void SetOffset(float X, float Y);
	BOOL GetCoor(float& X, float& Y);
	void ClearOffsets();
	void ClearAll();
	void Calc();

	XYLoc Pt[6];
	float XOff, YOff, Z1Off, Z2Off;
	float dX, dXy, dY, dYx;
	BOOL bNeedOffset;
	float x1r, y1r, x2r, y2r;

	CDiceLoc();
	virtual ~CDiceLoc();
};
