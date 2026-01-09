#pragma once

#include "atlimage.h"
#include "Image16L.h"
#include "PixelType.h"

namespace IMGL
{
	// This class modify read, retrieve and change the bit buffer [4/4/2015 VIT#1]
	// Low level function, speed priority, no sanity check [4/4/2015 VIT#1]
	// No parallelization ether
	class CImageIO : public CImage
	{
		template<typename T> void SetBuffer(int *pInt);
		template<typename T> void SetBuffer(float *pflt);
		template<typename T> void Copy(CImageIO &DiB, short scale);
	public:
		PIXELTYPE ptype;				// Pixel type: see defines above

	public:
		//////////////////////////////////////////////////////////////////////////
		// Image properties [4/4/2015 VIT#1]
		int GetBYPR();// Byte per row [3/7/2015 Yuen]
		int GetBYPP();// Byte per pixel [3/7/2015 Yuen]
		//////////////////////////////////////////////////////////////////////////
		// Query functions [7/10/2016 Yuen]
		BOOL IsSameSize(CImageIO &DiB);
		BOOL IsSize(int wd, int ht);
		void GetDimension(int &wd, int &ht);
		//////////////////////////////////////////////////////////////////////////
		// Image buffers [3/11/2015 Yuen]
		BYTE *GetBufferStart();
		unsigned long GetImageSize();
		unsigned long GetBufferSize();

		//////////////////////////////////////////////////////////////////////////
		// Creation and destruction [4/4/2015 VIT#1]
		BOOL Copy(CImageIO &DiB);
		BOOL Copy(CImageIO &DiB, short scale);
		BOOL LoadL(CString fname);
		BOOL CreateIf(int wd, int ht, int bpp);
		BOOL CreateIf(int wd, int ht, PIXELTYPE type);
		void Serialize(CArchive &ar); // Useful to serialize an array of images into a single file [4/4/2015 VIT#1]
		//////////////////////////////////////////////////////////////////////////
		// Single pixel operation [3/11/2015 Yuen]
		void GetPixel(int x, int y, BYTE &r, BYTE &g, BYTE &b);
		void SetPixel(int x, int y, BYTE v);
		void SetPixel(int x, int y, BYTE r, BYTE g, BYTE b);
		void SetPixelRed(int x, int y, BYTE v);
		void _SetAllPixel(BYTE r, BYTE g, BYTE b);

		short GetPixelInt(int x, int y);

		//////////////////////////////////////////////////////////////////////////
		// pInt value ranges from 0 to 255 [7/3/2016 Yuen]
		void SetBuffer(int *pInt);
		void SetBuffer(float *pflt);
	public:
		CImageIO();
		virtual ~CImageIO();
	};
}
