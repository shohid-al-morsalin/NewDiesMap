#include "stdafx.h"
#include "ONLINE/ShareMemory/DataBank.h"
#include "HostMessage.h"

#include "GlobalDeviceContainer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHostMessage::CHostMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CHostMessage::IDD, pParent) {
	bAck = FALSE;
	bSending = FALSE;
	strMsg = _T("");
}

void CHostMessage::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HOST_MSG, m_HostMessage);
}

BEGIN_MESSAGE_MAP(CHostMessage, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ACK, OnButtonAck)
END_MESSAGE_MAP()

BOOL CHostMessage::OnInitDialog() {
	CDialog::OnInitDialog();

	bAck = FALSE;
	CString strMsg = pGDev->pDataBnk->m_HostMsg1;

	if (!bSending) {
		GetDlgItem(IDC_EDIT_HOST_MSG)->SetWindowText(strMsg);
	}
	else {
		GetDlgItem(IDC_BUTTON_ACK)->SetWindowText("Send to host");
		GetDlgItem(IDC_STATIC_MSG)->SetWindowText("Enter the message to be sent to the host");
		SetWindowText("Terminal message to Host");
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CHostMessage::OnButtonAck() {
	if (bSending) {
		OnOK();
	}

	bAck = TRUE;
}

void CHostMessage::OnOK() {
	// TODO: Add extra validation here
	this->m_HostMessage.GetWindowText(this->strMsg);
	if (bSending) {
		if (0 == strMsg.GetLength()) {
			AfxMessageBox("Please enter something");
		}
		else {
			CDialog::OnOK();
		}
	}
	else {
		CDialog::OnOK();
		bAck = TRUE;
	}
}