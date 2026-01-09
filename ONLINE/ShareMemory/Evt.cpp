// Event.cpp: implementation of the CEvt class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Evt.h"
#include "..\GEM\SecsItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char x = 'A';

CEvt::CEvt() {
	Clear();
	pReport = NULL;
}

CEvt::CEvt(int id, int mk) {
	Clear();
	Set(id, mk);
}

CEvt::CEvt(const CEvt& Rhs) {
	Clear();

	eventid = Rhs.eventid;
	make = Rhs.make;
	no = Rhs.no;
	SetMsg(Rhs.pmsg, Rhs.nmsg);
	SysByte = Rhs.SysByte;
	wafercnt = Rhs.wafercnt;
	memcpy(slotnolist, Rhs.slotnolist, sizeof(slotnolist));
	slotno = Rhs.slotno;

	//BOOL onoff;
	alid = Rhs.alid;
	alcd = Rhs.alcd;
	almno = Rhs.almno;
	//	cnt = Rhs.cnt;
	ReplyWnd = Rhs.ReplyWnd;
	ASSERT(0);  // Todo [7/4/2010 Yuen]
// 	memcpy(&scndta,&Rhs.scndta,sizeof(ScanData));
	PortNo = Rhs.PortNo;
	memcpy(SubstID, Rhs.SubstID, OBJIDLEN + 1);
	memcpy(LocID, Rhs.LocID, LOCIDLEN + 1);
	pReport = Rhs.pReport;
	Rhs.pReport = NULL;
}

CEvt& CEvt::operator =(const CEvt& Rhs) {
	Clear();

	eventid = Rhs.eventid;
	make = Rhs.make;
	no = Rhs.no;
	phase = Rhs.phase;
	option = Rhs.option;
	SetMsg(Rhs.pmsg, Rhs.nmsg);
	SysByte = Rhs.SysByte;
	wafercnt = Rhs.wafercnt;
	memcpy(slotnolist, Rhs.slotnolist, sizeof(slotnolist));
	slotno = Rhs.slotno;

	//BOOL onoff;
	alid = Rhs.alid;
	alcd = Rhs.alcd;
	almno = Rhs.almno;
	//	cnt = Rhs.cnt;
	ReplyWnd = Rhs.ReplyWnd;
	memcpy(&scndta, &Rhs.scndta, sizeof(sScanData));
	PortNo = Rhs.PortNo;
	memcpy(SubstID, Rhs.SubstID, OBJIDLEN + 1);
	memcpy(LocID, Rhs.LocID, LOCIDLEN + 1);
	pReport = Rhs.pReport;
	Rhs.pReport = NULL;
	PRJobState = Rhs.PRJobState;
	return *this;
}

CEvt::~CEvt() {
	Dealloc();
	delete pReport;
	pReport = NULL;
}

void CEvt::Set(int id, int mk, char* bytes, short cnt) {
	eventid = id;
	make = mk;
	SetMsg((BYTE*)bytes, cnt);
}

void CEvt::SetObjID(int id, int mk, char* txt, short cnt) {
	Set(id, mk, txt, cnt);
}

void CEvt::SetPrjID(int id, int mk, char* txt, short cnt) {
	Set(id, mk, txt, cnt);
}

void CEvt::Set(BOOL off, int id, int mk, short aid, short acd, short amno) {
	//	cnt=0;
	eventid = id;
	make = mk;
	alid = aid;
	alcd = acd;
	almno = amno;
}

void CEvt::Set(int id, int mk, short n) {
	//	cnt=0;
	eventid = id;
	make = mk;
	no = n;
}
void CEvt::Set(int id, int mk, short n, char* message) {
	//	cnt=0;
	eventid = id;
	make = mk;
	no = n;
	SetMsg((BYTE*)message, strlen(message));
}

void CEvt::Set(int id, int mk, short ct, short* slt) {
	//	cnt=0;
	eventid = id;
	make = mk;
	wafercnt = ct;
	for (int i = 0; i < 25; i++) {
		slotnolist[i] = *(slt++);
	}
}
void CEvt::Set(int id, int mk, short acd, short aid, char* altxt) {
	eventid = id;
	make = mk;
	alcd = acd;
	alid = aid;
	SetMsg((BYTE*)altxt, strlen(altxt));
}

void CEvt::Clear() {
	pmsg = NULL;
	nmsg = 0;
	phase = 0;
	option = NORMAL;

	pMPoint = NULL;
	pMeParam = NULL;
	pRcp = NULL;

	wafercnt = 0;
	for (int i = 0; i < 25; i++) slotnolist[i] = 0;
	//	cnt = 0;
	PortNo = -1;
	alid = -1;
	alcd = -1;
	almno = -1;
	eventid = -1;
	make = -1;
	slotno = 0;
	Dealloc();
	ZeroMemory(SubstID, OBJIDLEN + 1);
	ZeroMemory(LocID, LOCIDLEN + 1);
	ZeroMemory(ObjID, OBJIDLEN + 1);
	ZeroMemory(PRJobID, PRJIDLEN + 1);
	ZeroMemory(CJobID, CRJIDLEN + 1);
	scndta.act = sScanData::READ;
	scndta.BowHeight = 0;
	scndta.EndTime = 0;
	scndta.FilmThick = 0;
	scndta.LineNo = 0;
	scndta.PortNo = 0;
	scndta.Radius = 0;
	scndta.ScanNo = 0;
	scndta.SConst = 0;
	scndta.SlotNo = 0;
	scndta.StartTime = 0;
	scndta.Stress = 0;
	ZeroMemory(scndta.SubstID, sizeof(scndta.SubstID));
	scndta.SUnit = sScanData::DYNES;
	scndta.Temperature = 0;
	scndta.WaferNo = 0;
	scndta.WaferThick = 0;

	PortNo = 0;
	ReplyWnd;
	SysByte = 0;
}

void CEvt::Set(int id, int mk) {
	//	cnt=0;
	eventid = id;
	make = mk;
}

void CEvt::Set(int id, int mk, UINT SByte) {
	//	cnt=0;
	eventid = id;
	make = mk;
	SysByte = SByte;
}

BOOL CEvt::Alloc(long nbytes) {
	//	Dealloc();
	if (pmsg) {
		delete[] pmsg;
		pmsg = NULL;
		nmsg = 0;
	}
	pmsg = new BYTE[nbytes + 2];
	if (pmsg) {
		nmsg = nbytes;
		*(pmsg + nbytes + 1) = 0;
		return TRUE;
	}
	ASSERT(0);
	return FALSE;
}

BOOL CEvt::SetMsg(BYTE* bytrs, long nbytes) {
	if (nbytes) {
		if (Alloc(nbytes)) {
			memcpy(pmsg, bytrs, nbytes);
			// @@Vijay What is the purpose of the next statement ??
			// 11 July 03
			//*(pmsg+nbytes) = x;
			*(pmsg + nbytes) = 0;
			x++;
			return TRUE;
		}
	}
	else {
		Dealloc();
	}
	return FALSE;
}

void CEvt::Dealloc() {
	if (pmsg) {
		delete[] pmsg;
		pmsg = NULL;
		nmsg = 0;
	}
}

CEvt::OPTION CEvt::GetOption() {
	return option;
}

BYTE* CEvt::GetMsg(long len) {
	if (!pmsg) {
		ASSERT(0);
		return NULL;
	}
	if (len < 0) {
		return pmsg;
	}
	if (len > nmsg) {
		ASSERT(0);
		return NULL;
	}
	return pmsg;
}

int CEvt::GetNMsg() {
	return nmsg;
}