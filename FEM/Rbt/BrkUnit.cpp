#include "stdafx.h"

#include "GlobalDeviceContainer.h"
#include "LTW/LTWMgr.h"
#include "SRC/DOSUtil.h"

#include "BrkUnit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CBrkUnit::CBrkUnit() {
	ID = "BRK";
	WaferAtLoc = 0xFFFFFFFF;

	arm = 0;

	DeviceType = STANDARD;

	// Pre-load ini file
	DosUtil.GetAlignerPostPos(1);
	DosUtil.GetAlignerOffsetPos(1);
	DosUtil.GetAlignerPostPos(2);
	DosUtil.GetAlignerOffsetPos(2);
}

CBrkUnit::~CBrkUnit() {}

BOOL CBrkUnit::Start(short port) {
	BOOL ret = TRUE;
	switch (StartCom(port)) {
		//	case CRbtBase::CREATED:
		//		ErrorStr.Format("BrkUnit already connected to port %d", port);
		//		ASSERT(0);
		//		break;
	case CRbtBase::ALLOCERROR:
		ErrorStr = "BrkUnit alloc error";
		ret = FALSE;
		//		ASSERT(0);
		break;
	case CRbtBase::OPENERROR:
		ErrorStr.Format("BrkUnit could not open port %d", port);
		ret = FALSE;
		//		ASSERT(0);
		break;
		//	case CRbtBase::CONNECTED:
		//		ErrorStr = "BrkUnit already connected";
		//		ASSERT(0);
		//		break;
	case CRbtBase::SUCCESS:
		ErrorStr.Format("BrkUnit connected to port %d", port);
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

BOOL CBrkUnit::Stop() {
	// Must unlock before pCom could be destroyed
	Log("Unlock door", 10);
	LockDoor(FALSE);

	return CRbtUnit::Stop();
}

BOOL CBrkUnit::HomeAlign(short AlgnNum) {
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
	if (!SendBrk("INIT ALIGNER")) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}
	/*
	if (!SendBrk("SET STN ALGN 1 XFER RE"))
	{
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}
	*/

	long PostPos;
	long OffPos;

	PostPos = DosUtil.GetAlignerPostPos(AlgnNum); // LYF Convert to robot unit [1/14/2005]
	OffPos = DosUtil.GetAlignerOffsetPos(AlgnNum); // LYF Convert to robot unit [1/14/2005]

	CString str;
	switch (WaferSize) {
	case 150:
		str.Format("INIT ALIGNER WAFER SIZE 5 FDCL FLAT CCD 1 POSTPOS 1 POS %ld", OffPos + PostPos);
		// YFL:  [1/8/2005]
		//		if (!SendBrk("INIT ALIGNER WAFER SIZE 5 FDCL FLAT CCD 1"))
		if (!SendBrk(str.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		break;
	case 200:
		str.Format("INIT ALIGNER WAFER SIZE 6 FDCL NTCH CCDPOS %d POS %ld POSTPOS 1 POS %ld", DosUtil.GetCCDNum(AlgnNum), DosUtil.GetCCDPos(AlgnNum), OffPos + PostPos);
		// YFL:  [1/8/2005]
		//		if (!SendBrk("INIT ALIGNER WAFER SIZE 6 FDCL NTCH CCD 1"))
		if (!SendBrk(str.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		break;
	case 300:
		str.Format("INIT ALIGNER WAFER SIZE 7 FDCL NTCH CCDPOS %d POS %ld POSTPOS 1 POS %ld", DosUtil.GetCCDNum(AlgnNum), DosUtil.GetCCDPos(AlgnNum), OffPos + PostPos);
		// YFL:  [1/8/2005]
		//		if (!SendBrk("INIT ALIGNER"))
		if (!SendBrk(str.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		break;
	}
	Log(GetResp(), 3);
	/*
	if (!SendBrk("SET STN ALGN 1 XFER RE"))
	{
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}
	*/
	if (!IsWaferOnAlgn()) {
		if (!SendBrk("ALIGNER HOME ALL")) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
	}
	//	else
	//	{
	//		Log("Wafer present on Pre-aligner.",3);
	//	}
	////	Log(GetResp(), 3);
	//	if (!SendBrk("ALIGNER HOME ALL"))
	//	{
	////		ErrorStr = GetResp();
	////		Log(ErrorStr, 3);
	//		return FALSE;
	//	}
	//	Log(GetResp(), 32);
	//	ErrorStr = "Aligner homed";
	//	Log(ErrorStr,2);
	return TRUE;
}

BOOL CBrkUnit::HomeRobot(short AlgnNum) {
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

BOOL CBrkUnit::Xfer(char* text) {
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

	// 	if (str.Find("ARM A") != -1)
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
	// 	else if (str.Find("ARM B") != -1)
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
	else {
		char dmy[16];
		char ch;
		short FrStn, FrSlt, ToStn, ToSlt;
		sscanf(text, "%s %s %c %s %hd %hd %hd %hd", dmy, dmy, &ch, dmy, &FrStn, &FrSlt, &ToStn, &ToSlt);
		sprintf(cmdStr, "XFER ARM B %d SLOT %d 1", FrStn, FrSlt);
		if (SendBrk(cmdStr)) {
			strcpy(cmdStr, "PICK 1 ARM B ALGN");
			if (SendBrk(cmdStr)) {
				sprintf(cmdStr, "PLACE %d SLOT %d ARM B", ToStn, ToSlt);
				if (SendBrk(cmdStr)) {
					Log(GetResp());
					ErrorStr = text;
					Log(ErrorStr);
					return TRUE;
				}
			}
		}
	}
	ErrorStr = GetResp();
	Log(ErrorStr, 3);
	return FALSE;
}

BOOL CBrkUnit::ReqStatus() {
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
BOOL CBrkUnit::LockDoor(BOOL bLock) {
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

BOOL CBrkUnit::WaferPresentCheck() {
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
	strcpy(cmdStr, "RQ IO STATE TINY_EYE");
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

BOOL CBrkUnit::GetTinyEye() {
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
	strcpy(cmdStr, "RQ IO STATE TINY_EYE");
	if (!SendBrk(cmdStr)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		return FALSE;
	}
	Log(GetResp(), 3);
	CString val;
	val = GetResp();
	val = val.Mid(val.Find("(0x", 0) + 3, 8);
	//	val = val.Left(val.Find("(", 0));
	WaferAtLoc = atol(val);
	ErrorStr = "Tiny Eye Check OK";
	Log(ErrorStr, 3);
	return TRUE;
}

BOOL CBrkUnit::IsWaferOnArm() {
	if (!IsConnected()) {
		ErrorStr = "BrkUnit not connected";
		ASSERT(0);
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "LOAD OFF";
		return FALSE;
	}
	char cmdStr[256];
	strcpy(cmdStr, "RQ LOAD");
	if (!SendBrk(cmdStr, 3000)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		return FALSE;
	}
	Log(GetResp(), 3);
	CString val;
	val = GetResp();
	if (-1 != val.Find("ON")) {
		return TRUE;
	}
	return FALSE;
}

BOOL CBrkUnit::IsWaferOnArm(short arm) {
	if (!IsConnected()) {
		ErrorStr = "BrkUnit not connected";
		//		ASSERT(0);
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "LOAD OFF";
		return FALSE;
	}
	BOOL ret = TRUE;
	char cmdStr[256];
	switch (arm) {
	case 1:
		strcpy(cmdStr, "VAC ON A");
		break;
	case 2:
		strcpy(cmdStr, "VAC ON B");
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}
	if (!SendBrk(cmdStr, 3000)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		return FALSE;
	}

	Sleep(1000); // wait till vacuum takes effect
	switch (arm) {
	case 1:
		strcpy(cmdStr, "RQ LOAD A");
		break;
	case 2:
		strcpy(cmdStr, "RQ LOAD B");
		break;
	default:
		ASSERT(0);
		return TRUE;
		break;
	}
	if (!SendBrk(cmdStr, 3000)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		return FALSE;
	}
	Log(GetResp(), 3);
	CString val;
	ErrorStr = GetResp();
	Log(ErrorStr, 3);
	if (-1 != ErrorStr.Find("ON")) {
		ret = TRUE;
	}
	else {
		ret = FALSE;
	}

	if (ret == FALSE) {
		// no wafer so release vacuum
		switch (arm) {
		case 1:
			strcpy(cmdStr, "VAC OFF A");
			break;
		case 2:
			strcpy(cmdStr, "VAC OFF B");
			break;
		default:
			ASSERT(0);
			return TRUE;
			break;
		}
		if (!SendBrk(cmdStr, 3000)) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			WaferAtLoc = 0;
			return FALSE;
		}
	}

	return ret;
}

BOOL CBrkUnit::IsWaferOnAlgn() {
	if (!IsConnected()) {
		ErrorStr = "BrkUnit not connected";
		ASSERT(0);
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "ALIGNER RQWPRS";
		return FALSE;
	}
	char cmdStr[256];
	strcpy(cmdStr, "ALIGNER RQWPRS");
	if (!SendBrk(cmdStr)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		return FALSE;
	}
	Log(GetResp(), 3);
	CString val;
	val = GetResp();
	if (-1 != val.Find("YES")) {
		return TRUE;
	}
	return FALSE;
}

// BOOL CBrkUnit::AlignWafer(short arm)
// {
// 	if (!DosUtil.GetEnaAlignerCtrl())
// 	{
// 		// LYF Bypass if not required [4/20/2006]
// 		return TRUE;
// 	}
// 	long postpos = DosUtil.GetAlignerPostPos(arm); // LYF Convert to robot unit [1/14/2005]
// 	long offpos = DosUtil.GetAlignerOffsetPos(arm); // LYF Convert to robot unit [1/14/2005]
// 	CString str;
// 	str.Format("INIT ALIGNER WAFER SIZE 7 FDCL FLAT CCDPOS %d POS %ld POSTPOS 1 POS %ld", DosUtil.GetCCDNum(arm), DosUtil.GetCCDPos(arm), offpos+postpos);
//	switch (arm) {
//	case 1:
////		str.Format("INIT ALIGNER WAFER SIZE 7 FDCL FLAT CCDPOS 1 POS 340000 POSTPOS 1 POS %ld", offpos+postpos);
////		str.Format("INIT ALIGNER CCDPOS 0 POS 340000");
//		break;
//	case 2:
////		str.Format("INIT ALIGNER WAFER SIZE 7 FDCL FLAT CCDPOS 1 POS 62000 POSTPOS 1 POS %ld", offpos+postpos);
////		str.Format("INIT ALIGNER CCDPOS 0 POS 62000");
//		break;
//	default:
//		ASSERT(0);
//		break;
//	}
//	str.Format("INIT ALIGNER WAFER SIZE 7 FDCL FLAT CCD 1 POSTPOS 1 POS %ld", offpos+postpos);
//	// YFL:  [1/8/2005]
//	//		if (!SendBrk("INIT ALIGNER WAFER SIZE 5 FDCL FLAT CCD 1"))
// 	if (!SendBrk(str.GetBuffer(0)))
// 	{
//			ErrorStr = GetResp();
//			Log(ErrorStr, 3);
// 		return FALSE;
// 	}
// 	SendBrk("SET STN ALGN 1 XFER RE");
// 	return TRUE;
// }

void CBrkUnit::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CBrkUnit");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CBrkUnit::IsWaferOnAlgn(short type) {
	if (!IsConnected()) {
		ErrorStr = "BrkUnit not connected";
		ASSERT(0);
		return FALSE;
	}
	if (IsSimu()) {
		ErrorStr = "ALIGNER RQWPRS";
		return FALSE;
	}
	// 	short orgarm = arm;
	// 	if (type == 1)
	// 	{
	// 		if (arm != 1)
	// 		{
	// 			if (CAlignerSwitcher::Swith128())
	// 			{
	// 				arm = 1;
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
	// 			}
	// 		}
	// 	}
	// 	else {
	// 		// OK to proceed
	// 	}
	BOOL ret = FALSE;
	char cmdStr[256];
	strcpy(cmdStr, "ALIGNER RQWPRS");
	if (!SendBrk(cmdStr)) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		WaferAtLoc = 0;
		goto HERE;
	}
	{
		Log(GetResp(), 3);
		CString val;
		val = GetResp();
		if (-1 != val.Find("YES")) {
			ret = TRUE;
		}
	}

HERE:
	// 	if (orgarm != arm)
	// 	{
	// 		// LYF By switching back to original condition will avoid unnecessary AlignWafer() which is time consuming [4/20/2006]
	// 		switch (orgarm)
	// 		{
	// 		case 1:
	// 			if (CAlignerSwitcher::Swith128())
	// 			{
	// 				arm = 1;
	// 			}
	// 			break;
	// 		case 2:
	// 			if (CAlignerSwitcher::Switch900())
	// 			{
	// 				arm = 2;
	// 			}
	// 			break;
	// 		default:
	// 			ASSERT(0);
	// 			break;
	// 		}
	// 	}
	return ret;
}

int CBrkUnit::GetDIOIN() {
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
	//	val.SetAt(val.Find(")"),0);
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

BOOL CBrkUnit::SetPostPos(short AlgnNum, long PostPos, long OffsetPos) {
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
		ErrorStr = "PostPos Set";
		Log(ErrorStr, 2);
		return TRUE;
	}

	CString str;
	switch (WaferSize) {
	case 150:
		str.Format("INIT ALIGNER WAFER SIZE 5 FDCL FLAT CCD 1 POSTPOS 1 POS %ld", OffsetPos + PostPos);
		if (!SendBrk(str.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		break;
	case 200:
		str.Format("INIT ALIGNER WAFER SIZE 6 FDCL NTCH CCDPOS %d POS %ld POSTPOS 1 POS %ld", DosUtil.GetCCDNum(AlgnNum), DosUtil.GetCCDPos(AlgnNum), OffsetPos + PostPos);
		if (!SendBrk(str.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		break;
	case 300:
		str.Format("INIT ALIGNER WAFER SIZE 7 FDCL NTCH CCDPOS %d POS %ld POSTPOS 1 POS %ld", DosUtil.GetCCDNum(AlgnNum), DosUtil.GetCCDPos(AlgnNum), OffsetPos + PostPos);
		if (!SendBrk(str.GetBuffer(0))) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
		break;
	}
	Log(GetResp(), 3);
	if (!SendBrk("SET STN ALGN 1 XFER RE")) {
		ErrorStr = GetResp();
		Log(ErrorStr, 3);
		return FALSE;
	}
	if (!IsWaferOnAlgn()) {
		if (!SendBrk("ALIGNER HOME ALL")) {
			ErrorStr = GetResp();
			Log(ErrorStr, 3);
			return FALSE;
		}
	}
	return TRUE;
}