// ViewCassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "XTabCtrl.h"
#include "InfoPanelView.h"
#include "ViewCassDlg.h"
#include "..\SYS\CUMMsg.h"

#include "..\SRC\DOSUtil.h"

#include "GlobalUIContainer.h"
// #include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewCassDlg dialog

CViewCassDlg::CViewCassDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewCassDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CViewCassDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Parent = NULL;
}

void CViewCassDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewCassDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CViewCassDlg, CDialog)
	//{{AFX_MSG_MAP(CViewCassDlg)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewCassDlg message handlers

void CViewCassDlg::OnLoad() {
	if (m_Parent) {
		CFileDialog dlg(TRUE, "C2C", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "Data file|*.c2c|");
		if (dlg.DoModal()) {
			// use standard dialog to get filename for now. Implement tree view later T.W.
			m_Parent->m_FileToLoad.Format("%s", dlg.GetFileName());  // pass filename back to parent
			m_Parent->m_DirToLoad.Format("%s", dlg.GetPathName());
			DosUtil.ExtractPath(m_Parent->m_DirToLoad);
			ShowWindow(SW_HIDE);
			m_Parent->PostMessage(UM_LOADFILE, 0, 0);
		}
	}
}

LRESULT CViewCassDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("View\n%s", (char*)lP);
	if (pUDev) {
		pUDev->SetCurrentView(msg);
	}
	// 	else
	// 	{
	// 		ASSERT(0);
	// 	}
	return 0;
}

void CViewCassDlg::PostNcDestroy() {
	CDialog::PostNcDestroy();
	delete this;
}