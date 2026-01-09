#pragma once

// ImgWnd.h : header file
//

#include <afxmt.h>
#include "sidefs.h"
#include "PackedDIB1.h"

/////////////////////////////////////////////////////////////////////////////
// CImgWnd window

struct SLnX {
	float angle, offset;
	float score, maxderiv;	// maximum derivative [6/8/2013 Administrator]

	SLnX() {
		Clear();
	}

	void Clear() {
		angle = 0;
		maxderiv = 0;
		offset = score = 0;
	}
	void Copy(SLnX* co) {
		angle = co->angle;
		offset = co->offset;
		score = co->score;
		maxderiv = co->maxderiv;
	}
};

class CRecipe413;

class CImgWnd : public CStatic {
	float* pAve;
	CRecipe413* pRcp;
	CCriticalSection CS;

public:
	CImgWnd();
	virtual ~CImgWnd();

public:
	enum FNDMTHD {
		DARKFWD, DARKBWD, BRITFWD, BRITBWD
	};

	CPackedDIB* pPackedDIB1 = nullptr;	// Operating and display [4/13/2013 Administrator]
	CPackedDIB* pPackedDIB2 = nullptr;	// Backup for reloading PackedDIB1 [4/13/2013 Administrator]
	CPackedDIB* pOverlayDIB1 = nullptr, * pOverlayDIB2 = nullptr;
	CString SaveName = "";
	BOOL bStoreComposite = FALSE, bScaled = FALSE, bShowOverlay = FALSE, bShowScale = FALSE;
	int Oly1CX = 0, Oly1CY = 0, Oly2CX = 0, Oly2CY = 0;  // Coordinate of center of Overlay images with respect to full image [9/13/2011 Administrator]
	short PointNum = 0;
	float fAve = 0, fStdev = 0;
	float fTWEdge1 = 0, fTWEdge2 = 0, TWRadius = 0, TWWidth = 0, TWBevel = 0, TWAngle = 0;
	int nTWEdge1 = 0, nTWEdge2 = 0;	// Trim width edge offset in pixel [5/31/2013 Yuen]
	CString FileName, FileBase, FileDirc;
	BOOL bStore, bSaveDIB;

	BOOL bAllowMoveXHair;
	BOOL bSetRedXHair;
	BOOL bAutoExpose;
	BOOL bDispImage;
	BOOL bPause;
	BOOL bRet;
	BOOL bSnapRet;
	BOOL bChangeExposure;

	BOOL bYFlip;
	short ImgWd, ImgHt;


public:

	virtual BOOL DestroyWindow();

public:
	//struct SPont {
	//	short x, y, v;
	//	SPont() : x(0), y(0), v(0) {};
	//	SPont(float _x, float _y, short _v) {
	//		x = _x; y = _y; v = _v;
	//	}
	//};
	//struct SLine {
	//	float r, a;
	//	std::vector<SPont> P;
	//};
	short ExtractLine(int& idx, short* pBuf, int nBuf, float ang, short offset, BOOL& bX/*, BYTE r, BYTE g, BYTE b*/);
	bool FindColor(int& idx, int nBuf, float ang, short offset, BOOL& bX, short& left, short& right);
	short DetectBevel(int& idx, short* pBuf, int nBuf, float ang, short offset, BOOL& bX, short left, short right);
	void ReplaceLine(short* pBuf, float ang, short offset, BOOL& bX);
	short FindEdge(SLnX* pL1, SLnX* pL2, int& In, int& Out, float angle, FNDMTHD Mth, float Thres);
	short FindEdgeEx(SLnX* pL1, SLnX* pL2, int& In, int& Out, float angle, FNDMTHD Mth, float Thres);

public:
	void ClearOverlay();
	float FindAngle(float* pV, short nS, float max, float min, short nL, float sf);
	float Average2(float* pVal, int nS, float max, float min, float sf);
	float Average1(float* pVal, int nS, float max, float min, float sf);
	float AverageOf2Angles(float* pVal, int nS, float max, float min, float sf);
	BOOL FindEdge360(float* pV, float angle, int siz, short* pBuf, /*float *pScore,*/ short nL/*, SLnX *pL*/);
	float GetAngle360(short Ang);
	void ClearTextInfo();
	void SetLineColor(float ang, float offset, BYTE R, BYTE G, BYTE B);
	BOOL Smooth(float* pBuf, short len);
	BOOL Integrate(float* pBuf, short len);
	BOOL LoadDIB();
	float ScoreLine1(short* pBuf, short len);
	BOOL FindEdgeMax(float* pV, float angle, int siz, short* pBuf, short nL);
	BOOL LoadDIB(CString fname);
	void SetAve(float* pAve);
	void SetRecipe(CRecipe413* pRcp);
	int GetBytesPerRow();
	void Draw(CDC* pDC);
	void StoreComposite(CBitmap& bmp, CString name);
	void ShowEchoView(CDC* pDC, CRect& rc, short wd);
	void Save(CString& name);
	void Clean();
	CPackedDIB* CreateDIB(int wd, int ht, int bpp);
	void Refresh();
	void ClearOly(int n);
	void ClearDIB();
	void ClearDIB2();
	void DupStore1(CPackedDIB& DIB);
	void DupStore2(CPackedDIB& DIB);
	void Add(CPackedDIB* pDIB);
	void UpdateLineInfo(float ang, float offset, bool* lineInfo);
	// Generated message map functions
protected:
	//{{AFX_MSG(CImgWnd)
	afx_msg void OnPaint();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	BOOL ShowImage(CDC* pDC, SIPackedDIB* pPackedDIB);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
