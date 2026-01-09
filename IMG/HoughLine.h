#pragma once
#include "MemInt.h"

namespace IMGL
{
	class CRadiusItem
	{
	public:
		int radius, intensity;

		CRadiusItem()
		{
			radius = 0;
			intensity = 0;
		}
	};
	class CHoughLine : public CMemInt
	{
		union
		{
			int nr;  int iht;
		};
		union
		{
			int na;  int iwd;
		};
		float redR, redA;
		int ast, aed, ainc; // Angles

	public:
		std::vector<CRadiusItem>Radii;

	public:
		CHoughLine();
		~CHoughLine();

		int GetWidth();
		int GetHeight();
		float GetAreaRatio(float prcs);
		BOOL ParseRadiusDB(const CString &fname);
		//BOOL AllocA(int wd, int ht);
		BOOL AllocLine(int wd, int ht);
		BOOL AllocCircle(int wd, int ht);
		//BOOL TransformA(int x, int y);
		BOOL TransformLine(int x, int y);
		BOOL TransformCircle(int cx, int cy, int r);
		BOOL TransformCircleCartesian(int x, int y, int r);
	};
}
