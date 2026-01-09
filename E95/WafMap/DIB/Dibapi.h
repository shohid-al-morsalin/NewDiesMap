#pragma once

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

/* Handle to a DIB */
DECLARE_HANDLE(HDIB);

/* DIB constants */
#define PALVERSION   0x300

/* DIB Macros*/

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)

// WIDTHBYTES performs DWORD-aligning of DIB scanlines.  The "bits"
// parameter is the bit count for the scanline (biWidth * biBitCount),
// and this macro returns the number of DWORD-aligned bytes needed
// to hold those bits.

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)

/* Function prototypes */
BOOL   PaintDIB(CDC*, LPRECT, HDIB, CPalette*);
BOOL   CreateDIBPalette(HDIB hDIB, CPalette* cPal);
LPSTR  FindDIBBits(LPSTR lpbi);
DWORD  DIBWidth(LPSTR lpDIB);
DWORD  DIBHeight(LPSTR lpDIB);
WORD   PaletteSize(LPSTR lpbi);
WORD   DIBNumColors(LPSTR lpbi);
HANDLE CopyHandle(HANDLE h);
BOOL   SaveDIB(HDIB hDib, CFile& file);
HDIB    ReadDIBFile(CFile& file);
DWORD PASCAL lread(int, VOID far*, DWORD);
HPALETTE  MakeDIBPalette(LPBITMAPINFOHEADER);
HANDLE  MakeIndexHeader(LPBITMAPINFOHEADER);
int InitDIB(HWND);
void     RealizeDIBPalette(HDC hDC, LPBITMAPINFO lpbmi);
HBITMAP  DIBToBitmap(HANDLE hDIB, HPALETTE hPal);
HANDLE   BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal);
void     InitBitmapInfoHeader(LPBITMAPINFOHEADER lpBmInfoHdr,
	DWORD dwWidth,
	DWORD dwHeight,
	int nBPP);
