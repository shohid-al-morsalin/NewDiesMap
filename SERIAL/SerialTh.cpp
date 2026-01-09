#include "StdAfx.h"
#include "SerialTh.h"
#include "SerialICC.h"
#include "..\SYS\CUMMsg.h"

CSerialTh::CSerialTh(void) {
	pCom = NULL;
	bSimu = TRUE;

	IDSrc = 0;
	cnt = 0;
	port = 1;
	ytime = 10;
	cdelay = 1;
	hMsgInQue = 0;
	Protocol = MSGITEMFORMAT;
	CTOut = 15000;
	CTOut1 = 4000;
	pWnd = NULL;
	srand((unsigned)time(NULL));
}

CSerialTh::~CSerialTh(void) {
	Que.Clear();
	if (pCom) {
		pCom->Close();
		delete pCom;
	}
}

CSerialTh* CSerialTh::Start(CSerialTh* pThread, int port) {
	//	CSerialTh * pThread = new CSerialTh;
	if (pThread) {
		pThread->port = port;
		pThread->StartThread();
		return pThread;
	}
	return NULL;
}

UINT CSerialTh::ThreadMain(void) {
	if (!Initialize()) {
		//TRACE( "Thread cannot run\n" );
		return -1;
	}

	switch (Protocol) {
	case TEXTFORMAT: // Using char *
		while (!ShouldDie()) {
			ProcessReadB();
			ProcessWriteB();
			Sleep(ytime);
		};
		break;
	case MSGITEMFORMAT: // Using CMsgItem
		while (!ShouldDie()) {
			ProcessRead();
			ProcessWrite();
			Sleep(ytime);
		};
		break;
	default:
		//TRACE("Format Error\n");
		ASSERT(0);
		break;
	}
	//TRACE("Thread Terminated\n");
	return 0;
}

int CSerialTh::Initialize(void) {
	if (pCom) {
		pCom->Close();
		delete pCom;
	}
	bSimu = TRUE;
	pCom = new CSerialICC;
	if (pCom) {
		if (pCom->Open(port, 57600)) {
			bSimu = FALSE;
			return 1;
		}
		delete pCom;
		pCom = NULL;
	}
	return 0;
}

int CSerialTh::ProcessRead() {
	BYTE ch;

	if (!pCom || !pCom->ReadByte(ch)) {
		return 0;
	}
	//	TRACE("SerialTh: First byte\n");
	int len; // make sure that the type of len is identical to CMsgItem::Len
//	cdelay = 1;
	// look for header
	short idx = 0;
	const char header[4] = "\t\r\n";
	if (ch != header[idx]) {
		return 0;
	}
	idx++;
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < CTOut1) {
		if (!pCom->ReadByte(ch)) {
			Sleep(cdelay);
			continue;
		}
		if (ch == header[idx]) {
			tick = GetTickCount64();
			idx++;
			if (idx >= 3) {
				//				TRACE( "Header found " );
				break; // found header
			}
			else {
				Sleep(1);
				//				cdelay = 1;
				continue;
			}
		}
		else {
			//TRACE("Out of Sync\n");
			return 0;
		}
	}
	//TRACE( "\n" );
	if (idx < 3) {
		// Read header timeout
		return 0;
	}
	// read in length
	if (!GetItemOverComm(len)) {
		//TRACE( "Get length timeout\n" );
		return 0;
	}
	// out of sync, throw away
	if (len < 0 || len > 63999) {
		//TRACE( "Wrong length byte\n" );
		return 0;
	}
	//	TRACE( "Length found " );

	SSDT Dt;
	if (!GetItemOverComm(Dt.Rpy)) {
		//TRACE( "Get bRpy timeout\n" );
		return 0;
	}

	if (!GetItemOverComm(Dt.IDIdx)) {
		//TRACE( "Get ID timeout\n" );
		return 0;
	}

	if (!GetItemOverComm(Dt.DataTyp)) {
		//TRACE( "Get DataType timeout\n" );
		return 0;
	}

	if (!GetItemOverComm(Dt.DataID)) {
		//TRACE( "Get DataID timeout\n" );
		return 0;
	}

	if (!GetItemOverComm(Dt.MsgSrc)) {
		//TRACE( "Get MsgSrc timeout\n" );
		return 0;
	}

	if (!GetItemOverComm(Dt.MsgDes)) {
		//TRACE( "Get MsgDes timeout\n" );
		return 0;
	}

	if (!GetItemOverComm(Dt.MsgEvent)) {
		//TRACE( "Get MsgEvent timeout\n" );
		return 0;
	}

	if (!GetItemOverComm(Dt.Unit)) {
		//TRACE( "Get Unit timeout\n" );
		return 0;
	}

	if (!GetItemOverComm(Dt.AckWnd)) {
		//TRACE("Get AckWnd timeout\n");
		return 0;
	}

	if (!GetItemOverComm(Dt.Ack)) {
		//TRACE("Get AckWnd timeout\n");
		return 0;
	}

	if (!GetItemOverComm(Dt.Nak)) {
		//TRACE("Get AckWnd timeout\n");
		return 0;
	}

	//	int cnt;
	cnt = 0;
	tick = GetTickCount64();
	ULONGLONG tick2 = tick;
	while (((tick2 = GetTickCount64()) - tick) < CTOut) {
		if (cnt >= len) {
			//			TRACE( ".\n" );
			return MessageDone(len, Dt);
		}
		if (!pCom->ReadByte(ch)) {
			Sleep(1);
			continue;
		}
		ReadBuff[cnt++] = ch;
		//		TRACE1( "%c", ch );
		tick = tick2;
	}
	ReadBuff[cnt] = 0;
	//TRACE( "Inter-charecter timeout: " );
	//TRACE( ReadBuff );
	//TRACE( "\n" );
	return 0;
}

int CSerialTh::ProcessWrite() {
	CMsgItem* p = Que.Check();
	if (p) {
		// if driver is active, send it
		if (pCom) {
			//		TRACE("SerialTh: Start write\n");
			short len;
			pCom->SendData("\t\r\n", 3);
			len = pCom->SendData((char*)&p->Len, sizeof(p->Len));
			len += pCom->SendData((char*)&p->bRpy, sizeof(p->bRpy));
			len += pCom->SendData((char*)&p->IDIdx, sizeof(p->IDIdx));
			len += pCom->SendData((char*)&p->DataTyp, sizeof(p->DataTyp));
			len += pCom->SendData((char*)&p->DataID, sizeof(p->DataID));
			len += pCom->SendData((char*)&p->MsgSrc, sizeof(p->MsgSrc));
			len += pCom->SendData((char*)&p->MsgDes, sizeof(p->MsgDes));
			len += pCom->SendData((char*)&p->MsgEvent, sizeof(p->MsgEvent));
			len += pCom->SendData((char*)&p->Unit, sizeof(p->Unit));
			len += pCom->SendData((char*)&p->AckWnd, sizeof(p->AckWnd));
			len += pCom->SendData((char*)&p->Ack, sizeof(p->Ack));
			len += pCom->SendData((char*)&p->Nak, sizeof(p->Nak));
			if (p->Len) {
				len += pCom->SendData(p->pMsg, p->Len);
			}
			if (p->bRpy == 1) {
				Que.RpyQue.AddTail(p);
				// do not destroy if reply need
			}
			else {
				delete p; // destroy memory allocation
			}
			return len;
		}
		else {
			// sink the command so that it does not overflow
			//TRACE("Message sinked\n");
			delete p;
		}
	}
	return 0;
}

int CSerialTh::ProcessWriteB() {
	ASSERT(0);
	return 0;
}

int CSerialTh::ProcessReadB() {
	BYTE ch;
	if (!pCom->ReadByte(ch)) {
		return 0;
	}
	cnt = 0;
	ReadBuff[cnt++] = ch;
	//	TRACE1( "%c\n", ch );
	ULONGLONG tick = GetTickCount64();
	while (GetTickCount64() < (tick + CTOut)) {
		if (!pCom->ReadByte(ch)) {
			continue;
		}
		switch (ch) {
		case '\n':
			//TRACE( "<newline>\n" );
			break;
		case '\r':
			// Add ReadBuff to message queue
			ReadBuff[cnt] = 0;
			//				TRACE( ReadBuff );
			//				TRACE( "\n" );
			return cnt;
			break;
		default:
			ReadBuff[cnt++] = ch;
			//				TRACE1( "%c\n", ch );
			break;
		}
		tick = GetTickCount64();
	}
	ReadBuff[cnt] = 0;
	//TRACE( "Inter-charecter timeout: " );
	//TRACE( ReadBuff );
	//TRACE( "\n" );
	return 0;
}

void CSerialTh::SelectProtocol(CSerialTh::PROTOCL proto) {
	Protocol = proto;
}

// Use by client only
CRtnCode::RCODE CSerialTh::Put(char* pMsg, short bRpy) {
	CMsgItem* m = new CMsgItem;
	if (m) {
		if (m->SetAlloc(pMsg, (int)strlen(pMsg))) {
			if (bRpy) {
				if (m->hEvent) {
					CloseHandle(m->hEvent);
				}
				m->hEvent = ::CreateEvent(NULL, FALSE, FALSE, "FSM128");
				if (m->hEvent) {
					// Use by client only
					m->bRpy = 1;
					ResetEvent(m->hEvent);
					CRtnCode::RCODE ret = Que.OutQue.AddTail(m);
					WaitForSingleObject(m->hEvent, INFINITE);
					//AfxMessageBox( "Reply detected" );
				}
			}
			else {
				CRtnCode::RCODE ret = Que.OutQue.AddTail(m);
			}
		}
		else {
			delete m;
		}
	}
	return CRtnCode::ERR;
}

void CSerialTh::SetMessageEventHandler(int MsgInQue) {
	hMsgInQue = MsgInQue;
}

BOOL CSerialTh::GetItemOverComm(int& val) {
	BYTE ch;
	char* p = (char*)&val;
	int idx = 0;
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < CTOut1) {
		if (idx >= sizeof(val)) {
			break; // found ID byte
		}
		if (!pCom->ReadByte(ch)) {
			Sleep(cdelay);
			continue;
		}
		tick = GetTickCount64();
		idx++;
		*(p++) = ch;
		Sleep(1);
	}
	if (idx < sizeof(val)) {
		// Read timeout
		val = 0;
		//TRACE( "Get val timeout\n" );
		return FALSE;
	}
	//	TRACE( "val found\n" );
	return TRUE;
}

BOOL CSerialTh::GetItemOverComm(short& val) {
	BYTE ch;
	char* p = (char*)&val;
	int idx = 0;
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < CTOut1) {
		if (idx >= sizeof(val)) {
			break; // found ID byte
		}
		if (!pCom->ReadByte(ch)) {
			Sleep(cdelay);
			continue;
		}
		tick = GetTickCount64();
		idx++;
		*(p++) = ch;
		Sleep(1);
	}
	if (idx < sizeof(val)) {
		// Read timeout
		val = 0;
		//TRACE( "Get val timeout\n" );
		return FALSE;
	}
	//	TRACE( "val found\n" );
	return TRUE;
}

short CSerialTh::MessageDone(int len, SSDT& Dt) {
	CMsgItem* m = new CMsgItem;
	if (m) {
		if (m->SetAlloc(ReadBuff, len) == CRtnCode::NOERR) {
			if (Dt.Rpy == 2) {
				// Try to match with RpyQue
				CMsgItem* p = Que.RpyQue.Remove(Dt.IDIdx);
				if (p) {
					m->bRpy = Dt.Rpy;
					Que.InQue.AddTail(m);
					SetEvent(p->hEvent);
					delete p;
				}
				else {
					delete m;
				}
			}
			else {
				m->bRpy = Dt.Rpy;
				m->IDIdx = Dt.IDIdx;
				m->MsgSrc = Dt.MsgSrc;
				m->MsgDes = Dt.MsgDes;
				m->MsgEvent = Dt.MsgEvent;
				m->Unit = Dt.Unit;
				m->AckWnd = Dt.AckWnd;
				m->Ack = Dt.Ack;
				m->Nak = Dt.Nak;
				m->DataTyp = (CMsgItem::DATATYPE)Dt.DataTyp;
				m->DataID = (CMsgItem::DATAID)Dt.DataID;
				if (Que.InQue.AddT(m) == CRtnCode::NOERR)	// Add to tail without increment IDIdx
				{
					if (pWnd) {
						if (hMsgInQue) {
							pWnd->PostMessage(hMsgInQue, Dt.IDIdx, (long)m);  // receiver is responsible to delete alloc q
//							CString str;
//							str.Format("Message Posted: %d\n", Dt.IDIdx);
//							TRACE(str);
						}
						else {
							ASSERT(0);
						}
					}
				}
				else {
					delete m;
				}
			}
			return len;
		}
		else {
			delete m;
			return 0;
		}
	}
	return 0;
}