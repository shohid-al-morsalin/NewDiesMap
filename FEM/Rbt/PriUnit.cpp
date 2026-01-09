// PriUnit.cpp: implementation of the CPriUnit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "strSOp.h"
#include "PriUnit.h"
#include "..\..\Sys\Var.h"
//#include "..\..\IPC\SMOL.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//BOOL CPriUnit::Stop()
//{
//	BOOL ret = FALSE;
//	switch(StopCom()) {
//	case CRbtBase::NOTCONNECTED:
//		ErrorStr = "PriUnit was not connected";
//		Log(ErrorStr,2);
//		ASSERT(0);
//		break;
//	case CRbtBase::SUCCESS:
//		ErrorStr = "PriUnit disconnected";
//		Log(ErrorStr,2);
//		ret = TRUE;
//		break;
////	case 1:
////		ErrorStr = "PriUnit disconnected";
////		Log(ErrorStr,2);
////		break;
//	default:
//		ErrorStr = "Unhandled error";
//		Log(ErrorStr,2);
//		ASSERT(0);
//		break;
//	}
//	return ret;
//}

BOOL CPriUnit::ReqStatus() {
	//	if (!IsConnected())
	//	{
	//		ErrorStr = "PriUnit not connected";
	//		Log(ErrorStr,2);
	//		return FALSE;
	//	}
	//	if (!bHomed)
	//	{
	//		ErrorStr = "PriUnit not homed";
	//		Log(ErrorStr,2);
	//		return FALSE;
	//	}
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "";
		return TRUE;
	}
	char cmdStr[256];
	strcpy(cmdStr, "HLLO");
	if (!SendPri(cmdStr)) {
		//		ErrorStr = GetResp();
		//		Log(ErrorStr, 2);
		return FALSE;
	}
	//	ErrorStr = GetResp();
	//	Log(ErrorStr,2);
	return TRUE;
}

BOOL CPriUnit::Xfer(char* text) {
	//	if (!IsConnected())
	//	{
	//		ErrorStr = "PriUnit not connected";
	//		Log(ErrorStr,2);
	//		ASSERT(0);
	//		return FALSE;
	//	}
	//	if (!bHomed)
	//	{
	//		ErrorStr = "PriUnit not homed";
	//		Log(ErrorStr,2);
	//		ASSERT(0);
	//		return FALSE;
	//	}
	if (!CanProceed(1)) {
		return FALSE;
	}
	if (IsSimu()) {
		//		ErrorStr = text;
		//		Log(ErrorStr,2);
		return TRUE;
	}
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < 120000) {
		if (BusyCheck()) {
			Sleep(1000);
			continue;
		}
		tick = -1;
		break;
	}
	if (tick != -1) {
		//		ErrorStr = "Robot busy time out";
		//		Log(ErrorStr,2);
		return FALSE;
	}
	char cmdStr[256];
	strscpy(cmdStr, sizeof(cmdStr), text);
	if (!SendPri(cmdStr)) {
		//		ErrorStr = GetResp();
		//		Log(ErrorStr, 2);
		return FALSE;
	}
	//	Log(GetResp(), 2);
	//	ErrorStr = text;
	//	Log(ErrorStr,2);
	return TRUE;
}

BOOL CPriUnit::HomeAlign(short AlgnNum) {
	//	if (!IsConnected())
	//	{
	//		ErrorStr = "BrkUnit not connected";
	//		Log(ErrorStr,2);
	//		ASSERT(0);
	//		return FALSE;
	//	}
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		//		ErrorStr = "Aligner homed";
		//		Log(ErrorStr,2);
		return TRUE;
	}
	//	ErrorStr = "Aligner homed";
	//	Log(ErrorStr,2);
	return TRUE;
}

BOOL CPriUnit::HomeRobot(short AlgnNum) {
	//	if (!IsConnected())
	//	{
	//		ErrorStr = "PriUnit not connected";
	//		Log(ErrorStr,2);
	//		ASSERT(0);
	//		return FALSE;
	//	}
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		bHomed = TRUE;
		//		ErrorStr = "Robot homed";
		//		Log(ErrorStr,2);
		return TRUE;
	}

	if (IsHomed()) {
		bHomed = TRUE;
		return InitRobot();
	}

	if (!SendPri("XHOM")) {
		//		ErrorStr = GetResp();
		//		Log(ErrorStr,2);
		return FALSE;
	}
	bHomed = TRUE;
	return InitRobot();
}

BOOL CPriUnit::Start(short port) {
	BOOL ret = TRUE;

	switch (StartCom(port)) {
		//	case CRbtBase::CREATED:
		//		ErrorStr.Format("PriUnit already connected to port %d", port);
		//		ASSERT(0);
		//		break;
	case CRbtBase::ALLOCERROR:
		//		ErrorStr = "PriUnit alloc error";
		ret = FALSE;
		//		ASSERT(0);
		break;
	case CRbtBase::OPENERROR:
		//		ErrorStr.Format("PriUnit could not open port %d", port);
		ret = FALSE;
		//		ASSERT(0);
		break;
		//	case CRbtBase::CONNECTED:
		//		ErrorStr = "PriUnit already connected";
		//		ASSERT(0);
		//		break;
		//	case CRbtBase::SUCCESS:
		//		ErrorStr.Format("PriUnit connected to port %d", port);
		//		break;
		//	case 1:
		//		ErrorStr = "PriUnit connected";
		//		break;
	}
	//	Log(ErrorStr,2);
	return ret;
}

CPriUnit::CPriUnit() {
	ID = "PRI";
	Status = 0;
	WaferSize = 300;
}

CPriUnit::~CPriUnit() {
	//	if (bConnected)
	//	{
	//		Stop();
	//	}
}

BOOL CPriUnit::Stat() {
	if (!SendPri("XSTAT")) {
		//		ErrorStr = GetResp();
		//		Log(ErrorStr,2);
		return FALSE;
	}
	Status = ExtractHex(ErrorStr);
	return TRUE;
}

BOOL CPriUnit::IsHomed() {
	if (Stat()) {
		if (Status & 0x40) {
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}

long CPriUnit::ExtractHex(CString& str) {
	long val = 0;
	int c = str.GetLength();
	for (int i = 0; i < c; i++) {
		if (isdigit(str.GetAt(i))) {
			CString sub;
			sub = str.Mid(i);
			sscanf(sub, "%x", &val);
			return val;
		}
	}
	return val;
}

BOOL CPriUnit::InitRobot() {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < 60000) {
		if (BusyCheck()) {
			Sleep(1000);
			continue;
		}
		tick = -1;
		break;
	}
	if (tick != -1) {
		//		Log("PRI Robot busy",2);
		return FALSE;
	}
	if (!StatusCheck()) {
		//		Log("PRI Fail status check",0);
		return FALSE;
	}

	Send("SSP A, 6000, 4000, 700");
	Send("SAD A, 12000, 8000, 1400");
	Send("SDL A, 12000, 8000, 1400");
	Send("SAV");

	Send("ABM");
	Send("SVON");
	Send("PU");

	SetSpeed();

	SetRetractPos();

	SetStation();

	//	ErrorStr = "Robot homed";
	//	Log(ErrorStr,2);

	return TRUE;
}

void CPriUnit::SetSpeed() {
	//OnConnect(); if (!com) return;
	if (BusyCheck()) return;

	Send("SSP A, 6000, 4000, 700");
	Send("SAD A, 12000, 8000, 1400");
	Send("SDL A, 12000, 8000, 1400");
	Send("SAV");
}

void CPriUnit::SetRetractPos() {
	CString a;
	int sret = -6000;
	switch (WaferSize) {
	case 300:
		a.Format("SRET E, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET D, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET C, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET B, %d", sret);
		Send(a.GetBuffer(0));
		break;
	case 200:
		a.Format("SRET F, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET D, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET C, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET B, %d", sret);
		Send(a.GetBuffer(0));
		break;
	case 150:
		a.Format("SRET G, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET D, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET C, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET B, %d", sret);
		Send(a.GetBuffer(0));
		break;
	case 100:
		a.Format("SRET H, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET D, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET C, %d", sret);
		Send(a.GetBuffer(0));
		a.Format("SRET B, %d", sret);
		Send(a.GetBuffer(0));
		break;
	}
	Send("RETH");
}

void CPriUnit::SetStation() {
	if (BusyCheck()) return;
	switch (WaferSize) {
	case 300:
		Send("SWS 300");
		break;
	case 200:
		Send("SWS 200");
		break;
	case 150:
		Send("SWS 150");
		break;
	case 100:
		Send("SWS 100");
		break;
	}
	CString tmp;
	// Set flat angle alignment
	tmp.Format("SFAA %ld", 90 * 100L);
	Send(tmp.GetBuffer(0));
	// Set Max cassette number
	tmp.Format("R57=%d", Var.MaxSlot);
	Send(tmp.GetBuffer(0));
}

BOOL CPriUnit::BusyCheck() {
	if (Stat()) {
		if (Status & 0x240) {
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CPriUnit::StatusCheck() {
	if (Stat()) {
		ErrorStr = "";
		if (Status & 0x8000) {
			if (!Send("AST 1")) {
				ErrorStr += "Error on com1\r\n";
			}
		}
		if (Status & 0x4000) {
			ErrorStr += "Controller Error\r\n";
			return FALSE;
		}
		if (Status & 0x2000) {
			if (!Send("RNCS")) {
				ErrorStr += "NV-SRAM error\r\n";
				return FALSE;
			}
		}
		if (Status & 0x800) {
			if (!Send("AST 2")) {
				ErrorStr += "AST 2 Failed\r\n";
			}
		}
		if (Status & 0x400) {
			if (!Send("SVON")) {
				ErrorStr += "SVON Failed\r\n";
			}
		}
		if (Status & 0x200) {
			// One or more axis are moving [10/22/2002]
			ErrorStr += "One or more axis are moving\r\n";
		}
		if (Status & 0x100) {
			// Macro is running [10/22/2002]
			ErrorStr += "Macro is running\r\n";
		}
		if (Status & 0x80) {
			ErrorStr += "Error on last Aligment\r\n";
		}
		if (Status & 0x40) {
			ErrorStr += "One or more axis are not homed\r\n";
		}
		if (Status & 0x20) {
			if (!Send("RLS")) {
				ErrorStr += "One or more limit switch are triggered\r\n";
			}
		}
		if (Status & 0x10) {
			if (Send("SVON")) {
				ErrorStr += "Motor error on one or more axes\r\n";
			}
		}
		if (Status & 0x8) {
			ErrorStr += "Vacuum swithc is on\r\n";
		}
		if (Status & 0x4) {
			ErrorStr += "Vacuum sensor is activated\r\n";
		}
		if (Status & 0x2) {
			ErrorStr += "Previous command invalid\r\n";
		}
		if (Status & 0x1) {
			ErrorStr += "Previous command not executed\r\n";
		}
		//		Log(ErrorStr,2);
		return TRUE;
	}
	ErrorStr = "STAT fail";
	return FALSE;
}

BOOL CPriUnit::Send(char* text) {
	//	Log(text,10);
	BOOL ret = CRbtUnit::SendPri2(text);
	return ret;
}

BOOL CPriUnit::Map() {
	if (!CanProceed(1)) {
		return FALSE;
	}
	if (!SendPri("XMAP E")) {
		//		ErrorStr = GetResp();
		//		Log(ErrorStr,2);
		return FALSE;
	}
	ErrorStr = GetResp();
	//	Log(ErrorStr.Mid(0, Var.MaxSlot+1),2);
	//	SlotMap[0] = ErrorStr.GetAt(0);
	//	for (int i=1; i<= Var.MaxSlot; i++) {
	//		switch(ErrorStr.GetAt(i)) {
	//		case '0':
	//			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::EMPTY);
	//			break;
	//		case '1':
	//			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::CORRECTLY_OCCUPIED);
	//			break;
	//		default:
	//			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::CROSS_SLOTTED);
	//			break;
	//		}
	//	}
	strcpy(SlotMap, ErrorStr.Left(Var.MaxSlot + 1).GetBuffer(0));
	SlotMap[sizeof(SlotMap) - 1] = 0;
	return TRUE;
}

CString CPriUnit::GetSM() {
	for (int i = 1; i <= Var.MaxSlot; i++) {
		switch (SlotMap[i]) {
		case '0':
			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_EMPTY);
			break;
		case '1':
			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
			break;
		default:
			SlotMap[i] = CSubstObj::GetSMChar(CSubstObj::SM_CROSS_SLOTTED);
			break;
		}
	}
	return SlotMap;
}

BOOL CPriUnit::IsWaferOnArm() {
	ASSERT(!"Not implemented");
	return FALSE;
}

BOOL CPriUnit::IsWaferOnAlgn() {
	ASSERT(!"Not implemented");
	return FALSE;
}