// StateModels.cpp: implementation of the CStateModels class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StateModels.h"
#include "..\..\FEM\Ldp\LPMgr.h"
#include "..\..\FEM\Ldp\LPSvr.h"

#include "..\..\SRC\DOSUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// CStateModels StMdl;

CStateModels::CStateModels() {
	EqpState.LineMode = EqpStatus::OFFLINEHOST;

	strcpy(Algn.ObjID, "Aligner");
	strcpy(Algn.ObjType, "SubstLoc");
	Algn.LocNo = 3;
	Algn.SubstLocState = CSubstLocObj::UNOCCUPIED;

	strcpy(Scale.ObjID, "Scale");
	strcpy(Scale.ObjType, "SubstLoc");
	Scale.LocNo = 4;
	Scale.SubstLocState = CSubstLocObj::UNOCCUPIED;

	strcpy(Stage1.ObjID, "Stage413et");
	strcpy(Stage1.ObjType, "SubstLoc");
	Stage1.LocNo = 5;
	Stage1.SubstLocState = CSubstLocObj::UNOCCUPIED;

	strcpy(Stage2.ObjID, "Stage2");
	strcpy(Stage2.ObjType, "SubstLoc");
	Stage2.LocNo = 6;
	Stage2.SubstLocState = CSubstLocObj::UNOCCUPIED;

	strcpy(RbtArm.ObjID, "RobotArm");
	strcpy(RbtArm.ObjType, "SubstLoc");
	RbtArm.LocNo = 7;
	RbtArm.SubstLocState = CSubstLocObj::UNOCCUPIED;

	CString str;
	CString sect;
	CString winini;
	CString AcsMode;
	DosUtil.GetLocalSysFile(winini);
	RbtObj.Service = CRbtObj::INSERVICE;

	// 	sect = "900Chamber";
	// 	C900.Service = C900Obj::INSERVICE;
	// 	C900.Service = (C900Obj::SERVICESTATUS)GetPrivateProfileInt("Service",sect,C900.Service,winini);
	// 	str.Format("%d", C900.Service);
	// 	WritePrivateProfileString("Service", sect, str, winini);

	// 	sect = "128Chamber";
	// 	C128.Service = C128Obj::INSERVICE;
	// 	C128.Service = (C128Obj::SERVICESTATUS)GetPrivateProfileInt("Service",sect,C128.Service,winini);
	// 	str.Format("%d", C128.Service);
	// 	WritePrivateProfileString("Service", sect, str, winini);

	sect = "413Chamber";
	C413.Service = C413Obj::INSERVICE;
	C413.Service = (C413Obj::SERVICESTATUS)GetPrivateProfileInt("Service", sect, C413.Service, winini);
	str.Format("%d", C413.Service);
	WritePrivateProfileString("Service", sect, str, winini);
}

CStateModels::~CStateModels() {}

CSubstLocObj* CStateModels::GetAlgnSubstLocObj() {
	return &Algn;
}

CSubstLocObj* CStateModels::GetScaleSubstLocObj() {
	return &Scale;
}

CSubstLocObj* CStateModels::GetRbtArmSubstLocObj() {
	return &RbtArm;
}

CSubstLocObj* CStateModels::Get128SubstLocObj() {
	return &Stage1;
}

CSubstLocObj* CStateModels::Get413SubstLocObj() {
	return &Stage1;
}

CSubstLocObj* CStateModels::Get900SubstLocObj() {
	return &Stage2;
}

EqpStatus* CStateModels::GetEqpStatus() {
	return &EqpState;
}

C900Obj* CStateModels::Get900Obj() {
	return &C900;
}

C128Obj* CStateModels::Get128Obj() {
	return &C128;
}

C413Obj* CStateModels::Get413Obj() {
	return &C413;
}

// void CStateModels::Set900Serv(C900Obj::SERVICESTATUS status)
// {
// 	C900.Service = status;
// 	CString str;
// 	CString sect;
// 	CString winini;
// 	CString AcsMode;
// 	DosUtil.GetLocalCfgFile(winini);
// 	sect = "900Chamber";
// 	str.Format("%d", C900.Service);
// 	WritePrivateProfileString("Service", sect, str, winini);
// }

// void CStateModels::Set128Serv(C128Obj::SERVICESTATUS status)
// {
// 	C128.Service = status = status;
// 	CString str;
// 	CString sect;
// 	CString winini;
// 	CString AcsMode;
// 	DosUtil.GetLocalCfgFile(winini);
// 	sect = "128Chamber";
// 	str.Format("%d", C128.Service);
// 	WritePrivateProfileString("Service", sect, str, winini);
// }

void CStateModels::Set413Serv(C413Obj::SERVICESTATUS status) {
	C413.Service = status = status;
	CString str;
	CString sect;
	CString winini;
	CString AcsMode;
	DosUtil.GetLocalSysFile(winini);
	sect = "413Chamber";
	str.Format("%d", C413.Service);
	WritePrivateProfileString("Service", sect, str, winini);
}

CString CStateModels::Get900ServStr() {
	switch (C900.Service) {
	case C900Obj::INSERVICE:
		return "In Service";
		break;
	case C900Obj::OUTOFSERVICE:
		return "Out of Service";
		break;
	default:
		break;
	}
	return "";
}

CString CStateModels::Get128ServStr() {
	switch (C128.Service) {
	case C900Obj::INSERVICE:
		return "In Service";
		break;
	case C900Obj::OUTOFSERVICE:
		return "Out of Service";
		break;
	default:
		break;
	}
	return "";
}

CString CStateModels::Get413ServStr() {
	switch (C413.Service) {
	case C900Obj::INSERVICE:
		return "In Service";
		break;
	case C900Obj::OUTOFSERVICE:
		return "Out of Service";
		break;
	default:
		break;
	}
	return "";
}

CString CStateModels::GetRbtServStr() {
	switch (RbtObj.Service) {
	case C900Obj::INSERVICE:
		return "In Service";
		break;
	case C900Obj::OUTOFSERVICE:
		return "Out of Service";
		break;
	default:
		break;
	}
	return "";
}

void CStateModels::SetRbtServ(CRbtObj::SERVICESTATUS status) {
	RbtObj.Service = status;
}

CRbtObj* CStateModels::GetRbtObj() {
	return &RbtObj;
}

CString CStateModels::GetScaleServStr() {
	switch (ScaleObj.Service) {
	case C900Obj::INSERVICE:
		return "In Service";
		break;
	case C900Obj::OUTOFSERVICE:
		return "Out of Service";
		break;
	default:
		break;
	}
	return "";
}

void CStateModels::SetScaleServ(CScaleObj::SERVICESTATUS status) {
	ScaleObj.Service = status;
}

CScaleObj* CStateModels::GetScaleObj() {
	return &ScaleObj;
}