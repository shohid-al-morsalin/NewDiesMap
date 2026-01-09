// RGB3.cpp: implementation of the CRGB3 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RGB3.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BYTE SBGR::I() {
	return (R + 2 * G + B) / 4;
}

void SBGR::Grey(BYTE v) {
	//v = (v<256)?v:255;
	//v = (v<0)? 0:v;
	R = G = B = v;
}

void SBGR::Set(BYTE r, BYTE g, BYTE b) {
	R = r;
	G = g;
	B = b;
}

void SBGR::Dup(SBGR& rgb1) {
	R = rgb1.R;
	G = rgb1.G;
	B = rgb1.B;
}

void SBGR::Ave(SBGR* rgb1, short BG) {
	short v;
	float fac = 1.0f;
	v = (rgb1->R - BG) * fac;
	if (v < 0) {
		v = 0;
	}
	if (v > 255) {
		v = 255;
	}
	R = (R + v) / 2;
	v = (rgb1->G - BG) * fac;
	if (v < 0) {
		v = 0;
	}
	if (v > 255) {
		v = 255;
	}
	G = (G + v) / 2;
	v = (rgb1->B - BG) * fac;
	if (v < 0) {
		v = 0;
	}
	if (v > 255) {
		v = 255;
	}
	B = (B + v) / 2;
}

void SBGR::Add(int* histoR, int* histoG, int* histoB) {
	histoR[R]++;
	histoG[G]++;
	histoB[B]++;
}

void SBGR::HistoEqualize(int* histoR, int* histoG, int* histoB) {
	R = histoR[R];
	G = histoG[G];
	B = histoB[B];
}

BYTE SBGR::MaxRGB() {
	if (R >= B) {
		if (R >= G) {
			return R;
		}
	}
	else {
		if (B > G) {
			return B;
		}
	}
	return G;
}

BYTE SBGR::MinRGB() {
	if (R <= B) {
		if (R <= G) {
			return R;
		}
	}
	else {
		if (B <= G) {
			return B;
		}
	}
	return G;
}

float SLine::Score(short idx, std::vector<SPont>& L) {
	int sz = int(L.size());
	float sum = 0;
	for (int i = 0; i < sz; i++) {
		sum += L[i].v.I();
	}
	//Ln[idx].s = sum / float(sz);
	return sum / float(sz);
}