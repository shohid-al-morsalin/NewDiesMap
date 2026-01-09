#include "stdafx.h"
#include "TopTen.h"

void IMGL::CTopTen::PushUp(size_t n)
{
	size_t sz = vScore.size();
	for (size_t i = n; i < sz - 1; i++)
	{
		vScore[i] = vScore[i + 1];
	}
	vScore.pop_back();
}

void IMGL::CTopTen::PushDown(size_t n)
{
	size_t sz = vScore.size();
	if (sz == 0) return;
	vScore.push_back(vScore[sz - 1]); // duplicate last item [11/27/2016 Yuen]
	for (size_t i = sz - 2; i >= n; i--)
	{
		vScore[i + 1] = vScore[i];
	}
}

int IMGL::CTopTen::partition(std::vector<CScore>& A, int p, int q)
{
	int x = A[p].pV;
	int i = p;
	int j;

	for (j = p + 1; j < q; j++)
	{
		if (A[j].pV <= x)
		{
			i = i + 1;
			std::swap(A[i], A[j]);
		}
	}

	std::swap(A[i], A[p]);
	return i;
}

void IMGL::CTopTen::quickSort(std::vector<CScore>& A, int p, int q)
{
	int r;
	if (p < q)
	{
		r = partition(A, p, q);
		quickSort(A, p, r);
		quickSort(A, r + 1, q);
	}
}

IMGL::CTopTen::CTopTen()
{
	Clear();
}

int IMGL::CTopTen::GetValue(int i)
{
	return vScore[i].pV;
}

int IMGL::CTopTen::GetX(int i)
{
	return vScore[i].pX;
}

int IMGL::CTopTen::GetY(int i)
{
	return vScore[i].pY;
}

IMGL::CScore * IMGL::CTopTen::GetCScore(UINT i)
{
	if (i < vScore.size())
	{
		return &vScore[i];
	}
	return NULL;
}

void IMGL::CTopTen::Sort()
{
	if (vScore.size() < 2) return;
	quickSort(vScore, 0, int(vScore.size()));
}

void IMGL::CTopTen::Merge(short dist)
{	// Merge score point if they are close together [8/5/2016 FSM]
	size_t sz = vScore.size();
	for (size_t i = 0; i < sz; i++)
	{
		if (i <= (vScore.size() - 1))
		{
			for (size_t j = i + 1; j < sz; j++)
			{
				if (j <= (vScore.size() - 1))
				{
					if (vScore[j].pV > 0)
					{
						if (vScore[i].Distance(vScore[j].pX, vScore[j].pY) < dist)
						{
							vScore[i].IncCount(&vScore[j]);
							PushUp(j);
							j--;
						}
					}
				}
			}
		}
	}
	quickSort(vScore, 0, int(vScore.size() - 1));
}

void IMGL::CTopTen::Insert(int x, int y, int v, int r)
{
	if (!v) return;
	size_t sz = vScore.size();
	if (sz == 0)
	{
		CScore p;
		p.SetR(r);
		p.Set(x, y, v);
		vScore.push_back(p);
		return;
	}

	for (size_t i = 0; i < sz; i++)
	{
		if (v > vScore[i].pV)
		{
			if (i < 10)
			{
				PushDown(i);
				CScore *q = &vScore[i];
				q->SetR(r);
				q->Set(x, y, v);
				return;
			}
		}
	}
	if (sz < 10)
	{
		CScore p;
		p.SetR(r);
		p.Set(x, y, v);
		vScore.push_back(p);
	}
}

void IMGL::CTopTen::Clear()
{
	vScore.clear();
}