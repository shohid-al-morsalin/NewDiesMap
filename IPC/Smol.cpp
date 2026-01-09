// CSMOL.cpp: implementation of the CSMOL class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SMOL.h"
#include "..\SRC\DOSUtil.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSMOL::CSMOL() {
	cmd = cmdTERMINATE;
}

CSMOL::~CSMOL() {}

// void OSSMemS::Preset()
// {
// 	ZeroMemory(ObjID, OBJIDLEN + 1);
// 	ZeroMemory(ObjType, OBJTYPELEN + 1);
// }

void SubstHistory::Preset() {
	ZeroMemory(SubstLocID, LOTIDLEN + 1);
	TimeIn = 0;
	TimeOut = 0;
}

//short AGVObj::GGetStatus()
//{
//	switch (Status)
//	{
//		case AGVObj::OFF:
//			return 0;
//			break;
//		case AGVObj::ON:
//			return 1;
//			break;
//	}
//	return 2;
//}
//
//void AGVObj::Preset(short port)
//{
//	Status = AGVObj::ON;
//	PortNo = port;
//}

//short BCRObj::GGetStatus()
//{
//	switch (Status)
//	{
//		case BCRObj::OFF:
//			return 0;
//			break;
//		case BCRObj::ON:
//			return 1;
//			break;
//	}
//	return 2;
//}
//
//void BCRObj::Preset(short port)
//{
//	Status = BCRObj::ON;
//	if (DosUtil.GetBCRMode(Status))
//	{
//		Status = BCRObj::ON;
//	}
//	PortNo = port;
//}

void RPLY::Reply(int ans) {
	if (HWin) {
		::PostMessage(HWin, WM_COMMAND, ans, NULL);
	}
}

// short OSSMemS::GetBinarySize()
// {
// 	short siz = 0;
//
// 	siz = sizeof(ObjID);
// 	siz += sizeof(ObjType);
//
// 	return siz;
// }

int CIDRObj::GGetServiceStatus() {
	switch (Service) {
	case CIDRObj::OUTOFSERVICE:
		return 0;
		break;
	case CIDRObj::INSERVICE:
		return 1;
		break;
	default:
		ASSERT(0);
		break;
	}
	return -1;
}

CString CIDRObj::GetServiceStatusStr() {
	switch (Service) {
	case CIDRObj::OUTOFSERVICE:
		return "OUT OF SERVICE";
		break;
	case CIDRObj::INSERVICE:
		return "IN SERVICE";
		break;
	default:
		ASSERT(0);
		break;
	}
	return "";
}

CIDRObj::SERVICESTATUS CIDRObj::GetServiceStatus() {
	return Service;
}

CIDRObj::BYPASSSTATUS CIDRObj::GetBypassStatus() {
	return Bypass;
}

void CIDRObj::SetServiceStatus(SERVICESTATUS status) {
	Service = status;
}

void CIDRObj::SetBypassStatus(BYPASSSTATUS status) {
	Bypass = status;
}

CIDRObj::SERVICESTATUS CIDRObj::GetServiceStatus(short port) {
	CString str;
	CString sect;
	CString winini;
	DosUtil.GetLocalSysFile(winini);

	sect.Format("CIDR%d", port);
	Service = (SERVICESTATUS)GetPrivateProfileInt("Service", sect, Service, winini);

	return Service;
}

CIDRObj::BYPASSSTATUS CIDRObj::GetBypassStatus(short port) {
	CString str;
	CString sect;
	CString winini;
	DosUtil.GetLocalSysFile(winini);

	sect.Format("CIDR%d", port);
	Bypass = (BYPASSSTATUS)GetPrivateProfileInt("Bypass", sect, Bypass, winini);

	return Bypass;
}

void CIDRObj::SetServiceStatus(SERVICESTATUS status, short port) {
	Service = status;

	CString str;
	CString sect;
	CString winini;
	DosUtil.GetLocalSysFile(winini);
	sect.Format("CIDR%d", port);
	str.Format("%d", Service);
	WritePrivateProfileString("Service", sect, str, winini);
}

void CIDRObj::SetBypassStatus(BYPASSSTATUS status, short port) {
	Bypass = status;

	CString str;
	CString sect;
	CString winini;
	DosUtil.GetLocalSysFile(winini);
	sect.Format("CIDR%d", port);
	str.Format("%d", Bypass);
	WritePrivateProfileString("Bypass", sect, str, winini);
}