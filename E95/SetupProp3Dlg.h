#pragma once
// SetupProp3Dlg.h : header file
//

#include "SetupPropComm.h"
#include "SICam.h"
#include "WaferMap.h"
#include "MParam.h"
#include "EchoDraw.h"
#include "HistoDraw.h"
#include "..\413\EchoFinder.h"
#include "..\413\EchoScope\EchoProbeBase.h"
#include "..\SRC\ResizableDialog.h"
#include "CWL.h"

/////////////////////////////////////////////////////////////////////////////
// CSetupProp3Dlg dialog
struct SPROBEDISTPARAM {
	short n;		// Sequence [3/21/2013 Yuen]
	float dist;		// Z distance move per step [3/21/2013 Yuen]
	short nstep;	// Number of steps [3/21/2013 Yuen]
};

class CMPoint;
class CScanLine;
class CInfoPanelMain;
class COptimizeEchoDlg;

class CSetupProp3Dlg : public CResizableDialog, public CEchoFinder, public CSetupPropComm {
	COLORREF m_bgColor;
	COLORREF m_lnColor1;
	COLORREF m_lnColor2;
	COLORREF m_grColor;
	COLORREF m_txtColor;

public:
	enum NDISPLAY {
		ECHO1, CHNA, CHNB, ECHO2, POSN
	};
	NDISPLAY nDisplay;

	SPROBEDISTPARAM ProbDistParam;

	FILE* pDump;
	CWinThread* pDryRunThread;

	short Show, SimuPkBak;
	BOOL bDoMeasurement;
	short DumpCounter;
	short CalCount;
	float SumT;
	float SumB;
	//BOOL bStop;

	BOOL bDispHisto;// Display histogram [6/15/2011 XPMUser]
	short iHisto;	// Item to show [6/15/2011 XPMUser]
	// sample average buffer [10/27/2010 FSMT]
	float fThknBuf[MaXPA][MAXAVEBUFF]; // buffer to calculate running standard deviation [4/16/2013 Administrator]
	int nThknBuf;

	float m_ThresholdFactor[2];
	short m_Level[2];
	short m_PeakWidth[2];
	short code[MAXMEPASET * MaXPA];

	// Construction
public:
	void RecipeToLocal();
	void LocalToRecipe();
	float GetValue(char* key, CString* pStr, int NLines);
	int GetNbLine(char* pBuf, int FSize);
	void OptimizeEcho(COptimizeEchoDlg* pDlg, int NLines, CString* pStr);
	CString GetOneLine(char* pBuf, int& FilePointer, int FSize);
	void DoOptimizeEcho();
	void ExecuteProbeDistanceTest();
	short GetPAIdx(TCHAR ch);
	void Renumber();
	void ExportWithBaseline();
	void StartingEcho();
	void StoppingEcho();
	void RelocateToTop(CCoor* p);
	void RelocateToBack(CCoor* p);
	void UpdatePoint(CCoor* p);
	void SwapMeSet(short next);
	void Dpposition();
	void StartMotor();
	void StopMotor();
	void CalAveStdev(short n);
	void DumpValues();
	void StaticRepeatabilityTest();
	BOOL MeasureOneWafer(CRecipe413* pRcp);
	void UpdatePosition();
	BOOL DoMeasurement();
	static UINT ExecuteRecipe(LPVOID pObj);
	void SaveBitmap(CString Name, CBitmap& bmp);
	void PlotPeaks(CDC* pDC, CRect& rec, int nNumberOfData, int DispChn, int Scale, BOOL bBothChn = FALSE);
	void ResizeRect(CRect& rc);
	void UpdateThkn(int i, float& thkn);
	BOOL StopEcho();
	BOOL StartEcho();
	void Log(CString msg, short lvl = 5);
	void PostDrawChartMessage(void* pObject);
	void PostErrorMessage(CString MsgStr);
	void RefreshMeasurement();
	CSetupProp3Dlg(CWnd* pParent = NULL);   // standard constructor
	~CSetupProp3Dlg();	

	// Dialog Data
		//{{AFX_DATA(CSetupProp3Dlg)
	enum { IDD = IDD_SETUPPROP3 };
	CButton	m_SaveCalib;
	CButton	m_cOverlayInfo;
	CButton	m_cOverlay;
	CButton	m_cSleep;
	CButton	m_cClearPC2;
	CButton	m_cClearPC1;
	CButton	m_cStop;
	CButton	m_cClearSF;
	CStatic	m_cGenMessage;
	CButton	m_cSaveVideo;
	CButton	m_cDianostic;
	CEdit	m_cSimuPk;
	CStatic	m_cMouseMessage;
	CButton	m_cSetFocusPoint;
	CButton	m_cDoFocus;
	CStatic	m_cSetFocus;
	CButton	m_cDPCalib;
	CButton	m_cSPCalib;
	CButton	m_cTakePicture;
	CButton	m_cCameraSetting;
	CHistoDraw	m_cHisto;
	CStatic	m_cTime;
	CButton	m_cRun;
	CButton	m_cMeSet;
	CSICam	m_cVideo;
	CStatic	m_cMessage;
	CEchoDraw	m_cEchoPeaks;
	CWaferMap	m_cWaferMap;
	CStatic	m_cCounter;
	CButton	m_cStartEcho;
	BOOL	m_bPlot;
	int		m_nScale;
	float	m_ri;
	short	m_SimuPk;
	short	m_NAve;
	//}}AFX_DATA

	double XMIN, YMIN, XMAX, YMAX;

	CString	Thk[MaXPA];
	CString	Name[MaXPA];
	float	Ave[MaXPA];
	// 	float	Ave2[MaXPA];  // Intermediate, used to calculate stdev only [12/16/2010 C2C]
	float	Stdev[MaXPA];

	// 	CInfoPanelMain * m_Parent;

	BOOL bReal;
	BOOL bDPCalib, bDPCalibTest, bSPCalib, bSPCalibTest;
	float SPCalibThick;
	BOOL bSaveChart;

	CBitmap bmp;
	BOOL bAllcBMP;
	int WdBk, HtBk;

	BOOL bSetFocus, bDoFocus, bEchoStarted, bClearFocus;

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSetupProp3Dlg)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupProp3Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnMotioncontrol();
	afx_msg void OnNewrecipe();
	afx_msg void OnStartecho();
	afx_msg void OnTopprobe();
	afx_msg void OnBottomprobe();
	afx_msg void OnEdposition();
	afx_msg void OnA();
	afx_msg void OnB();
	afx_msg void OnDrawchart();
	afx_msg void OnDpcalib();
	afx_msg void OnSpcalib();
	afx_msg void OnPlot();
	afx_msg void OnRealunit();
	afx_msg void OnDryrun();
	afx_msg void OnStoprun();
	afx_msg void OnClose();
	afx_msg void OnMark();
	afx_msg void OnTest();
	afx_msg void OnCamerasetting();
	afx_msg void OnMakebaseline();
	afx_msg void OnKillfocusAirbot();
	afx_msg void OnKillfocusAirtop();
	afx_msg void OnSaveencoderfactor();
	afx_msg void OnTakepicture2();
	afx_msg void OnKillfocusNave();
	afx_msg void OnKillfocusEdscale();
	afx_msg void OnKillfocusRi();
	afx_msg void OnSet1();
	afx_msg void OnSet2();
	afx_msg void OnSet4();
	afx_msg void OnSet5();
	afx_msg void OnSet6();
	afx_msg void OnSet7();
	afx_msg void OnSet8();
	afx_msg void OnSet9();
	afx_msg void OnSet10();
	afx_msg void OnStarthisto();
	afx_msg void OnStophisto();
	afx_msg void OnConthisto();
	afx_msg void OnLoadhisto();
	afx_msg void OnKillfocusThresholdfactor3();
	afx_msg void OnKillfocusThresholdfactor2();
	afx_msg void OnKillfocusRcpLevel1();
	afx_msg void OnKillfocusRcpLevel2();
	afx_msg void OnKillfocusRcpPeakwidth1();
	afx_msg void OnKillfocusRcpPeakwidth2();
	afx_msg void OnSetfocuspoint();
	afx_msg void OnDofocus();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKillfocusSimupk();
	afx_msg void OnLocations2();
	afx_msg void OnRefreshui();
	afx_msg void OnDianostic();
	afx_msg void OnSavevideo2();
	afx_msg void OnClearbaseline();
	afx_msg void OnClearaf();
	afx_msg void OnClearpc();
	afx_msg void OnClearpc2();
	afx_msg void OnSleep();
	afx_msg void OnCleardata();
	afx_msg void OnViewdata();
	afx_msg void OnOverlayinfo();
	//}}AFX_MSG

	long OnRelocateToTop(WPARAM wP, LPARAM lP);
	long OnRelocateToBack(WPARAM wP, LPARAM lP);
	long OnTabSelected(WPARAM wP, LPARAM lP);
	long OnTabDeselected(WPARAM wP, LPARAM lP);
	long OnMoMove(WPARAM wP, LPARAM lP);
	long OnButClick(WPARAM wP, LPARAM lP);
	long OnAddpoint(WPARAM wP, LPARAM lP);
	long OnAddalgnpoint(WPARAM wP, LPARAM lP);

	DECLARE_MESSAGE_MAP()
};
