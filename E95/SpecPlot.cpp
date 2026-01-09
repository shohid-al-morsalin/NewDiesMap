// SpecPlot.cpp : implementation file
//

#include "stdafx.h"
#include "413App.h"
#include "SpecPlot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpecPlot

CSpecPlot::CSpecPlot() {
	m_lnColor1 = RGB(255, 0, 128);	//theoretical curve
	m_bgColor2 = RGB(245, 245, 245);	//background
	m_lnColor2 = RGB(0, 0, 255);		//measured curve
	m_lnColor3 = RGB(16, 16, 16);		//grid lines
	m_lnColor4 = RGB(16, 255, 128);
	m_lnColor5 = RGB(128, 16, 255);
	m_lnColor6 = RGB(50, 205, 205);
	m_lnColorTxt = RGB(0, 0, 0);		//text

	method = 0;
	bFilter = FALSE;
	bFileOpen = FALSE;

	bDrawBackgn = FALSE;
	bDrawRef = FALSE;

	msgStr = "";

	BmpNo = 0;
	bSaveBitmap = FALSE;
}

CSpecPlot::~CSpecPlot() {}

BEGIN_MESSAGE_MAP(CSpecPlot, CStatic)
	//{{AFX_MSG_MAP(CSpecPlot)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecPlot message handlers

void CSpecPlot::OnSize(UINT nType, int cx, int cy) {
	CStatic::OnSize(nType, cx, cy);

	CDC* pDC = GetDC();

	Draw(pDC);
}

void CSpecPlot::OnPaint() {
	CPaintDC dc(this); // device context for painting

	Draw(&dc);

	ReleaseDC(&dc);
	// Do not call CStatic::OnPaint() for painting messages
}

void CSpecPlot::Draw(CDC* pDC) {
	CRect rc;
	GetClientRect(&rc);

	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	pDC->SelectClipRgn(&Rg);

	CDC dcMem;
	CBitmap bmp;
	HBITMAP oldBmp;

	dcMem.CreateCompatibleDC(pDC);

	bmp.CreateCompatibleBitmap(pDC, (int)(rc.right), (int)(rc.bottom));

	oldBmp = (HBITMAP)dcMem.SelectObject(bmp);

	//draw background
	dcMem.FillSolidRect(0, 0, (int)(rc.right), (int)(rc.bottom), m_bgColor2);

	rc.right -= 2;
	DrawGrid(&dcMem, rc);

	COceanBase* pOcean = p413App->pOceanOp;

	if (bDrawBackgn) DrawSpectrum(&dcMem, rc, &pOcean->BackGdSpectrum, FALSE, m_lnColor5);
	if (bDrawRef) DrawSpectrum(&dcMem, rc, &pOcean->CalibrSpectrum, FALSE, m_lnColor4);
	if (p413App->Global.bShowSpec) DrawSpectrum(&dcMem, rc, &RSpec, TRUE, m_lnColor2);

	DrawHisto(&dcMem, rc, &RSpec);

	//copy bitmap to the screen
	pDC->BitBlt(0, 0, rc.right, (int)(rc.bottom), &dcMem, 0, 0, SRCCOPY);

	if (oldBmp) {
		dcMem.SelectObject(oldBmp);
	}
	dcMem.DeleteDC();

	// SaveBitmap() must be the last function to call [7/26/2011 XPMUser]
	if (bSaveBitmap) {
		CString name;
		name.Format("%s-%d-%s.BMP", SpecName, BmpNo, CTime::GetCurrentTime().Format("%H%M%S"));
		SaveBitmap2(name, bmp);
		bSaveBitmap = FALSE;
	}
	bmp.DeleteObject();
}

void CSpecPlot::DrawSpectrum(CDC* dcMem, CRect rc1, CSpec* pRSpec, BOOL bDrawLabel, COLORREF& co) {
	if (!pRSpec || !pRSpec->pPt) {
		return;
	}

	int i;
	CString str;
	CPen pen, pen2;

	HPEN oldPen;
	CRect rc = rc1;

	rc.bottom -= 32;
	//rc.bottom = rc.top + rc.Height()/2;
	rc.left += 22;

	//draw theoretical curve
	pen.CreatePen(PS_SOLID, 1, m_lnColor1);
	oldPen = (HPEN)dcMem->SelectObject(pen);

	//draw measured curve
	pen2.CreatePen(PS_SOLID, 1, co);
	dcMem->SelectObject(pen2);

	int wd = (rc.right - rc.left);
	float sf = rc.Height() / (pRSpec->Cmax - pRSpec->Cmin);
	float sfx = wd / float(pRSpec->N);

	// Draw raw spectrum [7/25/2010 C2C]
	if (pRSpec->N <= SPMAX) {
		CSpecPoint* pSp = pRSpec->pPt;
		for (i = 0; i < pRSpec->N; i++) {
			Pnts[i].x = rc.left + i * sfx;
			Pnts[i].y = rc.bottom - sf * (pSp->Y1 - pRSpec->Cmin);
			pSp++;
		}
		dcMem->Polyline(Pnts, pRSpec->N);
	}

	// 	CPen pen3;
	// 	pen3.CreatePen(PS_SOLID,1, m_lnColor1);
	// 	dcMem->SelectObject(pen3);
	//
	// 	pSp = pRSpec->pPt;
	// 	for (i = 0; i< pRSpec->N; i++)
	// 	{
	// 		if (pSp->Y2 > 0)
	// 		{
	// 			int ix = i*sfx;
	// 			if (ix > wd)
	// 			{
	// 				break;
	// 			}
	// 			dcMem->MoveTo(rc.left+ix,int(rc.bottom));
	// 			dcMem->LineTo(rc.left+ix,int(rc.bottom - sf * (pSp->Y1 - pRSpec->Cmin)));
	// 		}
	// 		pSp++;
	// 	}

	if (oldPen) dcMem->SelectObject(oldPen);

	if (bDrawLabel) {
		dcMem->SetBkMode(TRANSPARENT);
		dcMem->SetTextColor(RGB(128, 64, 64));
		str.Format("%.0f  %.0f", pRSpec->Cmax, pRSpec->Cmax - pRSpec->Cmin);
		dcMem->SetTextAlign(TA_LEFT | TA_TOP);
		dcMem->TextOut(rc1.left, rc.top, str);
		str.Format("%.0f", (pRSpec->Cmax + pRSpec->Cmin) / 2.0f);
		dcMem->SetTextAlign(TA_LEFT | TA_BASELINE);
		dcMem->TextOut(rc1.left, (rc.bottom - rc.top) / 2, str);
		str.Format("%.0f", pRSpec->Cmin);
		dcMem->SetTextAlign(TA_LEFT | TA_BOTTOM);
		dcMem->TextOut(rc1.left, rc.bottom, str);

		if (pRSpec->pPt[0].X1 < 1) {
			// wave number [1/6/2012 Administrator]
			dcMem->SetTextAlign(TA_LEFT | TA_TOP);
			str.Format("%.1f", 1 / pRSpec->pPt[0].X1);
			dcMem->TextOut(rc.left, rc.bottom, str);
			dcMem->SetTextAlign(TA_RIGHT | TA_TOP);
			str.Format("%.1f", 1 / pRSpec->pPt[pRSpec->N - 1].X1);
		}
		else {
			// wave length [1/6/2012 Administrator]
			dcMem->SetTextAlign(TA_LEFT | TA_TOP);
			str.Format("%.1f", pRSpec->pPt[0].X1);
			dcMem->TextOut(rc.left, rc.bottom, str);
			dcMem->SetTextAlign(TA_RIGHT | TA_TOP);
			str.Format("%.1f", pRSpec->pPt[pRSpec->N - 1].X1);
		}
		dcMem->TextOut(rc.right, rc.bottom, str);
		dcMem->SetTextAlign(TA_CENTER | TA_TOP);
		str.Format("wavelength(nm)");
		dcMem->TextOut((rc.right + rc.left) / 2, rc.bottom, str);

		str = "Thk: ";
		CString tstr;
		for (i = 0; i < MAXPOLYVLU; i++) {
			if (RSpec.Poly[i].fThk > 0) {
				tstr.Format("%.3f ", RSpec.Poly[i].fThk);
				str += tstr;
			}
			else {
				break;
			}
		}
		dcMem->SetTextAlign(TA_LEFT | TA_TOP);
		if (msgStr.GetLength() > 0) {
			str = str + "    " + msgStr;
		}
		dcMem->TextOut(rc.left, rc.bottom + 15, str);
	}

	pen.DeleteObject();
	pen2.DeleteObject();

	oldPen = NULL;
}

void CSpecPlot::Refresh() {
	Invalidate();
}

void CSpecPlot::DrawHisto(CDC* dcMem, CRect rc, CSpec* pRSpec) {
	if (!pRSpec || !pRSpec->S || !pRSpec->pV) {
		return;
	}
	rc.left += 26;

	float len = pRSpec->S;
	float sfy = rc.Height() * 0.5 / pRSpec->MaxV;

	CPen pen1, pen2, pen3, pen4;
	HPEN oldPen;
	CString str;
	pen1.CreatePen(PS_SOLID, 2, m_lnColor1);
	pen2.CreatePen(PS_SOLID, 1, m_lnColor6);
	pen3.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
	pen4.CreatePen(PS_SOLID, 2, RGB(0, 125, 125));
	oldPen = (HPEN)dcMem->SelectObject(pen2);
	dcMem->SetBkMode(TRANSPARENT);

	int i/*,j*/;
	int nx, x, y;
	int b = rc.bottom - 34;

	int jPLow = pRSpec->Poly[0].jPlow;
	int jPHi = pRSpec->Poly[0].jPhi;

	for (i = 1; i < MAXPOLYVLU; i++) {
		if (pRSpec->Poly[i].fThk > 0) {
			if (pRSpec->Poly[i].jPlow < jPLow) {
				jPLow = pRSpec->Poly[i].jPlow;
			}
			if (pRSpec->Poly[i].jPhi > jPHi) {
				jPHi = pRSpec->Poly[i].jPhi;
			}
		}
	}
	pRSpec->Poly[0].jPlow = jPLow;
	pRSpec->Poly[0].jPhi = jPHi;
	jPLow *= 0.94f;
	jPHi *= 1.06f;

	if (jPLow < 0) {
		jPLow = 0;
	}
	if (jPHi >= pRSpec->S) {
		jPHi = pRSpec->S - 1;
	}
	float sfx = (jPHi - jPLow) / float(rc.Width() - 1);
	// Draw bars [6/19/2011 XPMUser]
	for (i = 0; i < rc.Width(); i += 2) {
		int ii = i + rc.left;
		nx = sfx * i + jPLow;
		if (nx > pRSpec->S) {
			break;
		}
		y = (pRSpec->pV[nx].V) * sfy;
		dcMem->MoveTo(ii, b);
		dcMem->LineTo(ii, b - y);
	}

	dcMem->SelectObject(pen4);
	CFFTPoint* pSf = pRSpec->pV;
	for (i = 0; i < pRSpec->S; i++) {
		if (pSf->P > 0) {
			x = (i - jPLow) / sfx + rc.left;
			if (x < rc.right) {
				y = (pRSpec->pV[i].V) * sfy;
				dcMem->MoveTo(x, b);
				dcMem->LineTo(x, b - y);
			}
		}
		pSf++;
	}

	// Draw lower and upper limits [11/25/2012 Yuen]
	dcMem->SelectObject(pen3);
	i = (pRSpec->Poly[0].jPlow - jPLow) / sfx + rc.left;
	y = (pRSpec->pV[pRSpec->Poly[0].jPlow].V) * sfy;
	dcMem->MoveTo(i, b);
	dcMem->LineTo(i, b - y);
	i = (pRSpec->Poly[0].jPhi - jPLow) / sfx + rc.left;
	y = (pRSpec->pV[pRSpec->Poly[0].jPhi].V) * sfy;
	dcMem->MoveTo(i, b);
	dcMem->LineTo(i, b - y);

	// Label max peak [11/25/2012 Yuen]
	for (int k = 0; k < MAXPOLYVLU; k++) {
		if (pRSpec->Poly[k].jPmax > 0) {
			dcMem->SelectObject(pen1);
			i = (pRSpec->Poly[k].jPmax - jPLow) / sfx + rc.left;
			y = (pRSpec->pV[pRSpec->Poly[k].jPmax].V) * sfy;
			dcMem->MoveTo(i, b);
			dcMem->LineTo(i, b - y);

			// 			i=(pRSpec->Poly[k].jLo-jPLow)/sfx+rc.left;
			// 			j=(pRSpec->Poly[k].jHi-jPLow)/sfx+rc.left;
			// 			y = (pRSpec->pV[pRSpec->Poly[k].jPmax].V) * sfy * 0.5f;
			// 			dcMem->MoveTo(i, b-y);
			// 			dcMem->LineTo(j, b-y);

			dcMem->SetTextColor(RGB(255, 64, 64));
			dcMem->SetTextAlign(TA_CENTER | TA_BOTTOM);
			i = (pRSpec->Poly[k].jPmax - jPLow) / sfx + rc.left;
			y = (pRSpec->pV[pRSpec->Poly[k].jPmax].V) * sfy - 12;
			//CString str;
			str.Format("T%.3f", pRSpec->Poly[k].fThk);
			dcMem->TextOut(i, b - y - 14, str);
			str.Format("G:%.2f  W:%.2f", pRSpec->Poly[k].GOF, pRSpec->Poly[k].WHH);
			dcMem->TextOut(i, b - y, str);
			if (oldPen) dcMem->SelectObject(oldPen);
		}
	}
}

void CSpecPlot::DrawGrid(CDC* dcMem, CRect rc) {
	CPen pen3;
	HPEN oldPen;

	rc.bottom -= 32;
	rc.left += 22;

	//draw grid
	pen3.CreatePen(PS_DASH, 1, m_lnColor3);
	oldPen = (HPEN)dcMem->SelectObject(pen3);
	int n = 1;
	float sf = (rc.bottom - rc.top - 1) / float(n);
	int i;
	for (i = 0; i <= n; i++) {
		dcMem->MoveTo(rc.left, rc.top + (int)(sf * i));
		dcMem->LineTo((int)(rc.right), rc.top + (int)(sf * i));
	}
	sf = (rc.right - rc.left - 1) / float(n);
	for (i = 0; i <= n; i++) {
		dcMem->MoveTo(rc.left + (int)(sf * i), rc.top);
		dcMem->LineTo(rc.left + (int)(sf * i), (int)(rc.bottom));
	}
	if (oldPen) dcMem->SelectObject(oldPen);
	pen3.DeleteObject();
}

#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')

void CSpecPlot::SaveBitmap(CString Name, CBitmap& Bmp) {
	CString fname;
	fname = "C:\\TEMP\\";
	fname += Name + ".BMP";

	SaveBitmap2(fname, Bmp);
}

void CSpecPlot::SaveBitmap2(CString Name, CBitmap& Bmp) {
	DDBToDIB(Bmp, BI_RGB, NULL);

	BITMAP bitmapInfo;
	Bmp.GetBitmap(&bitmapInfo);
	BITMAPINFOHEADER pbih;

	long BytesPerRow = bitmapInfo.bmWidth * (bitmapInfo.bmBitsPixel / 8);
	BytesPerRow += (4 - BytesPerRow % 4) % 4;	// int alignment

	ZeroMemory(&pbih, sizeof(BITMAPINFOHEADER));
	pbih.biWidth = bitmapInfo.bmWidth;
	pbih.biHeight = bitmapInfo.bmHeight;
	pbih.biPlanes = bitmapInfo.bmPlanes;
	pbih.biBitCount = bitmapInfo.bmBitsPixel;
	pbih.biCompression = 0;
	pbih.biSizeImage = BytesPerRow * abs(bitmapInfo.bmHeight);
	pbih.biSize = sizeof(BITMAPINFOHEADER);
	pbih.biXPelsPerMeter = 4000;
	pbih.biYPelsPerMeter = 4000;

	int i;
	BITMAPFILEHEADER hdr;
	DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(hdr);
	hdr.bfType = DS_BITMAP_FILEMARKER;
	hdr.bfSize = dwFileHeaderSize + pbih.biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = dwFileHeaderSize;

	//int iTransfer;
	int iByteToTransfer;
	iByteToTransfer = BytesPerRow * abs(bitmapInfo.bmHeight);
	UCHAR* buf3 = new UCHAR[iByteToTransfer];
	/*iTransfer =*/ Bmp.GetBitmapBits(iByteToTransfer, buf3);

	CFile file;
	if (file.Open(Name, CFile::modeWrite | CFile::modeCreate)) {
		file.Write(&hdr, sizeof(BITMAPFILEHEADER));
		file.Write(&pbih, sizeof(BITMAPINFOHEADER));
		//file.Write(buf2, pbih.biSizeImage);
		int wd = bitmapInfo.bmWidth * (bitmapInfo.bmBitsPixel / 8);
		for (i = pbih.biHeight - 1; i >= 0; i--) {
			file.Write(buf3 + i * wd, BytesPerRow);
			//file.Write(buf3 + i * BytesPerRow, BytesPerRow);
		}
		file.Close();
	}
	delete[] buf3;
}

// SpecN requires full path specification [1/9/2012 Administrator]
void CSpecPlot::SaveBitmapA(CString SpecN, short n) {
	BmpNo = n;
	SpecName = SpecN;
	bSaveBitmap = TRUE;
	Invalidate(TRUE);
}

// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE CSpecPlot::DDBToDIB(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal) {
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;

	ASSERT(bitmap.GetSafeHandle());

	// The function has no arg for bitfields
	if (dwCompression == BI_BITFIELDS) {
		return NULL;
	}

	// If a palette has not been supplied use defaul palette
	hPal = (HPALETTE)pPal->GetSafeHandle();
	if (hPal == NULL) {
		hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);
	}

	// Get bitmap information
	bitmap.GetObject(sizeof(bm), (LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bm.bmWidth;
	bi.biHeight = bm.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression = dwCompression;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if (nColors > 256) {
		nColors = 0;
	}
	dwLen = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::GetDC(NULL);
	hPal = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	if (!hDIB) {
		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver
	// will calculate the biSizeImage field
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0) {
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8)
			* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE)) {
		hDIB = handle;
	}
	else {
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(),
		0L,				// Start scan line
		(DWORD)bi.biHeight,		// # of scan lines
		(LPBYTE)lpbi 			// address for bitmap bits
		+ (bi.biSize + nColors * sizeof(RGBQUAD)),
		(LPBITMAPINFO)lpbi,		// address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if (!bGotBits) {
		GlobalFree(hDIB);

		SelectPalette(hDC, hPal, FALSE);
		::ReleaseDC(NULL, hDC);
		return NULL;
	}

	SelectPalette(hDC, hPal, FALSE);
	::ReleaseDC(NULL, hDC);
	return hDIB;
}

float CSpecPlot::GetZoom(short N) {
	switch (N) {
	case 512:
		return 8.0f;
	case 1024:
		return 18.0f;
	case 1536:
		return 22.0f;
	case 2048:
		return 26.0f;
	case 2560:
		return 36.0f;
	case 3072:
		return 44.0f;
	case 3584:
		return 56.0f;
	case 4096:
		return 80.0f;
	default:
		return 160.0f;
	}
}