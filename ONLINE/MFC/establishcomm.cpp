// establishcomm.cpp : implementation file
//
#pragma warning( disable : 4701 4100 4244 4189 4245 4310 4786 )
#include "stdafx.h"
#include "establishcomm.h"
#include "..\TCPIP\HsmsParaMeter.h"
#include "..\inc\event.h"
#include "..\ShareMemory\Evt.h"
#include "..\ShareMemory\ShareMemory.h"
#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDT_INIT				9999
#define IDT_WAITDELAY			9998
#define IDT_REFRESH				9997

/////////////////////////////////////////////////////////////////////////////
// CEstablishComm dialog

CEstablishComm::CEstablishComm(CWnd* pParent /*=NULL*/)
	: CDialog(CEstablishComm::IDD, pParent) {
	//{{AFX_DATA_INIT(CEstablishComm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CEstablishComm::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEstablishComm)
	DDX_Control(pDX, IDC_MSG, m_clist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEstablishComm, CDialog)
	//{{AFX_MSG_MAP(CEstablishComm)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEstablishComm message handlers

BOOL CEstablishComm::OnInitDialog() {
	CDialog::OnInitDialog();

	m_clist.InsertColumn(0, _T("Message"), LVCFMT_LEFT);
	m_clist.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);

	SetTimer(IDT_INIT, 1000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEstablishComm::OnTimer(UINT nIDEvent) {
	if (nIDEvent == IDT_REFRESH) {
		KillTimer(IDT_REFRESH);
		if (hmCommState == CHsmsParaMeter::COMMUNICATING) {
			KillTimer(IDT_INIT);
			KillTimer(IDT_WAITDELAY);
			KillTimer(IDT_REFRESH);
			CDialog::OnOK();
			return;
		}
		SetTimer(IDT_REFRESH, 100, NULL);
	}
	if (nIDEvent == IDT_WAITDELAY) {
		KillTimer(IDT_WAITDELAY);
		hmCommState = CHsmsParaMeter::WAITDELAY;
		Message("S1F13 timeout");
		SetTimer(IDT_INIT, EstablishCommunicationTimeout, NULL);
	}
	if (nIDEvent == IDT_INIT) {
		KillTimer(IDT_INIT);
		Message("Try to establish communication");
		hmCommState = CHsmsParaMeter::WAITCRA;
		EstablishCommunication();
		SetTimer(IDT_REFRESH, 100, NULL);
	}
	CDialog::OnTimer(nIDEvent);
}

void CEstablishComm::EstablishCommunication() {
	switch (hmCommState) {
	case CHsmsParaMeter::WAITDELAY:
	case CHsmsParaMeter::WAITCRA:
		Message("Establishing communication");
		hmCommState = CHsmsParaMeter::WAITCRA;
		CSharedMemQue m_gEvtQue;
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_COMMESTABLISH);
		m_gEvtQue.OpenMemory("EVTMSG");
		m_gEvtQue.WriteQue(pEvt);
		m_gEvtQue.CloseMemory();
		SetTimer(IDT_WAITDELAY, EstablishCommunicationTimeout, NULL);
		return;
		break;
	}
	ASSERT(0);
}

void CEstablishComm::Message(CString msg) {
	if (m_clist.GetItemCount() > 64) {
		for (int i = 32; i < m_clist.GetItemCount(); i++) {
			m_clist.DeleteItem(i);
		}
	}
	m_clist.InsertItem(0, msg);
}

void CEstablishComm::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CEstablishComm");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

void CEstablishComm::OnStop() {
	hmCommState = CHsmsParaMeter::ENABLED;
	KillTimer(IDT_INIT);
	KillTimer(IDT_WAITDELAY);
	KillTimer(IDT_REFRESH);
	CDialog::OnOK();
}