// PackedDIB1.h: interface for the CPackedDIB class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "sidefs.h"

#include "RGB3.h"

// Wrapper class for SIPackedDIB [6/6/2010 Yuen]
class CPackedDIB {
public:
	CCriticalSection CS;
	//////////////////////////////////////////////////////////////////////////////////
	// LIM
public:
	void FormatAngle(float& angle);
	void Polar2Cartesian(float radius, float ang, float& x, float& y);
	void Cartesian2Polar(float X, float Y, float& radius, float& ang);

public:
	void* GetPixelAddress(short x, short y);

public:
	bool IsGreen(short x, short y);
	BOOL SetLine(std::vector<SPont>& Ln);
	BOOL GetLine(std::vector<SPont>& Ln, float angle, short xx, short yy);
	BOOL PlotLine(std::vector<SPont>& Ln, short y, float amp);
	BOOL PlotScore(std::vector<SPont>& Ln, short y, float amp);
	BOOL Dump(const CString& name, std::vector<SPont>& Ln);
	BOOL Set(std::vector<SPont>& Ln, BYTE r, BYTE g, BYTE b);
	BOOL Convolve(std::vector<SPont>& Ln, std::vector<float>& K) {}
	BOOL Filter(std::vector<SPont>& Ln);

public:
	SIPackedDIB* pDIB = nullptr; // Actual BMP data [6/6/2010 Yuen]
	short OffX = 0, OffY = 0;  // Offset of image with respect to the canvas [6/6/2010 Yuen]
	short BytesPerPixel = 0;
	short BytesPerRow = 0;
	short IWidth = 0, IHeight = 0;
	float fTgtX = 0, fTgtY = 0;	// Match position [6/8/2014 Yuen]

public:
	BOOL Duplicate(CPackedDIB& DIB, short X = 0, short Y = 0);
	BOOL Duplicate(SIPackedDIB* DIB, short X = 0, short Y = 0);
	BOOL GetDimension(int& wd, int& ht);
	BYTE GetPixel(short x, short y);
	BYTE GetPixelL(short x, short y);
	BYTE GetSetPixelM1(short x, short y);
	BYTE GetSetPixelRGB(short x, short y, BYTE r, BYTE g, BYTE b);
	BYTE HsL(short R1, short G1, short B1);
	float AverageHue();
	float AverageSat();
	float GetIntensityCount();
	int GetHeight();
	int GetSize();
	int GetWidth();
	int InTensity(BYTE* p);
	long Binary(char** msg);
	long Different(CPackedDIB& DIB2);
	long GetLength();
	short CalcBytePerRow();
	short SobelH(BYTE* pS1, BYTE* pS2, BYTE* pS3, short BPP);
	short SobelV(BYTE* pS1, BYTE* pS2, BYTE* pS3, short BPP);
	SIPackedDIB* Create(short Width, short Height, short bitCount);
	SIPackedDIB* CreateNew(short Width, short Height, short bitCount);
	SIPackedDIB* GetDIB();
	void AddAve(SIPackedDIB* pD, short BG, short X = 0, short Y = 0);
	void Binarize(short lv);
	void ClearDIB();
	void Contrast();
	void Contrast3Level();
	void ContrastBW();
	void ContrastColor();
	void ContrastStretchColor(float TMax, float TMin);
	void Conv(char** msg);
	void CreateBlank();
	void Gaussian();
	void GreyScale();
	void PreProcess();
	void Replace(SIPackedDIB* pD, short X = 0, short Y = 0);
	void Serialize(CArchive& ar);
	void SetIntensity(BYTE* p, BYTE R, BYTE G, BYTE B);
	void SetPixel(short x, short y, BYTE v);
	void SetPixelRed(short x, short y, BOOL v);
	void SetPixelRGB(short x, short y, BYTE r, BYTE g, BYTE b);
	void Sobel();

	BOOL Load(CString filename);
	BOOL Save(CString filename);


	// [08202024 ZHIMING
	void BThres(short lvl) {// RGB thresholding [11/3/2019 FSM]
		if (!pDIB || !pDIB->pDIBBits) {
			return;
		}
		int wd, ht, bpp = BytesPerPixel;
		GetDimension(wd, ht);
#pragma omp parallel for
		for (int y = 0; y < ht; y++) {
			short v;
			BYTE* des = (BYTE*)GetPixelAddress(0, y);
			for (int x = 0; x < wd; x++) {
				switch (bpp) {
				case 1:
					v = *(des)-lvl;
					*(des++) = (v >= 0) ? 255 : 0;
					break;
				case 3:
					for (int i = 0; i < 3; i++) {
						v = *(des)-lvl;
						*(des++) = (v >= 0) ? 255 : 0;
					}
					break;
				case 4:
					for (int i = 0; i < 3; i++) {
						v = *(des)-lvl;
						*(des++) = (v >= 0) ? 255 : 0;
					}
					des++;
					break;
				}
			}
		}
	}

	// ]

	CPackedDIB();
	virtual ~CPackedDIB();
};
