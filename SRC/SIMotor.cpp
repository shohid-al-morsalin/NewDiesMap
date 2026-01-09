// SiMotor.cpp: implementation of the CSiMotor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SiMotor.h"
#include "..\SRC\Comm.h"
//#include "..\SCN_DAQ\Daq.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSiMotor::CSiMotor()
{
	pCom = NULL;
	bSimu = FALSE;
}

CSiMotor::~CSiMotor()
{
	Close();
}

BOOL CSiMotor::Open(short port)
{
	if (pCom) 
	{
		Close();
	}
	pCom = new CComm;
	if (!pCom) 
	{
		ASSERT(0);
		return FALSE;
	}
	pCom->SetParam((BYTE)port,9600,8,NOPARITY,1,0);
	return pCom->Open();

}

BOOL CSiMotor::Close()
{
	if (pCom) 
	{
		pCom->Close();
		delete pCom;
	}
	pCom = NULL;
	return TRUE;
}

BOOL CSiMotor::FeedLength(long position,BOOL bWait)
{
    CSingleLock Lock(&CS);
	Lock.Lock();

	if (!pCom) {
		ASSERT(0);
		return FALSE;
	}
	CString str;
	str.Format("DI%ld\r\n",position);
	pCom->Send(str.GetBuffer(0),str.GetLength());
	Sleep(200);
	pCom->Send("FL\r\n",4);
	Sleep(200);
	if (!bWait) 
	{
		return TRUE;
	}
	while (1) 
	{
		pCom->Send("RS\r\n",4);
		Sleep(200);
		char re[257] = {0};
		pCom->ReadBlock(re,256);
		TRACE("%s/n",re);
		if (strchr(re,'R')) break;
	}
	return TRUE;
}

BOOL CSiMotor::SendCmd(CString str)
{
	if (!pCom) 
	{
		ASSERT(0);
		return FALSE;
	}

	pCom->SendCommandEcho(str.GetBuffer(0), str.GetLength());
//	Sleep(100);
	return TRUE;
}

BOOL CSiMotor::Reset()
{
    CSingleLock Lock(&CS);
	Lock.Lock();

	// Negative direction is down i.e. close
	pCom->Send("DI-1\r\n",6);
	Sleep(200);
	pCom->Send("FSHR\r\n",6);
	Sleep(1000);
	while (1) 
	{
		pCom->Send("RS\r\n",4);
		Sleep(500);
		char re[257] = {0};
		pCom->ReadBlock(re,256);
		TRACE("%s\n",re);
		if (strchr(re,'R'))
		{
			break;
		}
	}
	// Always set this position as ZERO for reference
	pCom->Send("SP0\r\n",5);
	Sleep(200);
	pCom->Send("DI1\r\n",5);
	return TRUE;
}

unsigned long CSiMotor::Position()
{
    CSingleLock Lock(&CS);
	Lock.Lock();

	pCom->Send("IP\r\n",4);
	Sleep(200);
	char re[257] = {0};
	pCom->ReadBlock(re,256);
	//TRACE("%s\n",re);
	char *pCh = &re[3];
	while (*pCh == '0') 
	{
		pCh++;
	}
	unsigned long Pos=0;
	::sscanf(pCh,"%x",&Pos);
	return Pos;
}

BOOL CSiMotor::QuerryData(CString &Reply, long tout)
{
    CSingleLock Lock(&CS);
	Lock.Lock();

	if (!SendCmd(Reply)) 
	{
		return FALSE;
	}
	Sleep(160);
	char re[32] = {0};
//	pCom->ReadBlock((char *)re,32);
	pCom->GetRespCR(re,32, 240000);
	Reply = re;
	return TRUE;
}

BOOL CSiMotor::Init(float Current,float Velocity)
{
	CString Fmt;
	// Set Current {.8A}
	Fmt.Format("CC%2.2f\r\n",Current);
	pCom->Send(Fmt.GetBuffer(0),Fmt.GetLength());
	::Sleep(500);

	// Set Power up Current to same value
	Fmt.Format("PC%2.2f\r\n",Current);
	pCom->Send(Fmt.GetBuffer(0),Fmt.GetLength());
	::Sleep(500);
 
	// Set Velocity 
	Fmt.Format("VE%2.2f\r\n",Velocity);
	pCom->Send(Fmt.GetBuffer(0),Fmt.GetLength());
	::Sleep(500);

	// Set Startup status 2
	Fmt.Format("PM2\r\n");
	pCom->Send(Fmt.GetBuffer(0),Fmt.GetLength());
	::Sleep(500);

	// Define Limits
	Fmt.Format("DL2\r\n");
	pCom->Send(Fmt.GetBuffer(0),Fmt.GetLength());
	::Sleep(500);
	return TRUE;
}

BOOL CSiMotor::IsMoving()
{
    CSingleLock Lock(&CS);
	Lock.Lock();

	pCom->Send("RS\r\n",4);
	Sleep(500);
	char re[257] = {0};
	pCom->ReadBlock(re,256);
	TRACE("%s\n",re);
	if (strchr(re,'M'))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CSiMotor::SetDigitalOut(unsigned char Byte, BOOL bHigh)
{
    CSingleLock Lock(&CS);
	Lock.Lock();

	// only 3 Outputs are availble
	if (Byte >0 && Byte <= 4) 
	{
		CString Str;
//		pCom->Flush();
		Str.Format("SO%d%s\r\n",Byte,(bHigh == TRUE)?"H":"L");
		pCom->SendCommandEcho(Str.GetBuffer(0),Str.GetLength());
		TRACE(Str);
//		char re[1024] = {0};
//		pCom->ReadBlock(re,1024);
//		TRACE("%s\n",re);
		return TRUE;
	}
	return FALSE;
}

BOOL CSiMotor::CheckBit(short Bit)
{
    CSingleLock Lock(&CS);
	Lock.Lock();

	// only 4 Inputs are availble
	if (pCom && (Bit >0 && Bit < 5))
	{
		pCom->SendCommandEcho("IS\r",3/*4*/);
		char re[257] = {0};
		::Sleep(120);
		pCom->ReadBlock(re,13);
		CString Temp = re;
		Temp.TrimLeft();
		Temp.TrimRight();
		if(Temp.GetLength()>10)
		{
			return Temp[11-Bit] == '1';
		}
		else
		{
			return TRUE;
		}
		// Reply will be #IS=00000000
		// Whereas last bit is Bit #1 & so on....
//		return re[11-Bit] == '1';
	}
	return FALSE;
}

BOOL CSiMotor::DigInM2(short Bit)
{
	ASSERT(0);
	// Motor is not supported for 900C2c
	return FALSE;//return pR->CheckBit(Bit);
}

BOOL CSiMotor::DigInM1(short Bit)
{
	return CheckBit(Bit);
}

BOOL CSiMotor::DigOutM2(unsigned char Bit, BOOL bHigh)
{
	ASSERT(0);
	// Motor is not supported for 900C2c
	return FALSE;
}

BOOL CSiMotor::DigOutM1(unsigned char Bit, BOOL bHigh)
{
	return SetDigitalOut(Bit,bHigh);
}

BOOL CSiMotor::IsLightCurtainTriggered()
{
	CString str;
	str = GetDigitalIn();
	if (str.GetAt(1) == '1')
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CSiMotor::TestClose()
{
	// To be completed
	ASSERT(0);
	return FALSE;
}


BOOL CSiMotor::TestOpen()
{
//	if (!pDAQ) {
//		return FALSE;
//	}
	if (DigOutM1(1,FALSE)) {
		Sleep(1000);
		// High:Close
//		BOOL Ret = pDAQ->i_DigInDAQ(3);
		CString str = GetDigitalIn();
		if (str.GetAt(3) == '1') {
			DigOutM1(1,TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSiMotor::CommOK()
{
	BOOL bRes = FALSE;
	CString Cmd;
	Cmd.Format("RS\r\n");
	if (QuerryData(Cmd,15000)) 
	{
		if (Cmd.GetLength()&&Cmd[0] == 'R') 
		{
			bRes = TRUE;
		}
	}
	return bRes;
}

CString CSiMotor::GetDigitalIn()
{
	CString Cmd;
	Cmd.Format("IS\r\n");
	if (QuerryData(Cmd,15000)) 
	{
		Cmd.TrimLeft();
		Cmd.TrimRight();
		return Cmd.Mid(3);
	}
	return "????????";
}

void CSiMotor::SelectLaser(short wavelength)
{
	switch(wavelength) {
	case 780:
	case 750:
		DigOutM1(4,FALSE);
		break;
	case 650:
	case 810:
		DigOutM1(4,TRUE);
		break;
	default:
		ASSERT(0);
		break;
	}
}

CString CSiMotor::GetDigitalInChamber()
{
	int cnt = 5;
	CString Cmd;
	while (cnt--)
	{
		Cmd.Format("IS\r\n");  // Just in case QuerryData modify the string
		if (QuerryData(Cmd,15000)) 
		{
			if (Cmd.GetLength() < 12)
			{
				continue;
			}
			Cmd.TrimLeft();
			Cmd.TrimRight();
			return Cmd.Mid(3);
		}
		else {
			break;
		}
	}
	return "????????";
}

short CSiMotor::GetWaferPresent()
{
	short status = 0;
	CString str;

	str = GetDigitalInChamber();
	if (str.GetAt(5) == '0')		// aligner
	{
		status += 1;
	}
	if (str.GetAt(4) == '0')		// scale
	{
		status += 2;
	}
	if (str.GetAt(3) == '0')		// scan stage
	{
		status += 4;;
	}
	return status;

}

BOOL CSiMotor::OpenChamber()
{
	CString Cmd;
	Cmd.Format("OPEN\r\n");  // Just in case QuerryData modify the string
	if (QuerryData(Cmd, 240000)) {
		if (Cmd.Find("OK") != -1)
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSiMotor::CloseChamber()
{
	CString Cmd;
	Cmd.Format("CLOSE\r\n");  // Just in case QuerryData modify the string
	if (QuerryData(Cmd, 240000)) {
		if (Cmd.Find("OK") != -1)
		{
			return TRUE;
		}
	}
	return FALSE;
}
