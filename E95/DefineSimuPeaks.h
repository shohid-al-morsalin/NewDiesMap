#pragma once

#include "PropList/PropertyList.h"

class CDefineSimuPeaks : public CDialog {
	// Construction
public:
	void DoDataEntry();
	void AddItemToList();
	void ParseEntry();
	CDefineSimuPeaks(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDefineSimuPeaks)
	enum { IDD = IDD_DEFINEPEAKS };
	CTreeCtrl	m_Tree1;
	CTreeCtrl	m_Tree2;
	CPropertyList	m_cList;
	//}}AFX_DATA

	short Set, Num;
	short OSet, ONum;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDefineSimuPeaks)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDefineSimuPeaks)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree2(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	friend void pfnOnSelChangedPropertyListDefine(const CPropertyItem* pItem);
};
