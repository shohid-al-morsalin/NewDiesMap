// DiceMapTemp.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "DiceMapTemp.h"
#include "afxdialogex.h"


// DiceMapTemp dialog

IMPLEMENT_DYNAMIC(DiceMapTemp, CDialog)

DiceMapTemp::DiceMapTemp(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DICEMAP_TEMP, pParent)
{

}

DiceMapTemp::~DiceMapTemp()
{
}

void DiceMapTemp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DiceMapTemp, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()



BOOL DiceMapTemp::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Subclass the static control
    m_cDiceMap.SubclassDlgItem(IDC_DICEMAP_TEXT, this);

    // Start a 500ms timer to auto-close
    SetTimer(1, 500, NULL); // Timer ID = 1, 500 ms

    return TRUE;
}

void DiceMapTemp::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        KillTimer(1);

        //if (!m_strSavePath.IsEmpty() && ::IsWindow(m_cDiceMap.m_hWnd))
        //{
        //    /*CFile file;
        //    if (file.Open(m_strSavePath, CFile::modeCreate | CFile::modeWrite))
        //    {*/
        //       /* CRect rc;
        //        m_cDiceMap.GetClientRect(&rc);
        //        if (rc.IsRectEmpty())
        //            rc = CRect(0, 0, 800, 600);
        //        m_cDiceMap.SaveBitmap(m_strSavePath);*/
        //       // file.Close();
        //   // }
        //}
        if ( ::IsWindow(m_cDiceMap.m_hWnd))
        {
            //    CFile file;
              /*  if (file.Open(m_strSavePath, CFile::modeCreate | CFile::modeWrite))
                {*/
            CRect rc;
            m_cDiceMap.GetClientRect(&rc);
            if (rc.IsRectEmpty())
                rc = CRect(0, 0, 800, 800);
            // 1. Save to PNG file (original functionality)
            //m_waferMap.SaveBitmap(file, rc);
            Sleep(200); // Extra safety wait   
            m_cDiceMap.CaptureToPDF();  //20251219_Mahmudul

            /* }*/
        }
        EndDialog(IDOK);
    }
    CDialog::OnTimer(nIDEvent);
}


// DiceMapTemp message handlers
