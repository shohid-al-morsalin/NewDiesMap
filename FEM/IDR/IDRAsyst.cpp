// IDRAsyst.cpp: implementation of the CIDRAsyst class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "IDRAsyst.h"
#include "..\..\SYS\Tool.h"
#include "..\..\SRC\Comm.h" // Added by ClassView

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIDRAsyst::CIDRAsyst() {}

CIDRAsyst::~CIDRAsyst() {}

BOOL CIDRAsyst::ReadData(short Seg, short len) {
	if (!pCom) {
		ASSERT(0);
		ErrorStr = "No pCom";
		return FALSE;
	}
	if (!pCom->bConnected) {
		ASSERT(0);
		ErrorStr = "Not connected";
		return FALSE;
	}
	CString msg;
	switch (Unit) {
	case 1:
		msg.Format("%02d HCS RR %d %d", IDR1, Seg, len);
		if (!pCom->SendHCA(msg, 6000)) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		return TRUE;
		break;
	case 2:
		msg.Format("%02d HCS RR %d %d", IDR2, Seg, len);
		if (!pCom->SendHCA(msg, 6000)) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		return TRUE;
		break;
	}
	//	}
	return FALSE;
}

BOOL CIDRAsyst::WriteData(short Seg, short len, char* data) {
	if (!pCom) {
		ASSERT(0);
		ErrorStr = "No pCom";
		return FALSE;
	}
	if (!pCom->bConnected) {
		ASSERT(0);
		ErrorStr = "Not connected";
		return FALSE;
	}
	CString msg;
	switch (Unit) {
	case 1:
		msg.Format("%02d HCS WDR %d %d %s", IDR1, Seg, len, data);
		if (!pCom->SendHCA(msg, 6000)) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		return TRUE;
		break;
	case 2:
		msg.Format("%02d HCS WDR %d %d %s", IDR2, Seg, len, data);
		if (!pCom->SendHCA(msg, 6000)) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CIDRAsyst::WriteID(char* id) {
	if (!pCom) {
		ASSERT(0);
		ErrorStr = "No pCom";
		return FALSE;
	}
	if (!pCom->bConnected) {
		ASSERT(0);
		ErrorStr = "Not connected";
		return FALSE;
	}
	CString msg;
	switch (Unit) {
	case 1:
		msg.Format("%02d HCS WMID %s", IDR1, id);
		if (!pCom->SendHCA(msg, 6000)) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		break;
	case 2:
		msg.Format("%02d HCS WMID %s", IDR2, id);
		if (!pCom->SendHCA(msg, 6000)) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		break;
	}
	return FALSE;
}

// Yuen: Do not lock [2002/11/03  12:54]
char* CIDRAsyst::GetCID(char* cid, short len) {
	short cnt = 0;
	char* q = NULL;
	char* p = cid + 16;
	len -= 16;
	for (int i = 0; i < len; i++) {
		if (*p == ' ') {
			p++;
			cnt++;
		}
		else {
			q = p;
			break;
		}
	}
	if (q) {
		for (int i = cnt; i < len; i++) {
			if (*p == '\r') {
				*p = 0;
				break;
			}
			p++;
		}
		return q;
	}
	return NULL;
}

CString CIDRAsyst::ReadCID() {
	if (!pCom) {
		ErrorStr = "No pCom";
		return ErrorStr;
	}
	if (!pCom->bConnected) {
		ErrorStr = "Not connected";
		return ErrorStr;
	}
	CString cmd;

	switch (Unit) {
	case 1:
		cmd.Format("%02d ", IDR1);
		break;
	case 2:
		cmd.Format("%02d ", IDR2);
		break;
	}
	cmd += "HCS RMID";
	if (!pCom->SendHCA(cmd, 6000)) {
		Sleep(500);
		if (!pCom->SendHCA(cmd, 6000)) {
			ErrorStr = pCom->resp;
			return ErrorStr;
		}
	}
	for (int i = 0; i < sizeof(pCom->resp); i++) {
		if (pCom->resp[i] == '\\') {
			pCom->resp[i] = 0;
			break;
		}
	}
	short len = short(strlen(pCom->resp));
	return GetCID(pCom->resp, len);
}

BOOL CIDRAsyst::ReadID() {
	ErrorStr = "";
	if (!pCom) {
		ErrorStr = "No pCom";
		return FALSE;
	}
	if (!pCom->bConnected) {
		ErrorStr = "Not connected";
		return FALSE;
	}
	int i;
	CString str;
	switch (Unit) {
	case 1:
		str.Format("%02d HCS RMID", IDR1);
		break;
	case 2:
		str.Format("%02d HCS RMID", IDR2);
		break;
	default:
		ASSERT(0);
		return FALSE;
		break;
	}
	int n = 3;
	// Retry n times
	for (i = 0; i < n; i++) {
		if (!pCom->SendHCA(str, 6000)) {
			Sleep(500);
			continue;
		}
		else break;
	}
	if (i == n) {
		ErrorStr = pCom->resp;
		return FALSE;
	}
	for (i = 0; i < sizeof(pCom->resp); i++) {
		if (pCom->resp[i] == '\\') {
			pCom->resp[i] = 0;
			break;
		}
	}
	short len = short(strlen(pCom->resp));
	switch (Unit) {
	case 1:
		Tool.IDRCarrierID1 = GetCID(pCom->resp, len);
		if (Tool.IDRCarrierID1.GetLength() > 1) {
			PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID1.GetBuffer(0));
			return TRUE;
		}
		break;
	case 2:
		Tool.IDRCarrierID2 = GetCID(pCom->resp, len);
		if (Tool.IDRCarrierID2.GetLength() > 1) {
			PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID2.GetBuffer(0));
			return TRUE;
		}
		break;
	}
	strcpy((char*)pCom->resp, "Fail to read CID");
	return FALSE;
}

BOOL CIDRAsyst::GetDevStatus() {
	if (!pCom) {
		ASSERT(0);
		ErrorStr = "No pCom";
		return FALSE;
	}
	if (!pCom->bConnected) {
		ASSERT(0);
		ErrorStr = "Not connected";
		return FALSE;
	}
	CString str;
	switch (Unit) {
	case 1:
		str.Format("%02d HCS R", IDR1);
		if (!pCom->SendHCA(str, 3000)) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		return TRUE;
		break;
	case 2:
		str.Format("%02d HCS R", IDR2);
		if (!pCom->SendHCA(str, 3000)) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		return TRUE;
		break;
	}
	return FALSE;
}