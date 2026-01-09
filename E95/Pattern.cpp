// Pattern.cpp: implementation of the CPattern class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MPoint.h"
#include "Pattern.h"
//#include "wafercal.h"
#include "ChuckMask.h"
#include "data.h"
#include <math.h>

#include "413App.h"
//#include "AppSystem.h"
//#include "AppViews.h"

//#include "S8Dlg.h"

#include "Recipe413.h"
//#include "PZT\measurephase.h"

#define sqr(x) ((x)*(x))

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPattern::CPattern(/*LPCTSTR lpszPatternName*/)/* : m_strPatternName(lpszPatternName)*/
{
	magic = 0;
	bUseMask = FALSE;
}

CPattern::~CPattern() {
	MP.Clear();
}

CPattern& CPattern::operator =(CPattern& Pattern) {
	magic = Pattern.magic;
	MP = Pattern.MP;
	return *this;
}

BOOL CPattern::DelPoint(CCoor& p) {
	return MP.DeletePoint(p);
}

CCoor* CPattern::GetCoor(short index) {
	CMPoint* p = MP.Get(index);
	if (!p) return NULL;
	return p->GetCoor();
}

void CPattern::AddMPoint(CCoor& p) {
	AddMPoint(p.n, p.x, p.y, p.z);
}

BOOL CPattern::AddMPoint(int n, float fX, float fY, float fZ) {
	BOOL bRes = FALSE;
	CMPoint* pMPoint = new CMPoint;
	if (!pMPoint) return FALSE;
	if (n > SHRT_MAX) return FALSE;

	pMPoint->SetCoorXYZ((short)n, fX, fY, fZ);
	if ((IsPointOnMask(pMPoint->Co)) || (!bUseMask)) {
		MP.AddTailPoint(pMPoint);
		bRes = TRUE;
	}
	else {
		delete pMPoint;
	}
	return bRes;
}

BOOL CPattern::AddMPoint(float fX, float fY, float fZ) {
	short n = MP.GetCount() + 1;
	if (n > SHRT_MAX) return FALSE;

	CMPoint* pMPoint = new CMPoint;
	if (!pMPoint) return FALSE;

	pMPoint->SetCoorXYZ(n, fX, fY, fZ);
	MP.AddTailPoint(pMPoint);
	return TRUE;
}

BOOL CPattern::IsPointOnMask(const CCoor& point) {
	// Allowed to add in case mask is absent
	BOOL bRes = TRUE;

	CChuckMask* pChuckMask = p413App->Global.GetChuckMask();
	//int iHole = 0;
	if (pChuckMask) {
		bRes = pChuckMask->IsPointOnMask(point.x, point.y);
	}
	else {
		//TRACE("Chuck mask not found ! The point will be added for measurement");
	}
	return bRes;
}

BOOL CPattern::IsPointDefined(float X, float Y) {
	// 	CCoor *pCoor = NULL;
	// 	for(int iPoint = 0 ;iPoint < MP.GetCount() ; iPoint++)
	// 	{
	// 		pCoor = GetCoor(iPoint);
	// 		if(pCoor)
	// 		{
	// 			float DeltaX = pCoor->x -X,DeltaY =pCoor->y - Y;
	// 			if((fabs(DeltaX) <.01f )&&(fabs(DeltaY)<.01f))
	// 			{
	// 				return TRUE;
	// 			}
	// 		}
	// 	}
	// 	return FALSE;

	CPtrList* p = &MP.MPLst;
	POSITION pos = p->GetHeadPosition();
	while (pos) {
		CMPoint* pt = (CMPoint*)p->GetNext(pos);
		if (pt) {
			float DeltaX = pt->Co.x - X, DeltaY = pt->Co.y - Y;
			if ((fabs(DeltaX) < .01f) && (fabs(DeltaY) < .01f)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CPattern::CheckAllPoints(CRecipe413* pRcp) {
	if (!pRcp) {
		ASSERT(0);
		return FALSE;
	}

	float Radius = pRcp->GetWp()->mapsize / 2.0f;
	CCoor* pCoor = NULL;
	int iPoint;
	for (iPoint = 0; iPoint < MP.GetCount(); iPoint++) {
		pCoor = GetCoor(iPoint);
		if (pCoor) {
			float Dist = (float)sqrt(sqr(pCoor->x) + sqr(pCoor->y));
			if (Dist > Radius) {
				return FALSE;
			}
		}
	}
	CChuckMask* pChuckMask = p413App->Global.GetChuckMask();
	if (!pChuckMask) {
		return TRUE;
	}
	for (iPoint = 0; iPoint < MP.GetCount(); iPoint++) {
		pCoor = GetCoor(iPoint);
		if (pCoor) {
			//int type = pRcp->Me.measurement;
			if ((!pChuckMask->IsPointOnMask(pCoor->x, pCoor->y)) && (bUseMask)) {
				return FALSE;
			};
		}
	}
	return TRUE;
}

BOOL CPattern::EliminateOutOfChuck(CRecipe413* pRcp) {
	if (!pRcp) {
		ASSERT(0);
		return FALSE;
	}

	float Radius = pRcp->GetWp()->mapsize / 2.0f;

	CCoor* pCoor = NULL;

	int iPoint;
	for (iPoint = 0; iPoint < MP.GetCount(); iPoint++) {
		pCoor = GetCoor(iPoint);
		if (pCoor) {
			float Dist = (float)sqrt(sqr(pCoor->x) + sqr(pCoor->y));
			if (Dist > Radius) {
				CCoor C;
				C.x = pCoor->x;
				C.y = pCoor->y;
				MP.DeletePoint(C);
				iPoint--;
			}
		}
	}
	CChuckMask* pChuckMask = p413App->Global.GetChuckMask();
	if (!pChuckMask) {
		return TRUE;
	}
	for (iPoint = 0; iPoint < MP.GetCount(); iPoint++) {
		pCoor = GetCoor(iPoint);
		if (pCoor) {
			if ((!pChuckMask->IsPointOnMask(pCoor->x, pCoor->y)) && (bUseMask)) {
				MP.DeletePoint(*pCoor);
				iPoint--;
			};
		}
	}

	return TRUE;
}

void CPattern::Clear() {
	MP.Clear();
}

BOOL CPattern::Interpolate(CRecipe413* pRcp, double fX, double fY, int iChan, double* fZ) {
	if (!pRcp) {
		ASSERT(0);
		return FALSE;
	}

	float Radius = pRcp->GetWp()->mapsize / 2.0f;
	CCoor* pCoor = NULL;

	double weight = 0;
	double totalWeight = 0;
	double nodeValue = 0;
	double dfRatio = 4 / Radius;

	int sz = MP.GetCount();

	for (int iPoint = 0; iPoint < MP.GetCount(); iPoint++) {
		pCoor = GetCoor(iPoint);
		CMPoint* p = MP.Get(iPoint);
		if (pCoor) {
			double dx = (pCoor->x - fX);
			double dy = (pCoor->y - fY);
			weight = exp(-sqrt(dx * dx + dy * dy) * dfRatio);
			if (p) {
				CData* pData = p->GetData(0, FALSE);
				if (pData) {
					double fZ1 = pData->Get(iChan);
					//if (fZ1 > 0) { // Commented 20240904 Sihab
						totalWeight += weight;
						double zz = fZ1 * weight;
						nodeValue += zz;
					//}
				}
			}
		}
	}
	if (totalWeight > 0) {
		*fZ = nodeValue / totalWeight;
	}
	else {
		*fZ = 0;
	}
	return TRUE;
}

CCoor* CPattern::GetPoint(int idx) {
	if (idx > 0) {
		return GetCoor(idx - 1);
	}
	return NULL;
}

void CPattern::Conv(char** msg) {
	CMake::Conv(bUseMask, msg);
	MP.Conv(msg);
}

long CPattern::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(bUseMask, msg);
	sz += MP.Binary(msg);

	return sz;
}

long CPattern::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(bUseMask);
	sz += MP.GetLength();

	return sz;
}

void CPattern::UpdateControl(CListCtrl& Ctrl) {
	MP.UpdateControl(Ctrl);
}

void CPattern::Renumber() {
	MP.Renumber();
}

BOOL CPattern::HasMarkers() {
	return MP.HasMarkers();
}

CMPoint* CPattern::GetMarker(short n) {
	return MP.GetMarker(n);
}