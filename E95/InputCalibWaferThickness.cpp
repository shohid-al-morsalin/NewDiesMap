// InputCalibWaferThickness.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "InputCalibWaferThickness.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputCalibWaferThickness dialog

CInputCalibWaferThickness::CInputCalibWaferThickness(CWnd* pParent /*=NULL*/)
	: CDialog(CInputCalibWaferThickness::IDD, pParent) {
	//{{AFX_DATA_INIT(CInputCalibWaferThickness)
	m_CalibThickness = 0.0f;
	//}}AFX_DATA_INIT
}

void CInputCalibWaferThickness::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInputCalibWaferThickness)
	DDX_Text(pDX, IDC_CALIBTHICK, m_CalibThickness);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInputCalibWaferThickness, CDialog)
	//{{AFX_MSG_MAP(CInputCalibWaferThickness)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputCalibWaferThickness message handlers