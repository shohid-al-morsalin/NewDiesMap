#pragma once

#include "PixelType.h"

namespace IMGL
{
	// float pixel image implementation [3/7/2015 Yuen]
	template<class T>
	class CImage16L
	{
		T *ptBits;
		int Width, Height;

	public:
		PIXELTYPE ptype;
		T tMax, tMin;

	public:
		BOOL IsNull();
		void Preset();
		BOOL IsSameSize(CRect rc);
		BOOL IsSameSize(int wd, int ht);
		BOOL IsSameSize(CImage16L<T> &DiB);

		int GetWidth();
		int GetHeight();
		short GetBYPP();
		BOOL Create(int wd, int ht);
		void Destroy();
		void GetMaxMin(T &min, T &max);

		T *GetPixel(int x, int y);
		T *GetBitsStart();
		T *GetPixelAddress(int x, int y);
		// For speed consideration, ptBits assumed to be valid and x and y within image range [4/2/2015 VIT#1]
		BOOL SetPixel(int x, int y, T v);

		// Dump one horizontal line [11/9/2014 Yuen]
		void Dump(int y);
		// Save simple float pixel bitmap [3/13/2015 VIT]
		BOOL Save(CString fname);
		// Load simple float pixel bitmap [3/13/2015 VIT]
		BOOL Load(CString fname);
		BOOL GetScore(T max, int &c1, int &c2);

		//////////////////////////////////////////////////////////////////////////
		CImage16L();
		CImage16L(int wd, int ht);
		~CImage16L();
		CImage16L<T> &operator=(CImage16L<T> &Img);
		virtual void Serialize(CArchive& ar);
	};
}
