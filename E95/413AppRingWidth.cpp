#include "stdafx.h"
#include "413AppRingWidth.h"
#include "SRC/DOSUtil.h"
#include "413/MotorS2.h"
#include "MPoint.h"
#include "Data.h"

C413AppRingWidth::C413AppRingWidth() {
	bCenterWafer = FALSE;
	Thres = 127;
	sfX = Global.ScrnCalibECH.dX;
	sfY = Global.ScrnCalibECH.dY;
	LoadParam(); SaveParam();
}

C413AppRingWidth::~C413AppRingWidth() {
	if (Img) Img.Destroy();
	if (BgImg) BgImg.Destroy();
}

void C413AppRingWidth::LoadParam() {
	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);
	bCenterWafer = DosUtil.ReadINI("RingWidth", "CenterWafer", bCenterWafer, inifile);
}

void C413AppRingWidth::SaveParam() {
	CString inifile;
	DosUtil.GetLocalCfgFile(inifile);
	DosUtil.WriteINI("RingWidth", "CenterWafer", bCenterWafer, inifile);
}

void C413AppRingWidth::AdjAngle(float& ang) {
	while (ang > 360.0f) { ang -= 360.0f; }
	while (ang < 0) { ang += 360; }
}

BOOL C413AppRingWidth::CenterWafer(CRecipe413* pRcp, BYTE Thr, CSICam* pVideo, CImgWnd* pImgWnd, CListCtrl* pList) {
	
	if (!pMtr) return FALSE;
	CString msg = "";
	C413Global* g = &Global;

	float fZ1 = static_cast<float>(pRcp->TRWMeasProp.ZPos);
	float WaferRadius = static_cast<float>(pRcp->TRWMeasProp.waferRadius);
	float offset = static_cast<float>(pRcp->TRWMeasProp.offsetAngle);

	// Assume radius is 150mm [2/4/2020 yuenl]
	Thres = Thr;
	float radius = WaferRadius + 0.35f; //must start from location > 150 due to interferance from narrow ring width
	float angle, x, y, dr;

	WC.ClearOffset();

	// Move to center, take background image [2/4/2020 yuenl]
	msg = "Goto origin error";
	if (!pMtr->GotoXY(0, 0)) { Message(pList, msg, 2); return FALSE; }
	if(g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); return FALSE; }

	// Move Z1 to focus point [2/4/2020 yuenl]
	msg = "Goto Z1 focus point error";
	if (!pMtr->MoveEchoTopZ(pRcp->TRWMeasProp.ZPos)) { Message(pList, msg, 0); return FALSE; }
	pMtr->WaitEchoTopZ();
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); return FALSE; }

	msg.Format("Move Position: X=0.00 Y=0.00 Z=%.2f", pRcp->TRWMeasProp.ZPos);
	Message(pList, msg, 2);

	// take background image [2/4/2020 yuenl]
	msg = "Take background image";
	Message(pList, msg, 2);

	if (BgImg) { BgImg.Destroy(); }

	pVideo->SnapOneFrame();
	CPackedDIB* pDIB = &g->StoredDIB;
	if (!pDIB->pDIB)
	{
		msg = "Grab one frame error";
		Message(pList, msg, 2);
		return FALSE;
	}

	BgImg.Attach(pDIB);

	if (BgImg.IsNull()) {
		msg = "Take background image FAILED";
		Message(pList, msg, 2);
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	// move to -5 degree [2/4/2020 yuenl]
	angle = offset; AdjAngle(angle);
	g->Polar2Cartesian(radius, angle, x, y);
	if (!pMtr->GotoXY(x, y)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// record edge position [2/4/2020 yuenl]
	if (!FindCenter(angle, dr, pVideo, pImgWnd)) { msg = "Find center error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// Covert dr to physical unit
	g->Polar2Cartesian(radius + dr, angle, WC.X0, WC.Y0);
	if (!pMtr->GotoXY(WC.X0, WC.Y0)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	//////////////////////////////////////////////////////////////////////////
	// move to 90 degree [2/4/2020 yuenl]
	angle = 90.0f + offset; AdjAngle(angle);
	g->Polar2Cartesian(radius, angle, x, y);
	if (!pMtr->GotoXY(x, y)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// record edge position [2/4/2020 yuenl]
	if (!FindCenter(angle, dr, pVideo, pImgWnd)) { msg = "Find center error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// Covert dr to physical unit
	g->Polar2Cartesian(radius + dr, angle, WC.X90, WC.Y90);
	if (!pMtr->GotoXY(WC.X90, WC.Y90)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	//////////////////////////////////////////////////////////////////////////
	// move to 180 degree [2/4/2020 yuenl]
	angle = 180.0f + offset;
	g->Polar2Cartesian(radius, angle, x, y);
	if (!pMtr->GotoXY(x, y)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// record edge position [2/4/2020 yuenl]
	if (!FindCenter(angle, dr, pVideo, pImgWnd)) { msg = "Find center error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// Covert dr to physical unit
	g->Polar2Cartesian(radius + dr, angle, WC.X180, WC.Y180);
	if (!pMtr->GotoXY(WC.X180, WC.Y180)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	//////////////////////////////////////////////////////////////////////////
	// move to 270 degree [2/4/2020 yuenl]
	angle = 270.0f + offset;
	g->Polar2Cartesian(radius, angle, x, y);
	if (!pMtr->GotoXY(x, y)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// record edge position [2/4/2020 yuenl]
	if (!FindCenter(angle, dr, pVideo, pImgWnd)) { msg = "Find center error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// Covert dr to physical unit
	g->Polar2Cartesian(radius + dr, angle, WC.X270, WC.Y270);
	if (!pMtr->GotoXY(WC.X270, WC.Y270)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	//////////////////////////////////////////////////////////////////////////
	// Calculate center offsets [2/4/2020 yuenl]
	WC.FindOffset();
	//BgImg.Destroy();
	return TRUE;
}

BOOL C413AppRingWidth::FindCenter(float ang, float& dr, CSICam* pVideo, CImgWnd* pImgWnd) {
	C413Global* g = &Global;
	if (Img) { Img.Destroy(); }
	pVideo->SnapOneFrame();
	CPackedDIB* pDIB = &g->StoredDIB;
	if (!pDIB->pDIB) return FALSE;
	Img.Attach(pDIB);
	if (!BgImg.IsNull()) {
		Img = (Img / BgImg); // perform background subtract [2/4/2020 yuenl]
	}
	Img.BThreshold(Thres);
	Img.FindEdge(Img.edge.Inner, ang, 0, FALSE);// find black pixel [2/4/2020 yuenl]
	ShowImage(Img, pImgWnd);

	int wd, ht, bpp;
	Img.GetDim(wd, ht, bpp);

	CPtLine::Vec2 p(wd / 2.0f, ht / 2.0f);
	CPtLine::Vec2 q;
	dr = Img.GetDisplacement(Img.edge.Inner, ang, p, q) * sfX;
	return TRUE;
}

void C413AppRingWidth::ShowImage(CIM Img, CImgWnd* pImgWnd){
	int wd, ht, bpp;
	Img.GetDim(wd, ht, bpp);
	C413Global* g = &Global;
	CPackedDIB* pDIBN = new CPackedDIB;
	pDIBN->Duplicate(g->StoredDIB);
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		PBYTE p = (PBYTE)pDIBN->GetPixelAddress(0, y);
		PBYTE q = (PBYTE)Img.GetPixelAddress(0, y);
		memcpy(p, q, pDIBN->BytesPerRow);
	}
	pImgWnd->DupStore1(*pDIBN);
	pImgWnd->Refresh();
	pDIBN->ClearDIB();
}

BOOL C413AppRingWidth::FindBevel(float WaferRadius, float offset, double& BevelWidth, CListCtrl* pList, CSICam* pVideo, CImgWnd* pImgWnd) {// BgImg must be valid [6/24/2020 yuenl]
	if (!pMtr) return FALSE;
	
	C413Global* g = &Global;
	float x, y, dr, x1, y1, x2, y2, r1, r2;
	float radius = WaferRadius;
	float angle = 180.0f + offset; AdjAngle(angle);
	BevelWidth = 0;
	CString msg = "";

	g->Polar2Cartesian(radius, angle, x, y);
	if (!pMtr->GotoXY(x, y)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	if (!FindCenter(angle, dr, pVideo, pImgWnd)) { msg = "Find center error"; Message(pList, msg, 2); return FALSE; }
	msg.Format(_T("Found center dr: %.2f"), dr);
	Message(pList, msg, 2);
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	g->Polar2Cartesian(radius + dr, angle, x1, y1); r1 = radius + dr;
	msg.Format(_T("Found edge at (%.2f, %.2f)"), x1, y1);
	Message(pList, msg, 2);
	if (!pMtr->GotoXY(x1, y1)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }

	angle = offset; AdjAngle(angle);
	g->Polar2Cartesian(radius, angle, x, y);
	if (!pMtr->GotoXY(x, y)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	// record edge position [2/4/2020 yuenl]
	if (!FindCenter(angle, dr, pVideo, pImgWnd)) { msg = "Find center error"; Message(pList, msg, 2); return FALSE; }
	msg.Format(_T("Found center dr: %.2f"), dr);
	Message(pList, msg, 2);
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }
	g->Polar2Cartesian(radius + dr, angle, x2, y2); r2 = radius + dr;
	msg.Format(_T("Found edge at (%.2f, %.2f)"), x2, y2);
	Message(pList, msg, 2);
	if (!pMtr->GotoXY(x2, y2)) { msg = "Goto XY position error"; Message(pList, msg, 2); return FALSE; }
	if (g->bAborting || g->bStop) { msg = "Process Abort"; Message(pList, msg, 2); BgImg.Destroy(); return FALSE; }

	//////////////////////////////////////////////////////////////////////////
	// Calculate bevel width here
	double fac = g->ScrnCalibECH.dX * (g->ScrnCalibECH.ppmX * 1e3); // um to mm
	BevelWidth = static_cast<double>((WaferRadius - float(r1 + r2) / 2.0f) * fac);
	return TRUE;
}

BOOL C413AppRingWidth::GenerateTaikoPoints(CRecipe413* pRcp) {
	HtLn.clear();
	if (!pRcp->TRWMeasProp.npoints) return FALSE;
	float dang = 360 / float(pRcp->TRWMeasProp.npoints);
	for (int i = 0; i < pRcp->TRWMeasProp.npoints; i++) {
		SHtPoint P;
		P.idx = i;
		float da = i * dang + pRcp->TRWMeasProp.offsetAngle;
		if (!pRcp->TRWMeasProp.fromAngle && !pRcp->TRWMeasProp.toAngle) {
			if ((da >= pRcp->TRWMeasProp.fromAngle) && (da <= pRcp->TRWMeasProp.toAngle)) {
				if ((da - pRcp->TRWMeasProp.fromAngle) < (pRcp->TRWMeasProp.toAngle - da)) da = pRcp->TRWMeasProp.fromAngle;
				else da = pRcp->TRWMeasProp.toAngle;
			}
		}
		while (da < 0) da += 360;
		while (da > 360) da -= 360;
		Global.Polar2Cartesian(pRcp->TRWMeasProp.waferRadius, da, P.x, P.y);
		P.angle = da;
		HtLn.push_back(P);
	}
	return TRUE;
}

BOOL C413AppRingWidth::GetSubstrateRingWidth(CRecipe413* pRcp, CMPoint* pMP, BOOL bLast, short* code, CListCtrl* pList) {
	if (!pRcp->IsValidMeSet(pMP)) {
		//MsgStr = "Invalid MeSet";
		return FALSE;
	}

	int i;
	BOOL bIsRingWidth = pRcp->IsRingWidthMeasurement();

	short MeSetX = MaXPA * (pMP->MeSet - 1);
	CMeParam* pMeP = &pRcp->MeParam[pMP->MeSet - 1];

	BOOL bAllOK = TRUE;
	for (i = 0; i < MaXPA; i++) {
		if (*(code + i) >= 1) {
			// done measurement [11/5/2011 Administrator]
			continue;
		}
		CString str;
		MParam* p = &pMeP->MPa[i];
		MParam::PRBNUM Pb = p->Prb;
		if (Pb > 0) {
			if (Pb == MParam::RINGPRB) {
				//-------------------------p->fRingWidth = pRough->Read(NAver);
				pRcp->TRWMeasProp.ringWidth += p->Off; // 031024
				p->fRingWidth = pRcp->TRWMeasProp.ringWidth + pRcp->TRWMeasProp.bevelWidth;
				if (p->Lo && p->Hi) {
					if (p->fRingWidth < p->Lo) {
						MsgStr.Format(_T("%s[%.3f] at (%s) did not pass Lo limit %.3f"), p->Name.GetBuffer(), p->fRingWidth, pMP->Co.GetXYStr().GetBuffer(), p->Lo);
						Message(pList, MsgStr);
					}
					else if (p->fRingWidth > p->Hi) {
						MsgStr.Format(_T("%s[%.3f] at (%s) did not pass Hi limit %.3f"), p->Name.GetBuffer(), p->fRingWidth, pMP->Co.GetXYStr().GetBuffer(), p->Lo);
						Message(pList, MsgStr);
					}
					else {
						*(code + i) = 1;
					}
				}
				else {
					if (p->fRingWidth < 0) {
						p->fRingWidth = -1;
					}
					else {
						*(code + i) = 1;
					}
				}
			}
			else {
				// No entry [11/5/2011 Administrator]
				*(code + i) = 2;
			}
		}
		else {
			// No entry [11/5/2011 Administrator]
			*(code + i) = 2;
		}
	}
	if (bAllOK || bLast) {
		CData* d = pMP->GetData(0, TRUE);
		if (!d) {
			return FALSE;
		}
		// Initialize to invalid value [8/16/2012 Administrator]
		for (i = 0; i < MaXPA; i++) {
			if ((pMeP->MPa[i].Prb > 0) && (*(code + i) == 1)) {
				d->Set(MeSetX + i, pMeP->MPa[i].fRingWidth);
			}
		}
		for (i = 0; i < MaXPA; i++) {
			if (*(code + i) == 0) {
				// Some value did not measure correctly [5/31/2012 Yuen]
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}
