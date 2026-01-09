#pragma once

#include "ImageIO.h"

namespace IMGL
{
	class CImageDraw :public CImageIO
	{
		template<typename T> void DrawCircle(int cx, int cy, int r);
		template<typename T> void DrawCircle(int cx, int cy, int r, int &marked, int &total, BOOL bWrite);
	public:
		void DrawCircle(int cx, int cy, int r);
		void DrawCircle(int cx, int cy, int r, int &marked, int &total, BOOL bWrite);
	};
}