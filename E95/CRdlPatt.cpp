#include "stdafx.h"
// #include "MPoint.h"
// #include <math.h>
#include "DefineCircular.h"

#include "CRdlPatt.h"

class CLine;

CRdlPatt::CRdlPatt()/* : CPattern(_T("Radial Pattern"))*/
{
	pPatt = NULL;
}

CRdlPatt::~CRdlPatt() {
	ClearList();
}

CRdlPatt& CRdlPatt::operator = (CRdlPatt& RdlPatt) {
	//	CPattern::operator=(RdlPatt);

	ClearList();
	for (int i = 0; i < RdlPatt.lines.GetCount(); i++) {
		CLine* l = new CLine;
		if (l) {
			*l = *RdlPatt.GetLine((short)i);
			lines.AddTail(l);
		}
	}

	return *this;
}

CLine* CRdlPatt::GetLine(int n) {
	return (CLine*)lines.GetAt(lines.FindIndex(n));
}

int CRdlPatt::GetNLine() {
	return lines.GetCount();
}

void CRdlPatt::Serialize(CArchive& ar) {
	short magic;
	if (ar.IsStoring()) {
		magic = 2;
		ar << magic;
		short n = (short)lines.GetCount();
		ar << n;
		if (n > 0) {
			POSITION pos = lines.GetHeadPosition();
			while (pos) {
				CLine* p = (CLine*)lines.GetNext(pos);
				if (p) p->Serialize(ar);
			}
		}
		//        dmy.MP.Serialize(ar);
	}
	else {
		ClearList();
		ar >> magic;
		short n;
		ar >> n;
		for (int i = 0; i < n; i++) {
			CLine* p = new CLine;
			if (p) {
				p->Serialize(ar);
				lines.AddTail(p);
			}
		}
		if (magic < 2) {
			if (pPatt) pPatt->MP.Serialize(ar);
		}
	}
}

void CRdlPatt::ClearList() {
	POSITION pos = lines.GetHeadPosition();
	while (pos) {
		CLine* p = (CLine*)lines.GetNext(pos);
		if (p) delete p;
	}
	if (lines.GetCount()) {
		lines.RemoveAll();
	}

	if (pPatt) pPatt->MP.Clear();
}

void CRdlPatt::SetData(CPtrList& lns) {
	ClearList();
	POSITION pos = lns.GetHeadPosition();
	while (pos) {
		CLine* p = (CLine*)lns.GetNext(pos);
		if (p) {
			CLine* p1 = new CLine;
			if (!p1) return;
			*p1 = *p;
			lines.AddTail(p1);
		}
	}
}

BOOL CRdlPatt::SetUp(CWnd* pWnd) {
	CDefineCircular dlg;

	dlg.DoModal();

	return TRUE;
}

#define PIE180     0.0174532925199432957692369076849

BOOL CRdlPatt::Generate(float fDiameter) {
	if (!pPatt) {
		return FALSE;
	}
	pPatt->MP.Clear();

	short idx = 1;
	int n = GetNLine();
	float r = fDiameter / 2.0f;

	BOOL bOrigin = FALSE;
	for (int a = 0; a < n; a++) {
		CLine* l = GetLine(a);
		if (!l) break;
		if (l->npoint < 2) continue;
		float sf = fDiameter / float(l->npoint - 1);
		float angle = float((l->angle) * PIE180);
		for (int i = 0; i < l->npoint; i++) {
			float xx = float((i * sf - r) * cos(angle));
			float yy = float((i * sf - r) * sin(angle));
			if ((xx == 0) && (yy == 0)) {
				if (bOrigin) continue;
				bOrigin = TRUE;
			}
			pPatt->AddMPoint(idx, xx, yy, 0/*, 0, 0*/);
			idx++;
		}
	}
	return (0 < idx);
}

BOOL CRdlPatt::Generate2(int NX, int NY, float X0, float Y0, float sizeX, float sizeY, float fDiameter) {
	return TRUE;
}

void CRdlPatt::DrawPattern(CDC* pDC, const CRect& rect, float) {
	CPen pen, * ppen;
	pen.CreatePen(PS_SOLID, 1, RGB(32, 200, 32));
	ppen = (CPen*)pDC->SelectObject(&pen);

	CPoint cpt = rect.CenterPoint();
	int n = GetNLine();
	float sz = rect.Width() / 2.0f;
	for (int a = 0; a < n; a++) {
		CLine* l = GetLine(a);
		if (!l) break;

		float angle = float((l->angle) * PIE180);
		pDC->MoveTo(cpt.x + (int)(sz * cos(angle)), cpt.y - (int)(sz * sin(angle)));
		pDC->LineTo(cpt.x - (int)(sz * cos(angle)), cpt.y + (int)(sz * sin(angle)));
	}
	if (ppen) pDC->SelectObject(ppen);
}