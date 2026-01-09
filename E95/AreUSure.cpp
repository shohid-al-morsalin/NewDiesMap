#include "stdafx.h"

#include "e95.h"

#include "AreUSure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAreUSure::CAreUSure(CWnd* pParent /*=NULL*/)
	: CDialog(CAreUSure::IDD, pParent) {
	m_Message = _T("");

	bRet = FALSE;
}

void CAreUSure::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TEXT, m_Message);
}

BEGIN_MESSAGE_MAP(CAreUSure, CDialog)
END_MESSAGE_MAP()

void CAreUSure::OnOK() {
	bRet = TRUE;
	CDialog::OnOK();
}