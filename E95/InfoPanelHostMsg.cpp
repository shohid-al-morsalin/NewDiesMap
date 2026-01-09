// InfoPanelHostMsg.cpp : implementation file
//

#include "stdafx.h"
#include "e95.h"
#include "InfoPanelHostMsg.h"
#include "NavigationPanel.h"
#include "ST_SplitterWnd.h"
#include "..\SYS\CUMMsg.h"

#include "GlobalUIContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelHostMsg

IMPLEMENT_DYNCREATE(CInfoPanelHostMsg, CResizableFormView)

CInfoPanelHostMsg::CInfoPanelHostMsg()
	: CResizableFormView(CInfoPanelHostMsg::IDD) {
	//{{AFX_DATA_INIT(CInfoPanelHostMsg)
	//}}AFX_DATA_INIT

	if (!pUDev) {
		ASSERT(0);
	}

	idx = 0;
	pUDev->pHostMessage = this;
}

CInfoPanelHostMsg::~CInfoPanelHostMsg() {
	if (pUDev) {
		pUDev->pHostMessage = NULL;
	}
}

void CInfoPanelHostMsg::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoPanelHostMsg)
	DDX_Control(pDX, IDC_MSGCOUNT, m_cMsgCount);
	DDX_Control(pDX, IDC_MESSAGE, m_cMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInfoPanelHostMsg, CResizableFormView)
	//{{AFX_MSG_MAP(CInfoPanelHostMsg)
	ON_BN_CLICKED(IDC_ACCEPT, OnAccept)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_MESSAGE(WM_STSELECTED, OnSTSelected)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UM_HOST_MESSAGE, OnHostMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelHostMsg diagnostics

#ifdef _DEBUG
void CInfoPanelHostMsg::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CInfoPanelHostMsg::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInfoPanelHostMsg message handlers
LRESULT CInfoPanelHostMsg::OnHostMessage(WPARAM wP, LPARAM lP) {
	if (!lP) {
		return -1;
	}
	string* pStr = (string*)lP;
	CString str = (*pStr).c_str();
	Message(str);
	return 0;
}

LRESULT CInfoPanelHostMsg::OnSTSelected(WPARAM wParam, LPARAM lParam) {
	if (pUDev) {
		pUDev->SetCurrentView("Host Message");
	}
	return 0;
}

void CInfoPanelHostMsg::OnAccept() {
	m_HostMsg.Remove(idx);
	Update();
}

// Entry point to add a message
void CInfoPanelHostMsg::Message(CString msg) {
	m_HostMsg.Add(msg);
	Update();
	if (pUDev->pNavigationPanel) {
		pUDev->pNavigationPanel->PostMessage(WM_COMMAND, ID_E95_BLINKHOSTMESSAGE, NULL);
	}
}

void CInfoPanelHostMsg::OnUp() {
	idx--;
	Update();
}

void CInfoPanelHostMsg::OnDown() {
	idx++;
	Update();
}

CHostMsgList::CHostMsgList() {}

CHostMsgList::~CHostMsgList() {
	ClearList();
}

void CHostMsgList::Add(CString msg) {
	CHostMsg* pMsg = new CHostMsg;

	if (pMsg) {
		pMsg->Msg = msg;
		pMsg->DateTime.Format("%s  ", CTime::GetCurrentTime().Format("%y:%m:%d %H-%M-%S"));
		m_MsgList.AddTail(pMsg);
		if (m_MsgList.GetCount() > 32) {
			CHostMsg* pMsgO = (CHostMsg*)m_MsgList.RemoveTail();
			if (pMsgO) {
				delete pMsgO;
			}
		}
	}
}

CHostMsg* CHostMsgList::Get(short idx) {
	short cnt = 1;
	POSITION pos = m_MsgList.GetHeadPosition();
	while (pos) {
		CHostMsg* pMsg = (CHostMsg*)m_MsgList.GetNext(pos);
		if (idx == cnt) {
			return pMsg;
		}
		cnt++;
	}
	return NULL;
}

void CHostMsgList::Remove(short idx) {
	short cnt = 1;
	POSITION opos;
	POSITION pos = m_MsgList.GetHeadPosition();
	while (pos) {
		opos = pos;
		CHostMsg* pMsg = (CHostMsg*)m_MsgList.GetNext(pos);
		if (idx == cnt) {
			m_MsgList.RemoveAt(opos);
			delete pMsg;
			return;
		}
		cnt++;
	}
}

void CHostMsgList::ClearList() {
	POSITION pos = m_MsgList.GetHeadPosition();
	while (pos) {
		CHostMsg* pMsg = (CHostMsg*)m_MsgList.GetNext(pos);
		if (pMsg) {
			delete pMsg;
		}
	}
	if (m_MsgList.GetCount()) {
		m_MsgList.RemoveAll();
	}
}

void CInfoPanelHostMsg::OnInitialUpdate() {
	AddResizedControl(IDC_MESSAGE, TOP_LEFT, BOTTOM_LEFT);
	AddResizedControl(IDC_ACCEPT, TOP_RIGHT);
	AddResizedControl(IDC_UP, TOP_RIGHT);
	AddResizedControl(IDC_DOWN, TOP_RIGHT);

	CResizableFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	bWnd = TRUE;
	CWnd* pWnd = GetDlgItem(IDC_FRAME);
	pWnd->GetWindowRect(&iniRC);
	ScreenToClient(&iniRC);

	AnchorControls();

	idx = 0;
	Update();
}

void CInfoPanelHostMsg::Update() {
	if (idx < 0) {
		idx = 0;
	}

	int cnt = m_HostMsg.GetCount();
	if (idx > cnt) {
		idx = cnt;
	}
	CString str;
	str.Format("%d of %d", idx, cnt);
	m_cMsgCount.SetWindowText(str);

	CHostMsg* pMsg = m_HostMsg.Get(idx);
	if (pMsg) {
		m_Message = pMsg->DateTime + "\r\n" + pMsg->Msg;
		m_cMessage.SetWindowText(m_Message);
	}
	else {
		m_cMessage.SetWindowText("");
		if (idx > 1) {
			idx--;
		}
	}
}

void CInfoPanelHostMsg::OnDestroy() {
	m_HostMsg.ClearList();
	CResizableFormView::OnDestroy();
}