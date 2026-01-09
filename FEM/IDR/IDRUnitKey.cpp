// IDRUnitKey.cpp: implementation of the CIDRUnitKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "IDRUnitKey.h"
#include "..\..\SYS\Tool.h"
#include "..\..\SRC\Comm.h"// Added by ClassView

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//extern CString CIDRCarrierID1, CIDRCarrierID2;

BOOL CIDRUnitKey::ReadID() {
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

	int n;

	switch (Unit) {
	case 1:
		if (!pCom->SendKEY("%T01-LON")) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		//		Log(pCom->resp,3);
		break;
	case 2:
		if (!pCom->SendKEY("%T02-LON")) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		//		Log(pCom->resp,3);
		break;
	}

	switch (Unit) {
	case 1:
		Tool.IDRCarrierID1 = pCom->resp;
		if (Tool.IDRCarrierID1.GetLength() > 1) {
			if (Tool.IDRCarrierID1.Find("ERROR") != -1) {
				PostMessage(hWnd, ID_CID_READFAILED, Unit, 0);
				return FALSE;
			}
			n = Tool.IDRCarrierID1.Find('\r');
			if (n != -1) {
				Tool.IDRCarrierID1.SetAt(n, 0);
				PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID1.GetBuffer(0));
				return TRUE;
			}
		}
		break;
	case 2:
		Tool.IDRCarrierID2 = pCom->resp;
		if (Tool.IDRCarrierID2.GetLength() > 1) {
			if (Tool.IDRCarrierID2.Find("ERROR") != -1) {
				PostMessage(hWnd, ID_CID_READFAILED, Unit, 0);
				return FALSE;
			}
			n = Tool.IDRCarrierID2.Find('\r');
			if (n != -1) {
				Tool.IDRCarrierID2.SetAt(n, 0);
				PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID2.GetBuffer(0));
				return TRUE;
			}
		}
		break;
	case 3:
		Tool.IDRCarrierID3 = pCom->resp;
		if (Tool.IDRCarrierID3.GetLength() > 1) {
			if (Tool.IDRCarrierID3.Find("ERROR") != -1) {
				PostMessage(hWnd, ID_CID_READFAILED, Unit, 0);
				return FALSE;
			}
			n = Tool.IDRCarrierID3.Find('\r');
			if (n != -1) {
				Tool.IDRCarrierID3.SetAt(n, 0);
				PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID3.GetBuffer(0));
				return TRUE;
			}
		}
		break;
	case 4:
		Tool.IDRCarrierID4 = pCom->resp;
		if (Tool.IDRCarrierID4.GetLength() > 1) {
			if (Tool.IDRCarrierID4.Find("ERROR") != -1) {
				PostMessage(hWnd, ID_CID_READFAILED, Unit, 0);
				return FALSE;
			}
			n = Tool.IDRCarrierID4.Find('\r');
			if (n != -1) {
				Tool.IDRCarrierID4.SetAt(n, 0);
				PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID4.GetBuffer(0));
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

CIDRUnitKey::CIDRUnitKey() {
	Parity = 2;
	bitSize = 7;
}

CIDRUnitKey::~CIDRUnitKey() {}

BOOL CIDRUnitKey::GetDevStatus() {
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

	switch (Unit) {
	case 1:
		if (!pCom->SendKEY("SRC01")) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		break;
	case 2:
		if (!pCom->SendKEY("SRC02")) {
			ErrorStr = pCom->resp;
			return FALSE;
		}
		break;
	}

	if (ErrorStr.Find("ON")) {
		return TRUE;
	}
	return FALSE;
}

BOOL CIDRUnitKey::WriteID(char* id) {
	return FALSE;
}

char* CIDRUnitKey::GetCID(char* cid, short len) {
	return NULL;
}

CString CIDRUnitKey::ReadCID() {
	if (!pCom) {
		ErrorStr = "No pCom";
		return ErrorStr;
	}

	if (!pCom->bConnected) {
		ErrorStr = "Not connected";
		return ErrorStr;
	}

	int n;

	switch (Unit) {
	case 1:
		if (!pCom->SendKEY("%T01-LON")) {
			ErrorStr = pCom->resp;
			return ErrorStr;
		}
		break;
	case 2:
		if (!pCom->SendKEY("%T02-LON")) {
			ErrorStr = pCom->resp;
			return ErrorStr;
		}
		break;
	}

	CString CID;
	CID = pCom->resp;
	if (CID.GetLength() > 1) {
		if (CID.Find("ERROR") != -1) {
			return CID;
		}
		n = CID.Find('\r');
		if (n != -1) {
			CID.SetAt(n, 0);
			return CID;
		}
	}
	return "READ ERROR";
}