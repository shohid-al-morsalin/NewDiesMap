#include "StdAfx.h"		    /* required for all Windows applications */

#include "dibapi.h"		    /* specific to this program		     */

#if 0
hDIBInfo = GlobalAlloc(GMEM_MOVEABLE,
	(DWORD)(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)));
#endif

HPALETTE  MakeDIBPalette(LPBITMAPINFOHEADER lpInfo) {
	NPLOGPALETTE npPal;
	RGBQUAD far* lpRGB;
	HPALETTE hLogPal;
	WORD i;

	/* since biClrUsed field was filled during the loading of the DIB,
	** we know it contains the number of colors in the color table.
	*/
	if (lpInfo->biClrUsed) {
		npPal = (NPLOGPALETTE)LocalAlloc(LMEM_FIXED, sizeof(LOGPALETTE) +
			(WORD)lpInfo->biClrUsed * sizeof(PALETTEENTRY));
		if (!npPal)
			return(FALSE);

		npPal->palVersion = 0x300;
		npPal->palNumEntries = (WORD)lpInfo->biClrUsed;

		/* get pointer to the color table */
		lpRGB = (RGBQUAD FAR*)((LPSTR)lpInfo + lpInfo->biSize);

		/* copy colors from the color table to the LogPalette structure */
		for (i = 0; i < (WORD)lpInfo->biClrUsed; i++, lpRGB++) {
			npPal->palPalEntry[i].peRed = lpRGB->rgbRed;
			npPal->palPalEntry[i].peGreen = lpRGB->rgbGreen;
			npPal->palPalEntry[i].peBlue = lpRGB->rgbBlue;
			npPal->palPalEntry[i].peFlags = 0;
		}

		hLogPal = CreatePalette((LPLOGPALETTE)npPal);
		LocalFree((HANDLE)npPal);
		return(hLogPal);
	}
	else
		return((HPALETTE)::GetStockObject(DEFAULT_PALETTE));
}

/****************************************************************************
 *									    *
 *  FUNCTION   : PaintDIB(hWnd)						    *
 *									    *
 *  PURPOSE    : Paint the currently loaded DIB using the options chosen
 *		 by the user.
 *									    *
 *  RETURNS    : None.
 *	HANDLE hDIBInfo;								    *
 ****************************************************************************/
BOOL  PaintDIB(CDC* pDC, LPRECT lprect, HDIB hDIB, CPalette* pPal) {
	BOOL     bSuccess = FALSE;      // Success/fail flag
	CPalette* pOldPal;
	LPBITMAPINFOHEADER lpInfo;
#if 0
	/* Check for valid DIB handle */
	if (hDIB == NULL)
		return FALSE;
	lpDIBHdr = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);
	lpDIBBits = ::FindDIBBits(lpDIBHdr);
	// Get the DIB's palette, then select it into DC
	if (pPal != NULL)
		pOldPal = pDC->SelectPalette(pPal, TRUE);
	pDC->SetStretchBltMode(COLORONCOLOR);
	if ((RECTWIDTH(lpDCRect) == RECTWIDTH(lpDIBRect)) &&
		(RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
		bSuccess = ::SetDIBitsToDevice(pDC->m_hDC,                    // hDC
			lpDCRect->left,             // DestX
			lpDCRect->top,              // DestY
			RECTWIDTH(lpDCRect),        // nDestWidth
			RECTHEIGHT(lpDCRect),       // nDestHeight
			lpDIBRect->left,            // SrcX
			(int)DIBHeight(lpDIBHdr) -
			lpDIBRect->top -
			RECTHEIGHT(lpDIBRect),   // SrcY
			0,                          // nStartScan
			(WORD)DIBHeight(lpDIBHdr),  // nNumScans
			lpDIBBits,                  // lpBits
			(LPBITMAPINFO)lpDIBHdr,     // lpBitsInfo
			DIB_RGB_COLORS);            // wUsage
	else
		bSuccess = ::StretchDIBits(pDC->m_hDC,                          // hDC
			lpDCRect->left,                 // DestX
			lpDCRect->top,                  // DestY
			RECTWIDTH(lpDCRect),            // nDestWidth
			RECTHEIGHT(lpDCRect),           // nDestHeight
			lpDIBRect->left,                // SrcX
			lpDIBRect->top,                 // SrcY
			RECTWIDTH(lpDIBRect),           // wSrcWidth
			RECTHEIGHT(lpDIBRect),          // wSrcHeight
			lpDIBBits,                      // lpBits
			(LPBITMAPINFO)lpDIBHdr,         // lpBitsInfo
			DIB_RGB_COLORS,                 // wUsage
			SRCCOPY);                       // dwROP

	::GlobalUnlock((HGLOBAL)hDIB);
#endif
	lpInfo = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
	if (!pPal) //lpInfo->biClrUsed
		pPal = CPalette::FromHandle(MakeDIBPalette(lpInfo));
	pOldPal = pDC->SelectPalette(pPal, FALSE);
	pDC->RealizePalette();
	WORD nNumColors, offBits;
	if (!(nNumColors = (WORD)lpInfo->biClrUsed)) {
		if (lpInfo->biBitCount != 24)
			nNumColors = 1 << lpInfo->biBitCount;	/* standard size table */
		else
			nNumColors = 16;
	}
	offBits = (WORD)lpInfo->biSize + nNumColors * sizeof(RGBQUAD);
	if (RECTWIDTH(lprect) == lpInfo->biWidth
		&& RECTHEIGHT(lprect) == lpInfo->biWidth)
		SetDIBitsToDevice(pDC->m_hDC, lprect->left, lprect->top, (WORD)lpInfo->biWidth,
			(WORD)lpInfo->biHeight, 0, 0, 0, (WORD)lpInfo->biHeight,
			(LPSTR)lpInfo + offBits, (LPBITMAPINFO)lpInfo, DIB_RGB_COLORS);
	else
		StretchDIBits(pDC->m_hDC, lprect->left, lprect->top, RECTWIDTH(lprect), RECTHEIGHT(lprect),
			0, 0, (WORD)lpInfo->biWidth, (WORD)lpInfo->biHeight,
			(LPSTR)lpInfo + offBits,
			(LPBITMAPINFO)lpInfo, DIB_RGB_COLORS, SRCCOPY);
	if (pOldPal != NULL)
		pDC->SelectPalette(pOldPal, TRUE);
	GlobalUnlock((HGLOBAL)hDIB);
	return bSuccess;
}

/****************************************************************************
 *									    *
 *  FUNCTION   : ReadDIB(hWnd)						    *
 *									    *
 *  PURPOSE    : Reads a DIB from a file, obtains a handle to it's          *
 *		 BITMAPINFO struct. and loads the DIB.                      *
 *									    *
 *  RETURNS    : TRUE  - DIB loads ok					    *
 *		 FALSE - otherwise					    *
 *									    *
 ****************************************************************************/
#if 0
int ReadDIB(HWND hWnd) {
	unsigned	       fh;
	LPBITMAPINFOHEADER lpbi;
	OFSTRUCT	       of;
	BITMAPFILEHEADER   bf;
	WORD		nNumColors;
	WORD		result = FALSE;		/* assume failure */

	/* Open the file and get a handle to it's BITMAPINFO */

	fh = OpenFile(achFileName, &of, OF_READ);
	if (fh == -1) {
		wsprintf(str, "Can't open file '%ls'", (LPSTR)achFileName);
		MessageBox(hWnd, str, "Error", MB_ICONSTOP | MB_OK);
		return (FALSE);
	}

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIBInfo);

	/* read the BITMAPFILEHEADER */
	if (sizeof(bf) != _lread(fh, (LPSTR)&bf, sizeof(bf)))
		goto ErrExit;

	if (bf.bfType != 0x4d42)	/* 'BM' */
		goto ErrExit;

	if (sizeof(BITMAPINFOHEADER) != _lread(fh, (LPSTR)lpbi, sizeof(BITMAPINFOHEADER)))
		goto ErrExit;

	/* !!!!! for now, don't even deal with CORE headers */
	if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
		goto ErrExit;

	if (!(nNumColors = (WORD)lpbi->biClrUsed)) {
		/* no color table for 24-bit, default size otherwise */
		if (lpbi->biBitCount != 24)
			nNumColors = 1 << lpbi->biBitCount;	/* standard size table */
	}

	/*	fill in some default values if they are zero */
	if (lpbi->biClrUsed == 0)
		lpbi->biClrUsed = (DWORD)nNumColors;

	if (lpbi->biSizeImage == 0) {
		lpbi->biSizeImage = ((((lpbi->biWidth * (DWORD)lpbi->biBitCount) + 31) & ~31) >> 3)
			* lpbi->biHeight;
	}

	/* get a proper-sized buffer for header, color table and bits */
	GlobalUnlock(hDIBInfo);
	hDIBInfo = GlobalReAlloc(hDIBInfo, lpbi->biSize +
		nNumColors * sizeof(RGBQUAD) +
		lpbi->biSizeImage, 0);
	if (!hDIBInfo)	/* can't resize buffer for loading */
		goto ErrExit2;

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIBInfo);

	/* read the color table */
	_lread(fh, (LPSTR)(lpbi)+lpbi->biSize, nNumColors * sizeof(RGBQUAD));

	/* offset to the bits from start of DIB header */
	offBits = (WORD)lpbi->biSize + nNumColors * sizeof(RGBQUAD);

	if (bf.bfOffBits != 0L) {
		_llseek(fh, bf.bfOffBits, SEEK_SET);
	}
	if (lpbi->biSizeImage == lread(fh, (LPSTR)lpbi + offBits, lpbi->biSizeImage))
		result = TRUE;

ErrExit:
	_lclose(fh);
	GlobalUnlock(hDIBInfo);
ErrExit2:
	return(result);
}

/**************** PRIVATE ROUTINE TO READ MORE THAN 64K *********************/
/****************************************************************************
 *									    *
 *  FUNCTION   : lread(int fh, VOID FAR *pv, DWORD ul)			    *
 *									    *
 *  PURPOSE    : Reads data in steps of 32k till all the data has been read.*
 *									    *
 *  RETURNS    : 0 - If read did not proceed correctly. 		    *
 *		 number of bytes read otherwise.			    *
 *									    *
 ****************************************************************************/
DWORD PASCAL lread(int fh, VOID far* pv, DWORD ul) {
	DWORD     ulT = ul;
	BYTE huge* hp = pv;

	while (ul > (DWORD)MAXREAD) {
		if (_lread(fh, (LPSTR)hp, (WORD)MAXREAD) != MAXREAD)
			return 0;
		ul -= MAXREAD;
		hp += MAXREAD;
	}
	if (_lread(fh, (LPSTR)hp, (WORD)ul) != (WORD)ul)
		return 0;
	return ulT;
}
/**************** PRIVATE ROUTINE TO READ MORE THAN 64K *********************/

/****************************************************************************
 *									    *
 *  FUNCTION   : InitDIB(hWnd)						    *
 *									    *
 *  PURPOSE    : cleans up old DIB info, reads new DIB from file,
 *		 resizes window for new DIB
 *									    *
 *  RETURNS    : TRUE  - DIB loads ok					    *
 *		 FALSE - otherwise					    *
 *									    *
 ****************************************************************************/
int InitDIB(HWND hWnd) {
	LPBITMAPINFOHEADER lpbi;
	RECT		Rectangle;

	/* if there was an old DIB, free it up */
	if (bDIBLoaded) {
		if (hDDBitmap) {
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
			DeleteObject(hDDBitmap);
			hDDBitmap = NULL;
		}
		bDIBLoaded = FALSE;
		if (hPalette) {
			DeleteObject(hPalette);
			hPalette = 0;
		}
		if (hPalHeader) {
			GlobalFree(hPalHeader);
			hPalHeader = NULL;
		}
	}

	/* load the DIB from the file */
	if (!ReadDIB(hWnd)) {
		MessageBox(hWnd, "Error attempting to read DIB", "Error", MB_ICONSTOP | MB_OK);
		return(FALSE);
	}

	bDIBLoaded = TRUE;		/* there is a DIB loaded now */

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIBInfo);
	SetWindowText(hWnd, achFileName);
	Rectangle.left = 0;
	Rectangle.top = 0;
	Rectangle.right = (WORD)lpbi->biWidth;
	Rectangle.bottom = (WORD)lpbi->biHeight;
	GlobalUnlock(hDIBInfo);

	/* Compute the size of the window rectangle based on the given
	 * client rectangle size and the window style, then size the
	 * window.  Do not deal with possibility of more than one menu line.
	 */
	AdjustWindowRect(&Rectangle, WS_OVERLAPPEDWINDOW, TRUE);
	SetWindowPos(hWnd, (HWND)NULL, 0, 0,
		Rectangle.right - Rectangle.left,
		Rectangle.bottom - Rectangle.top + 1,
		SWP_NOMOVE | SWP_NOZORDER);

	GetClientRect(hWnd, &Rectangle);

	return TRUE;
}

#endif
//-----------------------------------------------------------------------------
// DIB.C
//
// This is a collection of useful DIB manipulation/information gathering
// functions.  Many functions are supplied simply to take the burden
// of taking into account whether a DIB is a Win30 style or OS/2 style
// DIB away from the application.
//
// The functions in this module assume that the DIB pointers or handles
// passed to them point to a block of memory in one of two formats:
//
//       a) BITMAPINFOHEADER + color table + DIB bits (3.0 style DIB)
//       b) BITMAPCOREHEADER + color table + DIB bits (OS/2 PM style)
//
// The SDK Reference, Volume 2 describes these data structures.
//
// A number of functions in this module were lifted from SHOWDIB,
// and modified to handle OS/2 DIBs.
//
// The functions in this module could be streamlined (made faster and
// smaller) by removing the OS/2 DIB specific code, and assuming all
// DIBs passed to it are Win30 style DIBs.  The DIB file reading code
// would need to be modified to always convert DIBs to Win30 style
// DIBs.  The only reason this isn't done in DIBView is because DIBView
// was written to test display and printer drivers (which are supposed
// to support OS/2 DIBs wherever they support Win30 style DIBs).  SHOWDIB
// is a great example of how to go about doing this.
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------
//
// Function:   FindDIBBits
//
// Purpose:    Given a pointer to a DIB, returns a pointer to the
//             DIB's bitmap bits.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

LPSTR FindDIBBits(LPSTR lpbi) {
	return (lpbi + *(LPDWORD)lpbi + PaletteSize(lpbi));
}

//---------------------------------------------------------------------
//
// Function:   DIBNumColors
//
// Purpose:    Given a pointer to a DIB, returns a number of colors in
//             the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD DIBNumColors(LPSTR lpbi) {
	WORD wBitCount;

	// If this is a Windows style DIB, the number of colors in the
	//  color table can be less than the number of bits per pixel
	//  allows for (i.e. lpbi->biClrUsed can be set to some value).
	//  If this is the case, return the appropriate value.

	if (IS_WIN30_DIB(lpbi)) {
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;

		if (dwClrUsed)
			return (WORD)dwClrUsed;
	}

	// Calculate the number of colors in the color table based on
	//  the number of bits per pixel for the DIB.

	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	switch (wBitCount) {
	case 1:
		return 2;

	case 4:
		return 16;

	case 8:
		return 256;

	default:
		return 0;
	}
}

//---------------------------------------------------------------------
//
// Function:   PaletteSize
//
// Purpose:    Given a pointer to a DIB, returns number of bytes
//             in the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD PaletteSize(LPSTR lpbi) {
	if (IS_WIN30_DIB(lpbi))
		return (DIBNumColors(lpbi) * sizeof(RGBQUAD));
	else
		return (DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}

//---------------------------------------------------------------------
//
// Function:   CreateDIBPalette
//
// Purpose:    Given a handle to a DIB, constructs a logical palette,
//             and returns a handle to this palette.
//
//             Stolen almost verbatim from ShowDIB.
//
// Parms:      hDIB == HANDLE to global memory with a DIB header
//                     (either BITMAPINFOHEADER or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

HPALETTE CreateDIBPalette(HANDLE hDIB) {
	LPLOGPALETTE     lpPal;
	HANDLE           hLogPal;
	HPALETTE         hPal = NULL;
	int              i, wNumColors;
	LPSTR            lpbi;
	LPBITMAPINFO     lpbmi;
	LPBITMAPCOREINFO lpbmc;
	BOOL             bWinStyleDIB;

	if (!hDIB)
		return NULL;

	lpbi = (LPSTR)GlobalLock(hDIB);
	lpbmi = (LPBITMAPINFO)lpbi;
	lpbmc = (LPBITMAPCOREINFO)lpbi;
	wNumColors = DIBNumColors(lpbi);
	bWinStyleDIB = IS_WIN30_DIB(lpbi);

	if (wNumColors) {
		hLogPal = GlobalAlloc(GHND, sizeof(LOGPALETTE) +
			sizeof(PALETTEENTRY) * wNumColors);

		if (!hLogPal) {
			//         DIBError (ERR_CREATEPAL);
			GlobalUnlock(hDIB);
			return NULL;
		}

		lpPal = (LPLOGPALETTE)GlobalLock(hLogPal);

		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = wNumColors;

		for (i = 0; i < wNumColors; i++) {
			if (bWinStyleDIB) {
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else {
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		hPal = CreatePalette(lpPal);

		//      if (!hPal)

		//         DIBError (ERR_CREATEPAL);

		GlobalUnlock(hLogPal);
		GlobalFree(hLogPal);
	}

	GlobalUnlock(hDIB);

	return hPal;
}

//---------------------------------------------------------------------
//
// Function:   DIBHeight
//
// Purpose:    Given a pointer to a DIB, returns its height.  Note
//             that it returns a DWORD (since a Win30 DIB can have
//             a DWORD in its height field), but under Win30, the
//             high order word isn't used!
//
// Parms:      lpDIB == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

DWORD DIBHeight(LPSTR lpDIB) {
	LPBITMAPINFOHEADER lpbmi;
	LPBITMAPCOREHEADER lpbmc;

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	if (lpbmi->biSize == sizeof(BITMAPINFOHEADER))
		return lpbmi->biHeight;
	else
		return (DWORD)lpbmc->bcHeight;
}

//---------------------------------------------------------------------
//
// Function:   DIBWidth
//
// Purpose:    Given a pointer to a DIB, returns its width.  Note
//             that it returns a DWORD (since a Win30 DIB can have
//             a DWORD in its width field), but under Win30, the
//             high order word isn't used!
//
// Parms:      lpDIB == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

DWORD DIBWidth(LPSTR lpDIB) {
	LPBITMAPINFOHEADER lpbmi;
	LPBITMAPCOREHEADER lpbmc;

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	if (lpbmi->biSize == sizeof(BITMAPINFOHEADER))
		return lpbmi->biWidth;
	else
		return (DWORD)lpbmc->bcWidth;
}

//---------------------------------------------------------------------
//
// Function:   DIBToBitmap
//
// Purpose:    Given a handle to global memory with a DIB spec in it,
//             and a palette, returns a device dependent bitmap.  The
//             The DDB will be rendered with the specified palette.
//
// Parms:      hDIB == HANDLE to global memory containing a DIB spec
//                     (either BITMAPINFOHEADER or BITMAPCOREHEADER)
//             hPal == Palette to render the DDB with.  If it's NULL,
//                     use the default palette.
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

HBITMAP DIBToBitmap(HANDLE hDIB, HPALETTE hPal) {
	LPSTR    lpDIBHdr, lpDIBBits;
	HBITMAP  hBitmap;
	HDC      hDC;
	HPALETTE hOldPal = NULL;

	if (!hDIB)
		return NULL;

	lpDIBHdr = (LPSTR)GlobalLock(hDIB);
	lpDIBBits = FindDIBBits(lpDIBHdr);
	hDC = GetDC(NULL);

	if (!hDC) {
		GlobalUnlock(hDIB);
		return NULL;
	}

	if (hPal)
		hOldPal = SelectPalette(hDC, hPal, FALSE);

	RealizePalette(hDC);

	hBitmap = CreateDIBitmap(hDC,
		(LPBITMAPINFOHEADER)lpDIBHdr,
		CBM_INIT,
		lpDIBBits,
		(LPBITMAPINFO)lpDIBHdr,
		DIB_RGB_COLORS);

	//   if (!hBitmap)
	//      DIBError (ERR_CREATEDDB);

	if (hOldPal)
		SelectPalette(hDC, hOldPal, FALSE);

	ReleaseDC(NULL, hDC);
	GlobalUnlock(hDIB);

	return hBitmap;
}

//---------------------------------------------------------------------
//
// Function:   InitBitmapInfoHeader
//
// Purpose:    Does a "standard" initialization of a BITMAPINFOHEADER,
//             given the Width, Height, and Bits per Pixel for the
//             DIB.
//
//             By standard, I mean that all the relevant fields are set
//             to the specified values.  biSizeImage is computed, the
//             biCompression field is set to "no compression," and all
//             other fields are 0.
//
//             Note that DIBs only allow BitsPixel values of 1, 4, 8, or
//             24.  This routine makes sure that one of these values is
//             used (whichever is most appropriate for the specified
//             nBPP).
//
// Parms:      lpBmInfoHdr == Far pointer to a BITMAPINFOHEADER structure
//                            to be filled in.
//             dwWidth     == Width of DIB (not in Win 3.0 & 3.1, high
//                            word MUST be 0).
//             dwHeight    == Height of DIB (not in Win 3.0 & 3.1, high
//                            word MUST be 0).
//             nBPP        == Bits per Pixel for the DIB.
//
// History:   Date      Reason
//            11/07/91  Created
//
//---------------------------------------------------------------------

void InitBitmapInfoHeader(LPBITMAPINFOHEADER lpBmInfoHdr,
	DWORD dwWidth,
	DWORD dwHeight,
	int nBPP) {
	memset(lpBmInfoHdr, 0, sizeof(BITMAPINFOHEADER));

	lpBmInfoHdr->biSize = sizeof(BITMAPINFOHEADER);
	lpBmInfoHdr->biWidth = dwWidth;
	lpBmInfoHdr->biHeight = dwHeight;
	lpBmInfoHdr->biPlanes = 1;

	if (nBPP <= 1)
		nBPP = 1;
	else if (nBPP <= 4)
		nBPP = 4;
	else if (nBPP <= 8)
		nBPP = 8;
	else
		nBPP = 24;
	lpBmInfoHdr->biBitCount = nBPP;
	lpBmInfoHdr->biSizeImage = WIDTHBYTES(dwWidth * nBPP) * dwHeight;
}

//---------------------------------------------------------------------
//
// Function:   BitmapToDIB
//
// Purpose:    Given a device dependent bitmap and a palette, returns
//             a handle to global memory with a DIB spec in it.  The
//             DIB is rendered using the colors of the palette passed in.
//
//             Stolen almost verbatim from ShowDIB.
//
// Parms:      hBitmap == Handle to device dependent bitmap compatible
//                        with default screen display device.
//             hPal    == Palette to render the DDB with.  If it's NULL,
//                        use the default palette.
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

HANDLE BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal) {
	BITMAP             Bitmap;
	BITMAPINFOHEADER   bmInfoHdr;
	LPBITMAPINFOHEADER lpbmInfoHdr;
	LPSTR              lpBits;
	HDC                hMemDC;
	HANDLE             hDIB;
	HPALETTE           hOldPal = NULL;

	// Do some setup -- make sure the Bitmap passed in is valid,
	//  get info on the bitmap (like its height, width, etc.),
	//  then setup a BITMAPINFOHEADER.

	if (!hBitmap)
		return NULL;
	if (!GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap))
		return NULL;
	InitBitmapInfoHeader(&bmInfoHdr,
		Bitmap.bmWidth,
		Bitmap.bmHeight,
		Bitmap.bmPlanes * Bitmap.bmBitsPixel);
	// Now allocate memory for the DIB.  Then, set the BITMAPINFOHEADER
	//  into this memory, and find out where the bitmap bits go.

	hDIB = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) +
		PaletteSize((LPSTR)&bmInfoHdr) + bmInfoHdr.biSizeImage);
	if (!hDIB)
		return NULL;
	lpbmInfoHdr = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
	*lpbmInfoHdr = bmInfoHdr;
	lpBits = FindDIBBits((LPSTR)lpbmInfoHdr);
	// Now, we need a DC to hold our bitmap.  If the app passed us
	//  a palette, it should be selected into the DC.
	hMemDC = GetDC(NULL);
	if (hPal) {
		hOldPal = SelectPalette(hMemDC, hPal, FALSE);
		RealizePalette(hMemDC);
	}
	// We're finally ready to get the DIB.  Call the driver and let
	//  it party on our bitmap.  It will fill in the color table,
	//  and bitmap bits of our global memory block.
	if (!GetDIBits(hMemDC,
		hBitmap,
		0,
		Bitmap.bmHeight,
		lpBits,
		(LPBITMAPINFO)lpbmInfoHdr,
		DIB_RGB_COLORS)) {
		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
		hDIB = NULL;
	}
	else
		GlobalUnlock(hDIB);
	if (hOldPal)
		SelectPalette(hMemDC, hOldPal, FALSE);
	ReleaseDC(NULL, hMemDC);
	return hDIB;
}