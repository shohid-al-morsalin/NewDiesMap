// TCtrlBase.cpp: implementation of the CTCtrlBase class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TCtrlBase.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCtrlBase::CTCtrlBase() {
	Red = NOSTATE;
	Green = NOSTATE;
	Blue = NOSTATE;
	Amber = NOSTATE;

	bUpdate = TRUE;
}

CTCtrlBase::~CTCtrlBase() {}

void CTCtrlBase::Update() {
	//	TRACE3("Red %d Amb %d Grn %d\n", Red, Amber, Green);
}

void CTCtrlBase::Set(char Rd, char Am, char Gr, char Bl) {
	if (Green != GetState(Gr)) {
		bUpdate = TRUE;
		Green = GetState(Gr);
	}
	if (Amber != GetState(Am)) {
		bUpdate = TRUE;
		Amber = GetState(Am);
	}
	if (Red != GetState(Rd)) {
		bUpdate = TRUE;
		Red = GetState(Rd);
	}
	// 	Bl = '0';
	if (Blue != GetState(Bl)) {
		bUpdate = TRUE;
		Blue = GetState(Bl);
	}

	if (bUpdate) {
		bUpdate = FALSE;
		Update();
	}
}

CTCtrlBase::LIGHTSIGNAL CTCtrlBase::GetState(char st) {
	switch (st) {
	case '0':
		return OFF;
		break;
	case '1':
		return ON;
		break;
	case '2':
		return BLINK;
		break;
	default:
		ASSERT(0);
		break;
	}
	return NOSTATE;
}

BOOL CTCtrlBase::Init() {
	return TRUE;
}

BOOL CTCtrlBase::SetConditionAllOff() {
	return TRUE;
}

void CTCtrlBase::Set(CTCtrlBase::LIGHTSIGNAL Rd, CTCtrlBase::LIGHTSIGNAL Am, CTCtrlBase::LIGHTSIGNAL Gr, CTCtrlBase::LIGHTSIGNAL Bl) {
	Red = Rd;
	Amber = Am;
	Green = Gr;
	Blue = Bl;
}

char CTCtrlBase::GetState(CTCtrlBase::LIGHTSIGNAL st) {
	switch (st) {
	case OFF:
		return '0';
		break;
	case ON:
		return '1';
		break;
	case BLINK:
		return '2';
		break;
	case BEEP1:
		return '3';
		break;
	case BEEP2:
		return '4';
		break;
	default:
		ASSERT(0);
		break;
	}
	return '0';
}

void CTCtrlBase::Get(char& Rd, char& Am, char& Gr, char& Bl) {
	Rd = GetState(Red);
	Am = GetState(Amber);
	Gr = GetState(Green);
	Bl = GetState(Blue);
}