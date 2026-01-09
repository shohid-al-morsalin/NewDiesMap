// GemFactory.cpp: implementation of the CGemFactory class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "strSOp.h"
#include "resource.h"
#include <crtdbg.h>

#include "128/GEM/ObjectsContainer.h"
#include "413/EchoScope/EchoProbeBase.h"
#include "CtrlJobMgr.h"
#include "Data.h"
#include "DataConverter.h"
#include "FFUThread.h"
#include "GlobalDeviceContainer.h"
#include "GlobalUIContainer.h"
#include "IFC/SecsSvrF.h"
#include "InfoPanelHostMsg.h"
#include "IPC/SMCHdr.h"
#include "MPoint.h"
#include "ONLINE/INC/Event.h"
#include "ONLINE/MFC/SpoolMan.h"
#include "ONLINE/SECS/S2F23.h"
#include "ONLINE/SECS/S2F31.h"
#include "ONLINE/SECS/S2F41.h"
#include "ONLINE/SECS/S2Msg.h"
#include "ONLINE/ShareMemory/Evt.h"
#include "ONLINE/TCPIP/FsmCoreSocket.h"
#include "ONLINE/TCPIP/HSMSComThread.h"
#include "ONLINE/TCPIP/HsmsParaMeter.h"
#include "Recipe413.h"
#include "Recipe413NSMgr.h"
#include "SecsItem.h"
#include "SRC/DOSUtil.h"
#include "SYS/CUMMsg.h"
#include "TracTimer.h"
// #include "413App.h"
// #include "CediListCtroller.h"
// #include "direct.h"
// #include "IPC/EqpStatus.h"
// #include "ONLINE/INC/E5-ErrCodes.h"
// #include "ONLINE/INC/Status.h"
// #include "ONLINE/TCPIP/ComLog.h"
// #include "PRJOB.H"
// #include "PRJobMgr.h"

#include "GemFactory.h"

extern string CurrCtrlJobId;
extern BOOL bCjValid;
string CurrPrJobId;
BOOL bPrjValid;
USHORT SetupName;
extern BOOL bDisabled;
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
#pragma warning(push, 3)	//(disable:4244 4018 4663)

CPtrList CGemFactory::m_pTraceList;
sScanData CGemFactory::LastScan;
extern int m_gAuto;
extern BOOL m_COMMODE;
extern HWND m_hToolWnd;
extern HDC m_hDC;

struct Reply {
	BYTE StreamId;
	BYTE STRACK;
	vector<BYTE> ErrroFunctions;
};

/*
********************************************************************
* Extern Class
*/

extern CShareMemory m_ShareMemory;
// extern CFmsRWUPCoreSocket * m_pClientSocket;
extern HWND g_hBfWnd;

#define LOG_INFO(a) LogMessages((a),(__FILE__),(__LINE__));
#define LOG_WARN(a) LogMessages((a),(__FILE__),(__LINE__),(WARNING_MESSAGE));
#define LOG_ERR(a) LogMessages((a),(__FILE__),(__LINE__),(ERROR_MESSAGE));
#define LOG_FATAL_ERR(a) LogMessages((a),(__FILE__),(__LINE__),(FATAL_ERROR_MESSAGE));

/*
*****************************************************************
*  Global Define
*/

//  Yuen [7/5/2002/0:42]
UINT EstablishCommunicationTimeout = 6000;		// unit milliseconds [8/14/2012 Administrator]
UINT TimeFormat = 1;

CGemFactory* m_pFactory = NULL;

void ChangeControlStatus(EqpStatus::CONTROLSTATE nNewStatus) {
	if (nNewStatus == EqpStatus::ATTEMPTONLINE) {
		return;
	}
	pGDev->pGEMObjs->SetLineMode(nNewStatus);
	CSMOL SM;
	switch (nNewStatus) {
	case EqpStatus::REMOTE:
		SM.cmd = CSMOL::cmdLINEMODEACTION;
		SM.Lm.act = LineModeAction::GOREMOTE;
		if (pGDev->pGEMObjs->pSecsSvr) {
			if (!pGDev->pGEMObjs->pSecsSvr->LineModeActionsExec(SM)) {
				ASSERT(0);
			}
			else {
				AfxGetApp()->WriteProfileInt("ControlState", "LineMode", nNewStatus);
			}
		}
		break;
	case EqpStatus::LOCAL:
		SM.cmd = CSMOL::cmdLINEMODEACTION;
		SM.Lm.act = LineModeAction::GOLOCAL;
		if (pGDev->pGEMObjs->pSecsSvr) {
			if (!pGDev->pGEMObjs->pSecsSvr->LineModeActionsExec(SM)) {
				ASSERT(0);
			}
			else {
				AfxGetApp()->WriteProfileInt("ControlState", "LineMode", nNewStatus);
			}
		}
		break;
	case EqpStatus::OFFLINEEQP:
		SM.cmd = CSMOL::cmdLINEMODEACTION;
		SM.Lm.act = LineModeAction::GOEQPOFFLINE;
		if (pGDev->pGEMObjs->pSecsSvr) {
			if (!pGDev->pGEMObjs->pSecsSvr->LineModeActionsExec(SM)) {
				ASSERT(0);
			}
			else {
				AfxGetApp()->WriteProfileInt("ControlState", "LineMode", nNewStatus);
			}
		}
		break;
	case EqpStatus::OFFLINEHOST:
		SM.cmd = CSMOL::cmdLINEMODEACTION;
		SM.Lm.act = LineModeAction::GOHOSTOFFLINE;
		if (pGDev->pGEMObjs->pSecsSvr) {
			if (!pGDev->pGEMObjs->pSecsSvr->LineModeActionsExec(SM)) {
				ASSERT(0);
			}
			else {
				AfxGetApp()->WriteProfileInt("ControlState", "LineMode", nNewStatus);
			}
		}
		break;
	default:
		ASSERT(0);
		break;;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGemFactory::CGemFactory() {
	UIMode = DosUtil.GetUIMode();
	if (!pGDev) {
		ASSERT(0);
	}

	bJobAlertSupport = DosUtil.GetJobAlertSupport(); //  [9/6/2007 LYF]
	DosUtil.SetJobAlertSupport(bJobAlertSupport); //  [9/6/2007 LYF]
	bJobEventSupport = DosUtil.GetJobEventSupport(); //  [9/6/2007 LYF]
	DosUtil.SetJobEventSupport(bJobEventSupport); //  [9/6/2007 LYF]
	bIgnorRecipeParameter = DosUtil.GetIgnorRecipeParameter(); //  [9/6/2007 LYF]
	DosUtil.SetIgnorRecipeParameter(bIgnorRecipeParameter); //  [9/6/2007 LYF]
}

CGemFactory::~CGemFactory() {
	m_pTraceList.RemoveAll();
}

// void * CGemFactory::set_list(void * pptr, char pitem, BYTE plen)
// {
// 	typedef interface
// 	{
// 		BYTE item;
// 		BYTE len;
// 	}
// 	DATA;
// 	DATA * sv;
// 	sv = (DATA *) pptr;
// 	sv->item = pitem;
// 	sv->len = plen;
// 	pptr = (void *) sv;
// 	pptr = (BYTE *) pptr + 2;
// 	return (pptr);
// }

// void * CGemFactory::set_struct(void * pptr, unsigned char pitem, BYTE plen, void * data)
// {
// 	typedef interface
// 	{
// 		char item;
// 		BYTE len;
// 		char data[1];
// 	}
// 	DATA;
// 	unsigned char lv_data[5];
// 	memset(lv_data, 0, 5);
// 	DATA * sv;
// 	sv = (DATA *) pptr;
// 	sv->item = pitem;
// 	sv->len = plen;
// 	if (plen)
// 	{
// 		// Allow zero length items safely  @@Vijay$8Nov#01
// 		if (pitem == UNSIGN4)
// 		{
// 			UINT * p = (UINT *) data;
// 			lv_data[0] = (unsigned char) (* p >> 24);
// 			lv_data[1] = (unsigned char) (* p >> 16);
// 			lv_data[2] = (unsigned char) (* p >> 8);
// 			lv_data[3] = (unsigned char) (* p);
// 			memcpy(sv->data, lv_data, plen);
// 		}
// 		else if (pitem == svUNSIGN2)
// 		{
// 			UINT * p = (UINT *) data;
// 			lv_data[0] = (unsigned char) (* p >> 8);
// 			lv_data[1] = (unsigned char) (* p);
// 			memcpy(sv->data, lv_data, plen);
// 		}
// 		else
// 		{
// 			memcpy(sv->data, data, plen);
// 		}
// 	}
// 	pptr = (void *) sv;
// 	pptr = (char *) pptr + (plen + 2);
// 	return pptr;
// }

int CGemFactory::set_head(int wbit, char strm, char func, GEM_HEADER* head, int size, BYTE primarymode) {
	int retc;
	// 	long lv_system[4];
	retc = (int)size;
	Set_DataLen(head, size);

	head->m_SessionId[0] = (unsigned char)((m_DeviceId >> 8)); // | 0x80 );
	head->m_SessionId[1] = (unsigned char)(m_DeviceId & 0xFFFF);
	head->m_Stream = strm;
	head->m_Function = func;
	head->m_Ptype = 0x00;
	head->m_Stype = 0x00;
	if (wbit == 0) {
		head->m_Stream &= 0x7F;
	}
	else {
		head->m_Stream |= 0x80;
	}
	if (primarymode == PRI_EQUIP) {
		//for (int i = 0; i < 4; i++)
		//{
		//	lv_system[i] = m_SystemByte;
		//}
		head->m_Sysbyte[0] = (BYTE)(/*lv_system[0]*/m_SystemByte >> 24);
		head->m_Sysbyte[1] = (BYTE)(/*lv_system[1]*/m_SystemByte >> 16);
		head->m_Sysbyte[2] = (BYTE)(/*lv_system[2]*/m_SystemByte >> 8);
		head->m_Sysbyte[3] = (BYTE)/*lv_system[3]*/m_SystemByte;
		if (m_SysByteInc) {
			m_SystemByte += m_SysByteInc;
		}
	}
	return retc;
}

void CGemFactory::Set_DataLen(GEM_HEADER* head, long size) {
	head->m_DataLen[0] = (unsigned char)(size >> 24);
	head->m_DataLen[1] = (unsigned char)(size >> 16);
	head->m_DataLen[2] = (unsigned char)(size >> 8);
	head->m_DataLen[3] = (unsigned char)size;
}

// Changed for Spooling
BOOL CGemFactory::SendBlock(GEM_HEADER* pHeader) {
	char Msg[126] =
	{
		0
	};
	int retc = 0;
	BYTE dwStream, dwFunction;
	string Data;
	int dwSndCnt = Get_DataLen(pHeader->m_DataLen);
	dwStream = (pHeader->m_Stream) & 0x7f;
	dwFunction = pHeader->m_Function;
	if (hmHSMSSS > CHsmsParaMeter::NOTSELECTED) //  [12/23/2007 YUEN]
	{
		if (m_ConnectionMode == PASSIVE) {
			pGDev->SECSContent(pHeader->m_SessionId, dwSndCnt, TRUE);
			retc = CHsmsParaMeter::Pass_SndString(pHeader, dwSndCnt + DATALENSIZE);
		}
		else if (m_ConnectionMode == ACTIVE) {
			if (m_pClientSocket) {
				pGDev->SECSContent(pHeader->m_SessionId, dwSndCnt, TRUE);
				retc = m_pClientSocket->Socket_WriteString(pHeader, dwSndCnt + DATALENSIZE);
			}
			else {
				retc = 0;
			}
		}
		if (retc) {
			tmpStr.Format("Send S%dF%d", dwStream, dwFunction);
		}
		else {
			tmpStr.Format("Send S%dF%d failed", dwStream, dwFunction);
		}
		LogMessages(tmpStr);
	}
	else {
		// Check if we have spool able message
		CSpoolMan* pSpool = CSpoolMan::GetSpool();
		if (pSpool->CanSpool(dwStream, dwFunction)) {
			retc = dwSndCnt + DATALENSIZE;
			pGDev->SECSContent(pHeader->m_SessionId, dwSndCnt, TRUE);
			BOOL bRes = pSpool->WriteMessage(pHeader, dwSndCnt + DATALENSIZE);
			if (FALSE == bRes) {
				// Spool Full take appropriate action
				retc = 0;
			}
			else {
				if (1 == pSpool->SpooledSoFar()) {
					// Notification for Spool Activated
					CEvt* pSpoolEvt = new CEvt;
					ASSERT(pSpoolEvt);
					pSpoolEvt->Set(EVT_SPOOL, EVT_SPOOL_ACTIVATED);
					m_gEvtQue.OpenMemory("EVTMSG");
					m_gEvtQue.WriteQue(pSpoolEvt);
					m_gEvtQue.CloseMemory();
					sprintf(Msg, "Spooled: S%dF%d", dwStream, dwFunction);
					LOG_INFO(Msg);
				}
			}
		}
		else {
			sprintf(Msg, "Sink: S%dF%d", dwStream, dwFunction);
			LOG_WARN(Msg);
			retc = 0;
		}
		// Clear wait bit to avoid complexity
		pHeader->m_Stream &= 0x7F;
	}
	return retc;
}

int CGemFactory::Get_DataLen(BYTE* pData) {
	int bRet = ((long)pData[0] << 24) | ((long)pData[1] << 16) | ((long)pData[2] << 8) | ((long)pData[3]);
	return bRet;
}

void CGemFactory::Message_Seperation(GEM_HEADER* pHeader) {
	short int iDeviceid = GetDeviceFromHost(pHeader);
	int dwStream = pHeader->m_Stream & 0x7f;
	int dwFunction = pHeader->m_Function;
	if (dwStream > 63) {
		mk_S9F3(pHeader);
	}
	else if (dwFunction > 63) {
		mk_S9F5(pHeader);
	}
	else if (m_DeviceId != iDeviceid) {
		mk_S9F1(pHeader);
	}
	else {
		char Msg[256];
		if (!(dwFunction & 0x1)) {
			return;
		}
		switch (dwStream) {
		case 1:
			Stream1(pHeader);
			break;
		case 2:
			Stream2(pHeader);
			break;
		case 3:
			Stream3(pHeader);
			break;
		case 5:
			Stream5(pHeader);
			break;
		case 6:
			Stream6(pHeader);
			break;
		case 7:
			Stream7(pHeader);
			break;
		case 10:
			Stream10(pHeader);
			break;
		case 14:
			Stream14(pHeader);
			break;
		case 16:
			Stream16(pHeader);
			break;
		case 18:
			Stream18(pHeader);
			break;
		default:
			sprintf(Msg, "(S%dF%d) Unknown Stream", dwStream, pHeader->m_Function);
			LOG_WARN(Msg);
			mk_S9F3(pHeader);
			break;
		}
	}
}

int CGemFactory::Stream1(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(1, dwFunction)) {
		mk_S01F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 1:
		mk_S1F2(pHeader);
		break;
	case 3:
		mk_S1F4(pHeader);
		break;
	case 11:
		mk_S1F12(pHeader);
		break;
	case 13:
		mk_S1F14(pHeader);
		break;
	case 15:
		mk_S1F16(pHeader);
		break;
	case 17:
		mk_S1F18(pHeader);
		break;
	default:
		mk_S9F5(pHeader);
		break;
	}
	return 0;
}

int CGemFactory::Stream2(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(2, dwFunction)) {
		mk_S02F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 13:
		mk_S2F14(pHeader);
		break;
	case 15:
		mk_S2F16(pHeader);
		break;
	case 17:
		mk_S2F18(pHeader);
		break;
	case 21:
		mk_S2F22(pHeader);
		break;
	case 23:
		mk_S2F24(pHeader);
		break;
	case 25:
		mk_S2F26(pHeader);
		break;
	case 29:
		mk_S2F30(pHeader);
		break;
	case 31:
		mk_S2F32(pHeader);
		break;
	case 33:
		mk_S2F34(pHeader);
		break;
	case 35:
		mk_S2F36(pHeader);
		break;
	case 37:
		mk_S2F38(pHeader);
		break;
	case 39:
		mk_S2F40(pHeader);
		break;
	case 41:
		mk_S2F42(pHeader);
		break;
	case 43:
		mk_S2F44(pHeader);
		break;
	default:
	{
		char Msg[126];
		sprintf(Msg, "(S2F%d) Unknown Function ", pHeader->m_Function);
		LOG_WARN(Msg);
	}
	mk_S9F5(pHeader);
	break;
	}
	return 0;
}

int CGemFactory::Stream3(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(3, dwFunction)) {
		mk_S03F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 17:
		mk_S3F18(pHeader);
		break;
	case 21:
		mk_S3F22(pHeader);
		break;
	case 23:
		mk_S3F24(pHeader);
		break;
	case 25:
		mk_S3F26(pHeader);
		break;
	case 27:
		mk_S3F28(pHeader);
		break;
	default:
	{
		char Msg[126];
		sprintf(Msg, "(S3F%d) Unknown Function", pHeader->m_Function);
		LOG_WARN(Msg);
	}
	mk_S9F5(pHeader);
	break;
	}
	return 0;
}

int CGemFactory::Stream10(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(10, dwFunction)) {
		mk_S10F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 3: // Terminal display Single
		mk_S10F4(pHeader);
		break;
	case 5: // Terminal display ,Multiblock
		mk_S10F6(pHeader);
		break;
	case 9: // Broadcast (BCN)
		mk_S10F10(pHeader);
		break;
	default:
	{
		char Msg[126];
		sprintf(Msg, "(S10F%d) Unknown Function", pHeader->m_Function);
		LOG_WARN(Msg);
	}
	mk_S9F5(pHeader);
	break;
	}
	return 0;
}

int CGemFactory::Stream14(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(14, dwFunction)) {
		mk_S14F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 1:
		mk_S14F2(pHeader);
		break;
	case 3:
		mk_S14F4(pHeader);
		break;
	case 5:
		mk_S14F6(pHeader);
		break;
	case 7:
		mk_S14F8(pHeader);
		break;
	case 9:
		mk_S14F10(pHeader);
		break;
	case 11:
		mk_S14F12(pHeader);
		break;
	default:
	{
		char Msg[126];
		sprintf(Msg, "(S14F%d) Unknown Function", pHeader->m_Function);
		LOG_WARN(Msg);
	}
	mk_S9F5(pHeader);
	break;
	}
	return 0;
}

int CGemFactory::Stream16(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(16, dwFunction)) {
		mk_S16F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 1:
		mk_S16F2(pHeader);
		break;
	case 3:
		mk_S16F4(pHeader);
		break;
	case 5:
		mk_S16F6(pHeader);
		break;
	case 11:
		mk_S16F12(pHeader);
		break;
	case 13:
		mk_S16F14(pHeader);
		break;
	case 15:
		mk_S16F16(pHeader);
		break;
	case 17:
		mk_S16F18(pHeader);
		break;
	case 19:
		mk_S16F20(pHeader);
		break;
	case 21:
		mk_S16F22(pHeader);
		break;
	case 25:
		mk_S16F26(pHeader);
		break;
	case 27:
		mk_S16F28(pHeader);
		break;
	default:
	{
		char Msg[126];
		sprintf(Msg, "(S16F%d) Unknown Function", pHeader->m_Function);
		LOG_WARN(Msg);
	}
	mk_S9F5(pHeader);
	break;
	}
	return 0;
}

int CGemFactory::mk_S1F2(GEM_HEADER* pHeader) {
	LOG_INFO("(S1F1) Are you there R");

	int retc = 0;
	SecsMessage S1F1In;
	UINT Len = AtoUint(pHeader->m_DataLen) - 10;

	if (S1F1In.DecodeMessage(pHeader->m_Message, Len)) {
		if (0 == S1F1In.size()) {
			SecsMessage Message;
			SecsItemList* pList = new SecsItemList();
			pList->AddItem(new SecsItemASCII(string(CEIDCtlr.m_SVList.m_Svid[SVID_LIST::DEVICEID].GetV())));
			pList->AddItem(new SecsItemASCII(string(CEIDCtlr.m_SVList.m_Svid[SVID_LIST::SOFTREV].GetV())));
			Message.AddItem(pList);
			Message.MakeBinaryMessage(pHeader->m_Message);
			retc = set_head(0, 1, 2, pHeader, Message.BinaryMessageLength() + 10, PRI_HOST);
			SendBlock(pHeader);
			return retc;
		}
		else {
			LOG_ERR("(S1F1) Incorrect message structure");
		}
	}
	else {
		LOG_ERR("(S1F1) Unsupported or Incorrect binary format ");
	}
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S1F4(GEM_HEADER* pHeader) {
	LOG_INFO("(S1F3) Selected equipment status request SSR");

	int retc = 0;
	SecsMessage OutMsg;
	MessageS1F3 Message;
	vector < UINT > Svids;
	if (Message.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Message.GetSvidList(Svids); // Get list of SVIDS from the message
		GetSvData(Svids, OutMsg); // Get the data copied into the message
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 1, 4, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S1F3) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

/*
Support only main terminal i.e. terminal 0
The message indicator is a green button
this button when pressd will show a dialog of the message
*/

int CGemFactory::mk_S10F4(GEM_HEADER* pHeader) {
	LOG_INFO("(S1F3) Terminal request, single (VTN)");
	int retc = 0;
	int dw_Wbit = 0;
	BYTE ACKC10 = 0;
	static string MessageText; // need to persist for PostMessage to work [9/15/2007 LYF]
	BYTE TID;
	char Buf[126];
	MessageS10F3 Msg;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		TID = Msg.GetTerminalMsg(MessageText);
		if ((MessageText.size() < MAX_CONSOLE_MSG) && (TID == 0)) {
			//			memset(pGDev->pDataBank->m_HostMsg1, ' ', MAX_CONSOLE_MSG + 5);
			if (MessageText.size() > 0) {
				// strncpy(pGDev->pDataBank->m_HostMsg1, MessageText.c_str(), MessageText.size());
				// pGDev->pHostMessage->Message(MessageText.c_str()); //  [9/15/2007 LYF]
				pUDev->pHostMessage->PostMessage(UM_HOST_MESSAGE, 0, (LPARAM)&MessageText);
				//	::SendMessage(::m_gHwnd, WM_HOST_MESSAGE_RECD, MessageText.size(), 0);
			}
			else {
				ACKC10 = 1; // Excess message length cannot display
				sprintf(Buf, "(S10F3) Incorrect Message Length");
				LOG_ERR(Buf);
			}
		}
		else {
			ACKC10 = 2; // Terminal not available
			sprintf(Buf, "(S10F3) Invalid Terminal specified ");
			LOG_ERR(Buf);
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&ACKC10, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 10, 4, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	else {
		sprintf(Buf, "(S10F3) Incorrect Format");
		LOG_ERR(Buf);
		mk_S9F7(pHeader);
	}
	return retc;
}

//  [9/15/2007 LYF]
// int CGemFactory::mk_S10F2(GEM_HEADER * pHeader)
// {
// 	LOG_INFO("(S10F1) Terminal request TRN");
// 	int retc    = 0;
// 	MessageS10F1 InMsg;
// 	BYTE ACKC10 = 1;
// 	if (InMsg.CheckMessage(pHeader->m_DataLen,pHeader->m_Message))
// 	{
// 		if(InMsg.TID == 0){
// 			//			ZeroMemory(pGDev->pDataBank->m_HostMsg1, MAX_CONSOLE_MSG);
// 			//			strncpy(pGDev->pDataBank->m_HostMsg1,InMsg.Text.c_str(),MAX_CONSOLE_MSG);
// 			pGDev->pHostMessage->Message(InMsg.Text.c_str());
// 			ACKC10 = 0;
// 		}
// 		SecsMessage OutMsg;
// 		OutMsg.AddItem(new SecsItemBinary(&ACKC10,1));
// 		OutMsg.MakeBinaryMessage(pHeader->m_Message);
// 		retc = set_head(0, 10, 2, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
// 		SendBlock(pHeader);
// 		//		::InvalidateRect(m_gHwnd, & m_gRect, TRUE);
// 		return retc;
// 	}
// 	LOG_ERR("(S10F1) Incorrect Format")
// 	mk_S9F7(pHeader);
// 	return retc;
// }

unsigned int CGemFactory::AtoUint(unsigned char* pData) {
	unsigned int retc = (UINT)pData[0] << 24 | (UINT)pData[1] << 16 | (UINT)pData[2] << 8 | (UINT)pData[3];
	return retc;
}

USHORT CGemFactory::AtoUshort(unsigned char* pData) {
	USHORT retc = (UINT)(UINT)pData[0] << 8 | (UINT)pData[1];
	return retc;
}

UINT CGemFactory::ByteSwap(unsigned char* pData) {
	UINT retc = (UINT)(UINT)pData[0] << 16 | (UINT)pData[1] << 8 | (UINT)pData[2];
	return retc;
}

BOOL CGemFactory::Initial_ListContrll() {
	CEIDCtlr.Initialize_List();
	//	CSpoolMan *pSpool = CSpoolMan::GetSpool();
	//	pSpool->EnableAllStreamFunctions();
	//	pSpool->InitSpool();
	// memset(&m_Equipstatus,0,sizeof(m_Equipstatus));
	InitGlobals();
	return TRUE;
}

int CGemFactory::EventLineMode(short phase, CEvt* pEvt, GEM_HEADER& shead) {
	int retc = 0;
	switch (pEvt->make) {
	case EVT_LINEPHASE_CHGREMOTE: // may be obsolete
		ASSERT(0);
		retc = evt_line_remoteonline(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_REMOTEEVT:
		retc = evt_line_remoteevt(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_CHGLOCAL: // may be obsolete
		ASSERT(0);
		retc = evt_line_localonline(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_LOCALEVT:
		retc = evt_line_localevt(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_OFFLINE:
		retc = evt_line_offline(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_COMMESTABLISH:
		retc = evt_line_commestablish(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_HOSTOFF:
		retc = evt_line_hostoff(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_ATTEMPT_ONLINE:
		retc = evt_attempt_online(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_ATTEMPT_LOCAL:
		retc = evt_attempt_local(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_BCRON:
		retc = evt_line_bcron(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_BCROFF:
		retc = evt_line_bcroff(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_AGVON:
		retc = evt_line_agvon(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_AGVOFF:
		retc = evt_line_agvoff(phase, pEvt, &shead);
		break;
	case EVT_REPLY_TIMEOUT:
		evt_Reply_Timeout(phase, pEvt, &shead);
		break;
	case EVT_LINEPHASE_INIT_DATA:
		retc = evt_init_data(phase, pEvt, &shead);
		break;
	default:
		EventSink(pEvt->eventid, pEvt->make);
		retc = 0;
		break;
	}
	return retc;
}

int CGemFactory::EventCMS(short phase, CEvt* pEvt, GEM_HEADER& shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(&shead, pEvt);
	}
	else if (phase == 1) {
		retc = 1;
	}
	return retc;
}

int CGemFactory::Event_Proc(short phase, CEvt* pEvt, GEM_HEADER& shead) {
	if (!pEvt) {
		return 0;
	}

	int retc = 0;
	CString dd("");
	switch (pEvt->eventid) {
	case EVT_LINEMODE:
		retc = EventLineMode(phase, pEvt, shead);
		break;
	case EVT_CMS:
		retc = EventCMS(phase, pEvt, shead);
		break;
	case EVT_ALARM:
		retc = evt_alarm(phase, pEvt, &shead);
		break;
	case EVT_CJM:
		// Yuen Obsolete [11/25/2002]
		ASSERT(0);
		break;
		// 	case EVT_PJM:
		// 		retc = evt_pjm_Process(phase, pEvt, &shead);
		// 		break;
	case EVT_PJM_PRJEN:
		retc = evt_pjm_PJEN(phase, pEvt, &shead);
		break;
	case EVT_TIMETRACE:
		retc = evt_timetracer(phase, pEvt, &shead);
		break;
	case EVT_ETS:
		switch (pEvt->make) {
		case EVT_MSG_TO_HOST:
			// Send to event handler
			retc = evt_ETS_SendMsgToHost(phase, pEvt, &shead);
			//
			break;
		case EVT_MSG_RECOGNIZED:
			// Host  message is recognized by the operator
			retc = evt_ETS_MSG_Recognized(phase, pEvt, &shead);
			break;
		default:
			EventSink(pEvt->eventid, pEvt->make);
			retc = 0;
			break;
		}
		break;
	case EVT_SPOOL:
		switch (pEvt->make) {
		case EVT_UNLOAD_SPOOL:
			retc = Evt_Spool_Unload(phase, pEvt, &shead);
			break;
		case EVT_SPOOL_ACTIVATED:
			retc = Evt_Spool_Activated(phase, pEvt, &shead);
			break;
		case EVT_SPOOL_DEACTIVATED:
			retc = Evt_Spool_Deactivated(phase, pEvt, &shead);
			break;
		default:
			EventSink(pEvt->eventid, pEvt->make);
			retc = 0;
			break;
		};
		break;
	case EVT_OTHER:
		switch (pEvt->make) {
		case EVT_REQUEST_TIME_FROM_HOST:
			retc = EvtGetTimeFromHost(phase, pEvt, &shead);
			break;
		case EVT_REQ_MB_RECIPE:
			retc = EvtMBEnqRecipe(phase, pEvt, &shead);
			break;
		case EVT_UPLOAD_RECIPE:
			retc = EvtUploadRecipe(phase, pEvt, &shead);
			break;
		case EVT_RCP_CHANGE:
			retc = EvtRcpChange(phase, pEvt, &shead);
			break;
			//@@Vijay#4June$02 New code added
		case EVT_DOWNLOAD_RECIPE:
			retc = EvtRcpDownload(phase, pEvt, &shead);
			break;
		case EVT_EXCESS_MESSAGE_LENGTH:
			retc = ExcessMsgLen(phase, pEvt, &shead);
		case EVT_PJMSTARTNEXTPJ:
			CJobMgr.ActivateNextPRJob();
			break;
		default:
			EventSink(pEvt->eventid, pEvt->make);
			retc = 0;
			break;
		};
		break;
	case EVT_EXTRA:
		retc = Other_Events(phase, pEvt, &shead);  // Directly call mk_S6F11 [8/6/2010 XPMUser]
		break;
	case PRJOB_ALERT:
		retc = PrJobAlert_Notify(phase, pEvt, &shead);
		break;
	case PRJOB_ALERT2:
		retc = PrJobAlert_Notify2(phase, pEvt, &shead);
		break;
		// 	case EVT_CJM_NEW:
		// 		retc = CjobEventNotify(phase, pEvt, &shead);
		// 		break;
	case EVT_UI_DATA_REQUEST:
		retc = ProcessUIDataRequest(pEvt);
		break;
	default:
		EventSink(pEvt->eventid, pEvt->make);
		retc = 0;
		break;
	}
	return retc;
}

int CGemFactory::Other_Events(short phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::CjobEventNotify(short phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::PrJobAlert_Notify(short phase, CEvt* event, GEM_HEADER* shead) {
	//  [9/6/2007 LYF]
	if (!bJobAlertSupport) {
		return 1;
	}
	int retc = 0;
	if (phase == 0) {
		retc = mk_S16F7(shead, event);
	}
	else if (phase == 1) {
		retc = 1;
	}
	return retc;
}

int CGemFactory::PrJobAlert_Notify2(short phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) {
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_readcarrieridcompleted(short phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_line_remoteonline(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	//	BYTE COMMACK = 0;
	if (phase == 0) {
		retc = mk_S1F1(shead, event);
	}
	else {
		BYTE* pData = (BYTE*)shead->m_Message;
		if (pData[4] > 0) {
			// Host denied the connection so try ATTEMPT ONLINE
			// No need to try here again a thread will do the task
		}
		else {
			// Following  code is not necessary
			// It is taken care at the time the Attempt Online is
			// changed

			ChangeControlStatus(EqpStatus::REMOTE);
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_REMOTEEVT);
			m_gEvtQue.OpenMemory("EVTMSG");
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		//	}
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_line_remoteevt(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	CEvt evt;
	if (phase == 0) {
		//	ChangeControlStatus(EqpStatus::REMOTE);
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		//		if (shead->m_Message[4] == 0) {
		//			ChangeControlStatus(EqpStatus::REMOTE);
		//		}
		retc = 0;
	}
	return retc;
}

int CGemFactory::evt_line_localevt(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		//ChangeControlStatus(EqpStatus::LOCAL);
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		//		if (shead->m_Message[4] == 0) {
		//			ChangeControlStatus(EqpStatus::LOCAL);
		//		}
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_line_offline(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		ChangeControlStatus(EqpStatus::OFFLINEEQP);
		retc = mk_S6F11(shead, event);
		retc = 1;
	}
	else if (phase == 1) // ACTION HERE
	{
		if (shead->m_Message[4] == 0) {
			InitGlobals();
			//pGDev->pDataBank->ClearJobs();
		}
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_line_hostoff(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		ChangeControlStatus(EqpStatus::OFFLINEHOST);
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		if (shead->m_Message[4] == 0) {
			InitGlobals();
			//pGDev->pDataBank->ClearJobs();
		}
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_line_bcron(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_line_bcroff(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_line_agvon(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_line_agvoff(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_loadingcompleted(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_lotidread(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_idverifyok(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_slotmapReadOk(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_selected(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_setup(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_processing(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_foupclose(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	CEvt evt;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_carriercomplete(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	CEvt evt;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_undockingcomplete(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	CEvt evt;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_readytounload(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_transferblocked(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_readytoload(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_waferpickup(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_wafermeasurefinished(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_waferreturnback(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_cassettepresent(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_cassettenopresent(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_cassettefilepresent(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_cassettefilenopresent(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_pjm_PJEN(int phase, CEvt* event, GEM_HEADER* shead) {
	//  [9/6/2007 LYF]
	if (!bJobEventSupport) {
		return 1;
	}

	int retc = 0;
	if (phase == 0) {
		retc = mk_s16f9(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
		// if (event->make == 207)
		// {
		// 	// pGDev->pDataBank->PRJob_Completed();
		// }
	}
	return retc;
}

int CGemFactory::evt_pjm_Process(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		//strcpy(::PrJobStatus, & event->msg[100]);
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::mk_S3F18(GEM_HEADER* pHeader) {
	LOG_INFO("(S3F17) Carrier action request");

	int retc = 0, cnt = 0, i = 0, arrycnt = 0;
	char caack = 0;
	UINT nCarrierAct = 0, nVal = 0;
	CString dd("");

	CSMOL SM;
	vector < ParamData > CarrierPar;
	string CarrierAct;
	string PostNo;
	MessageS3F17 Msg;
	string MsgStr = "( CMS Service :";
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		if (Msg.CarrierID.length() <= 0) {
			if ((Msg.nPort > 0) && (Msg.nPort <= 4)) {
				Msg.CarrierID = pGDev->pGEMObjs->Cms[Msg.nPort - 1].CaObj.ObjID;
			}
			else {
				// LYF [11Jan05]
				LOG_ERR("(S3F17) Incorrect Format");
				mk_S9F7(pHeader);
				return retc;
			}
		}
		char Buf[52];
		sprintf(Buf, "LP%d", Msg.nPort);
		MsgStr += " " + Msg.Action + ": Carrier (" + Msg.CarrierID + ")" + Buf + ") ";
		BYTE ErrorCode = 0;
		BYTE PortNo = Msg.nPort;
		SecsItemList* pErrorList = new SecsItemList;
		ErrorCode = pGDev->CanPerformService(Msg.Action, Msg.CarrierID, Msg.nPort, Msg.AttrIDs);
		SecsItemList* pAck = new SecsItemList;
		string Temp;
		if (ErrorCode) {
			ErrorMessages::GetErrorTextS3F18(ErrorCode, Temp);
			SecsItemList* pErrorStat = new SecsItemList;
			pErrorStat->AddItem(new SecsItemUInt1(ErrorCode));
			pErrorStat->AddItem(new SecsItemASCII(Temp));
			pErrorList->AddItem(pErrorStat);
			CString MsgStr1;
			MsgStr1.Format("S3F17 Rejected, %s", (char*)Temp.c_str());
			LogMessages(MsgStr1);
		}
		pAck->AddItem(new SecsItemUInt1(ErrorCode));
		pAck->AddItem(pErrorList);
		SecsMessage OutMsg;
		OutMsg.AddItem(pAck);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 3, 18, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		if (!ErrorCode) {
			ErrorCode = pGDev->PerformE87Service(Msg.CarrierID.data(), Msg.Action.data(),
				Msg.AttrIDs, Msg.AttrValues, pErrorList, PortNo);
			MsgStr += " Success";
			LOG_INFO(MsgStr);
		}
		else {
			sprintf(Buf, " Fail (%s)", Temp.c_str());
			MsgStr += Buf;
			LOG_ERR(MsgStr);
		}
		return retc;
	}
	LOG_ERR("(S3F17) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

// VJTD Please edit to conform to Pomsglst.ss
int CGemFactory::mk_S16F16(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F15) PRJobMultiCreate");

	char MsgStr[126];
	CString JID;
	int retc = 0;
	char caack = 0;
	CPRJob* pJob = NULL;
	vector < CPRJob* > OkPjList;
	vector < UINT > ErrCodes, ErrPrIdsAll;
	vector < string > ErrPrIds;
	vector < PrjobRepNew > PrJobs;
	string LogMsg = "(PRJobMultiCreate) ";
	MessageS16F15 MsgNew;
	if (MsgNew.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		MsgNew.GetPrJobList(PrJobs);
		if (PrJobs.size() == 0) {
			// Some error
			ErrCodes.push_back(NO_PRJOB_SPECIFIED);
			LogMsg += "No PRjobs";
		}
		else {
			for (int iJob = 0; iJob < int(PrJobs.size()); iJob++) {
				PrjobRepNew& PrRef = PrJobs[iJob];
				if ((PrRef.PrJobID.empty()) ||
					(PrRef.CarrierID.empty()) ||
					(PrRef.RecipeID.empty())) {
					ErrCodes.push_back(UNKNOWN_OBJECT_IN_OBJECT_SPECIFIER);
					sprintf(MsgStr, "Null Object identifier  specified (PrID:%s,CarrierID:%s,RcpID:%s) ",
						PrRef.PrJobID.c_str(), PrRef.CarrierID.c_str(), PrRef.RecipeID.c_str());
					LogMsg += MsgStr;
					pJob = NULL;
					caack = 1;
				}
				else if (PrRef.RcpMethd != 2) {
					ErrCodes.push_back(RECIPE_METHOD_NOT_2);
					LogMsg += "Recipe method not 2";
					pJob = NULL;
					caack = 5;
				}
				else if ((PrRef.MF < 13) || (PrRef.MF > 14)) {
					ErrCodes.push_back(ILLEGAL_MF_CODE);
					LogMsg += "Illegal MF code";
					pJob = NULL;
					caack = 1;
				}
				else if (!PrRef.PrJobID.size()) {
					ErrCodes.push_back(ZERO_LENGTH_PRJOBID);
					LogMsg += "Zero length PRJob ID";
					pJob = NULL;
					caack = 1;
				}
				else if (PJobMgr.GetJobSpace()) {
					// Job space available
					if (!PJobMgr.IsPRJobExist(PrRef.PrJobID.c_str())) {
						// PRJOBID is not duplicated

						// Matching SlotMap and WaferParams YFL [3/22/2003]
						// SlotMap takes precedence YFL [3/22/2003]
						//	if (PrRef.Slots.size() > PrRef.WaferParams.size()) {
						//		Log("SlotID size is greater then recipe parameter",2);
						// Recipe parameter not use [8/30/2010 XPMUser]
// 						if (PrRef.WaferParams.size()) {
// 							for (int i = 0; i < PrRef.Slots.size(); i++)
// 							{
// 								BOOL bFound = FALSE;
// 								vector <ParamNew>::iterator it;
// 								it = PrRef.WaferParams.begin();
// 								while (it != PrRef.WaferParams.end())
// 								{
// 									if ((*it).SlotNo == PrRef.Slots[i])
// 									{
// 										bFound = TRUE;
// 										break;
// 									}
// 									it++;
// 								}
// 								if (!bFound) {
// 									PrRef.Slots[i]=0;
// 								}
// 							}
// 							vector<BYTE>::iterator it;
// 							it = PrRef.Slots.begin();
// 							while (it != PrRef.Slots.end())
// 							{
// 								if ((*it) == 0)
// 								{
// 									it = PrRef.Slots.erase(it);
// 								}
// 								else
// 								{
// 									it++;
// 								}
// 							}
// 						}
// 						else
// 						{
// 							// No WaferParam, thus erase all slots
// 							vector<BYTE>::iterator it;
// 							it = PrRef.Slots.begin();
// 							while (it != PrRef.Slots.end())
// 							{
// 								it = PrRef.Slots.erase(it);
// 							}
// 						}
// 						if (PrRef.Slots.size())
// 						{
// 							for (int i = 0; i < PrRef.WaferParams.size(); i++)
// 							{
// 								BOOL bFound = FALSE;
// 								vector<BYTE>::iterator it;
// 								it = PrRef.Slots.begin();
// 								while (it != PrRef.Slots.end())
// 								{
// 									if ((*it) == PrRef.WaferParams[i].SlotNo)
// 									{
// 										bFound = TRUE;
// 										break;
// 									}
// 									it++;
// 								}
// 								if (!bFound)
// 								{
// 									PrRef.WaferParams[i].SlotNo = 0;
// 								}
// 							}
// 							vector<ParamNew>::iterator it;
// 							it = PrRef.WaferParams.begin();
// 							while (it != PrRef.WaferParams.end())
// 							{
// 								if ((*it).SlotNo == 0)
// 								{
// 									it = PrRef.WaferParams.erase(it);
// 								}
// 								else
// 								{
// 									it++;
// 								}
// 							}
// 						}
// 						else
// 						{
// 							// No slot id, thus erase all WaferParams
// 							vector<ParamNew>::iterator it;
// 							it = PrRef.WaferParams.begin();
// 							while (it != PrRef.WaferParams.end())
// 							{
// 								it = PrRef.WaferParams.erase(it);
// 							}
// 						}

						pJob = new CPRJob;
						pJob->SetObjectID((char*)PrRef.PrJobID.c_str()); // PrjobId
						pJob->SetMtlName(PrRef.CarrierID.c_str());
						pJob->m_PortNo = pGDev->GetPortID(PrRef.CarrierID);
						for (int nIndex = 0; nIndex < int(PrRef.WaferParams.size()); nIndex++) {
							pJob->SetWaferParams(PrRef.WaferParams[nIndex], nIndex);
						}
						pJob->m_Slots = PrRef.Slots;
						pJob->SetMF(PrRef.MF);
						// wrong location, trap at earlier stage [9/10/2007 LYF]
						//if (PrRef.MF != 13)
						//{
						//	caack = 5;
						//	// Parameters improperly specified @@E-40 1101
						//	ErrCodes.push_back(RECIPE_METHOD_NOT_2);
						//	sprintf(MsgStr,"MF (%d) ",PrRef.MF);
						//	LogMsg += MsgStr;
						//}
						pJob->SetPRStart(PrRef.bAutoStart);
						// 						pJob->SetNumberScan(PrRef.NumScanLines);
						// 						if (PrRef.NumScanLines < 1 || PrRef.NumScanLines > 16)
						// 						{
						// 						}
						// 						pJob->SetScanNo(PrRef.ScanNo);
						// 						if (PrRef.ScanNo < 1)
						// 						{
						// 						}
						pJob->SetMtlType(1);
						// Wrong location, trap at earlier stage [9/10/2007 LYF]
						//if (PrRef.RcpMethd != 2)
						//{
						//	caack = 1;
						//	// Parameters improperly specified @@E-40 1101
						//	ErrCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
						//	sprintf(MsgStr,"RCPMethod (%d) ",PrRef.RcpMethd);
						//	LogMsg += MsgStr;
						//}
						// Pause events are not supported NOW ! Just keep track
						pJob->m_PauseEvents = PrRef.PauseEvts;
						// Recipe Support @@Vijay#15May$02
						pJob->m_Recipe = PrRef.RecipeID;

						//TODO: LYF Need to verify recipe availability
						if (!RcpMgr.IsRecipeAvailable(pJob->m_Recipe)) {
							caack = 16;
							ErrCodes.push_back(RECIPE_NOT_FOUND);
							sprintf(MsgStr, "(S16F15) Recipe (%s) Not on Equipment", pJob->m_Recipe.c_str());
							LOG_WARN(MsgStr);
						}
					}
					else {
						caack = 2; // Object Identifier in use @@E-40 1101
						ErrCodes.push_back(OBJECT_IDENTIFIER_IN_USE);
						sprintf(MsgStr, "Duplicate PrjobID (%s) ", PrRef.PrJobID.c_str());
						LogMsg += MsgStr;
					}
				}
				else {
					caack = 1; // Busy (no queueing or queue full) @@E-40 1101
					ErrCodes.push_back(BUSY_NOT_QUEUE_OR_QUEUE_FULL);
					sprintf(MsgStr, "PrJobSpace FULL ");
					LogMsg += MsgStr;
				}
				if (0 == ErrCodes.size()) {
					OkPjList.push_back(pJob);
					sprintf(MsgStr, "Suceess (%s) ", PrRef.PrJobID.c_str());
					LogMsg += MsgStr;
				}
				else {
					delete pJob;
					pJob = NULL;
					sprintf(MsgStr, " Fail (%s) ", PrRef.PrJobID.c_str());
					LogMsg += MsgStr;
					for (int nErr = 0; nErr < int(ErrCodes.size()); nErr++) {
						ErrPrIdsAll.push_back(ErrCodes[nErr]);
					}
					ErrCodes.clear();
				}
				ErrPrIds.push_back(PrRef.PrJobID);
			}
		}

		if (0 == ErrCodes.size()) {
			LOG_INFO(LogMsg);
		}
		else {
			LOG_ERR(LogMsg);
		}

		int iError;
		SecsMessage OutMsg;
		SecsItemList* pTop = new SecsItemList;
		for (iError = 0; iError < int(ErrPrIds.size()); iError++) {
			pTop->AddItem(new SecsItemASCII(ErrPrIds[iError]));
		}
		SecsItemList* List = new SecsItemList;
		List->AddItem(pTop);
		pTop = NULL; // Do not want to use it any more
		pTop = new SecsItemList;
		SecsItemList* pErrCode = NULL;
		pTop->AddItem(new SecsItemBoolean(caack == 0));
		SecsItemList* pBot = NULL; // new SecsItemList;
		SecsItemList* pErrs = new SecsItemList;
		string ErrText;
		for (iError = 0; iError < int(ErrPrIdsAll.size()); iError++) {
			pErrs->AddItem(MakeErrorList(ErrPrIdsAll[iError]));
		}
		pTop->AddItem(pErrs);
		List->AddItem(pTop);
		OutMsg.AddItem(List);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 16, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		if (0 < OkPjList.size()) {
			for (int nIndex = 0; nIndex < int(OkPjList.size()); nIndex++) {
				if (PJobMgr.Create(OkPjList[nIndex]) != CPRJobMgr::OK) {
					sprintf(MsgStr, "(S16F15) PJob Create Error: %s", PJobMgr.LastErr.GetBuffer());
					LOG_ERR(MsgStr);
					ASSERT(0);
				}
			}
		}
		if (ErrPrIdsAll.size() == 0) {
			int j;
			CSMOL SM;
			SM.Ack = 0;
			SM.Nak = 0;
			SM.HWin = NULL;
			SM.RetSvr[0] = NULL;
			SM.cmd = CSMOL::cmdPCJOBEXEC;
			SM.Pj.act = SPcJobExec::DEFINEPJ;
			for (int iJob = 0; iJob < int(PrJobs.size()); iJob++) {
				PrjobRepNew& PrRef = PrJobs[iJob];
				strscpy(SM.Pj.PrJID, PRJIDLEN, PrRef.PrJobID.c_str());
				strscpy(SM.Pj.CarrierID, OBJIDLEN, PrRef.CarrierID.c_str());
				strscpy(SM.Pj.RecipeName, RECIPEIDLEN, PrRef.RecipeID.c_str());
				for (j = 0; j < MAXCAPACITYLEN; j++) {
					SM.Pj.slot[j].slotno = -1;
				}
				for (j = 0; j<int(PrRef.Slots.size()); j++) {
					int slotno = PrRef.Slots[j];
					if (slotno) {
						SM.Pj.slot[j].slotno = slotno;
						//SM.Pj.slot[j].scanno = PrRef.WaferParams[j].ScanNo;
					}
				}
				if (pGDev->pGEMObjs->pSecsSvr) {
					pGDev->pGEMObjs->pSecsSvr->PrJobExec(SM);
				}
			}
		}
		return retc;
	}
	sprintf(MsgStr, "(S16F15) Incorrect Format");
	LOG_ERR(MsgStr);
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S16F14(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F13) PRJobDuplicateCreate");

	int retc = 0, nSlotNo = 0, nPtn = 0;
	char caack = 0, databuf[65], MsgStr[126];
	float fval = 0.0;
	CPRJob* pJob = NULL;
	BOOL flag = TRUE;
	BOOL bCreated = FALSE;
	vector < CPRJob* > OkPjList;
	vector < UINT> ErrCodes, ErrPrIdsAll;
	vector < string > ErrPrIds;
	vector < PrjobRepNew > PrJobs;
	string LogMsg = "(PRJobDuplicateCreate) ";
	MessageS16F13 MsgNew;
	if (MsgNew.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		MsgNew.GetPrJobList(PrJobs);
		if (PrJobs.size() == 0) {
			// Some error
			ErrCodes.push_back(NO_PRJOB_IN_QUEUE);
			LogMsg += "No PRjobs";
		}
		else {
			for (int iJob = 0; iJob < int(PrJobs.size()); iJob++) {
				PrjobRepNew& PrRef = PrJobs[iJob];
				if ((PrRef.PrJobID.empty()) ||
					(PrRef.CarrierID.empty()) ||
					(PrRef.RecipeID.empty())) {
					ErrCodes.push_back(UNKNOWN_OBJECT_IN_OBJECT_SPECIFIER);
					sprintf(MsgStr, "Null Object identifier  specified (PrID:%s,CarrierID:%s,RcpID:%s) ",
						PrRef.PrJobID.c_str(), PrRef.CarrierID.c_str(), PrRef.RecipeID.c_str());
					LogMsg += MsgStr;
					pJob = NULL;
					caack = 1;
				}
				else if (PJobMgr.GetJobSpace()) {
					// Job space available
					if (!PJobMgr.IsPRJobExist(PrRef.PrJobID.c_str())) {
						// PRJOBID is not duplicated
						pJob = new CPRJob;
						pJob->SetObjectID((char*)PrRef.PrJobID.c_str()); // PrjobId
						// Get the carrier Name
						memcpy(databuf, PrRef.CarrierID.c_str(), PrRef.CarrierID.size());
						databuf[PrRef.CarrierID.size()] = 0;
						if (!pGDev->IsCarrierPresent(PrRef.CarrierID)) {
							// Carrier not present on the machine so... generate error
							ASSERT(0);
						}
						pJob->SetMtlName(databuf);
						string tmp = databuf;
						pJob->m_PortNo = pGDev->GetPortID(tmp);
						for (int nIndex = 0; nIndex < int(PrRef.WaferParams.size()); nIndex++) {
							pJob->SetWaferParams(PrRef.WaferParams[nIndex], nIndex);
						}
						pJob->m_Slots = PrRef.Slots;
						pJob->SetMF(PrRef.MF);
						if (PrRef.MF != 13 /*&&PrRef.MF != 14*/) {
							caack = 5;
							// Parameters improperly specified @@E-40 1101
							ErrCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
							sprintf(MsgStr, "MF (%d) ", PrRef.MF);
							LogMsg += MsgStr;
						}
						pJob->SetPRStart(PrRef.bAutoStart);
						// 						pJob->SetNumberScan(PrRef.NumScanLines);
						// 						if (PrRef.NumScanLines < 1 || PrRef.NumScanLines > 100)
						// 						{
						// 							// caack = 6;
						// 							// Parameters improperly specified @@E-40 1101
						// 							// ErrCodes.push_back(1);
						// 						}
						// 						pJob->SetScanNo(PrRef.ScanNo);
						// 						if (PrRef.ScanNo < 1)
						// 						{
						// 							// caack = 7;
						// 							// Parameters improperly specified @@E-40 1101
						// 							// ErrCodes.push_back(1);
						// 						}
						pJob->SetMtlType(1);
						if (PrRef.RcpMethd != 2) {
							caack = 1;
							// Parameters improperly specified @@E-40 1101
							ErrCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
							sprintf(MsgStr, "RCPMethod (%d) ", PrRef.RcpMethd);
							LogMsg += MsgStr;
						}
						// Pause events are not supported NOW ! Just keep track
						pJob->m_PauseEvents = PrRef.PauseEvts;
						// Recipe Support @@Vijay#15May$02
						pJob->m_Recipe = PrRef.RecipeID;
						// @@Vijay: Recipe reference removed from online
						// Marker
						//ASSERT(!"Code needs to be verified");
						//  [12/19/2007 YUEN]
						if (!RcpMgr.IsRecipeAvailable(pJob->m_Recipe))
							//if (!CRecipe::LocateRecipe(pJob->m_Recipe))
						{
							ErrCodes.push_back(RECIPE_NOT_FOUND);
							sprintf(MsgStr, "(S16F13) Recipe (%s) Not on Equipment", pJob->m_Recipe.c_str());
							LOG_WARN(MsgStr);
						}
					}
					else {
						caack = 2; // Object Identifier in use @@E-40 1101
						ErrCodes.push_back(OBJECT_IDENTIFIER_IN_USE);
						sprintf(MsgStr, "Duplicate PrjobID (%s) ", PrRef.PrJobID.c_str());
						LogMsg += MsgStr;
					}
				}
				else {
					caack = 1; // Busy (no queueing or queue full) @@E-40 1101
					ErrCodes.push_back(BUSY_NOT_QUEUE_OR_QUEUE_FULL);
					sprintf(MsgStr, "PrJobSpace FULL ");
					LogMsg += MsgStr;
				}
				if (0 == ErrCodes.size()) {
					OkPjList.push_back(pJob);
					sprintf(MsgStr, "Suceess (%s) ", PrRef.PrJobID.c_str());
					LogMsg += MsgStr;
				}
				else {
					sprintf(MsgStr, " Fail (%s) ", PrRef.PrJobID.c_str());
					LogMsg += MsgStr;
					delete pJob;
					pJob = NULL;
					for (int nErr = 0; nErr < int(ErrCodes.size()); nErr++) {
						ErrPrIdsAll.push_back(ErrCodes[nErr]);
					}
					ErrCodes.clear();
				}
				ErrPrIds.push_back(PrRef.PrJobID);
			}
		} // Else
		if (0 == ErrCodes.size()) {
			LOG_INFO(LogMsg);
		}
		else {
			LOG_ERR(LogMsg);
		}
		int iError;
		SecsMessage OutMsg;
		SecsItemList* pTop = new SecsItemList;
		for (iError = 0; iError < int(ErrPrIds.size()); iError++) {
			pTop->AddItem(new SecsItemASCII(ErrPrIds[iError]));
		}
		SecsItemList* List = new SecsItemList;
		List->AddItem(pTop);
		pTop = NULL; // Do not want to use it any more
		pTop = new SecsItemList;
		SecsItemList* pErrCode = NULL;
		pTop->AddItem(new SecsItemBoolean(caack == 0));
		SecsItemList* pBot = NULL; // new SecsItemList;
		SecsItemList* pErrs = new SecsItemList;
		string ErrText;
		for (iError = 0; iError < int(ErrPrIdsAll.size()); iError++) {
			pBot = new SecsItemList;
			pBot->AddItem(new SecsItemUInt1(ErrPrIdsAll[iError]));
			ErrorMessages::GetErrTxtE5(ErrPrIdsAll[iError], ErrText);
			pBot->AddItem(new SecsItemASCII(ErrText));
			pErrs->AddItem(pBot);
		}
		pTop->AddItem(pErrs);
		List->AddItem(pTop);
		OutMsg.AddItem(List);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 14, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		if (0 < OkPjList.size()) {
			for (int nIndex = 0; nIndex < int(OkPjList.size()); nIndex++) {
				if (PJobMgr.Create(OkPjList[nIndex]) != CPRJobMgr::OK) {
					ASSERT(0);
				}
			}
		}
		return retc;
	}
	LOG_ERR("(S16F13) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S14F10(GEM_HEADER* pHeader) {
	LOG_INFO("(S14F9) Create object request COR");

	int retc = 0;
	MessageS14F9 Msg;
	string ObjSpec;
	string MsgStr;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		vector < int > ErrList;
		string ObjCreated;
		MsgStr = "(OSS : CreateObject) ";
		int ackc = pGDev->CreateObject(Msg.ObjSpec, Msg.ObjType, Msg.Attributes, ErrList, ObjCreated);
		MsgStr += (Msg.ObjType + " : " + ObjCreated + " ");
		BYTE ObjAck = ErrList.size() ? 1 : 0;
		SecsMessage OutMsg;
		SecsItemList* pList = new SecsItemList;
		SecsItemList* pListErr = new SecsItemList;
		pList->AddItem(new SecsItemASCII(ObjCreated));
		pListErr->AddItem(new SecsItemUInt1(ObjAck));
		SecsItemList* pErrCodes = new SecsItemList;
		string ErrDesc;
		for (int nIndex = 0; nIndex < int(ErrList.size()); nIndex++) {
			SecsItemList* pErr = new SecsItemList;
			pErr->AddItem(new SecsItemUInt1(ErrList[nIndex]));
			ErrorMessages::GetErrTxtE5(ErrList[nIndex], ErrDesc);
			pErr->AddItem(new SecsItemASCII(ErrDesc));
			pErrCodes->AddItem(pErr);
			MsgStr += ErrDesc + ", ";
		}
		pListErr->AddItem(pErrCodes);
		pList->AddItem(new SecsItemList);
		pList->AddItem(pListErr);
		OutMsg.AddItem(pList);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 14, 10, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		if (ObjAck) {
			MsgStr += "Fail";
			LOG_ERR(MsgStr);
		}
		else {
			MsgStr += "Success";
			LOG_INFO(MsgStr);
		}
		if (ErrList.size() == 0) {
			CSMOL SM;
			SM.Ack = 0;
			SM.Nak = 0;
			SM.HWin = NULL;
			SM.RetSvr[0] = NULL;
			SM.cmd = CSMOL::cmdPCJOBEXEC;
			SM.Pj.act = SPcJobExec::DEFINECJ;
			strscpy(SM.Pj.PrJID, PRJIDLEN, ObjCreated.c_str());
			if (pGDev->pGEMObjs->pSecsSvr) {
				pGDev->pGEMObjs->pSecsSvr->PrJobExec(SM);
			}
		}
		return retc;
	}
	LOG_ERR("(S14F9) Incorrect Format ");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S01F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 1, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S02F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 2, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S03F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 3, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S04F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 4, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S05F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 5, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S06F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 6, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S07F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 7, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S10F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 10, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S14F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 14, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S16F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 16, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S18F00(GEM_HEADER* shead) {
	int retc;
	retc = set_head(0, 18, 0, shead, 10, PRI_HOST);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S2F17(GEM_HEADER* shead) {
	LOG_INFO("Send: S2F17 Date and Time request DTR");
	int retc;
	retc = set_head(1, 2, 17, shead, 10, PRI_EQUIP);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S9F5(GEM_HEADER* shead) {
	char Msg[126];
	sprintf(Msg, "Send: S9F3 Unknown function type S%dF%d UFN", shead->m_Stream, shead->m_Function);
	LOG_WARN(Msg);

	int retc = 0;
	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemBinary((BYTE*)shead->m_SessionId, 10));
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(0, 9, 5, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP); // sizeof(SECS_HEAD) +);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S9F7(GEM_HEADER* shead) {
	LOG_WARN("Send: S9F7 Illegal data IDN");
	int retc = 0;
	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemBinary((BYTE*)shead->m_SessionId, 10));
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(0, 9, 7, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP); // sizeof(SECS_HEAD) +);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S9F11(GEM_HEADER* shead, CEvt* pEvt) {
	LOG_WARN("Send: S9F11 Data too long DLN");
	int retc = 0;
	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemBinary((BYTE*)pEvt->GetMsg(10), 10));
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(0, 9, 11, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP); // sizeof(SECS_HEAD) +);
	SendBlock(shead);
	return retc;
}

int CGemFactory::evt_line_localonline(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S1F1(shead, event);
	}
	else {
		BYTE* pData = (BYTE*)shead->m_Message;
		if (pData[4] > 0) {
			// Host denied the connection so try ATTEMPT ONLINE
		}
		else {
			ChangeControlStatus(EqpStatus::LOCAL);
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_LOCALEVT);
			m_gEvtQue.OpenMemory("EVTMSG");
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		retc = 1;
	}
	return retc;
}

int CGemFactory::mk_S2F34(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F33) Define report DR");

	int retc = 0;
	UINT nRptid = 0;
	UINT nSvid = 0;
	short data = 0;
	BOOL loopchk = FALSE;
	int nRptcnt = 0;
	int nSvidcnt = 0;
	UINT* svidval;
	CString dd("");
	char Message[126];
	int i = 0, j = 0;
	MessageS2F33 Msg;
	vector < RptData > Reports;
	SecsMessage OutMsg;
	vector < UINT > DelRpts;
	vector < RptData > DefRpts;
	RptData Dummy;
	BOOL bOk = TRUE;
	BYTE DRACK = 0;
	CString str;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetRptDefs(Reports);
		if (Reports.size()) {
			string RptInfo;
			char Str[50];
			RptInfo = "(S2F33) Define Report(s)";
			for (int iReports = 0; iReports < int(Reports.size()); iReports++) {
				Dummy = Reports[iReports];
				if (0 == Dummy.Vids.size()) {
					// VIDS are Not specified Delete report
					sprintf(Message, "Deleting report (%d)", Dummy.RptId);
					RptInfo += Message;
					if (TRUE == CEIDCtlr.chk_rptid(Dummy.RptId)) {
						DelRpts.push_back(Dummy.RptId);
					}
				}
				else {
					if (TRUE == CEIDCtlr.chk_rptid(Dummy.RptId)) {
						// Rptid Redefinition
						bOk = FALSE;
						DRACK = 3;
						sprintf(Message, "Report redefinied (%d)", Dummy.RptId);
						LOG_ERR(Message);
						break;
					}
					else {
						sprintf(Str, "\nReport (%d) \n", Dummy.RptId);
						RptInfo += Str;
						RptInfo += " VIDs ";
						// Validate all VIDS are VALID
						for (int iSvIds = 0; iSvIds < int(Dummy.Vids.size()); iSvIds++) {
							RptInfo += _itoa(Dummy.Vids[iSvIds], Str, 10);
							if (FALSE == CEIDCtlr.chk_Vid(Dummy.Vids[iSvIds])) {
								RptInfo += ">Not Found<";
								LOG_ERR(RptInfo.c_str());
								bOk = FALSE;
								DRACK = 4;
								break;
							}
							RptInfo += "  ";
						}
						DefRpts.push_back(Dummy);
					} // Validate all VIDS are VALID
					if (!bOk) {
						break;
					}
				}
				Dummy.Vids.clear(); // Safety precaution
			}
			if (bOk) {
				// Apply changes Everything is fine
				// Delete RPT
				LOG_INFO(RptInfo.c_str());
				for (int iDel = 0; iDel < int(DelRpts.size()); iDel++) {
					CEIDCtlr.Delete_Rptid(DelRpts[iDel]);
				}
				for (int iDef = 0; iDef < int(DefRpts.size()); iDef++) {
					Dummy = DefRpts[iDef];
					svidval = new UINT[Dummy.Vids.size()];
					for (int iVids = 0; iVids < int(Dummy.Vids.size()); iVids++) {
						svidval[iVids] = Dummy.Vids[iVids];
					}
					CEIDCtlr.Add_Rpt(svidval, Dummy.Vids.size(), Dummy.RptId);
					delete[] svidval;
				}
				// Define Reports
				CEIDCtlr.SaveReport();
				CEIDCtlr.SaveMapping();
			}
		}
		else {
			// delete all report definitions
			CEIDCtlr.Delete_All_RptList();
			CEIDCtlr.SaveReport();
			CEIDCtlr.SaveMapping();
			sprintf(Message, "(S2F33) Deleting All Reports");
			LOG_INFO(Message);
			// CEIDCtlr.evtrpt_all_setting(FALSE);
		}
		OutMsg.AddItem(new SecsItemBinary(&DRACK, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 2, 34, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S2F33) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S2F36(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F35) Link event report LER");

	int retc = 0;
	UINT nRptid = 0;
	UINT nSvid = 0;
	short data = 0;
	BOOL loopchk = FALSE;
	int nRptcnt = 0;
	int nSvidcnt = 0;
	UINT* RptIdVal;
	CString dd("");
	char Message[126];
	int i = 0, j = 0;
	MessageS2F35 Msg;
	vector < LinkData > Links;
	SecsMessage OutMsg;
	vector < UINT > DelLinks;
	vector < LinkData > DefLink;
	LinkData Dummy;
	BOOL bOk = TRUE;
	BYTE LRACK = 0;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetLinkDefs(Links);
		string LinkRpts = "(S2F35) Link Report(s)";
		if (Links.size()) {
			for (int iLinks = 0; iLinks < int(Links.size()); iLinks++) {
				Dummy = Links[iLinks];
				//LinkRpt += "\n";
				if (0 == Dummy.RptIds.size()) {
					// VIDS are Not specified Delete report
					// Report MUST BE DEFINED before
					sprintf(Message, "Delete Link (%d)", Dummy.CeId);
					Log(Message, 10);
					//LinkRpt += Message;
					if (TRUE == CEIDCtlr.ceid_defined(Dummy.CeId)) {
						DelLinks.push_back(Dummy.CeId);
					}
					else {
					}
				}
				else {
					sprintf(Message, "CEID (%d)", Dummy.CeId);
					LinkRpts = Message;
					if (FALSE == CEIDCtlr.Check_Ceid(Dummy.CeId)) {
						// Ceid NOT FOUND
						LinkRpts += ">Not Found<";
						LOG_ERR(LinkRpts.c_str());
						bOk = FALSE;
						LRACK = 3;
						break;
					}
					else {
						sprintf(Message, " RptID(%d) ", Dummy.CeId);
						LinkRpts += Message;
						// Validate all RPTIDs are VALID
						for (int iRptId = 0; iRptId < int(Dummy.RptIds.size()); iRptId++) {
							sprintf(Message, "%d ", Dummy.RptIds[iRptId]);
							LinkRpts += Message;
							if (FALSE == CEIDCtlr.chk_rptid(Dummy.RptIds[iRptId], TRUE)) {
								LinkRpts += ">Not Found<";
								LOG_ERR(LinkRpts.c_str());
								bOk = FALSE;
								LRACK = 4;
								break;
							}
							Log(LinkRpts.c_str(), 10);
						}
						DefLink.push_back(Dummy);
					} // Validate all VIDS are VALID
					if (!bOk) {
						break;
					}
				}
				Dummy.RptIds.clear(); // Safety precaution
			}
			if (bOk) {
				// Apply changes Everything is fine
				// Delete RPT
				for (int iDel = 0; iDel < int(DelLinks.size()); iDel++) {
					CEIDCtlr.UnlinkCeid(DelLinks[iDel]);
				}
				for (int iDef = 0; iDef < int(DefLink.size()); iDef++) {
					Dummy = DefLink[iDef];
					RptIdVal = new UINT[Dummy.RptIds.size()];
					for (int iVids = 0; iVids < int(Dummy.RptIds.size()); iVids++) {
						RptIdVal[iVids] = Dummy.RptIds[iVids];
					}
					CEIDCtlr.set_ceid1(Dummy.CeId, RptIdVal, Dummy.RptIds.size());
					delete[] RptIdVal;
				}
				// Define Reports
				CEIDCtlr.SaveMapping();
			}
		}
		else {
			// remove all associations
			// CEIDCtlr.Delete_All_RptList();
		}
		LOG_INFO(LinkRpts);
		OutMsg.AddItem(new SecsItemBinary(&LRACK, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 2, 36, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S2F35) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S2F38(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F37) Enable/Disable event report EDER");

	int ret = 0;
	BYTE Data = 0;
	MessageS2F37 MsgIn;
	char Str[52];
	string Enab = "(S2F37) Enable/Disable CEID(s):";
	if (MsgIn.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		if (MsgIn.CEIDs.size() == 0) {
			Enab += (MsgIn.CEED) ? "\n  Enable All" : "\n  Disable All";
			if (!CEIDCtlr.evtrpt_all_setting(MsgIn.CEED ? TRUE : FALSE)) {
				Data = 2;
			}
		}
		else {
			Enab += (MsgIn.CEED) ? "\n  Enable CEID(s) " : "\n  Disable CEID(s) ";
			for (int i = 0; i < int(MsgIn.CEIDs.size()); i++) {
				sprintf(Str, "%d", MsgIn.CEIDs[i]);
				Enab += Str;
				if (!CEIDCtlr.Check_Ceid(MsgIn.CEIDs[i])) {
					Enab += ">Not Found<";
					Data = 1;
					break;
				};
				Enab += ",";
			}
			if (Data == 0) {
				for (int i = 0; i < int(MsgIn.CEIDs.size()); i++) {
					CEIDCtlr.evtrpt_setting(MsgIn.CEIDs[i], MsgIn.CEED ? TRUE : FALSE);
				}
			}
		}
		if (Data == 0) {
			LOG_INFO(Enab.c_str());
			CEIDCtlr.SaveEnaDisCEIDAll();
			CEIDCtlr.SaveReport();
			CEIDCtlr.SaveMapping();
		}
		else {
			LOG_ERR(Enab.c_str());
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&Data, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		ret = set_head(0, 2, 38, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return ret;
	}
	mk_S9F7(pHeader);
	LOG_ERR("(S2F37) Incorrect Format")
		return ret;
}

int CGemFactory::mk_S2F40(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F39) Multi-block enquire DMBI");

	int retc = 0;
	MessageMBEnq Msg;
	char MsgStr[126];
	UINT DataLen;
	BYTE GRANT = 0;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		DataLen = Msg.GetDataLen();
		if (DataLen > 5000) {
			sprintf(MsgStr, "(S2F39) Message Too Long (%d)", DataLen);
			LOG_ERR(MsgStr);
			GRANT = 2;
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&GRANT, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 2, 40, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S2F39) Incorrect Message Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S6F6(GEM_HEADER* pHeader) {
	LOG_INFO("(S6F5) Multi-block data send inquire MBI");

	int retc = 0;
	MessageMBEnq Msg;
	UINT DataLen, GRANT = 0;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		DataLen = Msg.GetDataLen();
		if (DataLen > 14000) {
			GRANT = 2;
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemUInt4(GRANT));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 6, 6, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S6F5) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S6F11(GEM_HEADER* shead, CEvt* Evt) {
	// Allowed in Offline mode [10/30/2002]
	int retc = 0;
	unsigned int ceid = 0, rptlistcount = 0;
	unsigned int svidcount = 0;
	unsigned int dataid = 0;
	int sendcnt = 0, i = 0;
	string EventNote;
	char Buf[25] =
	{
		0
	};

	CString dd("");
	vector < SVID_VALUE > dSvval;
	if (Evt->eventid == EVT_LINEMODE) {
		switch (Evt->make) {
		case EVT_LINEPHASE_REMOTEEVT:
			ceid = CEID_LIST::ONLINEREMOTE; // EQUIPMENT REMOTE ONLINE
			break;
		case EVT_LINEPHASE_LOCALEVT:
			ceid = CEID_LIST::ONLINELOCAL; // EQUIPMENT LOCAL ONLINE
			break;
		case EVT_LINEPHASE_OFFLINE:
			ceid = CEID_LIST::OFFLINEEQUIPMENT; // EQUIPMENT OFFLINE
			break;
		case EVT_LINEPHASE_HOSTOFF:
			ceid = CEID_LIST::OFFLINEHOST; // 4; // HOST OFFLINE
			break;
		case EVT_LINEPHASE_BCRON:
			ceid = 5; // BCR ON
			break;
		case EVT_LINEPHASE_BCROFF:
			ceid = 6; // BCR OFF
			break;
		case EVT_LINEPHASE_AGVON:
			ceid = 7; // AGV ON
			break;
		case EVT_LINEPHASE_AGVOFF:
			ceid = 8; // AGV OFF
			break;
		default:
			ceid = 0;
			break;
		}
	}
	else if (Evt->eventid == EVT_CMS) {
		BOOL lflag = FALSE;
		CString txt;
		txt.Format("CEID (%d,%d)", Evt->eventid, Evt->make);
		Log(txt, 5);
		switch (Evt->make) {
			// 		case CEID_LIST::LP1LOADCOMPLETE:
			// 		case CEID_LIST::LP2LOADCOMPLETE:
			// 			{
			// 			CEvt *pEvt = new CEvt;
			// 			ASSERT(pEvt);
			// 			if (Evt->PortNo == 1)
			// 			{
			// 				ceid = CEID_LIST::LP1LOADCOMPLETE;
			// 				pEvt->Set(EVT_CMS, CEID_LIST::LP1WAITINGFORHOSTCID, Evt->PortNo);
			// 			}
			// 			else if (Evt->PortNo == 2)
			// 			{
			// 				ceid = CEID_LIST::LP2LOADCOMPLETE;
			// 				pEvt->Set(EVT_CMS, CEID_LIST::LP2WAITINGFORHOSTCID, Evt->PortNo);
			// 			}
			// 			m_gEvtQue.OpenMemory("EVTMSG");
			// 			m_gEvtQue.WriteQue(pEvt);
			// 			m_gEvtQue.CloseMemory();
			// 			}
			// 			break;
		case CEID_LIST::LP1WAFERREMOVED:
		case CEID_LIST::LP2WAFERREMOVED:
			switch (Evt->PortNo) {
			case 1:
				ceid = CEID_LIST::LP1WAFERREMOVED;
				break;
			case 2:
				ceid = CEID_LIST::LP2WAFERREMOVED;
				break;
			default:
				ASSERT(0);
				break;
			}
			pGDev->pGEMObjs->Cms[Evt->PortNo - 1].CaObj.Substrate[Evt->slotno - 1].SetState(CSubstObj::ATWORK);
			pGDev->pGEMObjs->Cms[Evt->PortNo - 1].CaObj.Substrate[Evt->slotno - 1].SetProcState(CSubstObj::IN_PROCESS);
			break;
		case CEID_LIST::LP1WAFERRETURNED:
		case CEID_LIST::LP2WAFERRETURNED:
			switch (Evt->PortNo) {
			case 1:
				ceid = CEID_LIST::LP1WAFERRETURNED;
				break;
			case 2:
				ceid = CEID_LIST::LP2WAFERRETURNED;
				break;
			default:
				ASSERT(0);
				break;
			}
			// The Substrate processed
			pGDev->pGEMObjs->Cms[Evt->PortNo - 1].CaObj.Substrate[Evt->slotno - 1].SetState(CSubstObj::ATDESTINATION);
			pGDev->pGEMObjs->Cms[Evt->PortNo - 1].CaObj.Substrate[Evt->slotno - 1].SetProcState(CSubstObj::PROCESSED);
			break;
		case CEID_LIST::LP1CANCELCARRIEROK:
		case CEID_LIST::LP2CANCELCARRIEROK:
			switch (Evt->PortNo) {
			case 1:
				ceid = CEID_LIST::LP1CANCELCARRIEROK; // CARRIER CANCEL
				break;
			case 2:
				ceid = CEID_LIST::LP2CANCELCARRIEROK; // CARRIER CANCEL
				break;
			default:
				ASSERT(0);
				break;
			}
			{
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				pEvt->Set(2, EVT_CMSPHASE_UNLOADREQUEST, Evt->PortNo);
				m_gEvtQue.OpenMemory("EVTMSG");
				m_gEvtQue.WriteQue(pEvt);
				m_gEvtQue.CloseMemory();
			}
			break;
		default:
			ceid = Evt->make;
			break;
		}
	}
	// Yuen: Obsoleted [2002/10/13  1:56]
	else if (Evt->eventid == EVT_CJM) {
		ASSERT(0);
	}

	// 	else if (Evt->eventid == EVT_PJM)
	// 	{
	// 		switch (Evt->make)
	// 		{
	// 		case EVT_PJMCREATECOMPLETE:
	// 			ASSERT(0);
	// 			ceid = 100;
	// 			break;
	// 		case EVT_PJMSTARTEDSETUP:
	// 			{
	// 			ASSERT(0);
	// 			ceid = 101;
	// 			CEvt *pEvt = new CEvt;
	// 			ASSERT(pEvt);
	// 			pEvt->Set(EVT_PJM, EVT_PJMSTARTEDPROCESSING);
	// 			m_gEvtQue.OpenMemory("EVTMSG");
	// 			m_gEvtQue.WriteQue(pEvt);
	// 			m_gEvtQue.CloseMemory();
	// 			}
	// 			break;
	// 		case EVT_PJMSTARTEDPROCESSING:
	// 			ASSERT(0);
	// 			ceid = 102;
	// 			break;
	// 		case EVT_PJMCPROCESSCOMPLETE:
	// 			ASSERT(0);
	// 			ceid = 103;
	// 			break;
	// 		default:
	// 			ceid = Evt->make;
	// 			break;
	// 		}
	// 	}
	else if (Evt->eventid == EVT_ALARM) {
		CEIDCtlr.m_alarm.GetLastAlarmEvt(ceid);
	}
	else if (Evt->eventid == EVT_ETS) {
		ASSERT(0);
		// Get CEID from AlrmList
		switch (Evt->make) {
		case EVT_MSG_RECOGNIZED:
			ceid = 3001; // CEID for
			break;
		}
	}
	else if (EVT_SPOOL == Evt->eventid) {
		switch (Evt->make) {
		case EVT_SPOOL_ACTIVATED:
			ceid = CEID_LIST::SPOOLACTIVATED;
			break;
		case EVT_SPOOL_DEACTIVATED:
			ceid = CEID_LIST::SPOOLDEACTIVATED;
			break;
		}
	}
	else if (EVT_OTHER == Evt->eventid) {
		switch (Evt->make) {
		case EVT_RCP_CHANGE:
			ceid = CEID_LIST::PROCESSPROGRAMCHANGED;
			break;
		case EVT_RCP_SELECTED:
			ceid = CEID_LIST::PROCESSPROGRAMSELECTED;
			break;
		}
	}
	else {
		ceid = Evt->make;
	}

	ceid = CEIDCtlr.m_CEList.GetID(ceid); // Convert to real CEID [8/4/2010 XPMUser]

	if (!ceid) {
		return retc;
	}

	if (!CEIDCtlr.check_rptflag(ceid)) {
		EventNote += "(Disabled)";
		LOG_INFO(EventNote);
		return 0;
	}

	char Msg[126] = { 0 };
	EventNote = "Event ";
	EventNote += "  ";
	EventNote += _itoa(ceid, Buf, 10);
	string Desc;
	::CEIDCtlr.CeidDesc(ceid, Desc);
	EventNote += " (";
	EventNote += Desc;
	EventNote += " ) \n\t";
	SecsMessage OutMsg;
	SecsItemList* pList = new SecsItemList;
	pList->AddItem(new SecsItemUInt4(dataid));
	pList->AddItem(new SecsItemUInt4(ceid));
	if (Evt->pReport) {
		pList->AddItem(Evt->pReport);
		Evt->pReport = NULL;
	}
	else {
		pList->AddItem(new SecsItemList);
	}
	OutMsg.AddItem(pList);
	UINT Len = OutMsg.BinaryMessageLength();
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(/*1*/0, 6, 11, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
	// 	if( CEIDCtlr.check_rptflag(ceid) )
	// 	{
	SendBlock(shead);
	EventNote += "(Enabled)";
	// 	}
	// 	else
	// 	{
	// 		retc = 0;
	// 		EventNote += "(Disabled)";
	// 	}
	LOG_INFO(EventNote);
	return retc;
}

int CGemFactory::evt_alarm(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S5F1(shead, event);
	}
	else if (phase == 1) {
	}
	return ret;
}

int CGemFactory::mk_S5F1(GEM_HEADER* pHeader, CEvt* ehead) {
	LOG_INFO("Send: S5F1 Alarm report send ARS");
	int retc = 0;
	BYTE data = (BYTE)ehead->alcd;;
	BOOL bSet = (data >> 7) ? TRUE : FALSE;

	//  [9/16/2007 LYF]
	if (!CEIDCtlr.m_alarm.IsAlarm(ehead->alid)) {
		return retc;
	}

	CString alarmMsg;
	alarmMsg = CEIDCtlr.m_alarm.GetAlarmText(ehead->alid); //  [9/16/2007 LYF]
	if (TRUE == CEIDCtlr.m_alarm.IsAlarmEnabled(ehead->alid)) //  [9/16/2007 LYF]
	{
		SecsMessage OutMsg;
		SecsItemList* pRes = new SecsItemList;
		pRes->AddItem(new SecsItemBinary(&data, 1));
		pRes->AddItem(new SecsItemUInt4(ehead->alid));
		pRes->AddItem(new SecsItemASCII(alarmMsg.GetBuffer(0)));
		OutMsg.AddItem(pRes);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(1, 5, 1, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
		SendBlock(pHeader);
	}
	CSharedMemQue pQue;
	if (bSet) {
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_EXTRA, CEID_LIST::ALARMSET);
		pQue.OpenMemory("EVTMSG");
		pQue.WriteQue(pEvt);
		pQue.CloseMemory();
	}
	else {
		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_EXTRA, CEID_LIST::ALARMCLEARED);
		pQue.OpenMemory("EVTMSG");
		pQue.WriteQue(pEvt);
		pQue.CloseMemory();
	}
	return retc;
}

int CGemFactory::mk_S2F18(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F17) Date and time request DTR");
	MessageS2F17 InMsg;
	int retc = 0;
	if (!InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		mk_S9F7(pHeader);
		return retc;
	}
	string Time;
	GetCurrTime(Time);
	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemASCII(Time));
	OutMsg.MakeBinaryMessage(pHeader->m_Message);
	retc = set_head(0, 2, 18, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
	SendBlock(pHeader);
	return retc;
}

int CGemFactory::mk_S2F22(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F21) Remote command send RCS");
	MessageS2F21 InMsg;
	int retc = 0;
	if (!InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		mk_S9F7(pHeader);
		return retc;
	}
	int RCmd = InMsg.GetRemoteCmd();
	// Process remote command [8/25/2010 XPMUser]
	// svINT1,  0 = completed, 1 = command not valid, 2 = cannot perform, >2 other equipment error [8/25/2010 XPMUser]

	if ((RCmd <= SVID_LIST::REMOTEFIRSTCMD) || (RCmd >= SVID_LIST::REMOTELASTCMD)) {
		retc = 1;
	}
	else {
		// Find a way to send this remote command [8/25/2010 XPMUser]
		switch (RCmd) {
		case SVID_LIST::REMOTERELEASEPORT1:
			pUDev->UnclampCarrier(1);
			break;
		case SVID_LIST::REMOTERELEASEPORT2:
			pUDev->UnclampCarrier(2);
			break;
		}
		retc = 0;
	}

	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemUInt1(retc));
	OutMsg.MakeBinaryMessage(pHeader->m_Message);
	retc = set_head(0, 2, 22, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
	SendBlock(pHeader);
	return retc;
}

int CGemFactory::mk_S2F26(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F25) Loopback diagnostic request LDR");
	int retc = 0;
	MessageS2F25 InMsg;
	if (!InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		mk_S9F7(pHeader);
		return retc;
	}
	InMsg.MakeBinaryMessage(pHeader->m_Message);
	retc = set_head(0, 2, 26, pHeader, InMsg.BinaryMessageLength() + 10, PRI_HOST);
	SendBlock(pHeader);
	return retc;
}

int CGemFactory::evt_timetracer(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F1(shead, event);
	}
	else if (phase == 1) {
		int dwStream = shead->m_Stream & 0x7f;
		int dwFunc = shead->m_Function;
		if (dwStream == 6 && dwFunc == 2) {
			retc = 1;
		}
	}
	return retc;
}

int CGemFactory::mk_S6F1(GEM_HEADER* shead, CEvt* ehead) {
	LOG_INFO("Send: S6F1 Trace data send");

	int retc = 0;
	CTraceTracker* pTracker;
	POSITION pos;
	UINT nSmpln = 0, * svid;
	int svidcnt = 0;
	char stime[17];
	COleDateTime nOleTime;
	vector < SVID_VALUE > dSvval;
	int i = 0;
	SecsMessage OutMsg;
	if (ehead->no == 0) {
		pos = m_pTraceList.GetHeadPosition();
		if (pos) {
			pTracker = (CTraceTracker*)m_pTraceList.GetHead();
			while (pos) {
				if (pTracker->m_trid == (UINT)ehead->make) {
					pTracker->m_svid.RemoveAll();
					m_pTraceList.RemoveAt(pos);
					break;
				}
				m_pTraceList.GetNext(pos);
				if (pos) {
					pTracker = (CTraceTracker*)m_pTraceList.GetAt(pos);
				}
			}
		}
		retc = 0;
	}
	else {
		pos = m_pTraceList.GetHeadPosition();
		if (pos) {
			pTracker = (CTraceTracker*)m_pTraceList.GetHead();
			while (pos) {
				if (pTracker->m_trid == (UINT)ehead->make) {
					nSmpln = pTracker->m_Smpln;
					pTracker->m_Smpln++;
					svidcnt = pTracker->m_svid.GetSize();
					svid = new UINT[svidcnt];
					for (i = 0; i < svidcnt; i++) {
						svid[i] = pTracker->m_svid.GetAt(i);
					}
					break;
				}
				m_pTraceList.GetNext(pos);
				if (pos) {
					pTracker = (CTraceTracker*)m_pTraceList.GetAt(pos);
				}
			}
		}
		else {
			return 0;
		}
		SecsItemList* pTopList = new SecsItemList;
		pTopList->AddItem(new SecsItemUInt4(ehead->make));
		pTopList->AddItem(new SecsItemUInt4(nSmpln));
		memset(stime, 0, sizeof stime);
		nOleTime = COleDateTime::GetCurrentTime();
		sprintf(stime, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d00", nOleTime.GetYear(), nOleTime.GetMonth(), nOleTime.GetDay(), nOleTime.GetHour(),
			nOleTime.GetMinute(), nOleTime.GetSecond());
		pTopList->AddItem(new SecsItemASCII(stime));
		SecsItemList* pSVList = new SecsItemList;
		int nIndex = 0;
		for (i = 0; i < svidcnt; i++) {
			pSVList->AddItem(get_svval(1, svid[i], NULL));
		}
		delete[] svid;
		pTopList->AddItem(pSVList);
		OutMsg.AddItem(pTopList);
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(1, 6, 1, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
		SendBlock(shead);
	}
	return retc;
}

BOOL CGemFactory::Check_Trid(UINT trid) {
	BOOL bRet = FALSE;
	CTraceTracker* tracker;
	if (!m_pTraceList.GetCount()) {
		return FALSE;
	}
	POSITION pos = m_pTraceList.GetHeadPosition();
	tracker = (CTraceTracker*)m_pTraceList.GetHead();
	while (pos) {
		if (tracker->m_trid == trid) {
			return TRUE;
		}
		else {
			m_pTraceList.GetNext(pos);
			if (pos) {
				tracker = (CTraceTracker*)m_pTraceList.GetAt(pos);
			}
		}
	}
	return bRet;
}

BOOL CGemFactory::DeleteTrid(UINT trid) {
	BOOL bRet = FALSE;
	CTraceTracker* tracker;
	if (!m_pTraceList.GetCount()) {
		return FALSE;
	}
	POSITION pos = m_pTraceList.GetHeadPosition();
	tracker = (CTraceTracker*)m_pTraceList.GetHead();
	while (pos) {
		if (tracker->m_trid == trid) {
			m_pTraceList.RemoveAt(pos);
			break;
		}
		else {
			m_pTraceList.GetNext(pos);
			if (pos) {
				tracker = (CTraceTracker*)m_pTraceList.GetAt(pos);
			}
		}
	}
	return bRet;
}

int CGemFactory::Stream5(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(5, dwFunction)) {
		mk_S05F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 3: // (EAS) Enable Disable Alarm
		mk_S5F4(pHeader);
		break;
	case 5: // (LAR) List alarm request
		mk_S5F6(pHeader);
		break;
	case 7: // (LEAR) List Enabled Alarm Request
		mk_S5F8(pHeader);
		break;
	default:
		mk_S9F5(pHeader);
		break;
	}
	return 0;
}

int CGemFactory::Stream6(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(6, dwFunction)) {
		mk_S06F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 5:
		mk_S6F6(pHeader);
		break;
	case 15:
		mk_S6F16(pHeader);
		break;
	case 19:
		mk_S6F20(pHeader);
		break;
	case 23:
		mk_S6F24(pHeader);
		break;
	default:
	{
		char Msg[126];
		sprintf(Msg, "(S6F%d) Unknown Function", pHeader->m_Function);
		LOG_WARN(Msg);
		mk_S9F5(pHeader);
	}
	break;
	}
	return 0;
}

int CGemFactory::mk_S5F8(GEM_HEADER* pHeader) //
{
	LOG_INFO("(S5F7) List enabled alarm request LEAR");

	int retc = 0;
	MessageS5F7 InMsg;
	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		vector < UINT > vecALID;
		vector < CString > vecALTX;
		//AlarmData alarmData; //  [9/16/2007 LYF]
		//pGDev->m_AlarmArry.GetAlarmsEnabled(vecALID); //  [9/16/2007 LYF]
		CEIDCtlr.m_alarm.GetAlarmEnabledID(vecALID, vecALTX); //  [9/16/2007 LYF]

		SecsMessage OutMsg;
		SecsItemList* pRes = new SecsItemList;
		for (UINT nIndex = 0; nIndex < vecALID.size(); nIndex++) {
			CAlrmItem* pA = CEIDCtlr.m_alarm.GetAlarm(vecALID[nIndex]); //  [9/16/2007 LYF]
			//pGDev->m_AlarmArry.GetAlarmData(vecALID[nIndex], alarmData); //  [9/16/2007 LYF]
			SecsItemList* AlrmDat = new SecsItemList;
			//AlrmDat->AddItem(new SecsItemBinary(& alarmData.ALCD,1)); //  [9/16/2007 LYF]
			//AlrmDat->AddItem(new SecsItemUInt4(alarmData.ALID));  //  [9/16/2007 LYF]
			//AlrmDat->AddItem(new SecsItemASCII(alarmData.ALTX));  //  [9/16/2007 LYF]
			//  [9/16/2007 LYF]
			BYTE alCD = pA->alCD;
			AlrmDat->AddItem(new SecsItemBinary(&alCD, 1));
			AlrmDat->AddItem(new SecsItemUInt4(pA->alID));
			AlrmDat->AddItem(new SecsItemASCII(pA->alTX));
			pRes->AddItem(AlrmDat);
		}
		OutMsg.AddItem(pRes);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 5, 8, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S5F7) Incorrect format ");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S2F14(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F13) Equipment constant data ECD");

	int retc = 0;
	vector < UINT > Ecids;
	MessageS2F13 Msg;
	SecsMessage OutMsg;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetEcidList(Ecids);
		GetEcData(Ecids, OutMsg);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 2, 14, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S2F13) Incorrect format ");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S2F16(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F15) New equipment constant send ECS");
	int retc = 0;
	BYTE data = 0;
	BOOL flag = FALSE;
	vector < EcidData > EcVals;
	int nIndex;
	MessageS2F15 Msg;
	SecsMessage OutMsg;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetNewECVal(EcVals);
		for (nIndex = 0; nIndex < int(EcVals.size()); nIndex++) {
			data = check_ecid(EcVals[nIndex].ECID, EcVals[nIndex].EcVal.c_str());
			if (0 != data) {
				flag = TRUE;
				break;
			}
		}
		if (!flag) {
			// Every thing OK !
			for (nIndex = 0; nIndex < int(EcVals.size()); nIndex++) {
				set_ecval(EcVals[nIndex].ECID, EcVals[nIndex].EcVal.c_str());
			}
		}
		OutMsg.AddItem(new SecsItemBinary(&data, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 2, 16, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S2F15) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S2F30(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F29) Equipment constant namelist request ECNR");
	int retc = 0;
	double scmin = 9.99999E-99, scmax = 9.99999E+99;
	MessageS2F29 Msg;
	SecsMessage OutMsg;
	vector < UINT > EcidList;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetEcids(EcidList);
		if (0 == EcidList.size()) {
			SVID_LIST* p = &CEIDCtlr.m_SVList;
			for (int i = 0; i < SVID_LIST::LASTSVID; i++) {
				if (p->m_Svid[i].m_bVarType == SVID_ITEM::EC_TYPE) {
					EcidList.push_back(p->m_Svid[i].m_svidno);
				}
			}
			//for (int iEcid = 0; iEcid < (sizeof(AllECIDs)) / (sizeof(AllECIDs[0])); iEcid++)
			//{
			//	EcidList.push_back(AllECIDs[iEcid].ID);
			//}
		}
		GetEcnrData(EcidList, OutMsg);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 2, 30, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S2F29) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

//TODO: When set value, must check for max and min limit YFL [4/20/2003]
BOOL CGemFactory::set_ecval(UINT ecid, const char* pVal) {
	if (!CEIDCtlr.m_SVList.IsECID(ecid)) {
		return FALSE;
	}

	BOOL bRet = TRUE;
	CString str;
	int portno = 0;
	UINT Val = AtoUint((BYTE*)pVal);
	USHORT sVal = AtoUshort((BYTE*)pVal);
	BYTE bVal = pVal[0];
	switch (CEIDCtlr.m_SVList.GetIndex(ecid)) {
	case SVID_LIST::DEVICEID:
		m_DeviceId = atoi(pVal);
		str.Format("%s", pVal);
		set_Sparameter(8, str);
		break;
	case SVID_LIST::SOFTREV:
		// Read only
		bRet = FALSE;
		break;
	case SVID_LIST::CONNECTIONMODE:	// Connection Mode
		m_ConnectionMode = pVal[0];
		str.Format("%d", m_ConnectionMode);
		set_Sparameter(9, str);
		break;
		// 	case SVID_LIST::PASSIVEADDRESS:	// Passive IP Address
		// 		SetPassiveIPaddress(pVal);
		// 		break;
	case SVID_LIST::ACTIVEIPADDRESS:	// Active IP Address
		sprintf(m_cActIP, "%s", pVal);
		set_Sparameter(11, m_cActIP);
		break;
	case SVID_LIST::PASSIVESOCKETPORT:	// Passive Socket Port No
		portno = atoi(pVal);
		m_SocketPortNo = portno;
		str.Format("%d", portno);
		set_Sparameter(10, str);
		break;
	case SVID_LIST::ACTIVESOCKETPORT:	// Active Socket Port No
		portno = atoi(pVal);
		m_ActPort = portno;
		str.Format("%d", portno);
		set_Sparameter(12, str);
		break;
	case SVID_LIST::SPOOLCOUNTTOTAL: // MaxSpoolTransmit
		Val = atoi(pVal);
		CSpoolMan::GetSpool()->MaxSpoolTransmit = atoi(pVal);
		// Update and write to the File
		break;
	case SVID_LIST::SPOOLOVERIDE: // OverWriteSpool
		CSpoolMan::GetSpool()->OverWriteSpool = (atoi(pVal) == 0) ? FALSE : TRUE;
		break;
	case SVID_LIST::ESTABLISHCOMMTIMEOUT: // EstablishCommunicationTimeout
		EstablishCommunicationTimeout = atoi(pVal);
		break;
	case SVID_LIST::TIMEFORMAT: // TimeFormat
	{
		Val = atoi(pVal);
		if (0 == Val) {
			TimeFormat = 0;
		}
		else if (1 == (Val)) {
			TimeFormat = 1;
		} // What if any other value is received ??
		else bRet = FALSE;
	}
	break;
	// 	case SECID::ECID_BYPASSREADID_LP1:
	// 		if (_strcmp(pVal, "true") == 0)
	// 		{
	// 			pGDev->pGEMObjs->Cms[0].m_bBypassReadID = TRUE;
	// 		}
	// 		else if (_strcmp(pVal, "false") == 0)
	// 		{
	// 			pGDev->pGEMObjs->Cms[0].m_bBypassReadID = FALSE;
	// 		}
	// 		break;
	// 	case SECID::ECID_BYPASSREADID_LP2:
	// 		if (_strcmp(pVal, "true") == 0)
	// 		{
	// 			pGDev->pGEMObjs->Cms[1].m_bBypassReadID = TRUE;
	// 		}
	// 		else if (_strcmp(pVal, "false") == 0)
	// 		{
	// 			pGDev->pGEMObjs->Cms[1].m_bBypassReadID = FALSE;
	// 		}
	// 		break;
	// 	case SECID::ECID_SETUP_NAME_E94:
	// 		::SetupName = (USHORT)atoi(pVal);
	// 		break;
	case SVID_LIST::DEFAULTONLINESTATE:
	{
		switch (atoi(pVal)) {
		default:
			m_DefOnlineState = CHsmsParaMeter::DEF_LOCAL;
			break;
		case CHsmsParaMeter::DEF_LOCAL:
		case CHsmsParaMeter::DEF_REMOTE:
			m_DefOnlineState = (CHsmsParaMeter::DEFONLINE)atoi(pVal);
			break;
		}
	}
	break;
	case SVID_LIST::DEFAULTOFFLINESTATE:
		switch (atoi(pVal)) {
		default:
			m_DefOfflineState = CHsmsParaMeter::DEF_HOST_OFF;
			break;
		case CHsmsParaMeter::DEF_HOST_OFF:
		case CHsmsParaMeter::DEF_EQP_OFF:
			m_DefOfflineState = (CHsmsParaMeter::DEFOFFLINE)atoi(pVal);
			break;
		}
		break;
	}
	return bRet;
}

short CGemFactory::check_ecid(UINT ecid, const char* pVal) {
	short ret = 0;
	int nVal = 0;
	UINT uVal = 0;
	switch (CEIDCtlr.m_SVList.GetIndex(ecid)) {
	case SVID_LIST::TIMEFORMAT:		// TimeFormat
		uVal = atoi(pVal);
		if ((uVal == 0) || (uVal == 1)) {
			ret = 0;
		}
		else {
			ret = 3;
		}
		break;
	case SVID_LIST::DEFAULTOFFLINESTATE: // Fallthrough
	case SVID_LIST::DEFAULTONLINESTATE:
		uVal = atoi(pVal);
		if ((uVal != 0) && (uVal != 1)) {
			ret = 3;
		}
		break;
	default:
		if (CEIDCtlr.m_SVList.IsECID(ecid)) {
			ret = 0;
		}
		else {
			ret = 1;
		}
		break;
	}
	return ret;
}

// int CGemFactory::get_svidmode(UINT svid, SVID_VALUE * pVal)
// {
// 	return CEIDCtlr.get_svidmode(svid,pVal->m_TYPE,pVal->m_DataSize);
// }

int CGemFactory::evt_remote_stop(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_remote_pause(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_remote_resume(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_remote_abort(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_remote_cancel(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_dockingcomplete(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_foupopen(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_foupopenfail(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_carrierunclamped(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_unloadcomplete(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	CEvt evt;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_carriercancel(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	CEvt evt;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::mk_S16F12(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F11) PRJobCreateEnh");

	char MsgStr[126];
	CString JID;
	int retc = 0;
	BYTE caack = 0;
	vector < UINT > ErrCodes;
	string LogMsg = "(PRJobCreateEnh) ";
	MessageS16F11 Msg;
	CPRJob* pJob = NULL;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		if ((Msg.PrJob.PrJobID.empty()) ||
			(Msg.PrJob.CarrierID.empty()) ||
			(Msg.PrJob.RecipeID.empty())) {
			ErrCodes.push_back(UNKNOWN_OBJECT_IN_OBJECT_SPECIFIER);
			sprintf(MsgStr, "Null Object identifier  specified (PrID:%s,CarrierID:%s,RcpID:%s) ",
				Msg.PrJob.PrJobID.c_str(), Msg.PrJob.CarrierID.c_str(), Msg.PrJob.RecipeID.c_str());
			LogMsg += MsgStr;
			caack = 1;
		}
		else if (PJobMgr.GetJobSpace()) {
			// Job space available
			if (!PJobMgr.IsPRJobExist(Msg.PrJob.PrJobID.c_str())) {
				// Matching SlotMap and WaferParams YFL [3/22/2003]
				// SlotMap takes precedence YFL [3/22/2003]
				if (Msg.PrJob.RcpMethd != 2) {
					ErrCodes.push_back(RECIPE_METHOD_NOT_2);
					LogMsg += "Recipe method not 2";
					pJob = NULL;
					caack = 1;
				}
				else if ((Msg.PrJob.MF < 13) || (Msg.PrJob.MF > 14)) {
					ErrCodes.push_back(ILLEGAL_MF_CODE);
					LogMsg += "Illegal MF code";
					pJob = NULL;
					caack = 1;
				}
				else if (!Msg.PrJob.PrJobID.size()) {
					ErrCodes.push_back(ZERO_LENGTH_PRJOBID);
					LogMsg += "Zero length PRJob ID";
					pJob = NULL;
					caack = 1;
				}
				else if (Msg.PrJob.WaferParams.size()) {
					for (int i = 0; i < int(Msg.PrJob.Slots.size()); i++) {
						BOOL bFound = FALSE;
						vector <ParamNew>::iterator it;
						it = Msg.PrJob.WaferParams.begin();
						while (it != Msg.PrJob.WaferParams.end()) {
							if ((*it).SlotNo == Msg.PrJob.Slots[i]) {
								bFound = TRUE;
								break;
							}
							it++;
						}
						if (!bFound) {
							Msg.PrJob.Slots[i] = 0;
						}
					}
					vector<BYTE>::iterator it;
					it = Msg.PrJob.Slots.begin();
					while (it != Msg.PrJob.Slots.end()) {
						if ((*it) == 0) {
							it = Msg.PrJob.Slots.erase(it);
						}
						else {
							it++;
						}
					}
				}
				if (Msg.PrJob.Slots.size()) {
					for (int i = 0; i < int(Msg.PrJob.WaferParams.size()); i++) {
						BOOL bFound = FALSE;
						vector<BYTE>::iterator it;
						it = Msg.PrJob.Slots.begin();
						while (it != Msg.PrJob.Slots.end()) {
							if ((*it) == Msg.PrJob.WaferParams[i].SlotNo) {
								bFound = TRUE;
								break;
							}
							it++;
						}
						if (!bFound) {
							Msg.PrJob.WaferParams[i].SlotNo = 0;
						}
					}
					vector<ParamNew>::iterator it;
					it = Msg.PrJob.WaferParams.begin();
					while (it != Msg.PrJob.WaferParams.end()) {
						if ((*it).SlotNo == 0) {
							it = Msg.PrJob.WaferParams.erase(it);
						}
						else {
							it++;
						}
					}
				}

				// PRJOBID is not duplicated
				pJob = new CPRJob;
				pJob->SetObjectID((char*)Msg.PrJob.PrJobID.c_str()); // PrjobId
				pJob->SetMtlName(Msg.PrJob.CarrierID.c_str());
				if (Msg.PrJob.CarrierID.size()) {
					CString MsgStr1;
					MsgStr1.Format("Material ID %s not verify", (char*)Msg.PrJob.CarrierID.c_str());
					Log(MsgStr1, 2);
				}
				else {
					CString MsgStr1;
					MsgStr1.Format("PRJob %s: Material ID contains NULL str", (char*)Msg.PrJob.PrJobID.c_str());
					Log(MsgStr1, 2);
				}
				pJob->m_PortNo = pGDev->GetPortID(Msg.PrJob.CarrierID);
				for (int nIndex = 0; nIndex < int(Msg.PrJob.WaferParams.size()); nIndex++) {
					pJob->SetWaferParams(Msg.PrJob.WaferParams[nIndex], nIndex);
				}
				pJob->m_Slots = Msg.PrJob.Slots;
				pJob->SetMF(Msg.PrJob.MF);
				if (Msg.PrJob.MF != 13) {
					caack = 5;
					// Parameters improperly specified @@E-40 1101
					ErrCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
					sprintf(MsgStr, "MF (%d) ", Msg.PrJob.MF);
					LogMsg += MsgStr;
				}
				pJob->SetPRStart(Msg.PrJob.bAutoStart);
				// 				pJob->SetNumberScan(Msg.PrJob.NumScanLines);
				// 				if (Msg.PrJob.NumScanLines < 1 || Msg.PrJob.NumScanLines > 100)
				// 				{
				// 				}
				// 				pJob->SetScanNo(Msg.PrJob.ScanNo);
				// 				if (Msg.PrJob.ScanNo < 1)
				// 				{
				// 				}
				pJob->SetMtlType(1);
				if (Msg.PrJob.RcpMethd != 2) {
					caack = 1;
					// Parameters improperly specified @@E-40 1101
					ErrCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
					sprintf(MsgStr, "Err :RCPMethod (%d) ", Msg.PrJob.RcpMethd);
					LogMsg += MsgStr;
				}
				// Pause events are not supported NOW ! Just keep track
				pJob->m_PauseEvents = Msg.PrJob.PauseEvts;
				// Recipe Support @@Vijay#15May$02
				pJob->m_Recipe = Msg.PrJob.RecipeID;
				if (!RcpMgr.IsRecipeAvailable(pJob->m_Recipe)) {
					caack = 16;
					ErrCodes.push_back(RECIPE_NOT_FOUND);
					sprintf(MsgStr, "(S16F15) Recipe (%s) Not on Equipment", pJob->m_Recipe.c_str());
					LOG_WARN(MsgStr);
				}
			}
			else {
				caack = 2; // Object Identifier in use @@E-40 1101
				ErrCodes.push_back(OBJECT_IDENTIFIER_IN_USE);
				sprintf(MsgStr, "Duplicate PrjobID (%s) ", Msg.PrJob.PrJobID.c_str());
				LogMsg += MsgStr;
			}
		}
		else {
			caack = 1;
			ErrCodes.push_back(BUSY_NOT_QUEUE_OR_QUEUE_FULL);
			sprintf(MsgStr, "PrJobSpace FULL ");
			LogMsg += MsgStr;
		}
		SecsMessage OutMsg;
		SecsItemList* pList = new SecsItemList;
		SecsItemList* pRes = new SecsItemList;
		pList->AddItem(new SecsItemASCII(Msg.PrJob.PrJobID));
		pRes->AddItem(new SecsItemBoolean((ErrCodes.size() ? FALSE : TRUE)));
		SecsItemList* pBot = NULL, * pErrs = new SecsItemList;
		string ErrText;
		for (int iError = 0; iError < int(ErrCodes.size()); iError++) {
			pBot = new SecsItemList;
			pBot->AddItem(new SecsItemUInt1(ErrCodes[iError]));
			ErrorMessages::GetErrTxtE5(ErrCodes[iError], ErrText);
			pBot->AddItem(new SecsItemASCII(ErrText));
			pErrs->AddItem(pBot);
		}
		pRes->AddItem(pErrs);
		pList->AddItem(pRes);
		OutMsg.AddItem(pList);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 12, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		if (!caack) {
			if (PJobMgr.Create(pJob) != CPRJobMgr::OK) {
				ASSERT(0);
			}
			sprintf(MsgStr, "Suceess (%s) ", Msg.PrJob.PrJobID.c_str());
			LogMsg += MsgStr;
			LOG_INFO(LogMsg);
		}
		else {
			delete pJob;
			pJob = NULL;
			sprintf(MsgStr, " Fail (%s) ", Msg.PrJob.PrJobID.c_str());
			LogMsg += MsgStr;
			LOG_ERR(LogMsg);
		}
		return retc;
	}
	LOG_ERR("(S16F11) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S16F22(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F21) PRGetSpace");
	int retc = 0;
	SecsMessage OutMsg;
	MessageS16F21 InMsg;
	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		OutMsg.AddItem(new SecsItemUInt2(PJobMgr.GetJobSpace()));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 22, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S16F21) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

// VJTD Wafer thickness is not necessary, please make it an option.
// @@Vijay this function implementation is cahanged to accomodate Parameters through this function
int CGemFactory::mk_S16F6(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F5) Process job command request PRJCMDR");

	int retc = 10, i = 0;
	BYTE ackc = 0;

	typedef interface {
		char* cmd;
		short evtid;
		short make;
		CPRJobMgr::PRACTION act;
	}
	PRJOBCMDLIST;
	PRJOBCMDLIST m_gPRjobComList[6] =
	{
		{
			"START", 1000, 200, CPRJobMgr::START
		},
		{
			"STOP", 1000, 201, CPRJobMgr::STOP
		},
		{
			"PAUSE", 1000, 202, CPRJobMgr::PAUSE
		},
		{
			"RESUME", 1000, 203, CPRJobMgr::RESUME
		},
		{
			"ABORT", 1000, 204, CPRJobMgr::ABORT
		},
		{
			"CANCEL", 1000, 205, CPRJobMgr::CANCEL
		}
	};
	string Command, PrjobId;
	MessageS16F5 Msg;
	string LogStr = "(PRJobCommand) ";
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetCommand(Command);
		Msg.GetPrJobId(PrjobId);
		int nDx;
		for (nDx = 0; nDx < (sizeof(m_gPRjobComList) / sizeof(m_gPRjobComList[0])); nDx++) {
			if (0 == Command.compare(m_gPRjobComList[nDx].cmd)) {
				break;
			}
		}
		// First check if the action is OK to perform ..
		// .... Check the parameters
		// ... Send the Acknowlwdge to the host & then
		// .... if OK perform the action .....

		/** Step 1 : Check if the action can be performed */
		switch (m_gPRjobComList[nDx].act) {
		case CPRJobMgr::START:
			ackc = PJobMgr.CanStart((char*)PrjobId.c_str());
			LogStr += ": Start) ";
			break;
		case CPRJobMgr::STOP:
			ackc = PJobMgr.CanStop((char*)PrjobId.c_str());
			LogStr += ": Stop) ";
			break;
		case CPRJobMgr::PAUSE:
			ackc = PJobMgr.CanPause((char*)PrjobId.c_str());
			LogStr += ": Pause) ";
			break;
		case CPRJobMgr::RESUME:
			ackc = PJobMgr.CanResume((char*)PrjobId.c_str());
			LogStr += ": Resume) ";
			break;
		case CPRJobMgr::ABORT:
			ackc = PJobMgr.CanAbort((char*)PrjobId.c_str());
			LogStr += ": Abort) ";
			break;
		case CPRJobMgr::CANCEL:
			ackc = PJobMgr.CanCancel((char*)PrjobId.c_str());
			LogStr += ": Cancel) ";
			break;
		default:
			ackc = 5;
			break;
		};

		/** Step 2 : Send appropriate acknowledge to host */
		SecsMessage OutMsg;
		SecsItemList* pTop = new SecsItemList;
		SecsItemList* pList = new SecsItemList;
		SecsItemList* pErrList = new SecsItemList;
		pTop->AddItem(new SecsItemASCII(PrjobId));
		pList->AddItem(new SecsItemBoolean(ackc == 0));
		pList->AddItem(pErrList);
		pTop->AddItem(pList);
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 6, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);

		/** Step 3 : If OK perform the action */
		if (0 == ackc) {
			switch (m_gPRjobComList[nDx].act) {
			case CPRJobMgr::START:
				PJobMgr.WFS2Start((char*)PrjobId.c_str());
				break;
			case CPRJobMgr::STOP:
				PJobMgr.Stop((char*)PrjobId.c_str());
				break;
			case CPRJobMgr::PAUSE:
				PJobMgr.Pause((char*)PrjobId.c_str());
				break;
			case CPRJobMgr::RESUME:
				PJobMgr.Resume((char*)PrjobId.c_str());
				break;
			case CPRJobMgr::ABORT:
				PJobMgr.Abort((char*)PrjobId.c_str());
				break;
			case CPRJobMgr::CANCEL:
				PJobMgr.Cancel((char*)PrjobId.c_str());
				break;
			default:
				ASSERT(0);
				break;
			};
			LogStr += " Success ";
			LOG_INFO(LogStr);
		}
		else {
			char Temp[52];
			sprintf(Temp, " Err(%d) Fail", ackc);
			LogStr += Temp;
			LOG_ERR(LogStr);
		}
		return retc;
	}
	LOG_ERR("(S16F5) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S16F20(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F19) PRGetAllJobs");

	int retc = 0;
	MessageS16F19 InMsg;

	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		CStringArray JList;
		PJobMgr.GetPRJobList(JList);
		SecsItemList* pPjList = new SecsItemList;
		for (int i = 0; i < JList.GetSize(); i++) {
			CString JobName = JList.GetAt(i);
			CPRJob* p = PJobMgr.GetPRJob(JobName);
			if (p) {
				SecsItemList* pPRj = new SecsItemList;
				pPRj->AddItem(new SecsItemASCII(p->m_ObjID));
				pPRj->AddItem(new SecsItemUInt1(p->GetState()));
				pPjList->AddItem(pPRj);
			}
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(pPjList);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 20, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S16F18(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F17) PRJobDequeue");

	int nIndex;
	int retc = 0;
	CPRJob* pJob = NULL;
	vector < string > PrjIDs;
	SecsMessage OutMsg;
	MessageS16F17 Msg;
	string LogMsg = "(PRJobDequeue) ";
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		SecsItemList* pTopList = new SecsItemList;
		SecsItemList* pPrIDList = new SecsItemList;
		SecsItemList* pResults = new SecsItemList;
		SecsItemList* pErrors = new SecsItemList;
		Msg.GetJobList(PrjIDs);
		if (0 == PrjIDs.size()) {
			// Delete all PrJobIds
			LogMsg += "(Dequeue All : ";
			CStringArray JList;
			PJobMgr.GetPRJobList(JList);
			for (int i = 0; i < JList.GetSize(); i++) {
				string Temp;
				Temp = JList.GetAt(i);
				LogMsg += Temp;
				CString JobName = JList.GetAt(i);
				if (PJobMgr.Cancel(JobName.GetBuffer(0)) == CPRJobMgr::OK) {
					LogMsg += "<Success> ";
					pPrIDList->AddItem(new SecsItemASCII(Temp));
				}
				else {
					LogMsg += "<Fail> ";
					pErrors->AddItem(new SecsItemASCII(Temp));
				}
				LogMsg += ", ";
			}
			LogMsg += " )";
			// Formulate the response
			pResults->AddItem(new SecsItemBoolean((pErrors->NumContainedItems() > 0) ? FALSE : TRUE));
			pResults->AddItem(pErrors);
			pTopList->AddItem(pPrIDList);
			pTopList->AddItem(pResults);
		}
		else {
			LogMsg += "(Dequeue : ";
			// Deque available jobs
			for (nIndex = 0; nIndex < int(PrjIDs.size()); nIndex++) {
				LogMsg += PrjIDs[nIndex];
				if (PJobMgr.Cancel((char*)PrjIDs[nIndex].c_str()) == CPRJobMgr::OK) {
					LogMsg += "<Success> ";
					pPrIDList->AddItem(new SecsItemASCII(PrjIDs[nIndex]));
				}
				else {
					LogMsg += "<Fail> ";
					pErrors->AddItem(new SecsItemASCII(PrjIDs[nIndex]));
				}
				LogMsg += ", ";
			}
			LogMsg += " )";
			// Formulate the response
			pResults->AddItem(new SecsItemBoolean((pErrors->NumContainedItems() > 0) ? FALSE : TRUE));
			pResults->AddItem(pErrors);
			pTopList->AddItem(pPrIDList);
			pTopList->AddItem(pResults);
		}
		OutMsg.AddItem(pTopList);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 18, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		LOG_INFO(LogMsg);
		return retc;
	}
	LOG_ERR("(S16F17) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S14F12(GEM_HEADER* pHeader) {
	LOG_INFO("(S14F11) Delete object request");
	int retc = 0;
	short ackc = 0;
	MessageS14F11 InMsg;
	string ObjID;
	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		InMsg.GetCjobId(ObjID);
		ackc = pGDev->DeleteObject(ObjID);
		BYTE ObjAck = ackc ? 1 : 0;
		SecsMessage OutMsg;
		SecsItemList* pMain = new SecsItemList;
		SecsItemList* pAtt = new SecsItemList;
		SecsItemList* pRes = new SecsItemList;
		pRes->AddItem(new SecsItemUInt1(ObjAck));
		pRes->AddItem(new SecsItemList);
		pMain->AddItem(pAtt);
		pMain->AddItem(pRes);
		OutMsg.AddItem(pMain);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 14, 12, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		if (ackc) {
			CString MsgStr;
			MsgStr.Format("S14F11 Rejected, ackc = %d", ackc);
			LogMessages(MsgStr);
		}
		return retc;
	}
	LOG_ERR("(S14F11) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S14F2(GEM_HEADER* pHeader) {
	LOG_INFO("(S14F1) GetAttr request GAR");

	int retc = 0;
	MessageS14F1 Msg;
	SecsMessage OutMsg;
	vector < string > ObjID;
	vector < string > AttrIDs;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetObjIds(ObjID);
		Msg.GetAttrIds(AttrIDs);
		if (Msg.ObjType.compare("ProcessJob") == 0) {
			GetObjectAttr(0, ObjID, AttrIDs, OutMsg);
		}
		else if (Msg.ObjType.compare("ControlJob") == 0) {
			GetObjectAttr(1, ObjID, AttrIDs, OutMsg);
		}
		else if (Msg.ObjType.compare("Carrier") == 0) {
			GetObjectAttr(2, ObjID, AttrIDs, OutMsg);
		}
		else if (Msg.ObjType.compare("Substrate") == 0) {
			GetObjectAttr(3, ObjID, AttrIDs, OutMsg);
		}
		else if (Msg.ObjType.compare("SubstLoc") == 0) {
			GetObjectAttr(4, ObjID, AttrIDs, OutMsg);
		}
		else {
			GetObjectAttr(-1, ObjID, AttrIDs, OutMsg);
		}
		string MsgStr = "(OSS : GetAttr REQ) ";
		MsgStr += "Object Type :" + Msg.ObjType;
		MsgStr += "\nObject IDs : ";
		int nObjs;
		for (nObjs = 0; nObjs<int(ObjID.size()); nObjs++) {
			MsgStr += ObjID[nObjs] + ", ";
		}
		MsgStr += "\nAttributes : ";
		for (nObjs = 0; nObjs<int(AttrIDs.size()); nObjs++) {
			MsgStr += AttrIDs[nObjs] + ", ";
		}
		LOG_INFO(MsgStr);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 14, 2, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S14F1) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::evt_cms_watingforhostcarrierid(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_watingforhostslotmap(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	CEvt evt;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::mk_S1F14(GEM_HEADER* shead) {
	LOG_INFO("(S1F13) Establish communication request ER");

	int retc = 0;
	BYTE commack = 0;
	MessageS1F13 S1F13In;
	if (S1F13In.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		if (hmCommState == CHsmsParaMeter::COMMUNICATING) {
			commack = 0;	// Alway accept even if already communicating
			Log("Already communicating", 2);
		}
		bDisabled = FALSE;
		hmCommState = CHsmsParaMeter::COMMUNICATING;
		Log("Communicating now", 10);
		SecsMessage Message;
		SecsItemList* pList1 = new SecsItemList();
		SecsItemList* pList2 = new SecsItemList();
		pList2->AddItem(new SecsItemASCII(string(CEIDCtlr.m_SVList.m_Svid[SVID_LIST::DEVICEID].GetV())));
		pList2->AddItem(new SecsItemASCII(string(CEIDCtlr.m_SVList.m_Svid[SVID_LIST::SOFTREV].GetV())));
		pList1->AddItem(new SecsItemBinary(&commack, 1));
		pList1->AddItem(pList2);
		Message.AddItem(pList1);
		Message.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 1, 14, shead, Message.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		CSMOL SM;
		CSMClt ipc;
		EqpStatus::CONTROLSTATE temp = pGDev->pGEMObjs->GetLineMode();
		temp = (EqpStatus::CONTROLSTATE)AfxGetApp()->GetProfileInt("Control€€State", "LineMode", temp);
		switch (EqpStatus::CONTROLSTATE(temp)) {
		case EqpStatus::REMOTE:
		{
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_ONLINE);
			m_gEvtQue.OpenMemory("EVTMSG");
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		break;
		case EqpStatus::LOCAL:
		{
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_LOCAL);
			m_gEvtQue.OpenMemory("EVTMSG");
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		break;
		case EqpStatus::OFFLINEEQP:
			ChangeControlStatus(EqpStatus::OFFLINEEQP);
			break;
		case EqpStatus::OFFLINEHOST:
			ChangeControlStatus(EqpStatus::OFFLINEHOST);
			break;
		}
		return retc;
	}
	LOG_ERR("(S1F13) Incorrect format ");
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::mk_S1F18(GEM_HEADER* shead) {
	LOG_INFO("(S1F17) Request ON-LINE RONL");

	BYTE data = 0;
	int retc = 0;
	CEvt evt;

	MessageS1F17 InMsg;
	if (InMsg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		EqpStatus::CONTROLSTATE LineMode = pGDev->pGEMObjs->GetLineMode();
		if ((LineMode == EqpStatus::OFFLINEEQP) ||
			(LineMode == EqpStatus::OFFLINEHOST)) {
			data = 0;
		}
		else if (LineMode == EqpStatus::ATTEMPTONLINE) {
			data = 0;
		}
		else {
			data = 2;
		}
		CHsmsParaMeter::m_HostMode = 2;
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&data, 1));
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 1, 18, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);

		ChangeControlStatus(EqpStatus::REMOTE);

		CEvt* pEvt = new CEvt;
		ASSERT(pEvt);
		pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_REMOTEEVT);
		m_gEvtQue.OpenMemory("EVTMSG");
		m_gEvtQue.WriteQue(pEvt);
		m_gEvtQue.CloseMemory();

		return retc;
	}
	LOG_ERR("(S1F17) Incorrect format ");
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::mk_S1F16(GEM_HEADER* shead) {
	LOG_INFO("(S1F15) Request OFF-LINE ROFL");
	int retc = 0;
	BYTE data = 0;
	MessageS1F15 InMsg;
	if (InMsg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		if (/*pGDev->pDataBank->m_EStatus.LineMode*/pGDev->pGEMObjs->GetLineMode() != EqpStatus::OFFLINEHOST) {
			data = 0;
			SecsMessage OutMsg;
			OutMsg.AddItem(new SecsItemBinary(&data, 1));
			OutMsg.MakeBinaryMessage(shead->m_Message);
			retc = set_head(0, 1, 16, shead, 10 + OutMsg.BinaryMessageLength(), PRI_HOST);
			SendBlock(shead);

			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_HOSTOFF);
			m_gEvtQue.OpenMemory("EVTMSG");
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		else {
			retc = mk_S01F00(shead);
		}
		return retc;
	}
	LOG_ERR("(S1F15) Incorrect format ");
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::evt_cms_idle(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_working(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_maintenance(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_outofservice(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_inservice(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::evt_cms_slotmapverify(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S6F11(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
	}
	return retc;
}

int CGemFactory::mk_S16F28(GEM_HEADER* shead) {
	LOG_INFO("(S16F27) Control job command request");

	int retc = 0;
	UINT nCtljobcmd = 0;
	char ackc = 1;
	char nBuf[65];
	MessageS16F27 Msg;
	string CJId, Cmd, Par, ParVal;
	string LogStr;
	if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		Msg.GetCommand(nCtljobcmd);
		Msg.GetCtrlJobId(CJId);
		Msg.GetParam(Par);
		Msg.GetParamVal(ParVal);
		memset(nBuf, 0, sizeof nBuf);
		memcpy(nBuf, CJId.c_str(), CJId.size());
		BOOL bSaveJobs = atoi(ParVal.c_str()) == 0 ? TRUE : FALSE;

		/** The Control job actions are performed in 3 steps */

		LogStr += "(";
		LogStr += CJId;
		/** Step 1: Check if the action can be performed */
		switch (nCtljobcmd) {
		case 1:
			// Start
			ackc = CJobMgr.CanStart(nBuf);
			LogStr += ": CJStart) ";
			break;
		case 2:
			// CJPause
			ackc = CJobMgr.CanPause(nBuf);
			LogStr += ": CJPause) ";
			break;
		case 3:
			// CJResume
			ackc = CJobMgr.CanResume(nBuf);
			LogStr += ": CJResume) ";
			break;
		case 4:
			// CJCancel
			ackc = CJobMgr.CanCancel(nBuf);
			LogStr += ": CJCancel) ";
			break;
		case 5:
			ackc = CJobMgr.CanDeselect(nBuf);
			LogStr += ": CJDeselect) ";
			break;
		case 6:
			// CJStop
			ackc = CJobMgr.CanStop(nBuf);
			LogStr += ": CJStop) ";
			break;
		case 7:
			// CJAbort
			ackc = CJobMgr.CanAbort(nBuf);
			LogStr += ": CJAbort) ";
			break;
		case 8:
			// CJHOQ : Do not support
			ackc = CJobMgr.CanHOQ(nBuf);
			LogStr += ": CJHOQ) ";
			break;
		default:
			LogStr += ": Unknown) ";
			ackc = 1;
			break;
		}

		/** Step 2: Send appropriate acknowledgement to host */
		SecsMessage Out;
		SecsItemList* pListTop = new SecsItemList;
		SecsItemList* pListErr = new SecsItemList;
		pListTop->AddItem(new SecsItemBoolean(ackc == 0));
		string ErrTxt;
		if (ackc != 0) {
			ErrorMessages::GetErrTxtE5(/*nCtljobcmd,*/ ackc, ErrTxt);
			pListErr->AddItem(new SecsItemUInt2(ackc));
			pListErr->AddItem(new SecsItemASCII(ErrTxt));
		}
		pListTop->AddItem(pListErr);
		Out.AddItem(pListTop);
		Out.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 16, 28, shead, Out.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);

		/** Step 3: Perform the action if OK */
		if (0 == ackc) {
			switch (nCtljobcmd) {
			case 1:
				// CJStart
				CJobMgr.Start(nBuf);
				break;
			case 2:
				// CJPause
				CJobMgr.Pause(nBuf);
				break;
			case 3:
				// CJResume
				CJobMgr.Resume(nBuf);
				break;
			case 4:
				// CJCancel
				CJobMgr.Cancel(nBuf);
				break;
			case 5:
				// CJDeselect
				CJobMgr.Deselect(nBuf);
				break;
			case 6:
				// CJStop
				CJobMgr.Stop(nBuf);
				break;
			case 7:
				// CJAbort
				CJobMgr.Abort(nBuf);
				break;
			case 8:
				// CJHOQ : Do not support
				//ackc = CJobMgr.CJob_HOQ(nBuf);
				//CJobMgr.CJob_Deque(nBuf);
				ackc = 1;
				break;
			default:
				ackc = 1;
				break;
			}
		}
		return retc;
	}
	LOG_ERR("(S164F27) Incorrect Format");
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::mk_S16F4(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F3) Process job create request PRJCR");

	int retc = 0;
	char prjob[65], rBuf[65], timebuf[16], * cptr = NULL;
	BYTE ACKC = 0;
	BOOL flag = TRUE;
	CString dd("");
	int SlotNum;
	int ErrCode = 0;
	string ErrText;
	MessageS16F3 Msg;
	float WaferTH, FilmTH;
	BOOL bOk = TRUE;
	vector < JobDetails > PrJobdetails;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		memset(prjob, 0, sizeof prjob);
		strcpy(prjob, "FSM_PRJOB");
		COleDateTime nTime = COleDateTime::GetCurrentTime();
		memset(timebuf, 0, sizeof timebuf);
		sprintf(timebuf, "%0.4d%0.2d%0.d%0.2d%0.2d%0.2d", nTime.GetYear(), nTime.GetMonth(), nTime.GetDay(), nTime.GetHour(), nTime.GetMinute(), nTime.GetSecond());
		strcat(prjob, timebuf);
		if (PJobMgr.GetJobSpace()) {
			CPRJob* pJob = new CPRJob;
			pJob->SetObjectID(prjob);
			pJob->SetParamPtnID(Msg.GetPortId());
			pGDev->pGEMObjs->Cms[Msg.GetPortId() - 1].CaObj.GetAttr("ObjID", rBuf, sizeof(rBuf));
			pJob->SetMtlName(rBuf);
			string tmp = rBuf;
			pJob->m_PortNo = pGDev->GetPortID(tmp);
			Msg.GetJobDetails(PrJobdetails);
			for (int iJobs = 0; iJobs < int(PrJobdetails.size()); iJobs++) {
				WaferStartInfo WaferInfo;
				SlotNum = atoi(PrJobdetails[iJobs].SlotNum.c_str());
				if (SlotNum > 25 || SlotNum < 1) {
					ACKC = 3;
					break;
				}
				WaferInfo.SlotNo = SlotNum;
				//pJob->SetParamSlotNo(SlotNum, iJobs);
				WaferTH = atof(PrJobdetails[iJobs].WaferThickness.c_str());
				if (WaferTH > 2 || WaferTH < 0) {
					ACKC = 4;
					break;
				}
				WaferInfo.Waferthk = WaferTH;
				//pJob->SetParamWaferThickness(WaferTH, iJobs);
				FilmTH = atof(PrJobdetails[iJobs].filmThickness.c_str());
				if (FilmTH > 200000.0 || FilmTH < 0) {
					ACKC = 5;
					break;
				}
				WaferInfo.Filmthk = FilmTH;
				//pJob->SetParamFilmThickness(FilmTH, iJobs);
				memset(rBuf, ' ', sizeof rBuf);
				pGDev->pGEMObjs->Cms[Msg.GetPortId() - 1].CaObj.CtMap[SlotNum - 1].GetAttr("LotID", rBuf, sizeof(rBuf));
				strscpy(WaferInfo.LotID, LOTIDLEN, rBuf);
				//pJob->SetParamLotID(rBuf, iJobs);
				memset(rBuf, 0, sizeof rBuf);
				pGDev->pGEMObjs->Cms[Msg.GetPortId() - 1].CaObj.CtMap[SlotNum - 1].GetAttr("SubstID", rBuf, sizeof(rBuf));
				strscpy(WaferInfo.Waferid, OBJIDLEN, rBuf);
				//pJob->SetParamWaferID(rBuf, iJobs);
				pJob->SetWaferStartInfo(WaferInfo, iJobs);
			}
			if (ACKC) {
				delete pJob;
				pJob = NULL;
			}
			else {
				pJob->SetPRStart(Msg.StartMethod());
				// 				pJob->SetNumberScan(Msg.GetScanLines());
				// 				pJob->SetScanNo(Msg.ScanNumber());
				pJob->SetMtlType(1);
				if (PJobMgr.Create(pJob) != CPRJobMgr::OK) {
					ASSERT(0);
				}
			}
			SecsMessage OutMsg;
			SecsItemList* pList = new SecsItemList;
			pList->AddItem(new SecsItemASCII(prjob));
			pList->AddItem(new SecsItemBinary(&ACKC, 1));
			OutMsg.AddItem(pList);
			OutMsg.MakeBinaryMessage(pHeader->m_Message);
			retc = set_head(0, 16, 4, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
			SendBlock(pHeader);
			return retc;
		}
	}
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S16F2(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F1) Multi-block Process Job Data Inquire PRJI");

	int retc = 0;
	MessageMBEnq Msg;
	UINT DataLen;
	BYTE GRANT = 0;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		DataLen = Msg.GetDataLen();
		if (DataLen > 14000) {
			GRANT = 2;
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&GRANT, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 2, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S16F1) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S16F26(GEM_HEADER* pHeader) {
	LOG_INFO("(S16F25) PRJobSetStartMethod");

	int retc = 0;
	MessageS16F25 InMsg;
	vector<string> ErrPjs;
	vector<BYTE> ErrCodes;
	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		BOOL bOk = TRUE;
		CPRJobMgr::RCODE RC;
		SecsItemList* pPrjs = new SecsItemList;
		int nIndex;
		for (nIndex = 0; nIndex<int(InMsg.PrJobs.size()); nIndex++) {
			if (!PJobMgr.IsPRJobExist(InMsg.PrJobs[nIndex].c_str())) {
				ErrPjs.push_back(InMsg.PrJobs[nIndex]);
				ErrCodes.push_back(UNKNOWN_OBJECT_INSTANCE);
				continue;
			}
			pPrjs->AddItem(new SecsItemASCII(InMsg.PrJobs[nIndex]));
		}
		for (nIndex = 0; nIndex<int(InMsg.PrJobs.size()); nIndex++) {
			RC = PJobMgr.SetStartMethod((char*)InMsg.PrJobs[nIndex].c_str(), InMsg.StartMethod);
			if (CPRJobMgr::OK != RC) {
				ErrPjs.push_back(InMsg.PrJobs[nIndex]);
				ErrCodes.push_back(RC);
				continue;
			}
			pPrjs->AddItem(new SecsItemASCII(InMsg.PrJobs[nIndex]));
		}
		SecsMessage OutMsg;
		SecsItemList* pRes = new SecsItemList;
		pRes->AddItem(pPrjs);
		SecsItemList* pErrList = new SecsItemList;
		pErrList->AddItem(new SecsItemBoolean(ErrCodes.size() ? false : true));
		SecsItemList* pRejPrjs = new SecsItemList;
		for (nIndex = 0; nIndex<int(ErrPjs.size()); nIndex++) {
			SecsItemList* pEr = new SecsItemList;
			pEr->AddItem(new SecsItemASCII(ErrPjs[nIndex]));
			pEr->AddItem(new SecsItemUInt1(ErrCodes[nIndex]));
			pRejPrjs->AddItem(pEr);
		}
		pErrList->AddItem(pRejPrjs);
		pRes->AddItem(pErrList);
		OutMsg.AddItem(pRes);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 16, 26, pHeader, 10 + OutMsg.BinaryMessageLength(), PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S16F25) Incorrect Format");
	mk_S9F5(pHeader);
	return retc;
}

int CGemFactory::mk_S14F4(GEM_HEADER* pHeader) {
	LOG_INFO("(S14F3) SetAttr request SAR");

	int retc = 0, sendcnt = 0, i = 0, n = 0, p = 0, a = 0;
	MessageS14F3 Msg;
	SecsMessage OutMsg;
	vector < string > ObjID;
	vector < string > AttrIDs;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		SecsItemList* pResult = new SecsItemList;
		SecsItemList* pAck = new SecsItemList;
		vector < SecsItem* > AttrValue;
		Msg.GetObjIds(ObjID);
		Msg.GetAttrIds(AttrIDs);
		Msg.GetAttrValue(AttrValue);
		if (Msg.ObjType.compare("ProcessJob") == 0) {
			SetObjectAttr(0, ObjID, AttrIDs, AttrValue, pResult, pAck);
		}
		else if (Msg.ObjType.compare("ControlJob") == 0) {
			SetObjectAttr(1, ObjID, AttrIDs, AttrValue, pResult, pAck);
		}
		if (Msg.ObjType.compare("Carrier") == 0) {
			SetObjectAttr(2, ObjID, AttrIDs, AttrValue, pResult, pAck);
		}
		if (Msg.ObjType.compare("Substrate") == 0) {
			SetObjectAttr(3, ObjID, AttrIDs, AttrValue, pResult, pAck);
		}
		else {
			pAck->AddItem(new SecsItemUInt1(1));
			pAck->AddItem(new SecsItemList);
		}
		SecsItemList* pList = new SecsItemList;
		pList->AddItem(pResult);
		pList->AddItem(pAck);
		OutMsg.AddItem(pList);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 14, 2, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S14F6(GEM_HEADER* pHeader) {
	LOG_INFO("(S14F5) Not supported");

	return 0;
}

int CGemFactory::mk_S14F8(GEM_HEADER* pHeader) {
	LOG_INFO("(S14F7) Not supported");
	return 0;
}

BOOL CGemFactory::GetPassiveIPaddress(char* ip) {
	*ip = 0;
	// TODO [9/15/2007 LYF]
	ASSERT(0);
	return TRUE;

	HKEY hKey = NULL;
	BOOL b = FALSE;
	char lpszCLSID[256];
	CString str;
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT) {
	}
	else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		if (RegOpenKey(HKEY_LOCAL_MACHINE, "System", &hKey) == ERROR_SUCCESS) {
			HKEY hKeyCLSID = NULL;
			memset(lpszCLSID, 0, sizeof lpszCLSID);
			if (RegOpenKey(hKey, "CurrentControlSet", &hKeyCLSID) == ERROR_SUCCESS) {
				HKEY hKeyInProc = NULL;
				if (RegOpenKey(hKeyCLSID, "Services", &hKey) == ERROR_SUCCESS) {
					HKEY hKey1 = NULL;
					if (RegOpenKey(hKey, "Class", &hKey1) == ERROR_SUCCESS) {
						HKEY hKey2 = NULL;
						if (RegOpenKey(hKey1, "NetTrans", &hKey2) == ERROR_SUCCESS) {
							HKEY hKey3 = NULL;
							if (RegOpenKey(hKey2, "0001", &hKey3) == ERROR_SUCCESS) {
								LPTSTR lpsz = str.GetBuffer(_MAX_PATH);
								DWORD dwSize = _MAX_PATH * sizeof(TCHAR);
								DWORD dwType;
								LONG lRes = ::RegQueryValueEx(hKey3, "IPAddress", NULL, &dwType, (BYTE*)lpsz, &dwSize);
								if (lRes == ERROR_SUCCESS) {
									memcpy(ip, lpsz, strlen(lpsz));
								}
								else {
									return FALSE;
								}
							}
							else {
								return FALSE;
							}
							RegCloseKey(hKey3);
						}
						else {
							return FALSE;
						}
						RegCloseKey(hKey2);
					}
					else {
						return FALSE;
					}
					RegCloseKey(hKey1);
				}
				else {
					return FALSE;
				}
				RegCloseKey(hKeyInProc);
			}
			else {
				return FALSE;
			}
			RegCloseKey(hKeyCLSID);
		}
		else {
			return FALSE;
		}
		RegCloseKey(hKey);
	}
	else {
	}
	return TRUE;
}

BOOL CGemFactory::SetPassiveIPaddress(const char* ip) {
	HKEY hKey = NULL;
	BOOL b = FALSE;
	char lpszCLSID[256];
	CString str;
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT) {
	}
	else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		if (RegOpenKey(HKEY_LOCAL_MACHINE, "System", &hKey) == ERROR_SUCCESS) {
			HKEY hKeyCLSID = NULL;
			memset(lpszCLSID, 0, sizeof lpszCLSID);
			if (RegOpenKey(hKey, "CurrentControlSet", &hKeyCLSID) == ERROR_SUCCESS) {
				HKEY hKeyInProc = NULL;
				if (RegOpenKey(hKeyCLSID, "Services", &hKey) == ERROR_SUCCESS) {
					HKEY hKey1 = NULL;
					if (RegOpenKey(hKey, "Class", &hKey1) == ERROR_SUCCESS) {
						HKEY hKey2 = NULL;
						if (RegOpenKey(hKey1, "NetTrans", &hKey2) == ERROR_SUCCESS) {
							HKEY hKey3 = NULL;
							if (RegOpenKey(hKey2, "0001", &hKey3) == ERROR_SUCCESS) {
								LPTSTR lpsz = str.GetBuffer(_MAX_PATH);
								DWORD dwSize = _MAX_PATH * sizeof(TCHAR);
								DWORD dwType;
								LONG lRes = ::RegQueryValueEx(hKey3, "IPAddress", NULL, &dwType, (BYTE*)lpsz, &dwSize);
								if (lRes == ERROR_SUCCESS) {
									dwSize = (lstrlen(lpsz) + 1) * sizeof(TCHAR);
									lRes = RegSetValueEx(hKey3, "IPAddress", NULL, REG_SZ, (BYTE* const)ip, strlen(ip));
								}
								else {
									return FALSE;
								}
							}
							else {
								return FALSE;
							}
							RegCloseKey(hKey3);
						}
						else {
							return FALSE;
						}
						RegCloseKey(hKey2);
					}
					else {
						return FALSE;
					}
					RegCloseKey(hKey1);
				}
				else {
					return FALSE;
				}
				RegCloseKey(hKeyInProc);
			}
			else {
				return FALSE;
			}
			RegCloseKey(hKeyCLSID);
		}
		else {
			return FALSE;
		}
		RegCloseKey(hKey);
	}
	else {
	}
	return TRUE;
}

BOOL CGemFactory::set_Sparameter(int rno, CString pVal) {
	FILE* fp;
	char dwBuf[12][200];
	char dwPar[15];
	int nRowNo = 1;
	int nArrycnt = 0;
	dwPar[14] = 0;
	memset(dwPar, ' ', sizeof(dwPar) - 1);
	int len = pVal.GetLength();
	memcpy(dwPar, pVal.operator const char* (), len);
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[256];
	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "SECSPARA", "PAR");
	if ((fp = fopen(inifile, "r")) == NULL) {
		Log("SecsPara File Open Fail");
		return FALSE;
	}
	else {
		while (fgets(dwBuf[nArrycnt], 200, fp)) {
			if (rno == nRowNo) {
				memcpy(&dwBuf[nArrycnt][28], dwPar, 14);
			}
			Sleep(10);
			nRowNo++;
			nArrycnt++;
		}
		fclose(fp);
	}
	_fcloseall();
	Sleep(1000);
	fp = NULL;
	if ((fp = fopen(inifile, "w")) == NULL) {
		Log("SecsPara.Par File One More Open Fail");
		return FALSE;
	}
	else {
		for (int i = 0; i < 12; i++) {
			fputs(dwBuf[i], fp);
		}
		fclose(fp);
	}
	return TRUE;
}

int CGemFactory::mk_S1F12(GEM_HEADER* shead) {
	LOG_INFO("(S1F11) Status variable namelist request SVNR");

	int retc = 0;
	vector < UINT > SvidList;
	MessageS1F11 Msg;
	SecsMessage OutMsg;
	if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		Msg.GetSvidList(SvidList);
		GetSvStat(SvidList, OutMsg);
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 1, 12, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		return retc;
	}
	LOG_ERR("(S1F11) Incorrect format ");
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::mk_S6F16(GEM_HEADER* shead) {
	LOG_INFO("(S6F15) Event report request ERR");

	int retc = 0, rptlistcount = 0, sendcnt = 10, svidcnt = 0;
	UINT ceid = 0, * rptno = NULL, * svidno = NULL;
	vector < SVID_VALUE > dSvval;
	CString dd;
	MessageS6F15 Msg;

	if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		ceid = Msg.GetCEID();
		rptlistcount = CEIDCtlr.get_rptcount(ceid);
		UINT Dummy = 0;
		SecsItemList* pTopList = new SecsItemList;
		// Item # 1 DataID Always 0
		pTopList->AddItem(new SecsItemUInt4(Dummy));
		// Item # 2 CEID
		pTopList->AddItem(new SecsItemUInt4(ceid));
		// Item # 3 List of SVID values
		SecsItemList* pRPTList = new SecsItemList;
		if (rptlistcount == 0) {
			; // Do nothing
		}
		else {
			rptno = new UINT[rptlistcount];
			CEIDCtlr.getting_rptno(ceid, rptno);
			for (int i = 0; i < rptlistcount; i++) {
				svidcnt = CEIDCtlr.get_svidcount(rptno[i]);
				svidno = new UINT[svidcnt];
				CEIDCtlr.get_svid(rptno[i], svidno);
				SecsItemList* pReport = new SecsItemList;
				pReport->AddItem(new SecsItemUInt4(rptno[i]));
				SecsItemList* pSVList = new SecsItemList;
				int nIndex = 0;
				for (int j = 0; j < svidcnt; j++) {
					pSVList->AddItem(get_svval(1, svidno[j], NULL));
				}
				pReport->AddItem(pSVList);
				pRPTList->AddItem(pReport);
				delete[] svidno;
			}
			delete[] rptno;
		}
		SecsMessage OutMsg;
		pTopList->AddItem(pRPTList);
		OutMsg.AddItem(pTopList);
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 6, 16, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		return retc;
	}
	LOG_ERR("(S6F15) Incorrect Format");
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::mk_S6F20(GEM_HEADER* shead) {
	LOG_INFO("(S6F19) Individual report request IRR");

	int retc = 0, svidcnt = 0, sendcnt = 10;
	CString dd("");
	UINT rptno;
	int nIndex = 0;
	vector < SVID_VALUE > dSvval;
	MessageS6F19 Msg;

	if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		rptno = Msg.GetRptID();
		UINT* svidno = NULL;
		SecsItemList* pTop = new SecsItemList;
		// Get # of attached SVIDs
		svidcnt = CEIDCtlr.get_svidcount(rptno);
		if (svidcnt) {
			svidno = new UINT[svidcnt];
			CEIDCtlr.get_svid(rptno, svidno);
			sendcnt += 2;
			for (int j = 0; j < svidcnt; j++) {
				pTop->AddItem(get_svval(1, svidno[j], NULL));
			}
			delete[] svidno;
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 6, 20, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		return retc;
	}
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::mk_S6F22(GEM_HEADER* shead) {
	LOG_INFO("(S6F21) Annotated individual report AIR");

	int retc = 0, svidcnt = 0, sendcnt = 10;
	CString dd("");
	MessageS6F21 Msg;
	UINT rptno;
	if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		rptno = Msg.GetRptID();
		vector < SVID_VALUE > dSvval;
		UINT* svidno = NULL;
		svidcnt = CEIDCtlr.get_svidcount(rptno);
		SecsItemList* pTopList = new SecsItemList;
		if (svidcnt) {
			svidno = new UINT[svidcnt];
			CEIDCtlr.get_svid(rptno, svidno);
			int nIndex = 0;
			for (int j = 0; j < svidcnt; j++) {
				SecsItemList* pSVRpt = new SecsItemList;
				pSVRpt->AddItem(new SecsItemUInt4(svidno[j]));
				pSVRpt->AddItem(get_svval(1, svidno[j], NULL));
				pTopList->AddItem(pSVRpt);
			}
			delete[] svidno;
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(pTopList);
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 6, 22, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		return retc;
	}
	LOG_ERR("(S6F21) Incorrect Format");
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::mk_S9F1(GEM_HEADER* shead) {
	LOG_WARN("Send: S9F1 Unrecognized device ID UDN");
	int retc = 0;
	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemBinary((BYTE*)shead->m_SessionId, 10));
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(0, 9, 1, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP); // sizeof(SECS_HEAD) +);
	SendBlock(shead);
	return retc;
}

short CGemFactory::GetDeviceFromHost(GEM_HEADER* pHeader) {
	short retc = 0;
	retc = ((short)pHeader->m_SessionId[0] << 8) | ((short)pHeader->m_SessionId[1]);
	return retc;
}

int CGemFactory::mk_S5F4(GEM_HEADER* pHeader) {
	LOG_INFO("(S5F3) Enable disable alarm send EAS");

	int retc = 0;
	BYTE ACK5 = 0;
	MessageS5F3 InMsg;
	InMsg.bAll = false;
	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		if (InMsg.bAll) {
			if (InMsg.bEnable) {
				CEIDCtlr.m_alarm.EnableAllAlarms();//  [9/16/2007 LYF]
			}
			else {
				CEIDCtlr.m_alarm.DisableAllAlarms();
			}
		}
		else {
			if (TRUE == CEIDCtlr.m_alarm.IsAlarm(InMsg.AlarmID)/*pGDev->m_AlarmArry.IsAlarm(InMsg.AlarmID)*/) //  [9/16/2007 LYF]
			{
				// Check for valid alarm entry
				if (InMsg.bEnable) {
					CEIDCtlr.m_alarm.EnableAlarm(InMsg.AlarmID);
				}
				else {
					CEIDCtlr.m_alarm.DisableAlarm(InMsg.AlarmID);
				}
			}
			else {
				ACK5 = 1;
			}
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&ACK5, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 5, 4, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S5F3) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S5F6(GEM_HEADER* pHeader) {
	LOG_INFO("(S5F5) List alarms request LAR");

	MessageS5F5 InMsg;
	vector < UINT > vecALID;
	vector < CString > vecALTX;
	int retc = 0;
	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		//AlarmData tmpData;
		SecsMessage OutMsg;
		SecsItemList* pMain = new SecsItemList;
		if (InMsg.ALIDs.empty()) {
			CEIDCtlr.m_alarm.GetAllAlarmID(vecALID, vecALTX);
		}
		else {
			vecALID = InMsg.ALIDs;
		}
		for (UINT nIndex = 0; nIndex < vecALID.size(); nIndex++) {
			// Get the Data
			SecsItemList* pALData = new SecsItemList;
			SecsItem* pBinary;

			CAlrmItem* pA = CEIDCtlr.m_alarm.GetAlarm(vecALID[nIndex]);
			if (pA) {
				BYTE byte = pA->alCD;
				pBinary = new SecsItemBinary(&byte, 1);
				pALData->AddItem(pBinary);
				pALData->AddItem(new SecsItemUInt4(pA->alID)); //  [9/16/2007 LYF]
				pALData->AddItem(new SecsItemASCII(pA->alTX));
			}
			else {
				pBinary = new SecsItemZeroLen(SecsItem::ITEM_BINARY);
				pALData->AddItem(pBinary);
				pALData->AddItem(new SecsItemUInt4(0/*pA->alID*/)); //  [9/16/2007 LYF]
				pALData->AddItem(new SecsItemASCII(""));
			}
			pMain->AddItem(pALData);
		}
		OutMsg.AddItem(pMain);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 5, 6, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S5F5) Incorrect Format ");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::evt_ETS_SendMsgToHost(short phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	//
	if (phase == 0) {
		ret = mk_S10F1(shead, event);
	}
	else if (phase == 1) {
		// Need notification ???
		ret = 1;
	}
	return ret;
}

int CGemFactory::mk_S10F1(GEM_HEADER* pHeader, CEvt* ehead) {
	LOG_INFO("Send: S10F1 Terminal request TRN");
	int retc;
	BYTE TID = ehead->no;
	SecsMessage OutMsg;
	SecsItemList* pList = new SecsItemList;
	pList->AddItem(new SecsItemBinary(&TID, 1));
	pList->AddItem(new SecsItemASCII(pGDev->pDataBnk->m_HostMsg1));
	OutMsg.AddItem(pList);
	OutMsg.MakeBinaryMessage(pHeader->m_Message);
	retc = set_head(1, 10, 1, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
	SendBlock(pHeader);
	return retc;
}

int CGemFactory::mk_S10F10(GEM_HEADER* pHeader) {
	LOG_INFO("(S10F9) Broadcast BCN");

	int retc = 0;
	int dw_Wbit = 0;
	BYTE ACKC10 = 0;
	static MessageS10F10 InMsg;
	//SecsMessage OutMsg;
	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		// Note We have only one terminal now so ....
		//		ZeroMemory(pGDev->pDataBank->m_HostMsg1,  MAX_CONSOLE_MSG);
		//		strncpy(pGDev->pDataBank->m_HostMsg1,InMsg.Text.c_str(),MAX_CONSOLE_MSG);
		//pGDev->pHostMessage->Message(InMsg.Text.c_str()); //  [9/15/2007 LYF]
		pUDev->pHostMessage->PostMessage(UM_HOST_MESSAGE, 0, (LPARAM)&InMsg.Text); //  [9/15/2007 LYF]
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&ACKC10, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 10, 4, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		//		::SendMessage(::m_gHwnd, WM_HOST_MESSAGE_RECD, 100, 0);
		return retc;
	}
	mk_S9F7(pHeader);
	return retc;
}

//CHECK
int CGemFactory::mk_S10F6(GEM_HEADER* pHeader) {
	LOG_INFO("(S10F5) Terminal display, multiblock VTN");
	int retc = 0;
	int dw_Wbit = 0;
	BYTE ACKC10 = 0;
	static string MsgString;
	string termTxt;
	BYTE TID;
	MessageS10F5 Msg;
	SecsMessage OutMsg;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		TID = Msg.GetTerminalMsg(MsgString);
		if (0 == TID) {
			if (MsgString.size() > MAX_CONSOLE_MSG) {
				ACKC10 = 1;
			}
			else {
				// memset(pGDev->pDataBank->m_HostMsg1, ' ', MAX_CONSOLE_MSG);
				// memcpy(pGDev->pDataBank->m_HostMsg1, MsgString.c_str(), MsgString.size());
				// pGDev->pHostMessage->Message(MsgString.c_str()); //  [9/15/2007 LYF]
				pUDev->pHostMessage->PostMessage(UM_HOST_MESSAGE, 0, (LPARAM)&MsgString); //  [9/15/2007 LYF]
				// ::SendMessage(::m_gHwnd, WM_HOST_MESSAGE_RECD, MsgString.size(), 0);
			}
		}
		else {
			ACKC10 = 2; // Incorrect terminal ID specified
		}
		OutMsg.AddItem(new SecsItemBinary(&ACKC10, 1));
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 10, 6, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		if (1 == ACKC10) {
			mk_S10F7(pHeader);
		}
		return retc;
	}
	LOG_ERR("(S10F5) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::evt_ETS_MSG_Recognized(short phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S6F11(shead, event);
	}
	else if (phase == 1) {
		// Need notification ???
		ret = 1;
	}
	return ret;
}

int CGemFactory::Evt_Spool_Activated(short phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S6F11(shead, event);
	}
	else if (phase == 1) {
		// Need notification ???
		ret = 1;
	}
	return ret;
}

int CGemFactory::Evt_Spool_Deactivated(short phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S6F11(shead, event);
	}
	else if (phase == 1) {
		// Need notification ???
		ret = 1;
	}
	return ret;
}

int CGemFactory::evt_line_commestablish(int phase, CEvt* event, GEM_HEADER* shead) {
	int retc = 0;
	if (phase == 0) {
		retc = mk_S1F13(shead, event);
	}
	else if (phase == 1) // ACTION HERE
	{
		retc = 1;
		if (CHsmsParaMeter::WAITCRA) {
			hmCommState = CHsmsParaMeter::COMMUNICATING;
			if (pGDev->pDataBnk->bAutoOnline) {
				// 				EqpStatus::CONTROLSTATE temp = pGDev->pGEMObjs->GetLineMode();
				EqpStatus::CONTROLSTATE temp = (EqpStatus::CONTROLSTATE)AfxGetApp()->GetProfileInt("ControlState", "LineMode", pGDev->pGEMObjs->GetLineMode());
				//				short temp = m_DefOnlineState;
				CSMOL SM;
				CSMClt ipc;
				switch (temp) {
				case EqpStatus::REMOTE:
				case EqpStatus::LOCAL:
					switch (EqpStatus::CONTROLSTATE(m_DefOnlineState)) {
					case EqpStatus::REMOTE:
					{
						CEvt* pEvt = new CEvt;
						ASSERT(pEvt);
						pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_ONLINE);
						m_gEvtQue.OpenMemory("EVTMSG");
						m_gEvtQue.WriteQue(pEvt);
						m_gEvtQue.CloseMemory();
					}
					break;
					case EqpStatus::LOCAL:
					{
						CEvt* pEvt = new CEvt;
						ASSERT(pEvt);
						pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_LOCAL);
						m_gEvtQue.OpenMemory("EVTMSG");
						m_gEvtQue.WriteQue(pEvt);
						m_gEvtQue.CloseMemory();
					}
					break;
					}
					break;
				case EqpStatus::OFFLINEEQP:
					ChangeControlStatus(EqpStatus::OFFLINEEQP);
					break;
				case EqpStatus::OFFLINEHOST:
					ChangeControlStatus(EqpStatus::OFFLINEHOST);
					break;
					// 					switch (EqpStatus::CONTROLSTATE(m_DefOfflineState))
					// 					{
					// 						case EqpStatus::OFFLINEEQP:
					// 							ChangeControlStatus(EqpStatus::OFFLINEEQP);
					// 							break;
					// 						case EqpStatus::OFFLINEHOST:
					// 							ChangeControlStatus(EqpStatus::OFFLINEHOST);
					// 							break;
					// 					}
					// 					break;
				}
			}
		}
	}
	return retc;
}

int CGemFactory::evt_attempt_online(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S1F1(shead, event);
		if (ret) {
			ChangeControlStatus(EqpStatus::ATTEMPTONLINE);
		}
	}
	else if (phase == 1) {
		BYTE* pData = (BYTE*)shead->m_Message;
		if (pData[4] > 0) {
			// Host denied the connection so try ATTEMPT ONLINE
			// No need to try here again a thread will do the task
		}
		else {
			ChangeControlStatus(EqpStatus::REMOTE);
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_REMOTEEVT);
			m_gEvtQue.OpenMemory("EVTMSG");
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		ret = 1;
	}
	return ret;
}

int CGemFactory::evt_attempt_local(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S1F1(shead, event);
		if (ret) {
			ChangeControlStatus(EqpStatus::ATTEMPTONLINE);
		}
	}
	else if (phase == 1) {
		BYTE* pData = (BYTE*)shead->m_Message;
		if (pData[4] > 0) {
			// Host denied the connection so try ATTEMPT ONLINE
			// No need to try here again a thread will do the task
		}
		else {
			ChangeControlStatus(EqpStatus::LOCAL);
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_LOCALEVT);
			m_gEvtQue.OpenMemory("EVTMSG");
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		ret = 1;
	}
	return ret;
}

int CGemFactory::mk_S1F1(GEM_HEADER* shead, CEvt* ehead) {
	int retc = 0;
	// Clean memory
	memset(shead, 0, sizeof GEM_HEADER);
	// Set header only
	retc = set_head(1, 1, 1, shead, 10, PRI_EQUIP);
	SendBlock(shead);
	return retc;
}

int CGemFactory::mk_S9F3(GEM_HEADER* shead) {
	char Msg[126];
	sprintf(Msg, "Send: S9F3 Unknown stream type S%dF%d USN", shead->m_Stream, shead->m_Function);
	LOG_WARN(Msg);

	int retc = 0;
	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemBinary((BYTE*)shead->m_SessionId, 10));
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(0, 9, 3, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP); // sizeof(SECS_HEAD) +);
	SendBlock(shead);
	return retc;
}

int CGemFactory::evt_Reply_Timeout(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		BYTE* pmsg = event->GetMsg(10);
		if ((pmsg[2]) & 0x7f) {
			memcpy(&(shead->m_Function), pmsg, 10);
			ret = mk_S9F9(shead);
		}
	}
	else if (phase == 1) {
		ret = 1;
	}
	return ret;
}

int CGemFactory::mk_S9F9(GEM_HEADER* shead) {
	LOG_WARN("Send: S9F9 Transaction timer timeout TTN");
	int retc = 0;
	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemBinary((BYTE*)shead->m_SessionId, 10));
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(0, 9, 9, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP); // sizeof(SECS_HEAD) +);
	SendBlock(shead);
	return retc;
}

int CGemFactory::evt_init_data(int phase, CEvt* event, GEM_HEADER* shead) {
	InitGlobals();
	//pGDev->pDataBank->ClearJobs();
	return 0;
}

int CGemFactory::mk_S2F44(GEM_HEADER* pHeader) {
	LOG_INFO("(S2F43) Reset spooling stream and functions RSSF");

	int retc = 0;
	BYTE StreamID;
	BYTE nFuncts;
	BYTE FunctNo;
	BYTE* pByte = NULL;
	BYTE RSPACK = 0;
	BYTE STRACK = 0;
	BOOL bStreamErr = FALSE;
	vector < Reply > ErrReport;
	Reply ErrorData;
	vector < int > FunctVec;
	vector < USHORT > StreamVec;

	CSpoolMan* pSpool = CSpoolMan::GetSpool();
	MessageS2F43 InMsg;
	if (InMsg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		pSpool->RemoveAllStreamFunct();
		if (InMsg.StFuncDat.empty()) {
			pSpool->RemoveAllStreamFunct();
		}
		else {
			for (UINT nStream = 0; nStream < InMsg.StFuncDat.size(); nStream++) {
				StreamID = InMsg.StFuncDat[nStream].StreamID;
				ErrorData.StreamId = StreamID;
				ErrorData.STRACK = 0;
				// Check for stream ID
				if (1 == StreamID) {
					ErrorData.STRACK = 1; // Spool is not allowed for this stream
					bStreamErr = TRUE;
				}
				else if ((StreamID < 2) || (StreamID > 64)) {
					ErrorData.STRACK = 2; // Unknown stream(Invalid)
					bStreamErr = TRUE;
				}
				nFuncts = BYTE(InMsg.StFuncDat[nStream].FcnID.size());
				if ((0 == nFuncts) && (!bStreamErr)) {
					// Spool all functions for this stream
					pSpool->EnableAllFunctions(StreamID);
					continue;
				}
				else {
					for (UINT Functs = 0; Functs < nFuncts; Functs++) {
						FunctNo = InMsg.StFuncDat[nStream].FcnID[Functs];
						// Check function code here
						if (!(FunctNo % 2)) {
							// Secondary function specified
							ErrorData.STRACK |= 3;
							ErrorData.ErrroFunctions.push_back(FunctNo);
						}
						else if ((StreamID < 1) || (StreamID > 64)) {
							// Invalid function specified
							ErrorData.STRACK |= 2;
							ErrorData.ErrroFunctions.push_back(FunctNo);
						}
						else {
							// Probably correct function !
							FunctVec.push_back(FunctNo);
						}
					}
				}
				if (!bStreamErr) {
					pSpool->SpoolStreamFunct(StreamID, FunctVec);
				}
				if (0 != ErrorData.STRACK) {
					ErrReport.push_back(ErrorData);
				}
				// Standard clean up
				FunctVec.clear();
				bStreamErr = FALSE;
				ErrorData.ErrroFunctions.clear();
			}
		}
		if (ErrReport.size() > 0) {
			// pRepare detailed report
			RSPACK = 1;
		}
		SecsItemList* pReply = new SecsItemList;
		pReply->AddItem(new SecsItemBinary(&RSPACK, 1));
		SecsItemList* pList = new SecsItemList;
		for (UINT nErr = 0; nErr < ErrReport.size(); nErr++) {
			SecsItemList* pErrDat = new SecsItemList;
			pErrDat->AddItem(new SecsItemUInt1(ErrReport[nErr].StreamId));
			pErrDat->AddItem(new SecsItemBinary(&ErrReport[nErr].STRACK, 1));
			SecsItemList* pFuncts = new SecsItemList;
			pErrDat->AddItem(pFuncts);
			for (/*UINT */nFuncts = 0; nFuncts < ErrReport[nErr].ErrroFunctions.size(); nFuncts++) {
				pFuncts->AddItem(new SecsItemUInt1(ErrReport[nErr].ErrroFunctions[nFuncts]));
			}
			pList->AddItem(pErrDat);
		}
		SecsMessage OutMsg;
		pReply->AddItem(pList);
		OutMsg.AddItem(pReply);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 2, 44, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::mk_S6F24(GEM_HEADER* sHead) {
	LOG_INFO("(S6F23) Request spooled data RSD");

	int retc = 0;
	BYTE RSDA = 0;
	MessageS6F23 InMsg;
	if (InMsg.CheckMessage(sHead->m_DataLen, sHead->m_Message)) {
		CSpoolMan* pSpool = CSpoolMan::GetSpool();
		if (0 == InMsg.RSDC) {
			// Send spooled data as per MaxSpoolTransmit
			if (pSpool->SpooledSoFar()) {
				// Start CountDown of MaxSpoolTransmit
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				pSpool->TransmitCount = (0 == pSpool->TransmitAmount()) ? pSpool->ActualSpooled() : pSpool->TransmitAmount();
				pSpool->TransmitCount = (pSpool->TransmitCount > pSpool->ActualSpooled()) ? pSpool->ActualSpooled() :
					pSpool->TransmitAmount();
				pEvt->Set(/*8*/ EVT_SPOOL, /*1*/ EVT_UNLOAD_SPOOL);
				m_gEvtQue.OpenMemory("EVTMSG");
				m_gEvtQue.WriteQue(pEvt);
				m_gEvtQue.CloseMemory();
			}
			else {
				RSDA = 2;
			}
		}
		else if (1 == InMsg.RSDC) {
			// Purge all messages
			pSpool->InitSpool();
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&RSDA, 1));
		OutMsg.MakeBinaryMessage(sHead->m_Message);
		retc = set_head(0, 6, 24, sHead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(sHead);
		return retc;
	}
	LOG_ERR("(S6F23) Incorrect format");
	mk_S9F7(sHead);
	return retc;
}

int CGemFactory::Evt_Spool_Unload(short phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	// Get next item from spool and transmit
	CSpoolMan* pSpool = CSpoolMan::GetSpool();
	if (phase == 0) {
		pSpool->GetNextSpooledMessage(/*&theHeader*/ shead);
		ret = SendBlock(/*&theHeader*/ shead);
		::Sleep(1000);
	}
	else if (phase == 1) {
		// Reduce the count
		pSpool->TransmitCount--;
		if (0 < pSpool->TransmitCount) {
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			pEvt->Set(/*8*/ EVT_SPOOL, /*1*/ EVT_UNLOAD_SPOOL);
			m_gEvtQue.OpenMemory("EVTMSG");
			m_gEvtQue.WriteQue(pEvt);
			m_gEvtQue.CloseMemory();
		}
		else {
			if (0 == pSpool->ActualSpooled()) {
				// Spool Reset here ??
				CEvt SpoolEvt;
				SpoolEvt.Set(EVT_SPOOL, EVT_SPOOL_DEACTIVATED);
				m_gEvtQue.OpenMemory("EVTMSG");
				m_gEvtQue.WriteQue(&SpoolEvt);
				m_gEvtQue.CloseMemory();
			}
			// Transmit spool is reached & no event is expected
		}
		ret = 1;
	}
	// Get one messsage
	// Transmit
	// If OK then
	// ... reduce the count
	// if more messages to send then set same event again !
	// Spool one message
	// If successfully send then send another message

	/* This scheme work as follows
	One message is send and the reply is checked. If transaction is failed then spooling is
	contd as before
	*/

	// if more items are in the spool to be transmitted then write the event again
	return ret;
}

int CGemFactory::EvtGetTimeFromHost(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S2F17(shead); // Header only
	}
	else if (phase == 1) {
		// Now set the timer to the Equipment
		//		int dwStream = shead->m_Stream & 0x7f;
		//		int dwFunc = shead->m_Function;
		//		if ((dwStream == 2) && (dwFunc == 18))
		//		{
		// Decode the entire message
		UINT MsgLen = 0;
		BYTE Len = (BYTE)(shead->m_Message[0]) & 3; // First 2 bits only
		switch (Len) {
		case 1:
			MsgLen = shead->m_Message[1];
			break;
		case 2:
			MsgLen = AtoUshort(&(shead->m_Message[1]));
			break;
		case 3:
			MsgLen = ByteSwap(&(shead->m_Message[1]));
			break;
		default:
			/*Len = Len*/;
			break;
		}
		if ((MsgLen == 12) || (MsgLen == 16)) {
			// Valid values please
			if (SetSecsTime((char*)&(shead->m_Message[Len + 1]), MsgLen)) {
			}
			else {
			}
		}
		//		}
		// Copy all the source code from S2F32
		ret = 1;
	}
	return ret;
}

int CGemFactory::EvtRcpChange(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		// Set value of PPChange Status & PPchangeName here
		ASSERT("Recipe ref removed. Code needs redone");
		//		CRecipe::PPChangeName = (char *)event->GetMsg();
		//		CRecipe::PPChangeStatus = event->no;
		ret = mk_S6F11(shead, event);
		// Clear value of PPChange Status & PPchangeName here
		//		CRecipe::PPChangeName = "";
		//		CRecipe::PPChangeStatus = 0;
		//		// Set value of PPChange Status & PPchangeName here
		//		CRecipe::PPChangeName = (char *)event->GetMsg();
		//		CRecipe::PPChangeStatus = event->no;
		//		ret = mk_S6F11(shead, event);
		//		// Clear value of PPChange Status & PPchangeName here
		//		CRecipe::PPChangeName = "";
		//		CRecipe::PPChangeStatus = 0;
	}
	else if (phase == 1) {
	}
	return ret;
}

int CGemFactory::ExcessMsgLen(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ASSERT(event && shead);
		ret = mk_S9F11(shead, event);
	}
	else if (phase == 1) {
	}
	return ret;
}

int CGemFactory::EvtUploadRecipe(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S7F3(shead, event);
	}
	else if (phase == 1) {
	}
	return ret;
}

//CHECK : How to report format error
//@@Vijay#4June$02 New message added
// @@Vijay [11/18/2002] This Message is not used anywhere. Need to check.
int CGemFactory::EvtRcpDownload(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S7F5(shead, event);
	}
	else if (phase == 1) {
		// Check name of requested PPID
		MessageS7F3 Msg; // Same format ! Reuse
		if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
			string PPID = (char*)event->GetMsg();
			if (PPID == Msg.PPID) {
				// Replace the
				ASSERT(!"Recipe ref removed code needs to be redone");
				/*				CRecipe Rcp;
				Rcp.BinaryToRecipe(Msg.PPBODY,Msg.PPLen);
				if (RcpMgr.Save(&Rcp))
				{
				// PostMessage
				return ret;
			}*/
			}
		}
		// If OK save
	}
	return ret;
}

//CHECK : How to report format error?
int CGemFactory::EvtMBEnqRecipe(int phase, CEvt* event, GEM_HEADER* shead) {
	int ret = 0;
	if (phase == 0) {
		ret = mk_S7F1(shead, event);
	}
	else if (phase == 1) {
		MessageS7F2 Msg;
		if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
			if (Msg.PPGNT == 0) {
				CSharedMemQue MemQue;
				CEvt* pEvt = new CEvt;
				ASSERT(pEvt);
				*pEvt = *event;
				pEvt->Set(EVT_OTHER, EVT_UPLOAD_RECIPE); //
				MemQue.OpenMemory("EVTMSG");
				MemQue.WriteQue(pEvt);
				MemQue.CloseMemory();
				return ret;
			}
		}
	}
	return ret;
}

void CGemFactory::InitGlobals() {
	CString winini;
	CString sect;
	sect = "FSM";
	DosUtil.GetLocalSysFile(winini);
}

short CGemFactory::SetSecsTime(char* TimeVal, int StrLen) {
	USHORT dwTT;
	SYSTEMTIME dwTime;
	short tiack = 0;
	char cbTimeVal[16];
	if (StrLen == 12) {
		memset(cbTimeVal, 0, sizeof cbTimeVal);
		memcpy(cbTimeVal, TimeVal, 2);
		dwTT = atoi(cbTimeVal);
		dwTime.wYear = (WORD)(2000 + dwTT);
		if (dwTime.wYear > 2099 || dwTime.wYear < 2000) {
			tiack = 1;
		}
		else {
			memset(cbTimeVal, 0, sizeof cbTimeVal);
			memcpy(cbTimeVal, &TimeVal[2], 2);
			dwTT = atoi(cbTimeVal);
			dwTime.wMonth = dwTT;
			if (dwTT < 1 || dwTT > 12) {
				tiack = 1;
			}
			else {
				memset(cbTimeVal, 0, sizeof cbTimeVal);
				memcpy(cbTimeVal, &TimeVal[4], 2);
				dwTT = atoi(cbTimeVal);
				dwTime.wDay = dwTT;
				if (dwTT < 1 || dwTT > 31) {
					tiack = 1;
				}
				else {
					memset(cbTimeVal, 0, sizeof cbTimeVal);
					memcpy(cbTimeVal, &TimeVal[6], 2);
					dwTT = atoi(cbTimeVal);
					dwTime.wHour = dwTT;
					if (dwTT < 0 || dwTT > 24) {
						tiack = 1;
					}
					else {
						memset(cbTimeVal, 0, sizeof cbTimeVal);
						memcpy(cbTimeVal, &TimeVal[8], 2);
						dwTT = atoi(cbTimeVal);
						dwTime.wMinute = dwTT;
						if (dwTT < 0 || dwTT > 59) {
							tiack = 1;
						}
						else {
							memset(cbTimeVal, 0, sizeof cbTimeVal);
							memcpy(cbTimeVal, &TimeVal[10], 2);
							dwTT = atoi(cbTimeVal);
							dwTime.wSecond = dwTT;
							if (dwTT < 0 || dwTT > 59) {
								tiack = 1;
							}
							dwTime.wMilliseconds = 0;
						}
					}
				}
			}
		}
	}
	else {
		memset(cbTimeVal, 0, sizeof cbTimeVal);
		memcpy(cbTimeVal, TimeVal, 4);
		dwTT = atoi(cbTimeVal);
		dwTime.wYear = (WORD)dwTT;
		if (dwTime.wYear > 2099 || dwTime.wYear < 2000) {
			tiack = 1;
		}
		else {
			memset(cbTimeVal, 0, sizeof cbTimeVal);
			memcpy(cbTimeVal, &TimeVal[4], 2);
			dwTT = atoi(cbTimeVal);
			dwTime.wMonth = dwTT;
			if (dwTT < 1 || dwTT > 12) {
				tiack = 1;
			}
			else {
				memset(cbTimeVal, 0, sizeof cbTimeVal);
				memcpy(cbTimeVal, &TimeVal[6], 2);
				dwTT = atoi(cbTimeVal);
				dwTime.wDay = dwTT;
				if (dwTT < 1 || dwTT > 31) {
					tiack = 1;
				}
				else {
					memset(cbTimeVal, 0, sizeof cbTimeVal);
					memcpy(cbTimeVal, &TimeVal[8], 2);
					dwTT = atoi(cbTimeVal);
					dwTime.wHour = dwTT;
					if (dwTT < 0 || dwTT > 24) {
						tiack = 1;
					}
					else {
						memset(cbTimeVal, 0, sizeof cbTimeVal);
						memcpy(cbTimeVal, &TimeVal[10], 2);
						dwTT = atoi(cbTimeVal);
						dwTime.wMinute = dwTT;
						if (dwTT < 0 || dwTT > 59) {
							tiack = 1;
						}
						else {
							memset(cbTimeVal, 0, sizeof cbTimeVal);
							memcpy(cbTimeVal, &TimeVal[12], 2);
							dwTT = atoi(cbTimeVal);
							dwTime.wSecond = dwTT;
							if (dwTT < 0 || dwTT > 59) {
								tiack = 1;
							}
							dwTime.wMilliseconds = 0;
						}
					}
				}
			}
		}
	}
	if (!tiack) {
		if (SetLocalTime(&dwTime)) {
			::PostMessage(HWND_BROADCAST, WM_TIMECHANGE, 0, 0);
		}
	}
	return tiack;
}

void CGemFactory::GetSvData(vector < UINT >& Svids, SecsMessage& OutMsg) {
	vector < SVID_VALUE > dSvval;
	int i;
	if (0 == Svids.size()) {
		// Fill all SVIDS
		CEIDCtlr.GetAllSVIDs(Svids);
	}
	// Process each SVID
	SecsItemList* pSvData = new SecsItemList;
	for (i = 0; i < int(Svids.size()); i++) {
		if (CEIDCtlr.chk_Vid(Svids[i])) // Check if valid SVID
		{
			pSvData->AddItem(get_svval(1, Svids[i], NULL));
		}
		else {
			// Vijay:  Was zero length ASCII should be Zero Length List [11/14/2002]
			pSvData->AddItem(new SecsItemList);
		}
	}
	OutMsg.AddItem(pSvData);
}

void CGemFactory::GetEcData(vector < UINT >& Ecids, SecsMessage& OutMsg) {
	CEvt ehead;
	ehead.eventid = 0;
	SecsItemList* pList = new SecsItemList;
	int i;
	if (0 == Ecids.size()) {
		// Fill all Ecids
		for (i = 0; i < SVID_LIST::LASTSVID; i++) {
			if (CEIDCtlr.m_SVList.m_Svid[i].m_bVarType == SVID_ITEM::EC_TYPE) {
				Ecids.push_back(CEIDCtlr.m_SVList.m_Svid[i].m_svidno);
			}
		}
	}
	// Process each ECID
	SVID_LIST* p = &CEIDCtlr.m_SVList;
	for (i = 0; i < int(Ecids.size()); i++) {
		pList->AddItem(new SecsItemASCII(p->m_Svid[Ecids[i]].GetVStr()));
	}
	OutMsg.AddItem(pList);
}

int CGemFactory::mk_S3F26(GEM_HEADER* pHeader) {
	LOG_INFO("(S3F25) Port group request");
	int retc = 0;
	MessageS3F25 Msg;
	SecsMessage OutMsg;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		string Action = Msg.PortAction;
		int nPort = Msg.nPort - 1;
		int nAck;
		if ((nPort < 0) || (1 < nPort)) {
			nAck = 3;
		}
		else {
			nAck = pGDev->pGEMObjs->Cms[nPort].CarryOutPortAction(Action.data() /*, pGDev->pDataBank->GetTempCarrier()*/);
		}
		SecsItemList* pList = new SecsItemList;
		pList->AddItem(new SecsItemUInt1(nAck));
		pList->AddItem(new SecsItemList);
		OutMsg.AddItem(pList);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 3, 26, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S3F25) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

// YFTD: CHECK
int CGemFactory::mk_S3F24(GEM_HEADER* pHeader) {
	LOG_INFO("(S3F23) Port group action request");
	int retc = 0;
	UINT Len = AtoUint(pHeader->m_DataLen) - 10;
	MessageS3F23 Msg;
	SecsMessage OutMsg;
	string PGaction, PgName;
	PortGroup PGrp;
	vector < BYTE > PortList;
	vector < int > ErrList;
	string ErrStr;
	BYTE nPort, CAACK = 0;
	string MsgStr = "( CMS Service : ";
	char Buf[52];
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetPortGroupName(PgName);
		Msg.GetPGrpAction(PGaction);
		SecsItemList* pTopList = new SecsItemList;
		SecsItemList* pRes = new SecsItemList;
		sprintf(Buf, "PGRP (%s) )", PgName.c_str());
		MsgStr += (PGaction + " " + Buf);
		if (pGDev->GetAssociatedPorts(PgName, PortList)) {
			for (int nIndex = 0; nIndex < int(PortList.size()); nIndex++) {
				nPort = PortList[nIndex] - 1;
				pGDev->pGEMObjs->Cms[nPort].CarryOutPortAction(PGaction.c_str() /*, pGDev->pDataBank->GetTempCarrier()*/);
			}
		}
		else {
			SecsItemList* pList = new SecsItemList;
			pList->AddItem(new SecsItemUInt2(21));
			ErrorMessages::GetErrTxt(21, ErrStr);
			pList->AddItem(new SecsItemASCII(ErrStr));
			CAACK = 3;
			pTopList->AddItem(pList);
		}
		pRes->AddItem(new SecsItemUInt1(CAACK));
		pRes->AddItem(pTopList);
		OutMsg.AddItem(pRes);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 3, 24, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		LOG_INFO(MsgStr);
		return retc;
	}
	LOG_ERR("(S3F23) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

// Port group define
int CGemFactory::mk_S3F22(GEM_HEADER* pHeader) {
	LOG_INFO("(S3F21) Port group definition");
	int retc = 0;
	MessageS3F21 Msg;
	string PORTACCESS;
	vector < int > ErrList;
	PortGroup PGrp;
	BYTE CAACK = 0;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		Msg.GetPortGroupName(PGrp.PGPNAME);
		Msg.GetPortNumbers(PGrp.PortList);
		Msg.GetPortAccess(PORTACCESS);
		BYTE nPort;
		SecsItemList* pTopList = new SecsItemList;
		SecsItemList* pRes = new SecsItemList;
		if (PORTACCESS.empty()) {
			ErrList.push_back(18);
		}
		for (int nIndex = 0; nIndex < int(PGrp.PortList.size()); nIndex++) {
			nPort = PGrp.PortList[nIndex] - 1;
			if ((nPort < 0) || (1 < nPort)) {
				ErrList.push_back(21);
			}
		}
		if (ErrList.empty()) {
			for (int nIndex = 0; nIndex < int(PGrp.PortList.size()); nIndex++) {
				nPort = PGrp.PortList[nIndex] - 1;
				pGDev->pGEMObjs->Cms[nPort].CarryOutPortAction(PORTACCESS.c_str() /*, pGDev->pDataBank->GetTempCarrier()*/);
			}
			pGDev->AddPortGroup(PGrp);
		}
		else {
			string ErrMsg;
			for (int nIndex = 0; nIndex < int(ErrList.size()); nIndex++) {
				SecsItemList* pList = new SecsItemList;
				pList->AddItem(new SecsItemInt2(ErrList[nIndex]));
				ErrorMessages::GetErrTxt(ErrList[nIndex], ErrMsg);
				pList->AddItem(new SecsItemASCII(ErrMsg));
				pTopList->AddItem(pList);
			}
			CAACK = 3;
		}
		pRes->AddItem(new SecsItemUInt1(CAACK));
		pRes->AddItem(pTopList);
		SecsMessage OutMsg;
		OutMsg.AddItem(pRes);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 3, 22, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		return retc;
	}
	LOG_ERR("(S3F21) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

void CGemFactory::GetSvStat(vector < UINT >& Svids, SecsMessage& OutMsg) {
	UINT i;
	BOOL bFound = FALSE;
	if (0 == Svids.size()) {
		CEIDCtlr.GetAllSVIDs(Svids);
	}
	SecsItemList* pSvData = new SecsItemList;
	for (i = 0; i < Svids.size(); i++) {
		SecsItemList* pSvStat = new SecsItemList;
		if (CEIDCtlr.chk_svid(Svids[i])) {
			for (int j = 0; j < SVID_LIST::LASTSVID; j++) {
				if (Svids[i] == CEIDCtlr.m_SVList.m_Svid[j].m_svidno) {
					pSvStat->AddItem(new SecsItemUInt4(Svids[i]));
					pSvStat->AddItem(new SecsItemASCII(CEIDCtlr.m_SVList.m_Svid[j].m_svname));
					UINT cTemp = atoi(CEIDCtlr.m_SVList.m_Svid[j].m_unit);
					pSvStat->AddItem(new SecsItemASCII(CEIDCtlr.SvidUnitString(cTemp)));
					bFound = TRUE;
					break;
				}
			}
		}
		else {
			pSvStat->AddItem(new SecsItemUInt4(Svids[i]));
			pSvStat->AddItem(new SecsItemASCII(""));
			pSvStat->AddItem(new SecsItemASCII(""));
			bFound = TRUE;
		}
		if (bFound) {
			pSvData->AddItem(pSvStat);
			bFound = FALSE;
		}
		else {
			delete pSvData; // Avoid memory leakages
		}
	}
	OutMsg.AddItem(pSvData);
}

void CGemFactory::GetCurrTime(string& TimeStr) {
	char dtime[25];
	SYSTEMTIME CurTime;
	::GetLocalTime(&CurTime);
	UINT nYear = CurTime.wYear;
	if (0 == TimeFormat) {
		nYear %= 1000;
	}
	memset(dtime, ' ', sizeof dtime);
	if (0 == TimeFormat) {
		// 12 digit format
		sprintf(dtime, "%0.2d%0.2d%0.2d%0.2d%0.2d%0.2d", nYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond);
	}
	else {
		sprintf(dtime, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d%0.2d", nYear, CurTime.wMonth, CurTime.wDay, CurTime.wHour, CurTime.wMinute, CurTime.wSecond, CurTime.wMilliseconds / 10);
	}
	TimeStr = dtime;
}

void CGemFactory::GetEcnrData(vector < UINT >& EcidLst, SecsMessage& OutMsg) {
	char ecname[128] = { 0 };
	char ecmin[128] = { 0 };
	char ecmax[128] = { 0 };
	char ecsize[128] = { 0 };
	char ecval[128] = { 0 };
	char ecunit[128] = { 0 };
	double scmin = 9.99999E-99, scmax = 9.99999E+99;
	SecsItemList* pList = new SecsItemList;

	SVID_LIST* p = &CEIDCtlr.m_SVList;
	for (int i = 0; i < int(EcidLst.size()); i++) {
		SecsItemList* pItemList = new SecsItemList;
		short idx = p->GetIndex(EcidLst[i]);
		switch (idx) {
		case SVID_LIST::DEVICEID:
		case SVID_LIST::SOFTREV:    // OK [7/31/2010 XPMUser]
		case SVID_LIST::ESTABLISHCOMMTIMEOUT:
			strscpy(ecname, sizeof(ecname), p->m_Svid[idx].m_svname);
			strscpy(ecmin, sizeof(ecmin), p->m_Svid[idx].GetRMinChar());
			strscpy(ecmax, sizeof(ecmax), p->m_Svid[idx].GetRMaxChar());
			strscpy(ecsize, sizeof(ecsize), p->m_Svid[idx].GetLengthChar());
			strscpy(ecval, sizeof(ecval), p->m_Svid[idx].GetVChar());
			strscpy(ecunit, sizeof(ecunit), p->m_Svid[idx].GetUnitStr());
			break;
		case SVID_LIST::MAXLOADPORT:
			pItemList->AddItem(new SecsItemUInt4(EcidLst[i]));
			pItemList->AddItem(new SecsItemASCII(p->m_Svid[idx].m_svname));
			pItemList->AddItem(new SecsItemASCII("1"));
			pItemList->AddItem(new SecsItemASCII("2"));
			pItemList->AddItem(new SecsItemASCII(Var.nLoadPort));
			pItemList->AddItem(new SecsItemASCII(p->m_Svid[idx].GetUnitStr()));
			pList->AddItem(pItemList);
			continue;
			// 		case SVID_LIST::SPOOLCOUNTTOTAL: // OK, MaxSpoolTransmit
			// 			strscpy(ecname,sizeof(ecname), p->m_Svid[idx].m_svname);
			// 			strscpy(ecmin,sizeof(ecmin),p->m_Svid[idx].iGetRMinChar());
			// 			strscpy(ecmax,sizeof(ecmax),p->m_Svid[idx].iGetRMaxChar());
			// 			strscpy(ecsize,sizeof(ecsize),p->m_Svid[idx].GetLengthChar());
			// 			sprintf(ecval,"%d",(CSpoolMan::GetSpool())->SpooledSoFar());
			// 			strscpy(ecunit,sizeof(ecunit),p->m_Svid[idx].GetUnitStr());
			// 			break;
			// 		case SVID_LIST::ESTABLISHCOMMTIMEOUT: // OK, EstablishCommunicationTimeout
			// 			strscpy(ecname,sizeof(ecname), p->m_Svid[idx].m_svname);
			// 			strscpy(ecmin,sizeof(ecmin),p->m_Svid[idx].iGetRMinChar());
			// 			strscpy(ecmax,sizeof(ecmax),p->m_Svid[idx].iGetRMaxChar());
			// 			strscpy(ecsize,sizeof(ecsize),p->m_Svid[idx].GetLengthChar());
			// 			sprintf(ecval,"%d",EstablishCommunicationTimeout);
			// 			strscpy(ecunit,sizeof(ecunit),p->m_Svid[idx].GetUnitStr());
			// 			break;
			// 		case SVID_LIST::CONNECTIONMODE:  // OK [8/1/2010 XPMUser]
			// 			strscpy(ecname,sizeof(ecname), p->m_Svid[idx].m_svname);
			// 			strscpy(ecmin,sizeof(ecmin),p->m_Svid[idx].iGetRMinChar());
			// 			strscpy(ecmax,sizeof(ecmax),p->m_Svid[idx].iGetRMaxChar());
			// 			strscpy(ecsize,sizeof(ecsize),p->m_Svid[idx].GetLengthChar());
			// 			sprintf(ecval,"%d",m_ConnectionMode);
			// 			strscpy(ecunit,sizeof(ecunit),p->m_Svid[idx].GetUnitStr());
			// 			break;
			// 		case SVID_LIST::SPOOLOVERIDE: // OK, OverwriteSpool
			// 			strscpy(ecname,sizeof(ecname), p->m_Svid[idx].m_svname);
			// 			strscpy(ecmin,sizeof(ecmin),p->m_Svid[idx].iGetRMinChar());
			// 			strscpy(ecmax,sizeof(ecmax),p->m_Svid[idx].iGetRMaxChar());
			// 			strscpy(ecsize,sizeof(ecsize),p->m_Svid[idx].GetLengthChar());
			// 			sprintf(ecval,"%d",CSpoolMan::GetSpool()->OverWriteSpool);
			// 			strscpy(ecunit,sizeof(ecunit),p->m_Svid[idx].GetUnitStr());
			// 			break;
		case SVID_LIST::TIMEFORMAT: // OK, TimeFormat
			strscpy(ecname, sizeof(ecname), p->m_Svid[idx].m_svname);
			strscpy(ecmin, sizeof(ecmin), p->m_Svid[idx].GetRMinChar());
			strscpy(ecmax, sizeof(ecmax), p->m_Svid[idx].GetRMaxChar());
			strscpy(ecsize, sizeof(ecsize), p->m_Svid[idx].GetLengthChar());
			sprintf(ecval, "%d", TimeFormat);
			strscpy(ecunit, sizeof(ecunit), p->m_Svid[idx].GetUnitStr());
			break;
		case SVID_LIST::PASSIVESOCKETPORT:
			strscpy(ecname, sizeof(ecname), p->m_Svid[idx].m_svname);
			strscpy(ecmin, sizeof(ecmin), p->m_Svid[idx].GetRMinChar());
			strscpy(ecmax, sizeof(ecmax), p->m_Svid[idx].GetRMaxChar());
			strscpy(ecsize, sizeof(ecsize), p->m_Svid[idx].GetLengthChar());
			strscpy(ecval, sizeof(ecval), CEIDCtlr.m_SecsParam.Param[CSecsParam::spPASSIVEPORT].GetiValueChar());
			strscpy(ecunit, sizeof(ecunit), p->m_Svid[idx].GetUnitStr());
			break;
		case SVID_LIST::ACTIVESOCKETPORT:
			strscpy(ecname, sizeof(ecname), p->m_Svid[idx].m_svname);
			strscpy(ecmin, sizeof(ecmin), p->m_Svid[idx].GetRMinChar());
			strscpy(ecmax, sizeof(ecmax), p->m_Svid[idx].GetRMaxChar());
			strscpy(ecsize, sizeof(ecsize), p->m_Svid[idx].GetLengthChar());
			strscpy(ecval, sizeof(ecval), CEIDCtlr.m_SecsParam.Param[CSecsParam::spACTIVEPORT].GetiValueChar());
			strscpy(ecunit, sizeof(ecunit), p->m_Svid[idx].GetUnitStr());
			break;
			// 		case SVID_LIST::ACTIVEIPADDRESS:
			// 			strscpy(ecname,sizeof(ecname), p->m_Svid[idx].m_svname);
			// 			strscpy(ecmin,sizeof(ecmin),p->m_Svid[idx].iGetRMinChar());
			// 			strscpy(ecmax,sizeof(ecmax),p->m_Svid[idx].iGetRMaxChar());
			// 			strscpy(ecsize,sizeof(ecsize),p->m_Svid[idx].GetLengthChar());
			// 			strscpy(ecval,sizeof(ecval),CEIDCtlr.m_SecsParam.Param[CSecsParam::spACTIVEIP].cValue);
			// 			strscpy(ecunit,sizeof(ecunit),p->m_Svid[idx].GetUnitStr());
			// 			break;
			// 		case SVID_LIST::LINEMODEENUM:
			// 			pItemList->AddItem(new SecsItemUInt4(EcidLst[i]));
			// 			pItemList->AddItem(new SecsItemASCII(p->m_Svid[idx].m_svname));
			// 			pItemList->AddItem(new SecsItemASCII("OFFLINE EQUIPMENT"));
			// 			pItemList->AddItem(new SecsItemASCII("OFFLINE HOST"));
			// 			pItemList->AddItem(new SecsItemASCII("ONLINE LOCAL"));
			// 			pItemList->AddItem(new SecsItemASCII("ONLINE REMOTE"));
			// 			pList->AddItem(pItemList);
			// 			continue;
			// 		case SVID_LIST::EQUIPMENTSTATEENUM:
			// 			Log("Equipment State Enum not implemented",0);
			// 			continue;
			// 		case SVID_LIST::LPOCCUPANCYSTATEENUM:
			// 			Log("Occupancy State Enum not implemented",0);
			// 			continue;
			// 		case SVID_LIST::LPACCESSSTATEENUM:
			// 			Log("Access State Enum not implemented",0);
			// 			continue;
			// 		case SVID_LIST::LPTRANSFERSTATEENUM:
			// 			Log("Transfer State Enum not implemented",0);
			// 			continue;
			// 		case SVID_LIST::LPASSOCIATESTATEENUM:
			// 			Log("Association State Enum not implemented",0);
			// 			continue;
			// 		case SVID_LIST::LPRESERVATIONSTATEENUM:
			// 			Log("Reservation State Enum not implemented",0);
			// 			continue;

			// 		case SVID_LIST::DEFAULTONLINESTATE:
			// 			pItemList->AddItem(new SecsItemUInt4(EcidLst[i]));
			// 			pItemList->AddItem(new SecsItemASCII(p->m_Svid[idx].m_svname));
			// 			pItemList->AddItem(new SecsItemASCII("0"));
			// 			pItemList->AddItem(new SecsItemASCII("1"));
			// 			pItemList->AddItem(new SecsItemASCII("0"));
			// 			pItemList->AddItem(new SecsItemASCII(""));
			// 			pList->AddItem(pItemList);
			// 			continue;
			// 		case SVID_LIST::DEFAULTOFFLINESTATE:
			// 			pItemList->AddItem(new SecsItemUInt4(EcidLst[i]));
			// 			pItemList->AddItem(new SecsItemASCII(p->m_Svid[idx].m_svname));
			// 			pItemList->AddItem(new SecsItemASCII("0"));
			// 			pItemList->AddItem(new SecsItemASCII("1"));
			// 			pItemList->AddItem(new SecsItemASCII("0"));
			// 			pItemList->AddItem(new SecsItemASCII(""));
			// 			pList->AddItem(pItemList);
			// 			continue;
		default:
			TRACE1("SVID: %d not implemented\n", idx);
			pItemList->AddItem(new SecsItemUInt4(EcidLst[i]));
			pItemList->AddItem(new SecsItemASCII(p->m_Svid[idx].m_svname));
			pItemList->AddItem(new SecsItemASCII(""));
			pItemList->AddItem(new SecsItemASCII(""));
			pItemList->AddItem(new SecsItemASCII(""));
			pItemList->AddItem(new SecsItemASCII("?"));
			pList->AddItem(pItemList);
			ASSERT(0);
			continue;
		}
		pItemList->AddItem(new SecsItemUInt4(EcidLst[i]));
		pItemList->AddItem(new SecsItemASCII(ecname));
		pItemList->AddItem(new SecsItemASCII(ecmin));
		pItemList->AddItem(new SecsItemASCII(ecmax));
		pItemList->AddItem(new SecsItemASCII(ecval));
		pItemList->AddItem(new SecsItemASCII(ecunit));
		pList->AddItem(pItemList);
	}
	OutMsg.AddItem(pList);
}

int CGemFactory::mk_S10F7(GEM_HEADER* shead) {
	LOG_INFO("Send: S10F7 Multi-block not allowed MNN");

	int retc;
	BYTE TID = 0;
	SecsMessage OutMsg;
	OutMsg.AddItem(new SecsItemBinary(&TID, 1));
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(0, 10, 7, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP); // sizeof(SECS_HEAD) +);
	SendBlock(shead);
	return retc;
}

void CGemFactory::GetObjectAttr(UINT nType, vector < string >& ObjID, vector < string >& AttrIDs, SecsMessage& OutMsg) {
	BYTE ObjAck = 0;
	SecsItemList* pMainList = new SecsItemList;
	SecsItemList* pErrList = new SecsItemList;
	BOOL bObjFound = FALSE, TypeFound = TRUE;
	CString MsgStr;

	if (nType < 0 || nType>4) {
		ObjAck = 1;
		pErrList->AddItem(MakeErrorList(UNKNOWN_OBJECT_TYPE));
		MsgStr.Format("GetObjectAttr Rejected, Unknown Objecttype <%d>", nType);
		LogMessages(MsgStr);
	}
	else {
		if (0 == ObjID.size()) {
			if (!GetAllObjectIDs(nType, ObjID)) {
				ObjAck = 1;
				pErrList->AddItem(MakeErrorList(UNKNOWN_E5_ERROR));
				MsgStr.Format("GetObjectAttr Rejected, Getting all objects fail");
				LogMessages(MsgStr);
				return;
			}
		}
		if (0 == AttrIDs.size()) {
			if (!GetAllAttributes(nType, AttrIDs)) {
				// Unknown Object type But should not be here
				// Already checked above
				MsgStr.Format("GetObjectAttr Rejected, Getting all attributes fail ");
				LogMessages(MsgStr);
				ASSERT(0);
				return;
			}
		}
		for (int nIdx = 0; nIdx < int(ObjID.size()); nIdx++) {
			SecsItemList* pObjList = new SecsItemList;
			SecsItemList* pAttrList = new SecsItemList;
			pObjList->AddItem(new SecsItemASCII(ObjID[nIdx]));
			pObjList->AddItem(pAttrList);
			pMainList->AddItem(pObjList);
			if (IsObject(nType, ObjID[nIdx])) {
				for (int nIdx2 = 0; nIdx2 < int(AttrIDs.size()); nIdx2++) // Each Attribute
				{
					SecsItemList* pAttr = new SecsItemList;
					// Check m_pDataBank->
					pAttr->AddItem(new SecsItemASCII(AttrIDs[nIdx2]));
					if (GetObjectAttr(nType, ObjID[nIdx], AttrIDs[nIdx2], pAttr)) {
						pAttrList->AddItem(pAttr);
					}
					else {
						delete pAttr;
						ObjAck = 1;
						pErrList->AddItem(MakeErrorList(UNKNOWN_ATTRIBUTE_NAME));
						//CString MsgStr;
						MsgStr.Format("GetObjectAttr Rejected, Unknown Attribute %s", (char*)AttrIDs[nIdx2].c_str());
						LogMessages(MsgStr);
					}
				}
			}
			else {
				ObjAck = 1;
				pErrList->AddItem(MakeErrorList(UNKNOWN_OBJECT_INSTANCE));
				//CString MsgStr;
				MsgStr.Format("GetObjectAttr Rejected, Unknown Object %s", (char*)ObjID[nIdx].c_str());
				LogMessages(MsgStr);
			}
			// Get Each process Job ID
		}
	}
	SecsItemList* pAckList = new SecsItemList;
	pAckList->AddItem(new SecsItemUInt1(ObjAck));
	pAckList->AddItem(pErrList);
	SecsItemList* pList = new SecsItemList;
	pList->AddItem(pMainList);
	pList->AddItem(pAckList);
	OutMsg.AddItem(pList);
}

BOOL CGemFactory::GetPrJobAttr(string PrJobId, string AttrID, SecsItemList* pList) {
	CPRJob* p = PJobMgr.GetPRJob((char*)PrJobId.c_str());
	if (p) {
		return p->GetAttr(AttrID, pList);
	}
	return FALSE;
}

BOOL CGemFactory::GetPrJobObj(string& PrJobId) {
	if (PJobMgr.GetPRJob((char*)PrJobId.c_str())) {
		return TRUE;
	}
	return FALSE;
}

BOOL CGemFactory::GetCjobObj(string& CJobId) {
	if (CJobMgr.GetCJob((char*)CJobId.c_str())) {
		return TRUE;
	}
	if (CJobMgr.GetCmplCJob((char*)CJobId.c_str())) {
		return TRUE;
	}
	return FALSE;
}

BOOL CGemFactory::GetCjobAttr(const string& CJobId, string AttrID, SecsItemList* pList) {
	CCtrlJob* cJob = CJobMgr.GetCJob((char*)CJobId.c_str());
	if (cJob) {
		return cJob->GetAttr(AttrID, pList);
	}
	cJob = CJobMgr.GetCmplCJob((char*)CJobId.c_str());
	if (cJob) {
		return cJob->GetAttr(AttrID, pList);
	}
	pList->AddItem(new SecsItemASCII(""));
	return FALSE;
}

BOOL CGemFactory::GetCarrierObj(string& CarrierID) {
	for (int i = 0; i < sizeof(pGDev->pGEMObjs->Cms) / sizeof(pGDev->pGEMObjs->Cms[0]); i++) {
		if (CarrierID.compare(pGDev->pGEMObjs->Cms[i].CaObj.ObjID) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGemFactory::GetCarrierAttr(string CarrierID, string AttrID, SecsItemList* pList) {
	for (int i = 0; i < sizeof(pGDev->pGEMObjs->Cms) / sizeof(pGDev->pGEMObjs->Cms[0]); i++) {
		if (CarrierID.compare(pGDev->pGEMObjs->Cms[i].CaObj.ObjID) == 0) {
			pGDev->pGEMObjs->Cms[i].GetAttr((char*)AttrID.data(), pList);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CGemFactory::GetSubstObj(string& SubstID) {
	return pGDev->IsSubstrate(SubstID);
}

BOOL CGemFactory::GetSubstLocObj(string& SubstLocID) {
	return pGDev->IsSubstrateLoc(SubstLocID);
}

BOOL CGemFactory::GetSubstAttr(string SubstID, string AttrID, SecsItemList* pList) {
	return NO_E5_ERROR == pGDev->GetSubstAttr(SubstID, AttrID, pList);
}

BOOL CGemFactory::GetSubstLocAttr(string SubstLocID, string AttrID, SecsItemList* pList) {
	return NO_E5_ERROR == pGDev->GetSubstLocAttr(SubstLocID, AttrID, pList);
}

int CGemFactory::SetObjectAttr(UINT nType, vector < string >& ObjID, vector < string >& AttrIDs,
	vector < SecsItem* > AttrValue, SecsItemList* pResultList, SecsItemList* pErrorList) {
	UINT nResult = 0;
	for (int nIdx = 0; nIdx < int(ObjID.size()); nIdx++) {
		SecsItemList* pObjList = new SecsItemList;
		pObjList->AddItem(new SecsItemASCII(ObjID[nIdx]));
		// Get Each process Job ID
		SecsItemList* pAttrList = new SecsItemList;
		for (int nIdx2 = 0; nIdx2 < int(AttrIDs.size()); nIdx2++) // Each Attribute
		{
			SecsItemList* pAttr = new SecsItemList;
			SecsItem* pAttrValue = AttrValue[nIdx2];
			// Check m_pDataBank->
			pAttr->AddItem(new SecsItemASCII(AttrIDs[nIdx2]));
			BOOL bResult;
			switch (nType) {
				// case 0: bResult = SetPrJobAttr(ObjID[nIdx], AttrIDs[nIdx2], pAttrValue); break;
			case 2:
				bResult = FALSE;
				break;
			case 3:
				bResult = SetSubstAttr(ObjID[nIdx], AttrIDs[nIdx2], pAttrValue);
				break;
			default:
				bResult = FALSE;
			}
			if (bResult) {
				pAttr->AddItem(pAttrValue);
				pAttrList->AddItem(pAttr);
			}
			else {
				nResult = 1;
				delete pAttr;
				SecsItemList* pList = new SecsItemList;
				pList->AddItem(new SecsItemUInt1(4));
				pList->AddItem(new SecsItemASCII("No Attributes Found"));
				pErrorList->AddItem(pList);
			}
		}
		pObjList->AddItem(pAttrList);
		pResultList->AddItem(pObjList);
	}
	SecsItemList* pAckList = new SecsItemList;
	pAckList->AddItem(new SecsItemUInt1(nResult));
	pAckList->AddItem(pErrorList);
	return nResult;
}

int CGemFactory::SetSubstAttr(string SubstID, string AttrID, SecsItem* pAttrValue) {
	for (int i = 0; i < sizeof(pGDev->pGEMObjs->Cms) / sizeof(pGDev->pGEMObjs->Cms[0]); i++) {
		for (int j = 0; j < pGDev->pGEMObjs->Cms[i].CaObj.Capacity; j++) {
			if (SubstID.compare(pGDev->pGEMObjs->Cms[i].CaObj.Substrate[j].ObjID) == 0) {
				string data;
				pAttrValue->GetData(data);
				pGDev->pGEMObjs->Cms[i].CaObj.Substrate[j].SetAttr((char*)AttrID.data(), (char*)data.data());
				return 1;
			}
		}
	}
	return 0;
}
BOOL CGemFactory::LogMessages(string Message, char* FileName, int Line, MsgType Type) {
	CSMClt ipcLog;
	if (2045 > Message.size()) {
		pGDev->SECSMessage(Message.c_str());
	}
	return TRUE;
}

BOOL CGemFactory::LogMessages(CString& Message) {
	string str;
	str = Message;
	return LogMessages(str);
	return TRUE;
}

int CGemFactory::mk_S3F28(GEM_HEADER* pHeader) {
	LOG_INFO("(S3F27) Change access");
	int retc = 0;
	MessageS3F27 Msg;
	BYTE CAACK = 0;
	int Temp = 0;
	vector < int > ErrorCodes;
	string MsgStr = "( CMS Service : ";
	vector < UINT > Ports;
	if (Msg.CheckMessage(pHeader->m_DataLen, pHeader->m_Message)) {
		if (Msg.Ports.empty()) {
			for (int nPort = 0; nPort < int(Var.nLoadPort); nPort++) {
				Ports.push_back(nPort + 1);
			}
		}
		else {
			Ports = Msg.Ports;
		}
		int nIndex;
		for (nIndex = 0; nIndex < int(Ports.size()); nIndex++) {
			if (Ports[nIndex] < 1 || Ports[nIndex] > UINT(Var.nLoadPort)) {
				CAACK = 1;
				ErrorCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
			}
		}
		UINT PAct = Msg.PortAction;
		if (PAct != 0 && PAct != 1) {
			CAACK = 1;
			ErrorCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
		}
		MsgStr += "( ";
		string Action;
		switch (PAct) {
		case 0:
			Action = "MANUAL";
			break;
		case 1:
			Action = "AUTO";
			break;
		default:
			Action = "Unknown";
			break;
		}
		MsgStr += Action + ") ";
		SecsItemList* pListTop = new SecsItemList;
		SecsItem* pAck = new SecsItemUInt1(CAACK);
		SecsItemList* pErrList = new SecsItemList;
		string strTemp;
		for (nIndex = 0; nIndex < int(ErrorCodes.size()); nIndex++) {
			SecsItem* pTemp = new SecsItemUInt1(ErrorCodes[nIndex]);
			ErrorMessages::GetErrTxtE5(ErrorCodes[nIndex], strTemp);
			SecsItem* pTempTxt = new SecsItemASCII(strTemp);
			SecsItemList* pList = new SecsItemList;
			SecsItemUInt1* pPort = new SecsItemUInt1(1); // Remove Hard coded value
			pList->AddItem(pPort);
			pList->AddItem(pTemp);
			pList->AddItem(pTempTxt);
			pErrList->AddItem(pList);
		}
		pListTop->AddItem(pAck);
		pListTop->AddItem(pErrList);
		SecsMessage OutMsg;
		OutMsg.AddItem(pListTop);
		OutMsg.MakeBinaryMessage(pHeader->m_Message);
		retc = set_head(0, 3, 28, pHeader, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(pHeader);
		if (ErrorCodes.size()) {
			MsgStr += "Err (" + strTemp + ")  Fail";
			LOG_ERR(MsgStr);
		}
		else {
			for (/*int */nIndex = 0; nIndex < int(Ports.size()); nIndex++) {
				pGDev->pGEMObjs->Cms[Ports[nIndex] - 1].CarryOutPortAction(Action.c_str());
			}
			MsgStr += "Success";
			LOG_INFO(MsgStr);
		}
		return retc;
	}
	LOG_ERR("(S3F27) Incorrect Format");
	mk_S9F7(pHeader);
	return retc;
}

int CGemFactory::Stream7(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(7, dwFunction)) {
		mk_S07F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 1:
		mk_S7F2(pHeader);
		break;
	case 3:
		mk_S7F4(pHeader);
		break;
	case 5:
		mk_S7F6(pHeader);
		break;
	case 17:
		mk_S7F18(pHeader);
		break;
	case 19:
		mk_S7F20(pHeader);
		break;
	case 25:
		mk_S7F26(pHeader);
		break;
	default:
		mk_S9F5(pHeader);
		break;
	}
	return 0;
}

// Check PPBodyLength as well, Validity of PPID, availability of PPID
int CGemFactory::mk_S7F2(GEM_HEADER* shead) {
	LOG_INFO("(S7F1) Porcess program load inquire PPI");

	int retc = 0;
	MessageS7F1 Msg;
	string MsgStr = "S7F1/F2 (PP Inquire) ";
	char Buf[52];
	BYTE PPGNT = 0;
	if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		MsgStr += ("(" + Msg.PPID + ")");
		if (Msg.PPLen > 10000) {
			sprintf(Buf, " Err (No Space) Deny");
			MsgStr += Buf;
			PPGNT = 2;
		}
		else if (/*(!RcpMgr.IsRecipeAvailable(Msg.PPID)) || (80 < Msg.PPID.size()) || */(Msg.PPID.empty())) {
			PPGNT = 3;
		}
		SecsItemBinary* pBinary = new SecsItemBinary(&PPGNT, 1);
		SecsMessage OutMsg;
		OutMsg.AddItem(pBinary);
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 7, 2, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		if (PPGNT) {
			LOG_ERR(MsgStr);
		}
		else {
			MsgStr += " Grant";
			LOG_INFO(MsgStr);
		}
		return retc;
	}
	LOG_ERR("(S7F1) Incorrect Format");
	mk_S9F7(shead);
	return retc;
}

// Yuen: Format of PPBODY is contained in Recipe.BinaryToRecipe [2002/10/31  19:44]
int CGemFactory::mk_S7F4(GEM_HEADER* shead) {
	LOG_INFO("(S7F3) Process program send PPS");

	int retc = 0;
	BYTE ACKC7 = 0;
	string MsgStr = "S7F3/F4 (PP Send) ";

	// Will deny [7/3/2012 user]
	ACKC7 = 1;	// permission deny [7/3/2012 user]
	SecsItemBinary* pBinary = new SecsItemBinary(&ACKC7, 1);
	SecsMessage OutMsg;
	OutMsg.AddItem(pBinary);
	retc = set_head(0, 7, 4, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
	SendBlock(shead);
	if (ACKC7) {
		LOG_ERR(MsgStr);
	}
	else {
		MsgStr += " Success";
		LOG_INFO(MsgStr);
	}
	return retc;

	//////////////////////////////////////////////////////////////////////////
	MessageS7F3* pMsg = new MessageS7F3;
	if (!pMsg) {
		ASSERT(0);
		return retc;
	}
	if (pMsg->CheckMessage(shead->m_DataLen, shead->m_Message)) {
		MsgStr += ("(" + pMsg->PPID + ")");
		CRecipe413* pRcp = new CRecipe413;
		if (!pRcp) {
			ASSERT(0);
		}
		if (!pRcp->BinaryToRecipe(pMsg->PPBODY, pMsg->PPLen)) {
			ACKC7 = 2;
			MsgStr += "Err (Read Fail) Fail";
		}
		{
			strscpy(pRcp->RcpeName, RCPNAMELEN, pMsg->PPID.c_str());
			// Should verify then save this recipe
			RcpMgr.Save(pRcp);
		}
		// Now we can attach this recipe to the "Process Job"
		// Yuen: S7f3 should not be tied to current downloaded PRJob
		// Yuen: Disabled [2002/11/01  3:22]
		//		if (!pGDev->pDataBank->SetRcpData(1, Rcp.RcpName, Rcp.scanno, Rcp.nlines, Rcp.scansize))
		//		{
		//			ACKC7 = 4;
		//		}
		if (!ACKC7) {
			PPExecName = pMsg->PPID;
		}
		ASSERT(!"Recipe ref removed : Needs redone");
		SecsItemBinary* pBinary1 = new SecsItemBinary(&ACKC7, 1);
		SecsMessage OutMsg1;
		OutMsg1.AddItem(pBinary1);
		OutMsg1.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 7, 4, shead, OutMsg1.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		if (ACKC7) {
			LOG_ERR(MsgStr);
		}
		else {
			MsgStr += " Success";
			LOG_INFO(MsgStr);
		}
		delete pRcp;
		delete pMsg;
		return retc;
	}
	LOG_ERR("(S7F3) Incorrect Format");
	mk_S9F7(shead);
	delete pMsg;
	return retc;
}

int CGemFactory::mk_S7F6(GEM_HEADER* shead) {
	LOG_INFO("(S7F5) Progcess program request PPR");

	int retc = 0;
	MessageS7F5* pMsg = new MessageS7F5;
	if (!pMsg) {
		ASSERT(0);
		return retc;
	}
	string MsgStr = "S7F5/F6 (PP Request) ";
	BOOL bFound = FALSE, bValid = FALSE;
	BYTE* pByte = NULL;
	if (pMsg->CheckMessage(shead->m_DataLen, shead->m_Message)) {
		CRecipe413* pRcp = new CRecipe413;
		if (!pRcp) {
			ASSERT(0);
		}
		MsgStr += ("(" + pMsg->PPID + ") ");
		if (pMsg->PPID.size() && RcpMgr.IsRecipeAvailable(pMsg->PPID)) {
			bFound = TRUE;
			CString rStr = pMsg->PPID.c_str();
			DosUtil.RemoveExtension(rStr);
			pRcp->SetRecipeName(rStr.GetBuffer(0)/*(char *)pMsg->PPID.c_str()*/);
			if (RcpMgr.Load(pRcp)) {
				bValid = TRUE;
			}
			else {
				MsgStr += "Err (Rcp Read Fail)";
			}
		}
		else
			//ASSERT(!"Recipe ref removed : Needs redone");
		{
			MsgStr += "Err (Rcp Not Found)";
		}
		// Yuen: List of zero is allowed [2002/11/01  4:36]
		SecsItemList* pList = new SecsItemList;
		if (bFound && bValid) {
			SecsItemASCII* pPPID = new SecsItemASCII(pMsg->PPID);
			pList->AddItem(pPPID);
			long len = pRcp->GetBinarySize();
			pByte = new BYTE[len];
			pRcp->RecipeToBinary(pByte, len);
			SecsItemBinary* pPPBody = new SecsItemBinary(pByte, len);
			pList->AddItem(pPPBody);
			delete[] pByte;
			pByte = NULL;
			MsgStr += " Done";
			LOG_INFO(MsgStr);
		}
		else {
			MsgStr += " Fail";
			LOG_ERR(MsgStr);
		}
		SecsMessage OutMsg;
		OutMsg.AddItem(pList);
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 7, 6, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		delete pMsg;
		delete pRcp;
		return retc;
	}
	LOG_ERR("(S7F5) Incorrect Format");
	mk_S9F7(shead);
	delete pMsg;
	return retc;
}

//@@ Vijay#3June$02 New message added
int CGemFactory::mk_S7F18(GEM_HEADER* shead) {
	LOG_INFO("(S7F18) Not supported");

	// Recipe reference removed
	int retc = 0;
	MessageS7F17* pMsg = new MessageS7F17;
	if (!pMsg) {
		ASSERT(0);
		return retc;
	}
	BYTE ACKC7 = 4;
	//CRecipe413 *pRcp = new CRecipe413;
	//if (!pRcp)
	//{
	//	ASSERT(0);
	//}
	string MsgStr = "S7F17/18 (Delete PP) ";
	if (pMsg->CheckMessage(shead->m_DataLen, shead->m_Message)) {
		MsgStr += "(";
		for (int nIndex = 0; nIndex<int(pMsg->PPIDs.size()); nIndex++) {
			MsgStr += (pMsg->PPIDs[nIndex] + ", ");
		}
		MsgStr += ")";
		// Can we perform this operation ?
		if (RcpMgr.CanDeleteRecipes(pMsg->PPIDs)) {
			// Now delete files
			if (RcpMgr.DeleteRecipes(pMsg->PPIDs)) {
				MsgStr += " Success";
				LOG_INFO(MsgStr);
				ACKC7 = 0;
			}
			else {
				MsgStr += "Fail (Could not delete Process Program(s))";
				LOG_ERR(MsgStr);
			}
		}
		else {
			MsgStr += "Err (PPID not found)";
			LOG_ERR(MsgStr);
		}
		// Create response
		SecsMessage OutMsg;
		OutMsg.AddItem(new SecsItemBinary(&ACKC7, 1));
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 7, 18, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		delete pMsg;
		//delete pRcp;
		return retc;
	}
	LOG_ERR("(S7F17) Incorrect Format");
	mk_S9F7(shead);
	delete pMsg;
	return retc;
}

//@@ Vijay#3June$02 New message added
// Only the header is received so no need to check
int CGemFactory::mk_S7F20(GEM_HEADER* shead) {
	LOG_INFO("(S7F19) Not supported");

	int retc = 0;
	MessageS7F19 Msg;
	vector < string > PPIDs;
	string MsgStr = "S7F19/20 (EPPD REQ) ";
	if (Msg.CheckMessage(shead->m_DataLen, shead->m_Message)) {
		RcpMgr.ListRecipes(PPIDs);
		SecsItemList* pRes = new SecsItemList;
		MsgStr += "(";
		for (int nIndex = 0; nIndex < int(PPIDs.size()); nIndex++) {
			pRes->AddItem(new SecsItemASCII(PPIDs[nIndex]));
			MsgStr += (PPIDs[nIndex] + ", ");
		}
		MsgStr += ")";
		SecsMessage OutMsg;
		OutMsg.AddItem(pRes);
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 7, 20, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(shead);
		LOG_INFO(MsgStr);
		return retc;
	}
	LOG_ERR("(S7F19) Incorrect Format");
	mk_S9F7(shead);
	return retc;
}

int CGemFactory::mk_S7F3(GEM_HEADER* shead, CEvt* Evt) {
	LOG_INFO("(S7F3) Not supported");
	int retc = 0;
	//	ASSERT(0);
// #if 0
// 	string PPID = (char *)Evt->GetMsg();
// 	CRecipe413 Rcp;
// 	SecsMessage OutMsg;
// 	BOOL bValid = FALSE;
// 	BYTE * pByte = NULL;
// 	string MsgStr = "S7F3 (Uploading Recipe) (" +PPID + ") ";
// 	Rcp.SetRecipeName((char *)PPID.c_str());
// 	if (RcpMgr.Load(&Rcp))
// 	{
// 		bValid = TRUE;
// 	}
// 	else
// 	{
// 		MsgStr += "Err (Invalid) ";
// 		LOG_ERR(MsgStr);
// 	}
// 	if (bValid)
// 	{
// 		SecsItemList * pList = new SecsItemList;
// 		SecsItemASCII * pPPID = new SecsItemASCII(PPID);
// 		pList->AddItem(pPPID);
// 		long len = Rcp.GetBinarySize();
// 		pByte = new BYTE[len];
// 		Rcp.RecipeToBinary(pByte, len);
// 		SecsItemBinary * pPPBody = new SecsItemBinary(pByte, len);
// 		pList->AddItem(pPPBody);
// 		delete[] pByte;
// 		pByte = NULL;
// 		OutMsg.AddItem(pList);
// 		OutMsg.MakeBinaryMessage(shead->m_Message);
// 		retc = set_head(1, 7, 3, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
// 		SendBlock(shead);
// 		MsgStr += "Success";
// 		LOG_INFO(MsgStr);
// 	}
// #endif
	return retc;
}

int CGemFactory::mk_S7F1(GEM_HEADER* shead, CEvt* Evt) {
	LOG_INFO("(S7F1) Not supported");
	int retc = 0;
	//	ASSERT(0);
	// #if 0
	// 	string PPID = (char *)Evt->GetMsg();
	// 	CRecipe413 Rcp;
	// 	SecsMessage OutMsg;
	// 	BOOL bValid = FALSE;
	//
	// 	Rcp.SetRecipeName((char *)PPID.c_str());
	// 	if (RcpMgr.Load(&Rcp))
	// 	{
	// 		bValid = TRUE;
	// 	}
	// 	if (bValid)
	// 	{
	// 		SecsItemList * pList = new SecsItemList;
	// 		SecsItemASCII * pPPID = new SecsItemASCII(PPID);
	// 		pList->AddItem(pPPID);
	// 		SecsItemUInt2 * pLen = new SecsItemUInt2(Rcp.GetBinarySize());
	// 		pList->AddItem(pLen);
	// 		OutMsg.AddItem(pList);
	// 		OutMsg.MakeBinaryMessage(shead->m_Message);
	// 		retc = set_head(1, 7, 1, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
	// 		SendBlock(shead);
	// 	}
	// #endif
	return retc;
}

//@@Vijay#4June$02 New message added
int CGemFactory::mk_S7F5(GEM_HEADER* shead, CEvt* Evt) {
	LOG_INFO("Send: S7F5 Process program request PPR");

	int retc = 0;
	string PPID = (char*)Evt->GetMsg();
	SecsMessage OutMsg;
	SecsItemASCII* pPPID = new SecsItemASCII(PPID);
	OutMsg.AddItem(pPPID);
	OutMsg.MakeBinaryMessage(shead->m_Message);
	retc = set_head(1, 7, 5, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
	SendBlock(shead);
	return retc;
}

int CGemFactory::Stream18(GEM_HEADER* pHeader) {
	int dwFunction = pHeader->m_Function;
	if (!CanCommunicate(18, dwFunction)) {
		mk_S18F00(pHeader);
		return 0;
	}
	switch (dwFunction) {
	case 1:
		mk_S18F2(pHeader);
		break;
	case 3:
		mk_S18F4(pHeader);
		break;
	case 5:
		mk_S18F6(pHeader);
		break;
	case 7:
		mk_S18F8(pHeader);
		break;
	case 9:
		mk_S18F10(pHeader);
		break;
	case 11:
		mk_S18F12(pHeader);
		break;
	case 13:
		mk_S18F14(pHeader);
		break;
	default:
	{
		char Msg[126];
		sprintf(Msg, "(S18F%d) Unknown Function", pHeader->m_Function);
		LOG_WARN(Msg);
		mk_S9F5(pHeader);
	}
	break;
	}
	return 0;
}

int CGemFactory::mk_S18F2(GEM_HEADER* GHead) {
	int nRet = 0;
	UINT Len = AtoUint(GHead->m_DataLen) - 10;
	MessageS18F1 Msg;
	if (Msg.CheckMessage(GHead->m_DataLen, GHead->m_Message)) {
		SecsMessage OutMsg;
		int HeadID = atoi(Msg.TargetID.c_str());
		SecsItemList* pAttrVals = new SecsItemList, * pRes = new SecsItemList, * pTop = new SecsItemList;
		bool bRes = pGDev->pGEMObjs->CidrSystem.GetAttributes(HeadID, Msg.AttrIDs, pAttrVals, pRes);
		string SSACK;
		pGDev->pGEMObjs->CidrSystem.GetErrCode(SSACK);
		pTop->AddItem(new SecsItemASCII(Msg.TargetID));
		pTop->AddItem(new SecsItemASCII(SSACK));
		pTop->AddItem(pAttrVals);
		pTop->AddItem(pRes);
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(GHead->m_Message);
		nRet = set_head(0, 18, 2, GHead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(GHead);
		return nRet;
	}
	LOG_ERR("(S18F1) Incorrect Format");
	mk_S9F7(GHead);
	return nRet;
}

int CGemFactory::mk_S18F4(GEM_HEADER* GHead) {
	int nRet = 0;
	MessageS18F3 Msg;
	if (Msg.CheckMessage(GHead->m_DataLen, GHead->m_Message)) {
		// All attributes are Read only so reply will always fail
		string SSACK;
		int HeadID = atoi(Msg.TargetID.c_str());
		SecsItemList* pStat = new SecsItemList, * pTop = new SecsItemList;
		bool bRes = pGDev->pGEMObjs->CidrSystem.SetAttributes(HeadID, Msg.AttrIDs, Msg.AttrVals, pStat);
		pGDev->pGEMObjs->CidrSystem.GetErrCode(SSACK);
		pTop->AddItem(new SecsItemASCII(Msg.TargetID));
		pTop->AddItem(new SecsItemASCII(SSACK));
		pTop->AddItem(pStat);
		SecsMessage OutMsg;
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(GHead->m_Message);
		nRet = set_head(0, 18, 4, GHead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(GHead);
		return nRet;
	}
	LOG_ERR("(S18F3) Incorrect Format");
	mk_S9F7(GHead);
	return nRet;
}

int CGemFactory::mk_S18F6(GEM_HEADER* GHead) {
	LOG_INFO("(S18F5) Read request RR");

	int nRet = 0;
	MessageS18F5 Msg;
	if (Msg.CheckMessage(GHead->m_DataLen, GHead->m_Message)) {
		int HeadID = atoi(Msg.TargetID.c_str()), DataSeg = atoi(Msg.DatSeg.c_str());
		string Data, SSACK;
		SecsItemList* pTop = new SecsItemList;
		bool bRes = pGDev->pGEMObjs->CidrSystem.ReadDataSeg(HeadID, Data, Msg.DataLength, DataSeg);
		pGDev->pGEMObjs->CidrSystem.GetErrCode(SSACK);
		pTop->AddItem(new SecsItemASCII(Msg.TargetID));
		pTop->AddItem(new SecsItemASCII(SSACK));
		pTop->AddItem(new SecsItemASCII(Data));
		SecsMessage OutMsg;
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(GHead->m_Message);
		nRet = set_head(0, 18, 6, GHead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(GHead);
		return nRet;
	}
	LOG_ERR("(S18F5) Incorrect Format");
	mk_S9F7(GHead);
	return nRet;
}

int CGemFactory::mk_S18F8(GEM_HEADER* GHead) {
	LOG_INFO("(S18F7) Write data request WDR");

	int nRet = 0;
	MessageS18F7 Msg;
	if (Msg.CheckMessage(GHead->m_DataLen, GHead->m_Message)) {
		int HeadID = atoi(Msg.TargetID.c_str());
		int DataSeg = atoi(Msg.DatSeg.c_str());
		SecsItemList* pTop = new SecsItemList, * pStat = new SecsItemList;
		string SSACK;
		bool bRes = pGDev->pGEMObjs->CidrSystem.WriteDataSeg(HeadID, Msg.Data, Msg.DataLength, DataSeg, pStat);
		pGDev->pGEMObjs->CidrSystem.GetErrCode(SSACK);
		pTop->AddItem(new SecsItemASCII(Msg.TargetID));
		pTop->AddItem(new SecsItemASCII(SSACK));
		pTop->AddItem(pStat);
		SecsMessage OutMsg;
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(GHead->m_Message);
		nRet = set_head(0, 18, 8, GHead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(GHead);
		return nRet;
	}
	LOG_ERR("(S18F7) Incorrect Format");
	mk_S9F7(GHead);
	return nRet;
}

int CGemFactory::mk_S18F10(GEM_HEADER* GHead) {
	LOG_INFO("(S18F9) Read ID request RR");

	int nRet = 0;
	MessageS18F9 Msg;
	if (Msg.CheckMessage(GHead->m_DataLen, GHead->m_Message)) {
		string CarrierID, SSACK;
		int HeadID = atoi(Msg.TargetID.c_str());
		SecsItemList* pTop = new SecsItemList, * pStat = new SecsItemList;
		bool bRet = pGDev->pGEMObjs->CidrSystem.ReadID(HeadID, CarrierID, pStat);
		pGDev->pGEMObjs->CidrSystem.GetErrCode(SSACK);
		pTop->AddItem(new SecsItemASCII(Msg.TargetID));
		pTop->AddItem(new SecsItemASCII(SSACK));
		pTop->AddItem(new SecsItemASCII(CarrierID));
		pTop->AddItem(pStat);
		SecsMessage OutMsg;
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(GHead->m_Message);
		nRet = set_head(0, 18, 10, GHead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(GHead);
		return nRet;
	}
	LOG_ERR("(S18F9) Incorrect Format");
	mk_S9F7(GHead);
	return nRet;
}

int CGemFactory::mk_S18F12(GEM_HEADER* GHead) {
	LOG_INFO("(S18F11) Write ID request WIR");

	int nRet = 0;
	MessageS18F11 Msg;
	if (Msg.CheckMessage(GHead->m_DataLen, GHead->m_Message)) {
		string SSACK;
		SecsItemList* pTop = new SecsItemList, * pStat = new SecsItemList;
		int HeadID = atoi(Msg.TargetID.c_str());
		bool bRet = pGDev->pGEMObjs->CidrSystem.WriteID(HeadID, Msg.MID, pStat);
		pGDev->pGEMObjs->CidrSystem.GetErrCode(SSACK);
		pTop->AddItem(new SecsItemASCII(Msg.TargetID));

		/*1*/

		pTop->AddItem(new SecsItemASCII(SSACK));

		/*2*/

		pTop->AddItem(pStat);

		/*3*/

		SecsMessage OutMsg;
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(GHead->m_Message);
		nRet = set_head(0, 18, 12, GHead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(GHead);
		return nRet;
	}
	LOG_ERR("(S18F11) Incorrect Format");
	mk_S9F7(GHead);
	return nRet;
}

int CGemFactory::mk_S18F14(GEM_HEADER* GHead) {
	LOG_INFO("(S18F13) Subsystem command request SCR");

	int nRet = 0;
	MessageS18F13 Msg;
	if (Msg.CheckMessage(GHead->m_DataLen, GHead->m_Message)) {
		int HeadID = atoi(Msg.TargetID.c_str());
		SecsItemList* pTop = new SecsItemList, * pStat = new SecsItemList;
		string SSACK;
		pGDev->pGEMObjs->CidrSystem.ProcessCmd(HeadID, Msg.SSCMD, Msg.CpList, pStat);
		pGDev->pGEMObjs->CidrSystem.GetErrCode(SSACK);
		pTop->AddItem(new SecsItemASCII(Msg.TargetID));
		pTop->AddItem(new SecsItemASCII(SSACK));
		pTop->AddItem(pStat);
		SecsMessage OutMsg;
		OutMsg.AddItem(pTop);
		OutMsg.MakeBinaryMessage(GHead->m_Message);
		nRet = set_head(0, 18, 14, GHead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
		SendBlock(GHead);
		return nRet;
	}
	LOG_ERR("(S18F13) Incorrect Format");
	mk_S9F7(GHead);
	return nRet;
}

void CGemFactory::EventSink(short EventID, short Make) {
	CString EventMsg;
	EventMsg.Format("Event Sink : Unprocessed (%d,%d)", EventID, Make);
	LOG_WARN(EventMsg.GetBuffer(0));
}

int CGemFactory::ProcessUIDataRequest(CEvt* event) {
	if (!event) {
		return 0;
	}
	switch (event->make) {
	case REQ_CEID_DATA:
		return SendCEIDData(event);
		break;
	case REQ_VAR_DATA:
		return SendVARData(event);
		break;
	case REQ_PJDATA:
		return SendPJData(event);
		break;
	case REQ_CJDATA:
		return SendCJData(event);
		break;
	case DUMP_VAR_DATA:
		return DumpVidData(event);
		break;
	case DUMP_CEID_DATA:
		return DumpCeidData(event);
	}
	return 0;
}

int CGemFactory::SendVARData(CEvt* event) {
	vector < SVID_VALUE > dSvval;
	string Data;
	SecsItem* pItem = get_svval(1, event->no, NULL);
	if (pItem) {
		pItem->GetData(Data);
	}
	else {
		Data = "DataNotFound";
	}
	delete pItem;
	SendData(event->ReplyWnd, Data);
	// #if 0
	// 	char VarData[512] =
	// 	{
	// 		0
	// 	};
	// 	strncpy(VarData, Data.c_str(), 500);
	// 	static COPYDATASTRUCT DataPass;
	// 	DataPass.cbData = Data.size();
	// 	DataPass.dwData = (ULONG)Data.c_str();
	// 	DataPass.lpData = (void *) Data.c_str();
	// 	SendMessage(event->ReplyWnd, WM_COPYDATA, NULL, (long) & DataPass);
	// #endif
	return 0;
}

int CGemFactory::SendPJData(CEvt* event) {
	string PJData;
	CString PjID = (char*)(event->GetMsg());
	CPRJob* pJob = pGDev->GetPrjobObj(PjID);
	if (pJob) {
		pJob->Dump(PJData);
		SendData(event->ReplyWnd, PJData);
	}
	return 0;
}

int CGemFactory::SendCJData(CEvt* event) {
	string CJData;
	CString CjID = (char*)event->GetMsg();
	CCtrlJob* cJob = CJobMgr.GetCJob(CjID);
	if (cJob) {
		cJob->Dump(CJData);
		SendData(event->ReplyWnd, CJData);
	}
	return 0;
}

int CGemFactory::DumpVidData(CEvt* event) {
	if (event) {
		CString FileName = (char*)event->GetMsg();
		CFile TxtFile;
		CFileException Ex;
		if (TxtFile.Open(FileName, CFile::modeCreate | CFile::modeWrite, &Ex)) {
			string strTemp, Data;
			GetCurrTime(strTemp);
			Data = "Snapshot of Variables at (" + strTemp + ")\r\n";
			TxtFile.Write(Data.data(), Data.size());
			vector<UINT> SVIDs;
			vector < SVID_VALUE > dSvval;
			CEIDCtlr.GetAllSVIDs(SVIDs);
			for (UINT iVid = 0; iVid < SVIDs.size(); iVid++) {
				Data.erase();
				if (CEIDCtlr.SvidDesc(SVIDs[iVid], Data)) {
					SecsItem* pItem = get_svval(1, SVIDs[iVid], NULL);
					strTemp.erase();
					if (pItem) {
						pItem->GetData(strTemp);
					}
					else {
						strTemp = "No Data";
					}
					Data += "\r\n" + strTemp + "\r\n";
					TxtFile.Write(Data.data(), Data.size());
				}
			}
			TxtFile.Close();
		}
	}
	return 0;
}

int CGemFactory::DumpCeidData(CEvt* event) {
	if (event) {
	}
	return 0;
}

int CGemFactory::SendCEIDData(CEvt* event) {
	string Report = "CEID : ";
	char Buf[512];
	int RptCount = CEIDCtlr.get_rptcount(event->no);
	Report += _itoa(event->no, Buf, 10);
	Report += CEIDCtlr.check_rptflag(event->no) ? " (Enabled) " : " (Disabled) ";
	if (RptCount) {
		Report += "  has\r\n   ";
		Report += _itoa(RptCount, Buf, 10);
		Report += " Report(s) attached ";
		UINT* pReports = new UINT[RptCount];
		CEIDCtlr.getting_rptno(event->no, pReports);
		for (int nIndex = 0; nIndex < RptCount; nIndex++) {
			Report += "\r\n      Report # (";
			Report += _itoa(nIndex + 1, Buf, 10);
			Report += ")   ";
			Report += _itoa(pReports[nIndex], Buf, 10);
			Report += " \r\n        ";
			int nSvIds = CEIDCtlr.get_svidcount(pReports[nIndex]);
			if (nSvIds) {
				UINT* pSvIds = new UINT[nSvIds];
				CEIDCtlr.get_svid(pReports[nIndex], pSvIds);
				for (int nIndex2 = 0; nIndex2 < nSvIds; nIndex2++) {
					Report += _itoa(pSvIds[nIndex2], Buf, 10);
					if (nIndex2 != nSvIds - 1) {
						Report += " , ";
					}
				}
				delete[] pSvIds;
			}
		}
		delete[] pReports;
	}
	else {
		Report += "   has no reports attached";
	}
	SendData(event->ReplyWnd, Report);
	return 0;
}

BOOL CGemFactory::SendData(HWND ReplyWnd, string& Report) {
	if (!IsWindow(ReplyWnd)) {
		return FALSE;
	}
	COPYDATASTRUCT DataPass;
	DataPass.cbData = Report.size();
	DataPass.dwData = (ULONG)Report.c_str();
	DataPass.lpData = (void*)Report.c_str();
	SendMessage(ReplyWnd, WM_COPYDATA, NULL, (long)&DataPass);
	return TRUE;
}

#pragma warning(pop)

// These events are generated in Prjob see Prjob.cpp
int CGemFactory::mk_S16F7(GEM_HEADER* shead, CEvt* ehead) {
	LOG_INFO("(S16F7) Process job alert notify PRJA");
	int retc = 0;
	string TimeStamp, PrJobId, MileStone;
	SecsMessage OutMsg;
	this->GetCurrTime(TimeStamp);
	SecsItemList* pTop = new SecsItemList;
	pTop->AddItem(new SecsItemASCII(TimeStamp));
	pTop->AddItem(new SecsItemASCII(ehead->ObjID));
	pTop->AddItem(new SecsItemUInt1(ehead->make - 10000));
	SecsItemList* pErr = new SecsItemList;
	pErr->AddItem(new SecsItemBoolean(TRUE));
	pErr->AddItem(new SecsItemList);
	pTop->AddItem(pErr);
	OutMsg.AddItem(pTop);
	switch (ehead->option) {
	case CEvt::NORMAL:
		OutMsg.MakeBinaryMessage(shead->m_Message);
		retc = set_head(0, 16, 7, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
		SendBlock(shead);
		break;
	case CEvt::FORMAT:
		if (ehead->Alloc(OutMsg.BinaryMessageLength())) {
			OutMsg.MakeBinaryMessage(ehead->GetMsg());
		}
		else {
			ASSERT(0);
		}
		// Do not take action
		return 0;
		break;
	case CEvt::SEND:
		memcpy(shead->m_Message, ehead->GetMsg(), ehead->GetNMsg());
		retc = set_head(0, 16, 7, shead, ehead->GetNMsg() + 10, PRI_EQUIP);
		SendBlock(shead);
		break;
	}
	return retc;
}

int CGemFactory::mk_s16f9(GEM_HEADER* shead, CEvt* ehead) {
	LOG_INFO("(S16F9) Process job event notify PRJE");

	int retc = 0;
	unsigned int ceid = 0, rptlistcount = 0, svidcount = 0, dataid = 0;
	int sendcnt = 0, i = 0;
	char timestamp[17];
	CTime ltime = CTime::GetCurrentTime();
	if (pGDev->pGEMObjs->GetLineMode()/* pGDev->pDataBank->m_EStatus.LineMode*/ == EqpStatus::LOCAL) {
		return retc;
	}

	if (ehead->make != EVT_PJMCPROCESSCOMPLETE) {
		SecsItemList* pTop = new SecsItemList;
		ceid = ehead->make;
		// Item # 1 PREVENTID
		pTop->AddItem(new SecsItemUInt4(ceid));
		//Item # 2 TimeStamp
		memset(timestamp, 0, sizeof timestamp);
		sprintf(timestamp, "%0.4d%0.2d%0.2d%0.2d%0.2d%0.2d00", ltime.GetYear(), ltime.GetMonth(), ltime.GetDay(), ltime.GetHour(),
			ltime.GetMinute(), ltime.GetSecond());
		string strTemp = timestamp;
		pTop->AddItem(new SecsItemASCII(strTemp));
		// Item # 3 ProcesJob ID
		strTemp = ehead->ObjID;
		pTop->AddItem(new SecsItemASCII(strTemp));
		/// Item # 4 List of Attached Reports values
		pTop->AddItem(ehead->pReport);
		ehead->pReport = NULL;
		SecsMessage OutMsg;
		OutMsg.AddItem(pTop);
		if (CEIDCtlr.check_rptflag(ceid)) {
			OutMsg.MakeBinaryMessage(shead->m_Message);
			retc = set_head(0, 16, 9, shead, OutMsg.BinaryMessageLength() + 10, PRI_EQUIP);
			SendBlock(shead);
		}
		else {
			retc = 0;
		}
	}
	switch (ehead->make) {
	case EVT_PJMCPROCESSCOMPLETE:
		if (pGDev->pDataBnk->bCJJobControl) {
			PJobMgr.SetCompleted(ehead->ObjID);
			//CJobMgr.ActivateNextPRJob();
			CSharedMemQue	EvtQue;
			CEvt* pEvt = new CEvt;
			ASSERT(pEvt);
			EvtQue.OpenMemory("EVTMSG");
			pEvt->Set(EVT_OTHER, EVT_PJMSTARTNEXTPJ);
			EvtQue.WriteQue(pEvt);
			EvtQue.CloseMemory();
		}
		else {
			CSMOL SM;
			SM.cmd = CSMOL::cmdCARRIERACTION;
			SM.Ca.act = CarrierAction::PROCESSINGCOMPLETE;
			CPRJob* p = PJobMgr.GetPRJob(ehead->ObjID);
			if (p) {
				strscpy(SM.Ca.CarrierID, OBJIDLEN, p->m_PRMtlName);
			}
			PJobMgr.SetCompleted(ehead->ObjID);
			CSMClt ipc;
			if (ipc.Start1("SecsSvr", sizeof(CSMOL))) {
				if (ipc.Send(&SM)) {
					return TRUE;
				}
			}
		}
		break;
	}
	return retc;
}

//  [12/21/2007 YUEN]
BOOL CGemFactory::CanCommunicate(BYTE Stream, BYTE Function/*, BOOL bSend*/) {
	// Allow all function 0 [12/23/2007 YUEN]
	if (Function == 0) {
		return TRUE;
	}
	// Allow all stream 9 functions [12/23/2007 YUEN]
	if (Stream == 9) {
		return TRUE;
	}
	switch (pGDev->pGEMObjs->GetLineMode()) {
	case EqpStatus::OFFLINEEQP:
		if (Stream == 1) {
			switch (Function) {
			case 1:		//  [3/14/2012 Administrator]
			case 3:
			case 13:
				return TRUE;
				break;
			}
		}
		if (Stream == 2) {
			switch (Function) {
			case 41:
				return TRUE;
				break;
			}
		}
		if (Stream == 7) {
			switch (Function) {
			case 25:
				return TRUE;
				break;
			}
		}
		return FALSE;
		break;
	case EqpStatus::OFFLINEHOST:
		if (Stream == 1) {
			switch (Function) {
			case 1:		//  [3/14/2012 Administrator]
			case 3:
			case 13:
			case 14:
			case 17:
				return TRUE;
				break;
			}
		}
		if (Stream == 2) {
			switch (Function) {
			case 41:
				return TRUE;
				break;
			}
		}
		if (Stream == 7) {
			switch (Function) {
			case 25:
				return TRUE;
				break;
			}
		}
		break;
	case EqpStatus::ATTEMPTONLINE:
		if (Stream == 1) {
			switch (Function) {
			case 1:
			case 2:
			case 13:
			case 14:
				return TRUE;
			}
		}
		break;
	case EqpStatus::LOCAL:
	case EqpStatus::REMOTE:
		return TRUE;
		break;
	}
	return FALSE;
}

int CGemFactory::mk_S1F13(GEM_HEADER* shead, CEvt* ehead) {
	LOG_INFO("Send: S1F13 Establish communication request ER");

	CS2Msg Msg;
	if (Msg.Decode(shead)) {
		if (Msg.Validate()) {
			SecsItemList* pList = new SecsItemList;
			pList->AddItem(new SecsItemASCII(string(CEIDCtlr.m_SVList.m_Svid[SVID_LIST::DEVICEID].GetV())));
			pList->AddItem(new SecsItemASCII(string(CEIDCtlr.m_SVList.m_Svid[SVID_LIST::SOFTREV].GetV())));
			CS2Msg OutMsg;
			OutMsg.AddItem(pList);
			set_head(1, 1, 13, shead, OutMsg.MakeBinaryMessage(shead->m_Message) + 10, PRI_EQUIP);
			return SendBlock(shead);
		}
	}
	return 1;
}

// Trace initialize ack [10/29/2002]
int CGemFactory::mk_S2F24(GEM_HEADER* shead) {
	LOG_INFO("(S2F23) Trace initialize send TIS");

	BYTE retc = 0;
	CS2F23 Msg;
	if (Msg.Decode(shead)) {
		if (Msg.Validate()) {
			UINT trid;
			string str;
			Msg.GetItem(0)->ContainedItems(0)->GetData(str);
			trid = atoi(str.c_str());
			if (Check_Trid(trid)) {
				DeleteTrid(trid);
			}
			short sec, min, hour;
			Msg.GetItem(0)->ContainedItems(1)->GetData(str);
			char buf[3];
			buf[2] = 0;
			buf[0] = str[0];
			buf[1] = str[1];
			hour = atoi(buf);
			buf[0] = str[2];
			buf[1] = str[3];
			min = atoi(buf);
			buf[0] = str[4];
			buf[1] = str[5];
			sec = atoi(buf);
			UINT elapse = UINT((hour * 3600) + (min * 60) + sec);
			Msg.GetItem(0)->ContainedItems(2)->GetData(str);
			UINT topsmp = atoi(str.c_str());
			UINT val;
			vector < UINT > SVID;
			SecsItem* pItem = Msg.GetItem(0)->ContainedItems(4);
			int svidcnt = pItem->NumContainedItems();
			for (int i = 0; i < svidcnt; i++) {
				pItem->ContainedItems(i)->GetUInt4(val);
				SVID.push_back(val);
				if (!CEIDCtlr.chk_svid(val)) {
					retc = 5;
					break;
				}
			}
			if (!retc) {
				CTraceTracker* pt = new CTraceTracker(trid, topsmp);
				pt->m_svid.SetSize(SVID.size(), 1);
				for (int i = 0; i < int(SVID.size()); i++) {
					pt->m_svid.SetAtGrow(i, SVID[i]);
				}
				m_pTraceList.AddHead(pt);
				pt->StartTrace(trid, elapse * 1000, topsmp);
			}
			CS2Msg OutMsg;
			OutMsg.AddItem(new SecsItemBinary(&retc, 1));
			set_head(0, 2, 24, shead, OutMsg.MakeBinaryMessage(shead->m_Message) + 10, PRI_HOST);
			SendBlock(shead);
			return retc;
		}
		return 6;
	}
	return 7;
}

int CGemFactory::mk_S2F32(GEM_HEADER* shead) {
	LOG_INFO("(S2F31) Date and time set request DTS");

	BYTE retc = 0;
	CS2F31 Msg;
	short tiack = 0;
	SYSTEMTIME dwTime;
	if (Msg.Decode(shead)) {
		if (Msg.Validate()) {
			string str;
			Msg.GetItem(0)->GetData(str);
			if (str.length() == 12) {
				::TimeFormat = 0;
				dwTime.wYear = (WORD)(2000 + (str[0] - '0') * 10 + (str[1] - '0'));
				if (dwTime.wYear > 2099 || dwTime.wYear < 2000) {
					tiack = 1;
				}
				else {
					dwTime.wMonth = (str[2] - '0') * 10 + (str[3] - '0');
					if (dwTime.wMonth < 1 || dwTime.wMonth > 12) {
						tiack = 1;
					}
					else {
						dwTime.wDay = (str[4] - '0') * 10 + (str[5] - '0');
						if (dwTime.wDay < 1 || dwTime.wDay > 31) {
							tiack = 1;
						}
						else {
							dwTime.wHour = (str[6] - '0') * 10 + (str[7] - '0');
							if (dwTime.wHour < 0 || dwTime.wHour > 23) {
								tiack = 1;
							}
							else {
								dwTime.wMinute = (str[8] - '0') * 10 + (str[9] - '0');
								if (dwTime.wMinute < 0 || dwTime.wMinute > 59) {
									tiack = 1;
								}
								else {
									dwTime.wSecond = (str[10] - '0') * 10 + (str[11] - '0');
									if (dwTime.wSecond < 0 || dwTime.wSecond > 59) {
										tiack = 1;
									}
									dwTime.wMilliseconds = 0;
								}
							}
						}
					}
				}
			}
			else {
				::TimeFormat = 1;
				dwTime.wYear = (str[0] - '0') * 1000 + (str[1] - '0') * 100 + (str[2] - '0') * 10 + (str[3] - '0');
				if (dwTime.wYear > 2099 || dwTime.wYear < 2000) {
					tiack = 1;
				}
				else {
					dwTime.wMonth = (str[4] - '0') * 10 + (str[5] - '0');
					if (dwTime.wMonth < 1 || dwTime.wMonth > 12) {
						tiack = 1;
					}
					else {
						dwTime.wDay = (str[6] - '0') * 10 + (str[7] - '0');
						if (dwTime.wDay < 1 || dwTime.wDay > 31) {
							tiack = 1;
						}
						else {
							dwTime.wHour = (str[8] - '0') * 10 + (str[9] - '0');
							if (dwTime.wHour < 0 || dwTime.wHour > 23) {
								tiack = 1;
							}
							else {
								dwTime.wMinute = (str[10] - '0') * 10 + (str[11] - '0');
								if (dwTime.wMinute < 0 || dwTime.wMinute > 59) {
									tiack = 1;
								}
								else {
									dwTime.wSecond = (str[12] - '0') * 10 + (str[13] - '0');
									if (dwTime.wSecond < 0 || dwTime.wSecond > 59) {
										tiack = 1;
									}
									dwTime.wMilliseconds = (str[14] - '0') * 10 + (str[15] - '0') * 10;
								}
							}
						}
					}
				}
			}
			if (!tiack) {
				if (SetLocalTime(&dwTime)) {
					::PostMessage(HWND_BROADCAST, WM_TIMECHANGE, 0, 0);
				}
			}
			CS2Msg OutMsg;
			OutMsg.AddItem(new SecsItemBinary(&retc, 1));
			set_head(0, 2, 32, shead, OutMsg.MakeBinaryMessage(shead->m_Message) + 10, PRI_HOST);
			return SendBlock(shead);
		}
	}
	return 1;
}

int CGemFactory::mk_S2F42(GEM_HEADER* shead) {
	LOG_INFO("(S2F41) Host command send HCS");

	char* nRemoteList[] =
	{
		"CHANGEREMOTE",			// 0 [5/14/2012 Yuen]
		"CHANGELOCAL",			// 1 [5/14/2012 Yuen]
		"CHANGEOFFLINE",		// 2 [5/14/2012 Yuen]
		"REMOTESTOP",			// 3 [5/14/2012 Yuen]
		"REMOTEPAUSE",			// 4 [5/14/2012 Yuen]
		"REMOTERESUME",			// 5 [5/14/2012 Yuen]
		"REMOTEABORT",			// 6 [5/14/2012 Yuen]
		"REMOTECANCEL",			// 7 [5/14/2012 Yuen]
		"GETCASSETTENAME",		// 8 [5/14/2012 Yuen]
		"CLAMP",				// 9 [5/14/2012 Yuen]
		"UNCLAMP"				// 10 [5/14/2012 Yuen]
	};
	int nItem = 11;	// Number of items in nRemoteList [5/14/2012 Yuen]
	CS2F41 Msg;
	string rcmd, rval;
	CString str;
	BYTE retc = /*2*/0;	// Alway return OK, fake response to host [2/15/2012 Administrator]
	if (Msg.Decode(shead)) {
		if (Msg.Validate()) {
			Msg.GetItem(0)->ContainedItems(0)->GetData(rcmd);
			for (int i = 0; i < nItem; i++) {
				if (rcmd == nRemoteList[i]) {
					switch (i) {
						// Change to REMOTE ONLINE
					case 0:
						if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::REMOTE) {
							CEvt* pEvt = new CEvt;
							ASSERT(pEvt);
							pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_ONLINE);
							m_gEvtQue.OpenMemory("EVTMSG");
							m_gEvtQue.WriteQue(pEvt);
							m_gEvtQue.CloseMemory();
							retc = 0;
						}
						break;
						// Change to LOCAL ONLINE
					case 1:
						if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::LOCAL) {
							CEvt* pEvt = new CEvt;
							ASSERT(pEvt);
							pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_ATTEMPT_LOCAL);
							m_gEvtQue.OpenMemory("EVTMSG");
							m_gEvtQue.WriteQue(pEvt);
							m_gEvtQue.CloseMemory();
							retc = 0;
						}
						break;
						// Change to OFFLINE
					case 2:
						if (pGDev->pGEMObjs->GetLineMode() != EqpStatus::OFFLINEEQP) {
							CEvt* pEvt = new CEvt;
							ASSERT(pEvt);
							pEvt->Set(EVT_LINEMODE, EVT_LINEPHASE_OFFLINE);
							m_gEvtQue.OpenMemory("EVTMSG");
							m_gEvtQue.WriteQue(pEvt);
							m_gEvtQue.CloseMemory();
							retc = 0;
						}
						break;
						// Send a Stop event to C2C task
					case 3:
						retc = 2;
						break;
						// Send a Pause event to C2C task
					case 4:
						retc = 2;
						break;
						// Send a Resume event to C2C task
					case 5:
						retc = 2;
						break;
						// Send a Abort event to C2C task
					case 6:
						retc = 2;
						break;
						// Send a Cancel event to C2C task
					case 7:
						retc = 2;
						break;
					case 8:
						retc = 2;
						break;
					case 9:
						retc = 2;
						Msg.GetItem(0)->ContainedItems(1)->ContainedItems(0)->GetData(rcmd);
						Msg.GetItem(0)->ContainedItems(1)->ContainedItems(1)->GetData(rval);
						str = rcmd.c_str();
						str.MakeUpper();
						if (str == "PORT") {
							pUDev->ClampCarrier(atoi(rval.c_str()));
							retc = 0;
						}
						break;
					}
				}
			}
		}
		CS2Msg OutMsg;
		SecsItemList* pList = new SecsItemList();
		pList->AddItem(new SecsItemBinary(&retc, 1));
		pList->AddItem(new SecsItemList());
		OutMsg.AddItem(pList);
		set_head(0, 2, 42, shead, OutMsg.MakeBinaryMessage(shead->m_Message) + 10, PRI_HOST);
		SendBlock(shead);
		return retc;
	}
	return 3;
}

/****************
All following functions should be in Seperate module for OSS
*******************/
BOOL CGemFactory::GetAllObjectIDs(short ObjectType, vector<string>& ObjIds) {
	BOOL TypeFound = TRUE;
	switch (ObjectType) {
	case 0:
		::pGDev->GetAllPRJobIDs(ObjIds);;
		break;
	case 1:
		::pGDev->GetAllCJobIDs(ObjIds);
		break;
	case 2:
		pGDev->GetCarrierIds(ObjIds);
		break;
	case 3:
		::pGDev->GetAllSubstIDS(ObjIds);
		break;
	case 4:
		::pGDev->GetSubstLocList(ObjIds);
		break;
	default:
		TypeFound = FALSE;
	}
	return TypeFound;
}

BOOL CGemFactory::IsObject(short ObjType, string ObjID) {
	BOOL bObjFound = TRUE;
	switch (ObjType) {
	case 0:
		bObjFound = GetPrJobObj(ObjID);
		break;
	case 1:
		bObjFound = GetCjobObj(ObjID);
		break;
	case 2:
		bObjFound = GetCarrierObj(ObjID);
		break;
	case 3:
		bObjFound = GetSubstObj(ObjID);
		break;
	case 4:
		bObjFound = GetSubstLocObj(ObjID);
		break;
	default:
		bObjFound = FALSE;
	}
	return bObjFound;
}

BOOL CGemFactory::GetObjectAttr(short ObjType, const string& ObjID, const string& AttrId, SecsItemList* pAttr) {
	BOOL bResult = FALSE;
	switch (ObjType) {
	case 0:
		bResult = GetPrJobAttr(ObjID, AttrId, pAttr);
		break;
	case 1:
		bResult = GetCjobAttr(ObjID, AttrId, pAttr);
		break;
	case 2:
		bResult = GetCarrierAttr(ObjID, AttrId, pAttr);
		break;
	case 3:
		bResult = GetSubstAttr(ObjID, AttrId, pAttr);
		break;
	case 4:
		bResult = GetSubstLocAttr(ObjID, AttrId, pAttr);
		break;
	default:
		bResult = FALSE;
	}
	return bResult;
}

BOOL CGemFactory::GetAllAttributes(short ObjectType, vector<string>& AttrIds) {
	BOOL bResult = TRUE;
	switch (ObjectType) {
	case 0:
		CPRJob::GetAttrIDs(AttrIds);
		break;
	case 1:
		CCtrlJob::GetAttrIDs(AttrIds);
		break;
	case 2:
		CCms::GetCarrierAttrIds(AttrIds);
		break;
	case 3:
		CCms::GetSubstAttrIds(AttrIds);
		break;
	case 4:
		CCms::GetSubstLocAttrIds(AttrIds);
		break;
	default:
		bResult = FALSE;
	}
	return bResult;
}

/****************
End OSS
*******************/

SecsItem* CGemFactory::MakeErrorList(UCHAR ErrCode) {
	string ErrTxt;
	SecsItemList* pList = new SecsItemList;
	pList->AddItem(new SecsItemUInt1(ErrCode));
	ErrorMessages::GetErrTxtE5(ErrCode, ErrTxt);
	pList->AddItem(new SecsItemASCII(ErrTxt));
	return pList;
}

SecsItem* CGemFactory::MakeErrorReport(vector < UINT > ErrorCodes) {
	SecsItemList* pReport = new SecsItemList;
	for (UINT iErr = 0; iErr < ErrorCodes.size(); iErr++) {
		pReport->AddItem(MakeErrorList(ErrorCodes[iErr]));
	}
	return pReport;
}

SecsItem* CGemFactory::get_svval(short PortID, UINT SVID, CEvt* pEvt) {
	UINT Type = svASCII;
	DataConverter Data;
	SVID_LIST* p = &CEIDCtlr.m_SVList;
	SVID_ITEM* q;
	short idx;

	idx = p->GetIndex(SVID);
	q = p->Get(SVID);
	if (!q || idx < 0) {
		CString str;
		str.Format("Unrecognized SVID %d", SVID);
		AfxMessageBox(str);
		return NULL;
	}
	//Type = q->m_type;
	switch (idx) {
	case SVID_LIST::DEVICEID:  // OK [7/31/2010 XPMUser]
		Data.Val(q->GetVStr());
		Type = svASCII;
		break;
	case SVID_LIST::CONNECTIONMODE:  // OK [7/31/2010 XPMUser]
		Data.Val(m_ConnectionMode);
		Type = svINT2;
		break;
	case SVID_LIST::SPOOLOVERIDE:  // OK [7/31/2010 XPMUser]
		Data.Val(CSpoolMan::GetSpool()->OverWriteSpool);
		Type = svBOOLEAN;
		break;
	case SVID_LIST::SOFTREV:
		Data.Val(q->GetVStr());
		Type = svASCII;
		break;
	case SVID_LIST::ESTABLISHCOMMTIMEOUT:
		Data.Val(EstablishCommunicationTimeout);
		Type = svINT2;
		break;
	case SVID_LIST::TIMEFORMAT:
		Data.Val(TimeFormat);
		Type = svINT2;
		break;
	case SVID_LIST::TIMESTAMP:
	case SVID_LIST::CLOCK:
	{
		string TempStr;
		GetCurrTime(TempStr);
		Data.Val(TempStr);
		Type = svASCII;
	}
	break;
	case SVID_LIST::CURRENTSLOTNO:
		Data.Val(pGDev->GetActiveSubstrateNo());
		Type = svINT2;
		break;
	case SVID_LIST::LASTKNOWNSM:
	{
		int nIndex = pGDev->pDataBnk->GetActivePort();
		switch (nIndex) {
		case 1:
			Data.Val(string(::pGDev->pDataBnk->LastSM1));
			break;
		case 2:
			Data.Val(string(::pGDev->pDataBnk->LastSM2));
			break;
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::CURRENTPJ:
	{
		string ID;
		CPRJob* pJob = pGDev->GetActivePrJob();
		if (pJob) {
			ID = pJob->m_ObjID;
		}
		else {
			pJob = CJobMgr.GetActivePRJob();
			if (pJob) {
				ID = pJob->m_ObjID;
			}
			else {
				CCtrlJob* cJob = CJobMgr.GetActiveCJob();
				if ((pJob) && (CJobMgr.m_PRJobList.GetCount())) {
					ID = cJob->ProcessingCtrlSpec[0];
				}
			}
		}
		Data.Val(ID);
		Type = svASCII;
	}
	break;
	case SVID_LIST::CURRENTCJ:
	{
		string ID;
		CCtrlJob* cJob = pGDev->GetActiveCtrlJob();
		if (cJob) {
			ID = cJob->m_ObjID;
		}
		Data.Val(ID);
		Type = svASCII;
	}
	break;
	case SVID_LIST::CURRENTPORTNO:
		Data.Val(pGDev->GetActivePort());
		Type = svINT2;
		break;
	case SVID_LIST::CURRENTCARRIERID:
	{
		int nIndex = pGDev->GetActivePort();
		if ((nIndex >= 1) && (nIndex <= 4)) //  [9/7/2007 LYF]
		{
			Data.Val(pGDev->pGEMObjs->Cms[nIndex - 1].CaObj.ObjID);
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP1WAFERCOUNT:
	{
		short wafercnt = 0;
		CCms* pCms = &pGDev->pGEMObjs->Cms[0];
		if (CCarrierObj::SMNOTREAD < pCms->CaObj.SlotmapStatus) {
			for (int nIndex = 0; nIndex < MAXCAPACITYLEN; nIndex++) {
				if (CSubstObj::SM_CORRECTLY_OCCUPIED == pCms->CaObj.SlotMaP[nIndex]) {
					wafercnt++;
				}
			}
		}
		Data.Val(wafercnt);
		Type = svINT2;
	}
	break;
	case SVID_LIST::LP2WAFERCOUNT:
	{
		short wafercnt = 0;
		CCms* pCms = &pGDev->pGEMObjs->Cms[1];
		if (CCarrierObj::SMNOTREAD < pCms->CaObj.SlotmapStatus) {
			for (int nIndex = 0; nIndex < MAXCAPACITYLEN; nIndex++) {
				if (CSubstObj::SM_CORRECTLY_OCCUPIED == pCms->CaObj.SlotMaP[nIndex]) {
					wafercnt++;
				}
			}
		}
		Data.Val(wafercnt);
		Type = svINT2;
	}
	break;
	case SVID_LIST::LP1SLOTMAP:
	{
		char nBuf[128];
		memset(nBuf, 0, 128);
		MakeSlotMap(nBuf, 1);// [8/14/2007 LYF]
		Data.Val(string(nBuf)); //  [9/12/2007 LYF]
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP2SLOTMAP:
	{
		char nBuf[128];
		MakeSlotMap(nBuf, 2);// [8/14/2007 LYF]
		Data.Val(string(nBuf)); //  [9/12/2007 LYF]
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP1SLOTMAPSTATUSSTR:
	{
		CCms* pCms = &pGDev->pGEMObjs->Cms[0];
		Data.Val(pCms->CaObj.GetSlotmapStatusStr());
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP1SLOTMAPSTATUS:
	{
		CCms* pCms = &pGDev->pGEMObjs->Cms[0];
		Data.Val(pCms->CaObj.SlotmapStatus);
		Type = svINT2;
	}
	break;
	case SVID_LIST::LP2SLOTMAPSTATUSSTR:
	{
		CCms* pCms = &pGDev->pGEMObjs->Cms[1];
		Data.Val(pCms->CaObj.GetSlotmapStatusStr());
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP2SLOTMAPSTATUS:
	{
		CCms* pCms = &pGDev->pGEMObjs->Cms[1];
		Data.Val(pCms->CaObj.SlotmapStatus);
		Type = svINT2;
	}
	break;
	case SVID_LIST::LP1CARRIERID:
	{
		CCms* pCms = &pGDev->pGEMObjs->Cms[0];
		Type = svASCII;
		if (CCarrierObj::IDNOTREAD < pCms->CaObj.IDStatus) {
			Data.Val(pCms->CaObj.ObjID);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP2CARRIERID:
	{
		CCms* pCms = &pGDev->pGEMObjs->Cms[1];
		Type = svASCII;
		if (CCarrierObj::IDNOTREAD < pCms->CaObj.IDStatus) {
			Data.Val(pCms->CaObj.ObjID);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::LINEMODE:
		Data.Val(pGDev->pGEMObjs->GGetLineMode()); //  [9/14/2007 LYF]
		Type = svINT2;
		break;
	case SVID_LIST::CONTROLSTATELINEMODE:
		Data.Val(pGDev->pGEMObjs->GGetLineMode());
		Type = svINT2;
		break;
	case SVID_LIST::EQUIPMENTSTATUS:
		Data.Val(pGDev->GGetARAMSState());
		Type = svINT2;
		break;
	case SVID_LIST::MAXLOADPORT:
		Data.Val(Var.nLoadPort);
		Type = svINT2;
		break;
	case SVID_LIST::LP1OCCUPANCYSTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[0].LPObj.GGetOccupency());
		Type = svINT2;
		break;
	case SVID_LIST::LP2OCCUPANCYSTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[1].LPObj.GGetOccupency());
		Type = svINT2;
		break;
	case SVID_LIST::LP1CARRIERACCESSSTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[0].CaObj.GetAccessingStatusStr());
		Type = svASCII;
		break;
	case SVID_LIST::LP2CARRIERACCESSSTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[1].CaObj.GetAccessingStatusStr());
		Type = svASCII;
		break;
	case SVID_LIST::LP1ACCESSMODE:
		Data.Val(pGDev->pGEMObjs->Cms[0].LPObj.GGetAccessMode());
		Type = svINT2;
		break;
	case SVID_LIST::LP2ACCESSMODE:
		Data.Val(pGDev->pGEMObjs->Cms[1].LPObj.GGetAccessMode());
		Type = svINT2;
		break;
	case SVID_LIST::LP1TRANSFERSTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[0].LPObj.GGetTransferStatus());
		Type = svINT2;
		break;
	case SVID_LIST::LP2TRANSFERSTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[1].LPObj.GGetTransferStatus());
		Type = svINT2;
		break;
	case SVID_LIST::LP1ASSOCIATESTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[0].LPObj.GGetAssociation());
		Type = svINT2;
		break;
	case SVID_LIST::LP2ASSOCIATESTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[1].LPObj.GGetAssociation());
		Type = svINT2;
		break;
	case SVID_LIST::LP1BARCODESTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[0].BCR.GGetServiceStatus());
		Type = svINT2;
		break;
	case SVID_LIST::LP2BARCODESTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[1].BCR.GGetServiceStatus());
		Type = svINT2;
		break;
		// 	case SVID_LIST::LP1DOCKINGSTATE:
		// 		Log("LP1DOCKINSTATE not implemented",1);
		// 		break;
		// 	case SVID_LIST::LP2DOCKINGSTATE:
		// 		Log("LP2DOCKINSTATE not implemented",1);
		// 		break;
	case SVID_LIST::LP1RESERVATIONSTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[0].LPObj.Reservation);
		Type = svINT2;
		break;
	case SVID_LIST::LP2RESERVATIONSTATUS:
		Data.Val(pGDev->pGEMObjs->Cms[1].LPObj.Reservation);
		Type = svINT2;
		break;
	case SVID_LIST::LINEMODEENUM:
	{
		SecsItemList* pList = new SecsItemList;
		for (int i = EqpStatus::NOSECS; i <= EqpStatus::ATTEMPTONLINE; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(EqpStatus::GetControlStateStr((EqpStatus::CONTROLSTATE)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::EQUIPMENTSTATEENUM:
	{
		SecsItemList* pList = new SecsItemList;
		for (int i = SArams::NOSTATE; i <= SArams::SETTINGUP; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(SArams::GetEqpStateStr((SArams::ARAMSSTATE)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::LPOCCUPANCYSTATEENUM:
	{
		SecsItemList* pList = new SecsItemList;
		for (int i = CLPObj::CARRIERNOTPRESENT; i <= CLPObj::CARRIERPRESENT; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(CLPObj::GetOccupancyStr((CLPObj::OCCUPANCY)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::LPACCESSSTATEENUM:
	{
		SecsItemList* pList = new SecsItemList;
		for (int i = CLPObj::MANUAL; i <= CLPObj::AUTO; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(CLPObj::GetAccessModeStr((CLPObj::ACCESSMODE)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::LPTRANSFERSTATEENUM:
	{
		SecsItemList* pList = new SecsItemList;
		for (int i = CLPObj::tsNOSTATE; i <= CLPObj::TRANSFERREADY; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(CLPObj::GetTransferStatusStr((CLPObj::TRANSFERSTATUS)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::LPASSOCIATESTATEENUM:
	{
		SecsItemList* pList = new SecsItemList;
		for (int i = CLPObj::NOTASSOCIATED; i <= CLPObj::ASSOCIATED; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(CLPObj::GetAssociationStr((CLPObj::ASSOCIATION)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::LPRESERVATIONSTATEENUM:
	{
		SecsItemList* pList = new SecsItemList;
		for (int i = CLPObj::NOTRESERVED; i <= CLPObj::RESERVED; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(CLPObj::GetReservationStr((CLPObj::RESERVATION)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::CJMAXQUEUECOUNT:
		Data.Val(CJobMgr.MAXCONTROLJOB);
		Type = svINT2;
		break;
	case SVID_LIST::CJAVAILABLESPACE:
		Data.Val(CJobMgr.GetJobSpace());
		Type = svINT2;
		break;
	case SVID_LIST::CJSTATEENUM:
		//Data.Val("To be implemented");
		//Type = svASCII;
	{
		CCtrlJob pC;
		SecsItemList* pList = new SecsItemList;
		for (int i = CCtrlJob::QUEUED; i <= CCtrlJob::STOPPED; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(pC.GetStateStr((CCtrlJob::STATE)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::QUEUEDCJSLIST:
		//Data.Val("To be implemented");
		//Type = svASCII;
	{
		CCtrlJob* cJob = pGDev->GetActiveCtrlJob();
		if (cJob) {
			Data.Val(cJob->m_ObjID);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::CURRENTCJID:
	{
		CCtrlJob* cJob = pGDev->GetActiveCtrlJob();
		if (cJob) {
			Data.Val(cJob->m_ObjID);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::CURRENTCJSTATE:
	{
		CCtrlJob* cJob = pGDev->GetActiveCtrlJob();
		if (cJob) {
			Data.Val(cJob->GetState());
		}
		else {
			Data.Val("");
		}
		Type = svINT2;
	}
	break;
	case SVID_LIST::CURRENTCJSTATESTR:
	{
		CCtrlJob* cJob = pGDev->GetActiveCtrlJob();
		if (cJob) {
			Data.Val(cJob->GetStateStr());
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::PJMAXQUEUECOUNT:
		Data.Val(PJobMgr.MAXPRJOB);
		Type = svINT2;
		break;
	case SVID_LIST::PJAVAILABLESPACE:
		Data.Val(PJobMgr.GetJobSpace()); //  [9/12/2007 LYF]
		Type = svINT2;
		break;
	case SVID_LIST::CURRENTPJID:
	{
		string ID;
		CPRJob* pJob = pGDev->GetActivePrJob();
		if (pJob) {
			ID = pJob->m_ObjID;
		}
		else {
			pJob = CJobMgr.GetActivePRJob();
			if (pJob) {
				ID = pJob->m_ObjID;
			}
			else {
				CCtrlJob* cJob = CJobMgr.GetActiveCJob();
				if ((pJob) && (CJobMgr.m_PRJobList.GetCount())) {
					ID = cJob->ProcessingCtrlSpec[0];
				}
			}
		}
		Data.Val(ID);
		Type = svASCII;
	}
	break;
	case SVID_LIST::CURRENTPJSTATE:
	{
		Data.Val(0);
		CPRJob* pJob = pGDev->GetActivePrJob();
		if (pJob) {
			Data.Val(pJob->GetState());
		}
		else {
			Data.Val("");
		}
		Type = svINT2;
	}
	break;
	case SVID_LIST::CURRENTPJSTATESTR:
	{
		CPRJob* pJob = pGDev->GetActivePrJob();
		if (pJob) {
			Data.Val(pJob->GetJobStateStr());
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::PJSTATEENUM:
	{
		CPRJob pJ;
		SecsItemList* pList = new SecsItemList;
		for (int i = CPRJob::QUEUED_POOLED; i <= CPRJob::ABORTING; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(pJ.GetJobStateStr((CPRJob::PRJOBSTATE)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;

	case SVID_LIST::ALARMENABLEDLIST:
	{
		vector<UINT> vecALID;
		vector<CString> vecALTX;
		SecsItemList* pList = new SecsItemList;
		if (CEIDCtlr.m_alarm.GetAlarmEnabledID(vecALID, vecALTX)) //  [9/16/2007 LYF]
		{
			for (int nIndex = 0; nIndex<int(vecALID.size()); nIndex++) {
				SecsItemList* pList2 = new SecsItemList;
				Data.Val(vecALID[nIndex]);
				pList2->AddItem(Data.GetType(svUNSIGN2));
				Data.Val(vecALTX[nIndex]);
				pList2->AddItem(Data.GetType(svASCII));
				Data.Val(pList2);
				pList->AddItem(Data.GetType(svLIST));
			}
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::ALARMSETLIST:
	{
		vector<UINT> vecALID;
		vector<CString> vecALTX;
		SecsItemList* pList = new SecsItemList;
		if (CEIDCtlr.m_alarm.GetAlarmSetID(vecALID, vecALTX)) //  [9/16/2007 LYF]
		{
			for (int nIndex = 0; nIndex<int(vecALID.size()); nIndex++) {
				SecsItemList* pList2 = new SecsItemList;
				Data.Val(vecALID[nIndex]);
				pList2->AddItem(Data.GetType(svUNSIGN2));
				Data.Val(vecALTX[nIndex]);
				pList2->AddItem(Data.GetType(svASCII));
				Data.Val(pList2);
				pList->AddItem(Data.GetType(svLIST));
			}
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::ALARMSENUM:
	{
		vector<UINT> vecALID;
		vector<CString> vecALTX;
		SecsItemList* pList = new SecsItemList;
		CEIDCtlr.m_alarm.GetAllAlarmID(vecALID, vecALTX);
		{
			int n = int(vecALID.size());
			for (int nIndex = 0; nIndex < n; nIndex++) {
				SecsItemList* pList2 = new SecsItemList;
				Data.Val(vecALID[nIndex]);
				pList2->AddItem(Data.GetType(svUNSIGN2));
				Data.Val(vecALTX[nIndex]);
				pList2->AddItem(Data.GetType(svASCII));
				Data.Val(pList2);
				pList->AddItem(Data.GetType(svLIST));
			}
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::LASTALARM:
		Data.Val(CEIDCtlr.m_alarm.LastAlarm);
		Type = svINT2;
		break;
	case SVID_LIST::LASTALARMSET:
		Data.Val((INT)CEIDCtlr.m_alarm.LastAlarmSet);
		Type = svINT2;
		break;

	case SVID_LIST::SPOOLCOUNTACTUAL:  // OK [7/31/2010 XPMUser]
	{
		CSpoolMan* pSpool = CSpoolMan::GetSpool();
		Data.Val(pSpool->ActualSpooled());
		Type = svINT2;
	}
	break;
	case SVID_LIST::SPOOLCOUNTTOTAL:  // OK [7/31/2010 XPMUser]
	{
		CSpoolMan* pSpool = CSpoolMan::GetSpool();
		Data.Val(pSpool->SpooledSoFar());
		Type = svINT2;
	}
	break;
	case SVID_LIST::SPOOLFULLTIME:  // OK [7/31/2010 XPMUser]
	{
		char Buf[26] = { 0 };
		CSpoolMan* pSpool = CSpoolMan::GetSpool();
		pSpool->GetSpoolFullTime(Buf);
		Data.Val(Buf);
		Type = svASCII;
	}
	break;
	case SVID_LIST::SPOOLSTARTTIME:
	{
		char Buf[26] = { 0 };
		CSpoolMan* pSpool = CSpoolMan::GetSpool();
		pSpool->GetSpoolStartTime(Buf);
		Data.Val(Buf);
		Type = svASCII;
	}
	break;

	case SVID_LIST::CARRIERLOCATIONMATRIX:
	{
		char Buf[128];
		SecsItemList* pItem = new SecsItemList;
		for (int i = 0; i < Var.nLoadPort; i++) {
			CCms* pCMS = &pGDev->pGEMObjs->Cms[i];
			for (UINT j = CLPObj::clUNDOCKED; j <= CLPObj::clNOSTATE; j++)	// YFL: Dock and undock location [1/12/2005]
			{
				SecsItemList* pMatElm = new SecsItemList;
				sprintf(Buf, "LP%i %s", i + 1, pCMS->GetLocationStr((CLPObj::CARRIERLOC)j));
				pMatElm->AddItem(new SecsItemASCII(Buf));
				if (j == pCMS->LPObj.CarrierLoc) {
					pMatElm->AddItem(new SecsItemASCII(pCMS->CaObj.ObjID));
				}
				else {
					pMatElm->AddItem(new SecsItemASCII(""));
				}
				pItem->AddItem(pMatElm);
			}
		}
		Data.Val(pItem);
		Type = svLIST;
	}
	break;
	case SVID_LIST::LP1CARRIERLOCATION:
	{
		short Port = 0;
		SecsItemList* pItem = new SecsItemList;
		CCms* pCMS = &pGDev->pGEMObjs->Cms[Port];
		pItem->AddItem(new SecsItemASCII(pCMS->GetLocationStr(pCMS->LPObj.CarrierLoc)));
		Data.Val(pItem);
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP2CARRIERLOCATION:
	{
		short Port = 1;
		SecsItemList* pItem = new SecsItemList;
		CCms* pCMS = &pGDev->pGEMObjs->Cms[Port];
		pItem->AddItem(new SecsItemASCII(pCMS->GetLocationStr(pCMS->LPObj.CarrierLoc)));
		Data.Val(pItem);
		Type = svASCII;
	}
	break;
	case SVID_LIST::PORTASSOCIATIONSTATELIST:
	{
		SecsItemList* pItem = new SecsItemList;
		for (int i = 0; i < Var.nLoadPort; i++) {
			SecsItemList* pItem2 = new SecsItemList;
			CCms* pCMS = &pGDev->pGEMObjs->Cms[i];
			Data.Val(pCMS->LPObj.GGetAssociation());
			pItem2->AddItem(Data.GetType(svUNSIGN1));
			Data.Val(pCMS->LPObj.GetAssociationStr());
			pItem2->AddItem(Data.GetType(svASCII));
			Data.Val(pItem2);
			pItem->AddItem(Data.GetType(svLIST));
		}
		Data.Val(pItem);
		Type = svLIST;
	}
	break;
	case SVID_LIST::PORTSTATEINFOLIST:
	{
		SecsItemList* pItem = new SecsItemList;
		for (int i = 0; i < Var.nLoadPort; i++) {
			CCms* pCMS = &pGDev->pGEMObjs->Cms[i];
			SecsItem* pPort = new SecsItemList;
			// Port Number [7/2/2012 user]
			Data.Val("LOAD PORT NUMBER");
			pPort->AddItem(Data.GetType(svASCII));
			Data.Val(i + 1);
			pPort->AddItem(Data.GetType(svUNSIGN1));
			// Access mode [7/2/2012 user]
			SecsItem* pPort2 = new SecsItemList;
			Data.Val("ACCESS MODE");
			pPort2->AddItem(Data.GetType(svASCII));
			SecsItem* pPort5 = new SecsItemList;
			Data.Val(pCMS->LPObj.GGetAccessMode());
			pPort5->AddItem(Data.GetType(svUNSIGN1));
			Data.Val(pCMS->LPObj.GetAccessModeStr());
			pPort5->AddItem(Data.GetType(svASCII));
			pPort2->AddItem(pPort5);
			pPort->AddItem(pPort2);
			// Transfer status [7/2/2012 user]
			SecsItem* pPort3 = new SecsItemList;
			Data.Val("TRANSFER STATUS");
			pPort3->AddItem(Data.GetType(svASCII));
			SecsItem* pPort6 = new SecsItemList;
			Data.Val(pCMS->LPObj.GGetTransferStatus());
			pPort6->AddItem(Data.GetType(svUNSIGN1));
			Data.Val(pCMS->LPObj.GetTransferStatusStr());
			pPort6->AddItem(Data.GetType(svASCII));
			pPort3->AddItem(pPort6);
			pPort->AddItem(pPort3);
			SecsItem* pPort4 = new SecsItemList;
			pPort4->AddItem(pPort);
			//pPort4->AddItem(pPort2);
			//pPort4->AddItem(pPort3);
			pItem->AddItem(pPort);
		}
		Data.Val(pItem);
		Type = svLIST;
	}
	break;
	case SVID_LIST::PORTTRANSFERSTATUSLIST:
	{
		SecsItemList* pItem = new SecsItemList;
		for (int i = 0; i < Var.nLoadPort; i++) {
			SecsItemList* pItem2 = new SecsItemList;
			CCms* pCMS = &pGDev->pGEMObjs->Cms[i];
			Data.Val("LOAD PORT NUMBER");
			pItem2->AddItem(Data.GetType(svASCII));
			Data.Val(i + 1);
			pItem2->AddItem(Data.GetType(svUNSIGN1));
			Data.Val("TRANSFER STATUS");
			pItem2->AddItem(Data.GetType(svASCII));
			SecsItemList* pItem3 = new SecsItemList;
			Data.Val(pCMS->LPObj.GGetTransferStatus());
			pItem3->AddItem(Data.GetType(svUNSIGN1));
			Data.Val(pCMS->LPObj.GetTransferStatusStr());
			pItem3->AddItem(Data.GetType(svASCII));

			pItem2->AddItem(pItem3);
			pItem->AddItem(pItem2);
		}
		Data.Val(pItem);
		Type = svLIST;
	}
	break;
	case SVID_LIST::PORTLOCATIONID:
	{
		int ActivePort = pGDev->GetActivePort();
		if (ActivePort) {
			Data.Val(pGDev->pGEMObjs->Cms[ActivePort - 1].CaObj.LocationID);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;

	case SVID_LIST::LP1CARRIERIDATPLACEMENT:
	{
		BOOL bResult = FALSE;
		if (pGDev->pGEMObjs->Cms[0].LPObj.CarrierLoc == CLPObj::clUNDOCKED) {
			bResult = TRUE;
		}
		if (bResult) {
			char* psz;
			if (pGDev->pGEMObjs->Cms[0].CaObj.IDStatus == CCarrierObj::IDNOTREAD) {
				psz = "UNKNOWN";
			}
			else {
				psz = pGDev->pGEMObjs->Cms[0].CaObj.ObjID;
			}
			Data.Val(string(psz));
		}
		else {
			Data.Val("UNKNOWN");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP1CARRIERIDATDOCK:
	{
		BOOL bResult = FALSE;
		if (pGDev->pGEMObjs->Cms[0].LPObj.CarrierLoc == CLPObj::clDOCKED) {
			bResult = TRUE;
		}
		if (bResult) {
			char* psz;
			if (pGDev->pGEMObjs->Cms[0].CaObj.IDStatus == CCarrierObj::IDNOTREAD) {
				psz = "UNKNOWN";
			}
			else {
				psz = pGDev->pGEMObjs->Cms[0].CaObj.ObjID;
			}
			Data.Val(string(psz));
		}
		else {
			Data.Val("UNKNOWN");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP2CARRIERIDATPLACEMENT:
	{
		BOOL bResult = FALSE;
		if (pGDev->pGEMObjs->Cms[1].LPObj.CarrierLoc == CLPObj::clUNDOCKED) {
			bResult = TRUE;
		}
		if (bResult) {
			char* psz;
			if (pGDev->pGEMObjs->Cms[1].CaObj.IDStatus == CCarrierObj::IDNOTREAD) {
				psz = "UNKNOWN";
			}
			else {
				psz = pGDev->pGEMObjs->Cms[1].CaObj.ObjID;
			}
			Data.Val(string(psz));
		}
		else {
			Data.Val("UNKNOWN");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP2CARRIERIDATDOCK:
	{
		BOOL bResult = FALSE;
		if (pGDev->pGEMObjs->Cms[1].LPObj.CarrierLoc == CLPObj::clDOCKED) {
			bResult = TRUE;
		}
		if (bResult) {
			char* psz;
			if (pGDev->pGEMObjs->Cms[1].CaObj.IDStatus == CCarrierObj::IDNOTREAD) {
				psz = "UNKNOWN";
			}
			else {
				psz = pGDev->pGEMObjs->Cms[1].CaObj.ObjID;
			}
			Data.Val(string(psz));
		}
		else {
			Data.Val("UNKNOWN");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::LP1LOCATIONIDATPLACEMENT:
		Data.Val(pGDev->pGEMObjs->Cms[0].GetLocationStr(CLPObj::clUNDOCKED));
		Type = svASCII;
		break;
	case SVID_LIST::LP1LOCATIONIDATDOCK:
		Data.Val(pGDev->pGEMObjs->Cms[0].GetLocationStr(CLPObj::clDOCKED));
		Type = svASCII;
		break;
	case SVID_LIST::LP2LOCATIONIDATPLACEMENT:
		Data.Val(pGDev->pGEMObjs->Cms[1].GetLocationStr(CLPObj::clUNDOCKED));
		Type = svASCII;
		break;
	case SVID_LIST::LP2LOCATIONIDATDOCK:
		Data.Val(pGDev->pGEMObjs->Cms[1].GetLocationStr(CLPObj::clDOCKED));
		Type = svASCII;
		break;
	case SVID_LIST::SUBSTLOTID:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			Data.Val(pSubst->LotID);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::SUBSTDESTINATION:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			Data.Val(pSubst->SubstDestination);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::SUBSTHISTORY:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			string TempStr;
			SecsItemList* pItem = new SecsItemList;
			for (int nIndex = 0; nIndex < SUBSTHISTITEMLEN; nIndex++) {
				if (0 == strlen(pSubst->History[nIndex].SubstLocID)) {
					break;
				}
				SecsItemList* pHist = new SecsItemList;
				pHist->AddItem(new SecsItemASCII(pSubst->History[nIndex].SubstLocID));
				CTime Tm(pSubst->History[nIndex].TimeIn);
				::TimeStamp(Tm, TempStr);
				pHist->AddItem(new SecsItemASCII(TempStr));
				TempStr.erase();
				::TimeStamp(CTime(pSubst->History[nIndex].TimeOut), TempStr);
				pHist->AddItem(new SecsItemASCII(TempStr));
				pItem->AddItem(pHist);
			}
			Data.Val(pItem);
		}
		else {
			SecsItemList* pItem = new SecsItemList;
			Data.Val(pItem);
		}
		Type = svLIST;
	}
	break;
	case SVID_LIST::SUBSTID:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			Data.Val(pSubst->ObjID);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::SUBSTLOCATIONID:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			Data.Val(pSubst->SubstLocID);
		}
		else {
			Data.Val(0);
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::SUBSTLOCATIONSTATE:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (!pSubst) {
			break;
		}
		BOOL bOCC = TRUE;
		for (int nIndex = 0; nIndex < SUBSTHISTITEMLEN; nIndex++) {
			if (0 == strlen(pSubst->History[nIndex].SubstLocID)) {
				break;
			}
			bOCC = (pSubst->History[nIndex].TimeOut == 0);
		}
		Data.Val(bOCC);
		Type = svBOOLEAN;
	}
	break;
	case SVID_LIST::SUBSTPROCESSINGSTATE:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		int val = 0;
		if (pSubst) {
			val = pSubst->SubstProcState;
		}
		Data.Val(val);
		Type = svINT2;
	}
	break;
	case SVID_LIST::SUBSTSOURCE:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			Data.Val(pSubst->SubstSource);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::SUBSTSTATE:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			Data.Val(pSubst->SubstState);
		}
		else {
			Data.Val("");
		}
		Type = svASCII;
	}
	break;
	case SVID_LIST::SUBSTTYPE:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			Data.Val(pSubst->SubstType);
		}
		else {
			Data.Val(0);
		}
		Type = svINT2;
	}
	break;
	case SVID_LIST::SUBSTUSAGE:
	{
		CSubstObj* pSubst = pGDev->GetActiveSubstrateObj();
		if (pSubst) {
			Data.Val(pSubst->SubstUsage);
		}
		else {
			Data.Val(0);
		}
		Type = svINT2;
	}
	break;
	case SVID_LIST::SUBSTPROCESSINGSTATEENUM:
	{
		CSubstObj pS;
		SecsItemList* pList = new SecsItemList;
		for (int i = CSubstObj::NO_PROCSTATE; i <= CSubstObj::REJECTED; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(pS.GetSubstProcStateStr((CSubstObj::SUBSTPROCSTATE)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::SUBSTSTATEENUM:
	{
		CSubstObj pS;
		SecsItemList* pList = new SecsItemList;
		for (int i = CSubstObj::NO_TR_STATE; i <= CSubstObj::ATDESTINATION; i++) {
			SecsItemList* pList2 = new SecsItemList;
			pList2->AddItem(new SecsItemUInt2(i));
			pList2->AddItem(new SecsItemASCII(pS.GetSubstStateStr((CSubstObj::SUBSTSTATE)i).GetBuffer(0)));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::CHUCKSUBSTLOCSTATE:
	{
		UCHAR LocStat = 0;
		SecsItemList List;
		pGDev->GetSubstLocAttr("Scanner", "SubstLocState", &List);
		List[0].GetUInt1(LocStat);
		Data.Val(LocStat);
		Type = svASCII;
	}
	break;
	case SVID_LIST::CHUCKSUBSTID:
	{
		string TempStr;
		SecsItemList List;
		pGDev->GetSubstLocAttr("Scanner", "SubstID", &List);
		List[0].GetASCII(TempStr);
		Data.Val(TempStr);
		Type = svASCII;
	}
	break;

	case SVID_LIST::EVENTENABLEDLIST:
	{
		vector<UINT> vecEvents;
		vector<CString> vecEventTX;
		SecsItemList* pList = new SecsItemList;
		CEIDCtlr.GetEventsEnabled(vecEvents, vecEventTX);
		for (int nIndex = 0; nIndex<int(vecEvents.size()); nIndex++) {
			SecsItemList* pList2 = new SecsItemList;
			Data.Val(vecEvents[nIndex]);
			pList2->AddItem(Data.GetType(svUNSIGN2));
			Data.Val(vecEventTX[nIndex]);
			pList2->AddItem(Data.GetType(svASCII));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;

	case SVID_LIST::EVENTENUM:
	{
		vector<UINT> vecEvents;
		vector<CString> vecEventTX;
		SecsItemList* pList = new SecsItemList;
		CEIDCtlr.GetAllEvents(vecEvents, vecEventTX);
		for (int nIndex = 0; nIndex<int(vecEvents.size()); nIndex++) {
			SecsItemList* pList2 = new SecsItemList;
			Data.Val(vecEvents[nIndex]);
			pList2->AddItem(Data.GetType(svUNSIGN2));
			Data.Val(vecEventTX[nIndex]);
			pList2->AddItem(Data.GetType(svASCII));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;

	case SVID_LIST::ACTIVESOCKETPORT:
		Data.Val(m_ActPort);
		Type = svINT2;
		break;
	case SVID_LIST::PASSIVESOCKETPORT:
		Data.Val(m_SocketPortNo);
		Type = svINT2;
		break;
	case SVID_LIST::ACTIVEIPADDRESS:
		Data.Val(m_cActIP);
		Type = svASCII;
		break;
	case SVID_LIST::DEFAULTOFFLINESTATE:
		Data.Val(m_DefOfflineState);
		Type = svINT2;
		break;
	case SVID_LIST::DEFAULTONLINESTATE:
		Data.Val(m_DefOnlineState);
		Type = svINT2;
		break;
	case SVID_LIST::RSLTWAFER:	// Wafer Level result includes statistics and final bow warp data [9/8/2012 Administrator]
		Data.Val(ResultWafer(pEvt, Data, Type));
		Type = svLIST;
		break;
	case SVID_LIST::RSLTSINGLE:
		Data.Val(ResultSingle(pEvt, Data, Type));
		Type = svLIST;
		break;
	case SVID_LIST::CONNECTIONMODEENUM:
	{
		SecsItemList* pList = new SecsItemList;
		SecsItemList* pList2 = new SecsItemList;
		pList2->AddItem(new SecsItemUInt2(PASSIVE));
		pList2->AddItem(new SecsItemASCII("PASSIVE"));
		pList->AddItem(pList2);
		SecsItemList* pList3 = new SecsItemList;
		pList3->AddItem(new SecsItemUInt2(ACTIVE));
		pList3->AddItem(new SecsItemASCII("ACTIVE"));
		pList->AddItem(pList3);
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::SLOTMAPSTATUSENUM:
	{
		SecsItemList* pList = new SecsItemList;
		for (int i = CCarrierObj::SMNOSTATE; i <= CCarrierObj::SMVERIFICATIONFAIL; i++) {
			SecsItemList* pList2 = new SecsItemList;
			Data.Val(i);
			pList2->AddItem(Data.GetType(svINT2));
			Data.Val(CCarrierObj::GetSlotmapStatusStr((CCarrierObj::SLOTMAPSTATUS)i));
			pList2->AddItem(Data.GetType(svASCII));
			pList->AddItem(pList2);
		}
		Data.Val(pList);
		Type = svLIST;
	}
	break;
	case SVID_LIST::FFUSTATUS:
	{
		// 			float FFUPressure = 0;
		// 			if (p413App->pFFU)
		// 			{
		// 				FFUPressure = p413App->pFFU->FFUPressure;
		// 			}
		Data.Val(FFUPressNow);
		Type = svFLOAT4;
	}
	break;
	// 	case SVID_LIST::TOTALTHICK:
	// 		break;
	// 	case SVID_LIST::THICKNESS:
	// 		break;
	// 	case SVID_LIST::TRIMDEPTH:
	// 		break;
	// 	case SVID_LIST::TRIMWIDTH:
	// 		break;
	// 	case SVID_LIST::SDITHICKNESS:
	// 		break;
	// 	case SVID_LIST::ODTHICKNESS:
	// 		break;
	// 	case SVID_LIST::WARPAGE:
	// 		break;
	// 	case SVID_LIST::WAFERBOW:
	// 		break;
	// 	case SVID_LIST::SITENUMBER:
	// 		break;
	// 	case SVID_LIST::STATS:
	// 		break;
	// 	default:
	// 		ASSERT(0);
	// 		break;
	}

	return Data.GetType(Type);
}

CPRJob* CGemFactory::MakePrJob(PrjobRepNew& ProcessJob, vector<UINT>& ErrCodes) {
	CPRJob* pJob = NULL;
	string LogMsg;
	char MsgStr[156];
	ErrCodes.clear();
	if (PJobMgr.GetJobSpace()) {
		if (!PJobMgr.IsPRJobExist(ProcessJob.PrJobID.c_str())) {
			if ((ProcessJob.MF != 13) || (ProcessJob.RcpMethd != 2)) {
				ErrCodes.push_back(UNSUPPORTED_OPTION_REQUESTED);
				sprintf(MsgStr, "MF (%d) RecipeMethod (%d)", ProcessJob.MF, ProcessJob.RcpMethd);
				LogMsg += MsgStr;
			}
			else if (ProcessJob.CarrierID.empty() || ProcessJob.PrJobID.empty() || ProcessJob.RecipeID.empty()) {
				ErrCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
				sprintf(MsgStr, "<<Zero length ID>> Carrier(%s) PrjobID (%s) Recipe (%s)", ProcessJob.CarrierID.c_str(),
					ProcessJob.PrJobID.c_str(), ProcessJob.RecipeID.c_str());
				LogMsg += MsgStr;
			}
			else {
				pJob = new CPRJob;

				strscpy(pJob->m_ObjID, OBJIDLEN, ProcessJob.PrJobID.c_str());  // PrjobId
				if (!pGDev->IsCarrierPresent(ProcessJob.CarrierID)) {
					sprintf(MsgStr, "Carrier (%s) Not on Loadport", ProcessJob.CarrierID.c_str());
					LOG_WARN(MsgStr);
				}
				pJob->SetMtlName(ProcessJob.CarrierID.c_str());
				pJob->m_PortNo = pGDev->GetPortID(ProcessJob.CarrierID);
				USHORT SlotNo;
				// IMP :Assmption the film thickness is provided in Angtroms.
				float FtF = 1;
				FtF = (DosUtil.GetFilmThkFactor(FtF));
				for (int nIndex = 0; nIndex < int(ProcessJob.WaferParams.size()); nIndex++) {
					SlotNo = ProcessJob.WaferParams[nIndex].SlotNo;
					if ((SlotNo == 0) || (SlotNo > 25)) {
						sprintf(MsgStr, "Invalid Slot (%d) specified", SlotNo);
						LOG_WARN(MsgStr);
						ErrCodes.push_back(PARAMETERS_IMPROPERLY_SPECIFIED);
						continue;
					}
					pJob->SetParamWaferThickness(ProcessJob.WaferParams[nIndex].WThick, SlotNo - 1);
					pJob->SetParamFilmThickness((ProcessJob.WaferParams[nIndex].FThick * FtF), SlotNo - 1);
					pJob->SetParamSlotNo(ProcessJob.WaferParams[nIndex].SlotNo, SlotNo - 1);
				}
				pJob->m_Slots = ProcessJob.Slots;
				pJob->SetMF(ProcessJob.MF);
				pJob->SetPRStart(ProcessJob.bAutoStart);
				// 				pJob->SetNumberScan(ProcessJob.NumScanLines);
				// 				pJob->SetScanNo(ProcessJob.ScanNo);
				pJob->SetMtlType(1);
				pJob->m_PauseEvents = ProcessJob.PauseEvts;
				pJob->m_Recipe = ProcessJob.RecipeID;
				//ASSERT(!"All references of recipe removed from online");
				if (!RcpMgr.IsRecipeAvailable(pJob->m_Recipe))
					//if (!CRecipe::LocateRecipe(pJob->m_Recipe))
				{
					sprintf(MsgStr, "(S16F13) Recipe (%s) Not on Equipment", pJob->m_Recipe.c_str());
					LOG_WARN(MsgStr);
				}
			}
		}
		else {
			ErrCodes.push_back(OBJECT_IDENTIFIER_IN_USE);
			sprintf(MsgStr, "Duplicate PrjobID (%s) ", ProcessJob.PrJobID.c_str());
			LOG_ERR(MsgStr);
		}
	}
	else {
		ErrCodes.push_back(BUSY);
		sprintf(MsgStr, "PrJobSpace FULL ");
		LOG_ERR(MsgStr);
	}
	if (ErrCodes.size()) {
		delete pJob;
		pJob = NULL;
	}
	return pJob;
}

SecsItem* CGemFactory::MakeReport(CEvt* pEvt, UINT CEID, UINT Id) {
	//20120920 SCOTT EDIT if (Id == EVT_CMS)  // This is a patch until the whole thing can be unified [8/18/2010 XPMUser]
// 	if(1)
// 	{
	if ((CEID >= 0) && (CEID < CEID_LIST::LASTCEID)) {
		CEID = CEIDCtlr.m_CEList.GetID(CEID);
	}
	//	}

	if (!CEIDCtlr.Check_Ceid(CEID)) {
		return NULL;
	}

	SecsItemList* pReportList = new SecsItemList;
	UINT nReports = CEIDCtlr.get_rptcount(CEID), nSVIDs = 0; // REPORT COUNTS.

	UINT* pReportIDs = NULL, * pSVIDs = NULL;
	int nTemp = 0, nIndex = 0;
	string SvName;
	if (nReports == 0) {
	}
	else {
		UINT DATAID = 0;
		pReportIDs = new UINT[nReports];
		CEIDCtlr.getting_rptno(CEID, pReportIDs);

		for (UINT iRpt = 0; iRpt < nReports; iRpt++) {
			// Each report
			SecsItemList* pSingleReport = new SecsItemList;
			//TRACE("CEID %d 0X%X\n",CEID,pSingleReport);
			pSingleReport->AddItem(new SecsItemUInt4(pReportIDs[iRpt]));

			nSVIDs = CEIDCtlr.get_svidcount(pReportIDs[iRpt]);
			pSVIDs = new UINT[nSVIDs];

			CEIDCtlr.get_svid(pReportIDs[iRpt], pSVIDs);
			string Temp;
			SecsItemList* pSvList = new SecsItemList;

			for (UINT iSVID = 0; iSVID < nSVIDs; iSVID++) {
				pSvList->AddItem(get_svval(1, pSVIDs[iSVID], pEvt));
			}
			pSingleReport->AddItem(pSvList);
			pReportList->AddItem(pSingleReport);

			delete[]pSVIDs;
		}//Reports
		delete[]pReportIDs;
	}
	return pReportList;
}

void CGemFactory::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CGemFactory");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

BOOL CGemFactory::MakeSlotMap(char* nBuf, short PortNo) {
	if (PortNo < 1 || PortNo > 4) {
		return FALSE;
	}
	int nIndex;
	CCarrierObj* pCarrier = NULL;

	pCarrier = &pGDev->pGEMObjs->Cms[PortNo - 1].CaObj;
	nBuf[0] = '0';
	for (nIndex = 0; nIndex < MAXCAPACITYLEN; nIndex++) {
		switch (pCarrier->SlotMaP[nIndex]) {
		case CSubstObj::SM_EMPTY:
			nBuf[nIndex] = CSubstObj::GetSMChar(CSubstObj::SM_EMPTY);
			break;
		case CSubstObj::SM_CORRECTLY_OCCUPIED:
			nBuf[nIndex] = CSubstObj::GetSMChar(CSubstObj::SM_CORRECTLY_OCCUPIED);
			break;
		case CSubstObj::SM_CROSS_SLOTTED:
			nBuf[nIndex] = CSubstObj::GetSMChar(CSubstObj::SM_CROSS_SLOTTED);
			break;
		case CSubstObj::SM_DOUBLE_SLOTTED:
			nBuf[nIndex] = CSubstObj::GetSMChar(CSubstObj::SM_DOUBLE_SLOTTED);
			break;
		default:
			nBuf[nIndex] = CSubstObj::GetSMChar(CSubstObj::SM_UNDEFINED);
			break;
		}
	}
	nBuf[nIndex] = 0;
	return TRUE;
}

char* CGemFactory::ToChar(int val) {
	sprintf(ctmpStr, "%d", val);
	return ctmpStr;
}

char* CGemFactory::ToChar(UINT val) {
	sprintf(ctmpStr, "%d", val);
	return ctmpStr;
}

char* CGemFactory::ToChar(float val) {
	sprintf(ctmpStr, "%f", val);
	return ctmpStr;
}

char* CGemFactory::ToChar(double val) {
	sprintf(ctmpStr, "%f", val);
	return ctmpStr;
}

void CGemFactory::Clamp(short port) {
	pUDev->ClampCarrier(port);
}

SecsItemList* CGemFactory::ResultSingle(CEvt* pEvt, DataConverter& Data, UINT& Type) {
	CString str;
	SecsItemList* pList = new SecsItemList;
	if (pEvt && (pEvt->pMPoint) && (pEvt->pMeParam)) {
		CMeParam* pMe = (CMeParam*)pEvt->pMeParam;
		CMPoint* pMPoint = (CMPoint*)pEvt->pMPoint;
		CData* d = pMPoint->GetData(0, FALSE);
		if (!d) {
			// Should not come here [5/14/2013 user]
			ASSERT(0);
			return NULL;
		}
		Data.Val(pMPoint->GetXCoorStr());		// Coordinate X
		pList->AddItem(Data.GetType(svASCII));
		Data.Val(pMPoint->GetYCoorStr());
		pList->AddItem(Data.GetType(svASCII));	// Coordinate Y
		Data.Val(pMPoint->Co.GetNStr());
		pList->AddItem(Data.GetType(svASCII));	// No of Point.

		// Note: d->DVal only covers the first MeSet [11/8/2011 Administrator]
		for (int i = 0; i < MaXPA; i++) {
			MParam* mp = &pMe->MPa[i];
			if ((mp->Prb > 0) && (mp->bHost) && !((mp->Prb == MParam::BOWWARP1) || (mp->Prb == MParam::BOWWARP2))) // Exclude initial bow warp data [9/8/2012 Administrator]
			{
				SecsItemList* pList1 = new SecsItemList;
				Data.Val(mp->Name);
				pList1->AddItem(Data.GetType(svASCII));
				float fdata = d->Get(i);
				CString dec1;
				dec1.Format("%%.%df", mp->D);
				str.Format(dec1, fdata);
				if ((fdata == BADDATANEG) || (fdata < 0)) {
					fdata = BADDATA;
					str = "N/A";
				}
				Data.Val(str);
				pList1->AddItem(Data.GetType(svASCII));
				pList->AddItem(pList1);
			}
		}
	}
	return pList;
}

// Note: TCP/IP buffer of 10K bytes could overflow [9/8/2012 Administrator]
SecsItemList* CGemFactory::ResultWafer(CEvt* pEvt, DataConverter& Data, UINT& Type) {
	CString str;
	CString StatName[8] = { "Max", "Min", "Ave", "StDev", "TTV", "Bow", "Warp" };

	SecsItemList* pList = new SecsItemList;
	if (pEvt && pEvt->pRcp) {
		SecsItemList* pList2;
		CRecipe413* pRcp = pEvt->pRcp;
		CPattern* pPatt = &pRcp->Patt;

		// Step 1: Send statistics [9/8/2012 Administrator]
		for (int j = 0; j < MAXMEPASET; j++) {
			CMeParam* pMeP = &pRcp->MeParam[j];
			short jj = j * MaXPA;
			for (int i = 0; i < MaXPA; i++) {
				if (pMeP->MPa[i].Prb > 0) {
					CString str1;
					CString cjName;
					SecsItemList* pList1 = new SecsItemList;
					SecsItemList* pList3 = new SecsItemList;
					Data.Val(pMeP->MPa[i].Name);
					pList1->AddItem(Data.GetType(svASCII));
					pList1->AddItem(pList3);
					for (int k = 0; k < 7; k++) {
						switch (k) {
						case 0: // Max [9/8/2012 Administrator]
							cjName = "MAX";
							str1.Format("%.3f", pPatt->MP.Stats[jj + i].m_fMax);
							pList2 = new SecsItemList;
							Data.Val(cjName);
							pList2->AddItem(Data.GetType(svASCII));
							Data.Val(str1);
							pList2->AddItem(Data.GetType(svASCII));
							pList3->AddItem(pList2);
							break;
						case 1: // Min [9/8/2012 Administrator]
							cjName = "MIN";
							str1.Format("%.3f", pPatt->MP.Stats[jj + i].m_fMin);
							pList2 = new SecsItemList;
							Data.Val(cjName);
							pList2->AddItem(Data.GetType(svASCII));
							Data.Val(str1);
							pList2->AddItem(Data.GetType(svASCII));
							pList3->AddItem(pList2);
							break;
						case 2: // Ave [9/8/2012 Administrator]
							cjName = "AVE";
							str1.Format("%.3f", pPatt->MP.Stats[jj + i].m_fAverage);
							pList2 = new SecsItemList;
							Data.Val(cjName);
							pList2->AddItem(Data.GetType(svASCII));
							Data.Val(str1);
							pList2->AddItem(Data.GetType(svASCII));
							pList3->AddItem(pList2);
							break;
						case 3: // StDev [9/8/2012 Administrator]
							cjName = "STDEV";
							str1.Format("%.3f", pPatt->MP.Stats[jj + i].m_fSTD);
							pList2 = new SecsItemList;
							Data.Val(cjName);
							pList2->AddItem(Data.GetType(svASCII));
							Data.Val(str1);
							pList2->AddItem(Data.GetType(svASCII));
							pList3->AddItem(pList2);
							break;
						case 4: // TTV [9/8/2012 Administrator]
							if (pMeP->MPa[i].Fr == 1) {
								cjName = "TTV";
								str1 = "";
								if (pMeP->MPa[i].IsTTV()) {
									str1.Format("%.3f", pPatt->MP.Stats[jj + i].m_fTTV);
								}
								pList2 = new SecsItemList;
								Data.Val(cjName);
								pList2->AddItem(Data.GetType(svASCII));
								Data.Val(str1);
								pList2->AddItem(Data.GetType(svASCII));
								pList3->AddItem(pList2);
							}
							else {
								cjName = "TTV";
								str1.Format("%.3f", pPatt->MP.Stats[jj + i].m_fTTV);
								pList2 = new SecsItemList;
								Data.Val(cjName);
								pList2->AddItem(Data.GetType(svASCII));
								Data.Val(str1);
								pList2->AddItem(Data.GetType(svASCII));
								pList3->AddItem(pList2);
							}
							break;
						case 5: // Bow [9/8/2012 Administrator]
							if ((pMeP->MPa[i].Prb == MParam::BOWWARP1) || (pMeP->MPa[i].Prb == MParam::BOWWARP2) || (pMeP->MPa[i].Prb == MParam::CONFOCAL_WARP) || (pMeP->MPa[i].Prb == MParam::CONFOCAL_DUALWARP)) { //[23102024 added CONFOCAL_DUALWARP
								cjName = "BOW";
								str1.Format("%.3f", pPatt->MP.Stats[jj + i].m_fBow);
								pList2 = new SecsItemList;
								Data.Val(cjName);
								pList2->AddItem(Data.GetType(svASCII));
								Data.Val(str1);
								pList2->AddItem(Data.GetType(svASCII));
								pList3->AddItem(pList2);
							}
							break;
						case 6: // Warp [9/8/2012 Administrator]
							if ((pMeP->MPa[i].Prb == MParam::BOWWARP1) || (pMeP->MPa[i].Prb == MParam::BOWWARP2) || (pMeP->MPa[i].Prb == MParam::CONFOCAL_WARP) || (pMeP->MPa[i].Prb == MParam::CONFOCAL_DUALWARP)) { //[23102024 added CONFOCAL_DUALWARP
								cjName = "WARP";
								str1.Format("%.3f", pPatt->MP.Stats[jj + i].m_fWarp);
								pList2 = new SecsItemList;
								Data.Val(cjName);
								pList2->AddItem(Data.GetType(svASCII));
								Data.Val(str1);
								pList2->AddItem(Data.GetType(svASCII));
								pList3->AddItem(pList2);
							}
							break;
						}
					}
					pList->AddItem(pList1);
				}
			}
		}

		// Step 2: Send final bow warp data [9/8/2012 Administrator]
		CPtrList* pMPList = &pPatt->MP.MPLst;
		if (pMPList) {
			POSITION pos = pMPList->GetHeadPosition();
			while (pos) {
				CMPoint* pMPoint = (CMPoint*)pMPList->GetNext(pos);
				if (pMPoint) {
					CMeParam* pMeP = &pRcp->MeParam[pMPoint->MeSet - 1];
					if (pMeP->IsBowWarp()) {
						SecsItemList* pList5 = new SecsItemList;
						CData* d = pMPoint->GetData(0, FALSE);
						if (!d) {
							ASSERT(0);
						}
						Data.Val(pMPoint->GetXCoorStr());
						pList5->AddItem(Data.GetType(svASCII));
						Data.Val(pMPoint->GetYCoorStr());
						pList5->AddItem(Data.GetType(svASCII));
						Data.Val(pMPoint->Co.GetNStr());
						pList5->AddItem(Data.GetType(svASCII));

						// Note: d->DVal only covers the first MeSet [11/8/2011 Administrator]
						for (int i = 0; i < MaXPA; i++) {
							MParam* mp = &pMeP->MPa[i];
							if ((mp->Prb > 0) && (mp->bHost) && ((mp->Prb == MParam::BOWWARP1) || (mp->Prb == MParam::BOWWARP2))) // Only report bow warp data [9/8/2012 Administrator]
							{
								SecsItemList* pList4 = new SecsItemList;
								Data.Val(mp->Name);
								pList4->AddItem(Data.GetType(svASCII));
								float fdata = d->Get(i);
								CString dec1;
								dec1.Format("%%.%df", mp->D);
								str.Format(/*"%.4f"*/dec1, fdata);
								if (fdata == BADDATANEG) {
									fdata = BADDATA;
									str = "N/A";
								}
								Data.Val(str);
								pList4->AddItem(Data.GetType(svASCII));
								pList5->AddItem(pList4);
							}
						}
						pList->AddItem(pList5);
					}
				}
			}
		}
	}
	return pList;
}

// Yuen: Format of PPBODY is contained in Recipe.BinaryToRecipe [2002/10/31  19:44]
int CGemFactory::mk_S7F26(GEM_HEADER* shead) {
	int retc = 0;
	MessageS7F5* pMsg = new MessageS7F5;
	if (!pMsg) {
		ASSERT(0);
		return retc;
	}

	BYTE ACKC7 = 0;
	string MsgStr = "S7F25/F26 Formatted Process Program Request (FPR/FPD) ";
	LOG_INFO(MsgStr);

	BOOL bFound = FALSE, bValid = FALSE;
	BYTE* pByte = NULL;
	if (pMsg->CheckMessage(shead->m_DataLen, shead->m_Message)) {
		MsgStr += ("(" + pMsg->PPID + ") ");
		if (pMsg->PPID.size() && RcpMgr.IsRecipeAvailable(pMsg->PPID)) {
			CRecipe413* pRcp = new CRecipe413;
			if (!pRcp) {
				ASSERT(0);
			}
			bFound = TRUE;
			CString rStr = pMsg->PPID.c_str();
			DosUtil.RemoveExtension(rStr);
			pRcp->SetRecipeName(rStr.GetBuffer(0));
			if (RcpMgr.Load(pRcp)) {
				SecsItemList* pList = new SecsItemList;
				SecsItemASCII* pPPID = new SecsItemASCII(pMsg->PPID);
				pList->AddItem(pPPID);	// Recipe name [1/20/2013 Administrator]
				pList->AddItem(new SecsItemASCII(string(CEIDCtlr.m_SVList.m_Svid[SVID_LIST::DEVICEID].GetV()))); // Model number [1/20/2013 Administrator]
				rStr.Format("%d", pRcp->Version);
				pList->AddItem(new SecsItemASCII(rStr.GetBuffer(0))); // Version [1/20/2013 Administrator]

				SecsItemList* pListTmp;
				SecsItemList* pList2 = new SecsItemList;
				pListTmp = GenMeSetAll(pRcp);
				if (pListTmp) {
					pList2->AddItem(pListTmp);
				}
				pListTmp = GenSubSamplingParam(pRcp);
				if (pListTmp) {
					pList2->AddItem(pListTmp);
				}
				pListTmp = GenPatMatParam(pRcp);
				if (pListTmp) {
					pList2->AddItem(pListTmp);
				}
				pListTmp = GenMeaTimeout(pRcp);
				if (pListTmp) {
					pList2->AddItem(pListTmp);
				}

				pList->AddItem(pList2);
				MsgStr += " Done";

				LOG_INFO(MsgStr);
				SecsMessage OutMsg;
				OutMsg.AddItem(pList);
				OutMsg.MakeBinaryMessage(shead->m_Message);
				retc = set_head(0, 7, 26, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
				SendBlock(shead);
				if (pMsg) {
					delete pMsg;
					pMsg = NULL;
				}
				if (pRcp) delete pRcp;
				return retc;
			}
			else {
				MsgStr += "Err (Rcp Read Fail)";
			}
		}
		else {
			MsgStr += "Err (Rcp Not Found)";
			SecsMessage OutMsg;
			SecsItemList* pList = new SecsItemList;
			OutMsg.AddItem(pList);
			OutMsg.MakeBinaryMessage(shead->m_Message);
			retc = set_head(0, 7, 26, shead, OutMsg.BinaryMessageLength() + 10, PRI_HOST);
			SendBlock(shead);
			if (pMsg) {
				delete pMsg;
				pMsg = NULL;
			}
			return retc;
		}
	}
	LOG_ERR("(S7F25) Incorrect Format");
	mk_S9F7(shead);
	if (pMsg) {
		delete pMsg;
		pMsg = NULL;
	}
	return retc;
}

SecsItemList* CGemFactory::GenMeItemEcho(MParam* pM) {
	SecsItemList* pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemUInt2(pM->Prb));
	pList4->AddItem(new SecsItemASCII(pM->Name.GetBuffer(0)));
	pList4->AddItem(new SecsItemUInt2(pM->Fr));
	pList4->AddItem(new SecsItemUInt2(pM->To));
	pList4->AddItem(new SecsItemFloat4(pM->RI));
	pList4->AddItem(new SecsItemFloat4(pM->Lo));
	pList4->AddItem(new SecsItemFloat4(pM->Hi));
	pList4->AddItem(new SecsItemFloat4(pM->DF));
	pList4->AddItem(new SecsItemFloat4(pM->Off));
	pList4->AddItem(new SecsItemUInt2(pM->bHost));

	return pList4;
}

SecsItemList* CGemFactory::GenMeItem(short j, MParam* pM, CGemRtn& GR) {
	CString MeSetStr;
	SecsItemList* pList2 = new SecsItemList;

	MeSetStr.Format("MeItem %d", j + 1);
	pList2->AddItem(new SecsItemASCII(MeSetStr.GetBuffer(0)));

	if (pM->IsEchoMeasurement()) {
		GR.bOK1 = TRUE;
		GR.bIsEcho = TRUE;
		pList2->AddItem(GenMeItemEcho(pM));
	}
	else if (pM->IsThinFilmMeasurement()) {
		GR.bOK2 = TRUE;
		GR.bIsThinF = TRUE;
		pList2->AddItem(GenMeItemEcho(pM));
	}
	else if (pM->IsEdgeMeasurement()) {
		GR.bIsTrim = TRUE;
		pList2->AddItem(GenMeItemEcho(pM));
	}
	else if (pM->IsDisplayPrb()) {
		pList2->AddItem(GenMeItemEcho(pM));
	}

	return pList2;
}

SecsItemList* CGemFactory::GenMeSet(short i, CMeParam* pMe, CRecipe413* pRcp, CGemRtn& GR) {
	BOOL bYes = FALSE;
	CString MeSetStr;
	SecsItemList* pList1 = new SecsItemList;
	SecsItemList* pList2 = new SecsItemList;
	MeSetStr.Format("MeSet %d", i + 1);
	pList1->AddItem(new SecsItemASCII(MeSetStr.GetBuffer(0)));
	pList1->AddItem(new SecsItemInt2(pMe->NAve));
	for (int j = 0; j < MaXPA; j++) {
		MParam* pM = &pMe->MPa[j];
		if (pM->Prb > 0) {
			bYes = TRUE;
			pList1->AddItem(GenMeItem(j, pM, GR));
		}
	}
	if (bYes) pList2->AddItem(pList1);
	if (GR.bOK1) {
		CString PCStr[] = { "Peak Control Top", "Peak Control Bottom" };
		for (int k = 0; k < 2; k++) {
			SecsItemList* pList3 = new SecsItemList;
			pList3->AddItem(new SecsItemASCII(PCStr[k].GetBuffer(0)));
			SecsItemList* pList4 = new SecsItemList;
			pList4->AddItem(new SecsItemUInt2(pMe->PSG[k].Thres));
			pList4->AddItem(new SecsItemUInt2(pMe->PSG[k].Level));
			pList4->AddItem(new SecsItemFloat4(pMe->PSG[k].Width));
			pList4->AddItem(new SecsItemUInt2(pMe->PSG[k].Near));
			pList4->AddItem(new SecsItemUInt2(pMe->PSG[k].Smooth));
			pList4->AddItem(new SecsItemUInt2(pMe->PSG[k].Minimum));
			pList3->AddItem(pList4);
			pList2->AddItem(pList3);
		}
	}
	if (GR.bOK2) {
		SecsItemList* pList3 = new SecsItemList;
		pList3->AddItem(new SecsItemASCII("Thin Film Ranges"));
		SecsItemList* pList4 = new SecsItemList;
		for (int l = 0; l < MAXTHKVLU; l++) {
			SecsItemList* pList7 = new SecsItemList;
			CString str;
			str.Format("TF Range %d", l + 1);
			pList7->AddItem(new SecsItemASCII(str.GetBuffer(0)));
			SecsItemList* pList6 = new SecsItemList;
			pList6->AddItem(new SecsItemFloat4(pMe->Ta.RI[l]));
			pList6->AddItem(new SecsItemFloat4(pMe->Ta.ThkMax[l]));
			pList6->AddItem(new SecsItemFloat4(pMe->Ta.ThkMin[l]));
			pList7->AddItem(pList6);
			pList4->AddItem(pList7);
		}
		pList3->AddItem(pList4);
		pList3->AddItem(new SecsItemASCII("Spectrometer Control"));
		pList4 = new SecsItemList;
		pList4->AddItem(new SecsItemUInt2(pMe->Ta.IntegTime));
		pList4->AddItem(new SecsItemUInt2(pMe->Ta.Boxcar));
		pList4->AddItem(new SecsItemUInt2(pMe->Ta.Method));
		pList4->AddItem(new SecsItemUInt2(pRcp->FFTFilter[i]));
		pList3->AddItem(pList4);
		pList2->AddItem(pList3);
	}
	if (bYes) {
		return pList2;
	}
	delete pList2;
	return NULL;
}

SecsItemList* CGemFactory::GenEchoParam(CRecipe413* pRcp) {
	SecsItemList* pList3 = new SecsItemList;
	pList3->AddItem(new SecsItemASCII("Search Around"));
	SecsItemList* pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemFloat4(pRcp->fStepSizeEcho));
	pList4->AddItem(new SecsItemInt4(pRcp->nTrialEcho));
	pList3->AddItem(pList4);
	pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemASCII("Echo Exposures"));
	pList4->AddItem(new SecsItemInt2(pRcp->EchoNormalExposure));
	pList4->AddItem(new SecsItemInt2(pRcp->AutoExposeEcho));
	pList3->AddItem(pList4);
	pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemASCII("Echo Motor"));
	SecsItemList* pList5 = new SecsItemList;
	pList5->AddItem(new SecsItemASCII(pRcp->MotorFile.GetBuffer(0)));
	pList5->AddItem(new SecsItemInt2(pRcp->bUseMotorFile));
	if (pRcp->bUseMotorFile) {
		pList5->AddItem(new SecsItemInt4(pRcp->SamplingFreq));
		pList5->AddItem(new SecsItemInt4(pRcp->NumberOfInterestedData));
	}
	else {
		pList5->AddItem(new SecsItemInt4(DEFFREQ));
		pList5->AddItem(new SecsItemInt4(DEFDATASET));
	}
	pList4->AddItem(pList5);
	pList3->AddItem(pList4);

	return pList3;
}

SecsItemList* CGemFactory::GenThinFParam(CRecipe413* pRcp) {
	SecsItemList* pList4;
	SecsItemList* pList3 = new SecsItemList;
	pList3->AddItem(new SecsItemASCII("Thin Film Offsets"));
	for (int i = 0; i < MAXMEPASET; i++) {
		for (int j = 0; j < 2; j++) {
			pList4 = new SecsItemList;
			pList4->AddItem(new SecsItemFloat4(pRcp->POffsetX[i][j]));
			pList4->AddItem(new SecsItemFloat4(pRcp->POffsetY[i][j]));
			pList3->AddItem(pList4);
		}
	}

	pList3->AddItem(new SecsItemASCII("Search Around"));
	pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemFloat4(pRcp->fStepSizeEcho));
	pList4->AddItem(new SecsItemUInt2(pRcp->nTrialEcho));
	pList3->AddItem(pList4);

	pList3->AddItem(new SecsItemASCII("Thin Film Exposures"));
	pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemUInt4(pRcp->TFNormalExposure));
	pList4->AddItem(new SecsItemUInt4(pRcp->TFTakeSpecExposure));
	pList4->AddItem(new SecsItemInt2(pRcp->AutoExposeTF));
	pList3->AddItem(pList4);

	pList3->AddItem(new SecsItemASCII("Thin Film Settings"));
	pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemInt4(pRcp->nMappedPoints));
	pList4->AddItem(new SecsItemInt2(pRcp->nCycle));
	pList4->AddItem(new SecsItemInt2(pRcp->nExpand));
	pList4->AddItem(new SecsItemInt2(pRcp->NSmooth));
	pList3->AddItem(pList4);

	return pList3;
}

SecsItemList* CGemFactory::GenTrimParam(CRecipe413* pRcp) {
	SecsItemList* pList3 = new SecsItemList;
	pList3->AddItem(new SecsItemASCII("Trim Width Settings"));
	SecsItemList* pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemUInt2(pRcp->TrimWidthSpacing));
	pList4->AddItem(new SecsItemUInt2(pRcp->TrimWidthLines));
	pList4->AddItem(new SecsItemFloat4(pRcp->TWExposureInner));
	pList4->AddItem(new SecsItemFloat4(pRcp->TWExposureOuter));
	pList4->AddItem(new SecsItemFloat4(pRcp->TWExposureBevel));
	pList4->AddItem(new SecsItemFloat4(pRcp->TrimWidthOuterRadius));
	pList3->AddItem(pList4);

	pList3->AddItem(new SecsItemASCII("Trim Height Settings"));
	pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemFloat4(pRcp->TrimHeightDistanceInner1));
	pList4->AddItem(new SecsItemFloat4(pRcp->TrimHeightDistanceInner2));
	pList4->AddItem(new SecsItemFloat4(pRcp->TrimHeightDistanceOuter1));
	pList4->AddItem(new SecsItemFloat4(pRcp->TrimHeightDistanceOuter2));
	pList4->AddItem(new SecsItemFloat4(pRcp->TrimHeightZOffset));
	// 	pList4->AddItem(new SecsItemFloat4(pRcp->CXOffset));
	// 	pList4->AddItem(new SecsItemFloat4(pRcp->CYOffset));
	pList4->AddItem(new SecsItemInt2(pRcp->NMeasureOut));
	pList4->AddItem(new SecsItemInt2(pRcp->NMeasureIn));
	pList3->AddItem(pList4);

	pList4 = new SecsItemList;
	pList4->AddItem(new SecsItemFloat4(pRcp->fStepSizeTrim));
	pList4->AddItem(new SecsItemUInt2(pRcp->nTrialTrim));
	pList3->AddItem(pList4);

	return pList3;
}

SecsItemList* CGemFactory::GenMeSetAll(CRecipe413* pRcp) {
	BOOL bAdd = FALSE;
	CGemRtn GR;
	SecsItemList* pList2 = new SecsItemList;
	pList2->AddItem(new SecsItemASCII("MeSets"));
	for (int i = 0; i < MAXMEPASET; i++)	// MeSet dump [1/20/2013 Administrator]
	{
		CMeParam* pMe = &pRcp->MeParam[i];
		SecsItemList* pList3 = GenMeSet(i, pMe, pRcp, GR);
		if (pList3) {
			bAdd = TRUE;
			pList2->AddItem(pList3);
		}
	}
	if (GR.bIsEcho) {
		SecsItemList* pList3 = new SecsItemList;
		pList3->AddItem(new SecsItemASCII("Echo Parameters"));
		pList3->AddItem(GenEchoParam(pRcp));
		pList2->AddItem(pList3);
	}
	if (GR.bIsThinF) {
		SecsItemList* pList3 = new SecsItemList;
		pList3->AddItem(new SecsItemASCII("Thin Film Parameters"));
		pList3->AddItem(GenThinFParam(pRcp));
		pList2->AddItem(pList3);
	}
	if (GR.bIsTrim) {
		SecsItemList* pList3 = new SecsItemList;
		pList3->AddItem(new SecsItemASCII("Trim Parameters"));
		pList3->AddItem(GenTrimParam(pRcp));
		pList2->AddItem(pList3);
	}

	if (bAdd) {
		return pList2;
	}
	delete pList2;
	return NULL;
}

SecsItemList* CGemFactory::GenSubSamplingParam(CRecipe413* pRcp) {
	// Pattern Matching parameters [7/3/2012 user]
	BOOL bFirst = TRUE;
	SecsItemList* pList2 = new SecsItemList;
	SecsItemList* pList3 = new SecsItemList;
	for (int l = 0; l < MAXMEPASET; l++) {
		if (pRcp->MatchDIBA1[l].pDIB) {
			if (bFirst) {
				bFirst = FALSE;
				pList2->AddItem(new SecsItemASCII("Sub-sampling"));
			}
			SecsItemList* pList5 = new SecsItemList;
			CString PatrStr;
			PatrStr.Format("Pattern %d", l + 1);
			pList5->AddItem(new SecsItemASCII(PatrStr.GetBuffer(0)));
			SecsItemList* pList4 = new SecsItemList;
			pList4->AddItem(new SecsItemUInt2(pRcp->SubSamI[l]));
			pList4->AddItem(new SecsItemUInt2(pRcp->SubSamS[l]));
			pList5->AddItem(pList4);
			pList3->AddItem(pList5);
		}
	}
	if (!bFirst) {
		pList2->AddItem(pList3);
		return pList2;
	}
	delete pList2;
	delete pList3;
	return NULL;
}

SecsItemList* CGemFactory::GenPatMatParam(CRecipe413* pRcp) {
	BOOL bFirst = TRUE;
	SecsItemList* pList2 = new SecsItemList;
	SecsItemList* pList3 = new SecsItemList;
	for (int l = 0; l < MAXMEPASET; l++) {
		if (pRcp->MatchDIBA1[l].pDIB) {
			if (bFirst) {
				bFirst = FALSE;
				pList2->AddItem(new SecsItemASCII("Matching Parameters"));
			}
			SecsItemList* pList5 = new SecsItemList;
			CString PatrStr;
			PatrStr.Format("Pattern %d", l + 1);
			pList5->AddItem(new SecsItemASCII(PatrStr.GetBuffer(0)));
			SecsItemList* pList4 = new SecsItemList;
			pList4->AddItem(new SecsItemUInt2(pRcp->GoFScore[l]));
			pList4->AddItem(new SecsItemUInt2(pRcp->MatchRes[l]));
			pList4->AddItem(new SecsItemUInt2(pRcp->SearchDept[l]));
			pList4->AddItem(new SecsItemUInt2(pRcp->SearchDist[l]));
			pList5->AddItem(pList4);
			pList4 = new SecsItemList;
			pList4->AddItem(new SecsItemFloat4(pRcp->fStepSizePat));
			pList4->AddItem(new SecsItemUInt2(pRcp->nTrialPat));
			pList5->AddItem(pList4);
			pList3->AddItem(pList5);
		}
	}
	if (!bFirst) {
		pList2->AddItem(pList3);
		return pList2;
	}

	delete pList2;
	delete pList3;
	return NULL;
}

SecsItemList* CGemFactory::GenMeaTimeout(CRecipe413* pRcp) {
	SecsItemList* pList2 = new SecsItemList;
	pList2->AddItem(new SecsItemASCII("Measurement Timeout"));
	SecsItemList* pList3 = new SecsItemList;
	pList3->AddItem(new SecsItemUInt2(pRcp->WaferTimeOut));
	pList3->AddItem(new SecsItemUInt2(pRcp->PointTimeOut));
	pList3->AddItem(new SecsItemUInt2(pRcp->FailCountTimeOut));
	pList2->AddItem(pList3);

	return pList2;
}