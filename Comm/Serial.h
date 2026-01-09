// Serial.h
#ifndef __SERIAL_H__
#define __SERIAL_H__
#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13
#include "CSerial.h"
class CSerialReal : public CSerial
{
public:
	BOOL ReadByte(BYTE & by);
	CSerialReal();
	~CSerialReal();
	BOOL Open(int nPort = 1, int nBaud = 9600, char nPa = 'n', int nBit = 8, int nStop = 1);
	BOOL Close(void);
	int ReadData(void *, int);
	int SendData(const char *, int);
	int ReadDataWaiting(void);

	BOOL IsOpened(void)
	{
		return (m_bOpened);
	}

	BOOL Flush();
	BOOL ClearRx();

	/** Read all info in comm buffer, (BINARY OPERATION) */
	int ReadBlock(unsigned char * pBuf, int len);
protected:
	//BOOL WriteCommByte(unsigned char);
	BOOL WriteCommByte(CString buffer, int size);
	
	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	int ICDelay;
	int ReadICDelay();
};
#endif
