// SetupProp31Dlg.cpp : implementation file
//

#include "stdafx.h"
#include <math.h>

#include "e95.h"
#include "SetupProp31Dlg.h"

#include "XTabCtrl.h"
#include "GlobalUIContainer.h"

#include "413App.h"
#include "..\413\EchoScope\ScanLine.h"
#include "..\413\EchoScope\EchoProbeBase.h"

#include "SimulatePeaks.h"
#include "InfoPanelSetup.h"
#include "SetupProp3Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupProp31Dlg dialog

CSetupProp31Dlg::CSetupProp31Dlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSetupProp31Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CSetupProp31Dlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	pSLineT = pSLineB = NULL;
}

void CSetupProp31Dlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupProp31Dlg)
	DDX_Control(pDX, IDC_LIST1, m_cList);
	DDX_Control(pDX, IDC_ECHOPEAKS, m_cEchoPeaks);
	DDX_Control(pDX, IDC_ZOOM, m_cZoomPos);
	DDX_Control(pDX, IDC_POSITION, m_cScrollPos);
	DDX_Control(pDX, IDC_POSITX, m_cPosition);
	DDX_Control(pDX, IDC_ZOOMTX, m_cZoomTx);
	DDX_Control(pDX, IDC_CHART, m_Chart);
	DDX_Control(pDX, IDC_CHART2, m_Chart2);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BASELINE1, m_cBaseline1);
	DDX_Control(pDX, IDC_BASELINE2, m_cBaseline2);
	DDX_Control(pDX, IDC_SLCHART, m_cSLChart);
}

BEGIN_MESSAGE_MAP(CSetupProp31Dlg, CResizableDialog)
	//{{AFX_MSG_MAP(CSetupProp31Dlg)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_FINDPEAKS, OnFindpeaks)
	ON_BN_CLICKED(IDC_LOADRECIPE, OnLoadrecipe)
	ON_BN_CLICKED(IDC_CLEARCONTENT, OnClearcontent)
	ON_BN_CLICKED(IDC_SWITCH, OnSwitch)
	ON_BN_CLICKED(IDC_DUMP, OnDump)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio3)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_SIMULATEDPEAKS, OnSimulatedpeaks)
	ON_BN_CLICKED(IDC_LOAD3, OnBnClickedLoad3)
	ON_BN_CLICKED(IDC_CLEAN, OnBnClickedClean)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTABDESELECTED, OnTabDeselected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupProp31Dlg message handlers
long CSetupProp31Dlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
	}

	bTabSelected = FALSE;

	return 0;
}

long CSetupProp31Dlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Setup\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	SetPointer(0);

	bTabSelected = TRUE;

	return 0;
}

BOOL CSetupProp31Dlg::DestroyWindow() {
	if (pSLineT) {
		delete pSLineT;
	}
	if (pSLineB) {
		delete pSLineB;
	}
	return CResizableDialog::DestroyWindow();
}

void CSetupProp31Dlg::PostNcDestroy() {
	CResizableDialog::PostNcDestroy();
	delete this;
}

BOOL CSetupProp31Dlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	hWndPC = m_hWnd;

	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);

	g = &p413App->Global;
	pRcp = &g->RcpSetup;

	SetPointer(0);

	m_cScrollPos.SetScrollRange(0, 10);
	m_cScrollPos.SetScrollPos(0);

	m_cZoomPos.SetScrollRange(0, 1000);  // Increment of 0.1% [9/24/2011 Administrator]
	m_cZoomPos.SetScrollPos(0);

	m_cEchoPeaks.bReal = FALSE;

	char* dwCjName[] =
	{
		"#", "Index", "CM1", "iCM1", "CM2", "iCM2", "Mid", "XMax", "YMax", "TT1", "TT2"
	};
	int nSize[] = { 32, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = dwCjName[i];
		m_cList.InsertColumn(i, &nListColumn);
	}
	m_cList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	Show = 0;
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);

	m_Chart.size = Raw1.Len;
	// 	m_Chart.zoom = 1.0;
	m_Chart.cpos = (m_Chart.size * m_Chart.zoom) / 2;
	m_Chart2.size = Raw2.Len;
	// 	m_Chart2.zoom = 1.0;
	m_Chart2.cpos = (m_Chart2.size * m_Chart2.zoom) / 2;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupProp31Dlg::OnLoad() {
	CFileDialog dlg(TRUE,
		"CSV",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"CSV file|*.CSV|");

	dlg.m_ofn.lpstrInitialDir = "C:\\TEMP\\";

	if (dlg.DoModal() == IDOK) {
		LoadCSV(&Raw1, dlg.GetPathName());
		p413App->Global.ImageTitle = dlg.GetPathName();
		DosUtil.ExtractFilename(p413App->Global.ImageTitle);
		CEchoProbeBase* pEcho = p413App->pEcho;
		Raw1.BLT = pEcho->CalcBaseline(Raw1.pS1);
		Raw1.BLB = pEcho->CalcBaseline(Raw1.pS2);
		RefreshChart();
	}
}

void CSetupProp31Dlg::OnBnClickedLoad3() {
	CFileDialog dlg(TRUE,
		"CSV",
		NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		"CSV file|*.CSV|");

	dlg.m_ofn.lpstrInitialDir = "C:\\TEMP\\";

	if (dlg.DoModal() == IDOK) {
		LoadCSV(&Raw2, dlg.GetPathName());
		CEchoProbeBase* pEcho = p413App->pEcho;
		Raw2.BLT = pEcho->CalcBaseline(Raw2.pS1);
		Raw2.BLB = pEcho->CalcBaseline(Raw2.pS2);
		RefreshChart();
	}
}

BOOL CSetupProp31Dlg::LoadCSV(CEchoRaw* pRaw, CString name) {
	FILE* fp = fopen(name, "rt");
	if (!fp) {
		return FALSE;
	}
	int p;
	int s1, s2, c1, c2, c3, c4;

	char buff[256];
	fscanf(fp, "%s\n", buff);
	int Len = 0;
	while (fscanf(fp, "%d,%d,%d,%d,%d,%d,%d\n", &p, &s1, &s2, &c1, &c2, &c3, &c4) != EOF) {
		Len++;
	}
	fseek(fp, 0, SEEK_SET);

	pRaw->Alloc(Len);

	Len = 0;
	fscanf(fp, "%s\n", buff);
	while (fscanf(fp, "%d,%d,%d,%d,%d,%d,%d\n", &p, &s1, &s2, &c1, &c2, &c3, &c4) != EOF) {
		*(pRaw->pPosA + Len) = p;
		*(pRaw->pS1 + Len) = s1;
		*(pRaw->pS2 + Len) = s2;
		*(pRaw->pA + Len) = c3;
		*(pRaw->pB + Len) = c4;
		Len++;
		if (Len >= pRaw->Len) {
			break;
		}
	}
	fclose(fp);

	SmoothPos(&pRaw->pPosA, Len);
	SmoothPos(&pRaw->pPosA, Len);
	SmoothPos(&pRaw->pPosA, Len);

	// 	int i,j;
	// 	int cnt = 1;
	// 	int nStep = 2000;
	// 	short TTL = 3072;
	// 	float sf;
	//
	// 	int OldPos = 0;
	// 	int *p1 = pRaw->pPosC+Len-1;
	// 	unsigned short *pAA = pRaw->pA+Len-1;	// Channel A data [7/14/2012 Yuen]
	//
	// 	// Build Position from A B channels [7/14/2012 Yuen]
	// 	unsigned short nA = *(pAA--);
	// 	*(p1--) = OldPos;	// starting point is alway 0 [11/17/2011 Administrator]
	// 	for (i=1; i<Len-1; i++)
	// 	{
	// 		if ((nA>TTL) == (*pAA > TTL))
	// 		{
	// 			cnt++;
	// 		}
	// 		else
	// 		{
	// 			nA = *pAA;
	// 			// Must take care of direction thing [11/17/2011 Administrator]
	// 			sf = nStep / float(cnt);
	// 			for (j=1; j<cnt; j++)
	// 			{
	// 				*p1 = OldPos + j*sf;
	// 				p1--;
	// 			}
	// 			cnt = 1;
	// 			OldPos += nStep;
	// 			*p1 = OldPos;
	// 			p1--;
	// 		}
	// 		pAA--;
	// 	}
	//
	// 	// Last mile [7/14/2012 Yuen]
	// 	if (cnt>1)
	// 	{
	// 		sf = nStep / float(cnt);
	// 		for (j=1; j<cnt; j++)
	// 		{
	// 			*p1 = OldPos + j*sf;
	// 			p1--;
	// 		}
	// 	}
	// 	float sum = 0;
	// 	p1 = pRaw->pPosC;
	// 	int *p2 = p1+2;
	// 	int *p3 = p1+5;
	// 	int *p4 = pRaw->pPosB;
	//
	// 	sum = *(p1) + *(p1+1) + *(p1+2) + *(p1+3) + *(p1+4);
	// 	*(p4++) = sum / 5;
	// 	*(p4++) = sum / 5;
	// 	*(p4++) = sum / 5;
	//
	// 	for (i = 3; i < Len-3; i++)
	// 	{
	// 		sum = sum - *(p1++) + *(p3++);
	// 		*(p4++) = sum / 5;
	// 	}
	// 	*(p4++) = sum / 5;
	// 	*(p4++) = sum / 5;

	return TRUE;
}

void CSetupProp31Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	if (pScrollBar == &m_cScrollPos) {
		ScrollPosHandler(nSBCode, nPos);
	}
	else {
		ZoomPosHandler(nSBCode, nPos);
	}

	CResizableDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSetupProp31Dlg::ScrollPosHandler(UINT nSBCode, UINT nPos) {
	int CurPos = m_cScrollPos.GetScrollPos();
	int mn, mx;
	m_cScrollPos.GetScrollRange(&mn, &mx);
	if (CurPos < mn) {
		CurPos = mn;
		m_cScrollPos.SetScrollPos(CurPos);
	}
	if (CurPos > mx) {
		CurPos = mx;
		m_cScrollPos.SetScrollPos(CurPos);
	}

	// Determine the new position of scroll box.
	switch (nSBCode) {
	case SB_LEFT:      // Scroll to far left.
		CurPos = mn;
		break;

	case SB_RIGHT:      // Scroll to far right.
		CurPos = mx;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		CurPos -= 100;
		if (CurPos < mn) {
			CurPos = mn;
		}
		break;

	case SB_LINERIGHT:   // Scroll right.
		CurPos += 100;
		if (CurPos > mx) {
			CurPos = mx;
		}
		break;

	case SB_PAGELEFT:    // Scroll one page left.
	{
		// Get the page size.
		SCROLLINFO   info;
		m_cScrollPos.GetScrollInfo(&info, SIF_ALL);
		if (CurPos > mn) {
			CurPos = max(mn, CurPos - (int)10000/*info.nPage*/);
		}
	}
	break;

	case SB_PAGERIGHT:      // Scroll one page right
	{
		// Get the page size.
		SCROLLINFO   info;
		m_cScrollPos.GetScrollInfo(&info, SIF_ALL);
		if (CurPos < mx) {
			CurPos = min(mx, CurPos + (int)10000/*info.nPage*/);
		}
	}
	break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		CurPos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		CurPos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	if ((CurPos >= mn) && (CurPos <= mx)) {
		// Set the new position of the thumb (scroll box).
		m_cScrollPos.SetScrollPos(CurPos);

		m_Chart.cpos = CurPos;
		m_Chart.Invalidate();

		m_Chart2.cpos = CurPos;
		m_Chart2.Invalidate();

		CString str;
		str.Format("Position %d", m_Chart.cpos);
		m_cPosition.SetWindowText(str);
	}
}

void CSetupProp31Dlg::ZoomPosHandler(UINT nSBCode, UINT nPos) {
	int CurPos = m_cZoomPos.GetScrollPos();
	int mn, mx;
	m_cZoomPos.GetScrollRange(&mn, &mx);

	// Determine the new position of scroll box.
	switch (nSBCode) {
	case SB_LEFT:      // Scroll to far left.
		CurPos = mn;
		break;

	case SB_RIGHT:      // Scroll to far right.
		CurPos = mx;
		break;

	case SB_ENDSCROLL:   // End scroll.
		break;

	case SB_LINELEFT:      // Scroll left.
		if (CurPos > 0) {
			CurPos--;
		}
		break;

	case SB_LINERIGHT:   // Scroll right.
		if (CurPos < mx) {
			CurPos++;
		}
		break;

	case SB_PAGELEFT:    // Scroll one page left.
	{
		// Get the page size.
		SCROLLINFO   info;
		m_cZoomPos.GetScrollInfo(&info, SIF_ALL);
		if (CurPos > mn) {
			CurPos = max(mn, CurPos - (int)10/*info.nPage*/);
		}
	}
	break;

	case SB_PAGERIGHT:      // Scroll one page right
	{
		// Get the page size.
		SCROLLINFO   info;
		m_cZoomPos.GetScrollInfo(&info, SIF_ALL);
		if (CurPos < mx) {
			CurPos = min(mx, CurPos + (int)10/*info.nPage*/);
		}
	}
	break;

	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		CurPos = nPos;      // of the scroll box at the end of the drag operation.
		break;

	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		CurPos = nPos;     // position that the scroll box has been dragged to.
		break;
	}

	// Set the new position of the thumb (scroll box).
	m_cZoomPos.SetScrollPos(CurPos);

	m_Chart.zoom = 1.0f - CurPos / float(mx - mn);
	m_Chart.Invalidate();

	m_Chart2.zoom = 1.0f - CurPos / float(mx - mn);
	m_Chart2.Invalidate();

	mn = (m_Chart.size * m_Chart.zoom) / 2;
	mx = m_Chart.size - mn;
	m_cScrollPos.SetScrollRange(mn, mx);

	CString str;
	str.Format("Zoom %.1f", 100 * CurPos / float(mx - mn));
	m_cZoomTx.SetWindowText(str);
}

void CSetupProp31Dlg::OnFindpeaks() {
	if (!Raw1.IsOK()) {
		return;
	}

	if (!pSLineT) {
		pSLineT = new CScanLine;
		if (pSLineT) {
			if (!pSLineT->Alloc(Raw1.Len)) {
				delete pSLineT;
				pSLineT = NULL;
			}
		}
	}
	else {
		if (pSLineT->Count < Raw1.Len) {
			pSLineT->Alloc(Raw1.Len);
		}
	}
	if (!pSLineB) {
		pSLineB = new CScanLine;
		if (pSLineB) {
			if (!pSLineB->Alloc(Raw1.Len)) {
				delete pSLineB;
				pSLineB = NULL;
			}
		}
	}
	else {
		if (pSLineB->Count < Raw1.Len) {
			pSLineB->Alloc(Raw1.Len);
		}
	}

	int i;
	CString str;
	CPeaksBuf PeaksBuf[2];

	CopyData3(pRcp, pSLineT, pSLineB, &Raw1);

	CEchoProbeBase* pEcho = p413App->pEcho;

	SaveSpec(TRUE);
	pEcho->FindPeak2(pRcp, 0, 0, &PeaksBuf[0], pSLineT, Raw1.pPosA, Raw1.pS1, g->CurMeSet);
	pEcho->FindPeak2(pRcp, 1, 0, &PeaksBuf[1], pSLineB, Raw1.pPosA, Raw1.pS2, g->CurMeSet);
	SaveSpec(FALSE);

	m_Chart.ClearPos();
	short m, n = 0;

	//n = pSLineT->GetLimits(&m_Chart.Pos[0]);

	for (i = 0; i < PeaksBuf[0].Total; i++) {
		m_Chart.Pos1[n] = PeaksBuf[0].Peaks[i].iRL;
		m_Chart.Pos2[n] = PeaksBuf[0].Peaks[i].iCMs;
		m_Chart.Pos3[n] = PeaksBuf[1].Peaks[i].iCMs;
		n++;
		m_Chart.Pos1[n] = PeaksBuf[0].Peaks[i].iLL;
		n++;

		m = 1;
		str.Format("%d", i + 1);	// # [3/22/2013 Yuen]
		int idx = m_cList.InsertItem(i, str);
		str.Format("%d", PeaksBuf[0].Peaks[i].iCMs);	// Index [3/22/2013 Yuen]
		m_cList.SetItemText(idx, m++, str);
		str.Format("%.4f", PeaksBuf[0].Peaks[i].fCMs * 1e-4);	// CM1 (um) [3/22/2013 Yuen]
		m_cList.SetItemText(idx, m++, str);
		str.Format("%d", PeaksBuf[0].Peaks[i].iCMs);	// iCM1 [3/22/2013 Yuen]
		m_cList.SetItemText(idx, m++, str);
		str.Format("%.4f", PeaksBuf[1].Peaks[i].fCMs * 1e-4);	// CM2 (um) [3/22/2013 Yuen]
		m_cList.SetItemText(idx, m++, str);
		str.Format("%d", PeaksBuf[1].Peaks[i].iCMs);	// iCM2 [3/22/2013 Yuen]
		m_cList.SetItemText(idx, m++, str);
		str.Format("%.4f", PeaksBuf[0].Peaks[i].Mid * 1e-4);	// Mid [3/22/2013 Yuen]
		m_cList.SetItemText(idx, m++, str);
		str.Format("%.4f", PeaksBuf[0].Peaks[i].fXMaxima * 1e-4);	// XMax [3/22/2013 Yuen]
		m_cList.SetItemText(idx, m++, str);
		str.Format("%d", PeaksBuf[0].Peaks[i].YMaxima);	// YMax [3/22/2013 Yuen]
		m_cList.SetItemText(idx, m++, str);
		if (i > 0) {
			str.Format("%.2f", fabs(PeaksBuf[0].Peaks[i].fCMs - PeaksBuf[0].Peaks[i - 1].fCMs) / 3.685e4);	// TT1 [3/22/2013 Yuen]
			m_cList.SetItemText(idx, m++, str);
			str.Format("%.2f", fabs(PeaksBuf[1].Peaks[i].fCMs - PeaksBuf[1].Peaks[i - 1].fCMs) / 3.685e4);	// TT2 [3/22/2013 Yuen]
			m_cList.SetItemText(idx, m++, str);
		}
	}

	double max = 4096;
	double min = 0;
	m_cEchoPeaks.SetData1(pSLineT, NULL, min, max, 0, 0);

	m_Chart.Invalidate(TRUE);
}

// This function is limited applies to loaded data from CSV files [9/24/2011 Administrator]
void CSetupProp31Dlg::CopyData3(CRecipe413* pRcp, CScanLine* pSLineTT, CScanLine* pSLineBB, CEchoRaw* pRaw) {
	// 	int i;
	short Level1 = pRcp->MeParam[g->CurMeSet - 1].PSG[0].Level + g->PSLevelOff[0];
	short Level2 = pRcp->MeParam[g->CurMeSet - 1].PSG[1].Level + g->PSLevelOff[1];

	CString fname;
	if (pSLineTT) {
		pSLineTT->Idx = 0;
		pSLineTT->MaxMinX.iMin = 0;
		pSLineTT->MaxMinX.iMax = *(pRaw->pPosA + pRaw->Len - 1);
		fname.Format("C:\\TEMP\\%s-DUMPT.CSV", p413App->Global.ImageTitle);
		fname = "";
		CopySL(pSLineTT, 0, pRaw, pRaw->pS1, pRaw->BLT, Level1, fname);
	}
	if (pSLineBB) {
		pSLineBB->Idx = 0;
		pSLineBB->MaxMinX.iMin = 0;
		pSLineBB->MaxMinX.iMax = *(pRaw->pPosA + pRaw->Len - 1);
		fname.Format("C:\\TEMP\\%s-DUMPB.CSV", p413App->Global.ImageTitle);
		fname = "";
		CopySL(pSLineBB, 0, pRaw, pRaw->pS2, pRaw->BLB, Level2, fname);
	}
}

void CSetupProp31Dlg::OnLoadrecipe() {
	RecipeIO();
	// 	if (LoadRcpSetup())
	// 	{
	// 	}
}

void CSetupProp31Dlg::OnClearcontent() {
	m_Chart.ClearPos();
	m_cList.DeleteAllItems();
}

void CSetupProp31Dlg::OnSwitch() {
	if (m_cEchoPeaks.bReal) {
		m_cEchoPeaks.bReal = FALSE;
	}
	else {
		m_cEchoPeaks.bReal = TRUE;
	}
	m_cEchoPeaks.Refresh();
}

void CSetupProp31Dlg::OnDump() {
	bSaveSpec1 = bSaveSpec2 = bSaveSpec3 = bSaveSpec31 = bSaveSpec4 = bSaveSpec5 = TRUE;
	OnFindpeaks();
	bSaveSpec1 = bSaveSpec2 = bSaveSpec3 = bSaveSpec31 = bSaveSpec4 = bSaveSpec5 = FALSE;
}

void CSetupProp31Dlg::OnRadio1() {
	Show = 0;
	RefreshChart();
}

void CSetupProp31Dlg::OnRadio3() {
	Show = 1;
	RefreshChart();
}

void CSetupProp31Dlg::OnRadio4() {
	Show = 2;
	RefreshChart();
}

void CSetupProp31Dlg::OnRadio5() {
	Show = 3;
	RefreshChart();
}

void CSetupProp31Dlg::RefreshChart() {
	m_Chart.size = Raw1.Len;
	m_Chart.pX = Raw1.pPosA;
	m_Chart2.size = Raw2.Len;
	m_Chart2.pX = Raw2.pPosA;
	switch (Show) {
	case 0:
		m_Chart.pY = Raw1.pS1;
		m_Chart2.pY = Raw2.pS1;
		break;
	case 1:
		m_Chart.pY = Raw1.pS2;
		m_Chart2.pY = Raw2.pS2;
		break;
	case 2:
		m_Chart.pY = Raw1.pA;
		m_Chart2.pY = Raw2.pA;
		break;
	case 3:
		m_Chart.pY = Raw1.pB;
		m_Chart2.pY = Raw2.pB;
		break;
	}
	m_Chart.size = Raw1.Len;
	// 	m_Chart.width = 1.0;
	// 	m_Chart.cpos = (m_Chart.size * m_Chart.width)/2;
	m_Chart2.size = Raw2.Len;
	// 	m_Chart2.width = 1.0;
	// 	m_Chart2.cpos = (m_Chart2.size * m_Chart2.width)/2;
	switch (Show) {
	case 0:
	case 1:
		m_Chart.method = 0;
		m_Chart2.method = 0;
		break;
	default:
		m_Chart.method = 1;
		m_Chart2.method = 1;
		break;
	}
	m_Chart.Invalidate();
	m_Chart2.Invalidate();

	int mn, mx;
	mn = (m_Chart.size * m_Chart.zoom) / 2.0f;
	mx = m_Chart.size - (m_Chart.size * m_Chart.zoom) / 2.0f;
	m_cScrollPos.SetScrollRange(mn, mx);
	m_cScrollPos.SetScrollPos(m_Chart.cpos);
	m_cZoomPos.SetScrollPos((1 - m_Chart.zoom) * 1000);

	CString str;
	str.Format("%.1f  %.1f", Raw1.BLT, Raw1.BLB);
	m_cBaseline1.SetWindowText(str);
	str.Format("%.1f  %.1f", Raw2.BLT, Raw2.BLB);
	m_cBaseline2.SetWindowText(str);
}

void CSetupProp31Dlg::OnSimulatedpeaks() {
	CSimulatePeaks dlg;

	dlg.DoModal();
}

void CSetupProp31Dlg::OnBnClickedClean() {
	Raw1.Clear();
	Raw2.Clear();
}

int CSetupProp31Dlg::Sign(int v) {
	return v > 0 ? 1 : (v < 0 ? -1 : 0);
}

void CSetupProp31Dlg::CopySL(CScanLine* pSLine, short Method, CEchoRaw* pRaw, unsigned short* pS, short BL, short Level, CString DumpFile) {
	if (pSLine) {
		int i, v;
		int* pP = pRaw->pPosA;
		int ThresT = 4000 - BL;
		switch (Method) {
		case 0:	// +ve and -ve peaks [7/17/2012 Yuen]
			for (i = 0; i < pRaw->Len; i++) {
				v = *(pS++) - BL;
				v = abs(v) - Level;	// Accept +ve and -ve peaks [7/16/2012 Yuen]
				if (v > 0) {
					if (v > 1400) {
						v = 1400;
					}
					pSLine->Set(*(pP), v, i);
				}
				pP++;
			}
			break;
		case 1: // +ve peak only [7/17/2012 Yuen]
			for (i = 0; i < pRaw->Len; i++) {
				v = *(pS++) - BL;
				v -= Level;	// Accept +ve peaks only [7/16/2012 Yuen]
				if (v > 0) {
					if (v > 1400) {
						v = 1400;
					}
					pSLine->Set(*(pP), v, i);
				}
				pP++;
			}
			break;
		case 2:	// -ve peak only [7/17/2012 Yuen]
			for (i = 0; i < pRaw->Len; i++) {
				v = *(pS++) - BL;
				v += Level;
				if (v < 0) {
					if (v > 1400) {
						v = 1400;
					}
					pSLine->Set(*(pP), abs(v), i);
				}
				pP++;
			}
			break;
		}
		if (DumpFile.GetLength() > 0) {
			pSLine->Dump(DumpFile);
		}
	}
}

void CSetupProp31Dlg::SaveSpec(BOOL bSave) {
	// 	bSaveSpec1 = bSave;
	// 	bSaveSpec2 = bSave;
	// 	bSaveSpec3 = bSave;
	// 	bSaveSpec31 = bSave;
	// 	bSaveSpec4 = bSave;
	bSaveSpec5 = bSave;
}

void CSetupProp31Dlg::SmoothPos(int** pos, int len) {
	float sum = 0;
	int* pPosC = new int[len];

	int* p1 = *pos;
	int* p2 = p1 + 2;
	int* p3 = p1 + 5;
	int* p4 = pPosC;

	sum = *(p1)+*(p1 + 1) + *(p1 + 2) + *(p1 + 3) + *(p1 + 4);
	*(p4++) = sum / 5;
	*(p4++) = sum / 5;
	*(p4++) = sum / 5;

	for (int i = 3; i < len - 3; i++) {
		sum = sum - *(p1++) + *(p3++);
		*(p4++) = sum / 5;
	}
	*(p4++) = sum / 5;
	*(p4++) = sum / 5;

	delete[] * pos;
	*pos = pPosC;
}