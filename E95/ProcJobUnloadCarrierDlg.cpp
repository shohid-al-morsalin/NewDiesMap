#include "stdafx.h"
#include "E95.h"

#include "128/GEM/ObjectsContainer.h"
#include "FEM/Ldp/LdPTOut.h"
#include "FEM/Ldp/LPMgr.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "InfoPanelMain.h"
#include "IPC/SMCHdr.h"
#include "SYS/CUMMsg.h"

#include "ProcJobUnloadCarrierDlg.h"

#define IDT_UNLATCHTIMEOUT		9999

IMPLEMENT_DYNAMIC(CProcJobUnloadCarrierDlg, CDialog)

CProcJobUnloadCarrierDlg::CProcJobUnloadCarrierDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcJobUnloadCarrierDlg::IDD, pParent) {
	//{{AFX_DATA_INIT(CProcJobUnloadCarrierDlg)
	m_Message = _T("");
	//}}AFX_DATA_INIT

	ID = 0;
	m_Parent = NULL;
}

CProcJobUnloadCarrierDlg::~CProcJobUnloadCarrierDlg() {}

void CProcJobUnloadCarrierDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcJobUnloadCarrierDlg)
	DDX_Text(pDX, IDC_MESSAGE, m_Message);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcJobUnloadCarrierDlg, CDialog)
	//{{AFX_MSG_MAP(CProcJobUnloadCarrierDlg)
	ON_COMMAND(ID_COM_BUTTONPUSHED, OnComButtonpushed)
	ON_COMMAND(ID_LP_UNCLAMPOK, OnLpUnclampok)
	ON_COMMAND(ID_LP_UNCLAMPFAIL, OnLpUnclampfail)
	ON_COMMAND(ID_COM_RELEASECMPL, OnComReleasecmpl)
	ON_COMMAND(ID_COM_CARRIERREMOVED, OnComCarrierremoved)
	ON_WM_TIMER()
	ON_MESSAGE(WM_XTABSELECTED, OnTabSelected)
	ON_MESSAGE(UM_CARRIERREMOVED, OnCarrierRemoved)
	ON_COMMAND(ID_COM_NOVACUUM, OnComNovacuum)
	ON_COMMAND(ID_COM_WAFEROUT, OnComWaferout)
	ON_COMMAND(ID_COM_VACUUMLOST, OnComVacuumlost)
	ON_COMMAND(ID_COM_SENSORTRIP, OnComSensortrip)
	ON_MESSAGE(ID_COM_TP1TIMEOUT, OnComTp1timeout)
	ON_MESSAGE(ID_COM_TP2TIMEOUT, OnComTp2timeout)
	ON_MESSAGE(ID_COM_TP3TIMEOUT, OnComTp3timeout)
	ON_MESSAGE(ID_COM_TP4TIMEOUT, OnComTp4timeout)
	ON_MESSAGE(ID_COM_TP5TIMEOUT, OnComTp5timeout)
	ON_MESSAGE(ID_COM_TP6TIMEOUT, OnComTp6timeout)
	ON_MESSAGE(ID_COM_LOSTE84SIGNAL, OnComLoste84signal)
	ON_MESSAGE(ID_COM_UNEXPECTEDE84SIGNAL, OnComUnexpectede84signal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// CProcJobUnloadCarrierDlg message handlers

BOOL CProcJobUnloadCarrierDlg::OnInitDialog() {
	CDialog::OnInitDialog();

	if ((ID < 0) || (ID >= MAX_LPSUPPORTED)) {
		ASSERT(0);
		return FALSE;
	}

	if (!m_Parent || !pGDev || !pUDev) {
		ASSERT(0);
		return FALSE;
	}

	// Connect LPSvr and this
	pGDev->RegisterWinWithLPMgr(m_hWnd, ID + 1);

	CString msg;
	msg.Format("Waiting for the carrier to leave the tool (LP%d)", ID + 1);
	Message(msg, 5);
	//UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CProcJobUnloadCarrierDlg::OnCarrierRemoved(WPARAM wP, LPARAM lP) {
	CString CarrierID = "";
	pGDev->pGEMObjs->SetLPAssociation(CarrierID, ID + 1, CLPObj::NOTASSOCIATED);

	CString msg;
	//m_Message += "\nCarrier has left the tool";
	//UpdateData(FALSE);
	msg.Format("Carrier has left the tool (LP%d)", ID + 1);
	Message(msg, 5);
	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBUNLOADCOMPLETE, ID, 0);
	return 0;
}

void CProcJobUnloadCarrierDlg::OnComButtonpushed() {
	Unclamp();	// Do unclamp if it is clamped
}

void CProcJobUnloadCarrierDlg::Unclamp() {
	CString msg;
	//m_Message += "\nUnclamping carrier";
	//UpdateData(FALSE);
	msg.Format("Unclamping carrier (LP%d)", ID + 1);
	Message(msg, 5);

	CSMOL SM;
	SM.cmd = CSMOL::cmdPRCSMGRSERVICES;
	SM.HWin = m_hWnd;
	SM.Ack = ID_LP_UNCLAMPOK;
	SM.Nak = ID_LP_UNCLAMPFAIL;
	SM.Pm.unit = ID + 1;
	SM.Pm.nunit = 1;
	SM.Pm.act = PrcsMgrServices::UNLATCHFOUP;
	CSMClt ipc;
	if (ipc.Start1(pGDev->pLPMgr->SvrName, sizeof(CSMOL))) {
		if (ipc.Send(&SM)) {
			SetTimer(IDT_UNLATCHTIMEOUT, UNLATCHFOUPTIMEOUT + 8000, NULL);
			return;
		}
		//m_Message += "Unclamp foup send fail";
		//UpdateData(FALSE);
		msg.Format("Unclamp carrier send fail (%d)", ID + 1);
		Message(msg, 0);
		PostMessage(WM_COMMAND, ID_LP_UNCLAMPFAIL, 0);
		return;
	}
	msg.Format("LPMgr not found (LP%d)", ID + 1);
	Message(msg, 0);
	PostMessage(WM_COMMAND, ID_LP_UNCLAMPFAIL, 0);
}

void CProcJobUnloadCarrierDlg::OnLpUnclampok() {
	CString msg;
	//m_Message += "\nUnclamp command send OK";
	//UpdateData(FALSE);
	msg.Format("Unclamp carrier command send OK (LP%d)", ID + 1);
	Message(msg, 5);
}

void CProcJobUnloadCarrierDlg::OnLpUnclampfail() {
	KillTimer(IDT_UNLATCHTIMEOUT);
	CString msg;
	//m_Message += "\nUnclamp command send fail";
	//UpdateData(FALSE);
	msg.Format("Unclamp carrier command send fail (LP%d)", ID + 1);
	Message(msg, 0);
}

void CProcJobUnloadCarrierDlg::OnComReleasecmpl() {
	KillTimer(IDT_UNLATCHTIMEOUT);
	CString msg;
	//m_Message += "\n\nWaiting for the carrier to leave the tool";
	//m_Message += "\nCarrier unclamped";
	//UpdateData(FALSE);
	msg.Format("Carrier unclamped (LP%d)", ID + 1);
	Message(msg, 5);
	msg.Format("Waiting for the carrier to leave the tool (LP%d)", ID + 1);
	Message(msg, 5);
}

void CProcJobUnloadCarrierDlg::OnComCarrierremoved() {
	CString CarrierID = "";
	pGDev->pGEMObjs->SetLPAssociation(CarrierID, ID + 1, CLPObj::NOTASSOCIATED);

	CString msg;
	//m_Message += "\nCarrier has left the tool";
	//UpdateData(FALSE);
	msg.Format("Carrier has left the tool (LP%d)", ID + 1);
	Message(msg, 5);

	ShowWindow(SW_HIDE);
	m_Parent->PostMessage(UM_TO_PROCJOBUNLOADCOMPLETE, ID, 0); // else just exit
}

void CProcJobUnloadCarrierDlg::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_UNLATCHTIMEOUT) {
		KillTimer(IDT_UNLATCHTIMEOUT);
		CString msg;
		msg.Format("Unclamp carrier timeout (LP%d)", ID + 1);
		Message(msg, 0);
		//m_Message += "\nUnclamp foup timeout";
		//UpdateData(FALSE);
		PostMessage(WM_COMMAND, ID_LP_UNCLAMPFAIL, NULL);
	}
}

LRESULT CProcJobUnloadCarrierDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	CString msg;
	msg.Format("Jobs\n%s", (char*)lP);
	pUDev->SetCurrentView(msg);
	return 0;
}

void CProcJobUnloadCarrierDlg::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CProcJobUnloadCarrierDlg");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CProcJobUnloadCarrierDlg::PostNcDestroy() {
	CDialog::PostNcDestroy();
	delete this;
}

void CProcJobUnloadCarrierDlg::OnComNovacuum() {
	Message("No vacuum error", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1NOVACUUM);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2NOVACUUM);
		break;
	}
	//CancelAndQuit();
}

void CProcJobUnloadCarrierDlg::OnComWaferout() {
	Message("Wafer protrusion error", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1EXCESSIVEWAFEROUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2EXCESSIVEWAFEROUT);
		break;
	}
	//CancelAndQuit();
}

void CProcJobUnloadCarrierDlg::OnComVacuumlost() {
	Message("Vacuum lost error", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1VACUUMLOST);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2VACUUMLOST);
		break;
	}
	//CancelAndQuit();
}

void CProcJobUnloadCarrierDlg::OnComSensortrip() {
	Message("Sensor trip error", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1PERISENSORTRIPED);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2PERISENSORTRIPED);
		break;
	}
	//CancelAndQuit();
}

LRESULT CProcJobUnloadCarrierDlg::OnComTp1timeout(WPARAM wP, LPARAM lP) {
	Message("TP1 timeout", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP1TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP1TIMEOUT);
		break;
	}
	return TPErrorRecovery(1, wP);
}

LRESULT CProcJobUnloadCarrierDlg::OnComTp2timeout(WPARAM wP, LPARAM lP) {
	Message("TP2 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP2TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP2TIMEOUT);
		break;
	}
	return TPErrorRecovery(2, wP);
}

LRESULT CProcJobUnloadCarrierDlg::OnComTp3timeout(WPARAM wP, LPARAM lP) {
	Message("TP3 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP3TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP3TIMEOUT);
		break;
	}
	return TPErrorRecovery(3, wP);
}

LRESULT CProcJobUnloadCarrierDlg::OnComTp4timeout(WPARAM wP, LPARAM lP) {
	Message("TP4 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP4TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP4TIMEOUT);
		break;
	}
	return TPErrorRecovery(4, wP);
}

LRESULT CProcJobUnloadCarrierDlg::OnComTp5timeout(WPARAM wP, LPARAM lP) {
	Message("TP5 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP5TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP5TIMEOUT);
		break;
	}
	return TPErrorRecovery(5, wP);
}

LRESULT CProcJobUnloadCarrierDlg::OnComTp6timeout(WPARAM wP, LPARAM lP) {
	Message("TP6 time out", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1E84TP6TIMEOUT);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2E84TP6TIMEOUT);
		break;
	}
	return TPErrorRecovery(6, wP);
}

LRESULT CProcJobUnloadCarrierDlg::OnComLoste84signal(WPARAM wP, LPARAM lP) {
	Message("Lost E84 signal", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1LOSTE84SIGNAL);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2LOSTE84SIGNAL);
		break;
	}
	return TPErrorRecovery(7, wP);
}

LRESULT CProcJobUnloadCarrierDlg::OnComUnexpectede84signal(WPARAM wP, LPARAM lP) {
	Message("Unexpected E84 signal", 1);
	switch (ID) {
	case  0:
		pGDev->SetAlarm(CAlrmList::LP1UNEXPECTEDE84SIGNAL);
		break;
	case 1:
		pGDev->SetAlarm(CAlrmList::LP2UNEXPECTEDE84SIGNAL);
		break;
	}
	return TPErrorRecovery(8, wP);
}

// TP error implies that load port is in AUTO mode [3/16/2012 Administrator]
int CProcJobUnloadCarrierDlg::TPErrorRecovery(short errCode, BOOL bCarrierPresent) {
	CString msg;
	msg.Format("TPRecovery (LP%d) carrier:%d", ID + 1, bCarrierPresent);
	Message(msg, 1);

	return 0;
}

void CProcJobUnloadCarrierDlg::Message(CString msg, short lvl) {
	Log(msg, lvl);
	m_Message += "\n";
	m_Message += msg;
	UpdateData(FALSE);
}