// MsgItemProcessor.h: interface for the CMsgItemProcessor class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class CStreamSocket;

class CMsgItem;

class CMsgItemProcessor {
public:
	CStreamSocket* m_pStream; // Stream socket to receive from
	int Send(CMsgItem* pMsg);
	CMsgItemProcessor();
	virtual ~CMsgItemProcessor();
};