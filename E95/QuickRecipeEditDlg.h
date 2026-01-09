#pragma once
// QuickRecipeEditDlg.h : header file
//

#include "..\PropList\PropertyList.h"

class CRecipe413;

/////////////////////////////////////////////////////////////////////////////
// CQuickRecipeEditDlg dialog

class CQuickRecipeEditDlg : public CDialog {
	// Construction
public:
	void AddMeasurementPoints(FILE* fp);
	void AddEntriesEhoMeasurement();
	void AddEntriesEchoPeakControl();
	void AddEntriesMeSetEcho();
	void AddEntriesMeSetThinFilm();
	void AddEntriesThinFilmMeasurement();
	void AddEntriesTrimWidth();
	void AddEnntriesTrimHeight();
	void AddEntriesBow();
	void AddEntriesPattern();
	short GetMatchMode(CString str);
	short GetFilterValue(CString name);
	void ParseBowWarp();
	void ParseTrimHeight();
	void ParseTrimWidth();
	void ParsePatternMatching();
	void ParseThinFilmMeasurement();
	void ParseThinFilmMeSet();
	void ParseEchoMeSet();
	void ParsePeakControl();
	void ParseEchoMeasurement();
	void ParseEntry();
	CQuickRecipeEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CQuickRecipeEditDlg)
	enum { IDD = IDD_QUICKRCPEDIT };
	CPropertyList	m_propList;
	CTreeCtrl	m_tree;
	//}}AFX_DATA

	CString SelItem;
	CRecipe413* pRcp;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CQuickRecipeEditDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CQuickRecipeEditDlg)
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnOk3();
	afx_msg void OnReplacepoints();
	afx_msg void OnLoadrecipe();
	afx_msg void OnLoaddatafile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	friend void pfnOnSelChangedPropertyList(const CPropertyItem* pItem);
};