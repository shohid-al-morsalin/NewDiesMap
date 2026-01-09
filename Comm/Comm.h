// Comm.h: interface for the CComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMM_H__DBF7CFE1_BA61_11D1_B8C5_444553540000__INCLUDED_)
#define AFX_COMM_H__DBF7CFE1_BA61_11D1_B8C5_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

class CComm  
{

public:
	BOOL WriteCh(unsigned char *ch);
	void FlushTx();
	void ClearTx();
	BOOL Send(CString msg);
	BOOL GetResMitutoyo(unsigned char *str, int len);
	void SetFlowControl (int ctrl);
	void Wait (DWORD ms);
	BOOL SendCommand(unsigned char *msg, int n);
	BOOL GetRespondCR(unsigned char *msg, int n);
	BOOL GetRespondLF(unsigned char *msg, int n);
	void ClearRx();
	BOOL WaitForRespond();
	void SetCallnWnd (HWND hWnd);
	HWND hWnd;
	BOOL SetParam (BYTE bPort, WORD wBaudRate, BYTE bBitSize, BYTE bParity, BYTE bStopBits,BYTE bFlowCtrl= FC_RTSCTS);
	HANDLE ComDev;
	BOOL Open ();
	CComm();
	virtual ~CComm();

	BYTE bPort;
	WORD wBaudRate;
	BOOL fConnected, nXonXoff;
	BYTE bBitSize, bFlowCtrl, bParity, bStopBits;

	DWORD dwThreadID ;
	HANDLE  hCommWatchThread;
	OVERLAPPED  osWrite,osRead, osPost;

	BOOL Close ();
	BOOL SetupConnection();
	int Write (unsigned char * lpByte,int len);
	int ReadBlock(unsigned char * lpBuf,DWORD len);
	int Read(unsigned char * lpszBlock, int nMaxLength);
	void ReadStr (unsigned char * lpstr, int nMaxLength, BYTE term);
};

#endif // !defined(AFX_COMM_H__DBF7CFE1_BA61_11D1_B8C5_444553540000__INCLUDED_)
