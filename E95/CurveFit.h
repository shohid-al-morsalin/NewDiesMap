#pragma once

class CCurveFit {
public:
	int Fit(double* XData, double* YData, double* Res, unsigned nData, unsigned nOrder);
	double CalcFunc(int Order, double* pParam, double XVal);
	CCurveFit();
	virtual ~CCurveFit();
};