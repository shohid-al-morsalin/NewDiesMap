#include "stdafx.h"
#include "BlobCount.h"

void IMGL::CBlob::ClearQue()
{
	while (!que.empty()) que.pop();
}

void IMGL::CBlob::Set(CPackedDIB * p)
{
	pDiB = p;
}

IMGL::CBlob::CBlob()
{
	pDiB = NULL;
}

IMGL::CBlob::~CBlob()
{
}

template<typename T>
void IMGL::CBlob::push_in_queue(int x, int y, short counter)
{
	if ((x < 0) || (x >= wd) || (y < 0) || (y >= ht)) return;
	T *p = (T *)pDiB->GetPixelAddress(x, y);
	if (p->CanMark())
	{
		p->Mark(counter);
		imgPoint Pt(x, y, counter);
		que.push(Pt);
	}
}

template<typename T>
int IMGL::CBlob::Count()
{
	ClearQue();
	short counter = 0;
	wd = pDiB->GetWidth();
	ht = pDiB->GetHeight();
	for (int y = 1; y < ht - 1; y++)
	{
		T *p = (T*)pDiB->GetPixelAddress(0, y);
		for (int x = 1; x < wd - 1; x++)
		{
			if (p->CanMark())
			{
				p->Mark(++counter);
				imgPoint Pt(x, y, counter);
				que.push(Pt);

				while (!que.empty())
				{
					imgPoint tmp = que.front();
					que.pop();
					push_in_queue<T>(tmp.x - 1, tmp.y, tmp.count);
					push_in_queue<T>(tmp.x - 1, tmp.y - 1, tmp.count);
					push_in_queue<T>(tmp.x - 1, tmp.y + 1, tmp.count);
					push_in_queue<T>(tmp.x + 1, tmp.y, tmp.count);
					push_in_queue<T>(tmp.x + 1, tmp.y - 1, tmp.count);
					push_in_queue<T>(tmp.x + 1, tmp.y + 1, tmp.count);
					push_in_queue<T>(tmp.x, tmp.y - 1, tmp.count);
					push_in_queue<T>(tmp.x, tmp.y + 1, tmp.count);
				}
			}
			p++;
		}
	}
	return 0;
}

// void push_in_queue(int x, int y, queue<struct point> *q, char mat[MAX][MAX], int nrow, int ncol)
// {
// 	if (x < 0 || x >= nrow || y < 0 || y >= ncol)
// 		return;
// 	struct point pnt;
// 	if (mat[x][y] == 'X' && !(USED & mat[x][y]))
// 	{
// 		mat[x][y] = USED | mat[x][y];
// 		pnt.x = x;
// 		pnt.y = y;
// 		q->push(pnt);
// 	}
// }

// while (!que.empty())
// {
// 	tmp = que.front();
// 	que.pop();
// 	push_in_queue(tmp.x - 1, tmp.y, &que, mat, nrow, ncol);
// 	push_in_queue(tmp.x - 1, tmp.y - 1, &que, mat, nrow, ncol);
// 	push_in_queue(tmp.x - 1, tmp.y + 1, &que, mat, nrow, ncol);
// 	push_in_queue(tmp.x + 1, tmp.y, &que, mat, nrow, ncol);
// 	push_in_queue(tmp.x + 1, tmp.y - 1, &que, mat, nrow, ncol);
// 	push_in_queue(tmp.x + 1, tmp.y + 1, &que, mat, nrow, ncol);
// 	push_in_queue(tmp.x, tmp.y - 1, &que, mat, nrow, ncol);
// 	push_in_queue(tmp.x, tmp.y + 1, &que, mat, nrow, ncol);
// }

int IMGL::CBlob::Count(CPackedDIB * p)
{
	pDiB = p;
	switch (pDiB->GetBYPP())
	{
	case 1:
		Count<sBINR>();
		break;
	case 3:
		Count<sBGR>();
		break;
	case 4:
		Count<sBGRA>();
		break;
	}
	return 0;
}