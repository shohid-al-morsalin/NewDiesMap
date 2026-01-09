// MaskPattern.cpp: implementation of the CMaskPattern class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MaskPattern.h"
#include "MaskInfo.h"	// for frame holes
#include "ChuckMask.h"

#include "413App.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMaskPattern::CMaskPattern()/* : CPattern(_T("Mask"))*/
{
	m_bSetUp = FALSE;
	pPatt = NULL;
}

CMaskPattern::~CMaskPattern() {}

CMaskPattern& CMaskPattern::operator=(CMaskPattern& MaskPattern) {
	//	CPattern::operator=(MaskPattern);
	m_bSetUp = MaskPattern.m_bSetUp;
	return *this;
}

void CMaskPattern::Serialize(CArchive& ar) {
	short magic;
	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		//        dmy.MP.Serialize(ar);
	}
	else {
		ar >> magic;
		if (magic < 1) {
			if (pPatt) pPatt->MP.Serialize(ar);
		}
	}
}

BOOL CMaskPattern::SetUp(CWnd*) {
	m_bSetUp = TRUE;
	return TRUE;
}

BOOL CMaskPattern::Generate(float fDiameter) {
	if (!pPatt) {
		return FALSE;
	}
	if (m_bSetUp) {
		m_bSetUp = FALSE;
		ASSERT(0);
		// Never call this function as it does NOT generate a correct pattern
		//...Vijay#3July03

		CChuckMask* ChuckMask = p413App->Global.GetChuckMask();
		if (!ChuckMask) {
			return FALSE;
		}
		//if (!ChuckMask.LoadFromFile(rp->GetMaskFile())) return FALSE;
		int n;
		float* pfPoint = ChuckMask->GetCenterPoints(n);
		if (!pfPoint) return FALSE;

		pPatt->MP.Clear();
		short nCount = 1;

		float* pf = pfPoint;
		for (int i = 0; i < n; i++) {
			float fX = *pf++;
			float fY = *pf++;
			if (fX * fX + fY * fY < fDiameter * fDiameter / 4) {
				pPatt->AddMPoint(nCount, fX, fY, 0/*, 0, 0*/);
				nCount++;
			}
		}
		delete pfPoint;
		return (0 < nCount);
	}
	return FALSE;
}

BOOL CMaskPattern::Generate2(int NX, int NY, float X0, float Y0, float sizeX, float sizeY, float fDiameter) {
	return TRUE;
}

void CMaskPattern::DrawPattern(CDC*, const CRect&, float) {}