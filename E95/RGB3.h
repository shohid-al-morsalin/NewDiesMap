// RGB3.h: interface for the CRGB3 class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

struct SBGR {
	//public:
	SBGR() {}

	BYTE B = 0, G = 0, R = 0;  // RGB color space [3/14/2011 XPMUser]

	BYTE I();
	BYTE MaxRGB();
	BYTE MinRGB();
	BYTE Max(BYTE A, BYTE B) { return (A > B) ? A : B; }
	BYTE Min(BYTE A, BYTE B) { return (A < B) ? A : B; }
	void Grey(BYTE v);
	void Set(BYTE r, BYTE g, BYTE b);
	void Dup(SBGR& rgb1);
	void Ave(SBGR* rgb1, short BG);

public:
	void Add(int* histoR, int* histoG, int* histoB);
	void HistoEqualize(int* histoR, int* histoG, int* histoB);
};

struct SPont {
	short x = 0, y = 0;
	float s = 0, t = 0;
	SBGR v;
	float b = 0;
};

struct SLine {
	short n = 0;
	std::vector<SPont> Ln;
	float Score(short idx, std::vector<SPont>& L);
};