#pragma once

#include "Pattern.h"

class CCustPatt /*: public CPattern*/
{
public:
	CPattern* pPatt;
	// 	CPattern dmy;

	void ClearList();
	CCustPatt();
	virtual ~CCustPatt();

	// t.w. the following four variable should really be in a new class crosspatt, do it later
	float m_XPitch, m_YPitch;
	float m_XCenter, m_YCenter;

	/** Standard operator Implementation */
	CCustPatt& operator = (CCustPatt& Rhs);
	/** Usual MFC stuff */
	virtual void Serialize(CArchive& ar);
	virtual BOOL SetUp(CWnd*);
	virtual BOOL Generate(float);
	virtual BOOL Generate2(int, int, float, float, float, float, float);
	virtual void DrawPattern(CDC*, const CRect&, float);
};
