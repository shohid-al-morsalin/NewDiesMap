// EchoDraw.cpp : implementation file
//

#include "stdafx.h"

#include "EchoDraw.h"
#include "..\413\EchoScope\ScanLine.h"
#include "..\413\EchoScope\ScanPoint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEchoDraw

CEchoDraw::CEchoDraw() {
	bAllcBMP = FALSE;
	WdBk = 0;
	HtBk = 0;

	bReal = TRUE;
	bSaveChart = FALSE;

	DrawMethod = 0;

	m_bgColor = RGB(255, 255, 255);
	m_lnColor1 = RGB(0, 128, 255);
	m_lnColor2 = RGB(255, 128, 0);
	m_grColor = RGB(64, 64, 64);
	m_txtColor = RGB(16, 16, 16);

	pSLn1 = new CScanLine;
	pSLn2 = new CScanLine;
	pY1 = pY2 = pX = NULL;

	mode = 0xFC;
	ChartType = 0;

	pointsCnt = 0;
	XMax = XMin = 0;
	YMax = YMin = 0;

	direc = 1; // 1 or -1 [7/19/2010 C2C]

	m_ChartName = "";

	// Simulation [7/19/2010 C2C]
// 	bSimu = TRUE;
// 	pointsCnt = 256;
//
// 	pSL1 = new CScanLine;
// 	pSL1->Fake(pointsCnt);
// 	pSL2 = new CScanLine;
// 	pSL2->Fake(pointsCnt);

// 	pX = new int[256];
// 	pY1 = new int[256];
// 	pY2 = new int[256];
}

CEchoDraw::~CEchoDraw() {
	// 	if (bSimu)
	// 	{
	// 		if (pX)
	// 		{
	// 			delete [] pX;
	// 		}
	// 		if (pY1)
	// 		{
	// 			delete [] pY1;
	// 		}
	// 		if (pY2)
	// 		{
	// 			delete [] pY2;
	// 		}
	// 		if (pSL1)
	// 		{
	// 			delete pSL1;
	// 		}
	// 		if (pSL2)
	// 		{
	// 			delete pSL2;
	// 		}
	// 	}

	if (pSLn1) {
		delete pSLn1;
	}
	if (pSLn2) {
		delete pSLn2;
	}
}

BEGIN_MESSAGE_MAP(CEchoDraw, CStatic)
	//{{AFX_MSG_MAP(CEchoDraw)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEchoDraw message handlers

void CEchoDraw::OnPaint() {
	CPaintDC dc(this); // device context for painting
	Draw(&dc);
	ReleaseDC(&dc);

	// Do not call CStatic::OnPaint() for painting messages
}

void CEchoDraw::OnSize(UINT nType, int cx, int cy) {
	CStatic::OnSize(nType, cx, cy);

	Invalidate();
}

void CEchoDraw::Draw0(CDC* pDC) {
	CRect rc;
	GetClientRect(&rc);
	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	pDC->SelectClipRgn(&Rg);

	HBITMAP oldBmp = NULL;
	HPEN oldPen = NULL;

	CDC dcMem;
	CPen pen;
	CPen pen3;
	int i, Wd, Ht, Ht2;

	BOOL bBmpDraw = FALSE;
	if (mode & 0x10) {
		bBmpDraw = TRUE;
	}

	Wd = rc.right;
	Ht = rc.bottom;
	Ht2 = Ht / 2;
	if (bBmpDraw) {
		if (!dcMem.CreateCompatibleDC(pDC)) return;
		if ((Wd != WdBk) || (Ht != HtBk)) {
			if (bAllcBMP) {
				bmp.DeleteObject();
			}
			bmp.CreateCompatibleBitmap(pDC, Wd, Ht);
			bAllcBMP = TRUE;
			WdBk = Wd; HtBk = Ht;
		}
		oldBmp = (HBITMAP)dcMem.SelectObject(bmp);
	}

	if (bBmpDraw) {
		dcMem.FillSolidRect(rc.left, rc.top, Wd, Ht, m_bgColor);
	}
	else {
		pDC->FillSolidRect(rc.left, rc.top, Wd, Ht, m_bgColor);
	}

	// Draw Frame [3/9/2010 Valued Customer]
	if (mode & 0x01) {
		pen3.CreatePen(PS_DASH, 1, m_grColor);
		if (bBmpDraw) {
			oldPen = (HPEN)dcMem.SelectObject(pen3);
		}
		else {
			oldPen = (HPEN)pDC->SelectObject(pen3);
		}
		int Ht4 = Ht / 4;
		for (i = 1; i <= 3; i++) {
			if (bBmpDraw) {
				dcMem.MoveTo(0, (int)(i * Ht4));
				dcMem.LineTo((int)rc.right, (int)(i * Ht4));
			}
			else {
				pDC->MoveTo(0, (int)(i * Ht));
				pDC->LineTo((int)rc.right, (int)(i * Ht4));
			}
		}
		float Wd1 = 0.1f * Wd;
		for (i = 1; i <= 9; i++) {
			if (bBmpDraw) {
				dcMem.MoveTo((int)(i * Wd1), 0);
				dcMem.LineTo((int)(i * Wd1), Ht);
			}
			else {
				pDC->MoveTo((int)(i * Wd1), 0);
				pDC->LineTo((int)(i * Wd1), Ht);
			}
		}

		if (bBmpDraw) {
			dcMem.SelectObject(oldPen);
		}
		else {
			pDC->SelectObject(oldPen);
		}
		pen3.DeleteObject();
	}

	int rangeX = rc.right - rc.left;
	int rangeY = rc.bottom - rc.top;
	float mmX = float(XMax - XMin);
	float mmY = float(YMax - YMin);
	float sf = (pointsCnt) / (float(rangeX));
	int bline = rangeY * (1 - (2048 - YMin) / mmY);
	float tx = rangeX / mmX;
	float ty = rangeY / mmY;

	// Draw Data 1 top probe signal [3/27/2010 Valued Customer]
	if (pX && pY1) {
		if (bBmpDraw) {
			if (direc > 0) {
				pen.CreatePen(PS_SOLID, 1, m_lnColor1);
			}
			else {
				pen.CreatePen(PS_SOLID, 1, m_lnColor2);
			}
			oldPen = (HPEN)dcMem.SelectObject(pen);
			dcMem.SelectObject(pen);
		}
		else {
			pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			oldPen = (HPEN)pDC->SelectObject(pen);
			pDC->SelectObject(pen);
		}
		int* pXX = pX;
		int* pYY1 = pY1;
		int px = tx * (*(pXX++) - XMin);
		switch (ChartType) {
		case 1:
			if (bBmpDraw) {
				dcMem.MoveTo(*pX, rangeY - ty * (*(pYY1++) - YMin));
			}
			else {
				pDC->MoveTo(*pX, rangeY - ty * (*(pYY1++) - YMin));
			}
			for (i = 0; i < pointsCnt; i++) {
				px = tx * (*(pXX++) - XMin);
				if (bBmpDraw) {
					dcMem.LineTo(px, rangeY - ty * (*(pYY1++) - YMin));
				}
				else {
					pDC->LineTo(px, rangeY - ty * (*(pYY1++) - YMin));
				}
			}
			break;
		default:
			for (i = 0; i < pointsCnt; i++) {
				int px1 = tx * (*(pXX++) - XMin);
				if (bBmpDraw) {
					dcMem.MoveTo(px1, bline);
					dcMem.LineTo(px1, rangeY - ty * (*(pYY1++) - YMin));
				}
				else {
					if (*pYY1 > 2047) {
						pDC->MoveTo(px1, bline);
						pDC->LineTo(px1, rangeY - ty * (*(pYY1++) - YMin));
					}
					else {
						pYY1++;
					}
				}
			}
			break;
		}
		if (bBmpDraw) {
			dcMem.SelectObject(oldPen);
		}
		else {
			pDC->SelectObject(oldPen);
		}
		pen.DeleteObject();
	}

	// Draw Data 2 bottom probe signal [3/27/2010 Valued Customer]
	if (pX && pY2) {
		if (bBmpDraw) {
			if (direc > 0) {
				pen.CreatePen(PS_SOLID, 1, m_lnColor1);
			}
			else {
				pen.CreatePen(PS_SOLID, 1, m_lnColor2);
			}
			oldPen = (HPEN)dcMem.SelectObject(pen);
			dcMem.SelectObject(pen);
		}
		else {
			pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
			oldPen = (HPEN)pDC->SelectObject(pen);
			pDC->SelectObject(pen);
		}
		int* pXX = pX;
		INT* pYY2 = pY2;
		for (i = 0; i < pointsCnt; i++) {
			int px = tx * (*(pXX++) - XMin);
			if (bBmpDraw) {
				dcMem.MoveTo(px, bline);
				dcMem.LineTo(px, ty * (*(pYY2++) - YMin));
			}
			else {
				if (*pYY2 > 2047) {
					pDC->MoveTo(px, bline);
					pDC->LineTo(px, ty * (*(pYY2++) - YMin));
				}
				else {
					pYY2++;
				}
			}
		}
		if (bBmpDraw) {
			dcMem.SelectObject(oldPen);
		}
		else {
			pDC->SelectObject(oldPen);
		}
		pen.DeleteObject();
	}

	char str[255];
	if (mode & 0x02) {
		if (bBmpDraw) {
			dcMem.SetTextColor(m_txtColor);
		}
		else {
			pDC->SetTextColor(m_txtColor);
		}
		int Ht4 = Ht / 4;
		for (i = 1; i <= 3; i++) {
			sprintf(str, "%.0f", (YMax - 0.25 * i * (YMax - YMin)));
			if (bBmpDraw) {
				dcMem.TextOut(rc.left, (int)(i * Ht4) - 8, str);
			}
			else {
				pDC->TextOut(rc.left, (int)(i * Ht4) - 8, str);
			}
		}
	}

	if (bSaveChart) {
		bSaveChart = FALSE;
		if (bBmpDraw) {
			SaveBitmap(m_ChartName, bmp);
		}
	}
	else {
		if (bBmpDraw) {
			pDC->BitBlt(0, 0, Wd, Ht, &dcMem, 0, 0, SRCCOPY);
		}
	}

	if (bBmpDraw) {
		if (oldBmp) dcMem.SelectObject(oldBmp);
		dcMem.DeleteDC();
		oldBmp = NULL;
	}
	oldPen = NULL;
}

void CEchoDraw::Refresh() {
	Invalidate();
}

#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')

void CEchoDraw::SaveBitmap(CString Name, CBitmap& bmp) {
	BITMAP bitmapInfo;
	bmp.GetBitmap(&bitmapInfo);
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER pbih;
	UCHAR* buf2;
	DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(hdr);

	long BPR = bitmapInfo.bmWidth * bitmapInfo.bmBitsPixel / 8;
	BPR += (4 - BPR % 4) % 4;

	pbih.biSize = sizeof(BITMAPINFOHEADER);
	pbih.biWidth = bitmapInfo.bmWidth;
	pbih.biHeight = bitmapInfo.bmHeight;
	pbih.biPlanes = bitmapInfo.bmPlanes;
	pbih.biBitCount = bitmapInfo.bmBitsPixel;
	pbih.biCompression = 0;
	pbih.biSizeImage = BPR * abs(pbih.biHeight);
	pbih.biXPelsPerMeter = 4000;
	pbih.biYPelsPerMeter = 4000;
	pbih.biClrUsed = 0;
	pbih.biClrImportant = 0;

	hdr.bfType = DS_BITMAP_FILEMARKER;
	hdr.bfSize = dwFileHeaderSize + pbih.biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = dwFileHeaderSize;

	buf2 = new UCHAR[pbih.biSizeImage];
	int iTransfer;
	int iByteToTransfer;
	iByteToTransfer = BPR * abs(bitmapInfo.bmHeight);
	UCHAR* buf3 = new UCHAR[iByteToTransfer];
	iTransfer = bmp.GetBitmapBits(pbih.biSizeImage, buf3);
	UCHAR* pSrc, * pDest;
	int i, j; // LYF:  [12/27/2006]
	for (i = 0, j = bitmapInfo.bmHeight - 1; i < bitmapInfo.bmHeight; i++, j--) {
		pSrc = buf3 + i * bitmapInfo.bmWidthBytes;
		pDest = buf2 + j * bitmapInfo.bmWidthBytes;
		memcpy(pDest, pSrc, bitmapInfo.bmWidthBytes);
	}
	delete[] buf3;

	CString fname;
	fname = "C:\\TEMP\\";
	fname += Name + ".BMP";

	CFile file;
	if (file.Open(fname, CFile::modeWrite | CFile::modeCreate)) {
		file.Write(&hdr, sizeof(BITMAPFILEHEADER));
		file.Write(&pbih, sizeof(BITMAPINFOHEADER));
		file.Write(buf2, pbih.biSizeImage);
		file.Close();
	}
	if (buf2) delete[](buf2);
}

void CEchoDraw::SaveChart(CString name) {
	m_ChartName = name;
	bSaveChart = TRUE;
}

void CEchoDraw::SetData0(int* mpX, int* mpY1, int* mpY2, int mpointsCnt,
	double mXMin, double mXMax, double mYMin, double mYMax,
	short mChartType, short mmode, short mDirc) {
	pX = mpX;
	pY1 = mpY1;
	pY2 = mpY2;
	pointsCnt = mpointsCnt;
	XMin = mXMin;
	XMax = mXMax;
	YMin = mYMin;
	YMax = mYMax;
	ChartType = mChartType;
	mode = mmode;
	direc = mDirc;
	DrawMethod = 0;

	Refresh();
}

void CEchoDraw::SetData1(CScanLine* pL1, CScanLine* pL2, double& mYMin, double& mYMax, short mChartType, short mmode) {
	pSLn1->Dup(pL1);
	pSLn2->Dup(pL2);
	YMax = mYMax;
	YMin = mYMin;
	ChartType = mChartType;
	mode = mmode;
	DrawMethod = 1;

	Refresh();
}

void CEchoDraw::Draw1(CDC* pDC) {
	if (!pSLn1) {
		// Nothing to draw [5/3/2010 Valued Customer]
		return;
	}

	CRect rc;
	GetClientRect(&rc);
	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	pDC->SelectClipRgn(&Rg);

	HBITMAP oldBmp = NULL;
	CDC dcMem;
	CPen pen;
	CPen pen3;
	int i, Wd, Ht, Ht2;
	HPEN oldPen = NULL;

	BOOL bBmpDraw = FALSE;
	if (mode & 0x10) {
		bBmpDraw = TRUE;
	}

	Wd = rc.right;
	Ht = rc.bottom;
	Ht2 = Ht / 2;
	if (bBmpDraw) {
		if (!dcMem.CreateCompatibleDC(pDC)) return;
		if ((Wd != WdBk) || (Ht != HtBk)) {
			if (bAllcBMP) {
				bmp.DeleteObject();
			}
			bmp.CreateCompatibleBitmap(pDC, Wd, Ht);
			bAllcBMP = TRUE;
			WdBk = Wd; HtBk = Ht;
		}
		oldBmp = (HBITMAP)dcMem.SelectObject(bmp);
	}

	if (bBmpDraw) {
		dcMem.FillSolidRect(rc.left, rc.top, Wd, Ht, m_bgColor);
	}
	else {
		pDC->FillSolidRect(rc.left, rc.top, Wd, Ht, m_bgColor);
	}

	// Draw Frame [3/9/2010 Valued Customer]
	if (mode & 0x01) {
		pen3.CreatePen(PS_DASH, 1, m_grColor);
		if (bBmpDraw) {
			oldPen = (HPEN)dcMem.SelectObject(pen3);
		}
		else {
			oldPen = (HPEN)pDC->SelectObject(pen3);
		}
		int Ht4 = Ht / 4;
		for (i = 1; i <= 3; i++) {
			if (bBmpDraw) {
				dcMem.MoveTo(0, (int)(i * Ht4));
				dcMem.LineTo((int)rc.right, (int)(i * Ht4));
			}
			else {
				pDC->MoveTo(0, (int)(i * Ht));
				pDC->LineTo((int)rc.right, (int)(i * Ht4));
			}
		}
		float Wd1 = 0.1f * Wd;
		for (i = 1; i <= 9; i++) {
			if (bBmpDraw) {
				dcMem.MoveTo((int)(i * Wd1), 0);
				dcMem.LineTo((int)(i * Wd1), Ht);
			}
			else {
				pDC->MoveTo((int)(i * Wd1), 0);
				pDC->LineTo((int)(i * Wd1), Ht);
			}
		}

		if (bBmpDraw) {
			dcMem.SelectObject(oldPen);
		}
		else {
			pDC->SelectObject(oldPen);
		}
		pen3.DeleteObject();
	}

	int rangeX = rc.right - rc.left;
	int rangeY = rc.bottom - rc.top;

	float mmX;
	if (bReal) {
		mmX = float(pSLn1->GetRangeX());
	}
	else {
		mmX = pSLn1->Idx;
	}
	float mmY = float(YMax - YMin);
	float sf = (pSLn1->Idx) / (float(rangeX));
	int bline = rangeY * (1 - (2047 - YMin) / mmY);
	float tx = rangeX / mmX;
	float ty = rangeY / mmY;
	// Draw Data 1 [3/27/2010 Valued Customer]
	if (pSLn1) {
		if (bBmpDraw) {
			if (direc > 0) {
				pen.CreatePen(PS_SOLID, 1, m_lnColor1);
			}
			else {
				pen.CreatePen(PS_SOLID, 1, m_lnColor2);
			}
			oldPen = (HPEN)dcMem.SelectObject(pen);
			dcMem.SelectObject(pen);
		}
		else {
			pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
			oldPen = (HPEN)pDC->SelectObject(pen);
			pDC->SelectObject(pen);
		}
		CScanPoint* pD2 = pSLn1->pData;
		switch (ChartType) {
		case 1:
		{
			int px;
			if (bReal) {
				px = tx * (pD2->x - pSLn1->MaxMinX.iMin);
			}
			else {
				px = tx * (0);
			}
			if (bBmpDraw) {
				dcMem.MoveTo(px, ty * (pD2->y2 - YMin));
			}
			else {
				pDC->MoveTo(px, ty * (pD2->y2 - YMin));
			}
			for (i = 0; i < pSLn1->Idx; i++) {
				if (bReal) {
					px = tx * (pD2->x - pSLn1->MaxMinX.iMin);
				}
				else {
					px = tx * (i);
				}
				if (bBmpDraw) {
					dcMem.LineTo(px, rangeY - ty * (pD2->y1 - YMin));
				}
				else {
					pDC->LineTo(px, rangeY - ty * (pD2->y1 - YMin));
				}
				pD2++;
			}
		}
		break;
		default:
		{
			if (bReal) {
				int opx = -1;
				if (!pSLn1) {
					break;
				}
				CScanPoint* pD1 = pSLn1->pData;
				if (!pD1) {
					break;
				}
				for (i = 0; i < pSLn1->Idx; i++) {
					if ((pD1->y1 > 2047) || (pD1->y2 != 0)) {
						int px;
						px = tx * (pD1->x - pSLn1->MaxMinX.iMin);
						if ((px == opx) && (pD1->y2 == 0)) {
							pD1++;
							continue;
						}
						opx = px;
						if (bBmpDraw) {
							if (mode) {
								dcMem.MoveTo(px, ty * (pD1->y2 - YMin));
							}
							else {
								dcMem.MoveTo(px, bline);
							}
							dcMem.LineTo(px, rangeY - ty * (pD1->y1 - YMin));

							if (pD1->y2 < 2047) {
								dcMem.MoveTo(px, bline);
								dcMem.LineTo(px, bline + ty * (pD1->y2 + 768));
							}
						}
						else {
							if (mode) {
								pDC->MoveTo(px, ty * (pD1->y2 - YMin));
							}
							else {
								pDC->MoveTo(px, bline);
							}
							pDC->LineTo(px, rangeY - ty * (pD1->y1 - YMin));

							if (pD1->y2 != 0) {
								pDC->MoveTo(px, bline);
								pDC->LineTo(px, bline + ty * (pD1->y2 + 768));
							}
						}
					}
					pD1++;
				}
			}
			else {
				float sxx = pSLn1->Idx / float(rangeX);
				for (i = 0; i < rangeX; i++) {
					int dxx = int(i * sxx);
					CScanPoint* pDa = pSLn1->pData + dxx;

					if (pDa->y1 > 2047) {
						int px;
						px = rc.left + i;
						if (bBmpDraw) {
							if (mode) {
								dcMem.MoveTo(px, ty * (pDa->y2 - YMin));
							}
							else {
								dcMem.MoveTo(px, bline);
							}
							dcMem.LineTo(px, rangeY - ty * (pDa->y1 - YMin));

							if (pDa->y2 < 2047) {
								dcMem.MoveTo(px, bline);
								dcMem.LineTo(px, bline + ty * (pDa->y2 + 768));
							}
						}
						else {
							if (mode) {
								pDC->MoveTo(px, ty * (pDa->y2 - YMin));
							}
							else {
								pDC->MoveTo(px, bline);
							}
							pDC->LineTo(px, rangeY - ty * (pDa->y1 - YMin));

							if (pDa->y2 != 2047) {
								pDC->MoveTo(px, bline);
								pDC->LineTo(px, bline + ty * (pDa->y2 + 768));
							}
						}
					}
				}
			}
		}
		break;
		}
		if (bBmpDraw) {
			dcMem.SelectObject(oldPen);
		}
		else {
			pDC->SelectObject(oldPen);
		}
		pen.DeleteObject();
	}

	// Draw Data 2 [3/27/2010 Valued Customer]
	if (pSLn2) {
		if (bBmpDraw) {
			if (direc > 0) {
				pen.CreatePen(PS_SOLID, 1, m_lnColor1);
			}
			else {
				pen.CreatePen(PS_SOLID, 1, m_lnColor2);
			}
			oldPen = (HPEN)dcMem.SelectObject(pen);
			dcMem.SelectObject(pen);
		}
		else {
			pen.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
			oldPen = (HPEN)pDC->SelectObject(pen);
			pDC->SelectObject(pen);
		}
		CScanPoint* pD = pSLn2->pData;
		switch (ChartType) {
		case 1:
		{
			int px;
			if (bReal) {
				px = tx * (pD->x - pSLn2->MaxMinX.iMin);
			}
			else {
				px = tx * (0);
			}
			if (bBmpDraw) {
				dcMem.MoveTo(px, bline);
			}
			else {
				pDC->MoveTo(px, bline);
			}
			for (i = 0; i < pSLn2->Idx; i++) {
				if (bReal) {
					px = tx * (pD->x - pSLn2->MaxMinX.iMin);
				}
				else {
					px = tx * (i);
				}
				if (bBmpDraw) {
					dcMem.LineTo(px, ty * (pD->y1 - YMin));
				}
				else {
					pDC->LineTo(px, ty * (pD->y1 - YMin));
				}
				pD++;
			}
		}
		break;
		default:
		{
			if (!pSLn2) {
				break;
			}
			CScanPoint* pD1 = pSLn2->pData;
			if (!pD1) {
				break;
			}
			for (i = 0; i < pSLn2->Idx; i++) {
				if (pD1->y1 > 2047) {
					int px;
					if (bReal) {
						px = tx * (pD1->x - pSLn2->MaxMinX.iMin);
					}
					else {
						px = tx * (i);
					}
					if (bBmpDraw) {
						if (mode) {
							dcMem.MoveTo(px, rangeY - ty * (pD1->y2 - YMin));
						}
						else {
							dcMem.MoveTo(px, bline);
						}
						dcMem.LineTo(px, ty * (pD1->y1 - YMin));
					}
					else {
						if (mode) {
							pDC->MoveTo(px, rangeY - ty * (pD1->y2 - YMin));
						}
						else {
							pDC->MoveTo(px, bline);
						}
						pDC->LineTo(px, ty * (pD1->y1 - YMin));
					}
				}
				pD1++;
			}
		}
		break;
		}
		if (bBmpDraw) {
			dcMem.SelectObject(oldPen);
		}
		else {
			pDC->SelectObject(oldPen);
		}
		pen.DeleteObject();
	}

	char str[255];
	if (mode & 0x02) {
		if (bBmpDraw) {
			dcMem.SetTextColor(m_txtColor);
		}
		else {
			pDC->SetTextColor(m_txtColor);
		}
		int Ht4 = Ht / 4;
		for (i = 1; i <= 3; i++) {
			sprintf(str, "%.0f", (YMax - 0.25 * i * (YMax - YMin)));
			if (bBmpDraw) {
				dcMem.TextOut(rc.left, (int)(i * Ht4) - 8, str);
			}
			else {
				pDC->TextOut(rc.left, (int)(i * Ht4) - 8, str);
			}
		}
	}

	if (bSaveChart) {
		bSaveChart = FALSE;
		if (bBmpDraw) {
			SaveBitmap(m_ChartName, bmp);
		}
	}
	else {
		if (bBmpDraw) {
			pDC->BitBlt(0, 0, Wd, Ht, &dcMem, 0, 0, SRCCOPY);
		}
	}

	if (bBmpDraw) {
		if (oldBmp) dcMem.SelectObject(oldBmp);
		dcMem.DeleteDC();
		oldBmp = NULL;
	}
	oldPen = NULL;
}

// void CEchoDraw::Simulate()
// {
// 	if (bSimu)
// 	{
// 		DrawMethod = 1;
// 		pSL1->FakPoint();
// 		pSL2->FakPoint();
// 		XMin = 0; XMax = pointsCnt;
// 		YMin = min(pSL1->MaxMinY1.iMin,pSL2->MaxMinY1.iMin);
// 		YMax = max(pSL1->MaxMinY1.iMax,pSL2->MaxMinY1.iMax);
// // 		int i;
// // 		for (i=0; i<pointsCnt; i++)
// // 		{
// // 			*(pX+i) = i;
// // 			*(pY1+i) = rand() * 100 / float(RAND_MAX);
// // 			*(pY2+i) = rand() * 100 / float(RAND_MAX);
// // 		}
// // 		XMin = 0; XMax = pointsCnt;
// // 		YMin = YMax = *pY1;
// // 		for (i=0; i<pointsCnt; i++)
// // 		{
// // 			if (YMax < *(pY1+i))
// // 			{
// // 				YMax = *(pY1+i);
// // 			}
// // 			if (YMin > *(pY1+i))
// // 			{
// // 				YMin = *(pY1+i);
// // 			}
// // 			if (YMax < *(pY2+i))
// // 			{
// // 				YMax = *(pY2+i);
// // 			}
// // 			if (YMin > *(pY2+i))
// // 			{
// // 				YMin = *(pY2+i);
// // 			}
// //
// // 		}
// 	}
// }

void CEchoDraw::Draw(CDC* pDC) {
	if (DrawMethod == 0) {
		Draw0(pDC);
	}
	else {
		Draw1(pDC);
	}
}