// FFTData.cpp: implementation of the CFFTData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "e95.h"
#include "FFTData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFFTData::CFFTData()
{
	pPt=NULL;
	nPoint=0;
	jPmax = jPlow = jPhi = 0;
	nlmin = nlmax = 0;
}

CFFTData::~CFFTData()
{
	Clear();
}

void CFFTData::ShrinkY2(int n)
{
	int i;
	CFFTPoint * p1 = pPt;
	CFFTPoint * p2 = pPt+1;

	n--;
	for (i=0; i<n; i++)
	{
		if (p1->y2 < p2->y2)
		{
			p1->y2=0;
		}
		p1++; p2++;
	}
	p1 = pPt+n;
	p2 = pPt+n-1;
	for (i=0; i<n; i++)
	{
		if (p1->y2 < p2->y2)
		{
			p1->y2=0;
		}
		p1--; p2--;
	}

	(pPt+n)->y2 = 0; // Last point is set to zero at the end [6/28/2011 XPMUser]
}

void CFFTData::Alloc(int n)
{
	if (nPoint == n)
	{
		return ;
	}
	Clear();
	nPoint = n;
	pPt = new CFFTPoint[nPoint+1];
}

void CFFTData::Clear()
{
	if (pPt)
	{
		delete [] pPt;
		pPt = NULL;
	}
}
