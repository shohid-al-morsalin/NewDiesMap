#include "stdafx.h"

// #include <math.h>
#include <algorithm>
// #include <vector>
// #include <float.h>

#include "Data.h"
#include "MPoint.h"
#include "EditPoint.h"
#include "LSF3D.h"

#include "CMPList.h"
//#include "Recipe413.h"

// Note there is an identical define in Var.h [8/16/2012 Administrator]

using std::vector;
using std::sort;

bool PointSort(CMPoint* Lhs, CMPoint* Rhs) {
	CCoor* LCor = Lhs->GetCoor(),
		* RCor = Rhs->GetCoor();
	float Delta = fabs(LCor->y - RCor->y);
	if (Delta < 2.f) {
		if (LCor->x < RCor->y) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (LCor->y < RCor->y) {
		return true;
	}
	return false;
}

bool PointSortXFwd(CMPoint* Lhs, CMPoint* Rhs) {
	CCoor* LCor = Lhs->GetCoor(),
		* RCor = Rhs->GetCoor();
	if (LCor->x < RCor->x) {
		return true;
	}
	else {
		return false;
	}
}

bool PointSortXRev(CMPoint* Lhs, CMPoint* Rhs) {
	CCoor* LCor = Lhs->GetCoor(),
		* RCor = Rhs->GetCoor();
	if (LCor->x > RCor->x) {
		return true;
	}
	else {
		return false;
	}
}

void CMPList::Clear() {
	for (int i = 0; i < MaXPA * MAXMEPASET; i++) {
		Stats[i].Clear();
	}
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			delete p;
		}
	}
	MPLst.RemoveAll();
}

CMPoint* CMPList::Find(CCoor& point) {
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			if (p->Co.IsPoint(point)) return p;
		}
	}
	return NULL;
}

// Input point [7/19/2013 Yuen]
// Return CMPoint and Distance [7/19/2013 Yuen]
CMPoint* CMPList::FindNearest(CCoor& point, float& Distance) {
	CMPoint* pNearest = NULL;
	Distance = -1;
	float a, b, t;
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			a = p->Co.x - point.x;
			b = p->Co.y - point.y;
			t = a * a + b * b;
			if (Distance == -1) {
				// First point [7/19/2013 Yuen]
				Distance = t;
				pNearest = p;
			}
			if (t < Distance) {
				// subsequent point [7/19/2013 Yuen]
				Distance = t;
				pNearest = p;
			}
		}
	}
	Distance = sqrt(Distance);
	return pNearest;
}

int CMPList::GetCount() {
	return (MPLst.GetCount());
}

CMPoint* CMPList::Get(short index) {
	if (index < 0) {
		return NULL;
	}
	if (index >= MPLst.GetCount()) {
		return NULL;
	}
	POSITION pos = MPLst.FindIndex(index);
	return (CMPoint*)MPLst.GetAt(pos);
}

void CMPList::AddTailPoint(CMPoint* p) {
	MPLst.AddTail(p);
}

void CMPList::AddHeadPoint(CMPoint* p) {
	MPLst.AddHead(p);
}

// Stat[] is not copy [6/23/2010 Yuen]
CMPList& CMPList::operator =(CMPList& co) {
	Clear();
	POSITION pos = co.MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)co.MPLst.GetNext(pos);
		if (p) {
			CMPoint* pp = new CMPoint;
			if (pp) {
				*pp = *p;
				MPLst.AddTail(pp);
			}
		}
	}
	return *this;
}

void CMPList::Serialize(CArchive& ar) {
	int i;
	short maxPA;
	if (ar.IsStoring()) {
		magic = 2;
		ar << magic;
		short n = short(MPLst.GetCount());
		ar << n;
		if (n > 0) {
			POSITION pos = MPLst.GetHeadPosition();
			while (pos) {
				CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
				if (p) p->Serialize(ar);
			}
		}
		maxPA = MaXPA * MAXMEPASET;
		ar << maxPA;
		for (i = 0; i < maxPA; i++) {
			Stats[i].Serialize(ar);
		}
	}
	else {
		Clear();
		ar >> magic;
		short n;
		ar >> n;
		for (i = 0; i < n; i++) {
			CMPoint* p = new CMPoint; // Use default contructor
			if (p) {
				p->Serialize(ar);
				MPLst.AddTail(p);
			}
		}
		if (magic < 2) {
			return;
		}
		ar >> maxPA;
		if (maxPA > MaXPA * MAXMEPASET) {
			ASSERT(0);
		}
		for (i = 0; i < maxPA; i++) {
			Stats[i].Serialize(ar);
		}
	}
}

CMPList::CMPList() {
	magic = 0;
}

CMPList::~CMPList() {
	Clear();
}

void CMPList::FakeData() {}

// Make a pointer copy, do not delete MPList [3/13/2010 Valued Customer]
BOOL CMPList::GetMPList(CPtrList& MPList) {
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			MPList.AddTail(p);
		}
	}

	return TRUE;
}

// Cor : baseline correction value [11/2/2011 Administrator]
// index ranges from 0 to MAXPATTERN * MAXPA [11/8/2011 Administrator]
// void CMPList::AddTo3DLSF(CMPoint *pP, short index, short which, float Cor)
// {
// 	CData *d = pP->GetData(0);
// 	if (!d) return ;
// 	float fData = d->Get(index);
// 	if (fData == BADDATANEG)
// 	{
// 		return;
// 	}
// 	fData -= Cor;
// 	lSf3D[which].Input(pP->Co.x, pP->Co.y, fData);
// }

void CMPList::NormalizeWaferHeight(short index) {
	float fX, fY;
	CLSF3D LSF3D;
	POSITION pos, oldpos;

	// Find LS plane [3/10/2011 FSMT]
	pos = MPLst.GetHeadPosition();
	while (pos) {
		oldpos = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;

		if (p->ProgressSM != CMPoint::COMPLETED) {
			continue;
		}

		CData* d = p->GetData(0, FALSE);
		if (!d) continue;
		float fData = d->Get(index);
		if (fData >= 0) {
			p->GetCoor(fX, fY);
			LSF3D.Input(fX, fY, fData);
		}
		else {
			MPLst.RemoveAt(oldpos);
			delete p;
		}
	}

	// Normalize data: All points [3/10/2011 FSMT]
	double fMin = 9e30;
	double dfA, dfB, dfC;
	if (LSF3D.GetResult(dfA, dfB, dfC)) {
		//double dfDivider = sqrt(dfA * dfA + dfB * dfB + 1);	// This is wrong [5/10/2013 user]
		pos = MPLst.GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
			if (!p) continue;

			if (p->ProgressSM != CMPoint::COMPLETED) {
				continue;
			}

			CData* d = p->GetData(0, FALSE);
			if (!d) continue;
			float fData = d->Get(index);
			if (fData >= 0) {
				p->GetCoor(fX, fY);
				double dfNorDist = (fData - dfC - dfA * fX - dfB * fY) /*/ dfDivider*/;
				if (dfNorDist < fMin) {
					fMin = dfNorDist;
				}
				d->Set(index, dfNorDist);
			}
		}
	}

	// Shift so that all data has positive value: all points [3/10/2011 FSMT]
	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;

		if (p->ProgressSM != CMPoint::COMPLETED) {
			continue;
		}

		CData* d = p->GetData(0, FALSE);
		if (!d) continue;
		float fData = d->Get(index);
		float fDif = fData - fMin;
		if (fData >= 0) {
			d->Set(index, fDif);
		}
		else {
			d->Set(index, fDif);
		}
	}
}

// PaSet & Pa is 0 offset [9/5/2012 Administrator]
void CMPList::CalcStat(/*int index,*/int PaSet, int Pa, MParam& MPa/*BOOL bBowWarp*/, BOOL bView) {
	// 	if (index >= sizeof(Stats))
	// 	{
	// 		ASSERT(0);
	// 		return;
	// 	}

	if (MPa.Prb < 1) {
		return;
	}

	int nCount = 0;
	int index = PaSet * MaXPA + Pa;
	double fMax = -1e30;
	double fMin = 1e30;
	double fSum = 0;
	double fAve = 0;
	CMPoint* pCenter = NULL;
	double dfCenterDistance = 0;
	double dfCenterRadius = 9000.f;  // Make it very big initially [3/10/2011 FSMT]

	CStat* pStat = &Stats[index];
	pStat->Clear();

	//[ temporary calc

	int iCount = 0/*, nCount2*/;
	//nCount2 = MPLst.GetCount();
	BOOL bFirst = TRUE;
	POSITION pos;
	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;

		CData* d = p->GetData(0, FALSE);
		if (!d) continue;
		float fData = d->Get(index);
		if (fData == BADDATANEG) {
			continue;
		}

		float fX, fY;
		p->GetCoor(fX, fY);

		double dfRadius = (fX * fX + fY * fY);
		if (dfRadius <= dfCenterRadius) {
			pCenter = p;
			dfCenterRadius = dfRadius;
			dfCenterDistance = fData;
		}

		if (!bFirst) {
			if (fMax < fData) {
				fMax = fData;
			}
			if (fData < fMin) {
				fMin = fData;
			}

			fSum += fData; //20230321  fSum += (fData * fData);
			fAve += fData;
			nCount++;
		}
		else {
			fSum = fData; //20230321 fSum = (fData * fData);
			fAve = fMax = fMin = fData;
			nCount++;
			bFirst = FALSE;
		}
	}

	//[20230321
	double Av = fSum / (double)nCount;
	double Av2 = 0;
	double dV;
	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;
		CData* d = p->GetData(0, FALSE);
		if (!d) continue;
		float fData = d->Get(index);
		if (fData == BADDATANEG) {
			continue;
		}
		dV = (double)(fData - Av);
		Av2 += (dV * dV);
	}
	Av2 /= double(nCount);
	//]

	if (nCount > 0) {
		fSum /= nCount;
		pStat->m_fAverage = (float)(fAve /= nCount);
		pStat->m_fSTD = sqrt(Av2);   //20230221 pStat->m_fSTD = (float)sqrt(fSum - fAve* fAve); 
		pStat->m_fMax = (float)fMax;
		pStat->m_fMin = (float)fMin;
		pStat->m_fTTV = (float)(fMax - fMin);
		if (MPa.IsEchoBowWarpMeasurement() || MPa.IsConfocalWarpMeasurement() || MPa.IsConfocalDualWarpMeasurement()) {//23102024
			pStat->m_fWarp = (float)(fMax - fMin);
			pStat->m_fBow = (float)dfCenterDistance;
		}
	}
}

BOOL CMPList::DeletePoint(CCoor& Point) {
	POSITION pos = MPLst.GetHeadPosition(), posFound = NULL;
	while (pos) {
		posFound = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if ((p) && (p->GetCoor()->IsPoint(Point))) {
			// The numbers will be out of order so needs readjustment !
			while (pos) {
				p = (CMPoint*)MPLst.GetNext(pos);
				if (p) p->GetCoor()->n--;
			}
			p = (CMPoint*)MPLst.GetAt(posFound);
			MPLst.RemoveAt(posFound);
			delete p;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CMPList::EditLocMP(CCoor& Point, float XCoord, float YCoord) {
	POSITION pos = MPLst.GetHeadPosition();
	bool bFound = false;
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if ((p) && (p->GetCoor()->IsPoint(Point))) {
			CEditPoint dlg;

			dlg.m_XCoor = XCoord;
			dlg.m_YCoor = YCoord;

			if (dlg.DoModal() == IDOK) {
				p->GetCoor()->x = dlg.m_XCoor;
				p->GetCoor()->y = dlg.m_YCoor;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CMPList::ClearData() {
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			p->ClearData();
		}
	}
}

BOOL CMPList::Sort(/*BOOL bASc*/) {
	int BucketSize = sqrt(MPLst.GetCount() * 1.3);
	float YMin, YMax;
	if (!GetMinMaxY(YMin, YMax)) {
		return FALSE;
	}
	float Delta = YMax - YMin;
	if (fabs(Delta) < .05f) {
		return FALSE;
	}
	float Div = Delta / (((BucketSize - 1) == 0) ? 1 : ((BucketSize - 1)));

	//added	20080415
	if (BucketSize < 5) {
		BucketSize = Delta / 12.7f;
		Div = 12.7f;
	}
	if (BucketSize < 1) {
		BucketSize = 1;
	}

	vector<vector<CMPoint*> > Map(BucketSize);

	POSITION pos = MPLst.GetHeadPosition();
	short Bucket = 0;
	while (pos) {
		CMPoint* pMPoint = (CMPoint*)MPLst.GetNext(pos);
		if (pMPoint) {
			CCoor* pCo = pMPoint->GetCoor();
			Bucket = short((pCo->y - YMin) / Div);
			if (Bucket >= BucketSize) {
				Bucket = BucketSize - 1;
			}
			if (Bucket < 0) {
				Bucket = 0;
			}
			Map[Bucket].push_back(pMPoint);
		}
	}
	//TRACE(">> Unsorted\n");
	MPLst.RemoveAll();
	int Index = 0, Point = 1;
	for (int iBucket = 0; iBucket < BucketSize; iBucket++) {
		if (Map[iBucket].empty()) {
			continue;
		}
		if (Index % 2) {
			sort(Map[iBucket].begin(), Map[iBucket].end(), PointSortXFwd);
		}
		else {
			sort(Map[iBucket].begin(), Map[iBucket].end(), PointSortXRev);
		}
		for (unsigned int iPt = 0; iPt < Map[iBucket].size(); iPt++) {
			CMPoint* pMp = Map[iBucket][iPt];
			pMp->Co.n = Point; /*pMp->Co.p = Point;*/
			Point++;
			MPLst.AddTail(pMp);
		}
		Index++;
	}
	return TRUE;
}

BOOL CMPList::GetMinMaxY(float& YMin, float& YMax) {
	POSITION pos = MPLst.GetHeadPosition();
	YMin = FLT_MAX, YMax = FLT_MIN;
	while (pos) {
		CMPoint* pMPoint = (CMPoint*)MPLst.GetNext(pos);
		if (pMPoint) {
			CCoor* pCo = pMPoint->GetCoor();
			if (YMin > pCo->y) {
				YMin = pCo->y;
			}
			if (YMax < pCo->y) {
				YMax = pCo->y;
			}
		}
	}
	return (YMin != FLT_MAX) && (YMax != FLT_MIN);
}

void CMPList::EnsureData() {
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* mp = (CMPoint*)MPLst.GetNext(pos);
		if (mp) {
			mp->GetData(0, TRUE);
		}
	}
}

void CMPList::Conv(char** msg) {
	int i;
	for (i = 0; i < MaXPA * MAXMEPASET; i++) {
		Stats[i].Conv(msg);
	}
	int cnt = 0;
	CMake::Conv(cnt, msg);

	ClearData();

	for (i = 0; i < cnt; i++) {
		CMPoint* p = new CMPoint;
		p->Conv(msg);
		MPLst.AddTail(p);
	}
}

long CMPList::Binary(char** msg) {
	long sz = 0;
	for (int i = 0; i < MaXPA * MAXMEPASET; i++) {
		sz += Stats[i].Binary(msg);
	}

	int cnt = MPLst.GetCount();
	sz += CMake::Binary(cnt, msg);

	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		sz += p->Binary(msg);
	}
	return sz;
}

long CMPList::GetLength() {
	long sz = 0;
	for (int i = 0; i < MaXPA * MAXMEPASET; i++) {
		sz += Stats[i].GetLength();
	}

	int cnt;
	sz += CMake::GetLength(cnt);

	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		sz += p->GetLength();
	}
	return sz;
}

void CMPList::UpdateControl(CListCtrl& Ctrl) {
	int i = 0;
	CString str;

	Ctrl.DeleteAllItems();

	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			short n = 1;
			str.Format("%d", p->Co.n);
			int idx = Ctrl.InsertItem(i++, str);
			Ctrl.SetItemText(idx, n++, p->GetPropStr());
			Ctrl.SetItemText(idx, n++, p->GetMeSetStr());
			Ctrl.SetItemText(idx, n++, p->GetPatSetStr());
		}
	}
}

BOOL CMPList::DeletePoint(short nSel) {
	POSITION pos = MPLst.FindIndex(nSel);
	if (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetAt(pos);
		if (p) {
			delete p;
		}
		MPLst.RemoveAt(pos);
		return TRUE;
	}
	return FALSE;
}

void CMPList::Renumber() {
	int i = 1;
	CString str;

	// Put points with marker at the head of list [7/23/2013 Yuen]
	POSITION opos;
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		opos = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p && (p->Co.p == 3)) {
			MPLst.RemoveAt(opos);
			MPLst.AddHead(p);
			break;
		}
	}
	pos = MPLst.GetHeadPosition();
	while (pos) {
		opos = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p && (p->Co.p == 2)) {
			MPLst.RemoveAt(opos);
			MPLst.AddHead(p);
			break;
		}
	}
	pos = MPLst.GetHeadPosition();
	while (pos) {
		opos = pos;
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p && (p->Co.p == 1)) {
			MPLst.RemoveAt(opos);
			MPLst.AddHead(p);
			break;
		}
	}

	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			p->Co.n = i;
			i++;
		}
	}
}

void CMPList::NormalizeData(short index, BOOL bShift, FILE* fp) {
	// Normalize data: All points [3/10/2011 FSMT]
	float fX, fY;
	BOOL bFirst = TRUE;
	float fMin = 0, fMax = 0;
	double dfA, dfB, dfC;
	POSITION pos;
	CLSF3D Lsf;

	// Phase one load data to lSf3D [5/11/2013 user]
	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;

		CData* d = p->GetData(0, FALSE);
		if (!d) continue;
		p->GetCoor(fX, fY);
		//[[20250225
		float zValue = d->Get(index);
		if (zValue == BADDATANEG) continue;
		//]]
		Lsf.Input(fX, fY, d->Get(index));
	}

	// Find best fit plane [5/11/2013 user]
	if (Lsf.GetResult(dfA, dfB, dfC)) {
		pos = MPLst.GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
			if (!p) continue;

			CData* d = p->GetData(0, FALSE);
			if (!d) continue;
			float fData = d->Get(index);
			//[[20250225
			if (fData == BADDATANEG)
			{
				continue;
			}
			//]]
			p->GetCoor(fX, fY);
			float dfNorDist = fData - (dfC + dfA * fX + dfB * fY);
			if (!bFirst) {
				if (dfNorDist < fMin) {
					fMin = dfNorDist;
				}
				if (dfNorDist > fMax) {
					fMax = dfNorDist;
				}
			}
			else {
				bFirst = FALSE;
				fMax = fMin = dfNorDist;
			}
			if (fp) {
				fprintf(fp, "%f,%f,%f,%f\n", fX, fY, fData, dfNorDist);
			}
			d->Set(index, dfNorDist);
		}
	}

	if (!bShift) {
		return;
	}

	// Shift so that all data has positive value: all points [3/10/2011 FSMT]
	if (fp) {
		fprintf(fp, "\nShifted\n");
	}
	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;

		CData* d = p->GetData(0, FALSE);
		if (!d) continue;
		float fData = d->Get(index);
		if (fData == BADDATANEG) {
			continue;
		}
		fData -= fMin;
		d->Set(index, fData);
		if (fp) {
			fprintf(fp, "%f,%f,%f\n", p->Co.x, p->Co.y, fData);
		}
	}
}

void CMPList::NormalizeData(short index, short RefPt1, short RefPt2, short RefPt3, FILE* fp) {
	// Normalize data: All points [3/10/2011 FSMT]
	float fX, fY;
	double dfA, dfB, dfC;
	POSITION pos;
	CLSF3D Lsf;

	// Load the three reference points [5/11/2013 user]
	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (p) {
			if ((p->Co.n == RefPt1) || (p->Co.n == RefPt2) || (p->Co.n == RefPt3)) {
				CData* d = p->GetData(0, FALSE);
				if (d) {
					p->GetCoor(fX, fY);
					float fData = d->Get(index);
					Lsf.Input(fX, fY, fData);
				}
			}
		}
	}

	if (Lsf.GetResult(dfA, dfB, dfC)) {
		pos = MPLst.GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
			if (p) {
				CData* d = p->GetData(0, FALSE);
				if (d) {
					p->GetCoor(fX, fY);
					float fData = d->Get(index);
					float dfNorDist = fData - (dfA * fX + dfB * fY + dfC);
					d->Set(index, dfNorDist);
					if (fp) {
						fprintf(fp, "%f,%f,%f\n", fX, fY, dfNorDist);
					}
				}
			}
		}
	}
}

// Flip top probe data point because it is opposite to begin with [5/9/2013 user]
// index now ranges from 0 to MAXPATTERN * MAXPA [11/8/2011 Administrator]
void CMPList::InvertSurfaceToTopProbeA(short index, FILE* fp) {
	float fMin = 9.99e20f, fMax = -9.99e20f;

	// Find Max and Min [11/2/2011 Administrator]
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;

		CData* d = p->GetData(0, FALSE);
		if (!d) continue;
		float fData = d->Get(index);
		if (fData == BADDATANEG) {
			continue;
		}
		if (fData < fMin) {
			fMin = fData;
		}
		if (fData > fMax) {
			fMax = fData;
		}
	}

	// Reverse data [11/2/2011 Administrator]
	pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;

		CData* d = p->GetData(0, FALSE);
		if (!d) continue;
		float v = d->Get(index);
		if (v == BADDATANEG) {
			continue;
		}
		d->Set(index, (fMax - v) + fMin);
	}
}

void CMPList::SetMarker(CMPoint* pMP, short Mkr) {
	if (!pMP) {
		return;
	}
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;
		if (p->Co.p == Mkr) {
			p->Co.p = 0;
			break;
		}
	}
	pMP->SetMarker(Mkr);
}

void CMPList::ClearMarker(CMPoint* pMP, short Mkr) {
	if (!pMP) {
		return;
	}
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;
		if (p->Co.p == Mkr) {
			p->Co.p = 0;
			break;
		}
	}
	pMP->ClearMarker(Mkr);
}

BOOL CMPList::HasMarkers() {
	short cnt = 0;

	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;
		if (p->Co.p != 0) {
			switch (p->Co.p) {
			case 1:
				cnt += 1;
				break;
			case 2:
				cnt += 2;
				break;
			case 3:
				cnt += 4;
				break;
			}
		}
	}
	if (cnt == 7) {
		// Correctly set markers [7/23/2013 Yuen]
		return TRUE;
	}
	// either no marker set or incorrectly set marker [7/23/2013 Yuen]
	return FALSE;
}

CMPoint* CMPList::GetMarker(short n) {
	POSITION pos = MPLst.GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)MPLst.GetNext(pos);
		if (!p) continue;
		if (p->Co.p == n) {
			return p;
		}
	}
	return NULL;
}