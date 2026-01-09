#include "stdafx.h"
#include "WMap2DDlg.h"

// WMap3DDlg.cpp : implementation file
//
#include "E95.h"
#include "afxdialogex.h"

#include "E95/Recipe413.h"
#include "E95/Glut2DMapView.h"
#include <atlimage.h> //20251210_Mohir


IMPLEMENT_DYNAMIC(CWMap2DDlg, CDialog)

void CWMap2DDlg::SetParam(CRecipe413* recipe, int dataIdx) {
	pRcp = recipe; nDataIdx = dataIdx;
}

CWMap2DDlg::CWMap2DDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_WAFERMAP2D_DLG, pParent)
{
}

CWMap2DDlg::~CWMap2DDlg()
{
}


void CWMap2DDlg::ExportToImage(CString exportPath) {
	while (!bReadyExport) {
		Yld(100);
	}
	Yld(300);
	//pView->ExportToCImage(exportPath);
	return;
}



void CWMap2DDlg::Yld(ULONGLONG dly) {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < dly) {
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CWMap2DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWMap2DDlg, CDialog)
END_MESSAGE_MAP()

// CWMap2DDlg message handlers

BOOL CWMap2DDlg::OnInitDialog()
{
	if (pRcp == nullptr) return FALSE;

	CDialog::OnInitDialog();

	CWnd* pWnd = GetDlgItem(IDC_2DPOP);
	CRect RectTargetCtrl;
	pWnd->GetWindowRect(RectTargetCtrl);
	this->ScreenToClient(RectTargetCtrl);
	pView = (CGlut2DMapView*)RUNTIME_CLASS(CGlut2DMapView)->CreateObject();
	if (NULL == pView)
	{
		return FALSE;
	}
	pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, RectTargetCtrl, this, IDC_2DPOP);
	pView->pRcp = pRcp;
	pView->nDataIdx = nDataIdx;
	pView->OnInitialUpdate();
	bReadyExport = TRUE;
	return TRUE;
}
