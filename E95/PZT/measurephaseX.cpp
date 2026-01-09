// measurephase.cpp : implementation file
//

#include "stdafx.h"
#include "..\E95.h"
#include "..\InterfaceTmptCtrl.h"
#include "measurephase.h"
//#include "..\HWControl\system.h"
//#include "..\HWControl\scanner.h"
//#include "..\HWControl\motionctrlbase.h"
//#include "..\SI3300CamDlg.h"
#include "..\MainFrm.h"
//#include "../../IPC/SMC.h"	// Added by ClassView
#include "fpdaq.h"
#include <math.h>

// #include "AppGlobal.h"
// #include "AppViews.h"
// 
// #include "S8Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeasurePhase dialog

#define IMG_STEP 1
#define THRESHOLD_RATIO 0.1
#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')    // is always "BM" = 0x4D42
#define ZPOSMAX 12

CMeasurePhase::CMeasurePhase(CWnd* pParent /*=NULL*/)
	: CDialog(CMeasurePhase::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMeasurePhase)
		// NOTE: the ClassWizard will add member initialization here
		m_fZStepSize = 0.062/4; //100um corresponds to 10V, we want step size 5um
		m_nDirection = 0;
		m_fRMSThreshold = 4.7;
		m_fMaskThreshold = 0.1;
		m_fSubThk = 775;
		m_fFilmThk = 1000;
		m_fYoungMod = 130;
		m_fPoisson = 0.26;
		m_fDiam = 50;
		m_fX1 = 0;
		m_fY1 = 0;
		m_fX2 = 0;
		m_fY2 = 0;
		m_fX3 = 0;
		m_fY3 = 0;
		m_chFileName = _T("");
		m_chRefFileName = _T("");
		m_fImageMin = -10.0;
		m_fImageMax = 10.0;
		m_nImageType = 0;
		m_nMarginX1 = 0;
		m_nMarginX2 = 0;
		m_nMarginY1 = 0;
		m_nMarginY2 = 0;
		m_nCenterX = 512/2;
		m_nCenterY = 384/2;
		m_nRepeat = 5;
		m_nRadius = 192;
		m_nMeasRepeat = 1;
		m_b3DImage = FALSE;
	//}}AFX_DATA_INIT
	m_XDim = 512;//2048;//512;
	m_YDim = 384;//1536;//400;
	m_hBitmap1 = NULL;
	m_ppvBits1  = NULL;
	m_hBitmap2 = NULL;
	m_ppvBits2  = NULL;
	m_pPhase = NULL;
	m_pR = NULL;
	m_pUnwrapped = NULL;
	m_pUnwrapped2 = NULL;

	m_Count = 0;
	bRefMeas = FALSE;

	for (int i = 0; i < MAX_PHASE_IMG; i++)
		m_pImg[i] = new BYTE[m_XDim*m_YDim];

	m_pInt  = NULL;
	m_pVis  = NULL;
	m_pMask = NULL;
	fCorrect = NULL;

	m_dfSpeed = 10.0;
	bSimu = FALSE;

	int iSize = m_XDim * m_YDim;

	m_pPhase = new double[iSize];
	memset(m_pPhase, 0, iSize*sizeof(double));
	m_pR = new double[iSize];
	memset(m_pR, 0, iSize*sizeof(double));
	m_pUnwrapped = new double [iSize];
	memset(m_pUnwrapped, 0, iSize*sizeof(double));
	m_pUnwrapped2 = new double [iSize];
	memset(m_pUnwrapped2, 0, iSize*sizeof(double));
	m_pInt = new double[iSize];
	memset(m_pInt, 0, iSize*sizeof(double));
	m_pVis = new double[iSize];
	memset(m_pVis, 0, iSize*sizeof(double));
	m_pMask = new BOOL[iSize];
	memset(m_pMask, 0, iSize*sizeof(BOOL));
	fCorrect = new StressPoint[iSize];
	for (i = 0; i < iSize; i++)
	{
		fCorrect[i].x = fCorrect[i].y =	1.0;
		fCorrect[i].z = fCorrect[i].W 
			= fCorrect[i].sigmaxx = fCorrect[i].sigmaxy = fCorrect[i].sigmayy 
			= fCorrect[i].sigmarr = fCorrect[i].sigmart = fCorrect[i].sigmatt = 0.0;
	}
	fRMS = 0.0;
	fWavelength = 0.62f;
	bSaveImage = FALSE;
	bSaveData = FALSE;
	nRefPlaneType = 0;
	nPZTPosType = 0;
	bStressInitialized = FALSE;
	bSetupMask = TRUE;
	fWarp = 0;
	fBowHeight = 0;
	fCurvature = 0;
	fStress = 0;
	LoadParam();
	pView = NULL;
}

CMeasurePhase::~CMeasurePhase()
{
	for (int i = 0; i < MAX_PHASE_IMG; i++)
	{
		if (m_pImg[i] ) 
		{
			delete m_pImg[i];
			m_pImg[i] = NULL;
		}
	}
	if (fCorrect)
	{
		delete [] fCorrect;
	}
	if (m_pPhase)
	{
		delete m_pPhase;
		m_pPhase = NULL;
	}
	if (m_pR)
	{
		delete m_pR;
		m_pR = NULL;
	}
	if (m_pUnwrapped2) 
	{
		delete m_pUnwrapped2;
		m_pUnwrapped2 = NULL;
	}
	if (m_pUnwrapped) 
	{
		delete m_pUnwrapped;
		m_pUnwrapped = NULL;
	}
	if (m_pInt)
	{
		delete  [] m_pInt;
		m_pInt = NULL;
	}
	if (m_pVis)
	{
		delete  [] m_pVis;
		m_pVis = NULL;
	}
	if (m_pMask) 
	{
		delete [] m_pMask;
		m_pMask = NULL;
	}
}

void CMeasurePhase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMeasurePhase)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_UNWRAP_STEP_SIZE, m_fZStepSize);
	DDX_Text(pDX, IDC_DIRECTION, m_nDirection);
	DDX_Text(pDX, IDC_RMS_THRESHOLD, m_fRMSThreshold);
	DDX_Text(pDX, IDC_MASK_THRESHOLD, m_fMaskThreshold);
	DDX_Text(pDX, IDC_THK_FILM, m_fFilmThk);
	DDX_Text(pDX, IDC_THK_SUBSTR, m_fSubThk);
	DDX_Text(pDX, IDC_YOUNG_MOD, m_fYoungMod);
	DDX_Text(pDX, IDC_POISSON_COEFF, m_fPoisson);
	DDX_Text(pDX, IDC_UNWRAP_DIAMETER, m_fDiam);
	DDX_Text(pDX, IDC_UNWRAP_X1, m_fX1);
	DDX_Text(pDX, IDC_UNWRAP_Y1, m_fY1);
	DDX_Text(pDX, IDC_UNWRAP_X2, m_fX2);
	DDX_Text(pDX, IDC_UNWRAP_Y2, m_fY2);
	DDX_Text(pDX, IDC_UNWRAP_X3, m_fX3);
	DDX_Text(pDX, IDC_UNWRAP_Y3, m_fY3);
	DDX_Text(pDX, IDC_UNWRAP_FILENAME, m_chFileName);
	DDX_Text(pDX, IDC_UNWRAP_IMAGE_MIN, m_fImageMin);
	DDX_Text(pDX, IDC_UNWRAP_IMAGE_MAX, m_fImageMax);
	DDX_Text(pDX, IDC_UNWRAP_IMAGE_TYPE, m_nImageType);
	DDX_Text(pDX, IDC_UNWRAP_MARGINX1, m_nMarginX1);
	DDX_Text(pDX, IDC_UNWRAP_MARGINX2, m_nMarginX2);
	DDX_Text(pDX, IDC_UNWRAP_MARGINY1, m_nMarginY2);
	DDX_Text(pDX, IDC_UNWRAP_MARGINY2, m_nMarginY1);
	DDX_Text(pDX, IDC_UNWRAP_CENTERX, m_nCenterX);
	DDX_Text(pDX, IDC_UNWRAP_CENTERY, m_nCenterY);
	DDX_Text(pDX, IDC_UNWRAP_REPEAT, m_nRepeat);
	DDX_Text(pDX, IDC_UNWRAP_RADIUS, m_nRadius);
	DDX_Text(pDX, IDC_UNWRAP_MEAS_REPEAT, m_nMeasRepeat);
	DDV_MinMaxInt(pDX, m_nMeasRepeat, 1, 30000);
	DDX_Check(pDX, IDC_UNWRAP_3DIMAGE, m_b3DImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMeasurePhase, CDialog)
	//{{AFX_MSG_MAP(CMeasurePhase)
 	ON_BN_CLICKED(IDC_UNWRAP_Y, OnUnwrapY)
	ON_BN_CLICKED(IDC_UNWRAP_X, OnUnwrapX)
	ON_BN_CLICKED(IDC_UNWRAP_SAVE, OnSaveImage)
	ON_BN_CLICKED(IDC_MOVE_LEFT, OnMoveLeft)
	ON_BN_CLICKED(IDC_MOVE_RIGHT, OnMoveRight)
	ON_BN_CLICKED(IDC_UNWRAP_PROCESS, OnUnwrapProcess)
	ON_EN_KILLFOCUS(IDC_UNWRAP_MARGINX1, OnKillfocusMarginX1)
	ON_EN_KILLFOCUS(IDC_UNWRAP_MARGINY1, OnKillfocusMarginY1)
	ON_EN_KILLFOCUS(IDC_UNWRAP_MARGINX2, OnKillfocusMarginX2)
	ON_EN_KILLFOCUS(IDC_UNWRAP_MARGINY2, OnKillfocusMarginY2)
	ON_EN_KILLFOCUS(IDC_UNWRAP_IMAGE_MIN, OnKillfocusImageMin)
	ON_EN_KILLFOCUS(IDC_UNWRAP_IMAGE_MAX, OnKillfocusImageMax)
	ON_EN_KILLFOCUS(IDC_UNWRAP_IMAGE_TYPE, OnKillfocusImageType)
	ON_EN_KILLFOCUS(IDC_UNWRAP_DIAMETER, OnKillfocusDiameter)
	ON_BN_CLICKED(IDC_UNWRAP_3DIMAGE, On3DImage)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasurePhase message handlers

float fXData[5];
float fADC[5];

void CMeasurePhase::MeasureImages() 
{
	// TODO: Add your control notification handler code here
	double fPiezo = 0.0;
	double fVStep = 1.8;
// 	double fXScale, fYScale;
	CSize imgSize;
	CString str;
	imgSize.cx = m_XDim;
	imgSize.cy = m_YDim;
//	m_fZStepSize = 0.062/4;
//	EnableButtons(FALSE);
//	UpdateData(TRUE);
	int iKernel[9];
	for (int m = 0; m < 9; m++)
		iKernel[m] = 1;
	float pi = atan2(0, -1);
	double* pTempImg = new double [m_XDim*m_YDim];

//	CSI3300CamDlg* pDlg = AppViews.pS8Dlg->m_pSI3300CamDlg;
/*	CMainFrame* pMainFrame = NULL;
	CCameraView* pView = NULL;
	pMainFrame = Global.GetMainFrame();
	if (pMainFrame)
		pView = (CCameraView*) pMainFrame->GetCameraView();*/
	if (pView)
	{
		CTime CurTime = CTime::GetCurrentTime();
		char str2[255];
// 		if (AppViews.pS8Dlg)
// 		{
// 			sprintf(str2, "X%.2f_Y%.2f_Z%.3f_Tilt%.0f_%.0f_%.0f_Date%d-%d-%d-%d:%d:%d", 
// 				AppViews.pS8Dlg->m_fX, AppViews.pS8Dlg->m_fY, AppViews.pS8Dlg->m_fZ, 
// 				AppViews.pS8Dlg->m_fTilt1Pos, AppViews.pS8Dlg->m_fTilt2Pos, AppViews.pS8Dlg->m_fTilt3Pos,
// 				CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(),
// 				CurTime.GetHour(), CurTime.GetMinute(), CurTime.GetSecond());
// 		}
// 		else
// 		{
			sprintf(str2,"%d-%d-%d-%d:%d:%d", 
				CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(),
				CurTime.GetHour(), CurTime.GetMinute(), CurTime.GetSecond());
// 		}


		float fPZT = GetPosition(99);
	//	GotoPZT(fPZT + 2 * m_fZStepSize);
		GotoPZT(0);
		GotoPZT(fPZT - 3 * m_fZStepSize);
		Sleep(200);
		for (int i = 0; i < 5; i++)
		{
			fADC[i] = GotoPZT(fPZT + (i-2)*m_fZStepSize);
			fXData[i] = fADC[i] * 4 * pi * 10.0 / fWavelength;
	//		str.Format("c:\\temp\\tempimg%d.bmp", i);
	/*		char str2[255];
			sprintf(str2, m_chFileName);
			int iLength = m_chFileName.GetLength();
			for (int j=0; j<4; j++)
			str2[iLength-4+j] = 0;
			str.Format("%s_img%d.bmp",str2, i);*/
			memset(pTempImg, 0, sizeof(double)*m_XDim*m_YDim);

			str.Format("C://Images//image%d_%s.bmp", i, str2);

			for (int j = 0; j<m_nRepeat; j++);
			{
				pView->GetImage(m_pImg[i], imgSize);
				for (int k=0;k<m_XDim*m_YDim;k++)
					pTempImg[k] += m_pImg[i][k];
			}
			for (int k=0;k<m_XDim*m_YDim;k++)
				m_pImg[i][k] = (BYTE) pTempImg[k]/m_nRepeat;
//			pView->GetImage(m_pImg[i], imgSize);

//			DoKernelFilter(iKernel, 1, m_pImg[i], NULL);
			if (bSaveImage)
			{
				pView->SaveImage(str);
			}
		}
		GetDlgItem(IDC_IMG)->SetWindowPos(NULL, 0, 0, m_XDim, m_YDim,SWP_NOZORDER|SWP_NOMOVE);
		SetBitmap2(m_pImg[0]);
		GotoPZT(fPZT);
	}
	if (pTempImg)
	{
		delete [] pTempImg;
	}
//	EnableButtons(TRUE);
}

void CMeasurePhase::SetBitmap(int nType, BOOL bType)
{
	double fMin = 1e10;
	double fMax = -1e10;
	int iSize = m_XDim * m_YDim;
	int iIndex, x, y, iShift;
// 	double fData;

	if (m_fImageMax <= m_fImageMin)
		return; 

	double* pData = new double[iSize];
	memset(pData, 0, iSize*sizeof(double));

	for (int i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
	{
		for (int j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
		{
			iIndex = m_XDim*i + j;
			switch(nType)
			{
			case 0: pData[iIndex] = m_pUnwrapped[iIndex]; break;
			case 1: pData[iIndex] = m_pPhase[iIndex]; break;
			case 2: pData[iIndex] = fCorrect[iIndex].W; break;
			case 3: pData[iIndex] = fCorrect[iIndex].sigmaxx; break;
			case 4: pData[iIndex] = fCorrect[iIndex].sigmaxy; break;
			case 5: pData[iIndex] = fCorrect[iIndex].sigmayy; break;
			case 6: pData[iIndex] = fCorrect[iIndex].sigmarr; break;
			case 7: pData[iIndex] = fCorrect[iIndex].sigmart; break;
			case 8: pData[iIndex] = fCorrect[iIndex].sigmatt; break;
			case 9: pData[iIndex] = m_pUnwrapped2[iIndex]; break;
			default: pData[iIndex] = m_pUnwrapped[iIndex]; break;
			}
			if (pData[iIndex] > fMax) fMax = pData[iIndex];
			if (pData[iIndex] < fMin) fMin = pData[iIndex];
		}
	}
	double fScale = 255.0/(fMax - fMin);
/// auto-level image
	double* pData2 = new double[iSize];
	memset(pData2, 0, iSize*sizeof(double));

	if (bType)
	{
		if (m_fImageMax > m_fImageMin)
		{
//			for (i = 0; i < iSize; i++)
			for (int i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
			{
				for (int j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
				{
					iIndex = m_XDim*i + j;
					if (pData[iIndex] < m_fImageMin)
						pData2[iIndex] = 0;
					if (pData[iIndex] > m_fImageMax)
						pData2[iIndex] = 255;
					if ((pData[iIndex] >= m_fImageMin) && (pData[iIndex] <= m_fImageMax))
						pData2[iIndex] = 255.0*(pData[iIndex] - m_fImageMin)/(m_fImageMax - m_fImageMin);
				}
			}
		}
	}
	else
	{
//		for (i = 0; i < iSize; i++)
		for (int i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
		{
			for (int j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
			{
				iIndex = m_XDim*i + j;
				pData2[iIndex] = pData[iIndex];
			}
		}
	}
///
	ZeroMemory( &m_DIBinfo1.bmiHeader, sizeof(BITMAPINFOHEADER) );
	m_DIBinfo1.bmiHeader.biWidth = m_XDim;     // Set size you need
	m_DIBinfo1.bmiHeader.biHeight= m_YDim;    // Set size you need
	m_DIBinfo1.bmiHeader.biPlanes=1;
	m_DIBinfo1.bmiHeader.biBitCount=24; // Can be 8, 16, 32 bpp or 
                           // other number
	m_DIBinfo1.bmiHeader.biSizeImage=0;
	m_DIBinfo1.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_DIBinfo1.bmiHeader.biClrUsed= 0;
	m_DIBinfo1.bmiHeader.biClrImportant= 0;
	HDC hDC = NULL;
    try 
	{
     // Should now have all the info we need to create the sucker.
     //TRACE(_T("Width %d, Height %d, Bits/pixel %d, Image Size %d\n"),
     //      bmih.biWidth, bmih.biHeight, bmih.biBitCount, bmih.biSizeImage);

     // Create a DC which will be used to get DIB, then create DIBsection
		hDC = ::GetDC(NULL);
		if (!hDC) 
	    {
			TRACE0("Unable to get DC\n");
			AfxThrowResourceException();
		}

		if (m_hBitmap1)
		{
			DeleteObject(m_hBitmap1);
			m_hBitmap1 = NULL;
		}
	    m_hBitmap1 = CreateDIBSection(hDC, (const BITMAPINFO*) m_DIBinfo1,
                                     DIB_RGB_COLORS, &m_ppvBits1, NULL, 0);
		if (!m_hBitmap1)
		{
			TRACE0("CreateDIBSection failed\n");
		    AfxThrowResourceException();
			::ReleaseDC(NULL, hDC);
		}

	    DWORD dwImageSize = m_DIBinfo1.bmiHeader.biSizeImage;
	    if (dwImageSize == 0)
		{
			dwImageSize = 3 * m_XDim * m_YDim;
		}

	    // Flush the GDI batch queue 
		GdiFlush();
		BYTE *pBits = (BYTE *)m_ppvBits1;
//		for (int i = 0; i < iSize; i++)
		for (int i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
		{
			for (int j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
			{
				BOOL bGoodPt = TRUE;
				iIndex = (m_YDim-1-i)*m_XDim + j;//i * m_XDim + j;

				if ((!m_b3DImage) || ((i%2 == 0) && (j%4>0)))
				{
					if (m_pMask)
					{
						bGoodPt = m_pMask[iIndex];
					}
					if (m_b3DImage)
					{
						if (bGoodPt)
						{
//							iShift = (int) ((m_YDim/4*(pData[iIndex] - (m_fImageMin+m_fImageMax)/2))
//								/(m_fImageMax-m_fImageMin));
							iShift = (int) (m_YDim / 4 * (pData2[iIndex] - 127.5) /127.5);
							if (iShift>m_YDim/4-1) iShift = m_YDim/4-1;
							if (iShift<-m_YDim/4+1) iShift = -m_YDim/4+1;
						}
						else
							iShift = 0;
					
						y = 3*m_YDim/4 - i/2 + iShift;
						x = m_XDim/32 + j*3/4 + i/4;

						if (bGoodPt)
						{
							int iTemp = pData2[iIndex];
							if (!bType)
								iTemp = (iTemp - fMin) * fScale;
							iTemp = min(iTemp, 255);
							iTemp = max(0, iTemp);
							BYTE iData = iTemp;
							pBits[3 * (m_XDim * y + x) + 0] = max(0, 2 * iData - 255);
							pBits[3 * (m_XDim * y + x) + 1] = 255 - 2 * abs(iData - 127);
							pBits[3 * (m_XDim * y + x) + 2] = max(0, 255 - iData * 2);
							for (int k=0;k<y;k++)
							{
								pBits[3 * (m_XDim * k + x) + 0] = 0;
								pBits[3 * (m_XDim * k + x) + 1] = 0;
								pBits[3 * (m_XDim * k + x) + 2] = 0;
							}
						}
						else
						{
							y = 3*m_YDim/4 - i/2;
							pBits[3 * (m_XDim * y + x) + 0] = 0;
							pBits[3 * (m_XDim * y + x) + 1] = 0;
							pBits[3 * (m_XDim * y + x) + 2] = 0;
						}
					}
					else
					{
						if (bGoodPt)
						{
							int iTemp = pData2[iIndex];
							if (!bType)
								iTemp = (iTemp - fMin) * fScale;
							iTemp = min(iTemp, 255);
							iTemp = max(0, iTemp);
							BYTE iData = iTemp;
							pBits[3 * (m_XDim * (m_YDim-1-i) + j) + 0] = max(0, 2 * iData - 255);
							pBits[3 * (m_XDim * (m_YDim-1-i) + j) + 1] = 255 - 2 * abs(iData - 127);
							pBits[3 * (m_XDim * (m_YDim-1-i) + j) + 2] = max(0, 255 - iData * 2);
						}
						else
						{
							pBits[3 * (m_XDim * (m_YDim-1-i) + j) + 0] = 0;
							pBits[3 * (m_XDim * (m_YDim-1-i) + j) + 1] = 0;
							pBits[3 * (m_XDim * (m_YDim-1-i) + j) + 2] = 0;
						}
					}
				}
			}
		}
//		BYTE *pData = pImg->GetRGBData();
//	    memcpy(m_ppvBits1, pData, dwImageSize);
		::ReleaseDC(NULL, hDC);
	}
	catch (CException *e)
	{
	    e->Delete();
		if (hDC) 
		{
			::ReleaseDC(NULL, hDC);
		}
		if (pData2)
		{
			delete [] pData2;
		}
		if (pData)
		{
			delete [] pData;
		}
	    return;
	}
//	Invalidate();
	if (pData2)
	{
		delete [] pData2;
	}
	if (pData)
	{
		delete [] pData;
	}
}

void CMeasurePhase::SetColorBar()
{
	int nX = 12;
	double fMin = 1e10;
	double fMax = -1e10;
	int iSize = nX * m_YDim;


	ZeroMemory( &m_DIBinfo2.bmiHeader, sizeof(BITMAPINFOHEADER) );
	m_DIBinfo2.bmiHeader.biWidth = nX;     // Set size you need
	m_DIBinfo2.bmiHeader.biHeight= m_YDim;    // Set size you need
	m_DIBinfo2.bmiHeader.biPlanes=1;
	m_DIBinfo2.bmiHeader.biBitCount=24; // Can be 8, 16, 32 bpp or 
                           // other number
	m_DIBinfo2.bmiHeader.biSizeImage=0;
	m_DIBinfo2.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_DIBinfo2.bmiHeader.biClrUsed= 0;
	m_DIBinfo2.bmiHeader.biClrImportant= 0;
	HDC hDC = NULL;
    try 
	{
     // Should now have all the info we need to create the sucker.
     //TRACE(_T("Width %d, Height %d, Bits/pixel %d, Image Size %d\n"),
     //      bmih.biWidth, bmih.biHeight, bmih.biBitCount, bmih.biSizeImage);

     // Create a DC which will be used to get DIB, then create DIBsection
		hDC = ::GetDC(NULL);
		if (!hDC) 
	    {
			TRACE0("Unable to get DC\n");
			AfxThrowResourceException();
		}

	    m_hBitmap2 = CreateDIBSection(hDC, (const BITMAPINFO*) m_DIBinfo2,
                                     DIB_RGB_COLORS, &m_ppvBits2, NULL, 0);
		if (!m_hBitmap2)
		{
			TRACE0("CreateDIBSection failed\n");
		    AfxThrowResourceException();
			::ReleaseDC(NULL, hDC);
		}

	    DWORD dwImageSize = m_DIBinfo2.bmiHeader.biSizeImage;
	    if (dwImageSize == 0)
		{
			dwImageSize = 3 * nX * m_YDim;
		}

	    // Flush the GDI batch queue 
		GdiFlush();
		BYTE *pBits = (BYTE *)m_ppvBits2;
		for (int i = 0; i < iSize; i++)
		{
			int iTemp = i / nX;
				iTemp = iTemp * 255 / m_YDim;
			iTemp = min(iTemp, 255);
			iTemp = max(0, iTemp);
			BYTE iData = iTemp;
			pBits[3 * (m_XDim * (i/m_XDim) + (i % m_XDim)) + 0] = max(0, 2 * iData - 255);
			pBits[3 * (m_XDim * (i/m_XDim) + (i % m_XDim)) + 1] = 255 - 2 * abs(iData - 127);
			pBits[3 * (m_XDim * (i/m_XDim) + (i % m_XDim)) + 2] = max(0, 255 - iData * 2);
		}
//		BYTE *pData = pImg->GetRGBData();
//	    memcpy(m_ppvBits1, pData, dwImageSize);
		::ReleaseDC(NULL, hDC);

	}
	catch (CException *e)
	{
	    e->Delete();
		if (hDC) 
			::ReleaseDC(NULL, hDC);
	    return;
	}
	Invalidate();

	CString strVal;
	strVal.Format("%.3f", m_fImageMin);
	GetDlgItem(IDC_UNWRAP_COLORBAR_LEV1)->SetWindowText(strVal);
	strVal.Format("%.3f", m_fImageMin + 0.25*(m_fImageMax - m_fImageMin));
	GetDlgItem(IDC_UNWRAP_COLORBAR_LEV2)->SetWindowText(strVal);
	strVal.Format("%.3f", m_fImageMin + 0.5*(m_fImageMax - m_fImageMin));
	GetDlgItem(IDC_UNWRAP_COLORBAR_LEV3)->SetWindowText(strVal);
	strVal.Format("%.3f", m_fImageMin + 0.75*(m_fImageMax - m_fImageMin));
	GetDlgItem(IDC_UNWRAP_COLORBAR_LEV4)->SetWindowText(strVal);
	strVal.Format("%.3f", m_fImageMax);
	GetDlgItem(IDC_UNWRAP_COLORBAR_LEV5)->SetWindowText(strVal);

}

void CMeasurePhase::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	if (m_hBitmap1)
	{
		CRect rc, rc2;
		GetDlgItem(IDC_IMG)->GetWindowRect(&rc);
		ScreenToClient(rc);
//		GetWindowRect(&rc2);
		HDC dcDisplay;
		BITMAP bm;
		HGDIOBJ hGdiObj;
		dcDisplay = CreateCompatibleDC(dc);
		GetObject(m_hBitmap1, sizeof(BITMAP), &bm);
		hGdiObj = SelectObject(dcDisplay, m_hBitmap1);
		SelectObject(dcDisplay, m_hBitmap1);
		SetStretchBltMode(dc, COLORONCOLOR); 
		StretchBlt(dc, rc.left, rc.top, 512, 384, //rc.right-rc.left+1, rc.bottom-rc.top+1, 
				   dcDisplay, 0, 0, m_DIBinfo1.bmiHeader.biWidth, m_DIBinfo1.bmiHeader.biHeight, SRCCOPY);

		GetDlgItem(IDC_UNWRAP_IMG2)->GetWindowRect(&rc2);
		ScreenToClient(rc2);
//		GetWindowRect(&rc2);
		GetObject(m_hBitmap2, sizeof(BITMAP), &bm);
		SelectObject(dcDisplay, m_hBitmap2);
		SetStretchBltMode(dc, COLORONCOLOR); 
		StretchBlt(dc, rc2.left, rc2.top, 12, 384, 
				   dcDisplay, 0, 0, m_DIBinfo2.bmiHeader.biWidth, m_DIBinfo2.bmiHeader.biHeight, SRCCOPY);

		
		SelectObject(dcDisplay, hGdiObj);

		DeleteDC(dcDisplay);
	}
	// Do not call CDialog::OnPaint() for painting messages
}

void CMeasurePhase::SetupMask()
{
	BYTE *p1;
	int iSize = m_XDim * m_YDim;
	double fMaxInt, fMinInt;
	double fMaxVis, fMinVis;
	int i, j, iIndex;
	fMaxInt = -1e10; fMinInt = 1e10;
	fMaxVis = -1e10; fMinVis = 1e10;

	for (i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
	{
		for (j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				iIndex = i * m_XDim + j;
				double fMax=-1e10, fMin = 1e10;
				double fSum = 0.0;
				for (int m = 0; m < 5; m++)
				{
					p1 = m_pImg[m];
					double fTemp = p1[iIndex];
					fSum += fTemp;
					if (fTemp > fMax) fMax = fTemp;
					if (fTemp < fMin) fMin = fTemp;
				}
				double fVal;
				m_pInt[iIndex] = fVal = fSum /= (double)m;
				if (fVal > fMaxInt) fMaxInt = fVal;
				if (fVal < fMinInt) fMinInt = fVal;
				m_pVis[iIndex] = fVal = fMax - fMin;
				if (fVal > fMaxVis) fMaxVis = fVal;
				if (fVal < fMinVis) fMinVis = fVal;
			}
		}
	}

	double fIntThreshold, fVisThreshold;
	fIntThreshold = (fMaxInt - fMinInt) * m_fMaskThreshold;//THRESHOLD_RATIO;
	fVisThreshold = (fMaxVis - fMinVis) * m_fMaskThreshold;//THRESHOLD_RATIO;

	for (i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
	{
		for (j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
		{
			iIndex = i * m_XDim + j;
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				m_pMask[iIndex] = (m_pInt[iIndex] > fIntThreshold) && (m_pVis[iIndex] > fVisThreshold);
			}
			else
				m_pMask[iIndex] = FALSE;
		}
	}
}

void CMeasurePhase::LevelShift(int nAxis)
{
	if (!m_pPhase) return;
	if (!m_pUnwrapped) return;

	double *pLine = NULL;
	double pi = atan2(0, -1);
	double fSum;
	int i, j, iIndex,/* iIndex2,*/ iOverlap, iUpDown, /*X, Y,*/ k, l, iDist;
	double fNormalDiff;
	double fSumm2, fRMSX, fRMSY, fData;
	int iCount;

	int Nsec = 360;
	int iOverlap2[360];
	double fSum2[360];
	double fSum3[360];

	switch(nAxis)
	{
	case 0: //horizontal fringes
		pLine = new double[m_XDim];
		ZeroMemory(pLine, sizeof(double) * m_XDim);
		for (i = 1+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
		{
			iOverlap = 0;
			fSum = 0.0;
			for (j = 0+m_nMarginY1; j < m_YDim-m_nMarginY2; j++)
			{
				iIndex = j * m_XDim + i;
				if (m_pMask[iIndex] && m_pMask[iIndex-1])
				{
					iOverlap++;
					fSum += m_pUnwrapped[iIndex] - m_pUnwrapped[iIndex -1];
				}
			}
			if (iOverlap > 0)
			{
				fNormalDiff = fSum/(iOverlap * 2 * pi); // normalize different
				if (fNormalDiff > 0.0) 
					iUpDown = floor(fNormalDiff + 0.5); // rounding
				else
					iUpDown = ceil(fNormalDiff - 0.5);
				pLine[i] = pLine[i-1] + iUpDown * 2 * pi;
			}
			else
			{
				TRACE("No overlap at line %d\n", i);
			}
		}
		for (i = 1+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
		{
			for (j = 0+m_nMarginY1; j < m_YDim-m_nMarginY2; j++)
			{
				iIndex = j * m_XDim + i;
				if (m_pMask[iIndex])
					m_pUnwrapped[iIndex] -= pLine[i];
				else
					m_pUnwrapped[iIndex] = 0.0;
			}
		}	break;
	case 1: //vertical fringes
		pLine = new double[m_YDim];
		ZeroMemory(pLine, sizeof(double) * m_YDim);
		for (i = 1+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
		{
			iOverlap = 0;
			fSum = 0.0;
			for (j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
			{
				iIndex = i * m_XDim + j;
				if (m_pMask[iIndex] && m_pMask[iIndex - m_XDim])
				{
					iOverlap++;
					fSum += m_pUnwrapped[iIndex] - m_pUnwrapped[iIndex - m_XDim];
				}
			}
			if (iOverlap > 0)
			{
				fNormalDiff = fSum/(iOverlap * 2 * pi); // normalize different
				if (fNormalDiff > 0.0) 
					iUpDown = floor(fNormalDiff + 0.5); // rounding
				else
					iUpDown = ceil(fNormalDiff - 0.5);
				pLine[i] = pLine[i-1] + iUpDown * 2 * pi;
			}
			else
			{
				TRACE("No overlap at line %d\n", i);
			}
		}
		for (i = 1+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
		{
			for (int j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
			{
				iIndex = i * m_XDim + j;
				if (m_pMask[iIndex])
					m_pUnwrapped[iIndex] -= pLine[i];
				else
					m_pUnwrapped[iIndex] = 0.0;
			}
		}	break;
	case 2: //concentric fringes, rectangular area
		pLine = new double[2*(m_XDim+m_YDim)];
		ZeroMemory(pLine, sizeof(double) * 2 * (m_XDim + m_YDim));
		iIndex = m_nCenterX + m_nCenterY * m_XDim;

		ZeroMemory(iOverlap2, sizeof(int) * Nsec);
		memset(fSum2, 0, sizeof(double) * Nsec);
		memset(fSum3, m_pUnwrapped[iIndex], sizeof(double) * Nsec);

		for (k = 1; k < m_XDim+m_YDim; k++)
		{
			ZeroMemory(iOverlap2, sizeof(int) * Nsec);
			for (j = 0+m_nMarginY1; j < m_YDim-m_nMarginY2; j++)
			{
				for (i = 0+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
				{
					iDist = (i - m_nCenterX) * (i - m_nCenterX) 
						+ (j - m_nCenterY) * (j - m_nCenterY);
					if ((iDist >= k*k) && (iDist < (k+1)*(k+1)))
					{
						iIndex = j * m_XDim + i;
						l = (int) (Nsec * (pi + atan2(j-m_nCenterY, i-m_nCenterX)) / (2*pi));
						if (l<0) l=0;
						if (l>=Nsec) l = Nsec-1;
						if (m_pMask[iIndex])
						{
							iOverlap2[l]++;
							fSum2[l] += (m_pUnwrapped[iIndex] - fSum3[l]);
						}
					}
				}
			}
			for (l = 0; l < Nsec; l++)
			{
				if (iOverlap2[l]>0)
				{
					fSum2[l] /= (iOverlap2[l] * 2 * pi);
					fSum3[l] = fSum2[l];
				}
			}
		}
		for (l = 0; l < Nsec; l++)
		{
			if (iOverlap2[l] > 0)
			{
				fNormalDiff = fSum2[l]/(iOverlap2[l] * 2 * pi); // normalize different
				if (fNormalDiff > 0.0) 
					iUpDown = floor(fNormalDiff + 0.5); // rounding
				else
					iUpDown = ceil(fNormalDiff - 0.5);
				pLine[l] = pLine[l-1] + (iUpDown * 2 * pi);
			}
		}
		for (i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
		{
			for (int j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
			{
				if ((i == m_nCenterY) && (j == m_nCenterX))
				{
				}
				else
				{
					iIndex = i * m_XDim + j;
					l = (int) (Nsec * (pi + atan2(j-m_nCenterY, i-m_nCenterX)) / (2*pi));
					if (m_pMask[iIndex])
						m_pUnwrapped[iIndex] -= pLine[l];
					else
						m_pUnwrapped[iIndex] = 0.0;
				}
			}
		}
		break;
	case 3: //concentric fringes, round area
		pLine = new double[2*(m_XDim+m_YDim)];
		ZeroMemory(pLine, sizeof(double) * 2 * (m_XDim + m_YDim));
		iIndex = m_nCenterX + m_nCenterY * m_XDim;

		ZeroMemory(iOverlap2, sizeof(int) * Nsec);
		memset(fSum2, 0, sizeof(double) * Nsec);
		memset(fSum3, m_pUnwrapped[iIndex], sizeof(double) * Nsec);

		for (k = 1; k <= m_nRadius; k++)
		{
			ZeroMemory(iOverlap2, sizeof(int) * Nsec);
			for (j = 0+m_nMarginY1; j < m_YDim-m_nMarginY2; j++)
			{
				for (i = 0+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
				{
					iDist = (i - m_nCenterX) * (i - m_nCenterX) 
						+ (j - m_nCenterY) * (j - m_nCenterY);
					if ((iDist >= k*k) && (iDist < (k+1)*(k+1)))
					{
						iIndex = j * m_XDim + i;
						l = (int) (Nsec * (pi + atan2(j-m_nCenterY, i-m_nCenterX)) / (2*pi));
						if (l<0) l=0;
						if (l>=Nsec) l = Nsec-1;
						if (m_pMask[iIndex])
						{
							iOverlap2[l]++;
							fSum2[l] += (m_pUnwrapped[iIndex] - fSum3[l]);
						}
					}
				}
			}
			for (l = 0; l < Nsec; l++)
			{
				if (iOverlap2[l]>0)
				{
					fSum2[l] /= (iOverlap2[l] * 2 * pi);
					fSum3[l] = fSum2[l];
				}
			}
		}
		for (l = 0; l < Nsec; l++)
		{
			if (iOverlap2[l] > 0)
			{
				fNormalDiff = fSum2[l]/(iOverlap2[l] * 2 * pi); // normalize different
				if (fNormalDiff > 0.0) 
					iUpDown = floor(fNormalDiff + 0.5); // rounding
				else
					iUpDown = ceil(fNormalDiff - 0.5);
				pLine[l] = pLine[l-1] + (iUpDown * 2 * pi);
			}
		}
		for (i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
		{
			for (int j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
			{
				if ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius)
				{
					if ((i == m_nCenterY) && (j == m_nCenterX))
					{
					}
					else
					{
						iIndex = i * m_XDim + j;
						l = (int) (Nsec * (pi + atan2(j-m_nCenterY, i-m_nCenterX)) / (2*pi));
						if (m_pMask[iIndex])
							m_pUnwrapped[iIndex] -= pLine[l];
						else
							m_pUnwrapped[iIndex] = 0.0;
					}
				}
			}
		}
		break;
	case 4:
		fSum = fSumm2 = 0.0;
		iCount = 0;
		fRMSY = 0;
		for (i = m_nCenterY-m_nRadius; i <= m_nCenterY+m_nRadius; i++)
		{
			iIndex = i*m_XDim + m_nRadius;
			fData = m_pPhase[iIndex];
			fSumm2 += 1.0 * fData * fData;
			fSum  += 1.0 * fData;
			iCount++;
		}
		if (iCount>0)
		{
			fRMSY = sqrt((fSumm2 - fSum * fSum/(double)iCount)/(iCount-1));
		}
		fSum = fSumm2 = 0.0;
		iCount = 0;
		fRMSX = 0;
		for (i = m_nCenterY-m_nRadius; i <= m_nCenterY+m_nRadius; i++)
		{
			iIndex = m_nRadius*m_XDim + i;
			fData = m_pPhase[iIndex];
			fSumm2 += 1.0 * fData * fData;
			fSum  += 1.0 * fData;
			iCount++;
		}
		if (iCount>0)
		{
			fRMSX = sqrt((fSumm2 - fSum * fSum/(double)iCount)/(iCount-1));
		}
		if (fRMSY>fRMSX)
		{
			pLine = new double[m_XDim];
			ZeroMemory(pLine, sizeof(double) * m_XDim);
			for (i = 1+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
			{
				iOverlap = 0;
				fSum = 0.0;
				for (j = 0+m_nMarginY1; j < m_YDim-m_nMarginY2; j++)
				{
					iIndex = j * m_XDim + i;
					if (m_pMask[iIndex] && m_pMask[iIndex-1])
					{
						iOverlap++;
						fSum += m_pUnwrapped[iIndex] - m_pUnwrapped[iIndex -1];
					}
				}
				if (iOverlap > 0)
				{
					fNormalDiff = fSum/(iOverlap * 2 * pi); // normalize different
					if (fNormalDiff > 0.0) 
						iUpDown = floor(fNormalDiff + 0.5); // rounding
					else
						iUpDown = ceil(fNormalDiff - 0.5);
					pLine[i] = pLine[i-1] + iUpDown * 2 * pi;
				}
				else
				{
					TRACE("No overlap at line %d\n", i);
				}
			}
			for (i = 1+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
			{
				for (j = 0+m_nMarginY1; j < m_YDim-m_nMarginY2; j++)
				{
					iIndex = j * m_XDim + i;
					if (m_pMask[iIndex])
						m_pUnwrapped[iIndex] -= pLine[i];
					else
						m_pUnwrapped[iIndex] = 0.0;
				}
			}
		}
		else
		{
			pLine = new double[m_YDim];
			ZeroMemory(pLine, sizeof(double) * m_YDim);
			for (i = 1+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
			{
				iOverlap = 0;
				fSum = 0.0;
				for (j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
				{
					iIndex = i * m_XDim + j;
					if (m_pMask[iIndex] && m_pMask[iIndex - m_XDim])
					{
						iOverlap++;
						fSum += m_pUnwrapped[iIndex] - m_pUnwrapped[iIndex - m_XDim];
					}
				}
				if (iOverlap > 0)
				{
					fNormalDiff = fSum/(iOverlap * 2 * pi); // normalize different
					if (fNormalDiff > 0.0) 
						iUpDown = floor(fNormalDiff + 0.5); // rounding
					else
						iUpDown = ceil(fNormalDiff - 0.5);
					pLine[i] = pLine[i-1] + iUpDown * 2 * pi;
				}
				else
				{
					TRACE("No overlap at line %d\n", i);
				}
			}
			for (i = 1+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
			{
				for (int j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
				{
					iIndex = i * m_XDim + j;
					if (m_pMask[iIndex])
						m_pUnwrapped[iIndex] -= pLine[i];
					else
						m_pUnwrapped[iIndex] = 0.0;
				}
			}
		}
		break;
	default: break;
	}
/*	float fMin = 1e10, fMax = -1e10, fWarp = 0;
	for (i = 0+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
	{
		for (j = 0+m_nMarginY1; j < m_YDim-m_nMarginY2; j++)
		{
			iIndex = j * m_XDim + i;
			if (m_pUnwrapped[iIndex] > fMax)
				fMax = m_pUnwrapped[iIndex];
			if (m_pUnwrapped[iIndex] < fMin)
				fMin = m_pUnwrapped[iIndex];
		}
	}
	if (fMax > fMin)
		fWarp = (fMax-fMin)*fWavelength/4/pi;
	CString strVal;
	strVal.Format("%.6f", fWarp);
	GetDlgItem(IDC_UNWRAP_WARP_VALUE)->SetWindowText(strVal);*/

	if (pLine)
	{
		delete [] pLine;
		pLine = NULL;
	}
//	SetBitmap(m_nImageType, FALSE);
}

void CMeasurePhase::FlattenX()
{
	if (!m_pPhase) return;
	if (!m_pUnwrapped) return;
	double *pLine = new double[m_XDim];
	ZeroMemory(pLine, sizeof(double) * m_XDim);
	int iCenter = m_YDim /2;

	for (int i = 0+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
	{
		for (int j = iCenter-1; j < iCenter+2; j++)
			pLine[i] += m_pPhase[j * m_XDim + i]/3.0;
	}
	
	double fLine1CenterAve = 0.0;
	for (int j = iCenter-1; j < iCenter+2; j++)
		fLine1CenterAve += m_pUnwrapped[j * m_XDim]/3.0;

	for (i = 1+m_nMarginX1; i < m_XDim-m_nMarginX2; i++)
	{
		double fLineCenterAve = 0.0;
		for (int j = iCenter-1; j < iCenter+2; j++)
			fLineCenterAve += m_pUnwrapped[j * m_XDim + i]/3.0;
		double fDiff = fLine1CenterAve + pLine[i] - pLine[0] - fLineCenterAve;
		for (int m = 0+m_nMarginY1; m < m_YDim-m_nMarginY2; m++)
		{
			m_pUnwrapped[m * m_XDim + i] += fDiff;
		}
	}
//	SetBitmap(m_nImageType, FALSE);
	delete [] pLine;
	pLine = NULL;
}

void CMeasurePhase::SetBitmap2(LPBYTE pData)
{
	int iSize = m_XDim * m_YDim;

	ZeroMemory( &m_DIBinfo1.bmiHeader, sizeof(BITMAPINFOHEADER) );
	m_DIBinfo1.bmiHeader.biWidth = m_XDim;     // Set size you need
	m_DIBinfo1.bmiHeader.biHeight= m_YDim;    // Set size you need
	m_DIBinfo1.bmiHeader.biPlanes=1;
	m_DIBinfo1.bmiHeader.biBitCount=24; // Can be 8, 16, 32 bpp or 
                           // other number
	m_DIBinfo1.bmiHeader.biSizeImage=0;
	m_DIBinfo1.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_DIBinfo1.bmiHeader.biClrUsed= 0;
	m_DIBinfo1.bmiHeader.biClrImportant= 0;
	HDC hDC = NULL;
	try 
	{
     // Should now have all the info we need to create the sucker.
     //TRACE(_T("Width %d, Height %d, Bits/pixel %d, Image Size %d\n"),
     //      bmih.biWidth, bmih.biHeight, bmih.biBitCount, bmih.biSizeImage);

     // Create a DC which will be used to get DIB, then create DIBsection
	    hDC = ::GetDC(NULL);
		if (!hDC) 
	    {
			TRACE0("Unable to get DC\n");
			AfxThrowResourceException();
		}

		if (m_hBitmap1)
		{
			DeleteObject(m_hBitmap1);
			m_hBitmap1 = NULL;
		}
	    m_hBitmap1 = CreateDIBSection(hDC, (const BITMAPINFO*) m_DIBinfo1,
                                     DIB_RGB_COLORS, &m_ppvBits1, NULL, 0);
		if (!m_hBitmap1)
		{
			TRACE0("CreateDIBSection failed\n");
		    AfxThrowResourceException();
			::ReleaseDC(NULL, hDC);
		}

	    DWORD dwImageSize = m_DIBinfo1.bmiHeader.biSizeImage;
	    if (dwImageSize == 0)
		{
			dwImageSize = 3 * m_XDim * m_YDim;
		}

	    // Flush the GDI batch queue 
//		GdiFlush();
		BYTE *pBits = (BYTE *)m_ppvBits1;
		for (int i = 0; i < iSize; i++)
		{
			BYTE iData = pData[i];
			pBits[3 * (m_XDim * (/*m_YDim - 1 - */i/m_XDim) + (i % m_XDim)) + 0] = max(0, 2 * iData - 255);
			pBits[3 * (m_XDim * (/*m_YDim - 1 - */i/m_XDim) + (i % m_XDim)) + 1] = 255 - 2 * abs(iData - 127);
			pBits[3 * (m_XDim * (/*m_YDim - 1 - */i/m_XDim) + (i % m_XDim)) + 2] = max(0, 255 - iData * 2);
		}
		::ReleaseDC(NULL, hDC);

	}
	catch (CException *e)
	{
		e->Delete();
		if (hDC) 
			::ReleaseDC(NULL, hDC);
		return;
	}
//	Invalidate();
}

void CMeasurePhase::UnwrapMask()
{
	double fY, fX, phase;
	int iSize = m_XDim * m_YDim, X, Y, i;
	BYTE *p1, *p2, *p3, *p4, *p5;

	double PI = 4.0*atan(1.0);
//	EnableButtons(FALSE);
//	UpdateData(TRUE);

	p1 = m_pImg[0];
	p2 = m_pImg[IMG_STEP];
	p3 = m_pImg[2*IMG_STEP];
	p4 = m_pImg[3*IMG_STEP];
	p5 = m_pImg[4*IMG_STEP];

	memset(m_pPhase, 0, iSize*sizeof(double));
	memset(m_pPhase, 0, iSize*sizeof(double));
	memset(m_pR, 0, iSize*sizeof(double));
	memset(m_pUnwrapped, 0, iSize*sizeof(double));
	memset(m_pUnwrapped2, 0, iSize*sizeof(double));
	memset(m_pInt, 0, iSize*sizeof(double));
	memset(m_pVis, 0, iSize*sizeof(double));
	memset(m_pMask, 0, iSize*sizeof(BOOL));
	
	if (bSetupMask)
		SetupMask();

	for (i = 0; i < iSize; i++)
	{
		fCorrect[i].x = fCorrect[i].y =	1.0;
		fCorrect[i].z = fCorrect[i].W 
			= fCorrect[i].sigmaxx = fCorrect[i].sigmaxy = fCorrect[i].sigmayy 
			= fCorrect[i].sigmarr = fCorrect[i].sigmart = fCorrect[i].sigmatt = 0.0;
	}

	double c1, c2, s1, s2;
	float fX2, fY2;
	if (nPZTPosType)
	{
		c1 = 2 * (cos(fXData[1]) - cos(fXData[3]));
		s1 = 2 * (sin(fXData[1]) - sin(fXData[3]));
		c2 = cos(fXData[0]) - 2 * cos(fXData[2]) + cos(fXData[4]);
		s2 = sin(fXData[0]) - 2 * sin(fXData[2]) + sin(fXData[4]);
	}
	for (X = 0+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
	{
		double phasePrev = 0.0;
		for (Y = 0+m_nMarginY1; Y < m_YDim-m_nMarginY2; Y++)
		{
			int iIndex = Y * m_XDim + X;
			if (m_pMask[iIndex])
			{
//				fY = 2 * (p4[iIndex] - p2[iIndex]);
				fY = 2 * (p2[iIndex] - p4[iIndex]);
				fX = p1[iIndex] - 2 * p3[iIndex] + p5[iIndex];

				if (nPZTPosType)
				{
					fY2 = fX * c1 - fY * c2;
					fX2 = - fX * s1 + fY * s2;
					phase = atan2(fY2, fX2);
				}
				else
					phase = atan2(fY, fX);
				m_pR[iIndex] = sqrt(fX * fX + fY * fY);
				if (!m_pMask[iIndex])
				//if (m_pR[iIndex] < 5.0)
					phase = phasePrev;
				m_pPhase[iIndex] = phase;
				phasePrev = phase;
			}
		}
	}
	for (X = 0+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
	{
		for (Y = 0+m_nMarginY1; Y < m_YDim-m_nMarginY2; Y++)
		{
			if ((m_nDirection < 3) || ((X - m_nCenterX) * (X - m_nCenterX) + (Y - m_nCenterY) * (Y - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				int iIndex = Y * m_XDim + X;
				double fPhase = 0.0;
				int  nCount=0;
				if (!m_pMask[iIndex])
				{
					if ((X>0) && m_pMask[iIndex-1])
					{
						fPhase += m_pPhase[iIndex-1];
						nCount ++;
					}
					if ((X<m_XDim-1) && m_pMask[iIndex+1])
					{
						if (nCount>0)
						{
							if (fPhase-m_pPhase[iIndex+1] > PI)
								fPhase -= 2*PI;
							if (fPhase-m_pPhase[iIndex+1] < -PI)
								fPhase += 2*PI;
						}
						fPhase += m_pPhase[iIndex+1];
						nCount ++;
					}
					if ((Y>0) && m_pMask[iIndex-m_XDim])
					{
						if (nCount>0)
						{
							if (fPhase/nCount-m_pPhase[iIndex-m_XDim] > PI)
								fPhase -= 2*PI;
							if (fPhase/nCount-m_pPhase[iIndex-m_XDim] < -PI)
								fPhase += 2*PI;
						}
						fPhase += m_pPhase[iIndex-m_XDim];
						nCount ++;
					}
					if ((Y<m_YDim-1) && m_pMask[iIndex+m_XDim])
					{
						if (nCount>0)
						{
							if (fPhase/nCount-m_pPhase[iIndex-m_XDim] > PI)
								fPhase -= 2*PI;
							if (fPhase/nCount-m_pPhase[iIndex-m_XDim] < -PI)
								fPhase += 2*PI;
						}
						fPhase += m_pPhase[iIndex+m_XDim];
						nCount ++;
					}
					if (nCount>0)
					{
						m_pPhase[iIndex] = fPhase/nCount;
						m_pMask[iIndex] = TRUE;
					}
				}
			}
		}
	}
	FilterPhase(2);
	FilterPhase(2);
//	SetBitmap(m_nImageType, FALSE);
//	EnableButtons(TRUE);
}


void CMeasurePhase::FilterPhase(int m)
{
	int X, Y, i, j, /*iIndex, */iIndex2, k;
	double phase, phase2;
	double PI = atan2(0, -1);
	for (X = 0+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
	{
		double phasePrev = 0.0;
		for (Y = 0+m_nMarginY1; Y < m_YDim-m_nMarginY2; Y++)
		{
			if ((m_nDirection < 3) || ((X - m_nCenterX) * (X - m_nCenterX) + (Y - m_nCenterY) * (Y - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				int iIndex = Y * m_XDim + X;
				if (m_pMask[iIndex])
				{
					phase = 0;
					k = 0;
					for (i=-m;i<=m;i++)
					{
						for (j=-m;j<=m;j++)
						{ 
							if (((Y+i)>=m_nMarginY1) && ((Y+i)<m_YDim-m_nMarginY2) 
								&& ((X+j)>=m_nMarginX1) && ((X+j)<m_XDim-m_nMarginX2) && (i*j != 0))
							{
								iIndex2 = (Y+i) * m_XDim + (X+j);
								if (m_pMask[iIndex2])
								{
									phase2 = m_pPhase[iIndex2] - m_pPhase[iIndex];
									if (phase2>PI) 
										phase2 -= 2*PI;							
									if (phase2<-PI) 
										phase2 += 2*PI;							
									phase += phase2;
									k++;
								}
							}
						}
					}
					if (k>0)
					{
						phase /= k;
						m_pR[iIndex] = m_pPhase[iIndex] + phase;
					}
					else 
					{
						m_pR[iIndex] = m_pPhase[iIndex];
					}
				}
			}
		}
	}
	for (X = 0+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
	{
		double phasePrev = 0.0;
		for (Y = 0+m_nMarginY1; Y < m_YDim-m_nMarginY2; Y++)
		{
			if ((m_nDirection < 3) || ((X - m_nCenterX) * (X - m_nCenterX) + (Y - m_nCenterY) * (Y - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				int iIndex = Y * m_XDim + X;
				if (m_pMask[iIndex])
				{
					m_pPhase[iIndex] = m_pR[iIndex];
				}
			}
		}
	}
}

void CMeasurePhase::OnUnwrapY()
{
	EnableButtons(FALSE);
	UpdateData(TRUE);
	LevelShift(1);
	SetBitmap(m_nImageType, FALSE);
	Invalidate();
	EnableButtons(TRUE);
}

void CMeasurePhase::OnUnwrapX()
{
	EnableButtons(FALSE);
	UpdateData(TRUE);
	LevelShift(0);
	SetBitmap(m_nImageType, FALSE);
	Invalidate();
	EnableButtons(TRUE);
}

BOOL CMeasurePhase::DoKernelFilter(int *iFilter, int iDim, LPBYTE pSrc, LPBYTE pDest)
{
	int nTotalPixel = m_XDim * m_YDim;
	int i, j;
	int m, n;
	LPBYTE pDestData = NULL;

	if (pDest == NULL)
	{
		pDestData = new BYTE[nTotalPixel];
	}
	else
		pDestData = pDest;

	double fSum = 0;
	for (i = 0; i < (iDim * 2 + 1) * (iDim * 2 + 1); i++)
		fSum += *(iFilter + i);
	double invSum;
	if (fSum>0) 
		invSum = 1/fSum;
	else
		invSum = 0;
	UINT iCenter = (iDim * 2 + 1) * (iDim * 2 + 1) / 2;
	double dfMax = 0;
	int iKernel;
	int iPixel;
	for (i = iDim + m_nMarginY1; i < m_YDim - iDim - m_nMarginY2; i++)
	{
		for (j = iDim + m_nMarginX1; j < m_XDim - iDim - m_nMarginX2; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0;
				for (m = -iDim; m < iDim + 1; m++)
					for (n = -iDim; n < iDim + 1; n++)
					{
						iKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter);
						iPixel = (i + m) * m_XDim + j + n;
						fTemp += *(pSrc + iPixel) * iKernel;
					}
				fTemp *= invSum;
				if (fTemp > dfMax)
					dfMax = fTemp ;
				pDestData[i * m_XDim + j] = (BYTE)fTemp;
			}
		}
	}
	//edge
	// top edge
	for (i = 0 + m_nMarginY1; i < iDim + m_nMarginY1; i++)
	{
		for (j = iDim + m_nMarginX1; j < m_XDim - iDim - m_nMarginX2; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0, fKernel;
				fSum = 0.0;
				for (m = 0; m < iDim + 1; m++)
				{
					for (n = -iDim; n < iDim + 1; n++)
					{
						fKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter);
						fSum += fKernel;
						iPixel = (i + m) * m_XDim + j + n;
						if ((iPixel > nTotalPixel) || (iPixel < 0))
							fTemp = 0;
						fTemp += *(pSrc + iPixel) * fKernel;
					}
				}
				if (fSum>0)
					fTemp /= fSum;
				else
					fTemp = 0.0;
				pDestData[i * m_XDim + j] = (BYTE) fTemp;									
				if (fTemp > dfMax)
					dfMax = fTemp ;
			}
		}
	}

	// botton edge
	int k;
	for (i =  m_YDim - iDim - m_nMarginY2, k = 0; i < m_YDim - m_nMarginY2; i++, k++)
	{
		for (j = iDim + m_nMarginX1; j < m_XDim - iDim - m_nMarginX2; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0, fKernel;
				fSum = 0.0;
				for (m = -iDim; m < iDim - k; m++)
					for (n = -iDim; n < iDim + 1; n++)
					{
						fKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter);
						fSum += fKernel;
						iPixel = (i + m) * m_XDim + j + n;
						if ((iPixel > nTotalPixel) || (iPixel < 0))
							fTemp = 0;
						fTemp += *(pSrc + iPixel) * fKernel;
					}
				if (fSum>0)
					fTemp /= fSum;
				else
					fTemp = 0.0;
				if (fTemp > dfMax)
					dfMax = fTemp ;
				pDestData[i * m_XDim + j] = (BYTE)fTemp;									
			}
		}
	}

	// left edge
	for (i = iDim + m_nMarginY1; i < m_YDim - iDim - m_nMarginY2; i++)
	{
		for (j = 0 + m_nMarginX1; j < iDim + m_nMarginX1; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0, fKernel;
				fSum = 0.0;
				for (m = -iDim; m < iDim ; m++)
					for (n = -j; n < iDim + 1; n++)
					{
						fKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter);
						fSum += fKernel;
						iPixel = (i + m) * m_XDim + j + n;
						if ((iPixel > nTotalPixel) || (iPixel < 0))
							fTemp = 0;
						fTemp += *(pSrc + iPixel) * fKernel;
					}
				if (fSum>0)
					fTemp /= fSum;
				else
					fTemp = 0.0;
				if (fTemp > dfMax)
					dfMax = fTemp ;
				pDestData[i * m_XDim + j] = (BYTE)fTemp;									
			}
		}
	}
	// right edge
	for (i = iDim + m_nMarginY1; i < m_YDim - iDim - m_nMarginY2; i++)
	{
		for (j = m_XDim - iDim - m_nMarginX2, k = 0; j < m_XDim - m_nMarginX2; j++, k++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0, fKernel;
				fSum = 0.0;
				for (m = -iDim; m < iDim; m++)
					for (n = -iDim; n < iDim - k; n++)
					{
						fKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter) ;
						fSum += fKernel;
						iPixel = (i + m) * m_XDim + j + n;
						if ((iPixel > nTotalPixel) || (iPixel < 0))
							fTemp = 0;
						fTemp += *(pSrc + iPixel) * fKernel;
					}
				if (fSum>0)
					fTemp /= fSum;
				else
					fTemp = 0.0;
				if (fTemp > dfMax)
					dfMax = fTemp ;
				pDestData[i * m_XDim + j] = (BYTE)fTemp;									
			}
		}
	}

	// if pDest == NULL, override the orginal data
	if (pDest == NULL)
	{
		memcpy(pSrc, pDestData, nTotalPixel * sizeof(BYTE));
		delete [] pDestData;
		pDestData = NULL;
	}
	return TRUE;
}

BOOL CMeasurePhase::DoKernelFilterDbl(int *iFilter, int iDim, double* pSrc)
{
	int nTotalPixel = m_XDim * m_YDim;
	int i, j;
	int m, n;
	double* pDestData = NULL;

	pDestData = new double[nTotalPixel];

	double fSum = 0;
	for (i = 0; i < (iDim * 2 + 1) * (iDim * 2 + 1); i++)
		fSum += *(iFilter + i);
	double invSum;
	if (fSum>0) 
		invSum = 1/fSum;
	else
		invSum = 0;
	UINT iCenter = (iDim * 2 + 1) * (iDim * 2 + 1) / 2;
	double dfMax = 0;
	int iKernel;
	int iPixel;
	for (i = iDim + m_nMarginY1; i < m_YDim - iDim - m_nMarginY2; i++)
	{
		for (j = iDim + m_nMarginX1; j < m_XDim - iDim - m_nMarginX2; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0;
				for (m = -iDim; m < iDim + 1; m++)
					for (n = -iDim; n < iDim + 1; n++)
					{
						iKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter);
						iPixel = (i + m) * m_XDim + j + n;
						fTemp += *(pSrc + iPixel) * iKernel;
					}
				fTemp *= invSum;
				if (fTemp > dfMax)
					dfMax = fTemp ;
				pDestData[i * m_XDim + j] = fTemp;
			}
		}
	}
	//edge
	// top edge
	for (i = 0 + m_nMarginY1; i < iDim + m_nMarginY1; i++)
	{
		for (j = iDim + m_nMarginX1; j < m_XDim - iDim - m_nMarginX2; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0, fKernel;
				fSum = 0.0;
				for (m = 0; m < iDim + 1; m++)
				{
					for (n = -iDim; n < iDim + 1; n++)
					{
						fKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter);
						fSum += fKernel;
						iPixel = (i + m) * m_XDim + j + n;
						if ((iPixel > nTotalPixel) || (iPixel < 0))
							fTemp = 0;
						fTemp += *(pSrc + iPixel) * fKernel;
					}
				}
				if (fSum>0)
					fTemp /= fSum;
				else
					fTemp = 0.0;
				pDestData[i * m_XDim + j] = fTemp;									
				if (fTemp > dfMax)
					dfMax = fTemp ;
			}
		}
	}

	// bottom edge
	int k;
	for (i =  m_YDim - iDim - m_nMarginY2, k = 0; i < m_YDim - m_nMarginY2; i++, k++)
	{
		for (j = iDim + m_nMarginX1; j < m_XDim - iDim - m_nMarginX2; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0, fKernel;
				fSum = 0.0;
				for (m = -iDim; m < iDim - k; m++)
					for (n = -iDim; n < iDim + 1; n++)
					{
						fKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter);
						fSum += fKernel;
						iPixel = (i + m) * m_XDim + j + n;
						if ((iPixel > nTotalPixel) || (iPixel < 0))
							fTemp = 0;
						fTemp += *(pSrc + iPixel) * fKernel;
					}
				if (fSum>0)
					fTemp /= fSum;
				else
					fTemp = 0.0;
				if (fTemp > dfMax)
					dfMax = fTemp ;
				pDestData[i * m_XDim + j] = fTemp;									
			}
		}
	}

	// left edge
	for (i = iDim + m_nMarginY1; i < m_YDim - iDim - m_nMarginY2; i++)
	{
		for (j = 0 + m_nMarginX1; j < iDim + m_nMarginX1; j++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0, fKernel;
				fSum = 0.0;
				for (m = -iDim; m < iDim ; m++)
					for (n = -j; n < iDim + 1; n++)
					{
						fKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter);
						fSum += fKernel;
						iPixel = (i + m) * m_XDim + j + n;
						if ((iPixel > nTotalPixel) || (iPixel < 0))
							fTemp = 0;
						fTemp += *(pSrc + iPixel) * fKernel;
					}
				if (fSum>0)
					fTemp /= fSum;
				else
					fTemp = 0.0;
				if (fTemp > dfMax)
					dfMax = fTemp ;
				pDestData[i * m_XDim + j] = fTemp;									
			}
		}
	}
	// right edge
	for (i = iDim + m_nMarginY1; i < m_YDim - iDim - m_nMarginY2; i++)
	{
		for (j = m_XDim - iDim - m_nMarginX2, k = 0; j < m_XDim - m_nMarginX2; j++, k++)
		{
			if ((m_nDirection < 3) || ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius))
			{
				double fTemp = 0.0, fKernel;
				fSum = 0.0;
				for (m = -iDim; m < iDim; m++)
					for (n = -iDim; n < iDim - k; n++)
					{
						fKernel = *(iFilter + m * (2 * iDim + 1) + n + iCenter) ;
						fSum += fKernel;
						iPixel = (i + m) * m_XDim + j + n;
						if ((iPixel > nTotalPixel) || (iPixel < 0))
							fTemp = 0;
						fTemp += *(pSrc + iPixel) * fKernel;
					}
				if (fSum>0)
					fTemp /= fSum;
				else
					fTemp = 0.0;
				if (fTemp > dfMax)
					dfMax = fTemp ;
				pDestData[i * m_XDim + j] = fTemp;									
			}
		}
	}

	memcpy(pSrc, pDestData, nTotalPixel * sizeof(double));
	
	delete [] pDestData;
	pDestData = NULL;

	return TRUE;
}

double CMeasurePhase::GetRMSValue2()
{
	LPBYTE pData;
	CSize imgSize;
	imgSize.cx = m_XDim;
	imgSize.cy = m_YDim;
	pData = new BYTE[imgSize.cx * imgSize.cy];

// 	double fXScale, fYScale;
	double fRMS = 0.0;
	double fSum = 0.0, fSum2 = 0.0;
	int iCount = 0;
	int i,j, iIndex;
	BYTE iData;

	if (pView)
		pView->GetImage(pData, imgSize);
	if (m_nDirection == 3)
	{
		fSum = fSum2 = 0.0;
		iCount = 0;
		for (i = 0+m_nMarginY1; i < m_YDim-m_nMarginY2; i++)
		{
			for (j = 0+m_nMarginX1; j < m_XDim-m_nMarginX2; j++)
			{
				iIndex = i*m_XDim + j;
				if ((j - m_nCenterX) * (j - m_nCenterX) + (i - m_nCenterY) * (i - m_nCenterY) <= m_nRadius * m_nRadius)
				{
					iData = pData[iIndex];
					fSum2 += 1.0 * iData * iData;
					fSum  += 1.0 * iData;
					iCount++;
				}
			}
		}
		if (iCount>0)
		{
			fRMS = sqrt((fSum2 - fSum * fSum/(double)iCount)/(iCount-1));
//			double fAve = fSum / (double)iCount;
		}
	}
	else
		fRMS = fGetRMSValue(pData);
	return fRMS;
}

void CMeasurePhase::SearchInterference()
{
//	CScannerBase *pScanner = m_pSystem->GetScanner();
//	pScanner->MoveZMotorRel(150.0, FALSE, 1.0);
	BOOL bDone = FALSE;
	LPBYTE pData;
	CSize imgSize;
	imgSize.cx = m_XDim;
	imgSize.cy = m_YDim;
	pData = new BYTE[imgSize.cx * imgSize.cy];

// 	double fXScale, fYScale;
//	EnableButtons(FALSE);
	UpdateData(TRUE);

//	CSI3300CamDlg* pDlg = AppViews.pS8Dlg->m_pSI3300CamDlg;
//	if (pDlg)
//		pDlg->GetImage(pData, imgSize);
/*	CMainFrame* pMainFrame = Global.GetMainFrame();
	CCameraView* pView = (CCameraView*) pMainFrame->GetCameraView();*/
	if (pView)
		pView->GetImage(pData, imgSize);
	fRMS = fGetRMSValue(pData);
	CString strVal;
	strVal.Format("%.3f", fRMS);
	GetDlgItem(IDC_UNWRAP_RMS_VALUE)->SetWindowText(strVal);

	if (fRMS <= m_fRMSThreshold)
	{
		float fZ = GetPosition(2);
		for (int i = -5 ;i<=5;i++)
		{
			if (i!=0)
			{
				GotoZ(fZ + i*0.005);
//				if (pDlg)
//					pDlg->GetImage(pData, imgSize);
				if (pView)
					pView->GetImage(pData, imgSize);
				fRMS = fGetRMSValue(pData);
				strVal.Format("%.3f", fRMS);
				GetDlgItem(IDC_UNWRAP_RMS_VALUE)->SetWindowText(strVal);
			}
			if (fRMS >= m_fRMSThreshold) break;
		}
		if (fRMS >= m_fRMSThreshold)
		{
			AppViews.pS8Dlg->m_fZ = fZ + i*m_fZStepSize;
		}
		else
		{
			GotoZ(fZ);
		}
	}
	if (pData)
	{
		delete [] pData;
		pData = NULL;
	}
//	EnableButtons(TRUE);
}

void CMeasurePhase::GetRMS()
{
	CSize imgSize;
	LPBYTE pData; 
	imgSize.cx = m_XDim;
	imgSize.cy = m_YDim;
	pData = new BYTE[imgSize.cx * imgSize.cy];
// 	double fXScale, fYScale;
//	pData = m_pSystem->GetVideoImage(OPTIC1_DIRECT, imgSize, &fXScale, &fYScale);
//	EnableButtons(FALSE);
	UpdateData(TRUE);

//	CSI3300CamDlg* pDlg = AppViews.pS8Dlg->m_pSI3300CamDlg;
//	if (pDlg)
//		pDlg->GetImage(pData, imgSize);
//	CMainFrame* pMainFrame = Global.GetMainFrame();
//	CCameraView* pView = (CCameraView*) pMainFrame->GetCameraView();
	if (pView)
		pView->GetImage(pData, imgSize);
	double fRMS = fGetRMSValue(pData);
	CString strVal;
	strVal.Format("%.3f", fRMS);
	GetDlgItem(IDC_UNWRAP_RMS_VALUE)->SetWindowText(strVal);
	delete [] pData;
	pData = NULL;
//	EnableButtons(TRUE);
}

double CMeasurePhase::fGetRMSValue(LPBYTE pData)
{
	int iKernel[25];
	for (int i = 0; i < 25; i++)
		iKernel[i] = 1;
//	LPBYTE pDestData = new BYTE[m_XDim * m_YDim];
//	DoKernelFilter(iKernel, 2, pData, pDestData);

	// the problem with getting the RMS for the whole image is that the 
	//  bow in the intersity will give large RMS. T.W.
	// divide into 25 regions, 
	// sample the 9 center regions to get the RMS value
	double fRMSArray[9];
	double fRoughArray[9];
	int x, y; 
	int iXStep, iYStep;
	iXStep = m_XDim /5;
	iYStep = m_YDim /5;
	y = 0;
	int nIndex = 0;
	double fAveRMS = 0.0;
	for (int m = 0; m < 3; m++)
	{
		x = iXStep;
		y += iYStep;
		for (int n = 0; n < 2; n++)
		{
			double fResult[2];
			x += iXStep;
			GetRoughness(pData, fResult, x, y, iXStep, iYStep);
//			GetRoughness(pDestData, fResult, x, y, iXStep, iYStep);
			fRMSArray[nIndex] = fResult[0];
			fRoughArray[nIndex] = fResult[1];
			fAveRMS += fResult[0];
			nIndex++;
		}
	}

//	delete [] pDestData;
//	pDestData = NULL;
	if (nIndex>0)
		fAveRMS /= (double)nIndex;
	else
		fAveRMS = 0.0;
	return fAveRMS;
}

double CMeasurePhase::GetRoughness(LPBYTE pData, double *pResult, int iStartX, int iStartY, int iXDim, int iYDim)
{
	int iSkip = 1;//4;
	int x, y;
	int iCount = 0;
	double fRMS = 0.0;
	double fSum = 0.0, fSum2 = 0.0;
	for (y = iStartY; y < iStartY + iYDim; y += iSkip)
	{
		LPBYTE pPtr = pData + y * m_XDim;
		for (x = iStartX; x < iStartX + iXDim; x += iSkip)
		{
			BYTE iData = pPtr[x]; 
			fSum2 += iData * iData;
			fSum  += iData;
			iCount++;
		}
	}
	fRMS = sqrt((fSum2 - fSum * fSum/(double)iCount)/(iCount-1));
	double fAve = fSum / (double)iCount;
	double fErr2 = 0.0, fErr= 0.0;

	for (y = iStartY; y < iStartY + iYDim; y += iSkip)
	{
		LPBYTE pPtr = pData + y * m_XDim;
		{
			double fDiff = (double)pPtr[x] - fAve; 
			fErr2 += fDiff * fDiff;
			fErr  += fabs(fDiff);
		}
	}
	double fRoughness = fErr/(double)iCount;	

	pResult[0] = fRMS;
	pResult[1] = fRoughness;
	return fRMS;
}

BOOL CMeasurePhase::SaveImage(LPBYTE pImage, LPCTSTR lpFileName)
{
	if (pImage)
	{
		BITMAPINFOHEADER BitmapHead;
		ZeroMemory( &BitmapHead, sizeof(BITMAPINFOHEADER));
		BitmapHead.biWidth = m_XDim;     
		BitmapHead.biHeight= m_YDim;
		BitmapHead.biPlanes		= 1;
		BitmapHead.biBitCount	= 8; // 8 bit grey color
		BitmapHead.biSizeImage	= m_XDim * m_YDim;
		BitmapHead.biSize		= sizeof(BITMAPINFOHEADER);
		BitmapHead.biClrUsed	= 0;
		BitmapHead.biClrImportant = 0;

		CFile file;
	    if (!file.Open(lpFileName, CFile::modeWrite|CFile::modeCreate))
			return FALSE;
			
		// assumes 256 grey level
		BITMAPFILEHEADER   hdr;
		DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(hdr) + 256 * sizeof(RGBQUAD);
		RGBQUAD	*pRGB = new RGBQUAD[256];
		for (int i = 0; i < 256; i++)
		{
			pRGB[i].rgbRed = i;
			pRGB[i].rgbGreen = i;
			pRGB[i].rgbBlue = i;
		}
		// Fill in the fields of the file header 
		hdr.bfType       = DS_BITMAP_FILEMARKER;
		hdr.bfSize       = dwFileHeaderSize + BitmapHead.biSizeImage;
		hdr.bfReserved1  = 0;
		hdr.bfReserved2  = 0;
		hdr.bfOffBits    = dwFileHeaderSize;

		// Write the file header 
		file.Write(&hdr, sizeof(hdr));

		// Write the DIB header
		file.Write(&BitmapHead, sizeof(BITMAPINFOHEADER));
		file.Write(pRGB, sizeof(RGBQUAD) * 256);
		file.Write(pImage, m_XDim * m_YDim);	
		file.Close();
		return TRUE;
	}
	return FALSE;			
}

BOOL CMeasurePhase::SaveImage(double *pImage, LPCTSTR lpFileName)
{
	double fMin = 1e10;
	double fMax = -1e10;
	int iSize = m_XDim * m_YDim;
	for (int i = 0; i < iSize; i++)
	{
		if (pImage[i] > fMax) fMax = pImage[i];
		if (pImage[i] < fMin) fMin = pImage[i];
	}
	double fScale = 255.0/(fMax - fMin);
	LPBYTE pData = new BYTE[iSize];
	for (i = 0; i < iSize; i++)
	{
		pData[i] = min(255, (pImage[i] - fMin) * fScale);
	}
	BOOL bRet = SaveImage(pData, lpFileName);
	delete [] pData;
	pData = NULL;
	return bRet;
}

void CMeasurePhase::OnMoveLeft()
{	
	if (!bSimu)
	{
		EnableButtons(FALSE);
		UpdateData(TRUE);
		float fPZT = GetPosition(99);
		float fZ = GetPosition(2);
		if ((fPZT - m_fZStepSize < 0.0) || (fPZT - m_fZStepSize > 10.0))
		{
			if (fPZT + m_fZStepSize < 0.0)
			{
				fZ -= 0.1f;
				fPZT += 10.0f;
			}
			if (fPZT + m_fZStepSize > 10.0)
			{
				fZ += 0.1f;
				fPZT -= 10.0f;
			}
			GotoZ(fZ);
		}
		GotoPZT(0);
		GotoPZT(fPZT - m_fZStepSize);
		AppViews.pS8Dlg->m_fPZT = fPZT - m_fZStepSize;
		AppViews.pS8Dlg->m_fZ = fPZT - m_fZStepSize;
		EnableButtons(TRUE);
	}
}

void CMeasurePhase::OnMoveRight()
{
	if (!bSimu)
	{
		EnableButtons(FALSE);
		UpdateData(TRUE);
		float fPZT = GetPosition(99);
		float fZ = GetPosition(2);
		if ((fPZT + m_fZStepSize < 0.0) || (fPZT + m_fZStepSize > 10.0))
		{
			if (fPZT + m_fZStepSize < 0.0)
			{
				fZ -= 0.1f;
				fPZT += 10.0f;
			}
			if (fPZT + m_fZStepSize > 10.0)
			{
				fZ += 0.1f;
				fPZT -= 10.0f;
			}
			GotoZ(fZ);
		}
		GotoPZT(0);
		GotoPZT(fPZT + m_fZStepSize);
		AppViews.pS8Dlg->m_fPZT = fPZT + m_fZStepSize;
		AppViews.pS8Dlg->m_fZ = fPZT + m_fZStepSize;
		EnableButtons(TRUE);
	}
}

void CMeasurePhase::EnableButtons(BOOL bEnable)
{
	GetDlgItem(IDC_UNWRAP_X)->EnableWindow(bEnable);
	GetDlgItem(IDC_UNWRAP_Y)->EnableWindow(bEnable);
	GetDlgItem(IDC_UNWRAP_SAVE)->EnableWindow(bEnable);
	GetDlgItem(IDC_MOVE_LEFT)->EnableWindow(bEnable);
	GetDlgItem(IDC_MOVE_RIGHT)->EnableWindow(bEnable);
	GetDlgItem(IDC_UNWRAP_PROCESS)->EnableWindow(bEnable);
}

void CMeasurePhase::Gotoxy(float fX, float fY)
{
	CSMHdr SM;
	SM.cmd = CSMHdr::CTGOTOXY;
	SM.sub[0] = fX;
	SM.sub[1] = fY;
	
	CS8Dlg* pDlg = AppViews.pS8Dlg;
	if (pDlg)
	{		
		if (pDlg->ipcS8.Send(&SM, sizeof(CSMHdr),240000))
		{
			if(SM.ret[0] == -1)
			{
				AfxMessageBox("Error In Motion Switch Open");
			}
			else
			{
				pDlg->m_fX = fX;
				pDlg->m_fY = fY;
			}
		}
		else
		{
			AfxMessageBox("Fail GOTOXY");
		}
	}
	else
	{
		AfxMessageBox("Fail GOTOXY");
	}
}

void CMeasurePhase::GotoZ(float fZ) 
{
	if (fZ < ZPOSMAX)
	{
		CSMHdr SM;
		SM.cmd = CSMHdr::CTPROBEDOWN;
		SM.sub[0] = fZ;
		CS8Dlg* pDlg = AppViews.pS8Dlg;
		if (pDlg)
		{		
			if (pDlg->ipcS8.Send(&SM, sizeof(CSMHdr),240000))
			{
				if(SM.ret[0] == -1)
				{
					AfxMessageBox("Error In Motion Switch Open");
				}
				else
					pDlg->m_fZ = fZ;
			}
			else
			{
				AfxMessageBox("Fail GOTOZ");
			}
		}
		else
		{
			AfxMessageBox("Fail GOTOZ");
		}
	}
}

float CMeasurePhase::GotoPZT(float fPZT) 
{
	float fADC, /*fADC2, fDelta,*/ fDat;
// 	float eps;
	int i, /*nCount,*/ nRpt=10;
	fADC = -9999;

//	fDat = fPZT;
	if (fPZT<=0) 
		fDat = 0;
	else if (fPZT>=10) 
		fDat = 10;
	else
	{
//		i = (int) fPZT;
//		fDat = fADCCal[i]+(fADCCal[i+1]-fADCCal[i])*(fPZT-i);
		fDat = fPZT;
	}

	if (AppViews.pS8Dlg)
	{
		if(AppViews.pS8Dlg->m_pDaq)
		{
			AppViews.pS8Dlg->m_pDaq->vSetDACOut(0,fPZT);
			Sleep(nPiezoDelay);
			fADC = 0;
			for (i=0;i<nRpt;i++)
			{
				fADC += AppViews.pS8Dlg->m_pDaq->fReadADCSingle(0, 0);
				Sleep(10);
			}
			fADC /= nRpt;
//			fADC2 = 0;
//			for (i=0;i<nRpt;i++)
//			{
//				fADC2 += AppViews.pS8Dlg->m_pDaq->fReadADCSingle(0, 0);
//				Sleep(10);
//			}
//			fADC2 /= nRpt;
/*			fDelta = 0;
			nCount = 0;
			eps = m_fZStepSize/10;
			while ((nCount<10)&&(fabs(fADC-fDat)>eps))
//			while ((nCount<10)&&(fabs(fADC - fADC2)/nRpt>eps))
			{
				fDelta += fDat - fADC;
				AppViews.pS8Dlg->m_pDaq->vSetDACOut(0,fPZT+fDelta);
				Sleep(nPiezoDelay);
				fADC = 0;
				for (i=0;i<nRpt;i++)
				{
					fADC += AppViews.pS8Dlg->m_pDaq->fReadADCSingle(0, 0);
					Sleep(10);
				}
				fADC /= nRpt;
//				fADC2 = 0;
//				for (i=0;i<nRpt;i++)
//				{
//						fADC2 += AppViews.pS8Dlg->m_pDaq->fReadADCSingle(0, 0);
//					Sleep(10);
//				}
//				fADC2 /= nRpt;
				nCount++;
			}*/
			AppViews.pS8Dlg->m_fPZT = fPZT;
		}
		else
		{
			AfxMessageBox("Fail GOTOPZT");
		}
	}
	return (fADC-fDat);
}

float CMeasurePhase::GetPosition(int nAxis) 
{
	if ((0 <= nAxis)&&(7>= nAxis))
	{
		CSMHdr SM;
		SM.cmd = CSMHdr::CTGETPOS;
		CS8Dlg* pDlg = AppViews.pS8Dlg;
		if (pDlg)
		{		
			if (pDlg->ipcS8.Request(&SM,sizeof(CSMHdr), 100))
			{
				if ((0 != SM.ret[0]) && (SM.ret[1 + nAxis] < 1000.f))
				{
					return (SM.ret[1+nAxis]);
				}
				else
				{
					return(-9999.0);
				}
			}
			else
			{
				return(-9999.0);
			}
		}
		else
		{
			return(-9999.0);
		}
	}
	if (nAxis == 99)
	{
		CS8Dlg* pDlg = AppViews.pS8Dlg;
		if (pDlg)
		{		
			return (pDlg->m_fPZT);					
		}
		else
		{
			return(-9999.0);
		}
	}
	return(-9999.0);
}

BOOL CMeasurePhase::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::OnClose();
	return 0;
}

void CMeasurePhase::OnClose() 
{
	DestroyWindow();
}

float fMin = 1e10, fMax = -1e10;

void CMeasurePhase::Unwrap()
{
	double pi = atan2(0, -1);
	int PhaseCounter = 0;
	int iSize = m_XDim * m_YDim;
	int i, /*j,*/ X, Y, iIndex/*, iIndex2*/;
	double phasePrev, phaseNow;
	double fSum, fSum2, fRMSX, fRMSY, fData;
	int iCount;
	BOOL bNewPhase;
///
	int k, l, iDist;
	int Nsec = 720;
	double phasePrev2[720], phaseNow2[720];
	int PhaseCounter2[720];
//	BOOL PhaseFirstPoint[720];
///
	if (!m_pPhase)
		return;

//	EnableButtons(FALSE);
//	UpdateData(TRUE);

	SetColorBar();

	if (!m_pUnwrapped)
		return;
//	m_pUnwrapped = new double[iSize];
	memcpy(m_pUnwrapped, m_pPhase, iSize * sizeof(double));

	switch(m_nDirection)
	{
	case 0: //fringes are parallel to X axis
	for (X = 0+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
	{
		iIndex = m_nMarginY1 * m_XDim + X;
		m_pUnwrapped[iIndex] = m_pPhase[iIndex];
		phasePrev = m_pPhase[iIndex];
		PhaseCounter = 0;
		for (Y = m_nMarginY1+1; Y < m_YDim-m_nMarginY2; Y++)
		{
			iIndex = Y * m_XDim + X;
			phaseNow = m_pPhase[iIndex];
			if ((phaseNow - phasePrev) > pi) PhaseCounter++;
			if ((phasePrev - phaseNow) > pi) PhaseCounter--;
			m_pUnwrapped[iIndex] = phaseNow - 2.0 * PhaseCounter * pi;
			phasePrev = phaseNow;
		}
	} 
	break;
	case 1: //fringes are parallel to Y axis
	for (Y = 0+m_nMarginY1; Y < m_YDim-m_nMarginY2; Y++)
	{
		iIndex = Y * m_XDim + m_nMarginX1;
		m_pUnwrapped[iIndex] = m_pPhase[iIndex];
		phasePrev = m_pPhase[iIndex];
		PhaseCounter = 0;
		for (X = 1+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
		{
			iIndex = Y * m_XDim + X;
			phaseNow = m_pPhase[iIndex];
			if ((phaseNow - phasePrev) > pi) PhaseCounter++;
			if ((phasePrev - phaseNow) > pi) PhaseCounter--;
			m_pUnwrapped[iIndex] = phaseNow - 2.0 * PhaseCounter * pi;
			phasePrev = phaseNow;
		}
	} 
	break;
	case 2: //fringes are concentric, rectangular area
	//left quarter
	iIndex = m_nCenterX + m_nCenterY * m_XDim;
	m_pUnwrapped[iIndex] = m_pPhase[iIndex];
	phasePrev = m_pPhase[iIndex];
	PhaseCounter = 0;
///	
	memset(phasePrev2, m_pPhase[iIndex], Nsec * sizeof(double));
	memset(PhaseCounter2, 0, Nsec * sizeof(int));
//	memset(PhaseFirstPoint, 1, Nsec * sizeof(BOOL));
///
	for (k = 1; k < m_XDim+m_YDim; k++)
	{  
		for (X = 0+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
		{
			for (Y = 0+m_nMarginY1; Y < m_YDim-m_nMarginY2; Y++)
			{
				iDist = (X - m_nCenterX) * (X - m_nCenterX) + (Y - m_nCenterY) * (Y - m_nCenterY);
				if ((iDist >= k*k) && (iDist < (k+1)*(k+1)))
				{
					iIndex = X + m_XDim * Y;
					l = (int) (Nsec * (pi + atan2(Y-m_nCenterY, X-m_nCenterX)) / (2*pi));
					if (l<0) l=0;
					if (l>=Nsec) l = Nsec-1;
					phaseNow2[l] = m_pPhase[iIndex];
/*
					if ((iDist>2) && (PhaseFirstPoint[l]))
					{	
						int l1, l2;
						l1 = l-1; if (l1<0) l1 = Nsec-1;
						l2 = l+1; if (l2>=Nsec) l1 = 0;
						if ((!PhaseFirstPoint[l1]) && (!PhaseFirstPoint[l2]))
							phasePrev2[l] = 1/2*(phasePrev2[l1]+phasePrev2[l2]);
					}
*/
					if (phaseNow2[l] - phasePrev2[l] > pi)	PhaseCounter2[l]++;
					if (phasePrev2[l] - phaseNow2[l] > pi)	PhaseCounter2[l]--;
					m_pUnwrapped[iIndex] = phaseNow2[l] - 2.0 * PhaseCounter2[l] * pi;
					phasePrev2[l] = phaseNow2[l];
//					PhaseFirstPoint[l] = FALSE;
				}
			}
		}
	}
	break;
	case 3: //fringes are concentric, round area
	//left quarter
	iIndex = m_nCenterX + m_nCenterY * m_XDim;
	m_pUnwrapped[iIndex] = m_pPhase[iIndex];
	phasePrev = m_pPhase[iIndex];
	PhaseCounter = 0;
///	
	memset(phasePrev2, m_pPhase[iIndex], Nsec * sizeof(double));
	memset(PhaseCounter2, 0, Nsec * sizeof(int));
//	memset(PhaseFirstPoint, 1, Nsec * sizeof(BOOL));
///
	for (k = 1; k < m_nRadius; k++)
	{
		for (X = 0+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
		{
			for (Y = 0+m_nMarginY1; Y < m_YDim-m_nMarginY2; Y++)
			{
				iDist = (X - m_nCenterX) * (X - m_nCenterX) + (Y - m_nCenterY) * (Y - m_nCenterY);
				if ((iDist >= k*k) && (iDist < (k+1)*(k+1)))
				{
					iIndex = X + m_XDim * Y;
					l = (int) (Nsec * (pi + atan2(Y-m_nCenterY, X-m_nCenterX)) / (2*pi));
					if (l<0) l=0;
					if (l>=Nsec) l = Nsec-1;
					phaseNow2[l] = m_pPhase[iIndex];
/*
					if ((iDist>2) && (PhaseFirstPoint[l]))
					{	
						int l1, l2;
						l1 = l-1; if (l1<0) l1 = Nsec-1;
						l2 = l+1; if (l2>=Nsec) l1 = 0;
						if ((!PhaseFirstPoint[l1]) && (!PhaseFirstPoint[l2]))
							phasePrev2[l] = 1/2*(phasePrev2[l1]+phasePrev2[l2]);
					}
*/
					if (phaseNow2[l] - phasePrev2[l] > pi)	PhaseCounter2[l]++;
					if (phasePrev2[l] - phaseNow2[l] > pi)	PhaseCounter2[l]--;
					m_pUnwrapped[iIndex] = phaseNow2[l] - 2.0 * PhaseCounter2[l] * pi;
					phasePrev2[l] = phaseNow2[l];
//					PhaseFirstPoint[l] = FALSE;
				}
			}
		}
	}
	break;
	case 4: //fringes are parallel to either X or Y axis, search inside the round area
		fSum = fSum2 = 0.0;
		iCount = 0;
		fRMSY = 0;
		for (i = m_nCenterY-m_nRadius; i <= m_nCenterY+m_nRadius; i++)
		{
			iIndex = i*m_XDim + m_nRadius;
			fData = m_pPhase[iIndex];
			fSum2 += 1.0 * fData * fData;
			fSum  += 1.0 * fData;
			iCount++;
		}
		if (iCount>0)
		{
			fRMSY = sqrt((fSum2 - fSum * fSum/(double)iCount)/(iCount-1));
		}
		fSum = fSum2 = 0.0;
		iCount = 0;
		fRMSX = 0;
		for (i = m_nCenterY-m_nRadius; i <= m_nCenterY+m_nRadius; i++)
		{
			iIndex = m_nRadius*m_XDim + i;
			fData = m_pPhase[iIndex];
			fSum2 += 1.0 * fData * fData;
			fSum  += 1.0 * fData;
			iCount++;
		}
		if (iCount>0)
		{
			fRMSX = sqrt((fSum2 - fSum * fSum/(double)iCount)/(iCount-1));
		}
		if (fRMSY>fRMSX)
		{
			for (X = 0+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
			{
				bNewPhase = TRUE;
				for (Y = m_nMarginY1+1; Y < m_YDim-m_nMarginY2; Y++)
				{
					if ((X - m_nCenterX) * (X - m_nCenterX) + (Y - m_nCenterY) * (Y - m_nCenterY)
						<= m_nRadius * m_nRadius)
					{
						iIndex = Y * m_XDim + X;
						if (bNewPhase)
						{
							m_pUnwrapped[iIndex] = m_pPhase[iIndex];
							phasePrev = m_pPhase[iIndex];
							PhaseCounter = 0;
							bNewPhase = FALSE;
						}
						else
						{
							phaseNow = m_pPhase[iIndex];
							if ((phaseNow - phasePrev) > pi) PhaseCounter++;
							if ((phasePrev - phaseNow) > pi) PhaseCounter--;
							m_pUnwrapped[iIndex] = phaseNow - 2.0 * PhaseCounter * pi;
							phasePrev = phaseNow;
						}
					}
				}
			} 
		}
		else
		{
			for (Y = 0+m_nMarginY1; Y < m_YDim-m_nMarginY2; Y++)
			{
				bNewPhase = TRUE;
				for (X = 1+m_nMarginX1; X < m_XDim-m_nMarginX2; X++)
				{
					if ((X - m_nCenterX) * (X - m_nCenterX) + (Y - m_nCenterY) * (Y - m_nCenterY)
						<= m_nRadius * m_nRadius)
					{
						iIndex = Y * m_XDim + X;
						if (bNewPhase)
						{
							m_pUnwrapped[iIndex] = m_pPhase[iIndex];
							phasePrev = m_pPhase[iIndex];
							PhaseCounter = 0;
							bNewPhase = FALSE;
						}
						else
						{
							phaseNow = m_pPhase[iIndex];
							if ((phaseNow - phasePrev) > pi) PhaseCounter++;
							if ((phasePrev - phaseNow) > pi) PhaseCounter--;
							m_pUnwrapped[iIndex] = phaseNow - 2.0 * PhaseCounter * pi;
							phasePrev = phaseNow;
						}
					}
				}
			}
		} 
	break;

	default: break;
	}
	LevelShift(m_nDirection);

//	CMainFrame* pMainFrame = Global.GetMainFrame();
//	CCameraView* pView = (CCameraView*) pMainFrame->GetCameraView();
	int nWindow = 100;
	if (pView)
		nWindow = pView->nWindow;

	for (int y=0+m_nMarginY1; y<m_YDim-m_nMarginY2; y++)
	{
		for (int x=0+m_nMarginX1; x<m_XDim-m_nMarginX2; x++)
		{
			int iIndex = y * m_XDim + x;
			if (m_pMask[iIndex])
			{
				fCorrect[iIndex].x = (x - m_XDim/2)*0.8*m_fDiam*0.01*nWindow/m_XDim;
				fCorrect[iIndex].y = (y - m_YDim/2)*0.6*m_fDiam*0.01*nWindow/m_YDim;
				fCorrect[iIndex].z = m_pUnwrapped[iIndex] / (2*pi) * fWavelength/2;
			}
		}
	}
}

void CMeasurePhase::UnwrapPart2(BOOL bStress, float* pfData)
{
///calculate a curvature and bow/height of the area

	int i, x, y;
//	double S1 = 0.0, Sxx = 0.0, Syy = 0.0, Sz = 0.0, Sxxz = 0.0, Syyz = 0.0, Sxxyy = 0.0;
	double S1, Sr, Sz, Srr, Srrr, Srz, Srrz;
	CString strVal;
	double fMin = 1e20;
	double fMax = -1e20;

	fWarp = 0;
	fBowHeight = 0;
	fCurvature = 0;
	fStress = 0;

	BOOL bGetStress = bStress;
/*	if (!bGetStress)
	{
		bGetStress  = (IDYES == AfxMessageBox("Calculate stress? (Y/N)", MB_YESNO|MB_ICONQUESTION));
	}*/
	
//	CMainFrame* pMainFrame = Global.GetMainFrame();
//	CCameraView* pView = (CCameraView*) pMainFrame->GetCameraView();
	int nWindow = 100;
	if (pView)
		nWindow = pView->nWindow;

	if (bGetStress)
	{
		CalculateStress();

		fMin = 1e20;
		fMax = -1e20;
		for (y=0+m_nMarginY1; y<m_YDim-m_nMarginY2; y++)
		{
			for (x=0+m_nMarginX1; x<m_XDim-m_nMarginX2; x++)
			{
				int iIndex = y * m_XDim + x;
				if (m_pMask[iIndex])
				{
					if (fCorrect[iIndex].W < fMin)
						fMin = fCorrect[iIndex].W;
					if (fCorrect[iIndex].W > fMax)
						fMax = fCorrect[iIndex].W;
				}
			}
		}
		
		if (fMax > fMin)
			fWarp = (fMax-fMin);
		else
			fWarp = 0;

		S1 = Sr = Sz = Srr = Srrr = Srz = Srrz = 0;
		for (y=0+m_nMarginY1;y<m_YDim-m_nMarginY2;y++)
		{
			for (x=0+m_nMarginX1;x<m_XDim-m_nMarginX2;x++)
			{
				int iIndex = y * m_XDim + x;
				double r = sqrt(fCorrect[iIndex].x * fCorrect[iIndex].x + fCorrect[iIndex].y * fCorrect[iIndex].y);
				double z = fCorrect[iIndex].W;
				if (m_pMask[iIndex])
				{
					S1 += 1.0;
					Sr += r;
					Srr += (r * r);
					Srrr += (r * r * r);
					Sz += z;
					Srz += (r * z);
					Srrz += (r * r * z);
				}
			}
		}
		double det1 = S1 * Srz - Sz * Sr;
		double det2 = Srrr * Sz - Srr * Srz;
		double det = Srrr * S1 - Srr*Sr;
		if (det == 0)
		{
			fCurvature = 0;
			fBowHeight = 0;
		}
		else
		{
			fCurvature =  2 * det1 / det;
			fBowHeight =  det2 / det;
		}

//		fWarp = m_fDiam * m_fDiam * fCurvature / 8;

		m_nImageType = 2;
	}
	else
	{
		fMin = 1e20;
		fMax = -1e20;
		for (y=0+m_nMarginY1; y<m_YDim-m_nMarginY2; y++)
		{
			for (x=0+m_nMarginX1; x<m_XDim-m_nMarginX2; x++)
			{
				int iIndex = y * m_XDim + x;
				if (m_pMask[iIndex])
				{
					if (fCorrect[iIndex].z < fMin)
						fMin = fCorrect[iIndex].z;
					if (fCorrect[iIndex].z > fMax)
						fMax = fCorrect[iIndex].z;
				}
			}
		}
		
		if (fMax > fMin)
			fWarp = (fMax-fMin);//*fWavelegnth/4/pi;
		else
			fWarp = 0;
		
		S1 = Sr = Sz = Srr = Srrr = Srz = Srrz = 0;
		for (y=0+m_nMarginY1;y<m_YDim-m_nMarginY2;y++)
		{
			for (x=0+m_nMarginX1;x<m_XDim-m_nMarginX2;x++)
			{
				int iIndex = y * m_XDim + x;
				double r = sqrt(fCorrect[iIndex].x * fCorrect[iIndex].x + fCorrect[iIndex].y * fCorrect[iIndex].y);
				double z = fCorrect[iIndex].z;
				if (m_pMask[iIndex])
				{
					S1 += 1.0;
					Sr += r;
					Srr += (r * r);
					Srrr += (r * r * r);
					Sz += z;
					Srz += (r * z);
					Srrz += (r * r * z);
				}
			}
		}
		double det1 = S1 * Srz - Sz * Sr;
		double det2 = Srrr * Sz - Srr * Srz;
		double det = Srrr * S1 - Srr*Sr;
		if (det == 0)
		{
			fCurvature = 0;
			fBowHeight = 0;
		}
		else
		{
			fCurvature =  2 * det1 / det;
			fBowHeight =  det2 / det;
		}

//		fWarp = m_fDiam * m_fDiam * fCurvature / 8;

		m_nImageType = 0;
	}

//	UpdateData(FALSE);	
	strVal.Format("%.6f", fWarp);
	GetDlgItem(IDC_UNWRAP_WARP_VALUE)->SetWindowText(strVal);
	strVal.Format("%.6f", fBowHeight);
	GetDlgItem(IDC_UNWRAP_BOWHEIGHT_VALUE)->SetWindowText(strVal);
	strVal.Format("%.6f", fCurvature);
	GetDlgItem(IDC_UNWRAP_CURVATURE_VALUE)->SetWindowText(strVal);

	if ((m_fSubThk>0)&&(m_fFilmThk>0)&&(m_fYoungMod>0)&&(m_fPoisson>0)&&(m_fPoisson<1))
	{
		fStress = 1e-3 * m_fYoungMod / 6 / (1-m_fPoisson) * m_fSubThk * m_fSubThk / m_fFilmThk * fCurvature;
	}
	else
		fStress = 0;

	CTime CurTime = CTime::GetCurrentTime();
	double fTemp = 0.0;
	if (Global.nTemperatureController == 1)
		fTemp = gTmptCtrl.GetPV();

	if (bSaveData)
	{
		FILE* f = fopen("C:\\TEMP\\Monitordata.csv", "a");
		if (f)
		{
			fprintf(f, "%d-%d-%d-%d:%d:%d,",
				CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(),
				CurTime.GetHour(), CurTime.GetMinute(), CurTime.GetSecond()); 
			if (AppViews.pS8Dlg)
				fprintf(f, " %.2f, %.2f, %.3f, %.0f, %.0f, %.0f, %.1f,", 
				AppViews.pS8Dlg->m_fX, AppViews.pS8Dlg->m_fY, AppViews.pS8Dlg->m_fZ, 
				AppViews.pS8Dlg->m_fTilt1Pos, AppViews.pS8Dlg->m_fTilt2Pos, AppViews.pS8Dlg->m_fTilt3Pos,
				fTemp);
			fprintf(f, " %.2f, %5.3f, %8.6f, %8.3f, %8.6f, %8.6f, %8.6f", 
				0.01*nWindow*m_fDiam, fWarp, fCurvature, 1/fCurvature, fBowHeight, 
				0.01*nWindow*0.01*nWindow*m_fDiam*m_fDiam*fCurvature/8,	48*48*fCurvature/8);
			for (i=0;i<5;i++)
				fprintf(f, ", %.1f", 1e4*fADC[i]);
			fprintf(f,"\n");
			fclose(f);
		}
	}

	if (pfData)
	{
		pfData[0] = fWarp; 
		pfData[1] = fCurvature; 
		pfData[2] = fBowHeight;
	}
//	EnableButtons(TRUE);
}

void CMeasurePhase::CalculateStress()
{
	int i, j, iIndex;
	double pi = atan2(0, -1);
	double fMin = 1e20, fMax = -1e20;

	StressPoint * fCorrect2 = new StressPoint[(m_XDim - m_nMarginX1 - m_nMarginX2)
									 *(m_YDim - m_nMarginY1 - m_nMarginY2)];
///temporarily
//	bStressInitialized = FALSE;	
///
//	EnableButtons(FALSE);
//	CMainFrame* pMainFrame = Global.GetMainFrame();
//	CCameraView* pView = (CCameraView*) pMainFrame->GetCameraView();
	int nWindow = 100;
	if (pView)
		nWindow = pView->nWindow;

	for (int y=0+m_nMarginY1; y<m_YDim-m_nMarginY2; y++)
	{
		for (int x=0+m_nMarginX1; x<m_XDim-m_nMarginX2; x++)
		{
			int iIndex = y * m_XDim + x;
			if (m_pMask[iIndex])
			{
				if (fCorrect[iIndex].z < fMin)
					fMin = fCorrect[iIndex].z;
				if (fCorrect[iIndex].z > fMax)
					fMax = fCorrect[iIndex].z;
			}
		}
	}
	double fM0;
///exclude margins and center point (0,0)
	if (nAlgorithm == 0)
	{
		if ((m_fYoungMod > 0) && (m_fFilmThk > 0) && (m_fSubThk > 0) && (m_fPoisson > 0) 
			&& (m_fPoisson < 1) && (fMax - fMin > 0) && (m_fDiam > 0))
			fM0 = 2e-2 / 3 * m_fYoungMod * m_fSubThk * m_fSubThk 
			/ (1 - m_fPoisson) / m_fFilmThk * (fMax - fMin) / (0.01*nWindow*m_fDiam) / (0.01*nWindow*m_fDiam);
		else
			fM0 = 0;
		int iIndex2 = 0;
		float fZtemp;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
			{
				iIndex = i*m_XDim + j;
				if (m_pMask[iIndex])
				{
					if ((i != m_YDim/2) || (j != m_XDim/2))
					{
						fCorrect2[iIndex2].x = fCorrect[iIndex].x;
						fCorrect2[iIndex2].y = fCorrect[iIndex].y;
						fCorrect2[iIndex2].z = fCorrect[iIndex].z;
						iIndex2++;
					}
					else
					{
						fZtemp = fCorrect[iIndex].z;
					}
				}
			}
		}		
		if (!bStressInitialized)
		{
			Calc.Initialize(iIndex2, fCorrect2, 0.01*nWindow*m_fDiam, m_fPoisson);
			bStressInitialized = TRUE;
		}
		Calc.CalculateStressMatrix(iIndex2,	fCorrect2, 0.01*nWindow*m_fDiam, m_fPoisson);
/// put the data, for the central point - an interpolation of neighbouring 8 points
		iIndex2 = 0;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
			{
				iIndex = i*m_XDim + j;
				if (m_pMask[iIndex])
				{
					if ((i != m_YDim/2) || (j != m_XDim/2))
					{
						fCorrect[iIndex].W = fCorrect2[iIndex2].W;
						iIndex2++;
					}
					else
					{
						fCorrect[iIndex].W = 0.146*(fCorrect[iIndex+1].W + fCorrect[iIndex-1].W 
											+ fCorrect[iIndex-m_XDim].W + fCorrect[iIndex+m_XDim].W)
											+ 0.104*(fCorrect[iIndex+m_XDim+1].W + fCorrect[iIndex+m_XDim-1].W 
											+ fCorrect[iIndex-m_XDim+1].W + fCorrect[iIndex-m_XDim-1].W);
					}
				}
			}
		}

		float fRad = (m_XDim*m_XDim+m_YDim*m_YDim)/4/4;
		for (i=0+m_nMarginY1+2;i<m_YDim-m_nMarginY2-2;i++)
		{
			for(j=0+m_nMarginX1+2;j<m_XDim-m_nMarginX2-2;j++)	
			{
				iIndex = i*m_XDim + j;
				if (m_pMask[iIndex])
				{
					double sxx = (fCorrect[iIndex+2].W - 2*fCorrect[iIndex].W + fCorrect[iIndex-2].W)*fRad;
					double sxy = (fCorrect[iIndex+1+m_XDim].W - fCorrect[iIndex+1-m_XDim].W 
						- fCorrect[iIndex-1+m_XDim].W + fCorrect[iIndex-1-m_XDim].W)*fRad;
					double syy = (fCorrect[iIndex+2*m_XDim].W - 2*fCorrect[iIndex].W + fCorrect[iIndex-2*m_XDim].W)*fRad;
					fCorrect[iIndex].sigmaxx = (1/(1-m_fPoisson)*sxx + m_fPoisson/(1-m_fPoisson)*syy);
					fCorrect[iIndex].sigmaxy = sxy;
					fCorrect[iIndex].sigmayy = (m_fPoisson/(1-m_fPoisson)*sxx + 1/(1-m_fPoisson)*syy);
				}
			}
		}
///
		iIndex = m_XDim*m_YDim/2 + m_XDim/2;
		fCorrect[iIndex].sigmaxy = fCorrect[iIndex+1].sigmaxy = fCorrect[iIndex-1].sigmaxy
								 = fCorrect[iIndex+m_XDim].sigmaxy = fCorrect[iIndex-m_XDim].sigmaxy
								 = fCorrect[iIndex+m_XDim+1].sigmaxy = fCorrect[iIndex-m_XDim+1].sigmaxy
								 = fCorrect[iIndex+m_XDim-1].sigmaxy = fCorrect[iIndex-m_XDim-1].sigmaxy
								 = 0.25*(fCorrect[iIndex+2].sigmaxy + fCorrect[iIndex-2].sigmaxy 
								 + fCorrect[iIndex+m_XDim].sigmaxy + fCorrect[iIndex-m_XDim].sigmaxy);
///
		for(j=0+m_nMarginX1+2;j<m_XDim-m_nMarginX2-2;j++)	
		{
			int iIndex2 = (0+m_nMarginY1+2)*m_XDim + j;
			int iIndex3 = (m_YDim-m_nMarginY2-3)*m_XDim + j;
			if (m_pMask[iIndex2]&&m_pMask[iIndex3])
			{
				fCorrect[iIndex2-2*m_XDim].sigmaxx = fCorrect[iIndex2].sigmaxx;
				fCorrect[iIndex2-2*m_XDim].sigmaxy = fCorrect[iIndex2].sigmaxy;
				fCorrect[iIndex2-2*m_XDim].sigmayy = fCorrect[iIndex2].sigmayy;
				fCorrect[iIndex2-m_XDim].sigmaxx = fCorrect[iIndex2].sigmaxx;
				fCorrect[iIndex2-m_XDim].sigmaxy = fCorrect[iIndex2].sigmaxy;
				fCorrect[iIndex2-m_XDim].sigmayy = fCorrect[iIndex2].sigmayy;
				fCorrect[iIndex3+m_XDim].sigmaxx = fCorrect[iIndex3].sigmaxx;
				fCorrect[iIndex3+m_XDim].sigmaxy = fCorrect[iIndex3].sigmaxy;
				fCorrect[iIndex3+m_XDim].sigmayy = fCorrect[iIndex3].sigmayy;
				fCorrect[iIndex3+2*m_XDim].sigmaxx = fCorrect[iIndex3].sigmaxx;
				fCorrect[iIndex3+2*m_XDim].sigmaxy = fCorrect[iIndex3].sigmaxy;
				fCorrect[iIndex3+2*m_XDim].sigmayy = fCorrect[iIndex3].sigmayy;
			}
		}
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			int iIndex2 = i*m_XDim + 0+m_nMarginX1+2;
			int iIndex3 = i*m_XDim + m_XDim-m_nMarginX2-3;
			if (m_pMask[iIndex2]&&m_pMask[iIndex3])
			{
				fCorrect[iIndex2-2].sigmaxx = fCorrect[iIndex2].sigmaxx;
				fCorrect[iIndex2-2].sigmaxy = fCorrect[iIndex2].sigmaxy;
				fCorrect[iIndex2-2].sigmayy = fCorrect[iIndex2].sigmayy;
				fCorrect[iIndex2-1].sigmaxx = fCorrect[iIndex2].sigmaxx;
				fCorrect[iIndex2-1].sigmaxy = fCorrect[iIndex2].sigmaxy;
				fCorrect[iIndex2-1].sigmayy = fCorrect[iIndex2].sigmayy;
				fCorrect[iIndex3+1].sigmaxx = fCorrect[iIndex3].sigmaxx;
				fCorrect[iIndex3+1].sigmaxy = fCorrect[iIndex3].sigmaxy;
				fCorrect[iIndex3+1].sigmayy = fCorrect[iIndex3].sigmayy;
				fCorrect[iIndex3+2].sigmaxx = fCorrect[iIndex3].sigmaxx;
				fCorrect[iIndex3+2].sigmaxy = fCorrect[iIndex3].sigmaxy;
				fCorrect[iIndex3+2].sigmayy = fCorrect[iIndex3].sigmayy;
			}
		}
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
			{
				iIndex = i*m_XDim + j;
				if (m_pMask[iIndex])
				{
//					fCorrect[iIndex].W *= (fMax-fMin);
//					fCorrect[iIndex].W += fMin;
					fCorrect[iIndex].sigmaxx *= fM0;
					fCorrect[iIndex].sigmaxy *= fM0;
					fCorrect[iIndex].sigmayy *= fM0;
				}
			}
		}
///
		double fi, cs2, cssn, sn2;
		for (i=0+m_nMarginY1+2;i<m_YDim-m_nMarginY2-2;i++)
		{
			for(j=0+m_nMarginX1+2;j<m_XDim-m_nMarginX2-2;j++)	
			{
				iIndex = i*m_XDim + j;
				if (m_pMask[iIndex])
				{
					fi = Calc.ArcTangent(fCorrect[iIndex].x, fCorrect[iIndex].y);
					cs2 = cos(fi);
					sn2 = sin(fi);
					cssn = cs2 * sn2;
					cs2 *= cs2;
					sn2 *= sn2;
					fCorrect[iIndex].sigmarr = fCorrect[iIndex].sigmaxx * cs2 + 2 * fCorrect[iIndex].sigmaxy * cssn + fCorrect[iIndex].sigmayy * sn2;
					fCorrect[iIndex].sigmart = (fCorrect[iIndex].sigmayy - fCorrect[iIndex].sigmaxx) * cssn + fCorrect[iIndex].sigmaxy * (cs2 - sn2);
					fCorrect[iIndex].sigmatt = fCorrect[iIndex].sigmaxx * sn2 + 2 * fCorrect[iIndex].sigmaxy * cssn + fCorrect[iIndex].sigmayy * cs2;
				}
			}
		}
///
	}
////
	if (nAlgorithm == 1)
	{
		if ((m_fYoungMod > 0) && (m_fFilmThk > 0) && (m_fSubThk > 0) && (m_fPoisson > 0) 
			&& (m_fPoisson < 1) && (fMax - fMin > 0) && (m_fDiam > 0))
			fM0 = 2e-2 / 3 * m_fYoungMod * m_fSubThk * m_fSubThk 
			/ (1 - m_fPoisson) / m_fFilmThk * (fMax - fMin) / (0.01*nWindow*m_fDiam) / (0.01*nWindow*m_fDiam);
		else
			fM0 = 0;
		int iIndex2=0;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
			{
				iIndex = i*m_XDim + j;
				if (m_pMask[iIndex])
				{
					fCorrect2[iIndex2].x = fCorrect[iIndex].x;
					fCorrect2[iIndex2].y = fCorrect[iIndex].y;
					fCorrect2[iIndex2].z = fCorrect[iIndex].z;
					iIndex2++;
				}
			}
		}		
		Calc.CalculateCubic(iIndex2, fCorrect2, 0.01*nWindow*m_fDiam, m_fPoisson);
		iIndex2 = 0;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
			{
				iIndex = i*m_XDim + j;
				if (m_pMask[iIndex])
				{
					fCorrect[iIndex].W = fCorrect2[iIndex2].W;
					fCorrect[iIndex].sigmaxx = fM0 * fCorrect2[iIndex2].sigmaxx; 
					fCorrect[iIndex].sigmaxy = fM0 * fCorrect2[iIndex2].sigmaxy;
					fCorrect[iIndex].sigmayy = fM0 * fCorrect2[iIndex2].sigmayy;
					fCorrect[iIndex].sigmarr = fM0 * fCorrect2[iIndex2].sigmarr;
					fCorrect[iIndex].sigmart = fM0 * fCorrect2[iIndex2].sigmart;
					fCorrect[iIndex].sigmatt = fM0 * fCorrect2[iIndex2].sigmatt;
					iIndex2++;
				}
			}
		}		
	}


///
	if (fCorrect2)
		delete [] fCorrect2;
//	EnableButtons(TRUE);
}

void CMeasurePhase::UnwrapPart2b(BOOL bStress)
{
	int i,j;
	for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
	{
		for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
		{
			int iIndex = i * m_XDim + j;
			if (m_pMask[iIndex])
			{
				m_pUnwrapped2[iIndex] = fCorrect[iIndex].z;
			}
		}
	}

	for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
	{
		for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
		{
			int iIndex = i * m_XDim + j;
			if (m_pMask[iIndex])
			{
				m_pUnwrapped[iIndex] = fCorrect[iIndex].z;
			}
		}
	}
	SubtractBasePlane(fCorrect);
	for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
	{
		for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
		{
			int iIndex = i * m_XDim + j;
			if (m_pMask[iIndex])
			{
				m_pUnwrapped[iIndex] = fCorrect[iIndex].z;
			}
		}
	}
	UnwrapPart2(TRUE, NULL);		
}

void CMeasurePhase::OnUnwrapProcess() 
{
	int i, j, iRepeat;
	UpdateData(TRUE);
	EnableButtons(FALSE);
	if ((AppViews.pS8Dlg)&&(Global.bStopMotor))
	{
		AppViews.pS8Dlg->StopMotor();
	}
for (iRepeat = 0; iRepeat<m_nMeasRepeat; iRepeat++)
{
/*   	CFileDialog dlg (FALSE,		// save
					 "TXT",		// no default extension
					 NULL,		// no initial file name
					 OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
					 "Text files|*.TXT|");
	if (dlg.DoModal()==IDOK) 
	{
		m_chFileName = dlg.GetPathName();*/
		UpdateData(FALSE);
		if (!bSimu)
		{
//			SearchInterference();
//if (iRepeat == 0)
			MeasureImages();
		}
		else
		{
			if (!ReadBmpFiles()) { EnableButtons(TRUE); return;}
		}
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		UnwrapMask();
		Unwrap();

		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
			{
				int iIndex = i * m_XDim + j;
				if (m_pMask[iIndex])
				{
					m_pUnwrapped2[iIndex] = fCorrect[iIndex].z;
				}
			}
		}

/*		if (IDYES == AfxMessageBox("Use reference?", MB_YESNO|MB_ICONQUESTION)) 
		{
			if (!ReadBmpFiles()) { EnableButtons(TRUE); return;}
			bSetupMask = FALSE;
			UnwrapMask();
			bSetupMask = TRUE;
			Unwrap();				
			for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
			{
				for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
				{
					int iIndex = i * m_XDim + j;
					if (m_pMask[iIndex])
					{
						fCorrect[iIndex].z = m_pUnwrapped2[iIndex] - fCorrect[iIndex].z;
//						m_pUnwrapped[iIndex] = -fCorrect[iIndex].z;
//						fCorrect[iIndex].z = m_pUnwrapped[iIndex];
						m_pUnwrapped[iIndex] = fCorrect[iIndex].z;
					}
				}
			}
		}
		else
		{*/
			for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
			{
				for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
				{
					int iIndex = i * m_XDim + j;
					if (m_pMask[iIndex])
					{
						m_pUnwrapped[iIndex] = fCorrect[iIndex].z;
					}
				}
			}
//		}
		SubtractBasePlane(fCorrect);
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for(j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)	
			{
				int iIndex = i * m_XDim + j;
				if (m_pMask[iIndex])
				{
					m_pUnwrapped[iIndex] = fCorrect[iIndex].z;
				}
			}
		}
		UnwrapPart2(TRUE, NULL);		
//		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
//	}
}
	SetBitmap(m_nImageType, TRUE);
	Invalidate();
	SaveParam();
	if ((AppViews.pS8Dlg)&&(Global.bStopMotor))
	{
		AppViews.pS8Dlg->RunMotor();
	}
	EnableButtons(TRUE);
}	

void CMeasurePhase::SetCCDCorrection(int Xdim, int Ydim, StressPoint* fCorr)
{
/*	if (fCorrect)
	{
		delete [] fCorrect;
		fCorrect = NULL;
	}
	fCorrect = new StressPoint[Xdim*Ydim];
*/
	if (!fCorrect)
		return;
	for (int i=0;i<Xdim*Ydim;i++)
	{
		fCorrect[i].x = fCorr[i].x;	
		fCorrect[i].y = fCorr[i].y;	
		fCorrect[i].z = fCorr[i].z;	
	}

}

void CMeasurePhase::CorrectCCDMatrix(int Xdim, int Ydim, StressPoint* p1, StressPoint* p2)
{
	int i;
	if (!fCorrect) 
	{
		for (i=0;i<Xdim*Ydim;i++)
		{
			p2[i].x = p1[i].x;
			p2[i].y = p1[i].y;
			p2[i].z = p1[i].z;
		}
		return;
	}
	for (i=0;i<Xdim*Ydim;i++)
	{
		p2[i].x = p1[i].x*fCorrect[i].x;
		p2[i].y = p1[i].y*fCorrect[i].x;
		p2[i].z = p1[i].z;
	}
}

void CMeasurePhase::GetPlaneEquation(StressPoint *fP, double * fResult)
{
	double det, det1, det2, det3;

	det = fP[0].x * fP[1].y - fP[0].x * fP[2].y 
		- fP[0].y * fP[1].x + fP[0].y * fP[2].x 
		+ fP[1].x * fP[2].y - fP[2].x * fP[1].y;
	if (det == 0)
	{
		fResult[0] = 0;
		fResult[1] = 0;
		fResult[2] = 0;
		return;
	}
	else
	{
		det1 = fP[0].z * fP[1].y - fP[0].z * fP[2].y 
			 - fP[0].y * fP[1].z + fP[0].y * fP[2].z 
			 + fP[1].z * fP[2].y - fP[2].z * fP[1].y;
		det2 = fP[0].x * fP[1].z - fP[0].x * fP[2].z 
			 - fP[0].z * fP[1].x + fP[0].z * fP[2].x 
			 + fP[1].x * fP[2].z - fP[2].x * fP[1].z;
		det3 = fP[0].x * (fP[1].y * fP[2].z - fP[2].y * fP[1].z) 
			 - fP[0].y * (fP[1].x * fP[2].z - fP[2].x * fP[1].z) 
			 + fP[0].z * (fP[1].x * fP[2].y - fP[2].x * fP[1].y);
		fResult[0] = det1 / det;
		fResult[1] = det2 / det;
		fResult[2] = det3 / det;
	}
}

void CMeasurePhase::SubtractBasePlane(StressPoint *fP)
{
	double fMin = 1e20, fMax = -1e20;
	int i, j, iIndex, Nmax = 0;
	StressPoint fPbase[3];
	double fPlaneCoeff[3];
	double S1, Sx, Sy, Sz, Sxx, Sxy, Syy, Sxz, Syz;
	double det, det1, det2, det3, alpha, beta, gamma;
	int nCount, i0, j0, NN = 5;

	memset(fPlaneCoeff, 0 ,3*sizeof(double));

	switch (nRefPlaneType)
	{
	case 0:
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for (j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)
			{
				iIndex = i * m_XDim + j;
				if (m_pMask[iIndex])
				{
					double dist = (fP[iIndex].x - m_fX1) * (fP[iIndex].x - m_fX1) 
						+ (fP[iIndex].y - m_fY1) * (fP[iIndex].y - m_fY1);
					if (dist < fMin)
					{
						fMin = dist;
						Nmax = iIndex;
						i0 = i;
						j0 = j;
					}
				}
			}
		}
		nCount = 0;
		Sx = Sy = Sz = 0.0;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for (j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)
			{
				if ((abs(i-i0)<NN)&&(abs(j-j0)<NN))
				{
					iIndex = i * m_XDim + j;
					Sx += fP[iIndex].x;
					Sy += fP[iIndex].y;
					Sz += fP[iIndex].z;					
					nCount++;
				}
			}
		}
		if (nCount>0)
		{
			fPbase[0].x = Sx/nCount;
			fPbase[0].y = Sy/nCount;
			fPbase[0].z = Sz/nCount;
		}
//		fPbase[0].x = fP[Nmax].x;
//		fPbase[0].y = fP[Nmax].y;
//		fPbase[0].z = fP[Nmax].z;

		fMin = 1e20;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for (j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)
			{
				iIndex = i * m_XDim + j;
				if (m_pMask[iIndex])
				{
					double dist = (fP[iIndex].x - m_fX2) * (fP[iIndex].x - m_fX2) 
						+ (fP[iIndex].y - m_fY2) * (fP[iIndex].y - m_fY2);
					if (dist < fMin)
					{
						fMin = dist;
						Nmax = iIndex;
						i0 = i;
						j0 = j;
					}
				}
			}
		}
		nCount = 0;
		Sx = Sy = Sz = 0.0;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for (j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)
			{
				if ((abs(i-i0)<NN)&&(abs(j-j0)<NN))
				{
					iIndex = i * m_XDim + j;
					Sx += fP[iIndex].x;
					Sy += fP[iIndex].y;
					Sz += fP[iIndex].z;					
					nCount++;
				}
			}
		}
		if (nCount>0)
		{
			fPbase[1].x = Sx/nCount;
			fPbase[1].y = Sy/nCount;
			fPbase[1].z = Sz/nCount;
		}
//		fPbase[1].x = fP[Nmax].x;
//		fPbase[1].y = fP[Nmax].y;
//		fPbase[1].z = fP[Nmax].z;

		fMin = 1e20;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for (j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)
			{
				iIndex = i * m_XDim + j;
				if (m_pMask[iIndex])
				{
					double dist = (fP[iIndex].x - m_fX3) * (fP[iIndex].x - m_fX3) 
						+ (fP[iIndex].y - m_fY3) * (fP[iIndex].y - m_fY3);
					if (dist < fMin)
					{
						fMin = dist;
						Nmax = iIndex;
						i0 = i;
						j0 = j;
					}
				}
			}
		}
		nCount = 0;
		Sx = Sy = Sz = 0.0;
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for (j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)
			{
				if ((abs(i-i0)<NN)&&(abs(j-j0)<NN))
				{
					iIndex = i * m_XDim + j;
					Sx += fP[iIndex].x;
					Sy += fP[iIndex].y;
					Sz += fP[iIndex].z;					
					nCount++;
				}
			}
		}
		if (nCount>0)
		{
			fPbase[2].x = Sx/nCount;
			fPbase[2].y = Sy/nCount;
			fPbase[2].z = Sz/nCount;
		}
//		fPbase[2].x = fP[Nmax].x;
//		fPbase[2].y = fP[Nmax].y;
//		fPbase[2].z = fP[Nmax].z;
		
		GetPlaneEquation(fPbase, fPlaneCoeff);
		break;
	case 1:
/// subtract a best-fit plane
		S1 = Sx = Sy = Sz = Sxx = Sxy = Syy = Sxz = Syz = 0;

		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for (j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)
			{
				iIndex = i * m_XDim + j;
				if (m_pMask[iIndex])
				{
					S1 += 1.0;
					Sx += fP[iIndex].x;
					Sxx += fP[iIndex].x * fP[iIndex].x;
					Sy += fP[iIndex].y;
					Syy += fP[iIndex].y * fP[iIndex].y;
					Sxy += fP[iIndex].x * fP[iIndex].y;
				}
			}
		}

		det  = Sxx*(Syy*S1 - Sy*Sy) - Sxy*(Sxy*S1 - Sx*Sy) + Sx*(Sxy*Sy - Sx*Syy);
		det1 = Sxz*(Syy*S1 - Sy*Sy) - Sxy*(Syz*S1 - Sz*Sy) + Sx*(Syz*Sy - Sz*Syy);
		det2 = Sxx*(Syz*S1 - Sz*Sy) - Sxz*(Sxy*S1 - Sx*Sy) + Sx*(Sxy*Sz - Sx*Syz);
		det3 = Sxx*(Syy*Sz - Sy*Syz) - Sxy*(Sxy*Sz - Sx*Syz) + Sxz*(Sxy*Sy - Sx*Syy);

		if (det == 0)
		{
			alpha = beta = gamma = 0;
		}
		else
		{
			alpha = det1/det;
			beta = det2/det;
			gamma = det3/det;
		}

		fPlaneCoeff[0] = alpha;
		fPlaneCoeff[1] = beta;
		fPlaneCoeff[2] = gamma;
		break;
	default: break;
	}
///
	if (fPlaneCoeff[0]*fPlaneCoeff[1]*fPlaneCoeff[2] !=0)
	{
		for (i=0+m_nMarginY1;i<m_YDim-m_nMarginY2;i++)
		{
			for (j=0+m_nMarginX1;j<m_XDim-m_nMarginX2;j++)
			{
				iIndex = i * m_XDim + j;
				if (m_pMask[iIndex])
				{
					fP[iIndex].z -= (fPlaneCoeff[0]*fP[iIndex].x + fPlaneCoeff[1]*fP[iIndex].y	+ fPlaneCoeff[2]);
				}
			}
		}
	}
	else
	{
//		AfxMessageBox("No reference plane subtracted");// or plane has been already built");
	}
//	return;
}

void CMeasurePhase::LoadParam()
{
	char inifile[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetModuleFileName(NULL, inifile, _MAX_PATH);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "FSMLocalCfg", "ini");
	bSimu = GetPrivateProfileInt("Stress", "bSimu", 0, inifile);
	char str[255];

	nPiezoDelay = GetPrivateProfileInt("Piezo", "Delay", 1000, inifile);
	sprintf(str, "%.6f", m_fZStepSize);
	GetPrivateProfileString("Piezo", "StepSize", str, str, 255, inifile);
	m_fZStepSize = atof(str);
	nPZTPosType = GetPrivateProfileInt("Piezo", "PZTPosType", 0, inifile);

	sprintf(str, "%.3f", fWavelength);
	GetPrivateProfileString("Stress", "Wavelength", str, str, 255, inifile);
	fWavelength = atof(str);
	sprintf(str, "%.2f", m_fX1);
	GetPrivateProfileString("Stress", "X1", str, str, 255, inifile);
	m_fX1 = atof(str);
	sprintf(str, "%.2f", m_fY1);
	GetPrivateProfileString("Stress", "Y1", str, str, 255, inifile);
	m_fY1 = atof(str);
	sprintf(str, "%.2f", m_fX2);
	GetPrivateProfileString("Stress", "X2", str, str, 255, inifile);
	m_fX2 = atof(str);
	sprintf(str, "%.2f", m_fY2);
	GetPrivateProfileString("Stress", "Y2", str, str, 255, inifile);
	m_fY2 = atof(str);
	sprintf(str, "%.2f", m_fX3);
	GetPrivateProfileString("Stress", "X3", str, str, 255, inifile);
	m_fX3 = atof(str);
	sprintf(str, "%.2f", m_fY3);
	GetPrivateProfileString("Stress", "Y3", str, str, 255, inifile);
	m_fY3 = atof(str);
	sprintf(str, "%.2f", m_fDiam);
	GetPrivateProfileString("Stress", "Diam", str, str, 255, inifile);
	m_fDiam = atof(str);
	sprintf(str, "%.2f", m_fSubThk);
	GetPrivateProfileString("Stress", "Thksubstr", str, str, 255, inifile);
	m_fSubThk = atof(str);
	sprintf(str, "%.2f", m_fFilmThk);
	GetPrivateProfileString("Stress", "Thkfilm", str, str, 255, inifile);
	m_fFilmThk = atof(str);
	sprintf(str, "%.2f", m_fYoungMod);
	GetPrivateProfileString("Stress", "Young", str, str, 255, inifile);
	m_fYoungMod = atof(str);
	sprintf(str, "%.2f", m_fPoisson);
	GetPrivateProfileString("Stress", "Poisson", str, str, 255, inifile);
	m_fPoisson = atof(str);
	sprintf(str, "%.3f", m_fRMSThreshold);
	GetPrivateProfileString("Stress", "RMSThreshold", str, str, 255, inifile);
	m_fRMSThreshold = atof(str);
	sprintf(str, "%.3f", m_fMaskThreshold);
	GetPrivateProfileString("Stress", "MaskThreshold", str, str, 255, inifile);
	m_fMaskThreshold = atof(str);
	m_nDirection = GetPrivateProfileInt("Stress", "Direction", 0, inifile);
	sprintf(str, "%.4f", m_fImageMin);
	GetPrivateProfileString("Stress", "ImageMin", str, str, 255, inifile);
	m_fImageMin = atof(str);
	sprintf(str, "%.4f", m_fImageMax);
	GetPrivateProfileString("Stress", "ImageMax", str, str, 255, inifile);
	m_fImageMax = atof(str);
	m_nMarginX1 = GetPrivateProfileInt("Stress", "MarginX1", 0, inifile);
	m_nMarginX2 = GetPrivateProfileInt("Stress", "MarginX2", 0, inifile);
	m_nMarginY1 = GetPrivateProfileInt("Stress", "MarginY1", 0, inifile);
	m_nMarginY2 = GetPrivateProfileInt("Stress", "MarginY2", 0, inifile);
	m_nCenterX = GetPrivateProfileInt("Stress", "CenterX", 512/2, inifile);
	m_nCenterY = GetPrivateProfileInt("Stress", "CenterY", 384/2, inifile);
	m_nRepeat = GetPrivateProfileInt("Stress", "Repeat", 5, inifile);
	m_nRadius = GetPrivateProfileInt("Stress", "Radius", 192, inifile);
	nAlgorithm = GetPrivateProfileInt("Stress", "Algorithm", 0, inifile);
	bSaveData = GetPrivateProfileInt("Stress", "SaveData", 0, inifile);
	bSaveImage = GetPrivateProfileInt("Stress", "SaveImage", 0, inifile);
	nRefPlaneType = GetPrivateProfileInt("Stress", "RefPlaneType", 0, inifile);
}

void CMeasurePhase::SaveParam()
{
	char inifile[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetModuleFileName(NULL, inifile, _MAX_PATH);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "FSMLocalCfg", "ini");

	char str[255];
	sprintf(str, "%.6f", m_fZStepSize);
	WritePrivateProfileString("Piezo", "StepSize", str, inifile);
	sprintf(str, "%d", m_nDirection);
	WritePrivateProfileString("Stress", "Direction", str, inifile);
	sprintf(str, "%.3f", m_fRMSThreshold);
	WritePrivateProfileString("Stress", "RMSThreshold", str, inifile);
	sprintf(str, "%.3f", m_fMaskThreshold);
	WritePrivateProfileString("Stress", "MaskThreshold", str, inifile);
	sprintf(str, "%.2f", m_fX1);
	WritePrivateProfileString("Stress", "X1", str, inifile);
	sprintf(str, "%.2f", m_fY1);
	WritePrivateProfileString("Stress", "Y1", str, inifile);
	sprintf(str, "%.2f", m_fX2);
	WritePrivateProfileString("Stress", "X2", str, inifile);
	sprintf(str, "%.2f", m_fY2);
	WritePrivateProfileString("Stress", "Y2", str, inifile);
	sprintf(str, "%.2f", m_fX3);
	WritePrivateProfileString("Stress", "X3", str, inifile);
	sprintf(str, "%.2f", m_fY3);
	WritePrivateProfileString("Stress", "Y3", str, inifile);
	sprintf(str, "%.2f", m_fDiam);
	WritePrivateProfileString("Stress", "Diam", str, inifile);
	sprintf(str, "%.2f", m_fSubThk);
	WritePrivateProfileString("Stress", "Thksubstr", str, inifile);
	sprintf(str, "%.2f", m_fFilmThk);
	WritePrivateProfileString("Stress", "Thkfilm", str, inifile);
	sprintf(str, "%.2f", m_fYoungMod);
	WritePrivateProfileString("Stress", "Young", str, inifile);
	sprintf(str, "%.2f", m_fPoisson);
	WritePrivateProfileString("Stress", "Poisson", str, inifile);
	sprintf(str, "%.4f", m_fImageMin);
	WritePrivateProfileString("Stress", "ImageMin", str, inifile);
	sprintf(str, "%.4f", m_fImageMax);
	WritePrivateProfileString("Stress", "ImageMax", str, inifile);
	sprintf(str, "%d", m_nMarginX1);
	WritePrivateProfileString("Stress", "MarginX1", str, inifile);
	sprintf(str, "%d", m_nMarginX2);
	WritePrivateProfileString("Stress", "MarginX2", str, inifile);
	sprintf(str, "%d", m_nMarginY1);
	WritePrivateProfileString("Stress", "MarginY1", str, inifile);
	sprintf(str, "%d", m_nMarginY2);
	WritePrivateProfileString("Stress", "MarginY2", str, inifile);
	sprintf(str, "%d", m_nCenterX);
	WritePrivateProfileString("Stress", "CenterX", str, inifile);
	sprintf(str, "%d", m_nCenterY);
	WritePrivateProfileString("Stress", "CenterY", str, inifile);
	sprintf(str, "%d", m_nRepeat);
	WritePrivateProfileString("Stress", "Repeat", str, inifile);
	sprintf(str, "%d", m_nRadius);
	WritePrivateProfileString("Stress", "Radius", str, inifile);
}


BOOL CMeasurePhase::ReadBmpFiles() 
{
	// TODO: Add your control notification handler code here
//	EnableButtons(FALSE);
	UpdateData(TRUE);
	BOOL bResult;

	int i,j,k;

	bResult = TRUE;

	CFileDialog dlg(TRUE, "BMP", NULL,
			   OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,
			   "Bitmap files|*.BMP|");
	if (dlg.DoModal()==IDOK) 
	{
		CString chFileName = dlg.GetPathName();
		int iLength = chFileName.GetLength();
		char str[255];
	
		SetBitmap2(m_pImg[0]);

		for (i=0;i<5;i++)
		{
			sprintf(str,"%s", chFileName);
			str[iLength-5] = i+48;
			LPBITMAPINFOHEADER bmf  = m_DIBinfo1.operator LPBITMAPINFOHEADER();
			LPBITMAPINFO bmi  = m_DIBinfo1.operator LPBITMAPINFO();
			DWORD dwTmp;
			int Nlines, Npoints;
			HANDLE hf = CreateFile( str, GENERIC_READ, 
				(DWORD) 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 
				(HANDLE) NULL); 		
			if (hf != INVALID_HANDLE_VALUE)
			{
				ReadFile(hf, bmf, sizeof(BITMAPFILEHEADER), &dwTmp, NULL);
				ReadFile(hf, bmi, sizeof(BITMAPINFOHEADER), &dwTmp, NULL);
				Nlines = bmi->bmiHeader.biHeight;
				Npoints = bmi->bmiHeader.biWidth;
				ReadFile(hf, m_ppvBits1, Nlines*Npoints*sizeof(UCHAR), &dwTmp, NULL);	
				CloseHandle(hf);	
			}
			else
				return FALSE;
			BYTE* pBits = (BYTE*) m_ppvBits1;
			for (k=0; k<Nlines; k++)
			{
				for (j=0; j<Npoints; j++)
				{
					double Color = 0.0;
					m_pImg[i][Npoints * k + j] = (double) pBits[Npoints*k+j];
				}
			}
/*			int iKernel[9];
			for (int m = 0; m < 9; m++)
				iKernel[m] = 1;
			DoKernelFilter(iKernel, 1, m_pImg[i], NULL);
			SetBitmap2(m_pImg[i]);*/
		}
		UpdateData(FALSE);
	}
	else
		return FALSE;
//	EnableButtons(TRUE);
	return TRUE;
}

void CMeasurePhase::OnKillfocusImageMin() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);	
	SetColorBar();
	SetBitmap(m_nImageType, TRUE);
	Invalidate();
}

void CMeasurePhase::OnKillfocusImageMax() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);	
	SetColorBar();
	SetBitmap(m_nImageType, TRUE);
	Invalidate();
}

void CMeasurePhase::OnKillfocusImageType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);	
	SetBitmap(m_nImageType, TRUE);
	Invalidate();
}

void CMeasurePhase::OnLButtonUp(UINT nFlags, CPoint point) 
{
//	CView::OnLButtonUp(nFlags, point);
	CRect rc;
	GetDlgItem(IDC_IMG)->GetWindowRect(&rc);
	ScreenToClient(rc);
	int nX = (point.x - rc.left)*m_XDim/(rc.right-rc.left+1);
	int nY = (point.y - rc.top)*m_YDim/(rc.bottom-rc.top+1);
	if ((nX>=0)&&(nX<m_XDim)&&(nY>=0)&&(nY<m_YDim)) 
	{
		if (IDYES == AfxMessageBox("Change margins? (Y/N)", MB_YESNO|MB_ICONQUESTION))
		{
			m_nMarginX1 = nX;
			m_nMarginY2 = nY;
			bStressInitialized = FALSE;
			UpdateData(FALSE);
		}
	}
}

void CMeasurePhase::OnRButtonUp(UINT nFlags, CPoint point) 
{
//	CView::OnRButtonUp(nFlags, point);
	CRect rc;
	GetDlgItem(IDC_IMG)->GetWindowRect(&rc);
	ScreenToClient(rc);

	int nX = m_XDim - (point.x - rc.left)*m_XDim/(rc.right-rc.left+1);
	int nY = m_YDim - (point.y - rc.top)*m_YDim/(rc.bottom-rc.top+1);
	if ((nX>=0)&&(nX<m_XDim)&&(nY>=0)&&(nY<m_YDim)) 
	{
		if (IDYES == AfxMessageBox("Change margins? (Y/N)", MB_YESNO|MB_ICONQUESTION))
		{
			m_nMarginX2 = nX;
			m_nMarginY1 = nY;
			bStressInitialized = FALSE;
			UpdateData(FALSE);
		}
	}
}

void CMeasurePhase::OnSaveImage()  
{
	// TODO: Add your command handler code here
	HANDLE hf;
	DWORD dwTmp;
	int i, j, Npoints, Nlines;
	int nExtra = 100, nX1 = 10, nX2 = 12, nX3 = 5;

	CString FileName, FileName2;

	if (!m_ppvBits1) return;

	CFileDialog dlg(FALSE, _T("bmp"), _T("*.bmp"),
					 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
					 _T("Bitmap files (*.BMP)|*.BMP|"));
    if (dlg.DoModal() == IDOK) 
            FileName = dlg.GetPathName();
	else
		return;
	
	Npoints = m_XDim;
	Nlines = m_YDim;

	BITMAPFILEHEADER hdr;
	hdr.bfType = 0x4d42;
	hdr.bfSize = 54+3*(m_XDim+nExtra)*m_YDim;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = 54;

//	BITMAPINFOHEADER bmiHeader;
	BITMAPINFO bmi;
	bmi.bmiHeader.biWidth  = m_XDim+nExtra;
	bmi.bmiHeader.biHeight= m_YDim;    
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=24; // Can be 8, 16, 32 bpp or 
                           // other number
	bmi.bmiHeader.biSizeImage=3*(m_XDim+nExtra)*m_YDim;
	bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biClrUsed= 0;
	bmi.bmiHeader.biCompression=0;
	bmi.bmiHeader.biClrImportant= 0;



	BYTE *pBits = (BYTE *)m_ppvBits1;
	
	BYTE *pBits2 = new BYTE [3*(m_XDim+nExtra)*m_YDim];

	for (i=0; i<m_YDim; i++)
	{
		for (j=0;j<3*m_XDim;j++)
		{
			pBits2[3*(m_XDim+nExtra)*i + j] = pBits[3*m_XDim*i + j];
		}
		for (j=0;j<3*nX1;j++)
		{
			pBits2[3*((m_XDim+nExtra)*i+m_XDim)+j] = 255;
		}

		int	iTemp = i * 255 / m_YDim;
		iTemp = min(iTemp, 255);
		iTemp = max(0, iTemp);
		BYTE iData = iTemp;
		for (int j = 0; j < nX2; j++)
		{
			pBits2[3 * ((m_XDim+nExtra) * i + m_XDim + nX1 + j) + 0] = max(0, 2 * iData - 255);
			pBits2[3 * ((m_XDim+nExtra) * i + m_XDim + nX1 + j) + 1] = 255 - 2 * abs(iData - 127);
			pBits2[3 * ((m_XDim+nExtra) * i + m_XDim + nX1 + j) + 2] = max(0, 255 - iData * 2);
		}

		for (j=0;j<3*(nExtra-nX1-nX2);j++)
		{
			pBits2[3*((m_XDim+nExtra)*i+m_XDim+nX1+nX2)+j] = 255;
		}

	}
///
	CDC* dc;
	CDC dcMem;
	CBitmap bmp;

	dc = GetDC();

	dcMem.CreateCompatibleDC(dc);

	RECT rc;
	rc.left = 0;
	rc.right = nExtra-nX1-nX2;
	rc.top = 0;
	rc.bottom = m_YDim;
	bmp.CreateCompatibleBitmap(dc, rc.right, rc.bottom);
	dcMem.SelectObject(&bmp);

	dcMem.FillSolidRect(0, 0, rc.right, rc.bottom, 
								RGB(  255,   255,   255));

	dcMem.SetTextColor(RGB(0,0,0));
	CString strVal;
	strVal.Format("%.3f", m_fImageMax);
	dcMem.TextOut(nX3,0, strVal);
	strVal.Format("%.3f", m_fImageMin + 0.75*(m_fImageMax - m_fImageMin));
	dcMem.TextOut(nX3,rc.bottom/4 - 10, strVal);
	strVal.Format("%.3f", m_fImageMin + 0.5*(m_fImageMax - m_fImageMin));
	dcMem.TextOut(nX3,rc.bottom/2 - 10, strVal);
	strVal.Format("%.3f", m_fImageMin + 0.25*(m_fImageMax - m_fImageMin));
	dcMem.TextOut(nX3,3*rc.bottom/4 - 10, strVal);
	strVal.Format("%.3f", m_fImageMin);
	dcMem.TextOut(nX3,rc.bottom - 20, strVal);

	for (i=0;i<m_YDim;i++)
	{
		for (j=nX3;j<nExtra-(nX1+nX2);j++)
		{
			COLORREF color = dcMem.GetPixel(j, i);
		 	pBits2[3 * ((m_XDim+nExtra) * (m_YDim - 1 - i) + m_XDim + nX1 + nX2 + j)] = color & 0xFF;
		 	pBits2[3 * ((m_XDim+nExtra) * (m_YDim - 1 - i) + m_XDim + nX1 + nX2 + j) + 1] = (color >> 8) & 0xFF;
		 	pBits2[3 * ((m_XDim+nExtra) * (m_YDim - 1 - i) + m_XDim + nX1 + nX2 + j) + 2] = (color >> 16) & 0xFF;
		}
	}
///

	hf = CreateFile( FileName, GENERIC_WRITE, 
		(DWORD) 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
		(HANDLE) NULL); 		
	if (hf != NULL)
	{
		WriteFile(hf, &hdr, sizeof(BITMAPFILEHEADER), &dwTmp, NULL);
		WriteFile(hf, &bmi.bmiHeader, sizeof(BITMAPINFOHEADER), &dwTmp, NULL);	
		WriteFile(hf, pBits2, 3*(m_XDim+nExtra)*m_YDim*sizeof(UCHAR), &dwTmp, NULL);	
		CloseHandle(hf);	
	}

	dcMem.DeleteDC();
	bmp.DeleteObject();
	ReleaseDC(dc);

	dc = NULL;

	if (pBits2)
		delete [] pBits2;

	return;
}

void CMeasurePhase::OnKillfocusMarginX1()
{
	UpdateData(true);	
	bStressInitialized = FALSE;
}

void CMeasurePhase::OnKillfocusMarginY1()
{
	UpdateData(true);	
	bStressInitialized = FALSE;
}

void CMeasurePhase::OnKillfocusMarginX2()
{
	UpdateData(true);	
	bStressInitialized = FALSE;
}

void CMeasurePhase::OnKillfocusMarginY2()
{
	UpdateData(true);	
	bStressInitialized = FALSE;
}

void CMeasurePhase::OnKillfocusDiameter()
{
	UpdateData(true);	
	bStressInitialized = FALSE;
}

void CMeasurePhase::On3DImage()
{
	UpdateData(true);
//	CButton* pButton = (CButton*) GetDlgItem(IDC_UNWRAP_3DIMAGE);
//	m_b3DImage = pButton->GetCheck() && 0x0001;
	SetBitmap(m_nImageType, TRUE);
	Invalidate();
}

void CMeasurePhase::GetTopography(StressPoint* p)
{
	int i, j, iIndex, iIndex2;
	float fScale = m_fDiam/sqrt(m_XDim*m_XDim+m_YDim*m_YDim);
	double pi = atan2(0, -1);

	for(i=0;i<m_YDim;i++)
	{
		for(j=0;j<m_YDim;j++)
		{
			iIndex = m_XDim*i + (j+m_nCenterX-m_nRadius);
			iIndex2 = m_YDim*i + j;
			p[iIndex2].x = fScale*(j-m_YDim/2);
			p[iIndex2].y = fScale*(i-m_YDim/2);
//			if (m_pMask[iIndex])
//				p[iIndex2].z = m_pUnwrapped[iIndex] / (2*pi) * fWavelength/2;
				p[iIndex2].z = fCorrect[iIndex].z;
//			else
//				p[iIndex2].z = -1000;
		}
	}
}

void CMeasurePhase::PutImage(int N, StressPoint* p)
{
	int i, j, iIndex, iIndex2;
	float fScale = 1.0*N/m_YDim;

	memset(fCorrect, 0, m_XDim*m_YDim*sizeof(StressPoint));
	memset(m_pUnwrapped, 0, m_XDim*m_YDim*sizeof(double));
	memset(m_pMask, 0, m_XDim*m_YDim*sizeof(BOOL));
	for(i=0;i<m_YDim;i++)
	{
		for(j=0;j<m_YDim;j++)
		{
			iIndex = ((int) (i*fScale))*N + ((int) (j*fScale));
			iIndex2 = i*m_XDim + j;
			if ((i-m_YDim/2)*(i-m_YDim/2) + (j-m_YDim/2)*(j-m_YDim/2) < m_YDim*m_YDim/4)
			{
				fCorrect[iIndex2].x = p[iIndex].x;
				fCorrect[iIndex2].y = p[iIndex].y;
				fCorrect[iIndex2].z = p[iIndex].z;
				m_pUnwrapped[iIndex2] = p[iIndex].z;
				fCorrect[iIndex2].W = p[iIndex].W;
				if(fCorrect[iIndex2].z > -500)
					m_pMask[iIndex2] = TRUE;
			}
		}
	}
	SetBitmap(m_nImageType, TRUE);
	SetColorBar();
	Invalidate();
}
 

/*//
			for (nCount=0;nCount<4;nCount++)
			{
				for (i=0;i<20;i++)
				{
					AppViews.pS8Dlg->m_pDaq->vSetDACOut(0, 0.5*i);
					Sleep(nPiezoDelay);
					fADC = AppViews.pS8Dlg->m_pDaq->fReadADCSingle(0, 0);
				}
				for (i=20;i>=0;i--)
				{
					AppViews.pS8Dlg->m_pDaq->vSetDACOut(0, 0.5*i);
					Sleep(nPiezoDelay);
					fADC = AppViews.pS8Dlg->m_pDaq->fReadADCSingle(0, 0);
				}
			}
*///
