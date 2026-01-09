#include "stdafx.h"

#include "128/CAS/CasMgr.h"
#include "EchoData.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "SERIAL/MsgItem.h"
#include "SRC/DOSUtil.h"

#include "ProcJobProcessingB.h"

CProcJobProcessingB::CProcJobProcessingB() {
	m_pCasMgr = NULL;

	bRecovery = FALSE;
	bProcessing = FALSE;
	bTreeUnderconstruction = FALSE;
	DispatchTo = NOMODULE;
}

CProcJobProcessingB::~CProcJobProcessingB() {
	if (m_pCasMgr) {
		delete m_pCasMgr;
	}
}

void CProcJobProcessingB::ShowReportView() {}

void CProcJobProcessingB::ShowChartView() {}

void CProcJobProcessingB::ShowHeatView() {}

CWnd* CProcJobProcessingB::GetDialogItem(UINT ID) {
	return NULL;
}

void CProcJobProcessingB::ScrnToClnt(LPRECT lpRect) {}

CWnd* CProcJobProcessingB::GetThis() {
	return NULL;
}

BOOL CProcJobProcessingB::ShowTreeView() {
	return FALSE;
}

void CProcJobProcessingB::PrepareToStart(/*short type*/) {
	if (!m_Parent) {
		ASSERT(0);  // must have m_Parent
	}
	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return;
	}
	if (!m_pCasMgr) {
		m_pCasMgr = new CCasMgr;
		if (!m_pCasMgr) {
			ASSERT(0);
			return;
		}
	}
	m_pCasMgr->bDeleteCasPointer = FALSE;
	m_pCasMgr->pCas = &pUDev->pProcJobInfo[ID].Cas;
}

LRESULT CProcJobProcessingB::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Jobs\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

LRESULT CProcJobProcessingB::MsgInQue(CMsgItem* pMsg) {
	if (!m_Parent) {
		ASSERT(0);
		return -9;
	}
	if (!pMsg) {
		ASSERT(0);
		return -3;
	}
	CString str;
	if (pMsg->DataTyp == CMsgItem::SCANDATA) {
		CEchoData* pScnDta;
		pScnDta = new CEchoData;
		if (pScnDta) {
			pScnDta->Conv(&pMsg->pMsg);
			/*CNCas * pCas = &*/ pUDev->pProcJobInfo[ID].Cas;
		}
	}
	else if (pMsg->DataTyp == CMsgItem::EVENTDATA) {
		switch (pMsg->DataID) {
		case CMsgItem::PROCESSCOMPLETE:
			Sleep(5000);  // wait a while for C2C to clean up [2/8/2010 Yuen]
			SetProcessingComplete(pMsg);
			break;
		case CMsgItem::PROCESSFAILED:
			SetProcessingFail(pMsg);
			break;
		case CMsgItem::PROCESSHALT:
			SetProcessingHalt(pMsg);
			break;
		default:
			ASSERT(0);
			break;
		}
	}
	else if (pMsg->DataTyp == CMsgItem::ROBOTCMD) {
		ASSERT(0);
		// should not come here
	}
	delete pMsg;
	return 0;
}

void CProcJobProcessingB::ShowView(VIEWWIN View) {}

void CProcJobProcessingB::CreateView(VIEWWIN View) {}

void CProcJobProcessingB::RememberPrJob(short unit, CProcJobInfo* pInfo) {
	CString Recipe;
	Recipe = pInfo->Cas.Recipe413.RcpeName;
	DosUtil.RememberPrJob(unit, pInfo->Cas.JobInfo.CarrierID, Recipe);
}

BOOL CProcJobProcessingB::NoOtherProcJobRuning() {
	if (!m_Parent) {
		ASSERT(0);
		return FALSE;
	}
	for (int i = 0; i < MAX_LPSUPPORTED; i++) {
		if (i != ID) {  // Do not start if other load port is in processing mode
			if (pUDev->pProcJob413ProcessingDlg[i]) {
				return FALSE;
			}
			if (pUDev->pProcJobQueued413Dlg[i]) {
				return FALSE;
			}
		}
	}
	return TRUE;
}