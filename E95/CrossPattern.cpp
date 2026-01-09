#include "stdafx.h"
#include "resource.h"

#include "413App.h"
#include "ChuckMask.h"
#include "MFC/KeyPad.h"
// #include "SRC/BMake.h"

#include "CrossPattern.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef AT_INTEL
#define X_MAX 82.0f
#define Y_MAX 72.0f
#else
#define X_MAX 130.0f
#define Y_MAX 130.0f
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrossPatternDlg class

CCrossPattern::CCrossPattern()/* : CPattern(_T("Cross"))*/
{
	m_fXCenter = 0;
	m_fYCenter = 0;
	m_fXPitch = 0;
	m_fYPitch = 0;
	m_fXMax = X_MAX;
	m_fYMax = Y_MAX;

	pPatt = NULL;
}

CCrossPattern::~CCrossPattern() {}

CCrossPattern& CCrossPattern::operator=(CCrossPattern& CrossPattern) {
	//	CPattern::operator=(CrossPattern);
	m_fXCenter = CrossPattern.m_fXCenter;
	m_fYCenter = CrossPattern.m_fYCenter;

	m_fXPitch = CrossPattern.m_fXPitch;
	m_fYPitch = CrossPattern.m_fYPitch;

	m_fXMax = CrossPattern.m_fXMax;
	m_fYMax = CrossPattern.m_fYMax;

	return *this;
}

void CCrossPattern::Serialize(CArchive& ar) {
	short magic;
	if (ar.IsStoring()) {
		magic = 2;
		ar << magic;
		ar << m_fXCenter;
		ar << m_fYCenter;
		ar << m_fXPitch;
		ar << m_fYPitch;
		ar << m_fXMax;
		ar << m_fYMax;
		//		dmy.MP.Serialize(ar);
	}
	else {
		ar >> magic;
		ar >> m_fXCenter;
		ar >> m_fYCenter;
		ar >> m_fXPitch;
		ar >> m_fYPitch;
		ar >> m_fXMax;
		ar >> m_fYMax;
		if (magic < 2) {
			if (pPatt) pPatt->MP.Serialize(ar);
		}
	}
}

BOOL CCrossPattern::SetUp(CWnd* pWnd) {
	CCrossPatternDlg dlg(pWnd);

	dlg.m_XCenter = m_fXCenter;
	dlg.m_YCenter = m_fYCenter;
	dlg.m_XPitch = m_fXPitch;
	dlg.m_YPitch = m_fYPitch;
	dlg.m_fXMax = m_fXMax;
	dlg.m_fYMax = m_fYMax;
	if (IDOK != dlg.DoModal()) return FALSE;

	m_fXPitch = dlg.m_XPitch;
	m_fYPitch = dlg.m_YPitch;
	m_fXCenter = dlg.m_XCenter;
	m_fYCenter = dlg.m_YCenter;
	m_fXMax = dlg.m_fXMax;
	m_fYMax = dlg.m_fYMax;
	return TRUE;
}

BOOL CCrossPattern::Generate(float) {
	if (!pPatt) {
		return FALSE;
	}
	pPatt->MP.Clear();

	if ((m_fXPitch < 1.f) || (m_fYPitch < 1.)) return FALSE;
	// Code changed to start the grid always at the center
	// Generate center point
	// Generate points to the
	// Positive points in + direction
	short nIndex = 1;
	float fY = m_fYCenter;
	float fX;
	for (fX = m_fXCenter; fX < m_fXMax; fX += m_fXPitch) {
		pPatt->AddMPoint(nIndex, fX, fY, 0/*, 0, 0*/);
		nIndex++;
	}
	for (fX = m_fXCenter - m_fXPitch; fX > -m_fXMax; fX -= m_fXPitch) {
		pPatt->AddMPoint(nIndex, fX, fY, 0/*, 0, 0*/);
		nIndex++;
	}
	fX = m_fXCenter;
	for (fY = m_fYCenter + m_fYPitch; fY < m_fYMax; fY += m_fYPitch) {
		pPatt->AddMPoint(nIndex, fX, fY, 0/*, 0, 0*/);
		nIndex++;
	}

	for (fY = m_fYCenter - m_fYPitch; fY > -m_fYMax; fY -= m_fYPitch) {
		pPatt->AddMPoint(nIndex, fX, fY, 0/*, 0, 0*/);
		nIndex++;
	}
	return (0 < nIndex);
}

BOOL CCrossPattern::Generate2(int, int, float, float, float, float, float) {
	return TRUE;
}

void CCrossPattern::DrawPattern(CDC* pDC, const CRect& rect, float fRatio) {
	CPen* pOldPen = pDC->SelectObject(new CPen(PS_SOLID, 1, RGB(32, 200, 32)));

	CPoint ptCenter = rect.CenterPoint();

	int nX = (int)(m_fXMax * fRatio);
	int nY = (int)(m_fYMax * fRatio);
	ptCenter.x += (int)(m_fXCenter * fRatio);
	ptCenter.y -= (int)(m_fYCenter * fRatio);

	pDC->MoveTo(ptCenter.x - nX, ptCenter.y);
	pDC->LineTo(ptCenter.x + nX, ptCenter.y);

	pDC->MoveTo(ptCenter.x, ptCenter.y - nY);
	pDC->LineTo(ptCenter.x, ptCenter.y + nY);
	delete pDC->SelectObject(pOldPen);
}

void CCrossPattern::Conv(char** msg) {
	CMake::Conv(m_fXCenter, msg);
	CMake::Conv(m_fYCenter, msg);
	CMake::Conv(m_fXPitch, msg);
	CMake::Conv(m_fYPitch, msg);
	CMake::Conv(m_fXMax, msg);
	CMake::Conv(m_fYMax, msg);

	//	CPattern::Conv(msg);
}

long CCrossPattern::Binary(char** msg) {
	long sz = 0;

	sz += CMake::Binary(m_fXCenter, msg);
	sz += CMake::Binary(m_fYCenter, msg);
	sz += CMake::Binary(m_fXPitch, msg);
	sz += CMake::Binary(m_fYPitch, msg);
	sz += CMake::Binary(m_fXMax, msg);
	sz += CMake::Binary(m_fYMax, msg);

	//	sz += CPattern::Binary(msg);

	return sz;
}

long CCrossPattern::GetLength() {
	long sz = 0;

	sz += CMake::GetLength(m_fXCenter);
	sz += CMake::GetLength(m_fYCenter);
	sz += CMake::GetLength(m_fXPitch);
	sz += CMake::GetLength(m_fYPitch);
	sz += CMake::GetLength(m_fXMax);
	sz += CMake::GetLength(m_fYMax);

	//	sz += CPattern::GetLength();

	return sz;
}

/////////////////////////////////////////////////////////////////////////////
// CCrossPatternDlg dialog

CCrossPatternDlg::CCrossPatternDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrossPatternDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CCrossPatternDlg)
	m_XCenter = 0.0f;
	m_XPitch = 1.0f;
	m_YPitch = 1.0f;
	m_YCenter = 0.0f;
	m_fXMax = 0.0f;
	m_fYMax = 0.0f;
	//}}AFX_DATA_INIT
}

void CCrossPatternDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrossPatternDlg)
	DDX_Text(pDX, IDC_CROSS_XCENTER, m_XCenter);
	DDV_MinMaxFloat(pDX, m_XCenter, -20.f, 20.f);
	DDX_Text(pDX, IDC_CROSS_YCENTER, m_YCenter);
	DDV_MinMaxFloat(pDX, m_YCenter, -20.f, 20.f);
	DDX_Text(pDX, IDC_CROSS_XPITCH, m_XPitch);
	DDX_Text(pDX, IDC_CROSS_YPITCH, m_YPitch);
	DDX_Text(pDX, IDC_CROSS_XMAX, m_fXMax);
	DDX_Text(pDX, IDC_CROSS_YMAX, m_fYMax);
	//}}AFX_DATA_MAP
// 	DDV_MinMaxFloat(pDX, m_XPitch, 1.f, m_fXMax);
// 	DDV_MinMaxFloat(pDX, m_YPitch, 1.f, m_fYMax);
// 	DDV_MinMaxFloat(pDX, m_fXMax, m_XPitch, X_MAX);
// 	DDV_MinMaxFloat(pDX, m_fYMax, m_YPitch, Y_MAX);
}

BEGIN_MESSAGE_MAP(CCrossPatternDlg, CDialog)
	//{{AFX_MSG_MAP(CCrossPatternDlg)
	ON_BN_CLICKED(IDC_USE_HOLE_PITCH, OnUseHolePitch)
	ON_BN_CLICKED(IDC_CENTERY_BUT, OnCenteryBut)
	ON_BN_CLICKED(IDC_MAXY_BUT, OnMaxyBut)
	ON_BN_CLICKED(IDC_XCENTER_BUT, OnXcenterBut)
	ON_BN_CLICKED(IDC_XMAX_BUT, OnXmaxBut)
	ON_BN_CLICKED(IDC_XPITCH_BUT, OnXpitchBut)
	ON_BN_CLICKED(IDC_YPITCH_BUT, OnYpitchBut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrossPatternDlg message handlers

void CCrossPatternDlg::OnUseHolePitch() {
	CChuckMask* pChuckMask = p413App->Global.GetChuckMask();
	if (pChuckMask) {
		m_XPitch = pChuckMask->GetXPitch();
		m_YPitch = pChuckMask->GetYPitch();
		UpdateData(FALSE);
	}
}

#define ON_BUTTON_MACRO(fX)	\
	UpdateData();\
	float fTemp = fX;\
	if (CKeyPad::Edit(fTemp))\
	{\
		fX = fTemp;\
		UpdateData(FALSE);\
	}

void CCrossPatternDlg::OnCenteryBut() {
	ON_BUTTON_MACRO(m_YCenter)
}

void CCrossPatternDlg::OnMaxyBut() {
	ON_BUTTON_MACRO(m_fYMax)
}

void CCrossPatternDlg::OnXcenterBut() {
	ON_BUTTON_MACRO(m_XCenter)
}

void CCrossPatternDlg::OnXmaxBut() {
	ON_BUTTON_MACRO(m_fXMax)
}

void CCrossPatternDlg::OnXpitchBut() {
	ON_BUTTON_MACRO(m_XPitch)
}

void CCrossPatternDlg::OnYpitchBut() {
	ON_BUTTON_MACRO(m_YPitch)
}

BOOL CCrossPatternDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	if (m_XPitch < 5.f) m_XPitch = 5.f;
	if (m_YPitch < 5.f) m_YPitch = 5.f;

	UpdateData(FALSE);

	CChuckMask* pChuckMask = p413App->Global.GetChuckMask();
	GetDlgItem(IDC_USE_HOLE_PITCH)->EnableWindow(pChuckMask != NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}