#include "stdafx.h"
#include "e95.h"
#include "DefineRadial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDefineRadial::CDefineRadial(CWnd* pParent /*=NULL*/)
	: CDialog(CDefineRadial::IDD, pParent) {
	//{{AFX_DATAINIT(CDefineRadial)
	m_NoLines = 8;
	m_PointPerDiameter = 7;
	//}}AFX_DATA_INIT
}

void CDefineRadial::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefineRadial)
	DDX_Text(pDX, IDC_NOLINES, m_NoLines);
	DDX_Text(pDX, IDC_POINTPERDIAMETER, m_PointPerDiameter);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDefineRadial, CDialog)
	//{{AFX_MSG_MAP(CDefineRadial)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefineRadial message handlers