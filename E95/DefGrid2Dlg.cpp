#include "stdafx.h"
#include "resource.h"
#include "DefGrid2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDefGrid2Dlg::CDefGrid2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDefGrid2Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CDefGrid2Dlg)
	m_fX0 = 0.0f;
	m_fY0 = 0.0f;
	m_NX = 5;
	m_NY = 5;
	m_fSx = 2.0f;
	m_fSy = 2.0f;
	m_DeletePrev = FALSE;
	m_bIgnore = TRUE;
	//}}AFX_DATA_INIT

	bClearData = FALSE;
}

void CDefGrid2Dlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefGrid2Dlg)
	DDX_Text(pDX, IDC_CX, m_fX0);
	DDV_MinMaxFloat(pDX, m_fX0, -300.f, 300.f);
	DDX_Text(pDX, IDC_CY, m_fY0);
	DDV_MinMaxFloat(pDX, m_fY0, -300.f, 300.f);
	DDX_Text(pDX, IDC_NX, m_NX);
	DDV_MinMaxInt(pDX, m_NX, 1, 301);
	DDX_Text(pDX, IDC_NY, m_NY);
	DDV_MinMaxInt(pDX, m_NY, 1, 301);
	DDX_Text(pDX, IDC_SX, m_fSx);
	DDV_MinMaxFloat(pDX, m_fSx, 0.f, 300.f);
	DDX_Text(pDX, IDC_SY, m_fSy);
	DDV_MinMaxFloat(pDX, m_fSy, 0.f, 300.f);
	DDX_Check(pDX, IDC_CHECK_DELETE_PREVIOUS, m_DeletePrev);
	DDX_Check(pDX, IDC_IGNOREPOINT, m_bIgnore);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDefGrid2Dlg, CDialog)
	//{{AFX_MSG_MAP(CDefGrid2Dlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefGrid2Dlg message handlers