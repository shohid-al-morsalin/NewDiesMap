// EchoPeak.cpp: implementation of the CEchoPeak class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "EchoPeak.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const int PEAK_REGION = (int)(0.06 * 10000000);

CEchoPeak::CEchoPeak() {
	Reset();
}

CEchoPeak::~CEchoPeak() {}

void CEchoPeak::SetPosition(int nData) {
	fXMaxima = float(nData);
}

void CEchoPeak::SetIndex(int nData) {
	iXMaxima = nData;
}

void CEchoPeak::SetHeight(int nData) {
	YMaxima = nData;
}

void CEchoPeak::SetCM(int index, float fData) {
	iCMs = index;
	fCMs = fData;
}

void CEchoPeak::SetMid(double fData) {
	Mid = fData;
}

float CEchoPeak::GetPosition() {
	return fXMaxima;
}

int CEchoPeak::GetHeight() {
	return YMaxima;
}

int CEchoPeak::GetIndex() {
	return iXMaxima;
}

float CEchoPeak::fGetCM() {
	return fCMs;
}

float CEchoPeak::GetMid() {
	return Mid;
}

void CEchoPeak::Reset() {
	fXMaxima = 0;
	iXMaxima = 0;
	YMaxima = 0;
	fCMs = 0;
	iCMs = 0;
	Mid = 0;
	iLL = iRL = 0;
}

float CEchoPeak::GetShapeFactor() {
	return (Mid - fCMs) * 1e-4;
	return (fXMaxima - fCMs) * 1e-4;  // Implementation 2
}

void CEchoPeak::Dump(FILE* fp) {
	if (!fp) {
		return;
	}
	fprintf(fp, "CM=%.4f, Mid=%.4f, XMaxima=%.4f, YMaxima=%d, Index=%d\r\n", fCMs * 1e-4, Mid * 1e-4, float(fXMaxima * 1e-4), YMaxima, iXMaxima);
}