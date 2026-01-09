#include "stdafx.h"

#include "GlobalDeviceContainer.h"
#include "OLnSvrObj.h"
#include "ONLINE/GEM/CediListCtroller.h"
#include "OnlineMgr.h"
#include "SRC/DOSUtil.h"

#include "AlrmList.h"

CAlrmList::CAlrmList() {
	Load();
	LastAlarm = -1;
	LastAlarmSet = CAlrmItem::RESET;
	LoadAlarmPersistant();
}

CAlrmList::~CAlrmList() {
	SaveAlarmPersistant();
}

void CAlrmList::Load() {
	int n = 1;

	Alarms[LP1DUPLICATEDCARRIERID].Set(n++, "LP1 Duplicated carrier error");
	Alarms[LP2DUPLICATEDCARRIERID].Set(n++, "LP2 Duplicated carrier error");

	Alarms[CIDR1COMMFAILURE].Set(n++, "CIDR 1 communication failure");
	Alarms[CIDR1READFAILURE].Set(n++, "CIDR 1 read operation failure");
	Alarms[CIDR2COMMFAILURE].Set(n++, "CIDR 2 communication failure");
	Alarms[CIDR2READFAILURE].Set(n++, "CIDR 2 read operation failure");

	Alarms[LP1OPENFAILURE].Set(n++, "LP1 open failure");
	Alarms[LP1CLOSEFAILURE].Set(n++, "LP1 close failure");
	Alarms[LP2OPENFAILURE].Set(n++, "LP2 open failure");
	Alarms[LP2CLOSEFAILURE].Set(n++, "LP2 close failure");

	Alarms[OUTOFSERVICELPUSED].Set(n++, "Attempt to use out of service load port");
	Alarms[CARRIERPRESENCEERROR].Set(n++, "Carrier presence error");
	Alarms[CARRIERPLACEMENTERROR].Set(n++, "Carrier placement error");
	Alarms[DOCKUNDOCKFAILURE].Set(n++, "Dock/Undock failure");
	Alarms[OPENCLOSEFAILURE].Set(n++, "Open/Close failure");
	Alarms[CARRIERREMOVALERROR].Set(n++, "Carrier removal error");

	Alarms[LPNOTASSOCIATEDERROR].Set(n++, "Load port not associated");
	Alarms[CARRIEROBJECTNOTFOUND].Set(n++, "Carrier object not found");

	Alarms[IMPROPERSCENARIOERROR].Set(n++, "Improper scenario");

	Alarms[CLAMP1FAILURE].Set(n++, "LP1 clamp failure");
	Alarms[CLAMP2FAILURE].Set(n++, "LP2 clamp failure");
	Alarms[UNCLAMP1FAILURE].Set(n++, "LP1 unclamp failure");
	Alarms[UNCLAMP2FAILURE].Set(n++, "LP2 unclamp failure");

	Alarms[WAFERHANDLINGFAILURE].Set(n++, "Wafer handling failure");

	Alarms[LP1EXCESSIVEWAFEROUT].Set(n++, "LP1 excessive wafer out failure");
	Alarms[LP2EXCESSIVEWAFEROUT].Set(n++, "LP2 excessive wafer out failure");

	Alarms[LP1PERISENSORTRIPED].Set(n++, "LP1 perimeter sensor trip failure");
	Alarms[LP2PERISENSORTRIPED].Set(n++, "LP2 perimeter sensor trip failure");

	Alarms[LP1WAFERCROSSSLOTTED].Set(n++, "LP1 wafer cross slotted");
	Alarms[LP2WAFERCROSSSLOTTED].Set(n++, "LP2 wafer cross slotted");

	Alarms[CARRIER1IDREADFAIL].Set(n++, "LP1 Carrier ID read failure");
	Alarms[CARRIER2IDREADFAIL].Set(n++, "LP2 Carrier ID read failure");

	Alarms[SLOTMAP1READFAILURE].Set(n++, "LP1 Slot map read failure");
	Alarms[SLOTMAP2READFAILURE].Set(n++, "LP2 Slot map read failure");

	Alarms[SLOTMAP1VERIFICATIONFAIL].Set(n++, "LP1 Slot map verification failure");
	Alarms[SLOTMAP2VERIFICATIONFAIL].Set(n++, "LP2 Slot map verification failure");

	Alarms[ID1VERIFICATIONFAIL].Set(n++, "LP1 ID verification failure");
	Alarms[ID2VERIFICATIONFAIL].Set(n++, "LP2 ID verification failure");

	Alarms[FANFILTERUNITFAIL].Set(n++, "Fan filter unit failure");

	Alarms[LOSTE84SIGNAL].Set(n++, "Lost E84 signal");
	Alarms[UNEXPECTEDE84SIGNAL].Set(n++, "Unexpected E84 signal");

	Alarms[LP1ALARMED].Set(n++, "LP1 in alarmed state");
	Alarms[LP2ALARMED].Set(n++, "LP2 in alarmed state");

	Alarms[LP1COMMFAILURE].Set(n++, "LP1 communication failure");
	Alarms[LP2COMMFAILURE].Set(n++, "LP2 communication failure");

	Alarms[LP1VACUUMLOST].Set(n++, "LP1 vacuum lost");
	Alarms[LP2VACUUMLOST].Set(n++, "LP2 vacuum lost");

	Alarms[LP1NOVACUUM].Set(n++, "LP1 no vacuum");
	Alarms[LP2NOVACUUM].Set(n++, "LP2 no vacuum");

	Alarms[LP1E84TP1TIMEOUT].Set(n++, "LP1 E84 TP1 timeout");
	Alarms[LP1E84TP2TIMEOUT].Set(n++, "LP1 E84 TP2 timeout");
	Alarms[LP1E84TP3TIMEOUT].Set(n++, "LP1 E84 TP3 timeout");
	Alarms[LP1E84TP4TIMEOUT].Set(n++, "LP1 E84 TP4 timeout");
	Alarms[LP1E84TP5TIMEOUT].Set(n++, "LP1 E84 TP5 timeout");
	Alarms[LP1E84TP6TIMEOUT].Set(n++, "LP1 E84 TP6 timeout");

	Alarms[LP2E84TP1TIMEOUT].Set(n++, "LP2 E84 TP1 timeout");
	Alarms[LP2E84TP2TIMEOUT].Set(n++, "LP2 E84 TP2 timeout");
	Alarms[LP2E84TP3TIMEOUT].Set(n++, "LP2 E84 TP3 timeout");
	Alarms[LP2E84TP4TIMEOUT].Set(n++, "LP2 E84 TP4 timeout");
	Alarms[LP2E84TP5TIMEOUT].Set(n++, "LP2 E84 TP5 timeout");
	Alarms[LP2E84TP6TIMEOUT].Set(n++, "LP2 E84 TP6 timeout");

	Alarms[EQPOUTOFSERVICE].Set(n++, "Equipment out of service");
	Alarms[ROBOTOUTOFSERVICE].Set(n++, "Robot out of service");
	Alarms[LP1OUTOFSERVICE].Set(n++, "LP1 out of service");
	Alarms[LP2OUTOFSERVICE].Set(n++, "LP2 out of service");
	Alarms[IDR1OUTOFSERVICE].Set(n++, "CIDR 1 out of service");
	Alarms[IDR2OUTOFSERVICE].Set(n++, "CIDR 2 out of service");
	Alarms[C413OUTOFSERVICE].Set(n++, "413 module is out of service");
	Alarms[C413NOTONLINE].Set(n++, "413 module not online");

	Alarms[LP1NOTREADYTOLOAD].Set(n++, "LP1 not ready to load");
	Alarms[LP2NOTREADYTOLOAD].Set(n++, "LP2 not ready to load");

	Alarms[LP1ACCESSMODEVIOLATION].Set(n++, "LP1 access mode violation");
	Alarms[LP2ACCESSMODEVIOLATION].Set(n++, "LP2 access mode violation");

	Alarms[WAFERHANDLERERROR].Set(n++, "General wafer handling failure");
	Alarms[ROBOTHOMEFAILURE].Set(n++, "Robot home failure");
	Alarms[ROBOTCOMMFAILURE].Set(n++, "Lost communication with robot failure");
	Alarms[ROBOTSTARTFAILURE].Set(n++, "Robot startup failure");
	Alarms[WAFERONARM].Set(n++, "Robot has wafer on arm");

	Alarms[LP1RESETFAILURE].Set(n++, "LP1 reset failure");
	Alarms[LP2RESETFAILURE].Set(n++, "LP2 reset failure");

	Alarms[ALIGNERHOMEFAILURE].Set(n++, "Aligner home failure");
	Alarms[WAFERONALIGNER].Set(n++, "Aligner has wafer");
	Alarms[WAFERINSIDETOOL].Set(n++, "Wafer present inside tool");

	Alarms[PRJOBEXEERROR].Set(n++, "PRJob Exec Error");

	// New 2/15/12
	Alarms[TAKEBACKGROUNDFAIL].Set(n++, "Take Background Error");
	Alarms[TAKEREFERENCEFAIL].Set(n++, "Take Reference Error");

	Alarms[ABNORMALSPECTROMETERBEHAVIOR].Set(n++, "Abnormal Spectrometer Behavior");

	Alarms[MEASUREMENTABORTED].Set(n++, "Measurement Aborted");

	Alarms[LP1IDWAITFORHOSTTIMEOUT].Set(n++, "LP1 ID waiting for host timeout");
	Alarms[LP2IDWAITFORHOSTTIMEOUT].Set(n++, "LP2 ID waiting for host timeout");
	Alarms[LP1SMWAITFORHOSTTIMEOUT].Set(n++, "LP1 SM waiting for host timeout");
	Alarms[LP2SMWAITFORHOSTTIMEOUT].Set(n++, "LP2 SM waiting for host timeout");

	Alarms[LP1CJWAITTIMEOUT].Set(n++, "LP1 CJ waiting for host timeout");
	Alarms[LP2CJWAITTIMEOUT].Set(n++, "LP2 CJ waiting for host timeout");

	Alarms[LP1PJWAITTIMEOUT].Set(n++, "LP1 PJ waiting for host timeout");
	Alarms[LP2PJWAITTIMEOUT].Set(n++, "LP2 PJ waiting for host timeout");

	Alarms[LP1WAFERPROCESSINGTOUT].Set(n++, "LP1 wafer processing timeout");
	Alarms[LP2WAFERPROCESSINGTOUT].Set(n++, "LP2 wafer processing timeout");

	Alarms[LP1POINTPROCESSINGTOUT].Set(n++, "LP1 point processing timeout");
	Alarms[LP2POINTPROCESSINGTOUT].Set(n++, "LP2 point processing timeout");

	Alarms[LP1TOOMANYFAILPOINT].Set(n++, "LP1 too many failed points");
	Alarms[LP2TOOMANYFAILPOINT].Set(n++, "LP2 too many failed points");

	Alarms[ECHOMOTORERROR].Set(n++, "Echo motor error");

	Alarms[PMOTORERROR].Set(n++, "P motor error");
	Alarms[DMYPMOTORERROR].Set(n++, "P motor error");

	Alarms[LP1PATMATFAILCONTINUE].Set(n++, "LP1 pattern matching fail and continue to next point");
	Alarms[LP2PATMATFAILCONTINUE].Set(n++, "LP2 pattern matching fail and continue to next point");

	Alarms[LP1LOSTE84SIGNAL].Set(n++, "LP1 E84 signal lost");
	Alarms[LP2LOSTE84SIGNAL].Set(n++, "LP2 E84 signal lost");

	Alarms[LP1UNEXPECTEDE84SIGNAL].Set(n++, "LP1 Unexpected E84 signal");
	Alarms[LP2UNEXPECTEDE84SIGNAL].Set(n++, "LP1 Unexpected E84 signal");

	// New 2/15/12 end

	Alarms[WAFERPRESENTONSTAGE].Set(n++, "WAFER PRESENT ON STAGE");
	Alarms[WAFERPRESENTONALIGNER].Set(n++, "WAFER PRESENT ON ALIGNER");
	Alarms[WAFERPRESENTONROBOTARM].Set(n++, "WAFER PRESENT ON ROBOT ARM");

	Alarms[INCOMMINGVACUUMLOW].Set(n++, "INCOMMING VACUUM LOW");
	Alarms[STAGEMOTORERROR].Set(n++, "XY STAGE MOTOR ERROR");
	Alarms[RESETSTAGEFAILED].Set(n++, "XY STAGE RESET FAILED");
	Alarms[INCOMPLETEPROCESS].Set(n++, "PREVIOUS INCOMPLETE PROCESS DETECTED, RECOVERY NEEDED");
	Alarms[POSSIBLEWAFERINTOOL].Set(n++, "WAFER MAY BE IN TOOL, RECOVERY NEEDED");

	Alarms[LP1CENTERINGFAIL].Set(n++, "LP1 WAFER CENTERING FAILED");
	Alarms[LP2CENTERINGFAIL].Set(n++, "LP2 WAFER CENTERING FAILED");

	// New 8/3/20 end
	Alarms[DOORINTERLOCK].Set(n++, "SIDE DOOR OPENED");

	Alarms[UNKNOWNERROR].Set(n++, "Unknown Error");
}

BOOL CAlrmList::SetAlarm(int alid) {
	int idx = GetAlrmIndex(alid);
	if (idx < 0) {
		CString str;
		str.Format("Alarm %d not found", alid);
		Log(str);
		//ASSERT(0);
		return FALSE;
	}
	CEIDCtlr.m_alarm.Alarms[idx].alCD = CAlrmItem::SET; //  [9/16/2007 LYF]
	LastAlarm = CEIDCtlr.m_alarm.Alarms[idx].alID;
	LastAlarmSet = CEIDCtlr.m_alarm.Alarms[idx].alCD;

	CSMOL SM;
	SM.cmd = CSMOL::cmdALARM;
	SM.Ar = CEIDCtlr.m_alarm.Alarms[idx];
	//SM.Ar.alCD = CAlrmItem::SET;//  [9/16/2007 LYF]
	Log("Alarm " + CMSAlarmText(alid) + " SET", 0);
	return Send(SM);
}

// BOOL CAlrmList::SetAlarm(int alID)
// {
// 	CAlrmItem *pA = GetAlarm(alID);
// 	if (pA)
// 	{
// 		return SetAlarm(pA->alidx);
// 	}
// 	return FALSE;
// }

BOOL CAlrmList::ClearAlarm(int alid) {
	int idx = GetAlrmIndex(alid);
	if (idx < 0) {
		ASSERT(0);
		return FALSE;
	}
	CEIDCtlr.m_alarm.Alarms[idx].alCD = CAlrmItem::RESET; //  [9/16/2007 LYF]
	LastAlarm = CEIDCtlr.m_alarm.Alarms[idx].alID;
	LastAlarmSet = CEIDCtlr.m_alarm.Alarms[idx].alCD;

	CSMOL SM;
	SM.cmd = CSMOL::cmdALARM;
	SM.Ar = CEIDCtlr.m_alarm.Alarms[idx];
	//SM.Ar.alCD = CAlrmItem::RESET;  //  [9/16/2007 LYF]
	Log("Alarm " + CMSAlarmText(alid) + " CLEARED", 2);
	return Send(SM);
}

// BOOL CAlrmList::ClearAlarm(int alID)
// {
// 	SAlrm *pA = GetAlarm(alID);
// 	if (pA)
// 	{
// 		return ClearAlarm(pA->alidx);
// 	}
// 	return FALSE;
// }

BOOL CAlrmList::Send(CSMOL& SM) {
	if (pGDev->pOLnMgr && pGDev->pOLnMgr->pOLnSvrObj) {
		pGDev->pOLnMgr->pOLnSvrObj->ProcessCmd(SM);
		return TRUE;
	}
	return FALSE;
	// 	CSMClt ipc;
	// 	if ( ipc.Start1( "OLNSvr", sizeof( CSMOL ) ) )
	// 	{
	// 		if ( ipc.Send( & SM ) )
	// 		{
	// 			return TRUE;
	// 		}
	// 	}
	// 	Reply( SM, FALSE );
	// 	return FALSE;
}

BOOL CAlrmList::Reply(CSMOL& SM, BOOL ret) {
	if (SM.HWin) {
		if (ret) {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else {
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}

CString CAlrmList::CMSAlarmText(int alid) {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		if (CEIDCtlr.m_alarm.Alarms[i].alID == alid) {
			return CEIDCtlr.m_alarm.Alarms[i].alTX;
		}
	}
	Log("ALID not in list", 3);
	return "";
}

void CAlrmList::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CAlarm");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

int CAlrmList::GetAlrmIndex(int alid) {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		if (CEIDCtlr.m_alarm.Alarms[i].alID == alid) {
			return i;
		}
	}
	return -1;
}

BOOL CAlrmList::GetAlarmSetID(std::vector <UINT>& alIDList, std::vector <CString>& alTxtList) {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		if (CEIDCtlr.m_alarm.Alarms[i].alCD == CAlrmItem::SET) {
			alIDList.push_back(CEIDCtlr.m_alarm.Alarms[i].alID);
			alTxtList.push_back(CEIDCtlr.m_alarm.Alarms[i].alTX);
		}
	}
	return TRUE;
}

BOOL CAlrmList::GetAlarmEnabledID(std::vector <UINT>& alIDList, std::vector <CString>& alTxtList) {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		if (CEIDCtlr.m_alarm.Alarms[i].alED == TRUE) {
			alIDList.push_back(CEIDCtlr.m_alarm.Alarms[i].alID);
			alTxtList.push_back(CEIDCtlr.m_alarm.Alarms[i].alTX);
		}
	}
	return TRUE;
}

BOOL CAlrmList::GetLastAlarmEvt(UINT& CEID) {
	CEID = -1;
	if (LastAlarm == -1) {
		return FALSE;
	}

	CAlrmItem* pA = GetAlarm(LastAlarm);
	if (pA) {
		CEID = pA->SetAlrmCEID;
		return TRUE;
	}
	return FALSE;
}

CAlrmItem* CAlrmList::GetAlarm(int alID) {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		if (CEIDCtlr.m_alarm.Alarms[i].alID == alID) {
			return &CEIDCtlr.m_alarm.Alarms[i];
		}
	}
	return NULL;
}

BOOL CAlrmList::IsAlarm(int alID) {
	if (GetAlarm(alID) == NULL) {
		return FALSE;
	}
	return TRUE;
}

CString CAlrmList::GetAlarmText(int alID) {
	CAlrmItem* pA = GetAlarm(alID);
	if (pA) {
		return pA->alTX;
	}
	return "";
}

void CAlrmList::SetLastAlarmEvt(int alID, short alCD) {
	LastAlarm = alID;
	if (alCD) {
		LastAlarmSet = CAlrmItem::SET;
	}
	else {
		LastAlarmSet = CAlrmItem::RESET;
	}
}

BOOL CAlrmList::IsAlarmEnabled(int alID) {
	CAlrmItem* pA = GetAlarm(alID);
	if (pA) {
		return pA->alED;
	}
	return FALSE;
}

void CAlrmList::EnableAllAlarms() {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		CEIDCtlr.m_alarm.Alarms[i].alED = TRUE;
	}
}

void CAlrmList::DisableAllAlarms() {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		CEIDCtlr.m_alarm.Alarms[i].alED = FALSE;
	}
}

BOOL CAlrmList::EnableAlarm(int alID) {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		if (CEIDCtlr.m_alarm.Alarms[i].alID == alID) {
			CEIDCtlr.m_alarm.Alarms[i].alED = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CAlrmList::DisableAlarm(int alID) {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		if (CEIDCtlr.m_alarm.Alarms[i].alID == alID) {
			CEIDCtlr.m_alarm.Alarms[i].alED = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CAlrmList::GetAllAlarmID(std::vector <UINT>& alIDList, std::vector <CString>& alTxtList) {
	for (int i = 0; i < CAlrmList::LASTALARM; i++) {
		alIDList.push_back(CEIDCtlr.m_alarm.Alarms[i].alID);
		alTxtList.push_back(CEIDCtlr.m_alarm.Alarms[i].alTX);
	}
	return TRUE;
}

int CAlrmList::GetLastAlarm() {
	return LastAlarm;
}

int CAlrmList::GetCount() {
	return CAlrmList::LASTALARM;
}

void CAlrmList::Serialize(CArchive& ar) {
	int count;
	int magic;
	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		count = GetCount();
		ar << count;
		for (int i = 0; i < count; i++) {
			SerializeAlrm(ar, CEIDCtlr.m_alarm.Alarms[i]);
		}
	}
	else {
		ar >> magic;
		ar >> count;
		for (int i = 0; i < count; i++) {
			SerializeAlrm(ar, CEIDCtlr.m_alarm.Alarms[i]);
		}
	}
}

void CAlrmList::SerializeAlrm(CArchive& ar, CAlrmItem& Alrm) {
	int magic;
	if (ar.IsStoring()) {
		magic = 1;
		ar << magic;
		ar << Alrm.alCD;
		ar << Alrm.alID;
		ar << Alrm.alED;
		ar << Alrm.EnableSetAlrmCEID;
		ar << Alrm.EnableClearAlrmCEID;
	}
	else {
		int tmp;
		ar >> magic;
		ar >> tmp;
		Alrm.alCD = (CAlrmItem::ALCD)tmp;
		ar >> Alrm.alID;
		ar >> Alrm.alED;
		ar >> Alrm.EnableSetAlrmCEID;
		ar >> Alrm.EnableClearAlrmCEID;
	}
}

BOOL CAlrmList::LoadAlarmPersistant() {
	CFile theFile;
	CString filename;

	DosUtil.GetProgramDir(filename);
	char ch = filename.GetAt(filename.GetLength() - 1);
	if (ch != '\\') {
		filename += "\\";
	}
	filename += "DEFAULT.ALR";

	if (theFile.Open(filename, CFile::modeRead)) {
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL CAlrmList::SaveAlarmPersistant() {
	CFile theFile;
	CString filename;

	DosUtil.GetProgramDir(filename);
	char ch = filename.GetAt(filename.GetLength() - 1);
	if (ch != '\\') {
		filename += "\\";
	}
	filename += "DEFAULT.ALR";

	if (theFile.Open(filename, CFile::modeCreate | CFile::modeWrite)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close();
		theFile.Close();
		return TRUE;
	}
	return FALSE;
}

int CAlrmList::GetAlarmID(CAlrmList::ALIDX alidx) {
	if ((alidx >= 0) && (alidx < CAlrmList::LASTALARM)) {
		return CEIDCtlr.m_alarm.Alarms[alidx].alID;
	}
	return -1;
}

void CAlrmList::LoadList(CListCtrl& m_List) {
	char buf[126] = { 0 };
	int i = 0;
	m_List.DeleteAllItems();
	while (i != CAlrmList::LASTALARM) {
		int pos = m_List.InsertItem(i, _itoa(CEIDCtlr.m_alarm.Alarms[i].alID, buf, 10));
		m_List.SetItemText(pos, 1, CEIDCtlr.m_alarm.Alarms[i].alTX);
		m_List.SetItemText(pos, 2, CEIDCtlr.m_alarm.Alarms[i].alED ? "Enabled" : "Disabled");
		i++;
	}
}