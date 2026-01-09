// InfoPanelHelp.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "InfoPanelHelp.h"
#include "ST_SplitterWnd.h"

#include "..\SRC\DOSUtil.h"

#include "GlobalUIContainer.h"
//#include "GlobalDeviceContainer.h"

// CInfoPanelHelp

IMPLEMENT_DYNCREATE(CInfoPanelHelp, CResizableFormView)

CInfoPanelHelp::CInfoPanelHelp()
	: CResizableFormView(CInfoPanelHelp::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelHelp)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pAcro = NULL;
	//	m_pExpr = NULL;
	//	ViewerType = ADOBEREADER;
}

CInfoPanelHelp::~CInfoPanelHelp() {
	if (m_pAcro) {
		delete m_pAcro;
	}
	//	if (m_pExpr) {
	//		delete m_pExpr;
	//	}
}

void CInfoPanelHelp::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelHelp)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelHelp, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelHelp)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	ON_BN_CLICKED(IDC_QUICKSTART, OnQuickstart)
	ON_BN_CLICKED(IDC_RECOVER, OnRecover)
	ON_BN_CLICKED(IDC_COLDSTART, OnColdstart)
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CInfoPanelHelp diagnostics

#ifdef _DEBUG
void CInfoPanelHelp::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelHelp::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelHelp message handlers

void CInfoPanelHelp::OnInitialUpdate() {
	AddResizedControl(IDC_HOST, TOP_LEFT, BOTTOM_RIGHT);
	AddResizedControl(IDC_OPEN, TOP_RIGHT);
	AddResizedControl(IDC_QUICKSTART, TOP_RIGHT);
	AddResizedControl(IDC_RECOVER, TOP_RIGHT);
	AddResizedControl(IDC_COLDSTART, TOP_RIGHT);
	AddResizedControl(IDC_CLOSE, TOP_RIGHT);
	AddResizedControl(IDC_ABOUT, TOP_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	bWnd = TRUE;
	CWnd* pWnd = GetDlgItem(IDC_FRAME);
	pWnd->GetWindowRect(&iniRC);
	ScreenToClient(&iniRC);

	AnchorControls();

	OnClose();
}

LRESULT CInfoPanelHelp::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	if (pUDev) pUDev->SetCurrentView("Help");
	return 0;
}

int CInfoPanelHelp::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CResizableFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//	switch(ViewerType) {
	//	case CInfoPanelHelp::ADOBEREADER:
	// 		if (m_pAcro)
	// 		{
	// 			delete m_pAcro;
	// 		}
	// 		m_pAcro = new CAcroAXDocShim;
	// 		if (m_pAcro)
	// 		{
	// 			try {
	// 				//Create the control, just make sure to use WS_CHILD and WS_VISIBLE.
	// 				if (!m_pAcro->Create("AdobeWnd", WS_CHILD | WS_VISIBLE, CRect(0, 0, 640, 480), this, 280))
	// 				{
	// 					delete m_pAcro;
	// 					m_pAcro = NULL;
	// 					//AfxMessageBox("Failed to create adobe wnd");
	// 				}
	// 			}
	// 			catch (...) {
	// 				delete m_pAcro;
	// 				m_pAcro = NULL;
	// 				//AfxMessageBox("Failed to create adobe wnd");
	// 			}
	// 		}
	//		break;
	//	case CInfoPanelHelp::MODZILLA:
	//		if (m_pExpr) {
	//			delete m_pExpr;
	//		}
	//		m_pExpr = new CWebBrowser2;
	//		if (m_pExpr) {
	//			m_pExpr->type = 2;
	//			//Create the control, just make sure to use WS_CHILD and WS_VISIBLE.
	//			if (!m_pExpr->Create("AdobeWnd", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 280))
	//			{
	//				AfxMessageBox("Failed to create adobe wnd");
	//				return -1;
	//			}
	//		}
	//		break;
	//	case CInfoPanelHelp::IEXPLORER:
	//		if (m_pExpr) {
	//			delete m_pExpr;
	//		}
	//		m_pExpr = new CWebBrowser2;
	//		if (m_pExpr) {
	//			m_pExpr->type = 1;
	//			//Create the control, just make sure to use WS_CHILD and WS_VISIBLE.
	//			if (!m_pExpr->Create("AdobeWnd", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 280))
	//			{
	//				AfxMessageBox("Failed to create adobe wnd");
	//				return -1;
	//			}
	//		}
	//		break;
	//	default:
	//		break;
	//	}
	return 0;
}

void CInfoPanelHelp::OnSize(UINT nType, int cx, int cy) {
	CResizableFormView::OnSize(nType, cx, cy);

	//	CRect rc;
	//	GetClientRect(rc);
	if (m_pAcro) {
		CWnd* pWnd = GetDlgItem(IDC_HOST);
		if (pWnd) {
			CRect rc;
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
			m_pAcro->MoveWindow(rc);
		}
	}
	//	if (m_pExpr) {
	//		m_pExpr->MoveWindow(rc);
	//	}
}

void CInfoPanelHelp::OnOpen() {
	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);

	if (GetPrivateProfileInt("FSM", "ManualShow", 0, inifile)) {
		static int ID = 280;
		if (!m_pAcro) {
			m_pAcro = new CAcroAXDocShim;
			if (m_pAcro) {
				//Create the control, just make sure to use WS_CHILD and WS_VISIBLE.
				if (!m_pAcro->Create("AdobeWnd", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID++)) {
					AfxMessageBox("Failed to create adobe wnd");
					return;
				}
				CWnd* pWnd = GetDlgItem(IDC_HOST);
				if (pWnd) {
					CRect rc;
					pWnd->GetWindowRect(&rc);
					ScreenToClient(&rc);
					m_pAcro->MoveWindow(rc);
				}
				m_pAcro->ShowWindow(SW_SHOW);
			}
		}
	}
	if (m_pAcro) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "MANUAL.PDF";
		m_pAcro->LoadFile(dir);
	}
}

void CInfoPanelHelp::OnClose() {
	if (m_pAcro) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "WELCOME.PDF";
		m_pAcro->LoadFile(dir);
	}
}

void CInfoPanelHelp::OnQuickstart() {
	if (m_pAcro) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "QUICKSTART.PDF";
		m_pAcro->LoadFile(dir);
	}
}

void CInfoPanelHelp::OnRecover() {
	if (m_pAcro) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "RECOVERY.PDF";
		m_pAcro->LoadFile(dir);
	}
}

void CInfoPanelHelp::OnColdstart() {
	if (m_pAcro) {
		CString dir;
		DosUtil.GetProgramDir(dir);
		dir += "COLDSTART.PDF";
		m_pAcro->LoadFile(dir);
	}
}

void CInfoPanelHelp::OnAbout() {
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}