#pragma once

#include "ImgRgn.h"
#include "HoughLine.h"
#include "ImageDraw.h"

#define INTY(x) ((*(x) + 2 * (*(x+1)) + *(x+2))/4)

namespace IMGL
{
	// This class perform calculation of properties of the image and return a numerical value describing that property  [4/4/2015 VIT#1]
	// No parallelization [4/4/2015 VIT#1]
	class CImageMeasure : public CImageDraw
	{
		template<typename T> short AverageScore();
		//template<typename T> int IntensityScore();
		template<typename T> float EntropyScore();
		template<typename T> float EntropyScore1();
		template<typename T> double StdevScore();
		template<typename T> double ContrastScore();
		template<typename T> int ThresScore(BYTE Thres);
		template<typename T> int PercentileScore(float lim);
		template<typename T> float VarianceScore(float mean);
		template<typename T> float VarianceScore1(float mean);

	protected:
		template<typename T> float CalcMean();

	public:
		// Measurement functions [7/3/2016 Yuen]
		short GetAverageScore();
		int GetThreshScore(BYTE Thres);
		int GetPercentileScore(float lim);
		//int GetIntensityScore();
		float GetEntropyScore();
		double GetStdevScore();
		double GetContrastScore();
		float GetVarianceScore();
		float CalcMean();

	public:
		virtual ~CImageMeasure();
	};
}
