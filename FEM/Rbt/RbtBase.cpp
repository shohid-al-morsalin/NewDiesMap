// RbtBase.cpp: implementation of the CRbtBase class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RbtBase.h"
#include "..\..\Src\DOSUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CRbtBase::SetWaferSize(short size) {
	WaferSize = size;
	ErrorStr.Format("Wafer size set to %d", WaferSize);
}

BOOL CRbtBase::ReqStatus() {
	return TRUE;
}

BOOL CRbtBase::GetTinyEye() {
	ErrorStr = "Tiny Eye READ";
	WaferAtLoc = 0xFFFFFFFF;
	return TRUE;
}

BOOL CRbtBase::WaferPresentCheck() {
	ErrorStr = "Wafer Present Checked";
	WaferAtLoc = 0xFFFFFF;
	return TRUE;
}

BOOL CRbtBase::Xfer(char* text) {
	ErrorStr = text;
	return TRUE;
}

BOOL CRbtBase::SetComMon() {
	return TRUE;
}

BOOL CRbtBase::HomeAlign(short AlgnNum) {
	ErrorStr = "Aligner homed";
	switch (AlgnNum) {
	case 1:
	case 2:
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CRbtBase::HomeRobot(short AlgnNum) {
	ErrorStr = "Robot homed";
	switch (AlgnNum) {
	case 1:
	case 2:
		break;
	default:
		ASSERT(0);
		break;
	}
	return TRUE;
}

BOOL CRbtBase::Start(short port) {
	ErrorStr.Format("RbtUnit connected to port %d", port);
	return TRUE;
}

CString CRbtBase::GetLastError() {
	return ErrorStr;
}

BOOL CRbtBase::Stop() {
	ErrorStr = "Disconnected";
	return TRUE;
}

CRbtBase::CRbtBase() {
	bSimu = TRUE;
	bSimu = DosUtil.GetRbtUnitSimu();

	ID = "SIMU";

	memset(SlotMap, 0, MAXCAPACITYLEN);

	WaferSize = 300;
}

CRbtBase::~CRbtBase() {}

BOOL CRbtBase::Map() {
	memset(SlotMap, 0, MAXCAPACITYLEN);
	return TRUE;
}

BOOL CRbtBase::IsWaferOnArm() {
	return FALSE;
}

BOOL CRbtBase::IsWaferOnArm(short arm) {
	return FALSE;
}

BOOL CRbtBase::IsWaferOnAlgn() {
	return FALSE;
}

BOOL CRbtBase::IsWaferOnAlgn(short arm) {
	return FALSE;
}

BOOL CRbtBase::Align() {
	return TRUE;
}

BOOL CRbtBase::SetPostPos(short AlgnNum, long PostPos, long OffsetPos) {
	return TRUE;
}
//just test
BOOL CRbtBase::chkResponse() {
	return TRUE;
}