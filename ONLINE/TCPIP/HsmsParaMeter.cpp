#include "stdafx.h"

#include "128/GEM/ObjectsContainer.h"
#include "GlobalDeviceContainer.h"
#include "ONLINE/GEM/CediListCtroller.h"
#include "ONLINE/general_utils.h"
#include "ONLINE/ShareMemory/DataBank.h"

#include "HsmsParaMeter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// @@Vijay: temporary arrangement
HWND m_gImageHwnd = NULL;

SOCKET CHsmsParaMeter::m_sClientSk = INVALID_SOCKET;

// DWORD	CHsmsParaMeter::m_T3Timer;
// DWORD	CHsmsParaMeter::m_T5Timer;
// DWORD	CHsmsParaMeter::m_T6Timer;
// DWORD	CHsmsParaMeter::m_T7Timer;
// DWORD	CHsmsParaMeter::m_T8Timer;
//
// short	CHsmsParaMeter::m_DeviceId;
// long	CHsmsParaMeter::m_SystemByte;
// short	CHsmsParaMeter::m_SysByteInc;
//
// int		CHsmsParaMeter::m_SocketPortNo;
// int		CHsmsParaMeter::m_ConnectionMode;
//
// CString	CHsmsParaMeter::m_ActIP;
// int		CHsmsParaMeter::m_ActPort;
// int     CHsmsParaMeter::m_StartMode;
//
// short CHsmsParaMeter::DefOnlineState = CHsmsParaMeter::DEF_LOCAL;
// short CHsmsParaMeter::DefOfflineState = CHsmsParaMeter::DEF_HOST_OFF;

// DWORD	m_T3Timer;
// DWORD	m_T5Timer;
// DWORD	m_T6Timer;
// DWORD	m_T7Timer;
// DWORD	m_T8Timer;

short	m_DeviceId;
long	m_SystemByte;
short	m_SysByteInc;

int		m_SocketPortNo;
int		m_ConnectionMode;

char	m_cActIP[24];
int		m_ActPort;
int     m_StartMode;

short	m_DefOnlineState = CHsmsParaMeter::DEF_LOCAL;
short	m_DefOfflineState = CHsmsParaMeter::DEF_HOST_OFF;

BYTE    CHsmsParaMeter::m_HostMode;
BOOL	CHsmsParaMeter::m_ActiveSocket = FALSE;

CHsmsParaMeter::COMMSM hmCommState = CHsmsParaMeter::DISABLED;
CHsmsParaMeter::HSMSSSSM hmHSMSSS = CHsmsParaMeter::NOTCONNECTED;

// CHsmsParaMeter::COMMSM CHsmsParaMeter::CommState = CHsmsParaMeter::DISABLED;
// CHsmsParaMeter::HSMSSSSM CHsmsParaMeter::HSMSSS = CHsmsParaMeter::NOTCONNECTED;

BOOL CHsmsParaMeter::m_ChgmodePossible = TRUE;

CHsmsParaMeter::CHsmsParaMeter() {
	m_HostMode = 0;
	m_ActiveSocket = FALSE;
}

CHsmsParaMeter::~CHsmsParaMeter() {}

// Receive nMaxLen char from socket or until m_T8Timer timeout [8/9/2012 Administrator]
// Return number of char collected on success [8/9/2012 Administrator]
// 0 on no character in buffer [8/12/2012 Administrator]
// -1 on T8 time out [8/12/2012 Administrator]
// pTime is set to 16 elements only, set last element to -1 on exit [8/13/2012 Administrator]
int CHsmsParaMeter::Pass_RecvString(BYTE* lpszString, int nMaxLen, short* pTime, BOOL bWait) {
	int bRet = 0;
	if (Pass_QCntChk() > 0) // Do the socket have any data ???
	{
		short pTC = 0;
		ULONGLONG T8 = ULONGLONG(CEIDCtlr.m_SecsParam.Param[CSecsParam::spT8TIMER].iValue);
		int bufcnt = 0;
		ULONGLONG dwNowTime, dwStartTime;
		dwStartTime = dwNowTime = GetTickCount64(); // unit is milliseconds [8/9/2012 Administrator]
		while (bWait || ((dwNowTime - dwStartTime) < T8)) {
			bRet = Pass_RevChar(&lpszString[bufcnt], nMaxLen - bufcnt);
			dwNowTime = GetTickCount64();
			if (bRet > 0) {
				if (pTime && (pTC < 15)) {
					short diff = short(dwNowTime - dwStartTime);
					*(pTime++) = diff;
					*(pTime++) = diff / bRet;
					pTC++;
				}
				bufcnt += bRet;
				if (nMaxLen == bufcnt) {
					if (pTime) {
						*pTime = -1;
					}
					return nMaxLen;
				}
				dwStartTime = dwNowTime;
			}
		}
		if (pTime) {
			*pTime = -1;
		}
		return -1;
	}
	return 0;
}

int CHsmsParaMeter::Pass_RevChar(BYTE* pData, int nLen) {
	int bRet = 0;
	int nRsize = nLen;

	//     if (Pass_QcntChk())
	// 	{
	bRet = ::recv(m_sClientSk, (char*)pData, nRsize, 0);

	if (bRet == SOCKET_ERROR) {
		int e = WSAGetLastError();
		return 0;
	}
	else if (bRet == 0) {
		return 0;
	}
	else if (bRet < 0) {
		return 0;
	}
	// 	}
	return bRet;
}

int  CHsmsParaMeter::Pass_SndString(void* pData, int nLen) {
	char* pcData = (char*)pData;
	int	n = nLen;
	int dwSdcnt = 0;

	do {
		int r1 = ::send(m_sClientSk, &pcData[dwSdcnt], n, 0);
		if (r1 == SOCKET_ERROR) {
			int e = WSAGetLastError();
			ASSERT(e != WSAEWOULDBLOCK);
			if (e == WSAECONNRESET) {
				return -1;
			}
			return 0;
		}
		else if (r1 == 0) {
			return 0;
		}
		else if (r1 < 0) {
			ASSERT(0);
			return 0;
		}

		pcData += r1;
		dwSdcnt += r1;
		n -= r1;
	} while (n > 0);

	ASSERT(n == 0);
	return nLen;
}

int CHsmsParaMeter::Pass_QCntChk() {
	int nError = 0;
	u_long rcnt = 0;
	// 	u_long *p = &rcnt;
	// 	u_long **p1 = &p;

	nError = ioctlsocket(m_sClientSk, FIONREAD, &rcnt);
	if (nError == NULL) {
		return rcnt;
		// 		if (rcnt)
		// 		{
		// 			return TRUE;
		// 		}
		// 		else
		// 		{
		// 			return FALSE;
		// 		}
	}
	return -1;	// indicating socket error [8/13/2012 Administrator]
// 	else
// 	{
// 		UINT ErrCode = ::WSAGetLastError();
// 		CString Temp;
// 		Temp.Format("Socket Error %d\n",ErrCode);
// 		TRACE(Temp);
// 	}
//	return FALSE;
}

int CHsmsParaMeter::Pass_GetDataLen(BYTE* nData) {
	int bRet = ((long)nData[0] << 24) | ((long)nData[1] << 16) | ((long)nData[2] << 8) | ((long)nData[3]);

	return bRet;
}

void CHsmsParaMeter::Pass_SetDataLen(BYTE* nData, int nLen) {
	nData[3] = (BYTE)nLen;
	nData[2] = (BYTE)(nLen >> 8);
	nData[1] = (BYTE)(nLen >> 16);
	nData[0] = (BYTE)(nLen >> 24);
}

// void CHsmsParaMeter::ParaMeter_Setting()
// {
// #define MAX_BUFSIZE  200
//
// 	FILE *fp;
// 	char dwBuf[MAX_BUFSIZE];
// 	char dwPar[50];
// 	long dwSystem = 0;
// 	int dwDevice = 0;
//
// 	int cnt = 0;
// 	CString dd("");
//
// 	char drive[_MAX_DRIVE];
// 	char dir[256];
// 	char fname[_MAX_FNAME];
// 	char ext[_MAX_EXT];
// 	char inifile[256];
//
// 	GetModuleFileName(NULL, inifile, 255);
// 	_splitpath(inifile, drive, dir, fname, ext);
// 	_makepath(inifile, drive, dir, "secspara" , "par");
//
// 	if ((fp = fopen(inifile  , "r")) == NULL)
// 	{
// 		Log("SecsPara.Par File Open Fail");
// 		return;
// 	}
// 	else
// 	{
// 		cnt = 0;
// 		while (1)
// 		{
// 			memset(dwBuf , 0x00 , sizeof(dwBuf));
// 			if (fgets(dwBuf , MAX_BUFSIZE , fp))
// 			{
// 				memset(dwPar , 0 , sizeof(dwPar));
// 				if (FALSE == Filter_ParaVal(dwBuf , dwPar , 6, 3))
// 				{
// 					continue;
// 				}
// 				switch (cnt)
// 				{
// 					case 0 :
// 						m_T3Timer = atoi(dwPar) * 1000;
// 						break;
// 					case 1 :
// 						m_T5Timer  = atoi(dwPar) * 1000;
// 						break;
// 					case 2 :
// 						m_T6Timer  = atoi(dwPar) * 1000;
// 						break;
// 					case 3 :
// 						m_T7Timer  = atoi(dwPar) * 1000;
// 						break;
// 					case 4 :
// 						m_T8Timer  = atoi(dwPar) * 1000;
// 						break;
// 					case 5 :
// 						sscanf(dwPar , "%8lx", &dwSystem);
// 						m_SystemByte = dwSystem;
// 						break;
// 					case 6 :
// 						m_SysByteInc = atoi(dwPar);
// 						break;
// 					case 7 :
// 						sscanf(dwPar , "%4x", &dwDevice);
// 						m_DeviceId = (short)dwDevice;
// 						break;
// 					case 8 :
// 						m_ConnectionMode = atoi(dwPar);
// 						break;
// 					case 9 :
// 						m_SocketPortNo = atoi(dwPar);
// 						break;
// 					case 10 :
// 						sprintf(m_cActIP,"%0.15s", dwPar);
// 						break;
// 					case 11 :
// 						m_ActPort = atoi(dwPar);
// 						break;
// 					case 12 :
// 						switch (atoi(dwPar))
// 						{
// 						default:
// 							DefOnlineState = CHsmsParaMeter::DEF_LOCAL;
// 							break;
// 						case CHsmsParaMeter::DEF_LOCAL:
// 						case CHsmsParaMeter::DEF_REMOTE:
// 							DefOnlineState = (CHsmsParaMeter::DEFONLINE) atoi(dwPar);
// 							break;
// 						}
// 						break;
// 						case 13 :
// 							switch (atoi(dwPar))
// 							{
// 							default:
// 								DefOfflineState = CHsmsParaMeter::DEF_HOST_OFF;
// 								break;
// 							case CHsmsParaMeter::DEF_HOST_OFF:
// 							case CHsmsParaMeter::DEF_EQP_OFF:
// 								DefOfflineState = (CHsmsParaMeter::DEFOFFLINE) atoi(dwPar);
// 								break;
// 							}
// 							break;
// 				}
// 				cnt++;
// 			}
// 			else
// 				break;
// 		}
// 		fclose(fp);
// 	}
// 	return;
// 	// ret ;
// }

void CHsmsParaMeter::Set_ActIP(CString pIP) {
	strcpy(m_cActIP, pIP.GetBuffer(0));
}

void CHsmsParaMeter::Set_ActPort(int pPort) {
	m_ActPort = pPort;
}

BOOL CHsmsParaMeter::Filter_ParaVal(char* input, char* output, UINT NumParams, UINT ParamLoc) {
	int loop = 0, cnt = 0, bStart = 0, bEnd = 0;
	vector<string> Tokens;
	TOKEN_RES Ret = Tokenize(input, 150, Tokens, ',', ';');
	switch (Ret) {
	case COMMENT:
		return FALSE;
		break;
	case LINE_ERROR:
		return FALSE;
		break;
	case LINE_TOO_LONG:
		return FALSE;
		break;
	case OK:
		if (NumParams == Tokens.size()) {
			strcpy(output, Tokens[ParamLoc - 1].c_str());
			return TRUE;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

//  [9/14/2007 LYF]
// Translate to GEM specific constant
// short CHsmsParaMeter::GGetLineMode()
// {
// 	switch (pGDev->pGEMObjs->GetLineMode() /*m_pDataBank->m_EStatus.LineMode*/)
// 	{
// 		case EqpStatus::REMOTE :
// 			return 1;
// 			break;
// 		case EqpStatus::LOCAL :
// 			return 2;
// 			break;
// 		case EqpStatus::OFFLINEEQP:
// 			return 3;
// 			break;
// 		case EqpStatus::OFFLINEHOST :
// 			return 4;
// 			break;
// 		case EqpStatus::ATTEMPTONLINE:
// 			return 5;
// 			break;
// 	}
// 	return 0;
// }

// CString CHsmsParaMeter::GetLineModeStr()
// {
// 	switch (pGDev->pGEMObjs->GetLineMode() /*m_pDataBank->m_EStatus.LineMode*/)
// 	{
// 		case EqpStatus::REMOTE :
// 			return "Remote";
// 			break;
// 		case EqpStatus::LOCAL :
// 			return "Local";
// 			break;
// 		case EqpStatus::OFFLINEEQP:
// 			return "Offline(E)";
// 			break;
// 		case EqpStatus::OFFLINEHOST :
// 			return "Offline(H)";
// 			break;
// 	}
// 	return "?";
// }

//EqpStatus::CONTROLSTATE CHsmsParaMeter::GGetDefOflMode()
//{
//	switch (DefOfflineState)
//	{
//		case DEF_HOST_OFF:
//			return EqpStatus::OFFLINEHOST;
//			break;
//		case DEF_EQP_OFF:
//			return EqpStatus::OFFLINEEQP;
//			break;
//	}
//	ASSERT(0);
//	return EqpStatus::NOSECS; // Place holder will not reach
//}
//EqpStatus::CONTROLSTATE CHsmsParaMeter::GGetDefOnlMode()
//{
//	switch (DefOnlineState)
//	{
//		case DEF_LOCAL:
//			return EqpStatus::LOCAL;
//			break;
//		case DEF_REMOTE:
//			return EqpStatus::REMOTE;
//			break;
//	}
//	ASSERT(0);
//	return EqpStatus::NOSECS; // Place holder will not reach
//}

BOOL CHsmsParaMeter::IsOnline() {
	EqpStatus::CONTROLSTATE LineMode = pGDev->pGEMObjs->GetLineMode();
	if ((/*m_pDataBank->m_EStatus.*/LineMode == EqpStatus::REMOTE) ||
		(/*m_pDataBank->m_EStatus.*/LineMode == EqpStatus::LOCAL)) {
		// YFL:  [4/8/2005]
		if (hmCommState == COMMUNICATING) {
			return TRUE;
		}
		return FALSE;// YFL:  [4/8/2005]
//		return TRUE; // YFL:  [4/8/2005]
	}
	return FALSE;
}

void CHsmsParaMeter::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CHsmsParaMeter");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

// flush input bufer [8/11/2012 Administrator]
void CHsmsParaMeter::Pass_DescardRevString(int MaxLen) {
	BYTE ch[256];
	int bufcnt = 0;
	ULONGLONG T8 = ULONGLONG(CEIDCtlr.m_SecsParam.Param[CSecsParam::spT8TIMER].iValue);
	ULONGLONG dwStartTime = GetTickCount64();	// unit is milli seconds [8/9/2012 Administrator]
	while (GetTickCount64() - dwStartTime < T8) {
		short dch = MaxLen - bufcnt;
		if (dch > 255) {
			dch = 255;
		}
		int n = Pass_RevChar(&ch[0], dch);
		bufcnt += n;
		if (MaxLen == bufcnt) {
			return;
		}
	}
}