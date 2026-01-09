// WaferMapSetting.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "WaferMapSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaferMapSetting dialog

CWaferMapSetting::CWaferMapSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CWaferMapSetting::IDD, pParent) {
	//{{AFX_DATA_INIT(CWaferMapSetting)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CWaferMapSetting::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaferMapSetting)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWaferMapSetting, CDialog)
	//{{AFX_MSG_MAP(CWaferMapSetting)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaferMapSetting message handlers