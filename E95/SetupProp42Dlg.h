#pragma once
// SetupProp42Dlg.h : header file
//

#include "SpecPlot.h"
#include "SetupPropComm.h"

#include "..\SRC\SortListCtrl.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp42Dlg dialog

class CSetupProp42Dlg : public CResizableDialog, public CSetupPropComm {
	short CurNo;
	CSpec ISpec1, ISpec2, ISpec3;
	CSpec OSpec1, OSpec2, OSpec3;

	float Amp[4];
	float Cycle[4];
	float Phase[4];
	CString bakname;

	// Construction
public:
	void LocalToRecipe();
	void RecipeToLocal();

	void SwapMeSet(short next);
	void DoCalculationC(CString filename);
	void DoCalculationB(CString filename, BOOL bForce = FALSE);
	void DoCalculationA(CString filename);
	BOOL GenerateSpectrum(short which);
	float RecalculateSpectrum(short which);
	void Wait();
	void DisplaySpectrum(short n, CString name, CSpec* pRSpec);
	BOOL OpenSpecFile(CString SpecName, short n);
	void DisplayFiles(LPTSTR Path);
	void OpenFolder(CString CStrPath);
	CSetupProp42Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp42Dlg)
	enum { IDD = IDD_SETUPPROP42 };
	CButton	m_cThkRange;
	CButton	m_cShowSpec;
	CButton	m_cCycleFirst;
	CStatic	m_cMessage1;
	CStatic	m_cMessage2;
	CStatic	m_cMessage;
	CButton	m_cGenerateReport;
	CSortListCtrl m_cTop;
	CSortListCtrl m_cLeft;
	CSortListCtrl m_cRight;
	CSpecPlot	m_cSpec4;
	CSpecPlot	m_cSpec3;
	CSpecPlot	m_cSpec2;
	CSpecPlot	m_cSpec1;
	int		m_NumberOfMappedPoints;
	float	m_TFScalingFactor;
	short	m_nCycle;
	float	m_Difference;
	float	m_Difference2;
	short	m_Smooth;
	short	m_Expand;
	short	m_Method;
	//}}AFX_DATA

	float	m_FThkMax[MAXTHKVLU];
	float	m_FThkMin[MAXTHKVLU];
	float	m_RI[MAXTHKVLU];

	CString Dirc;
	CString ParentDirc;
	CString SpecName;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp42Dlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp42Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkLeft(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTop(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkRightview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLoadrecipe();
	afx_msg void OnKillfocusNumberofmappedpoints();
	afx_msg void OnGeneratereport();
	afx_msg void OnKillfocusFthkmaxSp4();
	afx_msg void OnKillfocusFthkminSp4();
	afx_msg void OnKillfocusFthkmaxSp5();
	afx_msg void OnKillfocusFthkminSp5();
	afx_msg void OnKillfocusTfscalefactor();
	afx_msg void OnKillfocusNcycle();
	afx_msg void OnSavetop();
	afx_msg void OnSavebottom();
	afx_msg void OnSavespectrum();
	afx_msg void OnClickRightview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedRightview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLoadspectrum();
	afx_msg void OnKillfocusRi1();
	afx_msg void OnKillfocusRi2();
	afx_msg void OnLoadspectrum2();
	afx_msg void OnRadio7();
	afx_msg void OnRadio8();
	afx_msg void OnRadio9();
	afx_msg void OnRadio10();
	afx_msg void OnRadio3();
	afx_msg void OnRadio1();
	afx_msg void OnRadio11();
	afx_msg void OnSimulate();
	afx_msg void OnCalculate();
	afx_msg void OnKillfocusFthkminSp6();
	afx_msg void OnKillfocusFthkmaxSp6();
	afx_msg void OnKillfocusRi3();
	afx_msg void OnLoadspectrum3();
	afx_msg void OnKillfocusSmooth();
	afx_msg void OnCyclefirst();
	afx_msg void OnShowspec();
	afx_msg void OnKillfocusExpand();
	afx_msg void OnLoadref();
	afx_msg void OnKillfocusMethod();
	afx_msg void OnKillfocusFthkminSp7();
	afx_msg void OnKillfocusFthkminSp8();
	afx_msg void OnKillfocusFthkminSp9();
	afx_msg void OnKillfocusFthkmaxSp7();
	afx_msg void OnKillfocusFthkmaxSp8();
	afx_msg void OnKillfocusFthkmaxSp9();
	afx_msg void OnKillfocusRi4();
	afx_msg void OnKillfocusRi5();
	afx_msg void OnKillfocusRi6();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};