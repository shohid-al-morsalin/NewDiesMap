// IDRUnitOmron.cpp: implementation of the CIDRUnitOmron class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "IDRUnitOmron.h"
#include "..\..\SYS\Tool.h"
#include "..\..\SRC\Comm.h"// Added by ClassView

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//extern CString CIDRCarrierID1, CIDRCarrierID2;

CIDRUnitOmron::CIDRUnitOmron() {
	Parity = 2;
	bitSize = 8;
	cmdRead = "0100";
	cmdWrite = "0200";
	cmdTest, "10";
	memset(ID, 0, sizeof(ID));
}

CIDRUnitOmron::~CIDRUnitOmron() {}

BOOL CIDRUnitOmron::ReadID() {
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
	CString cmd;
	CString page1n2 = "0000000C";
	cmd = cmdRead + page1n2 + "\r";
	if (!pCom->SendOmron(cmd)) {
		ErrorStr = pCom->resp;
		PostMessage(hWnd, ID_CID_READFAILED, Unit, 0);
		return FALSE;
	}
	//	Log(pCom->resp,3);

	if (strstr((char*)pCom->resp, "72")) {
		//		Log("CIDR read fail",0);
		return FALSE;
	}
	switch (Unit) {
	case 1:
		Tool.IDRCarrierID1 = GetCID((char*)pCom->resp, 34);
		RemoveTrailingSpaces(Tool.IDRCarrierID1);
		//		Log(Tool.IDRCarrierID1,10);
		if (Tool.IDRCarrierID1.GetLength() > 1) {
			PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID1.GetBuffer(0));
			return TRUE;
		}
		break;
	case 2:
		Tool.IDRCarrierID2 = GetCID((char*)pCom->resp, 34);
		RemoveTrailingSpaces(Tool.IDRCarrierID2);
		//		Log(Tool.IDRCarrierID2,10);
		if (Tool.IDRCarrierID2.GetLength() > 1) {
			PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID2.GetBuffer(0));
			return TRUE;
		}
		break;
	case 3:
		Tool.IDRCarrierID3 = GetCID((char*)pCom->resp, 34);
		RemoveTrailingSpaces(Tool.IDRCarrierID3);
		//		Log(Tool.IDRCarrierID3,10);
		if (Tool.IDRCarrierID3.GetLength() > 1) {
			PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID3.GetBuffer(0));
			return TRUE;
		}
		break;
	case 4:
		Tool.IDRCarrierID4 = GetCID((char*)pCom->resp, 34);
		RemoveTrailingSpaces(Tool.IDRCarrierID4);
		//		Log(Tool.IDRCarrierID4,10);
		if (Tool.IDRCarrierID4.GetLength() > 1) {
			PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID4.GetBuffer(0));
			return TRUE;
		}
		break;
	}

	return TRUE;
}

// Yuen: Do not lock [2002/11/03  12:47]
char* CIDRUnitOmron::GetCID(char* cid, short len) {
	if ((*cid != '0') || (*(cid + 1) != '0')) {
		strcpy((char*)pCom->resp, "Error:Read");
		return NULL;
	}
	// Format : ASCII encoded hexa number
	char* q = ID;
	char* p = cid + 2;
	for (int i = 0; i < ((len - 2) / 2); i++) {
		char b;
		b = *(p + 1) - '0';
		if (b > 9) b -= 7;
		*(q++) = ((*p - '0') << 4) + b;
		p += 2;
	}
	*q = 0;
	return ID;
}

BOOL CIDRUnitOmron::GetDevStatus() {
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

	if (!pCom->SendOmron("1011111111")) {
		ErrorStr = pCom->resp;
		return FALSE;
	}
	ErrorStr = pCom->resp;

	if (ErrorStr.Find("00")) {
		return TRUE;
	}
	return FALSE;
}

void CIDRUnitOmron::RemoveTrailingSpaces(CString& str) {
	str = str.Left(str.Find(' '));
}

BOOL CIDRUnitOmron::WriteID(char* id) {
	return FALSE;
}

CString CIDRUnitOmron::ReadCID() {
	if (!pCom) {
		ErrorStr = "No pCom";
		return ErrorStr;
	}

	if (!pCom->bConnected) {
		ErrorStr = "Not connected";
		return ErrorStr;
	}
	CString cmd;
	CString page1n2 = "0000000C";
	cmd = cmdRead + page1n2 + "\r";
	if (!pCom->SendOmron(cmd)) {
		ErrorStr = pCom->resp;
		PostMessage(hWnd, ID_CID_READFAILED, Unit, 0);
		return ErrorStr;
	}

	if (strstr((char*)pCom->resp, "72")) {
		return "CIDR read fail";
	}
	cmd = GetCID((char*)pCom->resp, 34);
	RemoveTrailingSpaces(cmd);
	return cmd;
}