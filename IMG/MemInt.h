#pragma once
#include "Score.h"
#include "TopTen.h"

namespace IMGL
{
	// 1D implementation of 2D buffer [7/4/2016 Yuen]
	class CMemInt
	{
		int His[256], HisSum, HisMax, HisMaxIndex, HisMidIndex;
		CTopTen TopTen;

	public:
		int *pIntImg;
		int siz;
		int wd, ht; // Width and height of this 2D buffer [7/4/2016 Yuen]
	public:
		int cx, cy;
		int counts;

	private:
		int Min, Max;
		int Percentile(float pc);

	public:
		CMemInt();
		~CMemInt();

	public:
		void Clear();
		void ClearTopTen();
		BOOL AllocMem(int sz);
		void Dump(CString fname);
		void DumpHisto(CString fname);

	public:
		void Histo();
		void Merge(short dist);
		void Sort();
		CScore *GetCScore(int n);
		int GetHistoSumMax();
		int GetCumMidIdex(int mid);
		int GetCumDistance();
		int GetCumLevel(int cnt);
		int GetArea(int x, int y, int r);

	public:
		void Average3x3();
		void Normalize256(float cf);
		void MaxV();
		void MinMaxV();
		void FindBestPos(int r);

	public:
		void SetDimension(int w, int h);
		void SetValue(int v, int x, int y);
		void Increment(int x, int y);
	};
}