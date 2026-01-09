#include "stdafx.h"
#include "strSOp.h"
#include "E95.h"

#include "128/CAS/CasMgr.h"
#include "128/CAS/NCas.h"
#include "128/GEM/ObjectsContainer.h"
#include "413App.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "ProcJob413ProcessingDlg.h"
#include "ProcJobCancelCarrierDlg.h"
#include "ProcJobCompleteDlg.h"
#include "ProcJobCompleteManual.h"
#include "ProcJobDlg.h"
#include "ProcJobInfo.h"
#include "ProcJobManageDlg.h"
#include "ProcJobManDlg.h"
#include "ProcJobManual.h"
#include "ProcJobPWC1DlgBase.h"
#include "ProcJobPWC2Dlg.h"
#include "ProcJobPWC413Dlg.h"
#include "ProcJobQue413Dlg.h"
#include "ProcJobSMICDlg.h"
#include "ProcJobUnloadCarrierDlg.h"
#include "RecipeCreateDlg413.h"
#include "RecoveryDlg.h"
#include "SRC/DOSUtil.h"
#include "ST_SplitterWnd.h"
#include "SYS/CUMMsg.h"

#include "InfoPanelMain.h"

IMPLEMENT_DYNCREATE(CInfoPanelMain, CResizableFormView)

CInfoPanelMain::CInfoPanelMain()
	: CResizableFormView(CInfoPanelMain::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelMain)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

// 	pTmp1 = NULL;
// 	pTmp2 = NULL;
	pTmp3 = NULL;
	pTmp4 = NULL;

	InspectRecipeTabName = "Inspect Recipe";

	if (pUDev) {
		pUDev->pInfoPanelMain = this;
	}
}

CInfoPanelMain::~CInfoPanelMain() {
	for (int i = 0; i < MAX_LPSUPPORTED; i++) {
		pUDev->ClearAll(i);
	}

	pUDev->pInfoPanelMain = NULL;
}

void CInfoPanelMain::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelMain)
	DDX_Control(pDX, IDC_TAB1, m_tabctrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelMain, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelMain)
	ON_WM_TIMER()
	ON_MESSAGE(UM_TO_PROCJOBPWC1, OnToProcJobPWC1)
	ON_MESSAGE(UM_TO_PROCJOBPWC2, OnToProcJobPWC2)
	ON_MESSAGE(UM_TO_PROCJOBCANCELCARRIER, OnToProcJobCancelCarrier)
	ON_MESSAGE(UM_TO_PROCJOBUNLOADING, OnToProcJobUnloadCarrier)
	ON_MESSAGE(UM_TO_PROCJOBUNLOADCOMPLETE, OnToProcJobUnloadComplete)
	ON_MESSAGE(UM_TO_PROCJOBPROCESSING, OnToProcJobProcessing)
	ON_MESSAGE(UM_TO_PROCJOBQUEUED, OnToProcJobQueued)
	ON_MESSAGE(UM_TO_PROCJOBCOMPLETE, OnToProcJobComplete)
	ON_MESSAGE(UM_TO_PROCJOBINSPECTRECIPE413, OnToProcJobInspectRecipe413)
	ON_MESSAGE(UM_TO_PROCJOBRECOVERY, OnToProcJobRecovery)
	ON_MESSAGE(UM_TO_PROCJOBDLG, OnToProcJobDlg)
	ON_MESSAGE(UM_TO_PROCJOBMANUALDLG, OnToProcJobManualDlg)
	ON_MESSAGE(UM_CLOSETAB, OnCloseTab)
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	ON_MESSAGE(UM_CLOSETAB1, OnCloseView)
	ON_MESSAGE(UM_WAFERRECOVERY, OnRobotRecovery)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CInfoPanelMain diagnostics

#ifdef _DEBUG
void CInfoPanelMain::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelMain::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

// CInfoPanelMain message handlers
void CInfoPanelMain::OnInitialUpdate() {
	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	AnchorControls();

	if (Var.nLoadPort > MAX_LPSUPPORTED) {
		ASSERT(0);
		return;
	}

	m_tabctrl.SetMouseOverColor(RGB(255, 0, 0));

	CRect rc(0, 0, 640, 480);
	CWnd* pWnd = GetDlgItem(IDC_TAB1);
	if (pWnd) {
		pWnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
	}

	int i = 0;
	short nTab = 0;
	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		for (i = 0; i < Var.nLoadPort; i++) {
			// 			CString CarrierID, Recipe;
			// 			DosUtil.GetRememberedPrJob(i+1,CarrierID, Recipe);
			// 			if (CarrierID.GetLength() == 0)
			// 			{
			if (pGDev->SubModel != CGlobalDeviceContainer::SMIC) {
				if (pUDev->pProcJobGENDlg[i]) {
					ASSERT(0);
				}
				pUDev->pProcJobGENDlg[i] = new CProcJobDlg;
				if (pUDev->pProcJobGENDlg[i]) {
					pUDev->pProcJobGENDlg[i]->ID = i;
					pUDev->pProcJobGENDlg[i]->pXTab = &m_tabctrl;
					pUDev->pProcJobGENDlg[i]->m_Parent = this;
					pUDev->pProcJobGENDlg[i]->Create(CProcJobDlg::IDD, &m_tabctrl);
					CString str;
					str.Format("Load Port #%d", i + 1);
					m_tabctrl.AddTab(pUDev->pProcJobGENDlg[i], str.GetBuffer(0), nTab++, rc.Width(), rc.Height());
				}
			}
			else {
				if (pUDev->pProcJobSMICDlg[i]) {
					ASSERT(0);
				}
				pUDev->pProcJobSMICDlg[i] = new CProcJobSMICDlg;
				if (pUDev->pProcJobSMICDlg[i]) {
					pUDev->pProcJobSMICDlg[i]->ID = i;
					pUDev->pProcJobSMICDlg[i]->m_Parent = this;
					pUDev->pProcJobSMICDlg[i]->Create(CProcJobSMICDlg::IDD, &m_tabctrl);
					CString str;
					str.Format("SMIF Port #%d", i + 1);
					m_tabctrl.AddTab(pUDev->pProcJobSMICDlg[i], str.GetBuffer(0), nTab++, rc.Width(), rc.Height());
				}
			}
			// 			}
			// 			else
			// 			{
			// 				// This is recovery mode [6/28/2010 Yuen]
			// 				if ((pGDev->SystemModel ==  CGlobalDeviceContainer::FSM413C2C) ||
			// 					(pGDev->SystemModel ==  CGlobalDeviceContainer::FSM413MOT) ||
			// 					(pGDev->SystemModel ==  CGlobalDeviceContainer::FSM413SA))
			// 				{
			// 					RestartPage(i,CarrierID,Recipe);
			// 					if (pUDev->pProcJob413ProcessingDlg[i])
			// 					{
			// 						ASSERT(0);
			// 					}
			// 					//pUDev->pProcJob413ProcessingDlg[i] = new CProcJob413ProcessingDlg;
			// 					if (p413App->Global.Confg.bPatMatchMod || p413App->Global.Confg.bTrimMod)
			// 					{
			// 						if (p413App->Global.Confg.bTrimMod)
			// 						{
			// 							pUDev->pProcJob413ProcessingDlg[i] = new CProcJob413ProcessingDlg(IDD_PROCJOB413PROCESSING);
			// 						}
			// 						else
			// 						{
			// 							if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413SA)
			// 							{
			// 								pUDev->pProcJob413ProcessingDlg[i] = new CProcJob413ProcessingDlg(IDD_PROCJOB413PROCESSING2);
			// 							}
			// 							else
			// 							{
			// 								pUDev->pProcJob413ProcessingDlg[i] = new CProcJob413ProcessingDlg(IDD_PROCJOB413PROCESSING1);
			// 							}
			// 						}
			// 					}
			// 					else
			// 					{
			// 						pUDev->pProcJob413ProcessingDlg[i] = new CProcJob413ProcessingDlg(IDD_PROCJOB413PROCESSING2);
			// 					}
			// 					if (pUDev->pProcJob413ProcessingDlg[i])
			// 					{
			// 						pUDev->pProcJob413ProcessingDlg[i]->ID = i;
			// 						pUDev->pProcJob413ProcessingDlg[i]->bRecovery = /*FALSE*/TRUE;
			// 						pUDev->pProcJob413ProcessingDlg[i]->DispatchTo = CProcJobProcessingB::ECHOMODULE;
			// 						pUDev->pProcJob413ProcessingDlg[i]->m_Parent = this;
			// 						if (p413App->Global.Confg.bPatMatchMod || p413App->Global.Confg.bTrimMod)
			// 						{
			// 							if (p413App->Global.Confg.bTrimMod)
			// 							{
			// 								pUDev->pProcJob413ProcessingDlg[i]->Create(IDD_PROCJOB413PROCESSING,&m_tabctrl);
			// 							}
			// 							else
			// 							{
			// 								pUDev->pProcJob413ProcessingDlg[i]->Create(IDD_PROCJOB413PROCESSING1,&m_tabctrl);
			// 							}
			// 						}
			// 						else
			// 						{
			// 							pUDev->pProcJob413ProcessingDlg[i]->Create(IDD_PROCJOB413PROCESSING2,&m_tabctrl);
			// 						}
			// 						CString str;
			// 						str.Format("Measurement (LP%d)",i+1);
			// 						CWnd *pWnd1 = GetDlgItem(IDC_TAB1);
			// 						if (pWnd1)
			// 						{
			// 							//CRect rc;
			// 							pWnd1->GetWindowRect(&rc);
			// 							ScreenToClient(&rc);
			// 						}
			// 						m_tabctrl.AddTab(pUDev->pProcJob413ProcessingDlg[i],str.GetBuffer(0),nTab++,rc.Width(),rc.Height());
			// 					}
			// 				}
			// 			}
		}
	}
	else {
		pUDev->pProcJobManual = new CProcJobManual;
		if (pUDev->pProcJobManual) {
			pUDev->pProcJobManual->m_Parent = this;
			pUDev->pProcJobManual->Create(CProcJobManual::IDD, &m_tabctrl);
			CString str;
			str = "Measurement";
			CWnd* pWnd1 = GetDlgItem(IDC_TAB1);
			if (pWnd1) {
				pWnd1->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.AddTab(pUDev->pProcJobManual, str.GetBuffer(0), nTab++, rc.Width(), rc.Height());
		}
		//pUDev->pProcJobManDlg = new CProcJobManDlg;
		if (pUDev->pProcJobManDlg) {
			pUDev->pProcJobManDlg->m_Parent = this;
			pUDev->pProcJobManDlg->Create(CProcJobManDlg::IDD, &m_tabctrl);
			CString str;
			str = "Measurement";
			CWnd* pWnd1 = GetDlgItem(IDC_TAB1);
			if (pWnd1) {
				pWnd1->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.AddTab(pUDev->pProcJobManDlg, str.GetBuffer(0), nTab++, rc.Width(), rc.Height());
		}
	}

	// 	if (pUDev->pProcJob413ProcessingDlg[0] && (pUDev->pProcJob413ProcessingDlg[0]->bRecovery == FALSE))
	// 	{
	// 		pUDev->pProcJob413ProcessingDlg[0]->ID = 0;
	// 		pUDev->pProcJob413ProcessingDlg[0]->bRecovery = FALSE;
	// 		pUDev->pProcJob413ProcessingDlg[0]->DispatchTo = CProcJobProcessingB::RTMODULE;
	// 		pUDev->pProcJob413ProcessingDlg[0]->m_Parent = this;
	// 		pUDev->pProcJob413ProcessingDlg[0]->Create(CProcJob413ProcessingDlg::IDD,&m_tabctrl);
	// 		CString str;
	// 		str.Format("Measurement (LP%d)",i+1);
	// 		CWnd *pWnd1 = GetDlgItem(IDC_TAB1);
	// 		if (pWnd1)
	// 		{
	// 			pWnd1->GetWindowRect(&rc);
	// 			ScreenToClient(&rc);
	// 		}
	// 		m_tabctrl.AddTab(pUDev->pProcJob413ProcessingDlg[0],str.GetBuffer(0),nTab++,rc.Width(),rc.Height());
	// 	}

	if (pUDev->pProcJobManageDlg) {
		pUDev->pProcJobManageDlg->Create(CProcJobManageDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(pUDev->pProcJobManageDlg, "Manage Job", nTab++, rc.Width(), rc.Height());
	}

	// Temporary for SW development
//	pTmp1 = new CProcJobRTProcessingDlg;
// 	if (pTmp1) {
// 		pTmp1->m_Parent = this;
// 		pTmp1->ID = 0;
// 		pTmp1->Create(CProcJobRTProcessingDlg::IDD,&m_tabctrl);
// 		m_tabctrl.AddTab(pTmp1,"RT Process (LP1)",nTab++,rc.Width(),rc.Height());
// 	}
//	pTmp2 = new CProcJobHTProcessingDlg;
// 	if (pTmp2) {
// 		pTmp2->m_Parent = this;
// 		pTmp2->ID = 0;
// 		pTmp2->Create(CProcJobHTProcessingDlg::IDD,&m_tabctrl);
// 		m_tabctrl.AddTab(pTmp2,"HT Process (LP1)",nTab++,rc.Width(),rc.Height());
// 	}
//	pTmp3 = new CProcJobPWC1Dlg;
//	pTmp3 = new CProcJobPWC413Dlg;
	if (pTmp3) {
		pTmp3->ID = 0;
		pTmp3->m_Parent = this;
		pUDev->pProcJobInfo[0].Cas.JobInfo.CarrierID = "QWERTY";
		memcpy(pGDev->pGEMObjs->Cms[0].CaObj.SlotMaP, "1111100000111110000011111", MAXCAPACITYLEN);
		//		memcpy(pGDev->pProcJobInfo[0].SlotMap, "1111100000111110000011111",25);
		//		pTmp3->Create(CProcJobPWC1Dlg::IDD,&m_tabctrl);
		pTmp3->Create(CProcJobPWC413Dlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(pTmp3, "Recipe/Slot Info (LP1)", nTab++, rc.Width(), rc.Height());
	}
	// 	pTmp4 = new CProcJobCompleteDlg;
	if (pTmp4) {
		pTmp4->m_Parent = this;
		pTmp4->ID = 0;
		pTmp4->Create(CProcJobCompleteDlg::IDD, &m_tabctrl);
		m_tabctrl.AddTab(pTmp4, "Carrier/Job Complete (LP1)", nTab++, rc.Width(), rc.Height());
	}
}

LRESULT CInfoPanelMain::OnToProcJobPWC1(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	pUDev->ClearAll(ID);
	if (pUDev->pProcJobPWCDlg[ID]) {
		pUDev->pProcJobPWCDlg[ID]->DestroyWindow();
		pUDev->pProcJobPWCDlg[ID] = NULL;
	}

	int ResID;
	ResID = CProcJobPWC413Dlg::IDD;
	pUDev->pProcJobPWCDlg[ID] = new CProcJobPWC413Dlg;

	if (pUDev->pProcJobPWCDlg[ID]) {
		pUDev->pProcJobPWCDlg[ID]->ID = ID;
		pUDev->pProcJobPWCDlg[ID]->m_Parent = this;
		if (lP) {
			memcpy(pUDev->pProcJobPWCDlg[ID]->SlotMap, (char*)lP, 25);
		}
		pUDev->pProcJobPWCDlg[ID]->Create(ResID/*CProcJobPWC1Dlg::IDD*/, &m_tabctrl);
		CString str;
		str.Format("Recipe and Slot Info (LP%d)", ID + 1);
		CRect rc;
		CWnd* pWnd = GetDlgItem(IDC_TAB1);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
		}
		m_tabctrl.ChangeTab(ID, pUDev->pProcJobPWCDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
	}
	return LRESULT(0);
}

LRESULT CInfoPanelMain::OnToProcJobPWC2(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	pUDev->ClearAll(ID);

	if (pUDev->pProcJobPWC2Dlg[ID]) {
		pUDev->pProcJobPWC2Dlg[ID]->DestroyWindow();
		pUDev->pProcJobPWC2Dlg[ID] = NULL;
	}
	pUDev->pProcJobPWC2Dlg[ID] = new CProcJobPWC2Dlg;
	if (pUDev->pProcJobPWC2Dlg[ID]) {
		pUDev->pProcJobPWC2Dlg[ID]->ID = ID;
		pUDev->pProcJobPWC2Dlg[ID]->m_Parent = this;
		pUDev->pProcJobPWC2Dlg[ID]->Create(CProcJobPWC2Dlg::IDD, &m_tabctrl);
		CString str;
		str.Format("Proceed with carrier (LP%d)", ID + 1);
		CRect rc;
		CWnd* pWnd = GetDlgItem(IDC_TAB1);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
		}
		m_tabctrl.ChangeTab(ID, pUDev->pProcJobPWC2Dlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
	}
	return LRESULT(0);
}

LRESULT CInfoPanelMain::OnToProcJobCancelCarrier(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	pUDev->ClearAll(ID);
	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		if (pUDev->pProcJobCancelCarrierDlg[ID]) {
			pUDev->pProcJobCancelCarrierDlg[ID]->DestroyWindow();
			pUDev->pProcJobCancelCarrierDlg[ID] = NULL;
		}
		pUDev->pProcJobCancelCarrierDlg[ID] = new CProcJobCancelCarrierDlg;
		if (pUDev->pProcJobCancelCarrierDlg[ID]) {
			pUDev->pProcJobCancelCarrierDlg[ID]->ID = ID;
			pUDev->pProcJobCancelCarrierDlg[ID]->Next = (UINT)lP;
			pUDev->pProcJobCancelCarrierDlg[ID]->m_Parent = this;
			pUDev->pProcJobCancelCarrierDlg[ID]->Create(CProcJobCancelCarrierDlg::IDD, &m_tabctrl);
			CString str;
			str.Format("Cancel Carrier/Job (LP%d)", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJobCancelCarrierDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
	else {
		if (pUDev->pProcJobCompleteManual) {
			pUDev->pProcJobCompleteManual->DestroyWindow();
			pUDev->pProcJobCompleteManual = NULL;
		}
		pUDev->pProcJobCompleteManual = new CProcJobCompleteManual;
		if (pUDev->pProcJobCompleteManual) {
			pUDev->pProcJobCompleteManual->ID = ID;
			pUDev->pProcJobCompleteManual->m_Parent = this;
			pUDev->pProcJobCompleteManual->Create(CProcJobCompleteManual::IDD, &m_tabctrl);
			CString str;
			str.Format("Carrier/Job Complete (LP%d)", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJobCompleteManual, str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
	return LRESULT(0);
}

LRESULT CInfoPanelMain::OnToProcJobUnloadCarrier(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	pUDev->ClearAll(ID);
	if (pUDev->pProcJobUnloadCarrierDlg[ID]) {
		pUDev->pProcJobUnloadCarrierDlg[ID]->DestroyWindow();
		pUDev->pProcJobUnloadCarrierDlg[ID] = NULL;
	}
	pUDev->pProcJobUnloadCarrierDlg[ID] = new CProcJobUnloadCarrierDlg;
	if (pUDev->pProcJobUnloadCarrierDlg[ID]) {
		pUDev->pProcJobUnloadCarrierDlg[ID]->ID = ID;
		pUDev->pProcJobUnloadCarrierDlg[ID]->m_Parent = this;
		pUDev->pProcJobUnloadCarrierDlg[ID]->Create(CProcJobUnloadCarrierDlg::IDD, &m_tabctrl);
		CString str;
		str.Format("Unload Carrier (LP%d)", ID + 1);
		CRect rc;
		CWnd* pWnd = GetDlgItem(IDC_TAB1);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
		}
		m_tabctrl.ChangeTab(ID, pUDev->pProcJobUnloadCarrierDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
	}
	return LRESULT(0);
}

LRESULT CInfoPanelMain::OnToProcJobUnloadComplete(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	pUDev->ClearAll(ID);
	if (pGDev->SubModel != CGlobalDeviceContainer::SMIC) {
		if (pUDev->pProcJobGENDlg[ID]) {
			pUDev->pProcJobGENDlg[ID]->DestroyWindow();
			pUDev->pProcJobGENDlg[ID] = NULL;
		}
		pUDev->pProcJobGENDlg[ID] = new CProcJobDlg;
		if (pUDev->pProcJobGENDlg[ID]) {
			pUDev->pProcJobGENDlg[ID]->ID = ID;
			pUDev->pProcJobGENDlg[ID]->m_Parent = this;
			pUDev->pProcJobGENDlg[ID]->Create(CProcJobDlg::IDD, &m_tabctrl);
			CString str;
			str.Format("Load Port #%d", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJobGENDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
	else {
		if (pUDev->pProcJobSMICDlg[ID]) {
			pUDev->pProcJobSMICDlg[ID]->DestroyWindow();
			pUDev->pProcJobSMICDlg[ID] = NULL;
		}
		pUDev->pProcJobSMICDlg[ID] = new CProcJobSMICDlg;
		if (pUDev->pProcJobSMICDlg[ID]) {
			pUDev->pProcJobSMICDlg[ID]->ID = ID;
			pUDev->pProcJobSMICDlg[ID]->m_Parent = this;
			pUDev->pProcJobSMICDlg[ID]->Create(CProcJobSMICDlg::IDD, &m_tabctrl);
			CString str;
			str.Format("SMIF Port #%d", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJobSMICDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
	return LRESULT(0);
}

LRESULT CInfoPanelMain::OnToProcJobProcessing(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;
	short type = (short)lP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	CProcJobProcessingB::DISPTOMODULE DispatchTo = CProcJobProcessingB::NOMODULE;
	switch (lP) {
		// 	case 1:
		// 		DispatchTo = CProcJobProcessingB::RTMODULE;
		// 		break;
		// 	case 2:
		// 		DispatchTo = CProcJobProcessingB::HTMODULE;
		// 		break;
	case 3:
		DispatchTo = CProcJobProcessingB::ECHOMODULE;
		break;
	default:
		ASSERT(0);
		break;
	}

	pUDev->ClearAll(ID);
	if (lP == 3) {
		// 413 Cycle
		if (pUDev->pProcJob413ProcessingDlg[ID]) {
			pUDev->pProcJob413ProcessingDlg[ID]->DestroyWindow();
			pUDev->pProcJob413ProcessingDlg[ID] = NULL;
		}
		// 		pUDev->pProcJob413ProcessingDlg[ID] = new CProcJob413ProcessingDlg;
		if (p413App->Global.Confg.bPatMatchMod || p413App->Global.Confg.bTrimMod) {
			if (p413App->Global.Confg.bTrimMod) {
				pUDev->pProcJob413ProcessingDlg[ID] = new CProcJob413ProcessingDlg(IDD_PROCJOB413PROCESSING);
			}
			else {
				if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413SA) {
					pUDev->pProcJob413ProcessingDlg[ID] = new CProcJob413ProcessingDlg(IDD_PROCJOB413PROCESSING2);
				}
				else {
					pUDev->pProcJob413ProcessingDlg[ID] = new CProcJob413ProcessingDlg(IDD_PROCJOB413PROCESSING1);
				}
			}
		}
		else {
			pUDev->pProcJob413ProcessingDlg[ID] = new CProcJob413ProcessingDlg(IDD_PROCJOB413PROCESSING2);
		}
		if (pUDev->pProcJob413ProcessingDlg[ID]) {
			pUDev->pProcJob413ProcessingDlg[ID]->ID = ID;
			pUDev->pProcJob413ProcessingDlg[ID]->DispatchTo = DispatchTo;
			pUDev->pProcJob413ProcessingDlg[ID]->m_Parent = this;
			if (p413App->Global.Confg.bPatMatchMod || p413App->Global.Confg.bTrimMod) {
				if (p413App->Global.Confg.bTrimMod) {
					pUDev->pProcJob413ProcessingDlg[ID]->Create(IDD_PROCJOB413PROCESSING, &m_tabctrl);
				}
				else {
					if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413SA) {
						pUDev->pProcJob413ProcessingDlg[ID]->Create(IDD_PROCJOB413PROCESSING2, &m_tabctrl);
					}
					else {
						pUDev->pProcJob413ProcessingDlg[ID]->Create(IDD_PROCJOB413PROCESSING1, &m_tabctrl);
					}
				}
			}
			else {
				pUDev->pProcJob413ProcessingDlg[ID]->Create(IDD_PROCJOB413PROCESSING2, &m_tabctrl);
			}
			CString str;
			str.Format("Measurement (LP%d)", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJob413ProcessingDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
	return 0;
}

LRESULT CInfoPanelMain::OnToProcJobQueued(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	short type;
	type = (short)lP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		Log("Illegal load port ID (ToProcJobQueued)", 1);
		ASSERT(0);
		return -1;
	}
	DoProcessJobQueued(ID, type);
	return 0;
}

LRESULT CInfoPanelMain::OnToProcJobComplete(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		Log("Illegal load port ID (ToProcJobComplete)", 1);
		ASSERT(0);
		return -1;
	}
	DoProcessComplete(ID);
	return 0;
}

LRESULT CInfoPanelMain::OnToProcJobInspectRecipe413(WPARAM wP, LPARAM lP) {
	short ID = (short)wP;

	if (pUDev->pRecipeCreateDlg413) {
		int idx = m_tabctrl.GetTabIndexByName(InspectRecipeTabName);
		if (idx > -1) {
			m_tabctrl.DeleteTab(idx);
			if (((unsigned int)pUDev->pRecipeCreateDlg413->GetSafeHwnd() != 0xfeeefeee)) {
				pUDev->pRecipeCreateDlg413->DestroyWindow();  // if could not find tab, mean pRecipeCreateDlg413 has been destroyed
			}
			pUDev->pRecipeCreateDlg413 = NULL;
		}
	}
	pUDev->pRecipeCreateDlg413 = new CRecipeCreateDlg413;
	if (pUDev->pRecipeCreateDlg413) {
		pUDev->pRecipeCreateDlg413->bInspect = TRUE;
		pUDev->pRecipeCreateDlg413->ID = ID;
		pUDev->pRecipeCreateDlg413->m_Parent = this;
		pUDev->pRecipeCreateDlg413->Create(CRecipeCreateDlg413::IDD, &m_tabctrl);
		CRect rc(0, 0, 640, 480);
		CWnd* pWnd = GetDlgItem(IDC_TAB1);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
		}
		m_tabctrl.AddTab(pUDev->pRecipeCreateDlg413, InspectRecipeTabName.GetBuffer(0), 99, rc.Width(), rc.Height());
		int idx = m_tabctrl.GetTabIndexByName(InspectRecipeTabName);
		m_tabctrl.SelectTab(idx);
	}
	return 0;
}

LRESULT CInfoPanelMain::OnCloseTab(WPARAM wP, LPARAM lP) {
	CRecipeCreateDlg413* m_ViewRecipeCreate = (CRecipeCreateDlg413*)lP;
	if (m_ViewRecipeCreate) {
		int idx = m_tabctrl.GetTabIndexByName(InspectRecipeTabName);
		if (idx > -1) {
			if (idx > 0) {
				m_tabctrl.SelectTab(idx - 1);
			}
			m_tabctrl.DeleteTab(idx);
			if (((unsigned int)m_ViewRecipeCreate->GetSafeHwnd() != 0xfeeefeee)) {
				m_ViewRecipeCreate->DestroyWindow();	// if could not find tab means m_ViewRecipeCreate has been destroyed
			}
			m_ViewRecipeCreate = NULL;
		}
	}
	return 0;
}

LRESULT CInfoPanelMain::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	m_tabctrl.ActivateCurrentTab();
	return 0;
}

void CInfoPanelMain::PostNcDestroy() {
	// 	if (pTmp1) {
	// 		delete pTmp1;
	// 	}
	// 	if (pTmp2) {
	// 		delete pTmp2;
	// 	}
	// 	if (pTmp3) {
	// 		delete pTmp3;
	// 	}
	// 	if (pTmp4) {
	// 		delete pTmp4;
	// 	}

	CResizableFormView::PostNcDestroy();
	//	delete this;  // Do not add this [6/3/2011 XPMUser]
}

LRESULT CInfoPanelMain::OnRobotRecovery(WPARAM wP, LPARAM lP) {
	int idx;
	static CString ViewName = "";
	if (pUDev->pRecoveryDlg) {
		idx = m_tabctrl.GetTabIndexByName(ViewName.GetBuffer(0));
		if (idx > -1) {
			m_tabctrl.DeleteTab(idx);
			if (((unsigned int)pUDev->pRecoveryDlg->GetSafeHwnd() != 0xfeeefeee)) {
				pUDev->pRecoveryDlg->DestroyWindow();	// if could not find tab means pRecoveryDlg has been destroyed
			}
			pUDev->pRecoveryDlg = NULL;
		}
	}
	//[20211108
	if (pGDev->SystemModel != CGlobalDeviceContainer::FSM413C2C) {
		PostMessage(UM_TO_PROCJOBMANUALDLG, 0, 0);
		return 0;
	}
	//]
	pUDev->pRecoveryDlg = new CRecoveryDlg;
	if (pUDev->pRecoveryDlg) {
		pUDev->pRecoveryDlg->type = wP;	// 1 is 128, 2 is 900, 3 is 413
		pUDev->pRecoveryDlg->m_ParentMain = this;
		pUDev->pRecoveryDlg->m_ParentSystem = NULL;
		pUDev->pRecoveryDlg->Create(CRecoveryDlg::IDD, &m_tabctrl);
		switch (wP) {
		case 1:
			ViewName = "128 Recover";
			break;
		case 2:
			ViewName = "900 Recover";
			break;
		case 3:
			ViewName = "413 Recover";
			break;
		default:
			ASSERT(0);
			break;
		}
		CRect rc(0, 0, 640, 480);
		CWnd* pWnd = GetDlgItem(IDC_TAB1);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
		}
		m_tabctrl.AddTab(pUDev->pRecoveryDlg, ViewName.GetBuffer(0), 1, rc.Width(), rc.Height());
		idx = m_tabctrl.GetTabIndexByName(ViewName.GetBuffer(0));
		m_tabctrl.SelectTab(idx);
	}
	return 0;
}

LRESULT CInfoPanelMain::OnCloseView(WPARAM wP, LPARAM lP) {
	if (!lP) {
		ASSERT(0);
		return -1;
	}
	CRecoveryDlg* pV = (CRecoveryDlg*)lP;
	if (pV) {
		int idx = m_tabctrl.FindTabIndex(pV);
		if (idx > -1) {
			if (idx > 0) m_tabctrl.SelectTab(idx - 1);
			m_tabctrl.DeleteTab(idx);
			if (((unsigned int)pV->GetSafeHwnd() != 0xfeeefeee)) {
				pV->DestroyWindow();
			}
			pUDev->pRecoveryDlg = NULL;
		}
	}
	return 0;
}

LRESULT CInfoPanelMain::OnToProcJobRecovery(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	pUDev->ClearAll(ID);
	if (pUDev->pProcJobRecoveryDlg[ID]) {
		pUDev->pProcJobRecoveryDlg[ID]->DestroyWindow();
		pUDev->pProcJobRecoveryDlg[ID] = NULL;
	}
	//[20211108
	if (pGDev->SystemModel != CGlobalDeviceContainer::FSM413C2C) {
		PostMessage(UM_TO_PROCJOBMANUALDLG, ID, 0);
		return 0;
	}
	//]
	pUDev->pProcJobRecoveryDlg[ID] = new CRecoveryDlg;
	if (pUDev->pProcJobRecoveryDlg[ID]) {
		pUDev->pProcJobRecoveryDlg[ID]->Port = ID + 1;
		pUDev->pProcJobRecoveryDlg[ID]->type = lP;	// 1 is 128, 2 is 900
		pUDev->pProcJobRecoveryDlg[ID]->AckMsg = UM_TO_PROCJOBDLG;
		pUDev->pProcJobRecoveryDlg[ID]->m_ParentMain = this;
		pUDev->pProcJobRecoveryDlg[ID]->m_ParentSystem = NULL;
		pUDev->pProcJobRecoveryDlg[ID]->Create(CRecoveryDlg::IDD, &m_tabctrl);
		CString str = "413 Recover";
		CRect rc(0, 0, 640, 480);
		CWnd* pWnd = GetDlgItem(IDC_TAB1);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
		}
		m_tabctrl.ChangeTab(ID, pUDev->pProcJobRecoveryDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
	}
	return 0;
}

LRESULT CInfoPanelMain::OnToProcJobDlg(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	pUDev->ClearAll(ID);
	if (pGDev->SubModel != CGlobalDeviceContainer::SMIC) {
		if (pUDev->pProcJobGENDlg[ID]) {
			pUDev->pProcJobGENDlg[ID]->DestroyWindow();
			pUDev->pProcJobGENDlg[ID] = NULL;
		}
		pUDev->pProcJobGENDlg[ID] = new CProcJobDlg;
		if (pUDev->pProcJobGENDlg[ID]) {
			pUDev->pProcJobGENDlg[ID]->ID = ID;
			pUDev->pProcJobGENDlg[ID]->m_Parent = this;
			pUDev->pProcJobGENDlg[ID]->Create(CProcJobDlg::IDD, &m_tabctrl);
			CString str;
			str.Format("Load Port #%d", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJobGENDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
	else {
		if (pUDev->pProcJobSMICDlg[ID]) {
			pUDev->pProcJobSMICDlg[ID]->DestroyWindow();
			pUDev->pProcJobSMICDlg[ID] = NULL;
		}
		pUDev->pProcJobSMICDlg[ID] = new CProcJobSMICDlg;
		if (pUDev->pProcJobSMICDlg[ID]) {
			pUDev->pProcJobSMICDlg[ID]->ID = ID;
			pUDev->pProcJobSMICDlg[ID]->m_Parent = this;
			pUDev->pProcJobSMICDlg[ID]->Create(CProcJobSMICDlg::IDD, &m_tabctrl);
			CString str;
			str.Format("SMIF Port #%d", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJobSMICDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
	return 0;
}

LRESULT CInfoPanelMain::OnToProcJobManualDlg(WPARAM wP, LPARAM lP) {
	short ID;
	ID = (short)wP;

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return LRESULT(-1);
	}

	pUDev->ClearAll(ID);
	if (pUDev->pProcJobManual) {
		pUDev->pProcJobManual->DestroyWindow();
		pUDev->pProcJobManual = NULL;
	}
	pUDev->pProcJobManual = new CProcJobManual;
	if (pUDev->pProcJobManual) {
		pUDev->pProcJobManual->ID = ID;
		pUDev->pProcJobManual->m_Parent = this;
		pUDev->pProcJobManual->Create(CProcJobManual::IDD, &m_tabctrl);
		CString str;
		str = "Measurement";
		CRect rc;
		CWnd* pWnd = GetDlgItem(IDC_TAB1);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
		}
		m_tabctrl.ChangeTab(ID, pUDev->pProcJobManual, str.GetBuffer(0), 1, rc.Width(), rc.Height());
	}
	return 0;
}

void CInfoPanelMain::OnTimer(UINT nIDEvent) {
	CResizableFormView::OnTimer(nIDEvent);
}

void CInfoPanelMain::DoProcessComplete(short ID) {
	pUDev->ClearAll(ID);
	if (pGDev->SystemModel == CGlobalDeviceContainer::FSM413C2C) {
		if (pUDev->pProcJobCompleteDlg[ID]) {
			pUDev->pProcJobCompleteDlg[ID]->DestroyWindow();
			pUDev->pProcJobCompleteDlg[ID] = NULL;
		}
		pUDev->pProcJobCompleteDlg[ID] = new CProcJobCompleteDlg;
		if (pUDev->pProcJobCompleteDlg[ID]) {
			pUDev->pProcJobCompleteDlg[ID]->ID = ID;
			pUDev->pProcJobCompleteDlg[ID]->m_Parent = this;
			pUDev->pProcJobCompleteDlg[ID]->Create(CProcJobCompleteDlg::IDD, &m_tabctrl);
			CString str;
			str.Format("Carruer/Job Complete (LP%d)", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJobCompleteDlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
	else {
		if (pUDev->pProcJobCompleteManual) {
			pUDev->pProcJobCompleteManual->DestroyWindow();
			pUDev->pProcJobCompleteManual = NULL;
		}
		pUDev->pProcJobCompleteManual = new CProcJobCompleteManual;
		if (pUDev->pProcJobCompleteManual) {
			pUDev->pProcJobCompleteManual->ID = ID;
			pUDev->pProcJobCompleteManual->m_Parent = this;
			pUDev->pProcJobCompleteManual->Create(CProcJobCompleteManual::IDD, &m_tabctrl);
			CString str;
			str.Format("Carrier/Job Complete (LP%d)", ID + 1);
			CRect rc;
			CWnd* pWnd = GetDlgItem(IDC_TAB1);
			if (pWnd) {
				pWnd->GetWindowRect(&rc);
				ScreenToClient(&rc);
			}
			m_tabctrl.ChangeTab(ID, pUDev->pProcJobCompleteManual, str.GetBuffer(0), 1, rc.Width(), rc.Height());
		}
	}
}

void CInfoPanelMain::DoProcessJobQueued(short ID, short type) {
	pUDev->ClearAll(ID);
	pUDev->pProcJobQueued413Dlg[ID] = new CProcJobQue413Dlg;
	if (pUDev->pProcJobQueued413Dlg[ID]) {
		pUDev->pProcJobQueued413Dlg[ID]->ID = ID;
		pUDev->pProcJobQueued413Dlg[ID]->m_Parent = this;
		pUDev->pProcJobQueued413Dlg[ID]->Create(CProcJobQue413Dlg::IDD, &m_tabctrl);
		CString str;
		str.Format("Carrier/Job Queued (LP%d)", ID + 1);
		CRect rc;
		CWnd* pWnd = GetDlgItem(IDC_TAB1);
		if (pWnd) {
			pWnd->GetWindowRect(&rc);
			ScreenToClient(&rc);
		}
		m_tabctrl.ChangeTab(ID, pUDev->pProcJobQueued413Dlg[ID], str.GetBuffer(0), 1, rc.Width(), rc.Height());
	}
}

// Call from E95 only [12/15/2007 YUEN]
void CInfoPanelMain::RestartPage(short ID, CString& CarrierID, CString& Recipe) {
	CCasMgr* pCasMgr = new CCasMgr;

	CString filename;

	CProcJobInfo* pJobInfo = &pUDev->pProcJobInfo[ID];
	if (!pJobInfo) {
		delete pCasMgr;
		ASSERT(0);
		return;
	}
	pJobInfo->Cas.JobInfo.CarrierID = CarrierID;
	strscpy(pJobInfo->Cas.Recipe413.RcpeName, RCPNAMELEN, Recipe.GetBuffer(0));

	pCasMgr->SetCassette(&pJobInfo->Cas);
	pCasMgr->MakeCassetteName(filename, Recipe, "", CarrierID, FALSE, TRUE);
	pCasMgr->OpenCassette(filename);

	delete pCasMgr;
}

void CInfoPanelMain::OnDestroy() {
	for (int i = 0; i < MAX_LPSUPPORTED; i++) {
		pUDev->ClearAll(i);
	}
	if (pUDev->pProcJobManageDlg) {
		pUDev->pProcJobManageDlg->DestroyWindow();
		pUDev->pProcJobManageDlg = NULL;
	}

	if (pUDev->pRecipeCreateDlg413) {
		if (((unsigned int)pUDev->pRecipeCreateDlg413->GetSafeHwnd() != 0xfeeefeee)) {
			pUDev->pRecipeCreateDlg413->DestroyWindow();  // if could not find tab, mean m_RecipeCreateDlg has been destroyed
		}
		pUDev->pRecipeCreateDlg413 = NULL;
	}

	if (pUDev->pRecoveryDlg) {
		pUDev->pRecoveryDlg->DestroyWindow();
		pUDev->pRecoveryDlg = NULL;
	}

	// 	if (pTmp1) {
	// 		pTmp1->DestroyWindow();
	// 		pTmp1 = NULL;
	// 	}
	// 	if (pTmp2) {
	// 		pTmp2->DestroyWindow();
	// 		pTmp2 = NULL;
	// 	}
	if (pTmp3) {
		pTmp3->DestroyWindow();
		pTmp3 = NULL;
	}
	if (pTmp4) {
		pTmp4->DestroyWindow();
		pTmp4 = NULL;
	}

	CResizableFormView::OnDestroy();
}

void CInfoPanelMain::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CInfoPanelMain");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}