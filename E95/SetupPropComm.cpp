// SetupPropComm.cpp: implementation of the CSetupPropComm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SetupPropComm.h"
#include "413App.h"
#include "SICam.h"
#include "Coor.h"
#include "MPoint.h"
#include "ChuckMask.h"
#include <math.h>
#include "GlobalUIContainer.h"
#include "InfoPanelSetup.h"
#include "SetupProp3Dlg.h"
#include "GlobalDeviceContainer.h"
#include "..\413\MotorS2.h"
#include "MotionControl.h"
#include "..\413\DMCMotorBase.h"
#include "SelectMeasurementDlg.h"
#include "RecipeIO.h"

#include "DrLockMgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSetupPropComm::CSetupPropComm(CWnd* pParent) {
	m_hLiveThread1 = NULL;
	m_hLiveThread2 = NULL;
	m_Parent = NULL;

	g = NULL;
	pRcp = NULL;
	hWndPC = NULL;
	m_pVideo = NULL;

	nPatSSet = 0;
}

CSetupPropComm::~CSetupPropComm() {}

// void CSetupPropComm::StartCamera(CCamBase::CAMERAID No, CSICam *pVideo)
// {
// 	if (!g || !pRcp || !pVideo)
// 	{
// 		return;
// 	}
// 	if (No == CCamBase::SIMULCAM)
// 	{
// 		No = CCamBase::ECHOCAM;
// 	}
// 	if (p413App->StartCameraThread(No,pRcp,*pVideo))
// 	{
// 		// Must switch on microscope light just in case [1/20/2013 Yuen]
// 		if (p413App->pMtr)
// 		{
// 			switch (No)
// 			{
// 			default:
// 				p413App->pMtr->SetMicroscopeLight(TRUE);
// 				break;
// 			case CCamBase::THINFCAM:
// 				p413App->pMtr->SetSpectroIllumLight(TRUE);
// 				break;
// 			}
// 		}
// 		g->ActiveCamera = No;
// 		pVideo->XX = No;
// 		m_pVideo = pVideo;
// 	}
// }

void CSetupPropComm::StartCamera2(CCamBase::CAMERAID No, CSICam* pVideo) {
	if (!g || !pRcp || !pVideo) {
		return;
	}
	if (p413App->StartCameraThread2(No, pRcp, *pVideo)) {
		// Must switch on microscope light just in case [1/20/2013 Yuen]
		pVideo->XX = No;
	}
}

// void CSetupPropComm::StopCamera(CSICam *pVideo)
// {
// 	if (!g || !pVideo)
// 	{
// 		return ;
// 	}
// 	if (p413App->StopCameraThread(*pVideo))
// 	{
// 		pVideo->RSpecClear();
// 		pVideo->pWnd = NULL;
// 		pVideo = NULL;
// 		g->ActiveCamera = CCamBase::SIMULCAM;
// 	}
// }

void CSetupPropComm::StopCamera2(CSICam* pVideo) {
	if (!g || !pVideo) {
		return;
	}
	if (p413App->StopCameraThread2(*pVideo)) {
		pVideo->RSpecClear();
		pVideo->pWnd = NULL;
		pVideo = NULL;
	}
}

void CSetupPropComm::StopAllCamera(CSICam* pVideo) {
	if (pVideo) {
		p413App->StopCamera(pVideo, NULL);
		CMotorS2* pMtr = p413App->pMtr;
		if (pMtr) {
			pMtr->SetMicroscopeLight(TRUE);
			pMtr->SetSpectroIllumLight(TRUE);
		}
	}
}

void CSetupPropComm::ShowMotionControl(CWnd* pWnd) {
	CMotionControl* pMCtrl = p413App->pMCtrl;
	pGDev->PauseDoorMonitor();
	if (pMCtrl) {
		pMCtrl->pWnd = pWnd;
		pMCtrl->ShowWindow(SW_SHOW);
	}
}

BOOL CSetupPropComm::UpdatePointB(CCoor* p) {
	float Distance;
	CMotorS2* pMtr = p413App->pMtr;
	if (!g || !pRcp || !pMtr) {
		return FALSE;
	}
	CPattern* pPat = &pRcp->Patt;

	CMPoint* mp = pPat->FindNearestMP(*p, Distance);
	if (mp && (Distance < 5.0f)) {
		pMtr->GetXYPos(&mp->Co.x, &mp->Co.y);
		float ZPos;
		switch (g->ActiveCamera) {
		default:
			pMtr->GetEchoTopZPosDev(ZPos);
			mp->Co.z = ZPos - pRcp->fEchoTopZ;
			break;
		case CCamBase::THINFCAM:
			pMtr->GetThinFilmZPosDev(ZPos);
			mp->Co.z = ZPos - pRcp->fThinFilmZ;
			break;
		// [05082024 ZHIMING
		case CCamBase::CWL:
			pMtr->GetCWLTopZPosDev(ZPos);
			mp->Co.z = ZPos - pRcp->fCWLTopZ;
			break;
		case CCamBase::ROUGH:
			pMtr->GetRoughZPosDev(ZPos);
			mp->Co.z = ZPos - pRcp->fRoughZ;
			break;
		// ]
		}
		return TRUE;
	}
	return TRUE;
}

void CSetupPropComm::GotoPointB(CSICam* pVideo, CCoor* p, BOOL bZ) {
	if (!g || !pRcp) {
		return;
	}
	float Distance;
	CPattern* pPat = &pRcp->Patt;

	p413App->MouseWait(hWndPC);

	CMPoint* mp = pPat->FindNearestMP(*p, Distance);
	if (mp && (Distance < 5.0f)) {
		CMotorS2* pMtr = p413App->pMtr;
		switch (g->ActiveCamera) {
		default:
			if (pMtr) {
				if (bZ) pMtr->MoveEchoTopZ(pRcp->fEchoTopZ + mp->Co.z);
				pMtr->GotoXY(mp->Co.x, mp->Co.y);
				if (bZ) pMtr->WaitEchoTopZ();
			}
			if (pVideo) pVideo->Index = mp->Co.n;
			break;
		case CCamBase::THINFCAM:
			if (pMtr) {
				if (bZ) pMtr->MoveThinFilmZ(pRcp->fThinFilmZ + mp->Co.z);
				pMtr->GotoXY(mp->Co.x, mp->Co.y);
				if (bZ) pMtr->WaitThinFilmZ();
			}
			if (pVideo) pVideo->Index = mp->Co.n;
			break;
		// [05082024 ZHIMING
		case CCamBase::CWL:
			if (pMtr) {
				if (bZ) pMtr->MoveCWLTopZ(pRcp->fCWLTopZ + mp->Co.z);
				pMtr->GotoXY(mp->Co.x, mp->Co.y);
				if (bZ) pMtr->WaitCWLTopZ();
			}
			if (pVideo) pVideo->Index = mp->Co.n;
			break;
		case CCamBase::ROUGH:
			if (pMtr) {
				if (bZ) pMtr->MoveRoughZ(pRcp->fRoughZ + mp->Co.z);
				pMtr->GotoXY(mp->Co.x, mp->Co.y);
				if (bZ) pMtr->WaitRoughZ();
			}
			if (pVideo) pVideo->Index = mp->Co.n;
			break;
		// ]
		}
	}
	p413App->MouseNormal(hWndPC);
}

CMPoint* CSetupPropComm::FindMPoint(short nSel) {
	if (!pRcp) {
		return NULL;
	}
	CMPList* pList = (CMPList*)&pRcp->Patt.MP;
	if (pList) {
		return pList->Get(nSel);
	}
	return NULL;
}

BOOL CSetupPropComm::AddZPositionB(CMPoint* pMP) {
	CMotorS2* pMtr = p413App->pMtr;

	if (!g || !pRcp || !pMP || !pMtr) {
		return FALSE;
	}

	float zPos;
	switch (g->ActiveCamera) {
	default:
		if (pRcp->fEchoTopZ == 0) {
			AfxMessageBox("ZPos is zero");
		}
		pMtr->GetEchoTopZPosDev(zPos);
		pMP->Co.z = zPos - pRcp->fEchoTopZ;
		break;
	case CCamBase::THINFCAM:
		if (pRcp->fThinFilmZ == 0) {
			AfxMessageBox("ZPos is zero");
		}
		pMtr->GetThinFilmZPosDev(zPos);
		pMP->Co.z = zPos - pRcp->fThinFilmZ;
		break;
	// [05082024 ZHIMING
	case CCamBase::CWL:
		if (pRcp->fCWLTopZ == 0) {
			AfxMessageBox("CWL ZPos is zero");
		}
		pMtr->GetCWLTopZPosDev(zPos);
		pMP->Co.z = zPos - pRcp->fCWLTopZ;
		break;
	case CCamBase::ROUGH:
		if (pRcp->fRoughZ == 0) {
			AfxMessageBox("Rough ZPos is zero");
		}
		pMtr->GetRoughZPosDev(zPos);
		pMP->Co.z = zPos - pRcp->fRoughZ;
		break;
	// ]
	}
	return TRUE;
}

BOOL CSetupPropComm::MarkThisPoint() {
	CMotorS2* pMtr = p413App->pMtr;
	if (!g || !pRcp || !pMtr) {
		return FALSE;
	}

	float x, y, zPos;
	CPattern* pPatt = &pRcp->Patt;
	if (pPatt) {
		pMtr->GetXYPos(&x, &y);
		switch (g->ActiveCamera) {
		default:
			pMtr->GetEchoTopZPosDev(zPos);
			pPatt->AddMPoint(x, y, zPos - pRcp->fEchoTopZ);
			return TRUE;
			break;
		case CCamBase::THINFCAM:
			pMtr->GetThinFilmZPosDev(zPos);
			pPatt->AddMPoint(x, y, zPos - pRcp->fThinFilmZ);
			return TRUE;
			break;
		// [05082024 ZHIMING
		case CCamBase::CWL:
			pMtr->GetCWLTopZPosDev(zPos);
			pPatt->AddMPoint(x, y, zPos - pRcp->fCWLTopZ);
			return TRUE;
			break;
		case CCamBase::ROUGH:
			pMtr->GetRoughZPosDev(zPos);
			pPatt->AddMPoint(x, y, zPos - pRcp->fRoughZ);
			return TRUE;
			break;
		// ]
		}
	}
	return FALSE;
}

BOOL CSetupPropComm::RelocateToBackB(CCoor* p) {
	if (!pRcp) {
		return FALSE;
	}
	float Distance;
	CPattern* pPat = &pRcp->Patt;

	CMPoint* mp = pPat->FindNearestMP(*p, Distance);
	if (mp && Distance < 5.0f) {
		CMPoint* pMPoint = new CMPoint;
		if (pMPoint) {
			pMPoint->SetCoor(&mp->Co);
			pMPoint->MeSet = mp->MeSet;
			pMPoint->PatSet = mp->PatSet;
			pPat->MP.AddTailPoint(pMPoint);
			pPat->DelPoint(mp->Co);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSetupPropComm::RelocateToTopB(CCoor* p) {
	if (!pRcp) {
		return FALSE;
	}
	float Distance;
	CPattern* pPat = &pRcp->Patt;

	CMPoint* mp = pPat->FindNearestMP(*p, Distance);
	if (mp && Distance < 5.0f) {
		CMPoint* pMPoint = new CMPoint;
		if (pMPoint) {
			pMPoint->SetCoor(&mp->Co);
			pMPoint->MeSet = mp->MeSet;
			pMPoint->PatSet = mp->PatSet;
			pPat->DelPoint(mp->Co);
			pPat->MP.AddHeadPoint(pMPoint);
			return TRUE;
		}
	}
	return FALSE;
}

void CSetupPropComm::MPointApplyAll(CMPoint* pMP) {
	if (!pRcp) {
		return;
	}
	CPtrList* pMPList = (CPtrList*)&pRcp->Patt.MP.MPLst;

	if (!pMPList) {
		return;
	}

	// Getting ready to measure [7/12/2010 128 NT]
	POSITION pos = pMPList->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
		if (p) {
			p->MeSet = pMP->MeSet;
			p->PatSet = pMP->PatSet;
			p->m_fvalOffset = pMP->m_fvalOffset;	// added value offset [10/6/2020]
		}
	}
}

BOOL CSetupPropComm::DeletePoint(short nSel) {
	if (!pRcp) {
		return FALSE;
	}
	CMPList* pList = (CMPList*)&pRcp->Patt.MP;
	if (pList) {
		return pList->DeletePoint(nSel);
	}
	return NULL;
}

BOOL CSetupPropComm::AddAlgnPoint(CCoor* co) {
	float Distance;
	if (!g || !pRcp) {
		return FALSE;
	}
	CPattern* pPat = &pRcp->Patt;

	CMPoint* mp = pPat->FindNearestMP(*co, Distance);
	if (mp && Distance < 0.005f) {
	}
	else {
		if (g->GetChuckMask()->GetCenter(co->x, co->y)) {
			CMPoint* pMPoint = new CMPoint;
			pMPoint->SetCoor(co);
			pPat->MP.AddTailPoint(pMPoint);
			// Need to link in Me & Pa [8/10/2010 XPMUser]
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSetupPropComm::AddPoint(CCoor* co) {
	float Distance;
	if (!pRcp) {
		return FALSE;
	}
	CPattern* pPat = &pRcp->Patt;

	CMPoint* mp = pPat->FindNearestMP(*co, Distance);
	if (mp && Distance < 0.005f) {
	}
	else {
		CMPoint* pMPoint = new CMPoint;
		pMPoint->SetCoor(co);
		pPat->MP.AddTailPoint(pMPoint);
		// Need to link in Me & Pa [8/10/2010 XPMUser]
		return TRUE;
	}
	return FALSE;
}

BOOL CSetupPropComm::EditPoint(CCoor* co) {
	float Distance;
	if (!pRcp) {
		return FALSE;
	}
	CPattern* pPat = &pRcp->Patt;

	CMPoint* mp = pPat->FindNearestMP(*co, Distance);
	if (mp && (Distance < 5.0f)) {
		if (pPat->EditMP(mp->Co, mp->Co.x, mp->Co.y)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSetupPropComm::DeletePoint(CCoor* co) {
	float Distance;
	if (!pRcp) {
		return FALSE;
	}
	CPattern* pPat = &pRcp->Patt;

	CMPoint* mp = pPat->FindNearestMP(*co, Distance);
	if (mp && (Distance < 5.0f)) {
		pPat->DelPoint(mp->Co);
		return TRUE;
	}
	return FALSE;
}

BOOL CSetupPropComm::ReverseZ() {
	if (!pRcp) {
		return FALSE;
	}
	CMPList* pList = (CMPList*)&pRcp->Patt.MP;
	if (pList) {
		for (int i = 0; i < pList->GetCount(); i++) {
			CMPoint* pt = pList->Get(i);
			if (pt) {
				pt->Co.z *= -1;
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSetupPropComm::ZeroZ(CRecipe413* pRcp) {
	if (!pRcp) {
		return FALSE;
	}
	CMPList* pList = (CMPList*)&pRcp->Patt.MP;
	if (pList) {
		for (int i = 0; i < pList->GetCount(); i++) {
			CMPoint* pt = pList->Get(i);
			if (pt) {
				pt->Co.z = 0;
			}
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CSetupPropComm::OffsetZ(float cz) {
	if (!pRcp) {
		return FALSE;
	}
	CMPList* pList = (CMPList*)&pRcp->Patt.MP;
	if (pList) {
		float off = 0;
		CMPoint* pt;
		pt = pList->Get(0);
		if (pt) {
			off = cz - pt->Co.z;
			for (int i = 0; i < pList->GetCount(); i++) {
				CMPoint* pt1 = pList->Get(i);
				if (pt1) {
					pt1->Co.z += off;
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSetupPropComm::OffsetXY(float cx, float cy) {
	if (!pRcp) {
		return FALSE;
	}

	CMPList* pList = (CMPList*)&pRcp->Patt.MP;
	if (pList) {
		float OffX, OffY;
		CMPoint* pt;
		pt = pList->Get(0);
		if (pt) {
			OffX = cx - pt->Co.x;
			OffY = cy - pt->Co.y;
			for (int i = 0; i < pList->GetCount(); i++) {
				CMPoint* pt1 = pList->Get(i);
				if (pt1) {
					pt1->Co.x += OffX;
					pt1->Co.y += OffY;
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

// Center of rotation is at 0,0 [8/31/2011 XPMUser]
BOOL CSetupPropComm::Rotate(float& x, float& y, float angle) {
	x = x * cos(angle) - y * sin(angle);
	y = x * sin(angle) + y * cos(angle);

	return TRUE;
}

CString CSetupPropComm::MoMove(WPARAM wP, LPARAM lP) {
	if (!g) {
		return "";
	}
	CRect rc;
	short wd = 640, ht = 512, sourceWd = 640, sourceHt = 512;
	CString str = "";
	CSICamPoints* pP = (CSICamPoints*)lP;

	float sfx = 1.0f;
	float sfy = 1.0f;
	float asp = 1;

	if (m_pVideo) {
		m_pVideo->GetClientRect(&rc);
		wd = rc.Width();
		ht = rc.Height();

		asp = wd / float(ht);
		sourceWd = m_pVideo->ImgWd;
		sourceHt = m_pVideo->ImgHt;
	}

	switch (g->ActiveCamera) {
	default:
		if (m_pVideo) {
			sfx = g->ScrnCalibECH.ScrnX / float(wd);
			sfy = g->ScrnCalibECH.ScrnY / float(ht);
		}
		if (pP->StartPt.x == 9999) {
			str.Format("(%d %d)px   (%.1f %.1f)um   [%d,%d,%.2f] (%d,%d) flip:%d",
				pP->CurPt.x * sourceWd / wd, pP->CurPt.y * sourceHt / ht,
				pP->CurPt.x * sfx / g->ScrnCalibECH.ppmX, pP->CurPt.y * sfy / g->ScrnCalibECH.ppmY,
				wd, ht, asp, sourceWd, sourceHt, m_pVideo->bYFlip
			);
		}
		else {
			float dx1 = (pP->StartPt.x - pP->EndPt.x) * sfx / g->ScrnCalibECH.ppmX;
			float dy1 = (pP->StartPt.y - pP->EndPt.y) * sfy / g->ScrnCalibECH.ppmY;
			dx1 *= dx1;
			dy1 *= dy1;
			float dx2 = (pP->StartPt.x - pP->EndPt.x) * sfx / g->ScrnCalibECH.dX;
			float dy2 = (pP->StartPt.y - pP->EndPt.y) * sfy / g->ScrnCalibECH.dY;
			dx2 *= dx2;
			dy2 *= dy2;
			int dX = (pP->EndPt.x - pP->StartPt.x);
			int dY = (pP->EndPt.y - pP->StartPt.y);
			str.Format("[(%d %d)px   (%.1f %.1f)um]  S%.1fum  M%.1fum  [%d,%d,%.2f] flip:%d",
				dX * sourceWd / wd, dY * sourceHt / ht,
				dX * sfx / g->ScrnCalibECH.ppmX, dY * sfy / g->ScrnCalibECH.ppmY,
				sqrt(dx1 + dy1), sqrt(dx2 + dy2),
				wd, ht, asp, m_pVideo->bYFlip);
		}
		break;
	case CCamBase::THINFCAM:
		if (m_pVideo) {
			m_pVideo->GetClientRect(&rc);
			sfx = g->ScrnCalibTF.ScrnX / float(rc.Width());
			sfy = g->ScrnCalibTF.ScrnY / float(rc.Height());
		}
		if (pP->StartPt.x == 9999) {
			str.Format("(%d %d)px   (%.1f %.1f)um  [%d,%d,%.2f] (%d,%d) flip:%d",
				pP->CurPt.x * sourceWd / wd, pP->CurPt.y * sourceHt / ht,
				pP->CurPt.x * sfx / g->ScrnCalibTF.ppmX, pP->CurPt.y * sfy / g->ScrnCalibTF.ppmY,
				wd, ht, asp, sourceWd, sourceHt, m_pVideo->bYFlip);
		}
		else {
			float dx1 = (pP->StartPt.x - pP->EndPt.x) * sfx / g->ScrnCalibTF.ppmX;
			float dy1 = (pP->StartPt.y - pP->EndPt.y) * sfy / g->ScrnCalibTF.ppmY;
			dx1 *= dx1;
			dy1 *= dy1;
			float dx2 = (pP->StartPt.x - pP->EndPt.x) * sfx / g->ScrnCalibTF.ppmX;
			float dy2 = (pP->StartPt.y - pP->EndPt.y) * sfy / g->ScrnCalibTF.ppmY;
			dx2 *= dx2;
			dy2 *= dy2;
			int dX = (pP->EndPt.x - pP->StartPt.x);
			int dY = (pP->EndPt.y - pP->StartPt.y);
			str.Format("(%d %d)px   (%.1f %.1f)um   S%.3fum  M%.3fum  [%d,%d, %.2f] flip:%d",
				dX * sourceWd / wd, dY * sourceHt / ht,
				dX * sfx / g->ScrnCalibTF.ppmX, dY * sfy / g->ScrnCalibTF.ppmY,
				sqrt(dx1 + dy1), sqrt(dx2 + dy2),
				wd, ht, asp, m_pVideo->bYFlip);
		}
		break;
	}
	return str;
}

CString CSetupPropComm::MoMove2(WPARAM wP, LPARAM lP, int width, int height) {
	CString str = "";
	CSICamPoints* pP = (CSICamPoints*)lP;
	str.Format("Video Size (%d x %d)  Position (%.4f, %.4f)", width, height, pP->CurPt.x / float(width), pP->CurPt.y / float(height));
	return str;
}

long CSetupPropComm::ButClick(WPARAM wP, LPARAM lP, CRect& rc) {
	if (!g) {
		return FALSE;
	}
	CPoint* pT = (CPoint*)lP;
	return p413App->DeltaMove(pT, g->ActiveCamera, rc);
}

BOOL CSetupPropComm::LoadRcpSetup() {
	if (!pRcp) {
		return FALSE;
	}
	if (pRcp->LoadRecipe()) {
		if (pRcp->bUseMotorFile) {
			BOOL bRun = p413App->pDMC->bRun;
			pUDev->pInfoPanelSetup->m_Prop3Dlg->StoppingEcho();
			p413App->ResetEchoSamplingProperty(pRcp, NULL);
			if (bRun) {
				pUDev->pInfoPanelSetup->m_Prop3Dlg->StartingEcho();
			}
		}
	}
	return TRUE;
}

// void CSetupPropComm::UpdateTargets(CSICam *pVideo)
// {
// 	if (g && pRcp && pVideo)
// 	{
// 		pVideo->fTgtRedX = pRcp->fTGtRedX;
// 		pVideo->fTgtRedY = pRcp->fTGtRedY;
// 		pVideo->fTgtGrnX = pRcp->fTGtGrnX[g->CurPaSet-1];
// 		pVideo->fTgtGrnY = pRcp->fTGtGrnY[g->CurPaSet-1];
// 		pVideo->fTgtBluX = pRcp->fTGtBluX[g->CurPaSet-1];
// 		pVideo->fTgtBluY = pRcp->fTGtBluY[g->CurPaSet-1];
// 	}
// }

void CSetupPropComm::SetPointer(BOOL bArt) {
	if (!g) {
		g = &p413App->Global;
		if (bArt) ASSERT(0);
	}
	if (!pRcp) {
		pRcp = &g->RcpSetup;
		if (bArt) ASSERT(0);
	}
}

void CSetupPropComm::DoSelectMeasurment(CMPoint* pMP, short nSel, CListCtrl& cList, CWaferMap* pWaferMap) {
	CSelectMeasurementDlg dlg;
	dlg.pMP = pMP;
	dlg.PtNo = nSel + 1;
	dlg.DoModal();
	if (dlg.bDeletePoint == TRUE) {
		DeletePoint(nSel);
		nSel--;
		if (nSel < 0) {
			nSel = 0;
		}
		Renumber(cList, pWaferMap);
	}
	if (dlg.bClearPoint == TRUE) {
		short n = dlg.m_Range.Find('-');
		if (n != -1) {
			short st = atoi(dlg.m_Range.Left(n)) - 1; if (st < 0) st = 0;
			short ed = atoi(dlg.m_Range.Mid(n + 1)) - 1;
			for (int i = st; i <= ed; i++) {
				pMP = FindMPoint(i);
				if (pMP) {
					pMP->MeSet = 0;
					pMP->PatSet = 0;
				}
			}
		}
	}
	if (dlg.m_ApplyAll) {
		MPointApplyAll(pMP);
	}
	pRcp->UpdateControl(cList);
	cList.EnsureVisible(nSel, FALSE);
	cList.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
}

void CSetupPropComm::Renumber(CListCtrl& cList, CWaferMap* pWaferMap) {
	if (pRcp) {
		pRcp->Renumber();
		pRcp->UpdateControl(cList);
	}
	if (pWaferMap) pWaferMap->Redraw();
}

void CSetupPropComm::BlockUI() {
	if (m_Parent) {
		m_Parent->m_tabctrl.SetUIState(TRUE);
	}
	p413App->bUIBlocked = TRUE;
}

void CSetupPropComm::ReleaseUI() {
	if (m_Parent) {
		m_Parent->m_tabctrl.SetUIState(FALSE);
	}
	p413App->bUIBlocked = FALSE;
}

BOOL CSetupPropComm::DoPatternMatching(SDoPattrnMatching& Param, BOOL bMeasureTF) {
	CString MsgStr;
	float X1, Y1, Z2;

	p413App->pMtr->GetXYPos(&Param.initX, &Param.initY);
	MsgStr.Format("Start Loc (%.3f,  %.3f) mm", Param.initX, Param.initY);
	p413App->Message(Param.pList, MsgStr);

	p413App->PreprocessBeforeMatching(pRcp, g->CurPaSet);

	short PatMatchRsl = p413App->DoPattrnMatching(Param);
	switch (PatMatchRsl) {
	case C413AppPatt::pmOK:
		break;
	default:
		return FALSE;
		break;
	}
	if (bMeasureTF) {
		CPtrList* pMPList = &pRcp->Patt.MP.MPLst;
		if (pMPList) {
			POSITION pos = pMPList->GetHeadPosition();
			if (pos) {
				CMPoint* p = (CMPoint*)pMPList->GetNext(pos);
				if (p) {
					MsgStr.Format("Measurement TF @ %d", p->Co.n);
					p413App->Message(Param.pList, MsgStr);
					Param.pMP = p;
					if (p413App->GetThinFilmThicknesses(Param)) {
						MsgStr.Format("TF Measurement @ %d DONE", p->Co.n);
						p413App->Message(Param.pList, MsgStr);
					}
				}
			}
		}
		p413App->PreprocessAfterMatching(pRcp, g->CurPaSet);
	}
	switch (PatMatchRsl) {
	case C413AppPatt::pmOK:

		MsgStr.Format("Start Pos: (%.3f, %.3f) mm", Param.initX, Param.initY);
		p413App->Message(Param.pList, MsgStr);
		MsgStr.Format("End Pos: (%.3f, %.3f) mm", Param.finalX, Param.finalY);
		p413App->Message(Param.pList, MsgStr);
		MsgStr.Format("Found Pos: (%.3f, %.3f) mm", Param.finalX + Param.PatOffX, Param.finalY + Param.PatOffY);
		p413App->Message(Param.pList, MsgStr);
		MsgStr.Format("PatOff (PAT 2 CRN): (%.3f, %.3f) mm", Param.PatOffX, Param.PatOffY);
		p413App->Message(Param.pList, MsgStr);
		MsgStr.Format("Move Dist: (%d, %d) pix", Param.MoveDist.nX, Param.MoveDist.nY);
		p413App->Message(Param.pList, MsgStr);
		MsgStr.Format("Off Dist: (%d, %d) pix", Param.OffDist.nX, Param.OffDist.nY);
		p413App->Message(Param.pList, MsgStr);

		p413App->pMtr->GetXYPos(&X1, &Y1);
		p413App->pMtr->GetThinFilmZPosDev(Z2);
		MsgStr.Format("Final Pos: (%.3f, %.3f, %.3f) mm", X1, Y1, Z2);
		p413App->Message(Param.pList, MsgStr);
		break;
	default:
		if (Param.pImgWnd) Param.pImgWnd->Refresh();
		return FALSE;
		break;
	}
	if (Param.pImgWnd) Param.pImgWnd->Refresh();

	return TRUE;
}

void CSetupPropComm::ExeRecipeSel() {}

void CSetupPropComm::RecipeIO() {
	CRecipeIO dlg;
	dlg.pRcp = &g->RcpSetup;
	dlg.DoModal();
	{
		pRcp = &g->RcpSetup;
		RecipeToLocal();
		// [02282024 ZHIMING
		CString str = _T("FSM 413C2C Control Module : ");
		if (pGDev->SystemModel == pGDev->NMODEL::FSM413SA) {
			str = _T("FSM 413SA Control Module : ");
		}
		else if (pGDev->SystemModel == pGDev->NMODEL::FSM413MOT) {
			str = _T("FSM 413MOT Control Module : ");
		}
		str.AppendFormat("[%s]", pRcp->RcpeName);
		// ]
		AfxGetMainWnd()->SetWindowText(str);
	}
}

////10032025
void CSetupPropComm::DoMeasurmentSetOne(CMPoint* pMP, short nSel, CListCtrl& cList, CWaferMap* pWaferMap) {
	pRcp->UpdateControl(cList);
	cList.EnsureVisible(nSel, FALSE);
	cList.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
}
//

//[03142025_3
void CSetupPropComm::DoMeasurmentSetOneAllOnce(CMPoint* pMP, short nSel, CListCtrl& cList, CWaferMap* pWaferMap) {
	MPointApplyAll(pMP);
	pRcp->UpdateControl(cList);
	cList.EnsureVisible(nSel, FALSE);
	cList.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
}
//]