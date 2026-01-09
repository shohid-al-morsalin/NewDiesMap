#pragma once

#include "CRing.h"
#include "Pattern.h"

#define PIE180 0.0174532925199432957692369076849

class CCcrPatt  /*: public CPattern*/
{
public:
	BOOL SetUpSeg(CWnd*);
	CPattern* pPatt;
	// 	CPattern dmy;

	void SetData(CPtrList& rgs);
	CRing* GetRing(int n);
	int GetNRing();
	void ClearList();

	CCcrPatt& operator = (CCcrPatt& co);

	virtual void Serialize(CArchive& ar);
	virtual BOOL SetUp(CWnd*);
	virtual BOOL Generate(float);
	virtual BOOL Generate2(int, int, float, float, float, float, float);
	virtual void DrawPattern(CDC* pDC, const CRect& rect, float fRatio);

	CCcrPatt();
	virtual ~CCcrPatt();

private:
	CPtrList rings;
};
