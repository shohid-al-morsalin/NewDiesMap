#pragma once

class CXYPair {
public:

	short magic = 0;
	float x = 0, y = 0;

	CXYPair() {
		//x = y = 0;
	}
	void Get(CString& str) {
		str.Format("%.3f,%.3f", x, y);
	}
	void Set(float xx, float yy) {
		x = xx; y = yy;
	}

	void Serialize(CArchive& ar) {
		if (ar.IsStoring()) {
			magic = 1;
			ar << magic;
			ar << x;
			ar << y;
		}
		else {
			ar >> magic;
			ar >> x;
			ar >> y;
		}
	}
	CXYPair& operator=(CXYPair& co) {
		magic = co.magic;
		x = co.x;
		y = co.y;
		return *this;
	}
};