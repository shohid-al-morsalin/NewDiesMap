#include "stdafx.h"
#include "WaferMapViewDlg.h"
#include "resource.h"
#include <shlwapi.h>
// 21102025_1
//[20251215_Mohir
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
#include "413App.h" //20251215_MOhir
//]

CWaferMapViewDlg::CWaferMapViewDlg(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD, pParent)
{
}

void CWaferMapViewDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWaferMapViewDlg, CDialog)
    ON_WM_TIMER()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CWaferMapViewDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Subclass the static control
    m_waferMap.SubclassDlgItem(IDC_STATIC_WAFERMAP, this);

    // Start a 500ms timer to auto-close
    SetTimer(1, 500, NULL); // Timer ID = 1, 500 ms

    return TRUE;
}

void CWaferMapViewDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        KillTimer(1);

        //if (!m_strSavePath.IsEmpty() && ::IsWindow(m_waferMap.m_hWnd))
        //{
        //    CFile file;
        //    if (file.Open(m_strSavePath, CFile::modeCreate | CFile::modeWrite))
        //    {
        //        CRect rc;
        //        m_waferMap.GetClientRect(&rc);
        //        if (rc.IsRectEmpty())
        //            rc = CRect(0, 0, 800, 600);
        //        m_waferMap.SaveBitmap(file, rc); //20251210_Mohir Modify this function
        //        Sleep(200); // Extra safety wait
        //        file.Close();
        //    }
        //}   
        
        EndDialog(IDOK);
    }
    CDialog::OnTimer(nIDEvent);
}
