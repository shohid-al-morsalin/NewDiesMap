// EditCoordinateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "EditCoordinateDlg.h"

#include "413App.h"
#include "MPoint.h"

#include "..\SRC\DOSUtil.h"
#include "InvDistance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditCoordinateDlg dialog

CEditCoordinateDlg::CEditCoordinateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditCoordinateDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CEditCoordinateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	cmd = NOACTION;
}

void CEditCoordinateDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditCoordinateDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditCoordinateDlg, CDialog)
	//{{AFX_MSG_MAP(CEditCoordinateDlg)
	ON_BN_CLICKED(IDC_REVERSEZ, OnReversez)
	ON_BN_CLICKED(IDC_APPLYOFFSET, OnApplyXYOffset)
	ON_BN_CLICKED(IDC_ZEROZ, OnZeroz)
	ON_BN_CLICKED(IDC_OFFSETZ, OnOffsetz)
	ON_BN_CLICKED(IDC_EXPORT, OnExport)
	ON_BN_CLICKED(IDC_EXPORT4, OnExport4)
	ON_BN_CLICKED(IDC_IMPORT, OnImport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditCoordinateDlg message handlers

void CEditCoordinateDlg::OnReversez() {
	cmd = REVERSEZ;
	OnOK();
}

void CEditCoordinateDlg::OnApplyXYOffset() {
	cmd = APPLYXYOFFSET;
	OnOK();
}

void CEditCoordinateDlg::OnZeroz() {
	cmd = ZEROZ;
	OnOK();
}

void CEditCoordinateDlg::OnOffsetz() {
	cmd = OFFSETZ;
	OnOK();
}

void CEditCoordinateDlg::OnExport() {
	CRecipe413* pRcp;
	CString mdir, fname;

	pRcp = &p413App->Global.RcpSetup;

	DosUtil.GetProgramDir(mdir);
	fname.Format("%sRecipe\\%s.RCP.LOC.CSV", mdir, pRcp->RcpeName);
	FILE* fp = fopen(fname, "wb");

	fprintf(fp, "N,X,Y,Z,MeSet,PaSet\n");
	CPtrList* pMPLst = &pRcp->Patt.MP.MPLst;
	POSITION pos = pMPLst->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPLst->GetNext(pos);
		if (p) {
			p->Dump(fp);
		}
	}
	if (fp) {
		fclose(fp);
	}

	OnOK();
}

void CEditCoordinateDlg::OnExport4() {
	CRecipe413* pRcp;

	pRcp = &p413App->Global.RcpSetup;
	CPtrList* pMPLst = &pRcp->Patt.MP.MPLst;
	POSITION pos = pMPLst->GetHeadPosition();
	while (pos) {
		CMPoint* p = (CMPoint*)pMPLst->GetNext(pos);
		if (p) {
			p->Co.x -= pRcp->POffsetX[p->PatSet - 1][0];
			p->Co.y -= pRcp->POffsetY[p->PatSet - 1][0];
		}
	}

	OnOK();
}

void CEditCoordinateDlg::OnImport() {
	CRecipe413 RcpN;
	CInvDistance InvD;

	if (RcpN.LoadRecipe()) {
		// Load data [7/17/2013 Yuen]
		CPtrList* pMPLst = &RcpN.Patt.MP.MPLst;
		InvD.Alloc(pMPLst->GetCount() + 1);
		POSITION pos = pMPLst->GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)pMPLst->GetNext(pos);
			if (p) {
				InvD.Add(&p->Co);
			}
		}
		// Set Z position of current recipe [7/17/2013 Yuen]
		CRecipe413* pRcp = &p413App->Global.RcpSetup;

		// 		float MaxMfy = -99999;
		// 		float MaxOrg = -99999;
		pMPLst = &pRcp->Patt.MP.MPLst;
		pos = pMPLst->GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)pMPLst->GetNext(pos);
			if (p) {
				float Distance;
				RcpN.Patt.FindNearestMP(p->Co, Distance);
				p->zN = InvD.GetZ(p->Co.x, p->Co.y);
				// 				if (p->Co.z > MaxOrg)
				// 				{
				// 					MaxOrg = p->Co.z;
				// 				}
				// 				if (p->zN > MaxMfy)
				// 				{
				// 					MaxMfy = p->zN;
				// 				}
			}
		}

		CString mdir, fname;
		DosUtil.GetProgramDir(mdir);
		fname.Format("%sRecipe\\%s.RCP.MRG.CSV", mdir, pRcp->RcpeName);
		FILE* fp = fopen(fname, "wb");
		// 		float dif = MaxOrg - MaxMfy;
		float dif = 0;	// Disable offset [7/19/2013 Yuen]
		pMPLst = &pRcp->Patt.MP.MPLst;
		pos = pMPLst->GetHeadPosition();
		while (pos) {
			CMPoint* p = (CMPoint*)pMPLst->GetNext(pos);
			if (p) {
				// 				if (fp)
				// 				{
				// 					fprintf(fp,"%.3f,%.3f\n",p->Co.z,p->zN+dif);
				// 				}
				p->Co.z = p->zN + dif;
				p->Dump(fp);
			}
		}
		if (fp) {
			fclose(fp);
		}
	}

	OnOK();
}