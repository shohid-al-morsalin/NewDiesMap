#pragma once
// SICam.h : header file
//
#include "sidefs.h"
#include "CamBase.h"
#include "PackedDIB1.h"
#include "..\413\OceanOp\Spec.h"
#include "..\413\EchoScope\ScanLine.h"
#include "..\413\EchoScope\ScanPoint.h"

#include "SICamPoints.h"

// Aspect ratio of SiCam = 640/512 [5/22/2014 FSMT]
#define CAMASPECTRATIO 1.25f

/////////////////////////////////////////////////////////////////////////////
// CSICam window

class CPeaksBuf;
class CEchoPeak;
class CRecipe413;

class CSICam : public CStatic {
	CCriticalSection CS;
	CSICamPoints SIPoints;

	BOOL bMMove;
	float* pAve;
	CRecipe413* pRcp;

	// Construction
public:

	CCamBase* pCam;
	static CCamBase* pCamTF;
	static CCamBase* pCamECHO;
	static SIU_Config SIConfigEcho, SIConfigTF;
	static SIU_CaptureContext CapCxtEcho, CapCxtTF;

	CSICam();

	static CString Reserved;

	BOOL bStore, bSaveDIB, bStoreComposite;

	BOOL bAllowMoveXHair;
	BOOL bSetRedXHair;
	BOOL bAutoExpose;
	BOOL bDispImage;
	BOOL bPause;
	BOOL bRet;
	BOOL bSnapRet;
	BOOL bChangeExposure;
	float fStdev, fAve;
	BOOL bYFlip;

	//short StoreCount;

	CCriticalSection CS1, CSSLine;

	// Attributes
public:

	CWnd* pWnd;
	CSpec RsltSpec;

	int frame;
	short dir, Index;

	int nGain;
	int SnapRsl, SnapLimit;
	int ScanLineReady;

	BOOL bScaled;
	BOOL bDisableScaling;

	CScanLine* pSLineT, * pSLineB;
	CString FileName, FileBase, FileDirc;

	CCamBase::CAMERAID iCID;// Camera ID, ECHOCAMERA or THINFILMCAMERA [7/18/2012 Yuen]
	short iCNum;		// Camera number [7/18/2012 Yuen]
	short Level;
	short iExpose;
	//	CCriticalSection CSXpose;

	HANDLE hLiveThread;
	SIU_CaptureContext* pCapCxt;

	short XX;				// temp variable [8/11/2010 XPMUser]
	short ImgWd, ImgHt;
	SIPackedDIB SimuDIB;	// temp variable [8/15/2010 XPMUser]
	CPackedDIB FocusDIB;

	BYTE* pSwapBuf;
	CPeaksBuf* pPeakBuf;

	BOOL bBlt;
	BOOL bShow;
	BOOL bSaveVideo;
	BOOL bRun, bStop;
	BOOL bDisplayOverlay, bDisping;
	BOOL bClearRsltSpec;
	BOOL bShowCrossHair;
	BOOL bShowCenterTarget;

	BOOL bVideoOK;

	CString Name;
	static CString SimuDIBFilename;

	float PaOffX, PaOffY;
	float fTGtGrnX, fTGtGrnY;
	float fTGtBluX, fTGtBluY;

	// Operations
public:

private:
	void SleepEx(DWORD tout);
	void SaveSetting();
	void LoadSetting();
	BOOL ProcessStoreDIB(SIPackedDIB* pPackedDIB);
	void ProcessExposure(SIPackedDIB* pPackedDIB);
	void AppltSetting();
	BOOL ShowImage(CDC* pDC, SIPackedDIB* pPackedDIB);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSICam)
		//}}AFX_VIRTUAL

	// Implementation
private:
	void ResizeWindow();
	BOOL ProcessSaveDIB(SIPackedDIB* pPackedDIB);

	void ShowRedTarget(CDC& dcMem, CRect& rc);
	void ShowBlueTarget(CDC& dcMem, CRect& rc);
	void ShowGreenTarget(CDC& dcMem, CRect& rc);
	void ShowCenterTarget(CDC& dcMem, CRect& rc);
	void ClearTextInfo();
	void LoadImage();
	BOOL GetAngle(float& angle);
	float GetZoom(short N);
	void DrawHisto(CDC* dcMem, CRect& rc);
	void ChangeExposure(BOOL bWait = TRUE);
	void StorePeakInfo();
	void GetMaxMin(float* pWave, int count, float& Max, float& Min);
	void StoreComposite(CBitmap& bmp, int cnt = 0);
	void ShowTFView(CDC* pDC, CRect& rc);
	void ShowEchoView(CDC* pDC, CRect& rc);
	void FlipImageY(SIPackedDIB* pSIDIB);
	void DisplayOvelays(CDC& dcMem, CRect& rc);
	void AdjustExposure();

public:
	int AutoAdjust();
	void ClearEchoInfo();

	void RSpecClear();
	BOOL AllocSLine();

	void LoadSimuDIB(short n);
	void LoadSimuDIB(CString filename);
	BOOL SnapOneFrame();
	BOOL DoubleSnapOneFrame();
	BOOL SnapOneFrame(short Exposure);

	BOOL IsSimu();
	BOOL IsReserved();

	short GetExposure();

	void SetAve(float* pAve);
	void SetRecipe(CRecipe413* pRcp);
	void SetExposure(short exposure, BOOL bWait = TRUE);

public:
	int GetCount();
	int GetCount(SIPackedDIB* pPackedDIB);
	void LoadStockDIB(CString name);
	static BOOL StartEchoCamera();
	static BOOL StartThinFilmCamera();
	static BOOL Deinitialize(CCamBase* pCamera);
	static BOOL Initialize(CListBox* pMessage, CCamBase* pCamera, CCamBase::CAMERAID ID);
	static DWORD WINAPI LiveThreadFunc(LPVOID pParam);

	virtual ~CSICam();

	// Generated message map functions

	CPoint textout;
	int diameter;
protected:
	//{{AFX_MSG(CSICam)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// [06182024 ZHIMING // 12182024
	BOOL bXFlip;
	void FlipImageX(SIPackedDIB* pSIDIB);
	// ]
};