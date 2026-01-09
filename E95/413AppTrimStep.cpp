#include "stdafx.h"

#include "413AppTrimStep.h"

C413AppTrimStep::C413AppTrimStep() {}

C413AppTrimStep::~C413AppTrimStep() {}

void C413AppTrimStep::Integrate(short* pBuf, short len, short wdw) {
	short* pNew = new short[len + 16];
	short* p = pNew;
	short* q = pBuf;

	int i;
	//short wdw = 7;
	for (i = 0; i < wdw; i++) {
		*(p++) = 0;
		q++;
	}
	for (i = wdw; i < len - wdw; i++) {
		*(p++) = (*(q - wdw) - *(q + wdw));
		q++;
	}
	for (int j = i; j < len; j++) {
		*(p++) = 0;
	}
	memcpy(pBuf, pNew, len * sizeof(short));
	delete[] pNew;
}

void C413AppTrimStep::ContrastStretch(short* pBuf, short len, float contrastLevel) {
	int i;
	short* p;
	short Max, Min;

	p = pBuf;
	Max = Min = *p;
	for (i = 0; i < len; i++) {
		if (*p > Max) {
			Max = *p;
		}
		if (*p < Min) {
			Min = *p;
		}
		p++;
	}
	// 20210316 ZHIMING changed 0.1 -> 0.0
	Min += contrastLevel * (Max - Min);	// threshold by clipping 10% off black [5/28/2012 Yuen]
	float sf = 255.0f / float(Max - Min);
	p = pBuf;
	for (i = 0; i < len; i++) {
		*p = sf * (*p - Min);
		if (*p < 0) {
			*p = 0;
		}
		p++;
	}
}

void C413AppTrimStep::Smooth(short* pBuf, short len) {
	short* pNew = new short[len + 16];
	short* p = pNew + 4;
	short* q = pBuf + 4;
	int i, j;
	for (i = 4; i < len - 4; i++) {
		*(p++) = (*(q - 3) + *(q - 2) + *(q - 1) + *(q)+*(q + 1) + *(q + 2) + *(q + 3)) / 7;
		q++;
	}
	short v = *(p - 1);
	for (j = i; j < len; j++) {
		*(p++) = v;
	}

	p = pNew;
	v = *(p + 4);
	for (j = 0; j < 4; j++) {
		*(p++) = v;
	}

	memcpy(pBuf, pNew, len * sizeof(short));

	delete[] pNew;
}

void C413AppTrimStep::SaveExtractedLine(short n, short* pBuf, short len, CString tm) {
	CString fname;
	if (n > -1) {
		fname.Format("C:\\TEMP\\EL%s-%d.CSV", tm, n);
	}
	else {
		fname.Format("C:\\TEMP\\EL%s.CSV", tm);
	}
	FILE* fp = fopen(fname, "wb");
	if (fp) {
		for (int i = 0; i < len; i++) {
			fprintf(fp, "%d\n", *(pBuf + i));
		}
		fclose(fp);
	}
}

void C413AppTrimStep::PreprocessTrimData(short** pBuf, short len, BOOL bI2, short nSmooth, BOOL thresh, CString tm, float contrastLevel) {
	int i;
	BOOL bSave = FALSE;
	//BOOL bSave = TRUE;

	if (tm.GetLength() > 0) { bSave = TRUE; }
	//bSave = TRUE;	// disable saving data [6/28/2012 user]
	if (bSave) { SaveExtractedLine(-1, *pBuf, len, tm + "-R"); }
	for (i = 0; i < nSmooth; i++) {
		Smooth(*pBuf, len);
	}
	if (bSave) { SaveExtractedLine(-1, *pBuf, len, tm + "-S"); }
	if (thresh) {
		ContrastStretch(*pBuf, len, contrastLevel);	//20210329
		if (bSave) { SaveExtractedLine(-1, *pBuf, len, tm + "-C"); }
	}
	//	ThresholdTrimData(pBuf,len,tm);
	//if (bSave) { SaveExtractedLine(-1,*pBuf,len,tm+"-T"); }
	Integrate(*pBuf, len, 2);
	if (bSave) { SaveExtractedLine(-1, *pBuf, len, tm + "-I1"); }
	if (bI2) {
		Integrate(*pBuf, len, 2);
		// 	if (thresh) ThresholdTrimData(*pBuf,len,6,"");	// For bevel measurement only [5/28/2012 Yuen]
		if (bSave) { SaveExtractedLine(-1, *pBuf, len, tm + "-I2"); }
	}
	// [04232021] ZHIMING elliminate noise of first transition
	for (i = 0; i < nSmooth; i++) {
		Smooth(*pBuf, len);
	}
	if (bSave) { SaveExtractedLine(-1, *pBuf, len, tm + "-I2S"); }
}

// left on return contains the best estimated edge [4/21/2013 Administrator]
float C413AppTrimStep::GetTrimWidthIn(short* pBuf, short len, float& ang, BOOL& bX, short& left, short mid) {
	int i;
	short* p;
	short n = 2;
	short Max, Min;

	p = pBuf;
	Max = Min = *pBuf;
	for (i = 0; i < len; i++) {
		if (*p < Min) {
			Min = *p;
		}
		if (*p > Max) {
			Max = *p;
		}
		p++;
	}
	Max = (Max - Min) / 25;

	BOOL bFound = FALSE;
	left = 0;
	p = pBuf + n;
	for (i = n; i < len; i++) {
		if ((*(p - n) < 0) && (*(p + n) > 0)) {
			if ((*(p + n) - *(p - n)) >= Max) {
				short* q = p - n;
				for (int j = -n; j < n; j++) {
					// looking for the first negative to positive transition [4/21/2013 Administrator]
					if ((*q < 0) && *(q + 1) >= 0) {
						left = i + j;
						*(p + j) = Min;
						bFound = TRUE;
						break;
					}
					q++;
				}
			}
		}
		if (bFound) {
			break;
		}
		p++;
	}

	short d = left - mid;
	short v = d;
	if (bX) {
		d /= cos(ang * 0.01745329);
	}
	else {
		d /= sin(ang * 0.01745329);
	}
	return abs(d) * sign(long(v));
}

// return relative distance from center of image in pixel [4/23/2013 Administrator]
float C413AppTrimStep::GetTrimWidthOut(short* pBuf, short len, float& ang, BOOL& bX, short& right, short mid) {
	int i;
	short* p;
	short n = 7;
	short Max, Min;

	p = pBuf;
	Max = Min = *p;
	for (i = 0; i < len; i++) {
		if (*p < Min) {
			Min = *p;
		}
		if (*p > Max) {
			Max = *p;
		}
		p++;
	}
	Max = (Max - Min) / 25;

	BOOL bFound = FALSE;
	right = 0;

	int dd = len - n - 2;
	if (dd < 0) {
		ASSERT(0);  // diagnostics only [2/18/2012 Administrator]
	}
	p = pBuf + dd;
	for (i = len - n - 1; i >= n + 1; i--) {
		// looking for negative to positive zero crossing [2/18/2012 Administrator]
		if ((*(p - n) < 0) && (*(p + n) >= 0)) {
			if ((*(p + n) - *(p - n)) >= Max) {
				short* q = p - n;
				for (int j = -n; j < n; j++) {
					if ((*q < 0) && *(q + 1) >= 0) {
						right = (i + j);
						*(p + j + 1) = Min;
						bFound = TRUE;
						break;
					}
				}
				if (bFound) {
					break;
				}
				//q++;
			}
		}
		p--;
	}

	short d = right - mid;
	short v = d;
	if (bX) {
		d /= cos(ang * 0.01745329);
	}
	else {
		d /= sin(ang * 0.01745329);
	}
	return abs(d) * sign(long(v));
}

float C413AppTrimStep::GetEdgeOffset(short nLines, short Spacing, float angle, BOOL bInner, CImgWnd* pImgWnd) {
	if (!pImgWnd) {
		ASSERT(0);
		return 0;
	}

	int i;
	BOOL bX;
	short left, right;
	float* pfTrimWidth = new float[2 * nLines + 1];
	CPackedDIB* pDIB = pImgWnd->pPackedDIB1;
	short BPP = pDIB->pDIB->BmpInfo.biBitCount / 8;

	int siz = 3 * pDIB->pDIB->BmpInfo.biWidth * BPP;
	short* pBuf = new short[siz];
	short cnt = 0;
	for (i = -nLines; i <= nLines; i++) {
		int idx1 = 0;
		short len = pImgWnd->ExtractLine(idx1, pBuf, siz, angle, i * Spacing, bX);
		if (len >= siz) { ASSERT(0); return 0; }
		PreprocessTrimData(&pBuf, len, TRUE, 6, TRUE, "");
		int j;
		short* p;
		if (bInner) {
			*(pfTrimWidth + (cnt++)) = GetTrimWidthIn(pBuf, len, angle, bX, left, idx1);
			if (len > 0) {
				p = pBuf;
				for (j = 0; j <= left; j++) {
					*(p++) = 255;
				}
				for (j = left + 1; j < len; j++) {
					*(p++) = 0;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128; *(pBuf + idx1) = 128; *(pBuf + idx1 + 1) = 128;
				}
				p = pBuf + 24;
				*(p++) = 128; *(p++) = 128; *(p++) = 128; *(p++) = 128; *(p++) = 128;
			}
		}
		else {
			*(pfTrimWidth + (cnt++)) = GetTrimWidthOut(pBuf, len, angle, bX, right, idx1);
			if (len > 0) {
				p = pBuf;
				for (j = 0; j < right; j++) {
					*(p++) = 0;
				}
				for (j = right; j < len; j++) {
					*(p++) = 255;
				}
				if ((idx1 < len - 2) && (idx1 > 1)) {
					*(pBuf + idx1 - 1) = 128; *(pBuf + idx1) = 128; *(pBuf + idx1 + 1) = 128;
				}
				p = pBuf + 24;
				*(p++) = 128; *(p++) = 128; *(p++) = 128; *(p++) = 128; *(p++) = 128;
			}
		}
		pImgWnd->ReplaceLine(pBuf, angle, i * Spacing, bX);
	}
	if (pBuf) { delete[] pBuf; }
	pImgWnd->Refresh();

	float x, y;
	C413Global* g = &Global;
	float dR = fRemoveFlyer(cnt, pfTrimWidth, 0, 1);
	g->Polar2Cartesian(dR, angle, x, y);
	float sfx = g->ScrnCalibECH.ScrnX / float(pImgWnd->pPackedDIB1->IWidth);
	x = x * sfx / g->ScrnCalibECH.ppmX;
	float sfy = g->ScrnCalibECH.ScrnY / float(pImgWnd->pPackedDIB1->IHeight);
	y = y * sfy / g->ScrnCalibECH.ppmY;
	dR = sqrt(x * x + y * y) * 1e-3 * sign(dR);

	if (pfTrimWidth) { delete[] pfTrimWidth; }
	return dR;	// return value in mm [6/13/2014 Yuen]
}