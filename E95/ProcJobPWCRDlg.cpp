// ProcJobPWCRDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"

#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "SYS/CUMMsg.h"

#include "ProcJobPWCRDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcJobPWCRDlg dialog

CProcJobPWCRDlg::CProcJobPWCRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcJobPWCRDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobPWCRDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CProcJobPWCRDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobPWCRDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJobPWCRDlg, CDialog)
	//{{AFX_MSG_MAP(CProcJobPWCRDlg)
	ON_BN_CLICKED(IDC_CANCELCARRIER, OnCancelcarrier)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobPWCRDlg message handlers

void CProcJobPWCRDlg::OnCancelcarrier() {
	if (AfxMessageBox("Are you sure?", MB_YESNO) == IDYES) {
		if (m_Parent) {
			UpdateData();
			pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID = m_CarrierID;
			ShowWindow(SW_HIDE);
			m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_TO_PROCJOBPWC1);
		}
	}
}