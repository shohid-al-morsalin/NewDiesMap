#include "stdafx.h"
#include <math.h>
#include "CurveFit.h"
#include "CurveFit/Interface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurveFit::CCurveFit() {}

CCurveFit::~CCurveFit() {}

double CCurveFit::CalcFunc(int Order, double* pParam, double XVal) {
	double RetVal = 0;
	for (int i = 0; i < Order; i++) {
		RetVal += pParam[i] * ::pow(XVal, i);
	}
	return RetVal;
}

int CCurveFit::Fit(double* XData, double* YData, double* Res, unsigned int nData, unsigned int nOrder) {
	return CurveFit(XData, YData, Res, nData, nOrder);
}