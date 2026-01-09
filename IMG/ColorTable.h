#pragma once

#include "RGB.h"

namespace IMGL
{
	class CColorTable
	{
	public:
		short nL;	// number of entries in pLUT [11/26/2014 Yuen]
		short tL;	// temporary variable [11/26/2014 Yuen]
		CRGBf *pLUT;

	public:
		BOOL Alloc(short n)
		{
			Clear();
			pLUT = new CRGBf[n];
			if (pLUT)
			{
				nL = n;
				return TRUE;
			}
			return FALSE;
		}

		void Clear();
		void Create0(int n, double fR, double fG, double fB); // Black & white [11/9/2014 Yuen]
		void Create1(int n);
		void Create2(int n);
		void Create3(int n);
		void Create4(int n);

		void CycleLUT();

		// n = float number from 0 to nLUT [11/26/2014 Yuen]
		BYTE Get(float fn)
		{
			if (fn < 0 || fn >= nL)
			{
				ASSERT(0);
			}
			return pLUT[int(fn)].Intn();
		}

		// load p RGB triplet with LUT[n] [11/26/2014 Yuen]
		void Set(BYTE *p, int n)
		{
			*(p++) = BYTE(pLUT[n].GetR() * 255);
			*(p++) = BYTE(pLUT[n].GetG() * 255);
			*(p) = BYTE(pLUT[n].GetB() * 255);
		}

		CColorTable();
		~CColorTable();
	};
}
