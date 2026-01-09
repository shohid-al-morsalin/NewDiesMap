#pragma once
// RecipeIO.h : header file
//

class CRecipe413;

/////////////////////////////////////////////////////////////////////////////
// CRecipeIO dialog

class CRecipeIO : public CDialog {
	CString inifile;

	void LoadParam();
	void SaveParam();
public:
	CRecipe413* pRcp = NULL;
	CRecipeIO(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRecipeIO)
	enum { IDD = IDD_RECIPEIO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRecipeIO)
	afx_msg void OnNew();
	afx_msg void OnLoad();
	afx_msg void OnSave();
	afx_msg void OnLoadf();
	afx_msg void OnLoadq();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
