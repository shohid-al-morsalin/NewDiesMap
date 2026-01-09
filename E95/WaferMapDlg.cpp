// WaferMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "WaferMapDlg.h"
#include "Pattern.h"
#include "Recipe413.h"
#include "MPoint.h"
#include "413App.h"
#include "ChuckMask.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWaferMapDlg dialog

CWaferMapDlg::CWaferMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWaferMapDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CWaferMapDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CWaferMapDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWaferMapDlg)
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWaferMapDlg, CDialog)
	//{{AFX_MSG_MAP(CWaferMapDlg)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWaferMapDlg message handlers

void CWaferMapDlg::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

BOOL CWaferMapDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CWaferMapDlg::OnRefresh() {
	m_cWaferMap.Redraw();
}

// long CWaferMapDlg::OnDeletepoint(WPARAM wP, LPARAM lP)
// {
// 	if (!m_cWaferMap.pRcp)
// 	{
// 		return 0;
// 	}
// 	CCoor *p = (CCoor *)lP;
//
// 	float Distance;
// 	CPattern *pPat = &m_cWaferMap.pRcp->Patt;
//
// 	CMPoint *mp = pPat->FindNearestMP(*p,Distance);
// 	if (mp && Distance < 2.0f)
// 	{
// 		pPat->DelPoint(mp->Co);
// 		m_cWaferMap.Redraw();
// 	}
// 	return 0;
// }

// long CWaferMapDlg::OnUpdatePoint(WPARAM wP, LPARAM lP)
// {
// 	CCoor *p = (CCoor *)lP;
// 	float Distance;
// 	CPattern *pPat = &m_cWaferMap.pRcp->Patt;
//
// 	CMPoint *mp = pPat->FindNearestMP(*p,Distance);
// 	if (mp && Distance < 2.0f)
// 	{
// 		mp->Co.x = p413App->Global.LocationX;
// 		mp->Co.y = p413App->Global.LocationY;
// 		m_cWaferMap.Redraw();
// 	}
// 	return 0;
// }

// long CWaferMapDlg::OnRelocateToBack(WPARAM wP, LPARAM lP)
// {
// 	CCoor *p = (CCoor *)lP;
//
// 	float Distance;
// 	CPattern *pPat = &m_cWaferMap.pRcp->Patt;
//
// 	CMPoint *mp = pPat->FindNearestMP(*p,Distance);
// 	if (mp && Distance < 2.0f)
// 	{
// 		CMPoint *pMPoint = new CMPoint;
// 		pMPoint->SetCoor(&mp->Co);
// 		pMPoint->MeSet = mp->MeSet;
// 		pMPoint->PatSet = mp->PatSet;
// 		pPat->MP.AddPoint(pMPoint);
// 		pPat->DelPoint(mp->Co);
// 		m_cWaferMap.Redraw();
// 	}
// 	return 0;
//}

// long CWaferMapDlg::OnEditpoint(WPARAM wP, LPARAM lP)
// {
// 	CCoor *p = (CCoor *)lP;
//
// 	float Distance;
// 	CPattern *pPat = &m_cWaferMap.pRcp->Patt;
//
// 	CMPoint *mp = pPat->FindNearestMP(*p,Distance);
// 	if (mp && Distance < 2.0f)
// 	{
// 		if (pPat->EditMP(mp->Co,mp->Co.x,mp->Co.y))
// 		{
// 			m_cWaferMap.Redraw();
// 		}
// 	}
// 	return 0;
// }

// long CWaferMapDlg::OnAddpoint(WPARAM wP, LPARAM lP)
// {
// 	CCoor *p = (CCoor *)lP;
//
// 	float Distance;
// 	CPattern *pPat = &m_cWaferMap.pRcp->Patt;
//
// 	CMPoint *mp = pPat->FindNearestMP(*p,Distance);
// 	if (mp && Distance < 0.005f)
// 	{
// 	}
// 	else
// 	{
// 		CMPoint *pMPoint = new CMPoint;
// 		pMPoint->SetCoor(p);
// 		pPat->MP.AddPoint(pMPoint);
// 		m_cWaferMap.Redraw();
// 	}
// 	return 0;
// }

// long CWaferMapDlg::OnAddalgnpoint(WPARAM wP, LPARAM lP)
// {
// 	CCoor *p = (CCoor *)lP;
//
// 	float Distance;
// 	CPattern *pPat = &m_cWaferMap.pRcp->Patt;
//
// 	CMPoint *mp = pPat->FindNearestMP(*p,Distance);
// 	if (mp && Distance < 0.005f)
// 	{
// 	}
// 	else
// 	{
// 		if(p413App->Global.GetChuckMask()->GetCenter(p->x,p->y))
// 		{
// 			CMPoint *pMPoint = new CMPoint;
// 			pMPoint->SetCoor(p);
// 			pPat->MP.AddPoint(pMPoint);
// 			m_cWaferMap.Redraw();
// 		}
// 	}
// 	return 0;
// }

// long CWaferMapDlg::OnGotopoint(WPARAM wP, LPARAM lP)
// {
// 	CCoor *p = (CCoor *)lP;
//
// 	float Distance;
// 	CPattern *pPat = &m_cWaferMap.pRcp->Patt;
//
// 	CMPoint *mp = pPat->FindNearestMP(*p,Distance);
// 	if (mp && Distance < 2.0f)
// 	{
// 		p413App->pMtr->GotoXY(mp->Co.x,mp->Co.y);
// 	}
// 	return 0;
// }