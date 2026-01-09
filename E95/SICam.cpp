// SICam.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>

#include "CamBase.h"
#include "siusb.h"

#include "413App.h"
#include "Recipe413.h"

#include "..\SRC\DOSUtil.h"

#include "SpecPlot.h"
#include "..\SYS\CUMMsg.h"
#include "GlobalDeviceContainer.h"
#include "..\413\EchoScope\EchoProbeBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSICam

BOOL bInit1 = FALSE;
BOOL bInit2 = FALSE;

BOOL bEchoCfgOK = FALSE;
BOOL bTFCfgOK = FALSE;

CPen CursorRed(PS_SOLID, 1, RGB(255, 0, 0));
CPen CursorBlue(PS_SOLID, 1, RGB(0, 0, 255));
CPen CursorGreen(PS_SOLID, 1, RGB(0, 255, 0));
CPen CursorYellow(PS_SOLID, 1, RGB(255, 255, 0));
CPen CursorMagenta(PS_SOLID, 3, RGB(255, 0, 255));

SIU_Config CSICam::SIConfigEcho;
SIU_Config CSICam::SIConfigTF;
SIU_CaptureContext CSICam::CapCxtEcho;
SIU_CaptureContext CSICam::CapCxtTF;

CString CSICam::SimuDIBFilename = "";
CString CSICam::Reserved = "";

CCamBase* CSICam::pCamTF = NULL;
CCamBase* CSICam::pCamECHO = NULL;

CSICam::CSICam() {
	iCID = CCamBase::ECHOCAM;
	iCNum = 0;
	nGain = 10;
	bRet = TRUE;
	bSnapRet = TRUE;
	pRcp = NULL;
	pWnd = NULL;
	bStop = FALSE;
	bPause = FALSE;
	bShowCrossHair = TRUE;
	bShowCenterTarget = FALSE;
	bChangeExposure = FALSE;
	bSetRedXHair = FALSE;
	bAllowMoveXHair = FALSE;

	bDisplayOverlay = TRUE;
	bDisping = FALSE;
	bClearRsltSpec = FALSE;

	pCapCxt = NULL;
	hLiveThread = NULL;
	pSLineT = pSLineB = NULL;

	bScaled = TRUE;
	SnapRsl = 0;
	SnapLimit = 2000;
	bDisableScaling = FALSE;

	LoadSetting();

	bRun = FALSE;
	bStore = FALSE;
	bSaveDIB = FALSE;
	bStoreComposite = FALSE;

	bAutoExpose = FALSE;
	bDispImage = TRUE;
	iExpose = 800;

	fAve = fStdev = 0;

	dir = 0;
	Level = 90;

	XX = 0;
	SimuDIB.pDIBBits = NULL;

	FileName = "";
	FileBase = "";
	FileDirc = "";

	bSaveVideo = FALSE;
	ScanLineReady = 0;

	bBlt = TRUE;

	pAve = NULL;

	bShow = FALSE;
	bMMove = FALSE;

	bYFlip = 0;
	Index = 1;
	//StoreCount = 0;

	ImgWd = ImgHt = 0;

	SIPoints.StartPt.x = 9999;  // Initial condition [9/8/2011 XPMUser]

	pSwapBuf = NULL;
	pPeakBuf = NULL;

	//	CXOff = CYOff = 0;
	PaOffX = PaOffY = 0;
	fTGtGrnX = fTGtGrnY = 0.5f;
	fTGtBluX = fTGtBluY = 0.5f;

	frame = 0;
	pCam = NULL;

	// [06182024 ZHIMING // 12182024
	bXFlip = FALSE;
	// ]
	ClearTextInfo();
}

CSICam::~CSICam() {
	// Will not deallocate here, will do it in 413App.cpp [4/18/2014 Administrator]
	//if (pCamera)
	//{
	//	delete pCamera;
	//	pCamera = NULL;
	//}
	if (pSwapBuf) {
		delete[] pSwapBuf;
		pSwapBuf = NULL;
	}
	if (pSLineT) {
		delete pSLineT;
		pSLineT = NULL;
	}
	if (pSLineB) {
		delete pSLineB;
		pSLineB = NULL;
	}
}

BEGIN_MESSAGE_MAP(CSICam, CStatic)
	//{{AFX_MSG_MAP(CSICam)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSICam message handlers
void CSICam::LoadSetting() {
	return;

	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);

	iExpose = GetPrivateProfileInt("Video", "Exposure", 1024, inifile);
	nGain = GetPrivateProfileInt("Video", "Gain", 10, inifile);
}

void CSICam::SaveSetting() {
	return;

	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);

	char str[255];
	sprintf(str, "%d", iExpose);
	WritePrivateProfileString("Video", "Exposure", str, inifile);

	sprintf(str, "%d", nGain);
	WritePrivateProfileString("Video", "Gain", str, inifile);
}

DWORD WINAPI CSICam::LiveThreadFunc(LPVOID pParam) {
	CSICam* pCamera = (CSICam*)pParam;
	C413Global* g = &p413App->Global;

	switch (pCamera->iCID) {
	case CCamBase::THINFCAM:
		pCamera->pCam = CSICam::pCamTF;
		switch (pCamera->pCam->CamType) {
		case CCamBase::TYSICAM:
		case CCamBase::TYLUMICAM:
			pCamera->pCapCxt = &CapCxtTF;
			break;
		}
		pCamera->iExpose = g->Exposure[pCamera->iCID];
		pCamera->bYFlip = g->bImageFlipYTFILM;
		pCamera->bXFlip = g->bImageFlipXTFILM; // Added 12182024
		break;
	case CCamBase::ECHOCAM:
		pCamera->pCam = CSICam::pCamECHO;
		switch (pCamera->pCam->CamType) {
		case CCamBase::TYSICAM:
		case CCamBase::TYLUMICAM:
			pCamera->pCapCxt = &CapCxtEcho;
		}
		pCamera->iExpose = g->Exposure[pCamera->iCID];
		pCamera->bYFlip = g->bImageFlipYECHO;
		pCamera->bXFlip = g->bImageFlipXECHO; // Added 12182024
		break;
	default:
		pCamera->pCapCxt = NULL;
		break;
	}

	if (!pCamera || !pCamera->pCam) {
		ASSERT(0);
		return -1;
	}

	if (pCamera->IsSimu()) {
		//pCamera->LoadSimuDIB(9);
		pCamera->LoadSimuDIB(15);
	}

	if (pCamera->pCapCxt) {
		pCamera->AppltSetting();
		pCamera->pCam->SetExposure(pCamera->iCNum, pCamera->iExpose);
		pCamera->pCam->SetAutoMode(pCamera->iCNum, AUTO_WHITE_BALANCE_EXP_CONTROL);
	}

	pCamera->bRun = TRUE;
	pCamera->bVideoOK = FALSE;
	CDC* pDC = pCamera->GetDC();
	while (pCamera->bRun) {
		if (pCamera->bPause) {
			pCamera->bVideoOK = FALSE;
			Sleep(10);
			if (!pCamera->bRun) {
				break;
			}
			continue;
		}
		if (pCamera->bChangeExposure) {
			if (!pCamera->IsSimu()) {
				pCamera->bVideoOK = FALSE;
				if (pCamera->pCam->SetExposure(pCamera->iCNum, pCamera->iExpose)) {
				}
				pCamera->bChangeExposure = FALSE;
				pCamera->bVideoOK = TRUE;
			}
			else {
				// Simulate set exposure delay [8/6/2013 Yuen]
				Sleep(20);
				pCamera->bChangeExposure = FALSE;
			}
			continue;
		}

		pCamera->bVideoOK = FALSE;
		if (pCamera->pCapCxt) {
			if (pCamera->pCam->Capture(pCamera->pCapCxt)) {
				pCamera->bVideoOK = TRUE;
				if (pCamera->bYFlip) pCamera->FlipImageY(&pCamera->pCapCxt->PackedDIB);
				if (pCamera->bXFlip) pCamera->FlipImageX(&pCamera->pCapCxt->PackedDIB); // Added 12182024
				if (pCamera->ShowImage(pDC, &pCamera->pCapCxt->PackedDIB)) {
					pCamera->frame++;
				}
			}
			else {
				TRACE("SICamCapFAIL\n");
			}
		}
		else {
			Sleep(100);
			pCamera->bVideoOK = TRUE;
			if (pCamera->ShowImage(pDC, &pCamera->SimuDIB)) {
				pCamera->frame++;
			}
		}
		if (pCamera->frame > 300000) {
			pCamera->frame = 0;
		}
		Sleep(80);
	}
	pCamera->ReleaseDC(pDC);

	pCamera->bStop = TRUE;

	if (pCamera->SimuDIB.pDIBBits) {
		delete[] pCamera->SimuDIB.pDIBBits;
		pCamera->SimuDIB.pDIBBits = NULL;
	}
	return 0;
}

BOOL CSICam::ShowImage(CDC* pDC, SIPackedDIB* pPackedDIB) {
	if (!pPackedDIB) {
		return FALSE;
	}

	CRect rc, rc1;
	C413Global* g = &p413App->Global;
	//	BITMAPINFOHEADER & bmi = pPackedDIB->BmpInfo;

	// 	if (bFocus)
	// 	{
	// 		ProcessFocus(pPackedDIB);
	// 	}
	if (pPackedDIB->pDIBBits) {
		ImgWd = pPackedDIB->BmpInfo.biWidth;
		ImgHt = abs(pPackedDIB->BmpInfo.biHeight);
	}
	else {
		GetClientRect(&rc);
		ImgWd = rc.Width();
		ImgHt = rc.Height();
	}

	if (bAutoExpose) {
		ProcessExposure(pPackedDIB);
	}
	//ProcessStoreDIB(pPackedDIB); // bStore will be set to false in the called function
	if (bStore) {
		//if (frame > StoreCount+2)
		//{
			// wait 3 frames [8/11/2013 FSM413]
			//bSnapRet = TRUE;
		bSnapRet = ProcessStoreDIB(pPackedDIB);	// Make a copy of video image [5/22/2014 FSMT]
		bStore = FALSE;
		//	StoreCount = 0;
		//}
	}

	if (bSaveDIB) {
		ProcessSaveDIB(pPackedDIB);	// Save video to disk with no overlay [5/22/2014 FSMT]
		bSaveDIB = FALSE;
	}

	if (bDispImage) {
		CDC dcMem;
		CBitmap bmp;
		HBITMAP oldBmp;

		GetClientRect(&rc);
		GetClientRect(&rc1);

		dcMem.CreateCompatibleDC(pDC);
		if (bStoreComposite || bSaveVideo) {
			// Make bigger image [5/24/2014 FSMT]
			rc.SetRect(0, 0, ImgWd, ImgHt);
		}
		bmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		oldBmp = (HBITMAP)dcMem.SelectObject(bmp);

		CRgn Rg;

		HDC hDC = dcMem.GetSafeHdc();
		::SetStretchBltMode(hDC, COLORONCOLOR);
		if (pPackedDIB->pDIBBits) {
			::StretchDIBits(dcMem.GetSafeHdc(),
				0, 0, rc.Width(), rc.Height(),
				0, 0, ImgWd, ImgHt,
				pPackedDIB->pDIBBits,
				(BITMAPINFO*)(&pPackedDIB->BmpInfo),
				DIB_RGB_COLORS, SRCCOPY);
		}

		if (bDisplayOverlay) {
			bDisping = TRUE;
			DisplayOvelays(dcMem, rc);
			bDisping = FALSE;
		}

		if (bBlt) {
			if (!(bStoreComposite || bSaveVideo)) {
				if (!bDisableScaling && bScaled) {
					pDC->StretchBlt(0, 0, rc1.Width(), rc1.Height(), &dcMem, 0, 0, rc.Width(), rc.Height(), SRCCOPY);
				}
				else {
					int sx = (rc1.Width() - rc.Width()) / 2;
					int sy = (rc1.Height() - rc.Height()) / 2;
					pDC->BitBlt(sx, sy, rc.Width(), rc.Height(), &dcMem, 0, 0, SRCCOPY);
				}
			}
		}
		if (bStoreComposite || bSaveVideo) {
			// Save the image in window with overlays included [5/21/2014 FSMT]
			StoreComposite(bmp, ScanLineReady);
			if (bSaveVideo) {
				StorePeakInfo();
			}
			bStoreComposite = FALSE;
			bSaveVideo = FALSE;
			if (ScanLineReady) {
				ScanLineReady = 0;
			}
		}

		dcMem.SelectObject(oldBmp);
		ReleaseDC(&dcMem);

		// bShow act as a synchronization object [8/30/2011 C2C]
		if (bShow) {
			bShow = FALSE;
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// examples [3/25/2010 Valued Customer]

void CSICam::AppltSetting() {
	if (pCam) {
		pCam->SetExposure(iCNum, iExpose);
		pCam->SetGlobalGain(iCNum, nGain);
	}

	SaveSetting();
}

void CSICam::ProcessExposure(SIPackedDIB* pPackedDIB) {
	if (pCam->IsSimu()) {
		Sleep(1200);	// Simulate delays [8/6/2013 Yuen]
		bAutoExpose = FALSE;
		return;
	}
	if (!pPackedDIB->pDIBBits) {
		return;
	}
	C413Global* g = &p413App->Global;
	int max = GetCount(pPackedDIB);

	if (dir == 0) {
		if (max < Level) {
			dir = 1;
		}
		else if (max > Level) {
			dir = -1;
		}
		else {
			bAutoExpose = FALSE;
			return;
		}
	}
	if (dir == 1) {
		if (max > Level) {
			bAutoExpose = FALSE;
			return;
		}
		switch (iCID) {
		default:
			iExpose += g->AutoExposeStepEcho;
			break;
		case CCamBase::THINFCAM:
			iExpose += g->AutoExposeStepTF;
			break;
		}
		if (iExpose < 40000) {
			if (pCam) {
				pCam->SetExposure(iCNum, iExpose);
			}
		}
		else {
			bAutoExpose = FALSE;
			g->Exposure[iCID] = iExpose;
			return;
		}
	}
	else if (dir == -1) {
		if (max < Level) {
			bAutoExpose = FALSE;
			return;
		}
		switch (iCID) {
		default:
			iExpose -= g->AutoExposeStepEcho;
			break;
		case CCamBase::THINFCAM:
			iExpose -= g->AutoExposeStepTF;
			break;
		}
		if (iExpose > 0) {
			if (pCam) {
				pCam->SetExposure(iCNum, iExpose);
			}
		}
		else {
			bAutoExpose = FALSE;
			g->Exposure[iCID] = iExpose;
			return;
		}
	}
	else {
		bAutoExpose = FALSE;
		return;
	}
}

BOOL CSICam::ProcessStoreDIB(SIPackedDIB* pPackedDIB) {
	if (!pPackedDIB || !pPackedDIB->pDIBBits) {
		// 		bStore = FALSE;
		return FALSE;
	}
	C413Global* g = &p413App->Global;
	short BPP = pPackedDIB->BmpInfo.biBitCount / 8;
	long BytesPerRow = pPackedDIB->BmpInfo.biWidth * BPP;
	BytesPerRow += (4 - BytesPerRow % 4) % 4;	// int alignment
	DWORD SizeImage = BytesPerRow * abs(pPackedDIB->BmpInfo.biHeight);

	CSingleLock Lock(&g->StoredDIB.CS);
	Lock.Lock();
	SIPackedDIB* pStore = g->StoredDIB.pDIB;
	if (!pStore || !pStore->pDIBBits ||
		(pStore->BmpInfo.biHeight != pPackedDIB->BmpInfo.biHeight) ||
		(pStore->BmpInfo.biWidth != pPackedDIB->BmpInfo.biWidth) ||
		(pStore->BmpInfo.biBitCount != pPackedDIB->BmpInfo.biBitCount)) {
		g->StoredDIB.Create(pPackedDIB->BmpInfo.biWidth, pPackedDIB->BmpInfo.biHeight, pPackedDIB->BmpInfo.biBitCount);
		pStore = g->StoredDIB.pDIB;
	}
	if (pStore) {
		if (pStore->pDIBBits) {
			if (pStore->BmpInfo.biSizeImage != SizeImage) {
				delete[] pStore->pDIBBits;
				pStore->pDIBBits = new BYTE[SizeImage];
			}
		}
		else {
			pStore->pDIBBits = new BYTE[SizeImage];
		}
		// Set header info [9/18/2011 Administrator]
		pStore->BmpInfo.biBitCount = pPackedDIB->BmpInfo.biBitCount;
		pStore->BmpInfo.biHeight = pPackedDIB->BmpInfo.biHeight;
		pStore->BmpInfo.biWidth = pPackedDIB->BmpInfo.biWidth;
		pStore->BmpInfo.biPlanes = 1;
		pStore->BmpInfo.biCompression = 0;
		pStore->BmpInfo.biSize = sizeof(BITMAPINFOHEADER);
		pStore->BmpInfo.biXPelsPerMeter = pStore->BmpInfo.biYPelsPerMeter = 0;
		pStore->BmpInfo.biClrUsed = pStore->BmpInfo.biClrImportant = 0;
		SizeImage = BytesPerRow * abs(pPackedDIB->BmpInfo.biHeight);
		pStore->BmpInfo.biSizeImage = SizeImage;
		if (pStore->pDIBBits) {
			memcpy(g->StoredDIB.pDIB->pDIBBits, pPackedDIB->pDIBBits, SizeImage);
			g->StoredDIB.CalcBytePerRow();
			Lock.Unlock();
			bStore = FALSE;
			return TRUE;
		}
	}
	Lock.Unlock();
	bStore = FALSE;
	return FALSE;
}

BOOL CSICam::Initialize(CListBox* pMessage, CCamBase* pCamera, CCamBase::CAMERAID ID) {
	if (!pCamera) {
		ASSERT(0);
		return FALSE;
	}
	C413Global* g = &p413App->Global;

	switch (pCamera->CamType) {
	case CCamBase::TYSICAM:
	case CCamBase::TYLUMICAM:
	{
		CString mdir;
		DosUtil.GetProgramDir(mdir);
		CString m_strConfigFile;
		switch (ID) {
		case CCamBase::THINFCAM:
			m_strConfigFile.Format("%s%s", mdir.GetBuffer(), "SI3300C2.cfg", CCamBase::THINFCAM);
			if (pCamera->ReadConfig(m_strConfigFile, &SIConfigTF, CCamBase::THINFCAM, g->CamraNum[CCamBase::THINFCAM])) {
				bTFCfgOK = TRUE;
			}
			break;
		default:
			m_strConfigFile.Format("%s%s", mdir.GetBuffer(), "SI3300C1.cfg", CCamBase::ECHOCAM);
			if (pCamera->ReadConfig(m_strConfigFile, &SIConfigEcho, CCamBase::ECHOCAM, g->CamraNum[CCamBase::ECHOCAM])) {
				bEchoCfgOK = TRUE;
			}
			break;
		}
	}
	break;
	default:
		// No need to do anything [6/1/2014 Yuen]
		bTFCfgOK = TRUE;
		bEchoCfgOK = TRUE;
		break;
	}

	BOOL bStarted1 = FALSE;
	BOOL bStarted2 = FALSE;
	int CameraStarted = 0;

	CString str;
	CString CamTypeStr[] = { "Simu", "SI", "Lumi" };

	// SiCam need to be started lower number first [6/4/2014 Yuen]
	if (g->CamraNum[CCamBase::ECHOCAM] < g->CamraNum[CCamBase::THINFCAM]) {
		if (p413App->Global.Confg.bEchoMod && (pCamera->ID == CCamBase::ECHOCAM)) {
			if (StartEchoCamera()) {
				str.Format("Echo camera (%s) started OK", CamTypeStr[pCamera->CamType].GetBuffer());
				pMessage->InsertString(pMessage->GetCount(), str);
				CameraStarted++;
				bStarted1 = TRUE;
			}
			else {
				str.Format("Echo camera (%s) started FAILED", CamTypeStr[pCamera->CamType].GetBuffer());
				pMessage->InsertString(pMessage->GetCount(), str);
			}
		}
		if (p413App->Global.Confg.bThinFilmMod && (pCamera->ID == CCamBase::THINFCAM)) {
			if (StartThinFilmCamera()) {
				str.Format("Thin Film camera (%s) started OK", CamTypeStr[pCamera->CamType].GetBuffer());
				pMessage->InsertString(pMessage->GetCount(), str);
				CameraStarted++;
				bStarted2 = TRUE;
			}
			else {
				str.Format("Thin Film camera (%s) started FAILED", CamTypeStr[pCamera->CamType].GetBuffer());
				pMessage->InsertString(pMessage->GetCount(), str);
			}
		}
	}
	else {
		if (p413App->Global.Confg.bThinFilmMod && (pCamera->ID == CCamBase::THINFCAM)) {
			if (StartThinFilmCamera()) {
				str.Format("Thin Film camera (%s) started OK", CamTypeStr[pCamera->CamType].GetBuffer());
				pMessage->InsertString(pMessage->GetCount(), str);
				CameraStarted++;
				bStarted2 = TRUE;
			}
			else {
				str.Format("Thin Film camera (%s) started FAILED", CamTypeStr[pCamera->CamType].GetBuffer());
				pMessage->InsertString(pMessage->GetCount(), str);
			}
		}

		if (p413App->Global.Confg.bEchoMod && (pCamera->ID == CCamBase::ECHOCAM)) {
			if (StartEchoCamera()) {
				str.Format("Echo camera (%s) started OK", CamTypeStr[pCamera->CamType].GetBuffer());
				pMessage->InsertString(pMessage->GetCount(), str);
				CameraStarted++;
				bStarted1 = TRUE;
			}
			else {
				str.Format("Echo camera (%s) started FAILED", CamTypeStr[pCamera->CamType].GetBuffer());
				pMessage->InsertString(pMessage->GetCount(), str);
			}
		}
	}
	return TRUE;
}

BOOL CSICam::StartEchoCamera() {
	if (!bInit1 && bEchoCfgOK && pCamECHO) {
		C413Global* g = &p413App->Global;
		if (pCamECHO->Open(g->CamraNum[CCamBase::ECHOCAM], &SIConfigEcho)) {
			CapCxtEcho.iCameraID = g->CamraNum[CCamBase::ECHOCAM];
			SIConfigEcho.CameraConfig.iExposure = g->Exposure[CCamBase::ECHOCAM];
			SIConfigEcho.CameraConfig.iGlobalGain = 1;
			for (int i = 0; i < 3; i++) {
				if (pCamECHO->BeginCapture(&SIConfigEcho, &CapCxtEcho, CCamBase::ECHOCAM)) {
					bInit1 = TRUE;
					return TRUE;
				}
				pCamECHO->EndCapture(&CapCxtEcho, CCamBase::ECHOCAM);
				Sleep(1000);	// Retry 1 sec later [6/2/2014 FSMT]
			}
		}
	}
	return bInit1;
}

BOOL CSICam::StartThinFilmCamera() {
	if (!bInit2 && bTFCfgOK && pCamTF) {
		C413Global* g = &p413App->Global;
		if (pCamTF->Open(g->CamraNum[CCamBase::THINFCAM], &SIConfigTF)) {
			CapCxtTF.iCameraID = g->CamraNum[CCamBase::THINFCAM];
			SIConfigTF.CameraConfig.iExposure = g->Exposure[CCamBase::THINFCAM];
			SIConfigTF.CameraConfig.iGlobalGain = 1;
			for (int i = 0; i < 3; i++) {
				if (pCamTF->BeginCapture(&SIConfigTF, &CapCxtTF, CCamBase::THINFCAM)) {
					bInit2 = TRUE;
					return TRUE;
				}
				pCamTF->EndCapture(&CapCxtTF, CCamBase::THINFCAM);
				Sleep(1000);	// Retry 1 sec later [6/2/2014 FSMT]
			}
		}
	}
	return bInit2;
}

BOOL CSICam::Deinitialize(CCamBase* pCamera) {
	if (!pCamera) {
		return FALSE;
	}
	if (bInit1 && pCamera->ID == CCamBase::ECHOCAM) {
		pCamera->EndCapture(&CapCxtEcho, CCamBase::ECHOCAM);
		pCamera->Close(p413App->Global.CamraNum[CCamBase::ECHOCAM]);
		bInit1 = FALSE;
	}
	if (bInit2 && pCamera->ID == CCamBase::THINFCAM) {
		pCamera->EndCapture(&CapCxtTF, CCamBase::THINFCAM);
		pCamera->Close(p413App->Global.CamraNum[CCamBase::THINFCAM]);
		bInit2 = FALSE;
	}
	return TRUE;
}

void CSICam::AdjustExposure() {
	AppltSetting();
}

void CSICam::DisplayOvelays(CDC& dcMem, CRect& rc) {
	CSize szCursor;
	szCursor.cx = 50;
	szCursor.cy = 38;

	ShowCenterTarget(dcMem, rc);
	ShowGreenTarget(dcMem, rc);
	ShowBlueTarget(dcMem, rc);
	ShowRedTarget(dcMem, rc);

	if (bMMove) {
		if (::GetAsyncKeyState(VK_CONTROL) & 0x8000) {
			CPen* pOldPen = dcMem.SelectObject(&CursorMagenta);
			dcMem.MoveTo(SIPoints.StartPt.x, SIPoints.StartPt.y);
			dcMem.LineTo(SIPoints.StartPt.x, SIPoints.EndPt.y);
			dcMem.LineTo(SIPoints.EndPt.x, SIPoints.EndPt.y);
			dcMem.LineTo(SIPoints.EndPt.x, SIPoints.StartPt.y);
			dcMem.LineTo(SIPoints.StartPt.x, SIPoints.StartPt.y);
			if (pOldPen) dcMem.SelectObject(pOldPen);
		}
		else {
			CPen* pOldPen = dcMem.SelectObject(&CursorMagenta);
			dcMem.MoveTo(SIPoints.StartPt.x, SIPoints.StartPt.y);
			dcMem.LineTo(SIPoints.EndPt.x, SIPoints.EndPt.y);
			if (pOldPen) dcMem.SelectObject(pOldPen);
		}
	}

	if (bClearRsltSpec) {
		CSingleLock Lock(&RsltSpec.CS);
		Lock.Lock();
		RsltSpec.Clear();
		Lock.Unlock();
		bClearRsltSpec = FALSE;
	}
	switch (iCID) {
	case CCamBase::ECHOCAM:
		ShowEchoView(&dcMem, rc);
		break;
	case CCamBase::THINFCAM:
		ShowTFView(&dcMem, rc);
		break;
	}
}

int CSICam::AutoAdjust() {
	dir = 0;
	bAutoExpose = TRUE;
	C413Global* g = &p413App->Global;
	while (bAutoExpose && !g->bAborting) {
		Sleep(100);
	}
	g->Exposure[iCID] = iExpose;
	return iExpose;
}

static BYTE SDIBCount = 1;
void CSICam::LoadSimuDIB(CString mdir) {
	if (!IsSimu()) {
		return;
	}
	if (mdir.GetLength() < 1) {
		return;
	}

	FILE* fp = fopen(mdir, "rb");
	if (fp) {
		BITMAPFILEHEADER bmfh;

		fread(&bmfh, 1, sizeof(BITMAPFILEHEADER), fp);
		fread(&SimuDIB.BmpInfo, 1, sizeof(BITMAPINFOHEADER), fp);

		// Check file type signature
		if (bmfh.bfType != 'MB') return;		// File is not BMP

		// Assign some short variables:
		WORD BPP = SimuDIB.BmpInfo.biBitCount / 8;
		long Width = SimuDIB.BmpInfo.biWidth;
		long Height = abs(SimuDIB.BmpInfo.biHeight); // absolute value
		long BytesPerRow = Width * BPP;
		BytesPerRow += (4 - BytesPerRow % 4) % 4;	// int alignment

		if (BPP >= 3) {
			fseek(fp, bmfh.bfOffBits, SEEK_SET);
			if (SimuDIB.pDIBBits) {
				delete[] SimuDIB.pDIBBits;
				SimuDIB.pDIBBits = NULL;
			}
			SimuDIB.pDIBBits = new BYTE[BytesPerRow * Height];
			if (SimuDIB.BmpInfo.biHeight > 0) {
				for (int n = Height - 1; n >= 0; n--) {
					fread(SimuDIB.pDIBBits + BytesPerRow * n, 1, BytesPerRow, fp);
				}
				SimuDIB.BmpInfo.biHeight = -Height;
			}
			else {
				fread(SimuDIB.pDIBBits, 1, BytesPerRow * Height, fp);
			}
		}
		// so, we always have a up-bottom raster (that is negative height for windows):
		fclose(fp);
		SimuDIB.ColorTable[0].rgbBlue = SDIBCount++;
	}
}

void CSICam::LoadSimuDIB(short n) {
	if (SimuDIBFilename.GetLength() > 0) {
		LoadSimuDIB(SimuDIBFilename);
		return;
	}
	CString mdir;
	DosUtil.GetProgramDir(mdir);

	CString img;
	img.Format("IM%02d.BMP", n);
	mdir += img;

	LoadSimuDIB(mdir);
}

short CSICam::GetExposure() {
	// 	CSingleLock Lock(&CSXpose);
	// 	Lock.Lock();
	return iExpose;
}

void CSICam::SetAve(float* pA) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	pAve = pA;
	Lock.Unlock();
}

void CSICam::SetExposure(short exposure, BOOL bWait) {
	// Do not track Global.Exposure[] [8/21/2010 C2C]
	if (exposure < 1) {
		// zero exposure will hang the camera [6/16/2012 FSM 413 C2C]
		exposure = 1;
	}
	// 	CSingleLock Lock(&CSXpose);
	// 	Lock.Lock();
	iExpose = exposure;
	// 	Lock.Unlock();
	ChangeExposure(bWait);
}

void CSICam::FlipImageY(SIPackedDIB* pSIDIB) {
	if (!pSIDIB || !pSIDIB->pDIBBits) {
		return;
	}
	long BytesPerRow = pSIDIB->BmpInfo.biWidth * (pSIDIB->BmpInfo.biBitCount / 8);
	BytesPerRow += (4 - BytesPerRow % 4) % 4;
	if (!pSwapBuf) {
		// Allocate buffer for row swap [9/13/2011 Administrator]
		pSwapBuf = new BYTE[BytesPerRow * 4];
		if (!pSwapBuf) {
			ASSERT(0);
			return;
		}
	}
	int h = abs(pSIDIB->BmpInfo.biHeight);
	int yy = h - 1;  // last row [9/13/2011 Administrator]
	for (int y = 0; y < h / 2; y++) {
		BYTE* p = pSIDIB->pDIBBits + y * BytesPerRow;
		BYTE* q = pSIDIB->pDIBBits + yy * BytesPerRow;
		memcpy(pSwapBuf, p, BytesPerRow);
		memcpy(p, q, BytesPerRow);
		memcpy(q, pSwapBuf, BytesPerRow);
		yy--;
	}
}

void CSICam::FlipImageX(SIPackedDIB* pSIDIB) { // Added 12182024
	if (!pSIDIB || !pSIDIB->pDIBBits) {
		return;
	}

	long BytesPerPixel = pSIDIB->BmpInfo.biBitCount / 8;
	long BytesPerRow = pSIDIB->BmpInfo.biWidth * BytesPerPixel;
	BytesPerRow += (4 - BytesPerRow % 4) % 4;  // Align to 4-byte boundary

	if (!pSwapBuf) {
		// Allocate buffer for pixel swap [9/13/2011 Administrator]
		pSwapBuf = new BYTE[BytesPerPixel * 4];
		if (!pSwapBuf) {
			ASSERT(0);
			return;
		}
	}

	int w = pSIDIB->BmpInfo.biWidth;
	int halfWidth = w / 2;  // process only half the width for mirroring

	for (int y = 0; y < abs(pSIDIB->BmpInfo.biHeight); y++) {
		BYTE* pRow = pSIDIB->pDIBBits + y * BytesPerRow;
		for (int x = 0; x < halfWidth; x++) {
			BYTE* pPixel = pRow + x * BytesPerPixel;
			BYTE* qPixel = pRow + (w - x - 1) * BytesPerPixel;
			// Swap the pixels
			memcpy(pSwapBuf, pPixel, BytesPerPixel);
			memcpy(pPixel, qPixel, BytesPerPixel);
			memcpy(qPixel, pSwapBuf, BytesPerPixel);
		}
	}
}
void CSICam::ShowEchoView(CDC* pDC, CRect& rc) {
	if (!pDC) {
		return;
	}
	int i;

	if ((ScanLineReady || bShow) && pSLineT && pSLineB) {
		CPen pen2, pen1;
		HPEN oldPen;
		pen1.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
		pen2.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));

		short mode = 1;
		int i, div = 8, base1, base2, amp = 2;
		float sf;
		float sfx;

		// Note: rc.left assumed to be zero [5/7/2011 FSMT]
		// Top probe [5/4/2011 XPMUser]
		oldPen = (HPEN)pDC->SelectObject(pen2);
		base1 = rc.top + rc.Height() / div;
		base2 = base1 + 3;
		int height = rc.Height() / div;
		sf = (rc.Height() / div) / 2048.0f;
		CScanPoint* p;
		CSingleLock Lock(&CSSLine);
		if (Lock.Lock()) {
			if (mode) {
				int op = -1;
				int dif = (pSLineT->MaxMinX.iMax - pSLineT->MaxMinX.iMin) * 0.1f;
				int max = pSLineT->MaxMinX.iMax + dif;
				int min = pSLineT->MaxMinX.iMin - dif;
				p = pSLineT->pData;
				sfx = (rc.right - rc.left) / float(max - min);
				for (i = 0; i < pSLineT->Idx; i++) {
					int np = sfx * ((p + i)->x - min);
					if (np == op) {
						continue;
					}
					op = np;
					int x = rc.left + np;
					pDC->MoveTo(x, base1);
					int v = sf * ((p + i)->y1 - 2047) * amp;
					if (v > height) {
						v = height;
					}
					pDC->LineTo(x, base1 - v);
				}
			}
			else {
				sfx = (pSLineT->Idx - 1) / float(rc.right - rc.left);
				pDC->MoveTo(rc.left, base1 - sf * (pSLineT->pData[0].y1 - 2047));
				for (i = 0; i < (rc.right - rc.left); i++) {
					int x = int(i * sfx);
					pDC->LineTo(rc.left + i, base1 - sf * (pSLineT->pData[x].y1 - 2047));
				}
			}

			// Bottom probe [5/4/2011 XPMUser]
			pDC->SelectObject(pen1);
			pSLineB->CalcMaxMin();
			sf = (rc.Height() / div) / float(pSLineB->MaxMinY1.iMax - pSLineB->MaxMinY1.iMin);
			if (mode) {
				int op = -1;
				int dif = (pSLineB->MaxMinX.iMax - pSLineB->MaxMinX.iMin) * 0.1f;;
				int max = pSLineB->MaxMinX.iMax + dif;
				int min = pSLineB->MaxMinX.iMin - dif;
				p = pSLineB->pData;
				sfx = (rc.right - rc.left) / float(max - min);
				for (i = 0; i < pSLineB->Idx; i++) {
					int np = sfx * ((p + i)->x - min);
					if (np == op) {
						continue;
					}
					op = np;
					int x = rc.left + np;
					pDC->MoveTo(x, base2);
					int v = sf * ((p + i)->y1 - 2047) * amp;
					if (v > height) {
						v = height;
					}
					pDC->LineTo(x, base2 + v);
				}
			}
			else {
				base2 = rc.bottom;
				sfx = (pSLineB->Idx - 1) / float(rc.right - rc.left);
				pDC->MoveTo(rc.left, base2 - sf * (pSLineB->pData[0].y1 - 2047));
				for (i = 0; i < (rc.right - rc.left); i++) {
					int x = int(i * sfx);
					pDC->LineTo(rc.left + i, base2 - sf * (pSLineB->pData[x].y1 - 2047));
				}
			}
		}
		Lock.Unlock();

		if (oldPen) pDC->SelectObject(oldPen);
	}
	C413Global* g = &p413App->Global;

	// Draw scale [7/16/2013 Yuen]
	//float Xa = 0.10f; // unit in mm [5/24/2014 FSMT]
	float sfx = g->ScrnCalibECH.ScrnX / float(rc.Width());
	short wd = g->ScaleEcho * g->ScrnCalibECH.ppmX / sfx * 1e3;
	short dw = (rc.Width() - wd) / 2;
	CRect rcScale;
	rcScale.SetRect(dw, rc.bottom - 4, wd + dw, rc.bottom - 12);
	pDC->Rectangle(rcScale);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 128, 0));
	pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);

	CString str, str1;

	int Y = rc.bottom - 2;

	str.Format("%.3f mm", g->ScaleEcho);
	pDC->TextOut(rc.left, Y, str);

	Y -= 16;
	str.Format("X:%.2f Y:%.2f EchoTopZ:%.2f [%s] %d", g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(), frame);
	pDC->TextOut(rc.left, Y, str);

	str.Format("%d pixels", diameter);
	pDC->TextOut(textout.x, textout.y, str);

	CSingleLock Lock(&CS);
	Lock.Lock();
	if (pAve && pRcp) {
		for (i = 0; i < MaXPA; i++) {
			MParam* pa = &pRcp->MeParam[0].MPa[i];	// @@@XXX [6/30/2013 Yuen]
			if (pa->Name.GetLength()) {
				Y -= 18;
				str.Format("%s %.1f um", pa->Name.GetBuffer(), pAve[i]);
				pDC->TextOut(rc.left, Y, str);
			}
		}
	}
	Lock.Unlock();
}

void CSICam::ShowTFView(CDC* pDC, CRect& rc) {
	if (!pDC) {
		return;
	}
	CString str;
	C413Global* g = &p413App->Global;
	if (!RsltSpec.pPt) {
		CRect rcScale;
		//float Xa = 0.1f;
		float sfx = g->ScrnCalibTF.ScrnX / float(rc.Width());
		short wd = g->ScaleTF * g->ScrnCalibTF.ppmX / sfx;
		short dw = (rc.Width() - wd) / 2;
		rcScale.SetRect(dw, rc.bottom - 4, dw + wd, rc.bottom - 12);
		pDC->Rectangle(rcScale);

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 234, 0));
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
		// show motor's coordinates [10/27/2011 Administrator]
		int Y = rc.bottom - 2;
		str.Format("%.3f mm", g->ScaleTF);
		pDC->TextOut(rc.left, Y, str);

		Y -= 16;
		str.Format("XYZ(%.3f, %.3f, %.3f), E:%d [%s] %d", g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, iExpose, CTime::GetCurrentTime().Format("%H:%M:%S").GetBuffer(), frame);
		pDC->TextOut(rc.left, Y, str);

		// 		Y -= 16;
		// 		str.Format("CXY(%.3f, %.3f)  PXY(%.3f, %.3f)", CXOff, CYOff, PaOffX, PaOffY);
		// 		pDC->TextOut(rc.left,Y,str);

		return;
	}

	CSingleLock Lock(&RsltSpec.CS);
	if (Lock.Lock()) {
		COceanBase* p = p413App->pOceanOp;
		if (RsltSpec.Poly[0].fThk > -1) {
			float Max = RsltSpec.Cmax;
			float Min = RsltSpec.Cmin;
			float sf = rc.Height() / (Max - Min) / 3;
			float sfx = float(RsltSpec.N) / float(rc.Width());

			// Draw raw spectrum [7/25/2010 C2C]
			CPen pen2;
			HPEN oldPen;
			pen2.CreatePen(PS_SOLID, 2, RGB(0, 64, 255));
			oldPen = (HPEN)pDC->SelectObject(pen2);

			int mid = rc.Height() / 3;
			CSpecPoint* pSp = RsltSpec.pPt;
			pDC->MoveTo(rc.left, mid - sf * (pSp->Y1 - Min));
			for (int i = 0; i < rc.Width(); i++) {
				pDC->LineTo(rc.left + i, mid - sf * ((pSp + int(sfx * i))->Y1 - Min));
			}
			if (oldPen) pDC->SelectObject(oldPen);

			DrawHisto(pDC, rc);
		}

		CRect rcScale;
		//float Xa = 0.1f;
		float sfx = g->ScrnCalibTF.ScrnX / float(rc.Width());
		short wd = g->ScaleTF * g->ScrnCalibTF.ppmX / sfx;
		short dw = (rc.Width() - wd) / 2;
		rcScale.SetRect(dw, rc.bottom - 4, dw + wd, rc.bottom - 12);
		pDC->Rectangle(rcScale);

		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 234, 0));

		int Y = rc.bottom - 2;
		pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);

		str.Format("%.3f mm", g->ScaleTF);
		pDC->TextOut(rc.left, Y, str);

		if (RsltSpec.pPt) {
			if (RsltSpec.pPt[0].X1 < 1) {
				// wavenumber [1/6/2012 Administrator]
				str.Format("%.1f,%.1f|%d,%d|%.2f,%.2f",
					1 / RsltSpec.pPt[0].X1, 1 / RsltSpec.pPt[RsltSpec.N - 1].X1,
					p->nMeastime,
					p->nMethod, RsltSpec.Poly[0].fMin, RsltSpec.Poly[0].fMax);
			}
			else {
				// wavelength [1/6/2012 Administrator]
				str.Format("%.1f,%.1f|%d,%d|%.2f,%.2f",
					RsltSpec.pPt[0].X1, RsltSpec.pPt[RsltSpec.N - 1].X1,
					p->nMeastime,
					p->nMethod, RsltSpec.Poly[0].fMin, RsltSpec.Poly[0].fMax);
			}
			Y -= 16;
			pDC->TextOut(rc.left, Y, str);
		}

		// show motor's coordinates [10/27/2011 Administrator]
		str.Format("XYZ(%.3f, %.3f, %.3f), E:%d (%.3f,%.3f) %d", g->LocXY.X, g->LocXY.Y, g->LocEchoTopZ, iExpose, RsltSpec.Cmin, RsltSpec.Cmax, frame);
		Y -= 18;
		pDC->TextOut(rc.left, Y, str);

		str.Format("#%d, Thk: %.4fum,  RI: %.4f  %s", Index, RsltSpec.Poly[0].fThk, RsltSpec.Poly[0].fRI, CTime::GetCurrentTime().Format("%Y/%m/%d %H:%M:%S").GetBuffer());
		Y -= 18;
		pDC->TextOut(rc.left, Y, str);
	}
	Lock.Unlock();
}

#define DS_BITMAP_FILEMARKER  ((WORD) ('M' << 8) | 'B')

// bmp is DDB [8/10/2011 XPMUser]
void CSICam::StoreComposite(CBitmap& bmp, int cnt) {
	FileName = FileDirc + FileBase;

	CString path = DosUtil.DirSys.ExtractFilePath(FileName);
	DosUtil.DirSys.GotoDirec(path);

	CFile file;
	if (!file.Open(FileName, CFile::modeWrite | CFile::modeCreate)) {
		return;
	}
	BITMAP bitmapInfo;
	bmp.GetBitmap(&bitmapInfo);

	long BytesPerRow = bitmapInfo.bmWidth * (bitmapInfo.bmBitsPixel / 8);
	BytesPerRow += (4 - BytesPerRow % 4) % 4;

	BITMAPINFOHEADER BitmapHead;
	ZeroMemory(&BitmapHead, sizeof(BITMAPINFOHEADER));
	BitmapHead.biWidth = bitmapInfo.bmWidth;
	BitmapHead.biHeight = -bitmapInfo.bmHeight;
	BitmapHead.biPlanes = 1;
	BitmapHead.biBitCount = bitmapInfo.bmBitsPixel;
	BitmapHead.biSizeImage = BytesPerRow * abs(bitmapInfo.bmHeight);
	BitmapHead.biSize = sizeof(BITMAPINFOHEADER);
	BitmapHead.biClrUsed = 0;
	BitmapHead.biClrImportant = 0;
	BitmapHead.biCompression = BI_RGB;

	// assumes 256 gray level
	BITMAPFILEHEADER   hdr;
	DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(hdr) + 256 * sizeof(RGBQUAD);
	RGBQUAD* pRGB = new RGBQUAD[256];
	for (int i = 0; i < 256; i++) {
		pRGB[i].rgbRed = i;
		pRGB[i].rgbGreen = i;
		pRGB[i].rgbBlue = i;
	}
	// Fill in the fields of the file header
	hdr.bfType = DS_BITMAP_FILEMARKER;
	hdr.bfSize = dwFileHeaderSize + BitmapHead.biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = dwFileHeaderSize;

	// Write the file header
	file.Write(&hdr, sizeof(BITMAPFILEHEADER));
	// Write the DIB header
	file.Write(&BitmapHead, sizeof(BITMAPINFOHEADER));
	//  [8/11/2010 XPMUser]
	file.Write(pRGB, sizeof(RGBQUAD) * 256);
	//  [8/11/2010 XPMUser]

	UCHAR* buf3 = new UCHAR[/*iByteToTransfer*/BitmapHead.biSizeImage];
	bmp.GetBitmapBits(BitmapHead.biSizeImage, buf3);
	file.Write(buf3, BitmapHead.biSizeImage);
	file.Close();
	if (buf3) { delete[] buf3; }
	if (pRGB) {
		delete[] pRGB;
	}
}

void CSICam::GetMaxMin(float* pWave, int count, float& Max, float& Min) {
	Max = Min = *pWave;

	pWave++;
	for (int i = 1; i < count; i++) {
		if (*pWave < Min) {
			Min = *pWave;
		}
		if (*pWave > Max) {
			Max = *pWave;
		}
		pWave++;
	}
}

void CSICam::StorePeakInfo() {
	if (!pPeakBuf) {
		return;
	}
	CString name;

	name = FileName + ".TXT";

	FILE* fp;
	fp = fopen(name, "wb");
	if (fp) {
		int i;
		fprintf(fp, "Total Peaks\r\nTop: %d\r\nBottom:%d\r\n", pPeakBuf[0].Total, pPeakBuf[1].Total);
		if (pPeakBuf[0].Total <= MAXPEAKCOUNT) {
			fprintf(fp, "Top\r\n");
			for (i = 0; i < pPeakBuf[0].Total; i++) {
				pPeakBuf[0].Peaks[i].Dump(fp);
			}
		}
		if (pPeakBuf[1].Total <= MAXPEAKCOUNT) {
			fprintf(fp, "Bottom\r\n");
			for (i = 0; i < pPeakBuf[1].Total; i++) {
				pPeakBuf[1].Peaks[i].Dump(fp);
			}
		}

		CSingleLock Lock(&CSSLine);
		Lock.Lock();
		if (pSLineT) {
			fprintf(fp, "\r\n");
			pSLineT->Dump(fp);
		}
		if (pSLineB) {
			fprintf(fp, "\r\n");
			pSLineB->Dump(fp);
		}
		Lock.Unlock();

		fclose(fp);
	}
}

void CSICam::ChangeExposure(BOOL bWait) {
	if (bPause) {
		return;
	}
	bChangeExposure = TRUE;
	if (bWait) {
		while (bChangeExposure) {
			Sleep(80);
		}
	}
}

void CSICam::DrawHisto(CDC* pDC, CRect& rc) {
	if (!pDC || !RsltSpec.pV) {
		return;
	}

	CSingleLock Lock(&RsltSpec.CS);
	Lock.Lock();

	int wd = rc.Width();
	int ht = rc.Height() * 0.20;

	int i, y;
	float sfy = (ht * 0.9) / RsltSpec.MaxV;

	int jPLow = RsltSpec.Poly[0].jPlow;
	int jPHi = RsltSpec.Poly[0].jPhi;

	for (i = 1; i < MAXPOLYVLU; i++) {
		if (RsltSpec.Poly[i].fThk > 0) {
			if (RsltSpec.Poly[i].jPlow < jPLow) {
				jPLow = RsltSpec.Poly[i].jPlow;
			}
			if (RsltSpec.Poly[i].jPhi > jPHi) {
				jPHi = RsltSpec.Poly[i].jPhi;
			}
		}
	}
	RsltSpec.Poly[0].jPlow = jPLow;
	RsltSpec.Poly[0].jPhi = jPHi;
	jPLow *= 0.94f;
	jPHi *= 1.06f;

	if (jPLow < 0) {
		jPLow = 0;
	}
	if (jPHi >= RsltSpec.S) {
		jPHi = RsltSpec.S - 1;
	}
	float sfx = (jPHi - jPLow) / float(rc.Width() - 1);

	CPen pen1, pen2, pen3;
	HPEN oldPen;
	pen2.CreatePen(PS_SOLID, 1, RGB(16, 255, 128));
	pen1.CreatePen(PS_SOLID, 2, RGB(255, 128, 128));
	pen3.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
	oldPen = (HPEN)pDC->SelectObject(pen2);
	int b = rc.top + ht + 24;
	int nx;

	for (i = 0; i < rc.Width(); i += 2) {
		int ii = i + rc.left;
		nx = sfx * i + jPLow;
		if (nx > RsltSpec.S) {
			break;
		}
		y = (RsltSpec.pV[nx].V) * sfy;
		pDC->MoveTo(ii, b);
		pDC->LineTo(ii, b - y);
	}
	CString str;
	pDC->SelectObject(pen3);
	i = (RsltSpec.Poly[0].jPlow - jPLow) / sfx + rc.left;
	y = (RsltSpec.pV[RsltSpec.Poly[0].jPlow].V) * sfy;
	pDC->MoveTo(i, b);
	pDC->LineTo(i, b - y);
	i = (RsltSpec.Poly[0].jPhi - jPLow) / sfx + rc.left;
	y = (RsltSpec.pV[RsltSpec.Poly[0].jPhi].V) * sfy;
	pDC->MoveTo(i, b);
	pDC->LineTo(i, b - y);

	// Label max peak [11/25/2012 Yuen]
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(255, 64, 64));
	for (int k = 0; k < MAXPOLYVLU; k++) {
		if (RsltSpec.Poly[k].jPmax > 0) {
			pDC->SelectObject(pen1);
			i = (RsltSpec.Poly[k].jPmax - jPLow) / sfx + rc.left;
			y = (RsltSpec.pV[RsltSpec.Poly[k].jPmax].V) * sfy;
			pDC->MoveTo(i, b);
			pDC->LineTo(i, b - y);

			pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
			i = (RsltSpec.Poly[k].jPmax - jPLow) / sfx + rc.left;
			y = (RsltSpec.pV[RsltSpec.Poly[k].jPmax].V) * sfy;
			str.Format("T%.3f", RsltSpec.Poly[k].fThk);
			pDC->TextOut(i, b - y - 14, str);
			str.Format("G:%.2f  W:%.2f", RsltSpec.Poly[k].GOF, RsltSpec.Poly[k].WHH);
			pDC->TextOut(i, b - y, str);
		}
	}
	if (oldPen) pDC->SelectObject(oldPen);

	Lock.Unlock();
}

void CSICam::OnLButtonDblClk(UINT nFlags, CPoint point) {
	if (pWnd) {
		static CPoint Pt;
		CRect rc;
		GetClientRect(&rc);
		Pt.x = point.x - rc.CenterPoint().x;
		Pt.y = rc.CenterPoint().y - point.y;

		pWnd->PostMessage(UM_BUTTON_CLICK, 0, (LPARAM)&Pt);
	}
}

void CSICam::OnMouseMove(UINT nFlags, CPoint point) {
	if (bMMove) {
		SIPoints.EndPt = point;
	}
	if (pWnd) {
		CRect rc;
		GetClientRect(&rc);
		SIPoints.CurPt.x = point.x - rc.CenterPoint().x;
		SIPoints.CurPt.y = rc.CenterPoint().y - point.y;
		pWnd->PostMessage(UM_MOUSE_MOVE, 0, (LPARAM)&SIPoints);
	}
}

void CSICam::OnLButtonUp(UINT nFlags, CPoint point) {
	bMMove = FALSE;

	if (::GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		C413Global* g = &p413App->Global;
		CPackedDIB simu;
		simu.Duplicate(&SimuDIB);
		if (SIPoints.EndPt.x < SIPoints.StartPt.x || SIPoints.EndPt.y < SIPoints.StartPt.y) {
			SIPoints.SwapStartEnd();
		}
		int st_y = float(simu.IHeight / 512.0) * float(SIPoints.StartPt.y);
		int ed_y = float(simu.IHeight / 512.0) * float(SIPoints.EndPt.y);
		int st_x = float(simu.IWidth / 640.0) * float(SIPoints.StartPt.x);
		int ed_x = float(simu.IWidth / 640.0) * float(SIPoints.EndPt.x);
		int width = abs(ed_x - st_x), height = abs(ed_y - st_y);

		std::vector<double> line_score;
		std::vector<std::vector<BYTE>> img;

		for (int i = st_y; i < ed_y; i++)
		{
			double score = 0.0;
			std::vector<BYTE> line;
			for (int j = st_x; j < ed_x; j++)
			{
				BYTE pix = simu.GetPixelL(j, i);
				line.push_back(pix);

				// calculate each line score
				score += pix * pix;
			}
			score /= ed_x - st_x;
			img.push_back(line);
			line_score.push_back(score);
		}

		// bottom 2 to find lowest 2 scores
		double min1 = DBL_MAX;
		int min1ind = -1;
		for (int i = 0; i < height; i++)
		{
			if (line_score[i] < min1) {
				min1 = line_score[i];
				min1ind = i;
			}
		}
		double min2 = DBL_MAX;
		int min2ind = -1;
		for (int i = height - 1; i > 0; i--)
		{
			if (line_score[i] < min2 && i != min1ind) {
				min2 = line_score[i];
				min2ind = i;
			}
		}
		//FILE* fp = fopen("C:\\TEMP\\via_data.csv", "a+");
		//for (int i = 0; i < width; i++)
		//{
		//	fprintf(fp, "%d,%d\n", img[min1ind][i], img[min2ind][i]);
		//}
		//fprintf(fp, "\n");
		//if (fp) fclose(fp);
		//

		// process line to get diameter in pixel
		if (0) {	// contrast stretch
			short Max, Min;
			Max = Min = img[min1ind][0];
			for (int i = 0; i < width; i++) {
				if (img[min1ind][i] > Max) {
					Max = img[min1ind][i];
				}
				if (img[min1ind][i] < Min) {
					Min = img[min1ind][i];
				}
			}
			Min += 0.1 * (Max - Min);
			float sf = 255.0f / float(Max - Min);
			for (int i = 0; i < width; i++) {
				img[min1ind][i] = sf * (img[min1ind][i] - Min);
				if (img[min1ind][i] < 0) {
					img[min1ind][i] = 0;
				}
			}
			//FILE* fp = fopen("C:\\TEMP\\via_data.csv", "a+");
			//for (int i = 0; i < width; i++)
			//{
			//	fprintf(fp, "%d,%d\n", img[min1ind][i], img[min2ind][i]);
			//}
			//fprintf(fp, "\n");
			//if (fp) fclose(fp);
		}
		{	// smooth
			for (int repeat = 0; repeat < 3; repeat++)
			{
				std::vector<BYTE> newline; newline.assign(width, 0);
				for (int i = 1; i < width - 1; i++) {
					img[min1ind][i] = (img[min1ind][i - 1] + img[min1ind][i] + img[min1ind][i + 1]) / 3;
				}
				img[min1ind][0] = img[min1ind][1];
				img[min1ind][width - 1] = img[min1ind][width - 2];
			}
			//FILE* fp = fopen("C:\\TEMP\\via_data.csv", "a+");
			//for (int i = 0; i < width; i++)
			//{
			//	fprintf(fp, "%d,%d\n", img[min1ind][i], img[min2ind][i]);
			//}
			//fprintf(fp, "\n");
			//if (fp) fclose(fp);
		}

		std::vector<int> integrate1;
		{	// integrate
			for (int i = 1; i < width - 1; i++) {
				integrate1.push_back(img[min1ind][i - 1] - img[min1ind][i + 1]);
				if (i == 1 || i == width - 2) {
					integrate1.push_back(img[min1ind][i - 1] - img[min1ind][i + 1]);
				}
			}
			//FILE* fp = fopen("C:\\TEMP\\via_data.csv", "a+");
			//for (int i = 0; i < width; i++)
			//{
			//	fprintf(fp, "%d\n", integrate1[i]);
			//}
			//fprintf(fp, "\n");
			//if (fp) fclose(fp);
		}
		{	// look for max and min
			int max = INT_MIN, min = INT_MAX;
			int max_ind = -1, min_ind = -1;
			for (int i = 0; i < width; i++)
			{
				if (integrate1[i] > max) {
					max = integrate1[i];
					max_ind = i;
				}
				if (integrate1[i] < min) {
					min = integrate1[i];
					min_ind = i;
				}
			}
			textout = SIPoints.StartPt;
			diameter = abs(max_ind - min_ind);
		}
	}

	SIPoints.StartPt.x = 9999;

	CStatic::OnLButtonUp(nFlags, point);
}

void CSICam::OnLButtonDown(UINT nFlags, CPoint point) {
	bMMove = TRUE;
	SIPoints.StartPt = point;
	SIPoints.EndPt = point;
	if (pWnd) {
		CRect rc;
		GetClientRect(&rc);
		SIPoints.CurPt.x = point.x - rc.CenterPoint().x;
		SIPoints.CurPt.y = rc.CenterPoint().y - point.y;
		pWnd->PostMessage(UM_MOUSE_MOVE, 0, (LPARAM)&SIPoints);
	}
	CStatic::OnLButtonDown(nFlags, point);
}

void CSICam::OnRButtonDblClk(UINT nFlags, CPoint point) {
	if (pWnd) {
		pWnd->PostMessage(WM_COMMAND, UM_BUTTON_RDCLICK, NULL);
	}
}

void CSICam::OnRButtonUp(UINT nFlags, CPoint point) {
	CMenu menu;
	CMenu* pPopUp;
	menu.LoadMenu(IDR_MENU413);
	pPopUp = menu.GetSubMenu(4);
	if (!pPopUp) return;

	CRect rc;
	C413Global* g = &p413App->Global;

	if (pCam->IsSimu()) {
		pPopUp->AppendMenu(MF_STRING, ID_LOADIMAGE, "Load Image");
		pPopUp->AppendMenu(MF_STRING, ID_RELOADIMAGE, "Reload Image");
	}
	if (!bSetRedXHair) {
		pPopUp->EnableMenuItem(ID_PLACEREDXHAIR, MF_GRAYED | MF_BYCOMMAND);
	}
	if (!bAllowMoveXHair) {
		pPopUp->EnableMenuItem(ID_PLACEGREENXHAIR, MF_GRAYED | MF_BYCOMMAND);
		pPopUp->EnableMenuItem(ID_PLACEBLUEXHAIR, MF_GRAYED | MF_BYCOMMAND);
	}

	CPoint Pt = point;
	ClientToScreen(&Pt);
	int res = pPopUp->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD, Pt.x, Pt.y, this);
	switch (res) {
	case ID_LOADIMAGE:
		LoadImage();
		break;
	case ID_RELOADIMAGE:
		if (SimuDIBFilename.GetLength() > 0) LoadSimuDIB(SimuDIBFilename);
		break;
	case ID_PLACEREDXHAIR:
		if (pRcp && bSetRedXHair) {
			GetClientRect(&rc);
			switch (g->ActiveCamera) {
			case CCamBase::THINFCAM:
				g->fTgtRedTF.X = point.x / float(rc.Width());
				g->fTgtRedTF.Y = point.y / float(rc.Height());
				break;
			default:
				g->fTgtRedECHO.X = point.x / float(rc.Width());
				g->fTgtRedECHO.Y = point.y / float(rc.Height());
				break;
			}
			bShowCrossHair = TRUE;
			if (pWnd) {
				pWnd->PostMessage(UM_REDXHAIR);
			}
		}
		break;
	case ID_PLACEGREENXHAIR:
		if (pRcp && bAllowMoveXHair) {
			GetClientRect(&rc);
			fTGtGrnX = /*pRcp->fTgtGRnX[g->CurPaSet-1][0] =*/ point.x / float(rc.Width());
			fTGtGrnY = /*pRcp->fTgtGRnY[g->CurPaSet-1][0] =*/ point.y / float(rc.Height());
			bShowCrossHair = TRUE;
			if (pWnd) {
				pWnd->PostMessage(UM_GREENXHAIR);
			}
		}
		break;
	case ID_PLACEBLUEXHAIR:
		if (pRcp && bAllowMoveXHair) {
			GetClientRect(&rc);
			fTGtBluX = /*pRcp->fTgtBLuX[g->CurPaSet-1][0] =*/ point.x / float(rc.Width());
			fTGtBluY = /*pRcp->fTgtBLuY[g->CurPaSet-1][0] =*/ point.y / float(rc.Height());
			bShowCrossHair = TRUE;
			if (pWnd) {
				pWnd->PostMessage(UM_BLUEXHAIR);
			}
		}
		break;
	case ID_HIDEALLCROSSHAIR:
		bShowCrossHair = FALSE;
		bShowCenterTarget = FALSE;
		break;
	case ID_SHOWYELLOW:
		bShowCenterTarget = TRUE;
		break;
	case ID_SHOWRGBXHAIR:
		bShowCrossHair = TRUE;
		break;
	case ID_TAKEPICTUREOVERLAY:
		FileDirc.Format("C:\\TEMP\\%s\\", CTime::GetCurrentTime().Format("%y%m%d").GetBuffer());
		FileBase.Format("%s%d.BMP", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer(), int(rand() / float(RAND_MAX) * 100));
		bStoreComposite = TRUE;
		break;
	case ID_TAKEPICTURE:
		FileDirc.Format("C:\\TEMP\\%s\\", CTime::GetCurrentTime().Format("%y%m%d").GetBuffer());
		FileBase.Format("%s%d.BMP", CTime::GetCurrentTime().Format("%H%M%S").GetBuffer(), int(rand() / float(RAND_MAX) * 100));
		bSaveDIB = TRUE;
		break;
	}
}

BOOL CSICam::AllocSLine() {
	CSingleLock Lock(&CSSLine);
	Lock.Lock();
	if (!pSLineT) {
		pSLineT = new CScanLine;
	}
	if (!pSLineB) {
		pSLineB = new CScanLine;
	}
	Lock.Unlock();

	return pSLineT && pSLineB;
}

void CSICam::SetRecipe(CRecipe413* pR) {
	CSingleLock Lock(&CS);
	Lock.Lock();
	pRcp = pR;
	Lock.Unlock();
}

void CSICam::RSpecClear() {
	bClearRsltSpec = TRUE;
}

float CSICam::GetZoom(short N) {
	switch (N) {
	case 512:
		return 8.0f;
	case 1024:
		return 18.0f;
	case 1536:
		return 22.0f;
	case 2048:
		return 26.0f;
	case 2560:
		return 36.0f;
	case 3072:
		return 44.0f;
	case 3584:
		return 56.0f;
	case 4096:
		return 80.0f;
	default:
		return 160.0f;
	}
}

BOOL CSICam::GetAngle(float& angle) {
	int n;
	CString name;
	n = SimuDIBFilename.Find("SNAP");
	if (n != -1) {
		name = SimuDIBFilename.Mid(n);
		name = name.Mid(name.Find('-') + 1);
		name = name.Left(name.Find('-'));
		angle = float(atof(name));
		return TRUE;
	}
	return FALSE;
}

void CSICam::LoadImage() {
	CFileDialog dlg(TRUE, "BMP", NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"Bitmap files|*.BMP|");
	if (dlg.DoModal() == IDOK) {
		SimuDIBFilename = dlg.GetPathName();
		LoadSimuDIB(SimuDIBFilename);
	}
}

void CSICam::ClearTextInfo() {}

void CSICam::ShowCenterTarget(CDC& dcMem, CRect& rc) {
	if (bShowCenterTarget) {
		CPoint ptCenter;

		ptCenter.x = rc.Width() * 0.5f;
		ptCenter.y = rc.Height() * 0.5f;

		CPen* pOldPen = dcMem.SelectObject(&CursorYellow);
		dcMem.MoveTo(ptCenter.x, ptCenter.y - 38);
		dcMem.LineTo(ptCenter.x, ptCenter.y - 4);
		dcMem.MoveTo(ptCenter.x, ptCenter.y + 4);
		dcMem.LineTo(ptCenter.x, ptCenter.y + 38 + 4);

		dcMem.MoveTo(ptCenter.x - 50, ptCenter.y);
		dcMem.LineTo(ptCenter.x - 4, ptCenter.y);
		dcMem.MoveTo(ptCenter.x + 4, ptCenter.y);
		dcMem.LineTo(ptCenter.x + 50, ptCenter.y);

		if (pOldPen) dcMem.SelectObject(pOldPen);
	}
}

void CSICam::ShowGreenTarget(CDC& dcMem, CRect& rc) {
	if (pRcp && bShowCrossHair) {
		CPoint ptCenter;

		C413Global* g = &p413App->Global;
		ptCenter.x = rc.Width() * fTGtGrnX;
		ptCenter.y = rc.Height() * fTGtGrnY;

		CPen* pOldPen = dcMem.SelectObject(&CursorGreen);
		dcMem.MoveTo(ptCenter.x, ptCenter.y - 38);
		dcMem.LineTo(ptCenter.x, ptCenter.y - 4);
		dcMem.MoveTo(ptCenter.x, ptCenter.y + 4);
		dcMem.LineTo(ptCenter.x, ptCenter.y + 38 + 4);

		dcMem.MoveTo(ptCenter.x - 50, ptCenter.y);
		dcMem.LineTo(ptCenter.x - 4, ptCenter.y);
		dcMem.MoveTo(ptCenter.x + 4, ptCenter.y);
		dcMem.LineTo(ptCenter.x + 50, ptCenter.y);

		if (pOldPen) dcMem.SelectObject(pOldPen);
	}
}

void CSICam::ShowBlueTarget(CDC& dcMem, CRect& rc) {
	if (pRcp && bShowCrossHair) {
		CPoint ptCenter;

		C413Global* g = &p413App->Global;
		ptCenter.x = rc.Width() * fTGtBluX;
		ptCenter.y = rc.Height() * fTGtBluY;

		CPen* pOldPen = dcMem.SelectObject(&CursorBlue);
		dcMem.MoveTo(ptCenter.x, ptCenter.y - 38);
		dcMem.LineTo(ptCenter.x, ptCenter.y - 4);
		dcMem.MoveTo(ptCenter.x, ptCenter.y + 4);
		dcMem.LineTo(ptCenter.x, ptCenter.y + 38 + 4);

		dcMem.MoveTo(ptCenter.x - 50, ptCenter.y);
		dcMem.LineTo(ptCenter.x - 4, ptCenter.y);
		dcMem.MoveTo(ptCenter.x + 4, ptCenter.y);
		dcMem.LineTo(ptCenter.x + 50, ptCenter.y);

		if (pOldPen) dcMem.SelectObject(pOldPen);
	}
}

void CSICam::ShowRedTarget(CDC& dcMem, CRect& rc) {
	C413Global* g = &p413App->Global;
	if (pRcp && bShowCrossHair) {
		CPoint ptCenter;

		switch (g->ActiveCamera) {
		case CCamBase::THINFCAM:
			ptCenter.x = rc.Width() * g->fTgtRedTF.X;
			ptCenter.y = rc.Height() * g->fTgtRedTF.Y;
			break;
		default:
			ptCenter.x = rc.Width() * g->fTgtRedECHO.X;
			ptCenter.y = rc.Height() * g->fTgtRedECHO.Y;
			break;
		}

		CPen* pOldPen = dcMem.SelectObject(&CursorRed);
		dcMem.MoveTo(ptCenter.x, ptCenter.y - 38);
		dcMem.LineTo(ptCenter.x, ptCenter.y - 4);
		dcMem.MoveTo(ptCenter.x, ptCenter.y + 4);
		dcMem.LineTo(ptCenter.x, ptCenter.y + 38 + 4);

		dcMem.MoveTo(ptCenter.x - 50, ptCenter.y);
		dcMem.LineTo(ptCenter.x - 4, ptCenter.y);
		dcMem.MoveTo(ptCenter.x + 4, ptCenter.y);
		dcMem.LineTo(ptCenter.x + 50, ptCenter.y);
		if (pOldPen) dcMem.SelectObject(pOldPen);
	}
}

BOOL CSICam::IsReserved() {
	if (Reserved.GetLength() > 0) {
		return TRUE;
	}
	return FALSE;
}

BOOL CSICam::SnapOneFrame(short Exposure) {
	int ExpBak = GetExposure();
	SetExposure(Exposure);
	Sleep(400);
	BOOL bRet = SnapOneFrame();
	SetExposure(ExpBak);
	return bRet;
}

BOOL CSICam::SnapOneFrame() {
	if (bRun) {
		//Sleep(150);	// roughly 1.5 video frames [4/18/2013 Administrator
		bSnapRet = FALSE;
		bStore = TRUE;
		//StoreCount = frame;
		ULONGLONG tick = GetTickCount64();
		while (bStore) {
			if ((GetTickCount64() - tick) > 10000) {
				// Time out. Fail to store image [3/20/2013 FSMT]
				p413App->Log("SnapOneFrame timeout", 2);
				return FALSE;
			}
			Sleep(100);
		}
		bSnapRet = TRUE;
		return bSnapRet;
	}
	return FALSE;
}

void CSICam::SleepEx(DWORD tout) {
	Sleep(tout);
	return;

	ULONGLONG tm = GetTickCount64();
	while ((GetTickCount64() - tm) < tout) {
		MSG msg;
		while (PeekMessage(&msg, AfxGetMainWnd()->m_hWnd, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

BOOL CSICam::DoubleSnapOneFrame() {
	return SnapOneFrame(); // Bypassed [5/21/2014 FSMT]

	// Make 3 tries [7/28/2013 Yuen]
	CString name;
	C413Global* g = &p413App->Global;
	SnapLimit = 96;
	for (int i = 0; i < 3; i++) {
		if (SnapOneFrame()) {
			g->StoredDIB2.Duplicate(g->StoredDIB);
			if (SnapOneFrame()) {
				SnapRsl = g->StoredDIB.Different(g->StoredDIB2);
				if (SnapRsl < SnapLimit) {
					return TRUE;
				}
			}
			else return FALSE;
		}
		else return FALSE;
		Sleep(250);
	}

	return FALSE;
}

void CSICam::ClearEchoInfo() {
	CSingleLock Lock(&CSSLine);
	Lock.Lock();
	if (pSLineT) {
		delete pSLineT;
		pSLineT = NULL;
	}
	if (pSLineB) {
		delete pSLineB;
		pSLineB = NULL;
	}
	Lock.Unlock();
}

BOOL CSICam::ProcessSaveDIB(SIPackedDIB* pPackedDIB) {
	if (!pPackedDIB->pDIBBits) {
		return FALSE;
	}
	FileName = FileDirc + FileBase;

	CString path = DosUtil.DirSys.ExtractFilePath(FileName);
	DosUtil.DirSys.GotoDirec(path);

	CFile file;
	if (!file.Open(FileName, CFile::modeWrite | CFile::modeCreate)) {
		return FALSE;
	}

	long BytesPerRow = pPackedDIB->BmpInfo.biWidth * (pPackedDIB->BmpInfo.biBitCount / 8);
	BytesPerRow += (4 - BytesPerRow % 4) % 4;

	BITMAPINFOHEADER BitmapHead;
	ZeroMemory(&BitmapHead, sizeof(BITMAPINFOHEADER));
	BitmapHead.biWidth = pPackedDIB->BmpInfo.biWidth;
	BitmapHead.biHeight = pPackedDIB->BmpInfo.biHeight;
	BitmapHead.biPlanes = 1;
	BitmapHead.biBitCount = pPackedDIB->BmpInfo.biBitCount;
	BitmapHead.biSizeImage = BytesPerRow * abs(pPackedDIB->BmpInfo.biHeight);
	BitmapHead.biSize = sizeof(BITMAPINFOHEADER);
	BitmapHead.biClrUsed = 0;
	BitmapHead.biClrImportant = 0;
	BitmapHead.biCompression = BI_RGB;

	// assumes 256 gray level
	BITMAPFILEHEADER   hdr;
	DWORD dwFileHeaderSize = sizeof(BITMAPINFOHEADER) + sizeof(hdr) + 256 * sizeof(RGBQUAD);
	RGBQUAD* pRGB = new RGBQUAD[256];
	for (int i = 0; i < 256; i++) {
		pRGB[i].rgbRed = i;
		pRGB[i].rgbGreen = i;
		pRGB[i].rgbBlue = i;
	}
	// Fill in the fields of the file header
	hdr.bfType = DS_BITMAP_FILEMARKER;
	hdr.bfSize = dwFileHeaderSize + BitmapHead.biSizeImage;
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;
	hdr.bfOffBits = dwFileHeaderSize;

	// Write the file header
	file.Write(&hdr, sizeof(BITMAPFILEHEADER));
	// Write the DIB header
	file.Write(&BitmapHead, sizeof(BITMAPINFOHEADER));
	//  [8/11/2010 XPMUser]
	file.Write(pRGB, sizeof(RGBQUAD) * 256);
	//  [8/11/2010 XPMUser]

	file.Write(pPackedDIB->pDIBBits, BitmapHead.biSizeImage);
	file.Close();

	if (pRGB) {
		delete[] pRGB;
	}

	return TRUE;
}

void CSICam::ResizeWindow() {
	CRect rc;

	GetClientRect(&rc);
	ClientToScreen(&rc);
	rc.bottom = rc.top + rc.Width() / CAMASPECTRATIO;
	SetWindowPos(this, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER | SWP_SHOWWINDOW);
}

BOOL CSICam::IsSimu() {
	if (pCam) {
		return pCam->IsSimu();
	}
	// pCam must be initialized [6/1/2014 Yuen]
	// ASSERT(0); [6/10/2020] temporarily commented
	return TRUE;
}

void CSICam::LoadStockDIB(CString name) {
	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "\\IMAGES\\";
	dir += name;
	LoadSimuDIB(dir);
}

int CSICam::GetCount() {
	return GetCount(&pCapCxt->PackedDIB);
}

int CSICam::GetCount(SIPackedDIB* pPackedDIB) {
	int max = 0;
	int ht = abs(pPackedDIB->BmpInfo.biHeight);
	short bits = (pPackedDIB->BmpInfo.biBitCount / 8);
	long BytesPerRow = pPackedDIB->BmpInfo.biWidth * bits;
	BytesPerRow += (4 - BytesPerRow % 4) % 4;	// int alignment
	long sum = 0;
	int cnt = 0;
	int step = (ht - 32) / (ht * 0.3);
	if (bits == 3) {
		for (int j = 16; j < ht - 16; j += step) {
			//BYTE *p = pPackedDIB->pDIBBits + j*BytesPerRow;
			BYTE* p = pPackedDIB->pDIBBits + j * BytesPerRow + 1;
			for (int i = 0; i < BytesPerRow; i++) {
				//sum += *(p++) + *(p++) + *(p++);
				sum += *(p); p += 3;
				cnt++;
			}
		}
	}
	else {
		for (int j = 16; j < ht - 16; j += step) {
			BYTE* p = pPackedDIB->pDIBBits + j * BytesPerRow;
			for (int i = 0; i < BytesPerRow; i++) {
				sum += *(p++);
				cnt += 3;
			}
		}
	}
	max = sum / (cnt / 10);
	//max = sum / (cnt/2);
	return max;
}