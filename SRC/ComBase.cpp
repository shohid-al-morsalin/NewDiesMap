// ComBase.cpp: implementation of the CComBase class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ComBase.h"
#include "..\Src\Comm.h"
#include "..\IPC\SMHdr.h"
#include "..\IPC\SMCHdr.h"
#include "..\Sys\Var.h"
#include "ComFL300.h"
#include "..\Src\DOSUtil.h"
#include "ComThread.h"
#include "ComTDK.h"
#include "..\IPC\SMOL.h"
#include "..\Src\DosUtil.h"
//#include "c2c.h"
#include "resource.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComBase::CComBase()
{
	pCom = NULL;
	hWnd = NULL;
	hWndLPBar = NULL;
	SvrName = "";
	
	bSimu = FALSE;
	bStarted = FALSE;
	memset(Slotmap,0,sizeof(Slotmap));
}

CComBase::~CComBase()
{
	Disconnect();
}

// Called from other thread only. Will stall if call from same thread [5/20/2002]
void CComBase::WaitTillStart()
{
	while (!bStarted)
	{
		Sleep(100);
	}
}

BOOL CComBase::Disconnect()
{
	if (pCom)
	{
		pCom->Close();
		delete pCom;
		pCom = NULL;
		return TRUE;
	}
	return TRUE;
}

BOOL CComBase::Connect(short port)
{
	if (!bSimu) {
		CSingleLock Lock(& CS);
		if (Lock.Lock())
		{
			if (!pCom)
			{
				pCom = new CComm;
				if (!pCom)
				{
					ASSERT(0);
					return FALSE;
				}
			}
			pCom->SetParam(char(port), 9600, 8, 0, 1, 0);
			if (pCom->Open())
			{
				SetDefault();
				return TRUE;
			}
			delete pCom;
			pCom = NULL;
			return FALSE;
		}
	}
	return TRUE;
}

//BOOL CComBase::AddToSMList(CSMHdr & SM)
//{
//	CSingleLock Lock(& CS);
//	//if (Lock.IsLocked()) { /*Log("Lock applied",2);*/ }
//	Lock.Lock();
//	if (Lock.IsLocked())
//	{
//		SMlist.push_back(SM);
//		return TRUE;
//	}
//	return FALSE;
//}

void CComBase::CheckComIn()
{
	CSingleLock Lock(& CS);
	if (Lock.Lock())
	{
		if (pCom)
		{
			char ch[3];
			CString tmp = "";
			if (pCom->ReadChr(ch) > 0)
			{
				if (ch[0] == '\r')
				{
					return;
				}
				if (ch[0] == '\n')
				{
					return;
				}
				if (ch[0] == 7) {
					Log("Bell recieved",2);
					return;
				}
				tmp += ch[0];
//				int cnt = 65000;
				long tick = GetTickCount();
//				while ((GetTickCount()-tick) < 16000)
				while ((GetTickCount()-tick) < 60000)
				{
					if (pCom->ReadChr(ch) > 0)
					{
						if (ch[0] == '\r')
						{
							//QueOut.Add(tmp);
							Decode(tmp);
							return;
						}
						if (ch[0] != '\n')
						{
							tmp += ch[0];
						}
						if (ch[0] == 7) {
							ch[0] = 0;
							tmp += ch[0];
							Log(tmp,3);
							Log("Bell recieved",2);
						}
						tick = GetTickCount();
					}
//					else Sleep(5);
//					else Sleep(20);
				}
				// Char timeout occured [11/2/2002]
				
				Log(tmp,2);
				Log("Inter-charecter timeout",2);
//				ASSERT(0);
				return ;
			}
		}
	}
}

//void CComBase::CheckQueOut()
//{
//	CSingleLock Lock(& CS);
////	if (Lock.IsLocked()) { Log("Lock applied",2); }
//	Lock.Lock();
//	if (Lock.IsLocked())
//	{
//		if (SMlist.size())
//		{
//			vector < CSMHdr >::iterator i;
//			i = SMlist.begin();
//			SendMsg(*i);
//			SMlist.erase(i);
//		}
//	}
//}

BOOL CComBase::Read(CString msg, char * text, short max, int tout)
{
	CSingleLock Lock(& CS);
	if (Lock.Lock())
	{
		if (pCom)
		{
			if (pCom->bConnected)
			{
				if (pCom->RequestFoup(msg, text, max, tout))
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CComBase::ReadTDK(const CString& msg, char * text, short max)
{
//	CSingleLock Lock(& CS);
//	if (Lock.IsLocked()) { Log("Lock applied",2); }
//	Lock.Lock();
//	if (Lock.IsLocked())
	{
		if (pCom)
		{
			if (pCom->bConnected)
			{
				if (pCom->RequestTDK(msg, text, max))
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void CComBase::QueOutAdd(CString msg)
{
	//LYF3
	//QueOut.Add(msg);
}

BOOL CComBase::SendAck(CString msg, int tout)
{
	CSingleLock Lock(& CS);
	if (Lock.Lock())
	{
		if (pCom)
		{
			if (pCom->bConnected)
			{
				if (pCom->SendAck(msg, tout))
				{
					Log(msg,5);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CComBase::SendHCA(CString msg, int tout)
{
	CSingleLock Lock(& CS);
	if (Lock.Lock())
	{
		if (pCom)
		{
			pCom->SendHCA(msg, tout);
			Log(msg,5);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CComBase::SendTDK(CString msg, int Rpy)
{
	CSingleLock Lock(& CS);
	if (Lock.Lock())
	{
		if (pCom)
		{
			pCom->SendTDK(msg, Rpy);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CComBase::Reply(CSMHdr & SM, BOOL ret)
{
	if (SM.HWin)
	{
		if (ret)
		{
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else
		{
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CComBase::Reply(CSMOL & SM, BOOL ret)
{
	if (SM.HWin)
	{
		if (ret)
		{
			::PostMessage(SM.HWin, WM_COMMAND, SM.Ack, NULL);
		}
		else
		{
			::PostMessage(SM.HWin, WM_COMMAND, SM.Nak, NULL);
		}
		return TRUE;
	}
	return FALSE;
}

void CComBase::Log(CString msg, short lvl)
{
	CSMClt ipc;
	if (ipc.Start1("LogSvr",sizeof(CSMHdr))) {
		ipc.LogMsg("ComBase","",msg,lvl);
	}
}

BOOL CComBase::GetRespLFFoup(char *msg, short max, int tout)
{
	if (pCom) {
		pCom->GetRespLFFoup(msg,max, tout);
		return TRUE;
	}
	return FALSE;
}

char * CComBase::GetResp()
{
	if (pCom) {
		return &pCom->resp[0];
	}
	return NULL;
}

BOOL CComBase::NeedVac()
{
	return TRUE;
}

BOOL CComBase::Auto()
{
	return TRUE;
}

BOOL CComBase::Reset(BOOL bCalib)
{
	return TRUE;
}

BOOL CComBase::Calib()
{
	return TRUE;
}

BOOL CComBase::NoVac()
{
	return TRUE;
}

BOOL CComBase::Open()
{
	return TRUE;
}

BOOL CComBase::Close()
{
	return TRUE;
}

BOOL CComBase::CloseNoRelease()
{
	return TRUE;
}

BOOL CComBase::Map()
{
	return TRUE;
}

BOOL CComBase::Unclamp()
{
	return TRUE;
}

BOOL CComBase::Clamp()
{
	return TRUE;
}

BOOL CComBase::Status()
{
	return TRUE;
}

BOOL CComBase::AccessModeChange(LoadPortObj::ACCESSMODE mode)
{
	return TRUE;
}

BOOL CComBase::LEDLoadUnload(PrcsMgrServices::LPBSTATE LpState)
{
	return TRUE;
}

BOOL CComBase::ClearAllLED()
{
	return TRUE;
}
BOOL CComBase::LEDReserved(BOOL on)
{
	return TRUE;
}
BOOL CComBase::LEDManual(BOOL on)
{
	return TRUE;
}
BOOL CComBase::EnableHOAvbl(short port)
{
	return TRUE;
}

BOOL CComBase::LEDAlarm(BOOL on)
{
	return TRUE;
}
//BOOL CComBase::SendTDK(CString msg)
//{
//	CSingleLock Lock(& CS);
//	Lock.Lock();
//	if (Lock.IsLocked())
//	{
//		if (pCom)
//		{
//			pCom->SendTDK(msg);
//			return TRUE;
//		}
//	}
//	return FALSE;
//}
//
//BOOL CComBase::SendHCA(CString msg)
//{
//	CSingleLock Lock(& CS);
//	Lock.Lock();
//	if (Lock.IsLocked())
//	{
//		if (pCom)
//		{
//			pCom->SendHCA(msg);
//			Log(msg,5);
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

BOOL CComBase::Abort()
{
	return TRUE;
}
