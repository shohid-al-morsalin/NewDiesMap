// Spline.h: interface for the CSpline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPLINE_H__DE313BC1_2909_11D4_ADA7_C343E4B6F33F__INCLUDED_)
#define AFX_SPLINE_H__DE313BC1_2909_11D4_ADA7_C343E4B6F33F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSpline  
{
public:
	int GetCount();
	float GetLast();
	float GetFirst();
	float Get(float x);
	int na;
	float * ya;
	float * xa;
	float * y2;
	void Generate(float *x, float *y, int n, float yp1, float ypn);
	CSpline();
	virtual ~CSpline();

};

#endif // !defined(AFX_SPLINE_H__DE313BC1_2909_11D4_ADA7_C343E4B6F33F__INCLUDED_)
