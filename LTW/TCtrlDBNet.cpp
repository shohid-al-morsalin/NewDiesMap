// TCtrlDBNet.cpp: implementation of the CTCtrlDBNet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCtrlDBNet.h"
#include "..\SERIAL\MsgItem.h"
//#include "..\E95\MessageDispatcher.h"

#include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTCtrlDBNet::CTCtrlDBNet() {}

CTCtrlDBNet::~CTCtrlDBNet() {}

void CTCtrlDBNet::Update() {
	LTwrVar LT;

	// Tower control is assumed to resides in 128 Computer
// 	CMsgItem *pMsg = new CMsgItem;
// 	if (pMsg)
// 	{
// 		LT.Red = TranslateState(Red);		// Crolles: White
// 		LT.Amber = TranslateState(Amber);	// Crolles: Amber
// 		LT.Green = TranslateState(Green);	// Crolles: Blue
//
// 		long sz = LT.GetLength();
// 		if (pMsg->Alloc(sz+16)) {
// 			char *p = pMsg->pMsg;
// 			pMsg->Len = LT.Binary(&p);
// 			pMsg->MsgSrc = CMsgItem::MSE95;
// 			pMsg->DataTyp = CMsgItem::EVENTDATA;
// 			pMsg->DataID = CMsgItem::LTwr;
// 			CMessageDispatcher *pDspchr = pGDev->GetMessageDispatcher128();
// 			if (pDspchr) {
// 				pDspchr->Send(pMsg);
// 			}
// 			else {
// 				delete pMsg;
// 				ASSERT(0);
// 			}
// 		}
// 		else {
// 			delete pMsg;
// 			ASSERT(0);
// 		}
// 	}
}

LTwrVar::LTLED CTCtrlDBNet::TranslateState(LIGHTSIGNAL state) {
	switch (state) {
	case ON:
		return LTwrVar::ON;
		break;
	case BLINK:
		return LTwrVar::BLINK;
		break;
	case OFF:
		return LTwrVar::OFF;
		break;
	case BEEP1:
		return LTwrVar::BUZZER1;
		break;
	case BEEP2:
		return LTwrVar::BUZZER2;
		break;
	}
	// LYF Default to off [4/23/2006]
	return LTwrVar::OFF;
}