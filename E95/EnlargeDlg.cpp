// EnlargeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "EnlargeDlg.h"
#include "413App.h"
#include "WaferViewOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnlargeDlg dialog

IMPLEMENT_DYNAMIC(CEnlargeDlg, CDialog)

CEnlargeDlg::CEnlargeDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CEnlargeDlg::IDD, pParent)
    , m_pGl2DView(nullptr)
    , isMapStateDice(FALSE)
{
}

CEnlargeDlg::~CEnlargeDlg()
{
    /*if (m_pGl2DView)
    {
        m_pGl2DView->DestroyWindow();
        delete m_pGl2DView;
        m_pGl2DView = nullptr;
    }*/
}

void CEnlargeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CEnlargeDlg)
    DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
    DDX_Control(pDX, IDC_VIEW2, m_cDiceMap);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEnlargeDlg, CDialog)
    //{{AFX_MSG_MAP(CEnlargeDlg)
    ON_BN_CLICKED(IDC_SAVEBMP, OnSavebmp)
    ON_BN_CLICKED(IDC_SAVEBMP2, OnSavebmp2)
    ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
    ON_BN_CLICKED(IDC_PROFILEVIEW, OnProfileview)
    ON_BN_CLICKED(IDC_2DVIEW, On2dview)
    ON_BN_CLICKED(IDC_WAFERVIEW, OnWaferview)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Helper: Hide all views
void CEnlargeDlg::HideAllViews()
{
    // Destroy OpenGL view if exists
    if (m_pGl2DView)
    {
        m_pGl2DView->DestroyWindow();
        //delete m_pGl2DView;
        m_pGl2DView = nullptr;
    }

    // Hide static controls
    GetDlgItem(IDC_WAFERMAP)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_VIEW2)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_ENLARGE_2DGL)->ShowWindow(SW_HIDE);
}

void CEnlargeDlg::ShowOnlyWaferMap()
{
    HideAllViews();
    GetDlgItem(IDC_WAFERMAP)->ShowWindow(SW_SHOW);
    isMapStateDice = FALSE;
}

void CEnlargeDlg::ShowOnlyDiceMap()
{
    HideAllViews();
    GetDlgItem(IDC_VIEW2)->ShowWindow(SW_SHOW);
    m_cDiceMap.RedrawWithData();
    isMapStateDice = TRUE;
}

void CEnlargeDlg::ShowOnly2DView()
{
    HideAllViews();

    CWnd* pPlaceholder = GetDlgItem(IDC_ENLARGE_2DGL);
    if (!pPlaceholder)
    {
        AfxMessageBox(_T("IDC_ENLARGE_2DGL placeholder not found!"));
        return;
    }

    CRect rc;
    pPlaceholder->GetWindowRect(&rc);
    ScreenToClient(&rc);

    m_pGl2DView = static_cast<CGlut2DMapView*>(
        RUNTIME_CLASS(CGlut2DMapView)->CreateObject()
        );
    if (!m_pGl2DView)
    {
        AfxMessageBox(_T("Failed to create CGlut2DMapView!"));
        return;
    }

    if (!m_pGl2DView->Create(
        NULL,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
        rc,
        this,
        IDC_ENLARGE_2DGL
    ))
    {
        AfxMessageBox(_T("Failed to create OpenGL view window!"));
       /* delete m_pGl2DView;
        m_pGl2DView = nullptr;*/
        return;
    }

    m_pGl2DView->pRcp = m_cWaferMap.pRcp;
    m_pGl2DView->nDataIdx = m_cWaferMap.DataCol;
    m_pGl2DView->OnInitialUpdate();

    p413App->Global.isCallFromViewContentDlg413 = true;
    p413App->Global.callFromBtn = true;

    m_pGl2DView->Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// Message handlers

BOOL CEnlargeDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Save original layout BEFORE resizing
    SaveOriginalLayout();

    // Option A: Maximize (keeps title bar and close button)
    ShowWindow(SW_MAXIMIZE);


    ScaleControls();

    m_cWaferMap.dmMode = CWaferMap::dmUSER;

    if (p413App->Global.setDiceDisplay)
    {
        ShowOnlyDiceMap();
    }
    else
    {
        ShowOnlyWaferMap();
        m_cWaferMap.Redraw();
    }

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CEnlargeDlg::OnSavebmp()
{
    CFileDialog fd(FALSE, _T("BMP"), NULL,
        OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
        _T("Bitmap file (*.BMP)|*.BMP||"));

    if (fd.DoModal() == IDOK)
    {
        CString path = fd.GetPathName();
        if (isMapStateDice)
        {
            m_cDiceMap.SaveBitmap(path);
        }
        else
        {
            CFile fp;
            if (fp.Open(path, CFile::modeCreate | CFile::modeWrite))
            {
                CRect rc(0, 0, 1280, 960);
                m_cWaferMap.SaveBitmap(fp, rc);
                fp.Close();
            }
        }
    }
}

void CEnlargeDlg::OnSavebmp2()
{
    CFileDialog fd(FALSE, _T("BMP"), NULL,
        OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
        _T("Bitmap file (*.BMP)|*.BMP||"));

    if (fd.DoModal() == IDOK)
    {
        CString path = fd.GetPathName();
        if (isMapStateDice)
        {
            m_cDiceMap.SaveBitmap(path);
        }
        else
        {
            CFile fp;
            if (fp.Open(path, CFile::modeCreate | CFile::modeWrite))
            {
                CRect rc(0, 0, 1600, 1200);
                m_cWaferMap.SaveBitmap(fp, rc);
                fp.Close();
            }
        }
    }
}

void CEnlargeDlg::OnOptions()
{
    CWaferViewOptions dlg;
    dlg.pWaferMap = &m_cWaferMap;
    dlg.DoModal();
}

void CEnlargeDlg::OnProfileview()
{
    ShowOnlyWaferMap();
    m_cWaferMap.m_nImageType = 2;
    m_cWaferMap.Redraw();
}

void CEnlargeDlg::On2dview()
{
    ShowOnly2DView();
}

void CEnlargeDlg::OnWaferview()
{
    if (p413App->Global.setDiceDisplay)
    {
        ShowOnlyDiceMap();
    }
    else
    {
        ShowOnlyWaferMap();
        m_cWaferMap.m_nImageType = 0;
        m_cWaferMap.Redraw();
    }
}

void CEnlargeDlg::SaveOriginalLayout()
{
    // Get original dialog size (from resource)
    CRect rcDlg;
    GetWindowRect(&rcDlg);
    m_rcOriginalDialog = rcDlg;

    // List of controls you want to scale/reposition
    static const int ctrlIds[] = {
        IDC_WAFERMAP,
        IDC_VIEW2,
        IDC_SAVEBMP,
        IDC_SAVEBMP2,
        IDC_OPTIONS,
        IDC_PROFILEVIEW,
        IDC_2DVIEW,
        IDC_WAFERVIEW,
        IDC_ENLARGE_2DGL  // if used as placeholder
        // Add more as needed
    };

    m_arrCtrlIds.RemoveAll();
    m_arrOriginalCtrlRects.RemoveAll();

    for (int id : ctrlIds)
    {
        CWnd* pWnd = GetDlgItem(id);
        if (pWnd)
        {
            CRect rc;
            pWnd->GetWindowRect(&rc);
            ScreenToClient(&rc);
            m_arrOriginalCtrlRects.Add(rc);
            m_arrCtrlIds.Add(id);
        }
    }
}

void CEnlargeDlg::ScaleControls()
{
    CRect rcCurrent;
    GetClientRect(&rcCurrent);

    double scaleX = (double)rcCurrent.Width() / m_rcOriginalDialog.Width();
    double scaleY = (double)rcCurrent.Height() / m_rcOriginalDialog.Height();

    for (int i = 0; i < m_arrCtrlIds.GetSize(); i++)
    {
        CRect rcOrig = m_arrOriginalCtrlRects[i];
        CRect rcNew;

        // Scale position and size
        rcNew.left = (LONG)(rcOrig.left * scaleX);
        rcNew.top = (LONG)(rcOrig.top * scaleY);
        rcNew.right = (LONG)(rcOrig.right * scaleX);
        rcNew.bottom = (LONG)(rcOrig.bottom * scaleY);

        GetDlgItem(m_arrCtrlIds[i])->MoveWindow(&rcNew);
    }
}