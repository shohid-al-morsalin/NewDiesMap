#include "stdafx.h"
#include "e95.h"

#include "128/GEM/ObjectsContainer.h"
#include "413App.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "IPC/Toolvar.h"
#include "ONLINE/GEM/CediListCtroller.h"
#include "SERIAL/MsgItem.h"
#include "SYS/CUMMsg.h"
// #include "SRC/DOSUtil.h"

#include "ProcJobPWC413Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcJobPWC413Dlg dialog

#define IDT_START				9999
#define IDT_TVDATAREQ_TIMEOUT	9998

CProcJobPWC413Dlg::CProcJobPWC413Dlg(CWnd* pParent /*=NULL*/)
	: CProcJobPWC1DlgBase(CProcJobPWC413Dlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobPWC413Dlg)
	m_Recipe = _T("");
	m_SelectAll = TRUE;
	//}}AFX_DATA_INIT
	bRecipeSelected = FALSE;
	for (int i = 0; i < 25; i++) {
		m_sl[i] = FALSE;
		m_slEn[i] = TRUE;
	}
	p413App->Global.LoadMaxSlot();//20240820/Arup for read size of slotmap
}

void CProcJobPWC413Dlg::DoDataExchange(CDataExchange* pDX) {
	CProcJobPWC1DlgBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobPWC413Dlg)
	DDX_Control(pDX, IDC_PROCESSING, m_cProceed);
	DDX_Control(pDX, IDC_SCANMORE, m_cScanMore);
	DDX_Control(pDX, IDC_SCANSECOND, m_cScanSecond);
	DDX_Control(pDX, IDC_SCANFIRST, m_cScanFirst);
	DDX_Control(pDX, IDC_CARRIERID, m_cCarrierID);
	DDX_Text(pDX, IDC_RECIPE, m_Recipe);
	DDX_Control(pDX, IDC_LOTID, m_cLotID);
	DDX_Control(pDX, IDC_OPERATOR, m_cOperator);
	DDX_Check(pDX, IDC_SELECTALL, m_SelectAll);
	DDX_Control(pDX, IDC_SELECTALL, m_cSelectAll);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_WAFERREPEAT, p413App->Global.WaferRepeat);
	DDX_Text(pDX, IDC_CASSETTEREPEAT, p413App->Global.CassetteRepeat);

	DDX_Control(pDX, IDC_ID1, m_cID[0]);
	DDX_Control(pDX, IDC_ID2, m_cID[1]);
	DDX_Control(pDX, IDC_ID3, m_cID[2]);
	DDX_Control(pDX, IDC_ID4, m_cID[3]);
	DDX_Control(pDX, IDC_ID5, m_cID[4]);
	DDX_Control(pDX, IDC_ID6, m_cID[5]);
	DDX_Control(pDX, IDC_ID7, m_cID[6]);
	DDX_Control(pDX, IDC_ID8, m_cID[7]);
	DDX_Control(pDX, IDC_ID9, m_cID[8]);
	DDX_Control(pDX, IDC_ID10, m_cID[9]);
	DDX_Control(pDX, IDC_ID11, m_cID[10]);
	DDX_Control(pDX, IDC_ID12, m_cID[11]);
	DDX_Control(pDX, IDC_ID13, m_cID[12]);
	DDX_Control(pDX, IDC_ID14, m_cID[13]);
	DDX_Control(pDX, IDC_ID15, m_cID[14]);
	DDX_Control(pDX, IDC_ID16, m_cID[15]);
	DDX_Control(pDX, IDC_ID17, m_cID[16]);
	DDX_Control(pDX, IDC_ID18, m_cID[17]);
	DDX_Control(pDX, IDC_ID19, m_cID[18]);
	DDX_Control(pDX, IDC_ID20, m_cID[19]);
	DDX_Control(pDX, IDC_ID21, m_cID[20]);
	DDX_Control(pDX, IDC_ID22, m_cID[21]);
	DDX_Control(pDX, IDC_ID23, m_cID[22]);
	DDX_Control(pDX, IDC_ID24, m_cID[23]);
	DDX_Control(pDX, IDC_ID25, m_cID[24]);

	DDX_Control(pDX, IDC_SLOT1, m_csl[0]);
	DDX_Control(pDX, IDC_SLOT2, m_csl[1]);
	DDX_Control(pDX, IDC_SLOT3, m_csl[2]);
	DDX_Control(pDX, IDC_SLOT4, m_csl[3]);
	DDX_Control(pDX, IDC_SLOT5, m_csl[4]);
	DDX_Control(pDX, IDC_SLOT6, m_csl[5]);
	DDX_Control(pDX, IDC_SLOT7, m_csl[6]);
	DDX_Control(pDX, IDC_SLOT8, m_csl[7]);
	DDX_Control(pDX, IDC_SLOT9, m_csl[8]);
	DDX_Control(pDX, IDC_SLOT10, m_csl[9]);
	DDX_Control(pDX, IDC_SLOT11, m_csl[10]);
	DDX_Control(pDX, IDC_SLOT12, m_csl[11]);
	DDX_Control(pDX, IDC_SLOT13, m_csl[12]);
	DDX_Control(pDX, IDC_SLOT14, m_csl[13]);
	DDX_Control(pDX, IDC_SLOT15, m_csl[14]);
	DDX_Control(pDX, IDC_SLOT16, m_csl[15]);
	DDX_Control(pDX, IDC_SLOT17, m_csl[16]);
	DDX_Control(pDX, IDC_SLOT18, m_csl[17]);
	DDX_Control(pDX, IDC_SLOT19, m_csl[18]);
	DDX_Control(pDX, IDC_SLOT20, m_csl[19]);
	DDX_Control(pDX, IDC_SLOT21, m_csl[20]);
	DDX_Control(pDX, IDC_SLOT22, m_csl[21]);
	DDX_Control(pDX, IDC_SLOT23, m_csl[22]);
	DDX_Control(pDX, IDC_SLOT24, m_csl[23]);
	DDX_Control(pDX, IDC_SLOT25, m_csl[24]);

	DDX_Check(pDX, IDC_SLOT1, m_sl[0]);
	DDX_Check(pDX, IDC_SLOT2, m_sl[1]);
	DDX_Check(pDX, IDC_SLOT3, m_sl[2]);
	DDX_Check(pDX, IDC_SLOT4, m_sl[3]);
	DDX_Check(pDX, IDC_SLOT5, m_sl[4]);
	DDX_Check(pDX, IDC_SLOT6, m_sl[5]);
	DDX_Check(pDX, IDC_SLOT7, m_sl[6]);
	DDX_Check(pDX, IDC_SLOT8, m_sl[7]);
	DDX_Check(pDX, IDC_SLOT9, m_sl[8]);
	DDX_Check(pDX, IDC_SLOT10, m_sl[9]);
	DDX_Check(pDX, IDC_SLOT11, m_sl[10]);
	DDX_Check(pDX, IDC_SLOT12, m_sl[11]);
	DDX_Check(pDX, IDC_SLOT13, m_sl[12]);
	DDX_Check(pDX, IDC_SLOT14, m_sl[13]);
	DDX_Check(pDX, IDC_SLOT15, m_sl[14]);
	DDX_Check(pDX, IDC_SLOT16, m_sl[15]);
	DDX_Check(pDX, IDC_SLOT17, m_sl[16]);
	DDX_Check(pDX, IDC_SLOT18, m_sl[17]);
	DDX_Check(pDX, IDC_SLOT19, m_sl[18]);
	DDX_Check(pDX, IDC_SLOT20, m_sl[19]);
	DDX_Check(pDX, IDC_SLOT21, m_sl[20]);
	DDX_Check(pDX, IDC_SLOT22, m_sl[21]);
	DDX_Check(pDX, IDC_SLOT23, m_sl[22]);
	DDX_Check(pDX, IDC_SLOT24, m_sl[23]);
	DDX_Check(pDX, IDC_SLOT25, m_sl[24]);
}

BEGIN_MESSAGE_MAP(CProcJobPWC413Dlg, CProcJobPWC1DlgBase)
	//{{AFX_MSG_MAP(CProcJobPWC413Dlg)
	ON_BN_CLICKED(IDC_BROWSERECIPE, OnBrowserecipe)
	ON_BN_CLICKED(IDC_INSPECRECIPE, OnInspecrecipe)
	ON_BN_CLICKED(IDC_PROCESSING, OnProcessing)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_SCANFIRST, OnScanfirst)
	ON_BN_CLICKED(IDC_SCANSECOND, OnScansecond)
	ON_BN_CLICKED(IDC_SCANMORE, OnScanmore)
	ON_EN_KILLFOCUS(IDC_WAFERREPEAT, OnKillfocusWaferrepeat)
	ON_BN_CLICKED(IDC_CANCEL, OnCancel)
	ON_EN_KILLFOCUS(IDC_CASSETTEREPEAT, OnKillfocusCassetterepeat)
	//}}AFX_MSG_MAP
	ON_MESSAGE(ID_IPC_TVDATAACK, OnIpcTvdataack)
	ON_MESSAGE(ID_IPC_TVDATANAK, OnIpcTvdatanak)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcJobPWC413Dlg message handlers

void CProcJobPWC413Dlg::OnCancel() {
	//Note: do not change this message, it is decoded elsewhere
	CString str;
	str.LoadString(IDS_CARRIERCANCELED);
	pGDev->SetMessage(str);
	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBCANCELCARRIER, ID, UM_TO_PROCJOBPWC1);
}

void CProcJobPWC413Dlg::OnBrowserecipe() {
	CRecipe413* pRcp = &pUDev->pProcJobInfo[ID].Cas.Recipe413;

	if (p413App->LoadRecipe(pRcp)) {
		// Multiple instance could exist at one time. Simply load the recipe but do not execute the associated content ie ResetEchoSampling... here [11/11/2011 Administrator]
		if (IsStress(pRcp)) {
			ShowStressButton(TRUE);
		}
		else {
			ShowStressButton(FALSE);
		}
		m_Recipe = pRcp->RcpeName;
		UpdateData(FALSE);
	}
}

void CProcJobPWC413Dlg::OnInspecrecipe() {
	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBINSPECTRECIPE413, ID, (long)m_Recipe.GetBuffer(0));
}

void CProcJobPWC413Dlg::OnProcessing() {
	if (!CanProcess()) {
		return;
	}
}

void CProcJobPWC413Dlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CProcJobPWC413Dlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CProcJobPWC413Dlg::CanProcess() {
	PostMessage(ID_IPC_TVDATAACK, 0, NULL);
	return TRUE;
}

void CProcJobPWC413Dlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_START) {
		KillTimer(IDT_START);
		switch (pGDev->pGEMObjs->GetLineMode()) {
		case EqpStatus::REMOTE:
			// 			if (!pUDev->pProcJobInfo[ID].bRemoteOverride)
			// 			{
			break;
			// 			}
						// no break intended [9/12/2007 LYF]
		default:
		{
			CRecipe413* pRcp = &pUDev->pProcJobInfo[ID].Cas.Recipe413;
			if (p413App->LoadRecipe(pRcp)) {
				UpdateData(FALSE);
				bRecipeSelected = TRUE;
				if (IsStress(pRcp)) {
					ShowStressButton(TRUE);
				}
				else {
					ShowStressButton(FALSE);
				}
				m_Recipe = pRcp->RcpeName;
				UpdateData(FALSE);
				// Do not execute ResetEchoSampling... [11/11/2011 Administrator]
			}
			else {
				Log("Could not load recipe", 2);
			}
		}
		break;
		}
	}
	if (nIDEvent == IDT_TVDATAREQ_TIMEOUT) {
		KillTimer(IDT_TVDATAREQ_TIMEOUT);
		if ((pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE) /*|| (pUDev->pProcJobInfo[ID].bRemoteOverride)*/) {
			AfxMessageBox("413 Module is not communicating\nCould not start processing");
		}
	}
}

BOOL CProcJobPWC413Dlg::OnInitDialog() {
	CProcJobPWC1DlgBase::OnInitDialog();

	if ((ID < 0) || (ID > 3)) {
		ASSERT(0);
		return FALSE;
	}

	if (!m_Parent || !pGDev || !pUDev) {
		ASSERT(0);
		return FALSE;
	}

	AddAnchor(IDC_PROCESSING, TOP_RIGHT);
	AddAnchor(IDC_BROWSERECIPE, TOP_RIGHT);
	AddAnchor(IDC_INSPECRECIPE, TOP_RIGHT);
	AddAnchor(IDC_CANCEL, BOTTOM_RIGHT);
	AddAnchor(IDC_FILELOC, BOTTOM_LEFT);

	AddAnchor(IDC_SCANFIRST, TOP_RIGHT);
	AddAnchor(IDC_SCANSECOND, TOP_RIGHT);
	AddAnchor(IDC_SCANMORE, TOP_RIGHT);

	RegisterWithSecsSvr(this, ID);

	// Connect LPSvr and this
	pGDev->RegisterWinWithLPMgr(m_hWnd, ID + 1);

	CString str;
	CWaferInfoMgr* pWMgr = &pUDev->pProcJobInfo[ID].Cas.winfo;
	for (int i = 0; i < 25; i++) {
		CWaferInfo* p = pWMgr->FindWafer(i + 1);
		if (p) {
			m_cID[i].SetWindowText(p->WaferID);
		}
	}

	m_CarrierID = pUDev->pProcJobInfo[ID].Cas.JobInfo.CarrierID;
	str.Format("Carrier ID: %s", m_CarrierID);
	m_cCarrierID.SetWindowText(str.GetBuffer(0));

	SetSlotMapInfo(SlotMap);

	p413App->Global.WaferRepeat = 0;
	p413App->Global.CassetteRepeat = 0;

	SetTimer(IDT_START, 200, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CProcJobPWC413Dlg::OnIpcTvdataack(WPARAM wP, LPARAM lP) {
	// bypass distributed model [8/5/2010 XPMUser]
	UserSelectionCheck();
	return 0;

	KillTimer(IDT_TVDATAREQ_TIMEOUT);
	// Check returned data
	CToolVar SV;
	BOOL bCanContinue = FALSE;
	if (lP) {
		CMsgItem* p = (CMsgItem*)lP;
		if (p->DataTyp == CMsgItem::DATAREQACK) {
			char* q = p->pMsg;
			if (q) {
				SV.Conv(&q);
				// Crack message here
				switch (SV.OpMode) {
				case SArams::STANDBY:
				case SArams::ENGINEERING:
				case SArams::NONSCHEDULED:
					bCanContinue = TRUE;
					break;
				}
			}
		}
		delete p;
	}

	CString errStr0 = "413 Module is busy\nCould not proceed";
	CString errStr1 = "Abnormal Condition\n413 Module is down\nCould not proceed\nManual recovery of 413 module may be necessary";
	if (!bCanContinue) {
		if ((pGDev->pGEMObjs->GetLineMode() == EqpStatus::OFFLINEEQP) ||
			(pGDev->pGEMObjs->GetLineMode() == EqpStatus::OFFLINEHOST) /*|| (pUDev->pProcJobInfo[ID].bRemoteOverride)*/) {
			if (SV.OpMode == SArams::PRODUCTIVE) {
				AfxMessageBox(errStr0);
			}
			else {
				AfxMessageBox(errStr1);
				C413Obj* p413 = pGDev->pGEMObjs->StMd.Get413Obj();
				p413->Service = C413Obj::OUTOFSERVICE;
			}
		}
		else  // This is local or remote mode [10/27/2008 Yuen]
		{
			if (SV.OpMode == SArams::PRODUCTIVE) {
				Log(errStr0, 1);
			}
			else {
				Log(errStr1, 1);
				C413Obj* p413 = pGDev->pGEMObjs->StMd.Get413Obj();
				p413->Service = C413Obj::OUTOFSERVICE;
				pGDev->SetAlarm(CAlrmList::C413OUTOFSERVICE, CEIDCtlr.m_alarm.Alarms[CAlrmList::C413OUTOFSERVICE].alTX);
			}
		}
		return -1;
	}

	UserSelectionCheck();
	return 0;
}

LRESULT CProcJobPWC413Dlg::OnIpcTvdatanak(WPARAM wP, LPARAM lP) {
	KillTimer(IDT_TVDATAREQ_TIMEOUT);
	if ((pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE) /*|| (pUDev->pProcJobInfo[ID].bRemoteOverride)*/) {
		AfxMessageBox("Could not proceed");
	}
	return 0;
}

void CProcJobPWC413Dlg::UserSelectionCheck() {
	// Check user input
	UpdateData();

	int i;
	BOOL bOK = FALSE;
	for (i = 0; i < 25; i++) {
		if (m_sl[i]) {
			bOK = TRUE;
		}
	}
	if (!bOK) {
		if ((pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE) /*|| (pUDev->pProcJobInfo[ID].bRemoteOverride)*/) {
			AfxMessageBox("No wafer selected for processing");
		}
		return;
	}

	// proceed to next sequence of action [8/5/2010 XPMUser]
	ProceedOK();
}

void CProcJobPWC413Dlg::ProceedOK() {
	CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
	if (!pInfo) {
		ASSERT(0);
		return;
	}
	SendToQueue();
}

void CProcJobPWC413Dlg::SendToQueue() {
	int i;
	CString str;
	if (m_Parent) {
		UpdateData();
		CProcJobInfo* pInfo = &pUDev->pProcJobInfo[ID];
		pInfo->Cas.winfo.SetLPStation(ID + 1, ID + 1);
		for (i = 0; i < 25; i++) {
			if (m_csl[i].GetCheck() == BST_CHECKED) {
				CWaferInfo* p = pInfo->Cas.winfo.FindWafer(i + 1);
				if (!p) {
					p = new CWaferInfo;
					if (p) {
						p->wafernum = i + 1;	// This is must because winfo.Add() will compare it
						pInfo->Cas.winfo.Add(p, FALSE);
					}
				}
				if (p) {
					p->bSelected = TRUE;
					m_cID[i].GetWindowText(str);
					p->WaferID = str;
					p->wafernum = i + 1;
				}
			}
			else {
				CWaferInfo* p = pInfo->Cas.winfo.FindWafer(i + 1);
				if (p) {
					p->bSelected = FALSE;
				}
			}
		}

		CNCas* pCas = &pInfo->Cas;

		// Setup Documentation
		m_cLotID.GetWindowText(pCas->JobInfo.batchname);
		m_cOperator.GetWindowText(pCas->JobInfo.operatorname);

		ShowWindow(SW_HIDE);
		int type = 3; // For 413 [6/28/2010 Yuen]
		m_Parent->PostMessage(UM_TO_PROCJOBQUEUED, ID, type);
	}
}

void CProcJobPWC413Dlg::OnSelectall() {
	int i;
	UpdateData(TRUE);
	if (m_SelectAll) {
		m_cSelectAll.SetWindowText("Deselect All");
		for (i = 0; i < 25; i++) {
			if (m_slEn[i]) {
				m_sl[i] = TRUE;
			}
		}
	}
	else {
		m_cSelectAll.SetWindowText("Select All");
		for (i = 0; i < 25; i++) {
			m_sl[i] = FALSE;
		}
	}
	UpdateData(FALSE);
}

void CProcJobPWC413Dlg::SetSlotMapInfo(char* p) {
	pGDev->pGEMObjs->SetCOSlotmapStatus(m_CarrierID, ID + 1, p, CCarrierObj::SMVERIFICATIONOK); // Offline mode [12/24/2007 YUEN]
	//[[20240820/Arup
	//p413App->Global.LoadMaxSlot();
	int nSlot = p413App->Global.SlotsPerFOUP;
	for (int i = 0; i < 25; i++) {
		m_sl[i] = FALSE;
		m_slEn[i] = FALSE;
		m_cID[i].EnableWindow(FALSE);
		m_csl[i].EnableWindow(FALSE);
		if (i >= nSlot) {
			m_cID[i].ShowWindow(FALSE);
			m_csl[i].ShowWindow(FALSE);
		}
	}
	for (int i = 0; i < nSlot; i++)
	{
		if (*(p + i) == '3') {	// 1=NO OCCUPIED, 3=OCCUPIED
			m_sl[i] = TRUE;
			m_slEn[i] = TRUE;
			m_cID[i].EnableWindow();
			m_csl[i].EnableWindow();
		}
	}

	//if (nSlot == 13)
	//{
	//	for (int i = 0; i <= 25; i = i + 2) {
	//		if (*(p + i) == '3') {	// 1=NO OCCUPIED, 3=OCCUPIED
	//			m_sl[i] = TRUE;
	//			m_slEn[i] = TRUE;
	//			m_cID[i].EnableWindow();
	//			m_csl[i].EnableWindow();
	//		}
	//		//else {
	//		//	m_sl[i] = FALSE;
	//		//	m_slEn[i] = FALSE;
	//		//	m_cID[i].EnableWindow(FALSE);
	//		//	m_csl[i].EnableWindow(FALSE);
	//		//}
	//	}
	//}
	//else
	//{
	//	for (int i = 0; i < 25; i++) {
	//		if (*(p + i) == '3') {	// 1=NO OCCUPIED, 3=OCCUPIED
	//			m_sl[i] = TRUE;
	//			m_slEn[i] = TRUE;
	//			m_cID[i].EnableWindow();
	//			m_csl[i].EnableWindow();
	//		}
	//		//else {
	//		//	m_sl[i] = FALSE;
	//		//	m_slEn[i] = FALSE;
	//		//	m_cID[i].EnableWindow(FALSE);
	//		//	m_csl[i].EnableWindow(FALSE);
	//		//}
	//	}
	//}
	////for (int i = 0; i < 25; i++) {
	////	if (*(p + i) == '3') {	// 1=NO OCCUPIED, 3=OCCUPIED
	////		m_sl[i] = TRUE;
	////		m_slEn[i] = TRUE;
	////		m_cID[i].EnableWindow();
	////		m_csl[i].EnableWindow();
	////	}
	////	else {
	////		m_sl[i] = FALSE;
	////		m_slEn[i] = FALSE;
	////		m_cID[i].EnableWindow(FALSE);
	////		m_csl[i].EnableWindow(FALSE);
	////	}
	////}
	////]]
	UpdateData(FALSE);
}

void CProcJobPWC413Dlg::OnScanfirst() {
	CRecipe413* pRcp = &pUDev->pProcJobInfo[ID].Cas.Recipe413;
	pRcp->ScanNo = 0;
	OnProcessing();
}

void CProcJobPWC413Dlg::OnScansecond() {
	CRecipe413* pRcp = &pUDev->pProcJobInfo[ID].Cas.Recipe413;
	pRcp->ScanNo = 1;
	OnProcessing();
}

void CProcJobPWC413Dlg::OnScanmore() {
	CRecipe413* pRcp = &pUDev->pProcJobInfo[ID].Cas.Recipe413;
	pRcp->ScanNo = 2;	// Temporary set to 3 [2/28/2012 Administrator]
	OnProcessing();
}

void CProcJobPWC413Dlg::ShowStressButton(BOOL bShow) {
	if (bShow) {
		m_cProceed.ShowWindow(SW_HIDE);
		m_cScanFirst.ShowWindow(SW_SHOW);
		m_cScanSecond.ShowWindow(SW_SHOW);
		m_cScanMore.ShowWindow(SW_SHOW);
	}
	else {
		m_cProceed.ShowWindow(SW_SHOW);
		m_cScanFirst.ShowWindow(SW_HIDE);
		m_cScanSecond.ShowWindow(SW_HIDE);
		m_cScanMore.ShowWindow(SW_HIDE);
	}
}

BOOL CProcJobPWC413Dlg::IsStress(CRecipe413* pRcp) {
	for (int j = 0; j < MAXMEPASET; j++) {
		if (pRcp->IsStress(j)) {
			return TRUE;
		}
	}
	return FALSE;
}

void CProcJobPWC413Dlg::OnKillfocusWaferrepeat() {
	UpdateData(TRUE);
}

void CProcJobPWC413Dlg::OnKillfocusCassetterepeat() {
	UpdateData(TRUE);
}

void CProcJobPWC413Dlg::PostNcDestroy() {
	CProcJobPWC1DlgBase::PostNcDestroy();
	delete this;
}