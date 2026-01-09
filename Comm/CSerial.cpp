#include "stdafx.h"
#include "CSerial.h"
BOOL CSerial::ReadByte(BYTE & by)
{
	return FALSE;
}

BOOL CSerial::Close(void)
{
	m_bOpened = TRUE;
	return (TRUE);
}

BOOL CSerial::Open(int nPort, int nBaud, char nPa, int nBit, int nStop)
{
	if (!m_bOpened)
	{
		m_bOpened = TRUE;
	}
	return (TRUE);
}

BOOL CSerial::ClearRx()
{
	TRACE("Clear RX\n");
	return TRUE;
}

int CSerial::SendData(const char * buffer, int size)
{
	if (!m_bOpened)
	{
		TRACE("SendData: Not open\n");
		ASSERT(0);
		return -1;
	}
	TRACE("SendData\n");
	return 0;
}

int CSerial::ReadData(void * buffer, int limit)
{
	if (!m_bOpened)
	{
		TRACE("ReadData: Not open\n");
		return 0;
	}
	return 0;
}
