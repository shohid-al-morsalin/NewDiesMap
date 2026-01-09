// InfoPanelEqpMessage.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "InfoPanelEqpMessage.h"
#include "NavigationPanel.h"
#include "ST_SplitterWnd.h"

#include "GlobalUIContainer.h"
#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelEqpMessage

IMPLEMENT_DYNCREATE(CInfoPanelEqpMessage, CResizableFormView)

CInfoPanelEqpMessage::CInfoPanelEqpMessage()
	: CResizableFormView(CInfoPanelEqpMessage::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelEqpMessage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pUDev->pEqpMessage = this;
}

CInfoPanelEqpMessage::~CInfoPanelEqpMessage() {
	pUDev->pEqpMessage = NULL;
}

void CInfoPanelEqpMessage::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelEqpMessage)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelEqpMessage, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelEqpMessage)
	ON_COMMAND(ID_LOCAL_MSG, OnLocalMsg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelEqpMessage diagnostics

#ifdef _DEBUG
void CInfoPanelEqpMessage::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelEqpMessage::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelEqpMessage message handlers
LRESULT CInfoPanelEqpMessage::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	if (pUDev) pUDev->SetCurrentView("Equipment Message");
	return 0;
}

void CInfoPanelEqpMessage::DspMessage(CString msg, short lvl) {
	m_List.InsertString(0, msg);
	int n = m_List.GetCount();
	if (n > 32) {
		for (int i = n; i >= 24; i--) {
			m_List.DeleteString(i);
		}
	}
	if (pUDev->pNavigationPanel) {
		pUDev->pNavigationPanel->PostMessage(WM_COMMAND, ID_E95_BLINKEQPMESSAGE, NULL);
	}
}

void CInfoPanelEqpMessage::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CInfoPanelEqpMessage");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

// Entry point, call to display message
void CInfoPanelEqpMessage::Message(CString msg, short lvl) {
	SEQPMSG Msg;
	Msg.msg = pGDev->GetTimeStamp() + msg;
	Msg.lvl = lvl;
	pGDev->GetTimeStamp();
	m_Msgs.push_back(Msg);
	PostMessage(WM_COMMAND, ID_LOCAL_MSG, NULL);
}

void CInfoPanelEqpMessage::OnLocalMsg() {
	if (m_Msgs.size()) {
		vector <SEQPMSG>::iterator i;
		i = m_Msgs.begin();
		SEQPMSG msg = *i;
		DspMessage(msg.msg, msg.lvl);
		m_Msgs.erase(i);
	}
}

void CInfoPanelEqpMessage::OnInitialUpdate() {
	AddResizedControl(IDC_LIST1, TOP_LEFT, BOTTOM_LEFT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	AnchorControls();
}