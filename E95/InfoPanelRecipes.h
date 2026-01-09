#pragma once

// CInfoPanelRecipes form view

#include "ResizableFormView.h"
#include "XTabCtrl.h"

// class CRecipeCreateDlg;
class CRecipeCreateDlg413;
class CRecipeManageDlg;

class CInfoPanelRecipes : public CResizableFormView {
	DECLARE_DYNCREATE(CInfoPanelRecipes)

protected:
	CInfoPanelRecipes();           // protected constructor used by dynamic creation
	virtual ~CInfoPanelRecipes();

public:
	CXTabCtrl m_tabctrl;
	CRecipeCreateDlg413* m_RecipeCreateDlg413;
	CRecipeManageDlg* m_RecipeManageDlg;

	//{{AFX_DATA(CInfoPanelRecipes)
	enum { IDD = IDD_INFOPANELRECIPES };
	//}}AFX_DATA
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	LRESULT OnSTSelected(WPARAM wParam, LPARAM lParam);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoPanelRecipes)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CInfoPanelRecipes)
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
