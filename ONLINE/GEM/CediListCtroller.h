#pragma once

// #include <afxmt.h>
#include <map>
#include <vector>
// using namespace std;

#include "ONLINE/INC/IDlist.h"
#include "SVID_LIST.h"
#include "CEID_LIST.h"
#include "SecsParam.h"
#include "AlrmList.h"

typedef std::map<UINT, UINT> _ListMap;

class CCediListCtroller {
	CCriticalSection CS;
	//	_ListMap CEIDMap;
	//	_ListMap SVIDMap;

public:
	CCediListCtroller();
	virtual ~CCediListCtroller();

public:
	// 	SVID_LIST   m_sSvidList[ MAX_SVIDLIST ];
	SVID_LIST m_SVList;
	// 	CEID_LIST   m_sCeidList[ MAX_CEIDLIST ];
	CEID_LIST m_CEList;
	CSecsParam m_SecsParam;
	CAlrmList m_alarm;

	// 	int         m_CurMaxSvidCount;
	RPT_LIST* m_RptHead;
	int m_CurMaxRptCount;
	//  int         m_CurMaxCeidCount;

public:
	int GetCEID(CEID_LIST::CEIDNAME name);
	void Log(CString msg, short lvl = 5);
	std::string SvidUnitString(UINT Unit);
	BOOL LoadMapping();
	BOOL LoadReport();
	BOOL SaveMapping();
	BOOL SaveReport();
	BOOL SaveEnaDisCEIDAll();
	BOOL LoadEnaDisCEIDAll();

	int GetNumVIDs(void);
	int GetNumCEIDs(void);
	BOOL IsSVID(SVID_ITEM* SvData);
	BOOL IsDVID(SVID_ITEM* SvData);
	BOOL IsVID(SVID_ITEM* SvData);
	// 	bool IsSVID(const SVID_LIST &SvData);
	// 	bool IsDVID(const SVID_LIST &SvData);
	// 	bool IsVID(const SVID_LIST &SvData);

	void   GetAllSVIDs(std::vector<UINT>& Svid);
	BOOL   Delete_Rptid(UINT rptid);
	BOOL   UnlinkCeid(UINT nCeid);
	BOOL   check_rptflag(UINT ceid);
	int    getting_svid(UINT rptno, UINT* svval, RPT_LIST* rpt);
	int    get_svid(UINT rptno, UINT* svval);
	int    getting_svidcount(UINT rptid, RPT_LIST* pList);
	int    get_svidcount(UINT rptno);
	BOOL   getting_rptno(UINT ceid, UINT* rptno);
	int    get_rptcount(UINT ceid);
	BOOL   evtrpt_setting(UINT ceid, BOOL flag);
	BOOL   evtrpt_all_setting(BOOL flag);
	// Details required for SVID reporting in GemFactory
// 	int		get_svidmode(UINT svid, int &Type,int &Size) ;
	BOOL   CeidDesc(UINT Ceid, std::string&);
	BOOL   SvidDesc(UINT Ceid, std::string&);
	BOOL   set_ceid(UINT ceid, UINT* pRpt, int count);
	BOOL   set_ceid1(UINT ceid, UINT* pRpt, int count);
	BOOL   ceid_defined(UINT ceid);
	BOOL   Check_Ceid(UINT ceid);
	// 	void   Fill_Ceid_Data( char *pData , CEID_LIST* pList );
	BOOL   Initialization_CeidList();
	void   delete_all(RPT_LIST* rpt);
	void   Delete_All_RptList();

	RPT_LIST* memalloc();
	RPT_LIST* Add_rptlist(RPT_LIST* baseptr, UINT* svid, int svidcnt, UINT rptno);
	void Add_Rpt(UINT* svid, int svidcnt, UINT rptno);

	BOOL   Search_rptid(unsigned int rptno, RPT_LIST* pRptlist);
	BOOL   chk_svid(unsigned int nSvid);
	BOOL   chk_DVid(unsigned int nSvid);
	BOOL   chk_Vid(unsigned int nSvid);
	BOOL   chk_rptid(unsigned int nRptid, BOOL bLog = FALSE);

	// 	void   Fill_Svid_Data( char* pData , SVID_LIST* pList );
	BOOL   Initialize_List();
	// 	BOOL   Initialization_SvidList();
	void   GetEventsEnabled(std::vector<UINT>& EventsEnabled, std::vector<CString>& EventsTxt);
	void   GetAllEvents(std::vector<UINT>& EventsEnabled, std::vector<CString>& EventsTxt);
	BOOL   GetSvData(UINT SVID, std::string& SVName, std::string& SVUnit);
};

extern CCediListCtroller CEIDCtlr;
