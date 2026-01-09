// ImgWnd.cpp : implementation file
//

#include "stdafx.h"

#include <math.h>
#include "ImgWnd.h"
#include "413App.h"
#include "Data.h"
#include "GlobalDeviceContainer.h"
#include <atlimage.h> // Required for CImage

#define PIE180 0.0174532925199432957692369076849

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImgWnd

CImgWnd::CImgWnd() {
	pPackedDIB1 = new CPackedDIB;
	pPackedDIB2 = new CPackedDIB;
	pOverlayDIB1 = new CPackedDIB;
	pOverlayDIB2 = new CPackedDIB;

	pPackedDIB1->pDIB = NULL;
	pPackedDIB2->pDIB = NULL;
	pOverlayDIB1->pDIB = NULL;
	pOverlayDIB2->pDIB = NULL;

	bShowScale = TRUE;
	bShowOverlay = TRUE;
	Oly1CX = Oly1CY = Oly2CX = Oly2CY = 0;
	//bRun = FALSE;
	bStore = FALSE;
	bSaveDIB = FALSE;
	bStoreComposite = FALSE;

	bAutoExpose = FALSE;
	bDispImage = TRUE;

	FileName = "";
	FileBase = "";
	FileDirc = "";

	ClearTextInfo();
}

CImgWnd::~CImgWnd() {
	ClearDIB();
	ClearDIB2();

	delete pPackedDIB1;
	delete pPackedDIB2;
	delete pOverlayDIB1;
	delete pOverlayDIB2;
}

BEGIN_MESSAGE_MAP(CImgWnd, CStatic)
	//{{AFX_MSG_MAP(CImgWnd)
	ON_WM_PAINT()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()//03142025_2
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgWnd message handlers

void CImgWnd::OnPaint() {
	CPaintDC dc(this); // device context for painting
	if (bStoreComposite) {
		// Perform extra paint to store image [7/2/2014 Yuen]
		BOOL bScaleBak = bScaled;
		bScaled = TRUE;
		Draw(&dc);
		bScaled = bScaleBak;
	}
	Draw(&dc);
	ReleaseDC(&dc);
	// Do not call CStatic::OnPaint() for painting messages
}

void CImgWnd::ClearDIB() {
	if (pPackedDIB1->pDIB) {
		if (pPackedDIB1->pDIB->pDIBBits) {
			delete[] pPackedDIB1->pDIB->pDIBBits;
		}
		delete pPackedDIB1->pDIB;
		pPackedDIB1->pDIB = NULL;
	}
	ClearOly(1);
	ClearOly(2);
	//Refresh();	// Cannot refresh here [6/8/2014 Yuen]
}

void CImgWnd::ClearDIB2() {
	if (pPackedDIB2->pDIB) {
		if (pPackedDIB2->pDIB->pDIBBits) {
			delete[] pPackedDIB2->pDIB->pDIBBits;
		}
		delete pPackedDIB2->pDIB;
		pPackedDIB2->pDIB = NULL;
	}
}

void CImgWnd::ClearOly(int n) {
	switch (n) {
	case 1:
		if (pOverlayDIB1->pDIB) {
			if (pOverlayDIB1->pDIB->pDIBBits) {
				delete[] pOverlayDIB1->pDIB->pDIBBits;
			}
			delete pOverlayDIB1->pDIB;
			pOverlayDIB1->pDIB = NULL;
		}
		break;
	default:
		if (pOverlayDIB2->pDIB) {
			if (pOverlayDIB2->pDIB->pDIBBits) {
				delete[] pOverlayDIB2->pDIB->pDIBBits;
			}
			delete pOverlayDIB2->pDIB;
			pOverlayDIB2->pDIB = NULL;
		}
		break;
	}
}

void CImgWnd::ClearOverlay() {
	ClearOly(1);
	ClearOly(2);
	Refresh();
}

void CImgWnd::Add(CPackedDIB* pDIB) {
	ClearDIB();
	if (!pDIB || !pDIB->pDIB) {
		return;
	}
	pPackedDIB1->pDIB = pDIB->pDIB;
}

void CImgWnd::DupStore1(CPackedDIB& DIB) {
	ClearDIB();	// Clear first [4/16/2013 Administrator]
	if (!DIB.pDIB || !DIB.pDIB->pDIBBits) {
		Invalidate();
		return;
	}
	ClearDIB();
	pPackedDIB1->CreateBlank();
	pPackedDIB1->pDIB->BmpInfo = DIB.pDIB->BmpInfo;
	pPackedDIB1->CalcBytePerRow();
	int SizeImage = pPackedDIB1->BytesPerRow * pPackedDIB1->IHeight;
	pPackedDIB1->pDIB->pDIBBits = new BYTE[SizeImage];
	memcpy(pPackedDIB1->pDIB->pDIBBits, DIB.pDIB->pDIBBits, SizeImage);
	Invalidate();
}

void CImgWnd::DupStore2(CPackedDIB& DIB) {
	ClearDIB2();	// Clear first [4/16/2013 Administrator]
	if (!DIB.pDIB || !DIB.pDIB->pDIBBits) {
		Invalidate();
		return;
	}
	ClearDIB2();
	pPackedDIB2->CreateBlank();
	pPackedDIB2->pDIB->BmpInfo = DIB.pDIB->BmpInfo;
	pPackedDIB2->CalcBytePerRow();
	int SizeImage = pPackedDIB2->BytesPerRow * pPackedDIB2->IHeight;
	pPackedDIB2->pDIB->pDIBBits = new BYTE[SizeImage];
	memcpy(pPackedDIB2->pDIB->pDIBBits, DIB.pDIB->pDIBBits, SizeImage);
}

void CImgWnd::Refresh() {
	CDC* pDC = GetDC();
	if (bStoreComposite) {
		// Perform extra paint to store image [7/2/2014 Yuen]
		BOOL bScaleBak = bScaled;
		bScaled = TRUE;
		Draw(pDC);
		bScaled = bScaleBak;
	}
	Draw(pDC);
	ReleaseDC(pDC);
}

CPackedDIB* CImgWnd::CreateDIB(int wd, int ht, int bpp) {
	CPackedDIB* p = new CPackedDIB;
	if (p) {
		p->pDIB->BmpInfo.biBitCount = bpp;
		p->pDIB->BmpInfo.biHeight = ht;
		p->pDIB->BmpInfo.biWidth = (wd * 4) / 4;
		p->pDIB->BmpInfo.biPlanes = 1;
		p->pDIB->BmpInfo.biCompression = 0;
		p->pDIB->BmpInfo.biSize = sizeof(BITMAPINFOHEADER);
		p->pDIB->BmpInfo.biXPelsPerMeter = p->pDIB->BmpInfo.biYPelsPerMeter = 0;
		p->pDIB->BmpInfo.biClrUsed = p->pDIB->BmpInfo.biClrImportant = 0;
		p->CalcBytePerRow();
		p->pDIB->BmpInfo.biSizeImage = p->BytesPerRow * abs(p->IHeight);
		p->pDIB->pDIBBits = new BYTE[p->pDIB->BmpInfo.biSizeImage];
	}
	return p;
}

void CImgWnd::Clean() {
	if (!pPackedDIB1 || !pPackedDIB1->pDIB || !pPackedDIB1->pDIB->pDIBBits) {
		return;
	}

	SBGR* pB = (SBGR*)pPackedDIB1->pDIB->pDIBBits;

	for (int i = 0; i < abs(pPackedDIB1->IHeight); i++) {
		for (int j = 0; j < pPackedDIB1->IWidth; j++) {
			pB->Grey(pB->I());
			pB++;
		}
	}
	Invalidate();
}

//20220616/Haque 
//Set min and max pixel value for the outer and inside bevel
short CImgWnd::DetectBevel(int& idx, short* pBuf, int nBuf, float ang, short offset, BOOL& bX, short left, short right)
{
	// ensure ang to be within 0 and 360 but not including 360 [9/14/2011 Administrator]
	while (ang < 0) { ang += 360; }
	while (ang >= 360) { ang -= 360; }

	idx = -1; // indicate offset into pBuf the point at dx,dy [4/21/2013 Administrator]

	float dx, dy;		// Coordinate of the center point of the line [4/21/2013 Administrator]
	float da = ang + 90;
	while (da < 0) { da += 360; }
	while (da >= 360) { da -= 360; }

	p413App->Global.Polar2Cartesian(offset, da, dx, dy);  // Find displacement of offset radius at 90 degree to ang [9/12/2011 Administrator]

	float m;
	int x, y;
	float cx, cy;
	int siz = 0;
	SIPackedDIB* p = pPackedDIB1->pDIB;

	// Center of image [4/21/2013 Administrator]
	cx = pPackedDIB1->IWidth / 2.0f;
	cy = pPackedDIB1->IHeight / 2.0f;

	// Center of line [4/21/2013 Administrator]
	int cxl = cx + dx;
	int cyl = cy + dy;

	int imgHeight = pPackedDIB1->IHeight;
	// Note. top left corner is 0,0 in image space, trim calculation uses 0,0 coordinate at center of image [9/14/2011 Administrator]
	if (ang == 0) {
		// y is alway zero [9/14/2011 Administrator]
		bX = TRUE;
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if(siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 45) {
		bX = TRUE;
		m = float(tan(ang * 3.1415f / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(m * (x - cx - dx) + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if (siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 90) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if (siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 90) {
		bX = FALSE;
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return 0;
		}
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			if (siz < nBuf) {
				*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
				if (siz >= left && siz <= right)
				{
					*(pBuf++) = 0;
				}
				else
				{
					*(pBuf++) = 255;
				}
				siz++;
			}
		}
	}
	else if (ang < 135) {
		bX = FALSE;
		m = float(-tan((180 - ang) * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if (siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 180) {
		bX = TRUE;
		m = float(tan((ang) * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if (siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 180) {
		bX = TRUE;
		y = int(cy + dy);
		if ((y < 0) || (y >= pPackedDIB1->IHeight)) {
			return 0;	// Out of bound [4/13/2013 Administrator]
		}
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			if (siz < nBuf) {
				*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
				if (siz >= left && siz <= right)
				{
					*(pBuf++) = 0;
				}
				else
				{
					*(pBuf++) = 255;
				}
				siz++;
			}
		}
	}
	else if (ang < 225) {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if (siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 270) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if (siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 270) {
		bX = FALSE;
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return 0;
		}
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			if (siz < nBuf) {
				*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
				if (siz >= left && siz <= right)
				{
					*(pBuf++) = 0;
				}
				else
				{
					*(pBuf++) = 255;
				}
				siz++;
			}
		}
	}
	else if (ang < 315) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if (siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	else {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if (siz >= left && siz <= right)
					{
						*(pBuf++) = 0;
					}
					else
					{
						*(pBuf++) = 255;
					}
					siz++;
				}
			}
		}
	}
	return siz;
}

short CImgWnd::ExtractLine(int& idx, short* pBuf, int nBuf, float ang, short offset, BOOL& bX/*, BYTE r, BYTE g, BYTE b*/) {
	// ensure ang to be within 0 and 360 but not including 360 [9/14/2011 Administrator]
	while (ang < 0) { ang += 360; }
	while (ang >= 360) { ang -= 360; }

	idx = -1; // indicate offset into pBuf the point at dx,dy [4/21/2013 Administrator]

	float dx, dy;		// Coordinate of the center point of the line [4/21/2013 Administrator]
	float da = ang + 90;
	while (da < 0) { da += 360; }
	while (da >= 360) { da -= 360; }

	p413App->Global.Polar2Cartesian(offset, da, dx, dy);  // Find displacement of offset radius at 90 degree to ang [9/12/2011 Administrator]

	float m;
	int x, y;
	float cx, cy;
	int siz = 0;
	SIPackedDIB* p = pPackedDIB1->pDIB;

	// Center of image [4/21/2013 Administrator]
	cx = pPackedDIB1->IWidth / 2.0f;
	cy = pPackedDIB1->IHeight / 2.0f;

	// Center of line [4/21/2013 Administrator]
	int cxl = cx + dx;
	int cyl = cy + dy;

	int imgHeight = pPackedDIB1->IHeight;
	// Note. top left corner is 0,0 in image space, trim calculation uses 0,0 coordinate at center of image [9/14/2011 Administrator]
	if (ang == 0) {
		// y is alway zero [9/14/2011 Administrator]
		bX = TRUE;
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (x >= cxl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 45) {
		bX = TRUE;
		m = float(tan(ang * 3.1415f / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(m * (x - cx - dx) + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (x >= cxl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 90) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (y >= cyl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 90) {
		bX = FALSE;
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return 0;
		}
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			if (siz < nBuf) {
				*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
				if ((idx == -1) && (y >= cyl)) {
					idx = siz;
				}
				siz++;
			}
		}
	}
	else if (ang < 135) {
		bX = FALSE;
		m = float(-tan((180 - ang) * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (y >= cyl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 180) {
		bX = TRUE;
		m = float(tan((ang) * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (x <= cxl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 180) {
		bX = TRUE;
		y = int(cy + dy);
		if ((y < 0) || (y >= pPackedDIB1->IHeight)) {
			return 0;	// Out of bound [4/13/2013 Administrator]
		}
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			if (siz < nBuf) {
				*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
				if ((idx == -1) && (x <= cxl)) {
					idx = siz;
				}
				siz++;
			}
		}
	}
	else if (ang < 225) {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (x <= cxl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 270) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (y <= cyl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 270) {
		bX = FALSE;
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return 0;
		}
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			if (siz < nBuf) {
				*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
				if ((idx == -1) && (y <= cyl)) {
					idx = siz;
				}
				siz++;
			}
		}
	}
	else if (ang < 315) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (y <= cyl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	else {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					if ((idx == -1) && (x >= cxl)) {
						idx = siz;
					}
					siz++;
				}
			}
		}
	}
	return siz;
}

//20220616/Haque Find the green color position so that we get the bevel edge position
bool CImgWnd::FindColor(int& idx, int nBuf, float ang, short offset, BOOL& bX, short& left, short& right)
{
	while (ang < 0) { ang += 360; }
	while (ang >= 360) { ang -= 360; }

	idx = -1; // indicate offset into pBuf the point at dx,dy [4/21/2013 Administrator]
	left = -5000;
	right = -5000;

	float dx, dy;		// Coordinate of the center point of the line [4/21/2013 Administrator]
	float da = ang + 90;
	while (da < 0) { da += 360; }
	while (da >= 360) { da -= 360; }

	p413App->Global.Polar2Cartesian(offset, da, dx, dy);  // Find displacement of offset radius at 90 degree to ang [9/12/2011 Administrator]

	float m;
	int x, y;
	float cx, cy;
	int siz = 0;
	SIPackedDIB* p = pPackedDIB1->pDIB;

	// Center of image [4/21/2013 Administrator]
	cx = pPackedDIB1->IWidth / 2.0f;
	cy = pPackedDIB1->IHeight / 2.0f;

	// Center of line [4/21/2013 Administrator]
	int cxl = cx + dx;
	int cyl = cy + dy;

	int imgHeight = pPackedDIB1->IHeight;

	int cntGreen = 0;
	short dirX[4] = { 0, 0, 1, -1 };
	short dirY[4] = { 1, -1, 0, 0 };
	// Note. top left corner is 0,0 in image space, trim calculation uses 0,0 coordinate at center of image [9/14/2011 Administrator]
	if (ang == 0) {
		// y is alway zero [9/14/2011 Administrator]
		bX = TRUE;
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					//*(pBuf++) = pPackedDIB1->GetPixelL(short(x), short(pPackedDIB1->IHeight - y - 1));
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 45) {
		bX = TRUE;
		m = float(tan(ang * 3.1415f / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(m * (x - cx - dx) + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 90) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 90) {
		bX = FALSE;
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return 0;
		}
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			if (siz < nBuf) {
				short green = 0;
				for (int i = 0; i < 4; i++)
				{
					if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
					{
						green++;
					}
				}
				if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
				{
					if (left == -5000) left = siz;
					right = siz;
					cntGreen++;
				}
				siz++;
			}
		}
	}
	else if (ang < 135) {
		bX = FALSE;
		m = float(-tan((180 - ang) * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 180) {
		bX = TRUE;
		m = float(tan((ang) * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 180) {
		bX = TRUE;
		y = int(cy + dy);
		if ((y < 0) || (y >= pPackedDIB1->IHeight)) {
			return 0;	// Out of bound [4/13/2013 Administrator]
		}
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			if (siz < nBuf) {
				short green = 0;
				for (int i = 0; i < 4; i++)
				{
					if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
					{
						green++;
					}
				}
				if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
				{
					if (left == -5000) left = siz;
					right = siz;
					cntGreen++;
				}
				siz++;
			}
		}
	}
	else if (ang < 225) {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	else if (ang < 270) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	else if (ang == 270) {
		bX = FALSE;
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return 0;
		}
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			if (siz < nBuf) {
				short green = 0;
				for (int i = 0; i < 4; i++)
				{
					if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
					{
						green++;
					}
				}
				if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
				{
					if (left == -5000) left = siz;
					right = siz;
					cntGreen++;
				}
				siz++;
			}
		}
	}
	else if (ang < 315) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				if (siz < nBuf) {
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	else {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				if (siz < nBuf) {
					short green = 0;
					for (int i = 0; i < 4; i++)
					{
						if ((x + dirX[i] >= 5 && x + dirX[i] < pPackedDIB1->IWidth - 5 && y - dirY[i] >= 5 && y - dirY[i] < pPackedDIB1->IHeight - 5) && pPackedDIB1->IsGreen(short(x + dirX[i]), short(pPackedDIB1->IHeight - y - dirY[i] - 1)))
						{
							green++;
						}
					}
					if (pPackedDIB1->IsGreen(short(x), short(pPackedDIB1->IHeight - y - 1)) || green >= 2)
					{
						if (left == -5000) left = siz;
						right = siz;
						cntGreen++;
					}
					siz++;
				}
			}
		}
	}
	if (cntGreen >= 2)
	{
		return true;
	}
	return false;
}

void CImgWnd::SetLineColor(float ang, float offset, BYTE R, BYTE G, BYTE B) {
	// ensure ang to be within 0 and 360 but not including 360 [9/14/2011 Administrator]
	while (ang < 0) {
		ang += 360;
	}
	while (ang >= 360) {
		ang -= 360;
	}

	float da = ang + 90.0f;
	if (da < 0) {
		da += 360;
	}
	if (da >= 360) {
		da -= 360;
	}

	float dx, dy;
	p413App->Global.Polar2Cartesian(offset, da, dx, dy);  // Find displacement of offset radius at 90 degree to ang [9/12/2011 Administrator]

	float m;
	int x, y;
	float cx, cy;
	int siz = 0;
	SIPackedDIB* p = pPackedDIB1->pDIB;

	cx = pPackedDIB1->IWidth / 2.0f;
	cy = pPackedDIB1->IHeight / 2.0f;

	// Note. top left corner is 0,0 in image space, trim calculation uses 0,0 coordinate at center of image [9/14/2011 Administrator]
	if (ang == 0) {
		// y is alway zero [9/14/2011 Administrator]
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang <= 45) {
		m = float(tan(ang * 3.1415f / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(m * (x - cx - dx) + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang < 90) {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang == 90) {
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return;
		}
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
		}
	}
	else if (ang < 135) {
		m = float(-tan((180 - ang) * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang < 180) {
		m = float(tan((ang) * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang == 180) {
		y = int(cy + dy);
		if ((y < 0) || (y >= pPackedDIB1->IHeight)) {
			return;	// Out of bound [4/13/2013 Administrator]
		}
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
		}
	}
	else if (ang < 225) {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang < 270) {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang == 270) {
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return;
		}
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
		}
	}
	else if (ang <= 315) {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
}

void CImgWnd::UpdateLineInfo(float ang, float offset, bool* lineInfo)
{
	// ensure ang to be within 0 and 360 but not including 360 [9/14/2011 Administrator]
	while (ang < 0) {
		ang += 360;
	}
	while (ang >= 360) {
		ang -= 360;
	}

	float da = ang + 90.0f;
	if (da < 0) {
		da += 360;
	}
	if (da >= 360) {
		da -= 360;
	}

	float dx, dy;
	p413App->Global.Polar2Cartesian(offset, da, dx, dy);  // Find displacement of offset radius at 90 degree to ang [9/12/2011 Administrator]

	float m;
	int x, y;
	float cx, cy;
	int siz = 0;
	SIPackedDIB* p = pPackedDIB1->pDIB;

	cx = pPackedDIB1->IWidth / 2.0f;
	cy = pPackedDIB1->IHeight / 2.0f;

	// Note. top left corner is 0,0 in image space, trim calculation uses 0,0 coordinate at center of image [9/14/2011 Administrator]
	if (ang == 0) {
		// y is alway zero [9/14/2011 Administrator]
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang <= 45) {
		m = float(tan(ang * 3.1415f / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int(m * (x - cx - dx) + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang < 90) {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang == 90) {
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return;
		}
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
			//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
		}
	}
	else if (ang < 135) {
		m = float(-tan((180 - ang) * 3.1415 / 180.0f));
		for (y = 0; y < pPackedDIB1->IHeight; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang < 180) {
		m = float(tan((ang) * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang == 180) {
		y = int(cy + dy);
		if ((y < 0) || (y >= pPackedDIB1->IHeight)) {
			return;	// Out of bound [4/13/2013 Administrator]
		}
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
			//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
		}
	}
	else if (ang < 225) {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = pPackedDIB1->IWidth - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang < 270) {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else if (ang == 270) {
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return;
		}
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
			//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
		}
	}
	else if (ang <= 315) {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = pPackedDIB1->IHeight - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < pPackedDIB1->IWidth)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
	else {
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = 0; x < pPackedDIB1->IWidth; x++) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < pPackedDIB1->IHeight)) {
				lineInfo[(short(x) * pPackedDIB1->IHeight) + short(pPackedDIB1->IHeight - y - 1)] = true;
				//pPackedDIB1->SetPixelRGB(short(x), short(pPackedDIB1->IHeight - y - 1), R, G, B);
			}
		}
	}
}

void CImgWnd::Save(CString& name) {
	pPackedDIB1->Save(name);
}

BOOL CImgWnd::DestroyWindow() {
	ClearDIB();
	return CStatic::DestroyWindow();
}

void CImgWnd::ShowEchoView(CDC* pDC, CRect& rc, short sourceWd) {
	if (!pDC) {
		return;
	}

	CString str;
	C413Global* g = &p413App->Global;

	float Xa;
	float sfx;
	float ScrnX;
	switch (g->ActiveCamera) {
	default:
		Xa = g->ScaleEcho;
		ScrnX = g->ScrnCalibECH.ScrnX;
		break;
	case CCamBase::THINFCAM:
		Xa = g->ScaleTF;
		ScrnX = g->ScrnCalibTF.ScrnX;
		break;
	}

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 0, 0));
	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);

	int Y = rc.bottom - 2;

	if (bShowScale) {
		if (bScaled) {
			sfx = ScrnX / float(rc.Width());
		}
		else {
			sfx = ScrnX / float(sourceWd);
		}
		short wd;
		switch (g->ActiveCamera) {
		default:
			wd = Xa * g->ScrnCalibECH.ppmX / sfx * 1e3;
			break;
		case CCamBase::THINFCAM:
			wd = Xa * g->ScrnCalibTF.ppmX / sfx;
			break;
		}
		CRect rcScale;
		short dw = (rc.Width() - wd) / 2;
		rcScale.SetRect(dw, rc.bottom - 4, wd + dw, rc.bottom - 12);
		pDC->Rectangle(rcScale);

		str.Format("%.3f mm", Xa);
		pDC->TextOut(rc.left, Y, str);
		Y -= 16;

		str.Format("N:%d   X:%.3f Y:%.3f EchoTopZ:%.2f RoughZ:%.2f ", PointNum, g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, g->LocRoughZ);
		pDC->TextOut(rc.left, Y, str);
		Y -= 18;

		if (pAve && pRcp) {
			for (int i = 0; i < MaXPA; i++) {
				MParam* pa = &pRcp->MeParam[0].MPa[i];	// @@@XXX should replay [0] with actual info [6/30/2013 Yuen]
				if (pa->Name.GetLength()) {
					str.Format("%s %.3f um", pa->Name, pAve[i]);
					pDC->TextOut(rc.left, Y, str);
					Y -= 18;
				}
			}
		}
		CString str1 = "";
		if (nTWEdge1 != BADDATANEG) {
			str.Format("CX: %d px %.3f mm   CY: %d px %.3f mm", nTWEdge1, fTWEdge1, nTWEdge2, fTWEdge2);
			str1 += str;
		}
		if (nTWEdge2 != BADDATANEG) {
			str.Format("DL: %d px %.3f mm", nTWEdge2 - nTWEdge1, fTWEdge2 - fTWEdge1);
			str1 += str;
		}
		if (str1.GetLength() > 0) {
			pDC->TextOut(rc.left, Y, str1);
			Y -= 18;
		}
		str1 = "";
		if (TWRadius != BADDATANEG) {
			str.Format("Radius: %.3f mm Angle: %.2f ", TWRadius, TWAngle);
			str1 += str;
		}
		if (TWWidth != BADDATANEG) {
			str.Format("Width: %.3f mm ", TWWidth);
			str1 += str;
		}
		if (TWBevel != BADDATANEG) {
			str.Format("Bevel: %.3f mm", TWBevel);
			str1 += str;
		}
		if (str1.GetLength() > 0) {
			pDC->TextOut(rc.left, Y, str1);
			Y -= 18;
		}
	}
}

#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')

void CImgWnd::StoreComposite(CBitmap& bmp, CString FileName) {
	CString path = DosUtil.DirSys.ExtractFilePath(FileName);
	DosUtil.DirSys.GotoDirec(path);

	CFile file;
	if (!file.Open(FileName, CFile::modeWrite | CFile::modeCreate)) {
		return;
	}
	BITMAP bitmapInfo;
	bmp.GetBitmap(&bitmapInfo);

	long BytesPerRow = bitmapInfo.bmWidth * (bitmapInfo.bmBitsPixel / 8);
	BytesPerRow += (4 - BytesPerRow % 4) % 4;

	BITMAPINFOHEADER BitmapHead;
	ZeroMemory(&BitmapHead, sizeof(BITMAPINFOHEADER));
	BitmapHead.biWidth = bitmapInfo.bmWidth;
	BitmapHead.biHeight = -bitmapInfo.bmHeight;
	BitmapHead.biPlanes = 1;
	BitmapHead.biBitCount = bitmapInfo.bmBitsPixel;
	BitmapHead.biSizeImage = BytesPerRow * abs(bitmapInfo.bmHeight);
	BitmapHead.biSize = sizeof(BITMAPINFOHEADER);
	BitmapHead.biClrUsed = 0;
	BitmapHead.biClrImportant = 0;
	BitmapHead.biCompression = BI_RGB;

	// assumes 256 gray level
	// 	int i;
	BITMAPFILEHEADER   hdr;
	DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(hdr) + 256 * sizeof(RGBQUAD);
	RGBQUAD* pRGB = new RGBQUAD[256];
	for (int i = 0; i < 256; i++) {
		pRGB[i].rgbRed = i;
		pRGB[i].rgbGreen = i;
		pRGB[i].rgbBlue = i;
	}
	// Fill in the fields of the file header
	hdr.bfType = DS_BITMAP_FILEMARKER;
	hdr.bfSize = dwFileHeaderSize + BitmapHead.biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = dwFileHeaderSize;

	// Write the file header
	file.Write(&hdr, sizeof(BITMAPFILEHEADER));
	// Write the DIB header
	file.Write(&BitmapHead, sizeof(BITMAPINFOHEADER));
	//  [8/11/2010 XPMUser]
	file.Write(pRGB, sizeof(RGBQUAD) * 256);
	//  [8/11/2010 XPMUser]

	// 	int iByteToTransfer;
	// 	iByteToTransfer = bitmapInfo.bmWidth * bitmapInfo.bmHeight * bitmapInfo.bmBitsPixel /8;
	UCHAR* buf3 = new UCHAR[/*iByteToTransfer*/BitmapHead.biSizeImage];
	bmp.GetBitmapBits(BitmapHead.biSizeImage, buf3);
	file.Write(buf3, BitmapHead.biSizeImage);
	file.Close();
	if (buf3) {
		delete[] buf3;
	}
	if (pRGB) {
		delete[] pRGB;
	}
}

void CImgWnd::Draw(CDC* pDC) {
	if (!pPackedDIB1->pDIB || !pPackedDIB1->pDIB->pDIBBits) {
		return;
	}
	int w = pPackedDIB1->pDIB->BmpInfo.biWidth;
	int h = abs(pPackedDIB1->pDIB->BmpInfo.biHeight);

	CRect rc;
	GetClientRect(&rc);
	if (bStoreComposite) {
		rc.SetRect(0, 0, w, h);
	}

	//int sx,sy;
	CDC dcMem;
	CBitmap bmp;
	HBITMAP oldBmp;

	dcMem.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, (int)(rc.Width()), (int)(rc.Height()));
	oldBmp = (HBITMAP)dcMem.SelectObject(bmp);

	HDC hDC = dcMem.GetSafeHdc();
	::SetStretchBltMode(hDC, COLORONCOLOR);

	CRgn Rg;
	Rg.CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
	dcMem.SelectClipRgn(&Rg);

	if (!bStoreComposite) {
		CBrush br(RGB(200, 200, 200));
		//dcMem.FillRect(rc,&br);
		pDC->FillRect(rc, &br);
	}

	int rcw = rc.Width();
	int rch = rc.Height();

	// 	if (pPackedDIB1->pDIB && pPackedDIB1->pDIB->pDIBBits)
	// 	{
	int sx, sy;
	if ((w > 0) && (h > 0)) {
		sx = (rcw - w) / 2;
		sy = (rch - h) / 2;
		short BPP = pPackedDIB1->pDIB->BmpInfo.biBitCount / 8;
		memset(pPackedDIB1->pDIB->pDIBBits, 255, 2 * BPP);
		if (bScaled) {
			::StretchDIBits(dcMem.GetSafeHdc(),
				0, 0, rcw, rch,
				0, 0, w, h,
				pPackedDIB1->pDIB->pDIBBits,
				(BITMAPINFO*)(&pPackedDIB1->pDIB->BmpInfo),
				DIB_RGB_COLORS, SRCCOPY);
		}
		else {
			::StretchDIBits(dcMem.GetSafeHdc(),
				sx, sy, w, h,
				0, 0, w, h,
				pPackedDIB1->pDIB->pDIBBits,
				(BITMAPINFO*)(&pPackedDIB1->pDIB->BmpInfo),
				DIB_RGB_COLORS, SRCCOPY);
		}
	}
	if (pOverlayDIB1->pDIB && pOverlayDIB1->pDIB->pDIBBits) {
		int ow = pOverlayDIB1->pDIB->BmpInfo.biWidth;
		int oh = abs(pOverlayDIB1->pDIB->BmpInfo.biHeight);

		if ((ow > 0) && (oh > 0)) {
			sx = (rcw - w) / 2 + Oly1CX / 2 - ow / 2;
			sy = (rch - h) / 2 + Oly1CY / 2 - oh / 2;
			short BPP = pOverlayDIB1->pDIB->BmpInfo.biBitCount / 8;
			memset(pOverlayDIB1->pDIB->pDIBBits, 0, 2 * BPP);
			::StretchDIBits(dcMem.GetSafeHdc(),
				sx, sy, ow, oh,
				0, 0, ow, oh,
				pOverlayDIB1->pDIB->pDIBBits,
				(BITMAPINFO*)(&pOverlayDIB1->pDIB->BmpInfo),
				DIB_RGB_COLORS, SRCCOPY);
		}
	}
	if (pOverlayDIB2->pDIB && pOverlayDIB2->pDIB->pDIBBits) {
		int ow = pOverlayDIB2->pDIB->BmpInfo.biWidth;
		int oh = abs(pOverlayDIB2->pDIB->BmpInfo.biHeight);

		if ((ow > 0) && (oh > 0)) {
			sx = (rcw - w) / 2 + Oly2CX / 2 - ow / 2;
			sy = (rch - h) / 2 + Oly2CY / 2 - oh / 2;
			short BPP = pOverlayDIB2->pDIB->BmpInfo.biBitCount / 8;
			memset(pOverlayDIB2->pDIB->pDIBBits, 0, 2 * BPP);
			::StretchDIBits(dcMem.GetSafeHdc(),
				sx, sy, ow, oh,
				0, 0, ow, oh,
				pOverlayDIB2->pDIB->pDIBBits,
				(BITMAPINFO*)(&pOverlayDIB2->pDIB->BmpInfo),
				DIB_RGB_COLORS, SRCCOPY);
		}
	}
	if (bShowOverlay) ShowEchoView(&dcMem, rc, pPackedDIB1->pDIB->BmpInfo.biWidth);
	if (bStoreComposite) {
		StoreComposite(bmp, SaveName);
		bStoreComposite = FALSE;
	}
	else {
		pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
	}
	// 	}

	dcMem.SelectObject(oldBmp);
	ReleaseDC(&dcMem);
}

void CImgWnd::ReplaceLine(short* pBuf, float ang, short offset, BOOL& bX) {
	// ensure ang to be within 0 and 360 but not including 360 [9/14/2011 Administrator]
	while (ang < 0) {
		ang += 360;
	}
	while (ang >= 360) {
		ang -= 360;
	}

	float dx, dy;
	p413App->Global.Polar2Cartesian(offset, ang + 90, dx, dy);  // Find displacement of offset radius at 90 degree to ang [9/12/2011 Administrator]

	float m;
	int ht, ht1, wd, cx, cy;
	int x, y;
	SIPackedDIB* p = pPackedDIB1->pDIB;

	wd = p->BmpInfo.biWidth;
	ht = abs(p->BmpInfo.biHeight);
	ht1 = ht - 1;
	cx = wd / 2;
	cy = ht / 2;

	// Note. top left corner is 0,0 in image space, trim calculation uses 0,0 coordinate at center of image [9/14/2011 Administrator]
	if (ang == 0) {
		// y is alway zero [9/14/2011 Administrator]
		bX = TRUE;
		for (x = 0; x < wd; x++) {
			y = int(cy + dy);
			if ((y >= 0) && (y < ht)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
	else if (ang <= 45) {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = 0; x < wd; x++) {
			y = int(m * (x - cx - dx) + cy + dy);
			if ((y >= 0) && (y < ht)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
	else if (ang < 90) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = 0; y < ht; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < wd)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
	else if (ang == 90) {
		bX = FALSE;
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return;
		}
		for (y = 0; y < ht; y++) {
			pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
		}
	}
	else if (ang < 135) {
		bX = FALSE;
		m = float(-tan((180 - ang) * 3.1415 / 180.0f));
		for (y = 0; y < ht; y++) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < wd)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
	else if (ang < 180) {
		bX = TRUE;
		m = float(tan((ang) * 3.1415 / 180.0f));
		for (x = wd - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < ht)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
	else if (ang == 180) {
		bX = TRUE;
		y = int(cy + dy);
		if ((y < 0) || (y >= pPackedDIB1->IHeight)) {
			return;
		}
		for (x = wd - 1; x >= 0; x--) {
			pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
		}
	}
	else if (ang < 225) {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = wd - 1; x >= 0; x--) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < ht)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
	else if (ang < 270) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = ht - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < wd)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
	else if (ang == 270) {
		bX = FALSE;
		x = int(cx + dx);
		if ((x < 0) || (x >= pPackedDIB1->IWidth)) {
			return;
		}
		for (y = ht - 1; y >= 0; y--) {
			pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
		}
	}
	else if (ang <= 315) {
		bX = FALSE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (y = ht - 1; y >= 0; y--) {
			x = int((y - (cy + dy)) / m + cx + dx);
			if ((x >= 0) && (x < wd)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
	else {
		bX = TRUE;
		m = float(tan(ang * 3.1415 / 180.0f));
		for (x = 0; x < wd; x++) {
			y = int((x - (cx + dx)) * m + cy + dy);
			if ((y >= 0) && (y < ht)) {
				pPackedDIB1->SetPixelRed(x, ht1 - y, *(pBuf++));
			}
		}
	}
}

int CImgWnd::GetBytesPerRow() {
	if (!pPackedDIB1->pDIB) {
		// precaution [11/8/2011 Administrator]
		ASSERT(0);
		return 0;
	}
	long BytesPerRow = pPackedDIB1->pDIB->BmpInfo.biWidth * pPackedDIB1->pDIB->BmpInfo.biBitCount / 8;
	BytesPerRow += (4 - BytesPerRow % 4) % 4;	// int alignment
	return BytesPerRow;
}

BOOL CImgWnd::LoadDIB() {
	CFileDialog dlg(TRUE, "BMP", NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Bitmap files|*.BMP|");
	if (dlg.DoModal() == IDOK) {
		if (LoadDIB(dlg.GetPathName())) {
			SaveName = dlg.GetPathName();
			Invalidate(TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

void CImgWnd::SetRecipe(CRecipe413* pR) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	pRcp = pR;
	Lock.Unlock();
}

void CImgWnd::SetAve(float* pA) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	pAve = pA;
	Lock.Unlock();
}

//void CImgWnd::OnRButtonUp(UINT nFlags, CPoint point) {
//	LoadDIB();
//}


//[03142025_2
void CImgWnd::OnRButtonUp(UINT nFlags, CPoint point) {
	CMenu menu;
	CMenu* pPopUp;
	menu.LoadMenu(IDR_MENU413);
	pPopUp = menu.GetSubMenu(4);
	if (!pPopUp) return;



	pPopUp->DeleteMenu(ID_PLACEREDXHAIR, MF_BYCOMMAND | MF_ENABLED);
	pPopUp->DeleteMenu(ID_PLACEGREENXHAIR, MF_BYCOMMAND | MF_DISABLED);
	pPopUp->DeleteMenu(ID_PLACEBLUEXHAIR, MF_BYCOMMAND | MF_ENABLED);
	pPopUp->DeleteMenu(ID_HIDEALLCROSSHAIR, MF_BYCOMMAND | MF_ENABLED);
	pPopUp->DeleteMenu(ID_SHOWYELLOW, MF_BYCOMMAND | MF_ENABLED);
	pPopUp->DeleteMenu(ID_SHOWRGBXHAIR, MF_BYCOMMAND | MF_ENABLED);
	pPopUp->DeleteMenu(ID_TAKEPICTUREOVERLAY, MF_BYCOMMAND | MF_ENABLED);

	CRect rc;
	C413Global* g = &p413App->Global;

	CPoint Pt = point;
	ClientToScreen(&Pt);
	int res = pPopUp->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, Pt.x, Pt.y, this);
	switch (res) {
	case ID_TAKEPICTUREOVERLAY:
		FileDirc.Format("C:\\TEMP\\%s\\", CTime::GetCurrentTime().Format("%y%m%d").GetBuffer());
		FileBase.Format("%s%d.BMP", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer(), int(rand() / float(RAND_MAX) * 100));
		bStoreComposite = TRUE;
		break;

	case ID_TAKEPICTURE:

		FileDirc.Format("C:\\TEMP\\%s\\", CTime::GetCurrentTime().Format("%y%m%d").GetBuffer());
		FileBase.Format("%s%d.BMP", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer(), int(rand() / float(RAND_MAX) * 100));


		CreateDirectory(FileDirc, NULL);

		CDC* pDC = GetDC(); 
		if (!pDC) break;

	
		GetClientRect(&rc);
		int width = rc.Width();
		int height = rc.Height();


		CDC memDC;
		memDC.CreateCompatibleDC(pDC);


		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(pDC, width, height);

		CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
		memDC.BitBlt(0, 0, width, height, pDC, 0, 0, SRCCOPY);

		
		CString fullPath = FileDirc + FileBase;
		CImage image;
		image.Attach((HBITMAP)bitmap.Detach()); 
		image.Save(fullPath);              	

		
		memDC.SelectObject(pOldBitmap);
		ReleaseDC(pDC);

		break;
	}
}
//]



void CImgWnd::OnLButtonUp(UINT nFlags, CPoint point) {
	if (bScaled) {
		bScaled = FALSE;
	}
	else {
		bScaled = TRUE;
	}
	Refresh();
}

BOOL CImgWnd::LoadDIB(CString fname) {
	if (pPackedDIB1->Load(fname)) {
		DupStore2(*pPackedDIB1);
		return TRUE;
	}
	return FALSE;
}

// return 1 if pL1 is valid, 2 if pL2 is valid and 3 if both are valid [6/1/2013 Yuen]
// In and out are average score of intensity count of line between the green line [7/27/2013 Yuen]
short CImgWnd::FindEdge(SLnX* pL1, SLnX* pL2, int& In, int& Out, float angle, FNDMTHD Mth, float Thres) {
	if (pPackedDIB1->pDIB) {
		int idx1;		// Not use [7/27/2013 Yuen]
		short minLen = short(pPackedDIB1->IHeight * 0.25f);
		int siz = 3 * pPackedDIB1->pDIB->BmpInfo.biWidth * (pPackedDIB1->pDIB->BmpInfo.biBitCount / 8);
		short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
		float* pScore = new float[siz];
		if (pBuf) {
			BOOL bX = FALSE;
			short nL = sqrt(pPackedDIB1->IHeight * pPackedDIB1->IHeight + pPackedDIB1->IWidth * pPackedDIB1->IWidth) * 0.75;
			SLnX* pL = new SLnX[nL * 2 + 1];
			float* pS1, * pS = pScore;
			float Offset = 1.0f;//pixel offsets
			angle += 90.0f;
			if (angle >= 360.0f) {
				angle -= 360.0f;
			}
			short i;
			short m = 0;
			SLnX* pLL;
			float maxdev = -1;
			float Smin = 0, Smax = 0;

			pS1 = pScore;
			// Calculate score for every line [5/23/2013 user]
			for (i = -nL; i <= nL; i++) {
				pLL = pL + m;
				pLL->angle = angle;
				pLL->offset = i * Offset;
				short len = ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX);
				if (len > minLen) {
					*pS = pLL->score = ScoreLine1(pBuf, len);
					if (m) {
						if (*pS < Smin) {
							Smin = *pS;
						}
						if (*pS > Smax) {
							Smax = *pS;
						}
					}
					else {
						Smin = Smax = *pS;
					}
					if (m > 2) {
						// search for maxderiv [6/8/2013 Administrator]
						float v = fabs(*pS1 - *pS);
						if (v > maxdev) {
							maxdev = v;
						}
						pS1++;
					}
					m++;
					pS++;
				}
			}

			float v = (Smax - Smin) * Thres;
			Smin = Smin + v;
			Smax = Smax - v;
			FILE* fp = NULL;
			//fp = fopen("C:\\TEMP\\LINE.CSV", "wb");
			pS = pScore;
			for (i = 0; i < m; i++) {
				switch (Mth) {
				case DARKBWD:
				case DARKFWD:
					for (i = 0; i < m; i++) {
						v = (*(pS) < Smin) ? 0 : Smax;
						if (fp) fprintf(fp, "%.4f,%.4f\n", *(pS), v);
						*(pS++) = v;
					}
					break;
				default:
					for (i = 0; i < m; i++) {
						v = (*(pS) > Smax) ? 0 : Smax;
						if (fp) fprintf(fp, "%.4f,%.4f\n", *(pS), v);
						*(pS++) = v;
					}
					break;
				}
			}
			if (fp) fclose(fp);

			SLnX* pL1min = NULL;	// First transition [6/1/2013 Yuen]
			SLnX* pL2min = NULL;	// Subsequent transition [6/1/2013 Yuen]
			short dOff = 3; // Skip a few bytes at the beginning [6/1/2013 Yuen]
			short skip = 0;
			switch (Mth) {
			case DARKFWD:
			case BRITFWD:
				pS = pScore + m - 1 - dOff;
				skip = dOff;
				while (*pS == 0) {
					pS--;
					skip++;
				}
				pS = pScore + m - 1 - skip;
				pL1min = pL + skip;	// First non zero location [6/4/2013 Yuen]
				pL1min->maxderiv = maxdev;
				// First transition [6/1/2013 Yuen]
				if (skip == dOff) {
					// when start with not 0 score [6/8/2013 Yuen]
					for (i = m - 1 - skip; i >= 0 + dOff; i--) {
						if (*pS == 0) {
							pL1min = pL + i;
							pL1min->maxderiv = maxdev;
							break;
						}
						pS--;
					}
					for (; i >= 0 + dOff; i--) {
						if (*pS != 0) {
							pL2min = pL + i;
							break;
						}
						pS--;
					}
				}
				else {
					// when start with 0 score [6/8/2013 Yuen]
					for (i = m - 1 - skip; i >= 0 + dOff; i--) {
						if (*pS == 0) {
							pL2min = pL + i;
							break;
						}
						pS--;
					}
				}
				break;
			case DARKBWD:
			case BRITBWD:
				pS = pScore + dOff;
				skip = dOff;
				while (*pS == 0) {
					pS++;
					skip++;
				}
				// First transition [6/1/2013 Yuen]
				pS = pScore + skip;
				pL1min = pL + skip;	// First non zero location [6/4/2013 Yuen]
				pL1min->maxderiv = maxdev;
				if (skip == dOff) {
					// when start with not 0 score [6/8/2013 Yuen]
					for (i = 0 + skip; i < m - dOff; i++) {
						if (*pS == 0) {
							pL1min = pL + i;
							pL1min->maxderiv = maxdev;
							break;
						}
						pS++;
					}
					for (; i < m - dOff; i++) {
						if (*pS != 0) {
							pL2min = pL + i;
							break;
						}
						pS++;
					}
				}
				else {
					// when start with 0 score [6/8/2013 Yuen]
					for (i = 0 + skip; i < m - dOff; i++) {
						if (*pS == 0) {
							pL2min = pL + i;
							break;
						}
						pS++;
					}
				}
				break;
			}
			short st;
			short score = 0;
			if (pL1min) {
				int ave = 0;
				switch (Mth) {
				case DARKFWD:
				case BRITFWD:
					// Calc In value [7/27/2013 Yuen]
					ave = 0;
					st = pL1min->offset + 10;
					for (i = st; i < (st + 3); i++) {
						ave += ScoreLine1(pBuf, ExtractLine(idx1, pBuf, siz, angle, i, bX));
						//SetLineColor(angle,i,0,0,255);
					}
					In = ave / 6.0f;
					// Calc Out value [7/27/2013 Yuen]
					ave = 0;
					st = pL1min->offset - 6;
					for (i = st; i > st - 3; i--) {
						ave += ScoreLine1(pBuf, ExtractLine(idx1, pBuf, siz, angle, i, bX));
						//SetLineColor(angle,i,255,0,0);
					}
					Out = ave / 3.0f;
					break;
				default:
					// Calc Out value [7/27/2013 Yuen]
					ave = 0;
					st = pL1min->offset + 6;
					for (i = st; i < (st + 3); i++) {
						ave += ScoreLine1(pBuf, ExtractLine(idx1, pBuf, siz, angle, i, bX));
						//SetLineColor(angle,i,255,0,0);
					}
					Out = ave / 3.0f;
					// Calc In value [7/27/2013 Yuen]
					ave = 0;
					st = pL1min->offset - 10;
					for (i = st; i > st - 3; i--) {
						ave += ScoreLine1(pBuf, ExtractLine(idx1, pBuf, siz, angle, i, bX));
						//SetLineColor(angle,i,0,0,255);
					}
					In = ave / 6.0f;
					break;
				}
				for (i = -1; i <= 1; i++) {
					SetLineColor(angle, pL1min->offset + i, 0, 255, 0);
				}
				switch (Mth) {
				case DARKFWD:
				case BRITFWD:
					for (i = -1; i <= 1; i++) {
						SetLineColor(angle, pL1min->offset + 4 + i, 0, 0, 255);
					}
					break;
				case DARKBWD:
				case BRITBWD:
					for (i = 1; i >= -1; i--) {
						SetLineColor(angle, pL1min->offset - 3 + i, 0, 0, 255);
					}
					break;
				}
				score = 1;
				pL1->Copy(pL1min);
			}
			if (pL2min) {
				for (i = -1; i <= 1; i++) {
					SetLineColor(angle, pL2min->offset + i, 255, 255, 0);
				}
				score += 2;
				pL2->Copy(pL2min);
			}
			SetLineColor(angle, 0, 255, 0, 255);

			if (pL) { delete[] pL; pL = NULL; }
			if (pBuf) { delete[] pBuf; pBuf = NULL; }
			if (pScore) { delete[] pScore; pScore = NULL; }

			return score;
		}
		if (pBuf) delete[] pBuf;
		if (pScore) delete[] pScore;
	}
	return 0;
}

short CImgWnd::FindEdgeEx(SLnX* pL1, SLnX* pL2, int& In, int& Out, float angle, FNDMTHD Mth, float Thres) {
	if (pPackedDIB1->pDIB) {
		int idx1;		// Not use [7/27/2013 Yuen]
		short minLen = short(pPackedDIB1->IHeight * 0.25f);
		int siz = 3 * pPackedDIB1->pDIB->BmpInfo.biWidth * (pPackedDIB1->pDIB->BmpInfo.biBitCount / 8);
		short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
		float* pScore = new float[siz];
		if (pBuf) {
			BOOL bX = FALSE;
			short nL = sqrt(pPackedDIB1->IHeight * pPackedDIB1->IHeight + pPackedDIB1->IWidth * pPackedDIB1->IWidth) * 0.75;
			SLnX* pL = new SLnX[nL * 2 + 1];
			float* pS1, * pS = pScore;
			float Offset = 1.0f;
			angle += 90.0f; while (angle >= 360.0f) { angle -= 360.0f; }
			short i;
			short m = 0;
			SLnX* pLL;
			float maxdev = -1;
			float Smin = 0, Smax = 0;

			pS1 = pScore;
			// Calculate score for every line [5/23/2013 user]
			for (i = -nL; i <= nL; i++) {
				pLL = pL + m;
				pLL->angle = angle;
				pLL->offset = i * Offset;
				short len = ExtractLine(idx1, pBuf, siz, angle, i * Offset, bX);
				if (len > minLen) {
					*pS = pLL->score = ScoreLine1(pBuf, len);
					if (m) {
						if (*pS < Smin) {
							Smin = *pS;
						}
						if (*pS > Smax) {
							Smax = *pS;
						}
					}
					else {
						Smin = Smax = *pS;
					}
					if (m > 2) {
						// search for maxderiv [6/8/2013 Administrator]
						float v = fabs(*pS1 - *pS);
						if (v > maxdev) {
							maxdev = v;
						}
						pS1++;
					}
					m++;
					pS++;
				}
			}

			float v = (Smax - Smin) * Thres;
			Smin = Smin + v;
			Smax = Smax - v;
			FILE* fp = NULL;
			//fp = fopen("C:\\TEMP\\LINE.CSV", "wb");
			pS = pScore;
			for (i = 0; i < m; i++) {
				switch (Mth) {
				case DARKBWD:
				case DARKFWD:
					for (i = 0; i < m; i++) {
						v = (*(pS) < Smin) ? 0 : Smax;
						if (fp) fprintf(fp, "%.4f,%.4f\n", *(pS), v);
						*(pS++) = v;
					}
					break;
				default:
					for (i = 0; i < m; i++) {
						v = (*(pS) > Smax) ? 0 : Smax;
						if (fp) fprintf(fp, "%.4f,%.4f\n", *(pS), v);
						*(pS++) = v;
					}
					break;
				}
			}
			if (fp) fclose(fp);

			SLnX* pL1min = NULL;	// First transition [6/1/2013 Yuen]
			SLnX* pL2min = NULL;	// Subsequent transition [6/1/2013 Yuen]
			short dOff = 3; // Skip a few bytes at the beginning [6/1/2013 Yuen]
			short skip = 0;
			switch (Mth) {
			case DARKFWD:
			case BRITFWD:
				pS = pScore + m - 1 - dOff;
				skip = dOff;
				while (*pS == 0) {
					pS--;
					skip++;
				}
				pS = pScore + m - 1 - skip;
				pL1min = pL + skip;	// First non zero location [6/4/2013 Yuen]
				pL1min->maxderiv = maxdev;
				// First transition [6/1/2013 Yuen]
				if (skip == dOff) {
					// when start with not 0 score [6/8/2013 Yuen]
					for (i = m - 1 - skip; i >= 0 + dOff; i--) {
						if (*pS == 0) {
							pL1min = pL + i;
							pL1min->maxderiv = maxdev;
							break;
						}
						pS--;
					}
					for (; i >= 0 + dOff; i--) {
						if (*pS != 0) {
							pL2min = pL + i;
							break;
						}
						pS--;
					}
				}
				else {
					// when start with 0 score [6/8/2013 Yuen]
					for (i = m - 1 - skip; i >= 0 + dOff; i--) {
						if (*pS == 0) {
							pL2min = pL + i;
							break;
						}
						pS--;
					}
				}
				break;
			case DARKBWD:
			case BRITBWD:
				pS = pScore + dOff;
				skip = dOff;
				while (*pS == 0) {
					pS++;
					skip++;
				}
				// First transition [6/1/2013 Yuen]
				pS = pScore + skip;
				pL1min = pL + skip;	// First non zero location [6/4/2013 Yuen]
				pL1min->maxderiv = maxdev;
				if (skip == dOff) {
					// when start with not 0 score [6/8/2013 Yuen]
					for (i = 0 + skip; i < m - dOff; i++) {
						if (*pS == 0) {
							pL1min = pL + i;
							pL1min->maxderiv = maxdev;
							break;
						}
						pS++;
					}
					for (; i < m - dOff; i++) {
						if (*pS != 0) {
							pL2min = pL + i;
							break;
						}
						pS++;
					}
				}
				else {
					// when start with 0 score [6/8/2013 Yuen]
					for (i = 0 + skip; i < m - dOff; i++) {
						if (*pS == 0) {
							pL2min = pL + i;
							break;
						}
						pS++;
					}
				}
				break;
			}
			short st;
			short score = 0;
			if (pL1min) {
				int ave = 0;
				switch (Mth) {
				case DARKFWD:
				case BRITFWD:
					// Calc In value [7/27/2013 Yuen]
					ave = 0;
					st = pL1min->offset + 10;
					for (i = st; i < (st + 3); i++) {
						ave += ScoreLine1(pBuf, ExtractLine(idx1, pBuf, siz, angle, i, bX));
						//SetLineColor(angle,i,0,0,255);
					}
					In = ave / 6.0f;
					// Calc Out value [7/27/2013 Yuen]
					ave = 0;
					st = pL1min->offset - 6;
					for (i = st; i > st - 3; i--) {
						ave += ScoreLine1(pBuf, ExtractLine(idx1, pBuf, siz, angle, i, bX));
						//SetLineColor(angle,i,255,0,0);
					}
					Out = ave / 3.0f;
					break;
				default:
					// Calc Out value [7/27/2013 Yuen]
					ave = 0;
					st = pL1min->offset + 6;
					for (i = st; i < (st + 3); i++) {
						ave += ScoreLine1(pBuf, ExtractLine(idx1, pBuf, siz, angle, i, bX));
						//SetLineColor(angle,i,255,0,0);
					}
					Out = ave / 3.0f;
					// Calc In value [7/27/2013 Yuen]
					ave = 0;
					st = pL1min->offset - 10;
					for (i = st; i > st - 3; i--) {
						ave += ScoreLine1(pBuf, ExtractLine(idx1, pBuf, siz, angle, i, bX));
						//SetLineColor(angle,i,0,0,255);
					}
					In = ave / 6.0f;
					break;
				}
				for (i = -1; i <= 1; i++) {
					SetLineColor(angle, pL1min->offset + i, 0, 255, 0);
				}
				switch (Mth) {
				case DARKFWD:
				case BRITFWD:
					for (i = -1; i <= 1; i++) {
						SetLineColor(angle, pL1min->offset + 4 + i, 0, 0, 255);
					}
					break;
				case DARKBWD:
				case BRITBWD:
					for (i = 1; i >= -1; i--) {
						SetLineColor(angle, pL1min->offset - 3 + i, 0, 0, 255);
					}
					break;
				}
				score = 1;
				pL1->Copy(pL1min);
			}
			if (pL2min) {
				for (i = -1; i <= 1; i++) {
					SetLineColor(angle, pL2min->offset + i, 255, 255, 0);
				}
				score += 2;
				pL2->Copy(pL2min);
			}
			SetLineColor(angle, 0, 255, 0, 255);

			if (pL) { delete[] pL; pL = NULL; }
			if (pBuf) { delete[] pBuf; pBuf = NULL; }
			if (pScore) { delete[] pScore; pScore = NULL; }

			return score;
		}
		if (pBuf) delete[] pBuf;
		if (pScore) delete[] pScore;
	}
	return 0;
}

float CImgWnd::ScoreLine1(short* pBuf, short len) {
	long score = 0;
	for (int i = 0; i < len; i++) {
		score += (*pBuf) * (*pBuf);
		pBuf++;
	}
	return score / float(len);  // average of square [7/27/2013 Yuen]
// 	return score / float(len);	// average [7/27/2013 Yuen]
// 	return sqrt(score) / float(len);	// average of sqrt of sum [7/27/2013 Yuen]
}

// float CImgWnd::GetScore(float angle, float offset)
// {
// 	int siz = 3*pPackedDIB1->pDIB->BmpInfo.biWidth*(pPackedDIB1->pDIB->BmpInfo.biBitCount/8);
// 	short *pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
// 	if (pBuf)
// 	{
// 		angle += 90;
// 		if (angle >= 360)
// 		{
// 			angle -= 360;
// 		}
// 		BOOL bX;
// 		int idx1;
// 		short len = ExtractLine(idx1,pBuf,siz,angle,offset,bX,0,255,0);
// 		float score = ScoreLine1(pBuf,len);
// 		delete [] pBuf;
// 		Invalidate(TRUE);
// 		return score;
// 	}
// 	return 999.99f;
// }

BOOL CImgWnd::Integrate(float* pBuf, short len) {
	float* pNew = new float[len + 16];
	float* p = pNew;
	float* q = pBuf;

	int i;
	short n = 7;
	for (i = 0; i < n; i++) {
		*(p++) = 0;
		q++;
	}
	for (i = n; i < len - n; i++) {
		*(p++) = (*(q - n) - *(q + n));
		q++;
	}
	for (int j = i; j < len; j++) {
		*(p++) = 0;
	}
	memcpy(pBuf, pNew, len * sizeof(float));
	delete[] pNew;

	return TRUE;
}

BOOL CImgWnd::Smooth(float* pBuf, short len) {
	float* pNew = new float[len + 16];
	if (!pNew) {
		return FALSE;
	}
	float* p = pNew + 4;
	float* q = pBuf + 4;
	int i, j;
	for (i = 4; i < len - 4; i++) {
		*(p++) = (*(q - 3) + *(q - 2) + *(q - 1) + *(q)+*(q + 1) + *(q + 2) + *(q + 3)) / 7;
		q++;
	}
	float v = *(p - 1);
	for (j = i; j < len; j++) {
		*(p++) = v;
	}

	p = pNew;
	v = *(p + 4);
	for (j = 0; j < 4; j++) {
		*(p++) = v;
	}

	memcpy(pBuf, pNew, len * sizeof(float));

	delete[] pNew;

	return TRUE;
}

BOOL CImgWnd::FindEdgeMax(float* pV, float angle, int siz, short* pBuf, short nL) {
	if (pPackedDIB1->pDIB) {
		angle += 90;
		if (angle >= 360) angle -= 360;
		if (angle < 0) angle += 360;

		short minLen = short(pPackedDIB1->IHeight * 0.15f);
		if (pBuf) {
			short i;
			BOOL bX = FALSE;
			int idx = -1;
			float maxd = 9e30f;
			// Calculate score for every line [5/23/2013 user]
			for (i = -nL; i <= nL; i++) {
				short len = ExtractLine(idx, pBuf, siz, angle, 4 * i, bX);
				if (len > minLen) {
					float v = ScoreLine1(pBuf, len);
					if (v < maxd) {
						maxd = v;
					}
				}
			}
			*pV = maxd;
			return 1;
		}
	}
	return 0;
}

void CImgWnd::ClearTextInfo() {
	pRcp = NULL;
	pAve = NULL;

	PointNum = 0;
	TWBevel = BADDATANEG;
	TWWidth = BADDATANEG;
	nTWEdge1 = nTWEdge2 = BADDATANEG;
	fTWEdge1 = fTWEdge2 = TWRadius = TWAngle = BADDATANEG;
}

#define SQRE(x) ((x)*(x))

// Ang decides how the angle is calculated [7/3/2013 Yuen]
float CImgWnd::GetAngle360(short Ang) {
	float angle = 0;
	int x2 = SQRE(pPackedDIB1->pDIB->BmpInfo.biWidth);
	int y2 = SQRE(pPackedDIB1->pDIB->BmpInfo.biHeight);
	int siz = int(sqrt(x2 + y2) * (pPackedDIB1->pDIB->BmpInfo.biBitCount / 8) + 16);
	short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]

	short nL = short(sqrt(pPackedDIB1->IHeight * pPackedDIB1->IHeight +
		pPackedDIB1->IWidth * pPackedDIB1->IWidth) * 0.075);

	int i, nS = 360;
	float* pVal = new float[nS + 8];
	float* pV = pVal;
	float sf = 180.0f / float(nS);
	float min = 9e30f;
	int nmin, nmax;
	float max = -9e30f;

	for (i = 0; i < nS; i++) {
		FindEdge360(pV, i * sf, siz, pBuf, nL);
		if (*pV < min) {
			nmin = i;
			min = *pV;
		}
		if (*pV > max) {
			nmax = i;
			max = *pV;
		}
		pV++;
	}

	float dx = (max - min) * 0.0f;

	min += dx;
	max -= dx;
	switch (Ang) {
	case 0:
		angle = nmax * sf - 90.0f;
		break;
	default:
		angle = AverageOf2Angles(pVal, nS, max, min, sf);
		break;
	}

	if (pVal) delete[] pVal;
	if (pBuf) delete[] pBuf;

	return angle;
}

BOOL CImgWnd::FindEdge360(float* pV, float angle, int siz, short* pBuf, short nL) {
	if (pPackedDIB1->pDIB) {
		angle += 90;
		if (angle >= 360) angle -= 360;
		if (angle < 0) angle += 360;

		short minLen = short(pPackedDIB1->IHeight * 0.15f);
		if (pBuf) {
			BOOL bX = FALSE;
			short i;
			int idx = -1;
			float maxd = 9e30f;
			// Calculate score for every line [5/23/2013 user]
			for (i = -nL; i <= nL; i++) {
				short len = ExtractLine(idx, pBuf, siz, angle, float(4 * i), bX);
				if (len > minLen) {
					float v = ScoreLine1(pBuf, len);
					// find lowest score [7/2/2013 Yuen]
					if (v < maxd) {
						maxd = v;
					}
				}
			}
			*pV = maxd;
			return TRUE;
		}
	}
	return FALSE;
}

// Minimum is not at 0 angle [7/3/2013 Yuen]
float CImgWnd::Average1(float* pVal, int nS, float max, float min, float sf) {
	// average of 2 angle method [7/3/2013 Yuen]
	float* pV = pVal;
	BOOL bFound = FALSE;
	float angle1 = 0;
	float angle2 = 0;
	for (int i = 0; i < nS; i++) {
		if ((*pV) <= min) {
			angle1 = i * sf;
			for (; i < nS; i++) {
				if ((*pV) > min) {
					angle2 = (i - 1) * sf;
					bFound = TRUE;
					break;
				}
				pV++;
			}
		}
		if (bFound) {
			break;
		}
		pV++;
	}
	return (angle1 + angle2) / 2.0f;
}

// Minimum is within 0 angle [7/3/2013 Yuen]
float CImgWnd::Average2(float* pVal, int nS, float max, float min, float sf) {
	// average of 2 angle method [7/3/2013 Yuen]
	float* pV = pVal;
	BOOL bFound = FALSE;
	float angle1 = 0;
	float angle2 = 0;
	//	float dx = (max - min) * 0.005f;
	for (int i = 0; i < nS; i++) {
		if ((*pV) > min) {
			angle1 = (i - 1) * sf;
			pV = pVal + nS - 1;
			for (i = nS - 1; i >= 0; i--) {
				if ((*pV) > min) {
					angle2 = (i + 1) * sf;
					bFound = TRUE;
					break;
				}
				pV--;
			}
		}
		else {
			pV++;
		}
		if (bFound) {
			break;
		}
	}
	angle2 = angle2 - 180;
	angle2 = (angle1 + angle2) / 2.0f;
	if (angle2 < 0) {
		angle2 += 180;
	}
	if (angle2 >= 180) {
		angle2 -= 180;
	}
	return angle2;
}

float CImgWnd::AverageOf2Angles(float* pVal, int nS, float max, float min, float sf) {
	if (*pVal > min) {
		return Average1(pVal, nS, max, min, sf);
	}
	else {
		return Average2(pVal, nS, max, min, sf);
	}
	return 0;
}

float CImgWnd::FindAngle(float* pVal, short nS, float max, float min, short nL, float sf) {
	// average of 2 angle method [7/3/2013 Yuen]
	BOOL bFound = FALSE;
	float angle1 = 0;
	float angle2 = 0;
	short n2 = nS / 2;
	float* pV = pVal + n2 - 1;
	for (int i = -1; i > -n2; i--) {
		if ((*pV) > min) {
			angle1 = angle2 = (i + 1) * sf;
			pV = pVal + n2 + 1;
			for (i = 1; i < n2; i++) {
				if ((*pV) > min) {
					angle2 = (angle1 + (i - 1) * sf) / 2.0f;
					bFound = TRUE;
					break;
				}
				pV++;
			}
			break;	// This is needed to break out if the forward search return nothing
		}
		else {
			pV--;
		}
	}
	angle2 = (angle1 + angle2) / 2.0f;
	return angle2;
}