#include "stdafx.h"
#include "e95.h"
#include "HelpTrim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHelpTrim::CHelpTrim(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpTrim::IDD, pParent) {}

void CHelpTrim::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHelpTrim, CDialog)
END_MESSAGE_MAP()