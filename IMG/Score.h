#pragma once

namespace IMGL
{
	class CScore
	{
	public:
		int pX, pY;
		int pV, pC, pR;
		int marked, total;

	public:
		void Clear();
		void SetR(int r);
		int Distance(int x, int y);
		void Set(int x, int y, int v);
		void IncCount(CScore * pScore);
		CScore();
		CScore &operator=(CScore &co)
		{
			pV = co.pV;
			pX = co.pX;
			pY = co.pY;
			pC = co.pC;
			pR = co.pR;

			total = co.total;
			marked = co.marked;

			return *this;
		}
	};
}
