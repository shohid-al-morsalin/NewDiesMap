#include "stdafx.h"

#include "413/MotorS2.h"
#include "MPoint.h"
#include "SiCamSettingDlg.h"

#include "413AppCam.h"

HANDLE C413AppCam::hCameraThread1 = NULL;
HANDLE C413AppCam::hCameraThread2 = NULL;

C413AppCam::C413AppCam() {
	pVideo1 = NULL;
	pVideo2 = NULL;
}

C413AppCam::~C413AppCam() {
	if (hCameraThread1) {
		ASSERT(0);
	}
	if (hCameraThread2) {
		ASSERT(0);
	}
}

void C413AppCam::DoAutoExposure(CPackedDIB& StoreDIB, CPackedDIB& SobDIB, short dir, float target, short Wdw, float XOff, float YOff, CSICam* pVideo, CListBox* pList) {
	if (!pVideo) {
		return;
	}

	float score;
	CString str;
	short Exposure = pVideo->iExpose;
	for (int i = 0; i < 196; i++) {
		if (Global.bAborting) {
			break;
		}
		Exposure += dir;
		Global.Exposure[pVideo->iCID] = Exposure;
		if (pVideo->SnapOneFrame(Exposure)) {
			StoreDib1(Global.StoredDIB, SobDIB, Wdw, Wdw, XOff, YOff);
			score = SobDIB.GetIntensityCount();
			str.Format("Score: %.4f", score);
			Message(pList, str);
			if (dir > 0) {
				if (score > target) {
					break;
				}
			}
			else {
				if (score < target) {
					break;
				}
			}
		}
		else return;

		if (pVideo->IsSimu()) {
			// prevent infinate loop [6/1/2014 Yuen]
			break;
		}
	}
}

void C413AppCam::DoAutoFocus(CCamBase::CAMERAID ActiveCamera, float dir, float XOff, float YOff, CSICam* pVideo, short ID, CImgWnd* pImgWnd, CListBox* pList) {
	if ((!pVideo) || (!pMtr)) {
		return;
	}
	CString str;
	float z1 = 0, z2 = 0, z1b, z2b, score1, score2;
	CPackedDIB StoreDIBX, SobDIB;
	pMtr->GetZPosDev(z1, z2);
	z1b = z1;
	z2b = z2;
	int i;
	for (i = 0; i < 32; i++) {
		if (Global.bAborting) {
			i = 32;
			break;
		}
		float wSiz = 200;
		Sleep(50);
		if (pVideo->SnapOneFrame()) {
			StoreDib1(Global.StoredDIB, SobDIB, wSiz, wSiz, XOff, YOff);	// @@@ fTGtGrnX will be corrected later [11/3/2012 Yuen]
			SobDIB.Sobel();
			if (pImgWnd) pImgWnd->DupStore1(SobDIB);
			score1 = SobDIB.GetIntensityCount();
			if (i == 0) {
				score2 = score1;
			}
			str.Format("Score: %.4f", score1);
			Message(pList, str);
			if (score1 >= score2) {
				switch (ActiveCamera) {
				default:
					pMtr->MoveEchoTopZ(z1 + dir);
					break;
				case CCamBase::THINFCAM:
					pMtr->MoveThinFilmZ(z2 + dir);
					break;
				}
				pMtr->GetZPosDev(z1, z2);
				score2 = score1;
			}
			else break;
		}
		else return;

		if (pVideo->IsSimu()) {
			break;
		}
	}
	if (i == 32) {
		// failed [2/17/2011 C2C]
		switch (ActiveCamera) {
		default:
			pMtr->MoveEchoTopZ(z1b);
			break;
		case CCamBase::THINFCAM:
			pMtr->MoveThinFilmZ(z2b);
			break;
		}
	}
	else {
		// Back up one step [7/4/2013 Yuen]
		switch (ActiveCamera) {
		default:
			pMtr->MoveEchoTopZ(z1 - dir);
			break;
		case CCamBase::THINFCAM:
			pMtr->MoveThinFilmZ(z2 - dir);
			break;
		}
	}
}

void C413AppCam::AutoFocusVideo(CCamBase::CAMERAID ActiveCamera, float XOff, float YOff, CSICam* pVideo, CImgWnd* pImgWnd, CListBox* pList) {
	if (!pVideo) {
		return;
	}
	if (!(Global.Confg.bEchoMicroscopeMod || Global.Confg.bThinFilmMicroscopeMod)) {
		return;
	}
	if (!(Global.Confg.bEchoTopMotorMod || Global.Confg.bThinFilmMotorMod)) {
		return;
	}
	CPackedDIB StoreDIBX, SobDIB;

	CString str;
	double score1, score2;

	int wSiz = 200;

	float z1, z2, z1b, z2b;
	if (pMtr) pMtr->GetZPosDev(z1, z2);
	z1b = z1;
	z2b = z2;

	if (pVideo->SnapOneFrame()) {
		StoreDib1(Global.StoredDIB, SobDIB, wSiz, wSiz, XOff, YOff); // @@@ fTGtGrn[] will be change to appropriate index later [11/3/2012 Yuen]
		SobDIB.Sobel();
		if (pImgWnd) pImgWnd->DupStore1(SobDIB);
		score1 = SobDIB.GetIntensityCount();
		str.Format("Score1: %.4f", score1);
		Message(pList, str);
	}
	else return;

	float dir = 0.008f;
	switch (ActiveCamera) {
	default:
		//dir = -0.025f;
		dir = -Global.AutoFocusStepEcho;
		if (pMtr) pMtr->MoveEchoTopZ(z1 + dir);
		break;
	case CCamBase::THINFCAM:
		//dir = -0.005f;
		dir = -Global.AutoFocusStepTF;
		if (pMtr) pMtr->MoveThinFilmZ(z2 + dir);
		break;
	}

	if (pVideo->SnapOneFrame()) {
		StoreDib1(Global.StoredDIB, SobDIB, wSiz, wSiz, XOff, YOff);
		SobDIB.Sobel();
		if (pImgWnd) pImgWnd->DupStore1(SobDIB);
		score2 = SobDIB.GetIntensityCount();
		str.Format("Score2: %.4f", score2);
		Message(pList, str);
	}
	else return;

	if (score1 > score2) {
		dir = -dir;
	}

	DoAutoFocus(ActiveCamera, dir, XOff, YOff, pVideo, 0, pImgWnd, pList);
}

BOOL C413AppCam::StopCameraThread(CSICam& m_cVideo) {
	if (!m_cVideo.hLiveThread) {
		return FALSE;
	}
	if (hCameraThread1 == m_cVideo.hLiveThread) {
		m_cVideo.bRun = FALSE;
		::WaitForSingleObject(hCameraThread1, INFINITE);
		while (!m_cVideo.bStop) {
			ASSERT(0);
			Sleep(10);
		}
		::CloseHandle(hCameraThread1);
		m_cVideo.hLiveThread = NULL;
		m_cVideo.iCID = CCamBase::SIMULCAM;
		// 		pVideo1 = NULL;
		hCameraThread1 = NULL;
		Global.ActiveCamera = CCamBase::SIMULCAM;
		return TRUE;
	}
	return FALSE;
}

BOOL C413AppCam::StopCameraThread2(CSICam& m_cVideo) {
	if (!m_cVideo.hLiveThread) {
		return FALSE;
	}
	if (hCameraThread2 == m_cVideo.hLiveThread) {
		m_cVideo.bRun = FALSE;
		::WaitForSingleObject(hCameraThread2, INFINITE);
		while (!m_cVideo.bStop) {
			ASSERT(0);
			Sleep(10);
		}
		::CloseHandle(hCameraThread2);
		m_cVideo.hLiveThread = NULL;
		m_cVideo.iCID = CCamBase::SIMULCAM;
		// 		pVideo2 = NULL;
		hCameraThread2 = NULL;
		return TRUE;
	}
	return FALSE;
}

BOOL C413AppCam::StartCameraThread(CCamBase::CAMERAID No, CRecipe413* pRcp, CSICam& m_cVideo) {
	if (m_cVideo.hLiveThread) {
		// Already started, simply return [3/18/2011 XPMUser]
		return TRUE;
	}
	if (hCameraThread1) {
		// Someone else is using the camera [7/23/2013 Yuen]
		int i = 0;
		for (; i < 100; i++) {
			Sleep(100);
			if (!hCameraThread1) {
				break;
			}
		}
		if (i >= 100) {
			//ASSERT(0);	//12142024/HAQUE/CAMERA EXCEPTION STOP
			return FALSE;
		}
	}

	m_cVideo.SetRecipe(pRcp);
	m_cVideo.bRun = FALSE;
	m_cVideo.iCID = No;
	m_cVideo.iCNum = Global.CamraNum[No];
	hCameraThread1 = ::CreateThread(NULL, 0, m_cVideo.LiveThreadFunc, &m_cVideo, CREATE_SUSPENDED, NULL);
	if (!hCameraThread1) {
		return FALSE;
	}
	::SetThreadPriority(hCameraThread1, THREAD_PRIORITY_ABOVE_NORMAL);
	m_cVideo.iCID = No;
	m_cVideo.hLiveThread = hCameraThread1;
	pVideo1 = &m_cVideo;
	::ResumeThread(hCameraThread1);
	while (!m_cVideo.bRun) {
		Sleep(10);
	}
	m_cVideo.SetExposure(Global.Exposure[m_cVideo.iCID]);
	return TRUE;
}

BOOL C413AppCam::StartCameraThread2(CCamBase::CAMERAID No, CRecipe413* pRcp, CSICam& m_cVideo) {
	if (m_cVideo.hLiveThread) {
		// Already started, simply return [3/18/2011 XPMUser]
		return TRUE;
	}
	if (hCameraThread2) {
		// Someone else is using the camera [7/23/2013 Yuen]
		ASSERT(0);
		return FALSE;
	}

	m_cVideo.SetRecipe(pRcp);
	m_cVideo.bRun = FALSE;
	m_cVideo.iCID = No;
	m_cVideo.iCNum = Global.CamraNum[No];
	hCameraThread2 = ::CreateThread(NULL, 0, m_cVideo.LiveThreadFunc, &m_cVideo, CREATE_SUSPENDED, NULL);
	if (!hCameraThread2) {
		return FALSE;
	}
	m_cVideo.iCID = No;
	m_cVideo.hLiveThread = hCameraThread2;
	pVideo2 = &m_cVideo;
	::ResumeThread(hCameraThread2);
	while (!m_cVideo.bRun) {
		Sleep(10);
	}
	m_cVideo.SetExposure(Global.Exposure[m_cVideo.iCID]);
	return TRUE;
}

void C413AppCam::CameraSetting(CSICam& m_cVideo) {
	CSiCamSettingDlg dlg;

	if (m_cVideo.iCID < CCamBase::ECHOCAM || m_cVideo.iCID > CCamBase::THINFCAM) {
		ASSERT(0);
		return;
	}

	C413Global* g = &Global;
	dlg.nTime = g->Exposure[m_cVideo.iCID];

	if (dlg.DoModal() == IDOK) {
		CRecipe413* pRcp = &g->RcpSetup;
		if (!dlg.bAuto) {
			g->Exposure[m_cVideo.iCID] = dlg.nTime;
			m_cVideo.SetExposure(dlg.nTime);
		}
		else {
			if (m_cVideo.iCID == CCamBase::ECHOCAM) {
				m_cVideo.Level = dlg.m_TargetExposure;
				m_cVideo.AutoAdjust();
			}
			else if (m_cVideo.iCID == CCamBase::THINFCAM) {
				m_cVideo.Level = dlg.m_TargetExposure2;
				m_cVideo.AutoAdjust();
			}
		}
		if (dlg.bFocus) {
			AutoFocusVideo(m_cVideo.iCID, pRcp->fTgtGRnX[g->CurPaSet - 1][0], pRcp->fTgtGRnY[g->CurPaSet - 1][0], &m_cVideo);
		}
		g->SaveVideoExposure();
	}
}

// Copy a portion of Global.StoreDIB into MatchDIB [11/8/2012 Yuen]
void C413AppCam::StoreDIB2(CPackedDIB& StoreDIB, CPackedDIB& MatchDIB, float fTargetX, float fTargetY, int ww, int hh) {
	if (!StoreDIB.pDIB) {
		return;
	}
	CSingleLock Lock(&StoreDIB.CS);
	Lock.Lock();

	int w = StoreDIB.pDIB->BmpInfo.biWidth;
	int h = abs(StoreDIB.pDIB->BmpInfo.biHeight);
	short BPP = StoreDIB.pDIB->BmpInfo.biBitCount / 8;
	long bpr = w * BPP;
	bpr += (4 - bpr % 4) % 4;

	int sx = w * fTargetX - ww / 2;
	int sy = (h * fTargetY) - hh / 2;

	// Sanity check
	if ((sx + ww) >= w) {
		sx = w - ww - 1;
	}
	if ((sy + hh) >= h) {
		sy = h - hh - 1;
	}

	long bpr2 = ww * BPP;
	bpr2 += (4 - bpr2 % 4) % 4;
	BYTE* p = new BYTE[bpr2 * hh + bpr2];
	BYTE* q = (BYTE*)StoreDIB.pDIB->pDIBBits;

	BYTE* pp = p;
	for (int y = sy; y < sy + hh; y++) {
		BYTE* qq = q + (y * bpr + BPP * sx);
		memcpy(pp, qq, bpr2);
		pp += bpr2;
	}

	CPackedDIB* pM = &MatchDIB;
	pM->CreateBlank();
	pM->pDIB->pDIBBits = p;
	pM->pDIB->BmpInfo = StoreDIB.pDIB->BmpInfo;
	//pM->pDIB->BmpInfo = StoreDIB.pDIB->BmpInfo;
	pM->pDIB->BmpInfo.biWidth = ww;
	pM->pDIB->BmpInfo.biHeight = hh * sign(StoreDIB.pDIB->BmpInfo.biHeight);
	pM->pDIB->BmpInfo.biSizeImage = bpr2 * abs(hh);
	pM->CalcBytePerRow();

	Lock.Unlock();
}

// Store a portion of the image specified by Width and Height to MatchDIB [7/4/2013 Yuen]
// Extract center portion unless XOff and YOff is specified [7/4/2013 Yuen]
void C413AppCam::StoreDib1(CPackedDIB& StoreDIB, CPackedDIB& MatchDIB, int Width, int Height, float fTargetX, float fTargetY, short XOff, short YOff) {
	if (!StoreDIB.pDIB) {
		return;
	}
	CSingleLock Lock(&StoreDIB.CS);
	Lock.Lock();

	int w = StoreDIB.pDIB->BmpInfo.biWidth;
	int h = abs(StoreDIB.pDIB->BmpInfo.biHeight);
	short BPP = StoreDIB.pDIB->BmpInfo.biBitCount / 8;
	long bpr = w * BPP;
	bpr += (4 - bpr % 4) % 4;

	int ww = Width;
	int hh = Height;

	// sx and sy is converted to BMP space [9/13/2011 Administrator]
	int sx = w * fTargetX - ww / 2 + XOff;
	int sy = (h * fTargetY) - hh / 2 + YOff;
	if (sx < 0) {
		sx = 0;
	}
	if (sy < 0) {
		sy = 0;
	}
	if ((sx + ww) >= w) {
		sx = w - ww - 1;
	}
	if ((sy + hh) >= h) {
		sy = h - hh - 1;
	}
	long bpr2 = ww * BPP;
	bpr2 += (4 - bpr2 % 4) % 4;
	BYTE* p = new BYTE[bpr2 * hh + bpr2];
	BYTE* q = (BYTE*)StoreDIB.pDIB->pDIBBits;

	BYTE* pp = p;
	for (int y = sy; y < sy + hh; y++) {
		BYTE* qq = q + (y * bpr + BPP * sx);
		memcpy(pp, qq, bpr2);
		pp += bpr2;
	}

	CPackedDIB* pM = &MatchDIB;
	pM->CreateBlank();
	pM->pDIB->pDIBBits = p;
	pM->pDIB->BmpInfo = StoreDIB.pDIB->BmpInfo;
	pM->pDIB->BmpInfo.biSize = sizeof(BITMAPINFOHEADER);
	pM->pDIB->BmpInfo.biWidth = ww;
	pM->pDIB->BmpInfo.biHeight = hh * sign(StoreDIB.pDIB->BmpInfo.biHeight);
	pM->pDIB->BmpInfo.biSizeImage = bpr2 * hh;
	pM->CalcBytePerRow();

	Lock.Unlock();
}

// Take full res picture with no overlays
void C413AppCam::TakePicture(CSICam* pVideo, CString name) {
	if (pVideo->SnapOneFrame()) {
		CSingleLock Lock(&Global.StoredDIB.CS);
		Lock.Lock();
		CString v, str;
		v = CTime::GetCurrentTime().Format("%y%m%d-%H%M%S");
		str.Format("C:\\TEMP\\%s\\%s.BMP", v, name);
		Global.StoredDIB.Save(str);
		Lock.Unlock();
	}
}

void C413AppCam::PreMatchingTreatment(CCamBase::CAMERAID ID, CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CListCtrl* pList) {
	if (!pRcp) {
		ASSERT(0);
		return;
	}
	CString str;
	short AutoExpose = pRcp->AutoExposeEcho;
	if (ID == CCamBase::THINFCAM) {
		AutoExpose = pRcp->AutoExposeTF;
	}
	else {
		AutoExpose = pRcp->AutoExposeEcho;
	}
	if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::EXPB4FOCUS) {
		str.Format("AE B4F: %d", AutoExpose);
		Message(pList, str);
		pVideo->Level = AutoExpose;
		pVideo->AutoAdjust();
	}
	if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::FOCB4MATCH) {
		if (!(pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::EXPB4FOCUS)) {
			str.Format("AE B4F: %d", AutoExpose);
			Message(pList, str);
			pVideo->Level = AutoExpose;
			pVideo->AutoAdjust();
		}

		str = "AF B4M";
		Message(pList, str);
		AutoFocusVideo(CCamBase::THINFCAM, pRcp->fTgtGRnX[p->PatSet - 1][0], pRcp->fTgtGRnY[p->PatSet - 1][0], pVideo, 0, 0);
	}
	if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::EXPAFOCUS) {
		str.Format("AE AFF: %d", AutoExpose);
		Message(pList, str);
		pVideo->Level = AutoExpose;
		pVideo->AutoAdjust();
	}
}

void C413AppCam::PostMatchingTreatment(CCamBase::CAMERAID ID, CRecipe413* pRcp, CMPoint* p, CSICam* pVideo, CListCtrl* pList) {
	CString str;
	short AutoExpose = pRcp->AutoExposeEcho;
	if (ID == CCamBase::THINFCAM) {
		AutoExpose = pRcp->AutoExposeTF;
	}
	if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::FOCAMATCH) {
		str = "AF FAM";
		Message(pList, str);
		AutoFocusVideo(CCamBase::THINFCAM, pRcp->fTgtGRnX[p->PatSet - 1][0], pRcp->fTgtGRnY[p->PatSet - 1][0], pVideo, 0, 0);
	}
	if (pRcp->PatMatOpt[p->PatSet - 1] & CRecipe413::EXPAMATCH) {
		str.Format("AE EAM: %d", AutoExpose);
		Message(pList, str);
		pVideo->Level = AutoExpose;
		pVideo->AutoAdjust();
	}
}
//20130828-1 SCOTT ADD -------------------------------------
BOOL C413AppCam::isEdge(SFINDEDGES& Par) {
	if ((Par.dist2 == 0) || fabs(Par.dist1 - Par.dist2) > 0.045) {
		if (fabs(Par.dist1) > 0.125f)
			return FALSE;
	}

	if (fabs(Par.dist1) > 0.30f) {
		return FALSE;
	}
	return TRUE;
}
BOOL C413AppCam::refindEdges(SFINDEDGES& Par, float& fDistMin) {
	//char tch[255];
	//OutputDebugString("??? refindEdges() Step1");
	Par.method = CImgWnd::DARKBWD;
	FindEdges(Par);
	if (isEdge(Par)) {
		//sprintf(tch,"??? dist1 = %.2f dist2 = %.2f", Par.dist1, Par.dist2);
		//OutputDebugString(tch);
		return TRUE;
	}
	fDistMin = fabs(Par.dist1);

	//OutputDebugString("??? refindEdges() Step2");
	Par.method = CImgWnd::BRITBWD;
	FindEdges(Par);
	if (isEdge(Par)) {
		//sprintf(tch,"??? dist1 = %.2f dist2 = %.2f", Par.dist1, Par.dist2);
		//OutputDebugString(tch);
		return TRUE;
	}
	if (fDistMin > fabs(Par.dist1))
		fDistMin = fabs(Par.dist1);

	//OutputDebugString("??? refindEdges() Step3");
	Par.method = CImgWnd::BRITFWD;
	FindEdges(Par);
	if (isEdge(Par)) {
		//sprintf(tch,"??? dist1 = %.2f dist2 = %.2f", Par.dist1, Par.dist2);
		//OutputDebugString(tch);
		return TRUE;
	}
	if (fDistMin > fabs(Par.dist1))
		fDistMin = fabs(Par.dist1);

	//OutputDebugString("??? refindEdges() Step4");
	Par.method = CImgWnd::DARKFWD;
	FindEdges(Par);
	if (isEdge(Par)) {
		//sprintf(tch,"??? dist1 = %.2f dist2 = %.2f", Par.dist1, Par.dist2);
		//OutputDebugString(tch);
		return TRUE;
	}
	if (fDistMin > fabs(Par.dist1))
		fDistMin = fabs(Par.dist1);
	//OutputDebugString("??? refindEdges() Step Out");
	return FALSE;
}
//---------------------------------------------------

BOOL C413AppCam::FindEdges(SFINDEDGES& Par) {
	if (!Par.pVideo || !Par.pRcp || !Par.pImgWnd || !pMtr) {
		return FALSE;
	}
	// 	if (Par.pVideo->IsSimu() && (Par.name.GetLength() > 0))
	// 	{
	Par.pVideo->LoadSimuDIB(Par.name);
	// 	}

	CString msg;
	C413Global* g = &Global;

	float x, y;
	g->Polar2Cartesian(Par.radius, Par.angle, x, y); // Must not place inside if loop [7/25/2013 Yuen]

	MsgStr.Format("Set exposure %d", Par.Exposure);
	Message(Par.pList, MsgStr);

	Par.pVideo->SetExposure(Par.Exposure);
	if (Par.AutoExpose > 0) {
		if (Par.bMoveStage) {
			MsgStr = "Execute AutoExpose";
			Message(Par.pList, MsgStr);

			float x1 = x; float y1 = y;
			g->Polar2Cartesian(Par.radius - 0.7f, Par.angle, x1, y1);
			MsgStr.Format("GotoXYZ: %.3f,%.3f,%.3f", x1, y1, Par.ZPosition);
			Message(Par.pList, MsgStr);
			if (!pMtr->GotoXYZ1(x1, y1, Par.ZPosition)) {
				msg = "GotoXYZ1 error";
				Message(Par.pList, msg);
				return FALSE;
			}
		}
		Par.pVideo->Level = Par.AutoExpose;
		Par.pVideo->AutoAdjust();
	}

	if (Par.bMoveStage) {
		MsgStr.Format("GotoXYZ: %.3f,%.3f,%.3f", x, y, Par.ZPosition);
		Message(Par.pList, MsgStr);
		if (!pMtr->GotoXYZ1(x, y, Par.ZPosition)) {
			msg = "GotoXYZ1 error";
			Message(Par.pList, msg);
			return FALSE;
		}
	}

	//Par.pVideo->SnapOneFrame();
	Par.pVideo->DoubleSnapOneFrame();
	if (Par.bContrastStrecth) {
		MsgStr = "Contrast streatching color";
		Message(Par.pList, MsgStr);
		g->StoredDIB.ContrastStretchColor(Par.Cmax, Par.Cmin);	// Make this an option selectable from parameter [6/13/2013 FSM413]
	}
	Par.pImgWnd->DupStore1(g->StoredDIB);
	Par.pImgWnd->Refresh();

	float sfx = g->ScrnCalibECH.ScrnX / float(Par.pImgWnd->pPackedDIB1->IWidth);
	float sfy = g->ScrnCalibECH.ScrnY / float(Par.pImgWnd->pPackedDIB1->IHeight);

	// Find Edge here [4/14/2013 Administrator]
	SLnX L1, L2;
	int In, Out;

	if (Par.bFindBestAngle) {
		MsgStr = "Find best angle";
		Message(Par.pList, MsgStr);
		Par.angle = FindBestAngle(Par.angle, Par.pImgWnd);
	}

	MsgStr = "Find edge";
	Message(Par.pList, MsgStr);
	Par.score = Par.pImgWnd->FindEdge(&L1, &L2, In, Out, Par.angle, Par.method, Par.Thres);
	Par.In = In;
	Par.Out = Out;

	if (!Par.score) {
		msg = "FindEdge error";
		Message(Par.pList, msg);
		return FALSE;
	}

	float Xa, Ya;
	if (L1.angle) {
		g->Polar2Cartesian(L1.offset, Par.angle, x, y);
		Xa = x * sfx / g->ScrnCalibECH.ppmX * 1e-3;
		Ya = y * sfy / g->ScrnCalibECH.ppmY * 1e-3;

		// First edge found [6/7/2013 Yuen]
		Par.dist1 = sqrt(Xa * Xa + Ya * Ya) * sign(L1.offset);
	}
	else {
		Par.dist1 = 0;
	}

	if (L2.angle) {
		g->Polar2Cartesian(L2.offset, Par.angle, x, y);
		Xa = x * sfx / g->ScrnCalibECH.ppmX * 1e-3;
		Ya = y * sfy / g->ScrnCalibECH.ppmY * 1e-3;

		// Second edge found [6/7/2013 Yuen]
		Par.dist2 = sqrt(Xa * Xa + Ya * Ya) * sign(L2.offset);
	}
	else {
		Par.dist2 = 0;
	}

	Par.pImgWnd->nTWEdge1 = L1.offset;
	Par.pImgWnd->fTWEdge1 = Par.dist1;
	Par.pImgWnd->nTWEdge2 = L2.offset;
	Par.pImgWnd->fTWEdge2 = Par.dist2;
	Par.pImgWnd->TWRadius = Par.radius - Par.dist1;
	Par.pImgWnd->TWAngle = Par.angle;
	Par.pImgWnd->Refresh();

	MsgStr.Format("L1:%.3f  D1:%.3f  L2:%.3f  D2:%.3f  R:%.3f  A:%.3f", L1.offset, Par.dist1, L2.offset, Par.dist2, Par.radius - Par.dist1, Par.angle);
	Message(Par.pList, MsgStr);

	if (!Out) {
		Out = 1;
	}
	if (Par.bTest) {
		if (In / float(Out) < 3.0f) {
			msg = "Measurement error";
			Message(Par.pList, msg);
			return FALSE;
		}
		if (abs(In - Out) < 128) {
			msg = "Measurement error";
			Message(Par.pList, msg);
			return FALSE;
		}
	}
	return TRUE;
}

#define SQRE(x) ((x)*(x))

float C413AppCam::FindBestAngle(float angle, CImgWnd* pImgWnd) {
	// @XXX [6/13/2013 FSM413]
	// Need null pointer check [6/13/2013 FSM413]
	if (!pImgWnd || !pImgWnd->pPackedDIB1 || !pImgWnd->pPackedDIB1->pDIB) {
		return 0;
	}
	int x2 = SQRE(pImgWnd->pPackedDIB1->pDIB->BmpInfo.biWidth);
	int y2 = SQRE(pImgWnd->pPackedDIB1->pDIB->BmpInfo.biHeight);
	int siz = int(sqrt(x2 + y2) * (pImgWnd->pPackedDIB1->pDIB->BmpInfo.biBitCount / 8) + 16);
	short* pBuf = new short[siz];  // Must obtain new allocation here [8/21/2010 C2C]
	short nL = short(sqrt(y2 + x2) * 0.20);

	float min = 9e30f;
	float max = -9e30f;
	int x = 64;
	float sf = 0.5f;
	float ang = angle;
	float* pVal = new float[2 * x + 8];

	float* pV = pVal;
	for (int i = -x; i <= x; i++) {
		pImgWnd->FindEdgeMax(pV, angle + i * sf, siz, pBuf, nL);
		if (*pV < min) {
			min = *pV;
		}
		if (*pV > max) {
			max = *pV;
		}
		pV++;
	}
	float da = (max - min) * 0.0075f;
	min += da;
	max -= da;
	da = pImgWnd->FindAngle(pVal, x * 2 + 1, max, min, nL, sf);
	ang = angle + da;
	if (0) {
		CString name;
		name.Format("C:\\TEMP\\FBE%.3f.CSV", angle);
		FILE* fp = NULL;
		fp = fopen(name, "wb");
		if (fp) {
			pV = pVal;
			for (int i = -x; i <= x; i++) {
				fprintf(fp, "%.3f,%.3f\n", i * sf, *pV); pV++;
			}
			fprintf(fp, "max=%.3f, min=%.3f, angle=%.3f,  da=%.3f", max, min, angle, da);
			fclose(fp);
		}
	}

	if (pVal) delete[] pVal;
	if (pBuf) delete[] pBuf;

	return ang;
}

void C413AppCam::SwitchCameraSetExposure(CCamBase::CAMERAID  view, int Exposure, CSICam* pVideo, CListCtrl* pList, CRecipe413* pRcp) {
	C413Global* g = &Global;
	CString msg;
	switch (view) {
	default:	// Echo camera [5/14/2013 user]
		if (pVideo->hLiveThread) {
			if (pVideo->iCID != CCamBase::ECHOCAM) {
				StopCamera(pVideo, pList);
			}
		}
		if (pVideo->iCID != CCamBase::ECHOCAM) {
			msg = "Start ECHO camera";
			Message(pList, msg);
			StartCamera(pVideo, CCamBase::ECHOCAM, pList, pRcp);
		}
		msg.Format("SwitchCameraSetExposure: ECHO, exposure %d", Exposure);
		pVideo->SetExposure(Exposure);
		Message(pList, msg);
		break;
	case 1: // TF camera [5/14/2013 user]
		if (pVideo->hLiveThread) {
			if (pVideo->iCID != CCamBase::THINFCAM) {
				StopCamera(pVideo, pList);
			}
		}
		if (pVideo->iCID != CCamBase::THINFCAM) {
			msg = "Start TF camera";
			Message(pList, msg);
			StartCamera(pVideo, CCamBase::THINFCAM, pList, pRcp);
		}
		msg.Format("SwitchCameraSetExposure: THINF, exposure %d", Exposure);
		pVideo->SetExposure(Exposure);
		Message(pList, msg);
		break;
	}
}

BOOL C413AppCam::StartCamera(CSICam* pVideo, CCamBase::CAMERAID No, CListCtrl* pList, CRecipe413* pRcp) {
	if (!pRcp) {
		return FALSE;
	}
	CString msg;
	if (StartCameraThread(No, pRcp, *pVideo)) {
		switch (No) {
		default:
			msg = "Echo camera started";
			pRcp->startTime = CTime::GetCurrentTime(); // 20251014_Mohir
			Message(pList, msg);
			break;
		case CCamBase::THINFCAM:
			msg = "Thin film camera started";
			Message(pList, msg);
			break;
		}

		// Make sure that the lights are on [1/20/2013 Yuen]
		if (pMtr) {
			switch (No) {
			default:
				msg = "Switch on ECHO illumination";
				Message(pList, msg);
				pMtr->SetMicroscopeLight(TRUE);
				pMtr->SetSpectroIllumLight(FALSE);
				break;
			case CCamBase::THINFCAM:
				msg = "Switch on TF illumination";
				Message(pList, msg);
				pMtr->SetSpectroIllumLight(TRUE);
				pMtr->SetMicroscopeLight(FALSE);
				break;
			}
		}
		Global.ActiveCamera = No;
		pVideo->XX = No;
		return TRUE;
	}
	else {
		msg = "Camera start fail";
		Message(pList, msg);
	}
	return FALSE;
}

void C413AppCam::StopCamera(CSICam* pVideo, CListCtrl* pList) {
	CString msg;
	switch (pVideo->iCID) {
	default:
		msg = "Stop Echo camera";
		Message(pList, msg);
		break;
	case CCamBase::THINFCAM:
		msg = "Stop Thin Film camera";
		Message(pList, msg);
		break;
	}
	if (StopCameraThread(*pVideo)) {
		// Make sure that lights are off after measurement [1/20/2013 Yuen]
		if (pMtr) {
			switch (pVideo->iCID) {
			default:
				msg = "Switch off ECHO illumination";
				Message(pList, msg);
				pMtr->SetMicroscopeLight(FALSE);
				break;
			case CCamBase::THINFCAM:
				msg = "Switch off TF illumination";
				Message(pList, msg);
				pMtr->SetSpectroIllumLight(FALSE);
				break;
			}
		}
		//Global.ActiveCamera = CCamBase::UNKNOWNCAM;
	}
}