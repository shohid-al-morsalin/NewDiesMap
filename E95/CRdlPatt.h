#pragma once

#include "CLine.h"
#include "Pattern.h"

class CRdlPatt /* : public CPattern*/
{
public:
	CPattern* pPatt;
	// 	CPattern dmy;

	void SetData(CPtrList& lines);
	CLine* GetLine(int n);
	int GetNLine();
	void ClearList();

	CRdlPatt& operator = (CRdlPatt& RdlPatt);
	virtual void Serialize(CArchive& ar);
	virtual BOOL SetUp(CWnd* pWnd);
	virtual BOOL Generate(float fRadius);
	virtual BOOL Generate2(int NX, int NY, float X0, float Y0, float sizeX, float sizeY, float fDiameter);
	virtual void DrawPattern(CDC* pDC, const CRect& rect, float);

	CRdlPatt();
	virtual ~CRdlPatt();

private:
	CPtrList lines;
};
