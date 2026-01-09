// RunRecipe.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "RunRecipe.h"
#include "SetupProp3Dlg.h"
#include "413App.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_RUNRECIPE	9999
#define IDT_ISDONE		9998

/////////////////////////////////////////////////////////////////////////////
// CRunRecipe dialog

CRunRecipe::CRunRecipe(CWnd* pParent /*=NULL*/)
	: CDialog(CRunRecipe::IDD, pParent) {
	//{{AFX_DATA_INIT(CRunRecipe)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	pExe = NULL;
	pThread = NULL;
}

void CRunRecipe::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRunRecipe)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRunRecipe, CDialog)
	//{{AFX_MSG_MAP(CRunRecipe)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRunRecipe message handlers

void CRunRecipe::OnStop() {
	if (!pExe) {
		return;
	}
	p413App->Global.bStop = TRUE;
	while (pExe->bDoMeasurement) {
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (!AfxGetThread()->PumpMessage()) {
				break;
			}
		}
	}
	OnOK();
}

BOOL CRunRecipe::OnInitDialog() {
	CDialog::OnInitDialog();

	SetTimer(IDT_RUNRECIPE, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CRunRecipe::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_RUNRECIPE) {
		KillTimer(IDT_RUNRECIPE);
		if (!pExe) {
			return;
		}
		if (p413App->Global.bStop) {
			p413App->Global.bStop = FALSE;
			pThread = AfxBeginThread(CSetupProp3Dlg::ExecuteRecipe, pExe);
			SetTimer(IDT_ISDONE, 1000, NULL);
		}
	}
	if (nIDEvent == IDT_ISDONE) {
		if (IsDone()) {
			KillTimer(IDT_ISDONE);
			OnOK();
		}
	}
}

BOOL CRunRecipe::IsDone() {
	if (pExe->bDoMeasurement) {
		// 		MSG msg;
		// 		while (::PeekMessage(& msg, NULL, 0, 0, PM_NOREMOVE))
		// 		{
		// 			if (!AfxGetThread()->PumpMessage())
		// 			{
		// 				break;
		// 			}
		// 		}
		return FALSE;
	}
	return TRUE;
}