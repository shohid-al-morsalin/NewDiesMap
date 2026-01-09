// PackedDIB1.cpp: implementation of the CPackedDIB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <math.h>

#include "tRGB.h"
#include "413App.h"
#include "PLT/HSL2RGB.h"

#include "PackedDIB1.h"

#define PIE 3.14159265359
#define PIE180 0.0174532925199432957692369076849

#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')

CPackedDIB::CPackedDIB() {}

CPackedDIB::~CPackedDIB() {
	ClearDIB();
}

BOOL CPackedDIB::Save(CString filename) {
	if (!pDIB || !pDIB->pDIBBits) {
		return FALSE;
	}

	CString path = DosUtil.DirSys.ExtractFilePath(filename);
	DosUtil.DirSys.GotoDirec(path);

	CFile file;
	if (!file.Open(filename, CFile::modeWrite | CFile::modeCreate)) {
		return FALSE;
	}

	BITMAPINFOHEADER BitmapHead;
	ZeroMemory(&BitmapHead, sizeof(BITMAPINFOHEADER));
	BitmapHead.biWidth = pDIB->BmpInfo.biWidth;
	BitmapHead.biHeight = pDIB->BmpInfo.biHeight;
	BitmapHead.biPlanes = 1;
	BitmapHead.biBitCount = pDIB->BmpInfo.biBitCount; // 8 bit gray+ color
	BitmapHead.biSizeImage = BytesPerRow * abs(pDIB->BmpInfo.biHeight);  // Total size of BMP including padded bytes [3/21/2012 Yuen]
	BitmapHead.biSize = sizeof(BITMAPINFOHEADER);
	BitmapHead.biClrUsed = 0;
	BitmapHead.biClrImportant = 0;
	BitmapHead.biCompression = 0;

	DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER)/* + 256 * sizeof(RGBQUAD)*/;
	// Fill in the fields of the file header
	BITMAPFILEHEADER   hdr;
	hdr.bfType = DS_BITMAP_FILEMARKER;
	hdr.bfSize = dwFileHeaderSize + BitmapHead.biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = dwFileHeaderSize;

	// Write the file header
	file.Write(&hdr, sizeof(BITMAPFILEHEADER));
	// Write the DIB header
	file.Write(&BitmapHead, sizeof(BITMAPINFOHEADER));
	file.Write(pDIB->pDIBBits, BitmapHead.biSizeImage);
	file.Close();
	return TRUE;
}

BOOL CPackedDIB::Load(CString filename) {
	FILE* fp;

	fp = fopen(filename, "rb");
	if (fp) {
		ClearDIB();

		if (!pDIB) {
			pDIB = new SIPackedDIB;
			pDIB->pDIBBits = NULL;
		}

		BITMAPFILEHEADER bmfh;

		fread(&bmfh, 1, sizeof(BITMAPFILEHEADER), fp);
		fread(&pDIB->BmpInfo, 1, sizeof(BITMAPINFOHEADER), fp);

		// Check file type signature
		if (bmfh.bfType != 'MB') {
			if (pDIB) {
				delete pDIB;
				pDIB = NULL;
			}
			return FALSE;		// File is not BMP
		}

		CalcBytePerRow();

		if (BytesPerPixel >= 3) {
			fseek(fp, bmfh.bfOffBits, SEEK_SET);
			if (pDIB->pDIBBits) {
				delete[] pDIB->pDIBBits;
				pDIB->pDIBBits = NULL;
			}
			pDIB->pDIBBits = new BYTE[BytesPerRow * IHeight];
			if (pDIB->BmpInfo.biHeight > 0) {
				for (int n = IHeight - 1; n >= 0; n--) {
					fread(pDIB->pDIBBits + BytesPerRow * n, 1, BytesPerRow, fp);
				}
				pDIB->BmpInfo.biHeight = -IHeight;
			}
			else {
				fread(pDIB->pDIBBits, 1, BytesPerRow * IHeight, fp);
			}
		}
		// so, we always have a up-bottom raster (that is negative height for windows):
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

void CPackedDIB::ClearDIB() {
	if (pDIB) {
		if (pDIB->pDIBBits) {
			delete[] pDIB->pDIBBits;
			pDIB->pDIBBits = NULL;
		}
		delete pDIB;
		pDIB = NULL;
	}
	OffX = OffY = 0;
	IWidth = IHeight = 0;
	BytesPerRow = BytesPerPixel = 0;
}

// Will replace existing BMP [6/6/2010 Yuen]
void CPackedDIB::Replace(SIPackedDIB* pD, short X, short Y) {
	if (!pD) {
		return;
	}
	ClearDIB();
	pDIB = pD;
	OffX = X; OffY = Y;
	CalcBytePerRow();
}

void CPackedDIB::AddAve(SIPackedDIB* pD, short BG, short X, short Y) {
	if (!pD || !pDIB || !pD->pDIBBits || !pDIB->pDIBBits) {
		ASSERT(0);
		return;
	}
	OffX = X; OffY = Y;

	// Cycle through image and perform averaging [2/13/2011 XPMUser]
	for (int i = 0; i < IHeight; i++) {
		BYTE* p;
		p = (BYTE*)pDIB->pDIBBits + i * BytesPerRow;
		SBGR* pA = (SBGR*)p;
		p = (BYTE*)pD->pDIBBits + i * BytesPerRow;
		SBGR* pB = (SBGR*)p;
		for (int j = 0; j < IWidth; j++) {
			pA->Ave(pB, BG);
			pA++; pB++;
		}
	}
}

void* CPackedDIB::GetPixelAddress(short x, short y) {
	return pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;
}

void CPackedDIB::FormatAngle(float& angle) {
	// unit: angle degree [11/13/2021 yuenl]
	while (angle < 0) angle += 360;
	while (angle >= 360) angle -= 360;
}

void CPackedDIB::Polar2Cartesian(float radius, float ang, float& x, float& y) {
	// unit: angle degree [11/13/2021 yuenl]
	FormatAngle(ang);
	ang = ang * PIE / 180;
	x = float(radius * cos(ang));
	y = float(radius * sin(ang));
}

void CPackedDIB::Cartesian2Polar(float X, float Y, float& radius, float& ang) {
	// unit: angle degree [11/13/2021 yuenl]
	radius = float(sqrt(X * X + Y * Y));
	ang = atan2(Y, X);
	ang *= float(180 / PIE);
	FormatAngle(ang);
}

BOOL CPackedDIB::GetLine(std::vector<SPont>& Ln, float angle, short xx, short yy) {
	// data extracted from center of wafer and outwards, all direction similar
	// xx, yy in image space
	// angle in wafer space
	Ln.clear();
	if (!pDIB) return FALSE;
	if (!IWidth || !IHeight) { ASSERT(0); return FALSE; }

	FormatAngle(angle);

	SPont P;
	short cx = IWidth / 2.0f, cy = IHeight / 2.0f;
	float m;

	if (angle == 0) { //  [11/14/2021 yuenl]
		short y = yy;
		for (short x = 0; x < IWidth; x++) {
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle <= 45) { //  [11/14/2021 yuenl]
		m = float(-tan(angle * 3.1415f / 180.0f));
		for (int x = 0; x < IWidth; x++) {
			int y = int(m * (x - xx)) + yy;
			if ((y < 0) || (y >= IHeight)) continue;
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle < 90) { //  [11/14/2021 yuenl]
		m = float(tan(angle * 3.1415 / 180.0f));
		for (int y = IHeight - 1; y >= 0; y--) {
			int x = int((yy - y) / m) + xx;
			if ((x < 0) || (x >= IWidth)) continue;
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle == 90) { //  [11/14/2021 yuenl]
		short x = xx;
		for (int y = IHeight - 1; y >= 0; y--) {
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle <= 135) { //  [11/14/2021 yuenl]
		m = float(tan((180 - angle) * 3.1415 / 180.0f));
		for (int y = IHeight - 1; y >= 0; y--) {
			int x = int((y - yy) / m) + xx;
			if ((x < 0) || (x >= IWidth)) continue;
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle < 180) { //  [11/14/2021 yuenl]
		m = float(-tan((angle) * 3.1415 / 180.0f));
		for (int x = IWidth - 1; x >= 0; x--) {
			int y = int(m * (x - xx)) + yy;
			if ((y < 0) || (y >= IHeight)) continue;
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle == 180) { //  [11/14/2021 yuenl]
		short y = yy;
		for (int x = IWidth - 1; x >= 0; x--) {
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle <= 225) { //  [11/14/2021 yuenl]
		m = float(-tan(angle * 3.1415 / 180.0f));
		for (int x = IWidth - 1; x >= 0; x--) {
			int y = int(m * (x - xx)) + yy;
			if ((y < 0) || (y >= IHeight)) continue;
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle < 270) { //  [11/14/2021 yuenl]
		m = float(-tan(angle * 3.1415 / 180.0f));
		for (int y = 0; y < IHeight; y++) {
			int x = int((y - yy) / m) + xx;
			if ((x < 0) || (x >= IWidth)) continue;
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle == 270) { //  [11/14/2021 yuenl]
		short x = xx;
		for (int y = 0; y < IHeight; y++) {
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else if (angle <= 315) { //  [11/14/2021 yuenl]
		m = float(-tan(angle * 3.1415 / 180.0f));
		for (int y = 0; y < IHeight; y++) {
			int x = int((y - yy) / m) + xx;
			if ((x < 0) || (x >= IWidth)) continue;
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	else {
		m = float(-tan(angle * 3.1415 / 180.0f));
		for (int x = 0; x < IWidth; x++) { //  [11/14/2021 yuenl]
			int y = int(m * (x - xx)) + yy;
			if ((y < 0) || (y >= IHeight)) continue;
			P.x = x; P.y = y; P.v = *(SBGR*)GetPixelAddress(x, y);
			Ln.push_back(P);
		}
	}
	return TRUE;
}

BOOL CPackedDIB::PlotLine(std::vector<SPont>& Ln, short y, float amp) {
	int sz = int(Ln.size());
	float sf = IWidth / float(sz);
	for (short i = 0; i < sz; i++) {
		SBGR& P = Ln[i].v;
		SetPixelRGB(sf * i, short(y - amp * P.I()), 255, 0, 255);
	}
	return 0;
}

BOOL CPackedDIB::PlotScore(std::vector<SPont>& Ln, short y, float amp) {
	int sz = int(Ln.size());
	float sf = IWidth / float(sz);
	for (short i = 0; i < sz; i++) {
		short v = short(y - amp * Ln[i].s);
		if (v < 0) v = 0; if (v >= IHeight) v = IHeight - 1;
		SetPixelRGB(sf * i, v, 0, 255, 255);
	}
	return TRUE;
}

BOOL CPackedDIB::Dump(const CString& name, std::vector<SPont>& Ln) {
	int sz = int(Ln.size());
	FILE* fp = fopen(name, "wb");
	if (!fp) return FALSE;
	for (int i = 0; i < sz; i++) {
		SPont& P = Ln[i];
		fprintf(fp, "%d,%d,%d,%.5f,%.5f,%.5f\n", P.x, P.y, P.v.I(), P.s, P.t, P.b);
	}
	fclose(fp);
	return TRUE;
}

BOOL CPackedDIB::Set(std::vector<SPont>& Ln, BYTE r, BYTE g, BYTE b) {
	int sz = int(Ln.size());
	for (int i = 0; i < sz; i++) {
		Ln[i].v.R = r; Ln[i].v.G = g; Ln[i].v.B = b;
	}
	return TRUE;
}

BOOL CPackedDIB::Filter(std::vector<SPont>& Ln) {
	int sz = int(Ln.size());
	if (sz < 1) return FALSE;
	float max, min;
	max = min = Ln[0].s;
	for (int i = 1; i < sz; i++) {
		SPont& P = Ln[i];
		if (P.s > max) { max = P.s; }
		if (P.s < min) { min = P.s; }
	}
	FLOAT lim = (max - min) / 5 + min;
	for (int i = 0; i < sz; i++) {
		SPont& P = Ln[i];
		if (P.s < lim) P.t = 0;
		else P.t = 255;
	}

	lim = (max - min);
	for (int i = 0; i < sz; i++) {
		SPont& P = Ln[i];
		if (P.s < lim) P.b = 0;
		else P.b = 255;
	}
	return TRUE;
}

BOOL CPackedDIB::SetLine(std::vector<SPont>& Ln) {
	if (!IWidth || !IHeight) { ASSERT(0); return FALSE; }

	int sz = int(Ln.size());
	float sf = 255 / float(sz);
	for (int i = 0; i < sz; i++) {
		SPont& P = Ln[i];
		SBGR& C = P.v;
		SetPixelRGB(P.x, P.y, C.R, C.G, C.B);
	}
	return TRUE;
}

// aka Dup/Store [6/6/2010 Yuen]
BOOL CPackedDIB::Duplicate(CPackedDIB& DIB, short X, short Y) {
	return Duplicate(DIB.pDIB, X, Y);
}

BOOL CPackedDIB::Duplicate(SIPackedDIB* DIB, short X, short Y) {
	ClearDIB();
	if (DIB) {
		pDIB = new SIPackedDIB;
		if (pDIB) {
			pDIB->BmpInfo = DIB->BmpInfo;
			CalcBytePerRow();
			int Siz = BytesPerRow * IHeight;
			pDIB->pDIBBits = new BYTE[Siz + BytesPerRow];
			if (pDIB->pDIBBits) {
				memcpy(pDIB->pDIBBits, DIB->pDIBBits, Siz);
				OffX = X; OffY = Y;
				return TRUE;
			}
		}
	}
	OffX = X; OffY = Y;
	return FALSE;
}

SIPackedDIB* CPackedDIB::Create(short Width, short Height, short bitCount) {
	ClearDIB();
	pDIB = new SIPackedDIB;
	if (pDIB) {
		pDIB->BmpInfo.biBitCount = bitCount;
		pDIB->BmpInfo.biHeight = Height;
		pDIB->BmpInfo.biWidth = Width;
		pDIB->BmpInfo.biPlanes = 1;
		pDIB->BmpInfo.biCompression = 0;
		pDIB->BmpInfo.biSize = sizeof(BITMAPINFOHEADER);
		pDIB->BmpInfo.biXPelsPerMeter = pDIB->BmpInfo.biYPelsPerMeter = 0;
		pDIB->BmpInfo.biClrUsed = pDIB->BmpInfo.biClrImportant = 0;
		CalcBytePerRow();
		long SizeImage = BytesPerRow * IHeight;
		pDIB->BmpInfo.biSizeImage = SizeImage;
		pDIB->pDIBBits = new BYTE[SizeImage + BytesPerRow];
	}
	return pDIB;
}

SIPackedDIB* CPackedDIB::CreateNew(short Width, short Height, short bitCount) {
	SIPackedDIB* pDIB = new SIPackedDIB;
	if (pDIB) {
		pDIB->BmpInfo.biBitCount = bitCount;
		pDIB->BmpInfo.biHeight = Height;
		pDIB->BmpInfo.biWidth = Width;
		pDIB->BmpInfo.biPlanes = 1;
		pDIB->BmpInfo.biCompression = 0;
		pDIB->BmpInfo.biSize = sizeof(BITMAPINFOHEADER);
		pDIB->BmpInfo.biXPelsPerMeter = pDIB->BmpInfo.biYPelsPerMeter = 0;
		pDIB->BmpInfo.biClrUsed = pDIB->BmpInfo.biClrImportant = 0;
		CalcBytePerRow();
		long SizeImage = BytesPerRow * IHeight;
		pDIB->BmpInfo.biSizeImage = SizeImage;
		pDIB->pDIBBits = new BYTE[SizeImage + BytesPerRow];
	}
	return pDIB;
}

void CPackedDIB::Sobel() {
	if ((!pDIB) || (!pDIB->pDIBBits)) {
		return;
	}
	short exclude = 3;
	SIPackedDIB* pD = CreateNew(pDIB->BmpInfo.biWidth - exclude, pDIB->BmpInfo.biHeight - exclude, pDIB->BmpInfo.biBitCount);
	if (!pD) {
		return;
	}
	long BytesPerRow2 = (pDIB->BmpInfo.biWidth - exclude) * pDIB->BmpInfo.biBitCount / 8;
	BytesPerRow2 += (4 - BytesPerRow2 % 4) % 4;	// int alignment

	int i, j;
	BYTE* pB;
	short BPP = pDIB->BmpInfo.biBitCount / 8;
	for (i = 0; i < abs(pDIB->BmpInfo.biHeight) - exclude; i++) {
		pB = pDIB->pDIBBits + i * BytesPerRow;
		BYTE* pSrc1 = pB;
		BYTE* pSrc2 = pSrc1 + BytesPerRow;
		BYTE* pSrc3 = pSrc2 + BytesPerRow;
		pB = pD->pDIBBits + i * BytesPerRow2;
		for (j = 0; j < pD->BmpInfo.biWidth - exclude; j++) {
			short GH = SobelH(pSrc1, pSrc2, pSrc3, BPP);
			short GV = SobelV(pSrc1, pSrc2, pSrc3, BPP);
			pSrc1 += BPP; pSrc2 += BPP; pSrc3 += BPP;
			short v = short(sqrt(float(GH * GH + GV * GV)));
			if (v < 0) {
				v = 0;
			}
			if (v > 255) {
				v = 255;
			}
			//*(pB++) = 0;  // Blue [9/18/2011 Administrator]
			//*(pB++) = 0;  // Green [9/18/2011 Administrator]
			//*(pB++) = 0;  // Red [9/18/2011 Administrator]
			memset(pB, v, 3);
			pB += 3;
			if (BPP > 3) {
				*(pB++) = 0;
			}
		}
	}
	Replace(pD);
}

short CPackedDIB::SobelH(BYTE* pS1, BYTE* pS2, BYTE* pS3, short BPP) {
	short kH[] = { -1,-2,-1,0,0,0,1,2,1 };
	short InT = 0;
	InT += InTensity(pS1) * kH[0]; pS1 += BPP;
	InT += InTensity(pS1) * kH[1]; pS1 += BPP;
	InT += InTensity(pS1) * kH[2]; pS1 += BPP;
	InT += InTensity(pS2) * kH[3]; pS2 += BPP;
	InT += InTensity(pS2) * kH[4]; pS2 += BPP;
	InT += InTensity(pS2) * kH[5]; pS2 += BPP;
	InT += InTensity(pS3) * kH[6]; pS3 += BPP;
	InT += InTensity(pS3) * kH[7]; pS3 += BPP;
	InT += InTensity(pS3) * kH[8]; pS3 += BPP;
	return InT;
	// 	return (pS1++)->I() * kH[0] + (pS1++)->I() * kH[1] + (pS1++)->I() * kH[2] +
	// 		(pS2++)->I() * kH[3] + (pS2++)->I() * kH[4] + (pS2++)->I() * kH[5] +
	// 		(pS3++)->I() * kH[6] + (pS3++)->I() * kH[7] + (pS3++)->I() * kH[8] ;
}

short CPackedDIB::SobelV(BYTE* pS1, BYTE* pS2, BYTE* pS3, short BPP) {
	short kH[] = { 1,0,-1,2,0,-2,1,0,-1 };
	short InT = 0;
	InT += InTensity(pS1) * kH[0]; pS1 += BPP;
	InT += InTensity(pS1) * kH[1]; pS1 += BPP;
	InT += InTensity(pS1) * kH[2]; pS1 += BPP;
	InT += InTensity(pS2) * kH[3]; pS2 += BPP;
	InT += InTensity(pS2) * kH[4]; pS2 += BPP;
	InT += InTensity(pS2) * kH[5]; pS2 += BPP;
	InT += InTensity(pS3) * kH[6]; pS3 += BPP;
	InT += InTensity(pS3) * kH[7]; pS3 += BPP;
	InT += InTensity(pS3) * kH[8]; pS3 += BPP;
	return InT;
	// 	return (pS1++)->I() * kH[0] + (pS1++)->I() * kH[1] + (pS1++)->I() * kH[2] +
	// 		(pS2++)->I() * kH[3] + (pS2++)->I() * kH[4] + (pS2++)->I() * kH[5] +
	// 		(pS3++)->I() * kH[6] + (pS3++)->I() * kH[7] + (pS3++)->I() * kH[8] ;
}

// pDIB will be deleted [3/21/2012 Yuen]
SIPackedDIB* CPackedDIB::GetDIB() {
	SIPackedDIB* p = pDIB;
	pDIB = NULL;
	return p;
}

int CPackedDIB::GetHeight() {
	if (pDIB) {
		return abs(pDIB->BmpInfo.biHeight);
	}
	ASSERT(0);
	return 0;
}

int CPackedDIB::GetWidth() {
	if (pDIB) {
		return pDIB->BmpInfo.biWidth;
	}
	ASSERT(0);
	return 0;
}

// Calculate total size of bitmap buffer including padded bytes [3/21/2012 Yuen]
int CPackedDIB::GetSize() {
	if (pDIB) {
		return BytesPerRow * IHeight;
	}
	ASSERT(0);
	return 0;
}

void CPackedDIB::Serialize(CArchive& ar) {
	int magic;
	BOOL bImage = FALSE;
	if (ar.IsStoring()) {
		magic = 2;
		ar << magic;
		if (pDIB && pDIB->pDIBBits) {
			bImage = TRUE;
			ar << bImage;
			ar << OffX;
			ar << OffY;
			ar.Write((void*)&pDIB->BmpInfo, sizeof(BITMAPINFOHEADER));
			//ar.Write(pDIB->ColorTable,sizeof(RGBQUAD) * 256);
			int sz = GetSize();
			ar << sz;
			if (sz) {
				ar.Write(pDIB->pDIBBits, sz);
			}
		}
		else {
			bImage = FALSE;
			ar << bImage;
		}
	}
	else {
		ar >> magic;
		ClearDIB();
		ar >> bImage;
		if (bImage) {
			ar >> OffX;
			ar >> OffY;
			if (pDIB) {
				ASSERT(0);
			}

			pDIB = new SIPackedDIB;
			pDIB->pDIBBits = NULL;

			ar.Read((void*)&pDIB->BmpInfo, sizeof(BITMAPINFOHEADER));
			CalcBytePerRow();
			if (magic < 2) {
				ar.Read(pDIB->ColorTable, sizeof(RGBQUAD) * 256);
			}
			int sz;
			ar >> sz;
			if (sz) {
				if (pDIB->pDIBBits) {
					delete[] pDIB->pDIBBits;
				}
				int sz2 = BytesPerRow * IHeight + 1;
				if (sz > sz2) {
					ASSERT(0);
					sz2 = sz;
				}
				pDIB->pDIBBits = new BYTE[sz2];
				ar.Read(pDIB->pDIBBits, sz);
			}
		}
	}
}

void CPackedDIB::Conv(char** msg) {}
long CPackedDIB::Binary(char** msg) {
	return 0;
}

long CPackedDIB::GetLength() {
	return 0;
}

void CPackedDIB::CreateBlank() {
	ClearDIB();
	pDIB = new SIPackedDIB;
	if (pDIB) {
		pDIB->pDIBBits = NULL;
	}
}

// only works with RGB image [3/21/2012 Yuen]
// No validity check for speed of execution [3/21/2012 Yuen]
// Get RGB value converted to 1:2:1 gray intensity [8/15/2010 XPMUser]
BYTE CPackedDIB::GetPixel(short x, short y) {
	BYTE* p = pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;
	short v = *(p++);
	v += *(p++) * 2;
	v += *(p);

	return v / 4;
}

bool CPackedDIB::IsGreen(short x, short y)
{
	BYTE* p = pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;
	if (*(p + 2) == 0 && *(p + 1) == 255 && *(p) == 0) return true;
	else return false;
}

BYTE CPackedDIB::GetPixelL(short x, short y) {
	BYTE* p = pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;

	return HsL(*(p + 2), *(p + 1), *p);
}

BYTE CPackedDIB::HsL(short R1, short G1, short B1) {
	float fR = R1 / 255.0f;
	float fG = G1 / 255.0f;
	float fB = B1 / 255.0f;

	float max, min;
	max = min = fR;
	if (fG > max) {
		max = fG;
	}
	if (fB > max) {
		max = fB;
	}
	if (fG < min) {
		min = fG;
	}
	if (fB < min) {
		min = fB;
	}
	return 255 * (max + min) / 2.0f;
}

// Write in gray intensity [3/21/2012 Yuen]
void CPackedDIB::SetPixel(short x, short y, BYTE v) {
	BYTE* p = pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;
	*(p++) = v;
	*(p++) = v;
	*(p) = v;
}

// Write in gray intensity [3/21/2012 Yuen]
void CPackedDIB::SetPixelRGB(short x, short y, BYTE r, BYTE g, BYTE b) {
	BYTE* p = pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;
	*(p++) = b;
	*(p++) = g;
	*(p) = r;
}

void CPackedDIB::SetPixelRed(short x, short y, BOOL v) {
	BYTE* p = pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;
	switch (v) {
	case 255:
		*(p++) = 0;
		*(p++) = 0;
		*(p) = 255;
		break;
	case 128:
		*(p++) = 255;
		*(p++) = 0;
		*(p) = 0;
		break;
	default:
		*(p++) = 0;
		*(p++) = 255;
		*(p) = 0;
		break;
	}
}

BYTE CPackedDIB::GetSetPixelM1(short x, short y) {
	BYTE* p = pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;
	BYTE* q = p;
	short v = *(p++);
	v += *(p++) * 2;
	v += *(p);
	v /= 4;

	BYTE g = 0;
	BYTE r = 0;
	if (v < 128) {
		g = 255;
	}
	else {
		g = 255;
	}

	*(q++) = 0;	// Blue [4/13/2013 Administrator]
	*(q++) = g;	// Green [4/13/2013 Administrator]
	*(q) = r;	// Red [4/13/2013 Administrator]

	return (BYTE)v;
}

BYTE CPackedDIB::GetSetPixelRGB(short x, short y, BYTE r, BYTE g, BYTE b) {
	BYTE* p = pDIB->pDIBBits + y * BytesPerRow + x * BytesPerPixel;
	BYTE* q = p;
	short v = *(p++);
	v += *(p++) * 2;
	v += *(p);

	*(q++) = b;
	*(q++) = g;
	*(q) = r;

	return v / 4;
}

void CPackedDIB::Contrast() {
	// Implementing histogram equalization [2/14/2011 XPMUser]
	if (!pDIB || !pDIB->pDIBBits) {
		return;
	}

	int i, j;
	int histoR[256], histoG[256], histoB[256];
	memset(histoR, 0, 256 * sizeof(int));
	memset(histoG, 0, 256 * sizeof(int));
	memset(histoB, 0, 256 * sizeof(int));

	// Generate histogram [2/14/2011 XPMUser]
	for (i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		SBGR* pB = (SBGR*)p;
		for (j = 0; j < pDIB->BmpInfo.biWidth; j++) {
			pB->Add(histoR, histoG, histoB);
			pB++;
		}
	}
	int* pR = histoR, * pR1 = histoR + 1;
	int* pG = histoG, * pG1 = histoG + 1;
	int* pB = histoB, * pB1 = histoB + 1;
	// Cumulative histogram [2/14/2011 XPMUser]
	for (i = 1; i < 256; i++) {
		*(pR1++) += *(pR++);
		*(pG1++) += *(pG++);
		*(pB1++) += *(pB++);
	}
	int cdfRmin = -1;
	int cdfGmin = -1;
	int cdfBmin = -1;
	pR = histoR;
	pG = histoG;
	pB = histoB;
	for (i = 0; i < 256; i++) {
		if ((cdfRmin == -1) && (*pR > 0)) {
			cdfRmin = *pR;
		}
		if ((cdfGmin == -1) && (*pG > 0)) {
			cdfGmin = *pG;
		}
		if ((cdfBmin == -1) && (*pB > 0)) {
			cdfBmin = *pB;
		}
		pR++; pG++; pB++;
	}
	int ImgSize = abs(pDIB->BmpInfo.biHeight * pDIB->BmpInfo.biWidth);
	float sfR = 255.0f / (ImgSize - cdfRmin);
	float sfG = 255.0f / (ImgSize - cdfGmin);
	float sfB = 255.0f / (ImgSize - cdfBmin);
	// Actual equalization [2/14/2011 XPMUser]
	pR = histoR;
	pG = histoG;
	pB = histoB;
	for (i = 0; i < 256; i++) {
		*(pR++) = int((*pR - cdfRmin) * sfR);
		*(pG++) = int((*pG - cdfGmin) * sfG);
		*(pB++) = int((*pB - cdfBmin) * sfB);
	}

	for (i = 0; i < abs(pDIB->BmpInfo.biHeight); i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		SBGR* pBb = (SBGR*)p;
		for (j = 0; j < pDIB->BmpInfo.biWidth; j++) {
			pBb->HistoEqualize(histoR, histoG, histoB);
			pBb++;
		}
	}
}

short CPackedDIB::CalcBytePerRow() {
	IWidth = short(pDIB->BmpInfo.biWidth);
	IHeight = short(abs(pDIB->BmpInfo.biHeight));
	BytesPerPixel = pDIB->BmpInfo.biBitCount / 8;
	BytesPerRow = IWidth * BytesPerPixel;
	BytesPerRow += (4 - BytesPerRow % 4) % 4;	// int alignment
	return BytesPerRow;
}

float CPackedDIB::GetIntensityCount() {
	long sum = 0;
	for (int i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		BYTE* pB = p;
		for (int j = 0; j < IWidth; j++) {
			sum += InTensity(pB);
			p += BytesPerPixel;
		}
	}
	return sum / float(IWidth * IHeight);
}

int CPackedDIB::InTensity(BYTE* p) {
	BYTE R = *(p++);
	BYTE G = *(p++);
	BYTE B = *p;
	return (R + 2 * G + B) / 4;
}

void CPackedDIB::SetIntensity(BYTE* p, BYTE R, BYTE G, BYTE B) {
	*(p++) = R < 256 ? (R < 0 ? 0 : R) : 255;
	*(p++) = G < 256 ? (G < 0 ? 0 : G) : 255;
	*(p) = B < 256 ? (B < 0 ? 0 : B) : 255;
}

void CPackedDIB::ContrastBW() {
	// Implementing histogram equalization [2/14/2011 XPMUser]
	if (!pDIB || !pDIB->pDIBBits) {
		return;
	}

	int i, j;
	int histo[256];
	memset(histo, 0, 256 * sizeof(int));

	// Generate histogram [2/14/2011 XPMUser]
	for (i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		for (j = 0; j < IWidth; j++) {
			histo[InTensity(p)]++;
			p += BytesPerPixel;
		}
	}
	int* pR = histo, * pR1 = histo + 1;
	// Cumulative histogram [2/14/2011 XPMUser]
	for (i = 1; i < 256; i++) {
		*(pR1++) += *(pR++);
	}
	int cdfRmin = -1;
	pR = histo;
	for (i = 0; i < 256; i++) {
		if ((cdfRmin == -1) && (*pR > 0)) {
			cdfRmin = *pR;
		}
		pR++;
	}
	int ImgSize = IHeight * IWidth;
	short dd = 0;	// Conntrol parameter [4/15/2013 Administrator]
	cdfRmin += dd;
	float sfR = (255 - dd) / float(ImgSize - cdfRmin);
	// Actual equalization [2/14/2011 XPMUser]
	pR = histo;
	int a;
	for (i = 0; i < 256; i++) {
		a = int((*pR - cdfRmin) * sfR);
		if (a < 0) {
			a = 0;
		}
		if (a > 255) {
			a = 255;
		}
		*(pR++) = a;
	}

	for (i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		for (j = 0; j < IWidth; j++) {
			BYTE v = InTensity(p);
			v = histo[v];
			SetIntensity(p, v, v, v);
			p += BytesPerPixel;
		}
	}
}

// Evolution of ContrastBW [4/15/2013 Administrator]
void CPackedDIB::Contrast3Level() {
	// Implementing histogram equalization [2/14/2011 XPMUser]
	if (!pDIB || !pDIB->pDIBBits) {
		return;
	}

	int i, j;
	int histo[256];
	memset(histo, 0, 256 * sizeof(int));

	// Generate histogram [2/14/2011 XPMUser]
	for (i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		for (j = 0; j < IWidth; j++) {
			histo[InTensity(p)]++;
			p += BytesPerPixel;
		}
	}
	int* pR = histo, * pR1 = histo + 1;
	// Cumulative histogram [2/14/2011 XPMUser]
	for (i = 1; i < 256; i++) {
		*(pR1++) += *(pR++);
	}
	int cdfRmin = -1;
	pR = histo;
	for (i = 0; i < 256; i++) {
		if ((cdfRmin == -1) && (*pR > 0)) {
			cdfRmin = *pR;
		}
		pR++;
	}
	int ImgSize = IHeight * IWidth;
	short dd = 0;	// Conntrol parameter [4/15/2013 Administrator]
	cdfRmin += dd;
	float sfR = (255 - dd) / float(ImgSize - cdfRmin);
	// Actual equalization [2/14/2011 XPMUser]
	pR = histo;
	int a;
	for (i = 0; i < 256; i++) {
		a = int((*pR - cdfRmin) * sfR);
		if (a < 8) {
			a = 0;
		}
		else if (a < 253) {
			a = 128;
		}
		else {
			a = 255;
		}
		*(pR++) = a;
	}

	for (i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		for (j = 0; j < IWidth; j++) {
			BYTE v = InTensity(p);
			v = histo[v];
			SetIntensity(p, v, v, v);
			p += BytesPerPixel;
		}
	}
}

void CPackedDIB::GreyScale() {
	if (!pDIB || !pDIB->pDIBBits) {
		return;
	}
	for (int i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			BYTE v = InTensity(p);
			SetIntensity(p, v, v, v);
			p += BytesPerPixel;
		}
	}
}

void CPackedDIB::Binarize(short lv) {
	if (!pDIB || !pDIB->pDIBBits) {
		return;
	}
	for (int i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			BYTE v = (InTensity(p) > lv ? 255 : 0);
			SetIntensity(p, v, v, v);
			p += BytesPerPixel;
		}
	}
}

void CPackedDIB::PreProcess() {
	//ContrastBW();
}

void CPackedDIB::Gaussian() {}

void CPackedDIB::ContrastColor() {
	CHslRgb H;

	float Max, Min;
	float* pL = new float[IHeight * IWidth];
	float* pH = new float[IHeight * IWidth];
	float* pS = new float[IHeight * IWidth];
	float* ppL, * ppH, * ppS;

	if (!pDIB || !pDIB->pDIBBits) {
		return;
	}

	// Step 1: find Max and Min lightness L [5/16/2013 user]
	ppL = pL;
	ppH = pH;
	ppS = pS;
	BYTE* p = pDIB->pDIBBits;
	Max = Min = H.GetL(*p + 2, *p + 1, *p);
	for (int i = 0; i < IHeight; i++) {
		p = pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			H.R = *(p);
			H.G = *(p + 1);
			H.B = *(p + 2);
			H.HsL(H.R, H.G, H.G);
			float v = H.L;
			if (v > Max) {
				Max = v;
			}
			if (v < Min) {
				Min = v;
			}
			*(ppL++) = H.L;
			*(ppH++) = H.H;
			*(ppS++) = H.S;
			p += BytesPerPixel;
		}
	}

	// Show lightness map [5/16/2013 user]
	ppL = pL;
	ppH = pH;
	ppS = pS;
	float sf = 1.0f / (Max - Min);
	for (int i = 0; i < IHeight; i++) {
		float v;
		p = pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			v = (*(ppL++) - Min) * sf;
			H.RgB(*(ppH++), *(ppS++) + 0.15, v);
			//SetIntensity(p,v,v,v);
			SetIntensity(p, H.R, H.G, H.B);
			p += BytesPerPixel;
		}
	}

	delete[] pL;
	delete[] pH;
	delete[] pS;
}

void CPackedDIB::ContrastStretchColor(float TMax, float TMin) {
	if (!pDIB || !pDIB->pDIBBits) {
		return;
	}
	BYTE Max, Min;
	// Find Max Min [5/16/2013 user]
	BYTE* p = pDIB->pDIBBits;
	Max = Min = *p;
	for (int i = 0; i < IHeight; i++) {
		p = pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			BYTE* q = p;
			for (int k = 0; k < 3; k++) {
				if (*q > Max) {
					Max = *q;
				}
				if (*q < Min) {
					Min = *q;
				}
				q++;
			}
			p += BytesPerPixel;
		}
	}

	float df = (Max - Min);
	Min = Min + df * TMin;
	Max = Max - df * TMax;
	df = Max - Min;
	if (df) {
		df = 255.0f / df;
	}
	else df = 0;
	for (int i = 0; i < IHeight; i++) {
		BYTE* p = pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			BYTE* q = p;
			short v = (*q - Min) * df;
			*(q) = (v < 256) ? ((v >= 0) ? v : 0) : 255;
			q++;
			v = (*q - Min) * df;
			*(q) = (v < 256) ? ((v >= 0) ? v : 0) : 255;
			q++;
			v = (*q - Min) * df;
			*(q) = (v < 256) ? ((v >= 0) ? v : 0) : 255;
			p += BytesPerPixel;
		}
	}
}

// returns negative number in error [7/28/2013 Yuen]
long CPackedDIB::Different(CPackedDIB& DIB2) {
	if (!pDIB || !pDIB->pDIBBits) {
		return -1;
	}
	if (!DIB2.pDIB || !DIB2.pDIB->pDIBBits) {
		return -2;
	}
	// Source and destination image must be of same size and bit depth [7/28/2013 Yuen]

	if (pDIB->BmpInfo.biBitCount != DIB2.pDIB->BmpInfo.biBitCount) {
		return -3;
	}
	if (pDIB->BmpInfo.biWidth != DIB2.pDIB->BmpInfo.biWidth) {
		return -4;
	}
	if (pDIB->BmpInfo.biHeight != DIB2.pDIB->BmpInfo.biHeight) {
		return -5;
	}

	long sum = 0;	// sum of difference between 2 DIB [7/28/2013 Yuen]
	BYTE* p;
	BYTE* q;
	short dT1 = 0;
	short dT2 = 0;
	if (pDIB->BmpInfo.biBitCount > 24)	// 3 bytes [7/28/2013 Yuen]
	{
		dT1 = 1;
	}
	if (DIB2.pDIB->BmpInfo.biBitCount > 24) {
		dT2 = 1;
	}
	int cnt = 0;
	for (int i = 0; i < IHeight; i += 7) {
		p = pDIB->pDIBBits + i * BytesPerRow; // same BytesPerRow is used unless biBitCount restriction is lifted [7/28/2013 Yuen]
		q = DIB2.pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			sum += abs(*(p++) - *(q++));
			sum += abs(*(p++) - *(q++));
			sum += abs(*(p++) - *(q++));
			p += dT1; q += dT2; cnt++;
		}
	}
	return sum / cnt;
}

float CPackedDIB::AverageHue() {
	if (!pDIB || !pDIB->pDIBBits) {
		return -1;
	}
	CHslRgb H;
	int cnt = 0;
	float sum = 0;
	BYTE* p = pDIB->pDIBBits;
	for (int i = 0; i < IHeight; i += 5) {
		p = pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			cnt++;
			sum += H.GetH(*(p + 2), *(p + 1), *(p));
			p += BytesPerPixel;
		}
	}
	if (cnt) {
		return sum / (float)cnt;
	}
	return -2;
}

float CPackedDIB::AverageSat() {
	if (!pDIB || !pDIB->pDIBBits) {
		return -1;
	}
	CHslRgb H;
	int cnt = 0;
	float sum = 0;
	BYTE* p = pDIB->pDIBBits;
	for (int i = 0; i < IHeight; i += 5) {
		p = pDIB->pDIBBits + i * BytesPerRow;
		for (int j = 0; j < IWidth; j++) {
			cnt++;
			sum += H.GetS(*(p + 2), *(p + 1), *(p));
			p += BytesPerPixel;
		}
	}
	if (cnt) {
		return sum / (float)cnt;
	}
	return -2;
}

BOOL CPackedDIB::GetDimension(int& wd, int& ht) {
	if (pDIB && pDIB->pDIBBits) {
		wd = pDIB->BmpInfo.biWidth;
		ht = abs(pDIB->BmpInfo.biHeight);
		return TRUE;
	}
	return FALSE;
}