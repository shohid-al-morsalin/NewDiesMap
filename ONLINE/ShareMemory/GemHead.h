#ifndef _GEMHEAD_H_
#define _GEMHEAD_H_

typedef struct {
	BYTE	m_DataLen[4];
	BYTE	m_SessionId[2];
	BYTE	m_Stream;
	BYTE	m_Function;
	BYTE	m_Ptype;
	BYTE	m_Stype;
	BYTE	m_Sysbyte[4];
} GEM_HEADONLY; // Everything else except the actual data

#endif