// RbtUnit.cpp: implementation of the CRbtUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RbtUnit.h"
#include "..\..\Src\Comm.h" // Added by ClassView
// #include "..\..\Src\DOSUtil.h"

#include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CString CRbtUnit::GetLastError()
//{
//	return ErrorStr;
//}

CRbtUnit::CRbtUnit() {
	pCom = NULL;
	pCom2 = NULL;//tushar
	// 	bSimu = TRUE;
	bHomed = FALSE;
	bConnected = FALSE;
	bConnected2 = FALSE;

	// 	WaferSize = 300;
	ID = "REAL";

	//	bSimu = DosUtil.GetRbtUnitSimu();
	//	CString str;
	//	CString sect;
	//	CString winini;
	//	DosUtil.GetLocalCfgFile(winini);
	//	sect = "BrkUnit";
	//	bSimu = GetPrivateProfileInt(sect, "bSimu", bSimu, winini);
	//	str.Format("%d", bSimu);
	//	WritePrivateProfileString(sect, "bSimu", str, winini);
}

CRbtUnit::~CRbtUnit() {
	if (bConnected) {
		Stop();
	}
}

CRbtBase::ERRCODE CRbtUnit::StartCom(short port) {
	if (bConnected) {
		ErrorStr = "RbtUnit already connected";
		//		Log(ErrorStr,2);
		return CRbtBase::CONNECTED;
	}
	if (port == 0) {
		bSimu = TRUE;
	}
	if (bSimu) {
		bConnected = TRUE;
		ErrorStr.Format("RbtUnit is simulated on port %d", port);
		//		Log(ErrorStr,2);
		return CRbtBase::SUCCESS;
	}
	if (pCom) {
		// Already connected error
		ErrorStr.Format("RbtUnit already connected to port %d", port);
		//		Log(ErrorStr,2);
		return CRbtBase::CREATED;
	}
	pCom = new CComm;
	if (!pCom) {
		// Object creation error
		ErrorStr = "RbtUnit alloc error";
		//		Log(ErrorStr,2);
		return CRbtBase::ALLOCERROR;
	}
	pCom->SetParam(BYTE(port), CBR_9600, 8, NOPARITY, /*TWOSTOPBITS*/ONE5STOPBITS, 0);
	if (!pCom->Open()) {
		// Open port error
		delete pCom;
		pCom = NULL;
		ErrorStr.Format("RbtUnit could not open port %d", port);
		//		Log(ErrorStr,2);
		return CRbtBase::OPENERROR;
	}
	bConnected = TRUE;
	ErrorStr.Format("RbtUnit connected to port %d", port);
	//	Log(ErrorStr,2);
	return CRbtBase::SUCCESS;
}

//[tushar 21082024
CRbtBase::ERRCODE CRbtUnit::StartCom2(short port) {
	if (bConnected2) {
		ErrorStr2 = "LogosolUnit already connected";
		//		Log(ErrorStr,2);
		return CRbtBase::CONNECTED;
	}
	if (port == 0) {
		bSimu = TRUE;
	}
	if (bSimu) {
		bConnected2 = TRUE;
		ErrorStr2.Format("LogosolUnit is simulated on port %d", port);
		//		Log(ErrorStr,2);
		return CRbtBase::SUCCESS;
	}
	if (pCom2) {
		// Already connected error
		ErrorStr2.Format("LogosolUnit already connected to port %d", port);
		//		Log(ErrorStr,2);
		return CRbtBase::CREATED;
	}
	pCom2 = new CComm;
	if (!pCom2) {
		// Object creation error
		ErrorStr2 = "LogosolUnit alloc error";
		//		Log(ErrorStr,2);
		return CRbtBase::ALLOCERROR;
	}
	pCom2->SetParam(BYTE(port), CBR_9600, 8, NOPARITY, /*TWOSTOPBITS*/ONE5STOPBITS, 0);
	if (!pCom2->Open()) {
		// Open port error
		delete pCom2;
		pCom2 = NULL;
		ErrorStr2.Format("LogosolUnit could not open port %d", port);
		//		Log(ErrorStr,2);
		return CRbtBase::OPENERROR;
	}
	bConnected2 = TRUE;
	ErrorStr2.Format("LogosolUnit connected to port %d", port);
	//	Log(ErrorStr,2);
	return CRbtBase::SUCCESS;
}
//]
short CRbtUnit::StopCom() {
	if (!bConnected) {
		return CRbtBase::NOTCONNECTED;
	}
	if (bSimu) {
		bConnected = FALSE;
		return CRbtBase::SUCCESS;
	}
	if (!pCom) {
		return CRbtBase::NOTCREATED;
	}
	pCom->Close();
	delete pCom;
	pCom = NULL;
	bConnected = FALSE;
//tushar
	if (pCom2) {
		pCom2->Close();
		delete pCom2;
		pCom2 = NULL;
	}
	//
	return CRbtBase::SUCCESS;
}

CString CRbtUnit::GetResp() {
	if (pCom) {
		return pCom->resp;
	}
	// 	ASSERT(0);
	return "";
}

BOOL CRbtUnit::SendBrk(char* text, int tout) {
	if (pCom) {
		BOOL ret = pCom->SendBrk(text, tout);
		if (!ret) {
			ErrorStr.Format("Fail to send %s", text);
			Log(ErrorStr, 2);
		}
		else {
			ErrorStr = pCom->resp;
			Log(ErrorStr, 3);
		}
		return ret;
	}
	// 	ASSERT(0);
	return FALSE;
}
//Tushar 21082024
BOOL CRbtUnit::SendLogosol(char* text) {
	if (pCom2) {
		BOOL ret = pCom2->SendLogosol(text);
		if (!ret) {
			ErrorStr.Format("Fail to send %s", text);
			Log(ErrorStr, 2);
		}
		else {
			ErrorStr = pCom2->resp;
			Log(ErrorStr, 3);
		}
		return ret;
	}
	// 	ASSERT(0);
	return FALSE;
}

BOOL CRbtUnit::SendLogosolTest(char* text) {
	if (pCom2) {
		BOOL ret = pCom2->SendLogosolTest(text);
		if (!ret) {
			ErrorStr.Format("Fail to send %s", text);
			Log(ErrorStr, 2);
		}
		else {
			ErrorStr = pCom2->resp;
			Log(ErrorStr, 3);
		}
		return ret;
	}
	// 	ASSERT(0);
	return FALSE;
}

BOOL CRbtUnit::SendLogosolAction(char* text) {
	if (pCom2) {
		BOOL ret = pCom2->SendLogosolAction(text);
		if (!ret) {
			Log(ErrorStr, 2);
			ErrorStr.Format("Fail to send %s", text);
			Log(ErrorStr, 2);
		}
		else {
			ErrorStr = pCom2->resp;
			Log(ErrorStr, 3);
		}
		return ret;
	}
	// 	ASSERT(0);
	return FALSE;
}
//

BOOL CRbtUnit::SetComMon() {
	if (pCom) {
		BOOL ret = pCom->SendBrk("SET COMM M/B MON");
		ErrorStr = pCom->resp;
		return ret;
	}
	return FALSE;
}

BOOL CRbtUnit::SendPri(char* text) {
	if (pCom) {
		BOOL ret = pCom->SendPri(text);
		ErrorStr = pCom->resp;
		//		if (!ret) {
		//			Log(ErrorStr,2);
		//		}
		//		else {
		//			Log(ErrorStr,3);
		//		}
		return ret;
	}
	// 	ASSERT(0);
	return FALSE;
}

BOOL CRbtUnit::IsConnected() {
	return bConnected;
}

BOOL CRbtUnit::IsSimu() {
	return bSimu;
}

BOOL CRbtUnit::SendPri2(char* text) {
	if (pCom) {
		return pCom->SendPri2(text);
	}
	// 	ASSERT(0);
	return FALSE;
}

BOOL CRbtUnit::Stop() {
	BOOL ret = FALSE;
	switch (StopCom()) {
	case CRbtBase::NOTCONNECTED:
		ErrorStr = "RbtUnit was not connected";
		//		Log(ErrorStr,2);
		//		ASSERT(0);
		break;
	case CRbtBase::NOTCREATED:
		ErrorStr = "RbtUnit was not created";
		//		Log(ErrorStr,2);
		//		ASSERT(0);
		break;
	case CRbtBase::SUCCESS:
		ErrorStr = "RbtUnit disconnected";
		//		Log(ErrorStr,2);
		ret = TRUE;
		break;
		//	case 1:
		//		ErrorStr = "RbtUnit disconnected";
		//		Log(ErrorStr,2);
		//		break;
	default:
		ErrorStr = "Unhandled error";
		//		Log(ErrorStr,2);
		ASSERT(0);
		break;
	}
	return ret;
}

BOOL CRbtUnit::CanProceed(short level) {
	if (!IsConnected()) {
		ErrorStr = "RbtUnit not connected";
		//		Log(ErrorStr,2);
		return FALSE;
	}
	if (level < 1) {
		return TRUE;
	}
	if (!bHomed) {
		ErrorStr = "RbtUnit not homed";
		//		Log(ErrorStr,2);
		return FALSE;
	}
	return TRUE;
}

void CRbtUnit::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CRbtUnit");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}