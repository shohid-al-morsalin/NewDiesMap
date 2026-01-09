#pragma once

#include "ImageL.h"
#include "PLT\HSL2RGB.h"
#include "BlobCount.h"
#include "CamImgPar.h"

namespace IMGL
{
	// Semi wrapper class
	// Include UI/Display elements while CImageL encapsulate pure image property only [3/16/2015 VIT]
	class CPackedDIB : public CImageL
	{
		template<typename T> T *FillBackGrnd(T *p, int &n);
		template<typename T> T *SkipBackGrnd(T *p, int &n);
		template<typename T> T *SkipForeGrnd(T *p, int &n);
		template<typename T> void FillHoleLoop();
		template<typename T> void FindBlob();
		template<typename T> void FindLines(CHoughLine &H);
		template<typename T> void FindCircles(CHoughLine &H, int radius);
		template<typename T> void AddAve(CPackedDIB &DiB, BYTE BG);
		template<typename T> void HisAdd(int HisR[], int HisG[], int HisB[]);
		template<typename T> void HisEqualize(int HisR[], int HisG[], int HisB[]);
		template<typename T> void HisCompute(int HisR[], int HisG[], int HisB[]);

		template<typename T> void MaxMinI(int &Max, int &Min);
		template<typename T> void ContrastStretch(int Max, int Min, float TMax, float TMin);

		template<typename T> void MaxMinL(CHslRgb &H, float &Max, float &Min, float *pL, float *pH, float *pS);
		template<typename T> void ContrastColor(CHslRgb &H, float &Max, float &Min, float *pL, float *pH, float *pS);

		template<typename T> float AverageHue();
		template<typename T> float AverageSat();
		template<typename T> void EdgeDetect();
		template<typename T> void ContrastHistoBW();
		template<typename T> void ContrastHisto3LevelSet(int histo[]);
		template<typename T> void ContrastHisto3LevelCount(int histo[]);

	public:
		float H[10];
		float dI;  // Intensity difference [11/26/2016 FSM]
		int Ocx, Ocy; // Overlay center position of DiB with respect to its display rect [3/16/2015 VIT]

	public:
		void PreProcess();
		void ClearDIB();

	public:
		void TestVector();
		void FillHole();
		BOOL Binarize();
		float AverageSat();
		float AverageHue();
		BOOL EdgeDetect();
		void HistogramEqualize();
		void ContrastHistoBW();
		void ContrastHisto3Level();
		void ContrastColor();
		void ContrastStretchColor(float TMax, float TMin);

	public:
		void AddAve(CPackedDIB &DiB, short BG = 0);

		// Image analysis functions [7/3/2016 Yuen]
		//////////////////////////////////////////////////////////////////////////
		// Hough transformation on straight line [7/3/2016 Yuen]
		void FindLines(CHoughLine &H);
		// Hough transformation on a circle [7/3/2016 Yuen]
		void FindCircles(CHoughLine &H, int radius);
		void FindBlob();
		BOOL IsBumpArea(short radius, PAR::CCamImgPar *pPar);
		BOOL IsBackground2(float lim);
		//BOOL IsBumpArea(short radius, float SizCenter, BOOL bExclude, float BrightLimit, float DarkLimit);
		BOOL IsBackground(float lim);

	public:
		CPackedDIB();
		virtual ~CPackedDIB();
	};
}