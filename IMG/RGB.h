#pragma once

#define RCPNT  0.299
#define GCPNT  0.587
#define BCPNT  0.114

//Luminace = 0.3086 * Red + 0.6094 * Green + 0.0820 * Blue
//Luminace = 0.299 * Red + 0.587 * Green + 0.114 * Blue

namespace IMGL
{
	class CRGBf
	{
	public:
		double r, g, b;  // value range from 0 to 1 only [3/6/2015 VIT]

		CRGBf();
		~CRGBf();

		void Set(double _r, double _g, double _b);
		BYTE Intn();
		double fIntn();
		double GetR() { return r; }
		double GetG() { return g; }
		double GetB() { return b; }
		void Grey(BYTE v);
	};
}
