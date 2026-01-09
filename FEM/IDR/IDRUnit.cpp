// IDRUnit.cpp: implementation of the CIDRUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "GlobalDeviceContainer.h"
#include "InputCarrierID.h"
#include "IPC/SMHdr.h"
#include "SRC/Comm.h"
#include "SRC/DOSUtil.h"
#include "SYS/Tool.h"

#include "IDRUnit.h"

CString DefaultCarrierID1 = "1A27S360";
CString DefaultCarrierID2 = "1A27S361";
CString DefaultCarrierID3 = "1A27S362";
CString DefaultCarrierID4 = "1A27S363";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIDRUnit::CIDRUnit() {
	hWnd = NULL;
	port = 0;

	IDR1 = 1;
	IDR2 = 2;

	Unit = 0;

	pCom = NULL;
	Parity = 0;
	bitSize = 8;
	bDisabled = FALSE;

	//CString sect;
	//CString winini;
	//DosUtil.GetLocalCfgFile(winini);
	//sect = "CIDRUnit";
	//bSimu = GetPrivateProfileInt(sect, "bSimu", 1, winini);

	IDR1 = DosUtil.GetCIDRUnit(1);
	DosUtil.SetCIDRUnit(1, IDR1);

	IDR2 = DosUtil.GetCIDRUnit(2);
	DosUtil.SetCIDRUnit(2, IDR2);
}

CIDRUnit::~CIDRUnit() {
	Stop();

	//CString str;
	//CString sect;
	//CString winini;
	//DosUtil.GetLocalCfgFile(winini);
	//sect = "CIDRUnit";
	//str.Format("%d", bSimu);
	//WritePrivateProfileString(sect, "bSimu", str, winini);
}

BOOL CIDRUnit::Start(short unit, short pot) {
	Unit = unit;
	port = pot;
	if (!port) {
		bSimul = TRUE;
		ErrorStr = "Enter simulation";
		return TRUE;
	}
	bSimul = FALSE;

	if (bSimul) {
		ErrorStr.Format("CIDRUnit connected to port %d", port);
		return TRUE;
	}

	if (pCom) {
		ASSERT(0);
		ErrorStr.Format("CIDRUnit already connected to port %d", port);
		return TRUE;
	}

	pCom = new CComm;
	if (!pCom) {
		ASSERT(0);
		ErrorStr = "ComSvr alloc error";
		return FALSE;
	}

	pCom->SetParam(unsigned char(pot), 9600, bitSize, Parity, 1, 0);
	if (!pCom->Open()) {
		//		ASSERT(0);
		delete pCom;
		pCom = NULL;
		ErrorStr.Format("CIDRUnit could not open port %d", pot);
		return FALSE;
	}

	Unit = unit;
	port = pot;
	ErrorStr.Format("CIDRUnit connected to port %d", port);

	if (!IsDeviceOK()) {
		pCom->Close();
		delete pCom;
		pCom = NULL;

		ErrorStr.Format("CIDR %d at port %d fail to response", unit, port);
		if (pGDev) {
			switch (unit) {
			case 1:
				pGDev->SetAlarm(CAlrmList::CIDR1COMMFAILURE, ErrorStr);
				break;
			case 2:
				pGDev->SetAlarm(CAlrmList::CIDR2COMMFAILURE, ErrorStr);
				break;
			default:
				ASSERT(0);
				break;
			}
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CIDRUnit::Stop() {
	ErrorStr = "";
	if (bSimul) {
		port = 0;
		ErrorStr = "CIDRUnit disconnected";
		return TRUE;
	}

	if (pCom) {
		pCom->Close();
		delete pCom;
		pCom = NULL;
		port = 0;
		ErrorStr = "CIDRUnit disconnected";
		return TRUE;
	}

	port = 0;
	ErrorStr = "CIDRUnit was not connected";
	return FALSE;
}

BOOL CIDRUnit::ReadCarrierID() {
	if (bDisabled) {
		PostMessage(hWnd, ID_CID_READBYPASSED, Unit, 0);
		return TRUE;
	}
	if (bSimul) {
		// 		CInputCarrierID dlg;
		// 		switch (Unit)
		// 		{
		// 		case 1:
		// 			dlg.m_CarrierID = DefaultCarrierID1;
		// 			break;
		// 		case 2:
		// 			dlg.m_CarrierID = DefaultCarrierID2;
		// 			break;
		// 		}
		// 		if (dlg.DoModal() != IDOK)
		// 		{
		// 			return FALSE;
		// 		}
		// 		CString str = dlg.m_CarrierID;
		switch (Unit) {
		case 1:
			Tool.IDRCarrierID1 = DefaultCarrierID1;
			if (hWnd) {
				PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID1.GetBuffer(0));
			}
			break;
		case 2:
			Tool.IDRCarrierID2 = DefaultCarrierID2;
			if (hWnd) {
				PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID2.GetBuffer(0));
			}
			break;
		case 3:
			Tool.IDRCarrierID3 = DefaultCarrierID3;
			if (hWnd) {
				PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID3.GetBuffer(0));
			}
			break;
		case 4:
			Tool.IDRCarrierID4 = DefaultCarrierID4;
			if (hWnd) {
				PostMessage(hWnd, ID_CID_READIDOK, Unit, (long)Tool.IDRCarrierID4.GetBuffer(0));
			}
			break;
		}
		return TRUE;
	}
	else {
		if (ReadID()) {
			// YFL: Customize ID data here for specific customer [2/4/2005]
			// For Crolles2 900Combo
			switch (Unit) {
			case 1:
				Tool.IDRCarrierID1.TrimLeft();
				Tool.IDRCarrierID1.TrimRight();
				break;
			case 2:
				Tool.IDRCarrierID2.TrimLeft();
				Tool.IDRCarrierID2.TrimRight();
				break;
			case 3:
				Tool.IDRCarrierID3.TrimLeft();
				Tool.IDRCarrierID3.TrimRight();
				break;
			case 4:
				Tool.IDRCarrierID4.TrimLeft();
				Tool.IDRCarrierID4.TrimRight();
				break;
			default:
				ASSERT(0);
				break;
			}
			return TRUE;
		}
	}
	return FALSE;
}

CString CIDRUnit::GetLastError() {
	return ErrorStr;
}

//void CIDRUnit::Log(CString msg, short lvl)
//{
//	CString unt;
//	unt.Format("CIDRUnit%d",Unit);
//	GDev.Log(unt,msg,lvl);
//}

void CIDRUnit::SetPort(short pot) {
	port = pot;
}

BOOL CIDRUnit::IsDeviceOK() {
	if (bSimul) {
		return TRUE;
	}
	return GetDevStatus();
}

BOOL CIDRUnit::WriteCarrierID(char* id) {
	if (bSimul) {
		return TRUE;
	}
	return WriteID(id);
}

void CIDRUnit::Enable(BOOL bAction) {
	bDisabled = !bAction;
}

void CIDRUnit::Register(CSMHdr& SM) {
	hWnd = SM.HWin;
}

void CIDRUnit::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CIDRUnit");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}