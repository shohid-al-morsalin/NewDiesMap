#ifndef CSERIAL_H
#define CSERIAL_H
class CSerial
{
public:
	virtual BOOL Open(int nPort = 2, int nBaud = 9600, char nPa = 'n', int nBit = 8, int nStop = 1);
	virtual BOOL Close(void);
	virtual BOOL ReadByte(BYTE & by);
	virtual BOOL ClearRx();
	virtual int SendData(const char *, int);
	virtual int ReadData(void *, int);
protected:
	BOOL m_bOpened;
};
#endif //CSERIAL_H
