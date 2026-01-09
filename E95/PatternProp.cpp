// PatternProp.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "PatternProp.h"
#include "Recipe413.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatternProp dialog

CPatternProp::CPatternProp(CWnd* pParent /*=NULL*/)
	: CDialog(CPatternProp::IDD, pParent) {
	//{{AFX_DATA_INIT(CPatternProp)
	m_DI = 0;
	m_DT = 0;
	m_Size = 0;
	//}}AFX_DATA_INIT
	pRcp = NULL;
}

void CPatternProp::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatternProp)
	DDX_Control(pDX, IDC_MASK2, m_cMask2);
	DDX_Control(pDX, IDC_MASK1, m_cMask1);
	DDX_Text(pDX, IDC_MATCHDI, m_DI);
	DDX_Text(pDX, IDC_MATCHDT, m_DT);
	DDX_Text(pDX, IDC_TEMPLATESIZE, m_Size);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPatternProp, CDialog)
	//{{AFX_MSG_MAP(CPatternProp)
	ON_BN_CLICKED(IDC_MAKETEMPLATE1, OnMaketemplate1)
	ON_BN_CLICKED(IDC_MAKETEMPLATE2, OnMaketemplate2)
	ON_BN_CLICKED(IDC_CLEARTEMPLATE, OnCleartemplate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPatternProp message handlers

void CPatternProp::OnMaketemplate1() {}

void CPatternProp::OnMaketemplate2() {}

void CPatternProp::OnCleartemplate() {}