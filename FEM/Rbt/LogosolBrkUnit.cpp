#include "stdafx.h"

#include "GlobalDeviceContainer.h"
#include "LTW/LTWMgr.h"
#include "SRC/DOSUtil.h"

#include "LogosolBrkUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CLogosolBrkUnit::CLogosolBrkUnit() {
	ID = "LOGOSOLBRK";
	WaferAtLoc = 0xFFFFFFFF;

	arm = 0;

	DeviceType = STANDARD;

	// Pre-load ini file
	DosUtil.GetAlignerPostPos(1);
	DosUtil.GetAlignerOffsetPos(1);
	DosUtil.GetAlignerPostPos(2);
	DosUtil.GetAlignerOffsetPos(2);
}

CLogosolBrkUnit::~CLogosolBrkUnit() {}

BOOL CLogosolBrkUnit::Start(short port) {
	BOOL ret = TRUE;
	short port2= DosUtil.GetLogosolAlignerPort();//for logosol port
	switch (StartCom(port) && StartCom2(port2)) {
		//	case CRbtBase::CREATED:
		//		ErrorStr.Format("BrkUnit already connected to port %d", port);
		//		ASSERT(0);
		//		break;
	case CRbtBase::ALLOCERROR:
		ErrorStr = "LogosolBrkUnit alloc error";
		ret = FALSE;
		//		ASSERT(0);
		break;
	case CRbtBase::OPENERROR:
		ErrorStr.Format("LogosolBrkUnit could not open port %d", port);
		ret = FALSE;
		//		ASSERT(0);
		break;
		//	case CRbtBase::CONNECTED:
		//		ErrorStr = "BrkUnit already connected";
		//		ASSERT(0);
		//		break;
	case CRbtBase::SUCCESS:
		ErrorStr.Format("LogosolBrkUnit connected to port %d", port);
		LockDoor(TRUE);
		break;
		//	case 1:
		//		ErrorStr = "BrkUnit connected";
		//		LockDoor(TRUE);
		//		break;
	}
	Log(ErrorStr, 2);
	return ret;
}

BOOL CLogosolBrkUnit::Stop() {
	// Must unlock before pCom could be destroyed
	Log("Unlock door", 10);
	LockDoor(FALSE);

	return CRbtUnit::Stop();
}

BOOL CLogosolBrkUnit::HomeAlign(short AlgnNum) {
	switch (AlgnNum) {
	case 1:	// Aligner #1 [7/8/2012 Yuen]
	case 2:	// Aligner #2 [7/8/2012 Yuen]
		break;
	default:
		ASSERT(0);
		return FALSE;
	}

	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "Aligner homed";
		Log(ErrorStr, 2);
		return TRUE;
	}

	if (!SendLogosol("SON")) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}
	BOOL isComleted = FALSE;
	if (!SendLogosolTest("HOM")) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}

	long PostPos;
	long OffPos;

	PostPos = DosUtil.GetLogosolAlignerPostPosAngle();

	CString str;
	switch (WaferSize) {
	case 300:
		CString CCDPOS, POSTPOS;
		CCDPOS.Format("_SA %d", DosUtil.GetCCDNum(AlgnNum));
		POSTPOS.Format("FWOH %d", PostPos);
		if (!SendLogosol("WSZ 12")) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		if (!SendLogosol("_WT 0")) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		if (!SendLogosol(POSTPOS.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		if (!SendLogosol(CCDPOS.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		break;
	}
	Log(GetResp(), 3);
	return TRUE;
}

BOOL CLogosolBrkUnit::HomeRobot(short AlgnNum) {
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		bHomed = TRUE;
		ErrorStr = "Robot homed";
		Log(ErrorStr, 2);
		return TRUE;
	}
	if (!SendBrk("RQ LOAD", 3000)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 2);
		return FALSE;
	}

	if (!SendBrk("HOME ALL")) {
		ErrorStr = GetResp();
		Log(ErrorStr, 2);
		return FALSE;
	}
	// 	if (type == 1)
	// 	{
	// 		if (arm != 1)
	// 		{
	// 			if (CAlignerSwitcher::Swith128())
	// 			{
	// 				arm = 1;
	// 				if (!AlignWafer(arm))
	// 				{
	// 					return FALSE;
	// 				}
	// 			}
	// 		}
	// 	}
	// 	else if (type == 2)
	// 	{
	// 		if (arm != 2)
	// 		{
	// 			if (CAlignerSwitcher::Switch900())
	// 			{
	// 				arm = 2;
	// 				if (!AlignWafer(arm))
	// 				{
	// 					return FALSE;
	// 				}
	// 			}
	// 		}
	// 	}
	// 	else
	// 	{
	// 		// OK to proceed
	// 	}

	Log(GetResp(), 3);
	if (HomeAlign(AlgnNum)) {
		bHomed = TRUE;
		return TRUE;
	}
	ErrorStr = "Fail to home Aligner";
	if (pGDev) {
		pGDev->SetAlarm(CAlrmList::ALIGNERHOMEFAILURE, ErrorStr);
		pGDev->pLTMgr->SetGenAlarm(TRUE);
	}
	return FALSE;
}

BOOL CLogosolBrkUnit::Align() {
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		bHomed = TRUE;
		ErrorStr = "Logosol aligned";
		Log(ErrorStr, 2);
		return TRUE;
	}
	if (SendLogosolTest("BAL")) {
		return TRUE;
	}
	return FALSE;
}


BOOL CLogosolBrkUnit::Xfer(char* text) {
	if (IsSimu()) {
		ErrorStr = text;
		return TRUE;
	}
	CString str = text;
	if (str.Find("HOME") == -1) {
		if (!CanProceed(1)) {
			return FALSE;
		}
	}
	char cmdStr[256];

	if (str.Find("SPECIAL") == -1) {
		strcpy(cmdStr, text);
		if (SendBrk(cmdStr)) {
			Log(GetResp(), 3);
			ErrorStr = text;
			Log(ErrorStr, 2);
			return TRUE;
		}
	}
	ErrorStr = GetResp();
	Log(ErrorStr, 3);
	return FALSE;
}

BOOL CLogosolBrkUnit::ReqStatus() {
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "";
		return TRUE;
	}
	char cmdStr[256];
	strcpy(cmdStr, "HLLO");
	if (!SendBrk(cmdStr)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}
	ErrorStr = GetResp();
	Log(ErrorStr, 2);
	return TRUE;
}

// Tiny eye setup
// MAP TINY_EYE NUMERIC_IN LOW TO DIGITAL_IN 7
// Bit 0 - prealigner
// Bit 1 - scale
// Bit 3 - scanner
// To read
// RQ IO STATE TINY_EYE
// Reply
// Name                       State
// TINY_EYE                   0(0x00000000)    when no wafer present

// To setup the robot controller
// MAP DOOR_LOCK DISCRETE_OUT LOW TO DIGITAL_OUT 1
// To remove the name
// REMOVE IO DOOR_LOCK
// To see all maps
// RQ IO MAP ALL
BOOL CLogosolBrkUnit::LockDoor(BOOL bLock) {
	// LYF Obsolete [10/29/2005]
	return TRUE;

	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "Lock Door Operation OK";
		return TRUE;
	}
	char cmdStr[256];
	if (bLock) {
		strcpy(cmdStr, "SET IO STATE DOOR_LOCK ACTIVE");
	}
	else {
		strcpy(cmdStr, "SET IO STATE DOOR_LOCK INACTIVE");
	}
	if (!SendBrk(cmdStr)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}
	Log(GetResp(), 3);
	ErrorStr = "Lock Door Operation OK";
	Log(ErrorStr, 3);
	return TRUE;
}

BOOL CLogosolBrkUnit::WaferPresentCheck() {
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "Wafer Present Checked";
		Log(ErrorStr, 3);
		WaferAtLoc = 0xFFFFFFFF;
		return TRUE;
	}
	char cmdStr[256];
	strcpy(cmdStr, "RQ IO STATE dio_in");
	if (!SendBrk(cmdStr)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0xFFFFFFFF;
		return FALSE;
	}
	Log(GetResp(), 3);
	CString val;
	val = GetResp();
	val = val.Mid(val.Find("dio_in", 0) + 6);
	val = val.Left(val.Find("(", 0));
	WaferAtLoc = atol(val);
	ErrorStr.Format("Wafer at location %s", val);
	Log(ErrorStr, 3);
	return TRUE;
}

BOOL CLogosolBrkUnit::GetTinyEye() {
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "Tiny Eye READ";
		Log(ErrorStr, 3);
		WaferAtLoc = 0xFFFFFFFF;
		return TRUE;
	}
	char cmdStr[256];
	strcpy(cmdStr, "SET CPTR 32 ON");
	SendBrk(cmdStr);
	ErrorStr = GetResp();
	strcpy(cmdStr, "RQ IO STATE dio_in");
	if (!SendBrk(cmdStr)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		return FALSE;
	}
	Log(GetResp(), 3);
	CString val;
	val = GetResp();
	val = val.Mid(val.Find("0x", 0) + 2, 1);
	WaferAtLoc = val == "c";
	ErrorStr = "Tiny Eye Check OK";
	Log(ErrorStr, 3);
	return TRUE;
}

BOOL CLogosolBrkUnit::IsWaferOnArm() {
	if (!IsConnected()) {
		ErrorStr = "LogosolBrkUnit not connected";
		ASSERT(0);
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "LOAD OFF";
		return FALSE;
	}
	if (GetTinyEye()) {
		return WaferAtLoc;
	}
	ErrorStr = "Check wafer on arm command error";
	Log(ErrorStr, 3);
	return TRUE;
}

void CLogosolBrkUnit::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CLogosolBrkUnit");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CLogosolBrkUnit::IsWaferOnAlgn() {
	if (!IsConnected()) {
		ErrorStr = "LogosolBrkUnit not connected";
		ASSERT(0);
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "ALIGNER RQWPRS";
		return FALSE;
	}
	BOOL ret = FALSE;
	char cmdStr[256];
	strcpy(cmdStr, "DOC");
	if (SendLogosolTest(cmdStr)==0) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		return FALSE;
	}
	WaferAtLoc = 1;
	return TRUE;
}

int CLogosolBrkUnit::GetDIOIN() {
	if (!SendBrk("RQ IO STATE DIO_IN")) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		return 0xFFFF;
	}
	CString val;
	val = GetResp();
	int v = val.Find(")", 0);
	val = val.Mid(v - 1);
	val.MakeUpper();
	char ch = val.GetAt(0);
	if (ch > '9') {
		v = (ch - 'A') + 10;
	}
	else {
		v = ch - '0';
	}
	v = 0xFF & (~v);
	return v;
}

BOOL CLogosolBrkUnit::SetPostPos(short AlgnNum, long PostPos, long OffsetPos) {
	if (!CanProceed(0)) {
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "PostPos Set";
		Log(ErrorStr, 2);
		return TRUE;
	}

	CString str;
	switch (WaferSize) {
	case 300:
		CString CCDPOS, POSTPOS;
		CCDPOS.Format("_SA %d", DosUtil.GetLogosolCCDNum(AlgnNum));
		POSTPOS.Format("FWOH %d", PostPos);
		if (!SendLogosol("WSZ 12")) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		if (!SendLogosol("_WT 0")) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		if (!SendLogosol(POSTPOS.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		break;
	}
	Log(GetResp(), 3);
	if (!SendBrk("SET STN 1 XFER RE")) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}
	return TRUE;
}

BOOL CLogosolBrkUnit::chkResponse() {
	//SendLogosolTest("BAL");
	SendLogosolTest("DOC");//here
	return TRUE;
}
//Tushar 21082024