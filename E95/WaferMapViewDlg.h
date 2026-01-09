#pragma once

#include "afxwin.h"
#include "WaferMap.h"
#include <atlimage.h>  //  20251210_Mohir

class CWaferMapViewDlg : public CDialog
{
public:
    CWaferMapViewDlg(CWnd* pParent = nullptr);
    enum { IDD = IDD_WAFERMAP_VIEW };

    CWaferMap m_waferMap;
    CString m_strSavePath;
   

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
   // afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()
};