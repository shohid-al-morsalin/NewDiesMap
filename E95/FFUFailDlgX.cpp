// FFUFailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "FFUFailDlg.h"
#include "..\LTW\LTWMgr.h"

#include "413App.h"
#include "GlobalDeviceContainer.h"
#include "..\413\MotorS2.h"
#include "GlobalUIContainer.h"

#define IDT_CHECKFFU	9999
#define IDT_CONFIRMFFU	9998

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFFUFailDlg dialog

float CFFUFailDlg::FFUPressNow = 0;

CFFUFailDlg::CFFUFailDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFFUFailDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFFUFailDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFFUFailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFFUFailDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFFUFailDlg, CDialog)
	//{{AFX_MSG_MAP(CFFUFailDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFFUFailDlg message handlers

void CFFUFailDlg::OnOK() 
{
	bOnScreen = FALSE;
	if (pGDev && pGDev->pLTMgr)
	{
		pGDev->pLTMgr->SetGenAlarm(FALSE);
	}
// 	CDialog::OnOK();
}

BOOL CFFUFailDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	bOnScreen = FALSE;

	ChkDelay = 60000;
	CfnDelay = 2000;

	if (p413App->pMtr)
	{
		if (p413App->pMtr->IsSimu())
		{
			ChkDelay = 45000;
			CfnDelay = 2000;
		}
	}

	SetTimer(IDT_CHECKFFU,ChkDelay,NULL);

	this->ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFFUFailDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
//	delete this;
}

void CFFUFailDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == IDT_CHECKFFU)
	{
		if (p413App->IsOperating())
		{
			// Do not check ffu if tool is operating [4/25/2013 Administrator]
			return ;
		}
		if (p413App && p413App->pMtr)
		{
			if (!pUDev->CanStart(99))
			{
				return ;
			}
			KillTimer(IDT_CHECKFFU);
			FFUPressNow = p413App->pMtr->GetFFUPressure();
			if (FFUPressNow <= p413App->Global.FFUPressure)
			{
				SetTimer(IDT_CONFIRMFFU,CfnDelay,NULL);
			}
			else
			{
				SetTimer(IDT_CHECKFFU,ChkDelay,NULL);
			}
		}
	}
	if (nIDEvent == IDT_CONFIRMFFU)
	{
		if (p413App->IsOperating())
		{
			// Do not check FFU if tool is operating [4/25/2013 Administrator]
			return ;
		}
		KillTimer(IDT_CONFIRMFFU);
		if (pUDev->CanStart(99))
		{
			FFUPressNow = p413App->pMtr->GetFFUPressure();
		}
		if (FFUPressNow <= p413App->Global.FFUPressure)
		{
			if (!bOnScreen && !p413App->pMtr->IsSimu())
			{
				//ShowWindow(SW_SHOW);
				//bOnScreen = TRUE;
				if (pGDev && pGDev->pLTMgr)
				{
					//pGDev->pLTMgr->SetGenAlarm(TRUE);
					//pGDev->SetAlarm(CAlrmList::FANFILTERUNITFAIL);
				}
			}
		}
		SetTimer(IDT_CHECKFFU,ChkDelay,NULL);
	}
}

BOOL CFFUFailDlg::DestroyWindow() 
{
	return CDialog::DestroyWindow();
}

void CFFUFailDlg::Close()
{
	CDialog::OnOK();
}
