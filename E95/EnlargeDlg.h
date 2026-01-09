#pragma once
// EnlargeDlg.h : header file
//

#include "WaferMap.h"
#include "DiceView.h"
#include "..\E95\Glut2DMapView.h"

/////////////////////////////////////////////////////////////////////////////
// CEnlargeDlg dialog

class CEnlargeDlg : public CDialog
{
	DECLARE_DYNAMIC(CEnlargeDlg)  // 

	// Construction
public:
	CEnlargeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEnlargeDlg(); // 

// Dialog Data
	//{{AFX_DATA(CEnlargeDlg)
	enum { IDD = IDD_ENLARGE };
	CWaferMap	m_cWaferMap;
	CDiceView	m_cDiceMap;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnlargeDlg)
public:
	virtual void DoDataExchange(CDataExchange* pDX);    
	//}}AFX_VIRTUAL

// Implementation
protected:
	void HideAllViews();
	void ShowOnlyWaferMap();
	void ShowOnlyDiceMap();
	void ShowOnly2DView();

	// Generated message map functions
	//{{AFX_MSG(CEnlargeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSavebmp();
	afx_msg void OnSavebmp2();
	afx_msg void OnOptions();
	afx_msg void OnProfileview();
	afx_msg void On2dview();
	afx_msg void OnWaferview();
	afx_msg void Test();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	bool isMapStateDice = false;
	CGlut2DMapView* m_pGl2DView = nullptr; 

	CRect m_rcOriginalDialog;          // Original dialog size
	CArray<CRect, CRect&> m_arrOriginalCtrlRects; // Original control positions
	CArray<int, int> m_arrCtrlIds;     // Control IDs to scale

	void SaveOriginalLayout();
	void ScaleControls();
};