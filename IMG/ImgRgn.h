#pragma once

namespace IMGL
{
	class CImgRgn
	{
	public:
		short dI;
		BOOL bMono;  // Monochrome image of RGB buffer [4/5/2015 VIT#1]
		float scale; // Search area [4/5/2015 VIT#1]
		int sx, sy, ex, ey;
		CImgRgn();
		short GetSkip(short bpp);
		// Set image search region [4/5/2015 VIT#1]
		// Scale range from 0 to 1, centered at image center [4/5/2015 VIT#1]
		void SetSearchRgn(int wd, int ht);
	};
}
