#pragma once
// ViewContentDlg413.h : header file
//

#include "WaferMap.h"
#include "DiceView.h"
#include "ViewContentDlgBase.h"

#include "..\128\CAS\CasMgr.h"
#include "WafMap/WMapView.h"
#include "..\E95\Glut2DMapView.h"

/////////////////////////////////////////////////////////////////////////////
// CViewContentDlg413 dialog

class CInfoPanelView;
class CRecipe413;
class CMPoint;
class CWMap3DDlg;
class CWMap2DDlg;

class CViewContentDlg413 : public CViewContentDlgBase {
	// Construction
public:
	void RefreshDialog();
	CViewContentDlg413(CWnd* pParent = NULL);   // standard constructor
	virtual ~CViewContentDlg413();

	// Dialog Data
		//{{AFX_DATA(CViewContentDlg413)
	enum { IDD = IDD_VIEWCONTENTDLG413 };
	CListCtrl	m_cStats;
	CListCtrl	m_cReport;
	CWaferMap	m_cWaferMap;
	CDiceView	m_cDiceMap;
	//}}AFX_DATA

	short Show;
	short nCjName;
	CString dwCjName[MAXMEPASET * MaXPA];

	CCasMgr CasMgr;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CViewContentDlg413)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Attributes
public:
	void RecalculateStats();
	void GenerateBowA();
	void GenerateBowB();
	void GenSECSReport(short which);
	CPtrList* GetMPList();
	CRecipe413* GetCurrentRecipe();
	CString GetReportPointStr(int cnt, CRecipe413* pRcp, CMPoint* p);
	void UpdateReportPoint(CRecipe413* pRcp, CMPoint* p);
	void UpdateReport(CWaferMap& m_cWaferMap);
	void SetupReportHeader();

	// Implementation
protected:
	// View state management
	enum ViewState { VS_VALUE, VS_2D, VS_3D, VS_PROFILE, VS_BASELINE };
	ViewState m_currentViewState;
	BOOL m_bLastWas2DBefore3D;

	void UpdateViewVisibility();
	void ShowValueView();

	// Generated message map functions
	//{{AFX_MSG(CViewContentDlg413)
	afx_msg void OnClosetab();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnlarge();
	afx_msg void OnWaferview();
	afx_msg void On2dview();
	afx_msg void OnOptions();
	afx_msg void OnExport();
	afx_msg void OnSavebmp();
	afx_msg void OnProfileview();
	afx_msg void OnColumnclickReport(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBow();
	afx_msg void OnGensecsreport();
	afx_msg void OnGensecsreport2();
	afx_msg void OnColumnclickStats(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReload();
	afx_msg void OnBow2();
	afx_msg void OnSwitchview();
	afx_msg void OnCoordinateview();
	afx_msg void OnViewbaseline();
	afx_msg void OnBaselineview();
	afx_msg void OnExport2();
	afx_msg void OnMakebline();
	afx_msg void OnBlcorrect();
	afx_msg void OnBlrecover();
	afx_msg void OnXxx();
	afx_msg void OnXxx2();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);

	// Tooltip functionality
	CToolTipCtrl m_ToolTip;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void Show2DMapView();
	//afx_msg void OnStnClickedMyStatic();
	afx_msg LRESULT OnCreate3DDialogMsg(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClicked3dview();

	// [06262024 ZHIMING
	CButton cRoughnessUnit;
	afx_msg void OnBnClickedRoughnessUnitUm();
	// ]

	// LTV/STIR functionality
	void GenerateMap();
	void IsDiceTypeData();
	void Show3DMapView();
	void StoreData();
	CXYPair dX, dY;
	BOOL isLTV;
	BOOL isSTIR;
	int lastSelectedCol;
	bool is2Dclicked;
	float WaferSize;
	CWMap3DDlg* pDlg;
	CWMap2DDlg* pDlg2;
	int ltvIdx;
	int stirIdx;
	int precision;
	bool isRangeMeasurement;
	CPoint currPon;
	CGlut2DMapView* m_pGl2DView;

	BOOL is3DViewOpen = FALSE;
	void SimulateClickOnStatic(UINT nID);

	// Thread-related declarations
	static const UINT WM_CREATE_3DDIALOG_MSG = WM_USER + 200;

	struct Thread3DData {
		CRecipe413* pRecipe;
		int dataColumn;
		CViewContentDlg413* pDialog;
		bool bIsLTV;
		bool bIsSTIR;

		Thread3DData() :
			pRecipe(nullptr),
			dataColumn(0),
			pDialog(nullptr),
			bIsLTV(false),
			bIsSTIR(false)
		{}
	};

	// Thread management functions
private:
	void ExecuteDelayed3DView();
	static UINT Create3DDialogThreadProc(LPVOID pParam);
	void Create3DDialogOnUIThread(Thread3DData* pData);
	void PostDelayed3DDialogCreation();
public:
	// Thread-safe 3D view creation
	void Create3DViewAsync();

};