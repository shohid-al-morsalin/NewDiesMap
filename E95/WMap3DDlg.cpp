// WMap3DDlg.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "afxdialogex.h"
#include "WMap3DDlg.h"

#include "E95/Recipe413.h"
#include "WafMap/WMapView.h"
#include <atlimage.h> //20251210_Mohir
#include "413App.h"
 

BEGIN_MESSAGE_MAP(CWMap3DDlg, CDialog)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CWMap3DDlg, CDialog)

void CWMap3DDlg::SetParam(CRecipe413* recipe, int dataIdx) {
	pRcp = recipe; nDataIdx = dataIdx;
}

CWMap3DDlg::CWMap3DDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_WAFERMAP3D_DLG, pParent)
{
}

CWMap3DDlg::~CWMap3DDlg()
{
}


void CWMap3DDlg::ExportToImage(CString exportPath) {
	while (!bReadyExport) {
		Yld(100);
	}
	Yld(300);
	pView->ExportToCImage(exportPath);
	return ;
}
 


void CWMap3DDlg::Yld(ULONGLONG dly) {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < dly) {
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CWMap3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}





// CWMap3DDlg message handlers

BOOL CWMap3DDlg::OnInitDialog()
{
	if (pRcp == nullptr) return FALSE;

	CDialog::OnInitDialog();
	p413App->Global.is3DClosed = false;
	CWnd* pWnd = GetDlgItem(IDC_WAFERMAP_3D);
	CRect RectTargetCtrl;
	pWnd->GetWindowRect(RectTargetCtrl);
	this->ScreenToClient(RectTargetCtrl);
	pView = (CWMapView*)RUNTIME_CLASS(CWMapView)->CreateObject();
	if (NULL == pView)
	{
		return FALSE;
	}
	pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, RectTargetCtrl, this, IDC_WAFERMAP_3D);
	pView->pRcp = pRcp;
	pView->nDataIdx = nDataIdx;
	pView->OnInitialUpdate();
	bReadyExport = TRUE;
	return TRUE;
}

void CWMap3DDlg::OnClose()
{
	// Optional: Add cleanup logic here if needed
	// For example: stop rendering, save state, etc.
	p413App->Global.is3DClosed = true;

	// Destroy the view if it was created
	if (pView != nullptr)
	{
		pView->DestroyWindow();
		//delete pView;
		pView = nullptr;
	}

	// Call base class to destroy the dialog
	CDialog::OnClose();

	// Optional: Post a quit message or just let the dialog die
	// Since it's a modeless or modal dialog, usually OnClose + DestroyWindow is enough
	DestroyWindow();
}