#include "stdafx.h"
#include "strSOp.h"
#include "CEID_ITEM.h"

CEID_ITEM::CEID_ITEM() {
	m_reportflag = FALSE;
	m_ceid = 0;
	memset(m_cename, 0, 128);
	m_rptid = NULL;
	m_rptidcurrentcnt = 0;
}

CEID_ITEM::~CEID_ITEM() {
	if (m_rptid) {
		delete[] m_rptid;
	}
}

void CEID_ITEM::Set(UINT ceid, char* name) {
	m_ceid = ceid;
	strscpy(m_cename, sizeof(m_cename), name);
}