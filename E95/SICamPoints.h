// SICamPoints.h: interface for the CSICamPoints class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
class CSICamPoints {
public:
	CPoint CurPt, StartPt, EndPt;
public:
	void SwapStartEnd() {
		StartPt.x ^= EndPt.x;
		EndPt.x ^= StartPt.x;
		StartPt.x ^= EndPt.x;

		StartPt.y ^= EndPt.y;
		EndPt.y ^= StartPt.y;
		StartPt.y ^= EndPt.y;
	}
	CSICamPoints();
	virtual ~CSICamPoints();
};
