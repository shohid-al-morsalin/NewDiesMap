#pragma once

#include "RGB.h"
#include "HSV.h"

namespace IMGL
{
	class CHSVRGB
	{
		CHSV hsv;
		CRGBf rgb;

		void CalcHSV(CHSV &hsv);
		void CalcRGB(CRGBf &rgb);

	public:
		void SetHSV(CHSV &h) { hsv = h; }
		void SetHSV(float h, float s, float v);

		void SetRGB(CRGBf &r) { rgb = r; }
		void SetRGB(float r, float g, float b);
		void SetBGR3(BYTE *pBGR);

		CRGBf GetRGB();

		double GetH();
		double GetV();
		double GetS();
		CHSV GetHSV();
		void GetHSV(double &H, double &S, double &V);

		CHSVRGB();
		~CHSVRGB();
	};
}
