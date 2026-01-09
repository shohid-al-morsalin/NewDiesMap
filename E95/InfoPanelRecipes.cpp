// InfoPanelRecipes.cpp : implementation file
//

#include "stdafx.h"
#include "E95.h"
#include "InfoPanelRecipes.h"
#include "RecipeCreateDlg413.h"
#include "RecipeManageDlg.h"

#include "ST_SplitterWnd.h"

#include "GlobalDeviceContainer.h"

// CInfoPanelRecipes

IMPLEMENT_DYNCREATE(CInfoPanelRecipes, CResizableFormView)

CInfoPanelRecipes::CInfoPanelRecipes()
	: CResizableFormView(CInfoPanelRecipes::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelRecipes)
	//}}AFX_DATA_INIT
	m_RecipeCreateDlg413 = NULL;
	m_RecipeManageDlg = NULL;
}

CInfoPanelRecipes::~CInfoPanelRecipes() {}

void CInfoPanelRecipes::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelRecipes)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelRecipes, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelRecipes)
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CInfoPanelRecipes diagnostics

#ifdef _DEBUG
void CInfoPanelRecipes::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelRecipes::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelRecipes message handlers
void CInfoPanelRecipes::OnInitialUpdate() {
	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	AnchorControls();

	int nTab = 0;
	m_RecipeCreateDlg413 = new CRecipeCreateDlg413;
	if (m_RecipeCreateDlg413) {
		m_RecipeCreateDlg413->Create(CRecipeCreateDlg413::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_RecipeCreateDlg413, "View(413)", nTab++);
	}
	m_RecipeManageDlg = new CRecipeManageDlg;
	if (m_RecipeManageDlg) {
		m_RecipeManageDlg->Create(CRecipeManageDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(m_RecipeManageDlg, "Manage", nTab++);
	}
}

LRESULT CInfoPanelRecipes::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	m_tabctrl.ActivateCurrentTab();
	if (m_RecipeCreateDlg413) {
		m_RecipeCreateDlg413->LockView();
	}
	if (m_RecipeManageDlg) {
		m_RecipeManageDlg->LockView();
	}
	return 0;
}

void CInfoPanelRecipes::OnDestroy() {
	if (m_RecipeCreateDlg413) {
		m_RecipeCreateDlg413->DestroyWindow();
		m_RecipeCreateDlg413 = NULL;
	}
	if (m_RecipeManageDlg) {
		m_RecipeManageDlg->DestroyWindow();
		m_RecipeManageDlg = NULL;
	}
	CResizableFormView::OnDestroy();
}