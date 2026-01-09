#include "stdafx.h"

// #include <io.h>
// #include <fcntl.h>
#include "GlobalDeviceContainer.h"

#include "CediListCtroller.h"

CCediListCtroller CEIDCtlr;

CCediListCtroller::CCediListCtroller() {
	m_RptHead = NULL;
}

CCediListCtroller::~CCediListCtroller() {
	Delete_All_RptList();
}

BOOL CCediListCtroller::Initialize_List() {
	BOOL bRet = TRUE;
	// CEID LIST TO READ FROM CEID.PAR FILE
	if (!Initialization_CeidList()) {
		bRet = FALSE;
	}
	return bRet;
}

BOOL CCediListCtroller::Initialization_CeidList() {
	int  nArryCnt = 0;
	LoadEnaDisCEIDAll();
	LoadReport();
	LoadMapping();
	return TRUE;
}

BOOL CCediListCtroller::chk_rptid(unsigned int nRptid, BOOL bLog) {
	BOOL bRet = FALSE;
	if (m_RptHead != NULL) {
		bRet = Search_rptid(nRptid, m_RptHead);
	}
	if (!bRet && bLog) {
		CString msg;
		msg.Format("RptID %d not defined", nRptid);
		Log(msg);
	}
	return bRet;
}

BOOL CCediListCtroller::Search_rptid(unsigned int rptno, RPT_LIST* pRptlist) {
	BOOL bRet = FALSE;

	while (pRptlist != NULL) {
		if (pRptlist->m_rptno == rptno) {
			return TRUE;
		}
		else {
			pRptlist = pRptlist->m_rptnext;
		}
	}
	return bRet;
}

BOOL CCediListCtroller::chk_svid(unsigned int nSvid) {
	SVID_ITEM* p = m_SVList.Get(nSvid);
	if (!p) {
		CString msg;
		msg.Format("SVID %d not defined", nSvid);
		Log(msg);
		return FALSE;
	}

	if (IsSVID(p)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::chk_DVid(unsigned int nSvid) {
	SVID_ITEM* p = m_SVList.Get(nSvid);
	if (!p) {
		CString msg;
		msg.Format("DVID %d not defined", nSvid);
		Log(msg);
		return FALSE;
	}
	if (IsDVID(p)) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::chk_Vid(unsigned int nSvid) {
	SVID_ITEM* p = m_SVList.Get(nSvid);
	if (!p) {
		CString msg;
		msg.Format("VID %d not defined", nSvid);
		Log(msg);
		return FALSE;
	}
	if (IsVID(p)) {
		return TRUE;
	}
	return FALSE;
}

void CCediListCtroller::Add_Rpt(UINT* svid, int svidcnt, UINT rptno) {
	m_RptHead = Add_rptlist(m_RptHead, svid, svidcnt, rptno);
}

RPT_LIST* CCediListCtroller::Add_rptlist(RPT_LIST* baseptr, UINT* svid, int svidcnt, UINT rptno) {
	int i = 0;

	if (baseptr == NULL) {
		m_CurMaxRptCount++;
		baseptr = new  RPT_LIST;
		baseptr->svidcnt = svidcnt;
		baseptr->m_rptno = rptno;
		baseptr->svid = new UINT[svidcnt];
		for (i = 0; i < svidcnt; i++) {
			baseptr->svid[i] = svid[i];
		}

		baseptr->m_rptnext = NULL;
	}
	else {
		baseptr->m_rptnext = Add_rptlist(baseptr->m_rptnext, svid, svidcnt, rptno);
	}

	return (baseptr);
}

void CCediListCtroller::Delete_All_RptList() {
	if (m_RptHead != NULL) {
		delete_all(m_RptHead);
	}

	m_RptHead = NULL;

	for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
		if (m_CEList.m_Ceid[i].m_rptidcurrentcnt) {
			delete[]m_CEList.m_Ceid[i].m_rptid;
			m_CEList.m_Ceid[i].m_rptid = NULL;
		}
		m_CEList.m_Ceid[i].m_rptidcurrentcnt = 0;
	}
}

void CCediListCtroller::delete_all(RPT_LIST* rpt) {
	if (rpt != NULL) {
		delete_all(rpt->m_rptnext);
		if (rpt->svidcnt > 0) {
			delete rpt->svid;
			rpt->svid = NULL;
		}
		delete rpt;
		rpt = NULL;
	}
}

BOOL CCediListCtroller::Check_Ceid(UINT CEID) {
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::ceid_defined(UINT CEID) {
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		return (p->m_rptidcurrentcnt != 0) ? TRUE : FALSE;
	}
	return FALSE;
}

BOOL CCediListCtroller::evtrpt_all_setting(BOOL flag) {
	BOOL bRet = TRUE;
	int i = 0;
	if (flag == TRUE) {
		for (i = 0; i < CEID_LIST::LASTCEID; i++) {
			m_CEList.m_Ceid[i].m_reportflag = TRUE;
		}
	}
	else {
		for (i = 0; i < CEID_LIST::LASTCEID; i++) {
			m_CEList.m_Ceid[i].m_reportflag = FALSE;
		}
	}

	return bRet;
}

BOOL CCediListCtroller::evtrpt_setting(UINT CEID, BOOL flag) {
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		p->m_reportflag = flag;
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::SaveEnaDisCEIDAll() {
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[256];

	CSingleLock Lock(&CS);
	Lock.Lock();
	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);

	_makepath(inifile, drive, dir, "DEFAULT", "EVT");

	FILE* fp;
	fp = fopen(inifile, "wb");
	if (fp) {
		int siz = sizeof(m_CEList.m_Ceid[0].m_ceid);
		int count = CEID_LIST::LASTCEID;
		fwrite(&count, sizeof(count), 1, fp);
		for (int i = 0; i < count; i++) {
			fwrite(&m_CEList.m_Ceid[i].m_ceid, siz, 1, fp);
			fwrite(&m_CEList.m_Ceid[i].m_reportflag, sizeof(BOOL), 1, fp);
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::LoadEnaDisCEIDAll() {
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[256];

	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);

	_makepath(inifile, drive, dir, "DEFAULT", "EVT");
	FILE* fp;
	fp = fopen(inifile, "rb");
	if (fp) {
		BOOL flag;
		int cnt;
		unsigned int ceid;

		if (fread(&cnt, sizeof(cnt), 1, fp) == 1) {
			for (int i = 0; i < cnt; i++) {
				if (fread(&ceid, sizeof(ceid), 1, fp)) {
					if (fread(&flag, sizeof(BOOL), 1, fp)) {
						for (int j = 0; j < CEID_LIST::LASTCEID; j++) {
							if (m_CEList.m_Ceid[j].m_ceid == ceid) {
								m_CEList.m_Ceid[j].m_reportflag = flag;
								break;
							}
						}
					}
				}
			}
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

int CCediListCtroller::get_rptcount(UINT CEID) {
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		return p->m_rptidcurrentcnt;
	}
	return 0;
}

BOOL CCediListCtroller::getting_rptno(UINT CEID, UINT* rptno) {
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		for (int j = 0; j < p->m_rptidcurrentcnt; j++) {
			rptno[j] = p->m_rptid[j];
		}
		return TRUE;
	}
	return FALSE;
}

int CCediListCtroller::get_svidcount(UINT rptno) {
	int ret = 0;
	if (m_RptHead != NULL) {
		ret = getting_svidcount(rptno, m_RptHead);
	}
	return ret;
}

int CCediListCtroller::getting_svidcount(UINT rptid, RPT_LIST* pList) {
	int ret = 0;

	while (pList != NULL) {
		if (pList->m_rptno == rptid) {
			return pList->svidcnt;
		}

		pList = pList->m_rptnext;
	}
	return ret;
}

int CCediListCtroller::get_svid(UINT rptno, UINT* svval) {
	int ret = 0;
	if (m_RptHead != NULL) {
		ret = getting_svid(rptno, svval, m_RptHead);
	}

	return ret;
}

int CCediListCtroller::getting_svid(UINT rptno, UINT* svval, RPT_LIST* rpt) {
	int cnt = 0;
	CString dd("");
	while (rpt != NULL) {
		if (rpt->m_rptno == rptno) {
			for (UINT i = 0; i < rpt->svidcnt; i++) {
				svval[i] = rpt->svid[i];
			}
		}
		rpt = rpt->m_rptnext;
	}
	return cnt;
}

BOOL CCediListCtroller::check_rptflag(UINT CEID) {
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		return p->m_reportflag;
	}
	return FALSE;
}

// Yuen: This sets the enable flag [2002/11/12  10:22]
BOOL CCediListCtroller::set_ceid(UINT CEID, UINT* pRpt, int count) {
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		if (p->m_rptidcurrentcnt) {
			delete[] p->m_rptid;
			p->m_rptid = NULL;
		}
		p->m_rptidcurrentcnt = count;
		p->m_rptid = new UINT[count];
		for (int j = 0; j < count; j++) {
			p->m_rptid[j] = pRpt[j];
		}
		return TRUE;
	}
	return FALSE;
}

// Yuen: This does not set the enable flag [2002/11/12  10:22]
BOOL CCediListCtroller::set_ceid1(UINT CEID, UINT* pRpt, int count) {
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		if (p->m_rptidcurrentcnt) {
			delete[] p->m_rptid;
			p->m_rptid = NULL;
		}
		p->m_rptidcurrentcnt = count;
		p->m_rptid = new UINT[count];
		for (int j = 0; j < count; j++) {
			p->m_rptid[j] = pRpt[j];
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::UnlinkCeid(UINT CEID) {
	BOOL bRet = FALSE;

	for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
		if (m_CEList.m_Ceid[i].m_ceid == CEID && m_CEList.m_Ceid[i].m_rptidcurrentcnt) {
			m_CEList.m_Ceid[i].m_rptidcurrentcnt = 0;
			delete[] m_CEList.m_Ceid[i].m_rptid;
			m_CEList.m_Ceid[i].m_rptid = NULL;
			return TRUE;
		}
	}
	return bRet;
}

BOOL CCediListCtroller::Delete_Rptid(UINT rptid) {
	BOOL bRet = FALSE;
	/*
	 * report ID Memory free in CEID LIST
	 * Be careful never control  m_reportflag variable
	 */
	for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
		int rptcnt = m_CEList.m_Ceid[i].m_rptidcurrentcnt;
		bRet = FALSE;
		int j;
		for (j = 0; j < rptcnt; j++) {
			if (m_CEList.m_Ceid[i].m_rptid[j] == rptid)    // Find Report ID in linked CEID LIST
			{
				bRet = TRUE;
				break;
			}
		}
		if (bRet) {
			int bufcnt = 0;
			if (0 == (rptcnt - 1))                  //  Only exist one report id in ceid Linked list
			{
				m_CEList.m_Ceid[i].m_rptidcurrentcnt = 0;
				delete[]m_CEList.m_Ceid[i].m_rptid;
				m_CEList.m_Ceid[i].m_rptid = NULL;
			}
			else {
				int k;
				UINT* nBuf = new UINT[rptcnt - 1];
				for (k = 0; k < rptcnt; k++) {
					if (m_CEList.m_Ceid[i].m_rptid[j] != rptid) {
						nBuf[bufcnt] = m_CEList.m_Ceid[i].m_rptid[k];
						bufcnt++;
					}
				}
				delete[](m_CEList.m_Ceid[i].m_rptid);
				m_CEList.m_Ceid[i].m_rptid = NULL;
				m_CEList.m_Ceid[i].m_rptidcurrentcnt = rptcnt - 1;

				m_CEList.m_Ceid[i].m_rptid = new UINT[rptcnt - 1];

				for (k = 0; k < rptcnt - 1; k++) {
					m_CEList.m_Ceid[i].m_rptid[k] = nBuf[k];
				}
				delete[] nBuf;
			}
		}
	}

	/*
	 * Delete the Report ID  in RPTLIST Linked List
	 */
	RPT_LIST* pCurRpt = m_RptHead;
	RPT_LIST* pBRpt = m_RptHead;

	while (pCurRpt->m_rptno != rptid) {
		if (pCurRpt->m_rptnext == NULL) {
			// CAN NOT FIND REPORT ID
			return FALSE;
		}
		else {
			pBRpt = pCurRpt;
			pCurRpt = pCurRpt->m_rptnext;
		}
	}
	if (pCurRpt == m_RptHead)              // RPTID  located in First List
	{
		m_RptHead = pCurRpt->m_rptnext;
	}
	else {
		pBRpt->m_rptnext = pCurRpt->m_rptnext;
	}

	delete pCurRpt->svid;
	delete pCurRpt;

	return TRUE;
}

void  CCediListCtroller::GetEventsEnabled(std::vector<UINT>& EventsEnabled, std::vector<CString>& EventsTxt) {
	for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
		if (TRUE == m_CEList.m_Ceid[i].m_reportflag) {
			EventsEnabled.push_back(m_CEList.m_Ceid[i].m_ceid);
			EventsTxt.push_back(m_CEList.m_Ceid[i].m_cename);
		}
	}
	return;
}

void  CCediListCtroller::GetAllEvents(std::vector<UINT>& EventsEnabled, std::vector<CString>& EventsTxt) {
	for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
		EventsEnabled.push_back(m_CEList.m_Ceid[i].m_ceid);
		EventsTxt.push_back(m_CEList.m_Ceid[i].m_cename);
	}
	return;
}

void CCediListCtroller::GetAllSVIDs(std::vector<UINT>& Svid) {
	Svid.clear();
	for (int i = 0; i < SVID_LIST::LASTSVID; i++) {
		if (SVID_ITEM::SV_TYPE == m_SVList.m_Svid[i].m_bVarType) {
			Svid.push_back(m_SVList.m_Svid[i].m_svidno);
		}
	}
	return;
}

BOOL CCediListCtroller::IsSVID(SVID_ITEM* SvData) {
	return (SvData->m_bVarType == SVID_ITEM::SV_TYPE);
}

BOOL CCediListCtroller::IsDVID(SVID_ITEM* SvData) {
	return (SvData->m_bVarType == SVID_ITEM::DV_TYPE);
}

BOOL CCediListCtroller::IsVID(SVID_ITEM* SvData) {
	return ((SvData->m_bVarType == SVID_ITEM::SV_TYPE) ||
		(SvData->m_bVarType == SVID_ITEM::DV_TYPE));
}

int CCediListCtroller::GetNumCEIDs() {
	return CEID_LIST::LASTCEID;
}

int CCediListCtroller::GetNumVIDs() {
	return SVID_LIST::LASTSVID;
}

BOOL CCediListCtroller::CeidDesc(UINT CEID, string& Desc) {
	Desc.erase();
	CEID_ITEM* p = m_CEList.GetItem(CEID);
	if (p) {
		Desc = p->m_cename;
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::SvidDesc(UINT SVID, string& Desc) {
	SVID_ITEM* p = m_SVList.Get(SVID);
	if (!p) {
		return FALSE;
	}

	CString str;
	Desc.erase();

	Desc = "SVID: ";
	str.Format("%d", SVID);
	Desc += str;
	Desc += "\r\nDescription: ";
	CString tmp;
	tmp = p->m_svname;
	tmp.TrimRight();
	Desc += tmp;

	BOOL bRet = TRUE;
	switch (p->m_bVarType) {
	case SVID_ITEM::DV_TYPE:
		Desc += " (DVID)";
		break;
	case SVID_ITEM::SV_TYPE:
		Desc += " (SVID)";
		break;
	case SVID_ITEM::EC_TYPE:
		Desc += " (ECID)";
		break;
	default:
		bRet = FALSE;
		break;
	}
	return bRet;
}

// int CCediListCtroller::get_svidmode(UINT nSvid, int &Type,int &Size)
// {
// 	SVID_ITEM *p = m_SVList.Get(nSvid);
//
// 	if (p)
// 	{
// 		Type = p->m_type;
// 		Size = p->m_size;
// 		return p->m_mode;
// 	}
// 	return 0;
// }

BOOL CCediListCtroller::GetSvData(UINT SVID, string& SVName, string& SVUnit) {
	SVID_ITEM* p = m_SVList.Get(SVID);
	if (p) {
		SVName = p->m_svname;
		SVUnit = p->m_unit;
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::SaveReport() {
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[256];

	CSingleLock Lock(&CS);
	Lock.Lock();
	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);
	_makepath(inifile, drive, dir, "DEFAULT", "RPT");

	FILE* fp;
	fp = fopen(inifile, "wb");
	if (fp) {
		RPT_LIST* pRpt = m_RptHead;
		while (pRpt) {
			fwrite(&pRpt->m_rptno, sizeof(UINT), 1, fp);
			fwrite(&pRpt->svidcnt, sizeof(UINT), 1, fp);
			fwrite(pRpt->svid, sizeof(UINT), pRpt->svidcnt, fp);
			pRpt = pRpt->m_rptnext;
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::SaveMapping() {
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[256];

	CSingleLock Lock(&CS);
	Lock.Lock();
	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);

	_makepath(inifile, drive, dir, "DEFAULT", "MAP");
	FILE* fp;
	fp = fopen(inifile, "wb");
	if (fp) {
		for (int i = 0; i < CEID_LIST::LASTCEID; i++) {
			if (0 < m_CEList.m_Ceid[i].m_rptidcurrentcnt) {
				fwrite(&m_CEList.m_Ceid[i].m_ceid, sizeof(UINT), 1, fp);
				fwrite(&m_CEList.m_Ceid[i].m_rptidcurrentcnt, sizeof(UINT), 1, fp);
				fwrite(m_CEList.m_Ceid[i].m_rptid, sizeof(UINT), m_CEList.m_Ceid[i].m_rptidcurrentcnt, fp);
			}
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::LoadReport() {
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[256];

	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);

	_makepath(inifile, drive, dir, "DEFAULT", "RPT");
	FILE* fp;
	fp = fopen(inifile, "rb");
	if (fp) {
		UINT Data[2];
		while (2 == fread(Data, sizeof(UINT), 2, fp)) {
			UINT* pSVID = new UINT[Data[1]];
			if (!pSVID) break;
			if (Data[1] == fread(pSVID, sizeof(UINT), Data[1], fp)) {
				Add_Rpt(pSVID, Data[1], Data[0]);
			}
			delete[] pSVID;
			pSVID = NULL;
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

BOOL CCediListCtroller::LoadMapping() {
	char drive[_MAX_DRIVE];
	char dir[256];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char inifile[256];

	GetModuleFileName(NULL, inifile, 255);
	_splitpath(inifile, drive, dir, fname, ext);

	_makepath(inifile, drive, dir, "DEFAULT", "MAP");
	FILE* fp;
	fp = fopen(inifile, "rb");
	if (fp) {
		UINT Data[2];
		while (2 == fread(Data, sizeof(UINT), 2, fp)) {
			UINT* pRPTID = new UINT[Data[1]];
			if (!pRPTID) break;
			if (Data[1] == fread(pRPTID, sizeof(UINT), Data[1], fp)) {
				set_ceid1(Data[0], pRPTID, (int)Data[1]);
			}
			delete[] pRPTID;
			pRPTID = NULL;
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

// To be done
string CCediListCtroller::SvidUnitString(UINT Unit) {
	string UnitStr = "";
	/*	switch(Unit) {
		case :
			break;
		case :
			break;
		default:
		}*/
	return UnitStr;
}

void CCediListCtroller::Log(CString msg, short lvl) {
	pGDev->LogFile.Log(msg, lvl, "CCediListCtroller");
	if (lvl <= 1) {
		pGDev->LogFile.LogLocal(msg);
	}
}

int CCediListCtroller::GetCEID(CEID_LIST::CEIDNAME name) {
	if (name < CEID_LIST::LASTCEID) {
		return CEIDCtlr.m_CEList.m_Ceid[name].m_ceid;
	}
	ASSERT(0);
	return 0;
}