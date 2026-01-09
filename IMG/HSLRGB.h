#pragma once

#include "RGB.h"
#include "HSL.h"

namespace IMGL
{
	class CHSLRGB
	{
		CHSL hsl;
		CRGBf rgb;

	public:
		void SetHSL(CHSL h) { hsl = h; }
		void SetRGB(CRGBf r) { rgb = r; }
		void SetHSL(double h, double s, double l);
		void SetRGB(double r, double g, double b);

		void GetHSL(CHSL &hsl);
		void GetRGB(CRGBf &rgb);
		CHSL GetHSL();
		CRGBf GetRGB();

		CHSLRGB();
		~CHSLRGB();
	};
}
