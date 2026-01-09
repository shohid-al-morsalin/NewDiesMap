// PosHt.h: interface for the CPosHt class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CPosHtItem {
public:
	float Position;
	int Height;
	short dP, dH;

	CPosHtItem() {
		Position = Height = 0;
		dP = dH = 0;
	}
	void Set(float P, short ddP, int H, short ddH) {
		Position = P;
		Height = H;
		dH = ddH;
		dP = ddP;
	}
};

// Position and height of Echo peaks [10/25/2011 Administrator]
class CPosHt {
public:
	short n;
	CPosHtItem Pos[11];

	BOOL Add(float P, short dP, int H, short dH);
	void Clear();

	CPosHt();
	virtual ~CPosHt();
};
