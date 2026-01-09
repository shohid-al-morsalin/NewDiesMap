#include "stdafx.h"
#include "WMap.h"

CWMap::CWMap() {
}

CWMap::~CWMap() {
	if (scale) delete[] scale;
	if (buff) delete[] buff;
}

BOOL CWMap::Create(int w, int h) {
	if (buff) delete[] buff;
	buff = new float[w * h]; if (!buff) return FALSE;
	wd = w; ht = h;
	ZeroMemory(buff, wd * ht * sizeof(float));
	return TRUE;
}

BOOL CWMap::Expand() {
	if (!buff) return FALSE;

	int sz = 2 * (wd - 1) + 1;
	sz *= sz;
	float* buf = new float[sz]; if (!buf) return FALSE;
	ZeroMemory(buf, (sz - 1) * sizeof(float));
	for (int i = 0; i < ht; i++) {
		float* p1 = buf + 2 * i * (2 * wd - 1);
		float* p2 = buff + i * wd;
		int j; for (j = 0; j < wd; j++) {
			*(p1 + 2 * j) = *(p2 + j);
		}
	}
	delete[] buff;
	buff = buf; // swap pointer [2/10/2020 yuenl]
	wd = 2 * (wd - 1) + 1;
	ht = 2 * (ht - 1) + 1;
	return TRUE;
}

// Note: x & y are zero base
BOOL CWMap::Set(int x, int y, float v) {
	if (!buff) return FALSE;
	if ((x >= wd) || (y >= ht)) return FALSE;
	*(buff + y * wd + x) = v;
	if (v < min) min = v;   // @@Vijay02Aug01
	return TRUE;
}

// Note: x & y are zero base
float CWMap::Get(int x, int y) {
	if (!buff) return 0;
	if ((x >= wd) || (y >= ht)) return 0;
	return *(buff + y * wd + x);
}

BOOL CWMap::Fill() {
	int i, d;
	i = 0;
	for (int j = 1; j < wd - 1; j += 2) {
		d = i * wd + j;
		*(buff + d) = (*(buff + d - 1) + *(buff + d + 1)) / 2.0f;
	}
	i = ht - 1;
	for (int j = 1; j < wd - 1; j += 2) {
		d = i * wd + j;
		*(buff + d) = (*(buff + d - 1) + *(buff + d + 1)) / 2.0f;
	}

	// Vertical side
	for (i = 1; i < ht - 1; i += 2) {
		d = i * wd;
		*(buff + d) = (*(buff + d - wd) + *(buff + d + wd)) / 2.0f;
		d = i * wd + wd - 1;
		*(buff + d) = (*(buff + d - wd) + *(buff + d + wd)) / 2.0f;
	}

	// Fill center
	for (i = 1; i < ht - 1; i += 2) {
		for (int j = 1; j < wd - 1; j += 2) {
			float* p = buff + i * wd + j;
			*(p) = (*(p + wd + 1) + *(p + wd - 1) + *(p - wd + 1) + *(p - wd - 1)) / 4.0f;
		}
	}
	// Fill center
	for (i = 1; i < ht - 1; i += 2) {
		for (int j = 2; j < wd - 1; j += 2) {
			float* p = buff + i * wd + j;
			*(p) = (*(p + wd) + *(p - wd) + *(p + 1) * 0.6f + *(p - 1) * 0.6f) / 3.2f;
		}
	}

	for (i = 2; i < ht - 1; i += 2) {
		for (int j = 1; j < wd - 1; j += 2) {
			float* p = buff + i * wd + j;
			*(p) = (*(p + wd) + *(p - wd) + *(p + 1) * 0.6f + *(p - 1) * 0.6f) / 3.2f;
		}
	}
	return TRUE;
}

BOOL CWMap::SetScale(float* sc, short lv) {
	if (scale) delete[] scale;
	scale = new float[lv];
	if (!scale) return FALSE;
	level = lv;
	float* p = scale;
	int i; for (i = 0; i < level; i++) {
		*(p++) = *(sc++);
	}
	return TRUE;
}

// Note: return 0 implies value is below min
//		 return level imples value is above max
//		 return normalized value
float CWMap::GetQN(int x, int y) {
	float v = Get(x, y);
	int i;
	for (i = 0; i < level; i++) {
		if (v >= *(scale + i)) {
			if (v <= *(scale + i + 1)) return i / float(level);
		}
	}
	return i / float(level);
}

BOOL CWMap::AutoLevel(short lv) {
	if (!buff) return FALSE;
	MaxMin();
	if (scale) delete[] scale;
	scale = new float[lv + 1];
	if (!scale) return FALSE;
	level = lv;
	float sf = (max - min) / float(level);
	int i; for (i = 0; i <= level; i++) {
		*(scale + i) = min + i * sf;
	}
	return TRUE;
}

void CWMap::MaxMin() {
	if (!buff) return;

	float* p = buff;

	max = min = *p;
	int i; for (i = 0; i < wd * ht; i++) {
		if (max < *p) max = *p;
		if (min > *p) min = *p;
		p++;
	}
}

BOOL CWMap::BackFill(float con) {
	if (!buff) return FALSE;

	float* p = buff;
	int i; for (i = 0; i < ht; i++) {
		int j; for (j = 0; j < wd; j++) {
			if (*p == 0) *p = con;
			p++;
		}
	}
	return 0;
}

CString CWMap::GetLevelStr(short lv) {
	if (lv > level) return "";
	CString str;
	str.Format("%.1f", *(scale + lv));
	return str;
}

short CWMap::GetIndex(short l) {
	return short(l * 25 / level);
}