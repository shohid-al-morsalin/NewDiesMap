#include "stdafx.h"

#include "e95.h"

#include "AttachDelete.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAttachDelete::CAttachDelete(CWnd* pParent /*=NULL*/)
	: CDialog(CAttachDelete::IDD, pParent) {
	option = 0;
}

void CAttachDelete::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAttachDelete, CDialog)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
END_MESSAGE_MAP()

void CAttachDelete::OnDelete() {
	option = 1;
	OnCancel();
}