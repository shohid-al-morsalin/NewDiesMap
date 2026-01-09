// FSMLightTower.cpp: implementation of the CFSMLightTower class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FSMLightTower.h"
#include "..\src\comm.h"
#include "..\src\DosUtil.h"

CFSMLightTower* pFSMTower = NULL;
/*
Signal Tower Controller, Copyrights 2003.
Frontier Semiconductor Measurements, V2.0, Programmed by Qing He
Data Format : 9600 bps, 8 data bits  1 stop bit, no parity.
Command  Format :
R, r    RED     ON / OFF
O, o    YELLOW  ON / OFF
G, g    GREEN   ON / OFF
B, b    BLUE    ON / OFF
W, w    WHITE   ON / OFF
P, p    BUZZER1  ON / OFF
K, k    BUZZER2  ON / OFF
E, e    RED    FLASH   ON / OFF
Y, y    YELLOW FLASH   ON / OFF
N, n    GREEN  FLASH   ON / OFF
L, l    BLUE   FLASH   ON / OFF
H, h    WHITE  FLASH   ON / OFF
s, S        Reset System.
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFSMLightTower::CFSMLightTower() {
	pCom = NULL;
	Port = 0;
	bSound = FALSE;
	bInit = FALSE;
	pFSMTower = this;
}

CFSMLightTower::~CFSMLightTower() {
	pFSMTower = NULL;
	SetConditionAllOff();

	if (pCom) {
		pCom->Close();
		delete pCom;
		pCom = NULL;
	}
}

BOOL CFSMLightTower::SetConditionAllOff() {
	if (!pCom) {
		return FALSE;
	}
	Set('0', '0', '0', '0');

	// Turn buzzer 1 off
	if (!pCom->SendFSMSignalTower('p')) {
		// Log Message
	}
	// Turn buzzer 2 off
	if (!pCom->SendFSMSignalTower('k')) {
		// Log Message
	}
	// Orange off
	//if(!pCom->SendFSMSignalTower('o'))
	//{
		// Log Message
	//}
	// Green Off
	//if(!pCom->SendFSMSignalTower('g'))
	//{
		// Log Message
	//}
	// Red off
	//if(!pCom->SendFSMSignalTower('r'))
	//{
		// Log Message
	//}
	// Orange flash Off
	//if(!pCom->SendFSMSignalTower('b'))
	//{
		// Log Message
	//}
	return TRUE;
}

//BOOL CFSMLightTower::SetConditionDownOrManual()
//{
//	return SetConditionAllOff();
//}

//BOOL CFSMLightTower::SetConditionLoadUnloadRequest()
//{
//	if (!pCom)
//	{
//		return FALSE;
//	}
//	if(!SetConditionAllOff())
//	{
//		// Log Message
//	}
//	// Set green light on
//	if(!pCom->SendFSMSignalTower('G'))
//	{
//		// Log Message
//	}
//	// Amber Light Flash
//	if(!pCom->SendFSMSignalTower('Y'))
//	{
//		// Log Message
//	}
//	return TRUE;
//}

//BOOL CFSMLightTower::SetConditionStandby()
//{
//	if (!pCom)
//	{
//		return FALSE;
//	}
//	if(!SetConditionAllOff())
//	{
//		// Log Message
//	}
//	// Amber light flash
//	if(!pCom->SendFSMSignalTower('Y'))
//	{
//		// Log Message
//	}
//	return TRUE;
//}

//BOOL CFSMLightTower::SetConditionProcessing()
//{
//	if (!pCom)
//	{
//		return FALSE;
//	}
//	if(!SetConditionAllOff())
//	{
//		// Log Message
//	}
//	// Green Light On
//	if(!pCom->SendFSMSignalTower('G'))
//	{
//		// Log Message
//	}
//	return TRUE;
//}

//BOOL CFSMLightTower::SetConditionAlarmed()
//{
//	if (!pCom)
//	{
//		return FALSE;
//	}
//	if(!SetConditionAllOff())
//	{
//		// Log Message
//	}
//	// Flash red light
//	if(!pCom->SendFSMSignalTower('E'))
//	{
//		// Log Message
//	}
//	return TRUE;
//}

//BOOL CFSMLightTower::SetConditionAirAlarmed()
//{
//	if (!pCom)
//	{
//		return FALSE;
//	}
//	// Flash red light
//	if(!pCom->SendFSMSignalTower('E'))
//	{
//		// Log Message
//	}
//	if(bSound)
//	{
//		// Sound alarm 1 too
//		if(!pCom->SendFSMSignalTower('P'))
//		{
//			// Log Message
//		}
//	}
//	return TRUE;
//}

BOOL CFSMLightTower::Reset() {
	if (!pCom) {
		return FALSE;
	}
	// Reset the signal tower system
	if (!pCom->SendFSMSignalTower('S')) {
		// Log Message
	}
	return TRUE;
}

BOOL CFSMLightTower::Init() {
	if (bInit) {
		return TRUE;
	}
	Port = DosUtil.GetSignalTowerPort();
	if (Port < 1) {
		// Invalid value of port
		return FALSE;
	}
	pCom = new CComm;
	pCom->SetParam(unsigned char(Port), 9600, 8, 0, 1, 0);
	if (pCom->Open()) {
		bInit = TRUE;
		return TRUE;
	}
	return FALSE;
}

BOOL CFSMLightTower::GreenOn() {
	if (pCom) {
		return pCom->SendFSMSignalTower('G');
	}
	return FALSE;
}

BOOL CFSMLightTower::GreenOff() {
	if (pCom) {
		return pCom->SendFSMSignalTower('g');
	}
	return FALSE;
}

BOOL CFSMLightTower::RedOn() {
	if (pCom) {
		return pCom->SendFSMSignalTower('R');
	}
	return FALSE;
}

BOOL CFSMLightTower::RedOff() {
	if (pCom) {
		return pCom->SendFSMSignalTower('r');
	}
	return FALSE;
}

BOOL CFSMLightTower::AmbOn() {
	if (pCom) {
		return pCom->SendFSMSignalTower('O');
	}
	return FALSE;
}

BOOL CFSMLightTower::AmbOff() {
	if (pCom) {
		return pCom->SendFSMSignalTower('o');
	}
	return FALSE;
}

BOOL CFSMLightTower::RedFlOn() {
	if (pCom) {
		return pCom->SendFSMSignalTower('E');
	}
	return FALSE;
}

BOOL CFSMLightTower::RedFlOff() {
	if (pCom) {
		return pCom->SendFSMSignalTower('e');
	}
	return FALSE;
}

BOOL CFSMLightTower::AmbFlOn() {
	if (pCom) {
		return pCom->SendFSMSignalTower('Y');
	}
	return FALSE;
}

BOOL CFSMLightTower::AmbFlOff() {
	if (pCom) {
		return pCom->SendFSMSignalTower('y');
	}
	return FALSE;
}

BOOL CFSMLightTower::Buzzer1On() {
	if (pCom) {
		return pCom->SendFSMSignalTower('P');
	}
	return FALSE;
}

BOOL CFSMLightTower::Buzzer1Off() {
	if (pCom) {
		return pCom->SendFSMSignalTower('p');
	}
	return FALSE;
}

BOOL CFSMLightTower::Buzzer2On() {
	if (pCom) {
		return pCom->SendFSMSignalTower('K');
	}
	return FALSE;
}

BOOL CFSMLightTower::Buzzer2Off() {
	if (pCom) {
		return pCom->SendFSMSignalTower('k');
	}
	return FALSE;
}

BOOL CFSMLightTower::BlueOn() {
	if (pCom) {
		return pCom->SendFSMSignalTower('B');
	}
	return FALSE;
}

BOOL CFSMLightTower::BlueOff() {
	if (pCom) {
		return pCom->SendFSMSignalTower('b');
	}
	return FALSE;
}

BOOL CFSMLightTower::BlueFlOn() {
	if (pCom) {
		return pCom->SendFSMSignalTower('L');
	}
	return FALSE;
}

BOOL CFSMLightTower::BlueFlOff() {
	if (pCom) {
		return pCom->SendFSMSignalTower('l');
	}
	return FALSE;
}

BOOL CFSMLightTower::GreenFlOn() {
	if (pCom) {
		return pCom->SendFSMSignalTower('N');
	}
	return FALSE;
}

BOOL CFSMLightTower::GreenFlOff() {
	if (pCom) {
		return pCom->SendFSMSignalTower('n');
	}
	return FALSE;
}

void CFSMLightTower::Update() {
	switch (Red) {
	case ON:
		RedOn();
		break;
	case BLINK:
		RedFlOn();
		break;
	case OFF:
	default:
		RedOff();
		break;
	}

	switch (Green) {
	case ON:
		GreenOn();
		break;
	case BLINK:
		GreenFlOn();
		break;
	case OFF:
	default:
		GreenOff();
		break;
	}

	switch (Amber) {
	case ON:
		AmbOn();
		break;
	case BLINK:
		AmbFlOn();
		break;
	case OFF:
	default:
		AmbOff();
		break;
	}

	switch (Blue) {
	case ON:
		BlueOn();
		break;
	case BLINK:
		BlueFlOn();
		break;
	case OFF:
	default:
		BlueOff();
		break;
	}
}