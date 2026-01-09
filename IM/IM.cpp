#include "stdafx.h"
#include <direct.h>

#include "define.h"
#include "MatInv.h"

// [08202024 ZHIMING
#include "PackedDIB1.h"
// ]

#include "IM.h"

#define EDG_RES     8
#define EDG_EXCLUDE 128

#define sqre(x) (x)*(x)
#define TORADIAN 0.0174532925199
#define PIE180   0.0174532925199

// Macros
#define NINT(f)   ((f >= 0) ? (int)(f + .5) : (int)(f - .5))
#define THRESH(d) ((d > 255) ? 255 : ((d < 0) ? 0 : d))
#define INRANGE() (m_image.minmax.min >= 0 && m_image.minmax.max <= 255)

//////////////////////////////////////////////////////////////////////////
// buffer access [11/3/2019 FSM]
bool CIM::SetBuffer(PBYTE pBuffer, int wd, int ht, int bpp) {// Assume pBuffer is 8bpp rectangular image [1/3/2020 yuenl]
	if (bpp != 24) { ASSERT(0); return false; }
	if (IsNull()) {
		if (!Create(wd, ht, bpp)) return false;
	}
	else {
		int w, h, b;
		GetDim(w, h, b);
		if ((w != wd) || (h != ht) || (b != 24)) {
			Destroy();
			if (!Create(wd, ht, 24)) return false;
		}
	}
	if (IsNull()) return false;
	// convert 8 bpp to 24 bpp [1/3/2020 yuenl]
	for (int y = 0; y < ht; y++) {
		PBYTE q = pBuffer + y * wd;
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			*(p++) = *q; *(p++) = *q; *(p++) = *q;
			q++;
		}
	}
	//_wd = wd; _ht = ht; _bpp = bpp;
	CalcHisto();
	return true;
}

//////////////////////////////////////////////////////////////////////////
// helpers [11/6/2019 FSM]
void CIM::Polar2Cart(float fPr, float fPa, float& fPx, float& fPy) {
	while (fPa >= 360.0) fPa -= 360.0;
	while (fPa < 0) fPa += 360.0;
	double angle = fPa * PIE180;
	fPx = float(fPr * cos(angle));
	fPy = float(fPr * sin(angle));
}

void CIM::Cart2Polar(float fPx, float fPy, float& fPr, float& fPa) {
	fPr = float(sqrt(fPx * fPx + fPy * fPy));
	if (fPx == 0) {
		if (fPy >= 0) { fPa = 90.0; return; }
		else { fPa = 270.0; return; }
	}
	fPa = float(atan(fPy / fPx) * 57.29577951308230);
	if (fPx >= 0) {
		if (fPy >= 0) {}
		else fPa += 360.0;
	}
	else {
		if (fPy >= 0) fPa += 180.0;
		else fPa += 180.0;
	}
	while (fPa >= 360.0) fPa -= 360.0;
	while (fPa < 0) fPa += 360.0;
}

void CIM::DumpPoints(std::vector<SLoc>& ledge, std::vector<SLoc>& redge, char* filename) {
	int n;
	if (!ledge.size()) return;
	if (!redge.size()) return;

	FILE* fp = fopen(filename, "wb"); if (!fp) return;
	n = redge.size();
	fprintf(fp, "LEFT\n");
	fprintf(fp, "X,Y,X\n");
	for (int i = 0; i < n; i++) {
		fprintf(fp, "%d,%d,%d\n", redge[i].x, redge[i].y, redge[i].x);
	}
	n = ledge.size();
	fprintf(fp, "\n\nRIGHT\n");
	fprintf(fp, "X,Y,X\n");
	for (int i = 0; i < n; i++) {
		fprintf(fp, "%d,%d,%d\n", ledge[i].x, ledge[i].y, ledge[i].x);
	}
	fclose(fp);
}

void CIM::DumpEdge(char* filename, int* pX, int* pY, short n) {
	FILE* fp = fopen(filename, "wb"); if (!fp) return;
	fprintf(fp, "X,Y\n");
	for (int i = 0; i < n; i++) {
		fprintf(fp, "%d,%d\n", *(pX++), *(pY++));
	}
	fclose(fp);
}

int CIM::GetBYPP() {// Get bytes per pixel [11/3/2019 FSM]
	return GetBPP() / 8;
}

int CIM::GetBYPR() {// Get bytes per row [11/3/2019 FSM]
	return GetPitch();
}

CString CIM::GetEquationStr(float a, float b, BOOL bXind) {
	CString str;
	if (bXind) {
		str.Format("y = %.4fx + %.4f", a, b);
		return str;
	}
	str.Format("x = %.4fy + %.4f", a, b);
	return str;
}

bool CIM::ReduceSize(CIM& Image, short factor) {
	int wd, ht, bpp;
	if (factor < 2) return false;
	if (Image.IsNull()) return false;
	Image.GetDim(wd, ht, bpp); if (bpp != 24) return false; // only support 24 bits image [6/18/2020 yuenl]
	if (!IsNull()) Destroy();
	if (!Create(wd / factor, ht / factor, bpp)) return false;
	if (IsNull()) return false;
	int w = wd / factor, h = ht / factor;
	for (int y = 0; y < h; y++) {
		PBYTE q = (PBYTE)Image.GetPixelAddress(0, y * factor);
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < w; x++) {
			*(p++) = *(q++); *(p++) = *(q++); *(p++) = *(q++);
			q += 3 * (factor - 1);
		}
	}
	// _wd = Image._wd;
	// _ht = Image._ht;
	// _bpp = Image._bpp;
	return true;
}

int CIM::NEdge() {
	UINT sz = edge.width.size();
	int cnt = 0;
	for (UINT i = 0; i < sz; i++) {
		if (edge.width[i] > 0) cnt++;
	}
	return cnt;
}

CString CIM::Report() {
	CString msg;
	msg.Format("N: %d \nAve: %.3f mm, Stdev: %.2f um\nMax: %.3f mm, Min: %.3f mm, Range: %.3f mm\nDist: Outer(%.4f mm)(green) Inner(%.4f mm)(red)",
		NEdge(), StatD.fAve, StatD.fStdev * 1e3f, StatD.fMax, StatD.fMin, StatD.fMax - StatD.fMin, distOut, distIn);
	return msg;
}

void CIM::GetDim(int& wd, int& ht, int& bpp) {
	wd = GetWidth(); ht = GetHeight(); bpp = GetBPP();
}

ULONG CIM::ImageSize() {
	return GetWidth() * GetHeight();
}

ULONG CIM::BufferSize() {
	return abs(GetPitch()) * GetHeight();
}

PVOID CIM::BufferStart() {
	if (GetPitch() < 0) return (PVOID)GetPixelAddress(0, GetHeight() - 1);
	return (PVOID)GetPixelAddress(0, 0);
}

// void CIM::GetDimension()
// {
// 	GetDim(_wd, _ht, _bpp);
// }

COLORREF CIM::GetPixel(int x, int y) {
	BYTE* b = (BYTE*)GetPixelAddress(x, y);
	switch (GetBPP()) {
	case 8: // 8 bits image [11/3/2019 FSM]
		return RGB(*b, *b, *b);
		break;
	case 15: // 15 bits image [11/3/2019 FSM]
	case 16: // 16 bits image [11/3/2019 FSM]
			 // Not supported [11/3/2019 FSM]
		ASSERT(0);
		break;
	case 24: // 24 bits image [11/3/2019 FSM]
	case 32: // 32 bits image [11/3/2019 FSM]
		return RGB(*(b + 2), *(b + 1), *b);
		break;
	}
	return RGB(0, 0, 0);
}

bool CIM::SetPixel(int x, int y, COLORREF& cr) {
	BYTE* b = (BYTE*)GetPixelAddress(x, y);
	switch (GetBPP()) {
	case 8: // 8 bits image [11/3/2019 FSM]
		*b = BYTE((GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3);
		return true;
		break;
	case 15: // 15 bits image [11/3/2019 FSM]
	case 16: // 16 bits image [11/3/2019 FSM]
			 // Not supported [11/3/2019 FSM]
		ASSERT(0);
		break;
	case 24: // 24 bits image [11/3/2019 FSM]
	case 32: // 32 bits image [11/3/2019 FSM]
		*b = GetBValue(cr);
		*(b + 1) = GetGValue(cr);
		*(b + 2) = GetRValue(cr);
		return true;
		break;
	}
	return false;
}

void CIM::AddDiv(CIM& Img) {// Img1 = (Img1 + Img2) / 2 [11/3/2019 FSM]
	if (Img.IsNull()) return;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	if (IsNull() || (ImageSize() != Img.ImageSize())) {
		if (!Create(wd, ht, bpp)) return;
		memset(BufferStart(), 0, BufferSize());
	}
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		BYTE* src = (BYTE*)Img.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8:
				*(des) += *(src++); *(des++) /= 2;
				break;
			case 15:
			case 16:
				break;
			case 24:
				for (int i = 0; i < 3; i++) {
					*(des) += *(src++); *(des++) /= 2;
				}
				break;
			case 32:
				for (int i = 0; i < 3; i++) {
					*(des) += *(src++); *(des++) /= 2;
				}
				*(des++) = *(src++);
				break;
			}
		}
	}
}

void CIM::Sub(CIM& Img) {// Img1 = (Img1 - Img2) / 2 [11/3/2019 FSM]
	if (Img.IsNull()) return;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	if (IsNull() || (ImageSize() != Img.ImageSize())) {
		if (!Create(wd, ht, bpp)) return;
		memset(BufferStart(), 0, BufferSize());
	}
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		BYTE* src = (BYTE*)Img.GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8:
				*(des) -= *(src++);
				break;
			case 15:
			case 16:
				break;
			case 24:
				for (int i = 0; i < 3; i++) {
					*(des) -= *(src++);
				}
				break;
			case 32:
				for (int i = 0; i < 3; i++) {
					*(des) -= *(src++);
				}
				*(des++) = *(src++);
				break;
			}
		}
	}
}

void CIM::ContractStretch() {
	int Max, Min;
	HistoMaxMin(Max, Min);
	ContractStretch(Max, Min);
}

void CIM::ContractStretch(int Max, int Min) {
	if (IsNull()) return;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);

	float df = 255.0f / float(Max - Min);

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		short v;
		BYTE* p = (BYTE*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8:
				v = short((*p - Min) * df);
				if (v < 0) v = 0; if (v > 255) v = 255;
				*(p) = BYTE(v);
				p++;
				break;
			case 15:
			case 16:
				break;
			case 24:
				v = short((*p - Min) * df);
				if (v < 0) v = 0; if (v > 255) v = 255;
				*(p) = BYTE(v);
				*(p + 1) = BYTE(v);
				*(p + 2) = BYTE(v);
				p += 3;
				break;
			case 32:
				v = short((*p - Min) * df);
				if (v < 0) v = 0; if (v > 255) v = 255;
				*(p) = BYTE(v);
				*(p + 1) = BYTE(v);
				*(p + 2) = BYTE(v);
				p += 4;
				break;
			}
		}
	}
}

BOOL CIM::IsSameSize(CIM& DiB) {
	if (IsNull() || DiB.IsNull()) return FALSE;
	if ((DiB.GetWidth() != GetWidth()) || (DiB.GetHeight() != GetHeight()) ||
		(DiB.GetBPP() != GetBPP())) {
		return FALSE;
	}
	return TRUE;
}

BYTE* CIM::GetBufferStart() {
	// Get beginning of image buffer [11/21/2015 Yuen]
	// Caller must lock [1/24/2016 Yuen]
	if (IsNull()) return NULL;
	if (GetPitch() < 0) return (PBYTE)GetPixelAddress(0, GetHeight() - 1);
	return (PBYTE)GetPixelAddress(0, 0);
}

ULONG CIM::GetBufferSize() {
	return GetHeight() * abs(GetPitch());
}

void CIM::DrawPolyFit() {
	UINT sz = edge.Outer.size();
	float* pX = new float[sz];
	float* p = pX;
	for (UINT i = 0; i < sz; i++) {
		*(p++) = float(edge.Outer[i].x);
	}
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	SMatInv a; a.PolyfitFlt(2, pX, sz);
	p = pX;
	for (UINT i = 0; i < sz; i++) {
		int x = int(*(p++));
		int y = edge.Outer[i].y;
		//edge[y].Outer.x = x;
		if ((x >= 0) && (x < wd)) {
			SetPixel(x, y, 255, 0, 0);
		}
	}
	delete[]pX;
}

void CIM::DrawLSqFit(BOOL bOuter) {
	UINT sz = edge.width.size(); if (!sz) return;
	int* pX = new int[sz];
	int* pY = new int[sz];
	int* p = pX, * q = pY;
	int v, maxX, minX, maxY, minY;
	int n = -1, cnt = 0;;
	for (UINT i = 0; i < sz; i++) {// find first valid edge [2/3/2020 yuenl]
		if (edge.width[i] <= 0) continue;
		if (bOuter) {
			maxX = minX = edge.Outer[i].x;
			maxY = minY = edge.Outer[i].y;
		}
		else {
			maxX = minX = edge.Inner[i].x;
			maxY = minY = edge.Inner[i].y;
		}
		n = i + 1;
		*(p++) = maxX; *(q++) = maxY;
		cnt++;
		break;
	}
	if (n == -1) return; // no good edge found [2/3/2020 yuenl]
	for (UINT i = n; i < sz; i++) {
		if (edge.width[i] <= 0) continue;
		if (bOuter) v = edge.Outer[i].x;
		else v = edge.Inner[i].x;
		if (v > maxX) maxX = v; if (v < minX) minX = v;
		*(p++) = v;
		if (bOuter) v = edge.Outer[i].y;
		else v = edge.Inner[i].y;
		if (v > maxY) maxY = v; if (v < minY) minY = v;
		*(q++) = v;
		cnt++;
	}
	if (cnt == 0) return;
	float a, b;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	SMatInv MI; MI.LSQFit(cnt, pX, pY, a, b);
	if ((maxX - minX) > (maxY - minY)) {
		for (int i = 0; i < wd; i++) {
			v = int(a * i + b);
			if ((v > 0) && (v < ht - 1)) {
				SetPixel(i, v - 1, 255, 0, 0);
				SetPixel(i, v, 255, 0, 0);
				SetPixel(i, v + 1, 255, 0, 0);
			}
		}
	}
	else {
		for (int i = 0; i < ht; i++) {
			v = int((i - b) / a);
			if ((v > 0) && (v < wd - 1)) {
				SetPixel(v - 1, i, 255, 0, 0);
				SetPixel(v, i, 255, 0, 0);
				SetPixel(v + 1, i, 255, 0, 0);
			}
		}
	}
	delete[]pX;
	delete[]pY;
}

BOOL CIM::DrawLSqFit2(std::vector<SLoc>& ledge, float Angle, float& a, float& b, BOOL& bXind) {
	UINT sz = ledge.size(); if (!sz) return FALSE;
	bXind = TRUE;
	if ((Angle <= 45.0f) || (Angle >= 315.0f) ||
		((Angle >= 135.0f) && (Angle <= 225.0f))) {// vertical edges [2/1/2020 yuenl]
		bXind = FALSE;
	}

	//FILE* fp = fopen("C:\\TEMP\\EDGE.CSV", "wb");
	//if (fp) {
	//	fprintf(fp, "X,Y,X\n");
	//	for (UINT i = 0; i < sz; i++) {
	//		fprintf(fp, "%d,%d,%d\n", ledge[i].x, ledge[i].y, ledge[i].x);
	//	}
	//	fclose(fp);
	//}

	int* pX = new int[sz];
	int* pY = new int[sz];
	if (!pX || !pY) { if (pX)delete[]pX; if (pY)delete[]pY; return FALSE; }
	int* x = pX, * y = pY;
	for (UINT i = 0; i < sz; i++) {
		*(x++) = ledge[i].x; *(y++) = ledge[i].y;
	}
	RemoveFlyer(sz, pX, pY, bXind, a, b);
	//RemoveFlyer(sz, pX, pY, bXind, a, b);

	ledge.clear();
	x = pX, y = pY;
	for (UINT i = 0; i < sz; i++) {
		ledge.push_back(SLoc(*(x++), *(y++)));
	}

	// 	//////////////////////////////////////////////////////////////////////////
	// 	// Calculate max, min [6/14/2020 yuenl]
	// 	int v, maxX, minX, maxY, minY;
	// 	x = pX; y = pY;
	// 	maxX = minX = *(x++);
	// 	maxY = minY = *(y++);
	// 	for (UINT i = 1; i < sz; i++) {
	// 		v = *(x++);
	// 		if (v > maxX) maxX = v; if (v < minX) minX = v;
	// 		v = *(y++);
	// 		if (v > maxY) maxY = v; if (v < minY) minY = v;
	// 	}

	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	//////////////////////////////////////////////////////////////////////////
	if (bXind) {
		//SMatInv MI; MI.LSQFit(sz, pX, pY, a, b);
		for (int i = 0; i < wd; i++) {
			int v = int(a * i + b);
			if ((v > 0) && (v < ht - 1)) {
				SetPixel(i, v - 1, 255, 0, 60);
				SetPixel(i, v, 255, 0, 0);
				SetPixel(i, v + 1, 255, 0, 0);
			}
		}
	}
	else {
		//SMatInv MI; MI.LSQFit(sz, pY, pX, a, b);
		for (int i = 0; i < ht; i++) {
			int v = int(a * i + b);
			if ((v > 0) && (v < wd - 1)) {
				SetPixel(v - 1, i, 255, 0, 0);
				SetPixel(v, i, 255, 0, 0);
				SetPixel(v + 1, i, 255, 0, 0);
			}
		}
	}
	delete[]pX; delete[]pY;
	return TRUE;
}

void CIM::RemoveFlyer(UINT& sz, int* pX, int* pY, BOOL bXind, float& a, float& b) {
	int cnt = 0;
	float Ave, Stdev;
	int* pXX = pX, * pYY = pY, * x, * y;
	SMatInv MI;
	int wd, ht, bpp;

	GetDim(wd, ht, bpp);
	//wd -= EDG_EXCLUDE; ht -= EDG_EXCLUDE;

	x = pX; y = pY;
	//Dump(sz, pX, pY, "C:\\TEMP\\EDGEBEFORE.CSV");
	if (bXind) {
		AverStdev(sz, pY, Ave, Stdev); // Stdev /= 2.0f;
		if (Stdev > 32) Stdev = 32; if (Stdev < 8) Stdev = 8;
		MI.LSQFit(sz, pX, pY, a, b);
		for (UINT i = 0; i < sz; i++) {
			//if ((*pYY > EDG_EXCLUDE) && (*pYY < ht)) {
			if (abs((*pXX) * a + b - *pYY) < Stdev) {
				*(x++) = *(pXX); *(y++) = *(pYY);
				cnt++;
			}
			//}
			pXX++; pYY++;
		}
		sz = cnt;
		MI.LSQFit(sz, pX, pY, a, b);
	}
	else {
		AverStdev(sz, pX, Ave, Stdev); // Stdev /= 2.0f;
		if (Stdev > 32) Stdev = 32; if (Stdev < 8) Stdev = 8;
		MI.LSQFit(sz, pY, pX, a, b);
		for (UINT i = 0; i < sz; i++) {
			//if ((*pXX > EDG_EXCLUDE) && (*pXX < wd)) {
			if (abs((*pYY) * a + b - *pXX) < Stdev) {
				*(x++) = *(pXX); *(y++) = *(pYY);
				cnt++;
			}
			//}
			pXX++; pYY++;
		}
		sz = cnt;
		MI.LSQFit(sz, pY, pX, a, b);
	}
	//Dump(sz, pX, pY, "C:\\TEMP\\EDGEAFTER.CSV");
}

void CIM::Dump(UINT sz, int* pX, int* pY, char* Filename) {
	FILE* fp = fopen(Filename, "wb");
	if (!fp) return;
	fprintf(fp, "X,Y\n");
	for (UINT i = 0; i < sz; i++) {
		fprintf(fp, "%d,%d\n", *(pX++), *(pY++));
	}
	fclose(fp);
}

void CIM::RangeLEdge(std::vector<SLoc>& Inner, int& dx, int& dy, int* pX, int* pY) {// determine whether the edge is horizontal or vertical
	dx = 0; dy = 0;
	UINT sz = Inner.size(); if (!sz) return;
	int minX, maxX, minY, maxY;
	minX = maxX = Inner[0].x;
	minY = maxY = Inner[0].y;
	*(pX++) = minX; *(pY++) = minY;
	for (UINT i = 1; i < sz; i++) {
		int v = Inner[i].x; *(pX++) = v;
		if (v < minX) minX = v; if (v > maxX) maxX = v;
		v = Inner[i].y; *(pY++) = v;
		if (v < minY) minY = v; if (v > maxY) maxY = v;
	}
	dx = maxX - minX; dy = maxY - minY;
}

void CIM::LSQLEdge(std::vector<SLoc>& Inner, float& a, float& b) {// least square fit edges [2/1/2020 yuenl]
	int dx, dy;
	UINT sz = Inner.size(); if (!sz) return;
	int* pX = new int[sz];
	int* pY = new int[sz];
	if (!pX || !pY) {
		if (pX) delete[]pX;
		if (pY) delete[]pY;
		a = b = 0;
		return;
	}
	RangeLEdge(Inner, dx, dy, pX, pY);
	//DumpEdge("C:\\TEMP\\EDGE0.CSV", pX, pY, sz);
	SMatInv MI;
	if (dx > dy) {
		bVertEdge = FALSE;
		MI.LSQFit(sz, pX, pY, a, b);
	}
	else {
		bVertEdge = TRUE;
		MI.LSQFit(sz, pY, pX, a, b);
	}
	delete[]pX; delete[]pY;
}

void CIM::StraightLine(int x0, int y0, int x, int y, BYTE r, BYTE g, BYTE b) {
	if (x == x0) return; // no vertical line [11/5/2019 FSM]
	int xx, yy;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);

	float m = (y - y0) / float(x - x0);
	float c = y - m * x;
	if (m > 1) {// vary y [11/5/2019 FSM]
		if (y0 < y) {
			for (int i = y0; i < y; i++) {
				xx = int((i - c) / m);
				yy = i;
				if ((xx >= 0) && (xx < wd)) {
					if ((yy > 0) && (yy < ht)) {
						BYTE* des = (BYTE*)GetPixelAddress(xx, yy);
						switch (bpp) {
						case 8:
							*(des) = (r + b + g) / 3;
							break;
						case 15:
						case 16:
							break;
						case 24:
						case 32:
							*(des++) = b; *(des++) = g; *(des) = r;
							break;
						}
					}
				}
			}
		}
		else {
			for (int i = y; i < y0; i++) {
				xx = int((i - c) / m);
				yy = i;
				if ((xx >= 0) && (xx < wd)) {
					if ((yy > 0) && (yy < ht)) {
						BYTE* des = (BYTE*)GetPixelAddress(xx, yy);
						switch (bpp) {
						case 8:
							*(des) = (r + b + g) / 3;
							break;
						case 15:
						case 16:
							break;
						case 24:
						case 32:
							*(des++) = b; *(des++) = g; *(des) = r;
							break;
						}
					}
				}
			}
		}
	}
	else {// vary x [11/5/2019 FSM]
		if (x0 < x) {
			for (int i = x0; i < x; i++) {
				xx = i;
				yy = int(m * i + c);
				if ((xx >= 0) && (xx < wd)) {
					if ((yy > 0) && (yy < ht)) {
						BYTE* des = (BYTE*)GetPixelAddress(xx, yy);
						switch (bpp) {
						case 8:
							*(des) = (r + b + g) / 3;
							break;
						case 15:
						case 16:
							break;
						case 24:
						case 32:
							*(des++) = b; *(des++) = g; *(des) = r;
							break;
						}
					}
				}
			}
		}
		else {
			for (int i = x; i < x0; i++) {
				xx = i;
				yy = int(m * i + c);
				if ((xx >= 0) && (xx < wd)) {
					if ((yy > 0) && (yy < ht)) {
						BYTE* des = (BYTE*)GetPixelAddress(xx, yy);
						switch (bpp) {
						case 8:
							*(des) = (r + b + g) / 3;
							break;
						case 15:
						case 16:
							break;
						case 24:
						case 32:
							*(des++) = b; *(des++) = g; *(des) = r;
							break;
						}
					}
				}
			}
		}
	}
}

void CIM::BThres(short lvl) {// RGB thresholding [11/3/2019 FSM]
	if (IsNull()) return;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		short v;
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8:
				v = *(des)-lvl;
				*(des++) = (v >= 0) ? 255 : 0;
				break;
			case 15:
			case 16:
				break;
			case 24:
				for (int i = 0; i < 3; i++) {
					v = *(des)-lvl;
					*(des++) = (v >= 0) ? 255 : 0;
				}
				break;
			case 32:
				for (int i = 0; i < 3; i++) {
					v = *(des)-lvl;
					*(des++) = (v >= 0) ? 255 : 0;
				}
				des++;
				break;
			}
		}
	}
}

bool CIM::FillEdge() {
	if (IsNull()) return false;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	// Fill left side with black [11/4/2019 FSM]
	for (int y = 0; y < ht; y++) {
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		switch (bpp) {
		case 8:
			for (int x = 0; x < wd; x++) {
				if (*des > THRES)  *(des++) = 0;
				else break;
			}
			break;
		case 15:
		case 16:
			break;
		case 24:
			for (int x = 0; x < wd; x++) {
				if (*des > THRES) {
					*des = 0; *(des + 1) = 0; *(des + 2) = 0;
					des += 3;
				}
				else break;
			}
			break;
		case 32:
			for (int x = 0; x < wd; x++) {
				if (*des > THRES) {
					*des = 0; *(des + 1) = 0; *(des + 2) = 0;
					des += 4;
				}
				else break;
			}
			break;
		}
	}
	// fill right side with black [11/4/2019 FSM]
	for (int y = 0; y < ht; y++) {
		BYTE* des = (BYTE*)GetPixelAddress(wd - 1, y);
		switch (bpp) {
		case 8:
			for (int x = 0; x < wd; x++) {
				if (*des > THRES) *(des--) = 0;
				else break;
			}
			break;
		case 15:
		case 16:
			break;
		case 24:
			for (int x = 0; x < wd; x++) {
				if (*des > THRES) {
					*des = 0; *(des + 1) = 0; *(des + 2) = 0;
					des -= 3;
				}
				else break;
			}
			break;
		case 32:
			for (int x = 0; x < wd; x++) {
				if (*des > THRES) {
					*des = 0; *(des + 1) = 0; *(des + 2) = 0;
					des -= 4;
				}
				else break;
			}
			break;
		}
	}
	return true;
}

short CIM::CountWhite(BYTE* src, short wdw, short bpp, short& lcnt, short& rcnt) {	// count number of white pixels [11/4/2019 FSM]
	int sum = 0;
	int u = wdw / 2;
	int v, a = 0;
	lcnt = 0; rcnt = 0;
	switch (bpp) {
	case 8:
		for (int i = 0; i < u; i++) {
			lcnt += (*(src++) > THRES);
		}
		if (*(src++) > THRES) a = 1;
		for (int i = u + 1; i < wdw; i++) {
			rcnt += (*(src++) > THRES);
		}
		return lcnt + rcnt + a;
		break;
	case 15:
	case 16:
		break;
	case 24:
		for (int i = 0; i < u; i++) {
			v = (*src + *(src + 1) + *(src + 2)) / 3;
			lcnt += (v > THRES);
			src += 3;
		}
		v = (*src + *(src + 1) + *(src + 2)) / 3; if (v > THRES) a = 1;
		src += 3;
		for (int i = u + 1; i < wdw; i++) {
			v = (*src + *(src + 1) + *(src + 2)) / 3;
			rcnt += (v > THRES);
			src += 3;
		}
		return lcnt + rcnt + a;
		break;
	case 32:
		for (int i = 0; i < u; i++) {
			short v = (*src + *(src + 1) + *(src + 2)) / 3;
			lcnt += (v > THRES);
			src += 4;
		}
		v = (*src + *(src + 1) + *(src + 2)) / 3; if (v > THRES) a = 1;
		src += 4;
		for (int i = u + 1; i < wdw; i++) {
			short v = (*src + *(src + 1) + *(src + 2)) / 3;
			rcnt += (v > THRES);
			src += 4;
		}
		return lcnt + rcnt + a;
		break;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Convolution operation [1/5/2020 yuenl]

void CIM::ConvlHSobel() {
	Convl(-1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0);
	//Convl(-1.0, -3.0, -1.0, 0.0, 0.0, 0.0, 1.0, 3.0, 1.0);
	//Convl(-2.0, -3.0, -2.0, 0.0, 0.0, 0.0, 2.0, 3.0, 2.0);
}

void CIM::ConvlVSobel() {
	Convl(-1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0);
	//Convl(-1.0, 0.0, 1.0, -3.0, 0.0, 3.0, -1.0, 0.0, 1.0);
	//Convl(-2.0, 0.0, 2.0, -3.0, 0.0, 3.0, -2.0, 0.0, 2.0);
}

bool CIM::Sobel() {
	if (IsNull()) return false;
	CIM I1, I2;
	I1 = *this; I2 = *this;
	I1.ConvlVSobel();
	I2.ConvlHSobel();
	Magnitude(I1, I2);
	return true;
}

bool CIM::Magnitude(CIM& I1, CIM& I2) {// Note all 3 images must have same dimension and must be 24 bits gray image [1/5/2020 yuenl]
	int w, h, b;
	GetDim(w, h, b); b /= 8; if (b != 3) { ASSERT(0); return false; }
#pragma omp parallel for
	for (int y = 0; y < h; y++) {
		PBYTE pDes = (PBYTE)GetPixelAddress(0, y);
		PBYTE pI1 = (PBYTE)I1.GetPixelAddress(0, y);
		PBYTE pI2 = (PBYTE)I2.GetPixelAddress(0, y);
		for (int x = 0; x < w; x++) {
			BYTE v = BYTE(sqrt((*pI1) * (*pI1) + (*pI2) * (*pI2)));
			if (v < 0)v = 0; if (v > 255) v = 255;
			*(pDes++) = v; *(pDes++) = v; *(pDes++) = v;
			pI1 += b; pI2 += b;
		}
	}
	return false;
}

void CIM::Convl(float k1, float k2, float k3, float k4, float k5, float k6, float k7, float k8, float k9) {
	if (IsNull()) return;

	BYTE* p;						// Image ptr
	int nxx;
	unsigned long nx, ny;
	BYTE* m1, * m2, * m3;				// Pointers to buffers to free()
	BYTE* old_r1, * r1, * r2, * r3;	// Cycling pointers to rows
	float fsum;
	short byPP;

	nx = GetWidth();
	ny = GetHeight();
	byPP = GetBPP() / 8;
	nxx = (nx)*byPP;

	p = (BYTE*)GetBufferStart();// Ptr to bitmap
								// Allocate row buffers
	m1 = new BYTE[(nx + 2) * byPP];
	m2 = new BYTE[(nx + 2) * byPP];
	m3 = new BYTE[(nx + 2) * byPP];
	if (!m1 || !m2 || !m3) {
		if (m1) delete[] m1; if (m2) delete[] m2; if (m3) delete[] m3;
		return;
	}
	r1 = m1; r2 = m2; r3 = m3;

	int width = nx * byPP;
	// Initialize rows
	p = (BYTE*)GetPixelAddress(0, 0);
	memcpy_s(&r1[3], width, p, width);
	p = (BYTE*)GetPixelAddress(0, 1);
	memcpy_s(&r2[3], width, p, width);

	r1[0] = r1[3]; r1[1] = r1[4]; r1[2] = r1[5];
	r1[width + 0] = r1[width - 3];
	r1[width + 1] = r1[width - 2];
	r1[width + 2] = r1[width - 1];
	r2[0] = r2[3]; r2[1] = r2[4]; r2[2] = r2[5];
	r2[width + 0] = r2[width - 3];
	r2[width + 1] = r2[width - 2];
	r2[width + 2] = r2[width - 1];

	// Calc. sum of kernel
	fsum = k1 + k2 + k3 + k4 + k5 + k6 + k7 + k8 + k9;
	if (fsum == 0) fsum = 1; // Avoid div. by 0
	else fsum = 1 / fsum; // Invert so can mult.

	for (int j = 1; j < int(ny - 1); j++) {
		if (j == ny) r3 = r2;
		else  /* Read next row (into the 3rd row) */
		{
			p = (BYTE*)GetPixelAddress(0, j + 1);
			memcpy_s(&r3[3], width, p, width);
			r3[0] = r3[3]; r3[1] = r3[4]; r3[2] = r3[5];
			r3[width + 0] = r3[width - 3];
			r3[width + 1] = r3[width - 2];
			r3[width + 2] = r3[width - 1];
		}

		BYTE* pp = (BYTE*)GetPixelAddress(0, j);
#pragma omp parallel for
		for (int i = 0; i < int(nx); i++) {
			short nByte = i * byPP;
			BYTE* p0 = pp + nByte;
			BYTE* r11 = r1 + nByte;
			BYTE* r22 = r2 + nByte;
			BYTE* r33 = r3 + nByte;
			if (byPP > 3) { r11++; r22++; r33++; p0++; }
			// Red component
			float s;
			s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
				+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
				+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];
			BYTE v = (BYTE)THRESH(NINT(s * fsum));
			*(p0++) = v;
			*(p0++) = v;
			*(p0++) = v;
			r11 += byPP; r22 += byPP; r33 += byPP;

			// Green component
			// s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
			// 	+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
			// 	+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];
			// *p0 = (BYTE)THRESH(NINT(s * fsum));
			// r11++; r22++; r33++; p0++;

			// Blue component
			// s = k1 * r11[0] + k2 * r11[3] + k3 * r11[6]
			// 	+ k4 * r22[0] + k5 * r22[3] + k6 * r22[6]
			// 	+ k7 * r33[0] + k8 * r33[3] + k9 * r33[6];
			// *p = (BYTE)THRESH(NINT(s * fsum));
		}
		/* Cycle row pointers */
		old_r1 = r1;
		r1 = r2;
		r2 = r3;
		r3 = old_r1;
	}

	delete[] m1;
	delete[] m2;
	delete[] m3;
}

short CIM::CountBlack(BYTE* src, short wdw, short bpp, short& lcnt, short& rcnt) {
	int sum = 0;
	int u = wdw / 2;
	lcnt = 0; rcnt = 0;
	switch (bpp) {
	case 8:
		for (int i = 0; i < u; i++) {
			lcnt += (*(src++) < THRES);
		}
		src++;
		for (int i = u + 1; i < wdw; i++) {
			rcnt += (*(src++) < THRES);
		}
		return lcnt + rcnt;
		break;
	case 15:
	case 16:
		break;
	case 24:
		for (int i = 0; i < u; i++) {
			short v = (*src + *(src + 1) + *(src + 2)) / 3;
			lcnt += (v < THRES);
			src += 3;
		}
		src += 3;
		for (int i = u + 1; i < wdw; i++) {
			short v = (*src + *(src + 1) + *(src + 2)) / 3;
			rcnt += (v < THRES);
			src += 3;
		}
		return lcnt + rcnt;
		break;
	case 32:
		for (int i = 0; i < u; i++) {
			short v = (*src + *(src + 1) + *(src + 2)) / 3;
			lcnt += (v < THRES);
			src += 4;
		}
		src += 4;
		for (int i = u + 1; i < wdw; i++) {
			short v = (*src + *(src + 1) + *(src + 2)) / 3;
			rcnt += (v < THRES);
			src += 4;
		}
		return lcnt + rcnt;
		break;
	}
	return 0;
}

short CIM::Average(BYTE* src, short wdw, short bpp) {
	int sum = 0;
	switch (bpp) {
	case 8:
		for (int i = 0; i < wdw; i++) {
			sum += *(src++);
		}
		return sum / wdw;
		break;
	case 15:
	case 16:
		break;
	case 24:
		for (int i = 0; i < wdw; i++) {
			sum += (*(src)+*(src + 1) + *(src + 2)) / 3;
			src += 3;
		}
		return sum / wdw;
		break;
	case 32:
		for (int i = 0; i < wdw; i++) {
			sum += (*(src)+*(src + 1) + *(src + 2)) / 3;
			src += 4;
		}
		return sum / wdw;
		break;
	}
	return 0;
}

int CIM::MinValue() {
	short v, min = 256;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	for (int y = 0; y < ht; y++) {
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8:
				if (*des < min) min = *des; des++;
				break;
			case 15:
			case 16:
				break;
			case 24:
				v = (*(des)+*(des + 1) + *(des + 2)) / 3;
				if (v < min) min = v;
				des += 3;
				break;
			case 32:
				v = (*(des)+*(des + 1) + *(des + 2)) / 3;
				if (v < min) min = v;
				des += 4;
				break;
			}
		}
	}
	return min;
}

bool CIM::IsInRect(int x, int y, int wd, int ht) {// _wd and _ht must be valid on entry [1/7/2020 yuenl]
	if ((x < 0) || (x >= wd)) return false;
	if ((y < 0) || (y >= ht)) return false;
	return true;
}

int CIM::GetPx(float angle, int dy) {
	return int(dy / tan(angle * TORADIAN));
}

int CIM::GetPy(float angle, int dx) {
	if (angle == 90) return 0;
	else if (angle == 270) return 0;
	return int(dx * tan(angle * TORADIAN));
}

void CIM::SweepXTop(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight) {
	int wd, ht, bypp;

	while (angle >= 360.0f) angle -= 360.0f;
	while (angle < 0.0f) angle += 360.0f;

	GetDim(wd, ht, bypp); bypp /= 8;
	wd -= EDG_EXCLUDE;

	// not suitable for parallel computation due to ledge
	for (int x = EDG_EXCLUDE; x < wd; x += EDG_RES) {
		for (int y = 0; y < ht; y++) {
			PBYTE des = (PBYTE)GetPixelAddress(x, y);
			if (*des == pix) {
				ledge.push_back(SLoc(x, y));
				break; // continue looking at next line [6/12/2020 yuenl]
			}
			else {
				if (!bRight) { // red [6/19/2020 yuenl]
					*des = 0; *(des + 1) = 0;
				}
				else { // green [6/19/2020 yuenl]
					*des = 0; *(des + 2) = 0;
				}
			}
		}
	}
}

void CIM::SweepXBottom(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight) {
	int wd, ht, bypp;
	GetDim(wd, ht, bypp); bypp /= 8;
	wd -= EDG_EXCLUDE;

	while (angle >= 360.0f) angle -= 360.0f;
	while (angle < 0.0f) angle += 360.0f;

	// not suitable for parallel computation due to ledge
	for (int x = EDG_EXCLUDE; x < wd; x += EDG_RES) {
		for (int y = ht - 1; y >= 0; y--) {
			PBYTE des = (PBYTE)GetPixelAddress(x, y);
			if (*des == pix) {
				ledge.push_back(SLoc(x, y));
				break; // continue looking at next line [6/12/2020 yuenl]
			}
			else {
				if (!bRight) { // red [6/19/2020 yuenl]
					*des = 0; *(des + 1) = 0;
				}
				else { // green [6/19/2020 yuenl]
					*des = 0; *(des + 2) = 0;
				}
			}
		}
	}
}

void CIM::SweepX(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight) {
	if (bRight) {
		if ((angle > 45.0f) && (angle < 135.0f))
			SweepXTop(ledge, angle, pix, bRight);
		else
			SweepXBottom(ledge, angle, pix, bRight);
	}
	else {
		if ((angle > 45.0f) && (angle < 135.0f))
			SweepXBottom(ledge, angle, pix, bRight);
		else
			SweepXTop(ledge, angle, pix, bRight);
	}
}

void CIM::SweepYLeft(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight) {
	int wd, ht, bypp;
	GetDim(wd, ht, bypp); bypp /= 8;
	ht -= EDG_EXCLUDE;

	while (angle >= 360.0f) angle -= 360.0f;
	while (angle < 0.0f) angle += 360.0f;

	// not suitable for parallel computation due to ledge
	for (int y = EDG_EXCLUDE; y < ht; y += EDG_RES) {
		PBYTE des = (PBYTE)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			if (*des == pix) { // TODO: Consider using <= or >= comparison [6/12/2020 yuenl]
				ledge.push_back(SLoc(x, y));
				break; // continue looking at next line [6/12/2020 yuenl]
			}
			else {
				if (!bRight) { // red [6/19/2020 yuenl]
					*des = 0; *(des + 1) = 0;
				}
				else { // green [6/19/2020 yuenl]
					*des = 0; *(des + 2) = 0;
				}
			}
			des += bypp;
		}
	}
}

void CIM::SweepYRight(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight) {
	int wd, ht, bypp;
	GetDim(wd, ht, bypp); bypp /= 8;
	ht -= EDG_EXCLUDE;

	while (angle >= 360.0f) angle -= 360.0f;
	while (angle < 0.0f) angle += 360.0f;

	// not suitable for parallel computation due to ledge
	for (int y = EDG_EXCLUDE; y < ht; y += EDG_RES) {
		PBYTE des = (PBYTE)GetPixelAddress(wd - 1, y);
		for (int x = wd - 1; x >= 0; x--) {
			if (*des == pix) {
				ledge.push_back(SLoc(x, y));
				break; // continue looking at next line [6/12/2020 yuenl]
			}
			else {
				if (!bRight) { // red [6/19/2020 yuenl]
					*des = 0; *(des + 1) = 0;
				}
				else { // green [6/19/2020 yuenl]
					*des = 0; *(des + 2) = 0;
				}
			}
			des -= bypp;
		}
	}
}

void CIM::SweepY(std::vector<SLoc>& ledge, float angle, BYTE pix, BOOL bRight) {
	if (bRight) {
		if ((angle <= 45.0f) || (angle >= 315.f))
			SweepYRight(ledge, angle, pix, bRight);
		else
			SweepYLeft(ledge, angle, pix, bRight);
	}
	else {
		if ((angle <= 45.0f) || (angle >= 315.f))
			SweepYLeft(ledge, angle, pix, bRight);
		else
			SweepYRight(ledge, angle, pix, bRight);
	}
}

CPtLine::Vec2 CIM::FindBand(float angle) {// assume white background [2/3/2020 yuenl]
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	int max = 0, mst = 0, med = 0;
	if (((angle > 45.0f) && (angle < 135.0f)) || ((angle > 225.0f) && (angle < 315.0f))) {
		// find band in Y direction [2/3/2020 yuenl]
		int st = 0, ed = 0;
		for (int i = 0; i < ht; i++) {
			PBYTE p = (PBYTE)GetPixelAddress(wd / 2, i);
			if (*p > 127) continue;
			st = i;
			for (int j = i + 1; j < ht; j++) {
				p = (PBYTE)GetPixelAddress(wd / 2, j);
				if (*p < 127) continue;
				ed = j;
				if (max < (ed - st)) { max = ed - st; mst = st; med = ed; }
				i = j + 1; break;
			}
		}
		if (max > 0) {
			return CPtLine::Vec2(wd / 2.0f, (med + mst) / 2.0f);
		}
	}
	else {
		// find band in X direction [2/3/2020 yuenl]
		PBYTE p = (PBYTE)GetPixelAddress(0, ht / 2);
		int st = 0, ed = 0;
		for (int i = 0; i < wd; i++) {
			if (*p > 127) {
				p += 3; continue;
			}
			st = i;
			for (int j = i + 1; j < wd; j++) {
				if (*p < 127) {
					p += 3; continue;
				}
				ed = j;
				if (max < (ed - st)) { max = ed - st; mst = st; med = ed; }
				i = j + 1; break;
			}
		}
		if (max > 0) {
			return CPtLine::Vec2((med + mst) / 2.0f, ht / 2.0f);
		}
	}
	return CPtLine::Vec2(wd / 2.0f, ht / 2.0f); // no band found [2/3/2020 yuenl]
}

BOOL CIM::FindEdge(std::vector<SLoc>& Inner, float angle, BYTE pix, BOOL bRight) {
	Inner.clear();
	if ((angle <= 45.0f) || (angle >= 315.0f) ||
		((angle >= 135.0f) && (angle <= 225.0f))) {// vertical edges [2/1/2020 yuenl]
		SweepY(Inner, angle, pix, bRight);
		return TRUE;
	}
	else {// horizontal edges [2/1/2020 yuenl]
		SweepX(Inner, angle, pix, bRight);
		return TRUE;
	}
	return FALSE;
}

bool CIM::SweepToOuterX(SLoc& Outer, float ang, BYTE bg, int cx, int cy, BOOL bInv, BOOL bVert) {// _wd & _ht must be valid on entry [1/7/2020 yuenl]
	PBYTE des;
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	bool bfound = false;
	if (bInv) { // invert coordinate [6/12/2020 yuenl]
		if (bVert) { // y sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int y = cy - 1; y >= 0; y--) {// search toward -ve y from cy [1/5/2020 yuenl]
				int x = GetPy(ang, y - cy) + cx;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 255; *(des++) = 0; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Outer.x = x; Outer.y = y;
					for (int i = 0; i < 8; i++) {
						y--;
						x = GetPx(ang, y - cy) + cx;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
		else { // x sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int x = cx - 1; x >= 0; x--) {// search toward -ve x from cy [1/5/2020 yuenl]
				int y = GetPy(ang, x - cx) + cy;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 255; *(des++) = 0; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Outer.x = x; Outer.y = y;
					for (int i = 0; i < 8; i++) {
						x--;
						y = GetPy(ang, x - cx) + cy;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
	}
	else { // not invert coordinate [6/12/2020 yuenl]
		if (bVert) { // y sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int y = cy; y < ht; y++) {// search toward +ve y from cy [1/5/2020 yuenl]
				int x = GetPy(ang, y - cy) + cx;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 255; *(des++) = 0; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Outer.x = x; Outer.y = y;
					for (int i = 0; i < 8; i++) {
						y++;
						x = GetPy(ang, y - cy) + cx;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
		else { // x sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int x = cx; x < wd; x++) {// search toward +ve x from cx [1/5/2020 yuenl]
				int y = GetPy(ang, x - cx) + cy;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 255; *(des++) = 0; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Outer.x = x; Outer.y = y;
					for (int i = 0; i < 8; i++) {
						x++;
						y = GetPy(ang, x - cx) + cy;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
	}
	return bfound;
}

bool CIM::SweepToInnerX(SLoc& Inner, float ang, BYTE bg, int cx, int cy, BOOL bInv, BOOL bVert) {// _wd & _ht must be valid on entry [1/7/2020 yuenl]
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	PBYTE des;
	bool bfound = false;
	if (bInv) { // invert coordinate [6/12/2020 yuenl]
		if (bVert) { // y sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int y = cy; y < ht; y++) {// search toward +ve y from cy [1/5/2020 yuenl]
				int x = GetPy(ang, y - cy) + cx;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 0; *(des++) = 255; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Inner.x = x; Inner.y = y;
					for (int i = 0; i < 8; i++) {
						y++;
						x = GetPx(ang, y - cy) + cx;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
		else {
			int counter = 0;
			for (int x = cx; x < wd; x++) {// search toward +ve x from cx [1/5/2020 yuenl]
				int y = GetPy(ang, x - cx) + cy;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 0; *(des++) = 255; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Inner.x = x; Inner.y = y;
					for (int i = 0; i < 8; i++) {
						x++;
						y = GetPy(ang, x - cx) + cy;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
	}
	else { // not invert coordinate [6/12/2020 yuenl]
		if (bVert) { // y sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int y = cy - 1; y >= 0; y--) {// search toward -ve y from cy [1/5/2020 yuenl]
				int x = GetPy(ang, y - cy) + cx;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 0; *(des++) = 255; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Inner.x = x; Inner.y = y;
					for (int i = 0; i < 8; i++) {
						y--;
						x = GetPx(ang, y - cy) + cx;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
		else { // x sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int x = cx - 1; x >= 0; x--) {// search toward -ve from cx [1/5/2020 yuenl]
				int y = GetPy(ang, x - cx) + cy;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 0; *(des++) = 255; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Inner.x = x; Inner.y = y;
					for (int i = 0; i < 8; i++) {
						x--;
						y = GetPy(ang, x - cx) + cy;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
	}
	return bfound;
}

bool CIM::SweepToOuterY(SLoc& Outer, float ang, BYTE bg, int cx, int cy, BOOL bInv, BOOL bVert) {// _wd & _ht must be valid on entry [1/7/2020 yuenl]
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	PBYTE des;
	bool bfound = false;
	if (bInv) { // invert coordinate [6/12/2020 yuenl]
		if (bVert) { // y sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int y = cy - 1; y >= 0; y--) {// search toward -ve y from cy [1/5/2020 yuenl]
				int x = GetPx(ang, y - cy) + cx;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 255; *(des++) = 0; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Outer.x = x; Outer.y = y;
					for (int i = 0; i < 8; i++) {
						y--;
						x = GetPx(ang, y - cy) + cx;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
		else { // x sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int x = cx - 1; x >= 0; x--) {// search toward -ve x from cx [1/5/2020 yuenl]
				int y = GetPy(ang, x - cx) + cy;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 255; *(des++) = 0; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Outer.x = x; Outer.y = y;
					for (int i = 0; i < 8; i++) {
						x--;
						y = GetPy(ang, x - cx) + cy;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
	}
	else { // not invert coordinate [6/12/2020 yuenl]
		if (bVert) { // y sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int y = cy; y < ht; y++) {// search toward +ve y from cy [1/5/2020 yuenl]
				int x = GetPx(ang, y - cy) + cx;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 255; *(des++) = 0; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Outer.x = x; Outer.y = y;
					for (int i = 0; i < 8; i++) {
						y++;
						x = GetPx(ang, y - cy) + cx;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
		else { // x sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int x = cx; x < wd; x++) {// search toward +ve x from cx [1/5/2020 yuenl]
				int y = GetPy(ang, x - cx) + cy;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 255; *(des++) = 0; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Outer.x = x; Outer.y = y;
					for (int i = 0; i < 8; i++) {
						x++;
						y = GetPy(ang, x - cx) + cy;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
	}
	return bfound;
}

bool CIM::SweepToInnerY(SLoc& Inner, float ang, BYTE bg, int cx, int cy, BOOL bInv, BOOL bVert) {// _wd & _ht must be valid on entry [1/7/2020 yuenl]
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
	PBYTE des;
	bool bfound = false;
	if (bInv) { // invert coordinate [6/12/2020 yuenl]
		if (bVert) { // y sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int y = cy; y < ht; y++) {// search toward +ve y from cy [1/5/2020 yuenl]
				int x = GetPx(ang, y - cy) + cx;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 0; *(des++) = 255; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Inner.x = x; Inner.y = y;
					for (int i = 0; i < 8; i++) {
						y++;
						x = GetPx(ang, y - cy) + cx;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
		else { // x sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int x = cx; x < wd; x++) {// search toward +ve x from cx [1/5/2020 yuenl]
				int y = GetPy(ang, x - cx) + cy;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 0; *(des++) = 255; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Inner.x = x; Inner.y = y;
					for (int i = 0; i < 8; i++) {
						x++;
						y = GetPy(ang, x - cx) + cy;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
	}
	else { // not invert coordinate [6/12/2020 yuenl]
		if (bVert) { // y sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int y = cy - 1; y >= 0; y--) {// search toward -ve y from cy [1/5/2020 yuenl]
				int x = GetPx(ang, y - cy) + cx;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 0; *(des++) = 255; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Inner.x = x; Inner.y = y;
					for (int i = 0; i < 8; i++) {
						y--;
						x = GetPx(ang, y - cy) + cx;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
		else { // x sweep [6/12/2020 yuenl]
			int counter = 0;
			for (int x = cx - 1; x >= 0; x--) {// search toward -ve x from cx [1/5/2020 yuenl]
				int y = GetPy(ang, x - cx) + cy;
				if (IsInRect(x, y, wd, ht)) {
					des = (PBYTE)GetPixelAddress(x, y);
					if (*des != bg) {// is white [1/5/2020 yuenl]
						counter++;
						*(des++) = 0; *(des++) = 0; *(des++) = 255; continue;
					}
					else bfound = true;
					//if (counter < 256) return false;
					Inner.x = x; Inner.y = y;
					for (int i = 0; i < 8; i++) {
						x--;
						y = GetPy(ang, x - cx) + cy;
						if (IsInRect(x, y, wd, ht)) {
							des = (PBYTE)GetPixelAddress(x, y);
							*(des++) = 0; *(des++) = 255; *(des++) = 255;
						}
					}
					break;
				}
				break;
			}
		}
	}
	return bfound;
}

bool CIM::FindEdgesInner(SLoc& Inner, int cx, int cy, float ang, BYTE bg, BOOL bInv, BOOL bVert) {
	if (!bVert) return SweepToInnerX(Inner, ang, bg, cx, cy, bInv, bVert);//*
	return SweepToInnerY(Inner, ang, bg, cx, cy, bInv, bVert);
}

bool CIM::FindEdgesOuter(SLoc& Outer, int cx, int cy, float ang, BYTE bg, BOOL bInv, BOOL bVert) {
	if (!bVert) return SweepToOuterX(Outer, ang, bg, cx, cy, bInv, bVert);//*
	return SweepToOuterY(Outer, ang, bg, cx, cy, bInv, bVert);
}

bool CIM::FindEdgesDark(float angle, int cx, int cy) {	// start from middle, look for intensity transition from white to black [11/4/2019 FSM]
	// Assume object is white and background is black [11/4/2019 FSM]
	if (IsNull()) return false;

	while (angle >= 360.0f) angle -= 360.0f;
	while (angle < 0.0f) angle += 360.0f;

	int wd, ht, bpp;
	GetDim(wd, ht, bpp);

	int x, y;
	float fx, fy;
	float rangeR = float(sqrt(long(cx) * cx + long(cy) * cy)) * 0.50f;
	edge.Clear();
	SLoc Outer, Inner;
	if ((angle == 0) || (angle == 180)) {
		BOOL bInv = FALSE; if (angle == 180) bInv = TRUE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(int(x), int(y), wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 0, bInv, FALSE)) bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 0, bInv, FALSE)) bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		return true;
	}
	if ((angle == 90) || (angle == 270)) {
		BOOL bInv = FALSE; if (angle == 270) bInv = TRUE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, cy, angle, 0, bInv, TRUE))bret = false;
				if (!FindEdgesInner(Inner, x, cy, angle, 0, bInv, TRUE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		return true;
	}
	if ((angle <= 45.0f) || (angle >= 315.0f)) {
		BOOL bInv = FALSE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 0, bInv, FALSE))bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 0, bInv, FALSE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		return true;
	}
	if ((angle > 45.0f) && (angle < 135.0f)) {
		BOOL bInv = FALSE; //if (angle > 90) bInv = TRUE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 0, bInv, TRUE))bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 0, bInv, TRUE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		return true;
	}
	if ((angle >= 135.0f) && (angle <= 225.0f)) {
		BOOL bInv = TRUE; //if (angle > 180) bInv = TRUE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 0, bInv, FALSE))bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 0, bInv, FALSE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		return true;
	}
	if ((angle > 225.0f) && (angle < 315.0f)) {
		BOOL bInv = TRUE; //if (angle > 270) bInv = FALSE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 0, bInv, TRUE))bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 0, bInv, TRUE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		return true;
	}
	return false;
}

bool CIM::FindEdgesBright(float angle, int cx, int cy) {	// start from middle, look for intensity transition from white to black [11/4/2019 FSM]
	// Assume object is white and background is black [11/4/2019 FSM]
	if (IsNull()) return false;

	while (angle >= 360.0f) angle -= 360.0f;
	while (angle < 0.0f) angle += 360.0f;

	int wd, ht, bpp;
	GetDim(wd, ht, bpp);

	int x, y;
	float fx, fy;
	//int cx = _wd / 2, cy = _ht / 2;
	float rangeR = float(sqrt(long(cx) * cx + long(cy) * cy)) * 0.50f;
	edge.Clear();
	SLoc Outer, Inner;
	if ((angle == 0) || (angle == 180)) {
		BOOL bInv = FALSE; if (angle == 180) bInv = TRUE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(int(x), int(y), wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 255, bInv, FALSE)) bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 255, bInv, FALSE)) bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		//SmoothLeft(9, 3, bpp);
		//DrawPolyFit();
		return true;
	}
	if ((angle == 90) || (angle == 270)) {
		BOOL bInv = FALSE; if (angle == 270) bInv = TRUE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, cy, angle, 255, bInv, TRUE))bret = false;
				if (!FindEdgesInner(Inner, x, cy, angle, 255, bInv, TRUE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		//SmoothLeft(9, 3, bpp);
		//DrawPolyFit();
		return true;
	}
	if ((angle <= 45.0f) || (angle >= 315.0f)) {
		BOOL bInv = FALSE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 255, bInv, FALSE))bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 255, bInv, FALSE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		//SmoothLeft(9, 3, bpp);
		//DrawPolyFit();
		return true;
	}
	if ((angle > 45.0f) && (angle < 135.0f)) {
		BOOL bInv = FALSE; //if (angle > 90) bInv = TRUE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 255, bInv, TRUE))bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 255, bInv, TRUE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		//SmoothLeft(9, 3, bpp);
		//DrawPolyFit();
		return true;
	}
	if ((angle >= 135.0f) && (angle <= 225.0f)) {
		BOOL bInv = TRUE; //if (angle > 180) bInv = TRUE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 255, bInv, FALSE))bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 255, bInv, FALSE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		//SmoothLeft(9, 3, bpp);
		//DrawPolyFit();
		return true;
	}
	if ((angle > 225.0f) && (angle < 315.0f)) {
		BOOL bInv = TRUE; //if (angle > 270) bInv = FALSE;
		for (float r = -rangeR; r <= rangeR; r += EDG_RES) {
			Polar2Cart(r, angle + 90, fx, fy);
			x = int(fx + cx); y = int(fy + cy);
			if (IsInRect(x, y, wd, ht)) {
				bool bret = true;
				if (!FindEdgesOuter(Outer, x, y, angle, 255, bInv, TRUE))bret = false;
				if (!FindEdgesInner(Inner, x, y, angle, 255, bInv, TRUE))bret = false;
				if (bret) {
					edge.Outer.push_back(Outer); edge.Inner.push_back(Inner);
				}
			}
		}
		//SmoothLeft(9, 3, bpp);
		//DrawPolyFit();
		return true;
	}
	return false;
}

void CIM::SmoothLeft(std::vector<SLoc>& Inner, short wdw, short ntime, short bpp) {
	UINT ndata = Inner.size(); if (ndata < 1) return;
	int* pdata = new int[ndata];
	int* ydd = new int[ndata];
	if (!pdata || !ydd) {
		if (pdata) delete[] pdata;
		if (ydd) delete[] ydd;
		return;
	}
	int* pX = pdata;
	for (UINT i = 0; i < ndata; i++) {
		*(pX++) = Inner[i].x;
	}
	int sum;
	for (int j = 0; j < ntime; j++) {
		pX = pdata;
		for (UINT i = 0; i < ndata; i++) {
			sum = 0;
			int l = 0;
			for (int k = -wdw; k <= wdw; k++) {
				if ((i + k >= 0) && (i + k < ndata)) {
					sum += pX[i + k];
					l++;
				}
			}
			if (l > 0) ydd[i] = sum / l;
			else ydd[i] = pX[i];
		}
		// Copy back to main buffer [2/19/2019 yuenl]
		memcpy(pdata, ydd, ndata * sizeof(int));
	}
	// copy back to edge [11/7/2019 yuenl]
	pX = pdata;
	for (UINT i = 0; i < ndata; i++) {
		Inner[i].x = *(pX++);
		if (bpp == 8)
			SetPixel(Inner[i].x, Inner[i].y, 255, 255, 255);
		else
			SetPixel(Inner[i].x, Inner[i].y, 0, 255, 0);
	}

	if (ydd) delete[] ydd;
	if (pdata) delete[] pdata;
}

int CIM::NearestDistance0(std::vector<SLoc>& Inner, std::vector<SLoc>& Outer, int wdw, int nn) {// check first nearest distance [11/6/2019 FSM]
	int j;
	long min, v;
	int idx = 9999;
	int x0 = Inner[nn].x;
	int y0 = Inner[nn].y;

	min = sqre(Outer[nn].x - x0) + sqre(Outer[nn].y - y0);
	idx = nn;
	// large angle search [11/6/2019 FSM]
	for (int i = 1; i < wdw; i++) {
		j = nn + i;
		v = sqre(Outer[j].x - x0) + sqre(Outer[j].y - y0);
		if (v < min) { min = v; idx = j; }
		j = nn - i;
		v = sqre(Outer[j].x - x0) + sqre(Outer[j].y - y0);
		if (v < min) { min = v; idx = j; }
	}
	return idx - nn;
}

bool CIM::Distance(float BevelWidth, float sfX, float sfY) {// find selected point on left edge to right edge, right edge assumed to be straight [11/5/2019 FSM]
	edge.width.clear();
	if (IsNull()) return false;
	int n = 0;
	if (edge.Outer.size() < edge.Inner.size()) n = edge.Outer.size();
	else n = edge.Inner.size(); if (n < 1)return false;
	for (int i = 0; i < n; i++) {
		float dx = sfX * (edge.Outer[i].x - edge.Inner[i].x);
		float dy = sfY * (edge.Outer[i].y - edge.Inner[i].y);
		edge.width.push_back(float(sqrt(dx * dx + dy * dy)) + BevelWidth);
	}
	CalcStat();
	return true;
}

bool CIM::Distance1(std::vector<SLoc>& Inner, float BevelWidth, float sfX, float sfY, CPtLine::Vec2& v, CPtLine::Vec2& w) {// v and w are two points on the straight line [6/19/2020 yuenl]
	edge.width.clear();
	if (IsNull()) return false;
	int n = n = Inner.size(); if (n < 1)return false;
	for (int i = 0; i < n; i++) {
		CPtLine Ln;
		CPtLine::Vec2 p, q;
		p._x = float(Inner[i].x); p._y = float(Inner[i].y);
		Ln.DistancePointToLine(v, w, p, &q);
		float dx = sfX * (q._x - p._x);
		float dy = sfY * (q._y - p._y);
		edge.width.push_back(float(sqrt(dx * dx + dy * dy)) + BevelWidth);
	}
	CalcStat();
	return true;
}

CPtLine::Vec2 CIM::CTransform(CPtLine::Vec2 d, float ang) {
	float cost = float(cos(ang * TORADIAN));
	float sint = float(sin(ang * TORADIAN));
	return CPtLine::Vec2((d._x * cost + d._y * sint), (-d._x * sint + d._y * cost));
}

float CIM::GetDisplacement(std::vector<SLoc>& Inner, float ang, CPtLine::Vec2& p, CPtLine::Vec2& q) {   // Calculate distance of edge from center of image [6/17/2020 yuenl]
	// p is center of image [6/17/2020 yuenl]
	// q is the nearest on the edge [6/17/2020 yuenl]
	UINT sz = Inner.size(); if (!sz) return 0;
	sz--;
	CPtLine::Vec2 v1(float(Inner[0].x), float(Inner[0].y));
	CPtLine::Vec2 v2(float(Inner[sz].x), float(Inner[sz].y));
	CPtLine Ln;
	float v = Ln.DistancePointToLine(v1, v2, p, &q);
	if ((ang < 45.0f) || (ang > 315.0f)) {
		if (p._x < q._x) return v;
		else return -v;
	}
	if ((ang > 135.0f && ang < 225.0f)) {
		if (p._x < q._x) return -v;
		else return v;
	}
	if ((ang >= 45.0f) && (ang <= 135.0f)) {
		if (p._y < q._y) return -v;
		else return v;
	}
	if ((ang >= 225.0f) && (ang <= 315.0f)) {
		if (p._y < q._y) return v;
		else return -v;
	}
	return 0;
}

void CIM::SetPixel(int x, int y, BYTE r, BYTE g, BYTE b) {
	BYTE* p = (BYTE*)GetPixelAddress(x, y);
	switch (GetBPP()) {
	case 8: *p = r; break;
	case 15: break;
	case 16: break;
	case 24:
	case 32:
		*(p++) = b; *(p++) = g; *(p) = r;
		break;
	}
}

bool CIM::GetInnerEdge(std::vector<SLoc>& Outer, float& x, float& y) {
	x = 0; y = 0;
	UINT sz = Outer.size(); if (sz < 1) return false;
	for (UINT i = 0; i < sz; i++) {
		x += Outer[i].x;
		y += Outer[i].y;
	}
	x /= float(sz);
	y /= float(sz);
	return false;
}

bool CIM::GetOuterEdge(std::vector<SLoc>& Inner, float& x, float& y) {
	x = 0; y = 0;
	UINT sz = Inner.size(); if (sz < 1) return false;
	for (UINT i = 0; i < sz; i++) {
		x += Inner[i].x;
		y += Inner[i].y;
	}
	x /= float(sz);
	y /= float(sz);
	return true;
}

void CIM::DoStat(std::vector<float>& width) {
	StatD.Clear();
	UINT sz = width.size(); if (sz < 1) return;
	// first pass, calculate statistics [1/7/2020 yuenl]

	BOOL bFirst = TRUE;
	// max, min, ave [11/6/2019 FSM]
	int cnt = 0; double sum = 0;
	for (UINT i = 0; i < sz; i++) {
		if (width[i] < 0.0f) continue;
		if (bFirst) {
			bFirst = FALSE; StatD.fMax = StatD.fMin = width[i];
		}
		float v = width[i];
		sum += v;
		if (v > StatD.fMax) StatD.fMax = v;
		if (v < StatD.fMin) StatD.fMin = v;
		cnt++;
	}
	StatD.fAve = 0;
	if (cnt > 0) StatD.fAve = float(sum / float(cnt));
	// stdev [11/6/2019 FSM]
	StatD.fStdev = 0;
	for (UINT i = 0; i < sz; i++) {
		if (width[i] < 0.0f) continue;
		float dif = width[i] - StatD.fAve;
		StatD.fStdev += (dif * dif);
	}
	StatD.fStdev = float(sqrt(StatD.fStdev / float(sz)));
}

void CIM::AverStdev(UINT sz, int* pD, float& Ave, float& Stdev) {
	Ave = 0; Stdev = 0;
	float sum = 0;
	int* p = pD;
	for (UINT i = 0; i < sz; i++) {
		sum += *(p++);
	}
	Ave = sum / float(sz);
	p = pD;
	for (UINT i = 0; i < sz; i++) {
		float dif = *(p++) - Ave;
		Stdev += (dif * dif);
	}
	Stdev = float(sqrt(Stdev / float(sz)));
}

void CIM::EliminateFlyers(std::vector<float>& width) {
	UINT sz = width.size(); if (sz < 1) return;
	// FILE* fp = fopen("C:\\TEMP\\edge.before.csv", "wb");
	// if (fp) {
	// 	for (UINT i = 0; i < sz; i++) {
	// 		fprintf(fp, "%.4f\n", edge[i].width);
	// 	}
	// 	fclose(fp);
	// }
	//float ucl = StatD.fAver + 2.7f * StatD.fStdev;
	float lcl = StatD.fAve - StatD.fStdev;
	if (StatD.fStdev > 0.012) lcl = StatD.fAve - 0.012f;
	for (UINT i = 0; i < sz; i++) {
		if (width[i] < 0.0f) continue;
		//if (p->width > ucl) p->width = -1;
		if (width[i] < lcl) width[i] = -1;
	}
	// fp = fopen("C:\\TEMP\\edge.after.csv", "wb");
	// if (fp) {
	// 	for (UINT i = 0; i < sz; i++) {
	// 		fprintf(fp, "%.4f\n", edge[i].width);
	// 	}
	// 	fclose(fp);
	// }
}

void CIM::CalcStat() {
	DoStat(edge.width);
	EliminateFlyers(edge.width);
	DoStat(edge.width);
}

bool CIM::CalcHisto() {
	memset(Histo, 0, MAXBINS * sizeof(int));
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		short v;
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		for (int x = 0; x < wd; x++) {
			switch (bpp) {
			case 8: Histo[*(des++)]++; break;
			case 15:
			case 16:
				break;
			case 24:
				v = (*(des++) + *(des++) + *(des++)) / 3;
				Histo[v]++;
				break;
			case 32:
				v = (*(des++) + *(des++) + *(des++)) / 3; des++;
				Histo[v]++;
				break;
			}
		}
	}
	return true;
}

int CIM::HistoMedian() {
	long sum = 0;
	for (int i = 0; i < MAXBINS; i++) {
		sum += Histo[i];
	}
	long median = sum / 2;
	sum = 0;
	for (int i = 0; i < MAXBINS; i++) {
		sum += Histo[i];
		if (sum > median) return i;
	}
	return 0;
}

short CIM::HistoCG() {
	long sum = 0;
	for (short i = 0; i < 256; i++) {
		sum += Histo[i];
	}
	long lmt = long(sum * 0.30f);
	sum = 0;
	for (short i = 0; i < 256; i++) {
		sum += Histo[i];
		if (sum > lmt) return i;
	}
	return 128;
}

void CIM::HistoMaxMin(int& Max, int& Min) {
	if (IsNull()) return;
	CalcHisto(); // Do not delete this call [2/3/2020 yuenl]
	int* pH = Histo;
	// Forward search [6/15/2020 yuenl]
	for (int i = 0; i < 256; i++) {
		if (*pH) { // pH = 0 is not valid [6/15/2020 yuenl]
			Min = i; break;
		}
		else pH++;
	}
	pH = Histo + 255;
	for (int i = 255; i >= 0; i--) {
		if (*pH) { // pH = 0 is not valid [6/15/2020 yuenl]
			Max = i; break;
		}
		else pH--;
	}
}

void CIM::HistoMaxMin(int& Max, int& Min, float lmtpcl, float lmtpch) {
	if (IsNull()) return;
	CalcHisto(); // Do not delete this call [2/3/2020 yuenl]
	Max = 0; Min = 0;
	int total = 0;
	for (int i = 0; i < 256; i++) {
		total += Histo[i];
	}
	int lmt = int(total * lmtpcl);
	int sum = 0;
	for (int i = 1; i < 256; i++) {// look for first non zero bin [11/6/2019 FSM]
		sum += Histo[i];
		if (sum > lmt) { Min = i; break; } // bias toward black [2/3/2020 yuenl]
	}
	lmt = int(total * lmtpch);
	sum = 0;
	for (int i = 255; i >= 0; i--) {
		sum += Histo[i];
		if (sum > lmt) { Max = i; break; }
	}
}

//////////////////////////////////////////////////////////////////////////

void CIM::ConvlLowPass() {
	Convl(0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f);
}

bool CIM::Smooth() {
	if (IsNull()) return false;
	ConvlLowPass();
	return true;
}

bool CIM::BThreshold(BYTE Thres) {
	//BThres(short(HistoMedian() * 0.45f));
	//BThres(short(MinValue() * 2.5));
	//BThres(short(HistoCG()));
	BThres(Thres);
	return true;
}

void CIM::DumpEdge(BYTE* pSrc, BYTE* pDes, short wd, short bpp, char* filename) {
	FILE* fp = fopen(filename, "wb"); if (!fp) return;
	for (int i = 0; i < wd; i++) {
		fprintf(fp, "%d,%d\n", *(pSrc++) ? 1 : 0, *(pDes++) ? 1 : 0);
	}
	fclose(fp);
}

bool CIM::Compare(CIM& Img) {
	if (IsNull()) return false;
	if (Img.IsNull()) return false;

	int pitch = abs(GetPitch());
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);

	// #pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		BYTE* src = (BYTE*)Img.GetPixelAddress(0, y);
		switch (bpp) {
		case 8:
			for (int x = 0; x < wd; x++) {
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
			}
			break;
		case 15:
		case 16:
			break;
		case 24:
			for (int x = 0; x < wd; x++) {
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
			}
			break;
		case 32:
			for (int x = 0; x < wd; x++) {
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
				*des = (*des == *src) ? 0 : 255;
				src++; des++;
				*des = (*des == *src) ? 0 : 255;
				src += 2; des += 2;
			}
			break;
		}
	}
	return true;
}

bool CIM::RemoveSpeckle(short wdw) {	// dilation, black background [11/4/2019 FSM]
	if (IsNull()) return false;

	const short u = wdw / 2;
	int pitch = abs(GetPitch());
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);

	// erode [11/4/2019 FSM]
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		BYTE* pNL = new BYTE[pitch]; if (!pNL) break;
		short l, r;
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		memcpy(pNL, des, pitch);
		BYTE* pN = pNL;
		des += u * GetBYPP();
		switch (bpp) {
		case 8:
			for (int x = u; x < wd - u; x++) {
				BYTE v = BYTE(CountWhite(pN, wdw, bpp, l, r));
				*des = v ? 255 : 0;
				pN++; des++;
			}
			//Dump(pNL, (BYTE*)GetPixelAddress(0, y), wd, bpp, "C:\\TEMP\\NL.CSV");
			break;
		case 15:
		case 16:
			break;
		case 24:
			for (int x = u; x < wd - u; x++) {
				BYTE v = BYTE(CountWhite(pN, wdw, bpp, l, r));
				v = v ? 255 : 0;
				*(des) = v;
				*(des + 1) = v;
				*(des + 2) = v;
				pN += 3; des += 3;
			}
			//Dump(pNL, (BYTE*)GetPixelAddress(0, y), wd, bpp, "C:\\TEMP\\NL.CSV");
			break;
		case 32:
			for (int x = u; x < wd - u; x++) {
				BYTE v = BYTE(CountWhite(pN, wdw, bpp, l, r));
				v = v ? 255 : 0;
				*(des) = v;
				*(des + 1) = v;
				*(des + 2) = v;
				pN += 4; des += 4;
			}
			//Dump(pNL, (BYTE*)GetPixelAddress(0, y), wd, bpp, "C:\\TEMP\\NL.CSV");
			break;
		}
		delete[] pNL;
	}
	return true;
}

bool CIM::RemoveSpeckle2(short wdw) {
	if (IsNull()) return false;

	const short u = wdw / 2;
	int pitch = abs(GetPitch());
	int wd, ht, bpp;
	GetDim(wd, ht, bpp);
#if 1
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		BYTE* pNL = new BYTE[pitch]; if (!pNL) break;
		short l, r;
		BYTE* des = (BYTE*)GetPixelAddress(0, y);
		memcpy(pNL, des, pitch);
		BYTE* pN = pNL;
		des += u * GetBYPP();
		switch (bpp) {
		case 8:
			for (int x = u; x < wd - u; x++) {
				BYTE v = BYTE(CountBlack(pN, wdw, bpp, l, r));
				*des = v ? 0 : 255;
				pN++; des++;
			}
			break;
		case 15:
		case 16:
			break;
		case 24:
			for (int x = u; x < wd - u; x++) {
				BYTE v = BYTE(CountBlack(pN, wdw, bpp, l, r));
				v = v ? 0 : 255;
				*(des) = v;
				*(des + 1) = v;
				*(des + 2) = v;
				pN += 3; des += 3;
			}
			break;
		case 32:
			for (int x = u; x < wd - u; x++) {
				BYTE v = BYTE(CountBlack(pN, wdw, bpp, l, r));
				v = v ? 0 : 255;
				*(des) = v;
				*(des + 1) = v;
				*(des + 2) = v;
				pN += 4; des += 4;
			}
			break;
		}
		delete[] pNL;
	}
#endif
	return true;
}

bool CIM::SavePic() {
	if (IsNull()) return false;
	CFileDialog dlg(
		FALSE, _T("BMP"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		_T("BMP|*.BMP|PNG file|*.PNG|JPG|*.JPG|All|*.*|"));
	CString fname = GetProgramDir(); fname += "IMG"; int a = _mkdir(fname);
	dlg.m_ofn.lpstrInitialDir = fname.GetBuffer();
	dlg.m_ofn.lpstrTitle = _T("Save image");
	if (dlg.DoModal() == IDOK) {
		Save(dlg.GetPathName()); return true;
	}
	return false;
}

bool CIM::SavePicture(float r, float a) {
	if (IsNull()) return false;
	CString fname = GetProgramDir(); fname += "IMG"; _mkdir(fname.GetBuffer(0));
	CString str; str.Format("%s\\%.3f_%.3f.BMP", fname.GetBuffer(0), a, r);
	Save(str);
	return true;
}

bool CIM::SaveEdges() {
	if (IsNull()) return false;
	CFileDialog dlg(
		FALSE, _T("CSV"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		_T("CSV|*.CSV|"));
	CString fname = GetProgramDir(); fname += "IMG"; int a = _mkdir(fname);
	dlg.m_ofn.lpstrInitialDir = fname.GetBuffer();
	dlg.m_ofn.lpstrTitle = _T("Save edges");
	if (dlg.DoModal() == IDOK) {
		SaveEdges(dlg.GetPathName()); return true;
	}
	return false;
}

bool CIM::SaveEdges(CString name) {
	FILE* fp = fopen(name, "wb"); if (!fp) return false;
	UINT sz = edge.width.size(); if (!sz) { fclose(fp); return false; }
	edge.DumpConst(fp);
	StatD.Dump(fp);
	edge.DumpHeader(fp);
	edge.Dump(fp);
	fclose(fp);
	return true;
}

bool CIM::SaveLEdges() {
	if (IsNull()) return false;
	CFileDialog dlg(
		FALSE, _T("CSV"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		_T("CSV|*.CSV|"));
	CString fname = GetProgramDir(); fname += "IMG"; int a = _mkdir(fname);
	dlg.m_ofn.lpstrInitialDir = fname.GetBuffer();
	dlg.m_ofn.lpstrTitle = _T("Save edges");
	if (dlg.DoModal() == IDOK) {
		SaveLEdges(dlg.GetPathName()); return true;
	}
	return false;
}

bool CIM::SaveLEdges(CString name) {
	FILE* fp = fopen(name, "wb"); if (!fp) return false;
	UINT sz = edge.Inner.size(); if (!sz) { fclose(fp); return false; }

	fprintf(fp, "X,Y\n");
	for (UINT i = 0; i < sz; i++) {
		SLoc* p = &edge.Inner[i];
		fprintf(fp, "%d,%d\n", p->x, p->y);
	}
	fclose(fp);
	return true;
}

bool CIM::LoadPic(float& Ang) {
	CFileDialog dlg(
		TRUE, _T("BMP"), NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("BMP|*.BMP|PNG file|*.PNG|JPG|*.JPG|All|*.*|"));
	CString fname = GetProgramDir(); fname += "IMG"; int a = _mkdir(fname);
	dlg.m_ofn.lpstrInitialDir = fname.GetBuffer();
	dlg.m_ofn.lpstrTitle = _T("Load image");
	if (dlg.DoModal() == IDOK) {
		if (!IsNull()) Destroy();
		CString fname(dlg.GetPathName());
		Load(fname);
		filename = fname.Mid(fname.ReverseFind('\\') + 1);
		CalcHisto();
		CString str; str = fname.Mid(fname.ReverseFind('\\') + 1);
		str = str.Mid(0, str.Find('_'));
		//str = str.Mid(str.Find('.') + 1);
		Ang = float(atof(str));
		return true;
	}
	return false;
}

bool CIM::LoadPic(CString name, float& Ang) {
	if (!IsNull()) Destroy();
	Load(name); if (IsNull()) return false;
	CalcHisto();
	CString str; str = name.Mid(name.ReverseFind('\\') + 1);
	str = str.Mid(0, str.ReverseFind('.'));
	str = str.Mid(str.Find('.') + 1);
	Ang = float(atof(str));
	return true;
}

bool CIM::ExportHisto(char* filename) {
	CalcHisto();// Do not delete this call [2/3/2020 yuenl]
	FILE* fp = fopen(filename, "wb"); if (!fp) return false;
	for (int i = 0; i < MAXBINS; i++) {
		fprintf(fp, "%d,%d\n", i + 1, Histo[i]);
	}
	fclose(fp);
	return true;
}

CString CIM::GetProgramDir() {
	char dirname[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	GetModuleFileName(NULL, dirname, _MAX_PATH);
	_splitpath(dirname, drive, dir, fname, ext);
	_makepath(dirname, drive, dir, "", "");
	return dirname;
}

void CIM::BestRect(CRect& rc) {
	int nwd, nht;
	float aspImg = this->GetWidth() / float(this->GetHeight());
	float aspScr = rc.Width() / float(rc.Height());
	if (aspImg > aspScr) {
		nwd = rc.Width(); nht = int(nwd / aspImg);
	}
	else {
		nht = rc.Height(); nwd = int(nht * aspImg);
	}
	int x = (rc.Width() - nwd) / 2;
	int y = (rc.Height() - nht) / 2;
	rc.SetRect(x, y, x + nwd, y + nht);  // shift y up to 0, ie y is always start from 0 [3/9/2015 Yuen]
}

CIM::CIM() {
}

CIM::~CIM() {
}

void CIM::CopyColorMap(CIM& Image) {
	int n = Image.GetMaxColorTableEntries();
	if (n) {
		if (n > 256) n = 256;
		RGBQUAD rgbColors[256] = { 0 };
		Image.GetColorTable(0, n, rgbColors);
		SetColorTable(0, n, rgbColors);
	}
}

CIM& CIM::operator/(CIM& Image) {// assume image is monochrome [1/7/2020 yuenl]
	if (Image.IsNull()) return Image;
	if (IsNull()) return Image;
	if (this == &Image) return Image;
	if (!IsSameSize(Image)) return Image;
	CopyColorMap(Image);

	// require a higher bpp to operate [1/7/2020 yuenl]
	int wd, ht, bpp;
	GetDim(wd, ht, bpp); if (bpp != 24) { ASSERT(0); return Image; }
	float* pfImg = new float[wd * ht];
	PBYTE pSrc, pDes;
	float* pf, v, max = 0, min = FLT_MAX;
	for (int y = 0; y < ht; y++) {
		pSrc = (PBYTE)Image.GetPixelAddress(0, y);
		pDes = (PBYTE)GetPixelAddress(0, y);
		pf = pfImg + y * wd;
		for (int x = 0; x < wd; x++) {
			if (*pSrc == 0) *pSrc = 1;
			v = *pDes / float(*pSrc);
			if (v > max) max = v; if (v < min) min = v;
			*(pf++) = v;
			pDes += 3; pSrc += 3;
		}
	}
	// normalize pfImg [1/7/2020 yuenl]
	float sf = 255 / (max - min);
	for (int y = 0; y < ht; y++) {
		pDes = (PBYTE)GetPixelAddress(0, y);
		pf = pfImg + y * wd;
		for (int x = 0; x < wd; x++) {
			v = (*(pf++) - min) * sf;
			*(pDes++) = BYTE(v); *(pDes++) = BYTE(v); *(pDes++) = BYTE(v);
		}
	}
	edge = Image.edge;
	if (pfImg) delete[]pfImg;

	return *this;
}

CIM& CIM::operator=(CIM& Image) {
	if (Image.IsNull()) {
		if (!IsNull()) Destroy(); return *this;
	}
	if (this == &Image) return *this;
	if (IsNull()) {
		Create(Image.GetWidth(), Image.GetHeight(), Image.GetBPP());
	}
	else {
		if (!IsSameSize(Image)) {
			Destroy();
			Create(Image.GetWidth(), Image.GetHeight(), Image.GetBPP());
		}
	}
	if (IsNull()) return *this;

	int n = Image.GetMaxColorTableEntries();
	if (n) {
		if (n > 256) n = 256;
		RGBQUAD rgbColors[256] = { 0 };
		Image.GetColorTable(0, n, rgbColors);
		SetColorTable(0, n, rgbColors);
	}
	memcpy(GetBufferStart(), Image.GetBufferStart(), Image.GetBufferSize());

	edge = Image.edge;
	ang = Image.ang;
	pos = Image.pos;
	filename = Image.filename;
	distIn = Image.distIn;
	distOut = Image.distOut;

	return *this;
}

// void SStat::Clear()
// {
// 	aver = 0; stdev = 0;
// 	max = 0; min = 0;
// }

// void SStat::Dump(FILE* fp)
// {
// 	if (!fp) return;
// 	fprintf(fp, "Statistics:\n");
// 	fprintf(fp, "Average,%.3f\n", aver);
// 	fprintf(fp, "Std.Dev,%.4f\n", stdev);
// 	fprintf(fp, "TTV,%.3f\n", max - min);
// 	fprintf(fp, "Maximum,%.3f\n", max);
// 	fprintf(fp, "Minimum,%.3f\n\n", min);
// }

/*
// C++ program to generate Gaussian filter
#include <cmath>
#include <iomanip>
#include <iostream>
using namespace std;

// Function to create Gaussian filter
void FilterCreation(double GKernel[][5])
{
	// initializing standard deviation to 1.0
	double sigma = 1.0;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization
	double sum = 0.0;

	// generating 5x5 kernel
	for (int x = -2; x <= 2; x++) {
		for (int y = -2; y <= 2; y++) {
			r = sqrt(x * x + y * y);
			GKernel[x + 2][y + 2] = (exp(-(r * r) / s)) / (M_PI * s);
			sum += GKernel[x + 2][y + 2];
		}
	}

	// normalizing the Kernel
	for (int i = 0; i < 5; ++i)
		for (int j = 0; j < 5; ++j)
			GKernel[i][j] /= sum;
}

// Driver program to test above function
int main()
{
	double GKernel[5][5];
	FilterCreation(GKernel);

	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j)
			cout << GKernel[i][j] << "\t";
		cout << endl;
	}
}
*/

void SEdge::Clear() {
	Inner.clear(); Outer.clear(); width.clear();
}

BOOL SEdge::IsValid(int i) {
	if ((Inner[i].x != -1) && (Inner[i].y != -1) &&
		(Outer[i].x != -1) && (Outer[i].y != -1)) return TRUE;
	return FALSE;
}

void SEdge::DumpConst(FILE* fp) {
	fprintf(fp, "Distance:,%.3f\n", AveWidth());
	fprintf(fp, "Px:,%.3f\n", px);
	fprintf(fp, "Py:,%.3f\n\n", py);
}

void SEdge::DumpHeader(FILE* fp) {
	fprintf(fp, "Length, Inner X,Inner Y,Outer X,Outer Y\n");
}

void SEdge::Dump(FILE* fp) {
	UINT sz = width.size();
	for (UINT i = 0; i < sz; i++) {
		fprintf(fp, "%.4f,%s,%s\n", width[i], Inner[i].str().GetBuffer(), Outer[i].str().GetBuffer());
	}
}

float SEdge::AveWidth() {
	float sum = 0;
	UINT sz = width.size(); if (!sz) return 0;
	for (UINT i = 0; i < sz; i++) {
		sum += width[i];
	}
	return sum / float(sz);
}

// location of edge on image [1/7/2020 yuenl]

CString SLoc::str() {
	CString s; s.Format("%d,%d", x, y); return s;
}

//////////////////////////////////////////////////////////////////////////
// For CPackedDIB [08202024 ZHIMING]

void CIM::Attach(CPackedDIB* pDIB) {
	if (!IsNull()) {
		Destroy();
	}
	Create(pDIB->GetWidth(), pDIB->GetHeight(), pDIB->BytesPerPixel * 8);
#pragma omp parallel for
	for (int y = 0; y < GetHeight(); y++) {
		PBYTE p = (PBYTE)GetPixelAddress(0, y);
		PBYTE q = (PBYTE)pDIB->GetPixelAddress(0, y);
		memcpy(p, q, pDIB->BytesPerRow);
	}
}