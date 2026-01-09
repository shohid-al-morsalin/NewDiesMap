// PattMatch.h: interface for the CPattMatch class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

struct SLocMatch {
	// 3 best match position [5/1/2013 user]
	int nX, nY;
	double GOF;

	SLocMatch() {
		Clear();
	}
	void Clear() {
		nX = nY = 0;
		GOF = 0;
	};
	void Set(int X, int Y, double Sum) {
		nX = X; nY = Y; GOF = Sum;
	}
};

class CPattMatch {
public:
	CPattMatch();
	virtual ~CPattMatch();
};
