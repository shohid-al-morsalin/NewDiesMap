#pragma once
// #include <afxmt.h>

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

#define RESPLEN	256

class CComm {
	HWND hWnd;
	HANDLE ComDev;
	CCriticalSection CS;

	BYTE byPort;
	WORD wBaudRate;
	BOOL nXonXoff;
	BYTE bBitSize, bFlowCtrl, bParity, bStopBits;

	DWORD dwThreadID;
	HANDLE  hCommWatchThread;
	OVERLAPPED  osWrite, osRead, osPost;

public:
	BOOL bConnected;
	char resp[RESPLEN + 1];

public:
	short ReadChr(char* ch);
	BOOL SendKEY(CString msg);
	BOOL SendOmron(CString msg);
	BOOL SendHCA(CString msg, DWORD tout);
	BOOL SendTDK(CString msg, int Rpy);
	BOOL RequestTDK(CString msg, char* text, short max);
	BOOL GetRespCR(char* msg, short max, DWORD tout);
	BOOL GetRespLFFoup(char* msg, short max, DWORD tout);
	BOOL SendAck(CString msg, DWORD tout);
	BOOL GetRespBrk(char* msg, short max, DWORD tout);
	BOOL GetRespPri(char* msg, short max);
	BOOL GetRespPri2(char* msg, short max);
	BOOL SendBrk(/*char **/CString text, DWORD tout = 120000);
	BOOL SendLogosol(CString text);//Tushar 21082024
	BOOL SendLogosolAction(CString text);//Tushar 21082024
	BOOL SendLogosolTest(/*char **/CString text); //Tushar just for testing

			
	BOOL GetRespLogosol(char* msg, short max);
	BOOL GetRespLogosol_Action(char* msg, short max);
	BOOL GetRespLogosolTest(char* msg, short max, CString cmd);
	 //BOOL GetRespLogosolTest(char* msg, short max);
	//Tushar 21082024
	BOOL SendPri(/*char **/CString text);
	BOOL SendPri2(/*char **/CString text);
	BOOL WriteCh(char* ch);
	void FlushTx();
	void ClearTx();
	BOOL GetResMitutoyo(char* str, short len);
	void SetFlowControl(short ctrl);
	void Wait(DWORD ms);
	BOOL SendScl(char* msg, short n);
	BOOL GetRespondCR(char* msg, short n);
	BOOL GetRespondLF(char* msg, short n);
	void ClearRx();
	BOOL WaitForRespond();
	void SetCallnWnd(HWND hWnd);
	BOOL SetParam(BYTE bPort, WORD wBaudRate, BYTE bBitSize, BYTE bParity, BYTE bStopBits, BYTE bFlowCtrl = FC_RTSCTS);
	BOOL Open();

	BOOL Close();
	BOOL SetupConnection();
	BOOL _Send(CString& msg);
	BOOL _Send2(CString msg);
	int Write(char* lpByte, int len);
	int ReadBlock(char* lpBuf, DWORD len);
	int _Read(char* lpszBlock, int nMaxLength);
	void ReadStr(char* lpstr, int nMaxLength, BYTE term);
	BOOL SendFSMSignalTower(char Cmd);
	BOOL Send(char* msg, short max);
	BOOL SendCommandEcho(char* msg, short max);
	BOOL ReadResp(char* msg, short max);
public:
	BOOL QuerryData(CString& Cmd);
	void Log(CString msg, short lvl = 5);
	BOOL RequestFoup(CString msg, char* text, short max, DWORD tout);
	BOOL IsOpened();
	CString ErrorStr;
	int GetComPortNo();
	BOOL Request(CString msg, char* text, short max, DWORD tout);
	CComm();
	virtual ~CComm();
};
