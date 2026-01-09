#pragma once

// #include "SICam.h"
#include "SpecPlot.h"
#include "WaferMap.h"
// #include "Recipe413.h"

#include "SetupPropComm.h"
#include "SRC/ResizableDialog.h"

struct MParam;
class CRecipe413;
class CMeParam;

class CSetupProp4Dlg : public CResizableDialog, public CSetupPropComm {
	short CurNo;
	//short nPatSSet;

	float	m_FThkMax[MAXTHKVLU];
	float	m_FThkMin[MAXTHKVLU];
	float	m_RI[MAXTHKVLU];

	// Construction
public:
	void UpdateSelectionItem(CMPoint* pMP);

	void RecipeToLocal();
	void LocalToRecipe();
	//	void LoadDialog(short MeS);

	float RecalculateSpectrum(short which);
	void Renumber();
	void RelocateToTop(CCoor* p);
	void RelocateToBack(CCoor* p);
	void Addzposition(CMPoint* pMP);
	void UpdatePoint(CCoor* p, BOOL bRefresh = TRUE);
	void MatchUpdatePoint(int nSel);
	//	void LoadPoint();
	void SetupColumn();
	void Log(CString msg, short lvl = 5);
	void Message(CString msg);
	void ShowResultUI();
	void NormalizeSpectrum(float* R, int N);
	void DisplaySpectrum(CSpec* pSpec);
	void Deinitialize();
	static DWORD MeasureProcTF(LPVOID pParam);

	// [7/1/2020]
	static UINT RunRecipeProcTF(LPVOID pParam);
	void SaveData(CRecipe413* pRcp);
	// ==========

	void StopOOI();
	void StartOOI();
	void SwapMeSet(short next);
	BOOL Initialize();
	void SaveParam();
	void LoadParam();
	CSetupProp4Dlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupProp4Dlg)
	enum { IDD = IDD_SETUPPROP4 };
	CButton	m_cAutoExpose;
	CStatic	m_cMouseMessage;
	CStatic	m_cSelectedPt;
	CButton	m_cMicroscopeLightOff;
	CButton	m_cMicroscopeLightOn;
	CButton	m_cCameraSetting;
	CButton	m_cAutoFocus;
	CButton	m_cMatch;
	CListBox	m_cList3;
	CStatic	m_cResult3;
	CStatic	m_cResult2;
	CListCtrl	m_cPoint;
	CWaferMap	m_cWaferMap;
	CListCtrl	m_cList;
	CButton	m_cGotoZeroXY;
	CButton	m_cReferencePos;
	CButton	m_cBackgroundPos;
	CButton	m_cMeasure;
	CButton	m_cReference;
	CButton	m_cBackground;
	CStatic	m_cResult;
	CButton	m_cPositionTF;
	CSpecPlot	m_cSpec;
	CSICam	m_cVideo;
	//}}AFX_DATA
// 	float	m_ThkMax[MAXTHKVLU];
// 	float	m_ThkMin[MAXTHKVLU];
// 	float	m_RI[MAXTHKVLU];

// 	float m_Poly;

// 	int	m_Nwave;
	float m_nLmin;
	float m_nLmax;

	//	float* wave;
	float Rmeasmin;
	float Rmeasmax;

	HANDLE m_hThread; // OO thread for measuring continuously

	BOOL m_bStopFlag;

	short nSel;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp4Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp4Dlg)
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet3();
	afx_msg void OnSet4();
	afx_msg void OnSet5();
	afx_msg void OnSet6();
	afx_msg void OnSet7();
	afx_msg void OnSet8();
	afx_msg void OnSet9();
	afx_msg void OnBackground();
	afx_msg void OnReference();
	afx_msg void OnStart();
	afx_msg void OnReset();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnSaverecipe();
	afx_msg void OnCamerasetting();
	afx_msg void OnMotioncontrol3();
	afx_msg void OnMeasure();
	afx_msg void OnTakepicture();
	afx_msg void OnMicroscopelighton2();
	afx_msg void OnMicroscopelightoff2();
	afx_msg void OnSpectrometerlighton2();
	afx_msg void OnSpectrometerlightoff2();
	afx_msg void OnSavespectrum();
	afx_msg void OnDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLocations();
	afx_msg void OnRclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMatch();
	afx_msg void OnAutofocus1();
	afx_msg void OnMatchpattern();
	afx_msg void OnCoordinates();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRefresh();
	afx_msg void OnPatset1();
	afx_msg void OnPatset2();
	afx_msg void OnPatset3();
	afx_msg void OnPatset4();
	afx_msg void OnPatset5();
	afx_msg void OnPatset6();
	afx_msg void OnPatset7();
	afx_msg void OnPatset8();
	afx_msg void OnPatset9();
	afx_msg void OnMeasure2();
	afx_msg void OnAutoexpose();
	afx_msg void OnKeydownList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusFthkminSp4();
	afx_msg void OnKillfocusFthkmaxSp4();
	afx_msg void OnKillfocusRi1();
	afx_msg void OnKillfocusFthkminSp5();
	afx_msg void OnKillfocusFthkmaxSp5();
	afx_msg void OnKillfocusRi2();
	afx_msg void OnKillfocusFthkminSp6();
	afx_msg void OnKillfocusFthkmaxSp6();
	afx_msg void OnKillfocusRi3();
	afx_msg void OnKillfocusFthkminSp7();
	afx_msg void OnKillfocusFthkmaxSp7();
	afx_msg void OnKillfocusRi4();
	afx_msg void OnKillfocusFthkminSp8();
	afx_msg void OnKillfocusFthkmaxSp8();
	afx_msg void OnKillfocusRi5();
	afx_msg void OnKillfocusFthkminSp9();
	afx_msg void OnKillfocusFthkmaxSp9();
	afx_msg void OnKillfocusRi6();
	afx_msg void OnUp();
	afx_msg void OnDn();
	afx_msg void OnHere();
	afx_msg void OnUpdatept();
	afx_msg void OnAddzoffset();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRunrecipe();
	afx_msg void OnPatsseta();
	afx_msg void OnPatssetb();
	afx_msg void OnPatssetc();
	afx_msg void OnPatssetd();
	//}}AFX_MSG
	long OnDeletepoint(WPARAM wP, LPARAM lP);
	long OnRelocateToTop(WPARAM wP, LPARAM lP);
	long OnRelocateToBack(WPARAM wP, LPARAM lP);
	long OnEditpoint(WPARAM wP, LPARAM lP);
	long OnAddpoint(WPARAM wP, LPARAM lP);
	long OnAddalgnpoint(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
};
