// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "strSOp.h"
#include "E95.h"

#include "128/GEM/ObjectsContainer.h"
#include "413App.h"
#include "E95View.h"
#include "FEM/Rbt/RbtMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelAlarms.h"
#include "InfoPanelEqpMessage.h"
#include "InfoPanelHelp.h"
#include "InfoPanelHelpHTML.h"
#include "InfoPanelHostMsg.h"
#include "InfoPanelLogs.h"
#include "InfoPanelMain.h"
#include "InfoPanelRecipes.h"
#include "InfoPanelSetup.h"
#include "InfoPanelSystem.h"
#include "InfoPanelView.h"
#include "InfoPanelWelcome.h"
#include "IPC/SMCHdr.h"
#include "IssueAlarmDlg.h"
#include "MainFrm.h"
#include "MotionControl.h"
#include "NavigationPanel.h"
#include "ONLINE/GEM/CediListCtroller.h"
#include "ONLINE/GEM/CtrlJobMgr.h"
#include "ONLINE/GEM/GemFactory.h"
#include "ONLINE/MFC/getpw.h"
#include "ONLINE/MFC/vardisp.h"
#include "ProcJobDlg.h"
#include "ProcJobPWC1DlgBase.h"
#include "ProcJobUnloadCarrierDlg.h"
#include "SERIAL/BEME95.h"
#include "SERIAL/MsgItem.h"
#include "SetupProp3Dlg.h"
#include "SRC/DOSUtil.h"
#include "SRC/LogInDataNew.h"
#include "ST_SplitterWnd.h"
#include "SYS/CUMMsg.h"
#include "TitlePanel.h"
#include "ViewCassDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define IDT_STARTMGR	9999
#define IDT_SHUTDOWN	9997

// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CPersistSDIFrame)

BEGIN_MESSAGE_MAP(CMainFrame, CPersistSDIFrame)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOL_SWITCHINFOVIEW, OnToolSwitchinfoview)
	ON_COMMAND(ID_OPERATION_SENDNCAS, OnOperationSendncas)
	ON_COMMAND(ID_OPERATION_SENDXFER, OnOperationSendxfer)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(ID_SIMU_FFU_FAIL, OnSimuFfuFail)
	ON_COMMAND(ID_ALTDEL, OnAltdel)
	ON_WM_DESTROY()
	ON_COMMAND(ID_GENCEIDLIST, OnGenceidlist)
	ON_COMMAND(ID_GENECIDLIST, OnGenecidlist)
	ON_COMMAND(ID_GENSVIDLIST, OnGensvidlist)
	ON_COMMAND(ID_ALARM_ACCESSVIOLATION, OnAlarmAccessviolation)
	ON_COMMAND(ID_GENALRMLIST, OnGenalrmlist)
	ON_COMMAND(ID_OPERATION_LIST_CAOBJS, OnOperationListCaobjs)
	ON_COMMAND(ID_OPERATION_LIST_LPOBJS, OnOperationListLpobjs)
	ON_COMMAND(ID_GENCEIDPARFILE, OnGenceidparfile)
	ON_COMMAND(ID_GENPROGRAM, OnGenprogram)
	ON_COMMAND(ID_ISSUEALARM, OnIssuealarm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction
CMainFrame::CMainFrame()
	: m_pSplitterWnd1(NULL)
	, m_pSplitterWnd2(NULL) {}

CMainFrame::~CMainFrame() {}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CPersistSDIFrame::OnCreate(lpCreateStruct) == -1)
		return -1;
	EnableDocking(CBRS_ALIGN_ANY);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if (!CPersistSDIFrame::PreCreateWindow(cs))
		return FALSE;

	if (Var.UIMode == 0) {
		cs.hMenu = NULL;
		cs.style = (WS_POPUPWINDOW);
		//		cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE /*| WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX*/ | WS_MAXIMIZE;
	}
	else {
		//		cs.style= (WS_POPUPWINDOW);
		//		cs.style&= (~WS_CAPTION);
		//		cs.style&= (~WS_DLGFRAME);
				//	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE;
	}

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
	CPersistSDIFrame::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
	CPersistSDIFrame::Dump(dc);
}

#endif //_DEBUG

// CMainFrame message handlers
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) {
	// Must start pGDev before the splitter window
	pGDev = new CGlobalDeviceContainer;
	if (!pGDev) {
		Log("GlobalDeviceContainer create fail");
		ASSERT(0);
		return FALSE;
	}
	pGDev->Initialize();

	pUDev = new CGlobalUIContainer;
	if (!pUDev) {
		Log("GlobalUIContainer create fail");
		ASSERT(0);
		return FALSE;
	}
	pUDev->Initialize();

	m_pSplitterWnd1 = new ST_SplitterWnd();
	if (!m_pSplitterWnd1) {
		return CPersistSDIFrame::OnCreateClient(lpcs, pContext);
	}

	// Split into 3 horizontal pane
	m_pSplitterWnd1->Create(this, RUNTIME_CLASS(CTitlePanel), NULL, pContext, false);
	m_pSplitterWnd2 = m_pSplitterWnd1->AddSubDivision(BOTTOM_SIDE, NULL, RUNTIME_CLASS(CNavigationPanel), pContext, false);
	int cnt = 0;
	CString htype = DosUtil.GetHelpType();
	htype.MakeUpper();
	if (htype == "PDF") {
		m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelHelp), pContext);
	}
	else {
		m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelHelpHTML), pContext);
	}
	// Last object instantiated will gets executed first [6/30/2012 user]
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelAlarms), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelHostMsg), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelEqpMessage), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelView), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelLogs), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelSetup), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelRecipes), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelSystem), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelMain), pContext);
	m_nViewNo[cnt++] = m_pSplitterWnd2->AddView(TOP_SIDE, RUNTIME_CLASS(CInfoPanelWelcome), pContext);

	m_pSplitterWnd1->SetInitialStatus();

	//SetTimer(IDT_STARTMGR,500,NULL);	// Move to InfoPanelWelcome [6/30/2012 user]

	return TRUE;
}

void CMainFrame::OnToolSwitchinfoview() {
	m_pSplitterWnd2->SwitchToView(0);
}

void CMainFrame::SwitchPane(short PaneNum) {
	if (PaneNum != 6) {
		if (p413App->pMCtrl) {
			p413App->pMCtrl->ShowWindow(SW_HIDE);
			p413App->pMCtrl->HideWaferMap();
			if (pUDev->pInfoPanelSetup) {
				if (pUDev->pInfoPanelSetup->m_Prop3Dlg) {
					pUDev->pInfoPanelSetup->m_Prop3Dlg->StopEcho();
				}
			}
		}
	}
	m_pSplitterWnd2->SwitchToView(0, PaneNum);
}

// Test function [7/4/2010 Yuen]
void CMainFrame::OnOperationSendncas() {
	ASSERT(0);	// Obsoleted [3/17/2012 user]
#if 0
	CNCas Cas;

	// Job Info Section
	Cas.JobInfo.LPUnit = 1;
	Cas.JobInfo.batchname = "Batch";
	Cas.JobInfo.CarrierID = "CarrierID";
	Cas.JobInfo.cassettename = "CassetteName";
	Cas.JobInfo.comment = "This is JobInfo File";
	Cas.JobInfo.projectname = "ProjectName";
	Cas.JobInfo.time = 0;

	// Recipe Section
	strcpy(Cas.Recipe413.RcpeName, "413");

	// Wafer Info Section
	Cas.winfo.Clear();
	Cas.winfo.SetLPStation(1, 1);
	CWaferInfo W;
	W.Set("Wafer1", 1, 1000.f, 0.725f, 0);
	Cas.winfo.Add(&W, TRUE);
	W.Set("Wafer3", 3, 1000.f, 0.725f, 0);
	Cas.winfo.Add(&W, TRUE);

	CRGAData* pDat;
	pDat = new CRGAData;
	float* pressure = new float[100];
	memset(pressure, 9, sizeof(float) * 100);
	pDat->Alloc(100);
	pDat->Add(100, pressure);
	delete[] pressure;
	pDat->Entry.opmode = 1;
	pDat->Entry.linenum = 2;
	pDat->Entry.temp = 3;
	pDat->Entry.time = CTime::GetCurrentTime();
	pDat->Entry.wafernum = 4;
	Cas.rga.Add(pDat);

	CScanData* pScan = new CScanData;
	pScan->comment = "No comment";
	pScan->FakeData(200);
	CString str1 = "ID";
	CString str2 = "LotID";
	pScan->itm.Set(1, 2, 3, ScanParam::LINETYPE, ScanParam::HTMODE, 1, str1, str2);
	pScan->sp.Set(99, 500, 9, 399, 400);
	Cas.scan.AddScanData(pScan);
#endif
}

void CMainFrame::OnOperationSendxfer() {
	if (pGDev->pRbtMgr) {
		CSMOL SM;
		CSMClt ipc;
		SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
		SM.Pm.act = PrcsMgrServices::XFER;
		strscpy(SM.Pm.text, TEXTLEN, "XFER ARM A 1 SLOT 1 1 SLOT 1");
		if (ipc.Start1(pGDev->pRbtMgr->SvrName, sizeof(CSMOL))) {
			if (!ipc.Send(&SM)) {
			}
		}
		ipc.Destroy();
	}
}

UINT CMainFrame::OnNcHitTest(CPoint point) {
	switch (Var.UIMode) {
	case 0:
		UINT hit = CPersistSDIFrame::OnNcHitTest(point);
		if (hit == HTCAPTION) {
			return 0;
		}
		return hit;
		break;
	}
	return CPersistSDIFrame::OnNcHitTest(point);
}

void CMainFrame::OnAppExit() {
	PostMessage(WM_CLOSE);
}

void CMainFrame::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "MFrame");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_SHUTDOWN) {
		KillTimer(IDT_SHUTDOWN);
		CPersistSDIFrame::OnClose();
	}
}

void CMainFrame::OnClose() {
	if ((pGDev->GetARAMSState() == SArams::PRODUCTIVE) && (p413App->Global.mode <= 1)) {
		if (AfxMessageBox("Quiting now may cause the software to crush.\nContinue ?", MB_YESNO) != IDYES) {
			return;
		}
	}
	// 20230512 - Activate Login for all user mode
	//if (Var.UIMode <= 1) {	
		if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::RUNRCP) {
			short AuthenLevel;
			//if (!pGDev->AuthenticateUser(AuthenLevel)) {
			//	return;
			//}
			pUDev->pTitlePanel->AskLogin();
			if (pUDev->pTitlePanel->AuthenticateLevel < CLogInDataNew::RUNRCP) {
				return;
			}
		}

		
		if (AfxMessageBox("Do you want to exit the software, Proceed?", MB_YESNO) == IDNO)
		{
			return;
		}
		pUDev->pTitlePanel->LogUserInfo(pUDev->pTitlePanel->AuthenticateLevel, CLogInDataNew::LOGOUT);

		CString str;
		str.Format("User %s exited the application", pGDev->username);
		Log(str);
	//}

	if (m_pSplitterWnd1) {
		delete m_pSplitterWnd1;
		m_pSplitterWnd1 = NULL;
	}

	CPersistSDIFrame::OnClose();
}

void CMainFrame::OnSimuFfuFail() {
	// 	static BOOL bOnScreen = FALSE;
	// 	CFFUFailDlg * pFFUFailDlg = new CFFUFailDlg;
	// 	if (pFFUFailDlg)
	// 	{
	// 		pFFUFailDlg->bOnScreen = &bOnScreen;
	// 		pFFUFailDlg->Create(CFFUFailDlg::IDD);
	// 		pFFUFailDlg->ShowWindow(SW_SHOW);
	// 	}
}

void CMainFrame::OnAltdel() {
	AfxMessageBox("Nop");
}

void CMainFrame::OnDestroy() {
	CPersistSDIFrame::OnDestroy();
}

void CMainFrame::OnGenceidlist() {
	FILE* fp;
	string data;

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "\\CEIDRPT.TXT";
	fp = fopen(dir, "wb");
	if (fp) {
		for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
			CVarDisp::MakeCEIDReport(i, data);
			fprintf(fp, "[%d] %s\r\n\r\n", i + 1, data.c_str());
		}
		fclose(fp);
	}
}

void CMainFrame::OnGenecidlist() {
	FILE* fp;
	string data;

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "\\ECIDRPT.TXT";
	fp = fopen(dir, "wb");
	if (fp) {
		SVID_LIST* p = &CEIDCtlr.m_SVList;
		for (int i = 0; i < SVID_LIST::LASTSVID; i++) {
			if (p->m_Svid[i].m_bVarType == SVID_ITEM::EC_TYPE) {
				CVarDisp::MakeSVIDReport(p->m_Svid[i].m_svidno, data);
				fprintf(fp, "[%d] %s\r\n\r\n", i + 1, data.c_str());
			}
		}
		fclose(fp);
	}
}

void CMainFrame::OnGensvidlist() {
	FILE* fp;
	string data;

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "\\SVIDRPT.TXT";
	fp = fopen(dir, "wb");
	if (fp) {
		SVID_LIST* p = &CEIDCtlr.m_SVList;
		for (int i = 0; i < SVID_LIST::LASTSVID; i++) {
			if (p->m_Svid[i].m_bVarType != SVID_ITEM::EC_TYPE) {
				CVarDisp::MakeSVIDReport(p->m_Svid[i].m_svidno, data);
				fprintf(fp, "[%d] %s\r\n\r\n", i + 1, data.c_str());
			}
		}

		fclose(fp);
	}
}

void CMainFrame::OnAlarmAccessviolation() {
	int alID = CEIDCtlr.m_alarm.GetAlarmID(CAlrmList::LP1ACCESSMODEVIOLATION);
	if (alID == -1) {
		return;
	}
	CAlrmItem* pA = CEIDCtlr.m_alarm.GetAlarm(alID);
	if (pA) {
		if (pA->alCD & CAlrmItem::SET) {
			CEIDCtlr.m_alarm.ClearAlarm(pA->alID);
		}
		else {
			CEIDCtlr.m_alarm.SetAlarm(pA->alID);
		}
	}
}

void CMainFrame::OnGenalrmlist() {
	FILE* fp;

	CString dir;
	DosUtil.GetProgramDir(dir);
	dir += "\\ALARMRPT.TXT";
	fp = fopen(dir, "wb");
	if (fp) {
		for (int i = 0; i < CAlrmList::LASTALARM; i++) {
			CAlrmItem* pA = &CEIDCtlr.m_alarm.Alarms[i];
			CString str;
			str.Format("ID: %d\r\nDescription: %s\r\nEnabled: %s\r\nSet: %s", pA->alID, pA->alTX, pA->alED ? "YES" : "NO", (pA->alCD & CAlrmItem::SET) ? "YES" : "NO");
			fprintf(fp, "%s\r\n\r\n", str.GetBuffer(0));
		}
		fclose(fp);
	}
}

void CMainFrame::OnOperationListCaobjs() {
	pGDev->pGEMObjs->ListCaObjs();
}

void CMainFrame::OnOperationListLpobjs() {
	pGDev->pGEMObjs->ListLPObjs();
}

BOOL CMainFrame::DestroyWindow() {
	// 	p413App->StopSystem();
	if (pUDev) {
		pUDev->StopUIElements();
		delete pUDev;
		pUDev = NULL;
	}
	if (pGDev) {
		delete pGDev;
		pGDev = NULL;
	}
	return CPersistSDIFrame::DestroyWindow();
}

void CMainFrame::OnGenceidparfile() {
	CString mdir;
	DosUtil.GetProgramDir(mdir);
	mdir += "CEID.PAR";

	FILE* fp;
	fp = fopen(mdir, "wb");
	if (fp) {
		CString str;
		CEID_LIST* p = &CEIDCtlr.m_CEList;
		for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
			CEID_ITEM* q = &p->m_Ceid[i];
			str.Format("%d,\t%s\r\n", q->m_ceid, q->m_cename);
			fprintf(fp, str);
		}
		fclose(fp);
	}
}

void CMainFrame::OnGenprogram() {
	CString mdir;
	DosUtil.GetProgramDir(mdir);
	mdir += "Pomsglst.ss";

	FILE* fp;
	fp = fopen(mdir, "wt");
	if (fp) {
		CString str;
		CString SM = "3311111111111111111111111";
		CString SM1 = "3111111111111111111111111";
		CString SM2 = "1311111111111111111111111";
		CString CarrierID = "1A27S360";
		CString CJob = "CJobA";
		CString PJob1 = "PRJob1";
		CString PJob2 = "PRJob2";

		str = "* Establish Communication Request\nEstabComm: S1F13 OUTPUT W\n<L\n>.\n\n";
		fprintf(fp, str);

		str = "* Request Off-line\nGoOffLine: S1F15 OUTPUT W .\n\n";
		fprintf(fp, str);

		str = "* Request On-line\nGoOnLine: S1F17 OUTPUT W .\n\n";
		fprintf(fp, str);

		str = PWCID(CarrierID, 1, "LOTA", "A");
		fprintf(fp, str);

		// 		str = PWCID(CarrierID,2, "LOTB", "B");
		// 		fprintf(fp,str);

		str = PWCSM(CarrierID, 1, SM);
		fprintf(fp, str);

		// 		str = PWCSM(CarrierID,2,SM);
		// 		fprintf(fp,str);

		str = CreatePRJobMulti(CarrierID, SM1, SM2);
		fprintf(fp, str);

		str = CreateCJob(CJob, CarrierID);
		fprintf(fp, str);

		str = StartCJob(CJob);
		fprintf(fp, str);

		str = GenRemoteCmd(SVID_LIST::REMOTERELEASEPORT1);
		fprintf(fp, str);

		str = PauseCJob(CJob);
		fprintf(fp, str);

		str = ResumeCJob(CJob);
		fprintf(fp, str);

		str = CancelCJob(CJob);
		fprintf(fp, str);

		str = StopCJob(CJob);
		fprintf(fp, str);

		str = AbortCJob(CJob);
		fprintf(fp, str);

		str = CancelCarrieerAtPort(1);
		fprintf(fp, str);

		str = CancelCarrieerAtPort(2);
		fprintf(fp, str);

		str = CancelCarrier(CarrierID);
		fprintf(fp, str);

		str = DeleteObject(CJob);
		fprintf(fp, str);

		str = DeleteAllCJob();
		fprintf(fp, str);

		str = DeleteAllPJob();
		fprintf(fp, str);

		str = DeleteObject(PJob1);
		fprintf(fp, str);

		str = DeleteObject(PJob2);
		fprintf(fp, str);

		str = PJDequeue(PJob1);
		fprintf(fp, str);

		str = PJDequeue(PJob2);
		fprintf(fp, str);

		str = AllEquipStatusReq();
		fprintf(fp, str);

		str = EquipStatusReq();
		fprintf(fp, str);

		str = AllStatusVarNameListReq();
		fprintf(fp, str);

		str = "* Define Report\n";
		str += "DeleteAllRptIDs: S2F33 OUTPUT W\n";
		str += "<L [2]\n";
		str += "\t<U2 0>\n";
		str += "\t<L>\n";
		str += ">.\n\n";
		fprintf(fp, str);

		str = DefineReport();
		fprintf(fp, str);

		str = LinkEventReport();
		fprintf(fp, str);

		str = GenRemoteCmd(SVID_LIST::REMOTERELEASEPORT2);
		fprintf(fp, str);

		fclose(fp);
	}
}

CString CMainFrame::PWCID(CString CarrierID, short PortID, CString LotID, CString WaferID) {
	CString str, str1;

	//* Proceed With Carrier ID
	//* Min requirement : PTN, ObjID, ContentMap, if no ObjID, CARRIERSPEC must be non zero
	str1.Format("* Proceed With Carrier ID Port %d\n", PortID);
	str1.Format("PWC_ID_%s_%d: S3F17 OUTPUT W\n", CarrierID, PortID);
	str += str1;
	str += "<L\n";
	str += "\t<U4 1> * DATAID\n";
	str += "\t<A 'ProceedWithCarrier'> * CARRIERACTION\n";
	str1.Format("\t<A '%s'> * CARRIER SPEC\n", CarrierID);
	str += str1;
	str1.Format("\t<U1 %d> * PTN Port Number\n", PortID);
	str += str1;
	str += "\t<L * # of Carrier Attributes\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'ContentMap'>\n";
	str += "\t\t\t<L [25] * Must equal to all slots in cassette\n";
	for (int i = 0; i < 25; i++) {
		str += "\t\t\t\t<L [2]\n";
		str1.Format("\t\t\t\t\t<A '%s%d'> * LotID\n", LotID, i + 1);
		str += str1;
		str1.Format("\t\t\t\t\t<A '%s%d'>* Wafer ID, must be non zero length if LotID is specified\n", WaferID, i + 1);
		str += str1;
		str += "\t\t\t\t>\n";
	}
	str += "\t\t\t>\n";
	str += "\t\t>\n";
	str += "\t>\n";
	str += ">.\n\n";

	return str;
}

// SM must be at least 25 char [8/8/2010 XPMUser]
CString CMainFrame::PWCSM(CString CarrierID, short PortID, CString SM) {
	//* Proceed With Carrier SM Port 1
	//* Host has no ability to change tool's SM
	//* Host could either CancelCarrier or Accept it
	CString str, str1;
	str1.Format("* Proceed With Carrier SM Port %d\n", PortID);
	str = str1;
	str1.Format("PWC_SM_%s_%d: S3F17 OUTPUT W\n", CarrierID, PortID);
	str += str1;
	str += "<L [5]\n";
	str += "\t<U4 1>\n";
	str += "\t<A 'ProceedWithCarrier'>\n";
	str1.Format("\t<A '%s'> * CARRIER SPEC\n", CarrierID);
	str += str1;
	str1.Format("\t<U1 %d> * PTN Port Number\n", PortID);
	str += str1;
	str += "\t<L\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'ObjID'>\n";
	str1.Format("\t\t\t<A '%s'>\n", CarrierID);
	str += str1;
	str += "\t\t>\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'Slot map'>\n";
	str += "\t\t\t<L [25]\n";
	for (int i = 0; i < 25; i++) {
		str1.Format("\t\t\t\t<U2 %c>\n", SM.GetAt(i));
		str += str1;
	}
	str += "\t\t\t>\n";
	str += "\t\t>\n";
	str += "\t>\n";
	str += ">.\n\n";

	return str;
}

CString CMainFrame::CreatePRJobMulti(CString CarrierID, CString SM1, CString SM2) {
	CString str, str1;

	str = "* Create Process Job Multi\n";
	str1.Format("PRJob_Multi_%s: S16F15 OUTPUT W\n", CarrierID);
	str += str1;
	str += "<L [2]\n";
	str += "\t<U2 1> * DATAID\n";
	str += "\t<L\n";
	str += CreatePRJob("PRJob1", CarrierID, SM1, "R1", TRUE);
	str += CreatePRJob("PRJob2", CarrierID, SM2, "R1", TRUE);
	str += "\t>\n";
	str += ">.\n\n";

	return str;
}

CString CMainFrame::CreatePRJob(CString PRJobID, CString CarrierID, CString SM, CString Recipe, BOOL bStart) {
	int i;
	CString str, str1;

	str = "\t\t<L [6]\n";
	str1.Format("\t\t\t<A '%s'> * PRJob ID\n", PRJobID);
	str += str1;
	str += "\t\t\t<B 0D> * Material format code: 0D = Carrier\n";
	str += "\t\t\t<L [1] * List of carrier\n";
	str += "\t\t\t\t<L [2]\n";
	str1.Format("\t\t\t\t\t<A '%s'> * Carrier ID\n", CarrierID);
	str += str1;
	str += "\t\t\t\t\t<L\n";
	for (i = 0; i < 25; i++) {
		if (SM.GetAt(i) == '3') {
			str1.Format("\t\t\t\t\t\t<U1 %d>\n", i + 1);
			str += str1;
		}
	}
	str += "\t\t\t\t\t>\n";
	str += "\t\t\t\t>\n";
	str += "\t\t\t>\n";
	str += "\t\t\t<L [3]\n";
	str += "\t\t\t\t<U1 2> * Process Recipe Method\n";
	str1.Format("\t\t\t\t<A '%s'> * Recipe Spec\n", Recipe);
	str += str1;

	str += "\t\t\t\t<L\n";
	for (i = 0; i < 25; i++) {
		if (SM.GetAt(i) == '3') {
			str += "\t\t\t\t\t<L [2]\n\t\t\t\t\t\t<A ''>\n\t\t\t\t\t\t<A ''>\n\t\t\t\t\t>\n";
		}
	}
	str += "\t\t\t\t>\n";
	str += "\t\t\t>\n";
	if (bStart) {
		str1.Format("\t\t\t<BOOL T> * PR Process Start\n");
	}
	else {
		str1.Format("\t\t\t<BOOL F> * PR Process Start\n");
	}
	str += str1;
	str += "\t\t\t<L [0]> * PR Pause Event\n";
	str += "\t\t>\n";

	return str;
}

CString CMainFrame::CreateCJob(CString CJobID, CString CarrierID) {
	CString str, str1;

	str = "* Create Control Job\n";
	str1.Format("CJob_Multi_%s: S14F9 OUTPUT W\n", CarrierID);
	str += str1;
	str += "<L [3]\n";
	str += "\t<A 'Equipment'>\n";
	str += "\t<A 'ControlJob'>\n";
	str += "\t<L [7]\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'ObjID'>\n";
	str1.Format("\t\t\t<A '%s'>\n", CJobID);
	str += str1;
	str += "\t\t>\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'ProcessOrderMgmt'>\n";
	str += "\t\t\t<U1 1>\n";
	str += "\t\t>\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'StartMethod'>\n";
	str += "\t\t\t<BOOL F>\n";
	str += "\t\t>\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'CarrierInputSpec'>\n";
	str += "\t\t\t<L\n";
	str1.Format("\t\t\t\t<A '%s'>\n", CarrierID);
	str += str1;
	str += "\t\t\t>\n";
	str += "\t\t>\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'MtrlOutSpec'>\n";
	str += "\t\t\t<L [0]>\n";
	str += "\t\t>\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'ProcessingCtrlSpec'>\n";
	str += "\t\t\t<L [2]\n";
	str += ProcessCtrlSpec("PRJob1");
	str += ProcessCtrlSpec("PRJob2");
	str += "\t\t\t>\n";
	str += "\t\t>\n";
	str += "\t\t<L [2]\n";
	str += "\t\t\t<A 'PauseEvent'>\n";
	str += "\t\t\t<L [0]>\n";
	str += "\t\t>\n";
	str += "\t>\n";
	str += ">.\n\n";

	return str;
}

CString CMainFrame::ProcessCtrlSpec(CString PRJob) {
	CString str, str1;

	str += "\t\t\t\t<L[3]\n";
	str1.Format("\t\t\t\t\t<A '%s'>\n", PRJob);
	str += str1;
	str += "\t\t\t\t\t<L [0]>\n";
	str += "\t\t\t\t\t<L [0]>\n";
	str += "\t\t\t\t>\n";

	return str;
}

CString CMainFrame::CancelCarrieerAtPort(short PortID) {
	CString str, str1;

	str.Format("* Cancel Carrier At Port %d\n", PortID);
	str1.Format("CancelCarrierAtPort_Port%d: S3F17 OUTPUT W\n", PortID);
	str += str1;
	str += "<L [5]\n";
	str += "\t<U4 1> * DATAID\n";
	str += "\t<A 'CancelCarrierAtPort'> * CARRIERACTION\n";
	str += "\t<A ''> * CARRIERSPEC\n";
	str1.Format("\t<U1 %d> * PTN\n", PortID);
	str += str1;
	str += "\t<L> * Carrier Attributes\n";
	str += ">.\n\n";

	return str;
}

CString CMainFrame::CancelCarrier(CString CarrierID) {
	CString str, str1;

	str = "* Cancel Carrier\n";
	str1.Format("CancelCarrier_%s: S3F17 OUTPUT W\n", CarrierID);
	str += str1;
	str += "<L [4]\n";
	str += "\t<U4 1> * DATAID\n";
	str += "\t<A 'CancelCarrier'> * CARRIERACTION\n";
	str1.Format("\t<A '%s'> * CARRIERSPEC\n", CarrierID);
	str += str1;
	str += "\t<U2 0> * PTN\n";
	str += "\t<L > * Carrier Attributes\n";
	str += ">.\n\n";

	return str;
}

CString CMainFrame::StartCJob(CString CJobID) {
	CString str, str1;

	str.Format("StartCJob_%s: S16F27 OUTPUT W\n", CJobID);
	str += CJobAction(CJobID, 1);
	return str;
}

CString CMainFrame::PauseCJob(CString CJobID) {
	CString str, str1;

	str.Format("PauseCJob_%s: S16F27 OUTPUT W\n", CJobID);
	str += CJobAction(CJobID, 2);
	return str;
}

CString CMainFrame::ResumeCJob(CString CJobID) {
	CString str, str1;

	str.Format("ResumeCJob_%s: S16F27 OUTPUT W\n", CJobID);
	str += CJobAction(CJobID, 3);
	return str;
}

CString CMainFrame::CancelCJob(CString CJobID) {
	CString str, str1;

	str.Format("CamcelCJob_%s: S16F27 OUTPUT W\n", CJobID);
	str += CJobAction(CJobID, 4);
	return str;
}

CString CMainFrame::StopCJob(CString CJobID) {
	CString str, str1;

	str.Format("StopCJob_%s: S16F27 OUTPUT W\n", CJobID);
	str += CJobAction(CJobID, 6);
	return str;
}

CString CMainFrame::AbortCJob(CString CJobID) {
	CString str, str1;

	str.Format("AbortCJob_%s: S16F27 OUTPUT W\n", CJobID);
	str += CJobAction(CJobID, 7);
	return str;
}

CString CMainFrame::CJobAction(CString CJobID, short act) {
	CString str, str1;

	str += "<L [3]\n";
	str1.Format("\t<A '%s'> * CTLJOBID\n", CJobID);
	str += str1;
	str1.Format("\t<U1 %d> * CTLJOBCMD\n", act);
	str += str1;
	str += "\t<L [2]\n";
	str += "\t\t<A 'Action'> * CPNAME\n";
	str1.Format("\t\t<U1 %d> * CPVAL\n", act);
	str += str1;
	str += "\t>\n";
	str += ">.\n\n";

	return str;
}

CString CMainFrame::DeleteObject(CString JobID) {
	CString str, str1;

	str = "* Delete Object Request\n";
	str1.Format("DeleteObject_%s: S14F11 OUTPUT W\n", JobID);
	str += str1;
	str += "<L [2]\n";
	str1.Format("\t<A '%s'>\n", JobID);
	str += str1;
	str += "\t<L [0]>\n";
	str += ">.\n\n";
	return str;
}

CString CMainFrame::DeleteAllCJob() {
	CString str, str1;

	str = "* Delete Object Request\n";
	str += "DeleteCJob_All: S14F11 OUTPUT W\n";
	str += "<L [2]\n";
	str += "\t<A 'CONTROLLJOBALL'>\n";
	str += "\t<L [0]>\n";
	str += ">.\n\n";
	return str;
}

CString CMainFrame::DeleteAllPJob() {
	CString str, str1;

	str = "* Delete Object Request\n";
	str += "DeletePJob_All: S14F11 OUTPUT W\n";
	str += "<L [2]\n";
	str += "\t<A 'PROCESSJOBALL'>\n";
	str += "\t<L [0]>\n";
	str += ">.\n\n";
	return str;
}

CString CMainFrame::PJDequeue(CString PJobID) {
	CString str, str1;

	str.Format("PJobDequeue_%s: S16F17 OUTPUT W\n", PJobID);
	str += "<L [1]\n";
	str1.Format("\t<A 'PrJob1'>\n", PJobID);
	str += str1;
	str += ">.\n\n";
	return str;
}

CString CMainFrame::AllEquipStatusReq() {
	CString str, str1;

	str = "* All Equipment Status Request\n";
	str += "AllEqpStatusReq: S1F3 OUTPUT W\n";
	str += "<L\n";
	str += ">.	\n\n";
	return str;
}

// Get SVID number and name base on its index and return in string form [4/28/2013 Administrator]
CString CMainFrame::SVIDReq(UINT idx, short tab) {
	CString str = "", str1;
	for (int i = 0; i < tab; i++) {
		str += "\t";
	}
	str1.Format("<U2 %d> * %s\n", CEIDCtlr.m_SVList.m_Svid[idx].m_svidno, CEIDCtlr.m_SVList.m_Svid[idx].m_svname);
	str += str1;
	return str;
}

CString CMainFrame::EquipStatusReq() {
	CString str, sr1;

	str = "* Request LineMode and EqpStats\n";
	str += "ReqEqpStatus: S1F3 OUTPUT W\n";
	str += "<L\n";
	str += SVIDReq(SVID_LIST::LINEMODE, 1);
	str += SVIDReq(SVID_LIST::EQUIPMENTSTATUS, 1);
	str += SVIDReq(SVID_LIST::LP1ACCESSMODE, 1);
	str += SVIDReq(SVID_LIST::LP1OCCUPANCYSTATUS, 1);
	str += SVIDReq(SVID_LIST::LP1CARRIERACCESSSTATUS, 1);
	str += SVIDReq(SVID_LIST::LP1TRANSFERSTATUS, 1);
	str += SVIDReq(SVID_LIST::LP1ASSOCIATESTATUS, 1);
	str += SVIDReq(SVID_LIST::LP2ACCESSMODE, 1);
	str += SVIDReq(SVID_LIST::LP2OCCUPANCYSTATUS, 1);
	str += SVIDReq(SVID_LIST::LP2CARRIERACCESSSTATUS, 1);
	str += SVIDReq(SVID_LIST::LP2TRANSFERSTATUS, 1);
	str += SVIDReq(SVID_LIST::LP2ASSOCIATESTATUS, 1);
	str += SVIDReq(SVID_LIST::EQUIPMENTSTATEENUM, 1);
	str += SVIDReq(SVID_LIST::LINEMODEENUM, 1);
	str += SVIDReq(SVID_LIST::LPOCCUPANCYSTATEENUM, 1);
	str += SVIDReq(SVID_LIST::LPACCESSSTATEENUM, 1);
	str += SVIDReq(SVID_LIST::LPTRANSFERSTATEENUM, 1);
	str += SVIDReq(SVID_LIST::LPASSOCIATESTATEENUM, 1);
	str += ">.\n\n";
	return str;
}

CString CMainFrame::AllStatusVarNameListReq() {
	CString str;

	str = "* All Status Variable Namelist Request\n";
	str += "AllStatusVarNamelistReq: S1F11 OUTPUT W\n";
	str += "<L\n";
	str += ">.\n\n";
	return str;
}

CString CMainFrame::DefineReport() {
	CString str, str1;

	str = "* DefineReport\n";
	str += "DefineReport: S2F33 OUTPUT W\n";
	str += "<L [2]\n";
	str += "\t<U2 3> * DATA ID\n";
	str += "\t<L\n";

	RptItmCnt = 0;
	str += MakeReport10(2);
	str += MakeReport11(2);
	str += MakeReport12(2);
	str += MakeReport16(2);
	str += MakeReport17(2);
	str += MakeReport20(2);
	str += MakeReport24(2);
	str += MakeReport25(2);
	str += MakeReport30(2);
	str += MakeReport34(2);
	str += MakeReport36(2);
	str += MakeReport38(2);
	str += MakeReport300(2);
	str += MakeReport310(2);
	str += MakeReport510(2);
	str += MakeReport520(2);
	str += MakeReport600(2);
	str += MakeReport700(2);
	str += MakeReport750(2);
	str += MakeReport800(2);
	str += MakeReport850(2);
	str += MakeReport900(2);

	str += "\t>\n";
	str += ">.\n\n";

	return str;
}

CString CMainFrame::MakeReport10(short tab) {
	CString str, str1;

	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 10;
	p->Desc = "TIME";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::TIMESTAMP, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport11(short tab) {
	CString str, str1;

	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 11;
	p->Desc = "TIME & LP1 CONDITIONS";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::TIMESTAMP, tab + 2);
	str += SVIDReq(SVID_LIST::LP1ACCESSMODE, tab + 2);
	str += SVIDReq(SVID_LIST::LP1TRANSFERSTATUS, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport12(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 12;
	p->Desc = "TIME & LP2 CONDITIONS";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::TIMESTAMP, tab + 2);
	str += SVIDReq(SVID_LIST::LP2ACCESSMODE, tab + 2);
	str += SVIDReq(SVID_LIST::LP2TRANSFERSTATUS, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport14(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 14;
	p->Desc = "RESULT SINGLE";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::TIMESTAMP, tab + 2);
	str += SVIDReq(SVID_LIST::RSLTSINGLE, tab + 2);

	// 	str += SVIDReq(SVID_LIST::TOTALTHICK,tab+2);
	// 	str += SVIDReq(SVID_LIST::THICKNESS,tab+2);
	// 	str += SVIDReq(SVID_LIST::TRIMWIDTH,tab+2);
	// 	str += SVIDReq(SVID_LIST::TRIMDEPTH,tab+2);
	// 	str += SVIDReq(SVID_LIST::SDITHICKNESS,tab+2);
	// 	str += SVIDReq(SVID_LIST::ODTHICKNESS,tab+2);
	//
	// 	str += SVIDReq(SVID_LIST::SITENUMBER,tab+2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport15(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 15;
	p->Desc = "RESULT WAFER";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::TIMESTAMP, tab + 2);
	str += SVIDReq(SVID_LIST::RSLTWAFER, tab + 2);

	// 	str += SVIDReq(SVID_LIST::WARPAGE,tab+2);
	// 	str += SVIDReq(SVID_LIST::WAFERBOW,tab+2);
	// 	str += SVIDReq(SVID_LIST::STATS,tab+2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport16(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 16;
	p->Desc = "PORT & CARRIER";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTCARRIERID, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTPORTNO, tab + 2);
	str += SVIDReq(SVID_LIST::LASTKNOWNSM, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport17(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 17;
	p->Desc = "JOB PROPERTIES";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTCJID, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTPJID, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport20(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 20;
	p->Desc = "EQUIPMENT CONDITION";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::LINEMODE, tab + 2);
	str += SVIDReq(SVID_LIST::EQUIPMENTSTATUS, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport24(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 24;
	p->Desc = "MEASUREMENT RESULT (POINT)";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::TIMESTAMP, tab + 2);
	str += SVIDReq(SVID_LIST::RSLTSINGLE, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport25(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 25;
	p->Desc = "MEASUREMENT RESULT (WAFER)";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::TIMESTAMP, tab + 2);
	str += SVIDReq(SVID_LIST::RSLTWAFER, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport30(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 30;
	p->Desc = "PROCESS PROPERTIES";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CJMAXQUEUECOUNT, tab + 2);
	str += SVIDReq(SVID_LIST::CJAVAILABLESPACE, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTCJID, tab + 2);
	str += SVIDReq(SVID_LIST::PJMAXQUEUECOUNT, tab + 2);
	str += SVIDReq(SVID_LIST::PJAVAILABLESPACE, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTPJID, tab + 2);
	str += SVIDReq(SVID_LIST::SPOOLCOUNTACTUAL, tab + 2);
	//str += SVIDReq(SVID_LIST::SPOOLCOUNTTOTAL,tab+2);
	str += SVIDReq(SVID_LIST::CURRENTPJSTATE, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTPJSTATESTR, tab + 2);
	str += SVIDReq(SVID_LIST::QUEUEDCJSLIST, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport34(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 34;
	p->Desc = "LOAD PORT PROPERTIES";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::LP1OCCUPANCYSTATUS, tab + 2);
	str += SVIDReq(SVID_LIST::LP2OCCUPANCYSTATUS, tab + 2);
	str += SVIDReq(SVID_LIST::LP1ACCESSMODE, tab + 2);
	str += SVIDReq(SVID_LIST::LP2ACCESSMODE, tab + 2);
	str += SVIDReq(SVID_LIST::LP1TRANSFERSTATUS, tab + 2);
	str += SVIDReq(SVID_LIST::LP2TRANSFERSTATUS, tab + 2);
	str += SVIDReq(SVID_LIST::LP1CARRIERIDATPLACEMENT, tab + 2);
	str += SVIDReq(SVID_LIST::LP2CARRIERIDATPLACEMENT, tab + 2);
	str += SVIDReq(SVID_LIST::LP1CARRIERIDATDOCK, tab + 2);
	str += SVIDReq(SVID_LIST::LP2CARRIERIDATDOCK, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport36(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 36;
	p->Desc = "CHUCK SUBSTRATE LOCATION PROPERTIES";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CHUCKSUBSTLOCSTATE, tab + 2);
	str += SVIDReq(SVID_LIST::CHUCKSUBSTID, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport38(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 38;
	p->Desc = "ALARM INFORMATION";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::ALARMENABLEDLIST, tab + 2);
	str += SVIDReq(SVID_LIST::ALARMSETLIST, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport300(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 300;
	p->Desc = "PJ INFORMATION";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTPJSTATE, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTPJSTATESTR, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTPJID, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport310(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 310;
	p->Desc = "CJ INFORMATION";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTCJSTATE, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTCJSTATESTR, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTCJID, tab + 2);
	str += SVIDReq(SVID_LIST::QUEUEDCJSLIST, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport510(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 510;
	p->Desc = "CARRIER LOAD PORT";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTPORTNO, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTCARRIERID, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport520(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 520;
	p->Desc = "SLOTMAP INFORMATION";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTPORTNO, tab + 2);
	str += SVIDReq(SVID_LIST::CURRENTCARRIERID, tab + 2);
	str += SVIDReq(SVID_LIST::LP1SLOTMAPSTATUS, tab + 2);
	str += SVIDReq(SVID_LIST::LP2SLOTMAPSTATUS, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport600(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 600;
	p->Desc = "LOAD PORT STATUS";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTCARRIERID, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport700(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 700;
	p->Desc = "PORT ID";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTPORTNO, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport750(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 750;
	p->Desc = "LOAD PORT PROPERTIES";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTPORTNO, tab + 2);
	str += SVIDReq(SVID_LIST::LP1OCCUPANCYSTATUS, tab + 2);
	str += SVIDReq(SVID_LIST::LP2OCCUPANCYSTATUS, tab + 2);
	str += SVIDReq(SVID_LIST::LP1ACCESSMODE, tab + 2);
	str += SVIDReq(SVID_LIST::LP2ACCESSMODE, tab + 2);
	str += SVIDReq(SVID_LIST::LP1TRANSFERSTATUS, tab + 2);
	str += SVIDReq(SVID_LIST::LP2TRANSFERSTATUS, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport800(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 800;
	p->Desc = "CARRIER ID";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::CURRENTCARRIERID, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport850(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 850;
	p->Desc = "CARRIER LOCATION";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::LP1CARRIERIDATDOCK, tab + 2);
	str += SVIDReq(SVID_LIST::LP1CARRIERIDATPLACEMENT, tab + 2);
	str += SVIDReq(SVID_LIST::LP2CARRIERIDATDOCK, tab + 2);
	str += SVIDReq(SVID_LIST::LP2CARRIERIDATPLACEMENT, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::MakeReport900(short tab) {
	CString str, str1;
	CString tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";

	SRptItem* p = &RptList[RptItmCnt++];
	p->ID = 900;
	p->Desc = "SUBSTRATE LOCATION";
	str1.Format("\t<U2 %d> * %s\n", p->ID, p->Desc);
	str += tt + str1;

	str += tt + "\t<L\n";

	str += SVIDReq(SVID_LIST::SUBSTID, tab + 2);
	str += SVIDReq(SVID_LIST::SUBSTLOCATIONID, tab + 2);

	str += tt + "\t>\n";
	str += tt + ">\n";

	return str;
}

CString CMainFrame::LinkEventReport() {
	CString str, str1;

	str = "* Link Event Report\n";
	str += "LinkReport: S2F35 OUTPUT W\n";
	str += "<L[2]\n";
	str += "\t<U2 3> * DATAID\n";
	str += "\t<L\n";

	int RptID[16];

	int n;
	n = 0;
	RptID[n++] = 10;
	RptID[n++] = 20;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::ONLINEREMOTE, RptID, 2);
	str += LinkEvent(CEID_LIST::ONLINELOCAL, RptID, 2);
	str += LinkEvent(CEID_LIST::OFFLINEHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::OFFLINEEQUIPMENT, RptID, 2);

	n = 0;
	RptID[n++] = 10;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::OFFLINEHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::OFFLINEEQUIPMENT, RptID, 2);

	n = 0;
	RptID[n++] = 10;
	RptID[n++] = 16;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::CST1NOSTATETOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1IDNOTREADTOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2NOSTATETOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2IDNOTREADTOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1SLOTMAPNOTREADTOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2SLOTMAPNOTREADTOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1WAITINGFORHOSTTOSLOTMAPVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2WAITINGFORHOSTTOSLOTMAPVEROK, RptID, 2);

	n = 0;
	RptID[n++] = 10;
	RptID[n++] = 510;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::LP1CARRIERCLAMPPED, RptID, 2);
	str += LinkEvent(CEID_LIST::LP2CARRIERCLAMPPED, RptID, 2);
	str += LinkEvent(CEID_LIST::LP1CARRIERDOCKED, RptID, 2);
	str += LinkEvent(CEID_LIST::LP2CARRIERDOCKED, RptID, 2);
	str += LinkEvent(CEID_LIST::LP1CARRIEROPENED, RptID, 2);
	str += LinkEvent(CEID_LIST::LP2CARRIEROPENED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM1NOTASSOCIATEDTOASSOCIATED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM2NOTASSOCIATEDTOASSOCIATED, RptID, 2);

	n = 0;
	RptID[n++] = 10;
	RptID[n++] = 17;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::CJSM01NOSTATETOQUEUED, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM02QUEUEDTONOSTATE, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM03QUEUEDTOSELECTED, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM04SELECTEDTOQUEUED, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM05SELECTEDTOEXECUTING, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM06SELECTEDTOWAITINGFORSTART, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM07WAITINGFORSTARTTOEXECUTING, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM08EXECUTINGTOPAUSED, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM09PAUSEDTOEXECUTING, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM10EXECUTINGTOCOMPLETED, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM11ACTIVETOCOMPLETEDSTOP, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM12ACTIVETOCOMPLETEDABORT, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM13COMPLETEDTONOSTATE, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM01NOSTATETOQUEUED, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM02QUEUEDTOSETTINGUP, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM03SETTINGUPTOWAITINGFORSTART, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM04SETTINGUPTOPROCESSING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM05WAITINGFORSTARTTOPROCESSING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM06PROCESSINGTOPROCESSCOMPLETE, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM07PROCESSCOMPLETETONOSTATE, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM08EXECUTINGTOPAUSING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM09PAUSINGTOPAUSED, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM10PAUSETOEXECUTING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM11EXECUTINGTOSTOPPING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM12PAUSETOSTOPPING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM13EXECUTINGTOABORTING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM14STOPPINGTOABORTING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM15PAUSETOABORTING, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM16ABORTINGTONOSTATE, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM17STOPPINGTONOSTATE, RptID, 2);
	str += LinkEvent(CEID_LIST::PJSM18QUEUEDTONOSTATE, RptID, 2);

	n = 0;
	RptID[n++] = 24;
	RptID[n++] = 16;
	RptID[n++] = 17;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::RESULTSINGLE, RptID, 2);

	n = 0;
	RptID[n++] = 25;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::RESULTWAFER, RptID, 2);

	n = 0;
	RptID[n++] = 10;
	RptID[n++] = 310;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::CJSM03QUEUEDTOSELECTED, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM05SELECTEDTOEXECUTING, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM10EXECUTINGTOCOMPLETED, RptID, 2);
	str += LinkEvent(CEID_LIST::CJSM06SELECTEDTOWAITINGFORSTART, RptID, 2);

	n = 0;
	RptID[n++] = 10;
	RptID[n++] = 510;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::CST1NOSTATETOCARRIER, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2NOSTATETOCARRIER, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1NOSTATETOIDNOTREAD, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2NOSTATETOIDNOTREAD, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1NOSTATETOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2NOSTATETOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1NOSTATETOIDVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2NOSTATETOIDVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1NOSTATETOIDVERFAIL, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2NOSTATETOIDVERFAIL, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1IDNOTREADTOIDVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2IDNOTREADTOIDVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1IDNOTREADTOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2IDNOTREADTOWAITINGFORHOST, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1WAITINGFORHOSTTOIDVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2WAITINGFORHOSTTOIDVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1WAITINGFORHOSTTOIDVERFAIL, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2WAITINGFORHOSTTOIDVERFAIL, RptID, 2);

	str += LinkEvent(CEID_LIST::CST1NOSTATETOSLOTMAPNOTREAD, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2NOSTATETOSLOTMAPNOTREAD, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1SLOTMAPNOTREADTOSLOTMAPVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2SLOTMAPNOTREADTOSLOTMAPVEROK, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1WAITINGFORHOSTTOSLOTMAPVERFAIL, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2WAITINGFORHOSTTOSLOTMAPVERFAIL, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1NOTACCESSEDTOINACCESS, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2NOTACCESSEDTOINACCESS, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1INACCESSTOCARRIERCOMPLETE, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2INACCESSTOCARRIERCOMPLETE, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1INACCESSTOCARRIERSTOPPED, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2INACCESSTOCARRIERSTOPPED, RptID, 2);
	str += LinkEvent(CEID_LIST::CST1CARRIERTONOSTATE, RptID, 2);
	str += LinkEvent(CEID_LIST::CST2CARRIERTONOSTATE, RptID, 2);
	str += LinkEvent(CEID_LIST::LPRSTM1NOSTATETONOTRESERVED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPRSTM1NOTRESERVEDTORESERVED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPRSTM1RESERVEDTONOTRESERVED, RptID, 2);

	str += LinkEvent(CEID_LIST::LPRSTM2NOSTATETONOTRESERVED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPRSTM2NOTRESERVEDTORESERVED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPRSTM2RESERVEDTONOTRESERVED, RptID, 2);

	str += LinkEvent(CEID_LIST::LPASM1NOSTATETONOTASSOCIATED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM1ASSOCIATEDTONOTASSOCIATED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM1ASSOCIATEDTOASSOCIATED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM1ASSOCIATEDTONOTASSOCIATED, RptID, 2);

	str += LinkEvent(CEID_LIST::LPASM2NOSTATETONOTASSOCIATED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM2ASSOCIATEDTONOTASSOCIATED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM2ASSOCIATEDTOASSOCIATED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM1ASSOCIATEDTONOTASSOCIATED, RptID, 2);

	str += LinkEvent(CEID_LIST::LPASM1NOSTATETONOTOCCUPIED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM1NOTOCCUPIEDTOOCCUPIED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM1OCCUPIEDTONOTOCCUPIED, RptID, 2);

	str += LinkEvent(CEID_LIST::LPASM2NOSTATETONOTOCCUPIED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM2NOTOCCUPIEDTOOCCUPIED, RptID, 2);
	str += LinkEvent(CEID_LIST::LPASM2OCCUPIEDTONOTOCCUPIED, RptID, 2);

	str += LinkEvent(CEID_LIST::LP1CARRIERARRIVED, RptID, 2);
	str += LinkEvent(CEID_LIST::LP1CARRIERREMOVED, RptID, 2);

	str += LinkEvent(CEID_LIST::LP1LOADCOMPLETE, RptID, 2);
	str += LinkEvent(CEID_LIST::LP2LOADCOMPLETE, RptID, 2);

	n = 0;
	RptID[n++] = 10;
	RptID[n++] = 600;
	RptID[n++] = 700;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::AMSM2NOSTATETOMANUAL, RptID, 2);
	str += LinkEvent(CEID_LIST::AMSM2NOSTATETOAUTO, RptID, 2);
	str += LinkEvent(CEID_LIST::AMSM1MANUALTOAUTO, RptID, 2);
	str += LinkEvent(CEID_LIST::AMSM2AUTOTOMANUAL, RptID, 2);

	n = 0;
	RptID[n++] = 10;
	RptID[n++] = 900;
	RptID[n++] = -1;
	str += LinkEvent(CEID_LIST::SSMNOSTATETOATSOURCE, RptID, 2);
	str += LinkEvent(CEID_LIST::SSMATSOURCETOATWORK, RptID, 2);
	str += LinkEvent(CEID_LIST::SSMATWORKTOATDESTINATION, RptID, 2);
	str += LinkEvent(CEID_LIST::SSMNOSTATETONEEDSPROCESSING, RptID, 2);
	str += LinkEvent(CEID_LIST::SSMNEEDSPROCESSINGTOINPROGRESS, RptID, 2);
	str += LinkEvent(CEID_LIST::SSMINPROCESSTOPROCESSINGCOMPLETE, RptID, 2);

	str += "\t>\n";
	str += ">.\n\n";
	return str;
}

CString CMainFrame::LinkEvent(UINT idx, int RptID[], short tab) {
	CString str, str1, tt = "";

	for (int i = 0; i < tab; i++) {
		tt += "\t";
	}

	str = tt + "<L [2]\n";
	str1.Format("\t<U2 %d> * %s\n", CEIDCtlr.m_CEList.m_Ceid[idx].m_ceid, CEIDCtlr.m_CEList.m_Ceid[idx].m_cename);
	str += tt + str1;
	str += tt + "\t<L\n";

	int n = 0;
	while (RptID[n] != -1) {
		str1.Format("\t\t<U2 %d> * %s\n", RptID[n], GetReportDesc(RptID[n]));
		str += tt + str1;
		n++;
	}
	str += tt + "\t>\n";
	str += tt + ">\n";
	return str;
}

CString CMainFrame::GetReportDesc(short RptID) {
	for (int i = 0; i < RptItmCnt; i++) {
		if (RptList[i].ID == RptID) {
			return RptList[i].Desc;
		}
	}
	return "";
}

CString CMainFrame::GenRemoteCmd(short rcmd) {
	CString str;

	str.Format("RemoteCmd%d: S2F21 OUTPUT W\n<U1 %d>. * %s\n", rcmd, rcmd, CEIDCtlr.m_SVList.m_RCmd[rcmd].Name);

	return str;
}

void CMainFrame::OnIssuealarm() {
	CIssueAlarmDlg dlg;

	dlg.DoModal();
}