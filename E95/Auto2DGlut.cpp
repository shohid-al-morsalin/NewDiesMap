// Auto2DGlut.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "resource.h"
#include "Auto2DGlut.h"
#include "afxdialogex.h"
#include "E95/Recipe413.h"
#include "WafMap/WMapView.h"


// Auto2DGlut dialog

IMPLEMENT_DYNAMIC(Auto2DGlut, CDialog)

void Auto2DGlut::SetParam(CRecipe413* recipe, int dataIdx) {
	pRcp = recipe; nDataIdx = dataIdx;
}

Auto2DGlut::Auto2DGlut(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_WAFERMAP3D_DLG, pParent)
{
}

Auto2DGlut::~Auto2DGlut()
{
}

void Auto2DGlut::ExportTo(CString exportPath) {
	while (!bReadyExport) {
		Yld(100);
	}
	Yld(300);
	pView->ExportTo(exportPath);
}

void Auto2DGlut::Yld(ULONGLONG dly) {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < dly) {
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void Auto2DGlut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Auto2DGlut, CDialog)
END_MESSAGE_MAP()

// Auto2DGlut message handlers

BOOL Auto2DGlut::OnInitDialog()
{
	if (pRcp == nullptr) return FALSE;

	CDialog::OnInitDialog();

	CWnd* pWnd = GetDlgItem(IDC_STATIC_2DAUTO);
	CRect RectTargetCtrl;
	pWnd->GetWindowRect(RectTargetCtrl);
	this->ScreenToClient(RectTargetCtrl);
	pView = (CWMapView*)RUNTIME_CLASS(CWMapView)->CreateObject();
	if (NULL == pView)
	{
		return FALSE;
	}
	pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, RectTargetCtrl, this, IDC_STATIC_2DAUTO);
	pView->pRcp = pRcp;
	pView->nDataIdx = nDataIdx;
	pView->OnInitialUpdate();
	bReadyExport = TRUE;
	return TRUE;
}
//[20251215_Mohir
#include "hpdf.h"
#include "413App.h"
void Auto2DGlut::ExportToCImage(CString exportPath)
{ // Mohir 20251215
    UpdateWindow();
    // Get window dimensions
    CRect rect;
    GetClientRect(&rect);
    int width = rect.Width();
    int height = rect.Height();
    if (width <= 0 || height <= 0) {
        return;
    }
    // Get window DC
    CDC* pDC = GetDC();
    // Create memory DC
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    // Create CImage with 24-bit color
    CImage image;
    image.Create(width, height, 24);
    // Select image into memory DC and copy window
    HBITMAP hOldBmp = (HBITMAP)memDC.SelectObject(image);
    memDC.BitBlt(0, 0, width, height, pDC, 0, 0, SRCCOPY);
    memDC.SelectObject(hOldBmp);
    // Cleanup
    ReleaseDC(pDC);

    // =========== Extract filename from path ===========
    CString title = _T("");

    if (!exportPath.IsEmpty()) {
        // Method 1: Using PathFindFileName
        const TCHAR* fileName = PathFindFileName(exportPath);
        if (fileName && *fileName) {
            title = fileName;
            // Remove extension if present
            PathRemoveExtension(title.GetBuffer(MAX_PATH));
            title.ReleaseBuffer();
        }
    }
    p413App->PdfExporter.AddMultiPages(1);
    // 6. Get the current page (usually last page)
    HPDF_Page pdfPage = p413App->PdfExporter.GetPage(p413App->PdfExporter.ExtraPage);
    // 7. Calculate position (start near top of page)
    float pageHeight = HPDF_Page_GetHeight(pdfPage);
    float startY = pageHeight - 40.0f; // 100 points from top
    // 8. Insert image into PDF
    p413App->PdfExporter.InsertTopImage(
        pdfPage,                    // HPDF_Page
        pageHeight,                 // float startY
        title,                      // CString title - use extracted filename
        _T(""),                     // CString probeName
        image,                      // CImage
        _T("2D_IMAGE_VIEW")         // CString caption
    );
    ++p413App->PdfExporter.ExtraPage;
}
//]