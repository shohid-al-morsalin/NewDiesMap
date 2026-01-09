// MaskPattern.h: interface for the CMaskPattern class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "Pattern.h"

class CMaskPattern /*: public CPattern  */
{
public:
	CMaskPattern();
	virtual ~CMaskPattern();

public:
	CPattern* pPatt;
	// 	CPattern dmy;

	CMaskPattern& operator=(CMaskPattern& MaskPattern);

	virtual void Serialize(CArchive& ar);
	virtual BOOL SetUp(CWnd*);
	virtual BOOL Generate(float fDiameter);
	virtual BOOL Generate2(int NX, int NY, float X0, float Y0, float sizeX, float sizeY, float fDiameter);
	virtual void DrawPattern(CDC*, const CRect&, float);

protected:
	BOOL m_bSetUp;
};