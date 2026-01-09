#pragma once

#include "dib.h"

#define WINGAPI WINAPI
typedef HDC(WINGAPI WinGCreateDCProc)(void);
typedef HBITMAP(WINGAPI WinGCreateBitmapProc)(HDC WinGDC,
	const BITMAPINFO* pHeader,
	void** ppBits);
typedef UINT(WINGAPI WinGSetDIBColorTableProc)(HDC WinGDC,
	UINT StartIndex,
	UINT NumberOfEntries,
	const RGBQUAD* pColors);
typedef BOOL(WINGAPI WinGBitBltProc)(HDC hdcDest,
	int nXOriginDest,
	int nYOriginDest,
	int nWidthDest,
	int nHeightDest,
	HDC hdcSrc,
	int nXOriginSrc,
	int nYOriginSrc);

typedef BOOL(WINGAPI WinGStretchBltProc)(HDC hdcDest,
	int nXOriginDest,
	int nYOriginDest,
	int nWidthDest,
	int nHeightDest,
	HDC hdcSrc,
	int nXOriginSrc,
	int nYOriginSrc,
	int nWidthSrc,
	int nHeightSrc);

class CDIBSurface : public CDIB {
	DECLARE_SERIAL(CDIBSurface)

	// Attributes
public:
	CDC* GetDC() {
		return &m_dcBuffer;
	}
	HDC GetHDC() {
		return (HDC)m_dcBuffer.GetSafeHdc();
	}

	// Operations
public:
	BOOL Create(int cx, int cy, CPalette* pPal = NULL);
	BOOL Create(int width, int height, HPALETTE hPal);
	void BitBlt(CDC* pDC, int xd, int yd, int w, int h,
		int xs, int ys);
	void BitBlt(HDC hDC, int xd, int yd, int w, int h,
		int xs, int ys);
	void StretchBlt(CDC* pDC, int xd, int yd, int wd, int hd,
		int xs, int ys, int ws, int hs);
	void Draw(CDC* pDC);
	void Draw(HDC hDC, int x, int y);
	void SetPalette(CPalette* pPal);
	void SetPalette(HPALETTE hPal);
	BOOL Load(const char* pszFileName = NULL); // load DIB from disk file
	BOOL MapColorsToPalette(CPalette* pPal);
	BOOL MapColorsToPalette(HPALETTE hPal);

protected:
#if 0
	CPalette* m_pPal;       // Palette for drawing
#else
	HPALETTE m_hPal;
#endif
	CDC m_dcBuffer;         // DC for GDI ops

private:
	static BOOL m_bUseCreateDIBSection;        // flag
	static BOOL m_bUseWinG;                    // flag
	static WinGCreateDCProc* m_pWinGCDCProc;   // ptr to WinGCreateDC()
	static WinGCreateBitmapProc* m_pWinGCBProc;// ptr to WinGCreateBitmap()
	static WinGSetDIBColorTableProc* m_pWinGSDCTProc;  // ptr to WinGSetDIBColorTable()
	static WinGBitBltProc* m_pWinGBBProc;      // ptr to WinGBitBlt()
	static WinGStretchBltProc* m_pWinGSBProc;  // ptr to WinGStretchBlt()
	HBITMAP m_hbmBuffer;        // buffer DDB
	HBITMAP m_hbmOld;           // used for storing old bitmap

	static BOOL CDIBSurface::TestPlatform();

public:
	void Draw(CDC* pDC, int x, int y, int cx, int cy);
	CDIBSurface();
	virtual ~CDIBSurface();
};
