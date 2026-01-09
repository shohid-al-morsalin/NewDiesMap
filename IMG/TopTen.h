#pragma once

#include "Score.h"

namespace IMGL
{
	class CScore;

	class CTopTen
	{
	private:
		std::vector<CScore> vScore;

	private:
		void PushUp(size_t n);
		void PushDown(size_t n);
		int partition(std::vector<CScore>& A, int p, int q);
		void quickSort(std::vector<CScore> &A, int p, int q);
	public:
		CTopTen();
		int GetValue(int i);
		int GetX(int i);
		int GetY(int i);
		CScore *GetCScore(UINT i);

		void Sort();

		void Merge(short dist);
		void Insert(int x, int y, int v, int r);
		void Clear();
	};
}
