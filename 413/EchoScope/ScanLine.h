// ScanLine.h: interface for the CScanLine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "MaxMin.h"

#define LEFTPKEDGE	500
#define MIDPKEDGE	650
#define RIGHTPKEDGE	550
#define CTRMASSPK	1000

class CMeParam;
class CScanPoint;

class CScanLine {
public:
	int Count;
	int Idx;
	int iCM, yCM, iMid, yMid;
	float CM, Mid;
	float X1, X2;
	int iX1, iX2;
	CScanPoint* pData;	// contains interference signal of one channel [6/21/2013 Yuen]

public:
	void GetCMv2(short i, float wd, float thres);
	short GetLimits(int* Pos);
	void Dump(FILE* fp);
	void CalcMaxMin();
	void DupY3Y2(float baseline);
	void ZeroY2();
	void FakeMaxMin();
	void FakPoint();
	void Fake(int nPoints);
	void GetCM(int index, int width, short iH, int* pP, unsigned short* pS);
	void Dump(CString name);
	int GetRangeX();
	void Merge(CScanLine* pL, short Threshold, float baseline);
	CMaxMin MaxMinX, MaxMinY1, MaxMinY2;
	void Reset();
	void EliminateNeighbour(int n, short dx);
	void Clean(short Chan, CMeParam* Me);
	void RejectPeak(short spc, short PkHeight, float baseline);
	void DupY1();
	void Smooth(float baseline);
	void Reduce(float baseline);
	void Shrink(short skip);
	BOOL Dup(CScanLine* pL);
	BOOL Set(int x, int y, int n);
	BOOL Alloc(int size);
	void Clear();
	CScanLine();
	virtual ~CScanLine();
};
