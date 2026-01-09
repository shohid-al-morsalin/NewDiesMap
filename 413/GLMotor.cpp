// GLMotor.cpp: implementation of the CGLMotor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "413App.h"
#include "GLMotor.h"
#include "..\SRC\Tokenizer.h"
#include "GlobalDeviceContainer.h"
#include "..\IPC\TelnetClientSocket.h"
#include "..\IPC\Lock.h"
#include "FFUThread.h"

#define DELIMITER	':'

char AxN[4] = { 'A', 'B', 'C', 'D' };
char axN[4] = { 'a', 'b', 'c', 'd' };
short iN[4] = { 1, 2, 4, 8 };

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGLMotor* pGLM = NULL;	// Local use only [6/3/2012 Administrator]

CGLMotor::CGLMotor() {
	pGLM = this;
	pLockExec = new CDMLock("GLMotorExecLock");
	m_lpfnMessageProc = NULL;

	idx = 0;
	ID = GLMOTOR;
	IDStr = "GL Motor";
	pSock1 = pSock2 = NULL;
	hd = tl = 0;
}

CGLMotor::~CGLMotor() {
	if (pLockExec) {
		delete pLockExec;
		pLockExec = NULL;
	}

	ClosePort();
	pGLM = NULL;
}

// [5/27/2020]
BOOL CGLMotor::IsDoorInterlockTriggered() {
	char res[2];

	Sleep(150);
	Flush();

	cmd = "XQ #CIL,7\r";

	rsp = _SendR(cmd, 3000);
	if (rsp.GetLength() > 0) {
		res[0] = rsp.GetAt(0);
		return atoi(res);
	}
	else
		return FALSE;
}

// Send the command through the communication channel [5/29/2012 Yuen]
BOOL CGLMotor::_Send(const char* psz, int len) {
	if (pSock1) {
		int l = pSock1->Send(psz, len);
		if (l == SOCKET_ERROR) {
			DWORD Err = GetLastError();
			switch (Err) {
			case WSAEWOULDBLOCK:
				return TRUE;
				break;
			default:
				p413App->Log("Socket error (Send)", 2);
				return TRUE;
				break;
			}
		}
		else return TRUE;
	}
	return FALSE;
}

BOOL CGLMotor::_Send2(const char* psz) {
	if (pSock2->Send(psz, strlen(psz)) != -1) {
		return TRUE;
	}
	return FALSE;
}

// Read a string from communication channel [5/29/2012 Yuen]
// UINT CGLMotor::Receive(UINT nSize, char *chBuffer)
// {
// 	// not use [6/3/2012 Administrator]
// 	return 0;
// }

// Stop the axis from moving [5/29/2012 Yuen]
BOOL CGLMotor::Stop(int nAxis) {
	AXISID a = GetAxisNo(nAxis);
	if (a == NOAXIS) {
		return FALSE;
	}
	cmd.Format("~a=%d;ST~a", a);
	BOOL ret = _SendN11(cmd);
	return ret;
}

BOOL CGLMotor::devGetXYPos(float* pfX, float* pfY, DWORD tout) {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < tout) {
		cmd = "XQ #GPXY,5";
		rsp = _SendR(cmd, 2000);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[4];
			for (int j = 0; j < 4; j++) {
				tok.Next(cs[j]);
			}
			// First token must be #GPXY and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 5) || (cs[0] != "#GPXY")) {
				MsgStr.Format("devGetXYPos: receive '%s' instead of '#GPXY'", cs[0]);
				p413App->Log(MsgStr, 2);
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[3].GetLength() != 1) || (cs[3].GetAt(0) != '@')) {
				MsgStr.Format("devGetXYPos: receive '%s' instead of '@'", cs[3]);
				p413App->Log(MsgStr, 2);
				continue;
			}
			// Message format is now verified [10/1/2012 Yuen]
			*pfX = atof(cs[1]);
			*pfY = atof(cs[2]);
			return TRUE;
		}
		SleepEx(100);
	}
	p413App->Log("devGetXYPos fail", 2);
	return FALSE;
}

// Servo on the motor [6/5/2012 user]
BOOL CGLMotor::SetCurrent(int nAxis, float fCurrent) {
	switch (GetAxisNo(nAxis)) {
	case XAXIS:
		if (fCurrent != 0.0f)
			cmd = "SHA";
		else
			cmd = "MOA";
		break;
	case YAXIS:
		if (fCurrent != 0.0f)
			cmd = "SHB";
		else
			cmd = "MOB";
		break;
	case ECHOTOPZAXIS:
		if (fCurrent != 0.0f)
			cmd = "SHC";
		else
			cmd = "MOC";
		break;
	case ROUGHZAXIS:
		if (fCurrent != 0.0f)
			cmd = "SHD";
		else
			cmd = "MOD";
		break;
	default:
		return FALSE;
		break;
	}
	return _SendN11(cmd);
}

// Make sure no axis is moving before this function is called [6/15/2013 Yuen]
BOOL CGLMotor::devGetCurPos(int nAxis, float* pfPos) {
	AXISID a = GetAxisNo(nAxis);
	if (a == NOAXIS) {
		return FALSE;	// Invalid axis. Force software to hang [10/1/2012 Yuen]
	}
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < 10000) {
		cmd.Format("~a=%d;XQ #GPA,5", a);
		rsp = _SendR(cmd, 2000);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[3];
			for (int j = 0; j < 3; j++) {
				tok.Next(cs[j]);
			}
			// First token must be #GPA and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 4) || (cs[0] != "#GPA")) {
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[2].GetLength() != 1) || (cs[2] != "@")) {
				continue;
			}
			// Message format is now verified [10/1/2012 Yuen]
			*pfPos = atof(cs[1]);
			moAxis[nAxis - 1].fPos = *pfPos;
			return TRUE;
		}
		SleepEx(100);
	}
	p413App->Log("[ME] GetCurrentPos: Unexpected response from DMC", 2);
	return FALSE;
}

BOOL CGLMotor::devMoveA(int nAxis, float lPos) {
	AXISID a = GetAxisNo(nAxis);
	if (a == NOAXIS) {
		return FALSE;
	}
	cmd.Format("V%c=%.0f;XQ #MA%c,%d", axN[a], lPos, AxN[a], a + 1);
	if (_ExecPrgm(cmd, iN[a])) {
		BOOL ret = CSiBase::devMoveA(nAxis, lPos);
		return ret;
	}
	return FALSE;
}

BOOL CGLMotor::devMoveR(int nAxis, float lDist) {
	short a = GetAxisNo(nAxis);
	if (a == -1) {
		return FALSE;
	}
	cmd.Format("V%c=%.0f;XQ #MR%c,%d", axN[a], lDist, AxN[a], a + 1);
	if (_ExecPrgm(cmd, iN[a])) {
		BOOL ret = CSiBase::devMoveR(nAxis, lDist);
		return ret;
	}
	return FALSE;
}

BOOL CGLMotor::devSetAbsPos(int nAxis, float lPos) {
	AXISID a = GetAxisNo(nAxis);
	if (a != NOAXIS) {
		long pos = lPos/* / 6.103143119f*/;
		cmd.Format("~a=%d;DP~a=%ld", a, pos);
		if (!_SendN11(cmd)) return FALSE;
		cmd.Format("~a=%d;DE~a=%.0f", a, lPos);
		if (_SendN11(cmd)) {
			return CSiBase::devSetAbsPos(nAxis, lPos);
		}
	}
	return FALSE;
}

BOOL CGLMotor::StopAll() {
	cmd = "ST;MO";
	return _SendN11(cmd);
}

BOOL CGLMotor::SetOutput(int nAxis, int nChan, BOOL bOnOff) {
	//	ASSERT(0);	// Need to find alternative for this [5/29/2012 Yuen]
	return TRUE;
}

BOOL CGLMotor::GetInput(int nAxis, int nChan) {
	//	ASSERT(0);	// Need to find alternative for this [5/29/2012 Yuen]
	return TRUE;
}

float CGLMotor::GetAnalogInput(int nAxis) {
	//	ASSERT(0);	// Need to find alternative for this [5/29/2012 Yuen]
	return 0;
}

BOOL CGLMotor::OpenPort(short PortNo) {
	BOOL bOK = FALSE;
	// First channel [6/4/2012 Administrator]
	pSock1 = new CTelnetClientSocket();
	if (pSock1 != NULL) {
		pSock1->bConnected = FALSE;
		bOK = pSock1->Create();
		if (bOK == TRUE) {
			bOK = FALSE;
			pSock1->SetCallBack(OnMessageProc1);
			pSock1->AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT | FD_OOB);
			if (pSock1->Connect(p413App->IPAddress, 23) == 0) {
				if (GetLastError() == WSAEWOULDBLOCK) {
					// Connection OK, initialize it [6/3/2012 Administrator]
					bOK = TRUE;
					CString str = "Socket 23 connected";
					p413App->Log(str);
				}
			}
		}
		else {
			ASSERT(FALSE);  //Did you remember to call AfxSocketInit()?
			delete pSock1;
			pSock1 = NULL;
			return FALSE;
		}
	}
	bOK = FALSE;
	// Second channel [6/4/2012 Administrator]
	pSock2 = new CTelnetClientSocket();
	if (pSock2 != NULL) {
		pSock2->bConnected = FALSE;
		bOK = pSock2->Create();
		if (bOK == TRUE) {
			bOK = FALSE;
			pSock2->SetCallBack(OnMessageProc2);
			pSock2->AsyncSelect(/*FD_READ |*/ FD_WRITE | FD_CLOSE | FD_CONNECT | FD_OOB);	// Do not turn on FD_READ, will handle it independantly [7/18/2013 Yuen]
			if (pSock2->Connect(p413App->IPAddress, 4132) == 0) {
				if (GetLastError() == WSAEWOULDBLOCK) {
					// Connection OK, initialize it [6/3/2012 Administrator]
					bOK = TRUE;
					CString str = "Socket 4132 connected";
					p413App->Log(str);
				}
			}
		}
		else {
			ASSERT(FALSE);  //Did you remember to call AfxSocketInit()?
			delete pSock1;
			pSock1 = NULL;
			delete pSock2;
			pSock2 = NULL;
			return FALSE;
		}
	}
	return bOK;
}

BOOL CGLMotor::ClosePort() {
	if (pSock1) {
		pSock1->ShutDown(2);
		pSock1->Close();
		delete pSock1;
		pSock1 = NULL;
	}
	if (pSock2) {
		pSock2->ShutDown(2);
		pSock2->Close();
		delete pSock2;
		pSock2 = NULL;
	}
	return TRUE;
}

BOOL CGLMotor::_ExecPrgm(CString& msg, short nAxis) {
	TRACE2("EXEC PRGMS %d  %s\n", GetTickCount64(), msg);
	return _SendN11(msg);
}

// Send and get response [7/18/2013 Yuen]
CString CGLMotor::_SendR(CString cmd, long tout) {
	CString rsp = "";
	pLockExec->Lock();
	Flush();
	cmd += "\r";
	TRACE2("S %d  %s\n", GetTickCount64(), cmd);
	if (_Send(cmd.GetBuffer(0), cmd.GetLength()) > 0) {
		SleepEx(50);	// Wait network respond time [7/18/2013 Yuen]
		rsp = _GetResp(tout);
		TRACE2("E %d  %s\n", GetTickCount64(), rsp);
		short len = rsp.GetLength();
		if (rsp.GetLength() > 0) {
			pLockExec->Unlock();
			return rsp;
		}
	}
	pLockExec->Unlock();
	return rsp;
}

BOOL CGLMotor::setEncoderPos(int nAxis, int pos) {
	switch (GetAxisNo(nAxis)) {
	case XAXIS:
		cmd.Format("DEA=%d", pos);
		break;
	case YAXIS:
		cmd.Format("DEB=%d", pos);
		break;
	default:
		return FALSE;
		break;
	}
	return _SendN11(cmd);
}

// Send width no response [7/18/2013 Yuen]
BOOL CGLMotor::_SendN11(CString cmd) {
	pLockExec->Lock();
	cmd += "\r";
	if (_Send(cmd.GetBuffer(0), cmd.GetLength()) > 0) {
		pLockExec->Unlock();
		return TRUE;
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor::_SendN2(CString cmd) {
	pLockExec->Lock();
	cmd += "\r";
	if (_Send2(cmd.GetBuffer(0))) {
		SendReply(cmd);
		pLockExec->Unlock();
		return TRUE;
	}
	pLockExec->Unlock();
	return FALSE;
}

void CGLMotor::SetCallBack(lpfnMessageProc pFunc) {
	m_lpfnMessageProc = pFunc;
}

// Callback when a valid line is received [6/3/2012 Administrator]
void CGLMotor::OnMessageProc1(int wParam, LPVOID lParam) {
	switch (wParam) {
	case CTelnetClientSocket::NORMALMSG:
		CrackMessage((char*)lParam);
		break;
	case CTelnetClientSocket::SOCCONNECT:
		if (pGLM->pSock1) {
			pGLM->Initialize1();
			pGLM->pSock1->bConnected = TRUE;
		}
		break;
	case CTelnetClientSocket::SOCCLOSE:
	{
		int Err = (int)lParam;
		switch (Err) {
		case 0:  // OK [7/31/2013 Yuen]
			break;
		case WSAENETDOWN:	// Network subsystem failed [7/31/2013 Yuen]
			p413App->Log("Network down");
			break;
		case WSAECONNRESET:	// Connection reset by remote site [7/31/2013 Yuen]
			p413App->Log("Connection reset");
			break;
		case WSAECONNABORTED: // Connection aborted due to timeout or other error [7/31/2013 Yuen]
			p413App->Log("Connection abort");
			break;
		}
	}
	if (pGLM) {
		pGLM->pLockExec->Lock();

		// replaced [6/17/2020]
		pGLM->check_handle();
		// ====================

		//pGLM->CloseSock1();
		//pGLM->SleepEx(4000);
		//pGLM->ReconnetSoc1();
		pGLM->pLockExec->Unlock();
	}

	break;
	}
}

// Callback when a valid line is received [6/3/2012 Administrator]
void CGLMotor::OnMessageProc2(int wParam, LPVOID lParam) {
	CString str;
	switch (wParam) {
	case CTelnetClientSocket::NORMALMSG:
	{
		pGLM->pLockExec->Lock();
		pGLM->CirReturnText[pGLM->tl] = (char*)lParam;
		pGLM->tl++;
		if (pGLM->tl >= CIRNORMALTEXTLEN1) {
			pGLM->tl = 0;
		}

		if (pGLM->tl == pGLM->hd) {
			// collision detected [7/14/2013 Yuen]
		}
		pGLM->pLockExec->Unlock();
	}
	break;
	case CTelnetClientSocket::SOCCONNECT:
		if (pGLM->pSock2) {
			pGLM->Initialize2();
			pGLM->pSock2->bConnected = TRUE;
		}
		break;
	case CTelnetClientSocket::SOCCLOSE:
	{
		int Err = (int)lParam;
		DWORD Err1 = GetLastError();
		switch (Err) {
		case 0:  // OK [7/31/2013 Yuen]
			break;
		case WSAENETDOWN:	// Network subsystem failed [7/31/2013 Yuen]
			p413App->Log("Network don");
			break;
		case WSAECONNRESET:	// Connection reset by remote site [7/31/2013 Yuen]
			p413App->Log("Connection reset");
			break;
		case WSAECONNABORTED: // Connection aborted due to timeout or other error [7/31/2013 Yuen]
			p413App->Log("Connection abort");
			break;
		}
	}
	if (pGLM) {
		pGLM->pLockExec->Lock();

		// replaced [6/17/2020]
		pGLM->check_handle();
		// ====================

		//pGLM->CloseSock2();
		//pGLM->SleepEx(4000);
		//pGLM->ReconnectSoc2();
		pGLM->pLockExec->Unlock();
	}
	}
}

void CGLMotor::CrackMessage(char* msg) {
	p413App->Log(msg);
	// Let the message sink [6/14/2013 Yuen]
	// No need to delete msg allocation [6/14/2013 Yuen]
}

BOOL CGLMotor::Initialize1() {
	C413Global* g = &p413App->Global;
	cmd.Format("AC %d,%d,%d,%d", moAxis[g->X_Axis - 1].Acc, moAxis[g->Y_Axis - 1].Acc, moAxis[g->EchoTop_Axis - 1].Acc, moAxis[g->RoughZ_Axis - 1].Acc);
	if (!_SendN11(cmd)) {
		return FALSE;
	}

	cmd.Format("DC %d,%d,%d,%d", moAxis[g->X_Axis - 1].Acc, moAxis[g->Y_Axis - 1].Acc, moAxis[g->EchoTop_Axis - 1].Acc, moAxis[g->RoughZ_Axis - 1].Acc);
	if (!_SendN11(cmd)) {
		return FALSE;
	}

	cmd.Format("SP %d,%d,%d,%d", moAxis[g->X_Axis - 1].Spd, moAxis[g->Y_Axis - 1].Spd, moAxis[g->EchoTop_Axis - 1].Spd, moAxis[g->RoughZ_Axis - 1].Spd);
	if (!_SendN11(cmd)) {
		return FALSE;
	}
	return TRUE;
}

BOOL CGLMotor::Initialize2() {
	cmd = "CW2;CFI";
	if (_SendN2(cmd)) {
		return TRUE;
	}
	return FALSE;
}

void CGLMotor::SendReply(CString& msg) {
	if (m_lpfnMessageProc) {
		CirNormalText[idx] = msg;
		m_lpfnMessageProc(CTelnetClientSocket::NORMALMSG, (LPVOID)CirNormalText[idx].GetBuffer(0));
		idx++;
		if (idx > CIRNORMALTEXTLEN1) {
			idx = 0;
		}
	}
}

CString CGLMotor::_GetResp(DWORD tout) {
	char pbuf[16];
	int len, cnt = 0;
	BOOL bCH = FALSE;
	CString ret;

	ULONGLONG tm = GetTickCount64();
	while (((GetTickCount64() - tm) < tout)) {
		len = pSock2->Receive(pbuf, 1);
		if (len == 0) {
			// Socket is closed [8/5/2013 Yuen]
			p413App->Log("Socket closed", 2);
			return "";
		}
		else if (len == SOCKET_ERROR) {
			DWORD Err = GetLastError();
			switch (Err) {
			case WSAEWOULDBLOCK:
				Sleep(10);
				continue;
				break;
			case WSAECONNRESET:
			case WSAENOTCONN:
				p413App->Log("Socket reset or down", 2);
				return "";
				break;
			default:
				p413App->Log("Socket error (Receive)", 2);
				return "";
				break;
			}
		}
		else if (len == 1) {
			cnt++;
			if (pbuf[0] == '\r') {
				return ret;
			}
			else if (pbuf[0] == '\n') {
				continue;
			}
			else if (pbuf[0] == 0) {
				continue;
			}
			else if (pbuf[0] == '?') {
				ret += pbuf[0];
				return ret;
			}
			else if (pbuf[0] == ' ') {
				// skip starting space [7/18/2013 Yuen]
				if (!bCH) { continue; }
				else { ret += pbuf[0]; }
			}
			else {
				ret += pbuf[0];
				bCH = TRUE;
			}
		}
		else {
			SleepEx(50);
		}
	}
	p413App->Log("GetResp timeout", 2);
	return "";
}

void CGLMotor::Flush() {
	char pbuf[65];
	if (pSock2) {
		short len = pSock2->Receive(pbuf, 64);
		while (1) {
			len = pSock2->Receive(pbuf, 64);
			if (len == 0) {
				// Socket is closed [8/5/2013 Yuen]
				p413App->Log("Socket closed", 2);
				return;
			}
			else if (len == SOCKET_ERROR) {
				DWORD Err = GetLastError();
				switch (Err) {
				case WSAEWOULDBLOCK:
					return;
					break;
				case WSAECONNRESET:
				case WSAENOTCONN:
					p413App->Log("Socket reset or down", 2);
					return;
					break;
				default:
					p413App->Log("Socket error (Receive)", 2);
					return;
					break;
				}
				return;
			}
			else {
				CString str;
				pbuf[len] = 0;
				str.Format("Flush: %d %s\n", len, pbuf);
				p413App->Log(str, 2);
				SleepEx(50);
			}
		}
	}
}

BOOL CGLMotor::devGetPosAll(float* pfX, float* pfY, float* pfZ1, float* pfZ2) {
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < 10000) {
		cmd = "XQ #GP,5";
		rsp = _SendR(cmd, 2000);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[6];
			for (int j = 0; j < 6; j++) {
				tok.Next(cs[j]);
			}
			// First token must be #GP and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 3) || (cs[0] != "#GP")) {
				MsgStr.Format("devGetPosAll: Receive %s instead of '#GP'", cs[0]);
				p413App->Log(MsgStr, 2);
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[5].GetLength() != 1) || (cs[5] != "@")) {
				MsgStr.Format("devGetPosAll: Receive %s instead of '@'", cs[5]);
				p413App->Log(MsgStr, 2);
				continue;
			}
			// Message format is now verified [10/1/2012 Yuen]
			C413Global* g = &p413App->Global;
			*pfX = atof(cs[1]);
			moAxis[g->X_Axis - 1].fPos = *pfX;
			*pfY = atof(cs[2]);
			moAxis[g->Y_Axis - 1].fPos = *pfY;
			*pfZ1 = atof(cs[3]);
			moAxis[g->EchoTop_Axis - 1].fPos = *pfZ1;
			*pfZ2 = atof(cs[4]);
			moAxis[g->RoughZ_Axis - 1].fPos = *pfZ2;
			return TRUE;
		}
		SleepEx(100);
	}
	p413App->Log("devGetPosAll: timeout", 2);
	return FALSE;
}

// Do not lock [7/16/2013 Yuen]
BOOL CGLMotor::_WaitAllStop(int tout, BOOL& bPos) {
	ULONGLONG tick = GetTickCount64();
	BOOL bx = TRUE, by = TRUE, bz1 = TRUE, bz2 = TRUE;
	while (int(GetTickCount64() - tick) < tout) {
		cmd = "XQ #IMA,5";
		rsp = _SendR(cmd, 1000);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[11];
			cs[10] = "";
			for (int j = 0; j < 11; j++) {
				if (!tok.Next(cs[j])) {
					break;
				}
			}
			// First token must be #IMA and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 4) || (cs[0] != "#IMA")) {
				MsgStr.Format("_WaitAllStop: receive '%s' instead of '#IMA'", cs[0]);
				p413App->Log(MsgStr, 2);
				SleepEx(100);
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[5].GetLength() != 1) || (cs[5] != "@")) {
				MsgStr.Format("_WaitAllStop: receive '%s' instead of '@'", cs[5]);
				p413App->Log(MsgStr, 2);
				SleepEx(100);
				continue;
			}

			// Message format is now verified [10/1/2012 Yuen]
			if (CS2Value(cs[1])) {
				SleepEx(50);
				continue;
			} bx = FALSE;
			if (CS2Value(cs[2])) {
				SleepEx(50);
				continue;
			}by = FALSE;
			if (CS2Value(cs[3])) {
				SleepEx(50);
				continue;
			}bz1 = FALSE;
			if (CS2Value(cs[4])) {
				SleepEx(50);
				continue;
			}bz2 = FALSE;
			bPos = FALSE;
			if ((cs[10].GetLength() == 1) && (cs[10] == "*")) {
				C413Global* g = &p413App->Global;
				moAxis[g->X_Axis - 1].fPos = atoi(cs[6]);
				moAxis[g->Y_Axis - 1].fPos = atoi(cs[7]);
				moAxis[g->EchoTop_Axis - 1].fPos = atoi(cs[8]);
				moAxis[g->RoughZ_Axis - 1].fPos = atoi(cs[9]);
				//bPos = TRUE;	// Temporary disable this feature [7/18/2013 Yuen]
			}
			return TRUE;
		}
		else {
			SleepEx(500);
		}
	}
	CString str;
	if (bx) str.Format("X is still 1");
	else if (by) str.Format("Y is still 1");
	else if (bz1) str.Format("EchoTopZ is still 1");
	else if (bz2) str.Format("RoughZ is still 1");
	p413App->Log(str, 2);
	ResetAllMotionMemory(); // 1/11/22
	p413App->Log("_WaitAllStop Timeout!", 2);
	return FALSE;
}

BOOL CGLMotor::SetAcceleration(int nAxis, float fSpeed) {
	switch (GetAxisNo(nAxis)) {
	case XAXIS:
		cmd.Format("ACA=%f;DCA=%f;", fSpeed, fSpeed);
		break;
	case YAXIS:
		cmd.Format("ACB=%f;DCB=%f;", fSpeed, fSpeed);
		break;
	case ECHOTOPZAXIS:
		cmd.Format("ACC=%f;DCC=%f;", fSpeed, fSpeed);
		break;
	case ROUGHZAXIS:
		cmd.Format("ACD=%f;DCD=%f;", fSpeed, fSpeed);
		break;
	default:
		//ASSERT(0);
		return FALSE;
		break;
	}
	return _SendN11(cmd);
}

BOOL CGLMotor::SetSpeed(int nAxis, float fSpeed) {
	switch (GetAxisNo(nAxis)) {
	case XAXIS:
		cmd.Format("SPA=%f", fSpeed);
		break;
	case YAXIS:
		cmd.Format("SPB=%f", fSpeed);
		break;
	case ECHOTOPZAXIS:
		cmd.Format("SPC=%f", fSpeed);
		break;
	case ROUGHZAXIS:
		cmd.Format("SPD=%f", fSpeed);
		break;
	default:
		//ASSERT(0);
		return FALSE;
		break;
	}
	return _SendN11(cmd);
}

// Relative move [6/22/2012 FSM413]
BOOL CGLMotor::devMoveRXY(float iX, float iY) {
	if ((!iX) && (!iY)) {
		// all zero, no move need [6/25/2012 FSM413]
		return TRUE;
	}

	cmd.Format("Va=%.0f;Vb=%.0f;XQ #MRXY,1", iX, iY);
	if (_ExecPrgm(cmd, 3)) {
		return CSiBase::devMoveRXY(iX, iY);
	}
	MsgStr.Format("MoveAXY: devMoveRXY(%.3f,%.3f) fail", iX, iY);
	p413App->Log(MsgStr, 2);
	return FALSE;
}

// This function must return in split seconds or the rest of the motion system will stall [6/14/2013 Yuen]
// Return -1 to indicate no reading and previous reading should be used [6/14/2013 Yuen]
float CGLMotor::fGetFFUPressNow() {
	static float curValue = 0;

	cmd.Format("XQ #AN3,7");
	rsp = _SendR(cmd, 1000);
	if (rsp.GetLength() == 0) {
		return curValue;
	}
	CTokenizer tok(rsp, " ");
	CString cs[3];
	for (int j = 0; j < 3; j++) {
		tok.Next(cs[j]);
	}
	// First token must be #AN and one character only [10/1/2012 Yuen]
	if ((cs[0].GetLength() != 3) || (cs[0] != "#AN")) {
		return curValue;
	}
	// Last token must be @ and one character only [10/1/2012 Yuen]
	if ((cs[2].GetLength() != 1) || (cs[2] != "@")) {
		return curValue;
	}
	// Message format is now verified [10/1/2012 Yuen]
	FFUPressNow = atof(cs[1]);
	return FFUPressNow;
}

BOOL CGLMotor::ResetXY() {
	BOOL bPos = FALSE;
	SetFFU(TRUE); // for testing
	cmd = "XQ #RSX,1";
	if (_ExecPrgm(cmd, 1)) {
		cmd = "XQ #RSY,2";
		if (_ExecPrgm(cmd, 2)) {
			SleepEx(50);
			if (_WaitForXY(120000, bPos)) {
				if (bPos) {
					return TRUE;
				}
				return CSiBase::ResetXY();
			}
		}
	}
	_WaitForXY(120000, bPos);
	SetFFU(FALSE); // for testing
	return FALSE;
}

BOOL CGLMotor::ResetEchoTopZ() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSZ1,3";
	if (_ExecPrgm(cmd, 4)) {
		SleepEx(50);
		if (_WaitStop(GetMotorID(ECHOTOPZAXIS), 90000, bPos)) {
			pLockExec->Unlock();
			if (bPos) return TRUE;
			return CSiBase::ResetEchoTopZ();
		}
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor::ResetRoughZ() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSZ2,3";
	if (_ExecPrgm(cmd, 8)) {
		SleepEx(50);
		if (_WaitStop(GetMotorID(ROUGHZAXIS), 90000, bPos)) {
			pLockExec->Unlock();
			if (bPos) return TRUE;
			return CSiBase::ResetRoughZ();
		}
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor::ResetX() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSX,1";
	if (_ExecPrgm(cmd, 1)) {
		SleepEx(50);
		if (_WaitStop(GetMotorID(XAXIS), 90000, bPos)) {
			pLockExec->Unlock();
			if (bPos) return TRUE;
			return CSiBase::ResetX();
		}
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor::ResetY() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSY,2";
	if (_ExecPrgm(cmd, 2)) {
		SleepEx(50);
		if (_WaitStop(GetMotorID(YAXIS), 90000, bPos)) {
			pLockExec->Unlock();
			if (bPos) return TRUE;
			return CSiBase::ResetY();
		}
	}
	pLockExec->Unlock();
	return FALSE;
}

BOOL CGLMotor::ResetAll() {
	BOOL bPos = FALSE;
	pLockExec->Lock();
	cmd = "XQ #RSX,1";
	if (_ExecPrgm(cmd, 1)) {
		cmd = "XQ #RSY,2";
		if (_ExecPrgm(cmd, 2)) {
			cmd = "XQ #RSZ1,3";
			if (_ExecPrgm(cmd, 4)) {
				cmd = "XQ #RSZ2,4";
				if (_ExecPrgm(cmd, 8)) {
					SleepEx(50);
					if (_WaitAllStop(180000, bPos)) {
						pLockExec->Unlock();
						if (bPos) return TRUE;
						return CSiBase::ResetAll();
					}
				}
			}
		}
	}
	_WaitAllStop(120000, bPos);
	pLockExec->Unlock();
	p413App->Log("Reset timeout", 2);
	return FALSE;
}

BOOL CGLMotor::SetChuckVacuum(BOOL bOnOff, int nWaferDiameter) {
	p413App->Global.bChuckVac = bOnOff;
	if (bOnOff) {
		if (nWaferDiameter == 200) {
			cmd = "SB 3";
		}
		else {
			cmd = "SB 3; SB 2";
		}
		return _SendN11(cmd);
	}
	else {
		cmd = "CB 3;CB 2";
		return _SendN11(cmd);
	}
	return FALSE;
}

BOOL CGLMotor::SetMicroscopeLight(BOOL bOnOff) {
	if (bOnOff) {
		cmd = "SB 7"; // "SB 5"; // 16102024
	}
	else {
		cmd = "CB 7";  // "CB 5"; // 16102024
	}
	if (_SendN11(cmd)) {
		p413App->Global.bEchoLight = bOnOff;	// For simulation only [6/16/2013 Yuen]
		return TRUE;
	}
	return FALSE;
}

// 08142024
BOOL CGLMotor::SetFFU(BOOL bOnOff)
{
	p413App->Global.bSetFFU = bOnOff;
	if (bOnOff) {
		cmd = "SB 8";
	}
	else {
		cmd = "CB 8";
	}
	return _SendN11(cmd);
}

BOOL CGLMotor::SetSpectroSourceLight(BOOL bOnOff) {
	//p413App->Global.bSpecSource = bOnOff;
	//if (bOnOff) {
	//	cmd = "SB 8";
	//}
	//else {
	//	cmd = "CB 8";
	//}
	//return _SendN11(cmd);
	return TRUE;
}

BOOL CGLMotor::SetSpectroIllumLight(BOOL bOnOff) {
	return TRUE; //16102024

	if (bOnOff) {
		cmd = "SB 7";
	}
	else {
		cmd = "CB 7";
	}
	if (_SendN11(cmd)) {
		p413App->Global.bSpectLight = bOnOff;	// For simulation only [6/16/2013 Yuen]
		return TRUE;
	}
	return FALSE;
}

BOOL CGLMotor::SetSpectrometerPower(BOOL bOnOff) {
	p413App->Global.bSpecPower = bOnOff;
	if (bOnOff) {
		cmd = "SB 6";
	}
	else {
		cmd = "CB 6";
	}
	return _SendN11(cmd);
}

// Non blocking [3/30/2013 Administrator]
BOOL CGLMotor::devMoveAXY(float iX, float iY) {
	cmd.Format("Va=%.0f;Vb=%.0f;XQ #MAXY,1", iX, iY);
	if (_ExecPrgm(cmd, 3)) {
		return CSiBase::devMoveAXY(iX, iY);
	}
	MsgStr.Format("devMoveAXY(%.3f,%.3f) fail", iX, iY);
	p413App->Log(MsgStr, 2);
	return FALSE;
}

// Non blocking [3/30/2013 Administrator]
BOOL CGLMotor::devMoveAXYZ1(float iX, float iY, float iZ1) {
	ASSERT(0); // to be implemented [6/15/2013 Yuen]
	cmd.Format("Va=%.0f;Vb=%.0f;Vc=%.0f;XQ #MAXYZ1,1", iX, iY, iZ1);
	if (_ExecPrgm(cmd, 3)) {
		return CSiBase::devMoveAXYZ1(iX, iY, iZ1);
	}
	return FALSE;
}

// Do not lock [7/16/2013 Yuen]
BOOL CGLMotor::_WaitForXY(int tout, BOOL& bPos) {
	ULONGLONG tick = GetTickCount64();
	while (int(GetTickCount64() - tick) < tout) {
		Sleep(50);
		cmd = "XQ #IMA,5";
		rsp = _SendR(cmd, 500);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[11];
			for (int j = 0; j < 11; j++) {
				if (!tok.Next(cs[j])) {
					break;
				}
			}
			// First token must be #IMA and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 4) || (cs[0] != "#IMA")) {
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[5].GetLength() != 1) || (cs[5] != "@")) {
				continue;
			}
			// Message format is now verified [10/1/2012 Yuen]
			if (CS2Value(cs[1])) {
				continue;
			}
			if (CS2Value(cs[2])) {
				continue;
			}
			bPos = FALSE;
			if ((cs[10].GetLength() == 1) && (cs[10] == "*")) {
				C413Global* g = &p413App->Global;
				moAxis[g->X_Axis - 1].fPos = atoi(cs[6]);
				moAxis[g->Y_Axis - 1].fPos = atoi(cs[7]);
				moAxis[g->EchoTop_Axis - 1].fPos = atoi(cs[8]);
				moAxis[g->RoughZ_Axis - 1].fPos = atoi(cs[9]);
				bPos = TRUE;
			}
			return TRUE;
		}
	}
	ResetMotionMemory(XAXIS); // 1/11/22
	ResetMotionMemory(YAXIS); // 1/11/22
	p413App->Log("WaitForXY: Timeout", 2);
	return FALSE;
}

BOOL CGLMotor::IsWaferPresent() {
	cmd.Format("XQ #IN1,7");
	rsp = _SendR(cmd, 1000);
	if (rsp.GetLength() > 0) {
		CTokenizer tok(rsp, " ");
		CString cs[3];
		for (int j = 0; j < 3; j++) {
			tok.Next(cs[j]);
		}
		// First token must be #IN and one character only [10/1/2012 Yuen]
		if ((cs[0].GetLength() != 3) || (cs[0] != "#IN")) {
			return FALSE;
		}
		// Last token must be @ and one character only [10/1/2012 Yuen]
		if ((cs[2].GetLength() != 1) || (cs[2] != "@")) {
			return FALSE;
		}
		// Message format is now verified [10/1/2012 Yuen]
		if (pGDev->SubModel == CGlobalDeviceContainer::UMC) {// UMC TAIWAN 2022 IS USING 1 AS NOT PRESENT, 0 AS PRESENT
			return !(BOOL)atoi(cs[1]);
		}
		return (BOOL)atoi(cs[1]);
	}
	p413App->Log("[ME] IsWaferPresent: Unexpected response from DMC", 2);
	return TRUE; // False positive intentionally [10/2/2012 FSM413]
}

// do not lock [7/16/2013 Yuen]
BOOL CGLMotor::_WaitStop(int nAxis, long tout, BOOL& bPos) {
	AXISID a = GetAxisNo(nAxis);
	if (a == NOAXIS) {
		return FALSE;
	}
	ULONGLONG tick = GetTickCount64();
	while (int(GetTickCount64() - tick) < tout) {
		cmd = "XQ #IMA,5";
		rsp = _SendR(cmd, 500);
		if (rsp.GetLength() > 0) {
			CTokenizer tok(rsp, " ");
			CString cs[11];
			for (int j = 0; j < 11; j++) {
				if (!tok.Next(cs[j])) {
					break;
				}
			}
			// First token must be #IMA and one character only [10/1/2012 Yuen]
			if ((cs[0].GetLength() != 4) || (cs[0] != "#IMA")) {
				continue;
			}
			// Last token must be @ and one character only [10/1/2012 Yuen]
			if ((cs[5].GetLength() != 1) || (cs[5] != "@")) {
				continue;
			}

			// Message format is now verified [10/1/2012 Yuen]
			switch (a) {
			case XAXIS:
				if (CS2Value(cs[1])) {
					continue;
				}
				break;
			case YAXIS:
				if (CS2Value(cs[2])) {
					continue;
				}
				break;
			case ECHOTOPZAXIS:
				if (CS2Value(cs[3])) {
					continue;
				}
				break;
			case ROUGHZAXIS:
				if (CS2Value(cs[4])) {
					continue;
				}
				break;
			}
			bPos = FALSE;
			if ((cs[10].GetLength() == 1) && (cs[10] == "*")) {
				C413Global* g = &p413App->Global;
				moAxis[g->X_Axis - 1].fPos = atoi(cs[6]);
				moAxis[g->Y_Axis - 1].fPos = atoi(cs[7]);
				moAxis[g->EchoTop_Axis - 1].fPos = atoi(cs[8]);
				moAxis[g->RoughZ_Axis - 1].fPos = atoi(cs[9]);
				bPos = TRUE;
			}
			return TRUE;
		}
		else SleepEx(500);
	}
	ResetMotionMemory(nAxis); // 1/11/22
	p413App->Log("WaitStop Timeout!", 2);
	return FALSE;
}

void CGLMotor::SleepEx(DWORD tout) {
	Sleep(tout);
	return;

	ULONGLONG tm = GetTickCount64();
	while ((GetTickCount64() - tm) < tout) {
		MSG msg;
		while (PeekMessage(&msg, AfxGetMainWnd()->m_hWnd, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CGLMotor::ReconnectTCP() {
	pLockExec->Lock();
	CloseSock2();
	CloseSock1();
	SleepEx(8000);
	ReconnectSoc2();
	ReconnetSoc1();
	pLockExec->Unlock();
}

BOOL CGLMotor::ReconnetSoc1() {
	pSock1->bConnected = FALSE;
	if (pSock1->Create()) {
		pSock1->SetCallBack(OnMessageProc1);
		pSock1->AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT | FD_OOB);	// Do not turn on FD_READ, will handle it independently [7/18/2013 Yuen]
		if (pSock1->Connect(p413App->IPAddress, 23) == 0) {
			int err = GetLastError();
			if (err == WSAEWOULDBLOCK) {
				// Connection OK, initialize it [6/3/2012 Administrator]
				short cnt = 0;
				while (!pSock1->bConnected && (cnt < 10)) {
					p413App->Yld(500);
					cnt++;
				}
				if (cnt < 10) {
					CString str = "New socket 23 connected";
					p413App->Log(str);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CGLMotor::ReconnectSoc2() {
	pSock2->bConnected = FALSE;
	if (pSock2->Create()) {
		pSock2->SetCallBack(OnMessageProc2);
		pSock2->AsyncSelect(/*FD_READ |*/ FD_WRITE | FD_CLOSE | FD_CONNECT | FD_OOB);	// Do not turn on FD_READ, will handle it independantly [7/18/2013 Yuen]
		if (pSock2->Connect(p413App->IPAddress, 4132) == 0) {
			int err = GetLastError();
			if (err == WSAEWOULDBLOCK) {
				// Connection OK, initialize it [6/3/2012 Administrator]
				short cnt = 0;
				while (!pSock2->bConnected && (cnt < 10)) {
					p413App->Yld(500);
					cnt++;
				}
				if (cnt < 10) {
					Initialize2();
					CString str = "New socket 4132 connected";
					p413App->Log(str);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void CGLMotor::CloseSock1() {
	if (pSock1) {
		pSock1->ShutDown(2);
		pSock1->Close();
		pSock2->bConnected = FALSE;
	}
}

void CGLMotor::CloseSock2() {
	if (pSock2) {
		pSock2->ShutDown(2);
		pSock2->Close();
		pSock2->bConnected = FALSE;
	}
}

void CGLMotor::WaitConnect() {
	while (!pSock1->bConnected || !pSock2->bConnected) {
		p413App->Yld(500);	// Temporary measure to get the message pumping [7/30/2013 Yuen]
	}
}

// [6/17/2020] ===========================================
int CGLMotor::find_avilable_handle_number() {
	int number_of_available = 0;
	ULONGLONG tick = GetTickCount64();
	while ((GetTickCount64() - tick) < 10000) {
		cmd = "TH";
		rsp = _SendR(cmd, 2000);
		if (rsp.GetLength() > 0) {
			CTokenizer t(rsp, "\r");
			CString str;
			t.Next(str);
			while (t.Next(str)) {
				if (str.Find("AVAILABLE") != -1)
					number_of_available += 1;
			}
			return number_of_available;
		}
		SleepEx(100);
	}
	return 0;
}

BOOL CGLMotor::clear_handle() {
	cmd = "IHT=>-3";
	if (!_Send(cmd.GetBuffer(0), cmd.GetLength())) return FALSE;

	cmd = "IHS=>-3";
	if (!_Send(cmd.GetBuffer(0), cmd.GetLength())) return FALSE;

	CloseSock1();
	CloseSock2();
	SleepEx(4000);

	if (!ReconnetSoc1()) return FALSE;
	if (!ReconnectSoc2()) return FALSE;
	return TRUE;
}

BOOL CGLMotor::check_handle() {
	int result = 0, number_of_available = find_avilable_handle_number();
	if (number_of_available < 4) {
		return clear_handle();
	}
	return TRUE;
}
//========================================================

// [01102022]

BOOL CGLMotor::ResetMotionMemory(int nAxis) {
	cmd.Format("Va=%d;XQ#RSVM;", nAxis);
	SleepEx(100);
	if (_ExecPrgm(cmd, 3)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CGLMotor::ResetAllMotionMemory() {
	if (ResetMotionMemory(XAXIS)) {
		if (ResetMotionMemory(YAXIS)) {
			if (ResetMotionMemory(ECHOTOPZAXIS)) {
				if (ResetMotionMemory(ROUGHZAXIS)) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}