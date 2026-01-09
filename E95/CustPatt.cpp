#include "stdafx.h"
#include "CustPatt.h"

CCustPatt::CCustPatt()/* : CPattern(_T("Custom Pattern"))*/
{
	m_XCenter = 0.0f;
	m_YCenter = 0.0f;
	m_XPitch = 0.0f;
	m_YPitch = 0.0f;
	pPatt = NULL;
}

CCustPatt::~CCustPatt() {}

CCustPatt& CCustPatt::operator = (CCustPatt& CustPatt) {
	//	CPattern::operator =(CustPatt);
	m_XCenter = CustPatt.m_XCenter;
	m_YCenter = CustPatt.m_YCenter;
	m_XPitch = CustPatt.m_XPitch;
	m_YPitch = CustPatt.m_YPitch;
	return *this;
}

void CCustPatt::Serialize(CArchive& ar) {
	short magic;
	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		//dmy.MP.Serialize(ar);
	}
	else {
		ar >> magic;
		if (magic < 1) {
			if (pPatt) pPatt->MP.Serialize(ar);
		}
	}
}

BOOL CCustPatt::SetUp(CWnd*) {
	return FALSE;
}

BOOL CCustPatt::Generate(float) {
	return FALSE;
}

BOOL CCustPatt::Generate2(int, int, float, float, float, float, float) {
	return FALSE;
}

void CCustPatt::DrawPattern(CDC*, const CRect&, float) {}

void CCustPatt::ClearList() {
	m_XCenter = 0.0f;
	m_YCenter = 0.0f;
	m_XPitch = 0.0f;
	m_YPitch = 0.0f;
}