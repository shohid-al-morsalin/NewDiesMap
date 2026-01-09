#ifndef _SECSHEAD_H_
#define _SECSHEAD_H_

#include "define.h"

typedef struct {
	BYTE	m_DataLen[4];
	BYTE	m_SessionId[2];
	BYTE	m_Stream;
	BYTE	m_Function;
	BYTE	m_Ptype;
	BYTE	m_Stype;
	BYTE	m_Sysbyte[4];
	BYTE	m_Message[MAX_RECVSTRING];
} GEM_HEADER;	// GEM Header and Message, a very large structure, never alloc on stack [8/12/2012 Administrator]

#endif