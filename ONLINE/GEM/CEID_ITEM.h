#pragma once

#define MAX_CEIDLIST       600

class CEID_ITEM {
public:
	BOOL  m_reportflag;
	UINT  m_ceid;
	char  m_cename[128];
	UINT* m_rptid;
	int   m_rptidcurrentcnt;

public:
	void Set(UINT ceid, char* name);
	CEID_ITEM();
	virtual ~CEID_ITEM();
};
