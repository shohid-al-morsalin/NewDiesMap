#pragma once

#include <mmsystem.h>

class CDIB : public CObject {
	DECLARE_SERIAL(CDIB)
public:
	CDIB();
	virtual ~CDIB();

	BITMAPINFO* GetBitmapInfoAddress() {
		return m_pBMI;
	}                        // ptr to bitmap info
	void* GetBitsAddress() {
		return m_pBits;
	}                       // ptr to the bits
	RGBQUAD* GetClrTabAddress() {
		return (LPRGBQUAD)(((BYTE*)(m_pBMI))
			+ sizeof(BITMAPINFOHEADER));
	}       // ptr to color table
	int GetNumClrEntries();                     // number of color table entries
	BOOL Create(int width, int height, CPalette* pPal = NULL);         // create a new DIB
	BOOL Create(int width, int height, HPALETTE hPal);         // create a new DIB
	BOOL Create(BITMAPINFO* pBMI, BYTE* pBits, BOOL bOwnBits = FALSE); // create from existing mem,
	BOOL Create(BITMAPINFO* pPackedDIB); // create from packed DIB
	void* GetPixelAddress(int x, int y);
	int GetBitsPerPixel() {
		return m_pBMI->bmiHeader.biBitCount;
	}
	virtual BOOL Load(const char* pszFileName = NULL);// load DIB from disk file
	virtual BOOL Load(CFile* fp);               // load from file
	virtual BOOL Load(WORD wResid);             // load DIB from resource
	virtual BOOL Save(const char* pszFileName = NULL);// save DIB to disk file
	virtual BOOL Save(CFile* fp);               // save to file
	virtual void Serialize(CArchive& ar);
	virtual void Draw(CDC* pDC, int x, int y);
	virtual void Draw(HDC hDC, int x, int y);
	virtual int GetWidth() {
		return DibWidth();
	}   // image width
	virtual int GetStorageWidth() {
		return StorageWidth();
	}   // memory width
	virtual int GetHeight() {
		return DibHeight();
	} // image height
	virtual BOOL MapColorsToPalette(CPalette* pPal);
	virtual BOOL MapColorsToPalette(HPALETTE hPal);
	virtual void GetRect(RECT* pRect);
	virtual void CopyBits(CDIB* pDIB,
		int xd, int yd,
		int w, int h,
		int xs, int ys,
		COLORREF clrTrans = 0xFFFFFFFF);
	virtual void StretchBits(CDIB* pDIB,
		int xd, int yd,
		int wd, int hd,
		int xs, int ys,
		int ws, int hs,
		COLORREF clrTrans = 0xFFFFFFFF);
	virtual void DoubleBits(CDIB* pDIB,
		int xd, int yd,
		int xs, int ys,
		int ws, int hs,
		COLORREF clrTrans = 0xFFFFFFFF);
	BOOL SetColorTable(CPalette* pPal);

	static BOOL IsWinDIB(BITMAPINFOHEADER* pBIH);
	static int NumDIBColorEntries(BITMAPINFO* pBmpInfo);

public:
	BITMAPINFO* m_pBMI;         // pointer to BITMAPINFO struct
	BYTE* m_pBits;              // pointer to the bits
	BOOL m_bMyBits;             // TRUE if DIB owns Bits memory

public:
	int DibHeight() {
		return m_pBMI->bmiHeader.biHeight;
	}
	int DibWidth() {
		return m_pBMI->bmiHeader.biWidth;
	}
	int StorageWidth() {
		return (((m_pBMI->bmiHeader.biWidth * m_pBMI->bmiHeader.biBitCount)
			+ 31) & ~31) >> 3;
	}
};

class CDIBPal : public CPalette {
public:
	CDIBPal();
	~CDIBPal();
	BOOL Create(CDIB* pDIB);            // create from a DIB
	BOOL Create(BITMAPINFO* pBMI);      // create from color table
	BOOL Create(RGBQUAD* pRGB, int iClrs); // create from clr table
	void Draw(CDC* pDC, CRect* pRect, BOOL bBkgnd = FALSE);
	void Draw(HDC hDC, RECT* pRect, BOOL bBkgnd = FALSE);
	int GetNumColors();                 // get the no. of colors in the pal.
	BOOL SetSysPalColors();
	//BOOL Load(const char* pszFileName = NULL);
	//BOOL Load(CFile* fp);
	//BOOL Load(UINT hFile);
	//BOOL Load(HMMIO hmmio);
	//BOOL Save(const char* pszFileName = NULL);
	//BOOL Save(CFile* fp);
	//BOOL Save(UINT hFile);
	//BOOL Save(HMMIO hmmio);
	BOOL CreateWash();
	BOOL CreateSystemColorPalette();
};