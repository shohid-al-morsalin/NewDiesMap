#pragma once
// SetupProp31Dlg.h : header file
//
#include "afxwin.h"

#include "SetupPropComm.h"

#include "LineChart.h"
#include "EchoDraw.h"
#include "ScanLineChart.h"
#include "..\SRC\ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp31Dlg dialog

class CScanLine;
class CRecipe413;
class CInfoPanelMain;

// Raw data [7/14/2012 Yuen]
class CEchoRaw {
public:
	float BLT, BLB;	// Top and bottom baseline [7/15/2012 Yuen]
	int* pPosA, Len;
	unsigned short* pS1, * pS2, * pA, * pB;

	CEchoRaw() {
		pPosA = NULL;
		pS1 = pS2 = pA = pB = NULL;
		Clear();
	}

	~CEchoRaw() {
		Clear();
	}

	BOOL IsOK() {
		if (!pPosA || !pS1 || !pS2 || Len < 1) {
			return FALSE;
		}
		return TRUE;
	}

	void Alloc(int siz) {
		Clear();
		Len = siz;
		siz += 16;
		pPosA = new int[siz];
		pS1 = new unsigned short[siz];
		pS2 = new unsigned short[siz];
		pA = new unsigned short[siz];
		pB = new unsigned short[siz];
	}

	void Clear() {
		Len = 0;
		BLT = BLB = 2047;

		if (pPosA) {
			delete[] pPosA; pPosA = NULL;
		}
		if (pS1) {
			delete[] pS1; pS1 = NULL;
		}
		if (pS2) {
			delete[] pS2; pS2 = NULL;
		}
		if (pA) {
			delete[] pA; pA = NULL;
		}
		if (pB) {
			delete[] pB; pB = NULL;
		}
	}
};

class CSetupProp31Dlg : public CResizableDialog, public CSetupPropComm {
	// Construction
public:
	int Sign(int v);
	void RefreshChart();
	void CopyData3(CRecipe413* pRcp, CScanLine* pSLineTT, CScanLine* pSLineBB, CEchoRaw* pRaw);
	void ZoomPosHandler(UINT nSBCode, UINT nPos);
	void ScrollPosHandler(UINT nSBCode, UINT nPos);
	BOOL LoadCSV(CEchoRaw* pRaw, CString name);
	CSetupProp31Dlg(CWnd* pParent = NULL);   // standard constructor

	short Show;
	CEchoRaw Raw1, Raw2;
	CScanLine* pSLineT, * pSLineB;

	// Dialog Data
		//{{AFX_DATA(CSetupProp31Dlg)
	enum { IDD = IDD_SETUPPROP31 };
	CListCtrl	m_cList;
	CEchoDraw	m_cEchoPeaks;
	CScrollBar	m_cZoomPos;
	CScrollBar	m_cScrollPos;
	CStatic	m_cPosition;
	CStatic	m_cZoomTx;
	CLineChart	m_Chart;
	CLineChart m_Chart2;
	//}}AFX_DATA
	CScanLineChart m_cSLChart;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp31Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp31Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnFindpeaks();
	afx_msg void OnLoadrecipe();
	afx_msg void OnClearcontent();
	afx_msg void OnSwitch();
	afx_msg void OnDump();
	afx_msg void OnRadio1();
	afx_msg void OnRadio3();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnSimulatedpeaks();
	//}}AFX_MSG
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	DECLARE_MESSAGE_MAP()
public:
	void LocalToRecipe() {};
	void RecipeToLocal() {};

	void SmoothPos(int** pos, int len);
	void SaveSpec(BOOL bSave);
	afx_msg void OnBnClickedLoad3();
	afx_msg void OnBnClickedClean();
	CEdit m_cBaseline1;
	CEdit m_cBaseline2;
	void CopySL(CScanLine* pSLine, short Method, CEchoRaw* pRaw, unsigned short* pS, short BL, short Level, CString DumpFile);
};
