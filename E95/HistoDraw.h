#pragma once

#define MAXBINS	64

class CHistoDraw : public CStatic {
public:
	CHistoDraw();

public:
	int yMax;
	int nCount, nCountLimit;
	float xMin, xMax;  // Range of bins [6/15/2011 XPMUser]
	int Bins[MAXBINS];
	float vBin[MAXBINS];
	COLORREF m_bgColor;

public:
	void GetRange(float& min, float& max);
	void Draw(CDC* pDC);
	void Bin(float val);
	void SetRange(float min, float max);
	void ClearBins();
	virtual ~CHistoDraw();

protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
