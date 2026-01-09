#include "stdafx.h"

// #include "ComBase.h"
#include "ComThread.h"
#include "IPC/Smol.h"
#include "IPC/SMHdr.h"

#include "AsystFoup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

//BOOL CAsystFoup::LEDLoadUnload(CSMHdr & SM)
//{
//	BOOL ret = Add(SM);
//	return ret;
//}

BOOL CAsystFoup::LedManual(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::LedClearAll() {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTCLEARALLLED;
	return Add(SM);
}
BOOL CAsystFoup::LedReserved(CSMHdr& SM) {
	return Add(SM);
}

CAsystFoup::CAsystFoup() {
	//	CString sect;
	//	CString winini;
	//	DosUtil.GetLocalCfgFile(winini);
}

CAsystFoup::~CAsystFoup() {}

CString CAsystFoup::GetSM() {
	if (pComTh) {
		return pComTh->GetSlotMap();
	}
	return "000000000000000000000000000";
}

BOOL CAsystFoup::SetE84AccessMode(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::Open(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::Close(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::Calib(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::Map(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::Register(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::RegisterLPBar(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::Stop(CSMHdr& SM) {
	return Disconnect();
}

BOOL CAsystFoup::Status(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::GetStatus(BOOL& bRdy, BOOL& bInP, BOOL& bAlr) {
	if (!pComTh) {
		ASSERT(0);
		return FALSE;
	}

	return pComTh->GetStatus(bRdy, bInP, bAlr);
}

BOOL CAsystFoup::Clamp(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::Unclamp(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::CloseNoRelease(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::SetDefault(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::EnableHOAvbl(CSMHdr& SM) {
	return Add(SM);
}

BOOL CAsystFoup::EnableButtonReport(CSMHdr& SM) {
	return Add(SM);
}

//BOOL CAsystFoup::LedAlarmed(CSMHdr &SM)
//{
//	return Add(SM);
//}

BOOL CAsystFoup::Connect(RPLY& rpy, short unit, short port) {
	Unit = unit;
	CSMHdr SM;
	SM.cmd = CSMHdr::CTCONNECT;
	SM.sub[0] = port;
	SM.sub[1] = Unit;
	SM.sub[2] = bNoClose;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	return Add(SM);
}

BOOL CAsystFoup::Disconnect() {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTDISCONNECT;
	strcpy(SM.Caller, Caller);
	return Add(SM);
}

BOOL CAsystFoup::Reset(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTRESET;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	return Add(SM);
}

BOOL CAsystFoup::Auto(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTAUTO;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	return Add(SM);
}

BOOL CAsystFoup::NoVac(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTNOVAC;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	return Add(SM);
}

BOOL CAsystFoup::NeedVac(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTNEEDVAC;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	return Add(SM);
}

BOOL CAsystFoup::Abort(RPLY& rpy) {
	CSMHdr SM;
	SM.cmd = CSMHdr::CTABORT;
	SM.HWin = rpy.HWin;
	SM.Ack = rpy.Ack;
	SM.Nak = rpy.Nak;
	strcpy(SM.Caller, Caller);
	return Add(SM);
}