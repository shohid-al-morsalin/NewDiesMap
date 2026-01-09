// MsgItemProcessor.cpp: implementation of the CMsgItemProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MsgItemProcessor.h"
#include "..\SERIAL\MsgItem.h"
#include "..\TCP\StreamSocket.h"
#include "..\SRC\BMake.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgItemProcessor::CMsgItemProcessor() {
	m_pStream = NULL;
}

CMsgItemProcessor::~CMsgItemProcessor() {}

int CMsgItemProcessor::Send(CMsgItem* pMsg) {
	if (m_pStream) {
		STREAMDATA* pData = new STREAMDATA;
		if (pData) {
			pData->nLen = pMsg->GetLength() + sizeof(int);
			pData->pData = new char[pData->nLen + 16];
			if (pData->pData) {
				char* p = pData->pData;
				CMake::Binary(pData->nLen, &p);
				pMsg->Binary(p);
				if ((m_pStream->Write(pData)) == CWINSOCK_NOERROR) {
					delete pMsg;
					return CRtnCode::NOERR;
				}
				delete[] pData->pData;
			}
			delete pData;
		}
	}
	delete pMsg;
	return CRtnCode::ERR;
}