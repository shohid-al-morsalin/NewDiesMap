#if !defined(AFX_MEASUREPHASE_H__FAC80F49_55E4_4703_8D62_B303A5A5F9D2__INCLUDED_)
#define AFX_MEASUREPHASE_H__FAC80F49_55E4_4703_8D62_B303A5A5F9D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// measurephase.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeasurePhase dialog

#include "Roll.h"
// #include "..\CameraView.h"

#define MAX_PHASE_IMG 5

struct DIBINFO : public BITMAPINFO
{
	RGBQUAD	 arColors[255];    // Color table info - adds an extra 255 entries to palette

	operator LPBITMAPINFO()          { return (LPBITMAPINFO) this; }
	operator LPBITMAPINFOHEADER()    { return &bmiHeader;          }
	RGBQUAD* ColorTable()            { return bmiColors;           }
};


//class CSystem;

class CMeasurePhase : public CDialog
{
// Construction
public:
	CMeasurePhase(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMeasurePhase();
// Dialog Data
	//{{AFX_DATA(CMeasurePhase)
	enum { IDD = IDD_PHASE_UNWRAP };
	double m_fZStepSize;
	double m_fRMSThreshold;
	double m_fMaskThreshold;
	int m_nDirection;
	double m_fFilmThk;
	double m_fSubThk;
	double m_fYoungMod;
	double m_fPoisson;
	double m_fDiam;
	double m_fX1;
	double m_fY1;
	double m_fX2;
	double m_fY2;
	double m_fX3;
	double m_fY3;
	double m_fImageMin;
	double m_fImageMax;
	int m_nImageType;
	CString m_chFileName;
	CString m_chRefFileName;
	int m_nMarginX1;
	int m_nMarginX2;
	int m_nMarginY1;
	int m_nMarginY2;
	int m_nCenterX;
	int m_nCenterY;
	int m_nRepeat;
	int m_nRadius;
	int m_nMeasRepeat;
	BOOL m_b3DImage;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeasurePhase)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	virtual BOOL SaveImage(double *pImage, LPCTSTR lpFileName);
	virtual BOOL SaveImage(LPBYTE pImage, LPCTSTR lpFileName);
	virtual double GetRoughness(LPBYTE pData, double *pResult, int iStartX, int iStartY, int iXDim, int iYDim);
	virtual double fGetRMSValue(LPBYTE pData);
	virtual BOOL DoKernelFilter(int *iFilter, int iDim, LPBYTE pSrc, LPBYTE pDest);
	virtual BOOL DoKernelFilterDbl(int *iFilter, int iDim, double* pSrc);
	virtual void FilterPhase(int m);

	virtual void FlattenX();
	virtual void LevelShift(int nAxis);
	virtual void SetupMask();
	virtual void SetBitmap2(LPBYTE pData);
	virtual void SetBitmap(int nImage, BOOL bType);
	virtual void SetColorBar();

	virtual void CalculateStress();
	virtual void SetCCDCorrection(int Xdim, int Ydim, StressPoint* fCorr);
	virtual void CorrectCCDMatrix(int Xdim, int Ydim, StressPoint* p1, StressPoint* p2);
	virtual void PutImage(int N, StressPoint* p); 
	
	void EnableButtons(BOOL bEnable);
	void Gotoxy(float fX, float fY); 
	void GotoZ(float fZ); 
	float GotoPZT(float fPZT);
	float GetPosition(int nAxis); 
	BOOL DestroyWindow();
	void Unwrap();
	void UnwrapPart2(BOOL bStress, float* pfData);
	void UnwrapPart2b(BOOL bStress);
	void GetPlaneEquation(StressPoint *fP, double * fResult);
	void SubtractBasePlane(StressPoint *fP);
	void LoadParam();
	void SaveParam();
	BOOL ReadBmpFiles();
	void MeasureImages();
	void UnwrapMask();
	void SearchInterference();
	void GetRMS();
	double GetRMSValue2();
	void GetTopography(StressPoint* p);
	CCameraView* pView;	
	int m_XDim, m_YDim;
    HBITMAP  m_hBitmap1;          // Handle to DIBSECTION
    DIBINFO  m_DIBinfo1;          // Bitmap header & color table info
    VOID    *m_ppvBits1;          // Pointer to bitmap bits
	LPBYTE m_pImg[MAX_PHASE_IMG];
	//Colour bar
    HBITMAP  m_hBitmap2;          // Handle to DIBSECTION
    DIBINFO  m_DIBinfo2;          // Bitmap header & color table info
    VOID    *m_ppvBits2;          // Pointer to bitmap bits
	//
	double *m_pPhase;
	double *m_pR;
	double *m_pUnwrapped;
	double *m_pUnwrapped2;
	double *m_pInt; // intensity
	double *m_pVis; // visibility
	BOOL   *m_pMask;
	int m_Count;
	double m_dfSpeed;
	StressPoint* fCorrect;
	CRoll Calc;
	BOOL bSimu;
	int nPiezoDelay;
	BOOL bRefMeas;
	double fRMS;
	BOOL bStressInitialized;
	BOOL bSetupMask;
	int nAlgorithm;
	float fWavelength; 
	BOOL bSaveData;
	BOOL bSaveImage;
	int nRefPlaneType;
	int nPZTPosType;
	float fWarp;
	float fBowHeight;
	float fCurvature;
	float fStress;
//protected:
//	CSystem *m_pSystem;

// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CMeasurePhase)
	afx_msg void OnUnwrapY();
	afx_msg void OnUnwrapX();
	afx_msg void OnPaint();
	afx_msg void OnSaveImage();
	afx_msg void OnMoveLeft();
	afx_msg void OnMoveRight();
	afx_msg void OnClose();
	afx_msg void OnUnwrapProcess();
	afx_msg void OnKillfocusImageMin();
	afx_msg void OnKillfocusImageMax();
	afx_msg void OnKillfocusImageType();
	afx_msg void OnKillfocusMarginX1();
	afx_msg void OnKillfocusMarginY1();
	afx_msg void OnKillfocusMarginX2();
	afx_msg void OnKillfocusMarginY2();
	afx_msg void OnKillfocusDiameter();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void On3DImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MEASUREPHASE_H__FAC80F49_55E4_4703_8D62_B303A5A5F9D2__INCLUDED_)
