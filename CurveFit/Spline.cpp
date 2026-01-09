// Spline.cpp: implementation of the CSpline class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3dWafer.h"
#include "Spline.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpline::CSpline()
{
	y2 = NULL;
	xa = NULL;
	ya = NULL;
}

CSpline::~CSpline()
{
	if (y2) delete [] y2;
	if (xa) delete [] xa;
	if (ya) delete [] ya;
}

void CSpline::Generate(float *x, float *y, int n, float yp1, float ypn)
{
	int i,k;
	float p,qn,sig,un;
	float *u = NULL;

	na = n;
	if (!xa) xa = new float [na];
	float *pp = xa;
	for (i = 0; i < na; i++) *(pp++) = *(x++);
	if (!ya) ya = new float [na];
	pp = ya;
	for (i = 0; i < na; i++) *(pp++) = *(y++);

	if (!y2) y2 = new float [na];

	u = new float[na];
	if (yp1 > 0.99e30) {
		y2[0] = u[0] = 0.0f;
	}
	else {
		y2[0] = -0.5;
		u[0] = (3.0f/(xa[1]-xa[0]))*((ya[1]-ya[0])/(xa[1]-xa[0])-yp1);
	}

	for (i = 1; i < n-1; i++) {
		sig=(xa[i]-xa[i-1])/(xa[i+1]-xa[i-1]);
		p=sig*y2[i-1]+2.0f;
		y2[i] = (sig-1.0f)/p;
		u[i]=(ya[i+1]-ya[i])/(xa[i+1]-xa[i]) - (ya[i]-ya[i-1])/(xa[i]-xa[i-1]);
		u[i]=(6.0f*u[i]/(xa[i+1]-xa[i-1])-sig*u[i-1])/p;
	}

	if (ypn > 0.99e30) {
		qn=un=0.0f;
	}
	else {
		qn = 0.5;
		un=(3.0f/(xa[na-1]-xa[na-2]))*(ypn-(ya[na-1]-ya[na-2])/(xa[na-1]-xa[na-2]));
	}
	y2[na-1] = (un-qn*u[na-2])/(qn*y2[na-2]+1.0f);
	for (k=n-2; k>=1; k--) {
		y2[k]=y2[k]*y2[k+1]+u[k];
	}
	if (u) delete [] u;
}

float CSpline::Get(float x)
{
	if (!y2) return 0;

	int klo, khi, k;
	float h,b,a;

	klo=0;
	khi=na-1;
	while(khi-klo > 1) {
		k = (khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if (h == 0.0f) AfxMessageBox("Bad xa input");
	a=(xa[khi]-x)/h;
	b=(x-xa[klo])/h;
	return a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2[klo]+(b*b*b-b)*y2[khi])*(h*h)/6.0f;
}

float CSpline::GetFirst()
{
	return xa[0];
}

float CSpline::GetLast()
{
	return xa[na-1];
}

int CSpline::GetCount()
{
	return na;
}
