// TelnetClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "TelnetClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CirNormalText[CIRNORMALTEXTLEN + 1];	// Circular buffer string [5/26/2012 Administrator]

/////////////////////////////////////////////////////////////////////////////
// CTelnetClientSocket

CTelnetClientSocket::CTelnetClientSocket() {
	idx = 0;
	m_lpfnMessageProc = NULL;
}

CTelnetClientSocket::~CTelnetClientSocket() {}

// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CTelnetClientSocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CTelnetClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CTelnetClientSocket member functions

void CTelnetClientSocket::OnClose(int nErrorCode) {
	if (m_lpfnMessageProc) {
		m_lpfnMessageProc(SOCCLOSE, (void*)nErrorCode);
	}
	CAsyncSocket::OnClose(nErrorCode);
}

void CTelnetClientSocket::OnConnect(int nErrorCode) {
	if (m_lpfnMessageProc) {
		m_lpfnMessageProc(SOCCONNECT, NULL);
	}
	CAsyncSocket::OnConnect(nErrorCode);
}

void CTelnetClientSocket::OnOutOfBandData(int nErrorCode) {
	ASSERT(FALSE); //Telnet should not have OOB data
	CAsyncSocket::OnOutOfBandData(nErrorCode);
}

// This event is disabled [7/18/2013 Yuen]
void CTelnetClientSocket::OnReceive(int nErrorCode) {
	int nBytes = Receive(m_bBuf, ioBuffSize);
	m_bBuf[nBytes] = 0;		// terminate with zero [5/26/2012 Administrator]
	if (nBytes != SOCKET_ERROR) {
		int ndx = 0;
		while (ndx < nBytes) {
			if (GetLine(m_bBuf, nBytes, ndx) == TRUE) {
				//ProcessOptions();
				if (m_strNormalText.GetLength()) {
					CirNormalText[idx] = m_strNormalText;
					MessageReceived(CirNormalText[idx].GetBuffer(0));
					idx++;
					if (idx > CIRNORMALTEXTLEN) {
						idx = 0;
					}
				}
			}
		}
	}
}

void CTelnetClientSocket::MessageReceived(LPCSTR pText) {
	if (m_lpfnMessageProc) {
		m_lpfnMessageProc(NORMALMSG, (LPVOID)pText);
	}
}

void CTelnetClientSocket::OnSend(int nErrorCode) {
	CAsyncSocket::OnSend(nErrorCode);
}

void CTelnetClientSocket::SetCallBack(lpfnMessageProc pFunc) {
	m_lpfnMessageProc = pFunc;
}

void CTelnetClientSocket::ProcessOptions() {
	CString m_strTemp;
	CString m_strOption;
	unsigned char ch;
	int ndx;
	int ldx;
	BOOL bScanDone = FALSE;

	m_strTemp = m_strLine;
	while (!m_strTemp.IsEmpty() && bScanDone != TRUE) {
		ndx = m_strTemp.Find(IAC);
		if (ndx != -1) {
			m_strNormalText += m_strTemp.Left(ndx);
			ch = m_strTemp.GetAt(ndx + 1);
			switch (ch) {
			case DO:
			case DONT:
			case WILL:
			case WONT:
				m_strOption = m_strTemp.Mid(ndx, 3);
				m_strTemp = m_strTemp.Mid(ndx + 3);
				m_strNormalText = m_strTemp.Left(ndx);
				m_ListOptions.AddTail(m_strOption);
				break;
			case IAC:
				m_strNormalText = m_strTemp.Left(ndx);
				m_strTemp = m_strTemp.Mid(ndx + 1);
				break;
			case SB:
				m_strNormalText = m_strTemp.Left(ndx);
				ldx = m_strTemp.Find(SE);
				m_strOption = m_strTemp.Mid(ndx, ldx);
				m_ListOptions.AddTail(m_strOption);
				m_strTemp = m_strTemp.Mid(ldx);
				AfxMessageBox(m_strOption, MB_OK);
				break;
			}
		}
		else {
			m_strNormalText = m_strTemp;
			bScanDone = TRUE;
		}
	}
	RespondToOptions();
}

BOOL CTelnetClientSocket::GetLine(unsigned char* bytes, int nBytes, int& ndx) {
	BOOL bLine = FALSE;
	m_strLine = "";
	while (bLine == FALSE && ndx < nBytes) {
		unsigned char ch = bytes[ndx];

		switch (ch) {
		case '\r': // ignore
			//m_strLine += "\r"; //"CR";
			//m_strNormalText = m_strLine;
			//bLine = TRUE;
			break;
		case '\n': // end-of-line
			//	m_strLine += '\n'; //"LF";
			//	bLine = TRUE;
			break;
		case ':':
		case '?':
			m_strLine += ch;
			//m_strNormalText = m_strLine;
			bLine = TRUE;
			break;
		default:   // other....
			m_strLine += ch;
			break;
		}

		ndx++;

		if (ndx == nBytes) {
			bLine = TRUE;
		}
	}
	if (bLine) {
		m_strNormalText = m_strLine;
	}
	return bLine;
}

void CTelnetClientSocket::RespondToOptions() {
	CString strOption;
	while (!m_ListOptions.IsEmpty()) {
		strOption = m_ListOptions.RemoveHead();
		ArrangeReply(strOption);
	}
	DispatchMessage(m_strResp);
	m_strResp.Empty();
}

void CTelnetClientSocket::ArrangeReply(CString strOption) {
	unsigned char Verb;
	unsigned char Option;
	unsigned char Modifier;
	unsigned char ch;
	BOOL bDefined = FALSE;

	if (strOption.GetLength() < 3) return;

	Verb = strOption.GetAt(1);
	Option = strOption.GetAt(2);

	switch (Option) {
	case 1:	// Echo
	case 3: // Suppress Go-Ahead
		bDefined = TRUE;
		break;
	}

	m_strResp += IAC;
	if (bDefined == TRUE) {
		switch (Verb) {
		case DO:
			ch = WILL;
			m_strResp += ch;
			m_strResp += Option;
			break;
		case DONT:
			ch = WONT;
			m_strResp += ch;
			m_strResp += Option;
			break;
		case WILL:
			ch = DO;
			m_strResp += ch;
			m_strResp += Option;
			break;
		case WONT:
			ch = DONT;
			m_strResp += ch;
			m_strResp += Option;
			break;
		case SB:
			Modifier = strOption.GetAt(3);
			if (Modifier == SEND) {
				ch = SB;
				m_strResp += ch;
				m_strResp += Option;
				m_strResp += IS;
				m_strResp += IAC;
				m_strResp += SE;
			}
			break;
		}
	}
	else {
		switch (Verb) {
		case DO:
			ch = WONT;
			m_strResp += ch;
			m_strResp += Option;
			break;
		case DONT:
			ch = WONT;
			m_strResp += ch;
			m_strResp += Option;
			break;
		case WILL:
			ch = DONT;
			m_strResp += ch;
			m_strResp += Option;
			break;
		case WONT:
			ch = DONT;
			m_strResp += ch;
			m_strResp += Option;
			break;
		}
	}
}

void CTelnetClientSocket::DispatchMessage(CString strText) {
	Send(strText, strText.GetLength());
}