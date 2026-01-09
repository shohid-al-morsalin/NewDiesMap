#include "stdafx.h"
#include "Score.h"

#define SQRE(x) ((x)*(x))

void IMGL::CScore::Clear()
{
	marked = total = 0;

	pV = pC = pX = pY = pR = 0;
}

void IMGL::CScore::SetR(int r)
{
	pR = r;
}

void IMGL::CScore::Set(int x, int y, int v)
{
	pC = 1;
	pX = x; pY = y; pV = v;
}

void IMGL::CScore::IncCount(CScore * pScore)
{
	pC += pScore->pC;
	pV += pScore->pV;
	pX = (pX + pScore->pX) / 2.0f;
	pY = (pY + pScore->pY) / 2.0f;
}

IMGL::CScore::CScore()
{
	pV = pC = pR = 0;
	marked = 0; total = 0;
	pX = pY = 0;
}

int IMGL::CScore::Distance(int x, int y)
{
	return sqrt(SQRE(pX - x) + SQRE(pY - y));
}