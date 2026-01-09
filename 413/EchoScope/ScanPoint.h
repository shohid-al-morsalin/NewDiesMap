// ScanPoint.h: interface for the CScanPoint class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
class CScanPoint {
public:
	int x, y1, y2, y3;
	int n;  // n is the index into actual data [9/23/2011 C2C]

public:
	CScanPoint& operator = (CScanPoint& co);
	void Set(int xx, int yy, int nn);
	CScanPoint();
	virtual ~CScanPoint();
};
