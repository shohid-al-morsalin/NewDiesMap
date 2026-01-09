
#pragma once

namespace IMGLx
{
	struct CRGB3
	{
		CRGB3()
		{
			R = G = B = 0;
		}

		BYTE B, G, R;  // RGB color space [3/14/2011 XPMUser]
		BYTE I();
		BYTE MaxRGB();
		BYTE MinRGB();
		void Grey(BYTE v);
		void Threshold(BYTE lvl);
		void Set(BYTE r, BYTE g, BYTE b);
		void Dup(CRGB3 &rgb1);
		void Ave(CRGB3 *rgb1, short BG);

		void Add(int *histoB);
		void Add(int *histoR, int *histoG, int *histoB);
		void HistoEqualize(int *histoB);
		void HistoEqualize(int *histoR, int *histoG, int *histoB);
	};

// 	struct CRGBA : public CRGB3
// 	{
// 		BYTE A;
// 	};
}
