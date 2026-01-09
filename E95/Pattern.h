// Pattern.h: interface for the CPattern class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CMPList.h"
#include "Coor.h"	// Added by ClassView

class CCoor;
class CRecipe413;

class CPattern {
public:
	CMPList MP;

	short magic;
	BOOL bUseMask;	// Intermediate control value, not serialize [3/24/2012 Administrator]
	CPattern& operator=(CPattern& co);

public:
	CPattern(/*LPCTSTR lpszPatternName*/);
	virtual ~CPattern();
	BOOL IsPointOnMask(const CCoor& point);
	BOOL IsPointDefined(float X, float Y);
	void AddMPoint(CCoor& p);
	BOOL AddMPoint(float fX, float fY, float fZ);
	BOOL AddMPoint(int n, float fX, float fY, float fZ/*, float fXOffs, float fYOffs*/);

public:
	CMPoint* GetMarker(short n);
	BOOL HasMarkers();
	void Renumber();
	void UpdateControl(CListCtrl& Ctrl);
	void Conv(char** msg);
	long Binary(char** msg);
	long GetLength();

	CCoor* GetPoint(int idx);
	BOOL Interpolate(CRecipe413* pRcp, double fX, double fY, int iChan, double* fZ);
	void Clear();
	virtual BOOL DelPoint(CCoor& p);
	virtual CCoor* GetCoor(short index);

	//     virtual CMPoint * FindMP(CCoor &point) { return MP.Find(point); }
	virtual CMPoint* FindNearestMP(CCoor& point, float& Distance) { return MP.FindNearest(point, Distance); }
	virtual BOOL EditMP(CCoor& Point, float fX, float fY) { return MP.EditLocMP(Point, fX, fY); }
	virtual void ClearData(void) { MP.ClearData(); }
	virtual BOOL DelMP(CCoor& Point) { return MP.DeletePoint(Point); }
	virtual void FakeData() { MP.FakeData(); }
	virtual BOOL GetMPList(CPtrList& MPList) { return MP.GetMPList(MPList); }

	// 	virtual void Serialize(CArchive & ar) = NULL;
	// 	virtual BOOL SetUp(CWnd *pWnd) = NULL;
	// 	virtual BOOL Generate(float fDiameter) = NULL;
	// 	virtual BOOL Generate2(int NX, int NY, float X0, float Y0, float sizeX, float sizeY, float fDiameter) = NULL;

	BOOL CheckAllPoints(CRecipe413* pRcp);
	BOOL EliminateOutOfChuck(CRecipe413* pRcp);
	// 	virtual void DrawPattern(CDC *pDC, const CRect &rect, float fRatio) = NULL;
};
