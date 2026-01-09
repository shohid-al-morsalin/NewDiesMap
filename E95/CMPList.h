#pragma once

#include "CStat.h"
#include "MParam.h"

class CCoor;
class CMPoint;
struct MParam;
class CRecipe413;

class CMPList {
public:
	CMPoint* GetMarker(short n);
	BOOL HasMarkers();
	void SetMarker(CMPoint* pMP, short Mkr);
	void ClearMarker(CMPoint* pMP, short Mkr);
	void InvertSurfaceToTopProbeA(short index, FILE* fp);
	void NormalizeData(short index, BOOL bShift, FILE* fp = NULL);
	void NormalizeData(short index, short RefPt1, short RefPt2, short RefPt3, FILE* fp = NULL);

	void NormalizeWaferHeight(short idx);
	// 	void AddTo3DLSF(CMPoint *pP, short idx, short which, float Cor);
	void Renumber();
	BOOL DeletePoint(short nSel);
	void UpdateControl(CListCtrl& Ctrl);
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();
	void EnsureData();

	//CLSF3D lSf3D[8];  // Support only 8 set of bow and warp in one recipe only [10/27/2011 Administrator]
	CPtrList MPLst;
	CStat Stats[MaXPA * MAXMEPASET];

	void DumpPoints(const CString& Msg);
	BOOL GetMinMaxY(float& YMin, float& YMax);
	BOOL Sort(/*BOOL bASc*/);
	void ClearData(void);

	BOOL EditLocMP(CCoor& Point, float XCoord, float YCoord);
	BOOL DeletePoint(CCoor& Point);
	void CalcStat(/*int index,*/ int PaSet, int Pa, MParam& MPa/*BOOL bBowWarp*/, BOOL bView);

	void FakeData();
	BOOL GetMPList(CPtrList& MPList);

	CMPoint* Find(CCoor& point);
	CMPoint* FindNearest(CCoor& point, float& Distance);

	void Clear();
	int GetCount();
	CMPoint* Get(short index);
	void AddTailPoint(CMPoint* p);
	void AddHeadPoint(CMPoint* p);

	CMPList& operator = (CMPList& co);
	void Serialize(CArchive& ar);

	CMPList();
	~CMPList();

private:
	short magic;
};
