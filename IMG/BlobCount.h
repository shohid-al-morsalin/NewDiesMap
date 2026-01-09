#pragma once
#include <queue>
#include "ImgPixel.h"
#include "PackedDIB.h"

namespace IMGL
{
	class CPackedDIB;

	struct imgPoint
	{
		int x, y;
		short count;
		imgPoint()
		{
			count = 0;
			x = y = 0;
		}
		imgPoint(int xx, int yy, short n)
		{
			count = n;
			x = xx; y = yy;
		}
	};

	class CBlob
	{
		int wd, ht;
		CPackedDIB *pDiB;
		std::queue<imgPoint> que;

		void ClearQue();
		void Set(CPackedDIB *p);
		template<typename T> void push_in_queue(int x, int y, short counter);
		template<typename T> int Count();

	public:
		CBlob();
		~CBlob();
		int Count(CPackedDIB *p);
	};
}
