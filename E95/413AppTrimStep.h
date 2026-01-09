#pragma once

#include "413AppEcho.h"

class C413AppTrimStep : public C413AppEcho {
protected:
	void Smooth(short* pBuf, short len);
	void Integrate(short* pBuf, short len, short wdw);
	void ContrastStretch(short* pBuf, short len, float contrastLevel = 0.1f);	//20210329
	void SaveExtractedLine(short n, short* pBuf, short len, CString tm);
	float GetTrimWidthIn(short* pBuf, short len, float& ang, BOOL& bX, short& left, short mid);
	float GetTrimWidthOut(short* pBuf, short len, float& ang, BOOL& bX, short& right, short mid);
	void PreprocessTrimData(short** pBuf, short len, BOOL bI2, short nSmooth, BOOL thresh, CString tm, float contrastLevel = 0.1f);	//20210329

public:
	float GetEdgeOffset(short nLines, short Spacing, float angle, BOOL bInner, CImgWnd* pImgWnd);

	C413AppTrimStep();
	virtual ~C413AppTrimStep();
};
