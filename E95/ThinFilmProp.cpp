// ThinFilmProp.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "ThinFilmProp.h"
#include "Recipe413.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CThinFilmProp dialog

CThinFilmProp::CThinFilmProp(CWnd* pParent /*=NULL*/)
	: CDialog(CThinFilmProp::IDD, pParent) {
	//{{AFX_DATA_INIT(CThinFilmProp)
	m_Boxcar = 0;
	m_RefractiveIndex = 0.0f;
	m_IntegrationTime = 0;
	//}}AFX_DATA_INIT
	pRcp = NULL;
}

void CThinFilmProp::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CThinFilmProp)
	DDX_Text(pDX, IDC_BOXCAR_TF, m_Boxcar);
	DDX_Text(pDX, IDC_REFRACTIVEINDEX_TF, m_RefractiveIndex);
	DDX_Text(pDX, IDC_FILMMEASTIME_TF, m_IntegrationTime);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CThinFilmProp, CDialog)
	//{{AFX_MSG_MAP(CThinFilmProp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CThinFilmProp message handlers