// StatusReq.cpp: implementation of the CStatusRequest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "StatusReq.h"
#include "InfoPanelMain.h"
#include "..\SERIAL\MsgItem.h"
//#include "..\FEM\RBT\AlignerSwitcher.h"
//#include "MessageDispatcher.h"
#include "ProcJobInfo.h"

#include "GlobalDeviceContainer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusRequest::CStatusRequest() {
	srID = 0;
	srParent = NULL;
}

CStatusRequest::~CStatusRequest() {}

// BOOL CStatusRequest::StartReq128(BOOL bBook)
// {
// 	CMessageDispatcher *pDspchr = pGDev->GetMessageDispatcher128();
// 	if (pDspchr)
// 	{
// 		CMsgItem *pMsg = new CMsgItem;
// 		if (pMsg) {
// 			pMsg->AckWnd = GetHWnd();
// 			pMsg->DataTyp = CMsgItem::DATAREQ;
// 			if (bBook)
// 			{
// 				pMsg->Ack = ID_IPC_DATAREQACK1;
// 				pMsg->Nak = ID_IPC_DATAREQNAK1;
// 				pMsg->DataID = CMsgItem::SV1;  // negative return indicate tool is busy [12/19/2007 YUEN]
// 			}
// 			else
// 			{
// 				pMsg->Ack = ID_IPC_DATAREQACK;
// 				pMsg->Nak = ID_IPC_DATAREQNAK;
// 				pMsg->DataID = CMsgItem::SV;
// 			}
// 			// pMsg is deleted internally, except when receive pMsg->Nak
// 			if (pDspchr->Send(pMsg)) {
// 				SetDataReqTimeOut(10000);
// 				return TRUE;
// 			}
// 		}
// 	}
// 	return FALSE;
// }

// void CStatusRequest::DataReqAck128(LPARAM lP)
// {
// 	CString str;
// 	m_Message = "";
//
// 	if (lP)
// 	{
// 		CMsgItem *p = (CMsgItem *)lP;
// 		if ( p->DataTyp == CMsgItem::DATAREQACK )
// 		{
// 			char *q = p->pMsg;
// 			if (q)
// 			{
// 				SV128.Conv(&q);
// 				str.Format("Scale is %sONLINE\n", SV128.bScaleOnline?" ": "NOT ");
// 				m_Message += str;
// 				if (m_Parent) {
// 					CProcJobInfo *pInfo = &pGDev->m_ProcJobInfo[ID];
// 					if (pInfo) {
// 						CNCas *pCas = &pInfo->Cas;
// 						if (pCas) {
// 							pCas->SysInfo.BP750=SV128.bp780;
// 							pCas->SysInfo.BP810=SV128.bp810;
// 						}
// 					}
// 				}
// 				SV128.WaferPresent = CAlignerSwitcher::GetWaferPresent();
// 				str.Format("Aligner has %swafer\n",SV128.WaferPresent & 1?"":"NO ");
// 				m_Message += str;
// 				str.Format("Scale has %swafer\n",SV128.WaferPresent & 2?"":"NO ");
// 				m_Message += str;
// 				str.Format("Sample stage has %swafer\n",SV128.WaferPresent & 4?"":"NO ");
// 				m_Message += str;
// 			}
// 		}
// 		delete p;
// 	}
// }

// BOOL CStatusRequest::StartReq413(BOOL bBook)
// {
// 	CMessageDispatcher *pDspchr = pGDev->GetMessageDispatcher413();
// 	if (pDspchr)
// 	{
// 		CMsgItem *pMsg = new CMsgItem;
// 		if (pMsg)
// 		{
// 			pMsg->AckWnd = GetHWnd();
// 			pMsg->DataTyp = CMsgItem::DATAREQ;
// 			if (bBook)
// 			{
// 				pMsg->Ack = ID_IPC_DATAREQACK1;
// 				pMsg->Nak = ID_IPC_DATAREQNAK1;
// 				pMsg->DataID = CMsgItem::SV1;  // negative return indicate tool is busy [12/19/2007 YUEN]
// 			}
// 			else
// 			{
// 				pMsg->Ack = ID_IPC_DATAREQACK;
// 				pMsg->Nak = ID_IPC_DATAREQNAK;
// 				pMsg->DataID = CMsgItem::SV;
// 			}
// 			// pMsg is deleted internally, except when receive pMsg->Nak
// 			if (pDspchr->Send(pMsg))
// 			{
// 				SetDataReqTimeOut(10000);
// 				return TRUE;
// 			}
// 		}
// 	}
// 	return FALSE;
// }

void CStatusRequest::DataReqAck413(LPARAM lP) {
	CString str;
	m_Message = "";

	ASSERT(0); // TODO: Put in wafer detection, end effector, aligner and stage [7/8/2012 Yuen]

	if (lP) {
		CMsgItem* p = (CMsgItem*)lP;
		if (p->DataTyp == CMsgItem::DATAREQACK) {
			char* q = p->pMsg;
			if (q) {
				SV413.Conv(&q);
				str.Format("Sample stage has %swafer\n", SV413.bWaferPresent ? "" : "NO ");
				m_Message += str;
				//		BOOL bWaferPresent = CAlignerSwitcher::GetWaferPresent();
				//		str.Format("Aligner has %swafer\n",bWaferPresent?"":"NO ");
				//		m_Message += str;
				//		if (bWaferPresent)
				//		{
				//			SV413.bWaferPresent = TRUE;
				//		}
			}
		}
		delete p;
	}
}

// BOOL CStatusRequest::StartReq900(BOOL bBook)
// {
// 	CMessageDispatcher *pDspchr = pGDev->GetMessageDispatcher900();
// 	if (pDspchr)
// 	{
// 		CMsgItem *pMsg = new CMsgItem;
// 		if (pMsg) {
// 			pMsg->AckWnd = GetHWnd();
// 			pMsg->DataTyp = CMsgItem::DATAREQ;
// 			if (bBook)
// 			{
// 				pMsg->Ack = ID_IPC_DATAREQACK1;
// 				pMsg->Nak = ID_IPC_DATAREQNAK1;
// 				pMsg->DataID = CMsgItem::SV1;
// 			}
// 			else
// 			{
// 				pMsg->Ack = ID_IPC_DATAREQACK;
// 				pMsg->Nak = ID_IPC_DATAREQNAK;
// 				pMsg->DataID = CMsgItem::SV;
// 			}
// 			// pMsg is deleted internally, except when receive pMsg->Nak
// 			if (pDspchr->Send(pMsg)) {
// 				SetDataReqTimeOut(90000);
// 				return TRUE;
// 			}
// 		}
// 	}
// 	return FALSE;
// }

// void CStatusRequest::DataReqAck900(LPARAM lP)
// {
//
// 	m_Message = "";
// 	if (lP)
// 	{
// 		CMsgItem *p = (CMsgItem *)lP;
// 		if ( p->DataTyp == CMsgItem::DATAREQACK )
// 		{
// 			char *q = p->pMsg;
// 			if (q)
// 			{
// 				SV900.Conv(&q);
//
// 				if (SV900.OpMode == 2) { // Temporary not available
// 					delete p;
// 					return ;
// 				}
// 				m_L1.Format("%.2f",SV900.Lamp[0]);
// 				m_L2.Format("%.2f",SV900.Lamp[1]);
// 				m_L3.Format("%.2f",SV900.Lamp[2]);
// 				m_L4.Format("%.2f",SV900.Lamp[3]);
// 				m_L5.Format("%.2f",SV900.Lamp[4]);
// 				m_L6.Format("%.2f",SV900.Lamp[5]);
// 				m_L7.Format("%.2f",SV900.Lamp[6]);
// 				m_L8.Format("%.2f",SV900.Lamp[7]);
// 				m_L9.Format("%.2f",SV900.Lamp[8]);
// 				m_L10.Format("%.2f",SV900.Lamp[9]);
// 				m_L11.Format("%.2f",SV900.Lamp[10]);
// 				m_L12.Format("%.2f",SV900.Lamp[11]);
//
// 				CString str;
// 				str.Format("Chamber is %s\n",SV900.bChamberHot?"HOT  (>80degC)":"COLD (<80degC)");
// 				m_Message = str;
// 				str.Format("Gas is %s PRESENT\n",SV900.bGasPresent?"":"NOT");
// 				m_Message += str;
// 				if(SV900.bHeatOK) {
// 					str.Format("Chamber is READY TO HEAT\n");
// 				}
// 				else {
// 					str.Format("Chamber is NOT READY TO HEAT\n");
// 				}
// 				m_Message += str;
// 				str.Format("Over temperature sensor 1 is %sactive\n", SV900.bOverTemp1?"":"NOT ");
// 				m_Message += str;
// 				str.Format("Over temperature sensor 2 is %sactive\n", SV900.bOverTemp2?"":"NOT ");
// 				m_Message += str;
// 				str.Format("Roughing pump is %s\n",SV900.bRoughPump?"ON":"OFF");
// 				m_Message += str;
// 				str.Format("Turbo pump is %s\n",SV900.bTurboPump?"ON":"OFF");
// 				m_Message += str;
// 				str.Format("Vacuum has %sreach roughing level\n",SV900.bVacE3?"":"NOT ");
// 				m_Message += str;
// 				str.Format("Vacuum has %sreach operation level\n",SV900.bVacE5?"":"NOT ");
// 				m_Message += str;
// 				str.Format("Wafer is %sFLOWING\n",SV900.bWaterFlow?"":"NOT ");
// 				m_Message += str;
// 				str.Format("There is %swater leak\n",SV900.bWaterLeak?"":"NO ");
// 				m_Message += str;
//
// 				m_Message += "\n\n";
//
// 				str = "Chamber: ";
// 				switch(SV900.Environ) {
// 				case C900SVar::GASMODE:
// 					str += "GASMODE";
// 					break;
// 				case C900SVar::VACMODE:
// 					str += "VACMODE";
// 					break;
// 				case C900SVar::PURGEMODE:
// 					str += "PURGEMODE";
// 					break;
// 				default:
// 					str += "OFFMODE";
// 					break;
// 				}
// 				m_Message += str;
// 				m_Message += "\n";
//
// 				str = "Gas setting: ";
// 				switch(SV900.GasSetting) {
// 				case C900SVar::N2GAS:
// 					str += "NITROGEN";
// 					break;
// 				case C900SVar::ARGAS:
// 					str += "ARGON";
// 					break;
// 				case C900SVar::FORMINGGAS:
// 					str += "FORMING";
// 					break;
// 				default:
// 					str += "OFF";
// 					break;
// 				}
// 				m_Message += str;
// 				m_Message += "\n";
//
// 				str.Format("Temperature: %d\n",SV900.Tempr);
// 				m_Message += str;
// 				str.Format("IG Pressure: %.2e\n",SV900.VacIG);
// 				m_Message += str;
// 				str.Format("TC1 Pressure: %.2e\n",SV900.VacTC);
// 				m_Message += str;
//
// 				str = "Watch Dog is OFF";
// 				m_Message += str;
//
// 				if (m_Parent) {
// 					CProcJobInfo *pInfo = &pGDev->m_ProcJobInfo[ID];
// 					if (pInfo) {
// 						CNCas *pCas = &pInfo->Cas;
// 						if (pCas) {
// 							pCas->SysInfo.BP750=SV900.bp780;
// 							pCas->SysInfo.BP810=SV900.bp810;
// 						}
// 					}
// 				}
// 			}
// 		}
// 		delete p;
// 	}
// }
//