// InfoPanelView.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "InfoPanelView.h"
#include "ViewFileDlg.h"
#include "ViewCassDlg.h"
#include "ViewContentDlg413.h"
#include "..\SYS\CUMMsg.h"

#include "ST_SplitterWnd.h"

#include "GlobalDeviceContainer.h"

// CInfoPanelView

IMPLEMENT_DYNCREATE(CInfoPanelView, CResizableFormView)

CInfoPanelView::CInfoPanelView()
	: CResizableFormView(CInfoPanelView::IDD) {
	m_FileDlg = NULL;
	m_FileDlg413 = NULL;
	m_ViewCassDlg = NULL;
	m_ViewContentDlg413 = NULL;
	m_FileToLoad = "";
	m_DirToLoad = "";
}

CInfoPanelView::~CInfoPanelView() {}

void CInfoPanelView::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
}

BEGIN_MESSAGE_MAP(CInfoPanelView, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelView)
	ON_MESSAGE(UM_LOADFILE, OnLoadFile)
	ON_MESSAGE(UM_CLOSETAB, OnCloseView)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CInfoPanelView diagnostics

#ifdef _DEBUG
void CInfoPanelView::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelView::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelView message handlers
void CInfoPanelView::OnInitialUpdate() {
	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	bWnd = TRUE;
	CWnd* pWnd = GetDlgItem(IDC_FRAME);
	pWnd->GetWindowRect(&iniRC);
	ScreenToClient(&iniRC);

	AnchorControls();

	int nTab = 0;
	m_FileDlg413 = new CViewFileDlg;
	if (m_FileDlg413) {
		m_FileDlg413->m_Parent = this;
		m_FileDlg413->Create(CViewFileDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_FileDlg413, "  Data Files  ", nTab++);
	}
	//m_ViewCassDlg = new CViewCassDlg;
	if (m_ViewCassDlg) {
		m_ViewCassDlg->m_Parent = this;
		m_ViewCassDlg->Create(CViewCassDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_ViewCassDlg, "By Recipe", nTab++);
	}
	//m_ViewContentDlg413 = new CViewContentDlg413;
	if (m_ViewContentDlg413) {
		m_ViewContentDlg413->Parent = this;
		m_ViewContentDlg413->Create(CViewContentDlg413::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_ViewContentDlg413, "Content", nTab++);
	}
}

LRESULT CInfoPanelView::OnLoadFile(WPARAM wP, LPARAM lP) {
	Load413File(wP);
	return LRESULT(0);
}

LRESULT CInfoPanelView::OnCloseView(WPARAM wP, LPARAM lP) {
	CViewContentDlgBase* pV = (CViewContentDlgBase*)lP;
	if (pV) {
		int idx = m_tabctrl.FindTabIndex(pV);
		if (idx > -1) {
			if (idx > 0) {
				m_tabctrl.SelectTab(idx - 1);
			}
			m_tabctrl.DeleteTab(idx);
		}
		DeleteView(pV);
	}

	return LRESULT(0);
}

void CInfoPanelView::ClearAllView() {
	POSITION pos;

	pos = m_View.GetTailPosition();
	while (pos) {
		CViewContentDlgBase* p = (CViewContentDlgBase*)m_View.GetPrev(pos);
		if (p && ((unsigned int)p->GetSafeHwnd() != 0xfeeefeee)) {
			int idx = m_tabctrl.GetTabIndexByName(p->ViewName);
			if (idx > -1) {
				m_tabctrl.DeleteTab(idx);
			}
			p->DestroyWindow();
		}
	}
	m_View.RemoveAll();
}

void CInfoPanelView::AddView(CViewContentDlgBase* pView) {
	m_View.AddTail(pView);
}

BOOL CInfoPanelView::DeleteView(CViewContentDlgBase* pView) {
	if (!pView || ((unsigned int)pView->GetSafeHwnd() == 0xfeeefeee)) {
		ASSERT(0);
		return FALSE;
	}
	POSITION pos, opos;

	pos = m_View.GetHeadPosition();
	while (pos) {
		opos = pos;
		CViewContentDlgBase* p = (CViewContentDlgBase*)m_View.GetNext(pos);
		if (p && (p == pView)) {
			m_View.RemoveAt(opos);
			p->DestroyWindow();
			return TRUE;
		}
	}
	return FALSE;
}

void CInfoPanelView::OnDestroy() {
	ClearAllView();  // seems that view are automatically freed

	// Windows object, not good to delete at destructor
	if (m_FileDlg) {
		if (::IsWindow(m_FileDlg->m_hWnd)) {
			m_FileDlg->DestroyWindow();
			m_FileDlg = NULL;
		}
	}
	if (m_FileDlg413) {
		if (::IsWindow(m_FileDlg413->m_hWnd)) {
			m_FileDlg413->DestroyWindow();
			m_FileDlg413 = NULL;
		}
	}
	if (m_ViewCassDlg) {
		if (::IsWindow(m_ViewCassDlg->m_hWnd)) {
			m_ViewCassDlg->DestroyWindow();
			m_ViewCassDlg = NULL;
		}
	}
	if (m_ViewContentDlg413) {
		if (::IsWindow(m_ViewContentDlg413->m_hWnd)) {
			m_ViewContentDlg413->DestroyWindow();
			m_ViewContentDlg413 = NULL;
		}
	}
	CResizableFormView::OnDestroy();
}

LRESULT CInfoPanelView::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	m_tabctrl.ActivateCurrentTab();
	return 0;
}

void CInfoPanelView::Load413File(WPARAM wP) {
	CViewContentDlg413* m_ViewContentDlg413 = new CViewContentDlg413;
	if (m_ViewContentDlg413) {
		AddView(m_ViewContentDlg413);
		m_ViewContentDlg413->Parent = this;
		m_ViewContentDlg413->ViewName = m_FileToLoad;
		m_ViewContentDlg413->ViewDir = m_DirToLoad;
		m_ViewContentDlg413->Create(CViewContentDlg413::IDD, &m_tabctrl);
		CRect rc;
		m_tabctrl.GetClientRect(&rc);
		m_tabctrl.AddTab(m_ViewContentDlg413, m_ViewContentDlg413->ViewName.GetBuffer(0), 1, rc.Width(), rc.Height());
		int idx = m_tabctrl.GetTabIndexByName(m_ViewContentDlg413->ViewName.GetBuffer(0));
		m_tabctrl.SelectTab(idx);
	}
}